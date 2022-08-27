#pragma once

#include "game/basegame/resourcecomponent.h"
#include "game/basegame/meshrenderer.h"
#include "game/basegame/luaenvironment.h"
#include "game/basegame/screensteward.h"
#include "game/basegame/mechanicaltransform.h"

class CLuaTester
{
public:
	CLuaTester();
	~CLuaTester();

	void Init(CLoadProcessor * pLoader, CMeshRenderer * pRenderer, CLuaEnvironmentUpdater * pScriptUpdater, CMechanicsUpdater * pMechanicsUpdater);
	void Deinit();
	void StartScript(char const * pScriptName);
	void StopScript();

	CScreenSteward * GetScreenSteward();

protected:

	bool IsReady();
	void PrepareEnvironment();
	void StartScript();
	void SetupMesh(XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer);

	CUpdatingMechanicalTransform	m_oAnimatedTransform;
	CMeshGraphic					m_oMesh;
	CScreenSteward					m_oScreenSteward;
	bool							m_bInitialized;

	static const int				msc_nNumVertices = 4;
	static const int				msc_nVertexWidth = 3;
	static float					ms_pVertices[msc_nNumVertices][msc_nVertexWidth];
	static u8						ms_pIndices[msc_nNumVertices];
};