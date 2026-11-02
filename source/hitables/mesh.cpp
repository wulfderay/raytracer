//
// Created by mark on 10/26/26.
//

#include "mesh.h"

#include <memory>

mesh::mesh(const std::vector<vec3>& vertices, const std::vector<vec3>& normals, std::vector<unsigned int>& vertex_indices, std::vector<unsigned int> & normal_indices, material * _mat):
mat(_mat), vertices(vertices), vertex_normals(normals)
{
    // construct the bvh etc.
    if (vertices.empty()) throw std::invalid_argument("Mesh has no vertices");
    if ( vertex_indices.empty()) throw std::runtime_error("No vertex indices provided");
    if ( vertex_indices.size() % 3 != 0 ) throw std::invalid_argument("Vertex Indices should be in groups of 3");
    if ( normal_indices.empty()) throw std::runtime_error("No normal indices provided");
    if ( normal_indices.size() % 3 != 0 ) throw std::invalid_argument("Normal Indices should be in groups of 3");
    if ( normal_indices.size() != vertex_indices.size()) throw std::invalid_argument("Must have the same number of normals and verts."
                                                                                     "");
    for (size_t i = 0; i < vertex_indices.size(); i += 3)
    {
        // I originally had push_back, but the clang_hints suggested I use emplace_back.
        triangles.emplace_back(vertex_indices[i], vertex_indices[i + 1], vertex_indices[i + 2],
            normal_indices[i], normal_indices[i + 1], normal_indices[i + 2],
            this);
    }

    build_bvh(0,0);
}

bool mesh::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    return the_bvh->hit(r, t_min, t_max, rec);
}

bool mesh::bounding_box(float t0, float t1, aabb& box) const
{
    return the_bvh->bounding_box(t0,t1, box);
}


void mesh::build_bvh(float time0, float time1)
{
    if (triangles.empty())
        return;

    std::vector<hitable*> triangle_ptrs;
    triangle_ptrs.reserve(triangles.size());

    for (triangle& tri : triangles)
        triangle_ptrs.push_back(&tri);

    the_bvh = std::make_unique<bvh_node>(
        triangle_ptrs.data(),
        static_cast<int>(triangle_ptrs.size()),
        time0,
        time1
    );
}

triangle::triangle(unsigned int _v0, unsigned int _v1, unsigned int _v2, unsigned int _n0, unsigned int _n1, unsigned int _n2,  mesh * _parent):
    v0(_v0), v1(_v1), v2(_v2), n0(_n0), n1(_n1), n2(_n2), parent(_parent)
{
    if ( parent == nullptr ) throw std::invalid_argument("parent mesh is null");
    if ( parent->vertices.empty() ) throw std::invalid_argument("parent mesh is empty");
    size_t num_vertices = parent->vertices.size();
    if ( num_vertices <= _v0 ) throw std::invalid_argument("v0 index is larger than number of vertices");
    if ( num_vertices <= _v1 ) throw std::invalid_argument("v1 index is larger than number of vertices");
    if ( num_vertices <= _v2 ) throw std::invalid_argument("v2 index is larger than number of vertices");

    float minx = std::numeric_limits<float>::infinity();
    float maxx = -std::numeric_limits<float>::infinity();
    float miny = std::numeric_limits<float>::infinity();
    float maxy = -std::numeric_limits<float>::infinity();
    float minz = std::numeric_limits<float>::infinity();
    float maxz = -std::numeric_limits<float>::infinity();
    //min
    if ( parent->vertices[_v0].x() < minx ) minx = parent->vertices[_v0].x();
    if ( parent->vertices[_v1].x() < minx ) minx = parent->vertices[_v1].x();
    if ( parent->vertices[_v2].x() < minx ) minx = parent->vertices[_v2].x();

    if ( parent->vertices[_v0].y() < miny ) miny = parent->vertices[_v0].y();
    if ( parent->vertices[_v1].y() < miny ) miny = parent->vertices[_v1].y();
    if ( parent->vertices[_v2].y() < miny ) miny = parent->vertices[_v2].y();

    if ( parent->vertices[_v0].z() < minz ) minz = parent->vertices[_v0].z();
    if ( parent->vertices[_v1].z() < minz ) minz = parent->vertices[_v1].z();
    if ( parent->vertices[_v2].z() < minz ) minz = parent->vertices[_v2].z();

    //max
    if ( parent->vertices[_v0].x() > maxx ) maxx = parent->vertices[_v0].x();
    if ( parent->vertices[_v1].x() > maxx ) maxx = parent->vertices[_v1].x();
    if ( parent->vertices[_v2].x() > maxx ) maxx = parent->vertices[_v2].x();

    if ( parent->vertices[_v0].y() > maxy ) maxy = parent->vertices[_v0].y();
    if ( parent->vertices[_v1].y() > maxy ) maxy  = parent->vertices[_v1].y();
    if ( parent->vertices[_v2].y() > maxy ) maxy  = parent->vertices[_v2].y();

    if ( parent->vertices[_v0].z() > maxz ) maxz  = parent->vertices[_v0].z();
    if ( parent->vertices[_v1].z() > maxz ) maxz = parent->vertices[_v1].z();
    if ( parent->vertices[_v2].z() > maxz ) maxz = parent->vertices[_v2].z();

    the_bounding_box = aabb(vec3(minx, miny, minz), vec3(maxx, maxy, maxz));

}



bool triangle::hit(
    const ray& r,
    float t_min,
    float t_max,
    hit_record& rec) const
{
    const float epsilon = 1e-8f;

    vec3 a_ = parent->vertices[v0];
    vec3 b_ = parent->vertices[v1];
    vec3 c_ = parent->vertices[v2];

    vec3 edge1 = b_ - a_;
    vec3 edge2 = c_ - a_;

    // Begin the Möller–Trumbore test.
    vec3 h = cross(r.direction(), edge2);
    float determinant = dot(edge1, h);

    // Ray is parallel to the triangle, or nearly so.
    if (std::fabs(determinant) < epsilon)
        return false;

    float inv_determinant = 1.0f / determinant;

    vec3 s = r.origin() - a_;
    float u = inv_determinant * dot(s, h);

    // Intersection lies outside one edge.
    if (u < 0.0f || u > 1.0f)
        return false;

    vec3 q = cross(s, edge1);
    float v = inv_determinant * dot(r.direction(), q);

    // Intersection lies outside another edge.
    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = inv_determinant * dot(edge2, q);

    // Intersection is outside the allowed ray interval.
    if (t < t_min || t > t_max)
        return false;

    // We have a hit.
    rec.t = t;
    rec.p = r.point_at_parameter(t);

    // we need to use the vertex normals rather than the surface normal.

    vec3 normal_a = parent->vertex_normals[n0];
    vec3 normal_b = parent->vertex_normals[n1];
    vec3 normal_c = parent->vertex_normals[n2];
    // the equation is this:  vex3 outward_normal = w*normal_a + u*normal_b + v*normal_c;
    // in this case, w can be calculated as (1 - u - v).

    float w = (1 - u - v);
    vec3 outward_normal = unit_vector((normal_a * w) + (normal_b * u) + (normal_c * v));

    // then we need to normalize it.
    //outward_normal.make_unit_vector();


    // old way using surface normal:
    //vec3 outward_normal = unit_vector(cross(edge1, edge2));
    // Adapt this to your existing hit_record convention.
    rec.normal = outward_normal;
    rec.mat_ptr = parent->mat;
    return true;
}

bool triangle::bounding_box(float t0, float t1, aabb& box) const
{
    box = the_bounding_box;
    return true;
}


