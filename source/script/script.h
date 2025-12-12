#pragma once

#define SCRIPT_REFNIL -1

struct LuaObject
{
	virtual void Index(const char* field) = 0;
	virtual void NewIndex(const char* field) = 0;
	virtual int Call() = 0;
	virtual int Add() = 0;
	virtual int Subtract() = 0;
	virtual int Negate() = 0;
	virtual int Multiply() = 0;
};

namespace LuaBridge
{
	void GlobalIndex(const char* field);
	void GlobalNewIndex(const char* field);
	int GlobalCall();
	int GlobalArithmetic();
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
	void PushNil();
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
	bool IsNil(int argument);
	const char* TypeName(int argument);
	int StoreFunction(int argument);
	bool ExecuteFunction(int reference, void* value);
	bool IsFunction(int argument);
	void DeleteFunction(int* reference);
	void PushTable(int argument, int length);
	int ExplodeTable(int argument);
	bool IsTable(int argument);
	void Print();
	bool Require(const char* base);
	bool LoadFunctions(const char* name);
	[[noreturn]] void Throw(const char* msg);
	void EmitFailure(const char* msg, void (*log)(const char*));
	int GarbageCount();
	void PreFunctionLoop();
	void PostFunctionLoop();
	void* CreateManagedData(unsigned int size);
	void* GetExtraSpace();
	void PushTableValue(int reference, const char* key);
	void AssignTableValue(int reference, const char* key, int argument);
	const char* TableValueTypeName(int reference, const char* key);
	int StoreNewTable();
	void DeleteTable(int* reference);
	int CloneTable(int reference);
	int TraverseReadTable(int reference, void* opaque, void (*process)(void*));
	void TraverseAssignTable(int reference, int count, void* opaque, void (*process)(void*));
}
