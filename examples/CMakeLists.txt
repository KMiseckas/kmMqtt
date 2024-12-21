cmake_minimum_required (VERSION 3.14)

include(FetchContent)

include(cmake/options.cmake)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

set(DROIDSANS_FONT_PATH "${imgui_SOURCE_DIR}/misc/fonts/DroidSans.ttf")
add_compile_definitions(DROIDSANS_FONT_PATH="${DROIDSANS_FONT_PATH}")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

#OpenGL
find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    message("OpenGL found:")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}\n")
else()
    message(FATAL_ERROR "OpenGL not found on this system.")
endif()

#IMGUI static library with backend APIs
if(RENDERING_OPENGL)
    set(imgui_backend_render_lib ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
elseif(RENDERING_VULKAN)
    set(imgui_backend_render_cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
else()
    message(FATAL_ERROR "Rendering API not selected for examples projects.")
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_backend_render_lib}
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(imgui PUBLIC glfw)

#Shared examples library
add_library(examples_shared_lib INTERFACE)

if(RENDERING_OPENGL)
    target_compile_definitions(examples_shared_lib INTERFACE USE_OPENGL)
elseif(RENDERING_VULKAN)
    target_compile_definitions(examples_shared_lib INTERFACE USE_VULKAN)
endif()

target_link_libraries(examples_shared_lib INTERFACE imgui ${OPENGL_gl_LIBRARY} cleanMqtt)

#Add example projects
add_subdirectory(adapters/websocket)
add_subdirectory(mqttClient)