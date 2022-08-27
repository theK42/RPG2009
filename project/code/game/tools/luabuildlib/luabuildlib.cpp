#include "game/shared/gamepch.h"
#include "game/tools/luabuildlib/luabuildlib.h"

#include "game/klua/klua.h"

#include "engine/resource/xfileio.h"

extern "C" {
#include "game/lua/lua.h"
#include "game/lua/lauxlib.h"
}


///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

int luaopen_luabuildlib(lua_State * pState)
{
	const struct luaL_Reg pLibraryFunctions[] = 
	{
		{"CreateLuaTextResourceObject", LuaBuildLib::CreateLuaTextResourceObject},
		{nullptr, nullptr}
	};

	///This library requires the resdblib, so we need to call the "require" function with resdblib as the parameter
	lua_getglobal(pState, "require"); ///Get the "require" function 
	lua_pushstring(pState, "resdblib"); ///Push name of library ("resdblib")
	int nError = lua_pcall(pState, 1, 0, 0); ///Call the function, expecting no return value, no lua error function.

	///Theoretically that could have failed.
	if (nError != 0)
	{
		lua_error(pState);
	}

	luaL_newmetatable(pState, "LuaBuildLib.LuaTextResourceObject"); ///Create metatable for LuaTextResourceObject objects, puts it in the registry.  Pushes metatable on stack.
	lua_pushvalue(pState, -1); ///duplicate the metatable on the stack
	lua_setfield(pState, -2, "__index"); ///set the __index on the metatable to be itself (methods will live in the metatable).  Pops duplicate metatable off stack.
	luaL_getmetatable(pState, "ResDBLib.ResDBBuilderObject"); ///Get the base ResDBBuilderObject metatable onto the stack
	lua_setmetatable(pState, -2); ///Sets the metatable of the LuaTextResourceObject to be the ResDBBuilderObject metatable, so we can tell it inherits from this userdata type.  Pops the ResDBBuilderObject metatable off the stack.
	luaL_register(pState, "LuaBuildLib", pLibraryFunctions); ///register library functions (pushes new table for the library).  Pushes table on stack.
	return 1;	///Returns one value (the LuaBuildLib Library itself).
}

namespace LuaBuildLib
{
	///----------------------------------------------------------------------------

	int CreateLuaTextResourceObject(lua_State * pState)
	{
		if (!lua_isstring(pState, 1))
		{
			luaL_error(pState, "CreateTextResourceObject requires a filename");
		}
		char const * pFileName = lua_tostring(pState, 1);
		CLuaTextResourceObject ** ppLuaTextResourceObject  = (CLuaTextResourceObject **)lua_newuserdata(pState, sizeof(CLuaTextResourceObject *)); ///Create a new user data, sized for a pointer
		(*ppLuaTextResourceObject) = XNEW(&s_oAllocator) CLuaTextResourceObject; ///Set the pointer to a newly created ResourceDatabaseBuilder
		(*ppLuaTextResourceObject)->Init(pFileName);
		luaL_getmetatable(pState, "LuaBuildLib.LuaTextResourceObject"); ///Get the ResDBBuilder metatable onto the stack
		lua_setmetatable(pState, -2); ///Set the userdata's metatable to be the ResDBBuilder metatable. Pops the metatable off the stack.
		return 1;
	}

	///----------------------------------------------------------------------------

	int DestroyLuaTextResourceObject(lua_State * pState)
	{
		CLuaTextResourceObject ** ppLuaTextResourceObject = (CLuaTextResourceObject **)KLua::CheckUserData(pState, 1, "LuaBuildLib.LuaTextResourceObject");
		if (ppLuaTextResourceObject != nullptr)
		{
			(*ppLuaTextResourceObject)->Deinit();
			XDELETE(*ppLuaTextResourceObject);
			*ppLuaTextResourceObject = nullptr;
		}
		else
		{
			luaL_error(pState, "Expected LuaTextResourceObject userdata");
		}
		return 0;
	}

	///----------------------------------------------------------------------------
	///----------------------------------------------------------------------------

	CLuaTextResourceObject::CLuaTextResourceObject()
	{
		m_pBuff = nullptr;
		m_nLength = nullptr;
	}

	///----------------------------------------------------------------------------

	CLuaTextResourceObject::~CLuaTextResourceObject()
	{
		Deinit();
	}

	///----------------------------------------------------------------------------

	void CLuaTextResourceObject::Init( char const * pFileName )
	{
		XEngine::XSyncFile oFile;
		oFile.Open(pFileName, XEngine::XFileFlags::Read);
		m_nLength = oFile.GetSize();
		m_pBuff = XNEW(&s_oAllocator) char[m_nLength];
		oFile.Read(m_nLength, m_pBuff);
	}

	///----------------------------------------------------------------------------

	void CLuaTextResourceObject::Deinit()
	{
		if (m_pBuff != nullptr)
		{
			XDELETE_ARRAY(m_pBuff);
			m_pBuff = nullptr;
			m_nLength = 0;
		}
	}

	///----------------------------------------------------------------------------

	u32 CLuaTextResourceObject::GetType()
	{
		return CRC( 0x5B87D519, "LuaText" );
	}

	///----------------------------------------------------------------------------

	void CLuaTextResourceObject::Write( XEngine::XTools::XResourceDatabaseBuilder & rBuilder, XEngine::XTools::XResourceFileWriter & rWriter, handle hLink )
	{
		rWriter.ResolveLink(hLink);
		rWriter.WriteLink(m_pBuff);
		rWriter.WriteInt32(m_nLength);
		rWriter.ResolveLink(m_pBuff);
		rWriter.WriteBytes(m_nLength, m_pBuff);
	}

	///----------------------------------------------------------------------------
}