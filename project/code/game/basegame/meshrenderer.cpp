#include "game/shared/gamepch.h"
#include "game/basegame/meshrenderer.h"

#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xtexture.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CMeshSetupFunctor::CMeshSetupFunctor()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

CMeshSetupFunctor::~CMeshSetupFunctor()
{
	Deinit();
}

///------------------------------------------------------------------------

void CMeshSetupFunctor::Init( void (* pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &) )
{
	XEngine::XBaseFunctor::Init(nullptr, &pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &CMeshSetupFunctor::Thunk;
}

///------------------------------------------------------------------------

void CMeshSetupFunctor::Deinit()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

void CMeshSetupFunctor::operator()( XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer )
{
	if (m_pThunk != nullptr)
	{
		(this->*m_pThunk)(pGraphics, rMesh, rVertexBuffer);
	}
}

///------------------------------------------------------------------------

void CMeshSetupFunctor::Thunk( XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer )
{
	void (*pFunctionPointer)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &) = *(void (**)(XEngine::XGraphics *, XEngine::XMesh &, XEngine::XVertexBuffer &))m_pFunctionPointer;
	(*pFunctionPointer)(pGraphics, rMesh, rVertexBuffer);
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CMeshGraphic::CMeshGraphic()
{
	Reset();

}

///------------------------------------------------------------------------

CMeshGraphic::~CMeshGraphic()
{
	Deinit();
}

///------------------------------------------------------------------------

void CMeshGraphic::Init( CMeshRenderer * pRenderer, XEngine::XMatrix44 const * pTransform, CMeshSetupFunctor & rMeshSetupCallback, XEngine::XColor const & rColor, XEngine::XTexture const * pTexture /*= nullptr*/ )
{
	XASSERT(!m_bInitialized);
	XASSERT_PTR(pRenderer);
	XASSERT_PTR(pTransform);
	m_pRenderer = pRenderer;
	m_pTransform = pTransform;
	m_oColor = rColor;
	m_pTexture = pTexture;
	rMeshSetupCallback(pRenderer->m_pGraphics, m_oMesh, m_oVertexBuffer);
	m_bInitialized = true;
}

///------------------------------------------------------------------------

void CMeshGraphic::Deinit()
{
	if (m_bInitialized)
	{
		if (m_bRendering)
		{
			StopRendering();
		}
		m_oMesh.GetIndexBuffer().Deinit(m_pRenderer->m_pGraphics);
		m_oVertexBuffer.Deinit(m_pRenderer->m_pGraphics);
		Reset();
	}
}

///------------------------------------------------------------------------

void CMeshGraphic::Reset()
{
	m_bInitialized = false;
	m_bRendering = false;
	m_pRenderer = nullptr;
	m_pTransform = nullptr;
	m_pTexture = nullptr;
}

///------------------------------------------------------------------------

void CMeshGraphic::StartRendering()
{
	XASSERT(m_bInitialized);
	if (!m_bRendering)
	{
		m_pRenderer->AddToRenderList(this);
		m_bRendering = true;
	}
}

///------------------------------------------------------------------------

void CMeshGraphic::StopRendering()
{
	XASSERT(m_bInitialized);
	if (m_bRendering)
	{
		m_pRenderer->RemoveFromRenderList(this);
		m_bRendering = false;
	}
}

///------------------------------------------------------------------------

void CMeshGraphic::Render() const
{
	XASSERT(m_bInitialized);
	XASSERT(m_bRendering);
	XEngine::XGraphics * pGraphics = m_pRenderer->m_pGraphics;

	pGraphics->PushRenderState();
	pGraphics->SetColor( m_oColor );
	///A branch.  Hmm, maybe this needs to be a separate kind of renderer.
	if (m_pTexture != nullptr)
	{
		m_pTexture->Select(pGraphics, 0);
		pGraphics->EnableTexture(0, true);
	}

	pGraphics->SetModelMatrix( *m_pTransform );
	m_oMesh.Draw(pGraphics);
	pGraphics->PopRenderState();
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CMeshRenderer::CMeshRenderer()
{
	m_pGraphics = nullptr;
	m_bInitialized = false;
}

///------------------------------------------------------------------------

CMeshRenderer::~CMeshRenderer()
{
	Deinit();
}

///------------------------------------------------------------------------

void CMeshRenderer::Init( XEngine::XGraphics * pGraphics )
{
	XASSERT_PTR(pGraphics);
	XASSERT(!m_bInitialized);
	m_pGraphics = pGraphics;
	m_oMutex.Init();
	m_bInitialized = true;
}

///------------------------------------------------------------------------

void CMeshRenderer::Deinit()
{
	if (m_bInitialized)
	{
		m_oMutex.Deinit();
		m_oRenderList.Reset();
		m_pGraphics = nullptr;
		m_bInitialized = false;
	}
}
///------------------------------------------------------------------------

void CMeshRenderer::Render() const
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	CMeshGraphic * pGraphicToRender = m_oRenderList.GetFront();
	while (pGraphicToRender != m_oRenderList.GetEndMarker())
	{
		pGraphicToRender->Render();
		pGraphicToRender = pGraphicToRender->GetNext<0>();
	}
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

void CMeshRenderer::AddToRenderList( CMeshGraphic * pMeshGraphic )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	m_oRenderList.PushBack(pMeshGraphic);
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

void CMeshRenderer::RemoveFromRenderList( CMeshGraphic * pMeshGraphic )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	m_oRenderList.Remove(pMeshGraphic);
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

XEngine::XGraphics * CMeshRenderer::GetGraphics()
{
	XASSERT(m_bInitialized);
	return m_pGraphics;
}

///------------------------------------------------------------------------