#pragma once

struct LuaObject
{
	virtual void Index(const char* field) = 0;
	virtual void NewIndex(const char* field) = 0;
	virtual int Call() = 0;
};

namespace LuaBridge
{
	void GlobalIndex(const char* field);
	void GlobalNewIndex(const char* field);
	int GlobalCall();
}

namespace Script
{
	void NewState();
	void Close();
	void PushInteger(int value);
	void PushBoolean(bool value);
	void PushNumber(float value);
	void PushData(LuaObject* value);
	void PushString(const char* string);
	int ToInteger(int argument);
	bool ToBoolean(int argument);
	float ToNumber(int argument);
	LuaObject* ToData(int argument);
	const char* ToString(int argument);
	int ArgCount();
	bool IsInteger(int argument);
	bool IsBoolean(int argument);
	bool IsNumber(int argument);
	bool IsData(int argument);
	bool IsString(int argument);
	int StoreFunction(int argument);
	bool ExecuteFunction(int reference, void* value);
	bool IsFunction(int argument);
	void DeleteFunction(int* reference);
	void PushTable(int argument, int length);
	int ExplodeTable(int argument);
	bool IsTable(int argument);
	void Print();
	bool Require(const char* base);
	void LoadFunctions(const char* name);
	[[noreturn]] void Throw(const char* msg);
	void EmitFailure(const char* msg, void (*log)(const char*));
	void AddInformation(const char* msg);
	int GarbageCount();
	void PreFunctionLoop();
	void PostFunctionLoop(int results);
	void* CreateManagedData(unsigned int size);
	void* GetExtraSpace();
}
