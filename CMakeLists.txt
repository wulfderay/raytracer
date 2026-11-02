cmake_minimum_required(VERSION 4.3)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)

include_directories(source)

set(RAYTRACER_COMMON
        source/aabb.h
        source/hitables/box.cpp
        source/hitables/box.h
        source/hitables/bvh.cpp
        source/hitables/bvh.h
        source/engine/camera.cpp
        source/engine/camera.h
        source/textures/checker_texture.h
        source/textures/constant_texture.h
        source/materials/dielectric.h
        source/hitables/hitable.h
        source/hitables/hitablelist.h
        source/materials/lambertian.h
        source/materials/light.h
        source/materials/material.cpp
        source/materials/material.h
        source/materials/metal.h
        source/engine/ray.h

        source/engine/renderer.cpp
        source/engine/renderer.h
        source/hitables/sphere.h
        source/stb_image_write.h
        #source/stdafx.cpp
        #source/stdafx.h
        #source/targetver.h
        source/textures/texture.h
        source/threads.cpp
        source/threads.h
        source/hitables/transformation.cpp
        source/hitables/transformation.h
        source/engine/vec3.h
        source/math/Random.cpp
        source/math/Random.h
        source/hitables/translate.cpp
        source/hitables/translate.h
        source/hitables/rotate.cpp
        source/hitables/rotate.h
        source/standard_scenes.cpp
        source/standard_scenes.h
        source/math/pdf.h
        source/hitables/mesh.cpp
        source/hitables/mesh.h
        source/rapidobj/rapidobj.hpp
        source/hitables/scale.cpp
        source/hitables/scale.h
)

add_executable(raytracer
        ${RAYTRACER_COMMON}
        source/driver.cpp
        source/math/cosine_pdf.h
)

add_executable(raytracer_threaded
        ${RAYTRACER_COMMON}
        source/raytracer.cpp
)
