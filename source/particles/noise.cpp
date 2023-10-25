#include "..\definitions\includes.h"

void Noise::ReferencePermut()
{
	// Perlin's original permutation table
	const uchar refTable[] = {
		151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
		8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203,
		117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
		165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
		105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
		187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,
		3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
		227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70,
		221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178,
		185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
		81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176,
		115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
		78, 66, 215, 61, 156, 180
	};

	for (int i = 0; i < 256; ++i)
		permut[i] = permut[i + 256] = refTable[i];
}


void Noise::SeedPermut(int seed)
{
	// generate random permutation of integers 0-255
	int i;
	uchar table[256];

	i = 256;
	while (--i >= 0)
		table[i] = i;

	srand(seed);

	i = 256;
	while (--i > 0) // shuffle integers in table
	{
		int idx = rand() % (i + 1);

		auto temp = table[i];
		table[i] = table[idx];
		table[idx] = temp;
	}

	for (i = 255; i >= 0; i--)
		permut[i] = permut[i + 256] = table[i];
}


/*********** Perlin noise implementation ***********/

float Noise::PerlinNoise1D(float x) const
{
	int xi = fastfloor(x);

	float xf0 = x - xi;

	float tx = fade(xf0);

	xi &= 255;

	int h0 = permut[xi + 0];
	int h1 = permut[xi + 1];

	return lerp(dot1D(h0, xf0), dot1D(h1, xf0 - 1), tx) * 2.0f;
}


float Noise::PerlinNoise2D(float x, float y) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;

	xi &= 255;
	yi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);

	int h00 = permut[permut[xi + 0] + yi + 0];
	int h01 = permut[permut[xi + 0] + yi + 1];
	int h10 = permut[permut[xi + 1] + yi + 0];
	int h11 = permut[permut[xi + 1] + yi + 1];

	float x1 = lerp(dot2D(h00, xf0, yf0), dot2D(h10, xf1, yf0), tx);
	float x2 = lerp(dot2D(h01, xf0, yf1), dot2D(h11, xf1, yf1), tx);

	return lerp(x1, x2, ty);
}


float Noise::PerlinNoise3D(float x, float y, float z) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);
	int zi = fastfloor(z);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float zf0 = z - zi;
	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;
	float zf1 = zf0 - 1.0f;

	xi &= 255;
	yi &= 255;
	zi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);
	float tz = fade(zf0);

	int h000 = permut[permut[permut[xi + 0] + yi + 0] + zi + 0];
	int h001 = permut[permut[permut[xi + 0] + yi + 0] + zi + 1];
	int h010 = permut[permut[permut[xi + 0] + yi + 1] + zi + 0];
	int h011 = permut[permut[permut[xi + 0] + yi + 1] + zi + 1];
	int h100 = permut[permut[permut[xi + 1] + yi + 0] + zi + 0];
	int h101 = permut[permut[permut[xi + 1] + yi + 0] + zi + 1];
	int h110 = permut[permut[permut[xi + 1] + yi + 1] + zi + 0];
	int h111 = permut[permut[permut[xi + 1] + yi + 1] + zi + 1];

	float x11 = lerp(dot3D(h000, xf0, yf0, zf0), dot3D(h100, xf1, yf0, zf0), tx);
	float x12 = lerp(dot3D(h010, xf0, yf1, zf0), dot3D(h110, xf1, yf1, zf0), tx);
	float x21 = lerp(dot3D(h001, xf0, yf0, zf1), dot3D(h101, xf1, yf0, zf1), tx);
	float x22 = lerp(dot3D(h011, xf0, yf1, zf1), dot3D(h111, xf1, yf1, zf1), tx);

	float y1 = lerp(x11, x12, ty);
	float y2 = lerp(x21, x22, ty);

	return lerp(y1, y2, tz);
}


float Noise::PerlinNoise4D(float x, float y, float z, float w) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);
	int zi = fastfloor(z);
	int wi = fastfloor(w);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float zf0 = z - zi;
	float wf0 = w - wi;

	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;
	float zf1 = zf0 - 1.0f;
	float wf1 = wf0 - 1.0f;

	xi &= 255;
	yi &= 255;
	zi &= 255;
	wi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);
	float tz = fade(zf0);
	float tw = fade(wf0);

	int h0000 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 0] + wi + 0];
	int h0001 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 0] + wi + 1];
	int h0010 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 1] + wi + 0];
	int h0011 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 1] + wi + 1];
	int h0100 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 0] + wi + 0];
	int h0101 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 0] + wi + 1];
	int h0110 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 1] + wi + 0];
	int h0111 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 1] + wi + 1];
	int h1000 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 0] + wi + 0];
	int h1001 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 0] + wi + 1];
	int h1010 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 1] + wi + 0];
	int h1011 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 1] + wi + 1];
	int h1100 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 0] + wi + 0];
	int h1101 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 0] + wi + 1];
	int h1110 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 1] + wi + 0];
	int h1111 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 1] + wi + 1];

	float x111 = lerp(dot4D(h0000, xf0, yf0, zf0, wf0), dot4D(h1000, xf1, yf0, zf0, wf0), tx);
	float x112 = lerp(dot4D(h0100, xf0, yf1, zf0, wf0), dot4D(h1100, xf1, yf1, zf0, wf0), tx);
	float x121 = lerp(dot4D(h0010, xf0, yf0, zf1, wf0), dot4D(h1010, xf1, yf0, zf1, wf0), tx);
	float x122 = lerp(dot4D(h0110, xf0, yf1, zf1, wf0), dot4D(h1110, xf1, yf1, zf1, wf0), tx);
	float x211 = lerp(dot4D(h0001, xf0, yf0, zf0, wf1), dot4D(h1001, xf1, yf0, zf0, wf1), tx);
	float x212 = lerp(dot4D(h0101, xf0, yf1, zf0, wf1), dot4D(h1101, xf1, yf1, zf0, wf1), tx);
	float x221 = lerp(dot4D(h0011, xf0, yf0, zf1, wf1), dot4D(h1011, xf1, yf0, zf1, wf1), tx);
	float x222 = lerp(dot4D(h0111, xf0, yf1, zf1, wf1), dot4D(h1111, xf1, yf1, zf1, wf1), tx);

	float y11 = lerp(x111, x112, ty);
	float y12 = lerp(x121, x122, ty);
	float y21 = lerp(x211, x212, ty);
	float y22 = lerp(x221, x222, ty);

	float z1 = lerp(y11, y12, tz);
	float z2 = lerp(y21, y22, tz);

	return lerp(z1, z2, tw);
}


Vector3f Noise::PerlinGradient2D(float x, float y) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;

	xi &= 255;
	yi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);
	float dtx = fadederiv(xf0);
	float dty = fadederiv(yf0);

	int h00 = permut[permut[xi + 0] + yi + 0];
	int h10 = permut[permut[xi + 1] + yi + 0];
	int h01 = permut[permut[xi + 0] + yi + 1];
	int h11 = permut[permut[xi + 1] + yi + 1];

	float v00 = dot2D(h00, xf0, yf0);
	float v10 = dot2D(h10, xf1, yf0);
	float v01 = dot2D(h01, xf0, yf1);
	float v11 = dot2D(h11, xf1, yf1);

	float b = (v10 - v00);
	float c = (v01 - v00);
	float d = (v11 - v01 - v10 + v00);

	auto g00 = gradVec2D(h00);
	auto g10 = gradVec2D(h10);
	auto g01 = gradVec2D(h01);
	auto g11 = gradVec2D(h11);

	auto db = (g10 - g00);
	auto dc = (g01 - g00);
	auto dd = (g11 - g01 - g10 + g00);

	auto dnoise = g00 + db * tx + (dc + dd * tx) * ty;
	dnoise.x += (b + d * ty) * dtx;
	dnoise.y += (c + d * tx) * dty;
	dnoise.z = 0;

	return dnoise;
}


Vector3f Noise::PerlinGradient3D(float x, float y, float z) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);
	int zi = fastfloor(z);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float zf0 = z - zi;
	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;
	float zf1 = zf0 - 1.0f;

	xi &= 255;
	yi &= 255;
	zi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);
	float tz = fade(zf0);
	float dtx = fadederiv(xf0);
	float dty = fadederiv(yf0);
	float dtz = fadederiv(zf0);

	int h000 = permut[permut[permut[xi + 0] + yi + 0] + zi + 0];
	int h001 = permut[permut[permut[xi + 0] + yi + 0] + zi + 1];
	int h010 = permut[permut[permut[xi + 0] + yi + 1] + zi + 0];
	int h011 = permut[permut[permut[xi + 0] + yi + 1] + zi + 1];
	int h100 = permut[permut[permut[xi + 1] + yi + 0] + zi + 0];
	int h101 = permut[permut[permut[xi + 1] + yi + 0] + zi + 1];
	int h110 = permut[permut[permut[xi + 1] + yi + 1] + zi + 0];
	int h111 = permut[permut[permut[xi + 1] + yi + 1] + zi + 1];

	float v000 = dot3D(h000, xf0, yf0, zf0);
	float v100 = dot3D(h100, xf1, yf0, zf0);
	float v010 = dot3D(h010, xf0, yf1, zf0);
	float v001 = dot3D(h001, xf0, yf0, zf1);
	float v110 = dot3D(h110, xf1, yf1, zf0);
	float v101 = dot3D(h101, xf1, yf0, zf1);
	float v011 = dot3D(h011, xf0, yf1, zf1);
	float v111 = dot3D(h111, xf1, yf1, zf1);

	float b = (v100 - v000);
	float c = (v010 - v000);
	float d = (v001 - v000);
	float e = (v110 - v010 - v100 + v000);
	float f = (v101 - v001 - v100 + v000);
	float g = (v011 - v001 - v010 + v000);
	float h = (v111 - v011 - v101 - v110 + v100 + v010 + v001 - v000);

	auto g000 = gradVec3D(h000);
	auto g100 = gradVec3D(h100);
	auto g010 = gradVec3D(h010);
	auto g001 = gradVec3D(h001);
	auto g110 = gradVec3D(h110);
	auto g101 = gradVec3D(h101);
	auto g011 = gradVec3D(h011);
	auto g111 = gradVec3D(h111);

	auto db = g100 - g000;
	auto dc = g010 - g000;
	auto dd = g001 - g000;
	auto de = g110 - g010 - g100 + g000;
	auto df = g101 - g001 - g100 + g000;
	auto dg = g011 - g001 - g010 + g000;
	auto dh = g111 - g011 - g101 - g110 + g100 + g010 + g001 - g000;

	auto dnoise = g000 + db * tx + (dc + de * tx) * ty + (dd + df * tx + (dg + dh * tx) * ty) * tz;
	dnoise.x += (b + e * ty + (f + h * ty) * tz) * dtx;
	dnoise.y += (c + e * tx + (g + h * tx) * tz) * dty;
	dnoise.z += (d + f * tx + (g + h * tx) * ty) * dtz;

	return dnoise;
}


Vector3f Noise::PerlinGradient4D(float x, float y, float z, float w) const
{
	int xi = fastfloor(x);
	int yi = fastfloor(y);
	int zi = fastfloor(z);
	int wi = fastfloor(w);

	float xf0 = x - xi;
	float yf0 = y - yi;
	float zf0 = z - zi;
	float wf0 = w - wi;

	float xf1 = xf0 - 1.0f;
	float yf1 = yf0 - 1.0f;
	float zf1 = zf0 - 1.0f;
	float wf1 = wf0 - 1.0f;

	xi &= 255;
	yi &= 255;
	zi &= 255;
	wi &= 255;

	float tx = fade(xf0);
	float ty = fade(yf0);
	float tz = fade(zf0);
	float tw = fade(wf0);
	float dtx = fadederiv(xf0);
	float dty = fadederiv(yf0);
	float dtz = fadederiv(zf0);
	//float dtw = fadederiv(wf0); not needed, partial derivative in respect to the w variable won't be calculated

	int h0000 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 0] + wi + 0];
	int h0001 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 0] + wi + 1];
	int h0010 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 1] + wi + 0];
	int h0011 = permut[permut[permut[permut[xi + 0] + yi + 0] + zi + 1] + wi + 1];
	int h0100 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 0] + wi + 0];
	int h0101 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 0] + wi + 1];
	int h0110 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 1] + wi + 0];
	int h0111 = permut[permut[permut[permut[xi + 0] + yi + 1] + zi + 1] + wi + 1];
	int h1000 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 0] + wi + 0];
	int h1001 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 0] + wi + 1];
	int h1010 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 1] + wi + 0];
	int h1011 = permut[permut[permut[permut[xi + 1] + yi + 0] + zi + 1] + wi + 1];
	int h1100 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 0] + wi + 0];
	int h1101 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 0] + wi + 1];
	int h1110 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 1] + wi + 0];
	int h1111 = permut[permut[permut[permut[xi + 1] + yi + 1] + zi + 1] + wi + 1];

	float v0000 = dot4D(h0000, xf0, yf0, zf0, wf0);
	float v1000 = dot4D(h1000, xf1, yf0, zf0, wf0);
	float v0100 = dot4D(h0100, xf0, yf1, zf0, wf0);
	float v0010 = dot4D(h0010, xf0, yf0, zf1, wf0);
	float v0001 = dot4D(h0001, xf0, yf0, zf0, wf1);
	float v1100 = dot4D(h1100, xf1, yf1, zf0, wf0);
	float v1010 = dot4D(h1010, xf1, yf0, zf1, wf0);
	float v1001 = dot4D(h1001, xf1, yf0, zf0, wf1);
	float v0110 = dot4D(h0110, xf0, yf1, zf1, wf0);
	float v0101 = dot4D(h0101, xf0, yf1, zf0, wf1);
	float v0011 = dot4D(h0011, xf0, yf0, zf1, wf1);
	float v1110 = dot4D(h1110, xf1, yf1, zf1, wf0);
	float v1101 = dot4D(h1101, xf1, yf1, zf0, wf1);
	float v1011 = dot4D(h1011, xf1, yf0, zf1, wf1);
	float v0111 = dot4D(h0111, xf0, yf1, zf1, wf1);
	float v1111 = dot4D(h1111, xf1, yf1, zf1, wf1);

	float b = v1000 - v0000;
	float c = v0100 - v0000;
	float d = v0010 - v0000;
	float e = v0001 - v0000;
	float f = v1100 - v1000 - v0100 + v0000;
	float g = v1010 - v1000 - v0010 + v0000;
	float h = v1001 - v1000 - v0001 + v0000;
	float i = v0110 - v0100 - v0010 + v0000;
	float j = v0101 - v0100 - v0001 + v0000;
	float k = v0011 - v0010 - v0001 + v0000;
	float l = v1110 - v0110 - v1010 - v1100 + v1000 + v0100 + v0010 - v0000;
	float m = v1101 - v0101 - v1001 - v1100 + v1000 + v0100 + v0001 - v0000;
	float n = v1011 - v0011 - v1001 - v1010 + v1000 + v0010 + v0001 - v0000;
	float p = v0111 - v0011 - v0101 - v0110 + v0100 + v0010 + v0001 - v0000;
	float q = v1111 - v1110 - v1101 - v1011 - v0111 + v1100 + v1010 + v1001 + v0110 + v0101 + v0011 - v1000 - v0100 - v0010 - v0001 + v0000;

	auto g0000 = gradVec4D(h0000);
	auto g1000 = gradVec4D(h1000);
	auto g0100 = gradVec4D(h0100);
	auto g0010 = gradVec4D(h0010);
	auto g0001 = gradVec4D(h0001);
	auto g1100 = gradVec4D(h1100);
	auto g1010 = gradVec4D(h1010);
	auto g1001 = gradVec4D(h1001);
	auto g0110 = gradVec4D(h0110);
	auto g0101 = gradVec4D(h0101);
	auto g0011 = gradVec4D(h0011);
	auto g1110 = gradVec4D(h1110);
	auto g1101 = gradVec4D(h1101);
	auto g1011 = gradVec4D(h1011);
	auto g0111 = gradVec4D(h0111);
	auto g1111 = gradVec4D(h1111);

	auto db = g1000 - g0000;
	auto dc = g0100 - g0000;
	auto dd = g0010 - g0000;
	auto de = g0001 - g0000;
	auto df = g1100 - g1000 - g0100 + g0000;
	auto dg = g1010 - g1000 - g0010 + g0000;
	auto dh = g1001 - g1000 - g0001 + g0000;
	auto di = g0110 - g0100 - g0010 + g0000;
	auto dj = g0101 - g0100 - g0001 + g0000;
	auto dk = g0011 - g0010 - g0001 + g0000;
	auto dl = g1110 - g1100 - g1010 - g0110 + g1000 + g0100 + g0010 - g0000;
	auto dm = g1101 - g1100 - g1001 - g0101 + g1000 + g0100 + g0001 - g0000;
	auto dn = g1011 - g1010 - g1001 - g0011 + g1000 + g0010 + g0001 - g0000;
	auto dp = g0111 - g0110 - g0101 - g0011 + g0100 + g0010 + g0001 - g0000;
	auto dq = g1111 - g1110 - g1101 - g1011 - g0111 + g1100 + g1010 + g1001 + g0110 + g0101 + g0011 - g1000 - g0100 - g0010 - g0001 + g0000;

	// derived the 4D gradient equations myself, it was a blast...

	auto dnoise = g0000 + db * tx + (dc + df * tx) * ty + (dd + dg * tx + (di + dl * tx) * ty) * tz + (de + dh * tx + (dj + dm * tx) * ty + (dk + dn * tx + (dp + dq * tx) * ty) * tz) * tw;

	dnoise.x += (b + (f + l * tz) * ty + g * tz + (h + m * ty + (n + q * ty) * tz) * tw) * dtx;
	dnoise.y += (c + (f + l * tz) * tx + i * tz + (j + m * tx + (p + q * tx) * tz) * tw) * dty;
	dnoise.z += (d + (g + l * ty) * tx + i * ty + (k + n * tx + (p + q * tx) * ty) * tw) * dtz;
	//gradient.w += (e + (h + m * ty) * tx + j * ty + (k + n * tx + (p + q * tx) * ty) * tz) * dtw; we don't need the w partial derivative

	return Vector3f(dnoise.x, dnoise.y, dnoise.z); // return (dx, dy, dz) only
}


Vector3f Noise::PerlinCurl2D(float x, float y) const
{
	auto gradient = PerlinGradient2D(x, y);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f Noise::PerlinCurl2DTime(float time, float x, float y) const
{
	auto gradient = PerlinGradient3D(x, y, time);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f Noise::PerlinCurl3D(float x, float y, float z) const
{
	// sample noise gradients at different offsets
	auto gx = PerlinGradient3D(x, y, z);
	auto gy = PerlinGradient3D(x - 123.4567f, y + 56.7891f, z - 345.6789f);
	auto gz = PerlinGradient3D(x + 56.7891f, y - 123.4567f, z + 345.6789f);

	return computecurl3D(gx, gy, gz);
}


Vector3f Noise::PerlinCurl3DTime(float time, float x, float y, float z) const
{
	auto gx = PerlinGradient4D(x, y, z, time);
	auto gy = PerlinGradient4D(x - 123.4567f, y + 56.7891f, z - 345.6789f, time + 85.33333f);
	auto gz = PerlinGradient4D(x + 56.7891f, y - 123.4567f, z + 345.6789f, time + 170.66667f);

	return computecurl3D(gx, gy, gz);
}


/************* Simplex noise implementation *************/

const float Noise::grad2lut[8][2] = {
	{ -1.0f, -1.0f } , { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
	{ -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
};

const float Noise::grad3lut[16][3] = {
	{ 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
	{ -1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 1.0f },
	{ 1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, -1.0f },
	{ -1.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
	{ -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, -1.0f }, { 0.0f, -1.0f, -1.0f }
};

const float Noise::grad4lut[32][4] = {
  { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f, -1.0f, -1.0f },
  { 0.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 1.0f, -1.0f }, { 0.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, -1.0f, -1.0f, -1.0f },
  { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f, -1.0f }, { 1.0f, 0.0f, -1.0f, 1.0f }, { 1.0f, 0.0f, -1.0f, -1.0f },
  { -1.0f, 0.0f, 1.0f, 1.0f }, { -1.0f, 0.0f, 1.0f, -1.0f }, { -1.0f, 0.0f, -1.0f, 1.0f }, { -1.0f, 0.0f, -1.0f, -1.0f },
  { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, -1.0f }, { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, -1.0f, 0.0f, -1.0f },
  { -1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f, 1.0f, 0.0f, -1.0f }, { -1.0f, -1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f, 0.0f, -1.0f },
  { 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, -1.0f, 0.0f }, { 1.0f, -1.0f, 1.0f, 0.0f }, { 1.0f, -1.0f, -1.0f, 0.0f },
  { -1.0f, 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, -1.0f, 0.0f }, { -1.0f, -1.0f, 1.0f, 0.0f }, { -1.0f, -1.0f, -1.0f, 0.0f }
};

const uchar Noise::simplexlut[64][4] = {
  {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
  {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
  {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
  {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
};


float Noise::SimplexNoise1D(float x) const
{
	float n0 = 0, n1 = 0;
	float gx0 = 0, gx1 = 0;

	int i0 = fastfloor(x);
	float x0 = x - i0;
	float x1 = x0 - 1.0f;

	i0 &= 255;

	grad1(permut[i0], gx0);
	float t0 = 1.0f - x0 * x0;
	n0 = (t0 * t0 * t0) * gx0 * x0;

	grad1(permut[i0 + 1], gx1);
	float t1 = 1.0f - x1 * x1;
	n1 = (t1 * t1 * t1) * gx1 * x1;

	return (n0 + n1) * 0.295f;
}


float Noise::SimplexNoise2D(float x, float y) const
{
	float n0 = 0, n1 = 0, n2 = 0;

	// multiply input coordinates by 2/3 to match scale of original perlin noise
	x *= 0.66666667f;
	y *= 0.66666667f;

	float gx0, gy0;
	float gx1, gy1;
	float gx2, gy2;
	float t0, t1, t2;

	float s = (x + y) * F2;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);

	float u = (i + j) * G2;
	float x0 = x - (i - u);
	float y0 = y - (j - u);

	int i1 = 0, j1 = 1;
	if (x0 > y0) { i1 = 1; j1 = 0; }

	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + (2.0f * G2);
	float y2 = y0 - 1.0f + (2.0f * G2);

	int ii = i & 255;
	int jj = j & 255;

	t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 > 0.0f)
	{
		grad2(permut[ii + permut[jj]], gx0, gy0);
		n0 = (t0 * t0 * t0) * (gx0 * x0 + gy0 * y0);
	}

	t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 > 0.0f)
	{
		grad2(permut[ii + i1 + permut[jj + j1]], gx1, gy1);
		n1 = (t1 * t1 * t1) * (gx1 * x1 + gy1 * y1);
	}

	t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 > 0.0f)
	{
		grad2(permut[ii + 1 + permut[jj + 1]], gx2, gy2);
		n2 = (t2 * t2 * t2) * (gx2 * x2 + gy2 * y2);
	}

	return (n0 + n1 + n2) * 16.0f;
}


float Noise::SimplexNoise3D(float x, float y, float z) const
{
	float n0 = 0, n1 = 0, n2 = 0, n3 = 0;

	x *= 0.66666667f;
	y *= 0.66666667f;
	z *= 0.66666667f;

	float gx0, gy0, gz0;
	float gx1, gy1, gz1;
	float gx2, gy2, gz2;
	float gx3, gy3, gz3;

	float t0, t1, t2, t3;

	float s = (x + y + z) * F3;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);
	int k = fastfloor(z + s);

	float u = (i + j + k) * G3;
	float x0 = x - (i - u);
	float y0 = y - (j - u);
	float z0 = z - (k - u);

	int i1, j1, k1;
	int i2, j2, k2;

	if (x0 >= y0)
	{
		if (y0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; }
		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; }
	}
	else
	{
		if (y0 < z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; }
		else if (x0 < z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; }
		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
	}

	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + (2.0f * G3);
	float y2 = y0 - j2 + (2.0f * G3);
	float z2 = z0 - k2 + (2.0f * G3);
	float x3 = x0 - 1.0f + (3.0f * G3);
	float y3 = y0 - 1.0f + (3.0f * G3);
	float z3 = z0 - 1.0f + (3.0f * G3);

	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;

	t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 > 0.0f)
	{
		grad3(permut[ii + permut[jj + permut[kk]]], gx0, gy0, gz0);
		n0 = (t0 * t0 * t0) * (gx0 * x0 + gy0 * y0 + gz0 * z0);
	}

	t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 > 0.0f)
	{
		grad3(permut[ii + i1 + permut[jj + j1 + permut[kk + k1]]], gx1, gy1, gz1);
		n1 = (t1 * t1 * t1) * (gx1 * x1 + gy1 * y1 + gz1 * z1);
	}

	t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 > 0.0f)
	{
		grad3(permut[ii + i2 + permut[jj + j2 + permut[kk + k2]]], gx2, gy2, gz2);
		n2 = (t2 * t2 * t2) * (gx2 * x2 + gy2 * y2 + gz2 * z2);
	}

	t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 > 0.0f)
	{
		grad3(permut[ii + 1 + permut[jj + 1 + permut[kk + 1]]], gx3, gy3, gz3);
		n3 = (t3 * t3 * t3) * (gx3 * x3 + gy3 * y3 + gz3 * z3);
	}

	return (n0 + n1 + n2 + n3) * 10.0f;
}

float Noise::SimplexNoise4D(float x, float y, float z, float w) const
{
	float n0 = 0, n1 = 0, n2 = 0, n3 = 0, n4 = 0;

	x *= 0.66666667f;
	y *= 0.66666667f;
	z *= 0.66666667f;
	w *= 0.66666667f;

	float gx0, gy0, gz0, gw0;
	float gx1, gy1, gz1, gw1;
	float gx2, gy2, gz2, gw2;
	float gx3, gy3, gz3, gw3;
	float gx4, gy4, gz4, gw4;

	float t0, t1, t2, t3, t4;

	float s = (x + y + z + w) * F4;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);
	int k = fastfloor(z + s);
	int l = fastfloor(w + s);

	float u = (i + j + k + l) * G4;
	float x0 = x - (i - u);
	float y0 = y - (j - u);
	float z0 = z - (k - u);
	float w0 = w - (l - u);

	int c1 = (x0 > y0) ? 32 : 0;
	int c2 = (x0 > z0) ? 16 : 0;
	int c3 = (y0 > z0) ? 8 : 0;
	int c4 = (x0 > w0) ? 4 : 0;
	int c5 = (y0 > w0) ? 2 : 0;
	int c6 = (z0 > w0) ? 1 : 0;
	int c = c1 | c2 | c3 | c4 | c5 | c6;

	int i1, j1, k1, l1;
	int i2, j2, k2, l2;
	int i3, j3, k3, l3;

	i1 = simplexlut[c][0] >= 3 ? 1 : 0;
	i2 = simplexlut[c][0] >= 2 ? 1 : 0;
	i3 = simplexlut[c][0] >= 1 ? 1 : 0;

	j1 = simplexlut[c][1] >= 3 ? 1 : 0;
	j2 = simplexlut[c][1] >= 2 ? 1 : 0;
	j3 = simplexlut[c][1] >= 1 ? 1 : 0;

	k1 = simplexlut[c][2] >= 3 ? 1 : 0;
	k2 = simplexlut[c][2] >= 2 ? 1 : 0;
	k3 = simplexlut[c][2] >= 1 ? 1 : 0;

	l1 = simplexlut[c][3] >= 3 ? 1 : 0;
	l2 = simplexlut[c][3] >= 2 ? 1 : 0;
	l3 = simplexlut[c][3] >= 1 ? 1 : 0;

	float x1 = x0 - i1 + G4;
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	float x2 = x0 - i2 + (2.0f * G4);
	float y2 = y0 - j2 + (2.0f * G4);
	float z2 = z0 - k2 + (2.0f * G4);
	float w2 = w0 - l2 + (2.0f * G4);
	float x3 = x0 - i3 + (3.0f * G4);
	float y3 = y0 - j3 + (3.0f * G4);
	float z3 = z0 - k3 + (3.0f * G4);
	float w3 = w0 - l3 + (3.0f * G4);
	float x4 = x0 - 1.0f + (4.0f * G4);
	float y4 = y0 - 1.0f + (4.0f * G4);
	float z4 = z0 - 1.0f + (4.0f * G4);
	float w4 = w0 - 1.0f + (4.0f * G4);

	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	int ll = l & 255;

	t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if (t0 > 0.0f)
	{
		grad4(permut[ii + permut[jj + permut[kk + permut[ll]]]], gx0, gy0, gz0, gw0);
		n0 = (t0 * t0 * t0) * (gx0 * x0 + gy0 * y0 + gz0 * z0 + gw0 * w0);
	}

	t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if (t1 > 0.0f)
	{
		grad4(permut[ii + i1 + permut[jj + j1 + permut[kk + k1 + permut[ll + l1]]]], gx1, gy1, gz1, gw1);
		n1 = (t1 * t1 * t1) * (gx1 * x1 + gy1 * y1 + gz1 * z1 + gw1 * w1);
	}

	t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if (t2 > 0.0f)
	{
		grad4(permut[ii + i2 + permut[jj + j2 + permut[kk + k2 + permut[ll + l2]]]], gx2, gy2, gz2, gw2);
		n2 = (t2 * t2 * t2) * (gx2 * x2 + gy2 * y2 + gz2 * z2 + gw2 * w2);
	}

	t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if (t3 > 0.0f)
	{
		grad4(permut[ii + i3 + permut[jj + j3 + permut[kk + k3 + permut[ll + l3]]]], gx3, gy3, gz3, gw3);
		n3 = (t3 * t3 * t3) * (gx3 * x3 + gy3 * y3 + gz3 * z3 + gw3 * w3);
	}

	t4 = 0.6f - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if (t4 > 0.0f)
	{
		grad4(permut[ii + 1 + permut[jj + 1 + permut[kk + 1 + permut[ll + 1]]]], gx4, gy4, gz4, gw4);
		n4 = (t4 * t4 * t4) * (gx4 * x4 + gy4 * y4 + gz4 * z4 + gw4 * w4);
	}

	return (n0 + n1 + n2 + n3 + n4) * 10.0f;
}

Vector3f Noise::SimplexGradient2D(float x, float y) const
{
	x *= 0.66666667f;
	y *= 0.66666667f;

	float gx0, gy0;
	float gx1, gy1;
	float gx2, gy2;
	float t0, t1, t2;
	float t20, t21, t22;
	float t30, t31, t32;

	float s = (x + y) * F2;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);

	float u = (i + j) * G2;
	float x0 = x - (i - u);
	float y0 = y - (j - u);

	int i1 = 0, j1 = 1;
	if (x0 > y0) { i1 = 1; j1 = 0; }

	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + (2.0f * G2);
	float y2 = y0 - 1.0f + (2.0f * G2);

	int ii = i & 255;
	int jj = j & 255;

	t30 = t20 = gx0 = gy0 = 0.0f;
	t31 = t21 = gx1 = gy1 = 0.0f;
	t32 = t22 = gx2 = gy2 = 0.0f;

	t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 > 0.0f)
	{
		grad2(permut[ii + permut[jj]], gx0, gy0);
		t20 = t0 * t0;
		t30 = t20 * t0;
	}

	t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 > 0.0f)
	{
		grad2(permut[ii + i1 + permut[jj + j1]], gx1, gy1);
		t21 = t1 * t1;
		t31 = t21 * t1;
	}

	t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 > 0.0f)
	{
		grad2(permut[ii + 1 + permut[jj + 1]], gx2, gy2);
		t22 = t2 * t2;
		t32 = t22 * t2;
	}

	float temp0 = t20 * (gx0 * x0 + gy0 * y0);
	float dnoise_dx = temp0 * x0;
	float dnoise_dy = temp0 * y0;

	float temp1 = t21 * (gx1 * x1 + gy1 * y1);
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;

	float temp2 = t22 * (gx2 * x2 + gy2 * y2);
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;

	dnoise_dx *= -6.0f;
	dnoise_dy *= -6.0f;

	dnoise_dx += t30 * gx0 + t31 * gx1 + t32 * gx2;
	dnoise_dy += t30 * gy0 + t31 * gy1 + t32 * gy2;

	return Vector3f(dnoise_dx * 10.0f, dnoise_dy * 10.0f, 0);
}


Vector3f Noise::SimplexGradient3D(float x, float y, float z) const
{
	x *= 0.66666667f;
	y *= 0.66666667f;
	z *= 0.66666667f;

	float gx0, gy0, gz0;
	float gx1, gy1, gz1;
	float gx2, gy2, gz2;
	float gx3, gy3, gz3;

	float t0, t1, t2, t3;
	float t20, t21, t22, t23;
	float t30, t31, t32, t33;

	float s = (x + y + z) * F3;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);
	int k = fastfloor(z + s);

	float u = (i + j + k) * G3;
	float x0 = x - (i - u);
	float y0 = y - (j - u);
	float z0 = z - (k - u);

	int i1, j1, k1;
	int i2, j2, k2;

	if (x0 >= y0)
	{
		if (y0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; }
		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; }
	}
	else
	{
		if (y0 < z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; }
		else if (x0 < z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; }
		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
	}

	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + (2.0f * G3);
	float y2 = y0 - j2 + (2.0f * G3);
	float z2 = z0 - k2 + (2.0f * G3);
	float x3 = x0 - 1.0f + (3.0f * G3);
	float y3 = y0 - 1.0f + (3.0f * G3);
	float z3 = z0 - 1.0f + (3.0f * G3);

	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;

	t20 = t30 = gx0 = gy0 = gz0 = 0.0f;
	t21 = t31 = gx1 = gy1 = gz1 = 0.0f;
	t22 = t32 = gx2 = gy2 = gz2 = 0.0f;
	t23 = t33 = gx3 = gy3 = gz3 = 0.0f;

	t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 > 0.0f)
	{
		grad3(permut[ii + permut[jj + permut[kk]]], gx0, gy0, gz0);
		t20 = t0 * t0;
		t30 = t20 * t0;
	}

	t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 > 0.0f)
	{
		grad3(permut[ii + i1 + permut[jj + j1 + permut[kk + k1]]], gx1, gy1, gz1);
		t21 = t1 * t1;
		t31 = t21 * t1;
	}

	t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 > 0.0f)
	{
		grad3(permut[ii + i2 + permut[jj + j2 + permut[kk + k2]]], gx2, gy2, gz2);
		t22 = t2 * t2;
		t32 = t22 * t2;
	}

	t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 > 0.0f)
	{
		grad3(permut[ii + 1 + permut[jj + 1 + permut[kk + 1]]], gx3, gy3, gz3);
		t23 = t3 * t3;
		t33 = t23 * t3;
	}

	float temp0 = t20 * (gx0 * x0 + gy0 * y0 + gz0 * z0);
	float dnoise_dx = temp0 * x0;
	float dnoise_dy = temp0 * y0;
	float dnoise_dz = temp0 * z0;

	float temp1 = t21 * (gx1 * x1 + gy1 * y1 + gz1 * z1);
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;
	dnoise_dz += temp1 * z1;

	float temp2 = t22 * (gx2 * x2 + gy2 * y2 + gz2 * z2);
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;
	dnoise_dz += temp2 * z2;

	float temp3 = t23 * (gx3 * x3 + gy3 * y3 + gz3 * z3);
	dnoise_dx += temp3 * x3;
	dnoise_dy += temp3 * y3;
	dnoise_dz += temp3 * z3;

	dnoise_dx *= -6.0f;
	dnoise_dy *= -6.0f;
	dnoise_dz *= -6.0f;

	dnoise_dx += t30 * gx0 + t31 * gx1 + t32 * gx2 + t33 * gx3;
	dnoise_dy += t30 * gy0 + t31 * gy1 + t32 * gy2 + t33 * gy3;
	dnoise_dz += t30 * gz0 + t31 * gz1 + t32 * gz2 + t33 * gz3;

	return Vector3f(dnoise_dx, dnoise_dy, dnoise_dz) * 7.0f;
}


Vector3f Noise::SimplexGradient4D(float x, float y, float z, float w) const
{
	x *= 0.66666667f;
	y *= 0.66666667f;
	z *= 0.66666667f;
	w *= 0.66666667f;

	float gx0, gy0, gz0, gw0;
	float gx1, gy1, gz1, gw1;
	float gx2, gy2, gz2, gw2;
	float gx3, gy3, gz3, gw3;
	float gx4, gy4, gz4, gw4;

	float t0, t1, t2, t3, t4;
	float t20, t21, t22, t23, t24;
	float t30, t31, t32, t33, t34;

	float s = (x + y + z + w) * F4;
	int i = fastfloor(x + s);
	int j = fastfloor(y + s);
	int k = fastfloor(z + s);
	int l = fastfloor(w + s);

	float u = (i + j + k + l) * G4;
	float x0 = x - (i - u);
	float y0 = y - (j - u);
	float z0 = z - (k - u);
	float w0 = w - (l - u);

	int c1 = (x0 > y0) ? 32 : 0;
	int c2 = (x0 > z0) ? 16 : 0;
	int c3 = (y0 > z0) ? 8 : 0;
	int c4 = (x0 > w0) ? 4 : 0;
	int c5 = (y0 > w0) ? 2 : 0;
	int c6 = (z0 > w0) ? 1 : 0;
	int c = c1 | c2 | c3 | c4 | c5 | c6;

	int i1, j1, k1, l1;
	int i2, j2, k2, l2;
	int i3, j3, k3, l3;

	i1 = simplexlut[c][0] >= 3 ? 1 : 0;
	i2 = simplexlut[c][0] >= 2 ? 1 : 0;
	i3 = simplexlut[c][0] >= 1 ? 1 : 0;

	j1 = simplexlut[c][1] >= 3 ? 1 : 0;
	j2 = simplexlut[c][1] >= 2 ? 1 : 0;
	j3 = simplexlut[c][1] >= 1 ? 1 : 0;

	k1 = simplexlut[c][2] >= 3 ? 1 : 0;
	k2 = simplexlut[c][2] >= 2 ? 1 : 0;
	k3 = simplexlut[c][2] >= 1 ? 1 : 0;

	l1 = simplexlut[c][3] >= 3 ? 1 : 0;
	l2 = simplexlut[c][3] >= 2 ? 1 : 0;
	l3 = simplexlut[c][3] >= 1 ? 1 : 0;

	float x1 = x0 - i1 + G4;
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	float x2 = x0 - i2 + (2.0f * G4);
	float y2 = y0 - j2 + (2.0f * G4);
	float z2 = z0 - k2 + (2.0f * G4);
	float w2 = w0 - l2 + (2.0f * G4);
	float x3 = x0 - i3 + (3.0f * G4);
	float y3 = y0 - j3 + (3.0f * G4);
	float z3 = z0 - k3 + (3.0f * G4);
	float w3 = w0 - l3 + (3.0f * G4);
	float x4 = x0 - 1.0f + (4.0f * G4);
	float y4 = y0 - 1.0f + (4.0f * G4);
	float z4 = z0 - 1.0f + (4.0f * G4);
	float w4 = w0 - 1.0f + (4.0f * G4);

	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	int ll = l & 255;

	t20 = t30 = gx0 = gy0 = gz0 = gw0 = 0.0f;
	t21 = t31 = gx1 = gy1 = gz1 = gw1 = 0.0f;
	t22 = t32 = gx2 = gy2 = gz2 = gw2 = 0.0f;
	t23 = t33 = gx3 = gy3 = gz3 = gw3 = 0.0f;
	t24 = t34 = gx4 = gy4 = gz4 = gw4 = 0.0f;

	t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if (t0 > 0.0f)
	{
		t20 = t0 * t0;
		t30 = t20 * t0;
		grad4(permut[ii + permut[jj + permut[kk + permut[ll]]]], gx0, gy0, gz0, gw0);
	}

	t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if (t1 > 0.0f)
	{
		t21 = t1 * t1;
		t31 = t21 * t1;
		grad4(permut[ii + i1 + permut[jj + j1 + permut[kk + k1 + permut[ll + l1]]]], gx1, gy1, gz1, gw1);
	}

	t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if (t2 > 0.0f)
	{
		t22 = t2 * t2;
		t32 = t22 * t2;
		grad4(permut[ii + i2 + permut[jj + j2 + permut[kk + k2 + permut[ll + l2]]]], gx2, gy2, gz2, gw2);
	}

	t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if (t3 > 0.0f)
	{
		t23 = t3 * t3;
		t33 = t23 * t3;
		grad4(permut[ii + i3 + permut[jj + j3 + permut[kk + k3 + permut[ll + l3]]]], gx3, gy3, gz3, gw3);
	}

	t4 = 0.6f - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if (t4 > 0.0f)
	{
		t24 = t4 * t4;
		t34 = t24 * t4;
		grad4(permut[ii + 1 + permut[jj + 1 + permut[kk + 1 + permut[ll + 1]]]], gx4, gy4, gz4, gw4);
	}

	float temp0 = t20 * (gx0 * x0 + gy0 * y0 + gz0 * z0 + gw0 * w0);
	float dnoise_dx = x0 * temp0;
	float dnoise_dy = y0 * temp0;
	float dnoise_dz = z0 * temp0;
	//float dnoise_dw = w0 * temp0; // ignore the w partial derivative, we don't need it

	float temp1 = t21 * (gx1 * x1 + gy1 * y1 + gz1 * z1 + gw1 * w1);
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;
	dnoise_dz += temp1 * z1;
	//dnoise_dw += temp1 * w1;

	float temp2 = t22 * (gx2 * x2 + gy2 * y2 + gz2 * z2 + gw2 * w2);
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;
	dnoise_dz += temp2 * z2;
	//dnoise_dw += temp2 * w2;

	float temp3 = t23 * (gx3 * x3 + gy3 * y3 + gz3 * z3 + gw3 * w3);
	dnoise_dx += temp3 * x3;
	dnoise_dy += temp3 * y3;
	dnoise_dz += temp3 * z3;
	//dnoise_dw += temp3 * w3;

	float temp4 = t24 * (gx4 * x4 + gy4 * y4 + gz4 * z4 + gw4 * w4);
	dnoise_dx += temp4 * x4;
	dnoise_dy += temp4 * y4;
	dnoise_dz += temp4 * z4;
	//dnoise_dw += temp4 * w4;

	dnoise_dx *= -6.0f;
	dnoise_dy *= -6.0f;
	dnoise_dz *= -6.0f;
	//dnoise_dw *= -6.0f;

	dnoise_dx += t30 * gx0 + t31 * gx1 + t32 * gx2 + t33 * gx3 + t34 * gx4;
	dnoise_dy += t30 * gy0 + t31 * gy1 + t32 * gy2 + t33 * gy3 + t34 * gy4;
	dnoise_dz += t30 * gz0 + t31 * gz1 + t32 * gz2 + t33 * gz3 + t34 * gz4;
	//dnoise_dw += t30 * gw0 + t31 * gw1 + t32 * gw2 + t33 * gw3 + t34 * gw4;

	return Vector3f(dnoise_dx, dnoise_dy, dnoise_dz) * 8.0f; // return (dx, dy, dz) only
}


Vector3f Noise::SimplexCurl2D(float x, float y) const
{
	auto gradient = SimplexGradient2D(x, y);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f Noise::SimplexCurl2DTime(float time, float x, float y) const
{
	auto gradient = SimplexGradient3D(x, y, time);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f Noise::SimplexCurl3D(float x, float y, float z) const
{
	auto gx = SimplexGradient3D(x, y, z);
	auto gy = SimplexGradient3D(x - 197.53072f, y + 90.86256f, z - 553.08624f);
	auto gz = SimplexGradient3D(x + 90.86256f, y - 197.53072f, z + 553.08624f);

	return computecurl3D(gx, gy, gz);
}


Vector3f Noise::SimplexCurl3DTime(float time, float x, float y, float z) const
{
	auto gx = SimplexGradient4D(x, y, z, time);
	auto gy = SimplexGradient4D(x - 197.53072f, y + 90.86256f, z - 553.08624f, time + 85.33333f);
	auto gz = SimplexGradient4D(x + 90.86256f, y - 197.53072f, z + 553.08624f, time + 170.66667f);

	return computecurl3D(gx, gy, gz);
}
