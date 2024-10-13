#pragma once

struct Vector3f;
struct Vector3s;

struct LuaObjectClass : public LuaObject
{
	virtual int Call() final override;
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct LuaObjectClassPosition : public LuaObjectClass
{
	static const char* Name();

	virtual float GetX() = 0;
	virtual float GetY() = 0;
	virtual float GetZ() = 0;
	virtual explicit operator Vector3f() = 0;
};

struct LuaObjectClassRotation : public LuaObjectClass
{
	static const char* Name();

	virtual short GetX() = 0;
	virtual short GetY() = 0;
	virtual short GetZ() = 0;
	virtual explicit operator Vector3s() = 0;
};

struct LuaItemInfoPos final : public LuaObjectClassPosition
{
	LuaItemInfoPos(phd_3dpos* position) : pos(position) {}
	
	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;

	virtual float GetX() override;
	virtual float GetY() override;
	virtual float GetZ() override;
	virtual explicit operator Vector3f() override;

private:

	phd_3dpos* const pos;
};

struct LuaItemInfoRot final : public LuaObjectClassRotation
{
	LuaItemInfoRot(phd_3dpos* position) : pos(position) {}

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;

	virtual short GetX() override;
	virtual short GetY() override;
	virtual short GetZ() override;
	virtual explicit operator Vector3s() override;

private:

	phd_3dpos* const pos;
};

struct LuaItemInfoWrapper final : public LuaObjectClass
{
	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct TrngVarWrapper final : public LuaObjectClass
{
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct LuaObjectFunction : public LuaObject
{
	virtual void Index(const char* field) final override;
	virtual void NewIndex(const char* field) final override;
};

struct AbsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct AcosFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct AsinFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct AtanFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct Atan2Function final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct BoidAlignmentFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct BoidCohesionFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct BoidSeparationFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CbrtFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CheckDistFastFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ClampFloatFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ClampIntFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CosFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateColorFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateGroupFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateMeshPartFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreatePerlinNoiseFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateSimplexNoiseFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateSpritePartFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateVectorFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct FindNearestTargetFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetColorFromHSVFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetDistanceFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetGameTickFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetItemJointPosFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetItemInfoFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetItemRoomFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetLaraIndexFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetSelectedItemFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetTombIndexFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct KillPartsOfGroupFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct LerpFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct LerpInverseFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct MeshAlignVelocityFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct MeshLookAtTargetFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct MeshShatterFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct NoiseFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct NoiseCurlFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct NoiseCurlTimeFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAbsPosFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAnimateFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAttachFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAttractToItemFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAvoidRoomGeometryFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleCollidedItemFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleCollideFloorsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleCollideWallsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleDetachFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleFollowTargetFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleHomingFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleLimitSpeedFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleWindVelocityFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct PerformTriggerGroupFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct PrintFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct RandfloatFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct RandintFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct RoundFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SelectItemFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SinFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SoundEffectFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SphericalToCartesianFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SplinePosItemsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SplinePosVectorsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SplineVelItemsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SplineVelVectorsFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SqrtFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct TestCollisionSpheresFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct TriggerDynamicFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct TriggerShockwaveFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

namespace LuaGlobals
{
	extern AbsFunction AbsFunc;
	extern AcosFunction AcosFunc;
	extern AsinFunction AsinFunc;
	extern AtanFunction AtanFunc;
	extern Atan2Function Atan2Func;
	extern BoidAlignmentFunction BoidAlignmentFunc;
	extern BoidCohesionFunction BoidCohesionFunc;
	extern BoidSeparationFunction BoidSeparationFunc;
	extern CbrtFunction CbrtFunc;
	extern CheckDistFastFunction CheckDistFastFunc;
	extern ClampFloatFunction ClampFloatFunc;
	extern ClampIntFunction ClampIntFunc;
	extern CosFunction CosFunc;
	extern CreateColorFunction CreateColorFunc;
	extern CreateGroupFunction CreateGroupFunc;
	extern CreateMeshPartFunction CreateMeshPartFunc;
	extern CreatePerlinNoiseFunction CreatePerlinNoiseFunc;
	extern CreateSimplexNoiseFunction CreateSimplexNoiseFunc;
	extern CreateSpritePartFunction CreateSpritePartFunc;
	extern CreateVectorFunction CreateVectorFunc;
	extern FindNearestTargetFunction FindNearestTargetFunc;
	extern GetColorFromHSVFunction GetColorFromHSVFunc;
	extern GetDistanceFunction GetDistanceFunc;
	extern GetGameTickFunction GetGameTickFunc;
	extern GetItemInfoFunction GetItemInfoFunc;
	extern GetItemJointPosFunction GetItemJointPosFunc;
	extern GetItemRoomFunction GetItemRoomFunc;
	extern GetLaraIndexFunction GetLaraIndexFunc;
	extern GetSelectedItemFunction GetSelectedItemFunc;
	extern GetTombIndexFunction GetTombIndexFunc;
	extern KillPartsOfGroupFunction KillPartsOfGroupFunc;
	extern LerpFunction LerpFunc;
	extern LerpInverseFunction LerpInverseFunc;
	extern MeshAlignVelocityFunction MeshAlignVelocityFunc;
	extern MeshLookAtTargetFunction MeshLookAtTargetFunc;
	extern MeshShatterFunction MeshShatterFunc;
	extern NoiseFunction NoiseFunc;
	extern NoiseCurlFunction NoiseCurlFunc;
	extern NoiseCurlTimeFunction NoiseCurlTimeFunc;
	extern ParticleAbsPosFunction ParticleAbsPosFunc;
	extern ParticleAnimateFunction ParticleAnimateFunc;
	extern ParticleAttachFunction ParticleAttachFunc;
	extern ParticleAttractToItemFunction ParticleAttractToItemFunc;
	extern ParticleAvoidRoomGeometryFunction ParticleAvoidRoomGeometryFunc;
	extern ParticleCollidedItemFunction ParticleCollidedItemFunc;
	extern ParticleCollideFloorsFunction ParticleCollideFloorsFunc;
	extern ParticleCollideWallsFunction ParticleCollideWallsFunc;
	extern ParticleDetachFunction ParticleDetachFunc;
	extern ParticleFollowTargetFunction ParticleFollowTargetFunc;
	extern ParticleHomingFunction ParticleHomingFunc;
	extern ParticleLimitSpeedFunction ParticleLimitSpeedFunc;
	extern SplinePosItemsFunction SplinePosItemsFunc;
	extern SplinePosVectorsFunction SplinePosVectorsFunc;
	extern SplineVelItemsFunction SplineVelItemsFunc;
	extern SplineVelVectorsFunction SplineVelVectorsFunc;
	extern ParticleWindVelocityFunction ParticleWindVelocityFunc;
	extern PerformTriggerGroupFunction PerformTriggerGroupFunc;
	extern PrintFunction PrintFunc;
	extern RandfloatFunction RandfloatFunc;
	extern RandintFunction RandintFunc;
	extern RoundFunction RoundFunc;
	extern SelectItemFunction SelectItemFunc;
	extern SinFunction SinFunc;
	extern SoundEffectFunction SoundEffectFunc;
	extern SphericalToCartesianFunction SphericalToCartesianFunc;
	extern SqrtFunction SqrtFunc;
	extern TestCollisionSpheresFunction TestCollisionSpheresFunc;
	extern TriggerDynamicFunction TriggerDynamicFunc;
	extern TriggerShockwaveFunction TriggerShockwaveFunc;
	extern TrngVarWrapper TrngVars;
	extern LuaItemInfoWrapper LuaItemArray[];

	LuaObject* RetrieveFunction(const char* field);
	LuaObject* RetrieveGlobals(const char* field);
	std::optional<int> RetrieveIntegerConstant(const char* field);
	std::optional<float> RetrieveFloatConstant(const char* field);
}
