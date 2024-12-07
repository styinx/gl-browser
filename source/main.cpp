// Glad
#include "glad/gl.h"

// imgui
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

// SDL
#include "SDL3/SDL.h"
#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_opengl.h"

// modules
#include "Camera.hpp"
#include "Model.hpp"

struct Application
{
    SDL_Window*     window = nullptr;
    SDL_GLContext   context;
    SDL_WindowFlags flags  = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    int             width  = 1600;
    int             height = 1024;

    void init()
    {
        stbi_set_flip_vertically_on_load(true);

        // SDL

        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow("OpenGL", width, height, flags);

        // OpenGL

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        context = SDL_GL_CreateContext(window);

        SDL_GL_MakeCurrent(window, context);
        SDL_GL_SetSwapInterval(1);  // Enable vsync

        // glad

        int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
        printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
        printf("%s\n", glGetString(GL_VERSION));

        // imgui

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 330");
        glEnable(GL_DEPTH_TEST);
    }

    void deinit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

struct Renderer
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Camera camera      = Camera();

    void drawImGui()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();

        // imgui
        {
            ImGui::NewFrame();

            ImGui::Begin("Renderer");
            {
                ImGui::ColorEdit3("clear color", (float*)&clear_color);
                ImGui::Text(
                    "Average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate,
                    io.Framerate);
            }
            ImGui::End();

            ImGui::Begin("Camera");
            {
                ImGui::SliderFloat("Pitch", &camera.pitch, -89.0f, 89.0f);
                ImGui::SliderFloat("Yaw", &camera.yaw, -360.0f, 360.0f);
                ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 100.0f);
                ImGui::SliderFloat("Speed", &camera.speed, 0.1f, 10.0f);
                ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.1f, 10.0f);

                ImGui::SliderFloat3("Position", &camera.position[0], -100.0f, 100.0f);
                ImGui::SliderFloat3("Front", &camera.front[0], 0.0f, 1.0f);
                ImGui::SliderFloat3("Up", &camera.up[0], 0.0f, 1.0f);
                ImGui::SliderFloat3("Right", &camera.right[0], 0.0f, 1.0f);
                ImGui::SliderFloat3("World", &camera.world_up[0], 0.0f, 1.0f);
            }
            ImGui::End();

            ImGui::Begin("Console");
            {
                static char text[1024 * 16] = "bli bla blub\n";

                ImGui::InputTextMultiline(
                    "##source",
                    text,
                    IM_ARRAYSIZE(text),
                    ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8));
            }
            ImGui::End();

            ImGui::Begin("Browser");
            {
                if(ImGui::BeginListBox("Model"))
                {
                    const char* items[] = {
                        "AAAA",
                        "BBBB",
                        "CCCC",
                        "DDDD",
                        "EEEE",
                        "FFFF",
                        "GGGG",
                        "HHHH",
                        "IIII",
                        "JJJJ",
                        "KKKK",
                        "LLLLLLL",
                        "MMMM",
                        "OOOOOOO"};

                    static bool item_highlight       = false;
                    static int  item_selected_idx    = 0;
                    int         item_highlighted_idx = -1;

                    for(int n = 0; n < IM_ARRAYSIZE(items); n++)
                    {
                        const bool is_selected = (item_selected_idx == n);
                        if(ImGui::Selectable(items[n], is_selected))
                            item_selected_idx = n;

                        if(item_highlight && ImGui::IsItemHovered())
                            item_highlighted_idx = n;

                        if(is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();
                }

                if(ImGui::TreeNode("Trees"))
                {
                    for(int i = 0; i < 5; i++)
                    {
                        if(i == 0)
                            ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                        ImGui::PushID(i);
                        if(ImGui::TreeNode("", "Child %d", i))
                        {
                            ImGui::Text("blah");
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::End();

            ImGui::Render();
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

int main(int argc, char** argv)
{
    auto app      = Application();
    auto renderer = Renderer();

    app.init();

    auto shader = Shader();
    shader.vertexShader("resource/vertex_model.glsl");
    shader.fragmentShader("resource/fragment_model.glsl");
    shader.link();
    shader.validate();

    auto model = Model("resource/model/model.obj");

    // Renderer

    glViewport(0, 0, app.width, app.height);

    // Main loop

    SDL_Event event;
    bool      running     = true;
    auto      frame_start = SDL_GetTicks();
    auto      frame_time  = SDL_GetTicks();

    while(running)
    {
        frame_start = SDL_GetTicks();

        const bool* key_states = SDL_GetKeyboardState(nullptr);

        if(key_states[SDL_SCANCODE_W])
            renderer.camera.move(Direction::FORWARD);
        else if(key_states[SDL_SCANCODE_S])
            renderer.camera.move(Direction::BACKWARD);

        if(key_states[SDL_SCANCODE_A])
            renderer.camera.move(Direction::LEFT);
        else if(key_states[SDL_SCANCODE_D])
            renderer.camera.move(Direction::RIGHT);

        if(key_states[SDL_SCANCODE_PAGEUP])
            renderer.camera.move(Direction::UPWARD);
        else if(key_states[SDL_SCANCODE_PAGEDOWN])
            renderer.camera.move(Direction::DOWNWARD);

        while(SDL_PollEvent(&event))
        {
            // imgui event handler
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
            }

            // SDL event handler
            switch(event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                SDL_GetWindowSize(app.window, &app.width, &app.height);
                glViewport(0, 0, app.width, app.height);
                break;

            case SDL_EVENT_KEY_DOWN:
                if(event.key.key == SDLK_ESCAPE)
                    running = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if(key_states[SDL_SCANCODE_LCTRL])
                    renderer.camera.navigate(event.motion.xrel, event.motion.yrel);
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                renderer.camera.FOV(event.wheel.y);
                break;
            }
        }

        // Window is minimized
        if(SDL_GetWindowFlags(app.window) & SDL_WINDOW_MINIMIZED)
        {
            // Wait 16ms
            SDL_Delay(16);
            continue;
        }

        // Render
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(
                renderer.clear_color.x * renderer.clear_color.w,
                renderer.clear_color.y * renderer.clear_color.w,
                renderer.clear_color.z * renderer.clear_color.w,
                renderer.clear_color.w);

            shader.activate();

            shader.set("u_model", renderer.camera.model());
            shader.set("u_view", renderer.camera.view());
            shader.set("u_projection", renderer.camera.projection(app.width, app.height));

            model.Draw(shader);

            renderer.drawImGui();

            SDL_GL_SwapWindow(app.window);
        }

        frame_time = SDL_GetTicks() - frame_start;
    }

    app.deinit();

    return 0;
}
