#include "..\definitions\includes.h"

using namespace Utilities;
using namespace LuaHelpers;

int LuaObjectClass::Call()
{
	Script::ThrowError("attempt to call a data object");
}

void LuaObjectClass::Index(const char* field)
{
	if (field)
		Script::ThrowError(FormatString("attempt to access inexistent field \"%s\"", field));
	Script::ThrowError("attempt to index a data object");
}

void LuaObjectClass::NewIndex(const char* field)
{
	if (field)
		Script::ThrowError(FormatString("attempt to access inexistent field \"%s\"", field));
	Script::ThrowError("attempt to index a data object");
}

void LuaObjectFunction::Index(const char* field)
{
	if (field)
		Script::ThrowError("attempt to access a field of a function object");
	Script::ThrowError("attempt to index a function object");
}

void LuaObjectFunction::NewIndex(const char* field)
{
	if (field)
		Script::ThrowError("attempt to access a field of a function object");
	Script::ThrowError("attempt to index a function object");
}

const char* LuaObjectClassPosition::Name()
{
	return Vector3f::Name();
}

const char* LuaObjectClassRotation::Name()
{
	return Vector3s::Name();
}

const char* LuaItemInfoPos::Name()
{
	return Vector3f::Name();
}

void LuaItemInfoPos::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(pos->xPos);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(pos->yPos);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(pos->zPos);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void LuaItemInfoPos::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				pos->xPos = GetNumber(-1);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				pos->yPos = GetNumber(-1);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				pos->zPos = GetNumber(-1);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

float LuaItemInfoPos::GetX()
{
	return pos->xPos;
}

float LuaItemInfoPos::GetY()
{
	return pos->yPos;
}

float LuaItemInfoPos::GetZ()
{
	return pos->zPos;
}

LuaItemInfoPos::operator Vector3f()
{
	return Vector3f(pos->xPos, pos->yPos, pos->zPos);
}

const char* LuaItemInfoRot::Name()
{
	return Vector3s::Name();
}

void LuaItemInfoRot::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(ShortToRad(pos->xRot));
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(ShortToRad(pos->yRot));
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(ShortToRad(pos->zRot));
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void LuaItemInfoRot::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				pos->xRot = RadToShort(GetNumber(-1));
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				pos->yRot = RadToShort(GetNumber(-1));
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				pos->zRot = RadToShort(GetNumber(-1));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
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
		Script::ThrowError("attempt to read from a global variable");
	}
	Script::ThrowError("attempt to index the global environment");
}

void LuaBridge::GlobalNewIndex(const char* field)
{
	if (field)
	{
		if (LuaGlobals::RetrieveGlobals(field))
			Script::ThrowError("attempt to assign to a built-in object");
		if (LuaFunctions::RetrieveFunction(field))
			Script::ThrowError("attempt to assign to a built-in function");
		if (LuaGlobals::RetrieveIntegerConstant(field))
			Script::ThrowError("attempt to assign to a built-in constant");
		if (LuaGlobals::RetrieveFloatConstant(field))
			Script::ThrowError("attempt to assign to a built-in constant");
		Script::ThrowError("attempt to write to a global variable");
	}
	Script::ThrowError("attempt to index the global environment");
}

int LuaBridge::GlobalCall()
{
	Script::ThrowError("attempt to call the global environment");
}
