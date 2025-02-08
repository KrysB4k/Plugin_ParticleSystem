#pragma once
#include "luabridge.h"

struct Vector3f final : public LuaObjectClassPosition
{
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3f(float a, float b, float c) : x(a), y(b), z(c) {}

	static const char* Name();

	float GetX() final;
	float GetY() final;
	float GetZ() final;
	void SetX(float x) final;
	void SetY(float y) final;
	void SetZ(float z) final;
	explicit operator Vector3f() final;

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
