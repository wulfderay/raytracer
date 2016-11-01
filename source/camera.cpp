#include "camera.h"

vec3 random_in_unit_disc() {
	vec3 p;
	do {
		float x = float(rand()) / RAND_MAX;
		float y = float(rand()) / RAND_MAX;
		p = 2.0 *vec3(x, y, 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}