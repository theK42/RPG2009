#pragma once
#include "game/shared/practicallyuselessallocator.h"
#include "engine/tools/resource/xresdbbuilder.h"

struct lua_State;

extern "C"
{
	__declspec ( dllexport ) int luaopen_luabuildlib(lua_State * pState);
};

namespace LuaBuildLib
{
	static CPracticallyUselessAllocator s_oAllocator;
	int CreateLuaTextResourceObject(lua_State * pState);
	int DestroyLuaTextResourceObject(lua_State * pState);

	class CLuaTextResourceObject : public XEngine::XTools::XResourceDatabaseBuilderNamedObject
	{
	public:
		CLuaTextResourceObject();
		~CLuaTextResourceObject();
		void Init(char const * pFileName);
		void Deinit();
		virtual u32 GetType();
		virtual void Write( XEngine::XTools::XResourceDatabaseBuilder & rBuilder, XEngine::XTools::XResourceFileWriter & rWriter, handle hLink );
	private:
		char *	m_pBuff;
		int		m_nLength;

	};
}