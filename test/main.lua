local src_pos_len = 10
local log_pos_len = 50

local run_command = function(command)
    local f = io.popen(command)
    if f == nil then
        error("Failed to run command: " .. command)
    end
    local output = f:read("*a")
    local _, _, status = f:close()
    return output:sub(1, -2), status
end

local read_file = function(path)
    local file = io.open(path, "r")
    if file == nil then
        error("failed to open file: " .. path)
    end
    local content = file:read("*a")
    file:close()
    return content
end

local root = run_command("pwd"):gsub("/[^/]*$", "/")

return {
    output_file = "output.bmp",

    logger = function(lvl, src, file, line, msg)
        local colors = { "\27[34m", "\27[32m", "\27[33m", "\27[31m" }
        local color = colors[lvl + 1]

        local lvls = { "DEBUG  ", "INFO   ", "WARN   ", "ERROR  ", "UNKNOWN" }
        local lvl = lvls[lvl + 1]

        local src = src:sub(1, src_pos_len) .. string.rep(" ", src_pos_len - #src)

        local file = file:gsub(root, "")
        local pos = #file ~= 0 and file .. ":" .. line or ""
        pos = pos:sub(1, log_pos_len) .. string.rep(" ", log_pos_len - #pos)

        print(color .. lvl .. " │ " .. src .. " │ " .. pos .. " │ " .. msg:gsub("\n", "") .. "\27[0m")
    end,

    device_selector = function(devices)
        local best = { index = 1, score = 0 }
        for i, device in ipairs(devices) do
            local score = 0
            if device.type == "gpu" then
                score = score + 1
            end
            if score > best.score then
                best = { index = i, score = score }
            end
        end
        return best.index;
    end,

    renderer = {
        renderer_code = read_file("../shader/renderer.glsl"),
        output_code = read_file("../shader/output.glsl"),
        workgroup_size = { 16, 16 },
        image_size = { 1920, 1080 },
        iterations = 100,
        max_depth = 5,
    },

    scene = {
        size = { 50, 50, 50 },
        bg = { color = { 0.5, 0.5, 1.0 }, emission = 1 },
        voxel_placer = function(scene)
            local white = scene:register_material({ color = { 0.8, 0.8, 0.8 }, emission = 0 })
            local red = scene:register_material({ color = { 0.8, 0.1, 0.1 }, emission = 0 })
            local green = scene:register_material({ color = { 0.1, 0.8, 0.1 }, emission = 0 })
            local light = scene:register_material({ color = { 1.0, 1.0, 0.5 }, emission = 10 })

            -- floor and ceiling
            for x = 0, 50 - 1 do
                for z = 0, 50 - 1 do
                    scene:set({ x, 0, z }, white)
                    scene:set({ x, 50 - 1, z }, white)
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
        end,
    },

    camera = {
        sensor_size = { 1.9, 1.0 },
        focal_length = 1,
        position = { 40, 10, -75 },
        rotation = { 5, 10, 0 },
    },
}
