#pragma once

#include "Matrix.h"
#include "Vector.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace narwhal {

template<typename T>
struct Ray {
    Vector3<T> origin;
    Vector3<T> direction;
    T t;
};

template<typename T>
struct Line {
    Vector3<T> origin;
    Vector3<T> end;
};

template<typename T>
struct Frustum {
    Frustum(T fov, T aspectRatio, T n, T f)
        : width{ aspectRatio * T(2.0) * n * tan(fov / T(2.0)) },
        height{ T(2.0) * n * tan(fov / T(2.0)) },
        near{ n },
        far{ f } {}
    T width{}, height{};
    T near{}, far{};
};

template<typename T>
Ray<T> generateCameraRay(const Vector3<T>& eye, const Quaternion<T>& orientation, const Frustum<T>& frustum, T x, T y) {
    Vector3<T> eyeToPixel{
        x * T(0.5) * frustum.width,
        y * T(0.5) * frustum.height,
        -frustum.near
    };   // untranslated pixel coordinates
    Vector4<T> temp = Matrix4<T>::rotation(orientation) * Vector4<T>(eyeToPixel, T(1.0));
    eyeToPixel = Vector3<T>{ temp.x, temp.y, temp.z };
    eyeToPixel.normalize();
    return Ray<T>{ eye, eyeToPixel, std::numeric_limits<T>::max() };
}

/*
 * The plane normal is calculated from the input points in a counter clockwise sense,
 * according to the right hand rule.
 **/
template<typename T>
class Plane {
public:
    Plane() = delete;
    Plane(const Vector3<T>& p0, const Vector3<T>& p1, const Vector3<T>& p2)
        : edge1_{ p1 - p0 },
        edge2_{ p2 - p0 },
        point_{ p0 },
        normal_{ edge1_.cross(edge2_).normalized() } {}
    ~Plane() = default;

    const Vector3<T> point() const {
        return point_;
    }

    const Vector3<T> normal() const {
        return normal_;
    }

private:
    Vector3<T> edge1_;
    Vector3<T> edge2_;
    Vector3<T> point_;
    Vector3<T> normal_;
};

template<typename T>
struct AABox {
    Vector3<T> min;
    Vector3<T> max;

    Vector3<T> center() const {
        return T(0.5) * (min + max);
    }

    Vector3<T> sizes() const {
        return Vector3<T>{ max.x - min.x, max.y - min.y, max.z - min.z };
    }

    void grow(const AABox<T>& other) {
        min.x = other.min.x < min.x ? other.min.x : min.x;
        min.y = other.min.y < min.y ? other.min.y : min.y;
        min.z = other.min.z < min.z ? other.min.z : min.z;
        max.x = other.max.x > max.x ? other.max.x : max.x;
        max.y = other.max.y > max.y ? other.max.y : max.y;
        max.z = other.max.z > max.z ? other.max.z : max.z;
    }

    void inflate(T val) {
        Vector3<T> inflation{ val, val, val };
        min = min - inflation;
        max = max + inflation;
    }
};

template<typename T>
struct Sphere {
    Vector3<T> center;
    T radius;
};

using Rayf = Ray<float>;
using Linef = Line<float>;
using Frustumf = Frustum<float>;
using Planef = Plane<float>;
using AABoxf = AABox<float>;
using Spheref = Sphere<float>;

// the purpose of this struct is, given a bounding box of any shape, to create a cube that perfectly contains
// the bounding box. The cube's side length is given by the largest extent of the original bounding box.
struct RenderVolume {
public:
    RenderVolume(const AABoxf& aabb)
        : aabb_(aabb) {
        Vec3f extents = aabb_.sizes();
        float largestExtent = std::max(std::max(extents.x, extents.y), extents.z);
        Vec3f center = aabb_.center();
        float offCenter = 0.5f * largestExtent;
        aabb_.min = center - Vec3f{ offCenter, offCenter, offCenter };
        aabb_.max = center + Vec3f{ offCenter, offCenter, offCenter };
    }

    inline const AABoxf& aabb() const { return aabb_; }

    Mat4f orthoAlongZ() const;
    Mat4f orthoAlongY() const;
    Mat4f orthoAlongX() const;

private:
    AABoxf aabb_;
};

}
