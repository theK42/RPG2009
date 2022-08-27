
#pragma once

#include <cstddef>
#include <new>

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XAllocatorRegistry;
	class XAllocator;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	/// The allocator registry maintains an ordered list of allocator control
	/// records, each of which contains a memory address range and an allocator
	/// that is responsible for that range. Control ranges are permitted to
	/// overlap only in a specific way, which is called "containment". That is,
	/// if two ranges intersect, one must be a proper subset of the other.

	/// To find the registered allocator that controls an address p, the
	/// registry searches its records for the narrowest control range that
	/// contains p, then returns the allocator that is responsible for that
	/// range.

	/// When an allocator is unregistered, the registry removes records that
	/// refer to that allocator, and also removes all records with ranges that
	/// are contained in ranges that are controlled by that allocator. This is
	/// to ensure that the registry continues to operate correctly in case an
	/// allocator is unregistered without explicitly unregistering any
	/// nested allocators, based on the assumption that controlled memory is
	/// unusable after the controlling allocator is unregistered.

	class XAllocatorRegistry
	{
	public:
		void			RegisterAllocator(
							XAllocator * pAllocator,
							void * pRangeBegin,
							void * pRangeEnd );
		void			UnregisterAllocator( XAllocator const * pAllocator );

		XAllocator *	FindAllocator( void const * p ) const;

		static XAllocatorRegistry &	GetInstance();

	private:
		struct Record
		{
			XAllocator *	m_pAllocator;
			void *			m_pRangeBegin;
			void *			m_pRangeEnd;
		};

		static int const MAX_RECORDS = 256;

		Record	m_pRecords[ MAX_RECORDS ];
		int		m_nNumRecords;

	private:
		static XAllocatorRegistry	ms_oInstance;
	};

	///------------------------------------------------------------------------

	inline XAllocatorRegistry & XAllocatorRegistry::GetInstance()
	{
		return ms_oInstance;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XAllocator
	{
	public:
		virtual			~XAllocator();

		virtual void *	Allocate(
							uaddr nSize,
							uaddr nAlignment,
							uaddr nAlignmentOffset,
							char const * pFile,
							int nLine );
		virtual void *	Reallocate(
							void * p,
							uaddr nNewSize,
							uaddr nNewAlignment,
							uaddr nAlignmentOffset,
							char const * pFile,
							int nLine );
		virtual void	Free( void * p );

		void *			Allocate(
							uaddr nSize,
							uaddr nAlignment,
							char const * pFile,
							int nLine );
		void *			Reallocate(
							void * p,
							uaddr nNewSize,
							uaddr nNewAlignment,
							char const * pFile,
							int nLine );

		static void		InitModule();
		static void		DeinitModule();

	protected:
		void			Register( void * pRangeBegin, void * pRangeEnd );
		void			Unregister() const;
	};

	///------------------------------------------------------------------------

	inline XAllocator::~XAllocator()
	{
	}

	///------------------------------------------------------------------------

	inline void * XAllocator::Allocate(
		uaddr nSize,
		uaddr nAlignment,
		char const * pFile,
		int nLine )
	{
		return Allocate( nSize, nAlignment, 0, pFile, nLine );
	}

	///------------------------------------------------------------------------

	inline void * XAllocator::Reallocate(
		void * p,
		uaddr nNewSize,
		uaddr nNewAlignment,
		char const * pFile,
		int nLine )
	{
		return Reallocate( p, nNewSize, nNewAlignment, 0, pFile, nLine );
	}

	///------------------------------------------------------------------------

	inline void XAllocator::Register( void * pRangeBegin, void * pRangeEnd )
	{
		XAllocatorRegistry::GetInstance().RegisterAllocator(
			this,
			pRangeBegin,
			pRangeEnd );
	}

	///------------------------------------------------------------------------

	inline void XAllocator::Unregister() const
	{
		XAllocatorRegistry::GetInstance().UnregisterAllocator( this );
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XAllocatorStackEntry
	{
		XAllocatorStackEntry const *	m_pNext;
		XAllocator *					m_pAllocator;
	};

	extern XTLS_DECLARE( XAllocatorStackEntry const * ) g_pDeleteAllocatorStack;

	///------------------------------------------------------------------------

	template< class T >
	void XDelete( T * p, XAllocator * pAllocator = nullptr )
	{
		if( p == nullptr )
		{
			return;
		}

		if( pAllocator == nullptr )
		{
			pAllocator = XAllocatorRegistry::GetInstance().FindAllocator( p );
		}

		XASSERT_PTR( pAllocator );

		XAllocatorStackEntry oDeleteStackEntry;
		oDeleteStackEntry.m_pNext		= g_pDeleteAllocatorStack;
		oDeleteStackEntry.m_pAllocator	= pAllocator;

		g_pDeleteAllocatorStack = &oDeleteStackEntry;

		delete p;
	}

	///------------------------------------------------------------------------

	template< class T >
	void XDeleteArray( T * p, XAllocator * pAllocator = nullptr )
	{
		if( p == nullptr )
		{
			return;
		}

		if( pAllocator == nullptr )
		{
			pAllocator = XAllocatorRegistry::GetInstance().FindAllocator( p );
		}

		XASSERT_PTR( pAllocator );

		XAllocatorStackEntry oDeleteStackEntry;
		oDeleteStackEntry.m_pNext		= g_pDeleteAllocatorStack;
		oDeleteStackEntry.m_pAllocator	= pAllocator;

		g_pDeleteAllocatorStack = &oDeleteStackEntry;

		delete [] p;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline void * operator new(
	std::size_t nSize,
	XEngine::XAllocator * pAllocator,
	XEngine::uaddr nAlignment,
	char const * pFile,
	int nLine ) throw()
{
	XASSERT_PTR( pAllocator );
	return pAllocator->Allocate( nSize, nAlignment, pFile, nLine );
}

///----------------------------------------------------------------------------

inline void operator delete(
	void * p,
	XEngine::XAllocator * pAllocator,
	XEngine::uaddr nAlignment,
	char const * pFile,
	int nLine )
{
	XASSERT_PTR( pAllocator );
	pAllocator->Free( p );
}

///----------------------------------------------------------------------------

inline void * operator new [](
	std::size_t nSize,
	XEngine::XAllocator * pAllocator,
	XEngine::uaddr nAlignment,
	char const * pFile,
	int nLine ) throw()
{
	XASSERT_PTR( pAllocator );
	return pAllocator->Allocate( nSize, nAlignment, pFile, nLine );
}

///----------------------------------------------------------------------------

inline void operator delete [](
	void * p,
	XEngine::XAllocator * pAllocator,
	XEngine::uaddr nAlignment,
	char const * pFile,
	int nLine )
{
	XASSERT_PTR( pAllocator );
	pAllocator->Free( p );
}

///----------------------------------------------------------------------------

inline void operator delete( void * p )
{
	XEngine::XAllocatorStackEntry const * pDeleteStackEntry = XEngine::g_pDeleteAllocatorStack;
	XASSERT_PTR( pDeleteStackEntry );

	XEngine::g_pDeleteAllocatorStack = pDeleteStackEntry->m_pNext;
	pDeleteStackEntry->m_pAllocator->Free( p );
}

///----------------------------------------------------------------------------

inline void operator delete []( void * p )
{
	XEngine::XAllocatorStackEntry const * pDeleteStackEntry = XEngine::g_pDeleteAllocatorStack;
	XASSERT_PTR( pDeleteStackEntry );

	XEngine::g_pDeleteAllocatorStack = pDeleteStackEntry->m_pNext;
	pDeleteStackEntry->m_pAllocator->Free( p );
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

#define XDECLARE_DESTRUCTIBLE_NEW				\
static inline void * operator new [](			\
	std::size_t nSize,							\
	XEngine::XAllocator * pAllocator,			\
	XEngine::uaddr nAlignment,					\
	char const * pFile,							\
	int nLine ) throw()							\
{												\
	XASSERT_PTR( pAllocator );					\
	return pAllocator->Allocate(				\
		nSize,									\
		nAlignment,								\
		sizeof( std::size_t ),					\
		pFile,									\
		nLine );								\
}

///----------------------------------------------------------------------------

#define XDECLARE_DESTRUCTIBLE_DELETE															\
static inline void operator delete []( void * p )												\
{																								\
	XEngine::XAllocatorStackEntry const * pDeleteStackEntry = XEngine::g_pDeleteAllocatorStack;	\
	XASSERT_PTR( pDeleteStackEntry );															\
	XEngine::g_pDeleteAllocatorStack = pDeleteStackEntry->m_pNext;								\
	pDeleteStackEntry->m_pAllocator->Free( p );													\
}

///----------------------------------------------------------------------------

#define XDECLARE_DESTRUCTIBLE_NEW_DELETE	\
	XDECLARE_DESTRUCTIBLE_NEW				\
	XDECLARE_DESTRUCTIBLE_DELETE

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

#define XNEW(pAllocator)						new( pAllocator, 0, __FILE__, __LINE__ )
#define XNEW_ALIGNED(pAllocator, nAlignment)	new( pAllocator, nAlignment, __FILE__, __LINE__ )
#define XDELETE(...)							XEngine::XDelete( __VA_ARGS__ )
#define XDELETE_ARRAY(...)						XEngine::XDeleteArray( __VA_ARGS__ )

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
