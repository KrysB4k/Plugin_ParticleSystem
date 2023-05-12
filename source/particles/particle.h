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
	extern int activeParticles;
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

	ColorRGB(): R(255), G(255), B(255) {}

	ColorRGB(uchar red, uchar green, uchar blue): R(red), G(green), B(blue) {}

	ColorRGB(ulong HEX)
	{
		B = HEX & 0xFF;
		G = (HEX >> 8) & 0xFF;
		R = (HEX >> 16) & 0xFF;
	}

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;

	ColorRGB& operator= (ulong HEX)
	{
		B = HEX & 0xFF;
		G = (HEX >> 8) & 0xFF;
		R = (HEX >> 16) & 0xFF;

		return *this;
	}
};


enum SpawnMode
{
	SPAWN_ABSOLUTE,
	SPAWN_RELATIVE_ITEM,
	SPAWN_RELATIVE_ITEMNODE
};


struct NodeAttachment : LuaObject
{
	int offX, offY, offZ;
	short cutoff, random;

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


struct ParticleGroup : LuaObject
{
	int initIndex;
	int updateIndex;

	uchar groupIndex;

	NodeAttachment attach;
	short spriteSlot;
	uchar blendingMode;

	// various particle flags, more may be added with time
	bool Saved;
	bool NoPerspective;
	bool LineIgnoreVel;
	bool WindAffected;

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
};


// ************  Particle struct  ****************

struct Particle : public LuaObject
{
// fields
	Vector3f	pos;
	Vector3f	vel;
	Vector3f	accel;

	short		roomIndex;
	short		lifeSpan;
	short		lifeCounter;
	short		colorFadeTime;
	short		rot;
	short		rotVel;

	ushort		sizeStart;
	ushort		sizeEnd;
	ushort		sizeCust;

	short		emitterIndex;
	char		emitterNode;

	uchar		groupIndex;

	uchar		spriteIndex;

	uchar		fadeIn;
	uchar		fadeOut;

	ColorRGB	colStart;
	ColorRGB	colEnd;
	ColorRGB	colCust;

	virtual int Index(const char* field) override;
	virtual void NewIndex(const char* field) override;

// methods
	float		Parameter();
	void		ParticleLimitSpeed(float maxSpeed);
	Vector3f	ParticleAbsPos();
	void		ParticleAnimate(unsigned int startSprite, unsigned int endSprite, int framerate);
	void		ParticleAttach(int itemIndex, int node);
	void		ParticleDeattach();
	bool		ParticleCollideWalls(float rebound);
	bool		ParticleCollideFloors(float rebound, float friction, bool accurate);
	Vector3f	ParticleFollow(const Vector3f& v, float factor, float maxSpeed);
	bool		ParticleHoming(Tr4ItemInfo *item, int targetNode, float homingFactor, float homingAccel, bool predict);

	void		DrawParticle(const ParticleGroup& pgroup, long* const view, long smallest_size);
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
	extern Particle	parts[];
	extern ParticleGroup partGroups[];
	extern FunctionType caller;

	void ClearParts();
	void ClearPartGroups();
	void UpdateParts();
	void DrawParts();
	void InitParts();
	void InitPartGroups();

	int GetFreeParticle();
	int GetFreeParticleGroup();
};
