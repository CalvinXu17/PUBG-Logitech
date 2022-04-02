function reloadconfig(vars)

    OutputLogMessage("reloadconfig\n")
    dofile("%1/configc.lua")
    bindkeys(config,vars)
        
end
function pause_recognize(vars)

    vars["autorecognize"] = false
    OutputLogMessage("pause_recognize\n")
        
end
function resume_recognize(vars)

    vars["autorecognize"] = true
    OutputLogMessage("resume_recognize\n")
        
end
function leftbutton(vars)

	if vars["autorecognize"] then
        dofile("%2/x.lua")
        vars["weapon"] = rweapon
        vars["pose"] = rpose
        vars["scope"] = rscope
        vars["a2"] = rattachment2
        vars["a3"] = rattachment3
    end
	
    vars["left"] = true
    start_time = GetRunningTime()
	
	if ads(vars) and vars["weapon"] ~= "none" and vars["pose"] > 0 then
		shooting(vars)
	end
end
function leftbutton_release(vars)

    vars["left"] = false
        
end
function rightbutton(vars)
    vars["right"] = true
    if (vars["autoshift"] and vars["scope"] >= vars["autoshiftscope"]) or (table[vars["weapon"]]["shift"] == true) then
        Sleep(10)
        PressKey("lshift")
    end
end
function rightbutton_release(vars)
	
	if vars["autorecognize"] then
        dofile("%3/x.lua")
        vars["weapon"] = rweapon
        vars["pose"] = rpose
        vars["scope"] = rscope
        vars["a2"] = rattachment2
        vars["a3"] = rattachment3
    end
	
    vars["right"] = false
	if vars["autoshift"] then
		ReleaseKey("lshift")
	end
        
end
function ads(vars)
    if vars["adsmode"] == "HOLD" then
        return vars["right"]
	else
		return true
	end
end

function shooting(vars)
    repeat
        dorecoil(vars)
    until not IsMouseButtonPressed(1)
end

function dorecoil(vars)
	
	if vars["debug"] then
		ClearLog()
		OutputLogMessage("---Recoil---\n")
	end
		
	if current_time and vars["debug"] then
		OutputLogMessage("Loading:	" .. GetRunningTime() - current_time .. "\n")
	end

    current_time = GetRunningTime()

    if vars["debug"] then
        movex = 10
    else
        movex = 0
    end
    movey = 0

    if vars["autorecognize"] then
        dofile("%4/x.lua")
		if rpose > 0 then
			vars["pose"]=rpose
		end
    end

    step = math.ceil(((current_time - start_time == 0 and {1} or {current_time - start_time})[1]) / table[vars["weapon"]]["speed"])

    if step > table[vars["weapon"]]["max"] then	
        if vars["debug"] then
			OutputLogMessage("Finished \n")
            PressAndReleaseMouseButton(1)
        end
        return
    end

    base = table[vars["weapon"]]["base"]

    movey = base

    for index,value in ipairs(table[vars["weapon"]]["ballistic"]) do
        if (step >= value[1] and not (index==#table[vars["weapon"]]["ballistic"]) and step < table[vars["weapon"]]["ballistic"][index+1][1]) then
            movey = base + value[2] 
            break
        elseif ( index==#table[vars["weapon"]]["ballistic"] ) then
            movey = base + table[vars["weapon"]]["ballistic"][index-1][2]
            break
        end
    end
        
    movey = movey * table[vars["weapon"]]["limit"] * table[vars["weapon"]]["scope_sensitive"][vars["scope"]] * table[vars["weapon"]]["pose_sensitive"][vars["pose"]] * table[vars["weapon"]]["attachment2_sensitive"][vars["a2"]]* table[vars["weapon"]]["attachment3_sensitive"][vars["a3"]]
 
    if (table[vars["weapon"]]["single"] == true) then
        if (current_time - start_time)%table[vars["weapon"]]["speed"] < 15 then	
            PressAndReleaseMouseButton(1)
        end
    elseif (vars["left"] and vars["right"]) then
        if (IsModifierPressed("lshift") and (vars["scope"] < 3)) then
            movey = movey * 1.35
        elseif (vars["aim"]) then
		
        end
    elseif (vars["aim"]) then
        -- 肩射灵敏度
        -- movey = movey * aim_rate
        movey = movey * 1.3
    end

    if vars["autorecognize"] and step>3 then
        movey = movey + ry
        movex = rx + movex
        if not (movex == 0) and vars["debug"] then
            OutputLogMessage(rx .. ":" .. ry .. "\n" )
        end
    end
	
	movex = math.ceil(movex)
	movey = math.ceil(movey)
	
	if vars["debug"] then
		OutputLogMessage("AutoRecognize:	" .. tostring(vars["autorecognize"]) .. "\n")
		OutputLogMessage("Weapon:	" .. vars["weapon"] .. "\n")
		OutputLogMessage("Scope:	" .. vars["scope"] .. "\n")
		OutputLogMessage("Pose:	" .. vars["pose"] .. "\n")
		OutputLogMessage("Attachment 2:	" .. vars["a2"] .. "\n")
		OutputLogMessage("Attachment 3:	" .. vars["a3"] .. "\n")
		OutputLogMessage("Hold breath:	" .. tostring((vars["autoshift"] and vars["scope"] >= vars["autoshiftscope"]) or (table[vars["weapon"]]["shift"]) ) .. "\n")
		OutputLogMessage("Step:	" .. step .. "\n")
		OutputLogMessage("Move:	" .. movex .. ":" .. movey .. "\n")
	end
       
    if movey > 120 then
        -- move最大支持120
        movey = 120
    end

    MoveMouseRelative(movex, movey)
    Sleep(8)
    --SetMKeyState(3)
end
function bindkeys(config,vars)
	config["lctrl,lshift,lalt+6"] = {
        button = 6,
        modifier = {"lctrl","lshift","lalt"},
        funcPress = "reloadconfig",
    }
    config["lshift+4"] = {
        button = 4,
        modifier = {"lshift"},
        funcPress = "pause_recognize",
    }
    config["lctrl+4"] = {
        button = 4,
        modifier = {"lctrl"},
        funcPress = "resume_recognize",
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
	config["keys"]={"+1","+2","lctrl,lshift,lalt+6","lshift+4","lctrl+4"}
	vars["adsmode"]="%5"
	vars["aimbutton"]=5
	vars["autorecognize"]=%6
	vars["debug"]=%7
	vars["weapon"]="none"
	vars["scope"]=1
	vars["pose"]=1
	vars["autoshift"]=false
	vars["autoshiftscope"]=1
	vars["loading"]=8
	vars["a2"]=1
	vars["a3"]=1
dofile("%8/weapon.lua")

end
