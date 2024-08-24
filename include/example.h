#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

#include <vulkan/vulkan.h>

typedef struct {
    uint32_t vertexCount;
    VkPrimitiveTopology topology;
    const char *vertexShaderPath;
    const char *fragmentShaderPath;
} Draw;

typedef struct {
    const char *applicationName;
    const char *engineName;

    int windowWidth, windowHeight;

    uint32_t apiVersion;
    VkComponentMapping swapchainComponentsMapping;
    uint32_t swapchainBuffering;
    VkClearValue backgroundColor;

    uint32_t queueFamily;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;

    VkSwapchainKHR handle;

    uint32_t imageCount;
    VkImage *images;
    VkImageView *imageViews;

    VkFormat format;
    VkColorSpaceKHR colorSpace;
    VkExtent2D imageExtent;
    uint32_t imageAcquiredIndex;

    VkSurfaceKHR surface;

    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkFramebuffer* framebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    
    Draw draw;
} App;

enum SwapchainBuffering {
    SWAPCHAIN_DOUBLE_BUFFERING = 2,
    SWAPCHAIN_TRIPLE_BUFFERING = 3,
};

#endif