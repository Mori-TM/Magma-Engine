struct
{
	float Yaw;
	float Pitch;

	float DeltaX;
	float DeltaY;

	float MouseDeltaX;
	float MouseDeltaY;

	float DeltaTime;
	float LastTime;

	bool FirstInit;

	float Speed;
} CameraInfo;

void FpsCameraInit()
{
	CameraInfo.Yaw = 270.0;
	CameraInfo.Pitch = 0.0;
	CameraInfo.DeltaX = 0.0;
	CameraInfo.DeltaY = 0.0;
	CameraInfo.MouseDeltaX = 0.0;
	CameraInfo.MouseDeltaY = 0.0;
	CameraInfo.DeltaTime = 0.0;
	CameraInfo.LastTime = 0.0;
	CameraInfo.FirstInit = true;
	CameraInfo.Speed = 0.5;
}

void UpdateFpsCamera(vec3* CameraPos, vec3* CameraFront, vec3* CameraUp)
{
	float CurrentFrame = SDL_GetTicks();
	CameraInfo.DeltaTime = CurrentFrame - CameraInfo.LastTime;
	CameraInfo.LastTime = CurrentFrame;

	CameraInfo.DeltaTime *= 0.6;

	int32_t MouseX;
	int32_t MouseY;

	const Uint8* State = SDL_GetKeyboardState(NULL);
	uint32_t MouseState = SDL_GetMouseState(&MouseX, &MouseY);

	if (State[SDL_SCANCODE_LSHIFT])
		CameraInfo.DeltaTime *= 0.06 * CameraInfo.Speed;
	else if (State[SDL_SCANCODE_LCTRL])
		CameraInfo.DeltaTime *= 0.01 * CameraInfo.Speed;
	else
		CameraInfo.DeltaTime *= 0.03 * CameraInfo.Speed;

	vec3 CamerSpeed = { CameraInfo.DeltaTime, CameraInfo.DeltaTime, CameraInfo.DeltaTime };

	if (MouseState == 4 || CameraInfo.FirstInit)
	{
		CameraInfo.FirstInit = false;
		
		if (State[SDL_SCANCODE_W])
			*CameraPos = Add3(*CameraPos, Mul3P(&CamerSpeed, CameraFront));

		if (State[SDL_SCANCODE_S])
			*CameraPos = Sub3(*CameraPos, Mul3P(&CamerSpeed, CameraFront));

		if (State[SDL_SCANCODE_A])
		{
			*CameraPos = Sub3(*CameraPos, Mul3(CamerSpeed, Normalize3(Cross3P(CameraFront, CameraUp))));
		}

		if (State[SDL_SCANCODE_D])
			*CameraPos = Add3(*CameraPos, Mul3(CamerSpeed, Normalize3(Cross3P(CameraFront, CameraUp))));

		if (CameraInfo.MouseDeltaX != 0.0)
		{
			CameraInfo.DeltaX = (CameraInfo.MouseDeltaX - MouseX);
			CameraInfo.DeltaY = (CameraInfo.MouseDeltaY - MouseY);
		}
		else
			CameraInfo.DeltaX = CameraInfo.DeltaY = 0.0;

		CameraInfo.MouseDeltaX = MouseX;
		CameraInfo.MouseDeltaY = MouseY;

		float Sensitivity = 0.1;

		CameraInfo.Yaw -= (CameraInfo.DeltaX * Sensitivity);
		CameraInfo.Pitch -= (CameraInfo.DeltaY * Sensitivity);

		if (CameraInfo.Pitch >= 89.0)
			CameraInfo.Pitch = 89.0;
		if (CameraInfo.Pitch <= -89.0)
			CameraInfo.Pitch = -89.0;

		float CosPitch = cosf(ToRadians(CameraInfo.Pitch));

		vec3 Direction =
		{
			cosf(ToRadians(CameraInfo.Yaw)) * CosPitch,
			-sinf(ToRadians(CameraInfo.Pitch)),
			sinf(ToRadians(CameraInfo.Yaw)) * CosPitch
		};

		*CameraFront = Normalize3(Direction);
	}
	else
	{
		CameraInfo.MouseDeltaX = 0.0;
		CameraInfo.MouseDeltaY = 0.0;
		CameraInfo.DeltaX = 0.0;
		CameraInfo.DeltaY = 0.0;
	}
}