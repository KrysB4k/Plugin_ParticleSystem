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

struct ColorRGB : public LuaObject
{
	uchar R, G, B;

	ColorRGB(): R(0), G(0), B(0) {}

	ColorRGB(uchar red, uchar green, uchar blue): R(red), G(green), B(blue) {}

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


enum TetherType
{
	TETHER_ROTATING,
	TETHER_STATIC
};


enum DrawMode
{
	DRAW_SPRITE,
	DRAW_LINE,
	DRAW_ARROW,
	DRAW_NONE
};


struct NodeAttachment : LuaObject
{
	TetherType tether;
	short cutoff, random;

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};



// ************  ParticleGroup struct  ****************

struct ParticleGroup : LuaObject
{
	int initIndex;
	int updateIndex;

	NodeAttachment attach;

	short spriteSlot;

	int partLimit;
	mutable int partCount;

	DrawMode drawMode;
	uchar blendingMode;
	uchar groupIndex;

	bool Saved;
	bool ScreenSpace;
	bool IsBoid;
	bool LineIgnoreVel;

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


// ************  Particle structs  ****************

struct BaseParticle : LuaObject
{
	// fields
	Vector3f	pos;
	Vector3f	vel;
	Vector3f	accel;

	short		roomIndex;
	short		lifeSpan;
	short		lifeCounter;

	short		emitterIndex;
	char		emitterNode;
	uchar		groupIndex;

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
};


struct SpriteParticle : BaseParticle
{
	// fields
	ushort		sizeStart;
	ushort		sizeEnd;
	ushort		sizeCust;
	short		skew;

	short		rot;
	short		rotVel;

	uchar		spriteIndex;
	uchar		fadeIn;
	uchar		fadeOut;

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
	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


struct MeshParticle : BaseParticle
{
	// fields
	short		rotX;
	short		rotY;
	short		rotZ;

	short		rotVelX;
	short		rotVelY;
	short		rotVelZ;

	char		scaleX;
	char		scaleY;
	char		scaleZ;

	short		object;
	uchar		mesh;
	uchar		transparency;
	ColorRGB	tint;

	// methods
	virtual void Animate(int startMesh, int endMesh, int framerate) override;

	void		AlignToVel(float factor, bool invert);
	void		AlignToTarget(const Vector3f& v, float factor, bool invert);
	void		Shatter();

	// draw function
	void		DrawMeshPart();

	// boid-specific
	virtual Vector3f BoidSeparationRule(float radius, float factor) override;
	virtual Vector3f BoidCohesionRule(float radius, float factor) override;
	virtual Vector3f BoidAlignmentRule(float radius, float factor) override;

	// lua integration
	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


// ************  namespace ParticleFactory - declarations ****************

enum FunctionType
{
	FUNCTION_LIBRARY,
	FUNCTION_INIT,
	FUNCTION_UPDATE
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
