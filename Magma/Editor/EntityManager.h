void ResetEntityMesh(EntityInfo *Entity) {
    Entity->Mesh.MeshIndex = 0;
    strcpy(Entity->Mesh.Name, "None");
}

void ResetEntityMaterial(EntityInfo *Entity) {
    Entity->Material.MaterialIndex = 0;
    Entity->Material.Color = Vec3f(1.0);
    strcpy(Entity->Material.Name, "None");
}

void ResetEntityCamera(EntityInfo *Entity) {
    strcpy(Entity->Camera.Name, "None");
    Entity->Camera.FOV = 75.0;
    Entity->Camera.NearPlane = 0.01;
    Entity->Camera.FarPlane = 1000.0;
}

void AddEntity(EntityInfo *Entity) {
    ResetEntityMesh(Entity);
    ResetEntityMaterial(Entity);
    ResetEntityCamera(Entity);

    Entity->Translate = Vec3f(0.0);
    Entity->Rotate = Vec3f(0.0);
    Entity->Scale = Vec3f(1.0);
    Entity->Selected = false;

    uint32_t Count = 0;

    for (uint32_t i = 0; i < EntityCount; i++) {
        if (strstr(Entities[i].Name, "Entity") != 0) Count++;
    }

    if (Count > 0) sprintf(Entity->Name, "Entity (%d)", Count);
    else strcpy(Entity->Name, "Entity");

    Entities = (EntityInfo *)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
    Entities[EntityCount] = *Entity;
    SelectedEntity = EntityCount;
    EntityCount++;
}

void EditorEntities() {
    ImGui::Begin("Entities");
    {
        for (uint32_t i = 0; i < EntityCount; i++) {
            for (uint32_t j = 0; j < EntityCount; j++) {
                Entities[j].Selected = false;
            }

            Entities[SelectedEntity].Selected = true;

            char Buffer[2048];
            sprintf(Buffer, "%d %s\n", i, Entities[i].Name);

            if (ImGui::Selectable(Buffer, Entities[i].Selected)) {
                SelectedEntity = i;
            }
        }

        ImGui::BeginChild("Pop Up");

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::Selectable("Add Empty Entity")) {
                EntityInfo Entity;

                for (uint32_t i = 0; i < COMPONENT_COUNT; i++) {
                    Entity.UsedComponents[i] = false;
                }

                AddEntity(&Entity);
            }

            if (ImGui::Selectable("Add Mesh Entity")) {
                EntityInfo Entity;

                for (uint32_t i = 0; i < COMPONENT_COUNT; i++) {
                    Entity.UsedComponents[i] = false;
                }

                Entity.UsedComponents[COMPONENT_TYPE_MESH] = true;
                //	Entity.UsedComponents[COMPONENT_TYPE_MATERIAL] = true;

                AddEntity(&Entity);
            }

            if (ImGui::Selectable("Add Camera Entity")) {
                EntityInfo Entity;

                for (uint32_t i = 0; i < COMPONENT_COUNT; i++) {
                    Entity.UsedComponents[i] = false;
                }

                Entity.UsedComponents[COMPONENT_TYPE_CAMERA] = true;

                AddEntity(&Entity);
            }

            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }
    ImGui::End();
}