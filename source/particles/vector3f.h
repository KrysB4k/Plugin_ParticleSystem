#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

struct Vector3f
{
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}

	Vector3f(float a, float b, float c) : x(a), y(b), z(c) {}

	Vector3f operator- () const;

	Vector3f operator+ (const Vector3f& v) const;

	Vector3f& operator+= (const Vector3f& v);

	Vector3f operator- (const Vector3f& v) const;

	Vector3f& operator-= (const Vector3f& v);

	Vector3f operator* (float a) const;

	Vector3f& operator*= (float a);

	float magnitude() const;

	float dot(const Vector3f& v) const;

	Vector3f normalized() const;

	Vector3f cross(const Vector3f& v) const;

	Vector3f lerp(const Vector3f& v, float t) const;

	Vector3f slerp(const Vector3f& v, float t) const;
};
