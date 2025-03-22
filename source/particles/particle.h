#pragma once
#include "vector3f.h"

namespace Diagnostics
{
	extern double performanceMultiplier;
	extern int activeSpriteParticles;
	extern int activeMeshParticles;
	extern double initTime;
	extern double updateTime;
	extern double drawTime;
	extern double initTimePeak;
	extern double updateTimePeak;
	extern double drawTimePeak;

	double Time(void (*targetFunction)());
	void SetPeaks();
	void Print();
	void Initialise();
	void ResetFrame();
	void ResetLevel();
}

// ************  Particle-related structs  ************ //

struct ColorRGB final : public LuaObjectClass
{
	uchar R, G, B;

	ColorRGB(): R(0), G(0), B(0) {}
	ColorRGB(uchar red, uchar green, uchar blue): R(red), G(green), B(blue) {}

	static const char* Name();
	void Index(const char* field) final;
	void NewIndex(const char* field) final;
};

struct Vector3s final : public LuaObjectClassRotation
{
	short x, y, z;

	Vector3s() : x(0), y(0), z(0) {}
	Vector3s(short xrot, short yrot, short zrot) : x(xrot), y(yrot), z(zrot) {}

	static const char* Name();

	short GetX() final;
	short GetY() final;
	short GetZ() final;
	void SetX(short x) final;
	void SetY(short y) final;
	void SetZ(short z) final;
	explicit operator Vector3s() final;
};

struct Vector3i final : public LuaObjectClass
{
	int x, y, z;

	Vector3i() : x(16384), y(16384), z(16384) {}

	static const char* Name();
	void Index(const char* field) final;
	void NewIndex(const char* field) final;
};

enum BlendMode
{
	BLEND_TEXTURE,
	BLEND_DECAL,
	BLEND_COLORADD,
	BLEND_SEMITRANS,
	BLEND_NOZBUFFER,
	BLEND_COLORSUB,
	BLEND_LINE,
	BLEND_SEMITRANS_ZBUFFER,
	BLEND_DESTINATION_INV,
	BLEND_SCREEN_DARKEN,
	BLEND_SCREEN_CLEAR,
	BLEND_CUSTOM_11,
	BLEND_CUSTOM_12,
	BLEND_CUSTOM_13
};

enum LightMode
{
	LIGHT_STATIC,
	LIGHT_DYNAMIC
};

enum FunctionType
{
	FUNCTION_NONE = 0x0,
	FUNCTION_LEVEL = 0x1,
	FUNCTION_BIND = 0x2,
	FUNCTION_MODULE = 0x4,
	FUNCTION_INIT = 0x8,
	FUNCTION_UPDATE = 0x10
};

enum DataValueType
{
	DATA_VALUE_BOOLEAN,
	DATA_VALUE_NUMBER
};

namespace Particles
{
	struct SpriteParticleSave;
	struct MeshParticleSave;

	enum TetherType
	{
		TETHER_ROTATING,
		TETHER_STATIC,
		TETHER_NONE
	};

	enum DrawMode
	{
		DRAW_SPRITE,
		DRAW_SQUARE,
		DRAW_ORIENTED_SPRITE,
		DRAW_LINE,
		DRAW_ARROW,
		DRAW_NONE
	};

	struct NodeAttachment final : public LuaObjectClass
	{
		TetherType tether;
		short cutoff, random;

		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct ParticleGroup final : public LuaObjectClass
	{
		// fields
		int initIndex;
		int updateIndex;

		NodeAttachment attach;

		ushort groupIndex;
		short spriteSlot;

		int partLimit;
		mutable int partCount;

		DrawMode drawMode;
		BlendMode blendMode;
		LightMode lightMode;

		bool autoTrigger;
		bool immortal;
		bool saved;
		bool screenSpace;
		bool lineIgnoreVel;

		// lua integration
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct ModuleGroups final : public LuaTableObjectClass
	{
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct ModuleParameters final : public LuaTableObjectClass
	{
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct Module final : public LuaObjectClass
	{
		ModuleGroups groups;
		ModuleParameters parameters;
		bool createdInCurrentModule;

		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct ParticleData final : public LuaTableObjectClass
	{
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;
	};

	struct BaseParticle : public LuaObjectClass
	{
		// fields
		Vector3f	pos;
		Vector3f	vel;
		Vector3f	accel;

		ushort		groupIndex;

		short		roomIndex;
		short		lifeSpan;
		short		lifeCounter;

		short		emitterIndex;
		char		emitterNode;
		bool		createdInCurrentLoop;

		ParticleData data;

		// methods
		float		Parameter();
		Vector3f	AbsPos();
		void		Attach(int itemIndex, int node);
		void		Detach();
		void		LimitSpeed(float maxSpeed);
		bool		CollideWalls(bool bounce, float rebound);
		bool		CollideFloors(bool bounce, float rebound, float minBounce, int collMargin, bool accurate);
		bool		CollidedWithItem(Tr4ItemInfo* item, int radius);
		bool		TargetHoming(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict);
		Vector3f	FollowTarget(const Vector3f& v, float maxSpeed, float distInner, float distOuter);
		Vector3f	WindVelocities(float factor);
		Vector3f	AttractToItem(Tr4ItemInfo* item, float radius, float factor);
		Vector3f	AvoidRoomGeometry(int wallMargin, int floorMargin, float factor);
		virtual void Animate(int start, int end, int frameRate) = 0;

		// boid-specific
		virtual Vector3f BoidSeparationRule(float radius, float factor) = 0;
		virtual Vector3f BoidCohesionRule(float radius, float factor) = 0;
		virtual Vector3f BoidAlignmentRule(float radius, float factor) = 0;

		// lua integration
		static const char* Name();
		void Index(const char* field) override;
		void NewIndex(const char* field) override;
	};

	struct SpriteParticle final : public BaseParticle
	{
		// fields
		ushort		spriteIndex;

		ushort		sizeStart;
		ushort		sizeEnd;
		ushort		sizeCust;
		short		sizeRatio;

		short		rot;
		short		rotVel;

		short		fadeIn;
		short		fadeOut;

		short		colorFadeTime;
		ColorRGB	colStart;
		ColorRGB	colEnd;
		ColorRGB	colCust;

		// methods
		void Animate(int start, int end, int frameRate) final;

		// draw function
		void DrawSpritePart(const ParticleGroup& pgroup, float* const view, float smallest_size);

		// boid-specific
		Vector3f BoidSeparationRule(float radius, float factor) final;
		Vector3f BoidCohesionRule(float radius, float factor) final;
		Vector3f BoidAlignmentRule(float radius, float factor) final;

		// lua integration
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;

		void LoadParticle(const SpriteParticleSave* s);
	};

	struct MeshParticle final : public BaseParticle
	{
		// fields
		Vector3s	rot;
		Vector3s	rotVel;
		Vector3i	scale;

		short		object;
		uchar		mesh;
		uchar		transparency;
		ColorRGB	tint;

		Tr4ItemInfo	item;

		// methods
		void Animate(int startMesh, int endMesh, int framerate) final;

		void	AlignToVel(float factor, bool invert);
		void	AlignToTarget(const Vector3f& v, float factor, bool invert);
		void	Shatter();

		// draw function
		void	DrawMeshPart();

		// boid-specific
		Vector3f BoidSeparationRule(float radius, float factor) final;
		Vector3f BoidCohesionRule(float radius, float factor) final;
		Vector3f BoidAlignmentRule(float radius, float factor) final;

		// lua integration
		static const char* Name();
		void Index(const char* field) final;
		void NewIndex(const char* field) final;

		void LoadParticle(const MeshParticleSave* m);
	};


	struct SpriteParticleSave
	{
		SpriteParticleSave() = default;
		SpriteParticleSave(const SpriteParticle& s);

		float		posX, posY, posZ;
		float		velX, velY, velZ;
		float		accelX, accelY, accelZ;

		ushort		groupIndex;

		short		roomIndex;
		short		lifeSpan;
		short		lifeCounter;

		short		emitterIndex;
		char		emitterNode;

		ushort		spriteIndex;

		ushort		sizeStart;
		ushort		sizeEnd;
		ushort		sizeCust;
		short		sizeRatio;

		short		rot;
		short		rotVel;

		short		fadeIn;
		short		fadeOut;

		short		colorFadeTime;

		uchar	colStartR, colStartG, colStartB;
		uchar	colEndR, colEndG, colEndB;
		uchar	colCustR, colCustG, colCustB;
	};

	struct MeshParticleSave
	{
		MeshParticleSave() = default;
		MeshParticleSave(const MeshParticle& m);

		float		posX, posY, posZ;
		float		velX, velY, velZ;
		float		accelX, accelY, accelZ;

		ushort		groupIndex;

		short		roomIndex;
		short		lifeSpan;
		short		lifeCounter;

		short		emitterIndex;
		char		emitterNode;

		short		rotX, rotY, rotZ;
		short		rotVelX, rotVelY, rotVelZ;
		int			scaleX, scaleY, scaleZ;

		short		object;
		uchar		mesh;
		uchar		transparency;
		uchar		tintR, tintG, tintB;
	};

	struct CallerGuard
	{
		FunctionType previousCaller;

		CallerGuard(FunctionType caller);
		~CallerGuard();
	};

	struct BoundFunction
	{
		int ref;
		FunctionType type;

		BoundFunction() : ref(SCRIPT_REFNIL), type(FUNCTION_NONE) {}
	};

	// ************  Global declarations ************ //

	extern SpriteParticle spriteParts[];
	extern MeshParticle meshParts[];
	extern ParticleGroup partGroups[];
	extern Module modules[];
	extern BoundFunction functionRefs[];

	FunctionType GetCaller();
	void SetCaller(FunctionType caller);

	// ************  Particles functions ************ //

	void ClearParts();
	void ClearPartGroups();
	void ClearModules();
	void ClearFunctionRefs();

	void InitParts();
	void InitLevelScript();

	void UpdateParts();
	void UpdateSprites();
	void UpdateMeshes();
	void PostUpdateLoop();

	void DrawParts();
	void DrawSprites();
	void DrawMeshes();

	int GetFreeSpritePart();
	int GetFreeMeshPart();
	int GetFreeParticleGroup();
	int GetFreeModule();
	int GetLastModule();

	void ExecuteBoundFunction(int index);
};
