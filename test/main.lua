local pos = function(x, y, z)
    return { x = x, y = y, z = z }
end

local material = function(r, g, b, emission)
    return { color = { r = r, g = g, b = b }, emission = emission }
end

local scene_size = { x = 50, y = 50, z = 50 }

return {
    output_file = "output.bmp",
    settings = {
        image_size = { x = 1920, y = 1080 },
        iterations = 100,
        max_depth = 5,
    },
    scene = {
        size = scene_size,
        bg = material(0.5, 0.5, 1.0, 1),
        data = function(scene)
            local white = scene:register_material(material(0.8, 0.8, 0.8, 0))
            local red = scene:register_material(material(0.8, 0.1, 0.1, 0))
            local green = scene:register_material(material(0.1, 0.8, 0.1, 0))
            local light = scene:register_material(material(1.0, 1.0, 0.5, 10))

            -- floor and ceiling
            for x = 0, scene.size.x - 1 do
                for z = 0, scene.size.z - 1 do
                    scene:set(pos(x, 0, z), white)
                    scene:set(pos(x, scene.size.y - 1, z), white)
                end
            end

            -- side walls
            for y = 0, scene.size.y - 1 do
                for z = 0, scene.size.z - 1 do
                    scene:set(pos(0, y, z), red)
                    scene:set(pos(scene.size.x - 1, y, z), green)
                end
            end

            -- back wall
            for x = 0, scene.size.x - 1 do
                for y = 0, scene.size.y - 1 do
                    scene:set(pos(x, y, scene.size.z - 1), white)
                end
            end

            -- light
            for x = 0, scene.size.x / 2 - 1 do
                for z = 0, scene.size.y / 2 - 1 do
                    scene:set(pos(x + scene.size.x / 4, 0, z + scene.size.y / 4), light)
                end
            end

            -- cube
            for x = 0, scene.size.x / 5 - 1 do
                for y = 0, scene.size.y / 3 - 1 do
                    for z = 0, scene.size.z / 5 - 1 do
                        scene:set(pos(scene.size.x / 5 + x, scene.size.y - y, scene.size.z / 5 + z), white)
                    end
                end
            end
        end
    },
    camera = {
        sensor_size = { x = 1.9, y = 1.0 },
        focal_length = 1,
        position = {
            x = scene_size.x * 0.75,
            y = scene_size.y * 0.25,
            z = scene_size.z * -1.25
        },
        rotation = { x = 5, y = 10, z = 0 },
    }
}
