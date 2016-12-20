#pragma once
#include "hitable.h"
#include <list>

class xy_rect: public hitable
{
public:
	xy_rect();
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _depth, material * _mat) :
		x0(_x0), y0(_y0), x1(_x1), y1(_y1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3(x0, y0, depth - 0.0001), vec3(x1, y1, depth + 0.0001));
		return true;
	}

	float x0, y0, x1, y1, depth;
	material* mat;
};

class xz_rect : public hitable
{
public:
	xz_rect();
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _depth, material * _mat) :
		x0(_x0), z0(_z0), x1(_x1), z1(_z1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3(x0, depth - 0.0001, z0), vec3(x1, depth + 0.0001, z1));
		return true;
	}

	float x0, z0, x1, z1, depth;
	material* mat;
};

class yz_rect : public hitable
{
public:
	yz_rect();
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _depth, material * _mat) :
		y0(_y0), z0(_z0), y1(_y1), z1(_z1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3( depth - 0.0001,y0, z0), vec3(depth + 0.0001, y1, z1));
		return true;
	}

	float y0, z0, y1, z1, depth;
	material* mat;
};

class box : public hitable
{
public:
	box::box(vec3 min, vec3 max, material* mat);
	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....
	bool bounding_box(float t0, float t1, aabb& box) const override;
private:
	std::list<hitable *> sides;
	aabb bounds;
};
