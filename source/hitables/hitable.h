#pragma once

#include "../engine/ray.h"
#include "../materials/material.h"
#include "../aabb.h"


/**
 *  base class for hittable objects.
 */


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

	/**
	 * For use with Bounding Volume Hierarchies
	 * @param t0
	 * @param t1
	 * @param box
	 * @return
	 */
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};


