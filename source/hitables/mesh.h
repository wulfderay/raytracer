//
// Created by mark on 10/26/26.
//
#pragma once
#include <memory>

#include "bvh.h"
#include "hitable.h"


/**
 * a mesh of triangles representing an object.
 *
 * Note that unlike other axis-aligned primitives we have made, the triangles in this mesh
 * will have their coordinates relative to object space. This is because I think that enclosing each triangle in
 * a translate() and rotate() hitable would be colossally inefficient.
 *
 * Hmm.. the way we have it right now, we would basically need to implement a bvh or some such and populate it
 * in the constructor, such that the hit() and bounding_box() code would work as expected.
 * I need to check how bounding_box() is used, because for a large, weird mesh, it may not be doing much good.
 *
 * The way that Sebastian Lague does it is to use the bvh as the top-level "hitable", though he doesn't call it that,
 * then to construct the bvh from the set of triangles from a mesh and go from there.
 *
 * In this case, the relationship is a bit inverted... we are treating a mesh as the top level hitable that itself
 * owns a bvh that holds all the triangles. The distinction is that his bvh is specific to triangles while mine
 * was implemented to house generic hitables.
 *
 * One issue with the implementation so far is that the vertex normals are not being used for shading, so the image can
 * look flat and polygonal.
 * As a reminder, vertex normals are per-vertex vectors made during the creation of the model that show the way the surface
 * curves on some model. A surface normal on something like a triangle or other face would be the normal to the plane of the
 * vertexes that make up that face. They are used for calculating things like intersections, but because they are only
 * relative to that face, they do not take into account the overall surface of the idealized model.
 *
 * One example of the difference might be a low-poly sphere. Shading using the surface normals would render a low poly sphere as expected.
 * But if you calculate the vertex normals from the ideal sphere, then lerp between them while rendering each face, you will get a sphere
 * that looks appropriately round, even though it's low poly. THe only caveat is that the silhouette of the sphere will
 * still be low-poly.
 *
 * So the upshot of all this is that we should use the vertex normals for shading, which means that we need to
 * store them. After that, we need to change the way the calculation for the hit is done, lerping between the appropriate vertex normals
 * instead of calculating the surface normal.
 *
 */

class mesh;
class triangle;




class mesh : public hitable
{

    public:
        mesh() = delete;
        mesh(const std::vector<vec3>& vertices,const std::vector<vec3>& normals, std::vector<unsigned int>& vertex_indices, std::vector<unsigned int> & normal_indices, material * _mat);

        bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
        bool bounding_box(float t0, float t1, aabb& box) const override;

private:

    void build_bvh(float time0, float time1);

    material* mat;

    std::vector<vec3> vertices;
    std::vector<vec3> vertex_normals;
    std::vector<triangle> triangles;
    std::unique_ptr<bvh_node> the_bvh;
    friend class triangle;

};

class triangle: public hitable
{
    public:
    triangle() = delete;
    /**
     *
     * @param _v0 index into the vertex list for first vert
     * @param _v1 second vert
     * @param _v2 third vert
     * @param _n0 index into the normal list for first vertex normal
     * @param _n1 second normal
     * @param _n2 third normal
     * @param _parent parent mesh that contains the vertices, normals, etc.
     */
    triangle(unsigned int _v0, unsigned int _v1, unsigned int _v2, unsigned int _n0, unsigned int _n1, unsigned int _n2,  mesh * _parent);
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override;
    mesh * parent;
    unsigned int v0, v1, v2;
    unsigned int n0, n1, n2;
    aabb the_bounding_box;
};


