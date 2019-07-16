
 collision = false
for i=1, #world.objects do
 for i2 = 2, #world.objects do
 	 obj1 = world.objects[i]
 	 obj2 = world.objects[i2]
   fG = world:ForceOfGravity(obj1, obj2);
   collision = world:isCollision(obj1, obj2);
	if (collision) then
		fG = 0.00000000001;
	end
   distance = math.sqrt(math.pow(obj2.obj:getPos().x - obj1.obj:getPos().x,2) + math.pow(obj2.obj:getPos().y - obj1.obj:getPos().y,2))
   massJtoI = (obj2:getMass() / obj1:getMass())
   massItoJ =  (obj1:getMass() / obj2:getMass())
   fGmassJtoI = fG * math.pow(massJtoI, 3) / math.pow(distance, 0.25);
	 fGmassItoJ = fG * math.pow(massItoJ, 3) / math.pow(distance, 0.25);
   direction = vec2.new()
  if(obj1.obj:getPos().x > obj2.obj:getPos().x) then 
  	direction.x = -1
  else
  	direction.x = 1
  end
  if(obj1.obj:getPos().y > obj2.obj:getPos().y) then 
  	direction.y = -1
  else
  	direction.y = 1
  end
  if (not collision) then			
		if (obj2:getMass() > obj1:getMass()) then
			 v1 = multiply(vec2.new(fGmassJtoI / math.sqrt(distance) * massJtoI, fGmassJtoI / math.sqrt(distance) * massJtoI), direction);
			 v2 = multiply(vec2.new(fGmassItoJ, fGmassItoJ),invert(direction));
			obj1:addedAcceleration(v1);
			obj2:addedAcceleration(v2);	
		else
			 v2 = multiply(vec2.new(fGmassItoJ, fGmassItoJ), invert(direction));
			obj1:addedAcceleration(multiply(vec2.new(fGmassJtoI / math.sqrt(distance) * massJtoI, fGmassJtoI / math.sqrt(distance) * massJtoI),direction));
			obj2:addedAcceleration(v2);
	  end
	end
 end
end