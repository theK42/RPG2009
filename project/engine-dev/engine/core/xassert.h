
#pragma once

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	bool XAssertInternal(
		bool bCondition,
		char const * pFile,
		int nLine,
		char const * pExpression,
		char const * pMessage,
		... );

	///------------------------------------------------------------------------

	bool XWarnInternal(
		bool bCondition,
		char const * pFile,
		int nLine,
		char const * pExpression,
		char const * pMessage,
		... );

	///------------------------------------------------------------------------

	template< typename T >
	inline bool XAssertPointer(
		T const * p,
		char const * pFile,
		int nLine,
		char const * pExpression )
	{
		if( XUNLIKELY( p == nullptr ) )
		{
			return XAssertInternal(
				false,
				pFile,
				nLine,
				pExpression,
				"Pointer value is null." );
		}

		if( XUNLIKELY( !IsAligned( p, alignof( T ) ) ) )
		{
			return XAssertInternal(
				false,
				pFile,
				nLine,
				pExpression,
				"Pointer value is not aligned." );
		}

		return false;
	}

	///------------------------------------------------------------------------

	template<>
	inline bool XAssertPointer<void>(
		void const * p,
		char const * pFile,
		int nLine,
		char const * pExpression )
	{
		if( XUNLIKELY( p == nullptr ) )
		{
			return XAssertInternal(
				false,
				pFile,
				nLine,
				pExpression,
				"Pointer value is null." );
		}

		return false;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

#if defined( XENABLE_ASSERTS )

#	ifndef _MANAGED
#		define XASSERT(x)			do { if( XUNLIKELY( XEngine::XAssertInternal( !!(x), __FILE__, __LINE__, #x, XEngine::nullptr ) ) ) XBREAK; } while( false )
#	else
#		define XASSERT(x)			do { if( XUNLIKELY( XEngine::XAssertInternal( !!(x), __FILE__, __LINE__, #x, nullptr ) ) ) XBREAK; } while( false )
#	endif //_MANAGED
#	define XASSERT_MSG(x, ...)	do { if( XUNLIKELY( XEngine::XAssertInternal( !!(x), __FILE__, __LINE__, #x, __VA_ARGS__ ) ) ) XBREAK; } while( false )
#	define XASSERT_PTR(p)		do { if( XUNLIKELY( XEngine::XAssertPointer( (p), __FILE__, __LINE__, #p ) ) ) XBREAK; } while( false )
#else
#	define XASSERT(x)			XASSUME(x)
#	define XASSERT_MSG(x, ...)	XASSUME(x)
#	ifndef _MANAGED
#		define XASSERT_PTR(p)		XASSUME((p) != XEngine::nullptr)
#	else
#		define XASSERT_PTR(p)		XASSUME((p) != nullptr)
#	endif
#endif

#if defined( XENABLE_WARNINGS )
#	define XWARN(x, ...)		do { if( XUNLIKELY( XEngine::XWarnInternal( !!(x), __FILE__, __LINE__, #x, __VA_ARGS__ ) ) ) XBREAK; } while( false )
#else
#	define XWARN(x, ...)		((void)(0&&sizeof(x)))
#endif
