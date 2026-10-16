#pragma once

#include "engine/core/GUID.hpp"
#include "engine/core/Math.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace engine::ecs {

using EntityId = uint64_t;
constexpr EntityId kInvalidEntityId = 0;

// 1. Identité universelle persistante
struct IDComponent {
    core::GUID id{core::GUID::generate()};

    IDComponent() = default;
    explicit IDComponent(const core::GUID& guid) : id(guid) {}
};

// 2. Nommage lisible pour l'éditeur et l'arborescence
struct TagComponent {
    std::string tag{"Entity"};
    std::string name{"Entity"};

    TagComponent() = default;
    explicit TagComponent(std::string_view inName) : tag(inName), name(inName) {}
};

// 3. Transform spatial 2D / 3D
struct TransformComponent {
    core::Vec3 position{0.0f, 0.0f, 0.0f};
    core::Vec3 rotation{0.0f, 0.0f, 0.0f}; // Angles d'Euler en degrés
    core::Vec3 scale{1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    explicit TransformComponent(const core::Vec3& pos) : position(pos) {}
    explicit TransformComponent(const core::Vec2& pos2D, float z = 0.0f) : position(pos2D.x, pos2D.y, z) {}

    [[nodiscard]] core::Mat4 getLocalTransform() const noexcept {
        core::Mat4 t = core::Mat4::translation(position);
        core::Mat4 s = core::Mat4::scaling(scale);
        return t * s;
    }
};

// 4. Rendu de Sprite 2D
struct SpriteComponent {
    core::GUID textureGuid{core::GUID::Invalid};
    core::Vec2 size{32.0f, 32.0f};
    core::Vec4 colorTint{1.0f, 1.0f, 1.0f, 1.0f};
    core::Vec4 tint{1.0f, 1.0f, 1.0f, 1.0f};
    int sortingLayer{0};
    bool flipX{false};
    bool flipY{false};

    SpriteComponent() = default;
    SpriteComponent(const core::Vec2& inSize, const core::Vec4& inColor = core::Vec4(1.0f))
        : size(inSize), colorTint(inColor), tint(inColor) {}
};

// 5. Mouvement & Contrôles Gameplay (Platformer 2D)
struct MovementComponent {
    core::Vec2 velocity{0.0f, 0.0f};
    float speed{200.0f}; // Vitesse modifiable dans l'Inspector
    bool isGrounded{false};

    MovementComponent() = default;
    explicit MovementComponent(float inSpeed) : speed(inSpeed) {}
};

#include <glm/glm.hpp>

// 6. Hiérarchie Scène (Relations Parent / Enfants)
struct HierarchyComponent {
    EntityId parent{kInvalidEntityId};
    std::vector<EntityId> children;

    HierarchyComponent() = default;
};

// 7. Physique 2D (Box2D v3)
enum class RigidBody2DType : std::uint8_t { Static, Kinematic, Dynamic };

struct RigidBody2DComponent {
    RigidBody2DType type = RigidBody2DType::Dynamic;
    float gravityScale = 1.0f;
    float linearDamping = 0.0f;
    bool fixedRotation = false;
    bool isSensor = false;
};

enum class Collider2DType : std::uint8_t { Box, Circle, Capsule };

struct Collider2DComponent {
    Collider2DType type = Collider2DType::Box;
    glm::vec2 size = glm::vec2(1.0f);
    float radius = 0.5f;
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
    std::uint64_t categoryBits = 1;
    std::uint64_t maskBits = UINT64_MAX;
    bool isTrigger = false;
    bool enableContactEvents = false;
    bool enableSensorEvents = false;
};

struct CharacterController2DComponent {
    float maxSpeed = 350.0f;
    float acceleration = 2500.0f;
    float jumpVelocity = 750.0f;
    float coyoteTime = 0.12f;
    float jumpBufferTime = 0.12f;
    float maxSlopeAngle = 45.0f;
    bool grounded = false;
    bool jumping = false;
    float coyoteTimer = 0.0f;
    float jumpBufferTimer = 0.0f;
};

} // namespace engine::ecs

namespace engine {
    using ecs::RigidBody2DType;
    using ecs::RigidBody2DComponent;
    using ecs::Collider2DType;
    using ecs::Collider2DComponent;
    using ecs::CharacterController2DComponent;
} // namespace engine
