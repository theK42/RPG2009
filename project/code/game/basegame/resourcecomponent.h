#pragma once

#include "engine/algorithms/xlist.h"
#include "engine/resource/xresdb.h"
#include "engine/resource/xresfile.h"
#include "engine/resource/xfileio.h"
#include "engine/threading/xmutex.h"
#include "engine/algorithms/xfunctor.h"

class CResourceComponent;
class CLoadProcessor;

class CLoadProcessorPrivate
{
private:
	friend class CResourceComponent;
	friend class CLoadProcessor;
	enum ELoadProcessorLists
	{
		ELoadProcessorLists_ProcessList,
		ELoadProcessorLists_FinishedList,
		ELoadProcessorLists_Count
	};
};

class CResourceComponent : public XEngine::XListElement<CResourceComponent, CLoadProcessorPrivate::ELoadProcessorLists_Count>
{
public:

	CResourceComponent();

	void Load(char const * pResourceName, CLoadProcessor * pLoadProcessor, XEngine::XAllocator * pMainAllocator, XEngine::XAllocator * pDiscardableAllocator = nullptr, XEngine::XSimpleFunctor & rCallback = XEngine::XSimpleFunctor::ms_oDoNothing);
	void Unload(CLoadProcessor * pLoadProcessor, XEngine::XSimpleFunctor & rCallback = XEngine::XSimpleFunctor::ms_oDoNothing);
	bool IsLoaded() const;
	XEngine::XResourceDatabase const * GetResourceDatabase() const;

protected:
	enum EState
	{
		eState_Unknown = -1,
		eState_Unloaded,
		eState_Unloading,
		eState_Loading,
		eState_Loaded,
		eState_Count
	};
	
	void Update(float fTime);

	EState							m_eState;
	CLoadProcessor *				m_pLoadProcessor;
	XEngine::XAsyncFile				m_oFile;
	XEngine::XResourceReader		m_oResourceReader;  
	XEngine::XSimpleFunctor			m_oCallback;

	friend class CLoadProcessor;
};


class CLoadProcessor
{
public:
	CLoadProcessor();
	~CLoadProcessor();
	
	void Init();
	void Deinit();

	void Update(float fTime);
protected:

	void AddToProcessList(CResourceComponent * pResourceComponent);
	void RemoveFromProcessList(CResourceComponent * pResourceComponent);

	XEngine::XList<CResourceComponent, CLoadProcessorPrivate::ELoadProcessorLists_ProcessList>	m_oProcessList;
	XEngine::XList<CResourceComponent, CLoadProcessorPrivate::ELoadProcessorLists_FinishedList>	m_oFinishedList;

	XEngine::XMutex	m_oAddMutex;
	XEngine::XMutex	m_oRemoveMutex;

	friend class CResourceComponent;
};