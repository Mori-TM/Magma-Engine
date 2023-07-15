typedef enum
{
	ICON_BUTTON_TYPE_LARGE = 0x0,
	ICON_BUTTON_TYPE_SMALL = 0x1,
	ICON_BUTTON_TYPE_EXT = 0x2,
} IconButtonTypes;

vec3 ImGuiIconButtonColor = { 0.8, 0.8, 0.8 };

//Gets init in the FXAA pipeline
uint32_t SceneRenderDescriptorSet = 0;

bool ImGuiIconButton(const char* Icon, const char* ID, uint32_t IconType, const ImVec2& Size = ImVec2(0, 0))
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(ImGuiIconButtonColor.x * 255, ImGuiIconButtonColor.y * 255, ImGuiIconButtonColor.z * 255, 255));
	if (IconType == ICON_BUTTON_TYPE_LARGE)
		ImGui::PushFont(IconFontLarge);
	else if (IconType == ICON_BUTTON_TYPE_SMALL)
		ImGui::PushFont(IconFontSmall);
	else if (IconType == ICON_BUTTON_TYPE_EXT)
		ImGui::PushFont(IconFontExt);
	ImGui::PushID(ID);
	bool Value = ImGui::Button(Icon, Size);
	ImGui::PopID();
	ImGui::PopFont();
	ImGui::PopStyleColor();

	return Value;
}

void ImGuiVec3Control(const char* label, vec3* values, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	
	float lineHeight = GImGui->Font->FontSize * GImGui->IO.FontGlobalScale + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, GImGui->Style.ItemSpacing.y });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values->x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values->x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::SameLine();
	}
	
	
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, GImGui->Style.ItemSpacing.y });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values->y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values->y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::SameLine();
	}
	
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, GImGui->Style.ItemSpacing.y });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values->z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values->z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}	

	ImGui::Columns(1);

	ImGui::PopID();
}

inline void ImGuiSetPosPaddingX(float Pos)
{
	ImGui::SetCursorPosX(Pos - GImGui->Style.FramePadding.x);
}

void EngineInitEditor()
{
	InitLua();

}

void EngineDestroyEditor()
{
	free(Entities);
	DeInitLua();
}