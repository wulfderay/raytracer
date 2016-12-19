#include "transformation.h"

bool flip_normals::hit(const ray & r, float t_min, float t_max, hit_record & rec) const
{
	auto result = target->hit(r, t_min, t_max, rec);
	if (result)
		rec.normal = -rec.normal;
	return result;
}

bool flip_normals::bounding_box(float t0, float t1, aabb & box) const
{
	return target->bounding_box(t0, t1, box);
}
