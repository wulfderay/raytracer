//
// Created by mark on 10/21/26.
//

#include "standard_scenes.h"

#include "hitables/box.h"
#include "hitables/mesh.h"
#include "textures/checker_texture.h"
#include "textures/constant_texture.h"
#include "materials/dielectric.h"
#include "materials/lambertian.h"
#include "materials/light.h"
#include "materials/metal.h"
#include "hitables/rotate.h"
#include "hitables/sphere.h"
#include "hitables/transformation.h"
#include "hitables/translate.h"


std::vector<hitable*> standard_scenes::cornel_box()
{
    std::vector<hitable*> vector;
    texture * white_texture = new constant_texture(vec3(0.73, 0.73, 0.73));
    texture * red_texture = new constant_texture(vec3(0.65, 0.05,0.05));
    texture * green_texture = new constant_texture(vec3(0.12, 0.45, 0.15));
    texture * black_texture = new constant_texture(vec3(0.15, 0.15, 0.15));
    texture * black_white_checker_texture = new checker_texture(white_texture, black_texture);
    material * red = new lambertian(red_texture);
    material * white = new lambertian(white_texture);
    material * green = new lambertian(green_texture);
    material * black = new lambertian(black_texture);
    material * checkerboard = new lambertian(black_white_checker_texture);
    material * light = new diffuse_light(new constant_texture(vec3(10, 10, 10)));
    material * metal_mat = new metal(vec3(1,1,1),0.3);

    material * yellow = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.15)));
    material * blue = new lambertian(new constant_texture(vec3(0.15, 0.15, 0.73)));

    //left wall
    vector.push_back(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
    //right wall
    vector.push_back(new yz_rect(0, 555, 0, 555, 0, red));
    //top light
    vector.push_back(new xz_rect(150, 400, 150, 400, 554, light));
    // ceiling
    vector.push_back(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
    // floor
    vector.push_back(new xz_rect(0, 555, 0, 555, 0, checkerboard));
    // back wall
    vector.push_back(new flip_normals(new xy_rect(0, 555, 0, 555, 555, checkerboard)));

    /*
    // back box
    vector.push_back(new box(vec3(255, 0, 300), vec3(455, 300, 500), yellow));

    // glass box
    vector.push_back(
        new translate(
        new rotate_y(
        new box(vec3(0, 0, 0), vec3(100, 100, 100), new dielectric(1.5)),
        -45),
        vec3(400, 1, 10))
        );
    // front box
    vector.push_back(new translate(
        new rotate_y(
            new box(vec3(100, 0, 20),vec3( 300, 90, 150), white),
            -18),
        vec3(20, 1, 10)));
    vector.push_back(new sphere(vec3(200, 200, 150), 90, new dielectric(1.5)));
    vector.push_back(new sphere(vec3(355, 350, 400), 50, metal_mat));





    // Test mesh: low-poly pyramid with a triangulated base.
    //
    // Base corners form a square; the apex is offset to make the
    // sides visibly asymmetric. All coordinates are in world space.

    std::vector<vec3> pyramid_vertices = {
        vec3(100, 300, 200),    // 0: front-left base
        vec3(200, 300, 200),    // 1: front-right base
        vec3(200, 300, 300),    // 2: back-right base
        vec3(100, 300, 300),    // 3: back-left base
        vec3(150, 400, 250),    // 4: apex, offset from centre
    //    vec3(145, 45, 290)    // 5: extra point on one side
    };

    std::vector<unsigned int> pyramid_indices = {
        // Four side faces
        0, 1, 4,
        1, 2, 4,
        3, 2, 4,
        3, 0, 4,

        // Triangulated base
        0, 2, 1,
        0, 3, 2
    };

    material* pyramid_mat = new lambertian(
        new constant_texture(vec3(0.15, 0.65, 0.65))
    );

    vector.push_back(
    new translate(
        new rotate_y(

        new mesh(pyramid_vertices, pyramid_indices, pyramid_mat)
        ,45)
        ,vec3(100, -130, 10))
    );
    */
    return vector;

}
