void EditorDrawScene()
{
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("###DockSpace", (bool*)0, window_flags);
	{
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	ImGui::End();	
	
	

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));
	ImGui::Begin("Scene");
	{
		ImGui::BeginChild("SceneRender");
		{
			ImVec2 WindowSize = ImGui::GetWindowSize();

			/*
			const float Scale = ((float)SceneScaling / 100);
			uint32_t x = WindowSize.x;
			uint32_t y = WindowSize.y;
			if (SceneWidth / Scale != x || SceneHeight / Scale != y)
			{
				printf("%d/%d, %d/%d\n", SceneWidth, uint32_t(x * Scale), SceneHeight, uint32_t(y * Scale));

				ForceResizeEvent = true;
			}				

			ImGuiScenePosX = ImGui::GetWindowPos().x;
			ImGuiScenePosY = ImGui::GetWindowPos().y;

			ImGuiSceneWidth = WindowSize.x;
			ImGuiSceneHeight = WindowSize.y;
			*/

			static ImVec2 PrevWindowSize = ImGui::GetWindowSize();
			// Check if window size has changed
			if (PrevWindowSize.x != WindowSize.x ||
				PrevWindowSize.y != WindowSize.y) {
				// Handle window resize event
				PrevWindowSize = ImGui::GetWindowSize();
				printf("%d, %d\n", SceneWidth, SceneHeight);

				ForceResizeEvent = true;
				PushEventSDL(0, 0);
			}

			ImGuiScenePosX = ImGui::GetWindowPos().x;
			ImGuiScenePosY = ImGui::GetWindowPos().y;

			ImGuiSceneWidth = WindowSize.x;
			ImGuiSceneHeight = WindowSize.y;

			ImGui::Image(&GetDescriptorSet(SSRDescriptorSet)[VkRenderer.CurrentFrame], WindowSize);
		}
		ImGui::EndChild();

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
			IsEditorActive = false;
		else
			IsEditorActive = true;
	}
	ImGui::End();
	ImGui::PopStyleVar();
}