#pragma once

struct LuaObjectClass : public LuaObject
{
	virtual int Call() final override;
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

struct CreateSpritePartFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct CreateVectorFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetTombIndexFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetLaraIndexFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct GetItemRoomFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct MeshAlignVelocityFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct MeshShatterFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct NoisePerlinFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct NoiseSimplexFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleAnimateFunction final : public LuaObjectFunction
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

struct ParticleHomingFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct ParticleLimitSpeedFunction final : public LuaObjectFunction
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

struct SeedNoiseFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SinFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

struct SqrtFunction final : public LuaObjectFunction
{
	virtual int Call() override;
};

namespace LuaGlobals
{
	extern AbsFunction Abs;
	extern AcosFunction Acos;
	extern AsinFunction Asin;
	extern AtanFunction Atan;
	extern Atan2Function Atan2;
	extern BoidAlignmentFunction BoidAlignment;
	extern BoidCohesionFunction BoidCohesion;
	extern BoidSeparationFunction BoidSeparation;
	extern CbrtFunction Cbrt;
	extern CosFunction Cos;
	extern CreateColorFunction CreateColor;
	extern CreateGroupFunction CreateGroup;
	extern CreateMeshPartFunction CreateMeshPart;
	extern CreateSpritePartFunction CreateSpritePart;
	extern CreateVectorFunction CreateVector;
	extern GetTombIndexFunction GetTombIndex;
	extern GetLaraIndexFunction GetLaraIndex;
	extern GetItemRoomFunction GetItemRoom;
	extern MeshAlignVelocityFunction MeshAlignVelocity;
	extern MeshShatterFunction MeshShatter;
	extern NoisePerlinFunction NoisePerlin;
	extern NoiseSimplexFunction NoiseSimplex;
	extern ParticleAnimateFunction ParticleAnimate;
	extern ParticleCollidedItemFunction ParticleCollidedItem;
	extern ParticleCollideFloorsFunction ParticleCollideFloors;
	extern ParticleCollideWallsFunction ParticleCollideWalls;
	extern ParticleHomingFunction ParticleHoming;
	extern ParticleLimitSpeedFunction ParticleLimitSpeed;
	extern PrintFunction Print;
	extern RandfloatFunction Randfloat;
	extern RandintFunction Randint;
	extern SeedNoiseFunction SeedNoise;
	extern SinFunction Sin;
	extern SqrtFunction Sqrt;

	LuaObject* RetrieveFunction(const char* field);
	std::optional<int> RetrieveIntegerConstant(const char* field);
}
