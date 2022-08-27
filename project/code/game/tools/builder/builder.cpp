#include "game/shared/gamepch.h"
#include "game/tools/builder/builder.h"
#include <stdio.h>

extern "C" {
#include "game/lua/lua.h"
#include "game/lua/lauxlib.h"
#include "game/lua/lualib.h"
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CBuilder::CBuilder()
{
	m_bInitialized = false;
	m_bFinished = false;
}

///----------------------------------------------------------------------------

CBuilder::~CBuilder()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CBuilder::Init()
{
	XASSERT(!m_bInitialized);
	m_pState = luaL_newstate();
	luaL_openlibs(m_pState);

	lua_createtable(m_pState, 0, 2); ///Create a table to represent the builder, pushed on the stack
	lua_pushlightuserdata(m_pState, this); ///Push the pointer to this on the stack
	lua_setfield(m_pState, -2, "this"); ///Add it to the table with the name "this", pop it from the stack
	lua_pushcfunction(m_pState, &CBuilder::LuaStop); ///Push Stop function on the stack
	lua_setfield(m_pState, -2, "Stop"); ///Add it to the table, name it stop
	lua_setglobal(m_pState, "Builder"); ///Give the table the global name Builder and pop it from the stack
	m_bInitialized = true;
	m_bFinished = false;
}

///----------------------------------------------------------------------------

void CBuilder::Deinit()
{
	if (m_bInitialized)
	{
		lua_close(m_pState);
	}
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

void CBuilder::RunLine( char const * pLine )
{
	XASSERT(m_bInitialized);
	XASSERT(!m_bFinished);
	int nError = luaL_loadbuffer(m_pState, pLine, strlen(pLine), "line");
	HandleError(nError);
	if (nError == 0)
	{
		nError = lua_pcall(m_pState, 0, 0, 0);
		HandleError(nError);
	}
}
///----------------------------------------------------------------------------

void CBuilder::HandleError(int nError)
{
	XASSERT(m_bInitialized);
	if (nError != 0)
	{
		fprintf(stderr, "%s", lua_tostring(m_pState, -1));
		lua_pop(m_pState, 1);
		Stop();
	}
}

///----------------------------------------------------------------------------

bool CBuilder::IsFinished() const
{
	return m_bFinished;
}

///----------------------------------------------------------------------------

void CBuilder::Stop()
{
	XASSERT(m_bInitialized);
	m_bFinished = true;
}

///----------------------------------------------------------------------------

int CBuilder::LuaStop( lua_State * pState )
{
	XASSERT(lua_istable(pState, -1));  ///Actually need to properly error handle this, as this might happen in a script by accident
	lua_getfield(pState, -1, "this"); ///Get the this pointer from the table onto the stack
	XASSERT(lua_islightuserdata(pState, -1));  ///Need to error handle this too 
	CBuilder * pThis = (CBuilder *)lua_touserdata(pState, -1);
	pThis->Stop();
	return 0;
}

///----------------------------------------------------------------------------

int main()
{
	static const size_t sc_nBufferSize = 256;
	char pBuff[sc_nBufferSize];
	int nError = 0;
	CBuilder oBuilder;
	oBuilder.Init();
	while (fgets(pBuff, sc_nBufferSize, stdin) != nullptr && !oBuilder.IsFinished())
	{
		oBuilder.RunLine(pBuff);
	}
	oBuilder.Deinit();
	return 0;
}
