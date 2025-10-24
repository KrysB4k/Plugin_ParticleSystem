#include "../definitions/DefTomb4Funct.h"
#include "../definitions/Tomb4Discoveries_mine.h"
#include "../definitions/Tomb4Globals.h"
#include "particle.h"
#include "utilities.h"
#include <random>
#include "../trng/trng.h"

// ************  Utility functions ************ //

namespace Utilities
{
	std::mt19937 mt;

	phd_vector RoundPos(const Vector3f& v)
	{
		return phd_vector(SaturateRound<int>(v.x), SaturateRound<int>(v.y), SaturateRound<int>(v.z));
	}

	short GetOrientDiff(short sourceOrient, short targetOrient)
	{
		int diff = targetOrient - sourceOrient;

		return short(diff); // returns (+) if to the right of source orient, (-) if to the left
	}

	float Lerp(float a, float b, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return a + t * (b - a);
	}

	float InverseLerp(float val1, float val2, float x)
	{
		float t = (x - val1) / (val2 - val1);

		return Clamp(t, 0.0f, 1.0f);
	}

	float SmoothStep(float x)
	{
		x = Clamp(x, 0.0f, 1.0f);

		return x * x * (3.0f - 2.0f * x);
	}

	float SimpleDist(const Vector3f& v1, const Vector3f& v2)
	{
		Vector3f d = (v1 - v2);
		d.x = abs(d.x);
		d.y = abs(d.y);
		d.z = abs(d.z);

		float maxDist = d.x;
		if (d.y > maxDist)
			maxDist = d.y;
		if (d.z > maxDist)
			maxDist = d.z;

		return maxDist;
	}

	int CheckDistFast(const Vector3f& v1, const Vector3f& v2, float dist)
	{
		auto vdif = v2 - v1;
		float vdifsqr = (vdif.x * vdif.x + vdif.y * vdif.y + vdif.z * vdif.z);
		float distsqr = dist * dist;

		if (vdifsqr < distsqr)
			return -1;
		else if (vdifsqr > distsqr)
			return 1;

		return 0;
	}

	float RealDist(const Vector3f& v1, const Vector3f& v2)
	{
		return (v1 - v2).magnitude();
	}

	ColorRGB Lerp(const ColorRGB& C1, const ColorRGB& C2, float t)
	{
		return ColorRGB(
			SaturateRound<uchar>(C1.R + (C2.R - C1.R) * t),
			SaturateRound<uchar>(C1.G + (C2.G - C1.G) * t),
			SaturateRound<uchar>(C1.B + (C2.B - C1.B) * t)
		);
	}

	ColorRGB HSVtoRGB(float hue, float sat, float val)
	{
		hue = fmodf(hue, 360.0f);
		if (hue < 0.0f)
			hue += 360.0f;

		if (sat <= 0.0f)
		{
			uchar v = SaturateRound<uchar>(val * 255);
			return ColorRGB(v, v, v);
		}

		int hextant = int(hue / 60.0f);
		float fhue = (hue - hextant * 60.0f) / 60.0f;

		float chroma = sat * val;
		float xval = chroma * (1 - abs(fmod(hextant + fhue, 2.0f) - 1));
		float m = val - chroma;
		float r = 0;
		float g = 0;
		float b = 0;

		switch (hextant)
		{
		case 0:
			r = chroma;
			g = xval;
			break;

		case 1:
			r = xval;
			g = chroma;
			break;

		case 2:
			g = chroma;
			b = xval;
			break;

		case 3:
			g = xval;
			b = chroma;
			break;

		case 4:
			r = xval;
			b = chroma;
			break;

		case 5:
			r = chroma;
			b = xval;
		}

		ColorRGB c(SaturateRound<uchar>((r + m) * 255),
			SaturateRound<uchar>((g + m) * 255),
			SaturateRound<uchar>((b + m) * 255));

		return c;
	}

	long TestCollisionSpheres(Tr4ItemInfo* item, const Vector3f& posTest, float radius)
	{
		static MeshSphere SphereList[34];
		long flags = 0;
		int num = 0;

		if (item)
		{
			num = GetSpheres(item, SphereList, 1);

			for (int i = 0; i < num; ++i)
			{
				auto sph = &SphereList[i];
				int r = SaturateRound<int>(sph->r + radius);
				if (r > 0)
				{
					int x = SaturateRound<int>(posTest.x - sph->x);
					int y = SaturateRound<int>(posTest.y - sph->y);
					int z = SaturateRound<int>(posTest.z - sph->z);

					if ((x * x + y * y + z * z) < r * r)
						flags |= (1 << i);
				}
			}
		}

		return flags;
	}

	int FindNearestTarget(const Vector3f& posTest, float radius, short* const slotList, int count)
	{
		int itemIndex = NO_ITEM;

		for (int i = 0; i < level_items; ++i)
		{
			auto item = &items[i];

			bool slotCheck = false;

			for (int j = 0; j < count; ++j)
			{
				if (slotList[j] == item->object_number)
				{
					slotCheck = true;
					break;
				}
			}

			if (slotCheck)
			{
				if (objects[item->object_number].intelligent && item->hit_points <= 0)
					continue;

				Vector3f target((float)item->pos.xPos, (float)item->pos.yPos, (float)item->pos.zPos);

				if (CheckDistFast(posTest, target, radius) < 0)
				{
					radius = RealDist(posTest, target);
					itemIndex = i;
				}
			}
		}

		return itemIndex;
	}

	Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff)
	{
		static const uchar lara_joint_map[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 8 };

		phd_vector v(xOff, yOff, zOff);

		if (!item->object_number)
			GetLaraJointPos(&v, lara_joint_map[joint]);
		else
			GetJointAbsPosition((StrItemTr4*)item, (StrMovePosition*)&v, joint);

		return Vector3f((float)v.x, (float)v.y, (float)v.z);
	}

	Vector3f RotatePoint3D(const Vector3f& point, short xrot, short yrot, short zrot)
	{
		Vector3f res;

		phd_PushUnitMatrix();
		phd_RotYXZ(yrot, xrot, zrot);

		res.x = (phd_mxptr[M00] * point.x + phd_mxptr[M01] * point.y + phd_mxptr[M02] * point.z) * (1.0f / 16384);
		res.y = (phd_mxptr[M10] * point.x + phd_mxptr[M11] * point.y + phd_mxptr[M12] * point.z) * (1.0f / 16384);
		res.z = (phd_mxptr[M20] * point.x + phd_mxptr[M21] * point.y + phd_mxptr[M22] * point.z) * (1.0f / 16384);

		phd_PopMatrix();

		return res;
	}

	Vector3f SphericalToCartesian(float r, float theta, float phi)
	{
		phi += float(M_PI_2);
		float x = r * sin(phi) * sin(theta);
		float z = r * sin(phi) * cos(theta);
		float y = r * cos(phi);
		return Vector3f(x, -y, z);
	}

	int TestForWall(int x, int y, int z, short* room)
	{
		void* floor = GetFloor(x, y, z, room);

		int fh = GetHeight(floor, x, y, z);
		int ch = GetCeiling(floor, x, y, z);

		if (fh != NO_HEIGHT)
		{
			if ((fh - y) > 0 && (ch - y) < 0)
				return 0; // No wall

			return 2; // Block (floor or ceiling)
		}

		return 1; // Wall
	}

	Vector3f GetSlopeNormal(Tr4FloorInfo* floor, int x, int y, int z)
	{
		if (!floor)
			return Vector3f(0, -1, 0);

		WORD tilts = GetTiltType(floor, x, y, z);
		signed char tilt_x = tilts & 0xFF;
		signed char tilt_z = tilts >> 8;

		return Vector3f(float(-tilt_x), -4.0f, float(-tilt_z)).normalized();
	}

	static Vector3f Spline(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float t)
	{
		auto a = v1 * 2;
		auto b = (-v0 + v2) * t;
		auto c = (v0 * 2 - v1 * 5 + v2 * 4 - v3) * (t * t);
		auto d = (-v0 + v1 * 3 - v2 * 3 + v3) * (t * t * t);

		return (a + b + c + d) * 0.5f;
	}

	static Vector3f SplineStart(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t)
	{
		auto a = v0 * 2;
		auto b = (-v0 * 3 + v1 * 4 - v2) * t;
		auto c = (v0 - v1 * 2 + v2) * (t * t);

		return (a + b + c) * 0.5f;
	}

	static Vector3f SplineEnd(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t)
	{
		auto a = v1 * 2;
		auto b = (-v0 + v2) * t;
		auto c = (v0 - v1 * 2 + v2) * (t * t);

		return (a + b + c) * 0.5f;
	}

	static Vector3f SplineDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float t)
	{
		auto b = (-v0 + v2);
		auto c = (v0 * 2 - v1 * 5 + v2 * 4 - v3) * 2 * t;
		auto d = (-v0 + v1 * 3 - v2 * 3 + v3) * 3 * (t * t);

		return (b + c + d) * 0.5f;
	}

	static Vector3f SplineStartDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t)
	{
		auto b = (-v0 * 3 + v1 * 4 - v2);
		auto c = (v0 - v1 * 2 + v2) * 2 * t;

		return (b + c) * 0.5f;
	}

	static Vector3f SplineEndDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t)
	{
		auto b = (-v0 + v2);
		auto c = (v0 - v1 * 2 + v2) * 2 * t;

		return (b + c) * 0.5f;
	}

	Vector3f SplinePos(float t, Vector3f v[], int arrSize)
	{
		Vector3f splinePos;

		if (arrSize < 2)
			return v[0];

		if (arrSize == 2)
			return v[0].lerp(v[1], t);

		int div = arrSize - 1;
		int index = int(t * div);
		float tp = (t * div) - index;

		if (!index)
			splinePos = SplineStart(v[index], v[index + 1], v[index + 2], tp);
		else if (index + 1 == div)
			splinePos = SplineEnd(v[index - 1], v[index], v[index + 1], tp);
		else
			splinePos = Spline(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

		return splinePos;
	}

	Vector3f SplineVel(float t, float duration, Vector3f v[], int arrSize)
	{
		Vector3f splineVel;
		int div = arrSize - 1;

		if (div < 1)
			return splineVel;

		if (div == 1)
			return (v[1] - v[0]) * (1.0f / duration);

		int index = int(t * div);
		float tp = (t * div) - index;

		if (!index)
			splineVel = SplineStartDerivative(v[index], v[index + 1], v[index + 2], tp);
		else if (index + 1 == div)
			splineVel = SplineEndDerivative(v[index - 1], v[index], v[index + 1], tp);
		else
			splineVel = SplineDerivative(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

		return splineVel * (float(div) / duration);
	}

	int Clamp(int x, int min, int max)
	{
		if (x < min)
			return min;

		if (x > max)
			return max;

		return x;
	}

	float Clamp(float x, float min, float max)
	{
		if (x < min)
			return min;

		if (x > max)
			return max;

		return x;
	}

	float ShortToRad(short rotation)
	{
		return (float)M_PI * rotation / 32768;
	}

	short RadToShort(float angle)
	{
		return SaturateRound<short>(32768 * angle / (float)M_PI);
	}

	float DegToRad(float deg)
	{
		return (float)M_PI * deg / 180.0f;
	}

	float RadToDeg(float rad)
	{
		return rad * 180.0f / (float)M_PI;
	}

	float GetRandom()
	{
		return (float)mt() / (UINT_MAX);
	}

	float RandomNegate(float x)
	{
		return (mt() & 1) ? x : -x;
	}

	Vector3f RandomSpherePoint(float r)
	{
		float x1, x2, s;
		do
		{
			x1 = GetRandom() * 2.0f - 1.0f;
			x2 = GetRandom() * 2.0f - 1.0f;
			s = x1 * x1 + x2 * x2;
		} while (s >= 1.0f || s == 0.0f);

		float factor = sqrtf(1.0f - s);

		float x = x1 * factor * 2.0f;
		float y = x2 * factor * 2.0f;
		float z = 1.0f - (2.0f * s);

		return Vector3f(x * r, y * r, z * r);
	}

	float Remap(float x, float oldMin, float oldMax, float newMin, float newMax)
	{
		float dif = fabsf(oldMax - oldMin);
		if (dif <= 0.0f)
			return x;
		float min = fminf(oldMin, oldMax);
		float mult = Clamp(x - min, 0.0f, dif) / dif;
		return (fminf(newMin, newMax) + fabsf(newMax - newMin) * mult);
	}

	ushort ConvertTo16BitBGR(const ColorRGB& c)
	{
		ushort r = (c.R & 0xF8) >> 3;
		ushort g = (c.G & 0xF8) << 2;
		ushort b = (c.B & 0xF8) << 7;
		return b | g | r;
	}

	const char* FormatString(const char* format, ...)
	{
		static char buffer[4096];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, 4096, format, args);
		va_end(args);
		return buffer;
	}

	ColorRGB CalculateVertexDynamicLighting(int x, int y, int z)
	{
		ulong r = 0;
		ulong g = 0;
		ulong b = 0;

		auto dynamics = (DYNAMIC*)Trng.pGlobTomb4->BaseRemap.Old_804740.Start;

		for (int i = 0; i < 127; i++)
		{
			const auto& dptr = dynamics[i];

			if (dptr.on)
			{
				Vector3f d;

				d.x = (float)(dptr.x - x);
				d.y = (float)(dptr.y - y);
				d.z = (float)(dptr.z - z);
				float f = 256 * d.magnitude() / 150;

				if (f <= dptr.falloff)
				{
					float scale = 1 - f / dptr.falloff;
					r += (ulong)(scale * dptr.r);
					g += (ulong)(scale * dptr.g);
					b += (ulong)(scale * dptr.b);
				}
			}
		}

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		return ColorRGB((uchar)r, (uchar)g, (uchar)b);
	}
}