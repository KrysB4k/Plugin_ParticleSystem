#include "vector3f.h"

Vector3f Vector3f::operator- () const
{
	return Vector3f(-x, -y, -z);
}

Vector3f Vector3f::operator+ (const Vector3f& v) const
{
	return Vector3f(x + v.x, y + v.y, z + v.z);
}

Vector3f& Vector3f::operator+= (const Vector3f& v)
{
	x += v.x, y += v.y, z += v.z;
	return *this;
}

Vector3f Vector3f::operator- (const Vector3f& v) const
{
	return Vector3f(x - v.x, y - v.y, z - v.z);
}

Vector3f& Vector3f::operator-= (const Vector3f& v)
{
	x -= v.x, y -= v.y, z -= v.z;
	return *this;
}

Vector3f Vector3f::operator* (float a) const
{
	return Vector3f(x*a, y*a, z*a);
}

Vector3f& Vector3f::operator*= (float a)
{
	x *= a, y *= a, z *= a;
	return *this;
}

float Vector3f::magnitude() const
{
	return sqrt(x*x + y*y + z*z);
}

float Vector3f::dot(const Vector3f& v) const
{
	return (x*v.x + y*v.y + z*v.z);
}

Vector3f Vector3f::normalized() const
{
	float norm = 1.0f;
	if (x || y || z)
		norm /= magnitude();
	return (*this) * norm;
}

Vector3f Vector3f::cross(const Vector3f& v) const
{
	return Vector3f(
		y*v.z - z*v.y,
		z*v.x - x*v.z,
		x*v.y - y*v.x
		);
}

Vector3f Vector3f::lerp(const Vector3f& v, float t) const
{
	if (t <= 0.0f)
		return (*this);
	else if (t >= 1.0f)
		return v;

	return Vector3f(
		x + (v.x - x) * t,
		y + (v.y - y) * t,
		z + (v.z - z) * t
		);
}

Vector3f Vector3f::slerp(const Vector3f& v, float t) const
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