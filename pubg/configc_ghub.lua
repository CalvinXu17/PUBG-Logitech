configc_path = "%1"
rec_path = "%2"
weapon_path = "%3"
weapons = {}

function round(n)
    return math.floor(n+0.5)
end

function parse_bullets(t, count)
    if count <= 0 then
        return {}
    end
    local t1 = {}
    local t2 = {}
    for i=1, count do
        local movey = 0
        for index,value in ipairs(t) do
            if i >= value[1] and not (index==#t) and i < t[index+1][1] then
                movey = value[2] 
                break
            elseif index==#t and i >= value[1] then
                movey = value[2]
            end
        end
        t1[i] = movey
    end
    t2[1] = t1[1]
    for i=2, #t1 do
        t2[i] = t2[i-1] + t1[i]
    end
    return t2
end

function parse_weapons()
    weapons = {}
    dofile(weapon_path)
    for _, w in pairs(weapons) do
        w.ballistic = parse_bullets(w.ballistic, w.max)
    end
end

function reloadconfig(vars)
    OutputLogMessage("reloadconfig\n")
    dofile(configc_path)
    bindkeys(config,vars)    
end

function leftbutton(vars)
    if vars.autorecognize then
        dofile(rec_path)
        vars.weapon = rweapon
        vars.pose = rpose
        vars.scope = rscope
        vars.a1 = ra1
        vars.a2 = ra2
        vars.a4 = ra4
    end

    vars.left = true
    vars.start_time = GetRunningTime()
    vars.current_time = vars.start_time

    if ads(vars) and vars["weapon"] ~= "none" and vars["pose"] > 0 then
        shooting(vars)
    end
end

function leftbutton_release(vars)
    vars.left = false
end

function rightbutton(vars)
    vars.right = true
end

function rightbutton_release(vars)
    if vars.autorecognize then
        dofile(rec_path)
        vars.weapon = rweapon
        vars.pose = rpose
        vars.scope = rscope
        vars.a1 = ra1
        vars.a2 = ra2
        vars.a4 = ra4
    end
    vars.right = false
end

function ads(vars)
    if vars.adsmode == "HOLD" then
        return vars.right
    else
        return true
    end
end

function shooting(vars)
    vars.ycounter = 0
    repeat
        if not dorecoil(vars) then break end
    until not IsMouseButtonPressed(1)
end

function get_realY(vars, y)
    local weapon = weapons[vars.weapon]
    --lua index starts from 1, but pose ignores index 0
    local realy = y * weapon.scope_sensitive[vars.scope+1] * weapon.pose_sensitive[vars.pose] * weapon.a1_sensitive[vars.a1+1] * weapon.a2_sensitive[vars.a2+1] * weapon.a4_sensitive[vars.a4+1]
    realy = realy / vars.y_sensitive
    return round(realy)
end

function dorecoil(vars)
    if vars.debug then
        OutputLogMessage("---Recoil---\n")
    end

    if vars.current_time and vars.debug then
        OutputLogMessage("Loading: " .. GetRunningTime() - vars.current_time .. "\n")
    end

    vars.current_time = GetRunningTime()

    local movex = 0
    local movey = 0
    if vars.debug then
        movex = vars.xbase
    end
    if vars.autorecognize then
        dofile(rec_path)
        if rpose > 0 then
            vars.pose=rpose
        end
    end

    local weapon = weapons[vars.weapon]
    local step = math.ceil(((vars.current_time - vars.start_time == 0 and {1} or {vars.current_time - vars.start_time})[1]) / weapon.speed)

    if step > weapon.max then
        if vars.debug then
            OutputLogMessage("Finished \n")
            PressAndReleaseMouseButton(1)
        end
        return false
    end

    movey = round((vars.current_time - vars.start_time) / (weapon.speed * step) * weapon.ballistic[step]) - vars.ycounter
    realy = get_realY(vars, movey)

    if vars.autorecognize and step>3 then
        realy = realy + ry
        movex = rx + movex
    end

    movex = round(movex)
    realy = round(realy)

    if vars.debug then
        OutputLogMessage("AutoRecognize: " .. tostring(vars.autorecognize) .. "\n")
        OutputLogMessage("Weapon: " .. vars.weapon .. "\n")
        OutputLogMessage("Scope: " .. vars.scope .. "\n")
        OutputLogMessage("Pose: " .. vars.pose .. "\n")
        OutputLogMessage("Attachment 1: " .. vars.a1 .. "\n")
        OutputLogMessage("Attachment 2: " .. vars.a2 .. "\n")
        OutputLogMessage("Step: " .. step .. "\n")
        OutputLogMessage("Move: " .. movex .. ":" .. movey .. "realY:" .. realy .. "\n")
        OutputLogMessage("rx: " .. rx .. "\n")
        OutputLogMessage("ry: " .. ry .. "\n")
    end
 
    MoveMouseRelative(movex, realy)
    vars.ycounter = vars.ycounter + movey
    Sleep(%4)
    --SetMKeyState(3)
    return true
end

function bindkeys(config,vars)
    config["lctrl,lshift,lalt+6"] = {
        button = 6,
        modifier = {"lctrl","lshift","lalt"},
        funcPress = "reloadconfig",
    }
    config["+1"] = {
        button = 1,
        modifier = {},
        funcPress = "leftbutton",
        funcRelease = "leftbutton_release",
    }
    config["+2"] = {
        button = 2,
        modifier = {},
        funcPress = "rightbutton",
        funcRelease = "rightbutton_release",
    }
    config["keys"]={"+1","+2","lctrl,lshift,lalt+6"}
    vars["left"]=false
    vars["right"]=false
    vars["ycounter"]=0
    vars["autorecognize"]=1
    vars["adsmode"]="%5"
    vars["weapon"]="none"
    vars["scope"]=1
    vars["a1"]=1
    vars["a2"]=1
    vars["a4"]=1
    vars["pose"]=1
    vars["scope_sensitive"]=%6
    vars["y_sensitive"]=%7
    vars["debug"]=%8
    vars["xbase"]=%9
    parse_weapons()
end
