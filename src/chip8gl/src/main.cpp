#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_applog.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui_memory_editor.h"

#include "chip8/chip8.h"
#include "chip8/emulator.h"

using namespace CHIP8::Emulator;

//
// Shader Source
//

const char *vertex_shader_source = R""(
#version 330 core
layout (location = 0) in vec3 aPos;
  
out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)"";

const char *fragment_shader_source = R""(
#version 330 core
out vec4 FragColor;  
  
void main()
{
    FragColor = vec4(1, 1, 1, 1);
}
)"";

// ImGui / OpenGL generic
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// ImGui windows
static bool show_app_main = true;
static bool show_app_ram_edit = false;
static bool show_app_log = false;
static bool show_app_test = false;

// ImGui popups
const char *title_popup_rom_not_found = "ROM not found!";
static bool show_popup_rom_not_found = false;

// ImGui widgets
static MemoryEditor widget_ram_mem_edit;
static AppLog widget_log;

// ImGui input buffers
static char input_delay_timer[8];
static char input_sound_timer[8];

// CHIP-8 emulator
static CHIP8Emulator *chip = new CHIP8Emulator();
static bool opt_code_step = false; // If false: the code will just run like it normally would on device

static void error_callback(int error, const char *description)
{
    printf("Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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
    GLFWwindow *window = glfwCreateWindow(1400, 800, "CHIP-8", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    // Link ImGui objects with emulator
    chip->SetAsmLog(&widget_log);
    chip->SetWindowContext(window);

    //
    // Setup shaders
    //

    int success;
    char infoLog[512];

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        widget_log.AddLog("[error] [vertex shader] [compilation failed] %s\n", infoLog);
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        widget_log.AddLog("[error] [fragment shader] [compilation failed] %s\n", infoLog);
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        widget_log.AddLog("[error] [shader program] [linking failed] %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //
    // Setup our one quad
    //

	float vertices[] = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    //
    // Main Loop
    //

    while (!glfwWindowShouldClose(window))
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
                ImGui::MenuItem("Show log", NULL, &show_app_log);
                ImGui::Separator();
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
                    ImGui::Checkbox("Enable code step", &opt_code_step);

                    if (opt_code_step && !chip->IsRunning)
                    {
                        if (ImGui::Button("Continue"))
                        {
                            chip->EmulateCycleStep();
                        }
                    }
                    else
                    {
                        if (!chip->IsRunning)
                        {
                            if (ImGui::Button("Start"))
                            {
                                chip->IsRunning = true;
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Reset"))
                            {
                                chip->Initialize();
                            }
                        }
                        else
                        {
                            if (ImGui::Button("Stop"))
                            {
                                chip->IsRunning = false;
                            }
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Load rom"))
                    {
                        FILE *file = fopen("roms/PONG", "rb");

                        if (file == NULL)
                        {
                            show_popup_rom_not_found = true;
                        }
                        else
                        {
                            uint8_t *buffer;
                            uint32_t buffer_size;

                            CHIP8::LoadRom(file, &buffer, &buffer_size);
                            chip->LoadGame(buffer, buffer_size);

                            fclose(file);
                            free(buffer);
                        }
                    }

                    ImGui::Separator();

                    ImGui::Text("Opcode: %04X", chip->Opcode);
                    ImGui::Text("I: %04X (%d)", chip->I, chip->I);
                    ImGui::Text("Pc: %04X (%d)", chip->Pc, chip->Pc);
                    ImGui::Text("Sp: %02X (%d)", chip->Sp, chip->Sp);

                    if (opt_code_step == false)
                    {
                        ImGui::Text("DelayTimer: %02X (%d)", chip->DelayTimer, chip->DelayTimer);
                        ImGui::Text("SoundTimer: %02X (%d)", chip->SoundTimer, chip->SoundTimer);
                    }
                    else
                    {
                        snprintf(input_delay_timer, sizeof(input_delay_timer), "%d", chip->DelayTimer);
                        snprintf(input_sound_timer, sizeof(input_sound_timer), "%d", chip->SoundTimer);

                        if (ImGui::InputText("DelayTimer", input_delay_timer, sizeof(input_delay_timer), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            sscanf(input_delay_timer, "%" SCNu8, &chip->DelayTimer);
                        }

                        if (ImGui::InputText("SoundTimer", input_sound_timer, sizeof(input_sound_timer), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            sscanf(input_sound_timer, "%" SCNu8, &chip->SoundTimer);
                        }
                    }

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
                }
                else
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

        // App Log
        if (show_app_log)
        {
            widget_log.Draw("Log", &show_app_log);
        }

        // Test/demo window
        if (show_app_test)
        {
            // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_app_test);
        }

        // ROM not found popup
        if (show_popup_rom_not_found)
        {
            show_popup_rom_not_found = false;
            ImGui::OpenPopup(title_popup_rom_not_found);
        }

        if (ImGui::BeginPopupModal(title_popup_rom_not_found, NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Couldn't find the rom!\n\n");

            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        //
        // Rendering
        //

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        if (chip->DrawFlag)
        {
            // TODO: Render CHIP-8 screen
            chip->DrawFlag = false;
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ImGui::Render();

        glfwSwapBuffers(window);
    }

    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
