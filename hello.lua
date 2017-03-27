--[[
charlotte's test script
]]

function showtable(tab)
  print "====="
  for k, v in pairs(tab) do print(k, v) end
  print "====="
end

function onnoteup(ev)
  print "NOTE UP"
  showtable(ev)
end

function onnotedown(ev)
  print "NOTE DOWN"
  showtable(ev)
end

function oncontrol(ev)
  print "CONTROL"
  showtable(ev)
end
