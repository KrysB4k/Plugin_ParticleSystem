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
			Script::ThrowError(FormatString("at most %d arguments expected", maximum));
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

	int GetClampedInteger(int argument, int min, int max, bool throwBoundsError)
	{
		int x;

		x = GetInteger(argument);
		if (x < min)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is less than the minimum of %d, aborting", x, min));
			Script::EmitWarning(FormatString("%d is less than the minimum of %d, clamping to minimum", x, min));
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is less than the maximum of %d, aborting", x, min));
			Script::EmitWarning(FormatString("%d is greater than the maximum of %d, clamping to maximum", x, max));
			return max;
		}
		return x;
	}

	float GetClampedNumber(int argument, float min, float max, bool throwBoundsError)
	{
		float x;

		x = GetNumber(argument);
		if (x < min)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is less than the minimum of %d, aborting", x, min));
			Script::EmitWarning(FormatString("%f is less than the minimum of %f, clamping to minimum", x, min));
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%d is less than the maximum of %d, aborting", x, min));
			Script::EmitWarning(FormatString("%f is greater than the maximum of %f, clamping to maximum", x, max));

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
	PerformTriggerGroupFunction PerformTriggergroup;
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
		if (!strcmp(field, "performTriggerGroup"))
			return &PerformTriggergroup;
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

std::optional<int> LuaGlobals::RetrieveIntegerConstant(const char* field)
{
	switch (field[0])
	{
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
	}
	return std::nullopt;
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
				B = 255 * GetClampedNumber(-1, 0.0f, 1.0f, false);
				return;
			}
			break;
		case 'g':
			if (!strcmp(field, "g"))
			{
				G = 255 * GetClampedNumber(-1, 0.0f, 1.0f, false);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "r"))
			{
				R = 255 * GetClampedNumber(-1, 0.0f, 1.0f, false);
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
				x = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false);
				return;
			}
			break;
		case 'y':
			if (!strcmp(field, "y"))
			{
				y = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false);
				return;
			}
			break;
		case 'z':
			if (!strcmp(field, "z"))
			{
				z = 16384 * GetClampedNumber(-1, 0.0f, 65536.0f, false);
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
				cutoff = GetClampedInteger(-1, 0, 32767, false);
				return;
			}
			break;
		case 'r':
			if (!strcmp(field, "random"))
			{
				random = GetClampedInteger(-1, 0, 32767, false);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "tether"))
			{
				tether = static_cast<TetherType>(GetClampedInteger(-1, 0, 2, false));
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
				blendingMode = GetClampedInteger(-1, 0, 13, false);
				return;
			}
			break;
		case 'd':
			if (!strcmp(field, "drawMode"))
			{
				drawMode = static_cast<DrawMode>(GetClampedInteger(-1, 0, 3, false));
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
				emitterIndex = GetClampedInteger(-1, -1, level_items - 1, false);
				if (emitterIndex != -1)
					emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
				else
					emitterNode = -1;
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
				lifeCounter = lifeSpan = GetClampedInteger(-1, 0, 32767, false);
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
				colorFadeTime = GetClampedInteger(-1, -32768, 32767, false);
				return;
			}
			break;
		case 'f':
			if (!strcmp(field, "fadeIn"))
			{
				fadeIn = GetClampedInteger(-1, 0, 32767, false);
				return;
			}
			if (!strcmp(field, "fadeOut"))
			{
				fadeOut = GetClampedInteger(-1, 0, 32767, false);
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
				sizeCust = GetClampedInteger(-1, 0, 65535, false);
				return;
			}
			if (!strcmp(field, "sizeEnd"))
			{
				sizeEnd = GetClampedInteger(-1, 0, 65535, false);
				return;
			}
			if (!strcmp(field, "sizeStart"))
			{
				sizeStart = GetClampedInteger(-1, 0, 65535, false);
				return;
			}
			if (!strcmp(field, "sizeRatio"))
			{
				sizeRatio = 32767 * GetClampedNumber(-1, -1.0f, 1.0f, false);
				return;
			}
			if (!strcmp(field, "spriteIndex"))
			{
				spriteIndex = GetClampedInteger(-1, 0, (-objects[ParticleFactory::partGroups[groupIndex].spriteSlot].nmeshes) - 1, false);
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
				mesh = Clamp(mesh, 0, objects[object].nmeshes - 1);
				return;
			}
			break;
		case 't':
			if (!strcmp(field, "transparency"))
			{
				transparency = GetClampedInteger(-1, 0, 255, false);
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
	float factor = GetClampedNumber(2, 0.0f, 1.0f, false);
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
	float rebound = GetClampedNumber(2, 0.0f, 1.0f, false);
	float minbounce = GetNumber(3);
	int margin = GetInteger(4);
	bool accurate = GetBoolean(5);
	Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
	return 1;
}

int ParticleCollideWallsFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float rebound = GetClampedNumber(2, 0.0f, 1.0f, false);
	Script::PushBoolean(part->CollideWalls(rebound));
	return 1;
}

int ParticleHomingFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = GetItem(2);
	int node = item ? GetClampedInteger(3, 0, objects[item->object_number].nmeshes, false) : GetInteger(3);
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

int PerformTriggerGroupFunction::Call()
{
	int indexTG = GetClampedInteger(1, 1, 9999, true);
	bool state = PerformTriggerGroup(indexTG) ? true : false;
	Script::PushBoolean(state);
	return 1;
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
	std::optional<int> opt;

	if (field)
	{
		object = LuaGlobals::RetrieveFunction(field);
		if (object)
		{
			Script::PushData(object);
			return;
		}
		opt = LuaGlobals::RetrieveIntegerConstant(field);
		if (opt)
		{
			Script::PushInteger(*opt);
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
		if (LuaGlobals::RetrieveFunction(field))
			Script::ThrowError("attempt to assign to a built-in function");
		if (LuaGlobals::RetrieveIntegerConstant(field))
			Script::ThrowError("attempt to assign to a built-in constant");
		Script::ThrowError("attempt to write to a global variable");
	}
	Script::ThrowError("attempt to index the global environment");
}

int LuaBridge::GlobalCall()
{
	Script::ThrowError("attempt to call the global environment");
}
