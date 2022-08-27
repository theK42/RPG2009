#pragma once

#include "engine/graphics/xmesh.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/algorithms/xlist.h"
#include "engine/threading/xmutex.h"
#include "game/basegame/rectangle.h"

namespace XEngine
{
	class XTexture;
}
class CSpriteRenderer;

class CSprite : public XEngine::XListElement<CSprite, 1>
{
public:
	CSprite();
	~CSprite();

	void Init(CSpriteRenderer * pRenderer, XVector4 const & rPos, float fWidth, float fHeight, XEngine::XTexture * pTexture, SRectangle const & rTextureArea = SRectangle(0.0f, 0.0f, 1.0f, 1.0f));
	void Deinit();

	void BeginRendering();
	void StopRendering();
protected:
	void Render() const;
	void Reset();

	enum eCorners
	{
		eCorner_UpperLeft,
		eCorner_UpperRight,
		eCorner_LowerRight,
		eCorner_LowerLeft,
		eCorner_Count
	};

	struct SVertex
	{

		void Reset();
		float	m_fPosX;
		float	m_fPosY;
		float	m_fPosZ;
		float	m_fTexU;
		float	m_fTexV;
	};

	CSpriteRenderer *		m_pRenderer;
	XEngine::XTexture *		m_pTexture;
	XVector4				m_vPosition;
	float					m_fWidth;
	float					m_fHeight;

	XEngine::XVertexBuffer	m_oVertexBuffer;
	XEngine::XMesh			m_oMesh;
	SVertex					m_pVertices[eCorner_Count];
	u8						m_pIndices[eCorner_Count];
	bool					m_bRendering;

	friend class CSpriteRenderer;
};

class CSpriteRenderer
{
public:
	CSpriteRenderer();
	~CSpriteRenderer();
	void Init(XEngine::XGraphics * pGraphics);
	void Deinit();
	void Render() const;
protected:
	void AddToRenderList(CSprite * pSprite);
	void RemoveFromRenderList(CSprite * pSprite);
	XEngine::XGraphics * GetGraphics() const;

	XEngine::XList<CSprite, 0>	m_oRenderList;
	mutable	XEngine::XMutex		m_oMutex; ///God I hate the mutable keyword, but I guess it's appropriate here
	XEngine::XGraphics *		m_pGraphics;

	friend class CSprite;
};