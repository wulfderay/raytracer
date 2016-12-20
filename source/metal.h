#pragma once

#include "hitable.h"



class metal : public material {
public:
	metal(const vec3& a, float _fuzziness) :albedo(a) { if (_fuzziness < 1) fuzziness = _fuzziness; else fuzziness = 1; }

	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzziness* random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	vec3 albedo;
	float fuzziness;
};
