#include <SDL3/SDL.h>
#include <string>

#include "ImGui/imgui.h"

#include "SDL.hpp"
#include "Chip-8.hpp"

class Gui {
private:
    ImGuiIO io;
    ImGuiStyle style;
    float mainScale;
    SDL_Window *window;
    SDL_Renderer *renderer;

    bool fileToLoad{false};
    std::string filePath;

    bool ShowMemViewer{false};
    bool showVRegViewer{false};

    unsigned char previousMem[60][4096];
    int prevMemIndex{0};
    int prevMemSize{0};

public:
    Gui(SDL *sdl);
    ~Gui();

    void Initilize(SDL *sdl);

    void Draw(SDL *sdl, Chip8 *chip);

    bool FileToLoad();
    const char* GetFilePath();
};