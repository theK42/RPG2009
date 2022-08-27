#pragma once

#include "engine/math/xmatrix.h"
#include "engine/math/xquaternion.h"
#include "engine/algorithms/xlist.h"
#include "engine/threading/xmutex.h"

#include "updater.h"


class CMechanicalTransform : public XEngine::XListElement<CMechanicalTransform, 1>
{
public:
	CMechanicalTransform();
	~CMechanicalTransform();

	void Init(XEngine::XMatrix44 const & rMatrix);
	void Deinit();

	void SetVelocities(XEngine::XVector4 const & rLinearVelocity, XEngine::XVector4 const & rAxisOfRotation, float fAngularVeloctiy);

	void Update(float fTime);

	XEngine::XMatrix44 const * GetMatrix() const;
protected:
	XEngine::XMatrix44		m_oTransform;
	XEngine::XVector4		m_oLinearVelocity;
	XEngine::XVector4		m_oAxisOfRotation;
	float					m_fAngularVelocity;
	bool					m_bInitialized;
};



class CUpdatingMechanicalTransform : public CUpdating<CMechanicalTransform>
{
public:
	void Init(CUpdater<CMechanicalTransform> * pUpdater, XEngine::XMatrix44 const & rMatrix);
};

typedef CUpdater<CMechanicalTransform> CMechanicsUpdater;