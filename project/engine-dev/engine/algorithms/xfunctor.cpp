#include "engine/core/xcore.h"
#include "engine/algorithms/xfunctor.h"

#include <cstring>

///------------------------------------------------------------------------
///------------------------------------------------------------------------

XEngine::XBaseFunctor::XBaseFunctor()
{
	m_pFunctionPointer[0] = nullptr; ///Yes I know there could still be junk in the rest of it.  Just going to assume that all function pointers will use the first byte for something
	m_pObjectPointer = nullptr;
}

///------------------------------------------------------------------------

void XEngine::XBaseFunctor::Init( void * pObjectPointer, void * ppFunctionPointer, size_t nFunctionPointerSize )
{
	m_pObjectPointer = pObjectPointer;
	XASSERT(nFunctionPointerSize <= MAX_FUNCTION_POINTER_SIZE);
	std::memcpy(m_pFunctionPointer, ppFunctionPointer, nFunctionPointerSize);
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

XEngine::XSimpleFunctor::XSimpleFunctor()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

XEngine::XSimpleFunctor::~XSimpleFunctor()
{
	Deinit();
}

///------------------------------------------------------------------------

void XEngine::XSimpleFunctor::Init( void (* pFunctionPointer)() )
{
	XBaseFunctor::Init(nullptr, &pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &XSimpleFunctor::Thunk;
}

///------------------------------------------------------------------------

void XEngine::XSimpleFunctor::Deinit()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

void XEngine::XSimpleFunctor::Thunk()
{
	void (*pFunctionPointer)() = *(void (**)())m_pFunctionPointer;
	(*pFunctionPointer)();
}

///------------------------------------------------------------------------

void XEngine::XSimpleFunctor::operator()()
{
	if (m_pThunk != nullptr)
	{
		(this->*m_pThunk)();
	}
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

XEngine::XTestFunctor::XTestFunctor()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

XEngine::XTestFunctor::~XTestFunctor()
{
	Deinit();
}

///------------------------------------------------------------------------

void XEngine::XTestFunctor::Init( bool (* pFunctionPointer)() )
{
	XBaseFunctor::Init(nullptr, &pFunctionPointer, sizeof pFunctionPointer);
	m_pThunk = &XTestFunctor::Thunk;
}

///------------------------------------------------------------------------

void XEngine::XTestFunctor::Deinit()
{
	m_pThunk = nullptr;
}

///------------------------------------------------------------------------

bool XEngine::XTestFunctor::operator()() const
{
	if (m_pThunk != nullptr)
	{
		return (this->*m_pThunk)();
	}
	return false;
}

///------------------------------------------------------------------------

bool XEngine::XTestFunctor::Thunk() const
{
	bool (*pFunctionPointer)() = *(bool (**)())m_pFunctionPointer;
	return (*pFunctionPointer)();
}

///------------------------------------------------------------------------