#pragma once
#include "aabb.h"
#include "hitable.h"



class bvh_node : public hitable {
public:
	bvh_node() {};
	bvh_node(hitable **l, int n, float time0, float time1);
	bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;
	bool bounding_box(float t0, float t1, aabb& box) const override;
	hitable * left; 
	hitable * right;
	aabb box;
};
