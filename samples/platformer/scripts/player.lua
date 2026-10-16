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
    engine.log("Player script started")
end

function onUpdate(delta)
    if transform == nil or movement == nil then
        return
    end
    local axis = engine.move_axis()
    transform.position.x = transform.position.x + axis.x * movement.speed * delta
    transform.position.y = transform.position.y + axis.y * movement.speed * delta
end
