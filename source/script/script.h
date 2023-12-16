#pragma once

struct LuaObject
{
	virtual int Index(const char* field) = 0;
	virtual void NewIndex(const char* field) = 0;
};

namespace LuaBridge
{
	int Call(const char* function);
}

namespace Script
{
	void NewState();
	void Close();
	void PushInteger(int value);
	void PushBoolean(bool value);
	void PushNumber(float value);
	void PushData(void* value);
	int ToInteger(int argument);
	bool ToBoolean(int argument);
	float ToNumber(int argument);
	void* ToData(int argument);
	int ArgCount();
	bool IsInteger(int argument);
	bool IsBoolean(int argument);
	bool IsNumber(int argument);
	bool IsData(int argument);
	int StoreFunction(int argument);
	bool ExecuteFunction(int reference, void* value);
	bool IsFunction(int argument);
	void DeleteFunction(int* reference);
	void Print();
	void LoadFunctions(const char* filename);
	void ThrowError(const char* msg);
	void EmitWarning(const char* msg);
	void AddInformation(const char* msg);
	int GarbageCount();
	void PreFunctionLoop();
	void PostFunctionLoop();
}
