#pragma once

struct lua_State;

class CBuilder
{
public:
	CBuilder();
	~CBuilder();
	void Init();
	void Deinit();
	void RunLine(char const * pLine);
	bool IsFinished() const;
private:
	void Stop();
	void HandleError(int nError);
	static int LuaStop(lua_State * pState);
	bool		m_bInitialized;
	bool		m_bFinished;
	lua_State *	m_pState;
};