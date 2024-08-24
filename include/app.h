#ifndef APP_H
#define APP_H

#include "example.h"
#include "init.h"
#include "loop.h"
#include "utils.h"

// Initializes the necessary Vulkan resources
void init(App *app);

// Runs the main loop to render the swapchain images
void loop(App *app);

// Cleans up the Vulkan resources
void cleanup(App *app);

#endif