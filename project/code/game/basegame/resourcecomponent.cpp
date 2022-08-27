#include "game/shared/gamepch.h"
#include "game/basegame/resourcecomponent.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CResourceComponent::CResourceComponent()
{
	m_eState = eState_Unloaded;
	m_pLoadProcessor = nullptr;
}

///------------------------------------------------------------------------

void CResourceComponent::Load( char const * pResourceName, CLoadProcessor * pLoadProcessor, XEngine::XAllocator * pMainAllocator, XEngine::XAllocator * pDiscardableAllocator /*= nullptr*/, XEngine::XSimpleFunctor & rCallback /*= DoNothing */ )
{
	switch (m_eState)
	{
	case eState_Unloaded:
		{
			XASSERT(m_pLoadProcessor == nullptr);
			pLoadProcessor->AddToProcessList(this);
			m_pLoadProcessor = pLoadProcessor;
			m_oFile.Open(pResourceName, XEngine::XFileFlags::Read);
			m_oResourceReader.Init(pMainAllocator, pDiscardableAllocator);
			m_oResourceReader.Load(m_oFile);
			m_oResourceReader.LoadAllSections();
			m_oCallback = rCallback;
			m_eState = eState_Loading;
		}
		break;
	case eState_Unloading:
		{
			///Loading while unloading is not currently supported
			XASSERT(false);
		}
		break;
	case eState_Loading:
	case eState_Loaded:
		{
			///Loading while loading or loaded will never be supported, what are you crazy?
			XASSERT(false);
		}
		break;
	}
}

///------------------------------------------------------------------------

void CResourceComponent::Unload(CLoadProcessor * pLoadProcessor, XEngine::XSimpleFunctor & rCallback /*= DoNothing */)
{	
	switch (m_eState)
	{
	case eState_Unloaded:
	case eState_Unloading:
		{
			///Loading while loading or loaded will never be supported, what are you crazy?
			XASSERT(false);
		}
		break;
	case eState_Loading:
	case eState_Loaded:
		{
			m_oResourceReader.Unload();
			if (m_pLoadProcessor == nullptr)
			{
				pLoadProcessor->AddToProcessList(this);
				m_pLoadProcessor = pLoadProcessor;
			}
			m_oCallback = rCallback;
			m_eState = eState_Unloading;
		}
	}
}

///------------------------------------------------------------------------

void CResourceComponent::Update( float fTime )
{
	m_oResourceReader.Process();
	if (m_oResourceReader.IsLoadComplete())
	{
		m_pLoadProcessor->RemoveFromProcessList(this);
	}
	m_pLoadProcessor = nullptr;
	switch ( m_eState )
	{
	case eState_Unloading:
		m_oFile.Close();
		m_eState = eState_Unloaded;
		m_oCallback();
		break;
	case eState_Loading:
		m_eState = eState_Loaded;
		m_oCallback();
		break;
	case eState_Unloaded:
	case eState_Loaded:
		XASSERT(false); ///Should not be in the process list, why is it being updated?
		break;
	}
}

///------------------------------------------------------------------------

bool CResourceComponent::IsLoaded() const
{
	return m_eState == eState_Loaded;
}

///------------------------------------------------------------------------

XEngine::XResourceDatabase const * CResourceComponent::GetResourceDatabase() const
{
	XASSERT(IsLoaded());
	return (XEngine::XResourceDatabase const *)m_oResourceReader.GetRootObject();
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CLoadProcessor::CLoadProcessor()
{
	m_oAddMutex.Init();
	m_oRemoveMutex.Init();
}

///------------------------------------------------------------------------

CLoadProcessor::~CLoadProcessor()
{
	XASSERT(m_oProcessList.IsEmpty());
	m_oAddMutex.Deinit();
	m_oRemoveMutex.Deinit();
}

///------------------------------------------------------------------------

void CLoadProcessor::Update( float fTime )
{
	m_oAddMutex.Lock();
	CResourceComponent * pComponentToProcess = m_oProcessList.GetFront();
	while (pComponentToProcess != m_oProcessList.GetEndMarker())
	{
		pComponentToProcess->Update(fTime);
		pComponentToProcess = pComponentToProcess->GetNext<CLoadProcessorPrivate::ELoadProcessorLists_ProcessList>();
	}
	m_oAddMutex.Unlock();

	m_oRemoveMutex.Lock();
	while (m_oFinishedList.GetFront() != m_oFinishedList.GetEndMarker())
	{
		m_oProcessList.Remove(m_oFinishedList.PopFront());
	}
	m_oRemoveMutex.Unlock();
}

///------------------------------------------------------------------------

void CLoadProcessor::AddToProcessList( CResourceComponent * pResourceComponent )
{
	m_oAddMutex.Lock();
	m_oProcessList.PushBack(pResourceComponent);
	m_oAddMutex.Unlock();
}

void CLoadProcessor::RemoveFromProcessList( CResourceComponent * pResourceComponent )
{
	m_oRemoveMutex.Lock();
	m_oFinishedList.PushBack(pResourceComponent);
	m_oRemoveMutex.Unlock();
}

///------------------------------------------------------------------------