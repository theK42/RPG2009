#include "game/shared/gamepch.h"
#include "game/klua/klua.h"
extern "C" {
#include "game/lua/lua.h"
}

namespace KLua
{
	void * CheckUserData(lua_State * pState, int nUserDataIndex, const char * pRegisteredTypeName)
	{
		void * pUserData = lua_touserdata(pState, nUserDataIndex);
		if (pUserData != NULL) 
		{
			lua_getfield(pState, LUA_REGISTRYINDEX, pRegisteredTypeName);  ///Get the metatable for the type we're hoping for
			bool bMatch = false;
			bool bMetaFound = lua_getmetatable(pState, nUserDataIndex);
			while (!bMatch && bMetaFound) 
			{ 
				if (lua_rawequal(pState, -1, -2))  ///Is this the metatable we're looking for?
				{  
					lua_pop(pState, 2);  ///Restore the stack
					bMatch = true;
				}
				else
				{
					bMetaFound = lua_getmetatable(pState, -1) != 0; ///Get the next metatable on the chain
					lua_remove(pState, -2);	///Remove the metatable we already looked at from the stack
				}
			}
			if (!bMatch)
			{
				pUserData = nullptr;
			}
			if (!bMetaFound)
			{
				lua_pop(pState, 1);	///If we got here then there is a metatable on the stack we should get rid of
			}
		}
		return pUserData;
	}
}