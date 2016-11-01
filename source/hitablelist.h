#pragma once
#include <list>
#include <iterator>
#include "hitable.h"

class hitable_list : public hitable {
public:
	hitable_list() {}
	hitable_list(std::list<hitable*> l) { theList = l; }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	std::list<hitable*> theList;
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
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