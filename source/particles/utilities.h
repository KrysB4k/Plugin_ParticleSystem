// ************  Utility Functions - can be made accessible in Lua, unless stated otherwise ************

int Round(float x);

phd_vector Round(const Vector3f& v);

short GetOrientDiff(short sourceOrient, short targetOrient);

float Lerp(float a, float b, float t);

Vector3f Lerp(const Vector3f& a, const Vector3f& b, float t);

float InverseLerp(float val1, float val2, float x);

float SimpleDist(const Vector3f& v1, const Vector3f& v2);

float RealDist(const Vector3f& v1, const Vector3f& v2);

ColorRGB Lerp(const ColorRGB& C1, const ColorRGB& C2, float t);

ColorRGB RandomColor(int RedRange, int GreenRange, int BlueRange);

ColorRGB AddColors(const ColorRGB& C1, const ColorRGB& C2, bool overflow);

ColorRGB HSLtoRGB(float hue, float sat, float light);

bool TestCollisionSpheres(const Vector3f& posTest, Tr4ItemInfo* item, unsigned long bitMask);

Tr4ItemInfo* FindNearestTarget(const Vector3f& posTest, short* const slotList, float range);

Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff);

// should not be available in Lua
int TestForWall(int x, int y, int z, short* room);

// should not be available in Lua
Vector3f GetSlopeNormal(Tr4FloorInfo *floor, int x, int y, int z);

int Clamp(int x, int min, int max);

float ShortToRad(short rotation);

short RadToShort(float angle);

float GetRandom();
