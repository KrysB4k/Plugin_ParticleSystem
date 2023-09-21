#include "..\definitions\includes.h"

PerlinNoise::PerlinNoise()
{
	ReferencePermut();
}

PerlinNoise::PerlinNoise(int seed)
{
	SeedPermut(seed);
}

void PerlinNoise::ReferencePermut()
{
	// Perlin's original permutation table
	const unsigned char reference[] = {
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
		permut[i] = permut[i + 256] = reference[i];
}


void PerlinNoise::SeedPermut(int seed)
{
	// generate random permutation of integers 0-255
	int i;
	unsigned char table[256];

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


float PerlinNoise::Noise1D(float domainScale, float x) const
{
	if (domainScale) // scale the noise domain
	{
		float s = (1 / domainScale);
		x *= s;
	}

	int xi = floor(x);

	float xf0 = x - xi;

	xi &= 255;

	float tx = fade(xf0);

	int h0 = permut[xi + 0];
	int h1 = permut[xi + 1];

	return lerp(dot1D(h0, xf0), dot1D(h1, xf0 - 1), tx);
}


float PerlinNoise::Noise2D(float domainScale, float x, float y) const
{
	if (domainScale)
	{
		float s = (1 / domainScale);
		x *= s;
		y *= s;
	}

	int xi = floor(x);
	int yi = floor(y);

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


float PerlinNoise::Noise3D(float domainScale, float x, float y, float z) const
{
	if (domainScale)
	{
		float s = (1 / domainScale);
		x *= s;
		y *= s;
		z *= s;
	}

	int xi = floor(x);
	int yi = floor(y);
	int zi = floor(z);

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


float PerlinNoise::Noise4D(float domainScale, float x, float y, float z, float w) const
{
	if (domainScale)
	{
		float s = (1 / domainScale);
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	int xi = floor(x);
	int yi = floor(y);
	int zi = floor(z);
	int wi = floor(w);

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


Vector3f PerlinNoise::NoiseGradient2D(float domainScale, float x, float y) const
{
	if (domainScale)
	{
		float s = (1 / domainScale);
		x *= s;
		y *= s;
	}

	int xi = floor(x);
	int yi = floor(y);

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

	float a = dot2D(h00, xf0, yf0);
	float v10 = dot2D(h10, xf1, yf0);
	float v01 = dot2D(h01, xf0, yf1);
	float v11 = dot2D(h11, xf1, yf1);

	float b = (v10 - a);
	float c = (v01 - a);
	float d = (v11 - v01 - v10 + a);

	auto da = grad2D(h00);
	auto g10 = grad2D(h10);
	auto g01 = grad2D(h01);
	auto g11 = grad2D(h11);

	auto db = (g10 - da);
	auto dc = (g01 - da);
	auto dd = (g11 - g01 - g10 + da);

	//float value = a + b * tx + (c + d * tx) * ty;

	auto gradient = da + db * tx + (dc + dd * tx) * ty;
	gradient.x += (b + d * ty) * dtx;
	gradient.y += (c + d * tx) * dty;
	gradient.z = 0;

	return gradient;
}


Vector3f PerlinNoise::NoiseGradient3D(float domainScale, float x, float y, float z) const
{
	if (domainScale)
	{
		float s = (1 / domainScale);
		x *= s;
		y *= s;
		z *= s;
	}

	int xi = floor(x);
	int yi = floor(y);
	int zi = floor(z);

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

	float a = dot3D(h000, xf0, yf0, zf0);
	float v100 = dot3D(h100, xf1, yf0, zf0);
	float v010 = dot3D(h010, xf0, yf1, zf0);
	float v001 = dot3D(h001, xf0, yf0, zf1);
	float v110 = dot3D(h110, xf1, yf1, zf0);
	float v101 = dot3D(h101, xf1, yf0, zf1);
	float v011 = dot3D(h011, xf0, yf1, zf1);
	float v111 = dot3D(h111, xf1, yf1, zf1);

	float b = (v100 - a);
	float c = (v010 - a);
	float d = (v001 - a);
	float e = (v110 - v010 - v100 + a);
	float f = (v101 - v001 - v100 + a);
	float g = (v011 - v001 - v010 + a);
	float h = (v111 - v011 - v101 - v110 + v100 + v010 + v001 - a);

	auto da = grad3D(h000);
	auto g100 = grad3D(h100);
	auto g010 = grad3D(h010);
	auto g001 = grad3D(h001);
	auto g110 = grad3D(h110);
	auto g101 = grad3D(h101);
	auto g011 = grad3D(h011);
	auto g111 = grad3D(h111);

	auto db = g100 - da;
	auto dc = g010 - da;
	auto dd = g001 - da;
	auto de = g110 - g010 - g100 + da;
	auto df = g101 - g001 - g100 + da;
	auto dg = g011 - g001 - g010 + da;
	auto dh = g111 - g011 - g101 - g110 + g100 + g010 + g001 - da;

	//float value = a + b * tx + (c + e * tx) * ty + (d + f * tx + (g + h * tx) * ty) * tz; 

	auto gradient = da + db * tx + (dc + de * tx) * ty + (dd + df * tx + (dg + dh * tx) * ty) * tz;
	gradient.x += (b + e * ty + (f + h * ty) * tz) * dtx;
	gradient.y += (c + e * tx + (g + h * tx) * tz) * dty;
	gradient.z += (d + f * tx + (g + h * tx) * ty) * dtz;

	return gradient;
}


Vector3f PerlinNoise::CurlNoise2D(float domainScale, float x, float y) const
{
	auto gradient = NoiseGradient2D(domainScale, x, y);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f PerlinNoise::CurlNoise2DTime(float domainScale, float time, float x, float y) const
{
	auto gradient = NoiseGradient3D(domainScale, x, y, time);

	return Vector3f(gradient.y, -gradient.x, 0);
}


Vector3f PerlinNoise::CurlNoise3D(float domainScale, float x, float y, float z) const
{
	// get noise gradients at different offsets
	auto gx = NoiseGradient3D(domainScale, x, y, z);
	auto gy = NoiseGradient3D(domainScale, x - 123.4567f, y + 56.7891f, z - 345.6789f);
	auto gz = NoiseGradient3D(domainScale, x + 56.7891f, y - 123.4567f, z + 345.6789f);

	return ComputeCurl3D(gx, gy, gz);
}
