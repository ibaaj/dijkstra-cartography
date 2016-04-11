-- Railway profile
 
speed_profile = {
  ["default"] = 70,
  ["rail"] = 70,
  ["station"] = 70,
  ["platform"] = 70,
  ["halt"] = 70,
  ["switch"] = 70,
  ["level_crossing"] = 70
}
 
-- these settings are read directly by osrm
 
take_minimum_of_speeds  = true
obey_oneway             = true
obey_barriers           = true
use_turn_restrictions   = true
ignore_areas            = true  -- future feature
traffic_signal_penalty  = 7     -- seconds
u_turn_penalty          = 20
 
function limit_speed(speed, limits)
  -- don't use ipairs(), since it stops at the first nil value
  for i=1, #limits do
    limit = limits[i]
    if limit ~= nil and limit > 0 then
      if limit < speed then
        return limit        -- stop at first speedlimit that's smaller than speed
      end
    end
  end
  return speed
end
 
function node_function (node)
end
 
function way_function (way, result)
 
 if way:get_value_by_key("railway") then
 
  local railway = way:get_value_by_key("railway")
  local service = way:get_value_by_key("service")
  local usage = way:get_value_by_key("usage")
 
  
  if railway ~= "rail"  then
    return
  end
  
  if service == "spur" then
    return
  end
  
  if usage == "industrial" or usage == "military" or usage == "tourism" then
    return
  end
 
  local name = way:get_value_by_key("name")
  local oneway = way:get_value_by_key("oneway")
 
  result.name = name
 
  local speed_forw = speed_profile[railway] or speed_profile['default']
  local speed_back = speed_forw

  result.forward_speed = speed_forw
  result.backward_speed = speed_back
  
  if oneway == "no" or oneway == "0" or oneway == "false" then
    -- nothing to do
  elseif oneway == "-1" then
    result.backward_mode = 0
  elseif oneway == "yes" or oneway == "1" or oneway == "true" then
    result.backward_mode = 0
  end
 end
end
