package.cpath = package.cpath .. ";/home/charlotte/devel/vjidi/build/?.so"

local midi = require("luamidi")

midi.openmididevice()
midi.timingstart()

midi.tempo(120)
midi.timesig(4, 2)

for i = 0, 8 do
	midi.noteon(50, 64)
	midi.waitrel(96)
	midi.noteoff(50)
end

midi.timingstop()
midi.closemididevice()
