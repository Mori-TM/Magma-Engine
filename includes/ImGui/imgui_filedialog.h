#include <ctime>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <filesystem>
#include <unordered_map>

#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <OpenVK/stb_image.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <lmcons.h>
#pragma comment(lib, "Shell32.lib")
#else
#include <unistd.h>
#include <pwd.h>
#endif

#define IM_FILE_DIALOG_PATH_LENGTH 4096
#define IM_ICON_SIZE ImGui::GetFont()->FontSize + 3
#define IM_GUI_ELEMENT_SIZE std::max(GImGui->FontSize + 10.f, 24.f)
#define IM_DEFAULT_ICON_SIZE 32

namespace ImGui
{
	std::vector<uint32_t> IconArray;
	std::unordered_map<std::string, uint32_t> IconMap;
	uint32_t(*LoadTexture)(uint32_t Width, uint32_t Height, uint8_t* Data);
	void (*DestroyTexture)(uint32_t TextureIndex);

	uint32_t GetIcon(const char* Path)
	{
#ifdef _WIN32
		size_t PathLength = strlen(Path);
		if (PathLength >= IM_FILE_DIALOG_PATH_LENGTH)
			return -1;
		wchar_t PathW[IM_FILE_DIALOG_PATH_LENGTH];
		char PathA[IM_FILE_DIALOG_PATH_LENGTH];
	
		for (size_t i = 0; i < PathLength; i++)
		{
			PathW[i] = Path[i];
			if (PathW[i] == '/')
				PathW[i] = '\\';
			PathA[i] = PathW[i];
		}

		if (IconMap.find(PathA) != IconMap.end())
			return IconMap[PathA];

		UINT Flags = SHGFI_ICON | SHGFI_LARGEICON;
		DWORD Attrs = GetFileAttributesW(PathW);

		SHFILEINFOW FileInfo = { 0 };
		SHGetFileInfoW(PathW, Attrs, &FileInfo, sizeof(SHFILEINFOW), Flags);

		if (FileInfo.hIcon == NULL)
			return -1;

		ICONINFO IconInfo = { 0 };
		GetIconInfo(FileInfo.hIcon, &IconInfo);

		if (IconInfo.hbmColor == nullptr)
			return -1;

		DIBSECTION DS;
		GetObject(IconInfo.hbmColor, sizeof(DS), &DS);
		int ByteSize = DS.dsBm.bmWidth * DS.dsBm.bmHeight * (DS.dsBm.bmBitsPixel / 8);

		if (ByteSize == 0)
			return -1;

		uint8_t* Data = (uint8_t*)malloc(ByteSize);
		GetBitmapBits(IconInfo.hbmColor, ByteSize, Data);

		//BGR to RGB
		for (int32_t i = 0; i < DS.dsBm.bmWidth * DS.dsBm.bmHeight; i++)
		{
			int32_t Index = i * 4;

			uint8_t B = Data[Index];
			uint8_t R = Data[Index + 2];

			Data[Index] = R;
			Data[Index + 2] = B;
		}

		//Data gets freed by this function
		IconMap[PathA] = LoadTexture(DS.dsBm.bmWidth, DS.dsBm.bmHeight, Data);

		free(Data);

		return IconMap[PathA];
#else
#error Linux is still not fully implemented
		if (m_icons.count(path.u8string()) > 0)
			return m_icons[path.u8string()];

		std::string pathU8 = path.u8string();
		ImageData ErrorImage;
		memset(&ErrorImage, 0, sizeof(ImageData));

		std::error_code ec;
		int iconID = 1;
		if (std::filesystem::is_directory(path, ec))
			iconID = 0;

		// check if icon is already loaded
		auto itr = std::find(m_iconIndices.begin(), m_iconIndices.end(), iconID);
		if (itr != m_iconIndices.end()) {
			const std::string& existingIconFilepath = m_iconFilepaths[itr - m_iconIndices.begin()];
			m_icons[pathU8] = m_icons[existingIconFilepath];
			return m_icons[pathU8];
		}

		m_iconIndices.push_back(iconID);
		m_iconFilepaths.push_back(pathU8);

		ImVec4 wndBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

		// light theme - load default icons
		if ((wndBg.x + wndBg.y + wndBg.z) / 3.0f > 0.5f) {
			uint8_t* data = (uint8_t*)ifd::GetDefaultFileIcon();
			if (iconID == 0)
				data = (uint8_t*)ifd::GetDefaultFolderIcon();
			m_icons[pathU8] = this->CreateTexture(data, DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE, 0);
		}
		// dark theme - invert the colors
		else {
			uint8_t* data = (uint8_t*)ifd::GetDefaultFileIcon();
			if (iconID == 0)
				data = (uint8_t*)ifd::GetDefaultFolderIcon();

			uint8_t* invData = (uint8_t*)malloc(DEFAULT_ICON_SIZE * DEFAULT_ICON_SIZE * 4);
			for (int y = 0; y < 32; y++) {
				for (int x = 0; x < 32; x++) {
					int index = (y * DEFAULT_ICON_SIZE + x) * 4;
					invData[index + 0] = 255 - data[index + 0];
					invData[index + 1] = 255 - data[index + 1];
					invData[index + 2] = 255 - data[index + 2];
					invData[index + 3] = data[index + 3];
				}
			}
			m_icons[pathU8] = this->CreateTexture(invData, DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE, 0);

			free(invData);
		}

		return m_icons[pathU8];
#endif
	}

	void InitFileDialog()
	{

	}

	void DeInitFileDialog()
	{

	}

	void RenderFileDialog()
	{
		/*
		bool noBackHistory = false;

		ImGui::PushStyleColor(ImGuiCol_Button, 0);
		if (noBackHistory) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if (ImGui::ArrowButtonEx("##back", ImGuiDir_Left, ImVec2(IM_GUI_ELEMENT_SIZE, IM_GUI_ELEMENT_SIZE), m_backHistory.empty() * ImGuiItemFlags_Disabled)) {
		//	std::filesystem::path newPath = m_backHistory.top();
		//	m_backHistory.pop();
		//	m_forwardHistory.push(m_currentDirectory);
		//
		//	m_setDirectory(newPath, false);
		}
		if (noBackHistory) ImGui::PopStyleVar();
		ImGui::SameLine();

		if (noForwardHistory) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if (ImGui::ArrowButtonEx("##forward", ImGuiDir_Right, ImVec2(IM_GUI_ELEMENT_SIZE, IM_GUI_ELEMENT_SIZE), m_forwardHistory.empty() * ImGuiItemFlags_Disabled)) {
			std::filesystem::path newPath = m_forwardHistory.top();
			m_forwardHistory.pop();
			m_backHistory.push(m_currentDirectory);

			m_setDirectory(newPath, false);
		}
		if (noForwardHistory) ImGui::PopStyleVar();
		ImGui::SameLine();

		if (ImGui::ArrowButtonEx("##up", ImGuiDir_Up, ImVec2(IM_GUI_ELEMENT_SIZE, IM_GUI_ELEMENT_SIZE))) {
			if (m_currentDirectory.has_parent_path())
				m_setDirectory(m_currentDirectory.parent_path());
		}

		std::filesystem::path curDirCopy = m_currentDirectory;
		if (PathBox("##pathbox", curDirCopy, m_pathBuffer, ImVec2(-250, IM_GUI_ELEMENT_SIZE)))
			m_setDirectory(curDirCopy);
		ImGui::SameLine();

		if (FavoriteButton("##dirfav", std::count(m_favorites.begin(), m_favorites.end(), m_currentDirectory.u8string()))) {
			if (std::count(m_favorites.begin(), m_favorites.end(), m_currentDirectory.u8string()))
				RemoveFavorite(m_currentDirectory.u8string());
			else
				AddFavorite(m_currentDirectory.u8string());
		}
		ImGui::SameLine();
		ImGui::PopStyleColor();

		if (ImGui::InputTextEx("##searchTB", "Search", m_searchBuffer, 128, ImVec2(-FLT_MIN, IM_GUI_ELEMENT_SIZE), 0)) // TODO: no hardcoded literals
		{

		}
		//	m_setDirectory(m_currentDirectory, false); // refresh
		*/
	}

	class FileDialog
	{
	public:
		bool Open = false;
		bool OpenPopUp = false;

		bool OpenFileDialog(const char* Name, const char* Extensions)
		{
			if (Open && !OpenPopUp)
			{
				ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
				ImGui::OpenPopup(Name);
				OpenPopUp = true;
			}

			if (ImGui::BeginPopupModal(Name, &Open, ImGuiWindowFlags_NoScrollbar))
			{


				ImGui::EndPopup();
				
				return false;
			}

			OpenPopUp = false;

			return false;
		}
	};

	
}