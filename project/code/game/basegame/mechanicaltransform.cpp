#include "game/shared/gamepch.h"
#include "game/basegame/mechanicaltransform.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CMechanicalTransform::CMechanicalTransform()
{
	//m_pMechanicsUpdater = nullptr;
	m_bInitialized = false;
	//m_bMoving = false;
	m_fAngularVelocity = false;
}

///------------------------------------------------------------------------

CMechanicalTransform::~CMechanicalTransform()
{
	Deinit();
}

///------------------------------------------------------------------------

void CMechanicalTransform::Init( /*CMechanicsUpdater * pMechanicsUpdater, */XEngine::XMatrix44 const & rMatrix )
{
 	XASSERT(!m_bInitialized);
// 	XASSERT_PTR(pMechanicsUpdater);
// 	m_pMechanicsUpdater = pMechanicsUpdater;
	m_oTransform = rMatrix;
	m_bInitialized = true;
}

///------------------------------------------------------------------------

void CMechanicalTransform::Deinit()
{
	if (m_bInitialized)
	{
		m_bInitialized = false;
	}
}

///------------------------------------------------------------------------

void CMechanicalTransform::SetVelocities( XEngine::XVector4 const & rLinearVelocity, XEngine::XVector4 const & rAxisOfRotation, float fAngularVeloctiy )
{
	XASSERT(m_bInitialized);
	m_oLinearVelocity = rLinearVelocity;
	m_oAxisOfRotation = rAxisOfRotation;
	m_fAngularVelocity = fAngularVeloctiy;
}

///------------------------------------------------------------------------

void CMechanicalTransform::Update( float fTime )
{
	XVector4 vTranslation (m_oTransform(3,0), m_oTransform(3,1), m_oTransform(3,2), m_oTransform(3,3));

	XVector4 oLinearStep = m_oLinearVelocity;
	oLinearStep *= fTime;

	vTranslation += oLinearStep;
	
	m_oTransform(3, 0) = 0.0f;
	m_oTransform(3, 1) = 0.0f;
	m_oTransform(3, 2) = 0.0f;
	
	XMatrix44 oRotation;
	oRotation.MakeRotate(m_oAxisOfRotation, m_fAngularVelocity * fTime);

	XMatrix44::Multiply(m_oTransform, m_oTransform, oRotation);

	m_oTransform.PostTranslate(vTranslation);

}

///------------------------------------------------------------------------

XEngine::XMatrix44 const * CMechanicalTransform::GetMatrix() const
{
	return &m_oTransform;
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

void CUpdatingMechanicalTransform::Init( CUpdater<CMechanicalTransform> * pUpdater, XEngine::XMatrix44 const & rMatrix )
{
	m_oUpdatable.Init(rMatrix);
	CUpdating<CMechanicalTransform>::Init(pUpdater);
}

///------------------------------------------------------------------------