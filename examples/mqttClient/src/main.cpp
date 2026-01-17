// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

#ifdef USE_OPENGL
#include "backends/imgui_impl_opengl3.h"
#elif USE_VULKAN
#include "backends/imgui_impl_vulkan.h"
#endif

#include "mqttClient/MqttClientApp.h"
#include <mqttClient/Fonts.h>

constexpr int k_windowHeight = 1200;
constexpr int k_windowWidth = 800;
GLFWwindow* glfwWindow;

int previousWindowWidth;
int previousWindowHeight;

bool initGLFW()
{
    if (!glfwInit())
    {
        return false;
    }

    glfwWindow = glfwCreateWindow(k_windowHeight, k_windowWidth, "MQTT Client", nullptr, nullptr);

    if (!glfwWindow)
    {
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);

    return true;
}

#if defined(USE_OPENGL)
bool setupOpenGLImGui()
{
    // Setup ImGui with GLFW and OpenGL3 backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init();

    return true;
}
#elif defined(USE_VULKAN)
bool setupVulkanImGui()
{
    return false;
}
#endif

int main() {

    if (!initGLFW())
    {
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };

#if defined(USE_OPENGL)
    if (!setupOpenGLImGui())
    {
        return -1;
    }
#elif defined(USE_VULKAN)
    if (!setupVulkanImGui())
    {
        return -1;
    }
#endif

    MqttClientApp* app = new MqttClientApp();
    app->init();

    fonts::initFonts();

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Draw window
        if (app != nullptr)
        {
            if (!app->drawUi())
            {
                glfwSetWindowShouldClose(glfwWindow, true);
            }
        }
        else
        {
            glfwSetWindowShouldClose(glfwWindow, true);
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(glfwWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (previousWindowHeight != display_h || previousWindowWidth != display_w)
        {
            previousWindowWidth = display_w;
            previousWindowHeight = display_h;
            app->setWindowSize({ static_cast<float>(previousWindowWidth) , static_cast<float>(previousWindowHeight) });
        }

        glfwSwapBuffers(glfwWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    return 0;
}
