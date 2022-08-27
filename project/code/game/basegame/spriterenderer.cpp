#include "game/shared/gamepch.h"
#include "game/basegame/spriterenderer.h"

#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xtexture.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CSprite::CSprite()
{
	Reset();
}

///------------------------------------------------------------------------

CSprite::~CSprite()
{
	Deinit();
}

///------------------------------------------------------------------------

void CSprite::Reset()
{
	m_pRenderer = nullptr;
	m_pTexture = nullptr;
	m_fWidth = 0;
	m_fHeight = 0;
	m_bRendering = false;
}

///------------------------------------------------------------------------

void CSprite::Init(CSpriteRenderer * pRenderer, XVector4 const & rPos, float fWidth, float fHeight, XEngine::XTexture * pTexture, SRectangle const & rTextureArea /* = SRectangle(0.0f, 0.0f, 1.0f, 1.0f) */)
{
	XASSERT(m_pRenderer == nullptr);
	XASSERT(pRenderer != nullptr);

	m_pRenderer = pRenderer;
	m_vPosition = rPos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_pVertices[eCorner_UpperLeft].m_fPosX = 0.0f;
	m_pVertices[eCorner_UpperLeft].m_fPosY = 0.0f;
	m_pVertices[eCorner_UpperLeft].m_fPosZ = 0.0f;
	m_pVertices[eCorner_UpperLeft].m_fTexU = rTextureArea.m_fX0;
	m_pVertices[eCorner_UpperLeft].m_fTexV = rTextureArea.m_fY0;
	m_pIndices[eCorner_UpperLeft] = eCorner_UpperLeft;

	m_pVertices[eCorner_UpperRight].m_fPosX = m_fWidth;
	m_pVertices[eCorner_UpperRight].m_fPosY = 0.0f;
	m_pVertices[eCorner_UpperRight].m_fPosZ = 0.0f;
	m_pVertices[eCorner_UpperRight].m_fTexU = rTextureArea.m_fX1;
	m_pVertices[eCorner_UpperRight].m_fTexV = rTextureArea.m_fY0;
	m_pIndices[eCorner_UpperRight] = eCorner_UpperRight;

	m_pVertices[eCorner_LowerRight].m_fPosX = m_fWidth;
	m_pVertices[eCorner_LowerRight].m_fPosY = m_fHeight;
	m_pVertices[eCorner_LowerRight].m_fPosZ = 0.0f;
	m_pVertices[eCorner_LowerRight].m_fTexU = rTextureArea.m_fX1;
	m_pVertices[eCorner_LowerRight].m_fTexV = rTextureArea.m_fY1;
	m_pIndices[eCorner_LowerRight] = eCorner_LowerRight;

	m_pVertices[eCorner_LowerLeft].m_fPosX = 0.0f;
	m_pVertices[eCorner_LowerLeft].m_fPosY = m_fHeight;
	m_pVertices[eCorner_LowerLeft].m_fPosZ = 0.0f;
	m_pVertices[eCorner_LowerLeft].m_fTexU = rTextureArea.m_fX0;
	m_pVertices[eCorner_LowerLeft].m_fTexV = rTextureArea.m_fY1;
	m_pIndices[eCorner_LowerLeft] = eCorner_LowerLeft;

	XEngine::XVertexFormat oVertextFormat;
	oVertextFormat.Add( XEngine::XVertexAttributeId::Position, XEngine::XVertexAttributeType::F32, 3 );
	oVertextFormat.Add( XEngine::XVertexAttributeId::TexCoord0, XEngine::XVertexAttributeType::F32, 2); 
	
	m_oVertexBuffer.Init(
		pRenderer->GetGraphics(),
		oVertextFormat,
		XEngine::XBufferFlags::DynamicDraw,  ///I'm hoping this means "use my memory, don't be alarmed if stuff changes"
		eCorner_Count,
		m_pVertices );
	m_oMesh.GetVertexBufferSet().Add( &m_oVertexBuffer );


	m_oMesh.GetIndexBuffer().Init(
		pRenderer->GetGraphics(),
		XEngine::XPrimitiveType::TriangleFan,
		XEngine::XIndexType::U8,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		eCorner_Count,
		m_pIndices );

}

///------------------------------------------------------------------------

void CSprite::Deinit()
{
	if (m_bRendering)
	{
		StopRendering();
	}
	m_oMesh.GetIndexBuffer().Deinit(m_pRenderer->GetGraphics());
	m_oVertexBuffer.Deinit(m_pRenderer->GetGraphics());
	Reset();
}

///------------------------------------------------------------------------

void CSprite::BeginRendering()
{
	XASSERT_PTR(m_pRenderer);
	m_pRenderer->AddToRenderList(this);
	m_bRendering = true;
}

///------------------------------------------------------------------------

void CSprite::StopRendering()
{
	XASSERT(m_bRendering);
	m_pRenderer->RemoveFromRenderList(this);
	m_bRendering = false;
}

///------------------------------------------------------------------------

void CSprite::Render() const
{
	XASSERT(m_pRenderer != nullptr);
	XEngine::XGraphics * pGraphics = m_pRenderer->GetGraphics();
	pGraphics->PushRenderState();
	m_pTexture->Select(pGraphics, 0);
	pGraphics->EnableTexture(0, true);
	m_oMesh.Draw(pGraphics);
	pGraphics->PopRenderState();
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CSpriteRenderer::CSpriteRenderer()
{
	m_pGraphics = nullptr;
}

///------------------------------------------------------------------------

CSpriteRenderer::~CSpriteRenderer()
{
	XASSERT(m_pGraphics == nullptr);
}

///------------------------------------------------------------------------

void CSpriteRenderer::Init( XEngine::XGraphics * pGraphics )
{
	XASSERT(m_pGraphics == nullptr);
	XASSERT(pGraphics != nullptr);
	m_pGraphics = pGraphics;
	m_oMutex.Init();
}

///------------------------------------------------------------------------

void CSpriteRenderer::Deinit()
{
	if (m_pGraphics != nullptr)
	{	
		XASSERT(m_oRenderList.IsEmpty());
		m_pGraphics = nullptr;
	}
}

///------------------------------------------------------------------------

void CSpriteRenderer::Render() const
{
	XASSERT(m_pGraphics != nullptr);
	m_oMutex.Lock();
	CSprite * pSpriteToRender = m_oRenderList.GetFront();
	while (pSpriteToRender != m_oRenderList.GetEndMarker())
	{
		pSpriteToRender->Render();
		pSpriteToRender = pSpriteToRender->GetNext<0>();
	}
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

void CSpriteRenderer::AddToRenderList( CSprite * pSprite )
{
	XASSERT(m_pGraphics != nullptr);
	m_oMutex.Lock();
	m_oRenderList.PushBack(pSprite);
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

void CSpriteRenderer::RemoveFromRenderList( CSprite * pSprite )
{
	XASSERT(m_pGraphics != nullptr);
	m_oMutex.Lock();
	m_oRenderList.Remove(pSprite);
	m_oMutex.Unlock();
}

///------------------------------------------------------------------------

XEngine::XGraphics * CSpriteRenderer::GetGraphics() const
{
	///Not so much asserting that this function shouldn't return nullptr, as that this object is initialized
	XASSERT(m_pGraphics != nullptr);  
	return m_pGraphics;
}

///------------------------------------------------------------------------