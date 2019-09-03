local midi = require("luamidi")

function love.load()
  alphabet = {
    a=1,
    b=2,
    c=3,
    d=4,
    e=5,
    f=6,
    g=7,
    h=8,
    i=9,
    j=10,
    k=11,
    l=12,
    m=13,
    n=14,
    o=15,
    p=16,
    q=17,
    r=18,
    s=19,
    t=20,
    u=21,
    v=22,
    w=23,
    x=24,
    y=25,
    z=26,
  }

  for k, v in pairs(alphabet) do alphabet[k] = v+30 end

  tempo = 120

	midi.openmididevice()
	--midi.timingstart() --[[XXX?]]
	midi.tempo(tempo)
	midi.timesig(4, 2)

  for k, v in pairs(midi) do
    print(k, v)
  end
end

function love.keypressed(key, scancode, isrepeat)
	if key == "escape" then
	  love.event.quit()
    return
	end

	midi.noteon(alphabet[key], 127)
end

function love.keyreleased(key, scancode)
  if key == "escape" then return end
	midi.noteoff(alphabet[key])
end


function love.mousemoved(x, y, dx, dy, istouch)
  tempo = x/2
  midi.tempo(tempo)
end

function love.quit()
	--midi.timingstop() --[[XXX?]]
	midi.closemididevice()
end

function love.draw()
  love.graphics.print("tempo = "..tempo, 10, 10)
end
