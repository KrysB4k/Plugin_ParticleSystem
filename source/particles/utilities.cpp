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


ColorRGB RandomColor(int RedRange, int GreenRange, int BlueRange)
{
	uchar r = (RedRange > 0) ? (GetRandomControl() % RedRange) : 0;
	uchar g = (GreenRange > 0) ? (GetRandomControl() % GreenRange) : 0;
	uchar b = (BlueRange > 0) ? (GetRandomControl() % BlueRange) : 0;
	return ColorRGB(r, g, b);
}


ColorRGB AddColors(const ColorRGB& C1, const ColorRGB& C2, bool overflow)
{
	int r = C1.R + C2.R;
	int g = C1.G + C2.G;
	int b = C1.B + C2.B;

	if (!overflow)
	{
		r = (r > 255) ? 255 : r;
		g = (g > 255) ? 255 : g;
		b = (b > 255) ? 255 : b;
	}

	return ColorRGB(r&255, g&255, b&255);
}


ColorRGB HSLtoRGB(float hue, float sat, float light)
{
	hue = fmod(hue, 360.0f);
	if (sat < 0)
		sat = 0.0f;
	else if (sat > 1)
		sat = 1.0f;
	if (light < 0)
		light = 0.0f;
	else if (light > 1)
		light = 1.0f;

	int hextant = int(hue / 60.0f);
	float fhue = (hue - hextant * 60.0f) / 60.0f;

	float chroma = (1 - abs(2*light - 1)) * sat;
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


bool TestCollisionSpheres(const Vector3f& posTest, Tr4ItemInfo* item, unsigned long bitMask)
{
	int flags = 0;
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

	return (flags & bitMask) ? true : false;
}


Tr4ItemInfo* FindNearestTarget(const Vector3f& posTest, short* const slotList, float range)
{
	int nearest = 0x7FFFFFFF;
	Tr4ItemInfo* ret = nullptr;

	for (int i = 0; i < Trng.pGlobTomb4->pAdr->TotItemsMax; ++i)
	{
		auto item = &items[i];

		bool slotCheck = false;
		const short* slotIter = slotList;

		if (*slotIter == 0x7FFF)
		{
			if (objects[item->object_number].intelligent &&
				item->object_number != SLOT_GUIDE &&
				item->object_number != SLOT_VON_CROY)
				slotCheck = true;
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

			if (SimpleDist(posTest, target) < range)
			{
				int dist = Round(RealDist(posTest, target));
				if (dist < nearest)
				{
					ret = item;
					nearest = dist;
				}
			}
		}
	}

	return ret;
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