#include "../include/init.h"

void createInstance(App* app){
    uint32_t requiredExtensionsCount = 2;
    const char *requiredExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_DISPLAY_EXTENSION_NAME
    };

    EXPECT(vkCreateInstance(&(VkInstanceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &(VkApplicationInfo) {
                    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                    .pApplicationName = app->applicationName,
                    .pEngineName = app->engineName,
                    .apiVersion = app->apiVersion,
            },
            .enabledExtensionCount = requiredExtensionsCount,
            .ppEnabledExtensionNames = requiredExtensions,
    }, NULL, &app->instance), "Couldn't create instance")
}

void pickPhysicalDevice(App* app){
    uint32_t count;
    EXPECT(vkEnumeratePhysicalDevices(app->instance, &count, NULL), "Couldn't enumerate physical devices count")
    EXPECT(count == 0, "Couldn't find a vulkan supported physical device")
    VkResult result = vkEnumeratePhysicalDevices(app->instance, &(uint32_t){1}, &app->physicalDevice);
    if(result != VK_INCOMPLETE) EXPECT(result, "Couldn't enumerate physical devices")
}

void createSurface(App* app){
    // Creating Direct2Display surface
    uint32_t displayPropertyCount = 0;

	// Get display property
	EXPECT(vkGetPhysicalDeviceDisplayPropertiesKHR(app->physicalDevice, &displayPropertyCount, NULL), "Couldn't get physical device display properties count")
	EXPECT(displayPropertyCount==0, "Couldn't find any available physical device display properties")
    printf("Display properties: %d\n", displayPropertyCount);
    VkDisplayPropertiesKHR* pDisplayProperties = malloc(sizeof(VkDisplayPropertiesKHR)*displayPropertyCount);
	EXPECT(vkGetPhysicalDeviceDisplayPropertiesKHR(app->physicalDevice, &displayPropertyCount, pDisplayProperties), "Couldn't get physical device display properties")

	// Get plane property
	uint32_t planePropertyCount = 0;
	EXPECT(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(app->physicalDevice, &planePropertyCount, NULL), "Couldn't get physical device plane properties count")
	EXPECT(planePropertyCount==0, "Couldn't find any available display plane properties")
    VkDisplayPlanePropertiesKHR* pPlaneProperties = malloc(sizeof(VkDisplayPlanePropertiesKHR)*planePropertyCount);
	EXPECT(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(app->physicalDevice, &planePropertyCount, pPlaneProperties), "Couldn't get physical device plane properties")

	VkDisplayKHR display = VK_NULL_HANDLE;
	VkDisplayModeKHR displayMode;
	VkDisplayModePropertiesKHR* pModeProperties;

	int foundMode = 0;
	for(uint32_t i = 0; i < displayPropertyCount; i++){
		display = pDisplayProperties[i].display;
		uint32_t modeCount = 0;
		EXPECT(vkGetDisplayModePropertiesKHR(app->physicalDevice, display, &modeCount, NULL), "Couldn't get display mode properties count")
		EXPECT(modeCount==0, "Couldn't find any available display mode properties")
        pModeProperties = malloc(sizeof(VkDisplayModePropertiesKHR)*modeCount);
		EXPECT(vkGetDisplayModePropertiesKHR(app->physicalDevice, display, &modeCount, pModeProperties), "Couldn't get display mode properties")
	
    	for(uint32_t j = 0; j < modeCount; j++){

			const VkDisplayModePropertiesKHR* mode = &pModeProperties[j];

			if(mode->parameters.visibleRegion.width == app->windowWidth && mode->parameters.visibleRegion.height == app->windowHeight){

				displayMode = mode->displayMode;
				foundMode = 1;
				break;
			}
		}
		if(foundMode)
			break;
		
		free(pModeProperties);
	}

    EXPECT(foundMode==0, "Couldn't find a display and a display mode")

	// Search for a best plane we can use
	uint32_t bestPlaneIndex = UINT32_MAX;
	VkDisplayKHR* pDisplays = NULL;
	for(uint32_t i = 0; i < planePropertyCount; i++){
		uint32_t planeIndex=i;
		uint32_t displayCount;
		vkGetDisplayPlaneSupportedDisplaysKHR(app->physicalDevice, planeIndex, &displayCount, NULL);
		if (pDisplays)
            free(pDisplays);

		pDisplays = malloc(sizeof(VkDisplayKHR)*displayCount);
		vkGetDisplayPlaneSupportedDisplaysKHR(app->physicalDevice, planeIndex, &displayCount, pDisplays);

		// Find a display that matches the current plane
		bestPlaneIndex = UINT32_MAX;
		for(uint32_t j = 0; j < displayCount; j++){
			if(display == pDisplays[j]){
				bestPlaneIndex = i;
				break;
			}
		}
		if(bestPlaneIndex != UINT32_MAX)
			break;
	}

    EXPECT(bestPlaneIndex==UINT32_MAX, "Couldn't find a plane for displaying")

	VkDisplayPlaneCapabilitiesKHR planeCap;
	vkGetDisplayPlaneCapabilitiesKHR(app->physicalDevice, displayMode, bestPlaneIndex, &planeCap);
	VkDisplayPlaneAlphaFlagBitsKHR alphaMode = (VkDisplayPlaneAlphaFlagBitsKHR)0;

	if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR)
		alphaMode = VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR;
	else if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR)
		alphaMode = VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR;
	else if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR)
		alphaMode = VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR;
	else if (planeCap.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR)
		alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;

	VkDisplaySurfaceCreateInfoKHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .displayMode = displayMode,
        .planeIndex = bestPlaneIndex,
        .planeStackIndex = pPlaneProperties[bestPlaneIndex].currentStackIndex,
        .transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .globalAlpha = 1.0,
        .alphaMode = alphaMode,
        .imageExtent.width = app->windowWidth,
        .imageExtent.height = app->windowHeight,
    };

	EXPECT(vkCreateDisplayPlaneSurfaceKHR(app->instance, &surfaceInfo, NULL, &app->surface), "Couldn't create surface")
	free(pDisplays);
	free(pModeProperties);
	free(pDisplayProperties);
	free(pPlaneProperties);
}

void createLogicalDevice(App* app){
    queueFamilySelect(app);

    EXPECT(vkCreateDevice(app->physicalDevice, &(VkDeviceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = app->queueFamily,
                    .queueCount = 1,
                    .pQueuePriorities = &(float){1.0},
            },
            .queueCreateInfoCount = 1,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = &(const char *) {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
    }, NULL, &app->device), "Couldn't create device and queues")

    vkGetDeviceQueue(app->device, app->queueFamily, 0, &app->queue);
}

void queueFamilySelect(App* app){
    app->queueFamily = UINT32_MAX;

    // Get queue family properties of the physical device
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(app->physicalDevice, &queueFamilyCount, NULL);
	EXPECT(queueFamilyCount == 0, "Couldn't get the family queue count")
	VkQueueFamilyProperties *queueFamilyProperties = malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(app->physicalDevice, &queueFamilyCount, queueFamilyProperties);

    for (uint32_t i=0; i<queueFamilyCount; i++){
		if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			app->queueFamily = i;
            free(queueFamilyProperties);
            return;
		}
	}
    EXPECT(app->queueFamily == UINT32_MAX, "Couldn't find a suitable queue family")
    free(queueFamilyProperties);
}

void createSwapChain(App* app){
    VkSurfaceCapabilitiesKHR capabilities = surfaceCapabilitiesGet(app);
    VkSurfaceFormatKHR surfaceFormat = surfaceFormatsSelect(app);
    VkPresentModeKHR presentMode = surfacePresentModesSelect(app);

    app->format = surfaceFormat.format;
    app->colorSpace = surfaceFormat.colorSpace;
    app->imageExtent = capabilities.currentExtent;

    EXPECT(vkCreateSwapchainKHR(app->device, &(VkSwapchainCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = app->surface,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &app->queueFamily,
            .clipped = true,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = capabilities.currentTransform,
            .imageExtent = app->imageExtent,
            .imageFormat = app->format,
            .imageColorSpace = app->colorSpace,
            .presentMode = presentMode,
            .minImageCount = clamp(app->swapchainBuffering, capabilities.minImageCount, capabilities.maxImageCount ? capabilities.maxImageCount : UINT32_MAX),
    }, NULL, &app->handle), "Couldn't create swapchain")

    swapchainImagesGet(app);
}

VkSurfaceCapabilitiesKHR surfaceCapabilitiesGet(App* app){
    VkSurfaceCapabilitiesKHR capabilities;
    EXPECT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physicalDevice, app->surface, &capabilities), "Failed to get surface capabilities")
    return capabilities;
}

VkSurfaceFormatKHR surfaceFormatsSelect(const App *app) {
    uint32_t formatCount;
    EXPECT(vkGetPhysicalDeviceSurfaceFormatsKHR(app->physicalDevice, app->surface, &formatCount, NULL), "Couldn't get surface formats count")
    VkSurfaceFormatKHR *formats = malloc(formatCount*sizeof(VkSurfaceFormatKHR));
    EXPECT(!formats, "Couldn't allocate formats memory")
    EXPECT(vkGetPhysicalDeviceSurfaceFormatsKHR(app->physicalDevice, app->surface, &formatCount, formats), "Couldn't get surface formats")

    uint32_t formatIndex = 0;

    for (int i = 0; i < formatCount; ++i) {
        VkSurfaceFormatKHR format = formats[i];
        if(format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
            formatIndex = i;
            break;
        }
    }

    VkSurfaceFormatKHR format = formats[formatIndex];
    free(formats);
    return format;
}

VkPresentModeKHR surfacePresentModesSelect(const App *app) {
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    uint32_t presentModeCount;
    EXPECT(vkGetPhysicalDeviceSurfacePresentModesKHR(app->physicalDevice, app->surface, &presentModeCount, NULL), "Couldn't get surface present modes count")
    VkPresentModeKHR *presentModes = malloc(presentModeCount*sizeof(VkPresentModeKHR));
    EXPECT(!presentModes, "Couldn't allocate present modes memory")
    EXPECT(vkGetPhysicalDeviceSurfacePresentModesKHR(app->physicalDevice, app->surface, &presentModeCount, presentModes), "Couldn't get surface present modes")

    uint32_t presentModeIndex = UINT32_MAX;

    for (int i = 0; i < presentModeCount; ++i) {
        VkPresentModeKHR mode = presentModes[i];
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentModeIndex = i;
            break;
        }
    }

    if(presentModeIndex != UINT32_MAX) {
        presentMode = presentModes[presentModeIndex];
    }

    free(presentModes);
    return presentMode;
}

void swapchainImagesGet(App *app) {
    EXPECT(vkGetSwapchainImagesKHR(app->device, app->handle, &app->imageCount, NULL), "Couldn't get swapchain images count")
    app->images = malloc(app->imageCount*sizeof(VkImage));
    EXPECT(!app->images, "Couldn't allocate memory for swapchain images")
    EXPECT(vkGetSwapchainImagesKHR(app->device, app->handle, &app->imageCount, app->images), "Couldn't get swapchain images")
}

void createImageViews(App* app){
    app->imageViews = malloc(app->imageCount*sizeof(VkImageView));
    EXPECT(!app->imageViews, "Couldn't allocate memory for swapchain image views")

    for (int i = 0; i < app->imageCount; ++i) {
        EXPECT(vkCreateImageView(app->device, &(VkImageViewCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .format = app->format,
                .image = app->images[i],
                .components = app->swapchainComponentsMapping,
                .subresourceRange = (VkImageSubresourceRange) {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .layerCount = 1,
                        .levelCount = 1,
                },
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
        }, NULL, &app->imageViews[i]), "Couldn't create image view %i", i)
    }
}

void createRenderPass(App* app){
    VkFormat image_format = app->format;

    VkAttachmentReference color_attachment_references[] = {
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }
    };

    VkSubpassDescription subpass_descriptions[] = {
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = color_attachment_references,
        }
    };

    VkAttachmentDescription attachment_descriptions[] = {
        {
            .format = image_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        }
    };

    EXPECT(vkCreateRenderPass(app->device, &(VkRenderPassCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .subpassCount = sizeof(subpass_descriptions)/sizeof(*subpass_descriptions),
        .pSubpasses = (const VkSubpassDescription *) &subpass_descriptions,
        .attachmentCount = sizeof(attachment_descriptions)/sizeof(*attachment_descriptions),
        .pAttachments = attachment_descriptions,
        .pDependencies = &(VkSubpassDependency) {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,

            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        }
    }, NULL, &app->renderPass), "Couldn't create renderpass")
}

void createGraphicsPipeline(App* app){
    const char *shaderEntryFunctionName = "main";

    VkShaderModule vertexShaderModule = load_shader(app->draw.vertexShaderPath, app);
    VkShaderModule fragmentShaderModule = load_shader(app->draw.fragmentShaderPath, app);
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        (VkPipelineShaderStageCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .module = vertexShaderModule,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .pName = shaderEntryFunctionName,
        },
        (VkPipelineShaderStageCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .module = fragmentShaderModule,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pName = shaderEntryFunctionName,
        },
    };

    VkExtent2D imageExtent = app->imageExtent;

    VkViewport viewports[] = {
        {
            // .x =  (float) imageExtent.width / 2,
            .width = imageExtent.width, //(float) imageExtent.width / 2,
            // .y = (float) imageExtent.height / 2,
            .height = imageExtent.height, //(float) imageExtent.height / 2,
            .maxDepth = 1.0f,
        }
    };

    VkRect2D scissors[] = {
        {
            // .offset = {imageExtent.width/2, imageExtent.height/2},
            .extent = imageExtent //{imageExtent.width/2, imageExtent.height/2},
        }
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[] = {
        {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        }
    };

    EXPECT(vkCreatePipelineLayout(app->device, &(VkPipelineLayoutCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    }, NULL, &app->pipelineLayout), "Couldn't create pipeline layout")

    EXPECT(vkCreateGraphicsPipelines(app->device, NULL, 1, &(VkGraphicsPipelineCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .layout = app->pipelineLayout,
        .pVertexInputState = &(VkPipelineVertexInputStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        },
        .pInputAssemblyState = &(VkPipelineInputAssemblyStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = app->draw.topology,
        },
        .pStages = (const VkPipelineShaderStageCreateInfo *) &shaderStages,
        .stageCount = sizeof(shaderStages)/sizeof(*shaderStages),
        .pViewportState = &(VkPipelineViewportStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = sizeof(viewports)/sizeof(*viewports),
            .pViewports = viewports,
            .scissorCount = sizeof(scissors)/sizeof(*scissors),
            .pScissors = scissors,
        },
        .pRasterizationState = &(VkPipelineRasterizationStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .lineWidth = 1.0,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .cullMode = VK_CULL_MODE_BACK_BIT,
        },
        .pMultisampleState = &(VkPipelineMultisampleStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        },
        .pColorBlendState = &(VkPipelineColorBlendStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = sizeof(colorBlendAttachmentStates)/sizeof(*colorBlendAttachmentStates),
            .pAttachments = colorBlendAttachmentStates,
        },
        .renderPass = app->renderPass,
    }, NULL, &app->graphicsPipeline), "Couldn't create graphics pipeline")

    vkDestroyPipelineLayout(app->device, app->pipelineLayout, NULL);
    vkDestroyShaderModule(app->device, vertexShaderModule, NULL);
    vkDestroyShaderModule(app->device, fragmentShaderModule, NULL);
}

void createFramebuffers(App* app){
    uint32_t framebufferCount = app->imageCount;
    app->framebuffers = malloc(framebufferCount * sizeof(VkFramebuffer));
    EXPECT(app->framebuffers == NULL, "Couldn't allocate memory for framebuffers array")
    VkExtent2D framebufferExtent = app->imageExtent;

    for (int framebufferIndex = 0; framebufferIndex < framebufferCount; ++framebufferIndex) {
        EXPECT(vkCreateFramebuffer(app->device, &(VkFramebufferCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .layers = 1,
            .renderPass = app->renderPass,
            .width = framebufferExtent.width,
            .height = framebufferExtent.height,
            .attachmentCount = 1,
            .pAttachments = &app->imageViews[framebufferIndex],
        }, NULL, &app->framebuffers[framebufferIndex]), "Couldn't create framebuffer %i", framebufferIndex)
    }
}

void createCommandPool(App* app){
    EXPECT(vkCreateCommandPool(app->device, &(VkCommandPoolCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = app->queueFamily,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    }, NULL, &app->commandPool), "Couldn't create command pool")
}

void createCommandBuffers(App* app){
    EXPECT(vkAllocateCommandBuffers(app->device, &(VkCommandBufferAllocateInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = app->commandPool,
        .commandBufferCount = 1,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    }, &app->commandBuffer), "Couldn't allocate command buffer")
}

void createSyncObjects(App* app){
    EXPECT(vkCreateSemaphore(app->device, &(VkSemaphoreCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    }, NULL, &app->imageAvailableSemaphore), "Couldn't create image acquired semaphore")

    EXPECT(vkCreateSemaphore(app->device, &(VkSemaphoreCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    }, NULL, &app->renderFinishedSemaphore), "Couldn't create render finished semaphore")

    EXPECT(vkCreateFence(app->device, &(VkFenceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    }, NULL, &app->inFlightFence), "Couldn't create in-flight fence")
}