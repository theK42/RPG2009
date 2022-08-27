#pragma once

#include "engine/algorithms/xlist.h"
#include "engine/threading/xmutex.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

template <class TClass>
class CUpdater;

///------------------------------------------------------------------------
///------------------------------------------------------------------------

template <class TClass>
class CUpdating : public XEngine::XListElement<CUpdating<TClass>, 1>
{
public:
	CUpdating();
	~CUpdating();

	void Deinit();
	
	void Start();
	void Stop();

	TClass & GetInternalUpdatable();

protected:
	///Init is protected because most template instances will need to override it with more parameters.
	void Init(CUpdater<TClass> * pUpdater);

	///Hoping to keep Update protected so only the associated CUpdater can call it
	void Update(float fTime);

	TClass				m_oUpdatable;
	CUpdater<TClass> *	m_pUpdater;
	bool				m_bUpdating;

	friend class CUpdater<TClass>;
};

///------------------------------------------------------------------------

template <class TClass>
CUpdating<TClass>::CUpdating()
{
	m_pUpdater = nullptr;
	m_bUpdating = false;
}

///------------------------------------------------------------------------

template <class TClass>
CUpdating<TClass>::~CUpdating()
{
	Deinit();
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdating<TClass>::Init( CUpdater<TClass> * pUpdater )
{
	XASSERT_PTR(pUpdater);
	XASSERT(m_pUpdater == nullptr);  ///Don't allow double initialization
	m_pUpdater = pUpdater;
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdating<TClass>::Deinit()
{
	Stop();
	m_pUpdater = nullptr;
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdating<TClass>::Start()
{
	XASSERT_PTR(m_pUpdater); ///Initialized
	if (!m_bUpdating)
	{
		m_pUpdater->AddToUpdateList(this);
		m_bUpdating = true;
	}
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdating<TClass>::Stop()
{
	XASSERT_PTR(m_pUpdater); ///Initialized
	if (m_bUpdating)
	{
		m_pUpdater->RemoveFromUpdateList(this);
		m_bUpdating = false;
	}
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdating<TClass>::Update( float fTime )
{
	XASSERT(m_pUpdater != nullptr); ///Initialized
	XASSERT(m_bUpdating);
	m_oUpdatable.Update(fTime);
}

///------------------------------------------------------------------------

template <class TClass>
TClass & CUpdating<TClass>::GetInternalUpdatable()
{
	return m_oUpdatable;
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

template <class TClass>
class CUpdater
{
public:
	CUpdater();
	~CUpdater();

	void Init();
	void Deinit();

	void Update(float fTime);

protected:
	void AddToUpdateList(CUpdating<TClass> * pUpdatable);
	void RemoveFromUpdateList(CUpdating<TClass> * pUpdatable);

	XEngine::XList<CUpdating<TClass>, 0>	m_oUpdateList;
	mutable	XEngine::XMutex					m_oMutex; 
	bool									m_bInitialized;

	friend class CUpdating<TClass>;
};

///------------------------------------------------------------------------

template <class TClass>
CUpdater<TClass>::CUpdater()
{
	m_bInitialized = false;
}

///------------------------------------------------------------------------

template <class TClass>
CUpdater<TClass>::~CUpdater()
{
	Deinit();
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdater<TClass>::Init()
{
	if (!m_bInitialized)
	{
		m_oMutex.Init();
		m_bInitialized = true;
	}
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdater<TClass>::Deinit()
{
	if (m_bInitialized)
	{
		XASSERT(m_oUpdateList.IsEmpty()); /// The updater is a dependency for any updating updating objects, must ensure they are cleaned up first
		m_oMutex.Deinit();
		m_bInitialized = false;
	}
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdater<TClass>::Update(float fTime)
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	CUpdating<TClass> * pUpdatable = m_oUpdateList.GetFront();
	while (pUpdatable != m_oUpdateList.GetEndMarker())
	{
		pUpdatable->Update(fTime);
		pUpdatable = pUpdatable->GetNext<0>();
	}
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdater<TClass>::AddToUpdateList( CUpdating<TClass> * pUpdatable )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	m_oUpdateList.PushBack(pUpdatable);
}

///------------------------------------------------------------------------

template <class TClass>
void CUpdater<TClass>::RemoveFromUpdateList( CUpdating<TClass> * pUpdatable )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	m_oUpdateList.Remove(pUpdatable);
}

///------------------------------------------------------------------------