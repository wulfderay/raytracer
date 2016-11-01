#include "material.h"

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		float x = float(rand()) / RAND_MAX;
		float y = float(rand()) / RAND_MAX;
		float z = float(rand()) / RAND_MAX;
		p = 2.0*vec3(x, y, z) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;

}
float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

vec3 reflect(const vec3 & v, const vec3& n) // tbh, this probably belongs as a member function of the vector class.. 
{
	return v - 2 * dot(v, n)*n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt*ni_over_nt* (1 - dt*dt);
	if (discriminant > 0) {
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	return false;
}