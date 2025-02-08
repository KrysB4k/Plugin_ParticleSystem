#include "../definitions/DefTomb4Funct.h"
#include "../definitions/structures_mine.h"
#include "../definitions/Tomb4Globals.h"
#include "../trng/trng.h"
#include "particle.h"
#include "utilities.h"
#include "logger.h"

using namespace LuaHelpers;
using namespace Utilities;

namespace LuaGlobals
{
	TrngVarWrapper TrngVars;
	LuaItemInfoWrapper LuaItemArray[1024];

	const char* LuaItemInfoWrapper::Name()
	{
		return "ItemInfo";
	}

	void LuaItemInfoWrapper::Index(const char* field)
	{
		int itemIndex;
		Tr4ItemInfo* itemptr{ nullptr };

		if (field)
		{
			itemIndex = this - LuaGlobals::LuaItemArray;
			itemptr = &items[itemIndex];

			switch (field[0])
			{
			case 'a':
				if (!strcmp(field, "animNumber"))
				{
					int anim = itemptr->anim_number - objects[itemptr->object_number].anim_index;
					Script::PushInteger(anim);
					return;
				}
				break;
			case 'c':
				if (!strcmp(field, "currentAnimState"))
				{
					Script::PushInteger(itemptr->current_anim_state);
					return;
				}
				break;
			case 'f':
				if (!strcmp(field, "fallSpeed"))
				{
					Script::PushInteger(itemptr->fallspeed);
					return;
				}
				if (!strcmp(field, "floorDistance"))
				{
					Script::PushInteger(itemptr->floor);
					return;
				}
				if (!strcmp(field, "frameNumber"))
				{
					int frame = itemptr->frame_number - anims[itemptr->anim_number].frame_base;
					Script::PushInteger(frame);
					return;
				}
				break;
			case 'g':
				if (!strcmp(field, "goalAnimState"))
				{
					Script::PushInteger(itemptr->goal_anim_state);
					return;
				}
				break;
			case 'h':
				if (!strcmp(field, "hitPoints"))
				{
					Script::PushInteger(itemptr->hit_points);
					return;
				}
				break;
			case 'i':
				if (!strcmp(field, "itemFlag1"))
				{
					Script::PushInteger(itemptr->item_flags[0]);
					return;
				}
				if (!strcmp(field, "itemFlag2"))
				{
					Script::PushInteger(itemptr->item_flags[1]);
					return;
				}
				if (!strcmp(field, "itemFlag3"))
				{
					Script::PushInteger(itemptr->item_flags[2]);
					return;
				}
				if (!strcmp(field, "itemFlag4"))
				{
					Script::PushInteger(itemptr->item_flags[3]);
					return;
				}
				break;
			case 'm':
				if (!strcmp(field, "meshBits"))
				{
					Script::PushInteger(itemptr->mesh_bits);
					return;
				}
				break;
			case 'o':
				if (!strcmp(field, "ocbNumber"))
				{
					Script::PushInteger(itemptr->trigger_flags);
					return;
				}
				break;
			case 'p':
				if (!strcmp(field, "pos"))
				{
					ConstructManagedData<LuaItemInfoPos>(&itemptr->pos);
					return;
				}
				break;
			case 'r':
				if (!strcmp(field, "roomNumber"))
				{
					Script::PushInteger(itemptr->room_number);
					return;
				}
				if (!strcmp(field, "rot"))
				{
					ConstructManagedData<LuaItemInfoRot>(&itemptr->pos);
					return;
				}
				break;
			case 's':
				if (!strcmp(field, "slotNumber"))
				{
					Script::PushInteger(itemptr->object_number);
					return;
				}
				if (!strcmp(field, "speed"))
				{
					Script::PushInteger(itemptr->speed);
					return;
				}
				break;
			case 't':
				if (!strcmp(field, "triggered"))
				{
					Script::PushBoolean(TriggerActive((StrItemTr4*)itemptr));
					return;
				}
				break;
			}
		}
		LuaObjectClass::Index(field);
	}

	void LuaItemInfoWrapper::NewIndex(const char* field)
	{
		int itemIndex;
		Tr4ItemInfo* itemptr{ nullptr };

		if (field)
		{
			itemIndex = this - LuaGlobals::LuaItemArray;
			itemptr = &items[itemIndex];

			switch (field[0])
			{
			case 'a':
				if (!strcmp(field, "animNumber"))
				{
					int maxAnim = objects[itemptr->object_number + 1].anim_index - 1;
					int anim = GetClampedInteger(-1, 0, maxAnim, false) + objects[itemptr->object_number].anim_index;
					itemptr->anim_number = anim;
					itemptr->frame_number = anims[itemptr->anim_number].frame_base;
					return;
				}
				break;
			case 'c':
				if (!strcmp(field, "currentAnimState"))
				{
					itemptr->current_anim_state = GetClampedInteger(-1, 0, 32767, false);
					return;
				}
				break;
			case 'f':
				if (!strcmp(field, "fallSpeed"))
				{
					itemptr->fallspeed = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				if (!strcmp(field, "floorDistance"))
				{
					ReadOnlyFieldError(field);
				}
				if (!strcmp(field, "frameNumber"))
				{
					int maxFrames = anims[itemptr->anim_number].frame_end - anims[itemptr->anim_number].frame_base;
					int frame = GetClampedInteger(-1, 0, maxFrames - 1, false);
					itemptr->frame_number = anims[itemptr->anim_number].frame_base + frame;
					return;
				}
				break;
			case 'g':
				if (!strcmp(field, "goalAnimState"))
				{
					itemptr->goal_anim_state = GetClampedInteger(-1, 0, 32767, false);
					return;
				}
				break;
			case 'h':
				if (!strcmp(field, "hitPoints"))
				{
					itemptr->hit_points = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				break;
			case 'i':
				if (!strcmp(field, "itemFlag1"))
				{
					itemptr->item_flags[0] = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				if (!strcmp(field, "itemFlag2"))
				{
					itemptr->item_flags[1] = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				if (!strcmp(field, "itemFlag3"))
				{
					itemptr->item_flags[2] = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				if (!strcmp(field, "itemFlag4"))
				{
					itemptr->item_flags[3] = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				break;
			case 'm':
				if (!strcmp(field, "meshBits"))
				{
					ReadOnlyFieldError(field);
				}
				break;
			case 'o':
				if (!strcmp(field, "ocbNumber"))
				{
					itemptr->trigger_flags = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				break;
			case 'p':
				if (!strcmp(field, "pos"))
				{
					auto position = GetData<LuaObjectClassPosition>(-1);
					itemptr->pos.xPos = lroundf(position->GetX());
					itemptr->pos.yPos = lroundf(position->GetY());
					itemptr->pos.zPos = lroundf(position->GetZ());
					return;
				}
				break;
			case 'r':
				if (!strcmp(field, "roomNumber"))
				{
					ReadOnlyFieldError(field);
				}
				if (!strcmp(field, "rot"))
				{
					auto rotation = GetData<LuaObjectClassRotation>(-1);
					itemptr->pos.xRot = rotation->GetX();
					itemptr->pos.yRot = rotation->GetY();
					itemptr->pos.zRot = rotation->GetZ();
					return;
				}
				break;
			case 's':
				if (!strcmp(field, "speed"))
				{
					itemptr->speed = GetClampedInteger(-1, -32768, 32767, false);
					return;
				}
				break;
			case 't':
				if (!strcmp(field, "triggered"))
				{
					ReadOnlyFieldError(field);
				}
				break;
			}
		}
		LuaObjectClass::NewIndex(field);
	}

	LuaObject* RetrieveGlobals(const char* field)
	{
		if (!strcmp(field, "Lara"))
			return &LuaGlobals::LuaItemArray[lara_info.item_number];
		if (!strcmp(field, "Vars"))
			return &TrngVars;

		return nullptr;
	}

	std::optional<int> RetrieveIntegerConstant(const char* field)
	{
		switch (field[0])
		{
		case 'B':
			if (!strcmp(field, "BLEND_TEXTURE"))
				return std::optional(BlendMode::BLEND_TEXTURE);
			if (!strcmp(field, "BLEND_DECAL"))
				return std::optional(BlendMode::BLEND_DECAL);
			if (!strcmp(field, "BLEND_COLORADD"))
				return std::optional(BlendMode::BLEND_COLORADD);
			if (!strcmp(field, "BLEND_SEMITRANS"))
				return std::optional(BlendMode::BLEND_SEMITRANS);
			if (!strcmp(field, "BLEND_NOZBUFFER"))
				return std::optional(BlendMode::BLEND_NOZBUFFER);
			if (!strcmp(field, "BLEND_COLORSUB"))
				return std::optional(BlendMode::BLEND_COLORSUB);
			if (!strcmp(field, "BLEND_SEMITRANS_ZBUFFER"))
				return std::optional(BlendMode::BLEND_SEMITRANS_ZBUFFER);
			if (!strcmp(field, "BLEND_DESTINATION_INV"))
				return std::optional(BlendMode::BLEND_DESTINATION_INV);
			if (!strcmp(field, "BLEND_SCREEN_DARKEN"))
				return std::optional(BlendMode::BLEND_SCREEN_DARKEN);
			if (!strcmp(field, "BLEND_SCREEN_CLEAR"))
				return std::optional(BlendMode::BLEND_SCREEN_CLEAR);
			if (!strcmp(field, "BLEND_CUSTOM_11"))
				return std::optional(BlendMode::BLEND_CUSTOM_11);
			if (!strcmp(field, "BLEND_CUSTOM_12"))
				return std::optional(BlendMode::BLEND_CUSTOM_12);
			if (!strcmp(field, "BLEND_CUSTOM_13"))
				return std::optional(BlendMode::BLEND_CUSTOM_13);
			break;

		case 'D':
			if (!strcmp(field, "DRAW_SPRITE"))
				return std::optional(Particles::DrawMode::DRAW_SPRITE);
			if (!strcmp(field, "DRAW_SQUARE"))
				return std::optional(Particles::DrawMode::DRAW_SQUARE);
			if (!strcmp(field, "DRAW_LINE"))
				return std::optional(Particles::DrawMode::DRAW_LINE);
			if (!strcmp(field, "DRAW_ARROW"))
				return std::optional(Particles::DrawMode::DRAW_ARROW);
			if (!strcmp(field, "DRAW_NONE"))
				return std::optional(Particles::DrawMode::DRAW_NONE);
			break;

		case 'L':
			if (!strcmp(field, "LIGHT_STATIC"))
				return std::optional(LightMode::LIGHT_STATIC);
			if (!strcmp(field, "LIGHT_DYNAMIC"))
				return std::optional(LightMode::LIGHT_DYNAMIC);
			if (!strcmp(field, "LOG_NONE"))
				return std::optional(LoggerType::LOG_NONE);
			if (!strcmp(field, "LOG_CONSOLE"))
				return std::optional(LoggerType::LOG_CONSOLE);
			if (!strcmp(field, "LOG_FILE"))
				return std::optional(LoggerType::LOG_FILE);
			break;

		case 'S':
			if (!strcmp(field, "SLOT_LARA"))
				return std::optional(SLOT_LARA);
			if (!strcmp(field, "SLOT_PISTOLS_ANIM"))
				return std::optional(SLOT_PISTOLS_ANIM);
			if (!strcmp(field, "SLOT_UZI_ANIM"))
				return std::optional(SLOT_UZI_ANIM);
			if (!strcmp(field, "SLOT_SHOTGUN_ANIM"))
				return std::optional(SLOT_SHOTGUN_ANIM);
			if (!strcmp(field, "SLOT_CROSSBOW_ANIM"))
				return std::optional(SLOT_CROSSBOW_ANIM);
			if (!strcmp(field, "SLOT_GRENADE_GUN_ANIM"))
				return std::optional(SLOT_GRENADE_GUN_ANIM);
			if (!strcmp(field, "SLOT_SIXSHOOTER_ANIM"))
				return std::optional(SLOT_SIXSHOOTER_ANIM);
			if (!strcmp(field, "SLOT_FLARE_ANIM"))
				return std::optional(SLOT_FLARE_ANIM);
			if (!strcmp(field, "SLOT_LARA_SKIN"))
				return std::optional(SLOT_LARA_SKIN);
			if (!strcmp(field, "SLOT_LARA_SKIN_JOINTS"))
				return std::optional(SLOT_LARA_SKIN_JOINTS);
			if (!strcmp(field, "SLOT_LARA_SCREAM"))
				return std::optional(SLOT_LARA_SCREAM);
			if (!strcmp(field, "SLOT_LARA_CROSSBOW_LASER"))
				return std::optional(SLOT_LARA_CROSSBOW_LASER);
			if (!strcmp(field, "SLOT_LARA_REVOLVER_LASER"))
				return std::optional(SLOT_LARA_REVOLVER_LASER);
			if (!strcmp(field, "SLOT_LARA_HOLSTERS"))
				return std::optional(SLOT_LARA_HOLSTERS);
			if (!strcmp(field, "SLOT_LARA_HOLSTERS_PISTOLS"))
				return std::optional(SLOT_LARA_HOLSTERS_PISTOLS);
			if (!strcmp(field, "SLOT_LARA_HOLSTERS_UZIS"))
				return std::optional(SLOT_LARA_HOLSTERS_UZIS);
			if (!strcmp(field, "SLOT_LARA_HOLSTERS_SIXSHOOTER"))
				return std::optional(SLOT_LARA_HOLSTERS_SIXSHOOTER);
			if (!strcmp(field, "SLOT_LARA_SPEECH_HEAD1"))
				return std::optional(SLOT_LARA_SPEECH_HEAD1);
			if (!strcmp(field, "SLOT_LARA_SPEECH_HEAD2"))
				return std::optional(SLOT_LARA_SPEECH_HEAD2);
			if (!strcmp(field, "SLOT_LARA_SPEECH_HEAD3"))
				return std::optional(SLOT_LARA_SPEECH_HEAD3);
			if (!strcmp(field, "SLOT_LARA_SPEECH_HEAD4"))
				return std::optional(SLOT_LARA_SPEECH_HEAD4);
			if (!strcmp(field, "SLOT_ACTOR1_SPEECH_HEAD1"))
				return std::optional(SLOT_ACTOR1_SPEECH_HEAD1);
			if (!strcmp(field, "SLOT_ACTOR1_SPEECH_HEAD2"))
				return std::optional(SLOT_ACTOR1_SPEECH_HEAD2);
			if (!strcmp(field, "SLOT_ACTOR2_SPEECH_HEAD1"))
				return std::optional(SLOT_ACTOR2_SPEECH_HEAD1);
			if (!strcmp(field, "SLOT_ACTOR2_SPEECH_HEAD2"))
				return std::optional(SLOT_ACTOR2_SPEECH_HEAD2);
			if (!strcmp(field, "SLOT_LARA_WATER_MESH"))
				return std::optional(SLOT_LARA_WATER_MESH);
			if (!strcmp(field, "SLOT_LARA_PETROL_MESH"))
				return std::optional(SLOT_LARA_PETROL_MESH);
			if (!strcmp(field, "SLOT_LARA_DIRT_MESH"))
				return std::optional(SLOT_LARA_DIRT_MESH);
			if (!strcmp(field, "SLOT_CROWBAR_ANIM"))
				return std::optional(SLOT_CROWBAR_ANIM);
			if (!strcmp(field, "SLOT_TORCH_ANIM"))
				return std::optional(SLOT_TORCH_ANIM);
			if (!strcmp(field, "SLOT_HAIR"))
				return std::optional(SLOT_HAIR);
			if (!strcmp(field, "SLOT_MOTORBIKE"))
				return std::optional(SLOT_MOTORBIKE);
			if (!strcmp(field, "SLOT_JEEP"))
				return std::optional(SLOT_JEEP);
			if (!strcmp(field, "SLOT_VEHICLE_EXTRA"))
				return std::optional(SLOT_VEHICLE_EXTRA);
			if (!strcmp(field, "SLOT_ENEMY_JEEP"))
				return std::optional(SLOT_ENEMY_JEEP);
			if (!strcmp(field, "SLOT_SKELETON"))
				return std::optional(SLOT_SKELETON);
			if (!strcmp(field, "SLOT_SKELETON_MIP"))
				return std::optional(SLOT_SKELETON_MIP);
			if (!strcmp(field, "SLOT_GUIDE"))
				return std::optional(SLOT_GUIDE);
			if (!strcmp(field, "SLOT_GUIDE_MIP"))
				return std::optional(SLOT_GUIDE_MIP);
			if (!strcmp(field, "SLOT_VON_CROY"))
				return std::optional(SLOT_VON_CROY);
			if (!strcmp(field, "SLOT_VON_CROY_MIP"))
				return std::optional(SLOT_VON_CROY_MIP);
			if (!strcmp(field, "SLOT_BADDY_1"))
				return std::optional(SLOT_BADDY_1);
			if (!strcmp(field, "SLOT_BADDY_1_MIP"))
				return std::optional(SLOT_BADDY_1_MIP);
			if (!strcmp(field, "SLOT_BADDY_2"))
				return std::optional(SLOT_BADDY_2);
			if (!strcmp(field, "SLOT_BADDY_2_MIP"))
				return std::optional(SLOT_BADDY_2_MIP);
			if (!strcmp(field, "SLOT_SETHA"))
				return std::optional(SLOT_SETHA);
			if (!strcmp(field, "SLOT_SETHA_MIP"))
				return std::optional(SLOT_SETHA_MIP);
			if (!strcmp(field, "SLOT_MUMMY"))
				return std::optional(SLOT_MUMMY);
			if (!strcmp(field, "SLOT_MUMMY_MIP"))
				return std::optional(SLOT_MUMMY_MIP);
			if (!strcmp(field, "SLOT_SPHINX"))
				return std::optional(SLOT_SPHINX);
			if (!strcmp(field, "SLOT_SPHINX_MIP"))
				return std::optional(SLOT_SPHINX_MIP);
			if (!strcmp(field, "SLOT_CROCODILE"))
				return std::optional(SLOT_CROCODILE);
			if (!strcmp(field, "SLOT_CROCODILE_MIP"))
				return std::optional(SLOT_CROCODILE_MIP);
			if (!strcmp(field, "SLOT_HORSEMAN"))
				return std::optional(SLOT_HORSEMAN);
			if (!strcmp(field, "SLOT_HORSEMAN_MIP"))
				return std::optional(SLOT_HORSEMAN_MIP);
			if (!strcmp(field, "SLOT_SCORPION"))
				return std::optional(SLOT_SCORPION);
			if (!strcmp(field, "SLOT_SCORPION_MIP"))
				return std::optional(SLOT_SCORPION_MIP);
			if (!strcmp(field, "SLOT_JEAN_YVES"))
				return std::optional(SLOT_JEAN_YVES);
			if (!strcmp(field, "SLOT_JEAN_YVES_MIP"))
				return std::optional(SLOT_JEAN_YVES_MIP);
			if (!strcmp(field, "SLOT_TROOPS"))
				return std::optional(SLOT_TROOPS);
			if (!strcmp(field, "SLOT_TROOPS_MIP"))
				return std::optional(SLOT_TROOPS_MIP);
			if (!strcmp(field, "SLOT_KNIGHTS_TEMPLAR"))
				return std::optional(SLOT_KNIGHTS_TEMPLAR);
			if (!strcmp(field, "SLOT_KNIGHTS_TEMPLAR_MIP"))
				return std::optional(SLOT_KNIGHTS_TEMPLAR_MIP);
			if (!strcmp(field, "SLOT_MUTANT"))
				return std::optional(SLOT_MUTANT);
			if (!strcmp(field, "SLOT_MUTANT_MIP"))
				return std::optional(SLOT_MUTANT_MIP);
			if (!strcmp(field, "SLOT_HORSE"))
				return std::optional(SLOT_HORSE);
			if (!strcmp(field, "SLOT_HORSE_MIP"))
				return std::optional(SLOT_HORSE_MIP);
			if (!strcmp(field, "SLOT_BABOON_NORMAL"))
				return std::optional(SLOT_BABOON_NORMAL);
			if (!strcmp(field, "SLOT_BABOON_NORMAL_MIP"))
				return std::optional(SLOT_BABOON_NORMAL_MIP);
			if (!strcmp(field, "SLOT_BABOON_INV"))
				return std::optional(SLOT_BABOON_INV);
			if (!strcmp(field, "SLOT_BABOON_INV_MIP"))
				return std::optional(SLOT_BABOON_INV_MIP);
			if (!strcmp(field, "SLOT_BABOON_SILENT"))
				return std::optional(SLOT_BABOON_SILENT);
			if (!strcmp(field, "SLOT_BABOON_SILENT_MIP"))
				return std::optional(SLOT_BABOON_SILENT_MIP);
			if (!strcmp(field, "SLOT_WILD_BOAR"))
				return std::optional(SLOT_WILD_BOAR);
			if (!strcmp(field, "SLOT_WILD_BOAR_MIP"))
				return std::optional(SLOT_WILD_BOAR_MIP);
			if (!strcmp(field, "SLOT_HARPY"))
				return std::optional(SLOT_HARPY);
			if (!strcmp(field, "SLOT_HARPY_MIP"))
				return std::optional(SLOT_HARPY_MIP);
			if (!strcmp(field, "SLOT_DEMIGOD1"))
				return std::optional(SLOT_DEMIGOD1);
			if (!strcmp(field, "SLOT_DEMIGOD1_MIP"))
				return std::optional(SLOT_DEMIGOD1_MIP);
			if (!strcmp(field, "SLOT_DEMIGOD2"))
				return std::optional(SLOT_DEMIGOD2);
			if (!strcmp(field, "SLOT_DEMIGOD2_MIP"))
				return std::optional(SLOT_DEMIGOD2_MIP);
			if (!strcmp(field, "SLOT_DEMIGOD3"))
				return std::optional(SLOT_DEMIGOD3);
			if (!strcmp(field, "SLOT_DEMIGOD3_MIP"))
				return std::optional(SLOT_DEMIGOD3_MIP);
			if (!strcmp(field, "SLOT_LITTLE_BEETLE"))
				return std::optional(SLOT_LITTLE_BEETLE);
			if (!strcmp(field, "SLOT_BIG_BEETLE"))
				return std::optional(SLOT_BIG_BEETLE);
			if (!strcmp(field, "SLOT_BIG_BEETLE_MIP"))
				return std::optional(SLOT_BIG_BEETLE_MIP);
			if (!strcmp(field, "SLOT_WRAITH1"))
				return std::optional(SLOT_WRAITH1);
			if (!strcmp(field, "SLOT_WRAITH2"))
				return std::optional(SLOT_WRAITH2);
			if (!strcmp(field, "SLOT_WRAITH3"))
				return std::optional(SLOT_WRAITH3);
			if (!strcmp(field, "SLOT_WRAITH4"))
				return std::optional(SLOT_WRAITH4);
			if (!strcmp(field, "SLOT_BAT"))
				return std::optional(SLOT_BAT);
			if (!strcmp(field, "SLOT_DOG"))
				return std::optional(SLOT_DOG);
			if (!strcmp(field, "SLOT_DOG_MIP"))
				return std::optional(SLOT_DOG_MIP);
			if (!strcmp(field, "SLOT_HAMMERHEAD"))
				return std::optional(SLOT_HAMMERHEAD);
			if (!strcmp(field, "SLOT_HAMMERHEAD_MIP"))
				return std::optional(SLOT_HAMMERHEAD_MIP);
			if (!strcmp(field, "SLOT_SAS"))
				return std::optional(SLOT_SAS);
			if (!strcmp(field, "SLOT_SAS_MIP"))
				return std::optional(SLOT_SAS_MIP);
			if (!strcmp(field, "SLOT_SAS_DYING"))
				return std::optional(SLOT_SAS_DYING);
			if (!strcmp(field, "SLOT_SAS_DYING_MIP"))
				return std::optional(SLOT_SAS_DYING_MIP);
			if (!strcmp(field, "SLOT_SAS_CAPTAIN"))
				return std::optional(SLOT_SAS_CAPTAIN);
			if (!strcmp(field, "SLOT_SAS_CAPTAIN_MIP"))
				return std::optional(SLOT_SAS_CAPTAIN_MIP);
			if (!strcmp(field, "SLOT_SAS_DRAG_BLOKE"))
				return std::optional(SLOT_SAS_DRAG_BLOKE);
			if (!strcmp(field, "SLOT_AHMET"))
				return std::optional(SLOT_AHMET);
			if (!strcmp(field, "SLOT_AHMET_MIP"))
				return std::optional(SLOT_AHMET_MIP);
			if (!strcmp(field, "SLOT_LARA_DOUBLE"))
				return std::optional(SLOT_LARA_DOUBLE);
			if (!strcmp(field, "SLOT_LARA_DOUBLE_MIP"))
				return std::optional(SLOT_LARA_DOUBLE_MIP);
			if (!strcmp(field, "SLOT_SMALL_SCORPION"))
				return std::optional(SLOT_SMALL_SCORPION);
			if (!strcmp(field, "SLOT_LOCUST_EMITTER"))
				return std::optional(SLOT_LOCUST_EMITTER);
			if (!strcmp(field, "SLOT_GAME_PIECE1"))
				return std::optional(SLOT_GAME_PIECE1);
			if (!strcmp(field, "SLOT_GAME_PIECE2"))
				return std::optional(SLOT_GAME_PIECE2);
			if (!strcmp(field, "SLOT_GAME_PIECE3"))
				return std::optional(SLOT_GAME_PIECE3);
			if (!strcmp(field, "SLOT_ENEMY_PIECE"))
				return std::optional(SLOT_ENEMY_PIECE);
			if (!strcmp(field, "SLOT_WHEEL_OF_FORTUNE"))
				return std::optional(SLOT_WHEEL_OF_FORTUNE);
			if (!strcmp(field, "SLOT_SCALES"))
				return std::optional(SLOT_SCALES);
			if (!strcmp(field, "SLOT_DARTS"))
				return std::optional(SLOT_DARTS);
			if (!strcmp(field, "SLOT_DART_EMITTER"))
				return std::optional(SLOT_DART_EMITTER);
			if (!strcmp(field, "SLOT_HOMING_DART_EMITTER"))
				return std::optional(SLOT_HOMING_DART_EMITTER);
			if (!strcmp(field, "SLOT_FALLING_CEILING"))
				return std::optional(SLOT_FALLING_CEILING);
			if (!strcmp(field, "SLOT_FALLING_BLOCK"))
				return std::optional(SLOT_FALLING_BLOCK);
			if (!strcmp(field, "SLOT_FALLING_BLOCK2"))
				return std::optional(SLOT_FALLING_BLOCK2);
			if (!strcmp(field, "SLOT_SMASHABLE_BIKE_WALL"))
				return std::optional(SLOT_SMASHABLE_BIKE_WALL);
			if (!strcmp(field, "SLOT_SMASHABLE_BIKE_FLOOR"))
				return std::optional(SLOT_SMASHABLE_BIKE_FLOOR);
			if (!strcmp(field, "SLOT_TRAPDOOR1"))
				return std::optional(SLOT_TRAPDOOR1);
			if (!strcmp(field, "SLOT_TRAPDOOR2"))
				return std::optional(SLOT_TRAPDOOR2);
			if (!strcmp(field, "SLOT_TRAPDOOR3"))
				return std::optional(SLOT_TRAPDOOR3);
			if (!strcmp(field, "SLOT_FLOOR_TRAPDOOR1"))
				return std::optional(SLOT_FLOOR_TRAPDOOR1);
			if (!strcmp(field, "SLOT_FLOOR_TRAPDOOR2"))
				return std::optional(SLOT_FLOOR_TRAPDOOR2);
			if (!strcmp(field, "SLOT_CEILING_TRAPDOOR1"))
				return std::optional(SLOT_CEILING_TRAPDOOR1);
			if (!strcmp(field, "SLOT_CEILING_TRAPDOOR2"))
				return std::optional(SLOT_CEILING_TRAPDOOR2);
			if (!strcmp(field, "SLOT_SCALING_TRAPDOOR"))
				return std::optional(SLOT_SCALING_TRAPDOOR);
			if (!strcmp(field, "SLOT_ROLLINGBALL"))
				return std::optional(SLOT_ROLLINGBALL);
			if (!strcmp(field, "SLOT_SPIKEY_FLOOR"))
				return std::optional(SLOT_SPIKEY_FLOOR);
			if (!strcmp(field, "SLOT_TEETH_SPIKES"))
				return std::optional(SLOT_TEETH_SPIKES);
			if (!strcmp(field, "SLOT_JOBY_SPIKES"))
				return std::optional(SLOT_JOBY_SPIKES);
			if (!strcmp(field, "SLOT_SLICER_DICER"))
				return std::optional(SLOT_SLICER_DICER);
			if (!strcmp(field, "SLOT_CHAIN"))
				return std::optional(SLOT_CHAIN);
			if (!strcmp(field, "SLOT_PLOUGH"))
				return std::optional(SLOT_PLOUGH);
			if (!strcmp(field, "SLOT_STARGATE"))
				return std::optional(SLOT_STARGATE);
			if (!strcmp(field, "SLOT_HAMMER"))
				return std::optional(SLOT_HAMMER);
			if (!strcmp(field, "SLOT_BURNING_FLOOR"))
				return std::optional(SLOT_BURNING_FLOOR);
			if (!strcmp(field, "SLOT_COG"))
				return std::optional(SLOT_COG);
			if (!strcmp(field, "SLOT_SPIKEBALL"))
				return std::optional(SLOT_SPIKEBALL);
			if (!strcmp(field, "SLOT_FLAME"))
				return std::optional(SLOT_FLAME);
			if (!strcmp(field, "SLOT_FLAME_EMITTER"))
				return std::optional(SLOT_FLAME_EMITTER);
			if (!strcmp(field, "SLOT_FLAME_EMITTER2"))
				return std::optional(SLOT_FLAME_EMITTER2);
			if (!strcmp(field, "SLOT_FLAME_EMITTER3"))
				return std::optional(SLOT_FLAME_EMITTER3);
			if (!strcmp(field, "SLOT_ROPE"))
				return std::optional(SLOT_ROPE);
			if (!strcmp(field, "SLOT_FIREROPE"))
				return std::optional(SLOT_FIREROPE);
			if (!strcmp(field, "SLOT_POLEROPE"))
				return std::optional(SLOT_POLEROPE);
			if (!strcmp(field, "SLOT_ONEBLOCK_PLATFORM"))
				return std::optional(SLOT_ONEBLOCK_PLATFORM);
			if (!strcmp(field, "SLOT_TWOBLOCK_PLATFORM"))
				return std::optional(SLOT_TWOBLOCK_PLATFORM);
			if (!strcmp(field, "SLOT_RAISING_BLOCK1"))
				return std::optional(SLOT_RAISING_BLOCK1);
			if (!strcmp(field, "SLOT_RAISING_BLOCK2"))
				return std::optional(SLOT_RAISING_BLOCK2);
			if (!strcmp(field, "SLOT_EXPANDING_PLATFORM"))
				return std::optional(SLOT_EXPANDING_PLATFORM);
			if (!strcmp(field, "SLOT_SQUISHY_BLOCK1"))
				return std::optional(SLOT_SQUISHY_BLOCK1);
			if (!strcmp(field, "SLOT_SQUISHY_BLOCK2"))
				return std::optional(SLOT_SQUISHY_BLOCK2);
			if (!strcmp(field, "SLOT_PUSHABLE_OBJECT1"))
				return std::optional(SLOT_PUSHABLE_OBJECT1);
			if (!strcmp(field, "SLOT_PUSHABLE_OBJECT2"))
				return std::optional(SLOT_PUSHABLE_OBJECT2);
			if (!strcmp(field, "SLOT_PUSHABLE_OBJECT3"))
				return std::optional(SLOT_PUSHABLE_OBJECT3);
			if (!strcmp(field, "SLOT_PUSHABLE_OBJECT4"))
				return std::optional(SLOT_PUSHABLE_OBJECT4);
			if (!strcmp(field, "SLOT_PUSHABLE_OBJECT5"))
				return std::optional(SLOT_PUSHABLE_OBJECT5);
			if (!strcmp(field, "SLOT_TRIPWIRE"))
				return std::optional(SLOT_TRIPWIRE);
			if (!strcmp(field, "SLOT_SENTRY_GUN"))
				return std::optional(SLOT_SENTRY_GUN);
			if (!strcmp(field, "SLOT_MINE"))
				return std::optional(SLOT_MINE);
			if (!strcmp(field, "SLOT_MAPPER"))
				return std::optional(SLOT_MAPPER);
			if (!strcmp(field, "SLOT_OBELISK"))
				return std::optional(SLOT_OBELISK);
			if (!strcmp(field, "SLOT_FLOOR_4BLADE"))
				return std::optional(SLOT_FLOOR_4BLADE);
			if (!strcmp(field, "SLOT_ROOF_4BLADE"))
				return std::optional(SLOT_ROOF_4BLADE);
			if (!strcmp(field, "SLOT_BIRD_BLADE"))
				return std::optional(SLOT_BIRD_BLADE);
			if (!strcmp(field, "SLOT_CATWALK_BLADE"))
				return std::optional(SLOT_CATWALK_BLADE);
			if (!strcmp(field, "SLOT_MOVING_BLADE"))
				return std::optional(SLOT_MOVING_BLADE);
			if (!strcmp(field, "SLOT_PLINTH_BLADE"))
				return std::optional(SLOT_PLINTH_BLADE);
			if (!strcmp(field, "SLOT_SETH_BLADE"))
				return std::optional(SLOT_SETH_BLADE);
			if (!strcmp(field, "SLOT_LIGHTNING_CONDUCTOR"))
				return std::optional(SLOT_LIGHTNING_CONDUCTOR);
			if (!strcmp(field, "SLOT_ELEMENT_PUZZLE"))
				return std::optional(SLOT_ELEMENT_PUZZLE);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM1"))
				return std::optional(SLOT_PUZZLE_ITEM1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM2"))
				return std::optional(SLOT_PUZZLE_ITEM2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM3"))
				return std::optional(SLOT_PUZZLE_ITEM3);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM4"))
				return std::optional(SLOT_PUZZLE_ITEM4);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM5"))
				return std::optional(SLOT_PUZZLE_ITEM5);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM6"))
				return std::optional(SLOT_PUZZLE_ITEM6);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM7"))
				return std::optional(SLOT_PUZZLE_ITEM7);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM8"))
				return std::optional(SLOT_PUZZLE_ITEM8);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM9"))
				return std::optional(SLOT_PUZZLE_ITEM9);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM10"))
				return std::optional(SLOT_PUZZLE_ITEM10);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM11"))
				return std::optional(SLOT_PUZZLE_ITEM11);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM12"))
				return std::optional(SLOT_PUZZLE_ITEM12);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM1_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM1_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM1_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM1_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM2_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM2_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM2_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM2_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM3_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM3_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM3_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM3_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM4_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM4_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM4_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM4_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM5_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM5_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM5_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM5_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM6_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM6_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM6_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM6_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM7_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM7_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM7_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM7_COMBO2);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM8_COMBO1"))
				return std::optional(SLOT_PUZZLE_ITEM8_COMBO1);
			if (!strcmp(field, "SLOT_PUZZLE_ITEM8_COMBO2"))
				return std::optional(SLOT_PUZZLE_ITEM8_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM1"))
				return std::optional(SLOT_KEY_ITEM1);
			if (!strcmp(field, "SLOT_KEY_ITEM2"))
				return std::optional(SLOT_KEY_ITEM2);
			if (!strcmp(field, "SLOT_KEY_ITEM3"))
				return std::optional(SLOT_KEY_ITEM3);
			if (!strcmp(field, "SLOT_KEY_ITEM4"))
				return std::optional(SLOT_KEY_ITEM4);
			if (!strcmp(field, "SLOT_KEY_ITEM5"))
				return std::optional(SLOT_KEY_ITEM5);
			if (!strcmp(field, "SLOT_KEY_ITEM6"))
				return std::optional(SLOT_KEY_ITEM6);
			if (!strcmp(field, "SLOT_KEY_ITEM7"))
				return std::optional(SLOT_KEY_ITEM7);
			if (!strcmp(field, "SLOT_KEY_ITEM8"))
				return std::optional(SLOT_KEY_ITEM8);
			if (!strcmp(field, "SLOT_KEY_ITEM9"))
				return std::optional(SLOT_KEY_ITEM9);
			if (!strcmp(field, "SLOT_KEY_ITEM10"))
				return std::optional(SLOT_KEY_ITEM10);
			if (!strcmp(field, "SLOT_KEY_ITEM11"))
				return std::optional(SLOT_KEY_ITEM11);
			if (!strcmp(field, "SLOT_KEY_ITEM12"))
				return std::optional(SLOT_KEY_ITEM12);
			if (!strcmp(field, "SLOT_KEY_ITEM1_COMBO1"))
				return std::optional(SLOT_KEY_ITEM1_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM1_COMBO2"))
				return std::optional(SLOT_KEY_ITEM1_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM2_COMBO1"))
				return std::optional(SLOT_KEY_ITEM2_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM2_COMBO2"))
				return std::optional(SLOT_KEY_ITEM2_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM3_COMBO1"))
				return std::optional(SLOT_KEY_ITEM3_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM3_COMBO2"))
				return std::optional(SLOT_KEY_ITEM3_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM4_COMBO1"))
				return std::optional(SLOT_KEY_ITEM4_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM4_COMBO2"))
				return std::optional(SLOT_KEY_ITEM4_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM5_COMBO1"))
				return std::optional(SLOT_KEY_ITEM5_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM5_COMBO2"))
				return std::optional(SLOT_KEY_ITEM5_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM6_COMBO1"))
				return std::optional(SLOT_KEY_ITEM6_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM6_COMBO2"))
				return std::optional(SLOT_KEY_ITEM6_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM7_COMBO1"))
				return std::optional(SLOT_KEY_ITEM7_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM7_COMBO2"))
				return std::optional(SLOT_KEY_ITEM7_COMBO2);
			if (!strcmp(field, "SLOT_KEY_ITEM8_COMBO1"))
				return std::optional(SLOT_KEY_ITEM8_COMBO1);
			if (!strcmp(field, "SLOT_KEY_ITEM8_COMBO2"))
				return std::optional(SLOT_KEY_ITEM8_COMBO2);
			if (!strcmp(field, "SLOT_PICKUP_ITEM1"))
				return std::optional(SLOT_PICKUP_ITEM1);
			if (!strcmp(field, "SLOT_PICKUP_ITEM2"))
				return std::optional(SLOT_PICKUP_ITEM2);
			if (!strcmp(field, "SLOT_PICKUP_ITEM3"))
				return std::optional(SLOT_PICKUP_ITEM3);
			if (!strcmp(field, "SLOT_PICKUP_ITEM4"))
				return std::optional(SLOT_PICKUP_ITEM4);
			if (!strcmp(field, "SLOT_PICKUP_ITEM1_COMBO1"))
				return std::optional(SLOT_PICKUP_ITEM1_COMBO1);
			if (!strcmp(field, "SLOT_PICKUP_ITEM1_COMBO2"))
				return std::optional(SLOT_PICKUP_ITEM1_COMBO2);
			if (!strcmp(field, "SLOT_PICKUP_ITEM2_COMBO1"))
				return std::optional(SLOT_PICKUP_ITEM2_COMBO1);
			if (!strcmp(field, "SLOT_PICKUP_ITEM2_COMBO2"))
				return std::optional(SLOT_PICKUP_ITEM2_COMBO2);
			if (!strcmp(field, "SLOT_PICKUP_ITEM3_COMBO1"))
				return std::optional(SLOT_PICKUP_ITEM3_COMBO1);
			if (!strcmp(field, "SLOT_PICKUP_ITEM3_COMBO2"))
				return std::optional(SLOT_PICKUP_ITEM3_COMBO2);
			if (!strcmp(field, "SLOT_PICKUP_ITEM4_COMBO1"))
				return std::optional(SLOT_PICKUP_ITEM4_COMBO1);
			if (!strcmp(field, "SLOT_PICKUP_ITEM4_COMBO2"))
				return std::optional(SLOT_PICKUP_ITEM4_COMBO2);
			if (!strcmp(field, "SLOT_EXAMINE1"))
				return std::optional(SLOT_EXAMINE1);
			if (!strcmp(field, "SLOT_EXAMINE2"))
				return std::optional(SLOT_EXAMINE2);
			if (!strcmp(field, "SLOT_EXAMINE3"))
				return std::optional(SLOT_EXAMINE3);
			if (!strcmp(field, "SLOT_CROWBAR_ITEM"))
				return std::optional(SLOT_CROWBAR_ITEM);
			if (!strcmp(field, "SLOT_BURNING_TORCH_ITEM"))
				return std::optional(SLOT_BURNING_TORCH_ITEM);
			if (!strcmp(field, "SLOT_CLOCKWORK_BEETLE"))
				return std::optional(SLOT_CLOCKWORK_BEETLE);
			if (!strcmp(field, "SLOT_CLOCKWORK_BEETLE_COMBO1"))
				return std::optional(SLOT_CLOCKWORK_BEETLE_COMBO1);
			if (!strcmp(field, "SLOT_CLOCKWORK_BEETLE_COMBO2"))
				return std::optional(SLOT_CLOCKWORK_BEETLE_COMBO2);
			if (!strcmp(field, "SLOT_MINE_DETECTOR"))
				return std::optional(SLOT_MINE_DETECTOR);
			if (!strcmp(field, "SLOT_QUEST_ITEM1"))
				return std::optional(SLOT_QUEST_ITEM1);
			if (!strcmp(field, "SLOT_QUEST_ITEM2"))
				return std::optional(SLOT_QUEST_ITEM2);
			if (!strcmp(field, "SLOT_QUEST_ITEM3"))
				return std::optional(SLOT_QUEST_ITEM3);
			if (!strcmp(field, "SLOT_QUEST_ITEM4"))
				return std::optional(SLOT_QUEST_ITEM4);
			if (!strcmp(field, "SLOT_QUEST_ITEM5"))
				return std::optional(SLOT_QUEST_ITEM5);
			if (!strcmp(field, "SLOT_QUEST_ITEM6"))
				return std::optional(SLOT_QUEST_ITEM6);
			if (!strcmp(field, "SLOT_MAP"))
				return std::optional(SLOT_MAP);
			if (!strcmp(field, "SLOT_SECRET_MAP"))
				return std::optional(SLOT_SECRET_MAP);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE1"))
				return std::optional(SLOT_PUZZLE_HOLE1);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE2"))
				return std::optional(SLOT_PUZZLE_HOLE2);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE3"))
				return std::optional(SLOT_PUZZLE_HOLE3);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE4"))
				return std::optional(SLOT_PUZZLE_HOLE4);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE5"))
				return std::optional(SLOT_PUZZLE_HOLE5);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE6"))
				return std::optional(SLOT_PUZZLE_HOLE6);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE7"))
				return std::optional(SLOT_PUZZLE_HOLE7);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE8"))
				return std::optional(SLOT_PUZZLE_HOLE8);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE9"))
				return std::optional(SLOT_PUZZLE_HOLE9);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE10"))
				return std::optional(SLOT_PUZZLE_HOLE10);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE11"))
				return std::optional(SLOT_PUZZLE_HOLE11);
			if (!strcmp(field, "SLOT_PUZZLE_HOLE12"))
				return std::optional(SLOT_PUZZLE_HOLE12);
			if (!strcmp(field, "SLOT_PUZZLE_DONE1"))
				return std::optional(SLOT_PUZZLE_DONE1);
			if (!strcmp(field, "SLOT_PUZZLE_DONE2"))
				return std::optional(SLOT_PUZZLE_DONE2);
			if (!strcmp(field, "SLOT_PUZZLE_DONE3"))
				return std::optional(SLOT_PUZZLE_DONE3);
			if (!strcmp(field, "SLOT_PUZZLE_DONE4"))
				return std::optional(SLOT_PUZZLE_DONE4);
			if (!strcmp(field, "SLOT_PUZZLE_DONE5"))
				return std::optional(SLOT_PUZZLE_DONE5);
			if (!strcmp(field, "SLOT_PUZZLE_DONE6"))
				return std::optional(SLOT_PUZZLE_DONE6);
			if (!strcmp(field, "SLOT_PUZZLE_DONE7"))
				return std::optional(SLOT_PUZZLE_DONE7);
			if (!strcmp(field, "SLOT_PUZZLE_DONE8"))
				return std::optional(SLOT_PUZZLE_DONE8);
			if (!strcmp(field, "SLOT_PUZZLE_DONE9"))
				return std::optional(SLOT_PUZZLE_DONE9);
			if (!strcmp(field, "SLOT_PUZZLE_DONE10"))
				return std::optional(SLOT_PUZZLE_DONE10);
			if (!strcmp(field, "SLOT_PUZZLE_DONE11"))
				return std::optional(SLOT_PUZZLE_DONE11);
			if (!strcmp(field, "SLOT_PUZZLE_DONE12"))
				return std::optional(SLOT_PUZZLE_DONE12);
			if (!strcmp(field, "SLOT_KEY_HOLE1"))
				return std::optional(SLOT_KEY_HOLE1);
			if (!strcmp(field, "SLOT_KEY_HOLE2"))
				return std::optional(SLOT_KEY_HOLE2);
			if (!strcmp(field, "SLOT_KEY_HOLE3"))
				return std::optional(SLOT_KEY_HOLE3);
			if (!strcmp(field, "SLOT_KEY_HOLE4"))
				return std::optional(SLOT_KEY_HOLE4);
			if (!strcmp(field, "SLOT_KEY_HOLE5"))
				return std::optional(SLOT_KEY_HOLE5);
			if (!strcmp(field, "SLOT_KEY_HOLE6"))
				return std::optional(SLOT_KEY_HOLE6);
			if (!strcmp(field, "SLOT_KEY_HOLE7"))
				return std::optional(SLOT_KEY_HOLE7);
			if (!strcmp(field, "SLOT_KEY_HOLE8"))
				return std::optional(SLOT_KEY_HOLE8);
			if (!strcmp(field, "SLOT_KEY_HOLE9"))
				return std::optional(SLOT_KEY_HOLE9);
			if (!strcmp(field, "SLOT_KEY_HOLE10"))
				return std::optional(SLOT_KEY_HOLE10);
			if (!strcmp(field, "SLOT_KEY_HOLE11"))
				return std::optional(SLOT_KEY_HOLE11);
			if (!strcmp(field, "SLOT_KEY_HOLE12"))
				return std::optional(SLOT_KEY_HOLE12);
			if (!strcmp(field, "SLOT_WATERSKIN1_EMPTY"))
				return std::optional(SLOT_WATERSKIN1_EMPTY);
			if (!strcmp(field, "SLOT_WATERSKIN1_1"))
				return std::optional(SLOT_WATERSKIN1_1);
			if (!strcmp(field, "SLOT_WATERSKIN1_2"))
				return std::optional(SLOT_WATERSKIN1_2);
			if (!strcmp(field, "SLOT_WATERSKIN1_3"))
				return std::optional(SLOT_WATERSKIN1_3);
			if (!strcmp(field, "SLOT_WATERSKIN2_EMPTY"))
				return std::optional(SLOT_WATERSKIN2_EMPTY);
			if (!strcmp(field, "SLOT_WATERSKIN2_1"))
				return std::optional(SLOT_WATERSKIN2_1);
			if (!strcmp(field, "SLOT_WATERSKIN2_2"))
				return std::optional(SLOT_WATERSKIN2_2);
			if (!strcmp(field, "SLOT_WATERSKIN2_3"))
				return std::optional(SLOT_WATERSKIN2_3);
			if (!strcmp(field, "SLOT_WATERSKIN2_4"))
				return std::optional(SLOT_WATERSKIN2_4);
			if (!strcmp(field, "SLOT_WATERSKIN2_5"))
				return std::optional(SLOT_WATERSKIN2_5);
			if (!strcmp(field, "SLOT_SWITCH_TYPE1"))
				return std::optional(SLOT_SWITCH_TYPE1);
			if (!strcmp(field, "SLOT_SWITCH_TYPE2"))
				return std::optional(SLOT_SWITCH_TYPE2);
			if (!strcmp(field, "SLOT_SWITCH_TYPE3"))
				return std::optional(SLOT_SWITCH_TYPE3);
			if (!strcmp(field, "SLOT_SWITCH_TYPE4"))
				return std::optional(SLOT_SWITCH_TYPE4);
			if (!strcmp(field, "SLOT_SWITCH_TYPE5"))
				return std::optional(SLOT_SWITCH_TYPE5);
			if (!strcmp(field, "SLOT_SWITCH_TYPE6"))
				return std::optional(SLOT_SWITCH_TYPE6);
			if (!strcmp(field, "SLOT_SWITCH_TYPE7"))
				return std::optional(SLOT_SWITCH_TYPE7);
			if (!strcmp(field, "SLOT_SWITCH_TYPE8"))
				return std::optional(SLOT_SWITCH_TYPE8);
			if (!strcmp(field, "SLOT_UNDERWATER_SWITCH1"))
				return std::optional(SLOT_UNDERWATER_SWITCH1);
			if (!strcmp(field, "SLOT_UNDERWATER_SWITCH2"))
				return std::optional(SLOT_UNDERWATER_SWITCH2);
			if (!strcmp(field, "SLOT_TURN_SWITCH"))
				return std::optional(SLOT_TURN_SWITCH);
			if (!strcmp(field, "SLOT_COG_SWITCH"))
				return std::optional(SLOT_COG_SWITCH);
			if (!strcmp(field, "SLOT_LEVER_SWITCH"))
				return std::optional(SLOT_LEVER_SWITCH);
			if (!strcmp(field, "SLOT_JUMP_SWITCH"))
				return std::optional(SLOT_JUMP_SWITCH);
			if (!strcmp(field, "SLOT_CROWBAR_SWITCH"))
				return std::optional(SLOT_CROWBAR_SWITCH);
			if (!strcmp(field, "SLOT_PULLEY"))
				return std::optional(SLOT_PULLEY);
			if (!strcmp(field, "SLOT_DOOR_TYPE1"))
				return std::optional(SLOT_DOOR_TYPE1);
			if (!strcmp(field, "SLOT_DOOR_TYPE2"))
				return std::optional(SLOT_DOOR_TYPE2);
			if (!strcmp(field, "SLOT_DOOR_TYPE3"))
				return std::optional(SLOT_DOOR_TYPE3);
			if (!strcmp(field, "SLOT_DOOR_TYPE4"))
				return std::optional(SLOT_DOOR_TYPE4);
			if (!strcmp(field, "SLOT_DOOR_TYPE5"))
				return std::optional(SLOT_DOOR_TYPE5);
			if (!strcmp(field, "SLOT_DOOR_TYPE6"))
				return std::optional(SLOT_DOOR_TYPE6);
			if (!strcmp(field, "SLOT_DOOR_TYPE7"))
				return std::optional(SLOT_DOOR_TYPE7);
			if (!strcmp(field, "SLOT_DOOR_TYPE8"))
				return std::optional(SLOT_DOOR_TYPE8);
			if (!strcmp(field, "SLOT_PUSHPULL_DOOR1"))
				return std::optional(SLOT_PUSHPULL_DOOR1);
			if (!strcmp(field, "SLOT_PUSHPULL_DOOR2"))
				return std::optional(SLOT_PUSHPULL_DOOR2);
			if (!strcmp(field, "SLOT_KICK_DOOR1"))
				return std::optional(SLOT_KICK_DOOR1);
			if (!strcmp(field, "SLOT_KICK_DOOR2"))
				return std::optional(SLOT_KICK_DOOR2);
			if (!strcmp(field, "SLOT_UNDERWATER_DOOR"))
				return std::optional(SLOT_UNDERWATER_DOOR);
			if (!strcmp(field, "SLOT_DOUBLE_DOORS"))
				return std::optional(SLOT_DOUBLE_DOORS);
			if (!strcmp(field, "SLOT_BRIDGE_FLAT"))
				return std::optional(SLOT_BRIDGE_FLAT);
			if (!strcmp(field, "SLOT_BRIDGE_TILT1"))
				return std::optional(SLOT_BRIDGE_TILT1);
			if (!strcmp(field, "SLOT_BRIDGE_TILT2"))
				return std::optional(SLOT_BRIDGE_TILT2);
			if (!strcmp(field, "SLOT_SARCOPHAGUS"))
				return std::optional(SLOT_SARCOPHAGUS);
			if (!strcmp(field, "SLOT_SEQUENCE_DOOR1"))
				return std::optional(SLOT_SEQUENCE_DOOR1);
			if (!strcmp(field, "SLOT_SEQUENCE_SWITCH1"))
				return std::optional(SLOT_SEQUENCE_SWITCH1);
			if (!strcmp(field, "SLOT_SEQUENCE_SWITCH2"))
				return std::optional(SLOT_SEQUENCE_SWITCH2);
			if (!strcmp(field, "SLOT_SEQUENCE_SWITCH3"))
				return std::optional(SLOT_SEQUENCE_SWITCH3);
			if (!strcmp(field, "SLOT_SARCOPHAGUS_CUT"))
				return std::optional(SLOT_SARCOPHAGUS_CUT);
			if (!strcmp(field, "SLOT_HORUS_STATUE"))
				return std::optional(SLOT_HORUS_STATUE);
			if (!strcmp(field, "SLOT_GOD_HEAD"))
				return std::optional(SLOT_GOD_HEAD);
			if (!strcmp(field, "SLOT_SETH_DOOR"))
				return std::optional(SLOT_SETH_DOOR);
			if (!strcmp(field, "SLOT_STATUE_PLINTH"))
				return std::optional(SLOT_STATUE_PLINTH);
			if (!strcmp(field, "SLOT_PISTOLS_ITEM"))
				return std::optional(SLOT_PISTOLS_ITEM);
			if (!strcmp(field, "SLOT_PISTOLS_AMMO_ITEM"))
				return std::optional(SLOT_PISTOLS_AMMO_ITEM);
			if (!strcmp(field, "SLOT_UZI_ITEM"))
				return std::optional(SLOT_UZI_ITEM);
			if (!strcmp(field, "SLOT_UZI_AMMO_ITEM"))
				return std::optional(SLOT_UZI_AMMO_ITEM);
			if (!strcmp(field, "SLOT_SHOTGUN_ITEM"))
				return std::optional(SLOT_SHOTGUN_ITEM);
			if (!strcmp(field, "SLOT_SHOTGUN_AMMO1_ITEM"))
				return std::optional(SLOT_SHOTGUN_AMMO1_ITEM);
			if (!strcmp(field, "SLOT_SHOTGUN_AMMO2_ITEM"))
				return std::optional(SLOT_SHOTGUN_AMMO2_ITEM);
			if (!strcmp(field, "SLOT_CROSSBOW_ITEM"))
				return std::optional(SLOT_CROSSBOW_ITEM);
			if (!strcmp(field, "SLOT_CROSSBOW_AMMO1_ITEM"))
				return std::optional(SLOT_CROSSBOW_AMMO1_ITEM);
			if (!strcmp(field, "SLOT_CROSSBOW_AMMO2_ITEM"))
				return std::optional(SLOT_CROSSBOW_AMMO2_ITEM);
			if (!strcmp(field, "SLOT_CROSSBOW_AMMO3_ITEM"))
				return std::optional(SLOT_CROSSBOW_AMMO3_ITEM);
			if (!strcmp(field, "SLOT_CROSSBOW_BOLT"))
				return std::optional(SLOT_CROSSBOW_BOLT);
			if (!strcmp(field, "SLOT_GRENADE_GUN_ITEM"))
				return std::optional(SLOT_GRENADE_GUN_ITEM);
			if (!strcmp(field, "SLOT_GRENADE_GUN_AMMO1_ITEM"))
				return std::optional(SLOT_GRENADE_GUN_AMMO1_ITEM);
			if (!strcmp(field, "SLOT_GRENADE_GUN_AMMO2_ITEM"))
				return std::optional(SLOT_GRENADE_GUN_AMMO2_ITEM);
			if (!strcmp(field, "SLOT_GRENADE_GUN_AMMO3_ITEM"))
				return std::optional(SLOT_GRENADE_GUN_AMMO3_ITEM);
			if (!strcmp(field, "SLOT_GRENADE"))
				return std::optional(SLOT_GRENADE);
			if (!strcmp(field, "SLOT_SIXSHOOTER_ITEM"))
				return std::optional(SLOT_SIXSHOOTER_ITEM);
			if (!strcmp(field, "SLOT_SIXSHOOTER_AMMO_ITEM"))
				return std::optional(SLOT_SIXSHOOTER_AMMO_ITEM);
			if (!strcmp(field, "SLOT_BIGMEDI_ITEM"))
				return std::optional(SLOT_BIGMEDI_ITEM);
			if (!strcmp(field, "SLOT_SMALLMEDI_ITEM"))
				return std::optional(SLOT_SMALLMEDI_ITEM);
			if (!strcmp(field, "SLOT_LASERSIGHT_ITEM"))
				return std::optional(SLOT_LASERSIGHT_ITEM);
			if (!strcmp(field, "SLOT_BINOCULARS_ITEM"))
				return std::optional(SLOT_BINOCULARS_ITEM);
			if (!strcmp(field, "SLOT_FLARE_ITEM"))
				return std::optional(SLOT_FLARE_ITEM);
			if (!strcmp(field, "SLOT_FLARE_INV_ITEM"))
				return std::optional(SLOT_FLARE_INV_ITEM);
			if (!strcmp(field, "SLOT_DIARY_ITEM"))
				return std::optional(SLOT_DIARY_ITEM);
			if (!strcmp(field, "SLOT_COMPASS_ITEM"))
				return std::optional(SLOT_COMPASS_ITEM);
			if (!strcmp(field, "SLOT_MEMCARD_LOAD_INV_ITEM"))
				return std::optional(SLOT_MEMCARD_LOAD_INV_ITEM);
			if (!strcmp(field, "SLOT_MEMCARD_SAVE_INV_ITEM"))
				return std::optional(SLOT_MEMCARD_SAVE_INV_ITEM);
			if (!strcmp(field, "SLOT_PC_LOAD_INV_ITEM"))
				return std::optional(SLOT_PC_LOAD_INV_ITEM);
			if (!strcmp(field, "SLOT_PC_SAVE_INV_ITEM"))
				return std::optional(SLOT_PC_SAVE_INV_ITEM);
			if (!strcmp(field, "SLOT_SMOKE_EMITTER_WHITE"))
				return std::optional(SLOT_SMOKE_EMITTER_WHITE);
			if (!strcmp(field, "SLOT_SMOKE_EMITTER_BLACK"))
				return std::optional(SLOT_SMOKE_EMITTER_BLACK);
			if (!strcmp(field, "SLOT_STEAM_EMITTER"))
				return std::optional(SLOT_STEAM_EMITTER);
			if (!strcmp(field, "SLOT_EARTHQUAKE"))
				return std::optional(SLOT_EARTHQUAKE);
			if (!strcmp(field, "SLOT_BUBBLES"))
				return std::optional(SLOT_BUBBLES);
			if (!strcmp(field, "SLOT_WATERFALLMIST"))
				return std::optional(SLOT_WATERFALLMIST);
			if (!strcmp(field, "SLOT_GUNSHELL"))
				return std::optional(SLOT_GUNSHELL);
			if (!strcmp(field, "SLOT_SHOTGUNSHELL"))
				return std::optional(SLOT_SHOTGUNSHELL);
			if (!strcmp(field, "SLOT_GUN_FLASH"))
				return std::optional(SLOT_GUN_FLASH);
			if (!strcmp(field, "SLOT_BUTTERFLY"))
				return std::optional(SLOT_BUTTERFLY);
			if (!strcmp(field, "SLOT_SPRINKLER"))
				return std::optional(SLOT_SPRINKLER);
			if (!strcmp(field, "SLOT_RED_LIGHT"))
				return std::optional(SLOT_RED_LIGHT);
			if (!strcmp(field, "SLOT_GREEN_LIGHT"))
				return std::optional(SLOT_GREEN_LIGHT);
			if (!strcmp(field, "SLOT_BLUE_LIGHT"))
				return std::optional(SLOT_BLUE_LIGHT);
			if (!strcmp(field, "SLOT_AMBER_LIGHT"))
				return std::optional(SLOT_AMBER_LIGHT);
			if (!strcmp(field, "SLOT_WHITE_LIGHT"))
				return std::optional(SLOT_WHITE_LIGHT);
			if (!strcmp(field, "SLOT_BLINKING_LIGHT"))
				return std::optional(SLOT_BLINKING_LIGHT);
			if (!strcmp(field, "SLOT_LENS_FLARE"))
				return std::optional(SLOT_LENS_FLARE);
			if (!strcmp(field, "SLOT_AI_GUARD"))
				return std::optional(SLOT_AI_GUARD);
			if (!strcmp(field, "SLOT_AI_AMBUSH"))
				return std::optional(SLOT_AI_AMBUSH);
			if (!strcmp(field, "SLOT_AI_PATROL1"))
				return std::optional(SLOT_AI_PATROL1);
			if (!strcmp(field, "SLOT_AI_MODIFY"))
				return std::optional(SLOT_AI_MODIFY);
			if (!strcmp(field, "SLOT_AI_FOLLOW"))
				return std::optional(SLOT_AI_FOLLOW);
			if (!strcmp(field, "SLOT_AI_PATROL2"))
				return std::optional(SLOT_AI_PATROL2);
			if (!strcmp(field, "SLOT_AI_X1"))
				return std::optional(SLOT_AI_X1);
			if (!strcmp(field, "SLOT_AI_X2"))
				return std::optional(SLOT_AI_X2);
			if (!strcmp(field, "SLOT_LARA_START_POS"))
				return std::optional(SLOT_LARA_START_POS);
			if (!strcmp(field, "SLOT_KILL_ALL_TRIGGERS"))
				return std::optional(SLOT_KILL_ALL_TRIGGERS);
			if (!strcmp(field, "SLOT_TRIGGER_TRIGGERER"))
				return std::optional(SLOT_TRIGGER_TRIGGERER);
			if (!strcmp(field, "SLOT_SMASH_OBJECT1"))
				return std::optional(SLOT_SMASH_OBJECT1);
			if (!strcmp(field, "SLOT_SMASH_OBJECT2"))
				return std::optional(SLOT_SMASH_OBJECT2);
			if (!strcmp(field, "SLOT_SMASH_OBJECT3"))
				return std::optional(SLOT_SMASH_OBJECT3);
			if (!strcmp(field, "SLOT_SMASH_OBJECT4"))
				return std::optional(SLOT_SMASH_OBJECT4);
			if (!strcmp(field, "SLOT_SMASH_OBJECT5"))
				return std::optional(SLOT_SMASH_OBJECT5);
			if (!strcmp(field, "SLOT_SMASH_OBJECT6"))
				return std::optional(SLOT_SMASH_OBJECT6);
			if (!strcmp(field, "SLOT_SMASH_OBJECT7"))
				return std::optional(SLOT_SMASH_OBJECT7);
			if (!strcmp(field, "SLOT_SMASH_OBJECT8"))
				return std::optional(SLOT_SMASH_OBJECT8);
			if (!strcmp(field, "SLOT_MESHSWAP1"))
				return std::optional(SLOT_MESHSWAP1);
			if (!strcmp(field, "SLOT_MESHSWAP2"))
				return std::optional(SLOT_MESHSWAP2);
			if (!strcmp(field, "SLOT_MESHSWAP3"))
				return std::optional(SLOT_MESHSWAP3);
			if (!strcmp(field, "SLOT_DEATH_SLIDE"))
				return std::optional(SLOT_DEATH_SLIDE);
			if (!strcmp(field, "SLOT_BODY_PART"))
				return std::optional(SLOT_BODY_PART);
			if (!strcmp(field, "SLOT_CAMERA_TARGET"))
				return std::optional(SLOT_CAMERA_TARGET);
			if (!strcmp(field, "SLOT_WATERFALL1"))
				return std::optional(SLOT_WATERFALL1);
			if (!strcmp(field, "SLOT_WATERFALL2"))
				return std::optional(SLOT_WATERFALL2);
			if (!strcmp(field, "SLOT_WATERFALL3"))
				return std::optional(SLOT_WATERFALL3);
			if (!strcmp(field, "SLOT_PLANET_EFFECT"))
				return std::optional(SLOT_PLANET_EFFECT);
			if (!strcmp(field, "SLOT_ANIMATING1"))
				return std::optional(SLOT_ANIMATING1);
			if (!strcmp(field, "SLOT_ANIMATING1_MIP"))
				return std::optional(SLOT_ANIMATING1_MIP);
			if (!strcmp(field, "SLOT_ANIMATING2"))
				return std::optional(SLOT_ANIMATING2);
			if (!strcmp(field, "SLOT_ANIMATING2_MIP"))
				return std::optional(SLOT_ANIMATING2_MIP);
			if (!strcmp(field, "SLOT_ANIMATING3"))
				return std::optional(SLOT_ANIMATING3);
			if (!strcmp(field, "SLOT_ANIMATING3_MIP"))
				return std::optional(SLOT_ANIMATING3_MIP);
			if (!strcmp(field, "SLOT_ANIMATING4"))
				return std::optional(SLOT_ANIMATING4);
			if (!strcmp(field, "SLOT_ANIMATING4_MIP"))
				return std::optional(SLOT_ANIMATING4_MIP);
			if (!strcmp(field, "SLOT_ANIMATING5"))
				return std::optional(SLOT_ANIMATING5);
			if (!strcmp(field, "SLOT_ANIMATING5_MIP"))
				return std::optional(SLOT_ANIMATING5_MIP);
			if (!strcmp(field, "SLOT_ANIMATING6"))
				return std::optional(SLOT_ANIMATING6);
			if (!strcmp(field, "SLOT_ANIMATING6_MIP"))
				return std::optional(SLOT_ANIMATING6_MIP);
			if (!strcmp(field, "SLOT_ANIMATING7"))
				return std::optional(SLOT_ANIMATING7);
			if (!strcmp(field, "SLOT_ANIMATING7_MIP"))
				return std::optional(SLOT_ANIMATING7_MIP);
			if (!strcmp(field, "SLOT_ANIMATING8"))
				return std::optional(SLOT_ANIMATING8);
			if (!strcmp(field, "SLOT_ANIMATING8_MIP"))
				return std::optional(SLOT_ANIMATING8_MIP);
			if (!strcmp(field, "SLOT_ANIMATING9"))
				return std::optional(SLOT_ANIMATING9);
			if (!strcmp(field, "SLOT_ANIMATING9_MIP"))
				return std::optional(SLOT_ANIMATING9_MIP);
			if (!strcmp(field, "SLOT_ANIMATING10"))
				return std::optional(SLOT_ANIMATING10);
			if (!strcmp(field, "SLOT_ANIMATING10_MIP"))
				return std::optional(SLOT_ANIMATING10_MIP);
			if (!strcmp(field, "SLOT_ANIMATING11"))
				return std::optional(SLOT_ANIMATING11);
			if (!strcmp(field, "SLOT_ANIMATING11_MIP"))
				return std::optional(SLOT_ANIMATING11_MIP);
			if (!strcmp(field, "SLOT_ANIMATING12"))
				return std::optional(SLOT_ANIMATING12);
			if (!strcmp(field, "SLOT_ANIMATING12_MIP"))
				return std::optional(SLOT_ANIMATING12_MIP);
			if (!strcmp(field, "SLOT_ANIMATING13"))
				return std::optional(SLOT_ANIMATING13);
			if (!strcmp(field, "SLOT_ANIMATING13_MIP"))
				return std::optional(SLOT_ANIMATING13_MIP);
			if (!strcmp(field, "SLOT_ANIMATING14"))
				return std::optional(SLOT_ANIMATING14);
			if (!strcmp(field, "SLOT_ANIMATING14_MIP"))
				return std::optional(SLOT_ANIMATING14_MIP);
			if (!strcmp(field, "SLOT_ANIMATING15"))
				return std::optional(SLOT_ANIMATING15);
			if (!strcmp(field, "SLOT_ANIMATING15_MIP"))
				return std::optional(SLOT_ANIMATING15_MIP);
			if (!strcmp(field, "SLOT_ANIMATING16"))
				return std::optional(SLOT_ANIMATING16);
			if (!strcmp(field, "SLOT_ANIMATING16_MIP"))
				return std::optional(SLOT_ANIMATING16_MIP);
			if (!strcmp(field, "SLOT_HORIZON"))
				return std::optional(SLOT_HORIZON);
			if (!strcmp(field, "SLOT_SKY_GRAPHICS"))
				return std::optional(SLOT_SKY_GRAPHICS);
			if (!strcmp(field, "SLOT_BINOCULAR_GRAPHICS"))
				return std::optional(SLOT_BINOCULAR_GRAPHICS);
			if (!strcmp(field, "SLOT_TARGET_GRAPHICS"))
				return std::optional(SLOT_TARGET_GRAPHICS);
			if (!strcmp(field, "SLOT_DEFAULT_SPRITES"))
				return std::optional(SLOT_DEFAULT_SPRITES);
			if (!strcmp(field, "SLOT_MISC_SPRITES"))
				return std::optional(SLOT_MISC_SPRITES);
			if (!strcmp(field, "SLOT_MOTOR_BOAT"))
				return std::optional(SLOT_MOTOR_BOAT);
			if (!strcmp(field, "SLOT_MOTOR_BOAT_LARA"))
				return std::optional(SLOT_MOTOR_BOAT_LARA);
			if (!strcmp(field, "SLOT_RUBBER_BOAT"))
				return std::optional(SLOT_RUBBER_BOAT);
			if (!strcmp(field, "SLOT_RUBBER_BOAT_LARA"))
				return std::optional(SLOT_RUBBER_BOAT_LARA);
			if (!strcmp(field, "SLOT_MOTORBIKE_LARA"))
				return std::optional(SLOT_MOTORBIKE_LARA);
			if (!strcmp(field, "SLOT_FONT_GRAPHICS"))
				return std::optional(SLOT_FONT_GRAPHICS);
			if (!strcmp(field, "SLOT_PARALLEL_BARS"))
				return std::optional(SLOT_PARALLEL_BARS);
			if (!strcmp(field, "SLOT_PANEL_BORDER"))
				return std::optional(SLOT_PANEL_BORDER);
			if (!strcmp(field, "SLOT_PANEL_MIDDLE"))
				return std::optional(SLOT_PANEL_MIDDLE);
			if (!strcmp(field, "SLOT_PANEL_CORNER"))
				return std::optional(SLOT_PANEL_CORNER);
			if (!strcmp(field, "SLOT_PANEL_DIAGONAL"))
				return std::optional(SLOT_PANEL_DIAGONAL);
			if (!strcmp(field, "SLOT_PANEL_STRIP"))
				return std::optional(SLOT_PANEL_STRIP);
			if (!strcmp(field, "SLOT_PANEL_HALF_BORDER1"))
				return std::optional(SLOT_PANEL_HALF_BORDER1);
			if (!strcmp(field, "SLOT_PANEL_HALF_BORDER2"))
				return std::optional(SLOT_PANEL_HALF_BORDER2);
			if (!strcmp(field, "SLOT_PANEL_MIDDLE_CORNER"))
				return std::optional(SLOT_PANEL_MIDDLE_CORNER);
			if (!strcmp(field, "SLOT_TIGHT_ROPE"))
				return std::optional(SLOT_TIGHT_ROPE);
			if (!strcmp(field, "SLOT_LASER_HEAD"))
				return std::optional(SLOT_LASER_HEAD);
			if (!strcmp(field, "SLOT_LASER_HEAD_BASE"))
				return std::optional(SLOT_LASER_HEAD_BASE);
			if (!strcmp(field, "SLOT_LASER_HEAD_TENTACLE"))
				return std::optional(SLOT_LASER_HEAD_TENTACLE);
			if (!strcmp(field, "SLOT_HYDRA"))
				return std::optional(SLOT_HYDRA);
			if (!strcmp(field, "SLOT_HYDRA_MISSILE"))
				return std::optional(SLOT_HYDRA_MISSILE);
			if (!strcmp(field, "SLOT_ENEMY_SUB_MARINE"))
				return std::optional(SLOT_ENEMY_SUB_MARINE);
			if (!strcmp(field, "SLOT_ENEMY_SUB_MARINE_MIP"))
				return std::optional(SLOT_ENEMY_SUB_MARINE_MIP);
			if (!strcmp(field, "SLOT_SUB_MARINE_MISSILE"))
				return std::optional(SLOT_SUB_MARINE_MISSILE);
			if (!strcmp(field, "SLOT_FROG_MAN"))
				return std::optional(SLOT_FROG_MAN);
			if (!strcmp(field, "SLOT_FROG_MAN_HARPOON"))
				return std::optional(SLOT_FROG_MAN_HARPOON);
			if (!strcmp(field, "SLOT_FISH_EMITTER"))
				return std::optional(SLOT_FISH_EMITTER);
			if (!strcmp(field, "SLOT_KAYAK"))
				return std::optional(SLOT_KAYAK);
			if (!strcmp(field, "SLOT_KAYAK_LARA"))
				return std::optional(SLOT_KAYAK_LARA);
			if (!strcmp(field, "SLOT_CUSTOM_SPRITES"))
				return std::optional(SLOT_CUSTOM_SPRITES);
			if (!strcmp(field, "SLOT_BRIDGE_TILT3"))
				return std::optional(SLOT_BRIDGE_TILT3);
			if (!strcmp(field, "SLOT_BRIDGE_TILT4"))
				return std::optional(SLOT_BRIDGE_TILT4);
			if (!strcmp(field, "SLOT_BRIDGE_CUSTOM"))
				return std::optional(SLOT_BRIDGE_CUSTOM);
			if (!strcmp(field, "SLOT_ROBOT_CLEANER"))
				return std::optional(SLOT_ROBOT_CLEANER);
			if (!strcmp(field, "SLOT_ROBOT_STAR_WARS"))
				return std::optional(SLOT_ROBOT_STAR_WARS);
			if (!strcmp(field, "SLOT_MECH_WARRIOR"))
				return std::optional(SLOT_MECH_WARRIOR);
			if (!strcmp(field, "SLOT_MECH_WARRIOR_LARA"))
				return std::optional(SLOT_MECH_WARRIOR_LARA);
			if (!strcmp(field, "SLOT_UW_PROPULSOR"))
				return std::optional(SLOT_UW_PROPULSOR);
			if (!strcmp(field, "SLOT_UW_PROPULSOR_LARA"))
				return std::optional(SLOT_UW_PROPULSOR_LARA);
			if (!strcmp(field, "SLOT_MINE_CART"))
				return std::optional(SLOT_MINE_CART);
			if (!strcmp(field, "SLOT_MINE_CART_LARA"))
				return std::optional(SLOT_MINE_CART_LARA);
			if (!strcmp(field, "SLOT_NEW_SLOT5"))
				return std::optional(SLOT_NEW_SLOT5);
			if (!strcmp(field, "SLOT_NEW_SLOT6"))
				return std::optional(SLOT_NEW_SLOT6);
			if (!strcmp(field, "SLOT_NEW_SLOT7"))
				return std::optional(SLOT_NEW_SLOT7);
			if (!strcmp(field, "SLOT_NEW_SLOT8"))
				return std::optional(SLOT_NEW_SLOT8);
			if (!strcmp(field, "SLOT_NEW_SLOT9"))
				return std::optional(SLOT_NEW_SLOT9);
			if (!strcmp(field, "SLOT_NEW_SLOT10"))
				return std::optional(SLOT_NEW_SLOT10);
			if (!strcmp(field, "SLOT_NEW_SLOT11"))
				return std::optional(SLOT_NEW_SLOT11);
			if (!strcmp(field, "SLOT_NEW_SLOT12"))
				return std::optional(SLOT_NEW_SLOT12);
			if (!strcmp(field, "SLOT_NEW_SLOT13"))
				return std::optional(SLOT_NEW_SLOT13);
			if (!strcmp(field, "SLOT_NEW_SLOT14"))
				return std::optional(SLOT_NEW_SLOT14);
			if (!strcmp(field, "SLOT_NEW_SLOT15"))
				return std::optional(SLOT_NEW_SLOT15);
			if (!strcmp(field, "SLOT_NEW_SLOT16"))
				return std::optional(SLOT_NEW_SLOT16);
			if (!strcmp(field, "SLOT_NEW_SLOT17"))
				return std::optional(SLOT_NEW_SLOT17);
			if (!strcmp(field, "SLOT_NEW_SLOT18"))
				return std::optional(SLOT_NEW_SLOT18);
			break;

		case 'T':
			if (!strcmp(field, "TETHER_ROTATING"))
				return std::optional(Particles::TetherType::TETHER_ROTATING);
			if (!strcmp(field, "TETHER_STATIC"))
				return std::optional(Particles::TetherType::TETHER_STATIC);
			if (!strcmp(field, "TETHER_NONE"))
				return std::optional(Particles::TetherType::TETHER_NONE);
			break;
		}
		return std::nullopt;
	}

	std::optional<float> RetrieveFloatConstant(const char* field)
	{
		switch (field[0])
		{
		case 'P':
			if (!strcmp(field, "PI"))
				return std::optional((float)M_PI);
			if (!strcmp(field, "PI_HALF"))
				return std::optional((float)M_PI_2);
			if (!strcmp(field, "PI_QUART"))
				return std::optional((float)M_PI_4);
			if (!strcmp(field, "PI_TWO"))
				return std::optional((float)M_PI * 2.0f);
			break;
		}

		return std::nullopt;
	}

	void TrngVarWrapper::Index(const char* field)
	{
		auto vars = Trng.pGlobTomb4->pBaseVariableTRNG;
		int length;

		if (field)
		{
			length = strlen(field);

			switch (field[0])
			{
			case 'B':
				if (!strcmp(field, "BigText"))
				{
					Script::PushString(vars->Globals.TextBig);
					return;
				}
				break;

			case 'C':
				if (!strcmp(field, "CurrentValue"))
				{
					Script::PushInteger(vars->Globals.CurrentValue);
					return;
				}
				break;

			case 'G':
				if (length >= 12)
				{
					switch (field[6])
					{
					case 'B':
						switch (field[10])
						{
						case 'A':
							if (!strcmp(field, "GlobalByteAlfa1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[0]);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[1]);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa3"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[2]);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa4"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[3]);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "GlobalByteBeta1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[4]);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[5]);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta3"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[6]);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta4"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[7]);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "GlobalByteDelta1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[8]);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[9]);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta3"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[10]);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta4"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriByte[11]);
								return;
							}
							break;
						}
						break;

					case 'L':
						if (!strcmp(field, "GlobalLongAlfa"))
						{
							Script::PushInteger(vars->Globals.NumWar.VetNumeriLong[0]);
							return;
						}
						if (!strcmp(field, "GlobalLongBeta"))
						{
							Script::PushInteger(vars->Globals.NumWar.VetNumeriLong[1]);
							return;
						}
						if (!strcmp(field, "GlobalLongDelta"))
						{
							Script::PushInteger(vars->Globals.NumWar.VetNumeriLong[2]);
							return;
						}
						if (!strcmp(field, "GlobalLongTimer"))
						{
							Script::PushInteger(vars->Globals.NumWar.VetNumeriLong[3]);
							return;
						}
						break;

					case 'S':
						switch (field[11])
						{
						case 'A':
							if (!strcmp(field, "GlobalShortAlfa1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[0]);
								return;
							}
							if (!strcmp(field, "GlobalShortAlfa2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[1]);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "GlobalShortBeta1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[2]);
								return;
							}
							if (!strcmp(field, "GlobalShortBeta2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[3]);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "GlobalShortDelta1"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[4]);
								return;
							}
							if (!strcmp(field, "GlobalShortDelta2"))
							{
								Script::PushInteger(vars->Globals.NumWar.VetNumeriShort[5]);
								return;
							}
							break;
						}
					}
				}
				break;

			case 'L':
				if (!strcmp(field, "LastInputNumber"))
				{
					Script::PushInteger(vars->Globals.LastInputNumber);
					return;
				}
				if (!strcmp(field, "LastInputText"))
				{
					Script::PushString(vars->Globals.LastInputText);
					return;
				}

				if (length >= 11)
				{
					switch (field[5])
					{
					case 'B':
						switch (field[9])
						{
						case 'A':
							if (!strcmp(field, "LocalByteAlfa1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[0]);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[1]);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa3"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[2]);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa4"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[3]);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "LocalByteBeta1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[4]);
								return;
							}
							if (!strcmp(field, "LocalByteBeta2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[5]);
								return;
							}
							if (!strcmp(field, "LocalByteBeta3"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[6]);
								return;
							}
							if (!strcmp(field, "LocalByteBeta4"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[7]);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "LocalByteDelta1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[8]);
								return;
							}
							if (!strcmp(field, "LocalByteDelta2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[9]);
								return;
							}
							if (!strcmp(field, "LocalByteDelta3"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[10]);
								return;
							}
							if (!strcmp(field, "LocalByteDelta4"))
							{
								Script::PushInteger(vars->Locals.VetNumeriByte[11]);
								return;
							}
							break;
						}
						break;

					case 'L':
						if (!strcmp(field, "LocalLongAlfa"))
						{
							Script::PushInteger(vars->Locals.VetNumeriLong[0]);
							return;
						}
						if (!strcmp(field, "LocalLongBeta"))
						{
							Script::PushInteger(vars->Locals.VetNumeriLong[1]);
							return;
						}
						if (!strcmp(field, "LocalLongDelta"))
						{
							Script::PushInteger(vars->Locals.VetNumeriLong[2]);
							return;
						}
						if (!strcmp(field, "LocalLongTimer"))
						{
							Script::PushInteger(vars->Locals.VetNumeriLong[3]);
							return;
						}
						break;

					case 'S':
						switch (field[10])
						{
						case 'A':
							if (!strcmp(field, "LocalShortAlfa1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[0]);
								return;
							}
							if (!strcmp(field, "LocalShortAlfa2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[1]);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "LocalShortBeta1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[2]);
								return;
							}
							if (!strcmp(field, "LocalShortBeta2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[3]);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "LocalShortDelta1"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[4]);
								return;
							}
							if (!strcmp(field, "LocalShortDelta2"))
							{
								Script::PushInteger(vars->Locals.VetNumeriShort[5]);
								return;
							}
							break;
						}
						break;
					}
				}
				break;

			case 'S':
				if (length >= 6)
				{
					switch (field[5])
					{
					case 'B':
						if (length >= 10)
						{
							switch (field[9])
							{
							case 'A':
								if (!strcmp(field, "StoreByteA1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[0]);
									return;
								}
								if (!strcmp(field, "StoreByteA2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[1]);
									return;
								}
								if (!strcmp(field, "StoreByteA3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[2]);
									return;
								}
								if (!strcmp(field, "StoreByteA4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[3]);
									return;
								}
								break;

							case 'B':
								if (!strcmp(field, "StoreByteB1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[4]);
									return;
								}
								if (!strcmp(field, "StoreByteB2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[5]);
									return;
								}
								if (!strcmp(field, "StoreByteB3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[6]);
									return;
								}
								if (!strcmp(field, "StoreByteB4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[7]);
									return;
								}
								break;

							case 'C':
								if (!strcmp(field, "StoreByteC1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[8]);
									return;
								}
								if (!strcmp(field, "StoreByteC2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[9]);
									return;
								}
								if (!strcmp(field, "StoreByteC3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[10]);
									return;
								}
								if (!strcmp(field, "StoreByteC4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[11]);
									return;
								}
								break;

							case 'D':
								if (!strcmp(field, "StoreByteD1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[12]);
									return;
								}
								if (!strcmp(field, "StoreByteD2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[13]);
									return;
								}
								if (!strcmp(field, "StoreByteD3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[14]);
									return;
								}
								if (!strcmp(field, "StoreByteD4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[15]);
									return;
								}
								break;

							case 'E':
								if (!strcmp(field, "StoreByteE1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[16]);
									return;
								}
								if (!strcmp(field, "StoreByteE2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[17]);
									return;
								}
								if (!strcmp(field, "StoreByteE3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[18]);
									return;
								}
								if (!strcmp(field, "StoreByteE4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[19]);
									return;
								}
								break;

							case 'F':
								if (!strcmp(field, "StoreByteF1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[20]);
									return;
								}
								if (!strcmp(field, "StoreByteF2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[21]);
									return;
								}
								if (!strcmp(field, "StoreByteF3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[22]);
									return;
								}
								if (!strcmp(field, "StoreByteF4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[23]);
									return;
								}
								break;

							case 'G':
								if (!strcmp(field, "StoreByteG1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[24]);
									return;
								}
								if (!strcmp(field, "StoreByteG2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[25]);
									return;
								}
								if (!strcmp(field, "StoreByteG3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[26]);
									return;
								}
								if (!strcmp(field, "StoreByteG4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[27]);
									return;
								}
								break;

							case 'H':
								if (!strcmp(field, "StoreByteH1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[28]);
									return;
								}
								if (!strcmp(field, "StoreByteH2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[29]);
									return;
								}
								if (!strcmp(field, "StoreByteH3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[30]);
									return;
								}
								if (!strcmp(field, "StoreByteH4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[31]);
									return;
								}
								break;

							case 'I':
								if (!strcmp(field, "StoreByteI1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[32]);
									return;
								}
								if (!strcmp(field, "StoreByteI2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[33]);
									return;
								}
								if (!strcmp(field, "StoreByteI3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[34]);
									return;
								}
								if (!strcmp(field, "StoreByteI4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[35]);
									return;
								}
								break;

							case 'J':
								if (!strcmp(field, "StoreByteJ1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[36]);
									return;
								}
								if (!strcmp(field, "StoreByteJ2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[37]);
									return;
								}
								if (!strcmp(field, "StoreByteJ3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[38]);
									return;
								}
								if (!strcmp(field, "StoreByteJ4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[39]);
									return;
								}
								break;

							case 'K':
								if (!strcmp(field, "StoreByteK1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[40]);
									return;
								}
								if (!strcmp(field, "StoreByteK2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[41]);
									return;
								}
								if (!strcmp(field, "StoreByteK3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[42]);
									return;
								}
								if (!strcmp(field, "StoreByteK4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[43]);
									return;
								}
								break;

							case 'L':
								if (!strcmp(field, "StoreByteL1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[44]);
									return;
								}
								if (!strcmp(field, "StoreByteL2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[45]);
									return;
								}
								if (!strcmp(field, "StoreByteL3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[46]);
									return;
								}
								if (!strcmp(field, "StoreByteL4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[47]);
									return;
								}
								break;

							case 'M':
								if (!strcmp(field, "StoreByteM1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[48]);
									return;
								}
								if (!strcmp(field, "StoreByteM2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[49]);
									return;
								}
								if (!strcmp(field, "StoreByteM3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[50]);
									return;
								}
								if (!strcmp(field, "StoreByteM4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[51]);
									return;
								}
								break;

							case 'N':
								if (!strcmp(field, "StoreByteN1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[52]);
									return;
								}
								if (!strcmp(field, "StoreByteN2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[53]);
									return;
								}
								if (!strcmp(field, "StoreByteN3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[54]);
									return;
								}
								if (!strcmp(field, "StoreByteN4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[55]);
									return;
								}
								break;

							case 'O':
								if (!strcmp(field, "StoreByteO1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[56]);
									return;
								}
								if (!strcmp(field, "StoreByteO2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[57]);
									return;
								}
								if (!strcmp(field, "StoreByteO3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[58]);
									return;
								}
								if (!strcmp(field, "StoreByteO4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[59]);
									return;
								}
								break;

							case 'P':
								if (!strcmp(field, "StoreByteP1"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[60]);
									return;
								}
								if (!strcmp(field, "StoreByteP2"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[61]);
									return;
								}
								if (!strcmp(field, "StoreByteP3"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[62]);
									return;
								}
								if (!strcmp(field, "StoreByteP4"))
								{
									Script::PushInteger(vars->Globals.VetStoreByte[63]);
									return;
								}
								break;
							}
						}
						break;

					case 'L':
						if (!strcmp(field, "StoreLongA"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[0]);
							return;
						}
						if (!strcmp(field, "StoreLongB"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[1]);
							return;
						}
						if (!strcmp(field, "StoreLongC"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[2]);
							return;
						}
						if (!strcmp(field, "StoreLongD"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[3]);
							return;
						}
						if (!strcmp(field, "StoreLongE"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[4]);
							return;
						}
						if (!strcmp(field, "StoreLongF"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[5]);
							return;
						}
						if (!strcmp(field, "StoreLongG"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[6]);
							return;
						}
						if (!strcmp(field, "StoreLongH"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[7]);
							return;
						}
						if (!strcmp(field, "StoreLongI"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[8]);
							return;
						}
						if (!strcmp(field, "StoreLongJ"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[9]);
							return;
						}
						if (!strcmp(field, "StoreLongK"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[10]);
							return;
						}
						if (!strcmp(field, "StoreLongL"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[11]);
							return;
						}
						if (!strcmp(field, "StoreLongM"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[12]);
							return;
						}
						if (!strcmp(field, "StoreLongN"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[13]);
							return;
						}
						if (!strcmp(field, "StoreLongO"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[14]);
							return;
						}
						if (!strcmp(field, "StoreLongP"))
						{
							Script::PushInteger(vars->Globals.VetStoreLong[15]);
							return;
						}
						break;

					case 'S':
						if (length >= 11)
						{
							switch (field[10])
							{
							case 'A':
								if (!strcmp(field, "StoreShortA1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[0]);
									return;
								}
								if (!strcmp(field, "StoreShortA2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[1]);
									return;
								}
								break;

							case 'B':
								if (!strcmp(field, "StoreShortB1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[2]);
									return;
								}
								if (!strcmp(field, "StoreShortB2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[3]);
									return;
								}
								break;

							case 'C':
								if (!strcmp(field, "StoreShortC1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[4]);
									return;
								}
								if (!strcmp(field, "StoreShortC2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[5]);
									return;
								}
								break;

							case 'D':
								if (!strcmp(field, "StoreShortD1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[6]);
									return;
								}
								if (!strcmp(field, "StoreShortD2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[7]);
									return;
								}
								break;

							case 'E':
								if (!strcmp(field, "StoreShortE1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[8]);
									return;
								}
								if (!strcmp(field, "StoreShortE2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[9]);
									return;
								}
								break;

							case 'F':
								if (!strcmp(field, "StoreShortF1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[10]);
									return;
								}
								if (!strcmp(field, "StoreShortF2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[11]);
									return;
								}
								break;

							case 'G':
								if (!strcmp(field, "StoreShortG1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[12]);
									return;
								}
								if (!strcmp(field, "StoreShortG2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[13]);
									return;
								}
								break;

							case 'H':
								if (!strcmp(field, "StoreShortH1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[14]);
									return;
								}
								if (!strcmp(field, "StoreShortH2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[15]);
									return;
								}
								break;

							case 'I':
								if (!strcmp(field, "StoreShortI1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[16]);
									return;
								}
								if (!strcmp(field, "StoreShortI2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[17]);
									return;
								}
								break;

							case 'J':
								if (!strcmp(field, "StoreShortJ1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[18]);
									return;
								}
								if (!strcmp(field, "StoreShortJ2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[19]);
									return;
								}
								break;

							case 'K':
								if (!strcmp(field, "StoreShortK1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[20]);
									return;
								}
								if (!strcmp(field, "StoreShortK2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[21]);
									return;
								}
								break;

							case 'L':
								if (!strcmp(field, "StoreShortL1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[22]);
									return;
								}
								if (!strcmp(field, "StoreShortL2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[23]);
									return;
								}
								break;

							case 'M':
								if (!strcmp(field, "StoreShortM1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[24]);
									return;
								}
								if (!strcmp(field, "StoreShortM2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[25]);
									return;
								}
								break;

							case 'N':
								if (!strcmp(field, "StoreShortN1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[26]);
									return;
								}
								if (!strcmp(field, "StoreShortN2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[27]);
									return;
								}
								break;

							case 'O':
								if (!strcmp(field, "StoreShortO1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[28]);
									return;
								}
								if (!strcmp(field, "StoreShortO2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[29]);
									return;
								}
								break;

							case 'P':
								if (!strcmp(field, "StoreShortP1"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[30]);
									return;
								}
								if (!strcmp(field, "StoreShortP2"))
								{
									Script::PushInteger(vars->Globals.VetStoreShort[31]);
									return;
								}
								break;
							}
						}
					}
				}
				break;

			case 'T':
				if (!strcmp(field, "Text1"))
				{
					Script::PushString(vars->Globals.VetTextVar[0].Text);
					return;
				}
				if (!strcmp(field, "Text2"))
				{
					Script::PushString(vars->Globals.VetTextVar[1].Text);
					return;
				}
				if (!strcmp(field, "Text3"))
				{
					Script::PushString(vars->Globals.VetTextVar[2].Text);
					return;
				}
				if (!strcmp(field, "Text4"))
				{
					Script::PushString(vars->Globals.VetTextVar[3].Text);
					return;
				}
				break;
			}
		}
		LuaObjectClass::Index(field);
	}

	void TrngVarWrapper::NewIndex(const char* field)
	{
		auto vars = Trng.pGlobTomb4->pBaseVariableTRNG;
		int length;

		if (field)
		{
			length = strlen(field);

			switch (field[0])
			{
			case 'B':
				if (!strcmp(field, "BigText"))
				{
					CopyString(vars->Globals.TextBig, GetLuaString(-1));
					return;
				}
				break;

			case 'C':
				if (!strcmp(field, "CurrentValue"))
				{
					vars->Globals.CurrentValue = GetInteger(-1);
					return;
				}
				break;

			case 'G':
				if (length >= 12)
				{
					switch (field[6])
					{
					case 'B':
						switch (field[10])
						{
						case 'A':
							if (!strcmp(field, "GlobalByteAlfa1"))
							{
								vars->Globals.NumWar.VetNumeriByte[0] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa2"))
							{
								vars->Globals.NumWar.VetNumeriByte[1] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa3"))
							{
								vars->Globals.NumWar.VetNumeriByte[2] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteAlfa4"))
							{
								vars->Globals.NumWar.VetNumeriByte[3] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "GlobalByteBeta1"))
							{
								vars->Globals.NumWar.VetNumeriByte[4] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta2"))
							{
								vars->Globals.NumWar.VetNumeriByte[5] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta3"))
							{
								vars->Globals.NumWar.VetNumeriByte[6] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteBeta4"))
							{
								vars->Globals.NumWar.VetNumeriByte[7] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "GlobalByteDelta1"))
							{
								vars->Globals.NumWar.VetNumeriByte[8] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta2"))
							{
								vars->Globals.NumWar.VetNumeriByte[9] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta3"))
							{
								vars->Globals.NumWar.VetNumeriByte[10] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "GlobalByteDelta4"))
							{
								vars->Globals.NumWar.VetNumeriByte[11] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;
						}
						break;

					case 'L':
						if (!strcmp(field, "GlobalLongAlfa"))
						{
							vars->Globals.NumWar.VetNumeriLong[0] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "GlobalLongBeta"))
						{
							vars->Globals.NumWar.VetNumeriLong[1] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "GlobalLongDelta"))
						{
							vars->Globals.NumWar.VetNumeriLong[2] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "GlobalLongTimer"))
						{
							vars->Globals.NumWar.VetNumeriLong[3] = GetInteger(-1);
							return;
						}
						break;

					case 'S':
						switch (field[11])
						{
						case 'A':
							if (!strcmp(field, "GlobalShortAlfa1"))
							{
								vars->Globals.NumWar.VetNumeriShort[0] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "GlobalShortAlfa2"))
							{
								vars->Globals.NumWar.VetNumeriShort[1] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "GlobalShortBeta1"))
							{
								vars->Globals.NumWar.VetNumeriShort[2] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "GlobalShortBeta2"))
							{
								vars->Globals.NumWar.VetNumeriShort[3] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "GlobalShortDelta1"))
							{
								vars->Globals.NumWar.VetNumeriShort[4] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "GlobalShortDelta2"))
							{
								vars->Globals.NumWar.VetNumeriShort[5] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;
						}
					}
				}
				break;

			case 'L':
				if (!strcmp(field, "LastInputNumber"))
				{
					vars->Globals.LastInputNumber = GetInteger(-1);
					return;
				}
				if (!strcmp(field, "LastInputText"))
				{
					CopyString(vars->Globals.LastInputText, GetLuaString(-1));
					return;
				}

				if (length >= 11)
				{
					switch (field[5])
					{
					case 'B':
						switch (field[9])
						{
						case 'A':
							if (!strcmp(field, "LocalByteAlfa1"))
							{
								vars->Locals.VetNumeriByte[0] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa2"))
							{
								vars->Locals.VetNumeriByte[1] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa3"))
							{
								vars->Locals.VetNumeriByte[2] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteAlfa4"))
							{
								vars->Locals.VetNumeriByte[3] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "LocalByteBeta1"))
							{
								vars->Locals.VetNumeriByte[4] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteBeta2"))
							{
								vars->Locals.VetNumeriByte[5] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteBeta3"))
							{
								vars->Locals.VetNumeriByte[6] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteBeta4"))
							{
								vars->Locals.VetNumeriByte[7] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "LocalByteDelta1"))
							{
								vars->Locals.VetNumeriByte[8] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteDelta2"))
							{
								vars->Locals.VetNumeriByte[9] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteDelta3"))
							{
								vars->Locals.VetNumeriByte[10] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							if (!strcmp(field, "LocalByteDelta4"))
							{
								vars->Locals.VetNumeriByte[11] = GetClampedInteger(-1, 0, 255, false);
								return;
							}
							break;
						}
						break;

					case 'L':
						if (!strcmp(field, "LocalLongAlfa"))
						{
							vars->Locals.VetNumeriLong[0] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "LocalLongBeta"))
						{
							vars->Locals.VetNumeriLong[1] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "LocalLongDelta"))
						{
							vars->Locals.VetNumeriLong[2] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "LocalLongTimer"))
						{
							vars->Locals.VetNumeriLong[3] = GetInteger(-1);
							return;
						}
						break;

					case 'S':
						switch (field[10])
						{
						case 'A':
							if (!strcmp(field, "LocalShortAlfa1"))
							{
								vars->Locals.VetNumeriShort[0] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "LocalShortAlfa2"))
							{
								vars->Locals.VetNumeriShort[1] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;

						case 'B':
							if (!strcmp(field, "LocalShortBeta1"))
							{
								vars->Locals.VetNumeriShort[2] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "LocalShortBeta2"))
							{
								vars->Locals.VetNumeriShort[3] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;

						case 'D':
							if (!strcmp(field, "LocalShortDelta1"))
							{
								vars->Locals.VetNumeriShort[4] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							if (!strcmp(field, "LocalShortDelta2"))
							{
								vars->Locals.VetNumeriShort[5] = GetClampedInteger(-1, -32768, 32767, false);
								return;
							}
							break;
						}
						break;
					}
				}
				break;

			case 'S':
				if (length >= 6)
				{
					switch (field[5])
					{
					case 'B':
						if (length >= 10)
						{
							switch (field[9])
							{
							case 'A':
								if (!strcmp(field, "StoreByteA1"))
								{
									vars->Globals.VetStoreByte[0] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteA2"))
								{
									vars->Globals.VetStoreByte[1] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteA3"))
								{
									vars->Globals.VetStoreByte[2] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteA4"))
								{
									vars->Globals.VetStoreByte[3] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'B':
								if (!strcmp(field, "StoreByteB1"))
								{
									vars->Globals.VetStoreByte[4] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteB2"))
								{
									vars->Globals.VetStoreByte[5] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteB3"))
								{
									vars->Globals.VetStoreByte[6] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteB4"))
								{
									vars->Globals.VetStoreByte[7] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'C':
								if (!strcmp(field, "StoreByteC1"))
								{
									vars->Globals.VetStoreByte[8] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteC2"))
								{
									vars->Globals.VetStoreByte[9] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteC3"))
								{
									vars->Globals.VetStoreByte[10] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteC4"))
								{
									vars->Globals.VetStoreByte[11] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'D':
								if (!strcmp(field, "StoreByteD1"))
								{
									vars->Globals.VetStoreByte[12] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteD2"))
								{
									vars->Globals.VetStoreByte[13] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteD3"))
								{
									vars->Globals.VetStoreByte[14] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteD4"))
								{
									vars->Globals.VetStoreByte[15] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'E':
								if (!strcmp(field, "StoreByteE1"))
								{
									vars->Globals.VetStoreByte[16] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteE2"))
								{
									vars->Globals.VetStoreByte[17] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteE3"))
								{
									vars->Globals.VetStoreByte[18] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteE4"))
								{
									vars->Globals.VetStoreByte[19] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'F':
								if (!strcmp(field, "StoreByteF1"))
								{
									vars->Globals.VetStoreByte[20] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteF2"))
								{
									vars->Globals.VetStoreByte[21] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteF3"))
								{
									vars->Globals.VetStoreByte[22] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteF4"))
								{
									vars->Globals.VetStoreByte[23] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'G':
								if (!strcmp(field, "StoreByteG1"))
								{
									vars->Globals.VetStoreByte[24] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteG2"))
								{
									vars->Globals.VetStoreByte[25] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteG3"))
								{
									vars->Globals.VetStoreByte[26] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteG4"))
								{
									vars->Globals.VetStoreByte[27] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;
							case 'H':
								if (!strcmp(field, "StoreByteH1"))
								{
									vars->Globals.VetStoreByte[28] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteH2"))
								{
									vars->Globals.VetStoreByte[29] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteH3"))
								{
									vars->Globals.VetStoreByte[30] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteH4"))
								{
									vars->Globals.VetStoreByte[31] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'I':
								if (!strcmp(field, "StoreByteI1"))
								{
									vars->Globals.VetStoreByte[32] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteI2"))
								{
									vars->Globals.VetStoreByte[33] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteI3"))
								{
									vars->Globals.VetStoreByte[34] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteI4"))
								{
									vars->Globals.VetStoreByte[35] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'J':
								if (!strcmp(field, "StoreByteJ1"))
								{
									vars->Globals.VetStoreByte[36] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteJ2"))
								{
									vars->Globals.VetStoreByte[37] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteJ3"))
								{
									vars->Globals.VetStoreByte[38] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteJ4"))
								{
									vars->Globals.VetStoreByte[39] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'K':
								if (!strcmp(field, "StoreByteK1"))
								{
									vars->Globals.VetStoreByte[40] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteK2"))
								{
									vars->Globals.VetStoreByte[41] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteK3"))
								{
									vars->Globals.VetStoreByte[42] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteK4"))
								{
									vars->Globals.VetStoreByte[43] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'L':
								if (!strcmp(field, "StoreByteL1"))
								{
									vars->Globals.VetStoreByte[44] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteL2"))
								{
									vars->Globals.VetStoreByte[45] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteL3"))
								{
									vars->Globals.VetStoreByte[46] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteL4"))
								{
									vars->Globals.VetStoreByte[47] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'M':
								if (!strcmp(field, "StoreByteM1"))
								{
									vars->Globals.VetStoreByte[48] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteM2"))
								{
									vars->Globals.VetStoreByte[49] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteM3"))
								{
									vars->Globals.VetStoreByte[50] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteM4"))
								{
									vars->Globals.VetStoreByte[51] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'N':
								if (!strcmp(field, "StoreByteN1"))
								{
									vars->Globals.VetStoreByte[52] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteN2"))
								{
									vars->Globals.VetStoreByte[53] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteN3"))
								{
									vars->Globals.VetStoreByte[54] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteN4"))
								{
									vars->Globals.VetStoreByte[55] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'O':
								if (!strcmp(field, "StoreByteO1"))
								{
									vars->Globals.VetStoreByte[56] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteO2"))
								{
									vars->Globals.VetStoreByte[57] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteO3"))
								{
									vars->Globals.VetStoreByte[58] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteO4"))
								{
									vars->Globals.VetStoreByte[59] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;

							case 'P':
								if (!strcmp(field, "StoreByteP1"))
								{
									vars->Globals.VetStoreByte[60] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteP2"))
								{
									vars->Globals.VetStoreByte[61] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteP3"))
								{
									vars->Globals.VetStoreByte[62] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								if (!strcmp(field, "StoreByteP4"))
								{
									vars->Globals.VetStoreByte[63] = GetClampedInteger(-1, 0, 255, false);
									return;
								}
								break;
							}
						}
						break;

					case 'L':
						if (!strcmp(field, "StoreLongA"))
						{
							vars->Globals.VetStoreLong[0] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongB"))
						{
							vars->Globals.VetStoreLong[1] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongC"))
						{
							vars->Globals.VetStoreLong[2] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongD"))
						{
							vars->Globals.VetStoreLong[3] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongE"))
						{
							vars->Globals.VetStoreLong[4] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongF"))
						{
							vars->Globals.VetStoreLong[5] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongG"))
						{
							vars->Globals.VetStoreLong[6] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongH"))
						{
							vars->Globals.VetStoreLong[7] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongI"))
						{
							vars->Globals.VetStoreLong[8] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongJ"))
						{
							vars->Globals.VetStoreLong[9] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongK"))
						{
							vars->Globals.VetStoreLong[10] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongL"))
						{
							vars->Globals.VetStoreLong[11] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongM"))
						{
							vars->Globals.VetStoreLong[12] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongN"))
						{
							vars->Globals.VetStoreLong[13] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongO"))
						{
							vars->Globals.VetStoreLong[14] = GetInteger(-1);
							return;
						}
						if (!strcmp(field, "StoreLongP"))
						{
							vars->Globals.VetStoreLong[15] = GetInteger(-1);
							return;
						}
						break;

					case 'S':
						if (length >= 11)
						{
							switch (field[10])
							{
							case 'A':
								if (!strcmp(field, "StoreShortA1"))
								{
									vars->Globals.VetStoreShort[0] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortA2"))
								{
									vars->Globals.VetStoreShort[1] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'B':
								if (!strcmp(field, "StoreShortB1"))
								{
									vars->Globals.VetStoreShort[2] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortB2"))
								{
									vars->Globals.VetStoreShort[3] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'C':
								if (!strcmp(field, "StoreShortC1"))
								{
									vars->Globals.VetStoreShort[4] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortC2"))
								{
									vars->Globals.VetStoreShort[5] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'D':
								if (!strcmp(field, "StoreShortD1"))
								{
									vars->Globals.VetStoreShort[6] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortD2"))
								{
									vars->Globals.VetStoreShort[7] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'E':
								if (!strcmp(field, "StoreShortE1"))
								{
									vars->Globals.VetStoreShort[8] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortE2"))
								{
									vars->Globals.VetStoreShort[9] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'F':
								if (!strcmp(field, "StoreShortF1"))
								{
									vars->Globals.VetStoreShort[10] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortF2"))
								{
									vars->Globals.VetStoreShort[11] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'G':
								if (!strcmp(field, "StoreShortG1"))
								{
									vars->Globals.VetStoreShort[12] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortG2"))
								{
									vars->Globals.VetStoreShort[13] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'H':
								if (!strcmp(field, "StoreShortH1"))
								{
									vars->Globals.VetStoreShort[14] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortH2"))
								{
									vars->Globals.VetStoreShort[15] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'I':
								if (!strcmp(field, "StoreShortI1"))
								{
									vars->Globals.VetStoreShort[16] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortI2"))
								{
									vars->Globals.VetStoreShort[17] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'J':
								if (!strcmp(field, "StoreShortJ1"))
								{
									vars->Globals.VetStoreShort[18] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortJ2"))
								{
									vars->Globals.VetStoreShort[19] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'K':
								if (!strcmp(field, "StoreShortK1"))
								{
									vars->Globals.VetStoreShort[20] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortK2"))
								{
									vars->Globals.VetStoreShort[21] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'L':
								if (!strcmp(field, "StoreShortL1"))
								{
									vars->Globals.VetStoreShort[22] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortL2"))
								{
									vars->Globals.VetStoreShort[23] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'M':
								if (!strcmp(field, "StoreShortM1"))
								{
									vars->Globals.VetStoreShort[24] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortM2"))
								{
									vars->Globals.VetStoreShort[25] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'N':
								if (!strcmp(field, "StoreShortN1"))
								{
									vars->Globals.VetStoreShort[26] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortN2"))
								{
									vars->Globals.VetStoreShort[27] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'O':
								if (!strcmp(field, "StoreShortO1"))
								{
									vars->Globals.VetStoreShort[28] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortO2"))
								{
									vars->Globals.VetStoreShort[29] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;

							case 'P':
								if (!strcmp(field, "StoreShortP1"))
								{
									vars->Globals.VetStoreShort[30] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								if (!strcmp(field, "StoreShortP2"))
								{
									vars->Globals.VetStoreShort[31] = GetClampedInteger(-1, -32768, 32767, false);
									return;
								}
								break;
							}
						}
					}
				}
				break;

			case 'T':
				if (!strcmp(field, "Text1"))
				{
					CopyString(vars->Globals.VetTextVar[0].Text, GetLuaString(-1));
					return;
				}
				if (!strcmp(field, "Text2"))
				{
					CopyString(vars->Globals.VetTextVar[1].Text, GetLuaString(-1));
					return;
				}
				if (!strcmp(field, "Text3"))
				{
					CopyString(vars->Globals.VetTextVar[2].Text, GetLuaString(-1));
					return;
				}
				if (!strcmp(field, "Text4"))
				{
					CopyString(vars->Globals.VetTextVar[3].Text, GetLuaString(-1));
					return;
				}
				break;
			}
		}
		LuaObjectClass::NewIndex(field);
	}
}
