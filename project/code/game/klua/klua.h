#pragma once

struct lua_State;

namespace KLua
{
	void * CheckUserData(lua_State * pState, int nUserDataIndex, const char * pRegisteredTypeName);
}
