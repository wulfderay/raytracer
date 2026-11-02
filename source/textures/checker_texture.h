
#pragma once

#include <cmath>
#include "texture.h"

class checker_texture : public texture
{
public:
	checker_texture(texture* t0, texture* t1)
		: even(t0), odd(t1) {}

	vec3 value(float u, float v, const vec3& p) override
	{

		const float offset = 0.01f; // this is to prevent missing checker texture if the x,y or z component happens to be at 0.
		const float scale = 0.05f;
		const float sines =
			std::sin(scale * (p.x() + offset)) *
			std::sin(scale * (p.y() + offset)) *
			std::sin(scale * (p.z() + offset));

		if (sines < 0.0f)
			return odd->value(u, v, p);

		return even->value(u, v, p);
	}

private:
	texture* even;
	texture* odd;
};
