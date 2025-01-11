#pragma once

// ************  Utility functions ************ //

struct Vector3f;
struct ColorRGB;

namespace Utilities
{
	phd_vector RoundPos(const Vector3f& v);
	short GetOrientDiff(short sourceOrient, short targetOrient);
	float Lerp(float a, float b, float t);
	float InverseLerp(float val1, float val2, float x);
	float SmoothStep(float x);
	float SimpleDist(const Vector3f& v1, const Vector3f& v2);
	int CheckDistFast(const Vector3f& v1, const Vector3f& v2, float dist);
	float RealDist(const Vector3f& v1, const Vector3f& v2);
	ColorRGB Lerp(const ColorRGB& C1, const ColorRGB& C2, float t);
	ColorRGB HSVtoRGB(float hue, float sat, float val);
	long TestCollisionSpheres(Tr4ItemInfo* item, const Vector3f& posTest, float radius);
	int FindNearestTarget(const Vector3f& posTest, float radius, short* const slotList, int count);
	Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff);
	Vector3f RotatePoint3D(const Vector3f& point, short xrot, short yrot, short zrot);
	Vector3f SphericalToCartesian(float r, float theta, float phi);
	int TestForWall(int x, int y, int z, short* room);
	Vector3f GetSlopeNormal(Tr4FloorInfo* floor, int x, int y, int z);
	Vector3f SplinePos(float t, Vector3f v[], int arrSize);
	Vector3f SplineVel(float t, float duration, Vector3f v[], int arrSize);
	int Clamp(int x, int min, int max);
	float Clamp(float x, float min, float max);
	float ShortToRad(short rotation);
	short RadToShort(float angle);
	float GetRandom();
	float RandomNegate(float x);
	float DegToRad(float deg);
	float RadToDeg(float rad);
	ushort ConvertTo16BitBGR(const ColorRGB& c);
	const char* FormatString(const char* format, ...);
}
