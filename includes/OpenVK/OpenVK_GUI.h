#define OPENVK_GUI_MAX_CHAR_COUNT 256
#define OPENVK_GUI_BUFFER_ALLOCATE_BLOCK 65536 / 4
#define OPENVK_GUI_ALLOCATE_BLOCK 32

uint32_t(*OpenVkGetMousePos)(int32_t* x, int32_t* y) = NULL;

typedef struct
{
	float Pos[2];
	float UV[2];
	float Color[4];
} OpenVkGUIVertex;

typedef struct
{
	uint32_t Size;
	uint32_t AllocatedSize;
	void* Data;
	uint32_t Buffer;
} OpenVkGUIBuffer;

typedef struct
{
	int32_t x;
	int32_t y;
	uint32_t State;
} OpenVkGUIMouse;

typedef struct
{
	uint32_t x;
	uint32_t y;
	uint32_t sx;
	uint32_t sy;
	uint32_t w;
	uint32_t h;
	uint32_t sw;
	uint32_t sh;
	uint32_t LastX;
	uint32_t LastY;
	uint32_t LastW;
	uint32_t LastH;
	OpenVkBool Selected;
	OpenVkBool ResizeSelected;
} OpenVkGUIWindow;

typedef struct
{
	uint32_t RenderPass;
	uint32_t Pipeline;
	uint32_t PipelineLayout;
	uint32_t DescriptorSetLayout;
	uint32_t DescriptorPool;
	uint32_t Texture;
	uint32_t TextureSampler;
	uint32_t DescriptorSet;
	uint32_t Width; 
	uint32_t Height;
	uint32_t MsaaSamples;

	uint32_t AtlasWidth;
	uint32_t AtlasHeight;
	uint32_t OverSample[2];
	uint32_t CharCount;
	stbtt_packedchar Chars[OPENVK_GUI_MAX_CHAR_COUNT];

	uint32_t LastIndex;
	OpenVkGUIBuffer VertexBuffer;
	OpenVkGUIBuffer IndexBuffer;

	//Settings
	float Size;
	float SizeHalf;
	float SizeThird;
	float SizeQuater;
	float SizeEigth;
	float WindowColor;
	float Color;
	float ColorSelected;
	float ColorHovered;
	float Rounding;
	float ToolSize;
	float ToolSizeHalf;
	uint32_t Border;
	uint32_t Offset;

	OpenVkBool MousePressed;
	OpenVkGUIMouse LastMouse;
	OpenVkGUIMouse LastPressedMouse;
	OpenVkGUIMouse CurMouse;

	//Test Items
	int32_t sx;
	int32_t sy;
	OpenVkBool MouseItemCollision;

	uint32_t EntityCount;
	uint32_t SelectedEntity;

	uint32_t WindowCount;
	uint32_t WindowSize;
	OpenVkGUIWindow* Windows;
	//Current window where rendering happens
	OpenVkGUIWindow* Window;
} OpenVkGUIInfo;

OpenVkGUIInfo OpenVkGUI;

void OpenVkGUIResizeBuffer(OpenVkGUIBuffer* Buffer, OpenVkBool VertexBuffer)
{
	if (Buffer->Size >= Buffer->AllocatedSize)
	{
		while (Buffer->Size >= Buffer->AllocatedSize)
			Buffer->AllocatedSize += OPENVK_GUI_BUFFER_ALLOCATE_BLOCK;

		if (Buffer->Buffer != OPENVK_ERROR)
			OpenVkDestroyDynamicBuffer(Buffer->Buffer);

		if (VertexBuffer)
		{
			Buffer->Buffer = OpenVkCreateDynamicVertexBuffer(Buffer->AllocatedSize * sizeof(OpenVkGUIVertex));
			Buffer->Data = realloc(Buffer->Data, Buffer->AllocatedSize * sizeof(OpenVkGUIVertex));
			OpenVkRuntimeInfo("Vertex Resize", "");
		}
			
		else
		{
			Buffer->Buffer = OpenVkCreateDynamicIndexBuffer(Buffer->AllocatedSize * sizeof(uint32_t));
			Buffer->Data = realloc(Buffer->Data, Buffer->AllocatedSize * sizeof(uint32_t));
			OpenVkRuntimeInfo("Index Resize", "");
		}
			
	}
		


}

typedef struct
{
	OpenVkGUIVertex Vertices[4];
	float Offset[2];
} OpenVkGUIGlyph;

OpenVkGUIGlyph OpenVkGUIMakeGlyph(uint32_t Character, float OffsetX, float OffsetY)
{
	stbtt_aligned_quad Quad;

	stbtt_GetPackedQuad(OpenVkGUI.Chars, OpenVkGUI.AtlasWidth, OpenVkGUI.AtlasHeight, Character, &OffsetX, &OffsetY, &Quad, 1);
	
	OpenVkGUIGlyph Info;
	Info.Offset[0] = OffsetX;
	Info.Offset[1] = OffsetY;

	Info.Vertices[0].Pos[0] = Quad.x0;
	Info.Vertices[0].Pos[1] = Quad.y1;
	Info.Vertices[0].UV[0] = Quad.s0;
	Info.Vertices[0].UV[1] = Quad.t1;
	Info.Vertices[0].Color[0] = 1.0;
	Info.Vertices[0].Color[1] = 1.0;
	Info.Vertices[0].Color[2] = 1.0;
	Info.Vertices[0].Color[3] = 1.0;
	
	Info.Vertices[1].Pos[0] = Quad.x0;
	Info.Vertices[1].Pos[1] = Quad.y0;
	Info.Vertices[1].UV[0] = Quad.s0;
	Info.Vertices[1].UV[1] = Quad.t0;
	Info.Vertices[1].Color[0] = 1.0;
	Info.Vertices[1].Color[1] = 1.0;
	Info.Vertices[1].Color[2] = 1.0;
	Info.Vertices[1].Color[3] = 1.0;

	Info.Vertices[2].Pos[0] = Quad.x1;
	Info.Vertices[2].Pos[1] = Quad.y0;
	Info.Vertices[2].UV[0] = Quad.s1;
	Info.Vertices[2].UV[1] = Quad.t0;
	Info.Vertices[2].Color[0] = 1.0;
	Info.Vertices[2].Color[1] = 1.0;
	Info.Vertices[2].Color[2] = 1.0;
	Info.Vertices[2].Color[3] = 1.0;
	
	Info.Vertices[3].Pos[0] = Quad.x1;
	Info.Vertices[3].Pos[1] = Quad.y1;
	Info.Vertices[3].UV[0] = Quad.s1;
	Info.Vertices[3].UV[1] = Quad.t1;
	Info.Vertices[3].Color[0] = 1.0;
	Info.Vertices[3].Color[1] = 1.0;
	Info.Vertices[3].Color[2] = 1.0;
	Info.Vertices[3].Color[3] = 1.0;

	return Info;
}

uint32_t OpenVkGUIGetTextWidth(const char* Text)
{
	float OffsetX = 0;
	float OffsetY = 0;

	stbtt_aligned_quad Quad;

	for (uint32_t i = 0; i < strlen(Text); i++)
		stbtt_GetPackedQuad(OpenVkGUI.Chars, OpenVkGUI.AtlasWidth, OpenVkGUI.AtlasHeight, Text[i], &OffsetX, &OffsetY, &Quad, 1);

	return OffsetX;
}

OpenVkGUIVertex Verts[] =
{
	{ {0.5f,  0.5f}, {1.0, 1.0}, {1.0, 0.0, 0.0, 1.0} },
	{ {0.5f, -0.5f}, {1.0, 0.0}, {0.0, 1.0, 0.0, 1.0} },
	{ {-0.5f, -0.5f}, {0.0, 0.0}, {0.0, 0.0, 1.0, 1.0} },
	{ {-0.5f,  0.5f}, {0.0, 1.0}, {1.0, 0.0, 1.0, 1.0} },
};

uint32_t Indi[] =
{
	0, 1, 2,
	0, 2, 3
};

const uint32_t OpenVkGUIFragmentShader[] =
{	
	// 1011.6.0
	0x07230203,0x00010000,0x0008000a,0x00000094,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000021,0x00000024,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00090005,0x0000000e,0x6e756f52,0x42646564,0x4453786f,0x66762846,0x66763b32,
	0x31663b32,0x0000003b,0x00060005,0x0000000b,0x746e6543,0x6f507265,0x69746973,0x00006e6f,
	0x00040005,0x0000000c,0x657a6953,0x00000000,0x00040005,0x0000000d,0x69646152,0x00007375,
	0x00050005,0x00000021,0x4374754f,0x726f6c6f,0x00000000,0x00030005,0x00000022,0x00000000,
	0x00040006,0x00000022,0x00000000,0x00005655,0x00050006,0x00000022,0x00000001,0x6f6c6f43,
	0x00000072,0x00030005,0x00000024,0x00006e49,0x00050005,0x0000003d,0x74736944,0x65636e61,
	0x00000000,0x00040005,0x00000041,0x706d6153,0x0072656c,0x00040005,0x00000048,0x68706c41,
	0x00000061,0x00030005,0x00000068,0x00005655,0x00060005,0x0000006e,0x65676445,0x74666f53,
	0x7373656e,0x00000000,0x00040005,0x0000006f,0x69646152,0x00007375,0x00040005,0x00000072,
	0x657a6953,0x00000000,0x00050005,0x00000076,0x74736944,0x65636e61,0x00000000,0x00040005,
	0x0000007a,0x61726170,0x0000006d,0x00040005,0x0000007b,0x61726170,0x0000006d,0x00040005,
	0x0000007d,0x61726170,0x0000006d,0x00060005,0x00000080,0x6f6f6d53,0x64656874,0x68706c41,
	0x00000061,0x00040047,0x00000021,0x0000001e,0x00000000,0x00040047,0x00000024,0x0000001e,
	0x00000000,0x00040047,0x00000041,0x00000022,0x00000000,0x00040047,0x00000041,0x00000021,
	0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000002,0x00040020,0x00000008,0x00000007,
	0x00000007,0x00040020,0x00000009,0x00000007,0x00000006,0x00060021,0x0000000a,0x00000006,
	0x00000008,0x00000008,0x00000009,0x0004002b,0x00000006,0x00000017,0x00000000,0x00040017,
	0x0000001f,0x00000006,0x00000004,0x00040020,0x00000020,0x00000003,0x0000001f,0x0004003b,
	0x00000020,0x00000021,0x00000003,0x0004001e,0x00000022,0x00000007,0x0000001f,0x00040020,
	0x00000023,0x00000001,0x00000022,0x0004003b,0x00000023,0x00000024,0x00000001,0x00040015,
	0x00000025,0x00000020,0x00000001,0x0004002b,0x00000025,0x00000026,0x00000001,0x00040020,
	0x00000027,0x00000001,0x0000001f,0x00020014,0x0000002a,0x0004002b,0x00000025,0x0000002b,
	0x00000000,0x00040015,0x0000002c,0x00000020,0x00000000,0x0004002b,0x0000002c,0x0000002d,
	0x00000000,0x00040020,0x0000002e,0x00000001,0x00000006,0x0004002b,0x00000006,0x00000031,
	0x43fa0000,0x0004002b,0x0000002c,0x00000036,0x00000001,0x00090019,0x0000003e,0x00000006,
	0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x0003001b,0x0000003f,
	0x0000003e,0x00040020,0x00000040,0x00000000,0x0000003f,0x0004003b,0x00000040,0x00000041,
	0x00000000,0x00040020,0x00000043,0x00000001,0x00000007,0x0004002b,0x00000006,0x00000049,
	0x3f800000,0x0004002b,0x0000002c,0x0000004d,0x00000003,0x00040020,0x0000004e,0x00000003,
	0x00000006,0x0004002b,0x0000002c,0x00000051,0x00000002,0x00040017,0x00000059,0x00000006,
	0x00000003,0x0004002b,0x00000006,0x0000005b,0x3dcccccd,0x0004002b,0x00000006,0x0000005c,
	0x3ecccccd,0x0004002b,0x00000006,0x0000005d,0x3f400000,0x0006002c,0x00000059,0x0000005e,
	0x0000005b,0x0000005c,0x0000005d,0x0004002b,0x00000006,0x0000006b,0x447a0000,0x0005002c,
	0x00000007,0x0000006c,0x0000006b,0x0000006b,0x0004002b,0x00000006,0x00000082,0x40000000,
	0x0004002b,0x00000006,0x00000093,0x3f000000,0x00050036,0x00000002,0x00000004,0x00000000,
	0x00000003,0x000200f8,0x00000005,0x0004003b,0x00000009,0x0000003d,0x00000007,0x0004003b,
	0x00000009,0x00000048,0x00000007,0x0004003b,0x00000008,0x00000068,0x00000007,0x0004003b,
	0x00000009,0x0000006e,0x00000007,0x0004003b,0x00000009,0x0000006f,0x00000007,0x0004003b,
	0x00000008,0x00000072,0x00000007,0x0004003b,0x00000009,0x00000076,0x00000007,0x0004003b,
	0x00000008,0x0000007a,0x00000007,0x0004003b,0x00000008,0x0000007b,0x00000007,0x0004003b,
	0x00000009,0x0000007d,0x00000007,0x0004003b,0x00000009,0x00000080,0x00000007,0x00050041,
	0x00000027,0x00000028,0x00000024,0x00000026,0x0004003d,0x0000001f,0x00000029,0x00000028,
	0x0003003e,0x00000021,0x00000029,0x00060041,0x0000002e,0x0000002f,0x00000024,0x0000002b,
	0x0000002d,0x0004003d,0x00000006,0x00000030,0x0000002f,0x000500b8,0x0000002a,0x00000032,
	0x00000030,0x00000031,0x000400a8,0x0000002a,0x00000033,0x00000032,0x000300f7,0x00000035,
	0x00000000,0x000400fa,0x00000033,0x00000034,0x00000035,0x000200f8,0x00000034,0x00060041,
	0x0000002e,0x00000037,0x00000024,0x0000002b,0x00000036,0x0004003d,0x00000006,0x00000038,
	0x00000037,0x000500b8,0x0000002a,0x00000039,0x00000038,0x00000031,0x000200f9,0x00000035,
	0x000200f8,0x00000035,0x000700f5,0x0000002a,0x0000003a,0x00000032,0x00000005,0x00000039,
	0x00000034,0x000300f7,0x0000003c,0x00000000,0x000400fa,0x0000003a,0x0000003b,0x00000050,
	0x000200f8,0x0000003b,0x0004003d,0x0000003f,0x00000042,0x00000041,0x00050041,0x00000043,
	0x00000044,0x00000024,0x0000002b,0x0004003d,0x00000007,0x00000045,0x00000044,0x00050057,
	0x0000001f,0x00000046,0x00000042,0x00000045,0x00050051,0x00000006,0x00000047,0x00000046,
	0x00000000,0x0003003e,0x0000003d,0x00000047,0x0004003d,0x00000006,0x0000004a,0x0000003d,
	0x0008000c,0x00000006,0x0000004b,0x00000001,0x00000031,0x00000017,0x00000049,0x0000004a,
	0x0003003e,0x00000048,0x0000004b,0x0004003d,0x00000006,0x0000004c,0x00000048,0x00050041,
	0x0000004e,0x0000004f,0x00000021,0x0000004d,0x0003003e,0x0000004f,0x0000004c,0x000200f9,
	0x0000003c,0x000200f8,0x00000050,0x00060041,0x0000002e,0x00000052,0x00000024,0x00000026,
	0x00000051,0x0004003d,0x00000006,0x00000053,0x00000052,0x000500b4,0x0000002a,0x00000054,
	0x00000053,0x00000017,0x000300f7,0x00000056,0x00000000,0x000400fa,0x00000054,0x00000055,
	0x00000067,0x000200f8,0x00000055,0x00060041,0x0000002e,0x00000057,0x00000024,0x00000026,
	0x0000004d,0x0004003d,0x00000006,0x00000058,0x00000057,0x00060050,0x00000059,0x0000005a,
	0x00000058,0x00000058,0x00000058,0x00050085,0x00000059,0x0000005f,0x0000005a,0x0000005e,
	0x00050041,0x0000004e,0x00000060,0x00000021,0x0000002d,0x00050051,0x00000006,0x00000061,
	0x0000005f,0x00000000,0x0003003e,0x00000060,0x00000061,0x00050041,0x0000004e,0x00000062,
	0x00000021,0x00000036,0x00050051,0x00000006,0x00000063,0x0000005f,0x00000001,0x0003003e,
	0x00000062,0x00000063,0x00050041,0x0000004e,0x00000064,0x00000021,0x00000051,0x00050051,
	0x00000006,0x00000065,0x0000005f,0x00000002,0x0003003e,0x00000064,0x00000065,0x00050041,
	0x0000004e,0x00000066,0x00000021,0x0000004d,0x0003003e,0x00000066,0x00000049,0x000200f9,
	0x00000056,0x000200f8,0x00000067,0x00050041,0x00000043,0x00000069,0x00000024,0x0000002b,
	0x0004003d,0x00000007,0x0000006a,0x00000069,0x00050083,0x00000007,0x0000006d,0x0000006a,
	0x0000006c,0x0003003e,0x00000068,0x0000006d,0x0003003e,0x0000006e,0x00000049,0x00060041,
	0x0000002e,0x00000070,0x00000024,0x00000026,0x00000051,0x0004003d,0x00000006,0x00000071,
	0x00000070,0x0003003e,0x0000006f,0x00000071,0x00050041,0x00000027,0x00000073,0x00000024,
	0x00000026,0x0004003d,0x0000001f,0x00000074,0x00000073,0x0007004f,0x00000007,0x00000075,
	0x00000074,0x00000074,0x00000000,0x00000001,0x0003003e,0x00000072,0x00000075,0x0004003d,
	0x00000007,0x00000077,0x00000068,0x0004003d,0x00000007,0x00000078,0x00000072,0x00050085,
	0x00000007,0x00000079,0x00000077,0x00000078,0x0003003e,0x0000007a,0x00000079,0x0004003d,
	0x00000007,0x0000007c,0x00000072,0x0003003e,0x0000007b,0x0000007c,0x0004003d,0x00000006,
	0x0000007e,0x0000006f,0x0003003e,0x0000007d,0x0000007e,0x00070039,0x00000006,0x0000007f,
	0x0000000e,0x0000007a,0x0000007b,0x0000007d,0x0003003e,0x00000076,0x0000007f,0x0004003d,
	0x00000006,0x00000081,0x0000006e,0x00050085,0x00000006,0x00000083,0x00000081,0x00000082,
	0x0004003d,0x00000006,0x00000084,0x00000076,0x0008000c,0x00000006,0x00000085,0x00000001,
	0x00000031,0x00000017,0x00000083,0x00000084,0x0003003e,0x00000080,0x00000085,0x00060041,
	0x0000002e,0x00000086,0x00000024,0x00000026,0x0000004d,0x0004003d,0x00000006,0x00000087,
	0x00000086,0x00060050,0x00000059,0x00000088,0x00000087,0x00000087,0x00000087,0x00050085,
	0x00000059,0x00000089,0x00000088,0x0000005e,0x00050041,0x0000004e,0x0000008a,0x00000021,
	0x0000002d,0x00050051,0x00000006,0x0000008b,0x00000089,0x00000000,0x0003003e,0x0000008a,
	0x0000008b,0x00050041,0x0000004e,0x0000008c,0x00000021,0x00000036,0x00050051,0x00000006,
	0x0000008d,0x00000089,0x00000001,0x0003003e,0x0000008c,0x0000008d,0x00050041,0x0000004e,
	0x0000008e,0x00000021,0x00000051,0x00050051,0x00000006,0x0000008f,0x00000089,0x00000002,
	0x0003003e,0x0000008e,0x0000008f,0x0004003d,0x00000006,0x00000090,0x00000080,0x00050083,
	0x00000006,0x00000091,0x00000049,0x00000090,0x00050041,0x0000004e,0x00000092,0x00000021,
	0x0000004d,0x0003003e,0x00000092,0x00000091,0x000200f9,0x00000056,0x000200f8,0x00000056,
	0x000200f9,0x0000003c,0x000200f8,0x0000003c,0x000100fd,0x00010038,0x00050036,0x00000006,
	0x0000000e,0x00000000,0x0000000a,0x00030037,0x00000008,0x0000000b,0x00030037,0x00000008,
	0x0000000c,0x00030037,0x00000009,0x0000000d,0x000200f8,0x0000000f,0x0004003d,0x00000007,
	0x00000010,0x0000000b,0x0006000c,0x00000007,0x00000011,0x00000001,0x00000004,0x00000010,
	0x0004003d,0x00000007,0x00000012,0x0000000c,0x00050083,0x00000007,0x00000013,0x00000011,
	0x00000012,0x0004003d,0x00000006,0x00000014,0x0000000d,0x00050050,0x00000007,0x00000015,
	0x00000014,0x00000014,0x00050081,0x00000007,0x00000016,0x00000013,0x00000015,0x00050050,
	0x00000007,0x00000018,0x00000017,0x00000017,0x0007000c,0x00000007,0x00000019,0x00000001,
	0x00000028,0x00000016,0x00000018,0x0006000c,0x00000006,0x0000001a,0x00000001,0x00000042,
	0x00000019,0x0004003d,0x00000006,0x0000001b,0x0000000d,0x00050083,0x00000006,0x0000001c,
	0x0000001a,0x0000001b,0x000200fe,0x0000001c,0x00010038

};

const uint32_t OpenVkGUIVertexShader[] =
{
	// 1011.6.0
0x07230203,0x00010000,0x0008000a,0x00000024,0x00000000,0x00020011,0x00000001,0x0006000b,
0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000a,0x0000000f,0x0000001a,
0x0000001b,0x00000021,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
0x00000000,0x00060005,0x00000008,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,
0x00000008,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000000a,0x00000000,
0x00040005,0x0000000f,0x6f506e49,0x00000073,0x00030005,0x00000018,0x00000000,0x00040006,
0x00000018,0x00000000,0x00005655,0x00050006,0x00000018,0x00000001,0x6f6c6f43,0x00000072,
0x00030005,0x0000001a,0x0074754f,0x00040005,0x0000001b,0x56556e49,0x00000000,0x00040005,
0x00000021,0x6f436e49,0x00726f6c,0x00050048,0x00000008,0x00000000,0x0000000b,0x00000000,
0x00030047,0x00000008,0x00000002,0x00040047,0x0000000f,0x0000001e,0x00000000,0x00040047,
0x0000001a,0x0000001e,0x00000000,0x00040047,0x0000001b,0x0000001e,0x00000001,0x00040047,
0x00000021,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x0003001e,
0x00000008,0x00000007,0x00040020,0x00000009,0x00000003,0x00000008,0x0004003b,0x00000009,
0x0000000a,0x00000003,0x00040015,0x0000000b,0x00000020,0x00000001,0x0004002b,0x0000000b,
0x0000000c,0x00000000,0x00040017,0x0000000d,0x00000006,0x00000002,0x00040020,0x0000000e,
0x00000001,0x0000000d,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x0004002b,0x00000006,
0x00000011,0x00000000,0x0004002b,0x00000006,0x00000012,0x3f800000,0x00040020,0x00000016,
0x00000003,0x00000007,0x0004001e,0x00000018,0x0000000d,0x00000007,0x00040020,0x00000019,
0x00000003,0x00000018,0x0004003b,0x00000019,0x0000001a,0x00000003,0x0004003b,0x0000000e,
0x0000001b,0x00000001,0x00040020,0x0000001d,0x00000003,0x0000000d,0x0004002b,0x0000000b,
0x0000001f,0x00000001,0x00040020,0x00000020,0x00000001,0x00000007,0x0004003b,0x00000020,
0x00000021,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
0x00000005,0x0004003d,0x0000000d,0x00000010,0x0000000f,0x00050051,0x00000006,0x00000013,
0x00000010,0x00000000,0x00050051,0x00000006,0x00000014,0x00000010,0x00000001,0x00070050,
0x00000007,0x00000015,0x00000013,0x00000014,0x00000011,0x00000012,0x00050041,0x00000016,
0x00000017,0x0000000a,0x0000000c,0x0003003e,0x00000017,0x00000015,0x0004003d,0x0000000d,
0x0000001c,0x0000001b,0x00050041,0x0000001d,0x0000001e,0x0000001a,0x0000000c,0x0003003e,
0x0000001e,0x0000001c,0x0004003d,0x00000007,0x00000022,0x00000021,0x00050041,0x00000016,
0x00000023,0x0000001a,0x0000001f,0x0003003e,0x00000023,0x00000022,0x000100fd,0x00010038

};

OpenVkGUIWindow OpenVkGUIWindowInit(uint32_t i)
{
	OpenVkGUIWindow Win;
//	Win.x = 10;
//	Win.y = 10;
//	Win.w = 60;
//	Win.h = 60;
	Win.x = 560 + (i * 40);
	Win.y = 100 + (i * 40);
	Win.sx = 60;
	Win.sy = 40;
	Win.w = 560;
	Win.h = 650;
	Win.sw = 560;
	Win.sh = 600;
	Win.LastX = 0;
	Win.LastY = 0;
	Win.LastW = 0;
	Win.LastH = 0;
	Win.Selected = OpenVkFalse;
	Win.ResizeSelected = OpenVkFalse;
	return Win;
}

void OpenVkGUIInit(uint32_t Width, uint32_t Height, uint32_t RenderPass, uint32_t MsaaSamples, float FontSize, const char* Font, uint32_t (*GetMousePos)(int32_t* x, int32_t* y))
{
	OpenVkGetMousePos = GetMousePos;

	OpenVkGUI.RenderPass = RenderPass;
	OpenVkGUI.Width = Width;
	OpenVkGUI.Height = Height;
	OpenVkGUI.MsaaSamples = MsaaSamples;

	OpenVkGUI.Size = FontSize;
	OpenVkGUI.SizeHalf = FontSize / 2;
	OpenVkGUI.SizeThird = FontSize / 3;
	OpenVkGUI.SizeQuater = FontSize / 4;
	OpenVkGUI.SizeEigth = FontSize / 8;
	OpenVkGUI.AtlasWidth = 1024;
	OpenVkGUI.AtlasHeight = 1024;
	OpenVkGUI.OverSample[0] = 2;
	OpenVkGUI.OverSample[1] = 2;
	OpenVkGUI.CharCount = 256;

	OpenVkGUI.Rounding = 19.0;
	OpenVkGUI.ToolSize = OpenVkGUI.Size + OpenVkGUI.SizeQuater;
	OpenVkGUI.ToolSizeHalf = OpenVkGUI.ToolSize / 2;
	OpenVkGUI.Border = 10.0;
	OpenVkGUI.Offset = OpenVkGUI.SizeQuater;
	OpenVkGUI.Color = 0.55;
	OpenVkGUI.ColorSelected = 0.7;
	OpenVkGUI.ColorHovered = 0.6;
	OpenVkGUI.WindowColor = 0.1;

	OpenVkGUI.WindowCount = 0;
	OpenVkGUI.WindowSize = OPENVK_GUI_ALLOCATE_BLOCK;
	OpenVkGUI.Windows = NULL;
	OpenVkGUI.Window = NULL;
	
	OpenVkGUI.MouseItemCollision = OpenVkFalse;
	OpenVkGUI.sx = 0;
	OpenVkGUI.sy = 0;
	OpenVkGUI.SelectedEntity = 0;
	OpenVkGUI.EntityCount = 1;

	uint32_t DescriptorTypes[] = { OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER };
	uint32_t ShaderTypes[] = { OPENVK_SHADER_TYPE_FRAGMENT };
	uint32_t DescriptorCounts[] = { 1 };
	uint32_t Bindings[] = { 0 };

	OpenVkGUI.DescriptorSetLayout = OpenVkCreateDescriptorSetLayout(1, Bindings, DescriptorCounts, DescriptorTypes, NULL, ShaderTypes);
	OpenVkGUI.DescriptorPool = OpenVkCreateDescriptorPool(OPENVK_DESCRIPTOR_POOL_DEFAULT, 1, DescriptorTypes, DescriptorCounts);


		OpenVkFile File = OpenVkReadFile(Font);
	//	stbtt_fontinfo info;
	//	if (!stbtt_InitFont(&info, (unsigned char*)File.Data, 0))
	//	{
	//		printf("failed\n");
	//	}

		/* create a bitmap for the phrase */
		unsigned char* Bitmap = (unsigned char*)calloc(OpenVkGUI.AtlasWidth * OpenVkGUI.AtlasHeight, sizeof(unsigned char));

	//	font_.charInfo = std::make_unique<stbtt_packedchar[]>(font_.charCount);

		stbtt_pack_context context;
		if (!stbtt_PackBegin(&context, Bitmap, OpenVkGUI.AtlasWidth, OpenVkGUI.AtlasHeight, 0, 1, NULL))
			printf("Failed to initialize font\n");

		stbtt_PackSetOversampling(&context, OpenVkGUI.OverSample[0], OpenVkGUI.OverSample[1]);
		if (!stbtt_PackFontRange(&context, (unsigned char*)File.Data, 0, OpenVkGUI.Size, 0, OPENVK_GUI_MAX_CHAR_COUNT, OpenVkGUI.Chars))
			printf("Failed to pack font\n");

		stbtt_PackEnd(&context);

	OpenVkGUI.Texture = OpenVkCreateTextureImage(Bitmap, OpenVkGUI.AtlasWidth, OpenVkGUI.AtlasHeight, OPENVK_FORMAT_R);
	OpenVkGUI.TextureSampler = OpenVkCreateImageSampler(OPENVK_FILTER_LINEAR, OPENVK_ADDRESS_MODE_REPEAT);
	OpenVkFree(Bitmap);

	uint32_t ImageTypes[] = { OPENVK_IMAGE_TYPE_TEXTURE };
	uint32_t ImageLayouts[] = { OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT };

	OpenVkDescriptorSetCreateInfo DescriptorSetCreateInfo;
	DescriptorSetCreateInfo.DescriptorSetLayout = OpenVkGUI.DescriptorSetLayout;
	DescriptorSetCreateInfo.DescriptorPool = OpenVkGUI.DescriptorPool;
	DescriptorSetCreateInfo.DescriptorWriteCount = 1;
	DescriptorSetCreateInfo.DescriptorCounts = DescriptorCounts;
	DescriptorSetCreateInfo.DescriptorTypes = DescriptorTypes;
	DescriptorSetCreateInfo.Sampler = &OpenVkGUI.TextureSampler;
	DescriptorSetCreateInfo.ImageTypes = ImageTypes;
	DescriptorSetCreateInfo.ImageLayouts = ImageLayouts;
	DescriptorSetCreateInfo.Bindings = Bindings;
	DescriptorSetCreateInfo.Images = &OpenVkGUI.Texture;
	DescriptorSetCreateInfo.DescriptorSet = NULL;
	DescriptorSetCreateInfo.VariableDescriptorSetCount = 0;
	OpenVkGUI.DescriptorSet = OpenVkCreateDescriptorSet(&DescriptorSetCreateInfo);

	OpenVkPipelineLayoutCreateInfo Layout;
	Layout.PushConstantCount = 0;
	Layout.PushConstantShaderTypes = NULL;
	Layout.PushConstantOffsets = NULL;
	Layout.PushConstantSizes = NULL;
	Layout.DescriptorSetLayoutCount = 1;
	Layout.DescriptorSetLayouts = &OpenVkGUI.DescriptorSetLayout;
	OpenVkGUI.PipelineLayout = OpenVkCreatePipelineLayout(&Layout);

	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RGBA32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 8, 16 };

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
//	GraphicsPipelineCreateInfo.VertexShader.Size = sizeof(OpenVkGUIVertexShader);
//	GraphicsPipelineCreateInfo.VertexShader.Data = (char*)OpenVkGUIVertexShader;
//	GraphicsPipelineCreateInfo.VertexShader.Freeable = OpenVkFalse;
//	GraphicsPipelineCreateInfo.FragmentShader.Size = sizeof(OpenVkGUIFragmentShader);
//	GraphicsPipelineCreateInfo.FragmentShader.Data = (char*)OpenVkGUIFragmentShader;
//	GraphicsPipelineCreateInfo.FragmentShader.Freeable = OpenVkFalse;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/GUIVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/GUIFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = sizeof(OpenVkGUIVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 3;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = Width;
	GraphicsPipelineCreateInfo.Height = Height;
	GraphicsPipelineCreateInfo.DepthClamp = OpenVkFalse;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 1.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = MsaaSamples;
	GraphicsPipelineCreateInfo.AlphaBlending = OpenVkTrue;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PipelineLayout = OpenVkGUI.PipelineLayout;
	GraphicsPipelineCreateInfo.DepthStencil = OpenVkFalse;
	GraphicsPipelineCreateInfo.RenderPass = RenderPass;
	OpenVkGUI.Pipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);

	memset(&OpenVkGUI.VertexBuffer, 0, sizeof(OpenVkGUIBuffer));
	memset(&OpenVkGUI.IndexBuffer, 0, sizeof(OpenVkGUIBuffer));
	OpenVkGUI.VertexBuffer.Buffer = OPENVK_ERROR;
	OpenVkGUI.IndexBuffer.Buffer = OPENVK_ERROR;

	OpenVkGUI.Windows = (OpenVkGUIWindow*)malloc(OpenVkGUI.WindowSize * sizeof(OpenVkGUIWindow));
	for (uint32_t i = 0; i < OpenVkGUI.WindowSize; i++)
		OpenVkGUI.Windows[i] = OpenVkGUIWindowInit(i);

//	OpenVkGUI.VertexBuffer = OpenVkCreateDynamicVertexBuffer(4 * sizeof(OpenVkGUIVertex));
//	OpenVkGUI.IndexBuffer = OpenVkCreateDynamicIndexBuffer(6 * sizeof(uint32_t));
}

void OpenVkGUIRecreatePipeline()
{
	uint32_t ShaderAttributeFormats[] = { OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RG32F, OPENVK_FORMAT_RGBA32F };
	uint32_t ShaderAttributeOffsets[] = { 0, 8, 16 };

	OpenVkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
	//	GraphicsPipelineCreateInfo.VertexShader.Size = sizeof(OpenVkGUIVertexShader);
	//	GraphicsPipelineCreateInfo.VertexShader.Data = (char*)OpenVkGUIVertexShader;
	//	GraphicsPipelineCreateInfo.VertexShader.Freeable = OpenVkFalse;
	//	GraphicsPipelineCreateInfo.FragmentShader.Size = sizeof(OpenVkGUIFragmentShader);
	//	GraphicsPipelineCreateInfo.FragmentShader.Data = (char*)OpenVkGUIFragmentShader;
	//	GraphicsPipelineCreateInfo.FragmentShader.Freeable = OpenVkFalse;
	GraphicsPipelineCreateInfo.VertexShader = OpenVkReadFile("Data/Shader/GUIVertex.spv");
	GraphicsPipelineCreateInfo.FragmentShader = OpenVkReadFile("Data/Shader/GUIFragment.spv");
	GraphicsPipelineCreateInfo.BindingStride = sizeof(OpenVkGUIVertex);
	GraphicsPipelineCreateInfo.ShaderAttributeFormatCount = 3;
	GraphicsPipelineCreateInfo.ShaderAttributeFormats = ShaderAttributeFormats;
	GraphicsPipelineCreateInfo.ShaderAttributeOffsets = ShaderAttributeOffsets;
	GraphicsPipelineCreateInfo.PrimitiveTopology = OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE;
	GraphicsPipelineCreateInfo.x = 0;
	GraphicsPipelineCreateInfo.y = 0;
	GraphicsPipelineCreateInfo.Width = OpenVkGUI.Width;
	GraphicsPipelineCreateInfo.Height = OpenVkGUI.Height;
	GraphicsPipelineCreateInfo.DepthClamp = OpenVkFalse;
	GraphicsPipelineCreateInfo.PolygonMode = OPENVK_POLYGON_MODE_FILL;
	GraphicsPipelineCreateInfo.LineWidth = 1.0;
	GraphicsPipelineCreateInfo.CullMode = OPENVK_CULL_MODE_NONE;
	GraphicsPipelineCreateInfo.FrontFace = OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE;
	GraphicsPipelineCreateInfo.MsaaSamples = OpenVkGUI.MsaaSamples;
	GraphicsPipelineCreateInfo.AlphaBlending = OpenVkTrue;
	GraphicsPipelineCreateInfo.ColorBlendAttachments = 1;
	GraphicsPipelineCreateInfo.PipelineLayout = OpenVkGUI.PipelineLayout;
	GraphicsPipelineCreateInfo.DepthStencil = OpenVkFalse;
	GraphicsPipelineCreateInfo.RenderPass = OpenVkGUI.RenderPass;
	OpenVkGUI.Pipeline = OpenVkCreateGraphicsPipeline(&GraphicsPipelineCreateInfo);
}

void OpenVkGUIDestroy()
{
	
}

void OpenVkGUIBeginRender(uint32_t Width, uint32_t Height)
{
	/*
	OpenVkGUIGlyph Glyph;
	uint32_t Index = (float)SDL_GetTicks() * 0.01;
	Index %= 256;
	printf("%d\n", Index);
	Glyph = OpenVkGUIMakeGlyph((uint32_t)254, 0, 0);

	mat4 Proj = OrthoMat4(0.0, Width, Height, 0.0, -1.0, 1.0);
	for (uint32_t i = 0; i < 4; i++)
	{
		vec4 Pos = 
		{
			Glyph.Vertices[i].Pos[0] * 2,
			Glyph.Vertices[i].Pos[1] * 2 + 80,
			0.0, 
			1.0
		};

		vec4 P = MultiplyVec4Mat4P(&Pos, &Proj);
	//	PrintVec4(&P);
		Glyph.Vertices[i].Pos[0] = P.x;
		Glyph.Vertices[i].Pos[1] = P.y;
	}

	OpenVkUpdateDynamicBuffer(4 * sizeof(OpenVkGUIVertex), Glyph.Vertices, OpenVkGUI.VertexBuffer);
	OpenVkUpdateDynamicBuffer(6 * sizeof(uint32_t), Indi, OpenVkGUI.IndexBuffer);
	*/

	OpenVkGUI.LastMouse = OpenVkGUI.CurMouse;
	
	OpenVkGUI.CurMouse.State = OpenVkGetMousePos(&OpenVkGUI.CurMouse.x, &OpenVkGUI.CurMouse.y);
	if (OpenVkGUI.CurMouse.State == 1 && OpenVkGUI.LastMouse.State != 1)
	{
		OpenVkGUI.LastPressedMouse = OpenVkGUI.CurMouse;
		OpenVkGUI.MousePressed = OpenVkTrue;
	}
	else
		OpenVkGUI.MousePressed = OpenVkFalse;

	OpenVkGUIResizeBuffer(&OpenVkGUI.VertexBuffer, OpenVkTrue);
	OpenVkGUIResizeBuffer(&OpenVkGUI.IndexBuffer, OpenVkFalse);
	OpenVkGUI.VertexBuffer.Size = 0;
	OpenVkGUI.IndexBuffer.Size = 0;
	OpenVkGUI.LastIndex = 0;
	OpenVkGUI.EntityCount = 1;

	OpenVkGUI.Width = Width;
	OpenVkGUI.Height = Height;

	if (OpenVkGUI.WindowCount >= OpenVkGUI.WindowSize)
	{
		OpenVkGUI.WindowSize += OPENVK_GUI_ALLOCATE_BLOCK;
		OpenVkGUI.Windows = (OpenVkGUIWindow*)realloc(OpenVkGUI.Windows, OpenVkGUI.WindowSize * sizeof(OpenVkGUIWindow));
		for (uint32_t i = OpenVkGUI.WindowCount; i < OpenVkGUI.WindowSize; i++)
			OpenVkGUI.Windows[i] = OpenVkGUIWindowInit(i);
	}

	OpenVkGUI.WindowCount = 0;
	OpenVkGUI.Window = NULL;

//	OpenVkGUI.Rounding = 120.0 * sinf(SDL_GetTicks() * 0.0025) + 120.0;

	OpenVkSetScissor(0, 0, Width, Height);
	OpenVkSetViewport(0, 0, Width, Height);

	OpenVkBindPipeline(OpenVkGUI.Pipeline, OPENVK_PIPELINE_TYPE_GRAPHICS);
}

void OpenVkGUITransformVertices(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	float RLS = Right - Left;
	float TBS = Top - Bottom;

	float M[4] =
	{
		2 / RLS,
		-(Right + Left) / RLS,
		2 / TBS,
		-(Top + Bottom) / TBS
	};

	for (uint32_t j = 0; j < OpenVkGUI.VertexBuffer.Size; j++)
	{
		
		OpenVkGUIVertex* V = (OpenVkGUIVertex*)OpenVkGUI.VertexBuffer.Data;
		V[j].Pos[1] = OpenVkGUI.Height - V[j].Pos[1];
		float Pos[2] =
		{
			V[j].Pos[0],
			V[j].Pos[1]
		};

		float x = Pos[0] * M[0] + M[1];
		float y = Pos[1] * M[2] + M[3];

		V[j].Pos[0] = x;
		V[j].Pos[1] = y;
	}
}

void OpenVkGUIEndRender()
{
	if (OpenVkGUI.IndexBuffer.Size > 0 && OpenVkGUI.VertexBuffer.Size < OpenVkGUI.VertexBuffer.AllocatedSize && OpenVkGUI.IndexBuffer.Size < OpenVkGUI.IndexBuffer.AllocatedSize)
	{
		uint32_t VertexSize = OpenVkGUI.VertexBuffer.Size;		
		uint32_t IndexSize = OpenVkGUI.IndexBuffer.Size;

	//	if (OpenVkGUI.Window != NULL)
	//		OpenVkSetScissor(OpenVkGUI.Window->x, OpenVkGUI.Window->y, OpenVkGUI.Window->w, OpenVkGUI.Window->h);

		OpenVkGUITransformVertices(0.0, OpenVkGUI.Width, OpenVkGUI.Height, 0.0, -1.0, 1.0);

		OpenVkUpdateDynamicBuffer(VertexSize * sizeof(OpenVkGUIVertex), OpenVkGUI.VertexBuffer.Data, OpenVkGUI.VertexBuffer.Buffer);
		OpenVkUpdateDynamicBuffer(IndexSize * sizeof(uint32_t), OpenVkGUI.IndexBuffer.Data, OpenVkGUI.IndexBuffer.Buffer);
	
		OpenVkBindDescriptorSet(OpenVkGUI.PipelineLayout, 0, OpenVkGUI.DescriptorSet, OPENVK_PIPELINE_TYPE_GRAPHICS);
		OpenVkBindDynamicIndexBuffer(OpenVkGUI.VertexBuffer.Buffer, OpenVkGUI.IndexBuffer.Buffer);
		OpenVkDrawIndices(IndexSize);
	}
}

void OpenVkGUIAddVertices(uint32_t Count, OpenVkGUIVertex* Vertices)
{
	OpenVkGUIVertex* Vertex = (OpenVkGUIVertex*)OpenVkGUI.VertexBuffer.Data;

	if (OpenVkGUI.VertexBuffer.Size + Count <= OpenVkGUI.VertexBuffer.AllocatedSize)
	{
		Vertex += OpenVkGUI.VertexBuffer.Size;
		for (uint32_t i = 0; i < Count; i++)
		{			
			
			Vertex[i] = Vertices[i];
		//	Vertex[i].Color[0] = sinf(OpenVkGUI.VertexBuffer.Size + i * SDL_GetTicks() * 0.003) + 0.5;
		//	Vertex[i].Color[1] = cosf(OpenVkGUI.VertexBuffer.Size + i * SDL_GetTicks() * 0.001) + 0.5;
		//	Vertex[i].Color[2] = sinf(OpenVkGUI.VertexBuffer.Size + i * SDL_GetTicks() * 0.002) + 0.5;

		//	Normalize3P((vec3*)&Vertex[i].Color);
		}
			
	}

	OpenVkGUI.VertexBuffer.Size += Count;
}

void OpenVkGUIAddIndices(uint32_t Count, uint32_t* Indices)
{
	uint32_t* Index = (uint32_t*)OpenVkGUI.IndexBuffer.Data;

	if (OpenVkGUI.IndexBuffer.Size + Count <= OpenVkGUI.IndexBuffer.AllocatedSize)
	{
		Index += OpenVkGUI.IndexBuffer.Size;
		for (uint32_t i = 0; i < Count; i++)
			Index[i] = Indices[i];
	}

	OpenVkGUI.IndexBuffer.Size += Count;
}

char OpenVkClipQuad(OpenVkGUIVertex* Vertices)
{
	if (OpenVkGUI.Window == NULL)
		return 1;

	int32_t WinX = OpenVkGUI.Window->w + OpenVkGUI.Window->x;
	int32_t WinY = OpenVkGUI.Window->h + OpenVkGUI.Window->y;

	if (Vertices[0].Pos[0] > WinX &&
		Vertices[1].Pos[0] > WinX &&
		Vertices[2].Pos[0] > WinX &&
		Vertices[3].Pos[0] > WinX)
		return 0;
		
	if (Vertices[0].Pos[1] > WinY &&
		Vertices[1].Pos[1] > WinY &&
		Vertices[2].Pos[1] > WinY &&
		Vertices[3].Pos[1] > WinY)
		return 0;
		

	//clipping

	uint32_t MinX;
	uint32_t MinY;
	float MinXV = 100000.0;
	float MinYV = 100000.0;

	for (uint32_t i = 0; i < 4; i++)
	{
		if (Vertices[i].Pos[0] < MinXV)
		{
			MinXV = Vertices[i].Pos[0];
			MinX = i;
		}

		if (Vertices[i].Pos[1] < MinYV)
		{
			MinYV = Vertices[i].Pos[1];
			MinY = i;
		}
			
	}

	for (uint32_t i = 0; i < 4; i++)
	{
		

		if (Vertices[i].Pos[0] > WinX)
		{
			float WinDiff = Vertices[i].Pos[0] - WinX;
			float QuadWidth = (Vertices[i].Pos[0] - Vertices[MinX].Pos[0]);
			float Percent = WinDiff / QuadWidth;//0.0-1.0

			Vertices[i].Pos[0] = WinX;

			float UvDiff = Vertices[i].UV[0] - Vertices[MinX].UV[0];
			float UvPercent = Percent * UvDiff;//0.0-1.0

			Vertices[i].UV[0] -= UvPercent;
		}
		if (Vertices[i].Pos[1] > WinY)
		{
			float WinDiff = Vertices[i].Pos[1] - WinY;
			float QuadHeight = (Vertices[i].Pos[1] - Vertices[MinY].Pos[1]);
			float Percent = WinDiff / QuadHeight;//0.0-1.0

			Vertices[i].Pos[1] = WinY;

			float UvDiff = Vertices[i].UV[1] - Vertices[MinY].UV[1];
			float UvPercent = Percent * UvDiff;//0.0-1.0

			Vertices[i].UV[1] -= UvPercent;
		}
	}

	return 1;
}

uint32_t OpenVkGUITextPos(const char* Title, uint32_t x, uint32_t y, uint32_t MaxWidth, uint32_t MaxLength)
{
//	y = OpenVkGUI.Height - y;
	y += OpenVkGUI.Size / 1.75;
//	MaxWidth += x;

	float OffsetX = 0;
	float OffsetY = 0;

	uint32_t Length = strlen(Title);
	uint32_t Index = (float)SDL_GetTicks() * 0.02;
	Index %= Length;

	for (uint32_t i = 0; i < Length; i++)
	{
		while (Title[i] == '\r' || Title[i] == '\n')
		{
			OffsetX = 0;
			OffsetY += OpenVkGUI.Size;

			i++;
			if (Title[i] == '\r' && Title[i + 1] == '\n') i++;
			if (Title[i] == '\n' && Title[i - 1] == '\r') i++;
		}
		while (Title[i] == '\t')
		{
			OffsetX += OpenVkGUI.Size * 2;
			i++;
		}

		float t = OffsetX + OpenVkGUI.Size / 1.75;

	//	if (MaxLength != 0 && t >= MaxLength)
	//		return y + OffsetY;

		if (Title[i] != ' ' && MaxWidth > 0 && t >= MaxWidth)
		{
			OffsetX = 0;
			OffsetY += OpenVkGUI.Size;
		}	

		OpenVkGUIGlyph Glyph = OpenVkGUIMakeGlyph(Title[i], OffsetX, OffsetY);
		OffsetX = Glyph.Offset[0];
		OffsetY = Glyph.Offset[1];

	//	mat4 Proj = OrthoMat4(0.0, OpenVkGUI.Width, OpenVkGUI.Height, 0.0, -1.0, 1.0);
	//	for (uint32_t j = 0; j < 4; j++)
	//	{
	//		vec4 Pos =
	//		{
	//			Glyph.Vertices[j].Pos[0] + x,
	//			Glyph.Vertices[j].Pos[1] + y,
	//			0.0,
	//			1.0
	//		};
	//
	//		vec4 P = MultiplyVec4Mat4P(&Pos, &Proj);
	//
	//		Glyph.Vertices[j].Pos[0] = P.x;
	//		Glyph.Vertices[j].Pos[1] = P.y;
	//	}
		for (uint32_t j = 0; j < 4; j++)
		{
			Glyph.Vertices[j].Pos[0] += x;
			Glyph.Vertices[j].Pos[1] += y;

		//	Glyph.Vertices[j].Color[0] = sinf(OpenVkGUI.VertexBuffer.Size + (j) * SDL_GetTicks() * 0.003) + 0.5;
		//	Glyph.Vertices[j].Color[1] = cosf(OpenVkGUI.VertexBuffer.Size + (j) * SDL_GetTicks() * 0.001) + 0.5;
		//	Glyph.Vertices[j].Color[2] = sinf(OpenVkGUI.VertexBuffer.Size + (j) * SDL_GetTicks() * 0.002) + 0.5;
		}

	//	char RenderGlyph = 1;
	//
	//	if (OpenVkGUI.Window != NULL)
	//	{
	//		for (uint32_t i = 0; i < 4; i++)
	//		{
	//			if (Glyph.Vertices[i].Pos[0] > OpenVkGUI.Window->w + OpenVkGUI.Window->x)
	//			{
	//				RenderGlyph = 0;
	//				break;
	//			}
	//			if (Glyph.Vertices[i].Pos[1] > OpenVkGUI.Window->h + OpenVkGUI.Window->y)
	//			{
	//				RenderGlyph = 0;
	//				break;
	//			}
	//
	//		}
	//	}
	//	
	//	if (RenderGlyph)
		if (OpenVkClipQuad(Glyph.Vertices))
		{
			OpenVkGUIAddVertices(4, Glyph.Vertices);

			uint32_t Indices[6];
			Indices[0] = OpenVkGUI.LastIndex;
			Indices[1] = OpenVkGUI.LastIndex + 1;
			Indices[2] = OpenVkGUI.LastIndex + 2;
			Indices[3] = OpenVkGUI.LastIndex;
			Indices[4] = OpenVkGUI.LastIndex + 2;
			Indices[5] = OpenVkGUI.LastIndex + 3;
			OpenVkGUIAddIndices(6, Indices);

			OpenVkGUI.LastIndex += 4;
		}		
	}

	OpenVkGUI.EntityCount++;

	return y + OffsetY;
}

int32_t OpenVkGUIQuadPos(int32_t x, int32_t y, int32_t w, int32_t h, float Rounding, float Color)
{
	OpenVkGUIVertex Vertices[4] =
	{
		{ {x + w,  y + h},	{1001.0, 1001.0}, {w, h, Rounding, Color} },
		{ {x + w, y},		{1001.0, 999.0},  {w, h, Rounding, Color} },
		{ {x, y},			{999.0, 999.0},   {w, h, Rounding, Color} },
		{ {x,  y + h},		{999.0, 1001.0},  {w, h, Rounding, Color} },
	};

	if (!OpenVkClipQuad(Vertices))
		return y + h;

	OpenVkGUIAddVertices(4, Vertices);

	uint32_t Indices[6];
	Indices[0] = OpenVkGUI.LastIndex;
	Indices[1] = OpenVkGUI.LastIndex + 1;
	Indices[2] = OpenVkGUI.LastIndex + 2;
	Indices[3] = OpenVkGUI.LastIndex;
	Indices[4] = OpenVkGUI.LastIndex + 2;
	Indices[5] = OpenVkGUI.LastIndex + 3;
	OpenVkGUIAddIndices(6, Indices);

	OpenVkGUI.LastIndex += 4;

	return y + h;
}

OpenVkBool OpenVkGUICollision(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	//	int32_t MouseX;
	//	int32_t MouseY;
	//	uint32_t MouseState = OpenVkGetMousePos(&MouseX, &MouseY);

	if (OpenVkGUI.CurMouse.x < x + w &&
		OpenVkGUI.CurMouse.x > x &&
		OpenVkGUI.CurMouse.y < y + h &&
		OpenVkGUI.CurMouse.y > y)
	{
		//	*x += MouseX;
		//	*y += MouseY;
		if (OpenVkGUI.CurMouse.State == 1)
			return OpenVkTrue;
		else
			return 2;
	}


	return OpenVkFalse;
}

typedef struct
{
	int32_t x; 
	int32_t y; 
	int32_t w; 
	int32_t h; 
	float Rounding; 
	float Color; 
	float ColorSelected;
	float ColorHovered;
	int32_t LastX; 
	int32_t LastY;
	int32_t LastW;
	int32_t LastH;
	OpenVkBool Selected;
} OpenVkGUIQuad;

void OpenVkGUIQuadPosExt(OpenVkGUIQuad* Quad)
{
	Quad->Selected = OpenVkGUICollision(Quad->x, Quad->y, Quad->w, Quad->h);
	if (Quad->Selected == OpenVkTrue)
		Quad->Color = Quad->ColorSelected;
	else if (Quad->Selected == 2)
		Quad->Color = Quad->ColorHovered;

	OpenVkGUIVertex Vertices[4] =
	{
		{ { Quad->x + Quad->w, Quad->y + Quad->h },	{1001.0, 1001.0}, {Quad->w, Quad->h, Quad->Rounding, Quad->Color} },
		{ { Quad->x + Quad->w, Quad->y			 },	{1001.0,  999.0},  {Quad->w, Quad->h, Quad->Rounding, Quad->Color} },
		{ { Quad->x,		   Quad->y			 },	{ 999.0,  999.0},   {Quad->w, Quad->h, Quad->Rounding, Quad->Color} },
		{ { Quad->x,		   Quad->y + Quad->h },	{ 999.0, 1001.0},  {Quad->w, Quad->h, Quad->Rounding, Quad->Color} },
	};
	
	if (!OpenVkClipQuad(Vertices))
		return;

	OpenVkGUIAddVertices(4, Vertices);

	uint32_t Indices[6];
	Indices[0] = OpenVkGUI.LastIndex;
	Indices[1] = OpenVkGUI.LastIndex + 1;
	Indices[2] = OpenVkGUI.LastIndex + 2;
	Indices[3] = OpenVkGUI.LastIndex;
	Indices[4] = OpenVkGUI.LastIndex + 2;
	Indices[5] = OpenVkGUI.LastIndex + 3;
	OpenVkGUIAddIndices(6, Indices);

	OpenVkGUI.LastIndex += 4;

	Quad->LastX = Quad->x + Quad->w;
	Quad->LastY = Quad->y + Quad->h;

	Quad->LastW = Quad->w;
	Quad->LastH = Quad->h;
}

void OpenVkGUIBegin(const char* Title)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];

	uint32_t WindowTitleBorder = OpenVkGUI.SizeQuater;
	uint32_t WindowTitleSize = OpenVkGUI.Size;
	float ThirdRounding = -1.0;
	if (OpenVkGUI.Rounding > 0.0)
		ThirdRounding = OpenVkGUI.Rounding / 3;

	OpenVkBool TmpCollision = OpenVkGUICollision(OpenVkGUI.Window->x, OpenVkGUI.Window->y, OpenVkGUI.Window->w, WindowTitleSize);

	float Color = OpenVkGUI.Color;
	if (TmpCollision == 2)
		Color = OpenVkGUI.ColorSelected;

	OpenVkBool Selectable = OpenVkTrue;
	if (TmpCollision)
	{
		for (uint32_t i = 0; i < OpenVkGUI.WindowCount; i++)
		{
			if (OpenVkGUI.Windows[i].Selected)
			{
				Selectable = OpenVkFalse;
				break;
			}
		}
	}	

	if (Selectable)
	{
		if (TmpCollision == OpenVkTrue)
			OpenVkGUI.Window->Selected = OpenVkTrue;
		if (TmpCollision != OpenVkTrue && OpenVkGUI.CurMouse.State != 1)
			OpenVkGUI.Window->Selected = OpenVkFalse;

		
		if (OpenVkGUI.Window->Selected == OpenVkTrue)
		{
			OpenVkGUI.Window->x = OpenVkGUI.Window->sx + (OpenVkGUI.CurMouse.x - OpenVkGUI.LastPressedMouse.x);
			OpenVkGUI.Window->y = OpenVkGUI.Window->sy + (OpenVkGUI.CurMouse.y - OpenVkGUI.LastPressedMouse.y);
		}
		else
		{
			OpenVkGUI.Window->sx = OpenVkGUI.Window->x;
			OpenVkGUI.Window->sy = OpenVkGUI.Window->y;
		}

		//	TmpCollision = OpenVkGUICollision(OpenVkGUI.Window->x + OpenVkGUI.Window->w - 4, OpenVkGUI.Window->y + WindowTitleSize, OpenVkGUI.Window->w + 4, OpenVkGUI.Window->h - WindowTitleSize);

		TmpCollision = OpenVkGUICollision(OpenVkGUI.Window->x + OpenVkGUI.Window->w - 14, OpenVkGUI.Window->y + OpenVkGUI.Window->h - 14, 14, 14);

		if (TmpCollision == OpenVkTrue)
			OpenVkGUI.Window->ResizeSelected = OpenVkTrue;
		if (TmpCollision != OpenVkTrue && OpenVkGUI.CurMouse.State != 1)
			OpenVkGUI.Window->ResizeSelected = OpenVkFalse;

		if (OpenVkGUI.Window->ResizeSelected == OpenVkTrue)
		{
			OpenVkGUI.Window->w = OpenVkGUI.Window->sw + (OpenVkGUI.CurMouse.x - OpenVkGUI.LastPressedMouse.x);
			OpenVkGUI.Window->h = OpenVkGUI.Window->sh + (OpenVkGUI.CurMouse.y - OpenVkGUI.LastPressedMouse.y);
		}
		else
		{
			OpenVkGUI.Window->sw = OpenVkGUI.Window->w;
			OpenVkGUI.Window->sh = OpenVkGUI.Window->h;
		}
	}

	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;
/*
	static uint32_t x = 60;
	static uint32_t y = 40;

	static uint32_t tx = 60;
	static uint32_t ty = 40;
	uint32_t w = 560;
	uint32_t h = 600;

	static OpenVkBool Collision = OpenVkFalse;
	OpenVkBool TmpCollision = OpenVkGUICollision(x, y, w, h);

//	if (TmpCollision && !Collision)
//	{
//		tx = x;
//		ty = y;
//	}
	if (!TmpCollision && Collision)
	{
		tx = x;
		ty = y;
	}

	x = tx;
	y = ty;

	
	if (TmpCollision && !Collision)
		Collision = OpenVkTrue;
	if (!TmpCollision && OpenVkGUI.CurMouse.State != 1)
		Collision = OpenVkFalse;



	if (Collision)
	{
		x += OpenVkGUI.CurMouse.x - OpenVkGUI.LastPressedMouse.x;
		y += OpenVkGUI.CurMouse.y - OpenVkGUI.LastPressedMouse.y;
	//	tx = x;
	//	ty = y;
	}
	else
	{
		
		
		//	x = OpenVkGUI.LastPressedMouse.x;
		//	y = OpenVkGUI.LastPressedMouse.y;
	}

//	if (Collision && OpenVkGUI.CurMouse.x != OpenVkGUI.LastMouse.x && OpenVkGUI.CurMouse.y != OpenVkGUI.LastMouse.y)
	
	*/
	/*
	static int32_t MouseX = 60;
	static int32_t MouseY = 40;
	

	OpenVkBool Collision = OpenVkGUICollision(x, y, w, h);

	if (Collision)
	{
		int32_t TmpMouseX;
		int32_t TmpMouseY;
		uint32_t MouseState = OpenVkGetMousePos(&TmpMouseX, &TmpMouseY);
		x = TmpMouseX - MouseX;
		y = TmpMouseY - MouseY;
	}
	else
	{
	//	MouseX = x;
	//	MouseY = y;
	}
	*/
	/*
	* Close alligment
	* uint32_t WindowTitleBorder = OpenVkGUI.Size / 4;
	* uint32_t WindowTitleSize = OpenVkGUI.Size;
	* Wide alligment
	* uint32_t WindowTitleBorder = OpenVkGUI.Size / 2;
	* uint32_t WindowTitleSize = OpenVkGUI.Size + WindowTitleBorder;
	*/

	//window bg
	OpenVkGUIQuadPos(x, y + 1, w, h - 1, OpenVkGUI.Rounding, OpenVkGUI.WindowColor);
	//window title bar round
	OpenVkGUI.Window->LastY = OpenVkGUIQuadPos(x, y, w, WindowTitleSize, OpenVkGUI.Rounding, Color) + (OpenVkGUI.Border - OpenVkGUI.Offset);
	//window title bar square
	if (OpenVkGUI.Rounding > 0.0)
		OpenVkGUIQuadPos(x, y + WindowTitleSize - ThirdRounding, w, ThirdRounding + 1, 0.0, Color);
	OpenVkGUITextPos(Title, WindowTitleBorder + x, WindowTitleBorder + y, w - WindowTitleBorder, w - WindowTitleBorder);// + WindowTitleBorder;

	//	OpenVkGUIQuadPos(OpenVkGUI.Window->x + OpenVkGUI.Window->w - 14, OpenVkGUI.Window->y + OpenVkGUI.Window->h - 14, 14, 14, 0.0, 1.8);	
	
	
//	//Slider
//	OpenVkGUIQuadPos(OpenVkGUI.Border + x, OpenVkGUI.Border + PosY, w - OpenVkGUI.Border * 2, OpenVkGUI.Size + 10, OpenVkGUI.Rounding, 0.55);
//	PosY = OpenVkGUIQuadPos(OpenVkGUI.Border + x + 20, OpenVkGUI.Border + PosY + 5, OpenVkGUI.Size / 2, OpenVkGUI.Size, OpenVkGUI.Rounding / 2, 0.2);
//	OpenVkGUI.Window->LastY = PosY;
	//Check box
//	OpenVkGUIQuadPos(OpenVkGUI.Border + x, OpenVkGUI.Border + PosY, OpenVkGUI.Size + 10, OpenVkGUI.Size + 10, OpenVkGUI.Rounding, 0.55);
//	//Unselected
//	OpenVkGUIQuadPos(OpenVkGUI.Border + x + 4, OpenVkGUI.Border + PosY + 4, OpenVkGUI.Size + 2, OpenVkGUI.Size + 2, OpenVkGUI.Rounding, 0.2);
}

void OpenVkGUIEnd()
{
	if (OpenVkGUI.WindowCount < OpenVkGUI.WindowSize)
		OpenVkGUI.WindowCount++;
}

void OpenVkGUIText(const char* Text)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;
	OpenVkGUI.Window->LastY = OpenVkGUITextPos(Text, OpenVkGUI.Border + x,
												OpenVkGUI.Offset + OpenVkGUI.Window->LastY,
											   OpenVkGUI.Window->w - OpenVkGUI.Border, w - (OpenVkGUI.Window->LastW + OpenVkGUI.Border + OpenVkGUI.SizeQuater)); //Max Length cut off doesnt work

	OpenVkGUI.Window->LastX = x;
//	OpenVkGUI.Window->LastY = 0;
	OpenVkGUI.Window->LastW = 0;
	OpenVkGUI.Window->LastH = OpenVkGUI.Size;
}

void OpenVkGUITextSameLine(const char* Text)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;
	OpenVkGUITextPos(Text, OpenVkGUI.Offset + OpenVkGUI.Window->LastX,
												    (OpenVkGUI.Window->LastY - OpenVkGUI.Window->LastH / 2) - OpenVkGUI.SizeQuater,
											   0, w - (OpenVkGUI.Window->LastW + OpenVkGUI.Border + OpenVkGUI.SizeQuater)); //Max Length cut off doesnt work
}

OpenVkBool OpenVkGUIButton(const char* Name)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;

	OpenVkGUIQuad Quad;
	Quad.x = OpenVkGUI.Border + x;
	Quad.y = OpenVkGUI.Offset + OpenVkGUI.Window->LastY;
	Quad.w = OpenVkGUIGetTextWidth(Name) + OpenVkGUI.Offset * 2;
	Quad.h = OpenVkGUI.ToolSize;
	Quad.Rounding = OpenVkGUI.Rounding;
	Quad.Color = OpenVkGUI.Color;
	Quad.ColorSelected = OpenVkGUI.ColorSelected;
	Quad.ColorHovered = OpenVkGUI.ColorHovered;
	OpenVkGUIQuadPosExt(&Quad);
	//OpenVkGUI.Window->LastX = Quad.LastX;
	OpenVkGUI.Window->LastX = x + OpenVkGUI.Border;
	OpenVkGUI.Window->LastY = Quad.LastY;
	OpenVkGUI.Window->LastW = Quad.LastW;
	OpenVkGUI.Window->LastH = Quad.LastH;
	
	OpenVkGUITextSameLine(Name);

	return (Quad.Selected && (OpenVkGUI.MousePressed == OpenVkTrue));
}

OpenVkBool OpenVkGUICheckbox(const char* Name, OpenVkBool* State)
{
	OpenVkBool Selected = *State;
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;

//	uint32_t LastX = 0;
//	uint32_t LastY = 0;
//	OpenVkBool Collision = OpenVkGUIQuadPosExt(OpenVkGUI.Border + x, OpenVkGUI.Border + OpenVkGUI.Window->LastY, OpenVkGUI.ToolSize, OpenVkGUI.ToolSize, OpenVkGUI.Rounding, OpenVkGUI.Color, OpenVkGUI.ColorSelected, &LastX, &LastY);

	OpenVkGUIQuad Quad;
	Quad.x = OpenVkGUI.Border + x;
	Quad.y = OpenVkGUI.Offset + OpenVkGUI.Window->LastY;
	Quad.w = OpenVkGUI.ToolSize;
	Quad.h = OpenVkGUI.ToolSize;
	Quad.Rounding = OpenVkGUI.Rounding;
	Quad.Color = OpenVkGUI.Color;
	Quad.ColorSelected = OpenVkGUI.ColorSelected;
	Quad.ColorHovered = OpenVkGUI.ColorHovered;
	OpenVkGUIQuadPosExt(&Quad);
	
	if (Quad.Selected && OpenVkGUI.MousePressed)
		Selected = !Selected;

	if (Selected == OpenVkFalse)
		OpenVkGUIQuadPos(OpenVkGUI.Border + x + OpenVkGUI.SizeEigth, 
						 OpenVkGUI.Offset + OpenVkGUI.Window->LastY + OpenVkGUI.SizeEigth, 
						 OpenVkGUI.ToolSize-OpenVkGUI.SizeQuater + 1.5, 
						 OpenVkGUI.ToolSize-OpenVkGUI.SizeQuater + 1.5, 
						 OpenVkGUI.Rounding, OpenVkGUI.WindowColor);

	OpenVkGUI.Window->LastW = Quad.LastW;
	OpenVkGUI.Window->LastH = Quad.LastH;
	OpenVkGUI.Window->LastX = Quad.LastX;
	OpenVkGUI.Window->LastY = Quad.LastY;

	OpenVkGUITextSameLine(Name);
	
	*State = Selected;

	return Selected;
}

void OpenVkGUISlider(const char* Name, float Min, float Max, float* Value)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;

	OpenVkGUIQuad Quad;
	Quad.x = OpenVkGUI.Border + x;
	Quad.y = OpenVkGUI.Offset + OpenVkGUI.Window->LastY;
	Quad.w = w - (w / 3);
	Quad.h = OpenVkGUI.ToolSize;
	Quad.Rounding = OpenVkGUI.Rounding;
	Quad.Color = OpenVkGUI.Color;
	Quad.ColorSelected = OpenVkGUI.ColorHovered;
	Quad.ColorHovered = OpenVkGUI.ColorHovered;
	OpenVkGUIQuadPosExt(&Quad);
	OpenVkGUI.Window->LastW = Quad.LastW;
	OpenVkGUI.Window->LastH = Quad.LastH;
//	OpenVkGUI.Window->LastX = Quad.LastX;
	OpenVkGUI.Window->LastY = Quad.LastY;

	//text
	char Str[16];
	sprintf(Str, "%.4f", *Value);

	//	OpenVkGUI.Window->LastW = Quad.LastW;
	//	OpenVkGUI.Window->LastH = Quad.LastH;
	//	OpenVkGUI.Window->LastX = Quad.LastX;
	//	OpenVkGUI.Window->LastY = Quad.LastY;

	OpenVkGUI.Window->LastX = Quad.x + Quad.w / 2 - (OpenVkGUIGetTextWidth(Str) / 2);
	OpenVkGUITextSameLine(Str);
	OpenVkGUI.Window->LastX = Quad.LastX;

	//OpenVkGUI.Window->LastW = Quad.LastW;

	if (Quad.Selected == OpenVkTrue && OpenVkGUI.MousePressed == OpenVkTrue)
		OpenVkGUI.SelectedEntity = OpenVkGUI.EntityCount;
	if (Quad.Selected != OpenVkTrue && OpenVkGUI.CurMouse.State != 1)
		OpenVkGUI.SelectedEntity = 0;

//	if (Quad.Selected == OpenVkTrue)
//		OpenVkGUI.MouseItemCollision = OpenVkTrue;
//	if (Quad.Selected != OpenVkTrue && OpenVkGUI.CurMouse.State != 1)
//		OpenVkGUI.MouseItemCollision = OpenVkFalse;
//
//
//	if (OpenVkGUI.MouseItemCollision == OpenVkTrue)
	if (OpenVkGUI.SelectedEntity == OpenVkGUI.EntityCount)
	{
		Quad.w -= OpenVkGUI.ToolSize;
		Quad.x += OpenVkGUI.ToolSizeHalf;

	//	float M = OpenVkGUI.CurMouse.x / OpenVkGUI.Window->w;//Mouse in scren 0.0-1.0
	//	float M = ((((float)OpenVkGUI.CurMouse.x) / (fabsf((float)x) + (float)x + (float)w)) - (float)x);//Mouse in window 0.0-1.0
		float M = ((float)OpenVkGUI.CurMouse.x / (Quad.w)) - ((float)Quad.x / Quad.w);//Mouse in window 0.0-1.0

		if (M < 0.0)
			M = 0.0;
		else if (M > 1.0)
			M = 1.0;

		*Value = (M * (fabsf(Min) + Max)) - Min;

		//	*Value = (M * (fabsf(Min) + Max)) - Min;

	//	printf("%d\n", OpenVkGUI.CurMouse.State);
	//	float x = OpenVkGUI.sx + (OpenVkGUI.CurMouse.x - OpenVkGUI.LastPressedMouse.x);
	////	Quad.y = OpenVkGUI.sy + (OpenVkGUI.CurMouse.y - OpenVkGUI.LastPressedMouse.y);
	//
	//	*Value = ((x / OpenVkGUI.Width) * (fabsf(Min) + Max)) - Min;
	//
	////	if (Quad.x > x)
	////		x = Quad.x;
	//
	//	Quad.x = x;
		Quad.w += OpenVkGUI.ToolSize;
	}
	else
	{
	//	Quad.w += OpenVkGUI.ToolSizeHalf;
		//	OpenVkGUI.sx = Quad.x;
		//	OpenVkGUI.sy = Quad.y;
	}

//	Quad.w -= OpenVkGUI.ToolSize;
//	float SliderX = (((*Value) / (fabsf(Min) + Max)) - Min) * (Quad.w - OpenVkGUI.ToolSize) + OpenVkGUI.ToolSizeHalf;
	float SliderX = (((*Value) / (fabsf(Min) + Max)) - Min) * (Quad.w - OpenVkGUI.ToolSize);
	float T = (OpenVkGUI.ToolSizeHalf / 6);

	//Slider button
	Quad.w = OpenVkGUI.ToolSizeHalf;
	Quad.h = OpenVkGUI.ToolSize - T * 2;
	Quad.x = OpenVkGUI.Border + x + SliderX + OpenVkGUI.ToolSizeHalf - (Quad.w / 2);
//	Quad.x = OpenVkGUI.Border + x + SliderX;
	Quad.y += T;
	Quad.Rounding = OpenVkGUI.Rounding / 2;
	Quad.Color = OpenVkGUI.WindowColor;
	Quad.ColorHovered = OpenVkGUI.ColorSelected + OpenVkGUI.WindowColor;
	Quad.ColorSelected = Quad.ColorHovered - 0.1;
	
	OpenVkGUIQuadPosExt(&Quad);

	

//	if (Quad.Selected == OpenVkTrue)
//	{
//
//	}

	OpenVkGUITextSameLine(Name);

	

	/*OpenVkGUIQuadPos(OpenVkGUI.Border + x, OpenVkGUI.Border + PosY, w - OpenVkGUI.Border * 2, OpenVkGUI.Size + 10, OpenVkGUI.Rounding, 0.55);
//	PosY = OpenVkGUIQuadPos(OpenVkGUI.Border + x + 20, OpenVkGUI.Border + PosY + 5, OpenVkGUI.Size / 2, OpenVkGUI.Size, OpenVkGUI.Rounding / 2, 0.2);
	*/
	
}

void OpenVkGUIDragSlider(const char* Name, float Min, float Max, float* Value)
{
	OpenVkGUI.Window = &OpenVkGUI.Windows[OpenVkGUI.WindowCount];
	uint32_t x = OpenVkGUI.Window->x;
	uint32_t y = OpenVkGUI.Window->y;
	uint32_t w = OpenVkGUI.Window->w;
	uint32_t h = OpenVkGUI.Window->h;

	OpenVkGUIQuad Quad;
	Quad.x = OpenVkGUI.Border + x;
	Quad.y = OpenVkGUI.Offset + OpenVkGUI.Window->LastY;
	Quad.w = w - (w / 3);
	Quad.h = OpenVkGUI.ToolSize;
	Quad.Rounding = OpenVkGUI.Rounding;
	Quad.Color = OpenVkGUI.Color;
	Quad.ColorSelected = OpenVkGUI.ColorSelected;
	Quad.ColorHovered = OpenVkGUI.ColorHovered;
	OpenVkGUIQuadPosExt(&Quad);
	

	OpenVkGUI.Window->LastW = Quad.LastW;
	OpenVkGUI.Window->LastH = Quad.LastH;
	OpenVkGUI.Window->LastX = Quad.LastX;
	OpenVkGUI.Window->LastY = Quad.LastY;

	OpenVkGUITextSameLine(Name);

	//text
	char Str[16];
	sprintf(Str, "%.4f", *Value);

	OpenVkGUI.Window->LastX = Quad.x + Quad.w / 2 - (OpenVkGUIGetTextWidth(Str) / 2);
	OpenVkGUITextSameLine(Str);
	OpenVkGUI.Window->LastX = Quad.LastX;
}