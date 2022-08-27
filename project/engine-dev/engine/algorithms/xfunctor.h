#pragma once

#include "engine/core/xcore.h"

#define MAX_FUNCTION_POINTER_SIZE 2 * sizeof(uaddr)

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
 
	class XBaseFunctor 
	{
	public:
		XBaseFunctor();

	protected:
		void Init(void * pObjectPointer, void * ppFunctionPointer, size_t nFunctionPointerSize);
		
		u8 m_pFunctionPointer[MAX_FUNCTION_POINTER_SIZE];
		
		void * m_pObjectPointer;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XSimpleFunctor : protected XBaseFunctor 
	{
	public:
		XSimpleFunctor();
		~XSimpleFunctor();

		void Init(void (* pFunctionPointer)());
		template<class TClass>
		void Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)());
		void Deinit();

		void operator()();

		static XSimpleFunctor	ms_oDoNothing;
	private:
		void Thunk();

		template<class TClass>
		void TThunk();

		void (XSimpleFunctor::* m_pThunk)();
	};

	///------------------------------------------------------------------------

	template<class TClass>
	void XSimpleFunctor::Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)())
	{
		XBaseFunctor::Init(pObjectPointer, &pFunctionPointer, sizeof pFunctionPointer);
		m_pThunk = &XSimpleFunctor::TThunk<TClass>;
	}

	///------------------------------------------------------------------------

	template<class TClass>
	void XSimpleFunctor::TThunk()
	{
		void (TClass::*pFunctionPointer)() = *(void (TClass::**)())m_pFunctionPointer;
		TClass * pObjectPointer = (TClass * )m_pObjectPointer;
		(pObjectPointer->*pFunctionPointer)();
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XTestFunctor : protected XBaseFunctor 
	{
	public:
		XTestFunctor();
		~XTestFunctor();
		void Init(bool (* pFunctionPointer)());
		template<class TClass>
		void Init(TClass * pObjectPointer, bool (TClass::* pFunctionPointer)());
		void Deinit();

		bool operator()() const;

	private:
		bool Thunk() const;
		template<class TClass>
		bool TThunk() const;

		bool (XTestFunctor::* m_pThunk)() const;
	};

	///------------------------------------------------------------------------

	template<class TClass>
	void XTestFunctor::Init(TClass * pObjectPointer, bool (TClass::* pFunctionPointer)())
	{
		XBaseFunctor::Init(pObjectPointer, &pFunctionPointer, sizeof pFunctionPointer);
		m_pThunk = &XTestFunctor::TThunk<TClass>;
	}

	///------------------------------------------------------------------------

	template<class TClass>
	bool XTestFunctor::TThunk() const
	{
		bool (TClass::*pFunctionPointer)() = *(bool (TClass::**)())m_pFunctionPointer;
		TClass * pObjectPointer = (TClass *) m_pObjectPointer;
		return (pObjectPointer->*pFunctionPointer)();
	}

	///------------------------------------------------------------------------

}