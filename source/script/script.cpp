#include "pch.h"
#include "script.h"
#include "internal.h"

void Script::NewState()
{
	lua = luaL_newstate();
	lua_gc(lua, LUA_GCSTOP);
	AllocConsole();
	console = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	if (console != INVALID_HANDLE_VALUE)
		SetConsoleActiveScreenBuffer(console);
	SetConsoleColour(COL_WHITE);
	lua_pushliteral(lua, "");
	lua_createtable(lua, 0, 1);
	lua_pushliteral(lua, "__call");
	lua_pushcfunction(lua, MetaCall);
	lua_rawset(lua, -3);
	lua_setmetatable(lua, -2);
	lua_pushlightuserdata(lua, nullptr);
	lua_createtable(lua, 0, 2);
	lua_pushliteral(lua, "__index");
	lua_pushcfunction(lua, MetaIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__newindex");
	lua_pushcfunction(lua, MetaNewIndex);
	lua_rawset(lua, -3);
	lua_setmetatable(lua, -2);
	lua_pop(lua, 2);
}

void Script::Close()
{
	if (console != INVALID_HANDLE_VALUE)
		CloseHandle(console);
	FreeConsole();
	lua_close(lua);
}

void Script::PushInteger(int value)
{
	lua_pushinteger(lua, value);
}

void Script::PushBoolean(bool value)
{
	lua_pushboolean(lua, value);
}

void Script::PushNumber(float value)
{
	lua_pushnumber(lua, value);
}

void Script::PushData(void* value)
{
	lua_pushlightuserdata(lua, value);
}

int Script::ToInteger(int argument)
{
	return (int)roundf(luaL_checknumber(lua, argument + 1));
}

bool Script::ToBoolean(int argument)
{
	luaL_checktype(lua, argument + 1, LUA_TBOOLEAN);
	return lua_toboolean(lua, argument + 1);
}

float Script::ToNumber(int argument)
{
	return luaL_checknumber(lua, argument + 1);
}

void* Script::ToData(int argument)
{
	return lua_touserdata(lua, argument + 1);
}

void Script::ExecuteFunction(int reference, void* value)
{
	if (reference != LUA_REFNIL)
	{
		lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
		if (value)
			lua_pushlightuserdata(lua, value);
		FinalizeFunction(value ? lua_pcall(lua, 1, 0, -3) : lua_pcall(lua, 0, 0, -2));
	}
}

int Script::StoreFunction(int argument)
{
	if (lua_isnoneornil(lua, argument + 1))
		return LUA_REFNIL;
	luaL_checktype(lua, argument + 1, LUA_TFUNCTION);
	lua_pushvalue(lua, argument + 1);
	return luaL_ref(lua, LUA_REGISTRYINDEX);
}

void Script::Print()
{
	int top;
	const char* string;

	top = lua_gettop(lua);
	for (int i = 2; i <= top; i++)
	{
		if (i > 2)
			OutputString("\t");
		string = lua_tostring(lua, i);
		if (!string)
			string = FormatString("%s: %p", luaL_typename(lua, i), lua_topointer(lua, i));
		OutputString(string);
	}
	OutputString("\n");
}

void Script::LoadFunctions(const char* filename)
{
	int status;

	status = luaL_loadfile(lua, filename);
	if (status == LUA_OK)
	{
		lua_pushlightuserdata(lua, nullptr);
		lua_setupvalue(lua, -2, 1);
		status = lua_pcall(lua, 0, 0, -2);
	}
	FinalizeFunction(status);
}

int Script::TypeError(int argument, const char* type)
{
	return luaL_typeerror(lua, argument + 1, type);
}

const char* Script::FormatString(const char* format, ...)
{
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, 4096, format, args);
	va_end(args);
	return buffer;
}

int Script::GarbageCount()
{
	return lua_gc(lua, LUA_GCCOUNT);
}

void Script::PreFunctionLoop()
{
	lua_pushcfunction(lua, ExceptionHandler);
}

void Script::PostFunctionLoop()
{
	lua_pop(lua, 1);
	lua_gc(lua, LUA_GCCOLLECT);
}
