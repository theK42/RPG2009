#pragma once

#include "game/basegame/rectangle.h"

#include "engine/graphics/xmesh.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/graphics/xcolor.h"
#include "engine/threading/xmutex.h"
#include "engine/algorithms/xlist.h"
#include "engine/algorithms/xfunctor.h"
#include "engine/math/xquaternion.h"

namespace XEngine
{
	class XTexture;
	class XMatrix44;
}

class CMeshRenderer;

class CMeshSetupFunctor : protected XEngine::XBaseFunctor 
{
public:
	CMeshSetupFunctor();
	~CMeshSetupFunctor();
	void Init(void (* pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &));
	template<class TClass>
	void Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &));

	void Deinit();

	void operator()(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);

	static CMeshSetupFunctor	ms_oDoNothing;
private:
	void Thunk(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);
	template<class TClass>
	void TThunk(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);

	void (CMeshSetupFunctor::* m_pThunk)(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);
};

template<class TClass>
void CMeshSetupFunctor::Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &))
{
	XBaseFunctor::Init(pObjectPointer, &pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &CMeshSetupFunctor::TThunk<TClass>;
}

template<class TClass>
void CMeshSetupFunctor::TThunk(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer)
{
	void (TClass::*pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &) = *(void (TClass::**)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &))m_pFunctionPointer;
	TClass * pObjectPointer = (TClass *) m_pObjectPointer;
	(pObjectPointer->*pFunctionPointer)(pGraphics, rMesh, rVertexBuffer);
}

class CMeshGraphic : public XEngine::XListElement<CMeshGraphic, 1>
{
public:
	CMeshGraphic();
	~CMeshGraphic();

	void Init(CMeshRenderer * pRenderer, XEngine::XMatrix44 const * pTransform, CMeshSetupFunctor & rMeshSetupCallback, XEngine::XColor const & rColor, XEngine::XTexture const * pTexture = nullptr);
	void Deinit();

	void StartRendering();
	void StopRendering();

protected:
	void Render() const;
	void Reset();

	XEngine::XColor				m_oColor;
	XEngine::XMesh				m_oMesh;
	XEngine::XVertexBuffer		m_oVertexBuffer;
	bool						m_bInitialized;
	bool						m_bRendering;
	CMeshRenderer *				m_pRenderer;
	XEngine::XMatrix44 const *	m_pTransform;
	XEngine::XTexture const *	m_pTexture;

	friend class CMeshRenderer;
};

class CMeshRenderer
{
public:
	CMeshRenderer();
	~CMeshRenderer();
	void Init(XEngine::XGraphics * pGraphics);
	void Deinit();
	void Render() const;
protected:
	void AddToRenderList(CMeshGraphic * pMeshGraphic);
	void RemoveFromRenderList(CMeshGraphic * pMeshGraphic);
	XEngine::XGraphics * GetGraphics();

	XEngine::XList<CMeshGraphic, 0>	m_oRenderList;
	mutable	XEngine::XMutex			m_oMutex; 

	bool							m_bInitialized;

	XEngine::XGraphics *			m_pGraphics;

	friend class CMeshGraphic;
};