#include "../definitions/structures_mine.h"
#include "particle.h"
#include "utilities.h"

using namespace Utilities;
using namespace LuaHelpers;

int LuaObjectClass::Call()
{
	Script::Throw("attempt to call a data object");
}

int LuaObjectClass::Add()
{
	return ThrowArithmetic();
}

int LuaObjectClass::Subtract()
{
	return ThrowArithmetic();
}

int LuaObjectClass::Negate()
{
	return ThrowArithmetic();
}

int LuaObjectClass::Multiply()
{
	return ThrowArithmetic();
}

void LuaObjectClass::Index(const char* field)
{
	if (field)
		Script::Throw(FormatString("attempt to access inexistent field '%s'", field));
	Script::Throw("attempt to index a data object");
}

void LuaObjectClass::NewIndex(const char* field)
{
	if (field)
		Script::Throw(FormatString("attempt to access inexistent field '%s'", field));
	Script::Throw("attempt to index a data object");
}

int LuaObjectFunction::Add()
{
	return ThrowArithmeticFunction();
}

int LuaObjectFunction::Subtract()
{
	return ThrowArithmeticFunction();
}

int LuaObjectFunction::Negate()
{
	return ThrowArithmeticFunction();
}

int LuaObjectFunction::Multiply()
{
	return ThrowArithmeticFunction();
}

void LuaObjectFunction::Index(const char* field)
{
	if (field)
		Script::Throw("attempt to access a field of a function object");
	Script::Throw("attempt to index a function object");
}

void LuaObjectFunction::NewIndex(const char* field)
{
	if (field)
		Script::Throw("attempt to access a field of a function object");
	Script::Throw("attempt to index a function object");
}

const char* LuaObjectClassPosition::Name()
{
	return Vector3f::Name();
}

int LuaObjectClassPosition::Add()
{
	auto pos = GetData<LuaObjectClassPosition>(1);
	float x = this->GetX() + pos->GetX();
	float y = this->GetY() + pos->GetY();
	float z = this->GetZ() + pos->GetZ();
	ConstructManagedData<Vector3f>(x, y, z);
	return 1;
}

int LuaObjectClassPosition::Subtract()
{
	auto pos = GetData<LuaObjectClassPosition>(1);
	float x = this->GetX() - pos->GetX();
	float y = this->GetY() - pos->GetY();
	float z = this->GetZ() - pos->GetZ();
	ConstructManagedData<Vector3f>(x, y, z);
	return 1;
}

int LuaObjectClassPosition::Negate()
{
	float x = -this->GetX();
	float y = -this->GetY();
	float z = -this->GetZ();
	ConstructManagedData<Vector3f>(x, y, z);
	return 1;
}

int LuaObjectClassPosition::Multiply()
{
	float n = GetNumber(1);
	float x = this->GetX() * n;
	float y = this->GetY() * n;
	float z = this->GetZ() * n;
	ConstructManagedData<Vector3f>(x, y, z);
	return 1;
}

void LuaObjectClassPosition::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(GetX());
				return;
			}
			break;

		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(GetY());
				return;
			}
			break;

		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(GetZ());
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void LuaObjectClassPosition::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				SetX(GetNumber(-1));
				return;
			}
			break;

		case 'y':
			if (!strcmp(field, "y"))
			{
				SetY(GetNumber(-1));
				return;
			}
			break;

		case 'z':
			if (!strcmp(field, "z"))
			{
				SetZ(GetNumber(-1));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* LuaObjectClassRotation::Name()
{
	return Vector3s::Name();
}

void LuaObjectClassRotation::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(ShortToRad(GetX()));
				return;
			}
			break;

		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(ShortToRad(GetY()));
				return;
			}
			break;

		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(ShortToRad(GetZ()));
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void LuaObjectClassRotation::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				SetX(RadToShort(GetNumber(-1)));
				return;
			}
			break;

		case 'y':
			if (!strcmp(field, "y"))
			{
				SetY(RadToShort(GetNumber(-1)));
				return;
			}
			break;

		case 'z':
			if (!strcmp(field, "z"))
			{
				SetZ(RadToShort(GetNumber(-1)));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* LuaItemInfoPos::Name()
{
	return Vector3f::Name();
}

float LuaItemInfoPos::GetX()
{
	return (float)pos->xPos;
}

float LuaItemInfoPos::GetY()
{
	return (float)pos->yPos;
}

float LuaItemInfoPos::GetZ()
{
	return (float)pos->zPos;
}

void LuaItemInfoPos::SetX(float x)
{
	pos->xPos = SaturateRound<int>(x);
}

void LuaItemInfoPos::SetY(float y)
{
	pos->yPos = SaturateRound<int>(y);
}

void LuaItemInfoPos::SetZ(float z)
{
	pos->zPos = SaturateRound<int>(z);
}

LuaItemInfoPos::operator Vector3f()
{
	return Vector3f((float)pos->xPos, (float)pos->yPos, (float)pos->zPos);
}

const char* LuaItemInfoRot::Name()
{
	return Vector3s::Name();
}

short LuaItemInfoRot::GetX()
{
	return pos->xRot;
}

short LuaItemInfoRot::GetY()
{
	return pos->yRot;
}

short LuaItemInfoRot::GetZ()
{
	return pos->zRot;
}

void LuaItemInfoRot::SetX(short x)
{
	pos->xRot = x;
}

void LuaItemInfoRot::SetY(short y)
{
	pos->yRot = y;
}

void LuaItemInfoRot::SetZ(short z)
{
	pos->zRot = z;
}

LuaItemInfoRot::operator Vector3s()
{
	return Vector3s(pos->xRot, pos->yRot, pos->zRot);
}

void LuaBridge::GlobalIndex(const char* field)
{
	LuaObject* object;
	std::optional<int> opt_int;
	std::optional<float> opt_float;

	if (field)
	{
		object = LuaGlobals::RetrieveGlobals(field);
		if (object)
		{
			Script::PushData(object);
			return;
		}
		object = LuaFunctions::RetrieveFunction(field);
		if (object)
		{
			Script::PushData(object);
			return;
		}
		opt_int = LuaGlobals::RetrieveIntegerConstant(field);
		if (opt_int)
		{
			Script::PushInteger(*opt_int);
			return;
		}
		opt_float = LuaGlobals::RetrieveFloatConstant(field);
		if (opt_float)
		{
			Script::PushNumber(*opt_float);
			return;
		}
		Script::Throw("attempt to read from a global variable");
	}
	Script::Throw("attempt to index the global environment");
}

void LuaBridge::GlobalNewIndex(const char* field)
{
	if (field)
	{
		if (LuaGlobals::RetrieveGlobals(field))
			Script::Throw("attempt to assign to a built-in object");
		if (LuaFunctions::RetrieveFunction(field))
			Script::Throw("attempt to assign to a built-in function");
		if (LuaGlobals::RetrieveIntegerConstant(field))
			Script::Throw("attempt to assign to a built-in constant");
		if (LuaGlobals::RetrieveFloatConstant(field))
			Script::Throw("attempt to assign to a built-in constant");
		Script::Throw("attempt to write to a global variable");
	}
	Script::Throw("attempt to index the global environment");
}

int LuaBridge::GlobalCall()
{
	Script::Throw("attempt to call the global environment");
}

int LuaBridge::GlobalArithmetic()
{
	Script::Throw("attempt to perform arithmetic on the global environment");
}
