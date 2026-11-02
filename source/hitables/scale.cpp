#include "scale.h"

#include <stdexcept>

scale::scale(hitable* p, float factor)
    : ptr(p), factor(factor)
{
    if (ptr == nullptr)
        throw std::invalid_argument("scale requires a valid hittable");

    if (factor <= 0.0f)
        throw std::invalid_argument("scale factor must be positive");
}

bool scale::hit(
    const ray& r,
    float t_min,
    float t_max,
    hit_record& rec
) const
{
    // Transform the ray into the object's original coordinate system.
    ray scaled_ray(
        r.origin() / factor,
        r.direction() / factor,
        r.time()
    );

    if (ptr->hit(scaled_ray, t_min, t_max, rec))
    {
        // Transform the hit point back into world space.
        rec.p *= factor;

        // A positive uniform scale does not change the normal direction.
        // Normalizing is optional if the underlying hitable guarantees
        // that its normals are already normalized.
        rec.normal = unit_vector(rec.normal);

        return true;
    }

    return false;
}

bool scale::bounding_box(
    float t0,
    float t1,
    aabb& box
) const
{
    if (!ptr->bounding_box(t0, t1, box))
        return false;

    // Positive uniform scaling preserves min/max ordering.
    box = aabb(
        box.min() * factor,
        box.max() * factor
    );

    return true;
}

float scale::scale_to_largest_axis(
    const hitable& object,
    float target_size,
    float time0 = 0.0f,
    float time1 = 0.0f)
{
    if (target_size <= 0.0f)
        throw std::invalid_argument("Target size must be positive");

    aabb bounds(vec3(0, 0, 0), vec3(0, 0, 0));

    if (!object.bounding_box(time0, time1, bounds))
        throw std::runtime_error("Object has no bounding box");

    vec3 dimensions = bounds.max() - bounds.min();

    float largest_axis = std::max(
        std::max(
            dimensions.x(),
            dimensions.y()),
        dimensions.z()
    );

    if (largest_axis <= 0.0f)
        throw std::runtime_error("Object has zero size");

    return target_size / largest_axis;
}