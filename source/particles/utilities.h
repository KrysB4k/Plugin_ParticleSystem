// ************  Utility Functions - can be made accessible in Lua, unless stated otherwise ************

int Round(float x);

phd_vector Round(const Vector3f& v);

short GetOrientDiff(short sourceOrient, short targetOrient);

float Lerp(float a, float b, float t);

float InverseLerp(float val1, float val2, float x);

float SimpleDist(const Vector3f& v1, const Vector3f& v2);

int CheckDistFast(const Vector3f& v1, const Vector3f& v2, float dist);

float RealDist(const Vector3f& v1, const Vector3f& v2);

ColorRGB Lerp(const ColorRGB& C1, const ColorRGB& C2, float t);

ColorRGB HSVtoRGB(float hue, float sat, float val);

long TestCollisionSpheres(const Vector3f& posTest, Tr4ItemInfo* item, unsigned long bitMask);

int FindNearestTarget(const Vector3f& posTest, float radius, short* const slotList); // TODO: add lua function after implementing tables

Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff);

Vector3f RotatePoint3D(const Vector3f& point, short xrot, short yrot, short zrot);

Vector3f SphericalToCartesian(float r, float theta, float phi);

int TestForWall(int x, int y, int z, short* room);

Vector3f GetSlopeNormal(Tr4FloorInfo *floor, int x, int y, int z);

Vector3f Spline(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float t);

Vector3f SplineStart(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t);

Vector3f SplineEnd(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t);

Vector3f SplineDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float t);

Vector3f SplineStartDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t);

Vector3f SplineEndDerivative(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float t);

int Clamp(int x, int min, int max);

float Clamp(float x, float min, float max);

float ShortToRad(short rotation);

short RadToShort(float angle);

float GetRandom();

ushort ConvertTo16BitBGR(const ColorRGB& c);

const char* FormatString(const char* format, ...);
