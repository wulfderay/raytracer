#pragma once
#include "hitable.h"
#include <list>


/**
 *  A 0 width rectangle in the xy plane.
 *
 */
class xy_rect: public hitable
{
public:
	xy_rect() = delete; // not valid to instantiate without params.

	/**
	 *
	 * @param _x0 min x coord
	 * @param _x1 max x coord
	 * @param _y0 min y coord
	 * @param _y1 max y coord
	 * @param _depth z coordinate
	 * @param _mat material properties
	 */
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _depth, material * _mat) :
		x0(_x0), y0(_y0), x1(_x1), y1(_y1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3(x0, y0, depth - 0.0001), vec3(x1, y1, depth + 0.0001));
		return true;
	}

	float x0, y0, x1, y1, depth;
	material* mat;
};

/**
 * a 0 width rectangle in the xz plane
 */
class xz_rect : public hitable
{
public:
	xz_rect() = delete; // not valid to instantiate without params.

	/**
	 *
	 * @param _x0  min x coord
	 * @param _x1 max x coord
	 * @param _z0 min z coord
	 * @param _z1 max z coord
	 * @param _depth y coordinate
	 * @param _mat material properties
	 */
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _depth, material * _mat) :
		x0(_x0), z0(_z0), x1(_x1), z1(_z1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3(x0, depth - 0.0001, z0), vec3(x1, depth + 0.0001, z1));
		return true;
	}

	float x0, z0, x1, z1, depth;
	material* mat;
};

/**
 * A 0 width rectangle in the yz plane
 */
class yz_rect : public hitable
{
public:
	yz_rect() = delete; // not valid to instantiate without params.

	/**
	 *
	 * @param _y0 min y coord
	 * @param _y1 max y coord
	 * @param _z0 min z coord
	 * @param _z1 max z coord
	 * @param _depth x coord
	 * @param _mat material properties
	 */
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _depth, material * _mat) :
		y0(_y0), z0(_z0), y1(_y1), z1(_z1), depth(_depth), mat(_mat) {};

	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....

	bool bounding_box(float t0, float t1, aabb& box) const override
	{
		box = aabb(vec3( depth - 0.0001,y0, z0), vec3(depth + 0.0001, y1, z1));
		return true;
	}

	float y0, z0, y1, z1, depth;
	material* mat;
};

/**
 * A 6 sided box made of various rects
 * Note that this is always axis aligned. Instancing can be used for rotation etc.
 */
class box : public hitable
{
public:
	box() = delete; // not valid to instantiate without params.
	/**
	 *
	 * @param min bottom left corner
	 * @param max top right corner
	 * @param mat material properties
	 */
	box(vec3 min, vec3 max, material* mat);
	bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override; // hmm.. not hitting it....
	bool bounding_box(float t0, float t1, aabb& box) const override;
private:
	std::list<hitable *> sides;
	aabb bounds;
};
