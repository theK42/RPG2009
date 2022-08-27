#include "game/shared/gamepch.h"
#include "game/tools/resdblib/resdblib.h"

#include "game/klua/klua.h"

extern "C" {
#include "game/lua/lua.h"
#include "game/lua/lauxlib.h"
}

#include "engine/tools/resource/xresdbbuilder.h"

#include "engine/resource/xfileio.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

int luaopen_resdblib(lua_State * pState)
{
	const struct luaL_Reg pLibraryFunctions[] = 
	{
		{"CreateResDBBuilder", ResDBLib::CreateResDBBuilder},
		{"CreateSyncFile", ResDBLib::CreateSyncFile},
		{nullptr, nullptr}
	};

	const struct luaL_Reg pResDBBuilderFunctions[] =
	{
		{"AddObject", ResDBLib::ResDBBuilder::AddObject},
		{"Finalize", ResDBLib::ResDBBuilder::Finalize},
		{"__gc", ResDBLib::DestroyResDBBuilder},
		{nullptr, nullptr}
	};

	const struct luaL_Reg pSyncFileFunctions[] = 
	{
		{"Open", ResDBLib::SyncFile::Open},
		{"__gc", ResDBLib::DestroySyncFile},
		{nullptr, nullptr}
	};

	luaL_newmetatable(pState, "ResDBLib.ResDBBuilder"); ///Create metatable for ResDBBuilder objects, puts it in the registry.  Pushes metatable on stack.
	lua_pushvalue(pState, -1); ///duplicate the metatable on the stack
	lua_setfield(pState, -2, "__index"); ///set the __index on the metatable to be itself (methods will live in the metatable).  Pops duplicate metatable off stack.
	luaL_register(pState, nullptr, pResDBBuilderFunctions); ///register the methods on the metatable (does not alter the stack)
	luaL_newmetatable(pState, "ResDBLib.ResDBBuilderObject"); ///Create a metatable for ResDBBuilderObject objects, puts it in the register.  Pushes metatable on stack.
	luaL_newmetatable(pState, "ResDBLib.SeekIO"); ///Create a metatable for SeekIO objects, puts it in the registry.  Pushes metatable on stack.
	luaL_newmetatable(pState, "ResDBLib.SyncFile"); /// Create a metatable for SyncFile objects, puts it in the registry.  Pushes metatable on stack.
	luaL_getmetatable(pState, "ResDBLib.SeekIO"); ///Get the SeekIO metatable on top of the stack
	lua_setmetatable(pState, -2); ///Set the SeekIO metatable as the metatable for SyncFile (for inheritance).  Pops the SeekIO metatable off the top of the stack
	lua_pushvalue(pState, -1); ///duplicate the metatable on the stack
	lua_setfield(pState, -2, "__index"); ///set the __index on the metatable to be itself (methods will live in the metatable).  Pops duplicate metatable off stack.
	luaL_register(pState, nullptr, pSyncFileFunctions); ///register the methods on the metatable (does not alter the stack)
	luaL_register(pState, "ResDBLib", pLibraryFunctions); ///register library functions (pushes new table for the library).  Pushes table on stack.
	return 1;	///Returns one value (the ResDBLib Library itself).
}

///----------------------------------------------------------------------------

namespace ResDBLib
{
	///----------------------------------------------------------------------------

	int CreateResDBBuilder(lua_State * pState)
	{
		XEngine::XTools::XResourceDatabaseBuilder ** ppResDBBuilder  = (XEngine::XTools::XResourceDatabaseBuilder **)lua_newuserdata(pState, sizeof(XEngine::XTools::XResourceDatabaseBuilder *)); ///Create a new user data, sized for a pointer
		(*ppResDBBuilder) = XNEW(&s_oAllocator) XEngine::XTools::XResourceDatabaseBuilder; ///Set the pointer to a newly created ResourceDatabaseBuilder
		luaL_getmetatable(pState, "ResDBLib.ResDBBuilder"); ///Get the ResDBBuilder metatable onto the stack
		lua_setmetatable(pState, -2); ///Set the userdata's metatable to be the ResDBBuilder metatable. Pops the metatable off the stack.
		return 1;
	}

	///----------------------------------------------------------------------------

	int DestroyResDBBuilder(lua_State * pState)
	{
		XEngine::XTools::XResourceDatabaseBuilder ** ppResDBBuild = (XEngine::XTools::XResourceDatabaseBuilder **)KLua::CheckUserData(pState, 1, "ResDBLib.ResDBBuilder");
		if (ppResDBBuild != nullptr)
		{
			XDELETE(*ppResDBBuild);
			*ppResDBBuild = nullptr;
		}
		else
		{
			luaL_error(pState, "Expected ResDBBuilder userdata");
		}
		return 0;
	}
	///----------------------------------------------------------------------------

	int CreateSyncFile(lua_State * pState)
	{
		XEngine::XSyncFile ** ppSyncFile  = (XEngine::XSyncFile **)lua_newuserdata(pState, sizeof(XEngine::XSyncFile *)); ///Create a new user data, sized for a pointer
		(*ppSyncFile) = XNEW(&s_oAllocator) XEngine::XSyncFile; ///Set the pointer to a newly created SyncFile
		luaL_getmetatable(pState, "ResDBLib.SyncFile"); ///Get the SyncFile metatable onto the stack
		lua_setmetatable(pState, -2); ///Set the userdata's metatable to be the SyncFile metatable. Pops the metatable off the stack.
		return 1;
	}

	///----------------------------------------------------------------------------

	int DestroySyncFile(lua_State * pState)
	{
		XEngine::XSyncFile ** ppSyncFile = (XEngine::XSyncFile **)KLua::CheckUserData(pState, 1, "ResDBLib.SyncFile");
		if (ppSyncFile != nullptr)
		{
			XDELETE(*ppSyncFile);
			*ppSyncFile = nullptr;
		}
		else
		{
			luaL_error(pState, "Expected SyncFile userdata");
		}
		return 0;
	}
	namespace ResDBBuilder
	{
		///----------------------------------------------------------------------------

		int AddObject( lua_State * pState )
		{
			XEngine::XTools::XResourceDatabaseBuilder ** ppResDBBuild = (XEngine::XTools::XResourceDatabaseBuilder **)KLua::CheckUserData(pState, 1, "ResDBLib.ResDBBuilder");
			if (ppResDBBuild == nullptr)
			{
				luaL_error(pState, "Expected ResDBBuilder userdata");
			}

			XEngine::XTools::XResourceDatabaseBuilderObject ** ppResDBBuilderObject = (XEngine::XTools::XResourceDatabaseBuilderObject **)KLua::CheckUserData(pState, 2, "ResDBLib.ResDBBuilderObject");
			if (ppResDBBuilderObject == nullptr)
			{
				luaL_error(pState, "Expected ResDBBuilderObject userdata");
			}
			(*ppResDBBuild)->AddObject(*ppResDBBuilderObject);
			return 0;
		}

		///----------------------------------------------------------------------------

		int Finalize( lua_State * pState )
		{
			XEngine::XTools::XResourceDatabaseBuilder ** ppResDBBuild = (XEngine::XTools::XResourceDatabaseBuilder **)KLua::CheckUserData(pState, 1, "ResDBLib.ResDBBuilder");
			if (ppResDBBuild == nullptr)
			{
				luaL_error(pState, "Expected ResDBBuilder userdata");
			}

			XEngine::XSeekIO ** ppSeekIO = (XEngine::XSeekIO **)KLua::CheckUserData(pState, 2, "ResDBLib.SeekIO");
			if (ppSeekIO == nullptr)
			{
				luaL_error(pState, "Expected SeekIO userdata");
			}

			bool bWriteStringTable = false;
			if (lua_isboolean(pState, 3))
			{
				bWriteStringTable = (bool)lua_toboolean(pState, 3);
			}

			(*ppResDBBuild)->Finalize((**ppSeekIO), bWriteStringTable);
			return 0;
		}
	}

	namespace SyncFile
	{
		int Open( lua_State * pState )
		{
			XEngine::XSyncFile ** ppSyncFile = (XEngine::XSyncFile **)KLua::CheckUserData(pState, 1, "ResDBLib.SyncFile");
			if (ppSyncFile == nullptr)
			{
				luaL_error(pState, "Expected SyncFile userdata");
			}
			if (!lua_isstring(pState, 2))
			{
				luaL_error(pState, "Expected filename");
			}
			char const * pFileName = lua_tostring(pState, 2);
			(*ppSyncFile)->Open(pFileName, XEngine::XFileFlags::ReadWrite);
			return 0;
		}

		int Close( lua_State * pState )
		{
			XEngine::XSyncFile ** ppSyncFile = (XEngine::XSyncFile **)KLua::CheckUserData(pState, 1, "ResDBLib.SyncFile");
			if (ppSyncFile == nullptr)
			{
				luaL_error(pState, "Expected SyncFile userdata");
			}
			(*ppSyncFile)->Close();
			return 0;
		}

	}

	///----------------------------------------------------------------------------
}