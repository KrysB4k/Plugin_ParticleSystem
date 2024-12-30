#include "pch.h"
#include "script.h"
#include "logger.h"

namespace
{
	lua_State* lua;
	int metatable, parameters;

	void BuildFailureMessage(const char* msg)
	{
		luaL_where(lua, 1);
		lua_pushstring(lua, msg);
		lua_concat(lua, 2);
	}

	const char* StringRepresentation(int idx)
	{
		return lua_isstring(lua, idx) ? lua_tostring(lua, idx) : nullptr;
	}

	int MetaIndex(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				LuaBridge::GlobalIndex(StringRepresentation(2));
				return 1;
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			object->Index(StringRepresentation(2));
			return 1;
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int MetaNewIndex(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				LuaBridge::GlobalNewIndex(StringRepresentation(2));
				return 0;
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			object->NewIndex(StringRepresentation(2));
			return 0;
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int MetaCall(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				return LuaBridge::GlobalCall();
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			return object->Call();
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int ExceptionHandler(lua_State* L)
	{
		luaL_traceback(lua, lua, StringRepresentation(1), 1);
		return 1;
	}

	int ArgumentToStack(int argument)
	{
		if (argument > 0)
			return argument + 1;
		return argument;
	}

	int ErrorMetaIndex(lua_State* L)
	{
		BuildFailureMessage("module has no parameter with the given name");
		return lua_error(lua);
	}

	int ErrorMetaNewIndex(lua_State* L)
	{
		BuildFailureMessage("module has no parameter with the given name");
		return lua_error(lua);
	}

	const char* FindModule(const char* name, char* filename, int size)
	{
		strcpy_s(filename, size, name);
		_strlwr_s(filename, size);
		strcat_s(filename, size, ".lua");
		if (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES)
			return "t";
		strcat_s(filename, size, "c");
		if (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES)
			return "b";
		return nullptr;
	}
}

void Script::NewState()
{
	lua = luaL_newstate();
	lua_gc(lua, LUA_GCSTOP);
	lua_pushlightuserdata(lua, nullptr);
	lua_createtable(lua, 0, 3);
	lua_pushliteral(lua, "__index");
	lua_pushcfunction(lua, MetaIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__newindex");
	lua_pushcfunction(lua, MetaNewIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__call");
	lua_pushcfunction(lua, MetaCall);
	lua_rawset(lua, -3);
	lua_pushvalue(lua, -1);
	metatable = luaL_ref(lua, LUA_REGISTRYINDEX);
	lua_setmetatable(lua, -2);
	lua_pop(lua, 1);
	lua_createtable(lua, 0, 1);
	lua_pushliteral(lua, "__index");
	lua_pushcfunction(lua, ErrorMetaIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__newindex");
	lua_pushcfunction(lua, ErrorMetaNewIndex);
	lua_rawset(lua, -3);
	parameters = luaL_ref(lua, LUA_REGISTRYINDEX);
}

void Script::Close()
{
	luaL_unref(lua, LUA_REGISTRYINDEX, metatable);
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

void Script::PushData(LuaObject* value)
{
	lua_pushlightuserdata(lua, value);
}

void Script::PushString(const char* string)
{
	lua_pushstring(lua, string);
}

int Script::ToInteger(int argument)
{
	return lroundf(lua_tonumber(lua, ArgumentToStack(argument)));
}

bool Script::ToBoolean(int argument)
{
	return lua_toboolean(lua, ArgumentToStack(argument));
}

float Script::ToNumber(int argument)
{
	return lua_tonumber(lua, ArgumentToStack(argument));
}

LuaObject* Script::ToData(int argument)
{
	return (LuaObject*)lua_touserdata(lua, ArgumentToStack(argument));
}

const char* Script::ToString(int argument)
{
	return lua_tostring(lua, ArgumentToStack(argument));
}

int Script::ArgCount()
{
	return (lua_gettop(lua) - 1);
}

bool Script::IsInteger(int argument)
{
	return lua_isnumber(lua, ArgumentToStack(argument));
}

bool Script::IsBoolean(int argument)
{
	return lua_isboolean(lua, ArgumentToStack(argument));
}

bool Script::IsNumber(int argument)
{
	return lua_isnumber(lua, ArgumentToStack(argument));
}

bool Script::IsData(int argument)
{
	return lua_isuserdata(lua, ArgumentToStack(argument));
}

bool Script::IsString(int argument)
{
	return lua_isstring(lua, ArgumentToStack(argument));
}

int Script::StoreFunction(int argument)
{
	int stack;

	stack = ArgumentToStack(argument);
	if (lua_isnil(lua, stack))
		return LUA_REFNIL;
	lua_pushvalue(lua, stack);
	return luaL_ref(lua, LUA_REGISTRYINDEX);
}

bool Script::ExecuteFunction(int reference, void* value)
{
	int status;

	if (reference != LUA_REFNIL)
	{
		lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
		if (value)
		{
			lua_pushlightuserdata(lua, value);
			status = lua_pcall(lua, 1, 0, -3);
		}
		else
		{
			status = lua_pcall(lua, 0, 0, -2);
		}
		if (status != LUA_OK)
		{
			Logger::Fatal(lua_tostring(lua, -1));
			lua_pop(lua, 1);
			return false;
		}
	}
	return true;
}

bool Script::IsFunction(int argument)
{
	int stack;

	stack = ArgumentToStack(argument);
	return lua_isnil(lua, stack) || lua_isfunction(lua, stack);
}

void Script::DeleteFunction(int* reference)
{
	luaL_unref(lua, LUA_REGISTRYINDEX, *reference);
	*reference = LUA_REFNIL;
}

void Script::PushTable(int argument, int length)
{
	int stack;

	stack = ArgumentToStack(argument);
	lua_createtable(lua, length, 0);
	for (int i = 0; i < length; i++)
	{
		lua_pushvalue(lua, stack + i);
		lua_rawseti(lua, -2, i + 1);
	}
}

int Script::ExplodeTable(int argument)
{
	int length, stack;

	stack = ArgumentToStack(argument);
	length = lua_rawlen(lua, stack);
	for (int i = 0; i < length; i++)
		lua_rawgeti(lua, stack, i + 1);
	return length;
}

bool Script::IsTable(int argument)
{
	return lua_istable(lua, ArgumentToStack(argument));
}

void Script::Print()
{
	int top;

	top = lua_gettop(lua);
	for (int i = 2; i <= top; i++)
	{
		if (i > 2)
		{
			lua_pushliteral(lua, "\t");
			lua_concat(lua, 2);
		}
		luaL_tolstring(lua, i, nullptr);
	}
	lua_concat(lua, top - 1);
	Logger::Debug(lua_tostring(lua, -1));
	lua_pop(lua, 1);
}

bool Script::Require(const char* base)
{
	char name[100], filename[100];
	const char* mode;
	int status;

	strcpy_s(name, "effects\\");
	strcat_s(name, base);
	mode = FindModule(name, filename, 100);
	if (!mode)
		return false;
	status = luaL_loadfilex(lua, filename, mode);
	if (status == LUA_OK)
	{
		lua_pushlightuserdata(lua, nullptr);
		lua_setupvalue(lua, -2, 1);
		status = lua_pcall(lua, 0, 1, -2);
		if (status != LUA_OK)
		{
			Logger::Fatal(lua_tostring(lua, -1));
			lua_pop(lua, 1);
			lua_pushnil(lua);
		}
	}
	else
	{
		EmitFailure(lua_tostring(lua, -1), Logger::Error);
		lua_pop(lua, 1);
		lua_pushnil(lua);
	}
	if (lua_istable(lua, -1))
	{
		lua_rawgeti(lua, LUA_REGISTRYINDEX, parameters);
		lua_setmetatable(lua, -2);
	}
	else if (!lua_isnil(lua, -1))
	{
		lua_pop(lua, 1);
		lua_pushnil(lua);
	}
	return true;
}

void Script::LoadFunctions(const char* name)
{
	int status;
	char filename[100];
	const char* mode;

	mode = FindModule(name, filename, 100);
	if (!mode)
		return;
	status = luaL_loadfilex(lua, filename, mode);
	if (status == LUA_OK)
	{
		lua_pushlightuserdata(lua, nullptr);
		lua_setupvalue(lua, -2, 1);
		status = lua_pcall(lua, 0, 0, -2);
		if (status != LUA_OK)
		{
			Logger::Fatal(lua_tostring(lua, -1));
			lua_pop(lua, 1);
		}
	}
	else
	{
		EmitFailure(lua_tostring(lua, -1), Logger::Fatal);
		lua_pop(lua, 1);
	}
}

[[noreturn]] void Script::Throw(const char* msg)
{
	BuildFailureMessage(msg);
	throw std::exception();
}

void Script::EmitFailure(const char* msg, void (*log)(const char*))
{
	BuildFailureMessage(msg);
	luaL_traceback(lua, lua, lua_tostring(lua, -1), 1);
	log(lua_tostring(lua, -1));
	lua_pop(lua, 2);
}

void Script::AddInformation(const char* msg)
{
	Logger::Information(msg);
}

int Script::GarbageCount()
{
	return lua_gc(lua, LUA_GCCOUNT);
}

void Script::PreFunctionLoop()
{
	lua_pushcfunction(lua, ExceptionHandler);
}

void Script::PostFunctionLoop(int results)
{
	lua_remove(lua, -results - 1);
	lua_gc(lua, LUA_GCCOLLECT);
}

void* Script::CreateManagedData(unsigned int size)
{
	void* object;

	object = lua_newuserdatauv(lua, size, 0);
	lua_rawgeti(lua, LUA_REGISTRYINDEX, metatable);
	lua_setmetatable(lua, -2);
	return object;
}

void* Script::GetExtraSpace()
{
	return lua_getextraspace(lua);
}
