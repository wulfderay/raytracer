#pragma once
#include "texture.h"

class constant_texture : public texture
{
public:
	constant_texture();
	constant_texture(vec3 c) : color(c) {};

	vec3 value(float u, float v, const vec3& p) override
	{
		return color;
	}
	vec3 color;
};
