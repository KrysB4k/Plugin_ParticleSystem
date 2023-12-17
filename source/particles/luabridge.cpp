#include "..\definitions\includes.h"

namespace
{
	template<class T>
	T* GetData(int argument)
	{
		if (!Script::IsData(argument))
			Script::ThrowError(FormatString("%s expected", T::Name()));
		T* object = dynamic_cast<T*>((LuaObject*)Script::ToData(argument));
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
		int index = GetInteger(argument);
		if (index >= 0 && index < level_items)
			return &items[index];
		Script::EmitWarning(FormatString("%d does not correspond to a valid Tomb index", index));
		return nullptr;
	}

	int GetTombIndex(int argument)
	{
		int ngindex = GetInteger(argument);
		if (ngindex >= 0 && ngindex < 6000)
		{
			int index = FromNgleIndexToTomb4Index(ngindex);
			if (index != -1)
				return index;
		}
		Script::EmitWarning(FormatString("%d does not correspond to a valid NGLE index", ngindex));
		return -1;
	}

	float GetMathResult(int argument, float (*operation)(float))
	{
		float x = GetNumber(argument);
		float result = operation(x);
		if (!isnan(x) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	float GetMathResult(int firstArgument, int secondArgument, float (*operation)(float, float))
	{
		float x = GetNumber(1);
		float y = GetNumber(2);
		float result = operation(x, y);
		if (!isnan(x) && !isnan(y) && isnan(result))
			Script::EmitWarning("the operation resulted in a NaN");
		return result;
	}

	int GetClampedInteger(int argument, int min, int max)
	{
		int x = GetInteger(argument);
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
		float x = GetNumber(argument);
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
}

const char* ColorRGB::Name()
{
	return "ColorRGB";
}

int ColorRGB::Index(const char* field)
{
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "b"))
		{
			Script::PushNumber((float)B / 255);
			return 1;
		}
		break;
	case 'g':
		if (!strcmp(field, "g"))
		{
			Script::PushNumber((float)G / 255);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "r"))
		{
			Script::PushNumber((float)R / 255);
			return 1;
		}
		break;
	}
	return 0;
}

void ColorRGB::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "b"))
			B = 255 * GetClampedNumber(1, 0.0f, 1.0f);
		break;
	case 'g':
		if (!strcmp(field, "g"))
			G = 255 * GetClampedNumber(1, 0.0f, 1.0f);
		break;
	case 'r':
		if (!strcmp(field, "r"))
			R = 255 * GetClampedNumber(1, 0.0f, 1.0f);
		break;
	}
}

const char* Vector3f::Name()
{
	return "Vector3f";
}

int Vector3f::Index(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
		{
			Script::PushNumber(x);
			return 1;
		}
		break;
	case 'y':
		if (!strcmp(field, "y"))
		{
			Script::PushNumber(y);
			return 1;
		}
		break;
	case 'z':
		if (!strcmp(field, "z"))
		{
			Script::PushNumber(z);
			return 1;
		}
		break;
	}
	return 0;
}

void Vector3f::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
			x = GetNumber(1);
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = GetNumber(1);
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = GetNumber(1);
		break;
	}
}

const char* Vector3s::Name()
{
	return "Vector3s";
}

int Vector3s::Index(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
		{
			Script::PushNumber(ShortToRad(x));
			return 1;
		}
		break;
	case 'y':
		if (!strcmp(field, "y"))
		{
			Script::PushNumber(ShortToRad(y));
			return 1;
		}
		break;
	case 'z':
		if (!strcmp(field, "z"))
		{
			Script::PushNumber(ShortToRad(z));
			return 1;
		}
		break;
	}
	return 0;
}

void Vector3s::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
			x = RadToShort(GetNumber(1));
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = RadToShort(GetNumber(1));
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = RadToShort(GetNumber(1));
		break;
	}
}

const char* Vector3i::Name()
{
	return "Vector3i";
}

int Vector3i::Index(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
		{
			Script::PushNumber((float)x / 16384);
			return 1;
		}
		break;
	case 'y':
		if (!strcmp(field, "y"))
		{
			Script::PushNumber((float)y / 16384);
			return 1;
		}
		break;
	case 'z':
		if (!strcmp(field, "z"))
		{
			Script::PushNumber((float)z / 16384);
			return 1;
		}
		break;
	}
	return 0;
}

void Vector3i::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'x':
		if (!strcmp(field, "x"))
			x = 16384 * GetClampedNumber(1, 0.0f, 65536.0f);
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = 16384 * GetClampedNumber(1, 0.0f, 65536.0f);
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = 16384 * GetClampedNumber(1, 0.0f, 65536.0f);
		break;
	}
}

const char* NodeAttachment::Name()
{
	return "NodeAttachment";
}

int NodeAttachment::Index(const char* field)
{
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "cutoff"))
		{
			Script::PushInteger(cutoff);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "random"))
		{
			Script::PushInteger(random);
			return 1;
		}
		break;
	case 't':
		if (!strcmp(field, "tether"))
		{
			Script::PushInteger(tether);
			return 1;
		}
		break;
	}

	return 0;
}

void NodeAttachment::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "cutoff"))
			cutoff = GetClampedInteger(1, 0, 32767);
		break;
	case 'r':
		if (!strcmp(field, "random"))
			random = GetClampedInteger(1, 0, 32767);
		break;
	case 't':
		if (!strcmp(field, "tether"))
			tether = static_cast<TetherType>(GetClampedInteger(1, 0, 2));
		break;
	}
}

const char* ParticleGroup::Name()
{
	return "ParticleGroup";
}

int ParticleGroup::Index(const char* field)
{
	switch (field[0])
	{
	case 'a':
		if (!strcmp(field, "attach"))
		{
			Script::PushData(&attach);
			return 1;
		}
		break;
	case 'b':
		if (!strcmp(field, "blendingMode"))
		{
			Script::PushInteger(blendingMode);
			return 1;
		}
		break;
	case 'd':
		if (!strcmp(field, "drawMode"))
		{
			Script::PushInteger(drawMode);
			return 1;
		}
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
		{
			Script::PushBoolean(LineIgnoreVel);
			return 1;
		}
		break;
	case 's':
		if (!strcmp(field, "saved"))
		{
			Script::PushBoolean(Saved);
			return 1;
		}
		if (!strcmp(field, "screenSpace"))
		{
			Script::PushBoolean(ScreenSpace);
			return 1;
		}
		if (!strcmp(field, "spriteSlot"))
		{
			Script::PushInteger(spriteSlot);
			return 1;
		}
		break;
	}
	return 0;
}

void ParticleGroup::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_LIBRARY)
		return;
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "blendingMode"))
		{
			blendingMode = GetClampedInteger(1, 0, 13);
			return;
		}
		break;
	case 'd':
		if (!strcmp(field, "drawMode"))
		{
			drawMode = static_cast<DrawMode>(GetClampedInteger(1, 0, 3));
			return;
		}
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
		{
			LineIgnoreVel = GetBoolean(1);
			return;
		}
		break;
	case 's':
		if (!strcmp(field, "saved"))
		{
			Saved = GetBoolean(1);
			return;
		}
		if (!strcmp(field, "screenSpace"))
		{
			ScreenSpace = GetBoolean(1);
			return;
		}
		if (!strcmp(field, "spriteSlot"))
		{
			spriteSlot = GetConstrainedInteger(1, SLOT_DEFAULT_SPRITES, 3, SLOT_DEFAULT_SPRITES, SLOT_MISC_SPRITES, SLOT_CUSTOM_SPRITES);
			return;
		}
		break;
	}
}

const char* BaseParticle::Name()
{
	return "BaseParticle";
}

int BaseParticle::Index(const char* field)
{
	switch (field[0])
	{
	case 'a':
		if (!strcmp(field, "accel"))
		{
			Script::PushData(&accel);
			return 1;
		}
		break;
	case 'e':
		if (!strcmp(field, "emitterIndex"))
		{
			Script::PushInteger(emitterIndex);
			return 1;
		}
		if (!strcmp(field, "emitterNode"))
		{
			Script::PushInteger(emitterNode);
			return 1;
		}
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
		{
			Script::PushInteger(lifeCounter);
			return 1;
		}
		if (!strcmp(field, "lifeSpan"))
		{
			Script::PushInteger(lifeSpan);
			return 1;
		}
		break;
	case 'p':
		if (!strcmp(field, "pos"))
		{
			Script::PushData(&pos);
			return 1;
		}
	case 'r':
		if (!strcmp(field, "roomIndex"))
		{
			Script::PushInteger(roomIndex);
			return 1;
		}
		break;
	case 't':
		if (!strcmp(field, "t"))
		{
			Script::PushNumber(Parameter());
			return 1;
		}
		break;
	case 'v':
		if (!strcmp(field, "vel"))
		{
			Script::PushData(&vel);
			return 1;
		}
		break;
	}
	return 0;
}

void BaseParticle::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
		return;
	switch (field[0])
	{
	case 'e':
		if (!strcmp(field, "emitterIndex"))
		{
			emitterIndex = GetClampedInteger(1, -1, level_items - 1);
			if (emitterIndex != -1)
				emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			else
				emitterNode = -1;
			return;
		}
		if (!strcmp(field, "emitterNode"))
		{
			if (emitterIndex != -1)
				emitterNode = GetClampedInteger(1, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			return;
		}
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
		{
			lifeCounter = GetClampedInteger(1, 0, lifeSpan);
			return;
		}
		if (!strcmp(field, "lifeSpan"))
		{
			// set lifeCounter to lifeSpan automatically
			lifeCounter = lifeSpan = GetClampedInteger(1, 0, 32767);
			return;
		}
		break;
	case 'r':
		if (!strcmp(field, "roomIndex"))
		{
			roomIndex = GetClampedInteger(1, 0, number_rooms - 1);
			return;
		}
		break;
	}
}

const char* SpriteParticle::Name()
{
	return "SpriteParticle";
}

int SpriteParticle::Index(const char* field)
{
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "colCust"))
		{
			Script::PushData(&colCust);
			return 1;
		}
		if (!strcmp(field, "colEnd"))
		{
			Script::PushData(&colEnd);
			return 1;
		}
		if (!strcmp(field, "colStart"))
		{
			Script::PushData(&colStart);
			return 1;
		}
		if (!strcmp(field, "colorFadeTime"))
		{
			Script::PushInteger(colorFadeTime);
			return 1;
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
		{
			Script::PushInteger(fadeIn);
			return 1;
		}
		if (!strcmp(field, "fadeOut"))
		{
			Script::PushInteger(fadeOut);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "rot"))
		{
			Script::PushNumber(ShortToRad(rot));
			return 1;
		}
		if (!strcmp(field, "rotVel"))
		{
			Script::PushNumber(ShortToRad(rotVel));
			return 1;
		}
		break;
	case 's':
		if (!strcmp(field, "sizeCust"))
		{
			Script::PushInteger(sizeCust);
			return 1;
		}
		if (!strcmp(field, "sizeEnd"))
		{
			Script::PushInteger(sizeEnd);
			return 1;
		}
		if (!strcmp(field, "sizeStart"))
		{
			Script::PushInteger(sizeStart);
			return 1;
		}
		if (!strcmp(field, "sizeRatio"))
		{
			Script::PushNumber(sizeRatio / 32768.0f);
			return 1;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			Script::PushInteger(spriteIndex);
			return 1;
		}
		break;
	}
	return BaseParticle::Index(field);
}

void SpriteParticle::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
		return;
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "colorFadeTime"))
		{
			colorFadeTime = GetClampedInteger(1, -32768, 32767);
			return;
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
		{
			fadeIn = GetClampedInteger(1, 0, 32767);
			return;
		}
		if (!strcmp(field, "fadeOut"))
		{
			fadeOut = GetClampedInteger(1, 0, 32767);
			return;
		}
		break;
	case 'r':
		if (!strcmp(field, "rot"))
		{
			rot = RadToShort(GetNumber(1));
			return;
		}
		if (!strcmp(field, "rotVel"))
		{
			rotVel = RadToShort(GetNumber(1));
			return;
		}
		break;
	case 's':
		if (!strcmp(field, "sizeCust"))
		{
			sizeCust = GetClampedInteger(1, 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeEnd"))
		{
			sizeEnd = GetClampedInteger(1, 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeStart"))
		{
			sizeStart = GetClampedInteger(1, 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeRatio"))
		{
			sizeRatio = 32767 * GetClampedNumber(1, -1.0f, 1.0f);
			return;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			spriteIndex = GetClampedInteger(1, 0, (-objects[ParticleFactory::partGroups[groupIndex].spriteSlot].nmeshes) - 1);
			return;
		}
		break;
	}
	BaseParticle::NewIndex(field);
}

const char* MeshParticle::Name()
{
	return "MeshParticle";
}

int MeshParticle::Index(const char* field)
{
	switch (field[0])
	{
	case 'm':
		if (!strcmp(field, "mesh"))
		{
			Script::PushInteger(mesh);
			return 1;
		}
		break;
	case 'o':
		if (!strcmp(field, "object"))
		{
			Script::PushInteger(object);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "rot"))
		{
			Script::PushData(&rot);
			return 1;
		}
		if (!strcmp(field, "rotVel"))
		{
			Script::PushData(&rotVel);
			return 1;
		}
		break;
	case 's':
		if (!strcmp(field, "scale"))
		{
			Script::PushData(&scale);
			return 1;
		}
		break;
	case 't':
		if (!strcmp(field, "transparency"))
		{
			Script::PushInteger(transparency);
			return 1;
		}
		if (!strcmp(field, "tint"))
		{
			Script::PushData(&tint);
			return 1;
		}
		break;
	}
	return BaseParticle::Index(field);
}

void MeshParticle::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
		return;
	switch (field[0])
	{
	case 'm':
		if (!strcmp(field, "mesh"))
		{
			mesh = GetClampedInteger(1, 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 'o':
		if (!strcmp(field, "object"))
		{
			object = GetClampedInteger(1, 0, SLOT_NUMBER_OBJECTS - 1);
			mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 't':
		if (!strcmp(field, "transparency"))
		{
			transparency = GetClampedInteger(1, 0, 255);
			return;
		}
		break;
	}
	BaseParticle::NewIndex(field);
}

int LuaBridge::Call(const char* function)
{
	int i, init, update;
	ParticleGroup* group;
	float lower, upper;

	switch (function[0])
	{
	case 'a':
		if (!strcmp(function, "abs"))
		{
			Script::PushNumber(GetMathResult(1, fabsf));
			return 1;
		}
		if (!strcmp(function, "acos"))
		{
			Script::PushNumber(GetMathResult(1, acosf));
			return 1;
		}
		if (!strcmp(function, "asin"))
		{
			Script::PushNumber(GetMathResult(1, asinf));
			return 1;
		}
		if (!strcmp(function, "atan"))
		{
			Script::PushNumber(GetMathResult(1, atanf));
			return 1;
		}
		if (!strcmp(function, "atan2"))
		{
			Script::PushNumber(GetMathResult(1, 2, atan2f));
			return 1;
		}
		break;
	case 'b':
		if (!strcmp(function, "boidAlignment"))
		{
			auto part = GetData<BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidAlignmentRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidCohesion"))
		{
			auto part = GetData<BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidCohesionRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidSeparation"))
		{
			auto part = GetData<BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidSeparationRule(radius, factor);
			return 0;
		}
		break;
	case 'c':
		if (!strcmp(function, "cbrt"))
		{
			Script::PushNumber(GetMathResult(1, cbrtf));
			return 1;
		}
		if (!strcmp(function, "cos"))
		{
			Script::PushNumber(GetMathResult(1, cosf));
			return 1;
		}
		if (!strcmp(function, "createGroup"))
		{
			if (ParticleFactory::caller != FUNCTION_LIBRARY)
				return 0;
			init = GetFunction(1);
			update = GetFunction(2);
			i = ParticleFactory::GetFreeParticleGroup();
			if (i == -1)
				return 0;
			ParticleFactory::partGroups[i].initIndex = init;
			ParticleFactory::partGroups[i].updateIndex = update;
			Script::PushData(&ParticleFactory::partGroups[i]);
			return 1;
		}
		if (!strcmp(function, "createMeshPart"))
		{
			if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
				return 0;
			group = GetData<ParticleGroup>(1);
			i = ParticleFactory::GetFreeMeshPart();
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
		if (!strcmp(function, "createSpritePart"))
		{
			if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
				return 0;
			group = GetData<ParticleGroup>(1);
			i = ParticleFactory::GetFreeSpritePart();
			ParticleFactory::spriteParts[i].groupIndex = group->groupIndex;
			Script::PushData(&ParticleFactory::spriteParts[i]);
			return 1;
		}
		break;
	case 'g':
		if (!strcmp(function, "getTombIndex"))
		{
			Script::PushInteger(GetTombIndex(1));
			return 1;
		}
		if (!strcmp(function, "getLaraIndex"))
		{
			Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
			return 1;
		}
		if (!strcmp(function, "getItemRoom"))
		{
			auto item = GetItem(1);
			Script::PushInteger(item ? item->room_number : -1);
			return 1;
		}
		break;
	case 'm':
		if (!strcmp(function, "meshAlignVelocity"))
		{
			auto part = GetData<MeshParticle>(1);
			float factor = GetClampedNumber(2, 0.0f, 1.0f);
			bool invert = GetBoolean(3);
			part->AlignToVel(factor, invert);
			return 0;
		}
		if (!strcmp(function, "meshShatter"))
		{
			auto part = GetData<MeshParticle>(1);
			part->Shatter();
			return 0;
		}
		break;
	case 'n':
		if (!strcmp(function, "noisePerlin"))
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
		if (!strcmp(function, "noiseSimplex"))
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
		break;
	case 'p':
		if (!strcmp(function, "particleAnimate"))
		{
			auto part = GetData<BaseParticle>(1);
			int start = GetInteger(2);
			int end = GetInteger(3);
			int framerate = GetInteger(4);
			part->Animate(start, end, framerate);
			return 0;
		}
		if (!strcmp(function, "particleCollidedItem"))
		{
			auto part = GetData<BaseParticle>(1);
			auto item = GetItem(2);
			int radius = GetInteger(3);

			Script::PushBoolean(item ? part->CollidedWithItem(item, radius) : false);
			return 1;
		}
		if (!strcmp(function, "particleCollideFloors"))
		{
			auto part = GetData<BaseParticle>(1);
			float rebound = GetClampedNumber(2, 0.0f, 1.0f);
			float minbounce = GetNumber(3);
			int margin = GetInteger(4);
			bool accurate = GetBoolean(5);
			Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
			return 1;
		}
		if (!strcmp(function, "particleCollideWalls"))
		{
			auto part = GetData<BaseParticle>(1);
			float rebound = GetClampedNumber(2, 0.0f, 1.0f);
			Script::PushBoolean(part->CollideWalls(rebound));
			return 1;
		}
		if (!strcmp(function, "particleHoming"))
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
		if (!strcmp(function, "particleLimitSpeed"))
		{
			auto part = GetData<BaseParticle>(1);
			float speedMax = GetNumber(2);
			part->LimitSpeed(speedMax);
			return 0;
		}
		if (!strcmp(function, "print"))
		{
			Script::Print();
			return 0;
		}
		break;
	case 'r':
		if (!strcmp(function, "randfloat"))
		{
			lower = GetNumber(1);
			upper = GetNumber(2);
			Script::PushNumber(fabsf(upper - lower) * GetRandom() + fminf(lower, upper));
			return 1;
		}
		if (!strcmp(function, "randint"))
		{
			lower = GetInteger(1);
			upper = GetInteger(2);
			Script::PushInteger(floorf((fabsf(upper - lower) + 1) * GetRandom() + fminf(lower, upper)));
			return 1;
		}
		break;
	case 's':
		if (!strcmp(function, "seedNoise"))
		{
			int seed = GetInteger(1);
			ParticleFactory::noise.SeedPermut(seed);
			return 0;
		}
		if (!strcmp(function, "sin"))
		{
			Script::PushNumber(GetMathResult(1, sinf));
			return 1;
		}
		if (!strcmp(function, "sqrt"))
		{
			Script::PushNumber(GetMathResult(1, sqrtf));
			return 1;
		}
		break;
	}
	
		
	return 0;
}
