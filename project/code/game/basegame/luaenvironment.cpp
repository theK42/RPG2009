#include "game/shared/gamepch.h"
#include "game/basegame/luaenvironment.h"
#include "game/lua/lualib.h"
#include "game/lua/lauxlib.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CLuaEnvironment::CLuaEnvironment()
{
	m_pAllocator = nullptr;
	m_pMainState = nullptr;
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

CLuaEnvironment::~CLuaEnvironment()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CLuaEnvironment::Init( XEngine::XAllocator * pAllocator, CLuaEnvironmentSetupFunctor & rSetupFunctor )
{
	m_oMutex.Init();
	m_pAllocator = pAllocator;
	m_pMainState = lua_newstate(&LuaAlloc, this);
	rSetupFunctor(m_pMainState);
	m_bInitialized = true;
}

///----------------------------------------------------------------------------

void CLuaEnvironment::Deinit()
{
	if (m_bInitialized)
	{
		m_oMutex.Deinit();
		if (m_pMainState != nullptr)
		{
			lua_close(m_pMainState);
			m_pMainState = nullptr;
			m_pAllocator = nullptr;
		}
		m_bInitialized = false;
	}
}

///----------------------------------------------------------------------------

void CLuaEnvironment::Update( float fTime )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	for (int i = lua_gettop(m_pMainState); i > 0; i--)
	{
		if (lua_isthread(m_pMainState, i))
		{
			lua_State * pThread = lua_tothread(m_pMainState, i);
			int nRetValue = lua_resume(pThread, 0);
			if (nRetValue == 0) /// 0 signals that this thread is finished
			{
				lua_remove(m_pMainState, i); /// Allows this thread to be garbage collected, safe since I'm iterating from top of stack down.
			}
			else if (nRetValue != LUA_YIELD)
			{
				XASSERT(false); ///Error, should do error handling code here.
			}		
		}
	}
	m_oMutex.Unlock();
}

///----------------------------------------------------------------------------

lua_State * CLuaEnvironment::CreateCoroutine()
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	lua_checkstack(m_pMainState, 1);
	return lua_newthread(m_pMainState);
	m_oMutex.Unlock();
}

///----------------------------------------------------------------------------

void CLuaEnvironment::KillCoroutine( lua_State * pKill )
{
	XASSERT(m_bInitialized);
	m_oMutex.Lock();
	for (int i = lua_gettop(m_pMainState); i > 0; i--)
	{
		if (lua_isthread(m_pMainState, i))
		{
			lua_State * pThread = lua_tothread(m_pMainState, i);
			if (pThread == pKill)
			{
				lua_remove(m_pMainState, i);
			}
		}
	}
	m_oMutex.Unlock();
}

///----------------------------------------------------------------------------

void * CLuaEnvironment::LuaAlloc( void * pLuaWorld, void * pOldPointer, size_t nOldSize, size_t nNewSize )
{
	CLuaEnvironment * pLuaEnv = (CLuaEnvironment *)pLuaWorld;
	void * pRetVal = nullptr;
	if (nNewSize == 0 && nOldSize > 0)
	{
		pLuaEnv->m_pAllocator->Free(pOldPointer);
	}
	else if (nOldSize == 0 && nNewSize > 0)
	{
		pRetVal = pLuaEnv->m_pAllocator->Allocate(nNewSize, 0, __FILE__, __LINE__);
	}
	else
	{
		pRetVal = pLuaEnv->m_pAllocator->Reallocate(pOldPointer, nNewSize, 0, __FILE__, __LINE__);
	}
	return pRetVal;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void CUpdatingLuaEnvironment::Init( CUpdater<CLuaEnvironment> * pUpdater, XEngine::XAllocator * pAllocator, CLuaEnvironmentSetupFunctor & rSetupFunctor )
{
	m_pUpdater = pUpdater;
	m_oUpdatable.Init(pAllocator, rSetupFunctor);
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void LuaHelper::LoadScript( lua_State * pState, XEngine::XResourceDatabase const * pResDB, char const * pName )
{
	void * pData = pResDB->FindObjectByNameAndType(XEngine::XChecksum_CalculateFromString(pName), CRC( 0xCC68A8D6, "LuaResource" ));
	size_t nDataSize = *((size_t *)pData);
	pData = (size_t *)pData + 1;
	int nErrorCode = luaL_loadbuffer (pState, (const char *)pData, nDataSize, pName);

}

///----------------------------------------------------------------------------