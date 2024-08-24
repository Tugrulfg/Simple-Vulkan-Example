cmake_minimum_required(VERSION 3.23)
project(SimpleVulkanExample C)

set(CMAKE_C_STANDARD 17)

include_directories(include)

add_executable(SimpleVulkanExample src/main.c src/utils.c src/init.c src/loop.c src/app.c)
target_link_libraries(SimpleVulkanExample vulkan)