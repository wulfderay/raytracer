#pragma once
#include "../engine/vec3.h"

class texture
{
public:
	virtual ~texture() = default;
	virtual vec3 value(float u, float v, const vec3& p) = 0;
};
