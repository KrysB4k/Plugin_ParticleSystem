#pragma once

struct Vector3f;

class PerlinNoise
{
public:
	PerlinNoise();
	PerlinNoise(int seed);

	// reset to reference permutation table
	void ReferencePermut();

	// generate a new, seed-based permutation table
	void SeedPermut(int seed);
	
	// get single (scalar) noise value for n-dimensional input
	float Noise1D(float domainScale, float x);
	float Noise2D(float domainScale, float x, float y);
	float Noise3D(float domainScale, float x, float y, float z);
	float Noise4D(float domainScale, float x, float y, float z, float w);

	// get curl noise vector for 2D input
	Vector3f CurlNoise2D(float domainScale, float x, float y);
	Vector3f CurlNoise2DTime(float domainScale, float time, float x, float y);

	// get cheap "curl" noise 3D vector (not true curl noise)
	Vector3f CurlNoise3DCheap(float domainScale, float x, float y, float z);
	Vector3f CurlNoise3DTime(float domainScale, float time, float x, float y, float z);

	// get true curl noise 3D vector (requires sampling 3 noise gradients)
	static Vector3f CurlNoise3DAccurate(const PerlinNoise& nx, const PerlinNoise& ny, const PerlinNoise& nz, float scale, const Vector3f& p);

private:

	// permutation table for generating noise
	unsigned char permut[512];

	static inline int floor(float x) { int xi = int(x); return (x < xi ? xi - 1 : xi); }

	static inline float fade(float t) { return (t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f)); }

	static inline float fadederiv(float t) { return (30.0f * t * t * (t * (t - 2.0f) + 1.0f)); }

	static inline float lerp(float a, float b, float t) { return (a + (b - a) * t); }

	static inline float dot1D(int hash, float xf) { return (hash & 1) ? xf : -xf; }

	static inline float dot2D(int hash, float xf, float yf)
	{
		switch (hash & 3)
		{
		case 0:		return (xf + yf);
		case 1:		return (-xf + yf);
		case 2:		return (xf - yf);
		case 3:		return (-xf - yf);
		default:	return  0.0f;
		}
	}

	static inline float dot3D(int hash, float xf, float yf, float zf)
	{
		switch (hash & 15)
		{
			case 0:		return (xf + yf);
			case 1:		return (-xf + yf);
			case 2:		return (xf - yf);
			case 3:		return (-xf - yf);
			case 4:		return (xf + zf);
			case 5:		return (-xf + zf);
			case 6:		return (xf - zf);
			case 7:		return (-xf - zf);
			case 8:		return (yf + zf);
			case 9:		return (-yf + zf);
			case 10:	return (yf - zf);
			case 11:	return (-yf - zf);
			case 12:	return (yf + xf);
			case 13:	return (-yf + zf);
			case 14:	return (yf - xf);
			case 15:	return (-yf - zf);
			default:	return  0.0f;
		}
	}

	static inline float dot4D(int hash, float xf, float yf, float zf, float wf)
	{
		switch (hash & 31)
		{
		case 0:		return (xf + yf + zf);
		case 1:		return (-xf + yf + zf);
		case 2:		return (xf - yf + zf);
		case 3:		return (xf + yf - zf);
		case 4:		return (-xf - yf + zf);
		case 5:		return (-xf + yf - zf);
		case 6:		return (xf - yf - zf);
		case 7:		return (-xf - yf - zf);
		case 8:		return (xf + yf + wf);
		case 9:		return (-xf + yf + wf);
		case 10:	return (xf - yf + wf);
		case 11:	return (xf + yf - wf);
		case 12:	return (-xf - yf + wf);
		case 13:	return (-xf + yf - wf);
		case 14:	return (xf - yf - wf);
		case 15:	return (-xf - yf - wf);
		case 16:	return (xf + zf + wf);
		case 17:	return (-xf + zf + wf);
		case 18:	return (xf - zf + wf);
		case 19:	return (xf + zf - wf);
		case 20:	return (-xf - zf + wf);
		case 21:	return (-xf + zf - wf);
		case 22:	return (xf - zf - wf);
		case 23:	return (-xf - zf - wf);
		case 24:	return (yf + zf + wf);
		case 25:	return (-yf + zf + wf);
		case 26:	return (yf - zf + wf);
		case 27:	return (yf + zf - wf);
		case 28:	return (-yf - zf + wf);
		case 29:	return (-yf + zf - wf);
		case 30:	return (yf - zf - wf);
		case 31:	return (-yf - zf - wf);
		default:	return  0.0f;
		}
	}

	static inline Vector3f grad2D(int hash)
	{
		switch (hash & 3)
		{
		case 0:		return Vector3f(1, 1, 0);
		case 1:		return Vector3f(-1, 1, 0);
		case 2:		return Vector3f(1, -1, 0);
		case 3:		return Vector3f(-1, -1, 0);
		default:	return Vector3f();
		}
	}

	static inline Vector3f grad3D(int hash)
	{
		switch (hash & 15)
		{
		case 0:		return Vector3f(1, 1, 0);
		case 1:		return Vector3f(-1, 1, 0);
		case 2:		return Vector3f(1, -1, 0);
		case 3:		return Vector3f(-1, -1, 0);
		case 4:		return Vector3f(1, 0, 1);
		case 5:		return Vector3f(-1, 0, 1);
		case 6:		return Vector3f(1, 0, -1);
		case 7:		return Vector3f(-1, 0, -1);
		case 8:		return Vector3f(0, 1, 1);
		case 9:		return Vector3f(0, -1, 1);
		case 10:	return Vector3f(0, 1, -1);
		case 11:	return Vector3f(0, -1, -1);
		case 12:	return Vector3f(1, 1, 0);
		case 13:	return Vector3f(0, -1, 1);
		case 14:	return Vector3f(-1, 1, 0);
		case 15:	return Vector3f(0, -1, -1);
		default:	return Vector3f();
		}
	}

	static inline Vector3f ComputeCurl3D(const Vector3f& gradX, const Vector3f& gradY, const Vector3f& gradZ)
	{
		return Vector3f(gradZ.y - gradY.z, gradX.z - gradZ.x, gradY.x - gradX.y);
	}

	// get noise analytical derivatives - used for curl noise computation
	Vector3f NoiseGradient2D(float scale, float x, float y) const;
	Vector3f NoiseGradient3D(float scale, float x, float y, float z) const;
};