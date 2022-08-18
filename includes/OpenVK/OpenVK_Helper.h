static void (*GetWindowSizeFunc)(uint32_t* Width, uint32_t* Height) = NULL;

typedef uint32_t OpenVkBool;
#define OpenVkTrue 1
#define OpenVkFalse 0

#define OPENVK_ERROR 0xffffffffui32

typedef enum
{
	OPENVK_PRIMITIVE_TOPOLOGY_POINT = 0x0,
	OPENVK_PRIMITIVE_TOPOLOGY_LINE = 0x1,
	OPENVK_PRIMITIVE_TOPOLOGY_TRIANGLE = 0x2,
} OpenVkPrimitiveTopology;

typedef enum
{
	OPENVK_POLYGON_MODE_FILL = 0x0,
	OPENVK_POLYGON_MODE_LINE = 0x1,
	OPENVK_POLYGON_MODE_POINT = 0x2,
} OpenVkPolygonMode;

typedef enum
{
	OPENVK_CULL_MODE_NONE = 0x0,
	OPENVK_CULL_MODE_FRONT = 0x1,
	OPENVK_CULL_MODE_BACK = 0x2,
	OPENVK_CULL_MODE_FRONT_AND_BACK = 0x3,
} OpenVkCullMode;

typedef enum
{
	OPENVK_FRONT_FACE_COUNTER_CLOCK_WISE = 0x0,
	OPENVK_FRONT_FACE_CLOCK_WISE = 0x1,
} OpenVkFrontFace;

typedef enum
{
	OPENVK_SHADER_TYPE_VERTEX = 0x0,
	OPENVK_SHADER_TYPE_FRAGMENT = 0x1,
} OpenVkShaderType;

typedef enum
{
	OPENVK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 0x0,
	OPENVK_DESCRIPTOR_TYPE_DYNAMIC_UNIFORM_BUFFER = 0x1,
	OPENVK_DESCRIPTOR_TYPE_IMAGE_SAMPLER = 0x2,
} OpenVkDescriptorType;

typedef enum
{
	OPENVK_IMAGE_LAYOUT_COLOR_OUTPUT = 0x0,
	OPENVK_IMAGE_LAYOUT_DEPTH_OUTPUT = 0x1,
} OpenVkImageLayout;

typedef enum
{
	OPENVK_FILTER_NEAREST = 0x0,
	OPENVK_FILTER_LINEAR = 0x1,
} OpenVkFilter;

typedef enum
{
	OPENVK_ADDRESS_MODE_REPEAT = 0x0,
	OPENVK_ADDRESS_MODE_MIRRORED_REPEAT = 0x1,
	OPENVK_ADDRESS_MODE_CLAMP_TO_EDGE = 0x2,
	OPENVK_ADDRESS_MODE_CLAMP_TO_BORDER = 0x3,
	OPENVK_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 0x4,
} OpenVkAddressMode;

typedef enum
{
	OPENVK_IMAGE_TYPE_TEXTURE = 0x0,
	OPENVK_IMAGE_TYPE_ATTACHMENT = 0x1,
} OpenVkImageTypes;

typedef struct
{
	OpenVkBool	Dynamic;
	uint32_t	DescriptorPool;
} OpenVkDescriptorPool;

typedef struct
{
	const char*		VertexPath;
	const char*		FragmentPath;
	size_t			BindingStride;
	uint32_t		ShaderAttributeFormatCount;
	uint32_t*		ShaderAttributeFormats;
	uint32_t*		ShaderAttributeOffsets;
	uint32_t		PrimitiveTopology;
	uint32_t		x;
	uint32_t		y;
	uint32_t		Width;
	uint32_t		Height;
	OpenVkBool		DepthClamp;
	uint32_t		PolygonMode;
	float			LineWidth;
	uint32_t		CullMode;
	uint32_t		FrontFace;
	uint32_t		MsaaSamples;
	OpenVkBool		AlphaBlending;
	uint32_t		ColorBlendAttachments;
	uint32_t		PushConstantCount;
	uint32_t*		PushConstantShaderTypes;
	uint32_t*		PushConstantOffsets;
	uint32_t*		PushConstantSizes;
	uint32_t		DescriptorSetLayoutCount;
	uint32_t*		DescriptorSetLayouts;
	OpenVkBool		DepthStencil;
	uint32_t		RenderPass;
} OpenVkGraphicsPipelineCreateInfo;

typedef struct
{
	uint32_t	AttachmentCount;
	uint32_t*	Attachments;
	uint32_t	RenderPass;
	uint32_t	Width;
	uint32_t	Height;
} OpenVkFramebufferCreateInfo;

typedef struct
{
	uint32_t 				DescriptorSetLayout;
	OpenVkDescriptorPool 	DescriptorPool;
	uint32_t				DescriptorWriteCount;
	uint32_t*				DescriptorCounts;
	uint32_t*				DescriptorTypes;
	uint32_t*				UniformBuffers;
	size_t*					UniformBufferSizes;
	uint32_t*				Sampler;
	uint32_t*				ImageTypes;
	uint32_t*				Images;
	uint32_t*				ImageLayouts;
	uint32_t*				Bindings;
} OpenVkDescriptorSetCreateInfo;

typedef struct
{
	float		ClearColor[4];
	uint32_t	ClearColors;
	OpenVkBool	ClearDepth;
	uint32_t	RenderPass;
	uint32_t	Framebuffer;
	uint32_t	x;
	uint32_t	y;
	uint32_t	Width;
	uint32_t	Height;
} OpenVkBeginRenderPassInfo;

void OpenVkRuntimeInfo(const char* Msg, const char* Val)
{
	printf("\x1B[36m[Renderer Info]\033[0m\t%s%s\n", Msg, Val);
}

OpenVkBool OpenVkRuntimeError(const char* Msg)
{
	printf("\x1B[31m[Renderer Error]\033[0m\t%s\n", Msg);

	return OPENVK_ERROR;
}

void* OpenVkMalloc(size_t Size)
{
	void* Mem = malloc(Size);
	if (Mem == NULL)
		OpenVkRuntimeError("Failed to allcoate Memory!");

	return Mem;
}

void* OpenVkRealloc(void* Data, size_t Size)
{
	void* Mem = realloc(Data, Size);
	if (Mem == NULL)
		OpenVkRuntimeError("Failed to allcoate Memory!");

	return Mem;
}

void OpenVkFree(void* Data)
{
	if (Data != NULL)
		free(Data);	
	else
		OpenVkRuntimeError("No Memory to free!");
}

char* OpenVkReadFileData(const char* Path, size_t* Size)
{
	char* Buffer = NULL;
	size_t Length = 0;

	FILE* File = fopen(Path, "rb");
	if (!File)
		return NULL;

	fseek(File, 0, SEEK_END);
	Length = ftell(File);
	fseek(File, 0, SEEK_SET);
	Buffer = (char*)OpenVkMalloc(Length + 1);
	fread(Buffer, Length, 1, File);
	Buffer[Length] = '\0';

	fclose(File);

	*Size = Length;
	return Buffer;
}