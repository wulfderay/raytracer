#include "renderer.h"

#ifdef linux
#include <limits>
#define FLT_MAX std::numeric_limits<float>::max()
#endif

vec3 color(const ray& r, const hitable *world, int max_depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (max_depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation*color(scattered, world, max_depth + 1);
		}
		return emitted;
	}
	return vec3(0, 0, 0);
}
