void EditorEntities()
{
	ImGui::Begin("Entities");
	{
		for (uint32_t i = 0; i < EntityCount; i++)
		{
			for (uint32_t j = 0; j < EntityCount; j++)
				Entities[j].Selected = false;
			Entities[SelectedEntity].Selected = true;

			char Buffer[2048];
			ssprintf(Buffer, "%d %s\n", i, Entities[i].Name);

			if (ImGui::Selectable(Buffer, Entities[i].Selected))
			{
				SelectedEntity = i;
				ImGui::SetWindowFocus("Entity Inspector");
			}
		}

	//	ImGui::BeginChild("Pop Up");
		if (ImGui::BeginPopupContextWindow())
		{
		//	if (ImGui::Selectable("Add Empty Entity"))
		//		AddEntity(COMPONENT_TYPE_EMPTY);

			for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
			{
				char Name[64];
				ssprintf(Name, "Add %s Entity", ComponentNames[i]);
				if (ImGui::Selectable(Name))
					AddDefaultEntity(i);
			}

			ImGui::EndPopup();
		}
	//	ImGui::EndChild();
	}
	ImGui::End();
}