#pragma once

#include "engine/core/xcore.h"

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	/// This singleton template works differently than some others you may have encountered.
	/// A class inheriting from XSingleton is not necessarily the final implementation, it may
	/// still only be an interface.  However, we still guarantee that only one object exists 
	/// inheriting from that interface, avoiding the disgusting "multiton" pattern.  
	/// Note that GetSingletonInstance() may return nullptr!
	/// The actual singleton object must be created separately, either as a static object or 
	/// as a member of a class, or even dynamically (not recommended).
	template <typename T>
	class XSingleton
	{
	public:
		static T * GetSingletonInstance();
	protected:
		XSingleton();
		~XSingleton();
	private:
		static T * ms_pSingletonInstance;
	};

	///------------------------------------------------------------------------

	template <typename T>
	T * XSingleton<T>::ms_pSingletonInstance = nullptr;

	///------------------------------------------------------------------------

	template <typename T>
	XSingleton<T>::XSingleton<T>()
	{
		XASSERT(ms_pSingletonInstance == nullptr);
		ms_pSingletonInstance = this;
	}

	///------------------------------------------------------------------------

	template <typename T>
	XSingleton<T>::~XSingleton<T>()
	{
		XASSERT(ms_pSingletonInstance == this);
		ms_pSingletonInstance = nullptr;
	}

	///------------------------------------------------------------------------

	template <typename T>
	T * XSingleton::GetSingletonInstance()
	{
		///Note the lack of assert.  It is the caller's responsibility to check for nullptr!
		return ms_pSingletonInstance; 
	}

	///------------------------------------------------------------------------
}
