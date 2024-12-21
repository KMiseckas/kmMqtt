include_guard()

option(RENDERING_OPENGL "Build client with OpenGL rendering?" ON)
option(RENDERING_VULKAN "Build client with Vulkan rendering?" OFF)

message(STATUS "Mqtt Examples Options:")
message(STATUS "  RENDERING_OPENGL: ${RENDERING_OPENGL}")
message(STATUS "  RENDERING_VULKAN: ${RENDERING_VULKAN}")