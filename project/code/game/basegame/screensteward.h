#pragma once

#include "engine/algorithms/xfunctor.h"

class CScreenStewardship;

class CScreenSteward
{
public:
	CScreenSteward();
	~CScreenSteward();

	void Init(XEngine::XTestFunctor & rReadyTest, XEngine::XSimpleFunctor & rBeginStewardship, XEngine::XSimpleFunctor & rEndStewardship);
	void Deinit();

	bool IsReady() const;
	void TakeResponsibility(CScreenStewardship * pScreenStewardship);
	void PassResponsibility(CScreenSteward * pNextSteward);

private:
	void Update(float fTime);
	void EndStewardship();
	XEngine::XTestFunctor	m_oReadyTestFunctor;
	XEngine::XSimpleFunctor	m_oBeginStewardshipFunctor;
	XEngine::XSimpleFunctor	m_oEndStewardshipFunctor;
	bool					m_bInitialized;
	CScreenStewardship *	m_pScreenStewardship;
	CScreenSteward *		m_pNextSteward;

	friend class CScreenStewardship;
};

class CScreenStewardship
{
public:
	CScreenStewardship();
	~CScreenStewardship();

	void Init();
	void Deinit();

	void Update(float fTime);

	bool IsPermittedToEnd() const;
	void SetPermittedToEnd();

	CScreenSteward *	GetCurrentSteward() const;
protected:
	CScreenSteward *		m_pCurrentSteward;
	bool					m_bInitialized;
	bool					m_bPermittedToEnd;
	friend class CScreenSteward;
};