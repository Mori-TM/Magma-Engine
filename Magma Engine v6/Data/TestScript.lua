function Start()
	x = 0
	y = 2
	z = 4
	print("Hi")
end

function Update()
	w, h = GetWindowSize()
	mx, my = GetMousePosition()
	mx = (mx - (w))
	my = (my - (h))

	print(mx, my)

	if GetDownKey("w") then
		z = z - 0.01
	end
	if GetDownKey("s") then
		z = z + 0.01
	end
	if GetDownKey("a") then
		x = x - 0.01
	end
	if GetDownKey("d") then
		x = x + 0.01
	end
	
	SetEntity("Entity", x, y, z, my, mx, 0, 1, 1, 1)
	SetEntity("Camera", x, y, z + 4, 0, 0, 0, 1, 1, 1)
end