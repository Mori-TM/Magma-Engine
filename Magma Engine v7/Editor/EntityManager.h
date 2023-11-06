void EditorEntities()
{
	ImGui::Begin("Entities");
	{
		Mutex.lock();
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			for (uint32_t j = 0; j < EntityCount; j++)
				Entities[j].Selected = false;
			Entities[SelectedEntity].Selected = true;

			char Buffer[2048];
			sprintf(Buffer, "%d %s\n", i, Entities[i].Name);

			if (ImGui::Selectable(Buffer, Entities[i].Selected))
			{
				SelectedEntity = i;
				ImGui::SetWindowFocus("Entity Inspector");
			}
		}
		Mutex.unlock();

	//	ImGui::BeginChild("Pop Up");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Add Empty Entity"))
				AddEntity(UINT32_MAX);

			for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
			{
				char Name[64];
				snprintf(Name, 64, "Add %s Entity", ComponentNames[i]);
				if (ImGui::Selectable(Name))
					AddEntity(i);
			}

			/*
			if (ImGui::Selectable("Add Mesh Entity"))
				AddEntity(COMPONENT_TYPE_MESH);

			if (ImGui::Selectable("Add Camera Entity"))
				AddEntity(COMPONENT_TYPE_CAMERA);

			if (ImGui::Selectable("Add Audio Entity"))
				AddEntity(COMPONENT_TYPE_AUDIO);

			if (ImGui::Selectable("Add Animation Entity"))
				AddEntity(COMPONENT_TYPE_ANIMATION);

			if (ImGui::Selectable("Add Light Entity"))
				AddEntity(COMPONENT_TYPE_LIGHT);
				*/
			ImGui::EndPopup();
		}
	//	ImGui::EndChild();
	}
	ImGui::End();
}