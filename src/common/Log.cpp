#include "Log.h"

namespace narwhal {

std::ostream& operator<<(std::ostream& os, const Vec2f& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vec3f& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vec4f& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Quatf& q) {
    os << "[(" << q.v.x << ", " << q.v.y << ", " << q.v.z << "), " << q.w << "]";
    return os;
}

}
