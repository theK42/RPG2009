#pragma once
#include "engine/algorithms/xsingleton.h"


/// ModuleFactory is responsible for managing memory and creating modules.
/// You can use a ModuleFactory to create a module in two distinct ways:
/// it may be initialized with memory set aside for creating one or more modules,
/// or the a one-off Module may be created using a passed in Allocator.

template <typename T>
class CModuleFactory : XEngine::XSingleton<CModuleFactory>
{
public:
	void Initialize(XEngine::XAllocator * pMainAllocator, XEngine::XAllocator * pTempAllocator = nullptr, int nSlots = 1);
protected:
	T * Create(XEngine::XAllocator * pAllocator = nullptr, XEngine::XAllocator * pTempAllocator = nullptr );
};