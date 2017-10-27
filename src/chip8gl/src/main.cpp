#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_memory_editor.h"
#include "imgui_applog.h"

#include "chip8/chip8.h"
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
    static bool show_app_asm_log = false;
    static bool show_app_test = false;

    static MemoryEditor widget_ram_mem_edit;
    static AppLog asmLog;

    // CHIP-8 emulator options
    static bool opt_code_step = false;

    // Link ImGui objects with emulator
    chip->SetAsmLog(&asmLog);

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
                ImGui::MenuItem("Show ASM log", NULL, &show_app_asm_log);
                ImGui::MenuItem("Show test window", NULL, &show_app_test);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Main Window
        if (show_app_main)
        {
            ImGui::SetNextWindowSize(ImVec2(150, 560), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("CHIP-8", &show_app_main))
            {
                if (chip->IsInitialized)
                {
                    ImGui::Checkbox("Step through code", &opt_code_step);

                    if (ImGui::Button("Continue"))
                    {
                        chip->EmulateCycle();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Load rom"))
                    {
                        FILE *file = fopen("roms/PONG", "rb");
                        uint8_t *buffer;
                        uint32_t buffer_size;

                        CHIP8::LoadRom(file, &buffer, &buffer_size);
                        chip->LoadGame(buffer, buffer_size);

                        fclose(file);
                        free(buffer);
                    }

                    ImGui::Separator();

                    ImGui::Text("Opcode: %04X", chip->Opcode);
                    ImGui::Text("I: %04X (%d)", chip->I, chip->I);
                    ImGui::Text("Pc: %04X (%d)", chip->Pc, chip->Pc);
                    ImGui::Text("Sp: %04X (%d)", chip->Sp, chip->Sp);
                    ImGui::Text("DelayTimer: %04X (%d)", chip->DelayTimer, chip->DelayTimer);
                    ImGui::Text("DelayTimer: %04X (%d)", chip->SoundTimer, chip->SoundTimer);

                    if (ImGui::CollapsingHeader("Registers"))
                    {
                        for (int i = 0; i < sizeof(chip->V) / sizeof(chip->V[0]); i++)
                        {
                            ImGui::Text("V%X: %X (%d)", i, chip->V[i], chip->V[i]);
                        }
                    }

                    if (ImGui::CollapsingHeader("Stack"))
                    {
                        for (int i = 0; i < sizeof(chip->Stack) / sizeof(chip->Stack[0]); i++)
                        {
                            ImGui::Text("%i: %02X (%d)", i, chip->Stack[i], chip->Stack[i]);
                        }
                    }
                } else
                {
                    ImGui::TextUnformatted("Not initialized");

                    if (ImGui::Button("Initialize"))
                    {
                        chip->Initialize();
                    }
                }
            }
            ImGui::End();
        }

        // RAM Explorer
        if (show_app_ram_edit)
        {
            // ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
            // TODO: Draw own window and then use DrawContents()
            widget_ram_mem_edit.DrawWindow("RAM Explorer", chip->Memory, sizeof(chip->Memory));
        }

        // ASM Log
        if (show_app_asm_log)
        {
            asmLog.Draw("ASM Log", &show_app_asm_log);
        }

        // Test/demo window
        if (show_app_test)
        {
            // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
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