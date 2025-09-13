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
	int Call() final;
	int Add() override;
	int Subtract() override;
	int Negate() override;
	int Multiply() override;
	void Index(const char* field) override;
	void NewIndex(const char* field) override;
};

struct LuaObjectClassPosition : public LuaObjectClass
{
	static const char* Name();

	int Add() final;
	int Negate() final;
	int Subtract() final;
	int Multiply() final;

	void Index(const char* field) final;
	void NewIndex(const char* field) final;

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

	void Index(const char* field) final;
	void NewIndex(const char* field) final;

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

	float GetX() override;
	float GetY() override;
	float GetZ() override;
	void SetX(float x) override;
	void SetY(float y) override;
	void SetZ(float z) override;
	explicit operator Vector3f() override;

private:

	phd_3dpos* const pos;
};

struct LuaItemInfoRot final : public LuaObjectClassRotation
{
	LuaItemInfoRot(phd_3dpos* position) : pos(position) {}

	static const char* Name();

	short GetX() override;
	short GetY() override;
	short GetZ() override;
	void SetX(short x) override;
	void SetY(short y) override;
	void SetZ(short z) override;
	explicit operator Vector3s() override;

private:

	phd_3dpos* const pos;
};

struct LuaObjectFunction : public LuaObject
{
	int Add() final;
	int Negate() final;
	int Subtract() final;
	int Multiply() final;
	void Index(const char* field) final;
	void NewIndex(const char* field) final;
};

struct LuaTableObjectClass : public LuaObjectClass
{
	int table;

	LuaTableObjectClass() : table(SCRIPT_REFNIL) {};
};

namespace LuaFunctions
{
	LuaObject* RetrieveFunction(const char* field);
}


namespace LuaGlobals
{
	struct TrngVarWrapper final : public LuaObjectClass
	{
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct LuaItemInfoWrapper final : public LuaObjectClass
	{
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
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
	int GetClampedInteger(int argument, int min, int max, bool throwBoundsError = false);
	float GetClampedNumber(int argument, float min, float max, bool throwBoundsError = false);
	int GetConstrainedInteger(int argument, int defaultValue, int count, ...);
	void CheckCaller(int callers, const char* function);
	void CheckFieldCaller(int callers, const char* field);
	void ReadOnlyFieldError(const char* field);
	int ThrowArithmetic();
	int ThrowArithmeticFunction();
	Vector3f GetItemPos(int argument);
	const char* GetBoundedLuaString(int argument, unsigned int length);
	void RequireModule(int argument);
	void CheckModuleParameter(int argument);
	void CheckParticleData(int argument);
	int GetBoundFunction(int index);
	bool GetScriptIntegrity();
	void ExitSystem(const char* message);
}
