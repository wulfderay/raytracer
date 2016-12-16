#pragma once
#include "hitable.h"

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
