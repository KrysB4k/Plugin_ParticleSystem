#pragma once

#define _USE_MATH_DEFINES
#include <cmath>


typedef struct
{
	int x, y, z;
}phd_vector;


struct Vector3f
{
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}

	Vector3f(float a, float b, float c) : x(a), y(b), z(c) {}

	Vector3f(const phd_vector &v) : x(float(v.x)), y(float(v.y)), z(float(v.z)) {}

	Vector3f operator- () const
	{
		return Vector3f(-x, -y, -z);
	}

	Vector3f operator+ (const Vector3f &v) const
	{
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f& operator+= (const Vector3f &v)
	{
		x += v.x, y += v.y, z += v.z;
		return *this;
	}

	Vector3f operator- (const Vector3f &v) const
	{
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f& operator-= (const Vector3f &v)
	{
		x -= v.x, y -= v.y, z -= v.z;
		return *this;
	}

	Vector3f operator* (float a) const
	{
		return Vector3f(x*a, y*a, z*a);
	}

	Vector3f& operator*= (float a)
	{
		x *= a, y *= a, z *= a;
		return *this;
	}

	float magnitude() const
	{
		return sqrt(x*x + y*y + z*z);
	}

	float distance(const Vector3f &v) const
	{
		return ((*this) - v).magnitude();
	}

	float dot(const Vector3f &v) const
	{
		return (x*v.x + y*v.y + z*v.z);
	}

	Vector3f normalized() const
	{
		float norm = 1.0f;
		if (x || y || z)
			norm /= magnitude();
		return (*this) * norm;
	}

	Vector3f cross(const Vector3f &v) const
	{
		return Vector3f(
			y*v.z - z*v.y,
			z*v.x - x*v.z,
			x*v.y - y*v.x
			);
	}

	Vector3f slerp(const Vector3f &v, float t) const
	{
		if (t <= 0.0f)
			return (*this);
		else if (t >= 1.0f)
			return v;

		float dotprod = dot(v);
		if (abs(dotprod) >= 1.0f)
			return (*this);

		float theta = acos(dotprod);
		float sth = sin(theta);
		float fa = sin((1-t)*theta)/sth;
		float fb = sin(t*theta)/sth;
	
		return Vector3f(
		x*fa + v.x*fb,
		y*fa + v.y*fb,
		z*fa + v.z*fb
		);
	}

	phd_vector to_phdvect() const
	{
		int a = (x < 0.0f) ? int(x - 0.5f) : int(x + 0.5f);
		int b = (y < 0.0f) ? int(y - 0.5f) : int(y + 0.5f);
		int c = (z < 0.0f) ? int(z - 0.5f) : int(z + 0.5f);
		
		phd_vector v = {a, b, c};
		return v;
	}
};
