#pragma once

#include <cmath>
#include "pdf.h"
#include "../engine/vec3.h"
#include "Random.h"

class cosine_pdf : public pdf {
public:
    explicit cosine_pdf(const vec3& normal)
        : normal_(unit_vector(normal))
    {
        // Build an orthonormal basis around the surface normal.
        //
        // w_ points along the normal.
        // u_ and v_ are perpendicular to w_ and to each other.

        vec3 a = (std::fabs(normal_.x()) > 0.9f)
            ? vec3(0.0f, 1.0f, 0.0f)
            : vec3(1.0f, 0.0f, 0.0f);

        v_ = unit_vector(cross(normal_, a));
        u_ = cross(v_, normal_);
    }

    float value(const vec3& direction) const override
    {
        // The PDF is defined for unit directions.
        vec3 unit_direction = unit_vector(direction);

        float cosine = dot(normal_, unit_direction);

        if (cosine <= 0.0f)
            return 0.0f;

        return cosine / static_cast<float>(M_PI);
    }

    vec3 generate(Random& random) const override
    {
        // Generate a cosine-weighted direction in local space.

        float r1 = random.uniform(); // Must return a value in [0, 1)
        float r2 = random.uniform();

        float phi = 2.0f * static_cast<float>(M_PI) * r1;

        float radius = std::sqrt(r2);
        float z = std::sqrt(1.0f - r2);

        float x = radius * std::cos(phi);
        float y = radius * std::sin(phi);

        // Transform local coordinates into world coordinates.
        return x * u_ + y * v_ + z * normal_;
    }

private:
    vec3 normal_;
    vec3 u_;
    vec3 v_;
};
