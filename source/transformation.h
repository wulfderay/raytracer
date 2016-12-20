#pragma once

#include "hitable.h"

class flip_normals : public hitable {
public: 
	flip_normals() {};
	flip_normals(hitable* _target) : target(_target) {}; 
	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

	bool bounding_box(float t0, float t1, aabb& box) const override;
	hitable * target;
};