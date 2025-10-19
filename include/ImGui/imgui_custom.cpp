#include "imgui_custom.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <stdint.h>

ImFont* ImGuiIconFontLarge = NULL;
ImFont* ImGuiIconFontSmall = NULL;
ImFont* ImGuiIconFontExt = NULL;

ImVec4 ImGuiIconButtonColor = { 0.8, 0.8, 0.8, 1.0 };

bool ImGuiIconButton(const char* Icon, const char* ID, uint32_t IconType, const ImVec2& Size)
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(ImGuiIconButtonColor.x * 255, ImGuiIconButtonColor.y * 255, ImGuiIconButtonColor.z * 255, 255));
	if (IconType == ICON_BUTTON_TYPE_LARGE)
		ImGui::PushFont(ImGuiIconFontLarge);
	else if (IconType == ICON_BUTTON_TYPE_SMALL)
		ImGui::PushFont(ImGuiIconFontSmall);
	else if (IconType == ICON_BUTTON_TYPE_EXT)
		ImGui::PushFont(ImGuiIconFontExt);
	ImGui::PushID(ID);
	bool Value = ImGui::Button(Icon, Size);
	ImGui::PopID();
	ImGui::PopFont();
	ImGui::PopStyleColor();

	return Value;
}



bool ImGuiImageButtonExt(ImTextureID texture_id, const ImVec2& size, const ImVec2& Scale, const ImVec2& uv0, const ImVec2& uv1)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems)
		return false;

	// Default to using texture ID as ID. User can still push string/integer prefixes.
	ImGui::PushID((void*)(intptr_t)texture_id);
	const ImGuiID id = window->GetID("#image");
	ImGui::PopID();

	const ImVec2 padding(0, 0);

	float AspectWidth = Scale.x / Scale.y;
	float AspectHeight = Scale.y / Scale.x;
	float Width = size.x * AspectWidth;
	float Height = size.y * AspectHeight;
	//    float OffsetMin = bb.Min.x + 
//
//    ImVec2 Min(window->DC.CursorPos.x, window->DC.CursorPos.y);
//    ImVec2 Max(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y);
//    Max.x = Min.x + Width;
//    ImRect Rect(Min, Max);
//    ImGui::ItemSize(Rect);

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderNavHighlight(bb, id);
	ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
	if (Scale.y > Scale.x)
	{
		bb.Min.x += (size.x / 2) - (Width / 2);
		bb.Max.x = bb.Min.x + Width;
	}
	else
	{
		bb.Min.y += (size.y / 2) - (Height / 2);
		bb.Max.y = bb.Min.y + Height;
	}

	window->DrawList->AddImage(texture_id, bb.Min, bb.Max, uv0, uv1, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));

	return pressed;
}

bool ImGuiImageButtonEx(ImGuiID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderNavHighlight(bb, id);
	ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
	if (bg_col.w > 0.0f)
		window->DrawList->AddRectFilled(bb.Min + padding, bb.Max - padding, ImGui::GetColorU32(bg_col));
	window->DrawList->AddImage(texture_id, bb.Min + padding, bb.Max - padding, uv0, uv1, ImGui::GetColorU32(tint_col));

	return pressed;
}

void ImGuiVec3Control(const char* label, float* values, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	
//	float lineHeight = GImGui->Font->FontSize * GImGui->IO.FontGlobalScale + GImGui->Style.FramePadding.y * 2.0f;
//	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImVec2 buttonSize = { 0.0, 0.0 };

	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, GImGui->Style.ItemSpacing.y });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values[0] = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "%.2f");
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
			values[1] = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "%.2f");
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
			values[2] = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}	

	ImGui::Columns(1);

	ImGui::PopID();
}

void ImGuiSetPosPaddingX(float Pos)
{
	ImGui::SetCursorPosX(Pos - GImGui->Style.FramePadding.x);
}

bool ImGuiImageButtonID(const char* ID, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems)
		return false;

	// Default to using texture ID as ID. User can still push string/integer prefixes.
	ImGui::PushID(ID);
	const ImGuiID id = window->GetID("#image");
	ImGui::PopID();

	const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : g.Style.FramePadding;
	return ImGuiImageButtonEx(id, user_texture_id, size, uv0, uv1, padding, bg_col, tint_col);
}

//React[0] -> Min
//React[1] -> Max
void ImGuiGetMenuBarRectCurrentWindow(ImVec2* React)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImRect rect = window->MenuBarRect();

	React[0] = rect.Min;
	React[1] = rect.Max;
}


float srgbToLinear(float c) {
    if (c <= 0.0031308f)
        return c * 12.92f;
    else
        return 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}

ImU32 ImGuiColorConvertFloat4ToU32GammaCorrected(const ImVec4& in, bool GammaCorrected)
{
	if (GammaCorrected)
	{
		ImU32 out;
		out  = ((ImU32)IM_F32_TO_INT8_SAT(srgbToLinear(in.x))) << IM_COL32_R_SHIFT;
		out |= ((ImU32)IM_F32_TO_INT8_SAT(srgbToLinear(in.y))) << IM_COL32_G_SHIFT;
		out |= ((ImU32)IM_F32_TO_INT8_SAT(srgbToLinear(in.z))) << IM_COL32_B_SHIFT;
		out |= ((ImU32)IM_F32_TO_INT8_SAT(srgbToLinear(in.w))) << IM_COL32_A_SHIFT;
		return out;
	}

	return ImGui::ColorConvertFloat4ToU32(in);
}

void ImGuiGet32bitColorHexCode(int ColorInt[], uint32_t BufSize, char* Buf)
{
	ImFormatString(Buf, BufSize, "#%02X%02X%02X", ImClamp(ColorInt[0], 0, 255), ImClamp(ColorInt[1], 0, 255), ImClamp(ColorInt[2], 0, 255));
}