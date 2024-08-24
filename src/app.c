#include "../include/app.h"

void init(App *app) {
    createInstance(app);
    pickPhysicalDevice(app);
    createSurface(app);
    createLogicalDevice(app);
    createSwapChain(app);
    createImageViews(app);
    createRenderPass(app);
    createGraphicsPipeline(app);
    createFramebuffers(app);
    createCommandPool(app);
    createCommandBuffers(app);
    createSyncObjects(app);
}

void loop(App *app) {
    int count = 0;
    while(1) {
        drawFrame(app);
        if(++count == 1000)
            break;
    }

    vkDeviceWaitIdle(app->device);
}

void cleanup(App *app) {
    // Destroy swapchain
    for (int i = 0; i < app->imageCount; ++i) {
        vkDestroyImageView(app->device, app->imageViews[i], NULL);
    }
    free(app->imageViews);
    free(app->images);

    vkDestroySwapchainKHR(app->device, app->handle, NULL);

    // Destroy graphics pipeline
    vkDestroyPipeline(app->device, app->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(app->device, app->pipelineLayout, NULL);

    // Destroy render pass
    vkDestroyRenderPass(app->device, app->renderPass, NULL);

    // Destroy sync objects
    vkDestroySemaphore(app->device, app->renderFinishedSemaphore, NULL);
    vkDestroySemaphore(app->device, app->imageAvailableSemaphore, NULL);
    vkDestroyFence(app->device, app->inFlightFence, NULL);

    // Destroy command pool
    vkDestroyCommandPool(app->device, app->commandPool, NULL);

    // Destroy logical device
    vkDestroyDevice(app->device, NULL);

    // Destroy surface
    vkDestroySurfaceKHR(app->instance, app->surface, NULL);

    // Destroy instance
    vkDestroyInstance(app->instance, NULL);
}
