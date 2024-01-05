#pragma once

#define RAD(a) ((a) * float(M_PI) / HALF_ANGLE)
#define ANG(a) Round((a) * HALF_ANGLE / float(M_PI))
#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

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

// ************  Particle-related structs  ****************

struct ColorRGB final : public LuaObjectClass
{
	uchar R, G, B;

	ColorRGB(): R(0), G(0), B(0) {}

	ColorRGB(uchar red, uchar green, uchar blue): R(red), G(green), B(blue) {}

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct Vector3s final : public LuaObjectClass
{
	short x, y, z;

	Vector3s() : x(0), y(0), z(0) {}

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

struct Vector3i final : public LuaObjectClass
{
	int x, y, z;

	Vector3i() : x(16384), y(16384), z(16384) {}

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};

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
	DRAW_LINE,
	DRAW_ARROW,
	DRAW_NONE
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


struct NodeAttachment final : public LuaObjectClass
{
	TetherType tether;
	short cutoff, random;

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};



// ************  ParticleGroup struct  ****************

struct ParticleGroup final : public LuaObjectClass
{
	int initIndex;
	int updateIndex;

	NodeAttachment attach;

	ushort groupIndex;
	short spriteSlot;

	int partLimit;
	mutable int partCount;

	DrawMode drawMode;

	uchar blendingMode;
	bool Saved;
	bool ScreenSpace;
	bool LineIgnoreVel;

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


// ************  Particle structs  ****************

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

	// methods
	float		Parameter();
	Vector3f	AbsPos();
	void		Attach(int itemIndex, int node);
	void		Detach();
	void		LimitSpeed(float maxSpeed);
	bool		CollideWalls(float rebound);
	bool		CollideFloors(float rebound, float minBounce, int collMargin, bool accurate);
	bool		CollidedWithItem(Tr4ItemInfo* item, int radius);
	bool		TargetHoming(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict);
	Vector3f	FollowTarget(const Vector3f& v, float maxSpeed, float distFactor, float distCutOff);
	Vector3f	WindVelocities(float factor);
	Vector3f	SplinePos(const Vector3f v[], int arrSize, float t);
	Vector3f	SplineVel(const Vector3f v[], int arrSize, float t);
	Vector3f	AvoidRoomGeometry(int wallMargin, int floorMargin, float factor);
	Vector3f	AvoidItem(Tr4ItemInfo* item, float radius, float factor);

	virtual void Animate(int start, int end, int frameRate) = 0;

	// boid-specific
	virtual Vector3f BoidSeparationRule(float radius, float factor) = 0;
	virtual Vector3f BoidCohesionRule(float radius, float factor) = 0;
	virtual Vector3f BoidAlignmentRule(float radius, float factor) = 0;

	// lua integration
	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
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
	virtual void Animate(int start, int end, int frameRate) override;

	// draw function
	void DrawSpritePart(const ParticleGroup& pgroup, long* const view, long smallest_size);

	// boid-specific
	virtual Vector3f BoidSeparationRule(float radius, float factor) override;
	virtual Vector3f BoidCohesionRule(float radius, float factor) override;
	virtual Vector3f BoidAlignmentRule(float radius, float factor) override;

	// lua integration
	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
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
	virtual void Animate(int startMesh, int endMesh, int framerate) override;

	void	AlignToVel(float factor, bool invert);
	void	AlignToTarget(const Vector3f& v, float factor, bool invert);
	void	Shatter();

	// draw function
	void	DrawMeshPart();

	// boid-specific
	virtual Vector3f BoidSeparationRule(float radius, float factor) override;
	virtual Vector3f BoidCohesionRule(float radius, float factor) override;
	virtual Vector3f BoidAlignmentRule(float radius, float factor) override;

	// lua integration
	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


// ************  namespace ParticleFactory - declarations ****************

enum FunctionType
{
	FUNCTION_LIBRARY = 0x1,
	FUNCTION_INIT = 0x2,
	FUNCTION_UPDATE = 0x4
};

namespace ParticleFactory
{
	extern ulong gameTick;

	extern SpriteParticle spriteParts[];
	extern MeshParticle meshParts[];
	extern ParticleGroup partGroups[];

	extern FunctionType caller;

	void ClearParts();
	void ClearPartGroups();

	void InitParts();
	void InitPartGroups();
	
	void UpdateParts();
	void UpdateSprites();
	void UpdateMeshes();

	void DrawParts();
	void DrawSprites();
	void DrawMeshes();

	int GetFreeSpritePart();
	int GetFreeMeshPart();
	int GetFreeParticleGroup();
};
