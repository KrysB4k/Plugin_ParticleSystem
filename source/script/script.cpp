#include "pch.h"
#include "script.h"
#include "logger.h"

namespace
{
	lua_State* lua;
	int metatable;

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

	int MetaAdd(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				return LuaBridge::GlobalArithmetic();
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			return object->Add();
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int MetaSubtract(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				return LuaBridge::GlobalArithmetic();
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			return object->Subtract();
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int MetaNegate(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				return LuaBridge::GlobalArithmetic();
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			return object->Negate();
		}
		catch (const std::exception&)
		{

		}
		return lua_error(lua);
	}

	int MetaMultiply(lua_State* L)
	{
		LuaObject* object;

		object = (LuaObject*)lua_touserdata(lua, 1);
		if (!object)
		{
			try
			{
				return LuaBridge::GlobalArithmetic();
			}
			catch (const std::exception&)
			{

			}
			return lua_error(lua);
		}
		try
		{
			return object->Multiply();
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
		return argument + lua_gettop(lua) + 1;
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
	lua_gc(lua, LUA_GCGEN, 0, 0);
	lua_pushlightuserdata(lua, nullptr);
	lua_createtable(lua, 0, 7);
	lua_pushliteral(lua, "__index");
	lua_pushcfunction(lua, MetaIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__newindex");
	lua_pushcfunction(lua, MetaNewIndex);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__call");
	lua_pushcfunction(lua, MetaCall);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__add");
	lua_pushcfunction(lua, MetaAdd);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__sub");
	lua_pushcfunction(lua, MetaSubtract);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__unm");
	lua_pushcfunction(lua, MetaNegate);
	lua_rawset(lua, -3);
	lua_pushliteral(lua, "__mul");
	lua_pushcfunction(lua, MetaMultiply);
	lua_rawset(lua, -3);
	lua_pushvalue(lua, -1);
	metatable = luaL_ref(lua, LUA_REGISTRYINDEX);
	lua_setmetatable(lua, -2);
	lua_pop(lua, 1);
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

void Script::PushNil()
{
	lua_pushnil(lua);
}

int Script::ToInteger(int argument)
{
	return lua_tointeger(lua, ArgumentToStack(argument));
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
	int stack;

	stack = ArgumentToStack(argument);
	return lua_type(lua, stack) == LUA_TNUMBER && lua_isinteger(lua, stack);
}

bool Script::IsBoolean(int argument)
{
	return lua_isboolean(lua, ArgumentToStack(argument));
}

bool Script::IsNumber(int argument)
{
	int stack;

	stack = ArgumentToStack(argument);
	return lua_type(lua, stack) == LUA_TNUMBER && !lua_isinteger(lua, stack);
}

bool Script::IsData(int argument)
{
	return lua_isuserdata(lua, ArgumentToStack(argument));
}

bool Script::IsString(int argument)
{
	return lua_isstring(lua, ArgumentToStack(argument));
}

bool Script::IsNil(int argument)
{
	return lua_isnil(lua, ArgumentToStack(argument));
}

const char* Script::TypeName(int argument)
{
	return luaL_typename(lua, ArgumentToStack(argument));
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
			Logger::Error(lua_tostring(lua, -1));
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
	length = (int)lua_rawlen(lua, stack);
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
	char name[100];

	strcpy_s(name, "modulescripts\\");
	strcat_s(name, base);
	return LoadFunctions(name);
}

bool Script::LoadFunctions(const char* name)
{
	int status;
	char filename[100];
	const char* mode;

	mode = FindModule(name, filename, 100);
	if (!mode)
		return false;
	status = luaL_loadfilex(lua, filename, mode);
	if (status == LUA_OK)
	{
		lua_pushlightuserdata(lua, nullptr);
		lua_setupvalue(lua, -2, 1);
		status = lua_pcall(lua, 0, 0, -2);
		if (status != LUA_OK)
		{
			Logger::Error(lua_tostring(lua, -1));
			lua_pop(lua, 1);
		}
	}
	else
	{
		EmitFailure(lua_tostring(lua, -1), Logger::Error);
		lua_pop(lua, 1);
	}
	return true;
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

void Script::PushTableValue(int reference, const char* key)
{
	lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
	lua_pushstring(lua, key);
	lua_rawget(lua, -2);
	lua_remove(lua, -2);
}

void Script::AssignTableValue(int reference, const char* key, int argument)
{
	int stack;

	stack = ArgumentToStack(argument);
	lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
	lua_pushstring(lua, key);
	lua_pushvalue(lua, stack);
	lua_rawset(lua, -3);
	lua_pop(lua, 1);
}

const char* Script::TableValueTypeName(int reference, const char* key)
{
	const char* type;

	lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
	lua_pushstring(lua, key);
	lua_rawget(lua, -2);
	type = luaL_typename(lua, -1);
	lua_pop(lua, 2);
	return type;
}

int Script::StoreNewTable()
{
	lua_newtable(lua);
	return luaL_ref(lua, LUA_REGISTRYINDEX);
}

void Script::DeleteTable(int reference)
{
	luaL_unref(lua, LUA_REGISTRYINDEX, reference);
}

int Script::CloneTable(int reference)
{
	lua_newtable(lua);
	lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
	lua_pushnil(lua);
	while (lua_next(lua, -2))
	{
		lua_pushvalue(lua, -2);
		lua_insert(lua, -2);
		lua_rawset(lua, -5);
	}
	lua_pop(lua, 1);
	return luaL_ref(lua, LUA_REGISTRYINDEX);
}

int Script::TraverseReadTable(int reference, void* opaque, void (*process)(void*))
{
	int count;

	count = 0;

	if (reference != LUA_REFNIL)
	{
		lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
		lua_pushnil(lua);
		while (lua_next(lua, -2))
		{
			process(opaque);
			count++;
			lua_pop(lua, 1);
		}
		lua_pop(lua, 1);
	}

	return count;
}

void Script::TraverseAssignTable(int reference, int count, void* opaque, void (*process)(void*))
{
	if (reference != LUA_REFNIL)
	{
		lua_rawgeti(lua, LUA_REGISTRYINDEX, reference);
		for (int i = 0; i < count; i++)
		{
			process(opaque);
			lua_rawset(lua, -3);
		}
		lua_pop(lua, 1);
	}
}
