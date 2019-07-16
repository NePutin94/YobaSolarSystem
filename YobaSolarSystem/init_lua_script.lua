function invert(v1)
 v1.x = v1.x * -1
 v1.y = v1.y * -1
 return v1
end

world:addObject(CelestialObject.new(vec2.new(300,1280),vec2.new(3,-0.5),15,735))