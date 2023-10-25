#pragma once

typedef unsigned char uchar;

class Noise
{
public:
	Noise()
	{
		ReferencePermut();
	}

	// reset to reference permutation table
	void ReferencePermut();

	// generate a new, seed-based permutation table
	void SeedPermut(int seed);

	// get single (scalar) noise value for n-dimensional input

	float PerlinNoise1D(float x) const;
	float PerlinNoise2D(float x, float y) const;
	float PerlinNoise3D(float x, float y, float z) const;
	float PerlinNoise4D(float x, float y, float z, float w) const;

	float SimplexNoise1D(float x) const;
	float SimplexNoise2D(float x, float y) const;
	float SimplexNoise3D(float x, float y, float z) const;
	float SimplexNoise4D(float x, float y, float z, float w) const;

	// get curl noise vector (2D or 3D)

	Vector3f PerlinCurl2D(float x, float y) const;
	Vector3f PerlinCurl2DTime(float time, float x, float y) const; // evolve noise with time
	Vector3f PerlinCurl3D(float x, float y, float z) const;
	Vector3f PerlinCurl3DTime(float time, float x, float y, float z) const; // evolve noise with time
	
	Vector3f SimplexCurl2D(float x, float y) const;
	Vector3f SimplexCurl2DTime(float time, float x, float y) const; // evolve noise with time
	Vector3f SimplexCurl3D(float x, float y, float z) const;
	Vector3f SimplexCurl3DTime(float time, float x, float y, float z) const; // evolve noise with time

private:

	uchar permut[512]; // permutation table

	Vector3f PerlinGradient2D(float x, float y) const;
	Vector3f PerlinGradient3D(float x, float y, float z) const;
	Vector3f PerlinGradient4D(float x, float y, float z, float w) const;

	Vector3f SimplexGradient2D(float x, float y) const;
	Vector3f SimplexGradient3D(float x, float y, float z) const;
	Vector3f SimplexGradient4D(float x, float y, float z, float w) const;

	static inline int fastfloor(float x) { int xi = static_cast<int>(x); return (x < xi ? xi - 1 : xi); }

	static inline Vector3f computecurl3D(const Vector3f& gradX, const Vector3f& gradY, const Vector3f& gradZ)
	{
		return Vector3f(gradZ.y - gradY.z, gradX.z - gradZ.x, gradY.x - gradX.y);
	}


	/******** Perlin implementation stuff ********/

	static inline float fade(float t) { return (t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f)); }

	static inline float fadederiv(float t) { return (30.0f * t * t * (t * (t - 2.0f) + 1.0f)); }

	static inline float lerp(float a, float b, float t) { return (a + (b - a) * t); }

	static inline float dot1D(int hash, float xf) { return (hash & 1) ? xf : -xf; }

	static inline float dot2D(int hash, float xf, float yf)
	{
		switch (hash & 7)
		{
		case 0:		return (xf + yf);
		case 1:		return (yf);
		case 2:		return (-xf + yf);
		case 3:		return (-xf);
		case 4:		return (-xf - yf);
		case 5:		return (-yf);
		case 6:		return (xf - yf);
		case 7:		return (xf);
		}

		return 0.0f; // this return should never occur
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
		}

		return 0.0f;
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
		}

		return 0.0f;
	}

	static inline Vector3f gradVec2D(int hash)
	{
		switch (hash & 7)
		{
		case 0:		return Vector3f(1, 1, 0);
		case 1:		return Vector3f(0, 1, 0);
		case 2:		return Vector3f(-1, 1, 0);
		case 3:		return Vector3f(-1, 0, 0);
		case 4:		return Vector3f(-1, -1, 0);
		case 5:		return Vector3f(0, -1, 0);
		case 6:		return Vector3f(1, -1, 0);
		case 7:		return Vector3f(1, 0, 0);
		}

		return Vector3f();
	}

	static inline Vector3f gradVec3D(int hash)
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
		}

		return Vector3f();
	}

	struct Vector4f // helper struct
	{
		float x, y, z, w;

		Vector4f(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}
		Vector4f operator+(const Vector4f& v) { return Vector4f(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vector4f operator-(const Vector4f& v) { return Vector4f(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vector4f operator*(float a) { return Vector4f(x * a, y * a, z * a, w * a); }
	};

	static inline Vector4f gradVec4D(int hash)
	{
		switch (hash & 31)
		{
		case 0:		return Vector4f(1, 1, 1, 0);
		case 1:		return Vector4f(-1, 1, 1, 0);
		case 2:		return Vector4f(1, -1, 1, 0);
		case 3:		return Vector4f(1, 1, -1, 0);
		case 4:		return Vector4f(-1, -1, 1, 0);
		case 5:		return Vector4f(-1, 1, -1, 0);
		case 6:		return Vector4f(1, -1, -1, 0);
		case 7:		return Vector4f(-1, -1, -1, 0);
		case 8:		return Vector4f(1, 1, 0, 1);
		case 9:		return Vector4f(-1, 1, 0, 1);
		case 10:	return Vector4f(1, -1, 0, 1);
		case 11:	return Vector4f(1, 1, 0, -1);
		case 12:	return Vector4f(-1, -1, 0, 1);
		case 13:	return Vector4f(-1, 1, 0, -1);
		case 14:	return Vector4f(1, -1, 0, -1);
		case 15:	return Vector4f(-1, -1, 0, -1);
		case 16:	return Vector4f(1, 0, 1, 1);
		case 17:	return Vector4f(-1, 0, 1, 1);
		case 18:	return Vector4f(1, 0, -1, 1);
		case 19:	return Vector4f(1, 0, 1, -1);
		case 20:	return Vector4f(-1, 0, -1, 1);
		case 21:	return Vector4f(-1, 0, 1, -1);
		case 22:	return Vector4f(1, 0, -1, -1);
		case 23:	return Vector4f(-1, 0, -1, -1);
		case 24:	return Vector4f(0, 1, 1, 1);
		case 25:	return Vector4f(0, -1, 1, 1);
		case 26:	return Vector4f(0, 1, -1, 1);
		case 27:	return Vector4f(0, 1, 1, -1);
		case 28:	return Vector4f(0, -1, -1, 1);
		case 29:	return Vector4f(0, -1, 1, -1);
		case 30:	return Vector4f(0, 1, -1, -1);
		case 31:	return Vector4f(0, -1, -1, -1);
		}

		return Vector4f(0, 0, 0, 0);
	}


	/******** Simplex implementation stuff ********/

	static const float grad2lut[8][2]; // 2D gradient lookup table
	static const float grad3lut[16][3]; // 3D gradient lookup table
	static const float grad4lut[32][4]; // 4D gradient lookup table
	static const uchar simplexlut[64][4]; // 4D simplex traversal table

	static constexpr float F2 = 0.366025403f; // 2D skew factor
	static constexpr float G2 = 0.211324865f; // 2D unskew factor

	static constexpr float F3 = 0.333333333f; // 3D skew factor
	static constexpr float G3 = 0.166666667f; // 3D unskew factor

	static constexpr float F4 = 0.309016994f; // 4D skew factor
	static constexpr float G4 = 0.138196601f; // 4D unskew factor

	static inline void grad1(int hash, float& gx) {
		int h = hash & 15;
		gx = 1.0f + (h & 7);
		if (h & 8) gx = -gx;
	}

	static inline void grad2(int hash, float& gx, float& gy) {
		int h = hash & 7;
		gx = grad2lut[h][0];
		gy = grad2lut[h][1];
	}

	static inline void grad3(int hash, float& gx, float& gy, float& gz) {
		int h = hash & 15;
		gx = grad3lut[h][0];
		gy = grad3lut[h][1];
		gz = grad3lut[h][2];
	}

	static inline void grad4(int hash, float& gx, float& gy, float& gz, float& gw) {
		int h = hash & 31;
		gx = grad4lut[h][0];
		gy = grad4lut[h][1];
		gz = grad4lut[h][2];
		gw = grad4lut[h][3];
	}
};
