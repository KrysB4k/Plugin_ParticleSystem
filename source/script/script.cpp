#include "pch.h"
#include "script.h"
#include "logger.h"

namespace
{
	lua_State* lua;

	void BuildErrorMessage(const char* msg)
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
}

void Script::NewState()
{
	lua = luaL_newstate();
	Logger::Create();
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
	lua_setmetatable(lua, -2);
	lua_pop(lua, 1);
}

void Script::Close()
{
	Logger::Close();
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

int Script::ToInteger(int argument)
{
	if (argument > 0)
		argument++;
	return (int)roundf(lua_tonumber(lua, argument));
}

bool Script::ToBoolean(int argument)
{
	if (argument > 0)
		argument++;
	return lua_toboolean(lua, argument);
}

float Script::ToNumber(int argument)
{
	if (argument > 0)
		argument++;
	return lua_tonumber(lua, argument);
}

LuaObject* Script::ToData(int argument)
{
	if (argument > 0)
		argument++;
	return (LuaObject*)lua_touserdata(lua, argument);
}

int Script::ArgCount()
{
	return (lua_gettop(lua) - 1);
}

bool Script::IsInteger(int argument)
{
	if (argument > 0)
		argument++;
	return lua_isnumber(lua, argument);
}

bool Script::IsBoolean(int argument)
{
	if (argument > 0)
		argument++;
	return lua_isboolean(lua, argument);
}

bool Script::IsNumber(int argument)
{
	if (argument > 0)
		argument++;
	return lua_isnumber(lua, argument);
}

bool Script::IsData(int argument)
{
	if (argument > 0)
		argument++;
	return lua_isuserdata(lua, argument);
}

int Script::StoreFunction(int argument)
{
	if (argument > 0)
		argument++;
	if (lua_isnil(lua, argument))
		return LUA_REFNIL;
	lua_pushvalue(lua, argument);
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
			Logger::Error(lua_tostring(lua, -1));
			lua_pop(lua, 1);
			return false;
		}
	}
	return true;
}

bool Script::IsFunction(int argument)
{
	if (argument > 0)
		argument++;
	return lua_isnil(lua, argument) || lua_isfunction(lua, argument);
}

void Script::DeleteFunction(int* reference)
{
	luaL_unref(lua, LUA_REGISTRYINDEX, *reference);
	*reference = LUA_REFNIL;
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
	if (status != LUA_OK)
	{
		Logger::Error(lua_tostring(lua, -1));
		lua_pop(lua, 1);
	}
}

[[noreturn]] void Script::ThrowError(const char* msg)
{
	BuildErrorMessage(msg);
	throw std::exception();
}

void Script::EmitWarning(const char* msg)
{
	BuildErrorMessage(msg);
	luaL_traceback(lua, lua, lua_tostring(lua, -1), 1);
	Logger::Warning(lua_tostring(lua, -1));
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

void Script::PostFunctionLoop()
{
	lua_pop(lua, 1);
	lua_gc(lua, LUA_GCCOLLECT);
}
