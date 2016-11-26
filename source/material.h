#pragma once
#include "ray.h"
#include "hitable.h"
struct hit_record;

vec3 random_in_unit_sphere();
float schlick(float cosine, float ref_idx);
vec3 reflect(const vec3 & v, const vec3& n);
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);

class material {
public:
	virtual ~material()
	{
	}

	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};