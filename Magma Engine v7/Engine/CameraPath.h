#define CAMERA_PATH_INTERVALS 2

typedef struct
{
	vec3 Pos;
	vec3 Dir;
} CameraPathFrame;

typedef struct
{
	bool Record;
	bool Play;
	uint32_t FrameCount;

	uint32_t FramesAllocated;
	uint32_t FrameCountPath;
	uint32_t FrameLast;
	CameraPathFrame* Frames;
} CameraPathInfo;

CameraPathInfo CameraPath = 
{
	false,
	false,
	0,

	0,
	0,
	0,
	NULL
};

void CameraPlayRecordedPath()
{
	if (CameraPath.Record || !CameraPath.Play)
		return;

	CameraPathFrame FrameInter = CameraPath.Frames[CameraPath.FrameLast];

	if (CameraPath.FrameCount % CAMERA_PATH_INTERVALS != 0)
		if (CameraPath.FrameLast > 1 && CameraPath.FrameLast <= CameraPath.FrameCountPath)
		{
			CameraPathFrame* F1 = &CameraPath.Frames[CameraPath.FrameLast - 1];
			CameraPathFrame* F2 = &CameraPath.Frames[CameraPath.FrameLast];

			FrameInter.Pos = Div3(Add3P(&F1->Pos, &F2->Pos), Vec3(2.0, 2.0, 2.0));
			FrameInter.Dir = Div3(Add3P(&F1->Dir, &F2->Dir), Vec3(2.0, 2.0, 2.0));
		}

	CameraPos = FrameInter.Pos;
	CameraDir = FrameInter.Dir;

	if (CameraPath.FrameCount % CAMERA_PATH_INTERVALS == 0)
		CameraPath.FrameLast++;

	if (CameraPath.FrameLast >= CameraPath.FrameCountPath)
		CameraPath.FrameLast = 0;
}

void CameraUpdateRecordingPath()
{
	CameraPath.FrameCount++;

	if (CameraPath.Record)
	{
		CameraPath.Play = false;


		if (CameraPath.FrameCount % CAMERA_PATH_INTERVALS == 0)
		{


			if (CameraPath.FramesAllocated <= CameraPath.FrameCountPath)
			{
				CameraPath.FramesAllocated += 512;

				CameraPathFrame* Frames = NULL;

				if (CameraPath.Frames == NULL)
				{
					CameraPath.Frames = (CameraPathFrame*)malloc(CameraPath.FramesAllocated * sizeof(CameraPathFrame));
				}
				else
				{
					CameraPathFrame* Frames;
					Frames = (CameraPathFrame*)realloc(CameraPath.Frames, CameraPath.FramesAllocated * sizeof(CameraPathFrame));
				}

				if (!Frames || !CameraPath.Frames)
					return;

				CameraPath.Frames = Frames;
			}

			CameraPath.Frames[CameraPath.FrameCountPath].Pos = CameraPos;
			CameraPath.Frames[CameraPath.FrameCountPath].Dir = CameraDir;

			if (CameraPath.FrameCountPath > 1)
			{
				printf("same\n");

				CameraPathFrame* F1 = &CameraPath.Frames[CameraPath.FrameCountPath - 1];
				CameraPathFrame* F2 = &CameraPath.Frames[CameraPath.FrameCountPath];

				if (Compare3(F1->Pos, F2->Pos, 0.0001) &&
					Compare3(F1->Dir, F2->Dir, 0.0001))
				{

					CameraPath.FrameCount -= 4;
				}
				else
				{

					CameraPath.FrameCountPath++;
				}

			}
			else
				CameraPath.FrameCountPath++;


		}
	}
	else
		CameraPlayRecordedPath();
}