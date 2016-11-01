#include "renderer.h"

vec3 color(const ray& r, const hitable *world, int max_depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (max_depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation*color(scattered, world, max_depth + 1);
		}
		return vec3(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5*(unit_direction.y() + 1.0);
	return (1.0f - t)* vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}