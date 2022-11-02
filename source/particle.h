#pragma once

#define RAD(a) ((a) * float(M_PI) / HALF_ANGLE)
#define ANG(a) Round((a) * HALF_ANGLE / float(M_PI))
#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;


struct ColorRGB
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

	ColorRGB& operator= (ulong HEX)
	{
		B = HEX & 0xFF;
		G = (HEX >> 8) & 0xFF;
		R = (HEX >> 16) & 0xFF;

		return *this;
	}
};


struct NodeAttachment
{
	short offX, offY, offZ;
	short cutoff, random;
};


struct ParticleGroup
{
	NodeAttachment attach;
	short spriteSlot;
	uchar blendingMode;
	uchar updateIndex; // a numeric index, but it could become a function pointer eventually?

	// various particle flags, more may be added with time
	bool Saved;
	bool NoPerspective;
	bool LineIgnoreVel;
	bool WindAffected;
};


struct Particle
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
	short		angVel;

	ushort		startSize;
	ushort		destSize;

	short		emitterIndex;
	char		emitterNode;

	uchar		groupIndex;
	uchar		spriteIndex;

	uchar		fadeIn;
	uchar		fadeOut;

	ColorRGB	colStart;
	ColorRGB	colEnd;
	ColorRGB	colCust;

// methods
	float		Parameter();
	void		ParticleLimitSpeed(float maxSpeed);
	Vector3f	ParticleAbsPos();
	void		ParticleAnimate(unsigned int startSprite, unsigned int endSprite, int framerate);
	bool		ParticleCollideWalls(float rebound);
	bool		ParticleCollideFloors(float rebound, float friction, bool accurate);
	Vector3f	ParticleFollow(const Vector3f& v, float factor, float maxSpeed);
	bool		ParticleHoming(Tr4ItemInfo *item, int targetNode, float homingFactor, float homingAccel, bool predict);

	void		DrawParticle(ParticleGroup* const pdata, long* const view, long smallest_size);
};