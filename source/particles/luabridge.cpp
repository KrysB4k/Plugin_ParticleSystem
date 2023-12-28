#include "..\definitions\includes.h"

namespace
{
	template<class T>
	T* GetData(int argument)
	{
		if (!Script::IsData(argument))
			Script::ThrowError(FormatString("%s expected", T::Name()));
		T* object = dynamic_cast<T*>(Script::ToData(argument));
		if (!object)
			Script::ThrowError(FormatString("%s expected", T::Name()));
		return object;
	}

	int GetInteger(int argument)
	{
		if (!Script::IsInteger(argument))
			Script::ThrowError("integer expected");
		return Script::ToInteger(argument);
	}

	bool GetBoolean(int argument)
	{
		if (!Script::IsBoolean(argument))
			Script::ThrowError("boolean expected");
		return Script::ToBoolean(argument);
	}

	float GetNumber(int argument)
	{
		if (!Script::IsNumber(argument))
			Script::ThrowError("number expected");
		return Script::ToNumber(argument);
	}

	int GetFunction(int argument)
	{
		if (!Script::IsFunction(argument))
			Script::ThrowError("function or nil expected");
		return Script::StoreFunction(argument);
	}

	int GetArgCount(int minimum, int maximum)
	{
		int count;

		count = Script::ArgCount();
		if (count < minimum)
			Script::ThrowError(FormatString("at least %d arguments expected", minimum));
		if (count > maximum)
			Script::ThrowError(FormatString("at most %d arguments expected", minimum));
		return count;
	}

	Tr4ItemInfo* GetItem(int argument)
	{
		int index;

		index = GetInteger(argument);
		if (index >= 0 && index < level_items)
			return &items[index];
		Script::EmitWarning(FormatString("%d does not correspond to a valid Tomb index", index));
		return nullptr;
	}

	int GetTombIndexByNGLEIndex(int argument)
	{
		int ngindex, index;

		ngindex = GetInteger(argument);
		if (ngindex >= 0 && ngindex < 6000)
		{
			index = FromNgleIndexToTomb4Index(ngindex);
			if (index != -1)
				return index;
		}
		Script::EmitWarning(FormatString("%d does not correspond to a valid NGLE index", ngindex));
		return -1;
	}

	float GetMathResult(int argument, float (*operation)(float))
	{
		float x, result;

		x = GetNumber(argument);
		result = operation(x);
		if (!isnan(x) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	float GetMathResult(int firstArgument, int secondArgument, float (*operation)(float, float))
	{
		float x, y, result;

		x = GetNumber(firstArgument);
		y = GetNumber(secondArgument);
		result = operation(x, y);
		if (!isnan(x) && !isnan(y) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	int GetClampedInteger(int argument, int min, int max)
	{
		int x;

		x = GetInteger(argument);
		if (x < min)
		{
			Script::EmitWarning(FormatString("%d is less than the minimum of %d", x, min));
			return min;
		}
		if (x > max)
		{
			Script::EmitWarning(FormatString("%d is greater than the maximum of %d", x, max));
			return max;
		}
		return x;
	}

	float GetClampedNumber(int argument, float min, float max)
	{
		float x;

		x = GetNumber(argument);
		if (x < min)
		{
			Script::EmitWarning(FormatString("%f is less than the minimum of %f", x, min));
			return min;
		}
		if (x > max)
		{
			Script::EmitWarning(FormatString("%f is greater than the maximum of %f", x, max));
			return max;
		}
		return x;
	}

	int GetConstrainedInteger(int argument, int defaultValue, int count, ...)
	{
		va_list args;
		int x;
		bool present;

		x = GetInteger(argument);
		present = false;
		va_start(args, count);
		for (int i = 0; i < count; i++)
		{
			if (x == va_arg(args, int))
			{
				present = true;
				break;
			}
		}
		va_end(args);
		if (!present)
		{
			Script::EmitWarning(FormatString("%d is not in the list of allowed values", x));
			return defaultValue;
		}
		return x;
	}

	void CheckCaller(int callers, const char* function)
	{
		if (!(ParticleFactory::caller & callers))
			Script::ThrowError(FormatString("calling function \"%s\" is forbidden in this phase", function));
	}

	void CheckFieldCaller(int callers, const char* field)
	{
		if (!(ParticleFactory::caller & callers))
			Script::ThrowError(FormatString("assignment to field \"%s\" is forbidden in this phase", field));
	}
}

namespace LuaGlobals
{
	AbsFunction Abs;
	AcosFunction Acos;
	AsinFunction Asin;
	AtanFunction Atan;
	Atan2Function Atan2;
	BoidAlignmentFunction BoidAlignment;
	BoidCohesionFunction BoidCohesion;
	BoidSeparationFunction BoidSeparation;
	CbrtFunction Cbrt;
	CosFunction Cos;
	CreateGroupFunction CreateGroup;
	CreateMeshPartFunction CreateMeshPart;
	CreateSpritePartFunction CreateSpritePart;
	GetTombIndexFunction GetTombIndex;
	GetLaraIndexFunction GetLaraIndex;
	GetItemRoomFunction GetItemRoom;
	MeshAlignVelocityFunction MeshAlignVelocity;
	MeshShatterFunction MeshShatter;
	NoisePerlinFunction NoisePerlin;
	NoiseSimplexFunction NoiseSimplex;
	ParticleAnimateFunction ParticleAnimate;
	ParticleCollidedItemFunction ParticleCollidedItem;
	ParticleCollideFloorsFunction ParticleCollideFloors;
	ParticleCollideWallsFunction ParticleCollideWalls;
	ParticleHomingFunction ParticleHoming;
	ParticleLimitSpeedFunction ParticleLimitSpeed;
	PrintFunction Print;
	RandfloatFunction Randfloat;
	RandintFunction Randint;
	SeedNoiseFunction SeedNoise;
	SinFunction Sin;
	SqrtFunction Sqrt;
}

LuaObject* LuaGlobals::RetrieveFunction(const char* field)
{
	switch (field[0])
	{
	case 'a':
		if (!strcmp(field, "abs"))
			return &Abs;
		if (!strcmp(field, "acos"))
			return &Acos;
		if (!strcmp(field, "asin"))
			return &Asin;
		if (!strcmp(field, "atan"))
			return &Atan;
		if (!strcmp(field, "atan2"))
			return &Atan2;
		break;
	case 'b':
		if (!strcmp(field, "boidAlignment"))
			return &BoidAlignment;
		if (!strcmp(field, "boidCohesion"))
			return &BoidCohesion;
		if (!strcmp(field, "boidSeparation"))
			return &BoidSeparation;
		break;
	case 'c':
		if (!strcmp(field, "cbrt"))
			return &Cbrt;
		if (!strcmp(field, "cos"))
			return &Cos;
		if (!strcmp(field, "createGroup"))
			return &CreateGroup;
		if (!strcmp(field, "createMeshPart"))
			return &CreateMeshPart;
		if (!strcmp(field, "createSpritePart"))
			return &CreateSpritePart;
		break;
	case 'g':
		if (!strcmp(field, "getTombIndex"))
			return &GetTombIndex;
		if (!strcmp(field, "getLaraIndex"))
			return &GetLaraIndex;
		if (!strcmp(field, "getItemRoom"))
			return &GetItemRoom;
		break;
	case 'm':
		if (!strcmp(field, "meshAlignVelocity"))
			return &MeshAlignVelocity;
		if (!strcmp(field, "meshShatter"))
			return &MeshShatter;
		break;
	case 'n':
		if (!strcmp(field, "noisePerlin"))
			return &NoisePerlin;
		if (!strcmp(field, "noiseSimplex"))
			return &NoiseSimplex;
		break;
	case 'p':
		if (!strcmp(field, "particleAnimate"))
			return &ParticleAnimate;
		if (!strcmp(field, "particleCollidedItem"))
			return &ParticleCollidedItem;
		if (!strcmp(field, "particleCollideFloors"))
			return &ParticleCollideFloors;
		if (!strcmp(field, "particleCollideWalls"))
			return &ParticleCollideWalls;
		if (!strcmp(field, "particleHoming"))
			return &ParticleHoming;
		if (!strcmp(field, "particleLimitSpeed"))
			return &ParticleLimitSpeed;
		if (!strcmp(field, "print"))
			return &Print;
		break;
	case 'r':
		if (!strcmp(field, "randfloat"))
			return &Randfloat;
		if (!strcmp(field, "randint"))
			return &Randint;
		break;
	case 's':
		if (!strcmp(field, "seedNoise"))
			return &SeedNoise;
		if (!strcmp(field, "sin"))
			return &Sin;
		if (!strcmp(field, "sqrt"))
			return &Sqrt;
		break;
	}
	return nullptr;
}

int LuaObjectClass::Call()
{
	Script::ThrowError("attempt to call a data object");
}

void LuaObjectClass::Index(const char* field)
{
	if (field)
		Script::ThrowError(FormatString("attempt to access inexistent field %s", field));
	Script::ThrowError("attempt to index a data object");
}

void LuaObjectClass::NewIndex(const char* field)
{
	if (field)
		Script::ThrowError(FormatString("attempt to access inexistent field %s", field));
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

const char* ColorRGB::Name()
{
	return "ColorRGB";
}

void ColorRGB::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'b':
			if (!strcmp(field, "b"))
			{
				Script::PushNumber((float)B / 255);
				return;
			}
			break;
		case 'g':
			if (!strcmp(field, "g"))
			{
				Script::PushNumber((float)G / 255);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "r"))
			{
				Script::PushNumber((float)R / 255);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void ColorRGB::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'b':
			if (!strcmp(field, "b"))
			{
				B = 255 * GetClampedNumber(-1, 0.0f, 1.0f);
				return;
			}
			break;
		case 'g':
			if (!strcmp(field, "g"))
			{
				G = 255 * GetClampedNumber(-1, 0.0f, 1.0f);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "r"))
			{
				R = 255 * GetClampedNumber(-1, 0.0f, 1.0f);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* Vector3f::Name()
{
	return "Vector3f";
}

void Vector3f::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(x);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(y);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(z);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void Vector3f::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				x = GetNumber(-1);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				y = GetNumber(-1);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				z = GetNumber(-1);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* Vector3s::Name()
{
	return "Vector3s";
}

void Vector3s::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber(ShortToRad(x));
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber(ShortToRad(y));
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber(ShortToRad(z));
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void Vector3s::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				x = RadToShort(GetNumber(-1));
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				y = RadToShort(GetNumber(-1));
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				z = RadToShort(GetNumber(-1));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* Vector3i::Name()
{
	return "Vector3i";
}

void Vector3i::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				Script::PushNumber((float)x / 16384);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				Script::PushNumber((float)y / 16384);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				Script::PushNumber((float)z / 16384);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void Vector3i::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'x':
			if (!strcmp(field, "x"))
			{
				x = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				y = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				z = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* NodeAttachment::Name()
{
	return "NodeAttachment";
}

void NodeAttachment::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'c':
			if (!strcmp(field, "cutoff"))
			{
				Script::PushInteger(cutoff);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "random"))
			{
				Script::PushInteger(random);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "tether"))
			{
				Script::PushInteger(tether);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void NodeAttachment::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'c':
			if (!strcmp(field, "cutoff"))
			{
				cutoff = GetClampedInteger(-1, 0, 32767);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "random"))
			{
				random = GetClampedInteger(-1, 0, 32767);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "tether"))
			{
				tether = static_cast<TetherType>(GetClampedInteger(-1, 0, 2));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* ParticleGroup::Name()
{
	return "ParticleGroup";
}

void ParticleGroup::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'a':
			if (!strcmp(field, "attach"))
			{
				Script::PushData(&attach);
				return;
			}
			break;
		case 'b':
			if (!strcmp(field, "blendingMode"))
			{
				Script::PushInteger(blendingMode);
				return;
			}
			break;
		case 'd':
			if (!strcmp(field, "drawMode"))
			{
				Script::PushInteger(drawMode);
				return;
			}
			break;
		case 'l':
			if (!strcmp(field, "lineIgnoreVel"))
			{
				Script::PushBoolean(LineIgnoreVel);
				return;
			}
			break;
		case 's':
			if (!strcmp(field, "saved"))
			{
				Script::PushBoolean(Saved);
				return;
			}
			if (!strcmp(field, "screenSpace"))
			{
				Script::PushBoolean(ScreenSpace);
				return;
			}
			if (!strcmp(field, "spriteSlot"))
			{
				Script::PushInteger(spriteSlot);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void ParticleGroup::NewIndex(const char* field)
{
	CheckFieldCaller(FUNCTION_LIBRARY, field);
	if (field)
	{
		switch (field[0])
		{
		case 'b':
			if (!strcmp(field, "blendingMode"))
			{
				blendingMode = GetClampedInteger(-1, 0, 13);
				return;
			}
			break;
		case 'd':
			if (!strcmp(field, "drawMode"))
			{
				drawMode = static_cast<DrawMode>(GetClampedInteger(-1, 0, 3));
				return;
			}
			break;
		case 'l':
			if (!strcmp(field, "lineIgnoreVel"))
			{
				LineIgnoreVel = GetBoolean(-1);
				return;
			}
			break;
		case 's':
			if (!strcmp(field, "saved"))
			{
				Saved = GetBoolean(-1);
				return;
			}
			if (!strcmp(field, "screenSpace"))
			{
				ScreenSpace = GetBoolean(-1);
				return;
			}
			if (!strcmp(field, "spriteSlot"))
			{
				spriteSlot = GetConstrainedInteger(-1, SLOT_DEFAULT_SPRITES, 3, SLOT_DEFAULT_SPRITES, SLOT_MISC_SPRITES, SLOT_CUSTOM_SPRITES);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* BaseParticle::Name()
{
	return "SpriteParticle or MeshParticle";
}

void BaseParticle::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'a':
			if (!strcmp(field, "accel"))
			{
				Script::PushData(&accel);
				return;
			}
			break;
		case 'e':
			if (!strcmp(field, "emitterIndex"))
			{
				Script::PushInteger(emitterIndex);
				return;
			}
			if (!strcmp(field, "emitterNode"))
			{
				Script::PushInteger(emitterNode);
				return;
			}
			break;
		case 'l':
			if (!strcmp(field, "lifeCounter"))
			{
				Script::PushInteger(lifeCounter);
				return;
			}
			if (!strcmp(field, "lifeSpan"))
			{
				Script::PushInteger(lifeSpan);
				return;
			}
			break;
		case 'p':
			if (!strcmp(field, "pos"))
			{
				Script::PushData(&pos);
				return;
			}
		case 'r':
			if (!strcmp(field, "roomIndex"))
			{
				Script::PushInteger(roomIndex);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "t"))
			{
				Script::PushNumber(Parameter());
				return;
			}
			break;
		case 'v':
			if (!strcmp(field, "vel"))
			{
				Script::PushData(&vel);
				return;
			}
			break;
		}
	}
	LuaObjectClass::Index(field);
}

void BaseParticle::NewIndex(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'e':
			if (!strcmp(field, "emitterIndex"))
			{
				emitterIndex = GetClampedInteger(-1, -1, level_items - 1);
				if (emitterIndex != -1)
					emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
				else
					emitterNode = -1;
				return;
			}
			if (!strcmp(field, "emitterNode"))
			{
				if (emitterIndex != -1)
					emitterNode = GetClampedInteger(-1, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
				return;
			}
			break;
		case 'l':
			if (!strcmp(field, "lifeCounter"))
			{
				lifeCounter = GetClampedInteger(-1, 0, lifeSpan);
				return;
			}
			if (!strcmp(field, "lifeSpan"))
			{
				// set lifeCounter to lifeSpan automatically
				lifeCounter = lifeSpan = GetClampedInteger(-1, 0, 32767);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "roomIndex"))
			{
				roomIndex = GetClampedInteger(-1, 0, number_rooms - 1);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* SpriteParticle::Name()
{
	return "SpriteParticle";
}

void SpriteParticle::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'c':
			if (!strcmp(field, "colCust"))
			{
				Script::PushData(&colCust);
				return;
			}
			if (!strcmp(field, "colEnd"))
			{
				Script::PushData(&colEnd);
				return;
			}
			if (!strcmp(field, "colStart"))
			{
				Script::PushData(&colStart);
				return;
			}
			if (!strcmp(field, "colorFadeTime"))
			{
				Script::PushInteger(colorFadeTime);
				return;
			}
			break;
		case 'f':
			if (!strcmp(field, "fadeIn"))
			{
				Script::PushInteger(fadeIn);
				return;
			}
			if (!strcmp(field, "fadeOut"))
			{
				Script::PushInteger(fadeOut);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "rot"))
			{
				Script::PushNumber(ShortToRad(rot));
				return;
			}
			if (!strcmp(field, "rotVel"))
			{
				Script::PushNumber(ShortToRad(rotVel));
				return;
			}
			break;
		case 's':
			if (!strcmp(field, "sizeCust"))
			{
				Script::PushInteger(sizeCust);
				return;
			}
			if (!strcmp(field, "sizeEnd"))
			{
				Script::PushInteger(sizeEnd);
				return;
			}
			if (!strcmp(field, "sizeStart"))
			{
				Script::PushInteger(sizeStart);
				return;
			}
			if (!strcmp(field, "sizeRatio"))
			{
				Script::PushNumber(sizeRatio / 32768.0f);
				return;
			}
			if (!strcmp(field, "spriteIndex"))
			{
				Script::PushInteger(spriteIndex);
				return;
			}
			break;
		}
	}
	BaseParticle::Index(field);
}

void SpriteParticle::NewIndex(const char* field)
{
	CheckFieldCaller(FUNCTION_INIT | FUNCTION_UPDATE, field);
	if (field)
	{
		switch (field[0])
		{
		case 'c':
			if (!strcmp(field, "colorFadeTime"))
			{
				colorFadeTime = GetClampedInteger(-1, -32768, 32767);
				return;
			}
			break;
		case 'f':
			if (!strcmp(field, "fadeIn"))
			{
				fadeIn = GetClampedInteger(-1, 0, 32767);
				return;
			}
			if (!strcmp(field, "fadeOut"))
			{
				fadeOut = GetClampedInteger(-1, 0, 32767);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "rot"))
			{
				rot = RadToShort(GetNumber(-1));
				return;
			}
			if (!strcmp(field, "rotVel"))
			{
				rotVel = RadToShort(GetNumber(-1));
				return;
			}
			break;
		case 's':
			if (!strcmp(field, "sizeCust"))
			{
				sizeCust = GetClampedInteger(-1, 0, 65535);
				return;
			}
			if (!strcmp(field, "sizeEnd"))
			{
				sizeEnd = GetClampedInteger(-1, 0, 65535);
				return;
			}
			if (!strcmp(field, "sizeStart"))
			{
				sizeStart = GetClampedInteger(-1, 0, 65535);
				return;
			}
			if (!strcmp(field, "sizeRatio"))
			{
				sizeRatio = 32767 * GetClampedNumber(-1, -1.0f, 1.0f);
				return;
			}
			if (!strcmp(field, "spriteIndex"))
			{
				spriteIndex = GetClampedInteger(-1, 0, (-objects[ParticleFactory::partGroups[groupIndex].spriteSlot].nmeshes) - 1);
				return;
			}
			break;
		}
	}
	BaseParticle::NewIndex(field);
}

const char* MeshParticle::Name()
{
	return "MeshParticle";
}

void MeshParticle::Index(const char* field)
{
	if (field)
	{
		switch (field[0])
		{
		case 'm':
			if (!strcmp(field, "mesh"))
			{
				Script::PushInteger(mesh);
				return;
			}
			break;
		case 'o':
			if (!strcmp(field, "object"))
			{
				Script::PushInteger(object);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "rot"))
			{
				Script::PushData(&rot);
				return;
			}
			if (!strcmp(field, "rotVel"))
			{
				Script::PushData(&rotVel);
				return;
			}
			break;
		case 's':
			if (!strcmp(field, "scale"))
			{
				Script::PushData(&scale);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "transparency"))
			{
				Script::PushInteger(transparency);
				return;
			}
			if (!strcmp(field, "tint"))
			{
				Script::PushData(&tint);
				return;
			}
			break;
		}
	}
	BaseParticle::Index(field);
}

void MeshParticle::NewIndex(const char* field)
{
	CheckFieldCaller(FUNCTION_INIT | FUNCTION_UPDATE, field);
	if (field)
	{
		switch (field[0])
		{
		case 'm':
			if (!strcmp(field, "mesh"))
			{
				mesh = GetClampedInteger(-1, 0, objects[object].nmeshes - 1);
				return;
			}
			break;
		case 'o':
			if (!strcmp(field, "object"))
			{
				object = GetClampedInteger(-1, 0, SLOT_NUMBER_OBJECTS - 1);
				mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "transparency"))
			{
				transparency = GetClampedInteger(-1, 0, 255);
				return;
			}
			break;
		}
	}
	BaseParticle::NewIndex(field);
}

int AbsFunction::Call()
{
	Script::PushNumber(GetMathResult(1, fabsf));
	return 1;
}

int AcosFunction::Call()
{
	Script::PushNumber(GetMathResult(1, acosf));
	return 1;
}

int AsinFunction::Call()
{
	Script::PushNumber(GetMathResult(1, asinf));
	return 1;
}

int AtanFunction::Call()
{
	Script::PushNumber(GetMathResult(1, atanf));
	return 1;
}

int Atan2Function::Call()
{
	Script::PushNumber(GetMathResult(1, 2, atan2f));
	return 1;
}

int BoidAlignmentFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float radius = GetNumber(2);
	float factor = GetNumber(3);
	part->vel += part->BoidAlignmentRule(radius, factor);
	return 0;
}

int BoidCohesionFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float radius = GetNumber(2);
	float factor = GetNumber(3);
	part->vel += part->BoidCohesionRule(radius, factor);
	return 0;
}

int BoidSeparationFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float radius = GetNumber(2);
	float factor = GetNumber(3);
	part->vel += part->BoidSeparationRule(radius, factor);
	return 0;
}

int CbrtFunction::Call()
{
	Script::PushNumber(GetMathResult(1, cbrtf));
	return 1;
}

int CosFunction::Call()
{
	Script::PushNumber(GetMathResult(1, cosf));
	return 1;
}

int CreateGroupFunction::Call()
{
	CheckCaller(FUNCTION_LIBRARY, "createGroup");
	int init = GetFunction(1);
	int update = GetFunction(2);
	int i = ParticleFactory::GetFreeParticleGroup();
	if (i == -1)
		return 0;
	ParticleFactory::partGroups[i].initIndex = init;
	ParticleFactory::partGroups[i].updateIndex = update;
	Script::PushData(&ParticleFactory::partGroups[i]);
	return 1;
}

int CreateMeshPartFunction::Call()
{
	CheckCaller(FUNCTION_INIT | FUNCTION_UPDATE, "createMeshPart");
	auto group = GetData<ParticleGroup>(1);
	int i = ParticleFactory::GetFreeMeshPart();
	ParticleFactory::meshParts[i].groupIndex = group->groupIndex;
	Tr4ItemInfo* item = &ParticleFactory::meshParts[i].item;
	item->il.fcnt = -1;
	item->il.room_number = -1;
	item->il.RoomChange = 0;
	item->il.nCurrentLights = 0;
	item->il.nPrevLights = 0;
	item->il.ambient = -1;
	item->il.pCurrentLights = item->il.CurrentLights;
	item->il.pPrevLights = item->il.PrevLights;
	Script::PushData(&ParticleFactory::meshParts[i]);
	return 1;
}

int CreateSpritePartFunction::Call()
{
	CheckCaller(FUNCTION_INIT | FUNCTION_UPDATE, "createSpritePart");
	auto group = GetData<ParticleGroup>(1);
	int i = ParticleFactory::GetFreeSpritePart();
	ParticleFactory::spriteParts[i].groupIndex = group->groupIndex;
	Script::PushData(&ParticleFactory::spriteParts[i]);
	return 1;
}

int GetTombIndexFunction::Call()
{
	Script::PushInteger(GetTombIndexByNGLEIndex(1));
	return 1;
}

int GetLaraIndexFunction::Call()
{
	Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
	return 1;
}

int GetItemRoomFunction::Call()
{
	auto item = GetItem(1);
	Script::PushInteger(item ? item->room_number : -1);
	return 1;
}

int MeshAlignVelocityFunction::Call()
{
	auto part = GetData<MeshParticle>(1);
	float factor = GetClampedNumber(2, 0.0f, 1.0f);
	bool invert = GetBoolean(3);
	part->AlignToVel(factor, invert);
	return 0;
}

int MeshShatterFunction::Call()
{
	auto part = GetData<MeshParticle>(1);
	part->Shatter();
	return 0;
}

int NoisePerlinFunction::Call()
{
	float scale, x, y, z, w;

	int count = GetArgCount(2, 5);

	switch (count)
	{
	case 2:
		scale = GetNumber(1);
		x = GetNumber(2);
		if (scale)
			x /= scale;
		Script::PushNumber(ParticleFactory::noise.PerlinNoise1D(x));
		return 1;

	case 3:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.PerlinNoise2D(x, y));
		return 1;

	case 4:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		z = GetNumber(4);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.PerlinNoise3D(x, y, z));
		return 1;

	default:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		z = GetNumber(4);
		w = GetNumber(5);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.PerlinNoise4D(x, y, z, w));
		return 1;
	}
}

int NoiseSimplexFunction::Call()
{
	float scale, x, y, z, w;

	int count = GetArgCount(2, 5);

	switch (count)
	{
	case 2:
		scale = GetNumber(1);
		x = GetNumber(2);
		if (scale)
			x /= scale;
		Script::PushNumber(ParticleFactory::noise.SimplexNoise1D(x));
		return 1;

	case 3:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.SimplexNoise2D(x, y));
		return 1;

	case 4:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		z = GetNumber(4);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.SimplexNoise3D(x, y, z));
		return 1;

	default:
		scale = GetNumber(1);
		x = GetNumber(2);
		y = GetNumber(3);
		z = GetNumber(4);
		w = GetNumber(5);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;
		}
		Script::PushNumber(ParticleFactory::noise.SimplexNoise4D(x, y, z, w));
		return 1;
	}
}

int ParticleAnimateFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	int start = GetInteger(2);
	int end = GetInteger(3);
	int framerate = GetInteger(4);
	part->Animate(start, end, framerate);
	return 0;
}

int ParticleCollidedItemFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = GetItem(2);
	int radius = GetInteger(3);

	Script::PushBoolean(item ? part->CollidedWithItem(item, radius) : false);
	return 1;
}

int ParticleCollideFloorsFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float rebound = GetClampedNumber(2, 0.0f, 1.0f);
	float minbounce = GetNumber(3);
	int margin = GetInteger(4);
	bool accurate = GetBoolean(5);
	Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
	return 1;
}

int ParticleCollideWallsFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float rebound = GetClampedNumber(2, 0.0f, 1.0f);
	Script::PushBoolean(part->CollideWalls(rebound));
	return 1;
}

int ParticleHomingFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = GetItem(2);
	int node = item ? GetClampedInteger(3, 0, objects[item->object_number].nmeshes) : GetInteger(3);
	float factor = GetNumber(4);
	float accel = GetNumber(5);
	bool predict = GetBoolean(6);

	if (item)
		part->TargetHoming(item, node, factor, accel, predict);
	return 0;
}

int ParticleLimitSpeedFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float speedMax = GetNumber(2);
	part->LimitSpeed(speedMax);
	return 0;
}

int PrintFunction::Call()
{
	Script::Print();
	return 0;
}

int RandfloatFunction::Call()
{
	float lower = GetNumber(1);
	float upper = GetNumber(2);
	Script::PushNumber(fabsf(upper - lower) * GetRandom() + fminf(lower, upper));
	return 1;
}

int RandintFunction::Call()
{
	float lower = GetInteger(1);
	float upper = GetInteger(2);
	Script::PushInteger(floorf((fabsf(upper - lower) + 1) * GetRandom() + fminf(lower, upper)));
	return 1;
}

int SeedNoiseFunction::Call()
{
	int seed = GetInteger(1);
	ParticleFactory::noise.SeedPermut(seed);
	return 0;
}

int SinFunction::Call()
{
	Script::PushNumber(GetMathResult(1, sinf));
	return 1;
}

int SqrtFunction::Call()
{
	Script::PushNumber(GetMathResult(1, sqrtf));
	return 1;
}

void LuaBridge::GlobalIndex(const char* field)
{
	LuaObject* object;

	if (field)
	{
		object = LuaGlobals::RetrieveFunction(field);
		if (object)
		{
			Script::PushData(object);
			return;
		}
		Script::ThrowError("attempt to read from a global variable");
	}
	Script::ThrowError("attempt to index the global environment");
}

void LuaBridge::GlobalNewIndex(const char* field)
{
	LuaObject* object;

	if (field)
	{
		object = LuaGlobals::RetrieveFunction(field);
		if (object)
			Script::ThrowError("attempt to assign to a built-in function");
		Script::ThrowError("attempt to write to a global variable");
	}
	Script::ThrowError("attempt to index the global environment");
}

int LuaBridge::GlobalCall()
{
	Script::ThrowError("attempt to call the global environment");
}
