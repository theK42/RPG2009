#include "game/shared/gamepch.h"
#include "game/basegame/inputsystem.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CPointerInputFunctor::CPointerInputFunctor()
{
	m_pThunk = nullptr;
}

///----------------------------------------------------------------------------

CPointerInputFunctor::~CPointerInputFunctor()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CPointerInputFunctor::Init( void (* pFunctionPointer)(float, float) )
{
	XEngine::XBaseFunctor::Init(nullptr, &pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &CPointerInputFunctor::Thunk;
}

///----------------------------------------------------------------------------

void CPointerInputFunctor::Deinit()
{
	m_pThunk = nullptr;
}

///----------------------------------------------------------------------------

void CPointerInputFunctor::operator()( float fPointerX, float fPointerY )
{
	if (m_pThunk != nullptr)
	{
		(this->*m_pThunk)(fPointerX, fPointerY);
	}
}

///----------------------------------------------------------------------------

void CPointerInputFunctor::Thunk( float fPointerX, float fPointerY )
{
	void(*pFunctionPointer)(float, float) = *(void (**)(float, float))m_pFunctionPointer;
	return (*pFunctionPointer)(fPointerX, fPointerY);
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CPointerInputHandler::CPointerInputHandler()
{
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

CPointerInputHandler::~CPointerInputHandler()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CPointerInputHandler::Init( XEngine::XTestFunctor const & rTestFunctor )
{
	XASSERT(!m_bInitialized);
	m_oTestFunctor = rTestFunctor;
}

///----------------------------------------------------------------------------

void CPointerInputHandler::Deinit()
{
	if (m_bInitialized)
	{
		StopHandlingInput();
		m_oTestFunctor.Deinit();
	}
}

///----------------------------------------------------------------------------

void CPointerInputHandler::Update( float fTime, float fPointerX, float fPointerY )
{
	XASSERT(m_bInitialized);
	if (m_oTestFunctor())
	{
		m_oPointerInputFunctor(fPointerX, fPointerY);
	}
}

///----------------------------------------------------------------------------

void CPointerInputHandler::StartHandlingInput( CPointerInputFunctor const & rPointerInputFunctor )
{
	XASSERT(m_bInitialized);
	m_oPointerInputFunctor = rPointerInputFunctor;
}

///----------------------------------------------------------------------------

void CPointerInputHandler::StopHandlingInput()
{
	XASSERT(m_bInitialized);
	m_oPointerInputFunctor.Deinit();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CInputHandler::CInputHandler()
{
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

CInputHandler::~CInputHandler()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CInputHandler::Init( XEngine::XTestFunctor const & rTestFunctor )
{
	XASSERT(!m_bInitialized);
	m_oTestFunctor = rTestFunctor;
	m_bInitialized = true;
}

///----------------------------------------------------------------------------

void CInputHandler::Deinit()
{
	if (m_bInitialized)
	{
		m_oTestFunctor.Deinit();
		StopHandlingInput();
	}
}

///----------------------------------------------------------------------------

void CInputHandler::Update( float fTime )
{
	XASSERT(m_bInitialized);
	if (m_oTestFunctor())
	{
		m_oOnInputFunctor();
	}
}

///----------------------------------------------------------------------------

void CInputHandler::StartHandlingInput( XEngine::XSimpleFunctor const & rOnInputFunctor )
{
	XASSERT(m_bInitialized);
	m_oOnInputFunctor = rOnInputFunctor;
}

///----------------------------------------------------------------------------

void CInputHandler::StopHandlingInput()
{
	XASSERT(m_bInitialized);
	m_oOnInputFunctor.Deinit();
}

///----------------------------------------------------------------------------

