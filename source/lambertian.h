#pragma once
#include "hitable.h"
#include "texture.h"

class lambertian : public material {
public:
	lambertian(texture *a) :albedo(a) {}

	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo->value(0,0,rec.p);
		return true;
	}

	texture *albedo;
};
