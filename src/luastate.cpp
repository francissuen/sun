#include "luastate.h"
extern "C"
{
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}
#include <cassert>
#include "logger.h"
#include "string.h"
using namespace gb::utils;

luastate::luastate()
{
    _l = luaL_newstate();
    assert(_l != nullptr);
    luaL_openlibs(_l);
}

bool luastate::dofile(const char* szLuaFile)
{
    lua_pushcfunction(_l, _debug_traceback);
    if(luaL_loadfile(_l, szLuaFile) !=0 )
    {
	logger::Instance().error(string("luastate::dofile luaL_loadfile err@ ") + szLuaFile);
	return false;
    }
    return lua_pcall(_l, 0, 0, -2) == 0 ? true : false;
}

bool luastate::dostring(const char* szLua)
{
    lua_pushcfunction(_l, _debug_traceback);
    if(luaL_loadstring(_l, szLua) !=0 )
    {
	logger::Instance().error(string("luastate::dostring luaL_loadstring@ ") + szLua);
	return false;
    }
    return lua_pcall(_l, 0, 0, -2) == 0 ? true : false;
}

int luastate::_debug_traceback(lua_State* l)
{
    lua_getfield(l, LUA_GLOBALSINDEX, "debug");
    lua_getfield(l, -1, "traceback");
    lua_pushvalue(l, 1);
    lua_call(l, 1, 1);
    logger::Instance().error(string("_debug_traceback@ ") + lua_tostring(l, -1));
    return 1;
}

void luastate::append_packagepath(const char* path)
{
    lua_getglobal(_l, "package");
    lua_getfield(_l, -1, "path");
    string cur_path(lua_tostring(_l, -1));
    cur_path += ";";
    cur_path += path;
    cur_path += "/?.lua";
    lua_pop(_l, 1);
    lua_pushstring(_l, cur_path);
    lua_setfield(_l, -2, "path");
    lua_pop(_l, 1);
}

lua_State* luastate::getstate()
{
    return _l;
}

void luastate_mgr::initialize(const unsigned char threadCount)
{
    _threadCount = threadCount;
    
    _config_l = new luastate[threadCount];
    _logic_l = new luastate[threadCount];
    
}

luastate& luastate_mgr::getconfig_state(const unsigned char threadIdx)
{
    assert(threadIdx > 0 && threadIdx < _threadCount);
    
    return _config_l[threadIdx];
}

luastate& luastate_mgr::getlogic_state(const unsigned char threadIdx)
{
    assert(threadIdx > 0 && threadIdx < _threadCount);

    return _logic_l[threadIdx];
}



