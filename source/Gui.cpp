#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"
#include "Gui.hpp"
#include "SDL.hpp"

Gui::Gui(SDL *sdl) {
    Initilize(sdl);
}

Gui::~Gui() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Initilize(SDL *sdl) {
    mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    style = ImGui::GetStyle() ;
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;

    assert(sdl->GetWindow() != nullptr);
    assert(sdl->GetRenderer() != nullptr);

    ImGui_ImplSDL3_InitForSDLRenderer(sdl->GetWindow(), sdl->GetRenderer());\
    ImGui_ImplSDLRenderer3_Init(sdl->GetRenderer());
}

void Gui::Draw(SDL *sdl) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load")) {
            }
            if (ImGui::MenuItem("Exit")) {
                sdl->CloseGame();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }


    ImGui::Render();

    assert(sdl->GetRenderer() != nullptr);
    auto* renderer{sdl->GetRenderer()};

    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);*/
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}


