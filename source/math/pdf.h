//
// Created by mark on 10/24/26.
//

#pragma once
#include "Random.h"
#include "../engine/vec3.h"

/**
 * Probability distribution function
 *
 * Naive random scattering of rays is inefficient because there are many cases where a random ray
 * will not contribute meaningfully to a pixel's final colour. One observation that can help
 * is that certain directions relative to the incoming ray will contribute more.
 * For example, glancing hits on a lambertian surface contribute much less than rays that are
 * normal to the incoming ray.
 * We can make the raytracer more efficient and less noisy by biasing the direction of the bounced rays in favour of higher
 * rays, and then accounting for that bias with a weight for that ray.
 *
 * PDFs are a way to model the distribution of directions according to the weight of their
 * contribution to the final value. Of course, they are useful for much more than that outside
 * of this domain.
 *
 * However, exactly what distribution should be used depends upon the material being modeled.
 * Thus we have a PDF interface that can be implemented as several classes of PDF
 * such that they can be used by each material/the colour function
 *
 */
class pdf {
public:
    virtual ~pdf() = default;

    virtual float value(const vec3& direction) const = 0;
    virtual vec3 generate(Random& random) const = 0;
};
