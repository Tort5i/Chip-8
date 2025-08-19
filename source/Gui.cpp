#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"
#include "nfd.hpp"
#include "Gui.hpp"
#include "SDL.hpp"
#include "Chip-8.hpp"

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

void Gui::Draw(SDL *sdl, Chip8 *chip) {
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
                    chip->load(outpath);
                } else {
                    SDL_LogError(0, "NFD error: %s", NFD_GetError());
                }
            }
            if (ImGui::MenuItem("Unload")) {
                chip->Initilize();
            }
            if (ImGui::MenuItem("Exit")) {
                sdl->CloseGame();
            }
            ImGui::EndMenu();
        }
        

        if (ImGui::BeginMenu("Chip-8")) {
            if (ImGui::MenuItem("V Registers")) {
                showVRegViewer = true;
            }
            if (ImGui::MenuItem("Memory Viewer")) {
                ShowMemViewer = true;
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    if (showVRegViewer) {
        ImGui::Begin("V Register viewer", &showVRegViewer);

        ImGui::Text("mother flipping v reg viewer");

        ImGui::End();
    }

    if (ShowMemViewer) {
        ImGui::Begin("Memory viewer", &ShowMemViewer);

        if (ImGui::BeginTable("Memory", 0xF+2, ImGuiTableFlags_Borders)) {
            int i{0};
            for (int row{0}; row < 256; row++) {
                ImGui::TableNextColumn();
                for (int column{0}; column < 0xF+2; column++) {
                    ImGui::TableSetColumnIndex(column);
                    if (row == 0 && column != 0) {
                        ImGui::Text("%X", column-1);
                    }  else if (column == 0) {
                        ImGui::Text("%03X", row*0x10);
                    } else {
                        if (i < 0x4096) {
                            bool different{false};
                            for (int j{0}; j < prevMemSize; j++) {
                                int index = (prevMemIndex - 1 - j + 60) % 60;
                                if (index == -1) {
                                    index = 59;
                                }
                                if (previousMem[index][i] != chip->memory[i]) {
                                    different = true;
                                    break;
                                }
                            }
                            
                            if (different) {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,255,0,255));
                                ImGui::Text("%hu", chip->memory[i]);
                                ImGui::PopStyleColor();
                            } else {
                                ImGui::Text("%hu", chip->memory[i]);
                            }
                            i++;
                        }
                    }
                    
                }
            }

            for (int i{0}; i < 4096; i++) {
                previousMem[prevMemIndex][i] = chip->memory[i];
            }
            if (prevMemIndex == 59) {
                prevMemIndex = 0;
            } else {
                prevMemIndex++;
            }
            if (prevMemSize != 60) {
                prevMemSize++;
            }
            ImGui::EndTable();
        }

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