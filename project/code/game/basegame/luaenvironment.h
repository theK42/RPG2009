
#pragma once

#include "game/basegame/updater.h"

#include "engine/algorithms/xfunctor.h"
#include "engine/threading/xmutex.h"
#include "engine/resource/xresdb.h"

#include "game/lua/lua.h"

namespace XEngine
{
	class XAllocator;
}

typedef XEngine::XOneParameterProcedureFunctor<lua_State *> CLuaEnvironmentSetupFunctor;

class CLuaEnvironment
{
public:
	CLuaEnvironment();
	~CLuaEnvironment();

	void Init(XEngine::XAllocator * pAllocator, CLuaEnvironmentSetupFunctor & rSetupFunctor);
	void Deinit();

	void Update(float fTime);

	lua_State * CreateCoroutine();
	void KillCoroutine(lua_State * pKill);

private:
	static void * LuaAlloc(void * pLuaWorld, void * pOldPointer, size_t nOldSize, size_t nNewSize);

	XEngine::XMutex			m_oMutex;
	bool					m_bInitialized;
	XEngine::XAllocator *	m_pAllocator;
	lua_State *				m_pMainState;
};


class CUpdatingLuaEnvironment : public CUpdating<CLuaEnvironment>
{
public:
	void Init(CUpdater<CLuaEnvironment> * pUpdater, XEngine::XAllocator * pAllocator, CLuaEnvironmentSetupFunctor & rSetupFunctor);
};

namespace LuaHelper
{
	void LoadScript(lua_State * pState, XEngine::XResourceDatabase const * pResDB, char const * pName);
}

typedef CUpdater<CLuaEnvironment> CLuaEnvironmentUpdater;