#include "game/shared/gamepch.h"
#include "game/basegame/screensteward.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CScreenSteward::CScreenSteward()
{
	m_bInitialized = false;
	m_pScreenStewardship = nullptr;
	m_pNextSteward = nullptr;
}

///----------------------------------------------------------------------------

CScreenSteward::~CScreenSteward()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CScreenSteward::Init( XEngine::XTestFunctor & rReadyTest, XEngine::XSimpleFunctor & rBeginStewardship, XEngine::XSimpleFunctor & rEndStewardship )
{
	XASSERT(!m_bInitialized);
	m_oReadyTestFunctor = rReadyTest;
	m_oBeginStewardshipFunctor = rBeginStewardship;
	m_oEndStewardshipFunctor = rEndStewardship;
	m_bInitialized = true;
}

///----------------------------------------------------------------------------

void CScreenSteward::Deinit()
{
	if (m_bInitialized)
	{
		EndStewardship();
	}
}

///----------------------------------------------------------------------------

bool CScreenSteward::IsReady() const
{
	XASSERT(m_bInitialized);
	return m_pScreenStewardship == nullptr && m_oReadyTestFunctor();
}

///----------------------------------------------------------------------------

void CScreenSteward::TakeResponsibility( CScreenStewardship * pScreenStewardship )
{
	XASSERT(m_bInitialized);
	XASSERT(IsReady());
	m_pScreenStewardship = pScreenStewardship;
	pScreenStewardship->m_pCurrentSteward = this;
	m_oBeginStewardshipFunctor();
}

///----------------------------------------------------------------------------

void CScreenSteward::PassResponsibility( CScreenSteward * pNextSteward )
{
	XASSERT(m_bInitialized);
	m_pNextSteward = pNextSteward;
}

///----------------------------------------------------------------------------

void CScreenSteward::EndStewardship()
{
	XASSERT(m_bInitialized);
	if (m_pScreenStewardship != nullptr)
	{
		XASSERT(m_pScreenStewardship->IsPermittedToEnd());
		m_oEndStewardshipFunctor();
	}
}

///----------------------------------------------------------------------------

void CScreenSteward::Update( float fTime )
{
	XASSERT(m_bInitialized);
	if (m_pNextSteward != nullptr)
	{
		m_oEndStewardshipFunctor();
		m_pNextSteward->TakeResponsibility(m_pScreenStewardship);
		m_pScreenStewardship = nullptr;
		m_pNextSteward = nullptr;
	}
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CScreenStewardship::CScreenStewardship()
{
	m_pCurrentSteward = nullptr;
	m_bInitialized = false;
	m_bPermittedToEnd = false;
}

///----------------------------------------------------------------------------

CScreenStewardship::~CScreenStewardship()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CScreenStewardship::Init()
{
	XASSERT(!m_bInitialized);
	m_bInitialized = true;
	m_bPermittedToEnd = false;
}

///----------------------------------------------------------------------------

void CScreenStewardship::Deinit()
{
	if (m_bInitialized)
	{
		SetPermittedToEnd();
		if (m_pCurrentSteward != nullptr)
		{
			m_pCurrentSteward->EndStewardship();
		}
	}
}

///----------------------------------------------------------------------------

void CScreenStewardship::Update( float fTime )
{
	XASSERT(m_bInitialized);
	if (m_pCurrentSteward != nullptr)
	{
		m_pCurrentSteward->Update(fTime);
	}
}

///----------------------------------------------------------------------------

bool CScreenStewardship::IsPermittedToEnd() const
{
	XASSERT(m_bInitialized);
	return m_bPermittedToEnd;
}

///----------------------------------------------------------------------------

void CScreenStewardship::SetPermittedToEnd()
{
	XASSERT(m_bInitialized);
	m_bPermittedToEnd = true;
}

///----------------------------------------------------------------------------

CScreenSteward * CScreenStewardship::GetCurrentSteward() const
{
	XASSERT(m_bInitialized);
	return m_pCurrentSteward;
}

///----------------------------------------------------------------------------