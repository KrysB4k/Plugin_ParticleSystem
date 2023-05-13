#include "..\definitions\includes.h"

// ************  Utility Functions - can be made accessible in Lua, unless stated otherwise ************

int Round(float x)
{
	return (x > 0.0f) ? int(x+0.5f) : int(x-0.5f);
}


phd_vector Round(const Vector3f& v)
{
	return phd_vector(Round(v.x), Round(v.y), Round(v.z));
}


short GetOrientDiff(short sourceOrient, short targetOrient)
{
	int diff = targetOrient - sourceOrient;
	
	return short(diff); // returns (+) if to the right of source orient, (-) if to the left
}


float Lerp(float a, float b, float t)
{
	if (t > 1.0f)
		t = 1.0f;
	else if (t < 0.0f)
		t = 0.0f;

	return a + t*(b-a);
}


float InverseLerp(float val1, float val2, float x)
{
	float t = (x - val1) / (val2 - val1);

		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;

	return t;
}


float SimpleDist(const Vector3f &v1, const Vector3f &v2)
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
		Round(C1.R + (C2.R - C1.R) * t),
		Round(C1.G + (C2.G - C1.G) * t),
		Round(C1.B + (C2.B - C1.B) * t)
		);
}


ColorRGB HSLtoRGB(float hue, float sat, float light)
{
	hue = fmodf(hue, 360.0f);
	if (hue < 0)
		hue += 360.0f;

	sat = Clamp(sat, 0.f, 1.f);
	light = Clamp(light, 0.f, 1.f);

	if (sat <= 0.f)
	{
		int v = Round(light * 255);
		return ColorRGB(v, v, v);
	}

	int hextant = int(hue / 60.0f);
	float fhue = (hue - hextant * 60.0f) / 60.0f;

	float chroma = (1 - abs(2 * light - 1)) * sat;
	float xval = chroma * (1 - abs(fmod(hextant + fhue, 2.0f) - 1));
	float m = light - chroma / 2;
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

	ColorRGB c(Round((r + m) * 255),
		Round((g + m) * 255),
		Round((b + m) * 255));

	return c;
}


long TestCollisionSpheres(const Vector3f& posTest, Tr4ItemInfo* item, unsigned long bitMask)
{
	long flags = 0;
	int num = 0;

	if (item)
	{
		MeshSphere SphereList[34];
		num = GetSpheres(item, SphereList, 1);

		for (int i = 0; i < num; ++i)
		{
			auto sph = &SphereList[i];
			int r = sph->r;
			if (r > 0)
			{
				int x = Round(posTest.x - sph->x);
				int y = Round(posTest.y - sph->y);
				int z = Round(posTest.z - sph->z);

				if ((x*x + y*y + z*z) < r*r)
					flags |= (1 << i);
			}
		}
	}

	return flags;
}


int FindNearestTarget(const Vector3f& posTest, float radius, short* const slotList)
{
	int itemIndex = -1;
	int nearest = 0x7FFFFFFF;

	for (int i = 0; i < level_items; ++i)
	{
		auto item = &items[i];

		bool slotCheck = false;
		const short* slotIter = slotList;

		if (*slotIter == -1)
		{
			if (objects[item->object_number].intelligent &&
				item->object_number != SLOT_GUIDE &&
				item->object_number != SLOT_VON_CROY) {
				slotCheck = true;
			}
		}
		else
		{
			while (*slotIter >= 0)
			{
				if (*(slotIter++) == item->object_number)
				{
					slotCheck = true;
					break;
				}
			}
		}


		if (slotCheck && item->active)
		{
			if (objects[item->object_number].intelligent && item->hit_points <= 0)
				continue;

			Vector3f target(item->pos.xPos, item->pos.yPos, item->pos.zPos);

			int dist = Round(SimpleDist(posTest, target));
			if (dist < radius && dist < nearest)
			{
				nearest = dist;
				radius = dist;
				itemIndex = i;
			}
		}
	}

	return itemIndex;
}


Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff)
{
	phd_vector v(xOff, yOff, zOff);

	if (!item->object_number)
		GetLaraJointPos(&v, joint);
	else
		GetJointAbsPosition((StrItemTr4*)item, (StrMovePosition*)&v, joint);

	return Vector3f(v.x, v.y, v.z);
}


// should not be available in Lua
int TestForWall(int x, int y, int z, short* room)
{
	void *floor = GetFloor(x, y, z, room);

	int fh = GetHeight(floor, x, y, z);
	int ch = GetCeiling(floor, x, y, z);

	if (fh != (-0x7F00))
	{
		if ((fh-y) > 0 && (ch-y) < 0)
			return 0; // No wall

		return 2; // Block (floor or ceiling)
	}

	return 1; // Wall
}


// should not be available in Lua
Vector3f GetSlopeNormal(Tr4FloorInfo *floor, int x, int y, int z)
{
	if (!floor)
		return Vector3f(0, -1, 0);

	WORD tilts = GetTiltType(floor, x, y, z);
	signed char tilt_x = tilts & 0xFF;
	signed char tilt_z = tilts >> 8;

	return Vector3f(float(-tilt_x), -4.0f, float(-tilt_z)).normalized();
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
	return M_PI * rotation / 32768;
}

short RadToShort(float angle)
{
	return 32768 * angle / M_PI;
}

float GetRandom()
{
	return (float)rand() / (RAND_MAX + 1);
}
