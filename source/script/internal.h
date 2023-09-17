#pragma once

enum ConsoleColour
{
	COL_WHITE,
	COL_YELLOW,
	COL_RED
};

extern lua_State* lua;
extern HANDLE console;
extern char buffer[4096];

void SetConsoleColour(ConsoleColour colour);
void OutputString(const char* string);
int MetaIndex(lua_State* L);
int MetaNewIndex(lua_State* L);
int MetaCall(lua_State* L);
int ExceptionHandler(lua_State* L);
void FinalizeFunction(int status);
