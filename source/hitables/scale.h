#pragma once

#include "hitable.h"

class scale : public hitable
{
public:
    // Uniform scaling about the origin.
    // The factor must be positive and non-zero.
    scale(hitable* p, float factor);

    bool hit(
        const ray& r,
        float t_min,
        float t_max,
        hit_record& rec
    ) const override;

    bool bounding_box(
        float t0,
        float t1,
        aabb& box
    ) const override;
    static float scale_to_largest_axis(const hitable& object, float target_size, float time0, float time1);

private:
    hitable* ptr;
    float factor;
};