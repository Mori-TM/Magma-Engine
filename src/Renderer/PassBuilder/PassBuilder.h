typedef enum
{
	ATTACHMENT_STORE_OP_STORE = 0,
    ATTACHMENT_STORE_OP_DONT_CARE = 1,

	ATTACHMENT_LOAD_OP_LOAD = 0,
    ATTACHMENT_LOAD_OP_CLEAR = 1,
    ATTACHMENT_LOAD_OP_DONT_CARE = 2,
} AttachmentOperations;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH_SHORT];

	uint32_t Type;//OpenVkAttachmentTypes
	bool ScreenSize;
	//else
	uint32_t Width;
	uint32_t Height;

	uint32_t MsaaSamples;

	bool Sampled;//Usable as texture for later

	uint32_t Format;//OpenVkFormat

	//Render Pass Specific Options
	uint32_t LoadOp;
	uint32_t StoreOp;
	uint32_t InitLayout;
	uint32_t FinalLayout;


} RenderAttachment;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH_SHORT];
} RenderPass;

typedef struct
{
	uint32_t ShaderType;
	uint32_t PushSize;

	uint32_t Offset;//Calculated during build;
} RenderPushConst;

typedef struct
{

	uint32_t PushConstCount;//max 2
	RenderPushConst RenderPushConsts[2];

	uint32_t DescriptorSetLayoutCount;//max 4
	uint32_t DescriptorSetLayouts[4];


} RenderPipelineLayout;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH_SHORT];

	uint32_t VertexShader;
	uint32_t FragmentShader;

} RenderPipeline;

typedef struct
{

} RenderFramebuffer;

typedef struct
{
	char Name[MAX_CHAR_NAME_LENGTH_SHORT];
} RenderWorkflow;


CMA_MemoryZone RenderAttachments;
CMA_MemoryZone RenderRenderPasses;
CMA_MemoryZone RenderPipelines;

CMA_MemoryZone RenderWorkflows;

void RenderWorkflowInit()
{
	RenderWorkflows = CMA_Create(sizeof(RenderWorkflow), "Render Workflows");
}

void RenderWorkflowDestroy()
{
	CMA_Destroy(&RenderWorkflows);
}

void RenderWorkflowBuild()
{

}

void RenderWorkflowRun()
{

}

//given struct has to have short name at beginning
void RenderWorkflowBuilderBase(const char* StructName, CMA_MemoryZone* Struct, uint32_t* SelectedStruct, void (*OnAdd)(const char* StructName, uint32_t* SelectedStruct), void (*OnCreate)())
{
	char Tmp[MAX_CHAR_NAME_LENGTH_SHORT];
	
	ssprintf(Tmp, "Add New %s", StructName);
	if (ImGui::Button(Tmp))
	{
		
		OnAdd(StructName, SelectedStruct);
/*
		ssprintf(WF.Name, "%s %zu", StructName RenderWorkflows.Size);

		SelectedWorkflow = CMA_Push(&RenderWorkflows, &WF);
*/
	}

	if (Struct->Size > 0)
	{
		const char* Name = (const char*)CMA_GetAt(Struct, *SelectedStruct);

		if (Name && ImGui::BeginCombo(StructName, Name))
		{
			for (size_t i = 0; i < RenderWorkflows.Size; i++)
			{
				const char* Name = (const char*)CMA_GetAt(Struct, i);

				if (Name && ImGui::Button(Name))
					*SelectedStruct = i;
			}

			ImGui::EndCombo();
		}
		
		ssprintf(Tmp, "Create %s", StructName);
		if (ImGui::Button(Tmp));
		{
			OnCreate();
		}
	}
}

void RenderAttachmentBuilderOnAdd(const char* Name, uint32_t* SelectedStruct)
{
	RenderAttachment RA;
	ssprintf(RA.Name, "%s %zu", Name, RenderAttachments.Size);
	*SelectedStruct = CMA_Push(&RenderWorkflows, &RA);
}

void RenderAttachmentBuilderOnCreate()
{
	
}

void RenderWorkflowBuilderOnAdd(const char* Name, uint32_t* SelectedStruct)
{
	RenderWorkflow WF;
	ssprintf(WF.Name, "%s %zu", Name, RenderWorkflows.Size);
	*SelectedStruct = CMA_Push(&RenderWorkflows, &WF);
}

void RenderWorkflowBuilderOnCreate()
{

}

void RenderWorkflowDrawBuilder()
{
	static uint32_t SelectedWorkflow = 0;

	ImGui::Begin("Render Pass Builder");
	{
	//	ImGui::SetWindowFocus();

		if (ImGui::CollapsingHeader("Attachment Builder"))
		{

		}

		if (ImGui::CollapsingHeader("Render Pass Builder"))
		{

		}

		if (ImGui::CollapsingHeader("Pipeline Builder"))
		{

		}

		if (ImGui::CollapsingHeader("Workflow Builder"))
		{
			RenderWorkflowBuilderBase("Render Workflow", &RenderWorkflows, &SelectedWorkflow, RenderWorkflowBuilderOnAdd, RenderWorkflowBuilderOnCreate);
			/*
			if (ImGui::Button("Add New Render Workflow"));
			{
				RenderWorkflow WF;
				memset(&WF, 0, sizeof(RenderWorkflow));
				ssprintf(WF.Name, "Render Workflow %zu", RenderWorkflows.Size);

				SelectedWorkflow = CMA_Push(&RenderWorkflows, &WF);
			}

			if (RenderWorkflows.Size > 0)
			{
				RenderWorkflow* WF = (RenderWorkflow*)CMA_GetAt(&RenderWorkflows, SelectedWorkflow);

				if (WF && ImGui::BeginCombo("Render Workflows", WF->Name))
				{
					for (size_t i = 0; i < RenderWorkflows.Size; i++)
					{
						WF = (RenderWorkflow*)CMA_GetAt(&RenderWorkflows, i);

						if (WF && ImGui::Button(WF->Name))
							SelectedWorkflow = i;
					}

					ImGui::EndCombo();
				}
				

				if (ImGui::Button("Create Workflow"));
				{

				}
			}
			*/
		}

		

		
	}
	ImGui::End();
}

void RenderWorkflowRemove()
{

}

