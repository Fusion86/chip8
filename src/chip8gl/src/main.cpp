#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_memory_editor.h"

#include "chip8/emulator.h"

#ifdef DEBUG
#define SHOW_TEST_WINDOW true
#else
#define SHOW_TEST_WINDOW false
#endif

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
    GLFWwindow *window = glfwCreateWindow(800, 600, "CCHIP-8", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (window == NULL)
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
    ImGui_ImplGlfwGL3_Init(window, true);

    bool show_test_window = SHOW_TEST_WINDOW;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setup extra ImGui stuff
    static bool wnd_main_visible = true;
    static MemoryEditor ram_mem_edit;

    //
    // Main Loop
    //

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplGlfwGL3_NewFrame();

        // Main Window
        ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
        ImGui::Begin("CHIP-8", &wnd_main_visible);
        if (chip->IsInitialized)
        {
            ImGui::TextUnformatted("Waddup");
        } else
        {
            ImGui::TextUnformatted("Not initialized :(");

            if (ImGui::Button("Initialize"))
            {
                chip->Initialize();
            }
        }
        ImGui::End();

        // Memory Editors (RAM and display)
        if (chip->IsInitialized)
        {
            ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
            ram_mem_edit.DrawWindow("RAM Explorer", chip->Memory, sizeof(chip->Memory));
        }

        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();

        glfwSwapBuffers(window);
    }

    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}