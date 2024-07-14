cmake_minimum_required(VERSION 3.20)

project(FlowForge)

# add vendor subdirectories
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/spdlog)

# add flowforge specific subdirectories
add_subdirectory(src)
add_subdirectory(examples)