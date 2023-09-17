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
	void ExecuteFunction(int reference, void* value);
	int StoreFunction(int argument);
	void Print();
	void LoadFunctions(const char* filename);
	int TypeError(int argument, const char* type);
	const char* FormatString(const char* format, ...);
	int GarbageCount();
	void PreFunctionLoop();
	void PostFunctionLoop();
}
