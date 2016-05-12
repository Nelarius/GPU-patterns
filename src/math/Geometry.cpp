#define _USE_MATH_DEFINES
#include "Geometry.h"
#include "Quaternion.h"
#include <cmath>

namespace narwhal {

Mat4f RenderVolume::orthoAlongZ() const {
    Vec3f pos = aabb_.center();
    pos.z = aabb_.max.z;
    Vec3f sizes = aabb_.sizes();
    float width = sizes.x;
    float height = sizes.y;
    float depth = sizes.z;
    Mat4f O = Mat4f::orthographic(width, height, 0.f, depth);
    Mat4f V = Mat4f::translation(pos).inverse();
    return O*V;
}

Mat4f RenderVolume::orthoAlongY() const {
    Vec3f pos = aabb_.center();
    pos.y = aabb_.max.y;
    Vec3f sizes = aabb_.sizes();
    float width = sizes.x;
    float height = sizes.z;
    float depth = sizes.y;
    Mat4f O = Mat4f::orthographic(width, height, 0.f, depth);
    Quatf rotToY{ std::sin(-float(M_PI_4))*Vec3f {1.f, 0.f, 0.f}, std::cos(-float(M_PI_4))};
    Mat4f V = Mat4f::translation(pos) * Mat4f::rotation(rotToY);
    return O * V.inverse();
}

Mat4f RenderVolume::orthoAlongX() const {
    Vec3f pos = aabb_.center();
    pos.x = aabb_.max.x;
    Vec3f sizes = aabb_.sizes();
    float width = sizes.z;
    float height = sizes.y;
    float depth = sizes.x;
    Mat4f O = Mat4f::orthographic(width, height, 0.f, depth);
    Quatf rotToX{ std::sin(float(M_PI_4))*Vec3f {0.f, 1.f, 0.f}, std::cos(float(M_PI_4)) };
    Mat4f V = Mat4f::translation(pos) * Mat4f::rotation(rotToX);
    return O * V.inverse();
}

}
