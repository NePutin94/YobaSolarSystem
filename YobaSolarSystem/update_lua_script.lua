world:clearVertexArray()
for _, obj1 in ipairs(world.objects) do
    world:setSprite(obj1)
    for _, obj2 in ipairs(world.objects) do
        if(obj1.mass ~= obj2.mass) then
            local fG = world:ForceOfGravity(obj1, obj2)
            collision = world:isCollision(obj1, obj2)
            
            local distance = math.sqrt(math.pow(obj1.pos.x - obj2.pos.x, 2) + math.pow(obj2.pos.y - obj1.pos.y, 2))
            local massJtoI = (obj2.mass / obj1.mass)
            local massItoJ = (obj1.mass / obj2.mass)
            local fGmassJtoI = fG * math.pow(massJtoI, 3) / math.pow(distance, 0.25);
            localfGmassItoJ = fG * math.pow(massItoJ, 3) / math.pow(distance, 0.25);
            
            direction = vec2.new()
            if obj1.pos.x > obj2.pos.x then
                direction.x = -1
            else
                direction.x = 1
            end
            if obj1.pos.y > obj2.pos.y then
                direction.y = -1
            else
                direction.y = 1
            end
            
            if(not collision) then
                if((obj1 == biggestObject) or (obj2 == biggestObject)) then
                    if(obj2.mass > obj1.mass) then
                        obj1:addedAcceleration(multiply(vec2.new(fGmassJtoI, fGmassJtoI), direction))
                        obj2:addedAcceleration(multiply(vec2.new(fGmassItoJ / distance * massItoJ, fGmassItoJ / distance * massItoJ), invert(direction)))
                    else
                        obj1:addedAcceleration(multiply(vec2.new(fGmassItoJ / distance * massJtoI, fGmassItoJ / distance * massJtoI), direction))
                        obj2:addedAcceleration(multiply(vec2.new(fGmassJtoI, fGmassJtoI), invert(direction)))
                    end
                    world:calculateVertex(obj1.pos, obj2.pos)
                end
            else
                world:ifCollision(obj1, obj2)
            end
            
        end
    end
end

