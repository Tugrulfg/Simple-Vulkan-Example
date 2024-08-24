#include "../include/example.h"
#include "../include/app.h"

int main() {
    App app = {
            .applicationName = "Vulkan Example",
            .engineName = "",
            .windowWidth = 1366,
            .windowHeight = 768,
            .apiVersion = VK_API_VERSION_1_3,
            .swapchainComponentsMapping = (VkComponentMapping) {},
            .swapchainBuffering = SWAPCHAIN_TRIPLE_BUFFERING,
            .backgroundColor = {{0.01f, 0.01f, 0.01f, 0.01f}},
            .draw = {
                .vertexCount = 18,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .vertexShaderPath = "../res/shaders/SimpleExample.vert.spv",
                .fragmentShaderPath = "../res/shaders/SimpleExample.frag.spv",
            }
    };

    printf("******************************* INIT *******************************\n");
    init(&app);
    
    printf("******************************* LOOP *******************************\n");

    loop(&app);

    printf("******************************* CLEANUP *******************************\n");

    cleanup(&app);

    return EXIT_SUCCESS;
}
