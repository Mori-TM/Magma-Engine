bool CheckLua(lua_State* L, int32_t r)
{
	if (r != LUA_OK)
	{
		const char* err = lua_tostring(L, -1);
		printf("Error: %s\n", err);
		return false;
	}
	return true;
}

lua_State* LuaState;

void InitLua()
{
	LuaState = luaL_newstate();
	luaL_openlibs(LuaState);
}

int32_t GetEntity(lua_State* LuaState)
{
	bool Error = true;
	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (strcmp(Entities[i].Name, lua_tostring(LuaState, 1)) == 0)
		{
			lua_pushnumber(LuaState, Entities[i].Translate.x);
			lua_pushnumber(LuaState, Entities[i].Translate.y);
			lua_pushnumber(LuaState, Entities[i].Translate.z);

			lua_pushnumber(LuaState, Entities[i].Rotate.x);
			lua_pushnumber(LuaState, Entities[i].Rotate.y);
			lua_pushnumber(LuaState, Entities[i].Rotate.z);

			lua_pushnumber(LuaState, Entities[i].Scale.x);
			lua_pushnumber(LuaState, Entities[i].Scale.y);
			lua_pushnumber(LuaState, Entities[i].Scale.z);
			Error = false;
		}
	}

	if (Error)
	{
		printf("Error Entity not found: %s\n", lua_tostring(LuaState, 1));
		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);

		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);

		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);
		lua_pushnumber(LuaState, 0.0);
	}

	return 9;
}

int32_t SetEntity(lua_State* LuaState)
{
	Mutex.lock();
	for (uint32_t i = 0; i < EntityCount; i++)
	{
		if (lua_isstring(LuaState, 1))
		if (strcmp(Entities[i].Name, lua_tostring(LuaState, 1)) == 0)
		{
			if (lua_isnumber(LuaState, 2)) Entities[i].Translate.x = lua_tonumber(LuaState, 2);
			if (lua_isnumber(LuaState, 3)) Entities[i].Translate.y = lua_tonumber(LuaState, 3);
			if (lua_isnumber(LuaState, 4)) Entities[i].Translate.z = lua_tonumber(LuaState, 4);
			
			if (lua_isnumber(LuaState, 5)) Entities[i].Rotate.x = lua_tonumber(LuaState, 5);
			if (lua_isnumber(LuaState, 6)) Entities[i].Rotate.y = lua_tonumber(LuaState, 6);
			if (lua_isnumber(LuaState, 7)) Entities[i].Rotate.z = lua_tonumber(LuaState, 7);

			if (lua_isnumber(LuaState, 8)) Entities[i].Scale.x = lua_tonumber(LuaState, 8);
			if (lua_isnumber(LuaState, 9)) Entities[i].Scale.y = lua_tonumber(LuaState, 9);
			if (lua_isnumber(LuaState, 10)) Entities[i].Scale.z = lua_tonumber(LuaState, 10);
		}
	}
	Mutex.unlock();

	return 0;
}

int32_t GetDownKey(lua_State* LuaState)
{
	/*
	const Uint8* State = SDL_GetKeyboardState(NULL);
	int32_t Key = 0;
	for (uint32_t i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		if (State[i])
		{
			Key = SDL_GetKeyFromScancode((SDL_Scancode)i);
			break;
		}
	}
	*/
	lua_pushboolean(LuaState, false);

	bool IsString = lua_isstring(LuaState, 1);
	bool IsNumber = lua_isnumber(LuaState, 1);
	
	if (IsString || IsNumber)
	{
		int32_t Key;
		if (IsString)
			Key = lua_tostring(LuaState, 1)[0];
		if (IsNumber)
			Key = lua_tonumber(LuaState, 1);
		
		const Uint8* State = SDL_GetKeyboardState(NULL);
		if (State[SDL_GetScancodeFromKey(Key)])
			lua_pushboolean(LuaState, true);
	}	

	return 1;
}

int32_t GetMousePosition(lua_State* LuaState)
{
	int32_t MouseX;
	int32_t MouseY;

	uint32_t MouseState = SDL_GetMouseState(&MouseX, &MouseY);
	lua_pushnumber(LuaState, MouseX);
	lua_pushnumber(LuaState, MouseY);
	
	return 2;
}

int32_t GetWindowSize(lua_State* LuaState)
{
	lua_pushnumber(LuaState, SceneWidth);
	lua_pushnumber(LuaState, SceneHeight);

	return 2;
}

bool WindowWasFocused = true;

void LuaReloadFiles()
{
	if (!(SDL_GetWindowFlags(Window) & SDL_WINDOW_INPUT_FOCUS))
		WindowWasFocused = true;
	if (WindowWasFocused && SDL_GetWindowFlags(Window) & SDL_WINDOW_INPUT_FOCUS)
	{
		printf("Focus: %d\n", SDL_GetTicks());
		for (uint32_t i = 0; i < SceneScriptCount; i++)
		{
			OpenVkFile File = OpenVkReadFile(SceneScripts[SelectedScript].Path);
			strcpy(SceneScripts[SelectedScript].Script, File.Data);
		}
		WindowWasFocused = false;
	}
}

void UpdateLuaScripts()
{
	LuaReloadFiles();

	if (GameMode)
	{
		for (uint32_t i = 0; i < SceneScriptCount; i++)
		{
			if (CheckLua(LuaState, luaL_dostring(LuaState, SceneScripts[i].Script)))
			{
				if (LuaOnStart)
				{
					lua_getglobal(LuaState, "Start");
					if (lua_isfunction(LuaState, -1))
						if (CheckLua(LuaState, lua_pcall(LuaState, 0, 0, 0)))
						{

						}
				}

				lua_pushcfunction(LuaState, GetEntity);
				lua_setglobal(LuaState, "GetEntity");

				lua_pushcfunction(LuaState, SetEntity);
				lua_setglobal(LuaState, "SetEntity");

				lua_pushcfunction(LuaState, GetDownKey);
				lua_setglobal(LuaState, "GetDownKey");

				lua_pushcfunction(LuaState, GetMousePosition);
				lua_setglobal(LuaState, "GetMousePosition");

				lua_pushcfunction(LuaState, GetWindowSize);
				lua_setglobal(LuaState, "GetWindowSize");

				lua_getglobal(LuaState, "Update");
				if (lua_isfunction(LuaState, -1))
					if (CheckLua(LuaState, lua_pcall(LuaState, 0, 0, 0)))
					{

					}
			}
		}
		
		LuaOnStart = false;
	}
}

void DeInitLua()
{
	lua_close(LuaState);
}

void AddScript(const char* Path)
{
	strcpy(SceneScripts[SelectedScript].Path, Path);
	OpenVkFile File = OpenVkReadFile(Path);
	strcpy(SceneScripts[SelectedScript].Script, File.Data);
	free(File.Data);
}

void SaveScript(const char* Path)
{
	if (Path != NULL)
		strcpy(SceneScripts[SelectedScript].Path, Path);

	FILE* File = fopen(SceneScripts[SelectedScript].Path, "wb");
	fwrite(SceneScripts[SelectedScript].Script, 1, strlen(SceneScripts[SelectedScript].Script), File);
	fclose(File);
}