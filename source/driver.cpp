
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "hitables/hitable.h"
#include "hitables/sphere.h"
#include "hitables/hitablelist.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "hitables/transformation.h"
#include "textures/constant_texture.h"
#include "materials/light.h"
#include "hitables/box.h"
#include "engine/camera.h"
#include "engine/renderer.h"
#include "hitables/bvh.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "standard_scenes.h"

using namespace std::chrono;

RENDERCONTEXT getOptionsFromInput(int argc, char** argv);


int main(int argc, char** argv)
{
    RENDERCONTEXT rc = getOptionsFromInput(argc, argv);

    // make a mersenne twister based rng with a seed from some hardware random source
    Random rng = Random(std::random_device{}());
    // Construct the scene.
    auto objects = standard_scenes::cornel_box();

    rc.world = new bvh_node(
        objects.data(),
        static_cast<int>(objects.size()),
        0.0f,
        0.0f);

    // Configure the camera using the selected render dimensions.
    rc.cam = new camera(
        vec3(278, 278, -800),
        vec3(278, 278, 0),
        vec3(0, 1, 0),
        40.0f,
        static_cast<float>(rc.buffer_width) / rc.buffer_height,
        0.0f,
        10.0f);

    std::cout << "Starting render: "
              << rc.buffer_width << "x" << rc.buffer_height
              << ", " << rc.samples << " samples\n";

    const auto start = steady_clock::now();

    for (int j = 0; j < rc.buffer_height; ++j)
    {
        for (int i = 0; i < rc.buffer_width; ++i)
        {
            vec3 color_sum(0, 0, 0);

            for (int s = 0; s < rc.samples; ++s)
            {
                const float u =
                    (i + static_cast<float>(rand()) / RAND_MAX)
                    / rc.buffer_width;

                const float v =
                    (j + static_cast<float>(rand()) / RAND_MAX)
                    / rc.buffer_height;

                color_sum += color(rc.cam->get_ray(u, v, rng), rc.world, 0, rng);
            }

            rc.buffer[j * rc.buffer_width + i] =
                color_sum / static_cast<float>(rc.samples);
        }
    }

    // Convert linear RGB values to 8-bit sRGB-like output,
    // applying the same square-root gamma correction as before.
    std::vector<std::uint8_t> image(
        static_cast<std::size_t>(rc.buffer_width) *
        rc.buffer_height * 3);

    for (int j = 0; j < rc.buffer_height; ++j)
    {
        for (int i = 0; i < rc.buffer_width; ++i)
        {
            vec3 pixel =
                rc.buffer[j * rc.buffer_width + i];

            for (int k = 0; k < 3; ++k)
            {
                const float linear = std::max(0.0f, pixel[k]);
                const float gamma_corrected = std::sqrt(linear);

                const int value = static_cast<int>(
                    255.99f * std::min(1.0f, gamma_corrected));

                const std::size_t index =
                    (static_cast<std::size_t>(
                        (rc.buffer_height - j - 1) * rc.buffer_width + i)
                    * 3) + k;

                image[index] = static_cast<std::uint8_t>(value);
            }
        }
    }

    const auto end = steady_clock::now();
    const auto total_ms =
        duration_cast<milliseconds>(end - start).count();

    std::cout << "Render duration: "
              << total_ms / 1000 << "."
              << total_ms % 1000 << "s\n";

    if (!stbi_write_png(
            rc.filename,
            rc.buffer_width,
            rc.buffer_height,
            3,
            image.data(),
            rc.buffer_width * 3))
    {
        std::cerr << "Failed to write image: " << rc.filename << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << rc.filename << '\n';

    return EXIT_SUCCESS;
}

void printUsageAndExit(const char * progname)
{
    // Unknown parameter. print usage and exit.
    std::cout << "Usage: " << progname << " [-h height] [-w width] [-s number_multisamples] [-f output_filename]" << std::endl;
    exit(EXIT_FAILURE);
}


RENDERCONTEXT getOptionsFromInput(int argc, char** argv)
{

    RENDERCONTEXT rc;
    rc.buffer_height = 200;
    rc.buffer_width = 400;
    rc.samples = 100;
    rc.filename ="out.png";

    for (auto i = 1; i <argc; i ++)
    {
        if (i + 1 >= argc)
            printUsageAndExit(argv[0]);
        if (!strcmp(argv[i], "-h")) // height
        {
            rc.buffer_height = std::stoi(argv[i+1]);
            ++i;
            continue;
        }
        if (!strcmp(argv[i], "-w"))
        {
            rc.buffer_width = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }
        if (!strcmp(argv[i], "-s"))
        {
            rc.samples = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }
        if (!strcmp(argv[i], "-f"))
        {
            rc.filename = argv[i + 1];
            ++i;
            continue;
        }
        if (!strcmp(argv[i], "-c"))
        {
            rc.cores = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }
        printUsageAndExit(argv[0]);
    }
    rc.buffer = new vec3[rc.buffer_height*rc.buffer_width];
    return rc;
}
