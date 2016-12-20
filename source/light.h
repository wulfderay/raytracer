#pragma once
#include "material.h"
#include "texture.h"

class diffuse_light :public material{
public:
	diffuse_light(texture *light_texture) :emit(light_texture) {}
	bool scatter(const ray& r, const hit_record& rec, vec3& attenuation, ray& scattered) const override { return false; }
	vec3 emitted(float u, float v, const vec3& p) const override {
		return emit->value(u, v, p);
	}
	texture * emit;
};
