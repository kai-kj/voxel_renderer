local workgroup_size = { 16, 16 }

local run_command = function(command)
    local f = io.popen(command)
    if f == nil then error("Failed to run command: " .. command) end
    local output = f:read("*a")
    local _, _, status = f:close()
    return output, status
end

local compile_shader = function(path, workgroup_size)
    local defs = "-DWORKGROUP_SIZE_X=" .. workgroup_size[1] .. " -DWORKGROUP_SIZE_Y=" .. workgroup_size[2]
    local output, status = run_command("glslc -S -O -fshader-stage=comp " .. defs .. " " .. path .. " -o -")
    if status ~= 0 then
        print(output)
        error("Failed to compile shader " .. path .. " (see above)")
    end
    return output
end

return {
    output_file = "output.bmp",
    settings = {
        renderer_code = compile_shader("../shader/renderer.glsl", workgroup_size),
        output_code = compile_shader("../shader/output.glsl", workgroup_size),
        workgroup_size = workgroup_size,
        image_size = { 1920, 1080 },
        iterations = 100,
        max_depth = 5,
    },
    scene = {
        size = { 50, 50, 50 },
        bg = { color = { 0.5, 0.5, 1.0 }, emission = 1 },
        data = function(scene)
            local white = scene:register_material({ color = { 0.8, 0.8, 0.8 }, emission = 0 })
            local red = scene:register_material({ color = { 0.8, 0.1, 0.1 }, emission = 0 })
            local green = scene:register_material({ color = { 0.1, 0.8, 0.1 }, emission = 0 })
            local light = scene:register_material({ color = { 1.0, 1.0, 0.5 }, emission = 10 })

            -- floor and ceiling
            for x = 0, scene.size.x - 1 do
                for z = 0, scene.size.z - 1 do
                    scene:set({ x, 0, z }, white)
                    scene:set({ x, scene.size.y - 1, z }, white)
                end
            end

            -- side walls
            for y = 0, 50 - 1 do
                for z = 0, 50 - 1 do
                    scene:set({ 0, y, z }, red)
                    scene:set({ 50 - 1, y, z }, green)
                end
            end

            -- back wall
            for x = 0, 50 - 1 do
                for y = 0, 50 - 1 do
                    scene:set({ x, y, 50 - 1 }, white)
                end
            end

            -- light
            for x = 15, 35 - 1 do
                for z = 15, 35 - 1 do
                    scene:set({ x, 0, z }, light)
                end
            end

            -- cube
            for x = 10, 20 - 1 do
                for y = 35, 50 - 1 do
                    for z = 10, 20 - 1 do
                        scene:set({ x, y, z }, white)
                    end
                end
            end
        end
    },
    camera = {
        sensor_size = { 1.9, 1.0 },
        focal_length = 1,
        position = { 40, 10, -75 },
        rotation = { 5, 10, 0 },
    }
}
