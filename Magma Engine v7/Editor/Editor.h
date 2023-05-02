typedef enum
{
	ICON_BUTTON_TYPE_LARGE = 0x0,
	ICON_BUTTON_TYPE_SMALL = 0x1,
	ICON_BUTTON_TYPE_EXT = 0x2,
} IconButtonTypes;

vec3 ImGuiIconButtonColor = { 0.8, 0.8, 0.8 };

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