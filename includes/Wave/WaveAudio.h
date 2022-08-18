typedef struct
{
	uint32_t Length;
	unsigned char* Buffer;
	SDL_AudioSpec Spectrum;
	SDL_AudioDeviceID ID;
	uint8_t Play;
} WaveAudio;

WaveAudio WaveLoadAudio(const char* Path)
{
	WaveAudio Data;

	SDL_LoadWAV(Path, &Data.Spectrum, &Data.Buffer, &Data.Length);
	Data.ID = SDL_OpenAudioDevice(NULL, 0, &Data.Spectrum, NULL, 0);
	int32_t Success = SDL_QueueAudio(Data.ID, Data.Buffer, Data.Length);

	return Data;
}

void WavePlayAudio(WaveAudio* Data, uint8_t Play)
{
	Data->Play = Play;
	SDL_PauseAudioDevice(Data->ID, !Play);
}

void WaveFreeAudio(WaveAudio* Data)
{
	SDL_CloseAudioDevice(Data->ID);
	SDL_FreeWAV(Data->Buffer);
}