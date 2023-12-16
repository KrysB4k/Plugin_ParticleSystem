#include "..\definitions\includes.h"

namespace
{
	template<class T>
	T* GetData(int argument)
	{
		if (!Script::IsData(argument))
			Script::ThrowError(FormatString("bad argument %d: %s expected", argument, T::Name()));
		T* object = dynamic_cast<T*>((LuaObject*)Script::ToData(argument));
		if (!object)
			Script::ThrowError(FormatString("bad argument %d: %s expected", argument, T::Name()));
		return object;
	}

	int GetInteger(int argument)
	{
		if (!Script::IsInteger(argument))
			Script::ThrowError(FormatString("bad argument %d: integer expected", argument));
		return Script::ToInteger(argument);
	}

	bool GetBoolean(int argument)
	{
		if (!Script::IsBoolean(argument))
			Script::ThrowError(FormatString("bad argument %d: boolean expected", argument));
		return Script::ToBoolean(argument);
	}

	float GetNumber(int argument)
	{
		if (!Script::IsNumber(argument))
			Script::ThrowError(FormatString("bad argument %d: number expected", argument));
		return Script::ToNumber(argument);
	}

	int GetFunction(int argument)
	{
		if (!Script::IsFunction(argument))
			Script::ThrowError(FormatString("bad argument %d: function expected", argument));
		return Script::StoreFunction(argument);
	}

	int GetArgCount(int minimum, int maximum)
	{
		int count;

		count = Script::ArgCount();
		if (count < minimum)
			Script::ThrowError(FormatString("too few arguments: at least %d arguments expected", minimum));
		if (count > maximum)
			Script::ThrowError(FormatString("too many arguments: at most %d arguments expected", minimum));
		return count;
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
			B = Clamp(int(255 * GetNumber(1)), 0, 255);
		break;
	case 'g':
		if (!strcmp(field, "g"))
			G = Clamp(int(255 * GetNumber(1)), 0, 255);
		break;
	case 'r':
		if (!strcmp(field, "r"))
			R = Clamp(int(255 * GetNumber(1)), 0, 255);
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
			x = Clamp(int(16384 * GetNumber(1)), 0, INT_MAX);
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = Clamp(int(16384 * GetNumber(1)), 0, INT_MAX);
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = Clamp(int(16384 * GetNumber(1)), 0, INT_MAX);
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
			cutoff = Clamp(GetInteger(1), 0, 32767);
		break;
	case 'r':
		if (!strcmp(field, "random"))
			random = Clamp(GetInteger(1), 0, 32767);
		break;
	case 't':
		if (!strcmp(field, "tether"))
			tether = static_cast<TetherType>(Clamp(GetInteger(1), 0, 2));
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
			blendingMode = Clamp(GetInteger(1), 0, 13);
			return;
		}
		break;
	case 'd':
		if (!strcmp(field, "drawMode"))
		{
			drawMode = static_cast<DrawMode>(Clamp(GetInteger(1), 0, 3));
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
			spriteSlot = GetInteger(1);
			if (spriteSlot != SLOT_DEFAULT_SPRITES && spriteSlot != SLOT_MISC_SPRITES && spriteSlot != SLOT_CUSTOM_SPRITES)
				spriteSlot = SLOT_DEFAULT_SPRITES;
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
			emitterIndex = Clamp(GetInteger(1), -1, level_items - 1);
			if (emitterIndex != -1)
				emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			else
				emitterNode = -1;
			return;
		}
		if (!strcmp(field, "emitterNode"))
		{
			if (emitterIndex != -1)
				emitterNode = Clamp(GetInteger(1), -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			return;
		}
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
		{
			lifeCounter = Clamp(GetInteger(1), 0, lifeSpan);
			return;
		}
		if (!strcmp(field, "lifeSpan"))
		{
			// set lifeCounter to lifeSpan automatically
			lifeCounter = lifeSpan = Clamp(GetInteger(1), 0, 32767);
			return;
		}
		break;
	case 'r':
		if (!strcmp(field, "roomIndex"))
		{
			roomIndex = Clamp(GetInteger(1), 0, number_rooms - 1);
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
			colorFadeTime = Clamp(GetInteger(1), -32768, 32767);
			return;
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
		{
			fadeIn = Clamp(GetInteger(1), 0, 32767);
			return;
		}
		if (!strcmp(field, "fadeOut"))
		{
			fadeOut = Clamp(GetInteger(1), 0, 32767);
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
			sizeCust = Clamp(GetInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeEnd"))
		{
			sizeEnd = Clamp(GetInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeStart"))
		{
			sizeStart = Clamp(GetInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeRatio"))
		{
			sizeRatio = Clamp(Round(GetNumber(1) * 32768), -32768, 32767);
			return;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			spriteIndex = Clamp(GetInteger(1), 0, 32767);
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
			mesh = Clamp(GetInteger(1), 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 'o':
		if (!strcmp(field, "object"))
		{
			object = Clamp(GetInteger(1), 0, SLOT_NUMBER_OBJECTS - 1);
			mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 't':
		if (!strcmp(field, "transparency"))
		{
			transparency = Clamp(GetInteger(1), 0, 255);
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
			Script::PushNumber(abs(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "acos"))
		{
			Script::PushNumber(acosf(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "asin"))
		{
			Script::PushNumber(asinf(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "atan"))
		{
			Script::PushNumber(atanf(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "atan2"))
		{
			float y = GetNumber(1);
			float x = GetNumber(2);
			Script::PushNumber(atan2f(y, x));
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
			Script::PushNumber(cbrtf(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "cos"))
		{
			Script::PushNumber(cosf(GetNumber(1)));
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
			int index = GetInteger(1);
			if (index >= 0 && index < 6000)
			{
				index = FromNgleIndexToTomb4Index(index);
				if (index != -1)
				{
					Script::PushInteger(index);
					return 1;
				}
			}
			Script::PushInteger(-1);
			return 1;
		}
		if (!strcmp(function, "getLaraIndex"))
		{
			Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
			return 1;
		}
		if (!strcmp(function, "getItemRoom"))
		{
			int index = GetInteger(1);
			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				Script::PushInteger(item->room_number);
				return 1;
			}
			Script::PushInteger(-1);
			return 1;
		}
		break;
	case 'm':
		if (!strcmp(function, "meshAlignVelocity"))
		{
			auto part = GetData<MeshParticle>(1);
			float factor = GetNumber(2);
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

			case 5:
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
			int index = GetInteger(2);
			int radius = GetInteger(3);

			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				Script::PushBoolean(part->CollidedWithItem(item, radius));
				return 1;
			}
			Script::PushBoolean(false);
			return 1;
		}
		if (!strcmp(function, "particleCollideFloors"))
		{
			auto part = GetData<BaseParticle>(1);
			float rebound = GetNumber(2);
			float minbounce = GetNumber(3);
			int margin = GetInteger(4);
			bool accurate = GetBoolean(5);
			Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
			return 1;
		}
		if (!strcmp(function, "particleCollideWalls"))
		{
			auto part = GetData<BaseParticle>(1);
			float rebound = GetNumber(2);
			Script::PushBoolean(part->CollideWalls(rebound));
			return 1;
		}
		if (!strcmp(function, "particleHoming"))
		{
			auto part = GetData<BaseParticle>(1);
			int index = GetInteger(2);
			int node = GetInteger(3);
			float factor = GetNumber(4);
			float accel = GetNumber(5);
			bool predict = GetBoolean(6);

			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				part->TargetHoming(item, node, factor, accel, predict);
				return 0;
			}
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
			Script::PushNumber(sinf(GetNumber(1)));
			return 1;
		}
		if (!strcmp(function, "sqrt"))
		{
			float x = GetNumber(1);
			if (x < 0)
			{
				Script::PushNumber(NAN);
				return 1;
			}
			Script::PushNumber(sqrtf(x));
			return 1;
		}
		break;
	}
	
		
	return 0;
}
