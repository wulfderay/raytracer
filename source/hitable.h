#pragma once

#include "ray.h"
#include "material.h"
#include "aabb.h"

class material;

struct hit_record {
	float t;
	float u;
	float v;
	vec3 p;
	vec3 normal;
	material * mat_ptr;
};



class hitable
{
public:
	virtual ~hitable()
	{
	}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};


