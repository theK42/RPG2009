#pragma once

#include "game/basegame/meshrenderer.h"
#include "game/basegame/mechanicaltransform.h"
#include "game/basegame/screensteward.h"

#include "engine/graphics/xmesh.h"
#include "engine/graphics/xvertexbuffer.h"


class CMiniLoad
{
public:
	CMiniLoad();
	~CMiniLoad();

	void Init(CMeshRenderer * pRenderer, CMechanicsUpdater * pUpdater);
	void Deinit();

	CScreenSteward * GetScreenSteward();

protected:

	bool IsReady();
	void StartDisplayingAnimation();
	void StopDisplayingAnimation();
	void SetupLineMesh(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);

	XEngine::XMatrix44 				m_oStaticTransform;
	CUpdatingMechanicalTransform	m_oAnimatedTransform;
	CMeshGraphic					m_oStaticMesh;
	CMeshGraphic					m_oAnimatedMesh;
	CScreenSteward					m_oScreenSteward;
	bool							m_bInitialized;
	bool							m_bDisplayingAnimation;

	static const int				msc_nNumVertices = 2;
	static const int				msc_nVertexWidth = 3;
	static float					ms_pVertices[msc_nNumVertices][msc_nVertexWidth];
	static u8						ms_pIndices[msc_nNumVertices];
};
