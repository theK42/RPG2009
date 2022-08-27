
#pragma once

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	template< typename T, int N >
	class XListElement
	{
	public:
		template< int M >
		T *	GetNext() const;

		template< int M >
		T *	GetPrev() const;

		template< int M >
		void	SetNext( T * pNext );

		template< int M >
		void	SetPrev( T * pPrev );

	protected:
		T *	m_pNext[ N ];
		T *	m_pPrev[ N ];
	};

	///------------------------------------------------------------------------

	template< typename T, int N > template< int M >
	inline T * XListElement< T, N >::GetNext() const
	{
		return m_pNext[ M ];
	}

	///------------------------------------------------------------------------

	template< typename T, int N > template< int M >
	inline T * XListElement< T, N >::GetPrev() const
	{
		return m_pPrev[ M ];
	}

	///------------------------------------------------------------------------

	template< typename T, int N > template< int M >
	inline void XListElement< T, N >::SetNext( T * pNext )
	{
		m_pNext[ M ] = pNext;
	}

	///------------------------------------------------------------------------

	template< typename T, int N > template< int M >
	inline void XListElement< T, N >::SetPrev( T * pPrev )
	{
		m_pPrev[ M ] = pPrev;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	template< typename T, int M >
	class XList
	{
	public:
		XList();

		void		Reset();

		T const *	GetEndMarker() const;
		bool		IsEmpty() const;

		T *			GetFront() const;
		T *			GetBack() const;

		void		InsertBefore( T * pItem, T * pAnchor );
		void		InsertAfter( T * pItem, T * pAnchor );
		void		Remove( T * pItem );

		void		PushFront( T * pItem );
		void		PushBack( T * pItem );

		T *			PopFront();
		T *			PopBack();

	private:
		T	m_oListHead;
	};

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::Reset()
	{
		T * pHead = &m_oListHead;
		pHead->SetNext<M>( pHead );
		pHead->SetPrev<M>( pHead );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline XList< T, M >::XList()
	{
		Reset();
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline typename T const * XList< T, M >::GetEndMarker() const
	{
		return &m_oListHead;
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline bool XList< T, M >::IsEmpty() const
	{
		return m_oListHead.GetNext<M>() == &m_oListHead;
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline T * XList< T, M >::GetFront() const
	{
		return m_oListHead.GetNext<M>();
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline T * XList< T, M >::GetBack() const
	{
		return m_oListHead.GetPrev<M>();
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::InsertBefore( T * pItem, T * pAnchor )
	{
		T * pPrev = pAnchor->GetPrev<M>();
		T * pNext = pAnchor;

		pPrev->SetNext<M>( pItem );
		pNext->SetPrev<M>( pItem );
		pItem->SetPrev<M>( pPrev );
		pItem->SetNext<M>( pNext );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::InsertAfter( T * pItem, T * pAnchor )
	{
		T * pPrev = pAnchor;
		T * pNext = pAnchor->GetNext<M>();

		pPrev->SetNext<M>( pItem );
		pNext->SetPrev<M>( pItem );
		pItem->SetPrev<M>( pPrev );
		pItem->SetNext<M>( pNext );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::Remove( T * pItem )
	{
		T * pPrev = pItem->GetPrev<M>();
		T * pNext = pItem->GetNext<M>();

		pPrev->SetNext<M>( pNext );
		pNext->SetPrev<M>( pPrev );
		pItem->SetNext<M>( pItem );
		pItem->SetPrev<M>( pItem );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::PushFront( T * pItem )
	{
		InsertAfter( pItem, &m_oListHead );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline void XList< T, M >::PushBack( T * pItem )
	{
		InsertBefore( pItem, &m_oListHead );
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline T * XList< T, M >::PopFront()
	{
		T * pFront = GetFront();
		Remove( pFront );
		return pFront;
	}

	///------------------------------------------------------------------------

	template< typename T, int M >
	inline T * XList< T, M >::PopBack()
	{
		T * pBack = GetBack();
		Remove( pBack );
		return pBack;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine
