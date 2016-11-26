#pragma once
#include <list>
#include <iterator>
#include "hitable.h"

class hitable_list : public hitable {
public:
	hitable_list() {}
	hitable_list(std::list<hitable*> l) { theList = l; }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	bool bounding_box(float t0, float t1, aabb& box) const override;
	std::list<hitable*> theList;
};

inline bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (auto it = theList.begin(); it != theList.end(); it++) {
		if ((*it)->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

inline bool hitable_list::bounding_box(float t0, float t1, aabb& box) const
{
	return false; // I hope this is right?  if not I have to compute the min and max bounding boxes for the whole thing :/
}