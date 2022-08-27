#pragma once

#include "engine/algorithms/xfunctor.h"
#include "engine/algorithms/xlist.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

class CPointerInputFunctor : protected XEngine::XBaseFunctor 
{
public:
	CPointerInputFunctor();
	~CPointerInputFunctor();

	void Init(void (* pFunctionPointer)(float, float));
	template<class TClass>
	void Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(float, float));
	void Deinit();

	void operator()(float fPointerX, float fPointerY);

	static CPointerInputFunctor	ms_oDoNothing;
private:

	void Thunk(float fPointerX, float fPointerY);

	template<class TClass>
	static void TThunk(float fPointerX, float fPointerY);

	void (CPointerInputFunctor::* m_pThunk)(float fPointerX, float fPointerY);
};

///----------------------------------------------------------------------------

template<class TClass>
void CPointerInputFunctor::Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(float, float))
{
	XBaseFunctor::Init(pObjectPointer, pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &Thunk<TClass>;
}

///----------------------------------------------------------------------------

template<class TClass>
void CPointerInputFunctor::TThunk(float fPointerX, float fPointerY)
{
	void TClass::(*pFunctionPointer)() = (void TClass::(*)(float, float))m_pFunctionPointer;
	m_pObjectPointer->*pFunctionPointer(fPointerX, fPointerY);
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

class CPointerInputHandler
{
public:
	CPointerInputHandler();
	~CPointerInputHandler();

	void Init(XEngine::XTestFunctor const & rTestFunctor);
	void Deinit();

	void Update(float fTime, float fPointerX, float fPointerY);

	void StartHandlingInput(CPointerInputFunctor const & rPointerInputFunctor);
	void StopHandlingInput();
protected:
	bool					m_bInitialized;
	XEngine::XTestFunctor	m_oTestFunctor;
	CPointerInputFunctor	m_oPointerInputFunctor;
};

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

class CInputHandler : public XEngine::XListElement<CInputHandler, 1>
{
public:
	CInputHandler();
	~CInputHandler();

	void Init(XEngine::XTestFunctor const & rTestFunctor);
	void Deinit();

	void Update(float fTime);

	void StartHandlingInput(XEngine::XSimpleFunctor const & rOnInputFunctor);
	void StopHandlingInput();

protected:
	XEngine::XTestFunctor	m_oTestFunctor;
	XEngine::XSimpleFunctor	m_oOnInputFunctor;
	bool m_bInitialized;
};

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------