#pragma once

#include <random>
#include <cstdint>

/**
 * rand() can cause thread serialization because it uses shared stated behind a lock.
 * This class exists so that each thread can create an instance of this class and pass it to
 * classes/ functions that need a source of randomness.
 */
class Random {
public:
    explicit Random(std::uint32_t seed)
        : rng_(seed), distribution_(0.0f, 1.0f)
    {
    }

    float uniform()
    {
        return distribution_(rng_);
    }

private:
    std::mt19937 rng_;
    std::uniform_real_distribution<float> distribution_;
};