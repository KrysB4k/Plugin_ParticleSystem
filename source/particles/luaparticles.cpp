#include "../definitions/Tomb4Globals.h"
#include "particle.h"
#include "noise.h"
#include "utilities.h"
#include "logger.h"

using namespace LuaHelpers;
using namespace Utilities;

const char* Vector3s::Name()
{
	return "Vector3s";
}

short Vector3s::GetX()
{
	return x;
}

short Vector3s::GetY()
{
	return y;
}

short Vector3s::GetZ()
{
	return z;
}

void Vector3s::SetX(short x)
{
	this->x = x; 
}

void Vector3s::SetY(short y)
{
	this->y = y;
}

void Vector3s::SetZ(short z)
{
	this->z = z;
}

Vector3s::operator Vector3s()
{
	return *this;
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
				x = SaturateRound<int>(16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false));
				return;
			}
			break;

		case 'y':
			if (!strcmp(field, "y"))
			{
				y = SaturateRound<int>(16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false));
				return;
			}
			break;

		case 'z':
			if (!strcmp(field, "z"))
			{
				z = SaturateRound<int>(16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false));
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
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
				Script::PushInteger(B);
				return;
			}
			break;

		case 'g':
			if (!strcmp(field, "g"))
			{
				Script::PushInteger(G);
				return;
			}
			break;

		case 'r':
			if (!strcmp(field, "r"))
			{
				Script::PushInteger(R);
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
				B = GetClampedInteger(-1, 0, UINT8_MAX, false);
				return;
			}
			break;

		case 'g':
			if (!strcmp(field, "g"))
			{
				G = GetClampedInteger(-1, 0, UINT8_MAX, false);
				return;
			}
			break;

		case 'r':
			if (!strcmp(field, "r"))
			{
				R = GetClampedInteger(-1, 0, UINT8_MAX, false);
				return;
			}
			break;
		}
	}
	LuaObjectClass::NewIndex(field);
}

const char* Noise::Name()
{
	return "PerlinNoise or SimplexNoise";
}

const char* PerlinNoise::Name()
{
	return "PerlinNoise";
}

const char* SimplexNoise::Name()
{
	return "SimplexNoise";
}

namespace Particles
{
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
					cutoff = GetClampedInteger(-1, 0, INT16_MAX, false);
					return;
				}
				break;

			case 'r':
				if (!strcmp(field, "random"))
				{
					random = GetClampedInteger(-1, 0, INT16_MAX, false);
					return;
				}
				break;

			case 't':
				if (!strcmp(field, "tether"))
				{
					tether = static_cast<TetherType>(GetClampedInteger(-1, TetherType::TETHER_ROTATING, TetherType::TETHER_NONE, false));
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
		if (Particles::GetCaller() & FUNCTION_MODULE && field)
		{
			switch (field[0])
			{
			case 'a':
				if (!strcmp(field, "attach"))
				{
					Script::PushData(&attach);
					return;
				}
				if (!strcmp(field, "autoTrigger"))
				{
					Script::PushBoolean(autoTrigger);
					return;
				}
				break;

			case 'b':
				if (!strcmp(field, "blendMode"))
				{
					Script::PushInteger(blendMode);
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

			case 'i':
				if (!strcmp(field, "immortal"))
				{
					Script::PushBoolean(immortal);
					return;
				}
				break;

			case 'l':
				if (!strcmp(field, "lightMode"))
				{
					Script::PushInteger(lightMode);
					return;
				}
				if (!strcmp(field, "lineIgnoreVel"))
				{
					Script::PushBoolean(lineIgnoreVel);
					return;
				}
				break;

			case 's':
				if (!strcmp(field, "saved"))
				{
					Script::PushBoolean(saved);
					return;
				}
				if (!strcmp(field, "screenSpace"))
				{
					Script::PushBoolean(screenSpace);
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
		if (Particles::GetCaller() & FUNCTION_MODULE && field)
		{
			switch (field[0])
			{
			case 'a':
				if (!strcmp(field, "attach"))
				{
					attach = *GetData<NodeAttachment>(-1);
					return;
				}
				if (!strcmp(field, "autoTrigger"))
				{
					autoTrigger = GetBoolean(-1);
					return;
				}
				break;

			case 'b':
				if (!strcmp(field, "blendMode"))
				{
					blendMode = static_cast<BlendMode>(GetClampedInteger(-1, BlendMode::BLEND_TEXTURE, BlendMode::BLEND_CUSTOM_13, false));
					return;
				}
				break;

			case 'd':
				if (!strcmp(field, "drawMode"))
				{
					drawMode = static_cast<DrawMode>(GetClampedInteger(-1, DrawMode::DRAW_SPRITE, DrawMode::DRAW_NONE, false));
					return;
				}
				break;

			case 'i':
				if (!strcmp(field, "immortal"))
				{
					immortal = GetBoolean(-1);
					return;
				}
				break;

			case 'l':
				if (!strcmp(field, "lightMode"))
				{
					lightMode = static_cast<LightMode>(GetClampedInteger(-1, LightMode::LIGHT_STATIC, LightMode::LIGHT_DYNAMIC, false));
					return;
				}
				if (!strcmp(field, "lineIgnoreVel"))
				{
					lineIgnoreVel = GetBoolean(-1);
					return;
				}
				break;

			case 's':
				if (!strcmp(field, "saved"))
				{
					saved = GetBoolean(-1);
					return;
				}
				if (!strcmp(field, "screenSpace"))
				{
					screenSpace = GetBoolean(-1);
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

	const char* ModuleGroups::Name()
	{
		return "Groups";
	}

	void ModuleGroups::Index(const char* field)
	{
		if (field && strcmp(Script::TableValueTypeName(table, field), "nil"))
		{
			Script::PushTableValue(table, field);
			return;
		}
		LuaObjectClass::Index(field);
	}

	void ModuleGroups::NewIndex(const char* field)
	{
		CheckFieldCaller(FUNCTION_MODULE, field);
		if (field)
		{
			GetData<ParticleGroup>(-1);
			Script::AssignTableValue(table, field, -1);
			return;
		}
		LuaObjectClass::NewIndex(field);
	}

	const char* ModuleParameters::Name()
	{
		return "Parameters";
	}

	void ModuleParameters::Index(const char* field)
	{
		if (field && strcmp(Script::TableValueTypeName(table, field), "nil"))
		{
			Script::PushTableValue(table, field);
			return;
		}
		LuaObjectClass::Index(field);
	}

	void ModuleParameters::NewIndex(const char* field)
	{
		if (field)
		{
			if (Particles::GetCaller() & FUNCTION_MODULE)
			{
				CheckModuleParameter(-1);
				Script::AssignTableValue(table, field, -1);
				return;
			}
			const auto parameterType = Script::TableValueTypeName(table, field);
			if (strcmp(parameterType, "nil"))
			{
				const auto valueType = Script::TypeName(-1);
				if (strcmp(parameterType, valueType))
					Script::Throw(FormatString("attempt to assign value of type %s to parameter of type %s", valueType, parameterType));
				Script::AssignTableValue(table, field, -1);
				return;
			}
		}
		LuaObjectClass::NewIndex(field);
	}

	const char* Module::Name()
	{
		return "Module";
	}

	void Module::Index(const char* field)
	{
		if (field)
		{
			switch (field[0])
			{
			case 'g':
				if (!strcmp(field, "groups"))
				{
					Script::PushData(&groups);
					return;
				}
				break;

			case 'p':
				if (!strcmp(field, "params"))
				{
					Script::PushData(&parameters);
					return;
				}
				break;
			}
		}
		LuaObjectClass::Index(field);
	}

	void Module::NewIndex(const char* field)
	{
		if (field)
		{
			switch (field[0])
			{
			case 'g':
				if (!strcmp(field, "groups"))
				{
					ReadOnlyFieldError(field);
				}
				break;

			case 'p':
				if (!strcmp(field, "params"))
				{
					ReadOnlyFieldError(field);
				}
				break;
			}
		}
		LuaObjectClass::NewIndex(field);
	}

	const char* ParticleData::Name()
	{
		return "ParticleData";
	}

	void ParticleData::Index(const char* field)
	{
		if (field)
		{
			Script::PushTableValue(table, field);
			return;
		}
		LuaObjectClass::Index(field);
	}

	void ParticleData::NewIndex(const char* field)
	{
		if (field)
		{
			CheckParticleData(-1);
			Script::AssignTableValue(table, field, -1);
			return;
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

			case 'd':
				if (!strcmp(field, "data"))
				{
					Script::PushData(&data);
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
				break;

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
			case 'a':
				if (!strcmp(field, "accel"))
				{
					accel = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(-1));
					return;
				}
				break;

			case 'd':
				if (!strcmp(field, "data"))
				{
					const auto& partData = *GetData<ParticleData>(-1);
					Script::DeleteTable(data.table);
					data.table = Script::CloneTable(partData.table);
					return;
				}
				break;

			case 'e':
				if (!strcmp(field, "emitterIndex"))
				{
					emitterIndex = GetClampedInteger(-1, -1, level_items - 1, false);
					if (emitterIndex != -1)
						emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
					else
						emitterNode = NO_MESH;
					return;
				}
				if (!strcmp(field, "emitterNode"))
				{
					if (emitterIndex != -1)
						emitterNode = GetClampedInteger(-1, -1, objects[items[emitterIndex].object_number].nmeshes - 1, false);
					return;
				}
				break;

			case 'l':
				if (!strcmp(field, "lifeCounter"))
				{
					lifeCounter = GetClampedInteger(-1, 0, lifeSpan, false);
					return;
				}
				if (!strcmp(field, "lifeSpan"))
				{
					// set lifeCounter to lifeSpan automatically
					lifeCounter = lifeSpan = GetClampedInteger(-1, 0, INT16_MAX, false);
					return;
				}
				break;

			case 'p':
				if (!strcmp(field, "pos"))
				{
					pos = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(-1));
					return;
				}
				break;

			case 'r':
				if (!strcmp(field, "roomIndex"))
				{
					roomIndex = GetClampedInteger(-1, 0, number_rooms - 1, false);
					return;
				}
				break;

			case 't':
				if (!strcmp(field, "t"))
					ReadOnlyFieldError(field);
				break;

			case 'v':
				if (!strcmp(field, "vel"))
				{
					vel = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(-1));
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
				if (!strcmp(field, "colCust"))
				{
					colCust = *GetData<ColorRGB>(-1);
					return;
				}
				if (!strcmp(field, "colEnd"))
				{
					colEnd = *GetData<ColorRGB>(-1);
					return;
				}
				if (!strcmp(field, "colStart"))
				{
					colStart = *GetData<ColorRGB>(-1);
					return;
				}
				if (!strcmp(field, "colorFadeTime"))
				{
					colorFadeTime = GetClampedInteger(-1, INT16_MIN, INT16_MAX, false);
					return;
				}
				break;

			case 'f':
				if (!strcmp(field, "fadeIn"))
				{
					fadeIn = GetClampedInteger(-1, 0, INT16_MAX, false);
					return;
				}
				if (!strcmp(field, "fadeOut"))
				{
					fadeOut = GetClampedInteger(-1, 0, INT16_MAX, false);
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
					sizeCust = GetClampedInteger(-1, 0, UINT16_MAX, false);
					return;
				}
				if (!strcmp(field, "sizeEnd"))
				{
					sizeEnd = GetClampedInteger(-1, 0, UINT16_MAX, false);
					return;
				}
				if (!strcmp(field, "sizeStart"))
				{
					sizeStart = GetClampedInteger(-1, 0, UINT16_MAX, false);
					return;
				}
				if (!strcmp(field, "sizeRatio"))
				{
					sizeRatio = SaturateRound<short>(INT16_MAX * GetClampedNumber(-1, -1.0f, 1.0f, false));
					return;
				}
				if (!strcmp(field, "spriteIndex"))
				{
					spriteIndex = GetClampedInteger(-1, 0, (-objects[Particles::partGroups[groupIndex].spriteSlot].nmeshes) - 1, false);
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
					mesh = GetClampedInteger(-1, 0, objects[object].nmeshes - 1, false);
					return;
				}
				break;

			case 'o':
				if (!strcmp(field, "object"))
				{
					object = GetClampedInteger(-1, 0, SLOT_NUMBER_OBJECTS - 1, false);
					if (!objects[object].loaded)
					{
						Script::EmitFailure(FormatString("slot %d not present in level, defaulting to SLOT_LARA(0)", object), Logger::Warning);
						object = SLOT_LARA;
					}
					mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
					return;
				}
				break;

			case 'r':
				if (!strcmp(field, "rot"))
				{
					rot = static_cast<Vector3s>(*GetData<LuaObjectClassRotation>(-1));
					return;
				}
				if (!strcmp(field, "rotVel"))
				{
					rotVel = static_cast<Vector3s>(*GetData<LuaObjectClassRotation>(-1));
					return;
				}
				break;

			case 's':
				if (!strcmp(field, "scale"))
				{
					scale = *GetData<Vector3i>(-1);
					return;
				}
				break;

			case 't':
				if (!strcmp(field, "transparency"))
				{
					transparency = GetClampedInteger(-1, 0, UINT8_MAX, false);
					return;
				}
				if (!strcmp(field, "tint"))
				{
					tint = *GetData<ColorRGB>(-1);
					return;
				}
				break;
			}
		}
		BaseParticle::NewIndex(field);
	}
}
