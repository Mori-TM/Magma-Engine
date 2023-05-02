void ResetEntityMesh(EntityInfo* Entity)
{
	Entity->Mesh.MeshIndex = 0;
	strcpy(Entity->Mesh.Name, "None");
}

void ResetEntityMaterial(EntityInfo* Entity)
{
	Entity->Material.MaterialIndex = 0;
	strcpy(Entity->Material.Name, "None");
}

void ResetEntityCamera(EntityInfo* Entity)
{
	strcpy(Entity->Camera.Name, "None");
	Entity->Camera.FOV = 75.0;
	Entity->Camera.NearPlane = 0.01;
	Entity->Camera.FarPlane = 1000.0;
}

void ResetEntityAnimation(EntityInfo* Entity)
{
	strcpy(Entity->Animation.Name, "None");
	Entity->Animation.AnimationIndex = 0;
}

void AddEntity(uint32_t UsedComponent)
{
	Mutex.lock();
	EntityInfo Entity;
	for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
		Entity.UsedComponents[i] = false;

	if (UsedComponent != UINT32_MAX)
		Entity.UsedComponents[UsedComponent] = true;

	ResetEntityMesh(&Entity);
	ResetEntityMaterial(&Entity);
	ResetEntityCamera(&Entity);
	ResetEntityAnimation(&Entity);
	
	Entity.Translate = Vec3f(0.0);
	Entity.Rotate = Vec3f(0.0);
	Entity.Scale = Vec3f(1.0);
	Entity.Selected = false;
	
	uint32_t Count = 0;
	for (uint32_t i = 0; i < EntityCount; i++)
		if (strstr(Entities[i].Name, "Entity") != 0)
			Count++;

	if (Count > 0)
		sprintf(Entity.Name, "Entity (%d)", Count);
	else
		strcpy(Entity.Name, "Entity");
	
	Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
	Entities[EntityCount] = Entity;
	SelectedEntity = EntityCount++;

	Mutex.unlock();
}

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
				SelectedEntity = i;			
		}
		Mutex.unlock();

		ImGui::BeginChild("Pop Up");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Add Empty Entity"))
				AddEntity(UINT32_MAX);

			if (ImGui::Selectable("Add Mesh Entity"))
				AddEntity(COMPONENT_TYPE_MESH);

			if (ImGui::Selectable("Add Camera Entity"))
				AddEntity(COMPONENT_TYPE_CAMERA);

			if (ImGui::Selectable("Add Audio Entity"))
				AddEntity(COMPONENT_TYPE_AUDIO);

			if (ImGui::Selectable("Add Animation Entity"))
				AddEntity(COMPONENT_TYPE_ANIMATION);

			ImGui::EndPopup();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}