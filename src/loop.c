#include "../include/loop.h"

void drawFrame(App* app){
    // Acquire next image from swap chain
    EXPECT(vkAcquireNextImageKHR(app->device, app->handle, UINT64_MAX, app->imageAvailableSemaphore, NULL, &app->imageAcquiredIndex), "Couldn't acquire next image")

    // Record command buffer
    VkCommandBuffer commandBuffer = app->commandBuffer;
    uint32_t imageAcquiredIndex = app->imageAcquiredIndex;

    EXPECT(vkBeginCommandBuffer(commandBuffer, &(VkCommandBufferBeginInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    }), "Couldn't begin command buffer for frame")

    VkClearValue clearValues[] = {
            app->backgroundColor,
    };

    vkCmdBeginRenderPass(app->commandBuffer, &(VkRenderPassBeginInfo) {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = app->renderPass,
        .framebuffer = app->framebuffers[imageAcquiredIndex],
        .renderArea = (VkRect2D) {
            .extent = app->imageExtent,
        },
        .clearValueCount = 1,
        .pClearValues = clearValues,
    }, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app->graphicsPipeline);
    vkCmdDraw(commandBuffer, app->draw.vertexCount, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    EXPECT(vkEndCommandBuffer(commandBuffer), "Couldn't end command buffer")

    // Submit command buffer
     EXPECT(vkQueueSubmit(app->queue, 1, &(VkSubmitInfo) {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &app->commandBuffer,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &app->imageAvailableSemaphore,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &app->renderFinishedSemaphore,
        .pWaitDstStageMask = (VkPipelineStageFlags[]) {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        }
    }, app->inFlightFence), "Couldn't submit command buffer")

    // Present image
     EXPECT(vkQueuePresentKHR(app->queue, &(VkPresentInfoKHR) {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &app->handle,
        .pImageIndices = &app->imageAcquiredIndex,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &app->renderFinishedSemaphore,
    }), "Couldn't present swapchain image %i", app->imageAcquiredIndex)
    EXPECT(vkWaitForFences(app->device, 1, &app->inFlightFence, VK_FALSE, UINT64_MAX), "Couldn't wait for fence")
    EXPECT(vkResetFences(app->device, 1, &app->inFlightFence), "Couldn't reset the fence")
}