#pragma once
#include "game/shared/practicallyuselessallocator.h"

struct lua_State;

extern "C"
{
__declspec ( dllexport ) int luaopen_resdblib(lua_State * pState);
};

namespace ResDBLib
{
	static CPracticallyUselessAllocator s_oAllocator;
	int CreateResDBBuilder(lua_State * pState);
	int DestroyResDBBuilder(lua_State * pState);

	int CreateSyncFile(lua_State * pState);
	int DestroySyncFile(lua_State * pState);

	namespace ResDBBuilder 
	{
		int AddObject(lua_State * pState);
		int Finalize(lua_State * pState);
	}

	namespace SyncFile
	{
		int Open(lua_State * pState);
		int Close(lua_State * pState);
	}
}