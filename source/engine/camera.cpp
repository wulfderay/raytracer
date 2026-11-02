#include "camera.h"

#include "../math/Random.h"

vec3 random_in_unit_disc(Random& random) {
	vec3 p;
	do {
		float x = random.uniform();
		float y = random.uniform();
		p = 2.0 *vec3(x, y, 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}
