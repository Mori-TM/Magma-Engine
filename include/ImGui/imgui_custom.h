#include <stdint.h>

#include "imgui.h"

#pragma once

typedef enum
{
	ICON_BUTTON_TYPE_LARGE = 0x0,
	ICON_BUTTON_TYPE_SMALL = 0x1,
	ICON_BUTTON_TYPE_EXT = 0x2,
} IconButtonTypes;

extern ImVec4 ImGuiIconButtonColor;

extern ImFont* ImGuiIconFontLarge;
extern ImFont* ImGuiIconFontSmall;
extern ImFont* ImGuiIconFontExt;

bool ImGuiIconButton(const char* Icon, const char* ID, uint32_t IconType, const ImVec2& Size = ImVec2(0, 0));
bool ImGuiImageButtonExt(ImTextureID texture_id, const ImVec2& size, const ImVec2& Scale, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
bool ImGuiImageButtonEx(ImGuiID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
void ImGuiVec3Control(const char* label, float* values, float resetValue, float columnWidth);
void ImGuiSetPosPaddingX(float Pos);
bool ImGuiImageButtonID(const char* ID, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
void ImGuiGetMenuBarRectCurrentWindow(ImVec2* React);//React[0] -> Min; React[1] -> Max
ImU32 ImGuiColorConvertFloat4ToU32GammaCorrected(const ImVec4& in, bool GammaCorrected);
void ImGuiGet32bitColorHexCode(int ColorInt[], uint32_t BufSize, char* Buf);
