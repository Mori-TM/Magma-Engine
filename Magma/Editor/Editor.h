int32_t Frame = 0;
int32_t FinalTime = 0;
int32_t InitTime = 0;
float FPS = 0.0;
float MS = 0.0;

void EngineInitEditor() {
    InitLua();
    /*
       SceneMesh MeshInfo;

       strcpy(MeshInfo.Name, "Plane");
       strcpy(MeshInfo.Path, "None");
       MeshInfo.VertexCount = ARRAY_SIZE(PlaneVertices);
       MeshInfo.Vertices = (SceneVertex*)&PlaneVertices;
       MeshInfo.VertexBuffer = PlaneVertexBuffer;

       MeshInfo.IndexCount = ARRAY_SIZE(PlaneIndices);
       MeshInfo.Indices = (uint32_t*)&PlaneIndices;
       MeshInfo.IndexBuffer = PlaneIndexBuffer;

       AddMesh("Plane", &MeshInfo);


       strcpy(MeshInfo.Name, "Bean");
       strcpy(MeshInfo.Path, "None");
       MeshInfo.VertexCount = BeanVertexCount;
       MeshInfo.Vertices = BeanVertices;
       MeshInfo.VertexBuffer = BeanVertexBuffer;

       MeshInfo.IndexCount = BeanIndexCount;
       MeshInfo.Indices = BeanIndices;
       MeshInfo.IndexBuffer = BeanIndexBuffer;

       AddMesh("Bean", &MeshInfo);

       EntityInfo Entity;
       for (uint32_t i = 0; i < COMPONENT_COUNT; i++)
            Entity.UsedComponents[i] = false;

       Entity.UsedComponents[COMPONENT_TYPE_MESH] = true;
       Entity.UsedComponents[COMPONENT_TYPE_MATERIAL] = false;
       Entity.UsedComponents[COMPONENT_TYPE_CAMERA] = false;

       Entity.Translate = Vec3f(0.0);
       Entity.Rotate = Vec3f(0.0);
       Entity.Scale = Vec3f(10.0);
       Entity.Selected = false;
       Entity.Mesh.MeshIndex = 0;
       Entity.Material.MaterialIndex = -1;
       strcpy(Entity.Mesh.Name, "Plane");
       strcpy(Entity.Name, "Plane");

       Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
       Entities[EntityCount] = Entity;
       SelectedEntity = EntityCount;
       EntityCount++;

       Entity.Translate = Vec3(0.0, 1.0, 0.0);
       Entity.Rotate = Vec3f(0.0);
       Entity.Scale = Vec3f(1.0);
       Entity.Selected = false;
       Entity.Mesh.MeshIndex = 1;
       strcpy(Entity.Mesh.Name, "Bean");
       strcpy(Entity.Name, "Bean");

       Entities = (EntityInfo*)realloc(Entities, (EntityCount + 1) * sizeof(EntityInfo));
       Entities[EntityCount] = Entity;
       SelectedEntity = EntityCount;
       EntityCount++;
     */
}

void EngineDestroyEditor() {
    free(Entities);
    DeInitLua();
}

void EngineDrawEditor() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGuiIO *IO = &ImGui::GetIO();

    EditorDrawScene();
    EditorEntities();
    EditorInspector();
    EditorAssetBrowser();

    ImGui::Begin("Debug");
    {
        Frame++;
        FinalTime = time(NULL);

        if (FinalTime - InitTime > 0) {
            FPS = Frame / (FinalTime - InitTime);
            Frame = 0;
            InitTime = FinalTime;

            MS = 1000 / FPS;
        }

        ImGui::Text("FPS: %f MS: %f", FPS, MS);
        ImGui::Text("Shadow Render Time: %f ms", ShadowRenderingTime);
        ImGui::Text("Scene Render Time: %f ms", SceneRenderingTime);
        ImGui::Text("SwapChain Render Time: %f ms", SwapChainRenderingTime);
        ImGui::NewLine();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("Renderer Info:");
        ImGui::PopStyleColor();
        ImGui::Text("Swap Chain Image Count: %d", VkRenderer.SwapChainImageCount);
        ImGui::Text("Image Count: %d", VkRenderer.ImageCount);
        ImGui::Text("Render Pass Count: %d", VkRenderer.RenderPassCount);
        ImGui::Text("Pipeline Count: %d", VkRenderer.PipelineCount);
        ImGui::Text("Framebuffer Count: %d", VkRenderer.FramebufferCount);
        ImGui::Text("Frames in Flight: %d", MAX_FRAMES_IN_FLIGHT);
        ImGui::Text("Current Frame: %d", VkRenderer.CurrentFrame);
        ImGui::Text("Descriptor Set Layout Count: %d", VkRenderer.DescriptorSetLayoutCount);
        ImGui::Text("Descriptor Set Count: %d", VkRenderer.DescriptorSets.Size);
        ImGui::Text("Buffer Count: %d", VkRenderer.Buffers.Size);
        ImGui::Text("Uniform Buffer Count: %d", VkRenderer.UniformBufferCount);
        ImGui::Text("Texture Image Count: %d", VkRenderer.TextureImages.Size);
        ImGui::Text("Sampler Count: %d", VkRenderer.Sampler.Size);
        ImGui::Text("Max Msaa Sample Count: %d", VkRenderer.MsaaSamples);
        ImGui::Text("Dynamic Descriptor Pool Count: %d", VkRenderer.DynamicDescriptorPools.DescriptorPoolCount);
        ImGui::Text("Static Descriptor Pool Count: %d", VkRenderer.StaticDescriptorPools.DescriptorPoolCount);
        ImGui::Text("Frame: %d", SDL_GetTicks());
        ImGui::Text("Used CPU Memory: %f MB", WaveGetUsedMemory() * 0.000001);

        ImGui::NewLine();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("GPU Info:");
        ImGui::PopStyleColor();

        ImGui::Text("GPU: %s", VkRenderer.PhysicalDeviceProperties.deviceName);
        ImGui::Text("API version: %d.%d.%d", VK_VERSION_MAJOR(VkRenderer.PhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(VkRenderer.PhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(VkRenderer.PhysicalDeviceProperties.apiVersion));
        ImGui::Text("Device ID: %d", VkRenderer.PhysicalDeviceProperties.deviceID);
        ImGui::Text("Driver Version: %d", VkRenderer.PhysicalDeviceProperties.driverVersion);
        ImGui::Text("Max Framebuffer Color Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferColorSampleCounts);
        ImGui::Text("Max Framebuffer Depth Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferDepthSampleCounts);
        ImGui::Text("Max Framebuffer Stencil Sample Count: %d", VkRenderer.PhysicalDeviceProperties.limits.framebufferStencilSampleCounts);
        ImGui::Text("Max Bound Descriptor Sets: %d", VkRenderer.PhysicalDeviceProperties.limits.maxBoundDescriptorSets);
        ImGui::Text("Max Color Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxColorAttachments);
        ImGui::Text("Max Descriptor Set Input Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments);
        ImGui::Text("Max Descriptor Set Sampled Images: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetSampledImages);
        ImGui::Text("Max Descriptor Set Samplers: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetSamplers);
        ImGui::Text("Max Descriptor Set Uniform Buffers: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
        ImGui::Text("Max Descriptor Set Uniform Buffers Dynamic: %d", VkRenderer.PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
        ImGui::Text("Max Fragment Input Components: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFragmentInputComponents);
        ImGui::Text("Max Fragment Output Attachments: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFragmentOutputAttachments);
        ImGui::Text("Max Framebuffer Width: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFramebufferWidth);
        ImGui::Text("Max Framebuffer Height: %d", VkRenderer.PhysicalDeviceProperties.limits.maxFramebufferHeight);
        ImGui::Text("Max Memory Allocation Count: %d", VkRenderer.PhysicalDeviceProperties.limits.maxMemoryAllocationCount);
        ImGui::Text("Max Push Constants Size: %d", VkRenderer.PhysicalDeviceProperties.limits.maxPushConstantsSize);
        ImGui::Text("Max Viewport Dimensions: %d %d", VkRenderer.PhysicalDeviceProperties.limits.maxViewportDimensions[0], VkRenderer.PhysicalDeviceProperties.limits.maxViewportDimensions[1]);
        ImGui::Text("Max Viewports: %d", VkRenderer.PhysicalDeviceProperties.limits.maxViewports);
    }
    ImGui::End();

    ImGui::Begin("Scene Settings");
    {
        ImGui::ColorEdit3("Clear Color", (float *)&ClearColor);
        ImGui::DragFloat3("Light Direction", (float *)&SceneFragmentUBO.LightDirection, 0.1, -10000.0, 10000.0);

        ImGui::SliderFloat("Split lambda", &CascadeSplitLambda, 0.001, 1.0);
        ImGui::SliderFloat("Near Clip", &CascadeNearClip, 0.0001, 2.0);
        ImGui::SliderFloat("Far Clip", &CascadeFarClip, 10.0, 1000.0);

        const char *names[] = { "Split 0", "Split 1", "Split 2", "Split 3" };

        for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
            ImGui::DragFloat(names[i], &Cascades[i].SplitDepth, 0.01, -1000.0, 1000.0);
        }

        if (ImGui::Button("Reload Shaders")) ReloadShaders = true;

        ImGui::Checkbox("Scene Backface Culling", &SceneBackfaceCulling);
        ImGui::Checkbox("Shadow Backface Culling", &ShadowBackfaceCulling);
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Scene", "STRG+O")) {
            }

            if (ImGui::MenuItem("Save Scene", "STRG+S")) {
            }

            if (ImGui::MenuItem("New Scene", "STRG+N")) {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Settings")) {
            ImGui::DragFloat("Font Size", &IO->FontGlobalScale, 0.005, 0.1, 5.0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::EndMenu();
        }

        ImGui::PushFont(IconFontSmall);
        ImGui::SetCursorPosX((WindowWidth * 0.5));
        static char ButtonType = 'G';
        ImGui::PushID("Play Button");

        if (ImGui::Button(&ButtonType)) {
            GameMode = !GameMode;

            if (GameMode) ButtonType = 'L';
            else ButtonType = 'G';

            LuaOnStart = true;
        }

        ImGui::PopFont();

        ImGui::EndMainMenuBar();
    }

    UpdateLuaScripts();
}