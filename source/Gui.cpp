#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"
#include "nfd.hpp"
#include "Gui.hpp"
#include "SDL.hpp"
#include "Chip-8.hpp"

// Each REG gets a column
#define VREG_TABLE_COLUMNS 16
#define VREG_TABLE_ROWS 2

// 2 bytes plus a for the stuff on the left
#define MEM_TABLE_COLUMS 17
// total mem (0x1000) / colums (-1 as one is for labels) + 1 to display all mem locations
#define MEM_TABLE_ROWS (TOTAL_MEM / (MEM_TABLE_COLUMS-1))+1

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

    std::string fpsStr = "FPS: " + std::to_string(sdl->frameRate);
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(fpsStr.c_str())) {ImGui::EndMenu();}
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
            if (ImGui::MenuItem("Preferences")) {
                showPreferenceMenu = true;
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

    if (showPreferenceMenu) {
        ImGui::Begin("Preferences", &showPreferenceMenu);

        ImGui::ColorEdit4("On color", sdl->onColor);
        ImGui::ColorEdit4("Off color", sdl->offColor);

        ImGui::End();
    }

    if (showVRegViewer) {
        ImGui::Begin("V Register viewer", &showVRegViewer);

        bool intSelected{false};
        bool hexSelected{false};
        bool charSelected{false};

        if (ImGui::BeginCombo("Display type", "", ImGuiComboFlags_NoPreview)) {
            ImGui::Selectable("Interger", &intSelected);
            ImGui::Selectable("Hexidecimil", &hexSelected);
            ImGui::Selectable("Char", &charSelected);
            ImGui::EndCombo();
        }

        if (intSelected) {
            vDisplayType = Display_Type::Interger;
        } else if (hexSelected) {
            vDisplayType = Display_Type::Hexidecimal;
        } else if (charSelected) {
            vDisplayType = Display_Type::Character;
        }

        if (ImGui::BeginTable("V table", VREG_TABLE_COLUMNS, ImGuiTableFlags_Borders)) {
            int i{0};
            for (int row{0}; row < VREG_TABLE_ROWS; row++) {
                ImGui::TableNextColumn();
                for (int column{0}; column < VREG_TABLE_COLUMNS; column++) {
                    ImGui::TableSetColumnIndex(column);
                    if (0 == row) {
                        ImGui::Text("%d", column);
                    } else {
                        bool different{false};
                        for (int j{0}; j < prevVSize; j++) {
                            int index {(prevVIndex - 1 - j + RECORD_LENGTH) % RECORD_LENGTH};
                            if (index == -1) {
                                index = TOTAL_V-1;
                            }

                            if (previousV[index][i] != chip->V[i]) {
                                different = true;
                                break;
                            }
                        }

                        if (different) {
                                if (0 == chip->V[i]) {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                            }
                            }
                            switch (vDisplayType) {
                                case Display_Type::Interger:
                                    ImGui::Text("%hu", chip->V[i]);
                                    break;
                                case Display_Type::Hexidecimal:
                                    ImGui::Text("%hX", chip->V[i]);
                                    break;
                                case Display_Type::Character:
                                    ImGui::Text("%c", chip->V[i]);
                                    break;
                            }
                            if (different) {    
                                ImGui::PopStyleColor();
                            }
                        i++;
                    }
                }
            }
            for (int i{0}; i < TOTAL_V; i++) {
                previousV[prevVIndex][i] = chip->V[i];
            }
            if (prevVIndex == RECORD_LENGTH-1) {
                prevVIndex = 0;
            } else {
                prevVIndex++;
            }
            if (prevVSize != RECORD_LENGTH) {
                prevVSize++;
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

    if (ShowMemViewer) {
        ImGui::Begin("Memory viewer", &ShowMemViewer);

        bool intSelected{false};
        bool hexSelected{false};
        bool charSelected{false};

        if (ImGui::BeginCombo("Display type", "", ImGuiComboFlags_NoPreview)) {
            ImGui::Selectable("Interger", &intSelected);
            ImGui::Selectable("Hexidecimil", &hexSelected);
            ImGui::Selectable("Char", &charSelected);
            ImGui::EndCombo();
        }

        if (intSelected) {
            memDisplayType = Display_Type::Interger;
        } else if (hexSelected) {
            memDisplayType = Display_Type::Hexidecimal;
        } else if (charSelected) {
            memDisplayType = Display_Type::Character;
        }

        if (ImGui::BeginTable("Memory", MEM_TABLE_COLUMS, ImGuiTableFlags_Borders)) {
            int i{0};
            for (int row{0}; row < MEM_TABLE_ROWS; row++) {
                ImGui::TableNextColumn();
                for (int column{0}; column < MEM_TABLE_COLUMS; column++) {
                    ImGui::TableSetColumnIndex(column);
                    
                    // if top right leave blank
                    if (row == 0 && column == 0) {continue;}
                    // If it is the top row and not the first column
                    else if (row == 0 && column != 0) {
                        ImGui::Text("%X", column-1);
                    }  else if (column == 0) { // label for the left hand side
                        ImGui::Text("%03X", row*0x10);
                    } else { // if mem location
                        if (i < TOTAL_MEM) {
                            bool different{false};
                            for (int j{0}; j < prevMemSize; j++) {
                                int index = (prevMemIndex - 1 - j + RECORD_LENGTH) % RECORD_LENGTH;
                                if (index == -1) {
                                    index = 59;
                                }
                                if (previousMem[index][i] != chip->memory[i]) {
                                    different = true;
                                    break;
                                }
                            }
                            
                            if (different) {
                                if (0 == chip->memory[i]) {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                            }
                            }
                            switch (memDisplayType) {
                                case Display_Type::Interger:
                                    ImGui::Text("%hu", chip->memory[i]);
                                    break;
                                case Display_Type::Hexidecimal:
                                    ImGui::Text("%hX", chip->memory[i]);
                                    break;
                                case Display_Type::Character:
                                    ImGui::Text("%c", chip->memory[i]);
                                    break;
                            }
                            if (different) {    
                                ImGui::PopStyleColor();
                            }
                            i++;
                        }
                    }
                    
                }
            }
        

            for (int i{0}; i < TOTAL_MEM; i++) {
                previousMem[prevMemIndex][i] = chip->memory[i];
            }
            if (prevMemIndex == RECORD_LENGTH-1) {
                prevMemIndex = 0;
            } else {
                prevMemIndex++;
            }
            if (prevMemSize != RECORD_LENGTH) {
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