#include "..\definitions\includes.h"

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
			B = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	case 'g':
		if (!strcmp(field, "g"))
			G = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	case 'r':
		if (!strcmp(field, "r"))
			R = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	}
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
			x = RadToShort(Script::ToNumber(1));
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = RadToShort(Script::ToNumber(1));
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = RadToShort(Script::ToNumber(1));
		break;
	}
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
			x = Clamp(int(16384 * Script::ToNumber(1)), 0, INT_MAX);
		break;
	case 'y':
		if (!strcmp(field, "y"))
			y = Clamp(int(16384 * Script::ToNumber(1)), 0, INT_MAX);
		break;
	case 'z':
		if (!strcmp(field, "z"))
			z = Clamp(int(16384 * Script::ToNumber(1)), 0, INT_MAX);
		break;
	}
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
			cutoff = Clamp(Script::ToInteger(1), 0, 32767);
		break;
	case 'r':
		if (!strcmp(field, "random"))
			random = Clamp(Script::ToInteger(1), 0, 32767);
		break;
	case 't':
		if (!strcmp(field, "tether"))
			tether = static_cast<TetherType>(Clamp(Script::ToInteger(1), 0, 2));
		break;
	}
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
			blendingMode = Clamp(Script::ToInteger(1), 0, 13);
			return;
		}
		break;
	case 'd':
		if (!strcmp(field, "drawMode"))
		{
			drawMode = static_cast<DrawMode>(Clamp(Script::ToInteger(1), 0, 3));
			return;
		}
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
		{
			LineIgnoreVel = Script::ToBoolean(1);
			return;
		}
		break;
	case 's':
		if (!strcmp(field, "saved"))
		{
			Saved = Script::ToBoolean(1);
			return;
		}
		if (!strcmp(field, "screenSpace"))
		{
			ScreenSpace = Script::ToBoolean(1);
			return;
		}
		if (!strcmp(field, "spriteSlot"))
		{
			spriteSlot = Script::ToInteger(1);
			if (spriteSlot != SLOT_DEFAULT_SPRITES && spriteSlot != SLOT_MISC_SPRITES && spriteSlot != SLOT_CUSTOM_SPRITES)
				spriteSlot = SLOT_DEFAULT_SPRITES;
			return;
		}
		break;
	}
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
			emitterIndex = Clamp(Script::ToInteger(1), -1, level_items - 1);
			if (emitterIndex != -1)
				emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			else
				emitterNode = -1;
			return;
		}
		if (!strcmp(field, "emitterNode"))
		{
			if (emitterIndex != -1)
				emitterNode = Clamp(Script::ToInteger(1), -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			return;
		}
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
		{
			lifeCounter = Clamp(Script::ToInteger(1), 0, lifeSpan);
			return;
		}
		if (!strcmp(field, "lifeSpan"))
		{
			// set lifeCounter to lifeSpan automatically
			lifeCounter = lifeSpan = Clamp(Script::ToInteger(1), 0, 32767);
			return;
		}
		break;
	case 'r':
		if (!strcmp(field, "roomIndex"))
		{
			roomIndex = Clamp(Script::ToInteger(1), 0, number_rooms - 1);
			return;
		}
		break;
	}
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
			colorFadeTime = Clamp(Script::ToInteger(1), -32768, 32767);
			return;
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
		{
			fadeIn = Clamp(Script::ToInteger(1), 0, 32767);
			return;
		}
		if (!strcmp(field, "fadeOut"))
		{
			fadeOut = Clamp(Script::ToInteger(1), 0, 32767);
			return;
		}
		break;
	case 'r':
		if (!strcmp(field, "rot"))
		{
			rot = RadToShort(Script::ToNumber(1));
			return;
		}
		if (!strcmp(field, "rotVel"))
		{
			rotVel = RadToShort(Script::ToNumber(1));
			return;
		}
		break;
	case 's':
		if (!strcmp(field, "sizeCust"))
		{
			sizeCust = Clamp(Script::ToInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeEnd"))
		{
			sizeEnd = Clamp(Script::ToInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeStart"))
		{
			sizeStart = Clamp(Script::ToInteger(1), 0, 65535);
			return;
		}
		if (!strcmp(field, "sizeRatio"))
		{
			sizeRatio = Clamp(Round(Script::ToNumber(1) * 32768), -32768, 32767);
			return;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			spriteIndex = Clamp(Script::ToInteger(1), 0, 32767);
			return;
		}
		break;
	}
	BaseParticle::NewIndex(field);
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
			mesh = Clamp(Script::ToInteger(1), 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 'o':
		if (!strcmp(field, "object"))
		{
			object = Clamp(Script::ToInteger(1), 0, SLOT_NUMBER_OBJECTS - 1);
			mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
			return;
		}
		break;
	case 't':
		if (!strcmp(field, "transparency"))
		{
			transparency = Clamp(Script::ToInteger(1), 0, 255);
			return;
		}
		break;
	}
	BaseParticle::NewIndex(field);
}

int PerlinNoise::Index(const char* field)
{
	return 0;
}

void PerlinNoise::NewIndex(const char* field)
{

}

int LuaBridge::Call(const char* function)
{
	int i, init, update;
	ParticleGroup* group;
	PerlinNoise* perlin;
	float lower, upper;

	switch (function[0])
	{
	case 'a':
		if (!strcmp(function, "abs"))
		{
			Script::PushNumber(abs(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "acos"))
		{
			Script::PushNumber(acosf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "asin"))
		{
			Script::PushNumber(asinf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "atan"))
		{
			Script::PushNumber(atanf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "atan2"))
		{
			float y = Script::ToNumber(1);
			float x = Script::ToNumber(2);
			Script::PushNumber(atan2f(y, x));
			return 1;
		}
		break;
	case 'b':
		if (!strcmp(function, "boidAlignment"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidAlignmentRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidCohesion"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidCohesionRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidSeparation"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidSeparationRule(radius, factor);
			return 0;
		}
		break;
	case 'c':
		if (!strcmp(function, "cbrt"))
		{
			Script::PushNumber(cbrtf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "cos"))
		{
			Script::PushNumber(cosf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "createGroup"))
		{
			if (ParticleFactory::caller != FUNCTION_LIBRARY)
				return 0;
			init = Script::StoreFunction(1);
			update = Script::StoreFunction(2);
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
			group = dynamic_cast<ParticleGroup*>((LuaObject*)Script::ToData(1));
			if (!group)
				return Script::TypeError(1, "ParticleGroup");
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
		if (!strcmp(function, "createPerlinNoise"))
		{
			if (ParticleFactory::caller != FUNCTION_LIBRARY)
				return 0;
			i = ParticleFactory::GetFreePerlinNoise();
			if (i == -1)
				return 0;

			if (Script::ArgCount())
			{
				int seed = Script::ToInteger(1);
				ParticleFactory::perlinNoise[i].SeedPermut(seed);
			}
			Script::PushData(&ParticleFactory::perlinNoise[i]);
			return 1;
		}
		if (!strcmp(function, "createSpritePart"))
		{
			if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
				return 0;
			group = dynamic_cast<ParticleGroup*>((LuaObject*)Script::ToData(1));
			if (!group)
				return Script::TypeError(1, "ParticleGroup");
			i = ParticleFactory::GetFreeSpritePart();
			ParticleFactory::spriteParts[i].groupIndex = group->groupIndex;
			Script::PushData(&ParticleFactory::spriteParts[i]);
			return 1;
		}
		break;
	case 'g':
		if (!strcmp(function, "getTombIndex"))
		{
			Script::PushInteger(FromNgleIndexToTomb4Index(Script::ToInteger(1)));
			return 1;
		}
		if (!strcmp(function, "getLaraIndex"))
		{
			Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
			return 1;
		}
		if (!strcmp(function, "getItemRoom"))
		{
			int index = Script::ToInteger(1);
			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				Script::PushInteger(item->room_number);
				return 1;
			}
			return Script::ItemIndexError(1, index);
		}
		break;
	case 'm':
		if (!strcmp(function, "meshAlignVelocity"))
		{
			auto part = dynamic_cast<MeshParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "MeshParticle");
			float factor = Script::ToNumber(2);
			bool invert = false;
			invert = Script::ToBoolean(3);
			part->AlignToVel(factor, invert);
			return 0;
		}
		if (!strcmp(function, "meshShatter"))
		{
			auto part = dynamic_cast<MeshParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "MeshParticle");
			part->Shatter();
			return 0;
		}
		break;
	case 'n':
		if (!strcmp(function, "noise"))
		{
			float scale, x, y, z, w;
			perlin = dynamic_cast<PerlinNoise*>((LuaObject*)Script::ToData(1));
			if (!perlin)
				return Script::TypeError(1, "PerlinNoise");

			int count = Script::ArgCount();
			if (count < 3)
				return Script::ArgCountError(3);

			switch (count)
			{
			case 3:
				scale = Script::ToNumber(2);
				x = Script::ToNumber(3);
				Script::PushNumber(perlin->Noise1D(scale, x));
				return 1;

			case 4:
				scale = Script::ToNumber(2);
				x = Script::ToNumber(3);
				y = Script::ToNumber(4);
				Script::PushNumber(perlin->Noise2D(scale, x, y));
				return 1;

			case 5:
				scale = Script::ToNumber(2);
				x = Script::ToNumber(3);
				y = Script::ToNumber(4);
				z = Script::ToNumber(5);
				Script::PushNumber(perlin->Noise3D(scale, x, y, z));
				return 1;

			default:
				scale = Script::ToNumber(2);
				x = Script::ToNumber(3);
				y = Script::ToNumber(4);
				z = Script::ToNumber(5);
				w = Script::ToNumber(6);
				Script::PushNumber(perlin->Noise4D(scale, x, y, z, w));
				return 1;
			}
		}
		break;
	case 'p':
		if (!strcmp(function, "particleAnimate"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			int start = Script::ToInteger(2);
			int end = Script::ToInteger(3);
			int framerate = Script::ToInteger(4);
			part->Animate(start, end, framerate);
			return 0;
		}
		if (!strcmp(function, "particleCollidedItem"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");

			int index = Script::ToInteger(2);
			int radius = Script::ToInteger(3);

			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				Script::PushBoolean(part->CollidedWithItem(item, radius));
				return 1;
			}
			return Script::ItemIndexError(2, index);
		}
		if (!strcmp(function, "particleCollideFloors"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			float minbounce = Script::ToNumber(3);
			int margin = Script::ToInteger(4);
			bool accurate = Script::ToBoolean(5);
			Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
			return 1;
		}
		if (!strcmp(function, "particleCollideWalls"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			Script::PushBoolean(part->CollideWalls(rebound));
			return 1;
		}
		if (!strcmp(function, "particleHoming"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			int index = Script::ToInteger(2);
			int node = Script::ToInteger(3);
			float factor = Script::ToNumber(4);
			float accel = Script::ToNumber(5);
			bool predict = Script::ToBoolean(6);

			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				part->TargetHoming(item, node, factor, accel, predict);
				return 0;
			}
			return Script::ItemIndexError(2, index);
		}
		if (!strcmp(function, "particleLimitSpeed"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float speedMax = Script::ToNumber(2);
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
			lower = Script::ToNumber(1);
			upper = Script::ToNumber(2);
			Script::PushNumber(fabsf(upper - lower) * GetRandom() + fminf(lower, upper));
			return 1;
		}
		if (!strcmp(function, "randint"))
		{
			lower = Script::ToInteger(1);
			upper = Script::ToInteger(2);
			Script::PushInteger(floorf((fabsf(upper - lower) + 1) * GetRandom() + fminf(lower, upper)));
			return 1;
		}
		break;
	case 's':
		if (!strcmp(function, "sin"))
		{
			Script::PushNumber(sinf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "sqrt"))
		{
			float x = Script::ToNumber(1);
			if (x < 0)
				return Script::ArgError(1, Script::FormatString("received negative value \'%f\'", x));
			Script::PushNumber(sqrtf(x));
			return 1;
		}
		break;
	}
	
		
	return 0;
}
