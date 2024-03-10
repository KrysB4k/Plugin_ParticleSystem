#include "..\definitions\includes.h"

namespace
{
	template<class T>
	T* GetData(int argument)
	{
		T* object;

		if (!Script::IsData(argument))
			Script::ThrowError(FormatString("%s expected", T::Name()));
		object = dynamic_cast<T*>(Script::ToData(argument));
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

	int GetItemIndex(int argument)
	{
		int index;
		index = GetInteger(argument);
		if (index < 0 || index >= level_items)
			Script::ThrowError(FormatString("%d does not correspond to a valid Tomb index", index));
		return index;
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
				Script::ThrowError(FormatString("%d is greater than the maximum of %d, aborting", x, max));
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
				Script::ThrowError(FormatString("%f is less than the minimum of %f, aborting", x, min));
			Script::EmitWarning(FormatString("%f is less than the minimum of %f, clamping to minimum", x, min));
			return min;
		}
		if (x > max)
		{
			if (throwBoundsError)
				Script::ThrowError(FormatString("%f is greater than the maximum of %f, aborting", x, max));
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

	template<class T, class... Args>
	std::enable_if_t<std::is_trivially_destructible_v<T>, T>* ConstructManagedData(Args&&... args)
	{
		return new(Script::CreateManagedData(sizeof(T))) T(std::forward<Args>(args)...);
	}

	void ReadOnlyFieldError(const char* field)
	{
		Script::ThrowError(FormatString("field \"%s\" is read-only and cannot be assigned to", field));
	}

	int GetTable(int argument)
	{
		if (!Script::IsTable(argument))
			Script::ThrowError("table expected");
		int length = Script::ExplodeTable(argument);
		if (!length)
			Script::ThrowError("table is empty");
		return length;
	}

	Vector3f GetItemPos(int argument)
	{
		auto item = &items[GetItemIndex(argument)];
		return Vector3f(item->pos.xPos, item->pos.yPos, item->pos.zPos);
	}
}

namespace LuaGlobals
{
	AbsFunction AbsFunc;
	AcosFunction AcosFunc;
	AsinFunction AsinFunc;
	AtanFunction AtanFunc;
	Atan2Function Atan2Func;
	BoidAlignmentFunction BoidAlignmentFunc;
	BoidCohesionFunction BoidCohesionFunc;
	BoidSeparationFunction BoidSeparationFunc;
	CbrtFunction CbrtFunc;
	CheckDistFastFunction CheckDistFastFunc;
	ClampFloatFunction ClampFloatFunc;
	ClampIntFunction ClampIntFunc;
	CosFunction CosFunc;
	CreateColorFunction CreateColorFunc;
	CreateGroupFunction CreateGroupFunc;
	CreateMeshPartFunction CreateMeshPartFunc;
	CreatePerlinNoiseFunction CreatePerlinNoiseFunc;
	CreateSimplexNoiseFunction CreateSimplexNoiseFunc;
	CreateSpritePartFunction CreateSpritePartFunc;
	CreateVectorFunction CreateVectorFunc;
	FindNearestTargetFunction FindNearestTargetFunc;
	GetColorFromHSVFunction GetColorFromHSVFunc;
	GetDistanceFunction GetDistanceFunc;
	GetGameTickFunction GetGameTickFunc;
	GetItemInfoFunction GetItemInfoFunc;
	GetItemJointPosFunction GetItemJointPosFunc;
	GetItemRoomFunction GetItemRoomFunc;
	GetLaraIndexFunction GetLaraIndexFunc;
	GetTombIndexFunction GetTombIndexFunc;
	LerpFunction LerpFunc;
	LerpInverseFunction LerpInverseFunc;
	MeshAlignVelocityFunction MeshAlignVelocityFunc;
	MeshLookAtTargetFunction MeshLookAtTargetFunc;
	MeshShatterFunction MeshShatterFunc;
	NoiseFunction NoiseFunc;
	NoiseCurlFunction NoiseCurlFunc;
	NoiseCurlTimeFunction NoiseCurlTimeFunc;
	ParticleAbsPosFunction ParticleAbsPosFunc;
	ParticleAnimateFunction ParticleAnimateFunc;
	ParticleAttachFunction ParticleAttachFunc;
	ParticleAttractToItemFunction ParticleAttractToItemFunc;
	ParticleAvoidRoomGeometryFunction ParticleAvoidRoomGeometryFunc;
	ParticleCollidedItemFunction ParticleCollidedItemFunc;
	ParticleCollideFloorsFunction ParticleCollideFloorsFunc;
	ParticleCollideWallsFunction ParticleCollideWallsFunc;
	ParticleDetachFunction ParticleDetachFunc;
	ParticleFollowTargetFunction ParticleFollowTargetFunc;
	ParticleHomingFunction ParticleHomingFunc;
	ParticleLimitSpeedFunction ParticleLimitSpeedFunc;
	ParticleWindVelocityFunction ParticleWindVelocityFunc;
	PerformTriggerGroupFunction PerformTriggerGroupFunc;
	PrintFunction PrintFunc;
	RandfloatFunction RandfloatFunc;
	RandintFunction RandintFunc;
	RoundFunction RoundFunc;
	SelectItemFunction SelectItemFunc;
	SinFunction SinFunc;
	SphericalToCartesianFunction SphericalToCartesianFunc;
	SplinePosItemsFunction SplinePosItemsFunc;
	SplinePosVectorsFunction SplinePosVectorsFunc;
	SplineVelItemsFunction SplineVelItemsFunc;
	SplineVelVectorsFunction SplineVelVectorsFunc;
	SoundEffectFunction SoundEffectFunc;
	SqrtFunction SqrtFunc;
	TestCollisionSpheresFunction TestCollisionSpheresFunc;
	TriggerDynamicFunction TriggerDynamicFunc;
	TriggerShockwaveFunction TriggerShockwaveFunc;
}

LuaObject* LuaGlobals::RetrieveFunction(const char* field)
{
	switch (field[0])
	{
	case 'a':
		if (!strcmp(field, "abs"))
			return &AbsFunc;
		if (!strcmp(field, "acos"))
			return &AcosFunc;
		if (!strcmp(field, "asin"))
			return &AsinFunc;
		if (!strcmp(field, "atan"))
			return &AtanFunc;
		if (!strcmp(field, "atan2"))
			return &Atan2Func;
		break;
	case 'b':
		if (!strcmp(field, "boidAlignment"))
			return &BoidAlignmentFunc;
		if (!strcmp(field, "boidCohesion"))
			return &BoidCohesionFunc;
		if (!strcmp(field, "boidSeparation"))
			return &BoidSeparationFunc;
		break;
	case 'c':
		if (!strcmp(field, "cbrt"))
			return &CbrtFunc;
		if (!strcmp(field, "checkDistFast"))
			return &CheckDistFastFunc;
		if (!strcmp(field, "clampfloat"))
			return &ClampFloatFunc;
		if (!strcmp(field, "clampint"))
			return &ClampIntFunc;
		if (!strcmp(field, "cos"))
			return &CosFunc;
		if (!strcmp(field, "createColor"))
			return &CreateColorFunc;
		if (!strcmp(field, "createGroup"))
			return &CreateGroupFunc;
		if (!strcmp(field, "createMeshPart"))
			return &CreateMeshPartFunc;
		if (!strcmp(field, "createPerlinNoise"))
			return &CreatePerlinNoiseFunc;
		if (!strcmp(field, "createSimplexNoise"))
			return &CreateSimplexNoiseFunc;
		if (!strcmp(field, "createSpritePart"))
			return &CreateSpritePartFunc;
		if (!strcmp(field, "createVector"))
			return &CreateVectorFunc;
		break;
	case 'f':
		if (!strcmp(field, "findNearestTarget"))
			return &FindNearestTargetFunc;
		break;
	case 'g':
		if (!strcmp(field, "getColorFromHSV"))
			return &GetColorFromHSVFunc;
		if (!strcmp(field, "getDistance"))
			return &GetDistanceFunc;
		if (!strcmp(field, "getGameTick"))
			return &GetGameTickFunc;
		if (!strcmp(field, "getItemJointPosition"))
			return &GetItemJointPosFunc;
		if (!strcmp(field, "getItemInfo"))
			return &GetItemInfoFunc;
		if (!strcmp(field, "getItemRoom"))
			return &GetItemRoomFunc;
		if (!strcmp(field, "getLaraIndex"))
			return &GetLaraIndexFunc;
		if (!strcmp(field, "getTombIndex"))
			return &GetTombIndexFunc;
		break;
	case 'l':
		if (!strcmp(field, "lerp"))
			return &LerpFunc;
		if (!strcmp(field, "lerpInverse"))
			return &LerpInverseFunc;
		break;
	case 'm':
		if (!strcmp(field, "meshAlignVelocity"))
			return &MeshAlignVelocityFunc;
		if (!strcmp(field, "meshLookAtTarget"))
			return &MeshLookAtTargetFunc;
		if (!strcmp(field, "meshShatter"))
			return &MeshShatterFunc;
		break;
	case 'n':
		if (!strcmp(field, "noise"))
			return &NoiseFunc;
		if (!strcmp(field, "noiseCurl"))
			return &NoiseCurlFunc;
		if (!strcmp(field, "noiseCurlTime"))
			return &NoiseCurlTimeFunc;
		break;
	case 'p':
		if (!strcmp(field, "particleAbsPosition"))
			return &ParticleAbsPosFunc;
		if (!strcmp(field, "particleAnimate"))
			return &ParticleAnimateFunc;
		if (!strcmp(field, "particleAttach"))
			return &ParticleAttachFunc;
		if (!strcmp(field, "particleAttractToItem"))
			return &ParticleAttractToItemFunc;
		if (!strcmp(field, "particleAvoidRoomGeometry"))
			return &ParticleAvoidRoomGeometryFunc;
		if (!strcmp(field, "particleCollidedItem"))
			return &ParticleCollidedItemFunc;
		if (!strcmp(field, "particleCollideFloors"))
			return &ParticleCollideFloorsFunc;
		if (!strcmp(field, "particleCollideWalls"))
			return &ParticleCollideWallsFunc;
		if (!strcmp(field, "particleDetach"))
			return &ParticleDetachFunc;
		if (!strcmp(field, "particleFollow"))
			return &ParticleFollowTargetFunc;
		if (!strcmp(field, "particleHoming"))
			return &ParticleHomingFunc;
		if (!strcmp(field, "particleLimitSpeed"))
			return &ParticleLimitSpeedFunc;
		if (!strcmp(field, "particleWind"))
			return &ParticleWindVelocityFunc;
		if (!strcmp(field, "performTriggerGroup"))
			return &PerformTriggerGroupFunc;
		if (!strcmp(field, "print"))
			return &PrintFunc;
		break;
	case 'r':
		if (!strcmp(field, "randfloat"))
			return &RandfloatFunc;
		if (!strcmp(field, "randint"))
			return &RandintFunc;
		if (!strcmp(field, "round"))
			return &RoundFunc;
		break;
	case 's':
		if (!strcmp(field, "setActiveItem"))
			return &SelectItemFunc;
		if (!strcmp(field, "sin"))
			return &SinFunc;
		if (!strcmp(field, "soundEffect"))
			return &SoundEffectFunc;
		if (!strcmp(field, "sphericalToCartesian"))
			return &SphericalToCartesianFunc;
		if (!strcmp(field, "splinePosItems"))
			return &SplinePosItemsFunc;
		if (!strcmp(field, "splinePosVectors"))
			return &SplinePosVectorsFunc;
		if (!strcmp(field, "splineVelItems"))
			return &SplineVelItemsFunc;
		if (!strcmp(field, "splineVelVectors"))
			return &SplineVelVectorsFunc;
		if (!strcmp(field, "sqrt"))
			return &SqrtFunc;
		break;
	case 't':
		if (!strcmp(field, "testCollisionSpheres"))
			return &TestCollisionSpheresFunc;
		if (!strcmp(field, "triggerDynamicLight"))
			return &TriggerDynamicFunc;
		if (!strcmp(field, "triggerShockwave"))
			return &TriggerShockwaveFunc;
		break;
	}
	return nullptr;
}

std::optional<int> LuaGlobals::RetrieveIntegerConstant(const char* field)
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
			return std::optional(DrawMode::DRAW_SPRITE);
		if (!strcmp(field, "DRAW_SQUARE"))
			return std::optional(DrawMode::DRAW_SQUARE);
		if (!strcmp(field, "DRAW_LINE"))
			return std::optional(DrawMode::DRAW_LINE);
		if (!strcmp(field, "DRAW_ARROW"))
			return std::optional(DrawMode::DRAW_ARROW);
		if (!strcmp(field, "DRAW_NONE"))
			return std::optional(DrawMode::DRAW_NONE);
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
			return std::optional(TetherType::TETHER_ROTATING);
		if (!strcmp(field, "TETHER_STATIC"))
			return std::optional(TetherType::TETHER_STATIC);
		if (!strcmp(field, "TETHER_NONE"))
			return std::optional(TetherType::TETHER_NONE);
		break;
	}
	return std::nullopt;
}

std::optional<float> LuaGlobals::RetrieveFloatConstant(const char* field)
{
	switch (field[0])
	{
	case 'P':
		if (!strcmp(field, "PI"))
			return std::optional(M_PI);
		if (!strcmp(field, "PI_HALF"))
			return std::optional(M_PI_2);
		if (!strcmp(field, "PI_QUART"))
			return std::optional(M_PI_4);
		if (!strcmp(field, "PI_TWO"))
			return std::optional(M_PI * 2);
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
	return "Vector3f or ItemPos";
}

const char* LuaObjectClassRotation::Name()
{
	return "Vector3s or ItemRot";
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

float Vector3f::GetX()
{
	return x;
}

float Vector3f::GetY()
{
	return y;
}

float Vector3f::GetZ()
{
	return z;
}

Vector3f Vector3f::GetVector()
{
	return *this;
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

Vector3s Vector3s::GetVector()
{
	return *this;
}

const char* LuaItemInfoPos::Name()
{
	return "ItemPos";
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

Vector3f LuaItemInfoPos::GetVector()
{
	return Vector3f(pos->xPos, pos->yPos, pos->zPos);
}

const char* LuaItemInfoRot::Name()
{
	return "ItemRot";
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

Vector3s LuaItemInfoRot::GetVector()
{
	return Vector3s(pos->xRot, pos->yRot, pos->zRot);
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
		case 'a':
			if (!strcmp(field, "attach"))
			{
				attach = *GetData<NodeAttachment>(-1);
				return;
			}
			break;
		case 'b':
			if (!strcmp(field, "blendingMode"))
			{
				blendingMode = GetClampedInteger(-1, BlendMode::BLEND_TEXTURE, BlendMode::BLEND_CUSTOM_13, false);
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
				accel = GetData<LuaObjectClassPosition>(-1)->GetVector();
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
		case 'p':
			if (!strcmp(field, "pos"))
			{
				pos = GetData<LuaObjectClassPosition>(-1)->GetVector();
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
				vel = GetData<LuaObjectClassPosition>(-1)->GetVector();
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
		case 'r':
			if (!strcmp(field, "rot"))
			{
				rot = GetData<LuaObjectClassRotation>(-1)->GetVector();
				return;
			}
			if (!strcmp(field, "rotVel"))
			{
				rotVel = GetData<LuaObjectClassRotation>(-1)->GetVector();
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
				transparency = GetClampedInteger(-1, 0, 255, false);
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

const char* LuaItemInfoWrapper::Name()
{
	return "ItemInfo";
}

void LuaItemInfoWrapper::Index(const char* field)
{
	if (field && itemptr)
	{
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
	if (field && itemptr)
	{
		switch (field[0])
		{
		case 'a':
			if (!strcmp(field, "animNumber"))
			{
				int anim = GetClampedInteger(-1, 0, 1000, false) + objects[itemptr->object_number].anim_index;
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
				itemptr->pos.xPos = Round(position->GetX());
				itemptr->pos.yPos = Round(position->GetY());
				itemptr->pos.zPos = Round(position->GetZ());
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

int CheckDistFastFunction::Call()
{
	auto vec1 = GetData<LuaObjectClassPosition>(1)->GetVector();
	auto vec2 = GetData<LuaObjectClassPosition>(2)->GetVector();
	float dist = GetNumber(3);
	Script::PushInteger(CheckDistFast(vec1, vec2, dist));
	return 1;
}

int ClampFloatFunction::Call()
{
	float x = GetNumber(1);
	float min = GetNumber(2);
	float max = GetNumber(3);
	Script::PushNumber(Clamp(x, min, max));
	return 1;
}

int ClampIntFunction::Call()
{
	int x = GetInteger(1);
	int min = GetInteger(2);
	int max = GetInteger(3);
	Script::PushNumber(Clamp(x, min, max));
	return 1;
}

int CosFunction::Call()
{
	Script::PushNumber(GetMathResult(1, cosf));
	return 1;
}

int CreateColorFunction::Call()
{
	uchar r = 255 * GetClampedNumber(1, 0.0f, 1.0f, false);
	uchar g = 255 * GetClampedNumber(2, 0.0f, 1.0f, false);
	uchar b = 255 * GetClampedNumber(3, 0.0f, 1.0f, false);
	ConstructManagedData<ColorRGB>(r, g, b);
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

int CreatePerlinNoiseFunction::Call()
{
	CheckCaller(FUNCTION_LIBRARY, "createPerlinNoise");

	int args = GetArgCount(0, 1);

	if (args)
		ConstructManagedData<PerlinNoise>(GetInteger(1));
	else
		ConstructManagedData<PerlinNoise>();

	return 1;
}

int CreateSimplexNoiseFunction::Call()
{
	CheckCaller(FUNCTION_LIBRARY, "createSimplexNoise");

	int args = GetArgCount(0, 1);

	if (args)
		ConstructManagedData<SimplexNoise>(GetInteger(1));
	else
		ConstructManagedData<SimplexNoise>();

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

int CreateVectorFunction::Call()
{
	float x = GetNumber(1);
	float y = GetNumber(2);
	float z = GetNumber(3);
	ConstructManagedData<Vector3f>(x, y, z);
	return 1;
}

int FindNearestTargetFunction::Call()
{
	auto vec = GetData<LuaObjectClassPosition>(1)->GetVector();
	float radius = GetNumber(2);
	std::vector<short> slotList(GetTable(3));
	for (int i = 0; i < slotList.size(); i++)
		slotList[i] = GetClampedInteger(i + 4, SLOT_LARA, SLOT_NEW_SLOT18, true);
	Script::PushInteger(FindNearestTarget(vec, radius, slotList.data(), slotList.size()));
	return 1;
}

int GetColorFromHSVFunction::Call()
{
	float h = GetNumber(1);
	float s = GetClampedNumber(2, 0.0f, 1.0f, false);
	float v = GetClampedNumber(3, 0.0f, 1.0f, false);
	ConstructManagedData<ColorRGB>(HSVtoRGB(h, s, v));
	return 1;
}

int GetDistanceFunction::Call()
{
	auto vec1 = GetData<LuaObjectClassPosition>(1)->GetVector();
	auto vec2 = GetData<LuaObjectClassPosition>(2)->GetVector();
	Script::PushNumber(RealDist(vec1, vec2));
	return 1;
}

int GetGameTickFunction::Call()
{
	Script::PushInteger(ParticleFactory::gameTick);
	return 1;
}

int GetItemInfoFunction::Call()
{
	ConstructManagedData<LuaItemInfoWrapper>(&items[GetItemIndex(1)]);
	return 1;
}

int GetItemJointPosFunction::Call()
{
	auto item = &items[GetItemIndex(1)];
	int joint = GetClampedInteger(2, 0, objects[item->object_number].nmeshes - 1, false);
	int offX = GetInteger(3);
	int offY = GetInteger(4);
	int offZ = GetInteger(5);

	ConstructManagedData<Vector3f>(GetJointPos(item, joint, offX, offY, offZ));
	return 1;
}

int GetItemRoomFunction::Call()
{
	auto item = &items[GetItemIndex(1)];
	Script::PushInteger(item->room_number);
	return 1;
}

int GetLaraIndexFunction::Call()
{
	Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
	return 1;
}

int GetTombIndexFunction::Call()
{
	Script::PushInteger(GetTombIndexByNGLEIndex(1));
	return 1;
}

int LerpFunction::Call()
{
	float a = GetNumber(1);
	float b = GetNumber(2);
	float t = GetClampedNumber(3, 0.0f, 1.0f, false);
	Script::PushNumber(Lerp(a, b, t));
	return 1;
}

int LerpInverseFunction::Call()
{
	float val1 = GetNumber(1);
	float val2 = GetNumber(2);
	float x = GetNumber(3);
	Script::PushNumber(InverseLerp(val1, val2, x));
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

int MeshLookAtTargetFunction::Call()
{
	auto part = GetData<MeshParticle>(1);
	auto vector = GetData<LuaObjectClassPosition>(2)->GetVector();
	float factor = GetClampedNumber(3, 0.0, 1.0f, false);
	bool invert = GetBoolean(4);
	part->AlignToTarget(vector, factor, invert);
	return 0;
}

int MeshShatterFunction::Call()
{
	auto part = GetData<MeshParticle>(1);
	part->Shatter();
	return 0;
}

int NoiseFunction::Call()
{
	float scale, x, y, z, w;

	int count = GetArgCount(3, 6);
	auto noise = GetData<Noise>(1);

	switch (count)
	{
	case 3:
		scale = GetNumber(2);
		x = GetNumber(3);
		if (scale)
			x /= scale;
		Script::PushNumber(noise->Noise1D(x));
		return 1;

	case 4:
		scale = GetNumber(2);
		x = GetNumber(3);
		y = GetNumber(4);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
		}
		Script::PushNumber(noise->Noise2D(x, y));
		return 1;

	case 5:
		scale = GetNumber(2);
		x = GetNumber(3);
		y = GetNumber(4);
		z = GetNumber(5);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
		}
		Script::PushNumber(noise->Noise3D(x, y, z));
		return 1;

	default:
		scale = GetNumber(2);
		x = GetNumber(3);
		y = GetNumber(4);
		z = GetNumber(5);
		w = GetNumber(6);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;
		}
		Script::PushNumber(noise->Noise4D(x, y, z, w));
		return 1;
	}
}

int NoiseCurlFunction::Call()
{
	float scale, x, y, z;
	Vector3f vector;
	int count = GetArgCount(4, 5);
	auto noise = GetData<Noise>(1);

	switch (count)
	{
	case 4:
		scale = GetNumber(2);
		x = GetNumber(3);
		y = GetNumber(4);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
		}
		vector = noise->Curl2D(x, y);
		ConstructManagedData<Vector3f>(vector.x, vector.y, 0);
		return 1;

	default:
		scale = GetNumber(2);
		x = GetNumber(3);
		y = GetNumber(4);
		z = GetNumber(5);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
		}
		vector = noise->Curl3D(x, y, z);
		ConstructManagedData<Vector3f>(vector.x, vector.y, vector.z);
		return 1;
	}
}

int NoiseCurlTimeFunction::Call()
{
	float scale, time, x, y, z;
	Vector3f vector;
	int count = GetArgCount(5, 6);
	auto noise = GetData<Noise>(1);

	switch (count)
	{
	case 5:
		scale = GetNumber(2);
		time = GetNumber(3);
		x = GetNumber(4);
		y = GetNumber(5);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
		}
		vector = noise->Curl2DTime(time, x, y);
		ConstructManagedData<Vector3f>(vector.x, vector.y, 0);
		return 1;

	default:
		scale = GetNumber(2);
		time = GetNumber(3);
		x = GetNumber(4);
		y = GetNumber(5);
		z = GetNumber(6);
		if (scale)
		{
			scale = 1.0f / scale;
			x *= scale;
			y *= scale;
			z *= scale;
		}
		vector = noise->Curl3DTime(time, x, y, z);
		ConstructManagedData<Vector3f>(vector.x, vector.y, vector.z);
		return 1;
	}
}

int ParticleAbsPosFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	ConstructManagedData<Vector3f>(part->AbsPos());
	return 1;
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

int ParticleAttachFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	int index = GetItemIndex(2);
	int node = GetClampedInteger(3, -1, objects[items[index].object_number].nmeshes - 1, false);
	part->Attach(index, node);
	return 0;
}

int ParticleAttractToItemFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = &items[GetItemIndex(2)];
	float radius = GetNumber(3);
	float factor = GetNumber(4);
	part->vel = part->AttractToItem(item, radius, factor);
	return 0;
}

int ParticleAvoidRoomGeometryFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	int wallMargin = GetInteger(2);
	int floorMargin = GetInteger(3);
	float factor = GetNumber(4);
	part->vel += part->AvoidRoomGeometry(wallMargin, floorMargin, factor);

	return 0;
}

int ParticleCollidedItemFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = &items[GetItemIndex(2)];
	int radius = GetInteger(3);
	Script::PushBoolean(part->CollidedWithItem(item, radius));
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

int ParticleDetachFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	part->Detach();
	return 0;
}

int ParticleFollowTargetFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto vect = GetData<LuaObjectClassPosition>(2)->GetVector();
	float maxSpeed = GetNumber(3);
	float distInner = GetNumber(4);
	float distOuter = GetNumber(5);
	part->vel = part->FollowTarget(vect, maxSpeed, distInner, distOuter);
	return 0;
}

int ParticleHomingFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	auto item = &items[GetItemIndex(2)];
	int node = item ? GetClampedInteger(3, 0, objects[item->object_number].nmeshes, false) : GetInteger(3);
	float factor = GetNumber(4);
	float accel = GetNumber(5);
	bool predict = GetBoolean(6);
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

int ParticleWindVelocityFunction::Call()
{
	auto part = GetData<BaseParticle>(1);
	float factor = GetNumber(2);
	part->vel += part->WindVelocities(factor);
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

int RoundFunction::Call()
{
	Script::PushInteger(Round(GetNumber(1)));
	return 1;
}

int SelectItemFunction::Call()
{
	Trng.pGlobTomb4->ItemIndexSelected = Trng.pGlobTomb4->IndiceItemCondizione = GetItemIndex(1);
	return 0;
}

int SinFunction::Call()
{
	Script::PushNumber(GetMathResult(1, sinf));
	return 1;
}

int SoundEffectFunction::Call()
{
	int sfxIndex = GetInteger(1);
	int x = GetInteger(2);
	int y = GetInteger(3);
	int z = GetInteger(4);
	int flags = GetInteger(5);
	auto vec = phd_vector(x, y, z);
	SoundEffect(sfxIndex, &vec, flags);
	return 0;
}

int SphericalToCartesianFunction::Call()
{
	float r = GetNumber(1);
	float theta = GetNumber(2);
	float phi = GetClampedNumber(3, -M_PI_2, M_PI_2, false);
	ConstructManagedData<Vector3f>(SphericalToCartesian(r, theta, phi));
	return 1;
}

int SplinePosItemsFunction::Call()
{
	float t = GetNumber(1);
	std::vector<Vector3f> vecList(GetTable(2));
	for (int i = 0; i < vecList.size(); i++)
		vecList[i] = GetItemPos(i + 3);
	ConstructManagedData<Vector3f>(SplinePos(t, vecList.data(), vecList.size()));
	return 1;
}

int SplinePosVectorsFunction::Call()
{
	float t = GetNumber(1);
	std::vector<Vector3f> vecList(GetTable(2));
	for (int i = 0; i < vecList.size(); i++)
		vecList[i] = GetData<LuaObjectClassPosition>(i + 3)->GetVector();
	Vector3f vec = SplinePos(t, vecList.data(), vecList.size());
	ConstructManagedData<Vector3f>(vec);
	return 1;
}

int SplineVelItemsFunction::Call()
{
	float t = GetNumber(1);
	float duration = GetNumber(2);
	std::vector<Vector3f> vecList(GetTable(3));
	for (int i = 0; i < vecList.size(); i++)
		vecList[i] = GetItemPos(i + 4);
	ConstructManagedData<Vector3f>(SplineVel(t, duration, vecList.data(), vecList.size()));
	return 1;
}

int SplineVelVectorsFunction::Call()
{
	float t = GetNumber(1);
	float duration = GetNumber(2);
	std::vector<Vector3f> vecList(GetTable(3));
	for (int i = 0; i < vecList.size(); i++)
		vecList[i] = GetData<LuaObjectClassPosition>(i + 4)->GetVector();
	Vector3f vec = SplineVel(t, duration, vecList.data(), vecList.size());
	ConstructManagedData<Vector3f>(vec);
	return 1;
}

int SqrtFunction::Call()
{
	Script::PushNumber(GetMathResult(1, sqrtf));
	return 1;
}

int TestCollisionSpheresFunction::Call()
{
	auto item = &items[GetItemIndex(1)];
	auto vec = GetData<LuaObjectClassPosition>(2)->GetVector();
	float radius = GetNumber(3);
	Script::PushInteger(TestCollisionSpheres(item, vec, radius));
	return 1;
}

int TriggerDynamicFunction::Call()
{
	int x = GetInteger(1);
	int y = GetInteger(2);
	int z = GetInteger(3);
	int intensity = GetInteger(4);
	int red = Round(GetClampedNumber(5, 0, 1, false) * 255);
	int green = Round(GetClampedNumber(6, 0, 1, false) * 255);
	int blue = Round(GetClampedNumber(7, 0, 1, false) * 255);
	TriggerDynamic(x, y, z, intensity, red, green, blue);
	return 0;
}

int TriggerShockwaveFunction::Call()
{
	int x = GetInteger(1);
	int y = GetInteger(2);
	int z = GetInteger(3);
	int innerRad = GetClampedInteger(4, -32768, 32767, false);
	int outerRad = GetClampedInteger(5, -32768, 32767, false);
	int speed = GetInteger(6);
	int life = GetClampedInteger(7, 0, 255, false);
	int red = Round(GetClampedNumber(8, 0, 1, false) * 255);
	int green = Round(GetClampedNumber(9, 0, 1, false) * 255);
	int blue = Round(GetClampedNumber(10, 0, 1, false) * 255);
	int xRot = RadToShort(GetNumber(11));
	int flags = GetInteger(12);
	auto vec = phd_vector(x, y, z);
	TriggerShockwave(&vec, innerRad | (outerRad << 16), speed, RGBA(red, green, blue, life), xRot | (flags << 16));
	return 0;
}

void LuaBridge::GlobalIndex(const char* field)
{
	LuaObject* object;
	std::optional<int> opt_int;
	std::optional<float> opt_float;

	if (field)
	{
		object = LuaGlobals::RetrieveFunction(field);
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
		if (LuaGlobals::RetrieveFunction(field))
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
