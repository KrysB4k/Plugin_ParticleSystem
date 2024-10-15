#pragma once
#include "luabridge.h"

struct Vector3f final : public LuaObjectClassPosition
{
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3f(float a, float b, float c) : x(a), y(b), z(c) {}

	static const char* Name();
	virtual void Index(const char* field) override;
	virtual void NewIndex(const char* field) override;
	virtual float GetX() override;
	virtual float GetY() override;
	virtual float GetZ() override;
	virtual explicit operator Vector3f() override;

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
