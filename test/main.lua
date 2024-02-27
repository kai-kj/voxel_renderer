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
        bg_color = { r = 0.75, g = 0.75, b = 1.0 },
        bg_intensity = 0.5,
        data = function(scene)
            local white = { r = 0.8, g = 0.8, b = 0.8, emission = 0 }
            local red = { r = 0.8, g = 0.1, b = 0.1, emission = 0 }
            local green = { r = 0.1, g = 0.8, b = 0.1, emission = 0 }
            local light = { r = 0.8, g = 0.8, b = 1.0, emission = 0.9 }

            -- floor and ceiling
            for x = 0, scene.size.x - 1 do
                for z = 0, scene.size.z - 1 do
                    scene:set(x, 0, z, white)
                    scene:set(x, scene.size.y - 1, z, white)
                end
            end

            -- side walls
            for y = 0, scene.size.y - 1 do
                for z = 0, scene.size.z - 1 do
                    scene:set(0, y, z, red)
                    scene:set(scene.size.x - 1, y, z, green)
                end
            end

            -- back wall
            for x = 0, scene.size.x - 1 do
                for y = 0, scene.size.y - 1 do
                    scene:set(x, y, scene.size.z - 1, white)
                end
            end

            -- light
            for x = 0, scene.size.x / 2 - 1 do
                for z = 0, scene.size.y / 2 - 1 do
                    scene:set(x + scene.size.x / 4, 0, z + scene.size.y / 4, light)
                end
            end

            -- cube
            for x = 0, scene.size.x / 5 - 1 do
                for y = 0, scene.size.y / 3 - 1 do
                    for z = 0, scene.size.z / 5 - 1 do
                        scene:set(scene.size.x / 5 + x, scene.size.y - y, scene.size.z / 5 + z, white)
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
