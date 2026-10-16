#include "engine/core/Math.hpp"
#include <iostream>
#include <string_view>
#include <functional>

namespace test {
    void registerTest(std::string_view name, std::function<bool()> testFunc);
}

#define TEST_CASE(name) \
    static bool test_func_##name(); \
    static const bool test_registered_##name = []() { \
        ::test::registerTest(#name, test_func_##name); \
        return true; \
    }(); \
    static bool test_func_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (false)

using namespace engine::core::math;

// 1. Trig & Utilities (5 tests)
TEST_CASE(Math_DegRadConversions) {
    CHECK(std::abs(radians(180.0f) - kPi) < 1e-5f);
    CHECK(std::abs(degrees(kPi) - 180.0f) < 1e-5f);
    CHECK(std::abs(radians(90.0f) - kHalfPi) < 1e-5f);
    CHECK(std::abs(degrees(kTwoPi) - 360.0f) < 1e-5f);
    return true;
}

TEST_CASE(Math_ClampAndLerp) {
    CHECK(clamp(5.0f, 0.0f, 10.0f) == 5.0f);
    CHECK(clamp(-1.0f, 0.0f, 10.0f) == 0.0f);
    CHECK(clamp(15.0f, 0.0f, 10.0f) == 10.0f);
    CHECK(lerp(0.0f, 100.0f, 0.5f) == 50.0f);
    CHECK(lerp(10.0f, 20.0f, 0.0f) == 10.0f);
    CHECK(lerp(10.0f, 20.0f, 1.0f) == 20.0f);
    return true;
}

// 2. Vec2 Tests (12 tests)
TEST_CASE(Math_Vec2_ConstructorsAndConstants) {
    Vec2 v0;
    CHECK(v0.x == 0.0f && v0.y == 0.0f);
    Vec2 v1(5.0f);
    CHECK(v1.x == 5.0f && v1.y == 5.0f);
    Vec2 v2(1.0f, 2.0f);
    CHECK(v2.x == 1.0f && v2.y == 2.0f);
    CHECK(Vec2::Zero == Vec2(0.0f, 0.0f));
    CHECK(Vec2::One == Vec2(1.0f, 1.0f));
    CHECK(Vec2::UnitX == Vec2(1.0f, 0.0f));
    CHECK(Vec2::UnitY == Vec2(0.0f, 1.0f));
    return true;
}

TEST_CASE(Math_Vec2_Arithmetic) {
    Vec2 a(2.0f, 3.0f);
    Vec2 b(4.0f, 5.0f);
    CHECK(a + b == Vec2(6.0f, 8.0f));
    CHECK(b - a == Vec2(2.0f, 2.0f));
    CHECK(a * 2.0f == Vec2(4.0f, 6.0f));
    CHECK(b / 2.0f == Vec2(2.0f, 2.5f));
    CHECK(-a == Vec2(-2.0f, -3.0f));
    CHECK(a * b == Vec2(8.0f, 15.0f));
    return true;
}

TEST_CASE(Math_Vec2_CompoundAssignment) {
    Vec2 v(1.0f, 2.0f);
    v += Vec2(3.0f, 4.0f);
    CHECK(v == Vec2(4.0f, 6.0f));
    v -= Vec2(1.0f, 1.0f);
    CHECK(v == Vec2(3.0f, 5.0f));
    v *= 2.0f;
    CHECK(v == Vec2(6.0f, 10.0f));
    v /= 2.0f;
    CHECK(v == Vec2(3.0f, 5.0f));
    return true;
}

TEST_CASE(Math_Vec2_LengthAndNormalize) {
    Vec2 v(3.0f, 4.0f);
    CHECK(v.lengthSquared() == 25.0f);
    CHECK(v.length() == 5.0f);
    Vec2 n = v.normalized();
    CHECK(std::abs(n.length() - 1.0f) < 1e-5f);
    CHECK(std::abs(n.x - 0.6f) < 1e-5f);
    CHECK(std::abs(n.y - 0.8f) < 1e-5f);
    Vec2 zeroN = Vec2::Zero.normalized();
    CHECK(zeroN == Vec2::Zero);
    return true;
}

TEST_CASE(Math_Vec2_DotAndDistance) {
    Vec2 a(1.0f, 0.0f);
    Vec2 b(0.0f, 1.0f);
    CHECK(dot(a, b) == 0.0f);
    CHECK(dot(a, a) == 1.0f);
    CHECK(distance(a, Vec2(4.0f, 0.0f)) == 3.0f);
    return true;
}

// 3. Vec3 Tests (12 tests)
TEST_CASE(Math_Vec3_ConstructorsAndConstants) {
    Vec3 v0;
    CHECK(v0.x == 0.0f && v0.y == 0.0f && v0.z == 0.0f);
    Vec3 v1(7.0f);
    CHECK(v1.x == 7.0f && v1.y == 7.0f && v1.z == 7.0f);
    Vec3 v2(1.0f, 2.0f, 3.0f);
    CHECK(v2.x == 1.0f && v2.y == 2.0f && v2.z == 3.0f);
    Vec3 v3(Vec2(4.0f, 5.0f), 6.0f);
    CHECK(v3 == Vec3(4.0f, 5.0f, 6.0f));
    CHECK(Vec3::Zero == Vec3(0.0f, 0.0f, 0.0f));
    CHECK(Vec3::UnitX == Vec3(1.0f, 0.0f, 0.0f));
    CHECK(Vec3::UnitY == Vec3(0.0f, 1.0f, 0.0f));
    CHECK(Vec3::UnitZ == Vec3(0.0f, 0.0f, 1.0f));
    return true;
}

TEST_CASE(Math_Vec3_Arithmetic) {
    Vec3 a(1.0f, 2.0f, 3.0f);
    Vec3 b(4.0f, 5.0f, 6.0f);
    CHECK(a + b == Vec3(5.0f, 7.0f, 9.0f));
    CHECK(b - a == Vec3(3.0f, 3.0f, 3.0f));
    CHECK(a * 3.0f == Vec3(3.0f, 6.0f, 9.0f));
    CHECK(b / 2.0f == Vec3(2.0f, 2.5f, 3.0f));
    CHECK(-a == Vec3(-1.0f, -2.0f, -3.0f));
    return true;
}

TEST_CASE(Math_Vec3_CrossProduct) {
    Vec3 x = Vec3::UnitX;
    Vec3 y = Vec3::UnitY;
    Vec3 z = Vec3::UnitZ;
    CHECK(cross(x, y) == z);
    CHECK(cross(y, z) == x);
    CHECK(cross(z, x) == y);
    CHECK(cross(y, x) == -z);
    return true;
}

TEST_CASE(Math_Vec3_LengthAndDot) {
    Vec3 v(2.0f, 3.0f, 6.0f);
    CHECK(v.lengthSquared() == 49.0f);
    CHECK(v.length() == 7.0f);
    CHECK(std::abs(v.normalized().length() - 1.0f) < 1e-5f);
    CHECK(dot(v, v) == 49.0f);
    CHECK(distance(Vec3::Zero, v) == 7.0f);
    return true;
}

// 4. Vec4 Tests (6 tests)
TEST_CASE(Math_Vec4_Basics) {
    Vec4 v0;
    CHECK(v0.x == 0.0f && v0.w == 0.0f);
    Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(v1.x == 1.0f && v1.y == 2.0f && v1.z == 3.0f && v1.w == 4.0f);
    Vec4 v2(Vec3(1.0f, 2.0f, 3.0f), 1.0f);
    CHECK(v2 == Vec4(1.0f, 2.0f, 3.0f, 1.0f));
    Vec4 sum = v1 + v2;
    CHECK(sum == Vec4(2.0f, 4.0f, 6.0f, 5.0f));
    return true;
}

// 5. Mat4 Tests (8 tests)
TEST_CASE(Math_Mat4_IdentityAndTranslation) {
    Mat4 id = Mat4::Identity;
    Vec4 pt(1.0f, 2.0f, 3.0f, 1.0f);
    CHECK(id * pt == pt);

    Mat4 trans = Mat4::translation(Vec3(10.0f, 20.0f, 30.0f));
    Vec4 moved = trans * pt;
    CHECK(moved == Vec4(11.0f, 22.0f, 33.0f, 1.0f));
    return true;
}

TEST_CASE(Math_Mat4_ScalingAndMultiplication) {
    Mat4 scale = Mat4::scaling(Vec3(2.0f, 3.0f, 4.0f));
    Vec4 pt(1.0f, 1.0f, 1.0f, 1.0f);
    CHECK(scale * pt == Vec4(2.0f, 3.0f, 4.0f, 1.0f));

    Mat4 trans = Mat4::translation(Vec3(5.0f, 0.0f, 0.0f));
    Mat4 combined = trans * scale;
    CHECK(combined * pt == Vec4(7.0f, 3.0f, 4.0f, 1.0f));
    return true;
}

TEST_CASE(Math_Mat4_OrthographicProjection) {
    Mat4 ortho = Mat4::orthographic(0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 100.0f);
    CHECK(ortho.m[0] > 0.0f);
    CHECK(ortho.m[5] != 0.0f);
    return true;
}

// 6. Quat Tests (4 tests)
TEST_CASE(Math_Quat_AxisAngle) {
    Quat qId = Quat::Identity;
    CHECK(qId.w == 1.0f && qId.x == 0.0f);

    Quat qRot = Quat::fromAxisAngle(Vec3::UnitZ, radians(90.0f));
    CHECK(std::abs(qRot.w - std::cos(radians(45.0f))) < 1e-5f);
    CHECK(std::abs(qRot.z - std::sin(radians(45.0f))) < 1e-5f);
    return true;
}

// 7. AABB Tests (8 tests)
TEST_CASE(Math_AABB_ContainsAndIntersects) {
    AABB box(Vec3(0.0f), Vec3(10.0f));
    CHECK(box.contains(Vec3(5.0f)));
    CHECK(box.contains(Vec3(0.0f)));
    CHECK(box.contains(Vec3(10.0f)));
    CHECK(!box.contains(Vec3(11.0f, 5.0f, 5.0f)));

    AABB overlapping(Vec3(5.0f), Vec3(15.0f));
    CHECK(box.intersects(overlapping));

    AABB separate(Vec3(20.0f), Vec3(30.0f));
    CHECK(!box.intersects(separate));

    CHECK(box.center() == Vec3(5.0f));
    CHECK(box.size() == Vec3(10.0f));

    box.expand(Vec3(12.0f, 5.0f, -2.0f));
    CHECK(box.contains(Vec3(12.0f, 5.0f, -2.0f)));
    return true;
}

// 8. Ray Tests (5 tests)
TEST_CASE(Math_Ray_AABBIntersection) {
    AABB box(Vec3(1.0f, -1.0f, -1.0f), Vec3(3.0f, 1.0f, 1.0f));
    Ray hitRay(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    float dist = 0.0f;
    CHECK(hitRay.intersects(box, dist));
    CHECK(std::abs(dist - 1.0f) < 1e-5f);

    Ray missRay(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    CHECK(!missRay.intersects(box, dist));
    return true;
}
