#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"
#include "nfd.hpp"
#include "Gui.hpp"
#include "SDL.hpp"

Gui::Gui(SDL *sdl) {
    Initilize(sdl);
}

Gui::~Gui() {
    NFD_Quit();
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

    NFD_Init();
}

void Gui::Draw(SDL *sdl) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load")) {
                nfdu8char_t *outpath;
                nfdu8filteritem_t filters[1]{"ROM", "ch8"};
                nfdopendialogu8args_t args{0};
                args.filterList = filters;
                args.filterCount = 1;
                nfdresult_t result{ NFD_OpenDialogU8_With(&outpath, &args)};

                if (result == NFD_OKAY) {
                    SDL_Log("New file loaded, %s", outpath);
                    filePath = outpath;
                    fileToLoad = true;
                } else {
                    SDL_LogError(0, "NFD error: %s", NFD_GetError());
                }
            }
            if (ImGui::MenuItem("Exit")) {
                sdl->CloseGame();
            }
            ImGui::EndMenu();
        }
        

        if (ImGui::BeginMenu("Chip-8")) {
            if (ImGui::MenuItem("Memory Viewer")) {
                ShowMemViewer = true;
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    if (ShowMemViewer) {
        ImGui::Begin("Memory viewer");

        ImGui::Text("Mother flipping memory viewer");

        ImGui::End();
    }


    ImGui::Render();

    assert(sdl->GetRenderer() != nullptr);
    auto* renderer{sdl->GetRenderer()};

    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);*/
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

bool Gui::FileToLoad() {
    return fileToLoad;
}


const char* Gui::GetFilePath() {
    return filePath.c_str();
}