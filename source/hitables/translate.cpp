//
// Created by mark on 10/20/26.
//

#include "translate.h"

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    ray moved_ray(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(moved_ray, t_min, t_max, rec))
    {
        //since translation is effectively shifting the rays, not the object, we must account
        // for that in the hit record.
        rec.p += offset;
        return true;
    }
    return false;

}

bool translate::bounding_box(float t0, float t1, aabb& box) const
{
    if (ptr->bounding_box(t0, t1, box))
    {
        // just shift the object's bounding box over by the offset.
        box = aabb(box.min() + offset, box.max() + offset);
        return true;
    }
    return false;
}
