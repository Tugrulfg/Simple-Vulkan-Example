#ifndef INIT_H
#define INIT_H

#include "example.h"
#include "utils.h"

void createInstance(App* app);

void pickPhysicalDevice(App* app);

void createSurface(App* app);

void createLogicalDevice(App* app);
void queueFamilySelect(App* app);

void createSwapChain(App* app);
VkSurfaceCapabilitiesKHR surfaceCapabilitiesGet(App* app);
VkSurfaceFormatKHR surfaceFormatsSelect(const App *app);
VkPresentModeKHR surfacePresentModesSelect(const App *app);
void swapchainImagesGet(App *app);

void createImageViews(App* app);

void createRenderPass(App* app);

void createGraphicsPipeline(App* app);

void createFramebuffers(App* app);

void createCommandPool(App* app);

void createCommandBuffers(App* app);

void createSyncObjects(App* app);


#endif