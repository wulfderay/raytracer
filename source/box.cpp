#include "box.h"
#include "transformation.h"

bool xy_rect::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	auto t = (depth - r.origin().z()) / r.direction().z();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.origin().x() + t*r.direction().x();
	auto y = r.origin().y() + t*r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	rec.mat_ptr = mat;
	rec.p = r.point_at_parameter(t);
	rec.normal = vec3(0, 0, 1);
	return true;

}

bool xz_rect::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	auto t = (depth - r.origin().y()) / r.direction().y();
	if (t < t_min || t > t_max)
		return false;
	auto x = r.origin().x() + t*r.direction().x();
	auto z = r.origin().z() + t*r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mat;
	rec.p = r.point_at_parameter(t);
	rec.normal = vec3(0, 1, 0);
	return true;

}

bool yz_rect::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	auto t = (depth - r.origin().x()) / r.direction().x();
	if (t < t_min || t > t_max)
		return false;
	auto y = r.origin().y() + t*r.direction().y();
	auto z = r.origin().z() + t*r.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mat;
	rec.p = r.point_at_parameter(t);
	rec.normal = vec3(1, 0, 0);
	return true;

}

box::box( vec3 min, vec3 max, material* mat)
{
	sides.push_back(new xy_rect(min.x(), max.x(), min.y(), max.y(), min.z(), mat));
	sides.push_back(new xy_rect(min.x(), max.x(), min.y(), max.y(), max.z(), mat));
	sides.push_back(new xz_rect(min.x(), max.x(), min.z(), max.z(), min.y(), mat));
	sides.push_back(new xz_rect(min.x(), max.x(), min.z(), max.z(), max.y(), mat));
	sides.push_back(new yz_rect(min.y(), max.y(), min.z(), max.z(), min.x(), mat));
	sides.push_back(new yz_rect(min.y(), max.y(), min.z(), max.z(), max.x(), mat));
	bounds = aabb(min, max);
}

bool box::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	hit_record closest_hit;
	auto has_hit = false;
	for (auto side : sides)
	{
		if (side->hit(r, t_min, t_max, closest_hit))  // make sure we return the closest hit.
		{
			if (!has_hit || closest_hit.t < rec.t)
			{
				rec = closest_hit;
				has_hit = true;
			}
		}
	}
	return has_hit;
}

bool box::bounding_box(float t0, float t1, aabb& box) const
{
	box = bounds;
	return true;
}
