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
	///------------------------------------------------------------------------

	template<class TParameterClass> class XOneParameterProcedureFunctor
	{
	public:
		XOneParameterProcedureFunctor();
		~XOneParameterProcedureFunctor();
		void Init(void (* pFunctionPointer)(TParameterClass oParameter));
		template<class TClass>
		void Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(TParameterClass));

		void Deinit();

		void operator()(TParameterClass oParameter);

		static XOneParameterProcedureFunctor	ms_oDoNothing;
	private:
		void Thunk(TParameterClass oParameter);
		template<class TClass>
		void TThunk(TParameterClass oParameter);

		void (XOneParameterProcedureFunctor::* m_pThunk)(TParameterClass oParameter);
	};

	///------------------------------------------------------------------------

	template<class TParameterClass>
	XOneParameterProcedureFunctor<TParameterClass>::XOneParameterProcedureFunctor()
	{
		m_pThunk = nullptr;
	}

	///------------------------------------------------------------------------

	template<class TParameterClass>
	XOneParameterProcedureFunctor<TParameterClass>::~XOneParameterProcedureFunctor()
	{
		Deinit();
	}

	///------------------------------------------------------------------------

	template<class TParameterClass>
	void XOneParameterProcedureFunctor<TParameterClass>::Init(void (* pFunctionPointer)(TParameterClass oParameter))
	{
		XBaseFunctor::Init(nullptr, &pFunctionPointer, sizeof pFunctionPointer);
		m_pThunk = &XOneParameterProcedureFunctor<TParameterClass>::Thunk;
	}

	///------------------------------------------------------------------------

	template<class TParameterClass>
	template<class TClass>
	void XOneParameterProcedureFunctor<TParameterClass>::Init(TClass * pObjectPointer, void (TClass::* pFunctionPointer)(TParameterClass))
	{
		XBaseFunctor::Init(pObjectPointer, &pFunctionPointer, sizeof pFunctionPointer);
		m_pThunk = &XOneParameterProcedureFunctor<TParameterClass>::TThunk<TClass>;
	}

	template<class TParameterClass>
	void XOneParameterProcedureFunctor<TParameterClass>::Deinit()
	{
		m_pThunk = nullptr;
	}

	///------------------------------------------------------------------------

	template<class TParameterClass>
	void XEngine::XOneParameterProcedureFunctor<TParameterClass>::Thunk( TParameterClass oParameter )
	{
		void (*pFunctionPointer)(TParameterClass) = *(void (**)(TParameterClass))m_pFunctionPointer;
		(*pFunctionPointer)(oParameter);
	}

	///------------------------------------------------------------------------

	template<class TParameterClass>
	template<class TClass>
	void XEngine::XOneParameterProcedureFunctor<TParameterClass>::TThunk( TParameterClass oParameter )
	{
		void (TClass::*pFunctionPointer)(TParameterClass) = *(void (TClass::**)(TParameterClass))m_pFunctionPointer;
		TClass * pObjectPointer = (TClass *) m_pObjectPointer;
		return (pObjectPointer->*pFunctionPointer)(oParameter);
	}

	///------------------------------------------------------------------------
}