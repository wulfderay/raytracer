#pragma once
#include "../math/cosine_pdf.h"
#include "../hitables/hitable.h"
#include "../textures/texture.h"

class lambertian : public material {
public:
	lambertian(texture *a) :albedo(a) {}

	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, Random& random) const override
	{
		cosine_pdf pdf = cosine_pdf(rec.normal);
		vec3 direction = pdf.generate(random);
		//float density = pdf.value(direction); // we don't need this because the
		//lambertian pdf and cosine pdf cancel out. This is a special case.

		scattered = ray(rec.p, direction);
		attenuation = albedo->value(0,0,rec.p);
		return true;
	}

	texture *albedo;
};
