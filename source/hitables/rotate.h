//
// Created by mark on 10/20/26.
//
#pragma once
#include <cfloat>

#include "hitable.h"


class rotate_y: public hitable
{
    public:
    rotate_y(hitable * p, float angle);
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const
    {
        box = bbox; return hasbox;
    };
    hitable * ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};



