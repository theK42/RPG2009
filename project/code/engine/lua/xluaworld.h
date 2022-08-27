
#pragma once

#include "lua/lua.h"

namespace XEngine
{
	class XAllocator;

	class XLuaWorld
	{
	public:
		XLuaWorld();
		~XLuaWorld();

		void Init(XAllocator * pAllocator);
		void Deinit();

		void Update(float fTime);

		lua_State * CreateCoroutine();
		KillCoroutine(lua_State * pKill);

	private:
		static void * LuaAlloc(void * pLuaWorld, void * pOldPointer, size_t nOldSize, size_t nNewSize);

		XAllocator *	m_pAllocator;
		lua_State *		m_pMainState;

	};

}