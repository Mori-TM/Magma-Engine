function Start()
	x = 0
	y = 1
	z = 4
	print("Hi")
end

function Update()
	w, h = GetWindowSize()
	mx, my = GetMousePosition()
	mx = (mx - (w)) * 0.5
	my = (my - (h)) * 0.5

	print(mx, my)

	speed = 0.1	

	if GetDownKey("w") then
		z = z - speed
	end
	if GetDownKey("s") then
		z = z + speed
	end
	if GetDownKey("a") then
		x = x - speed
	end
	if GetDownKey("d") then
		x = x + speed
	end
	
	SetEntity("Player", x, y, z, my, mx, 0, 1, 1, 1)
end