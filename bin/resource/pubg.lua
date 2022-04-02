EnablePrimaryMouseButtonEvents(true)
config = {}
vars = {}

dofile("C:/Users/10842/AppData/Local/Temp/configc.lua")

bindkeys(config,vars)

function OnEvent(event, arg)
    for _,key in pairs(config["keys"]) do
	  buttonconfig = config[key]
        if arg == buttonconfig["button"] then
            match = true
            for _,modifier in pairs(buttonconfig["modifier"]) do
                if not IsModifierPressed(modifier) then
                    match = false
                end
            end
            if match then
                local func = _G[buttonconfig["funcPress"]]
                if func and event=="MOUSE_BUTTON_PRESSED" then
                    func(vars)
                return
                else
                    func = _G[buttonconfig["funcRelease"]]
                    if func then
                        func(vars)
				 return
                    end
                end
            end
        end
    end
end
