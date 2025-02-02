#pragma once
#include "../StdAfx.h"
#include "script.h"

struct Vector3f;
struct Vector3s;
struct phd_3dpos;

namespace Particles
{
	struct ParticleGroup;
}

struct LuaObjectClass : public LuaObject
{
	virtual int Call() final override;
	virtual int Add() override;
	virtual int Subtract() override;
	virtual int Negate() override;
	virtual int Multiply() override;
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct LuaObjectClassPosition : public LuaObjectClass
{
	static const char* Name();

	virtual int Add() final override;
	virtual int Negate() final override;
	virtual int Subtract() final override;
	virtual int Multiply() final override;

	virtual void Index(const char* field) final override;
	virtual void NewIndex(const char* field) final override;

	virtual float GetX() = 0;
	virtual float GetY() = 0;
	virtual float GetZ() = 0;
	virtual void SetX(float x) = 0;
	virtual void SetY(float y) = 0;
	virtual void SetZ(float z) = 0;
	virtual explicit operator Vector3f() = 0;
};

struct LuaObjectClassRotation : public LuaObjectClass
{
	static const char* Name();

	virtual void Index(const char* field) final override;
	virtual void NewIndex(const char* field) final override;

	virtual short GetX() = 0;
	virtual short GetY() = 0;
	virtual short GetZ() = 0;
	virtual void SetX(short x) = 0;
	virtual void SetY(short y) = 0;
	virtual void SetZ(short z) = 0;
	virtual explicit operator Vector3s() = 0;
};

struct LuaItemInfoPos final : public LuaObjectClassPosition
{
	LuaItemInfoPos(phd_3dpos* position) : pos(position) {}

	static const char* Name();

	virtual float GetX() override;
	virtual float GetY() override;
	virtual float GetZ() override;
	virtual void SetX(float x) override;
	virtual void SetY(float y) override;
	virtual void SetZ(float z) override;
	virtual explicit operator Vector3f() override;

private:

	phd_3dpos* const pos;
};

struct LuaItemInfoRot final : public LuaObjectClassRotation
{
	LuaItemInfoRot(phd_3dpos* position) : pos(position) {}

	static const char* Name();

	virtual short GetX() override;
	virtual short GetY() override;
	virtual short GetZ() override;
	virtual void SetX(short x) override;
	virtual void SetY(short y) override;
	virtual void SetZ(short z) override;
	virtual explicit operator Vector3s() override;

private:

	phd_3dpos* const pos;
};

struct LuaObjectFunction : public LuaObject
{
	virtual int Add() final override;
	virtual int Negate() final override;
	virtual int Subtract() final override;
	virtual int Multiply() final override;
	virtual void Index(const char* field) final override;
	virtual void NewIndex(const char* field) final override;
};


namespace LuaFunctions
{
	LuaObject* RetrieveFunction(const char* field);
}


namespace LuaGlobals
{
	struct TrngVarWrapper final : public LuaObjectClass
	{
		virtual void Index(const char* field) override;
		virtual void NewIndex(const char* field) override;
	};

	struct LuaItemInfoWrapper final : public LuaObjectClass
	{
		static const char* Name();
		virtual void Index(const char* field) override;
		virtual void NewIndex(const char* field) override;
	};

	extern TrngVarWrapper TrngVars;
	extern LuaItemInfoWrapper LuaItemArray[];

	LuaObject* RetrieveGlobals(const char* field);
	std::optional<int> RetrieveIntegerConstant(const char* field);
	std::optional<float> RetrieveFloatConstant(const char* field);
}


namespace LuaHelpers
{
	template<class T>
	T* GetData(int argument)
	{
		T* object;
		if (!Script::IsData(argument))
			Script::Throw(FormatString("%s expected", T::Name()));
		object = dynamic_cast<T*>(Script::ToData(argument));
		if (!object)
			Script::Throw(FormatString("%s expected", T::Name()));
		return object;
	}

	template<size_t size>
	void CopyString(char(&destination)[size], const char* source)
	{
		int count = min(size - 1, strlen(source));
		memcpy(destination, source, count);
		destination[count] = '\0';
	}

	template<class T, class... Args>
	std::enable_if_t<std::is_trivially_destructible_v<T>, T>* ConstructManagedData(Args&&... args)
	{
		return new(Script::CreateManagedData(sizeof(T))) T(std::forward<Args>(args)...);
	}

	int GetInteger(int argument);
	bool GetBoolean(int argument);
	float GetNumber(int argument);
	int GetFunction(int argument);
	const char* GetLuaString(int argument);
	int GetTable(int argument);
	int GetArgCount(int minimum, int maximum);
	int VerifyItemIndex(int argument);
	int GetTombIndexByNGLEIndex(int argument);
	float GetMathResult(int argument, float (*operation)(float));
	float GetMathResult(int firstArgument, int secondArgument, float (*operation)(float, float));
	int GetClampedInteger(int argument, int min, int max, bool throwBoundsError = false);
	float GetClampedNumber(int argument, float min, float max, bool throwBoundsError = false);
	int GetConstrainedInteger(int argument, int defaultValue, int count, ...);
	void CheckCaller(int callers, const char* function);
	void CheckFieldCaller(int callers, const char* field);
	void ReadOnlyFieldError(const char* field);
	int ThrowArithmetic();
	Vector3f GetItemPos(int argument);
	const char* GetBoundedLuaString(int argument, unsigned int length);
	void RequireModule(int argument);
	void CheckModuleParameter(int argument);
	int GetBoundFunction(int index);
}
