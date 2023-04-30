#include "pch.h"
#include "internal.h"
#include "script.h"

void script::newstate()
{
	L = luaL_newstate();
	luaL_openlibs(L);
}

void script::close()
{
	lua_close(L);
}
