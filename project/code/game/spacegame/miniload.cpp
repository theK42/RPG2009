#include "game/shared/gamepch.h"
#include "game/spacegame/miniload.h"
#include "engine/graphics/xgraphics.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

float CMiniLoad::ms_pVertices[msc_nNumVertices][msc_nVertexWidth] =
{
	{0.0f, 0.0f, 0.0f},
	{0.0f, -1.0f, 0.0f},	
};

///----------------------------------------------------------------------------

u8 CMiniLoad::ms_pIndices[msc_nNumVertices] =
{
	0,
	1
};

///----------------------------------------------------------------------------

CMiniLoad::CMiniLoad()
{
	m_bInitialized = false;
	m_bDisplayingAnimation = false;
}

///----------------------------------------------------------------------------

CMiniLoad::~CMiniLoad()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CMiniLoad::Init(CMeshRenderer * pRenderer, CMechanicsUpdater * pUpdater)
{
	XASSERT(!m_bInitialized);
	m_oStaticTransform.MakeTranslate(XVector4(146.0f, 88.0f, -1.0f));
	m_oAnimatedTransform.Init(pUpdater, m_oStaticTransform);

	CMeshSetupFunctor oMeshSetupFunctor;
	oMeshSetupFunctor.Init(this, &CMiniLoad::SetupLineMesh);
	
	XEngine::XColor oColor;
	oColor.Set(1.0f, 1.0f, 1.0f, 1.0f);  /// Blinding white

	m_oStaticMesh.Init(pRenderer, &m_oStaticTransform, oMeshSetupFunctor, oColor);
	m_oAnimatedMesh.Init(pRenderer, m_oAnimatedTransform.GetInternalUpdatable().GetMatrix(), oMeshSetupFunctor, oColor);
	
	XEngine::XTestFunctor oReadyTestFunctor;
	oReadyTestFunctor.Init(this, &CMiniLoad::IsReady);

	XEngine::XSimpleFunctor oStartDisplayingAnimationFunctor;
	oStartDisplayingAnimationFunctor.Init(this, &CMiniLoad::StartDisplayingAnimation);

	XEngine::XSimpleFunctor oStopDisplayingAnimationFunctor;
	oStopDisplayingAnimationFunctor.Init(this, &CMiniLoad::StopDisplayingAnimation);

	m_oScreenSteward.Init(oReadyTestFunctor, oStartDisplayingAnimationFunctor, oStopDisplayingAnimationFunctor);
	m_bInitialized = true;
}

///----------------------------------------------------------------------------

void CMiniLoad::Deinit()
{
	if (m_bInitialized)
	{
		if (m_bDisplayingAnimation)
		{
			StopDisplayingAnimation();
			m_bDisplayingAnimation = false;
		}
		m_oScreenSteward.Deinit();
		m_oStaticMesh.Deinit();
		m_oAnimatedMesh.Deinit();
		m_oAnimatedTransform.Deinit();
		m_bInitialized = false;
	}
}

///----------------------------------------------------------------------------

CScreenSteward * CMiniLoad::GetScreenSteward()
{
	XASSERT(m_bInitialized);
	return &m_oScreenSteward;
}

///----------------------------------------------------------------------------

bool CMiniLoad::IsReady()
{
	XASSERT(m_bInitialized);
	return true; ///I'm always ready!
}

///----------------------------------------------------------------------------

void CMiniLoad::StartDisplayingAnimation()
{
	XASSERT(m_bInitialized);
	if (!m_bDisplayingAnimation)
	{
		m_oAnimatedTransform.GetInternalUpdatable().SetVelocities(XVector4(ZERO), XVector4(UNIT_Z), 2 * XEngine::PI);
		m_oAnimatedTransform.Start();
		m_oStaticMesh.StartRendering();
		m_oAnimatedMesh.StartRendering();
		m_bDisplayingAnimation = true;
	}
}

///----------------------------------------------------------------------------

void CMiniLoad::StopDisplayingAnimation()
{
	XASSERT(m_bInitialized);
	if (m_bDisplayingAnimation)
	{
		m_oAnimatedTransform.Stop();
		m_oStaticMesh.StopRendering();
		m_oAnimatedMesh.StopRendering();
		m_bDisplayingAnimation = false;
	}
}

///----------------------------------------------------------------------------

void CMiniLoad::SetupLineMesh( XEngine::XGraphics * pGraphics, XEngine::XMesh & rMesh, XEngine::XVertexBuffer & rVertexBuffer )
{
	XEngine::XVertexFormat oVertextFormat;
	oVertextFormat.Add( XEngine::XVertexAttributeId::Position, XEngine::XVertexAttributeType::F32, msc_nVertexWidth );

	rVertexBuffer.Init(
		pGraphics,
		oVertextFormat,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		msc_nNumVertices,
		ms_pVertices );
	rMesh.GetVertexBufferSet().Add( &rVertexBuffer );

	rMesh.GetIndexBuffer().Init(
		pGraphics,
		XEngine::XPrimitiveType::LineList,
		XEngine::XIndexType::U8,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		msc_nNumVertices,
		ms_pIndices );
}

///----------------------------------------------------------------------------