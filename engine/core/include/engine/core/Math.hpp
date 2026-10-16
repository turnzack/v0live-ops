#pragma once

#include <cmath>
#include <algorithm>
#include <array>
#include <limits>

namespace engine::core::math {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kHalfPi = 1.57079632679489661923f;
constexpr float kTwoPi = 6.28318530717958647692f;
constexpr float kDegToRad = kPi / 180.0f;
constexpr float kRadToDeg = 180.0f / kPi;
constexpr float kEpsilon = 1e-6f;

[[nodiscard]] constexpr float radians(float degrees) noexcept {
    return degrees * kDegToRad;
}

[[nodiscard]] constexpr float degrees(float radians) noexcept {
    return radians * kRadToDeg;
}

[[nodiscard]] constexpr float clamp(float value, float minVal, float maxVal) noexcept {
    return (value < minVal) ? minVal : ((value > maxVal) ? maxVal : value);
}

[[nodiscard]] constexpr float lerp(float a, float b, float t) noexcept {
    return a + t * (b - a);
}

// ============================================================================
// Vec2
// ============================================================================
struct Vec2 {
    float x{0.0f};
    float y{0.0f};

    constexpr Vec2() noexcept = default;
    constexpr Vec2(float scalar) noexcept : x(scalar), y(scalar) {}
    constexpr Vec2(float inX, float inY) noexcept : x(inX), y(inY) {}

    static const Vec2 Zero;
    static const Vec2 One;
    static const Vec2 UnitX;
    static const Vec2 UnitY;

    constexpr Vec2 operator+(const Vec2& rhs) const noexcept { return {x + rhs.x, y + rhs.y}; }
    constexpr Vec2 operator-(const Vec2& rhs) const noexcept { return {x - rhs.x, y - rhs.y}; }
    constexpr Vec2 operator*(float scalar) const noexcept { return {x * scalar, y * scalar}; }
    constexpr Vec2 operator*(const Vec2& rhs) const noexcept { return {x * rhs.x, y * rhs.y}; }
    constexpr Vec2 operator/(float scalar) const noexcept { return {x / scalar, y / scalar}; }
    constexpr Vec2 operator-() const noexcept { return {-x, -y}; }

    constexpr Vec2& operator+=(const Vec2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vec2& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    constexpr Vec2& operator/=(float scalar) noexcept { x /= scalar; y /= scalar; return *this; }

    constexpr bool operator==(const Vec2& rhs) const noexcept {
        return std::abs(x - rhs.x) <= kEpsilon && std::abs(y - rhs.y) <= kEpsilon;
    }
    constexpr bool operator!=(const Vec2& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] constexpr float lengthSquared() const noexcept { return x * x + y * y; }
    [[nodiscard]] float length() const noexcept { return std::sqrt(lengthSquared()); }
    [[nodiscard]] Vec2 normalized() const noexcept {
        const float len = length();
        return (len > kEpsilon) ? (*this / len) : Vec2::Zero;
    }
};

inline const Vec2 Vec2::Zero{0.0f, 0.0f};
inline const Vec2 Vec2::One{1.0f, 1.0f};
inline const Vec2 Vec2::UnitX{1.0f, 0.0f};
inline const Vec2 Vec2::UnitY{0.0f, 1.0f};

[[nodiscard]] constexpr float dot(const Vec2& a, const Vec2& b) noexcept {
    return a.x * b.x + a.y * b.y;
}

[[nodiscard]] inline float distance(const Vec2& a, const Vec2& b) noexcept {
    return (a - b).length();
}

// ============================================================================
// Vec3
// ============================================================================
struct Vec3 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    constexpr Vec3() noexcept = default;
    constexpr Vec3(float scalar) noexcept : x(scalar), y(scalar), z(scalar) {}
    constexpr Vec3(float inX, float inY, float inZ) noexcept : x(inX), y(inY), z(inZ) {}
    constexpr Vec3(const Vec2& xy, float inZ) noexcept : x(xy.x), y(xy.y), z(inZ) {}

    static const Vec3 Zero;
    static const Vec3 One;
    static const Vec3 UnitX;
    static const Vec3 UnitY;
    static const Vec3 UnitZ;

    constexpr Vec3 operator+(const Vec3& rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    constexpr Vec3 operator-(const Vec3& rhs) const noexcept { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    constexpr Vec3 operator*(float scalar) const noexcept { return {x * scalar, y * scalar, z * scalar}; }
    constexpr Vec3 operator*(const Vec3& rhs) const noexcept { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
    constexpr Vec3 operator/(float scalar) const noexcept { return {x / scalar, y / scalar, z / scalar}; }
    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vec3& operator+=(const Vec3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    constexpr Vec3& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    constexpr Vec3& operator/=(float scalar) noexcept { x /= scalar; y /= scalar; z /= scalar; return *this; }

    constexpr bool operator==(const Vec3& rhs) const noexcept {
        return std::abs(x - rhs.x) <= kEpsilon &&
               std::abs(y - rhs.y) <= kEpsilon &&
               std::abs(z - rhs.z) <= kEpsilon;
    }
    constexpr bool operator!=(const Vec3& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] constexpr float lengthSquared() const noexcept { return x * x + y * y + z * z; }
    [[nodiscard]] float length() const noexcept { return std::sqrt(lengthSquared()); }
    [[nodiscard]] Vec3 normalized() const noexcept {
        const float len = length();
        return (len > kEpsilon) ? (*this / len) : Vec3::Zero;
    }
};

inline const Vec3 Vec3::Zero{0.0f, 0.0f, 0.0f};
inline const Vec3 Vec3::One{1.0f, 1.0f, 1.0f};
inline const Vec3 Vec3::UnitX{1.0f, 0.0f, 0.0f};
inline const Vec3 Vec3::UnitY{0.0f, 1.0f, 0.0f};
inline const Vec3 Vec3::UnitZ{0.0f, 0.0f, 1.0f};

[[nodiscard]] constexpr float dot(const Vec3& a, const Vec3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] constexpr Vec3 cross(const Vec3& a, const Vec3& b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

[[nodiscard]] inline float distance(const Vec3& a, const Vec3& b) noexcept {
    return (a - b).length();
}

// ============================================================================
// Vec4
// ============================================================================
struct Vec4 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{0.0f};

    constexpr Vec4() noexcept = default;
    constexpr Vec4(float scalar) noexcept : x(scalar), y(scalar), z(scalar), w(scalar) {}
    constexpr Vec4(float inX, float inY, float inZ, float inW) noexcept : x(inX), y(inY), z(inZ), w(inW) {}
    constexpr Vec4(const Vec3& xyz, float inW) noexcept : x(xyz.x), y(xyz.y), z(xyz.z), w(inW) {}

    constexpr Vec4 operator+(const Vec4& rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }
    constexpr Vec4 operator-(const Vec4& rhs) const noexcept { return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }
    constexpr Vec4 operator*(float scalar) const noexcept { return {x * scalar, y * scalar, z * scalar, w * scalar}; }

    constexpr bool operator==(const Vec4& rhs) const noexcept {
        return std::abs(x - rhs.x) <= kEpsilon &&
               std::abs(y - rhs.y) <= kEpsilon &&
               std::abs(z - rhs.z) <= kEpsilon &&
               std::abs(w - rhs.w) <= kEpsilon;
    }
};

// ============================================================================
// Mat4 (Row-Major Storage)
// ============================================================================
struct Mat4 {
    std::array<float, 16> m{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    constexpr Mat4() noexcept = default;
    constexpr Mat4(const std::array<float, 16>& elements) noexcept : m(elements) {}

    static const Mat4 Identity;

    [[nodiscard]] static constexpr Mat4 translation(const Vec3& pos) noexcept {
        Mat4 res = Identity;
        res.m[12] = pos.x;
        res.m[13] = pos.y;
        res.m[14] = pos.z;
        return res;
    }

    [[nodiscard]] static constexpr Mat4 scaling(const Vec3& scale) noexcept {
        Mat4 res = Identity;
        res.m[0] = scale.x;
        res.m[5] = scale.y;
        res.m[10] = scale.z;
        return res;
    }

    [[nodiscard]] static Mat4 orthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept {
        Mat4 res = Identity;
        res.m[0] = 2.0f / (right - left);
        res.m[5] = 2.0f / (top - bottom);
        res.m[10] = -1.0f / (zFar - zNear); // WebGPU depth range [0, 1]
        res.m[12] = -(right + left) / (right - left);
        res.m[13] = -(top + bottom) / (top - bottom);
        res.m[14] = -zNear / (zFar - zNear);
        return res;
    }

    [[nodiscard]] Mat4 operator*(const Mat4& rhs) const noexcept {
        Mat4 out{};
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[row * 4 + k] * rhs.m[k * 4 + col];
                }
                out.m[row * 4 + col] = sum;
            }
        }
        return out;
    }

    [[nodiscard]] Vec4 operator*(const Vec4& v) const noexcept {
        return {
            m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w,
            m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w,
            m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w,
            m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w
        };
    }
};

inline const Mat4 Mat4::Identity{};

// ============================================================================
// Quat (Quaternion)
// ============================================================================
struct Quat {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{1.0f};

    constexpr Quat() noexcept = default;
    constexpr Quat(float inX, float inY, float inZ, float inW) noexcept : x(inX), y(inY), z(inZ), w(inW) {}

    static const Quat Identity;

    static Quat fromAxisAngle(const Vec3& axis, float angleRadians) noexcept {
        const float halfAngle = angleRadians * 0.5f;
        const float s = std::sin(halfAngle);
        const Vec3 normAxis = axis.normalized();
        return {normAxis.x * s, normAxis.y * s, normAxis.z * s, std::cos(halfAngle)};
    }
};

inline const Quat Quat::Identity{0.0f, 0.0f, 0.0f, 1.0f};

// ============================================================================
// AABB (Axis-Aligned Bounding Box)
// ============================================================================
struct AABB {
    Vec3 minPoint{0.0f};
    Vec3 maxPoint{0.0f};

    constexpr AABB() noexcept = default;
    constexpr AABB(const Vec3& inMin, const Vec3& inMax) noexcept : minPoint(inMin), maxPoint(inMax) {}

    [[nodiscard]] constexpr bool contains(const Vec3& pt) const noexcept {
        return (pt.x >= minPoint.x && pt.x <= maxPoint.x &&
                pt.y >= minPoint.y && pt.y <= maxPoint.y &&
                pt.z >= minPoint.z && pt.z <= maxPoint.z);
    }

    [[nodiscard]] constexpr bool intersects(const AABB& other) const noexcept {
        return (minPoint.x <= other.maxPoint.x && maxPoint.x >= other.minPoint.x &&
                minPoint.y <= other.maxPoint.y && maxPoint.y >= other.minPoint.y &&
                minPoint.z <= other.maxPoint.z && maxPoint.z >= other.minPoint.z);
    }

    [[nodiscard]] constexpr Vec3 center() const noexcept {
        return (minPoint + maxPoint) * 0.5f;
    }

    [[nodiscard]] constexpr Vec3 size() const noexcept {
        return maxPoint - minPoint;
    }

    void expand(const Vec3& pt) noexcept {
        minPoint.x = std::min(minPoint.x, pt.x);
        minPoint.y = std::min(minPoint.y, pt.y);
        minPoint.z = std::min(minPoint.z, pt.z);
        maxPoint.x = std::max(maxPoint.x, pt.x);
        maxPoint.y = std::max(maxPoint.y, pt.y);
        maxPoint.z = std::max(maxPoint.z, pt.z);
    }
};

// ============================================================================
// Ray
// ============================================================================
struct Ray {
    Vec3 origin{0.0f};
    Vec3 direction{0.0f, 0.0f, 1.0f};

    constexpr Ray() noexcept = default;
    constexpr Ray(const Vec3& orig, const Vec3& dir) noexcept : origin(orig), direction(dir) {}

    [[nodiscard]] bool intersects(const AABB& box, float& outDistance) const noexcept {
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();

        for (int i = 0; i < 3; ++i) {
            const float orig = (i == 0) ? origin.x : ((i == 1) ? origin.y : origin.z);
            const float dir  = (i == 0) ? direction.x : ((i == 1) ? direction.y : direction.z);
            const float bMin = (i == 0) ? box.minPoint.x : ((i == 1) ? box.minPoint.y : box.minPoint.z);
            const float bMax = (i == 0) ? box.maxPoint.x : ((i == 1) ? box.maxPoint.y : box.maxPoint.z);

            if (std::abs(dir) < kEpsilon) {
                if (orig < bMin || orig > bMax) {
                    return false;
                }
            } else {
                const float invD = 1.0f / dir;
                float t1 = (bMin - orig) * invD;
                float t2 = (bMax - orig) * invD;
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                if (tMin > tMax) {
                    return false;
                }
            }
        }
        outDistance = tMin;
        return true;
    }
};

} // namespace engine::core::math

namespace engine::core {
    using math::Vec2;
    using math::Vec3;
    using math::Vec4;
    using math::Mat4;
    using math::Quat;
    using math::AABB;
    using math::Ray;
}
