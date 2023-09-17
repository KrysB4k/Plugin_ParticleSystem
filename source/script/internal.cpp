#include "pch.h"
#include "internal.h"
#include "script.h"

lua_State* lua;
HANDLE console;
char buffer[4096];

void SetConsoleColour(ConsoleColour colour)
{
	if (console != INVALID_HANDLE_VALUE)
	{
		switch (colour)
		{
		case COL_WHITE:
			SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case COL_YELLOW:
			SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case COL_RED:
			SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		}
	}
}

void OutputString(const char* string)
{
	DWORD bytes;

	if (console != INVALID_HANDLE_VALUE)
		WriteConsole(console, string, strlen(string), &bytes, nullptr);
}

int MetaIndex(lua_State* L)
{
	LuaObject* object;

	object = (LuaObject*)lua_touserdata(lua, 1);
	if (!object)
		return lua_isstring(lua, 2);
	if (lua_isstring(lua, 2))
	{
		lua_remove(lua, 1);
		return object->Index(lua_tostring(lua, 1));
	}
	return 0;
}

int MetaNewIndex(lua_State* L)
{
	LuaObject* object;

	object = (LuaObject*)lua_touserdata(lua, 1);
	if (object && lua_isstring(lua, 2))
	{
		lua_remove(lua, 1);
		object->NewIndex(lua_tostring(lua, 1));
	}
	return 0;
}

int MetaCall(lua_State* L)
{
	return LuaBridge::Call(lua_tostring(lua, 1));
}

int ExceptionHandler(lua_State* L)
{
	luaL_traceback(lua, lua, lua_isstring(lua, 1) ? lua_tostring(lua, 1) : nullptr, 1);
	return 1;
}

void FinalizeFunction(int status)
{
	if (status != LUA_OK)
	{
		SetConsoleColour(COL_RED);
		OutputString(lua_tostring(lua, -1));
		OutputString("\n");
		SetConsoleColour(COL_WHITE);
		lua_pop(lua, 1);
	}
}
