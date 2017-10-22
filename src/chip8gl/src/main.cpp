#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_memory_editor.h"

#include "chip8/emulator.h"

using namespace CHIP8::Emulator;

static CHIP8Emulator *chip = new CHIP8Emulator();

static void error_callback(int error, const char *description)
{
    printf("Error %d: %s\n", error, description);
}

// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
// {5
//     printf("Key: %x\n", key);

//     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, GL_TRUE);
// }

int main()
{
    //
    // Init
    //

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *pWindow = glfwCreateWindow(800, 600, "CCHIP-8", NULL, NULL);
    glfwMakeContextCurrent(pWindow);

    if (pWindow == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    // Set the required callback functions
    // glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(pWindow, true);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // ImGui windows
    static bool show_app_main = true;
    static bool show_app_ram_edit = false;

    static MemoryEditor frame_ram_mem_edit;

#ifndef NDEBUG
    static bool show_app_test = true;
#else
    static bool show_app_test = false;
#endif

    //
    // Main Loop
    //

    while (!glfwWindowShouldClose(pWindow))
    {
        glfwPollEvents();

        ImGui_ImplGlfwGL3_NewFrame();

        // Menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Show main window", NULL, &show_app_main);
                ImGui::MenuItem("Show RAM explorer", NULL, &show_app_ram_edit);
                ImGui::MenuItem("Show test window", NULL, &show_app_test);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Main Window
        ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("CHIP-8", &show_app_main) && show_app_main)
        {
            if (chip->IsInitialized)
            {
                ImGui::TextUnformatted("Waddup");
            } else
            {
                ImGui::TextUnformatted("Not initialized");

                if (ImGui::Button("Initialize"))
                {
                    chip->Initialize();
                }
            }
            ImGui::End();
        }

        // Memory Editors (RAM and display)
        if (chip->IsInitialized && show_app_ram_edit)
        {
            ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
            frame_ram_mem_edit.DrawWindow("RAM Explorer", chip->Memory, sizeof(chip->Memory));
        }

        if (show_app_test)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_app_test);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(pWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();

        glfwSwapBuffers(pWindow);
    }

    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}