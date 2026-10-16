export const initialScene = {
  format: "engine.scene",
  version: 1,
  name: "level_01.scene.json",
  entities: [
    {
      id: "11111111-1111-4111-8111-111111111111",
      name: "Player",
      tag: "Player",
      active: true,
      components: {
        Transform: {
          position: [0, 3, 0],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        SpriteComponent: {
          size: [48, 64],
          color: "#00f2fe",
          sortingLayer: 10,
          texture: "player.png"
        },
        RigidBody2DComponent: {
          type: "Dynamic",
          fixedRotation: true,
          gravityScale: 1.0
        },
        Collider2DComponent: {
          type: "Capsule",
          size: [0.8, 1.6],
          radius: 0.4,
          friction: 0.2,
          restitution: 0.0
        },
        CharacterController2DComponent: {
          maxSpeed: 350,
          jumpVelocity: 750
        },
        ScriptComponent: {
          scriptName: "player.lua",
          enabled: true
        }
      }
    },
    {
      id: "22222222-2222-4222-8222-222222222222",
      name: "Ground",
      tag: "Environment",
      active: true,
      components: {
        Transform: {
          position: [0, -2, 0],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        SpriteComponent: {
          size: [2000, 80],
          color: "#1e293b",
          sortingLayer: 0,
          texture: "ground_stone.png"
        },
        RigidBody2DComponent: {
          type: "Static",
          fixedRotation: true,
          gravityScale: 0.0
        },
        Collider2DComponent: {
          type: "Box",
          size: [2000, 80],
          friction: 0.8,
          restitution: 0.0
        }
      }
    },
    {
      id: "33333333-3333-4333-8333-333333333333",
      name: "Platform_Floating",
      tag: "Environment",
      active: true,
      components: {
        Transform: {
          position: [4, 1.2, 0],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        SpriteComponent: {
          size: [220, 30],
          color: "#334155",
          sortingLayer: 1,
          texture: "platform_wood.png"
        },
        RigidBody2DComponent: {
          type: "Static",
          fixedRotation: true,
          gravityScale: 0.0
        },
        Collider2DComponent: {
          type: "Box",
          size: [220, 30],
          friction: 0.5,
          restitution: 0.0
        }
      }
    },
    {
      id: "44444444-4444-4444-8444-444444444444",
      name: "Collectible_Coin",
      tag: "Pickup",
      active: true,
      components: {
        Transform: {
          position: [4, 2.5, 0],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        SpriteComponent: {
          size: [32, 32],
          color: "#ffb300",
          sortingLayer: 5,
          texture: "coin_gold.png"
        },
        Collider2DComponent: {
          type: "Box",
          size: [32, 32],
          isTrigger: true
        }
      }
    },
    {
      id: "55555555-5555-4555-8555-555555555555",
      name: "MainCamera",
      tag: "Camera",
      active: true,
      components: {
        Transform: {
          position: [0, 1.5, 10],
          rotation: [0, 0, 0],
          scale: [1, 1, 1]
        },
        CameraComponent: {
          orthographic: true,
          size: 7.0,
          nearClip: 0.1,
          farClip: 1000.0,
          followTarget: "Player"
        }
      }
    }
  ]
};

export const defaultAssets = [
  {
    guid: "a1b2c3d4-0001-4000-8000-000000000001",
    name: "player.png",
    type: "Texture2D",
    path: "assets/sprites/player.png",
    sha256: "9b57adfe6a0236a54ccc4403a661b40146c03fc7b22805d6e3b3410f8f36de75",
    size: "48.2 KB",
    status: "game-ready"
  },
  {
    guid: "a1b2c3d4-0002-4000-8000-000000000002",
    name: "ground_stone.png",
    type: "Texture2D",
    path: "assets/sprites/ground_stone.png",
    sha256: "489ea9cd36ba5062c510de58f6f54e1691baff1eb9acb6d9d31f9072111644d7",
    size: "128.6 KB",
    status: "game-ready"
  },
  {
    guid: "a1b2c3d4-0003-4000-8000-000000000003",
    name: "coin_gold.png",
    type: "Texture2D",
    path: "assets/sprites/coin_gold.png",
    sha256: "c510de58f6f54e1691baff1eb9acb6d9d31f9072111644d7489ea9cd36ba5062",
    size: "16.4 KB",
    status: "game-ready"
  },
  {
    guid: "a1b2c3d4-0004-4000-8000-000000000004",
    name: "jump.ogg",
    type: "AudioClip",
    path: "assets/audio/jump.ogg",
    sha256: "b22805d6e3b3410f8f36de756a0236a54ccc4403a661b40146c03fc79b57adfe",
    size: "24.1 KB",
    status: "game-ready"
  },
  {
    guid: "a1b2c3d4-0005-4000-8000-000000000005",
    name: "player.lua",
    type: "LuaScript",
    path: "samples/platformer/scripts/player.lua",
    sha256: "804650c5229f414fb11c6a0236a5ca074084d2b9476a1929d31f9072111644d7",
    size: "674 B",
    status: "approved"
  }
];

export const defaultLuaCode = `-- player.lua (v0live-ops script)
local player = nil
local transform = nil
local movement = nil

function onStart()
    player = engine.get_entity("Player")
    if player == nil then
        engine.log("Player entity not found")
        return
    end
    transform = engine.get_transform(player)
    movement = engine.get_movement(player)
    engine.log("Player script started successfully!")
end

function onUpdate(delta)
    if transform == nil or movement == nil then
        return
    end
    local axis = engine.move_axis()
    transform.position.x = transform.position.x + axis.x * movement.speed * delta
    transform.position.y = transform.position.y + axis.y * movement.speed * delta
end
`;

export const defaultInputBindings = [
  { key: "Left", action: "MoveLeft" },
  { key: "A", action: "MoveLeft" },
  { key: "Right", action: "MoveRight" },
  { key: "D", action: "MoveRight" },
  { key: "Up", action: "MoveUp" },
  { key: "W", action: "MoveUp" },
  { key: "Down", action: "MoveDown" },
  { key: "S", action: "MoveDown" },
  { key: "Space", action: "Jump" },
  { key: "Escape", action: "Pause" }
];
