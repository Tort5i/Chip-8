#include <SDL3/SDL.h>
#include <string>

#include "ImGui/imgui.h"

#include "SDL.hpp"
#include "Chip-8.hpp"

#define TOTAL_MEM 0x1000
#define TOTAL_V 16

#define RECORD_LENGTH 100

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

    unsigned char previousMem[RECORD_LENGTH][TOTAL_MEM];
    int prevMemIndex{0};
    int prevMemSize{0};

    unsigned char previousV[RECORD_LENGTH][TOTAL_V];
    int prevVIndex{0};
    int prevVSize{0};

public:
    Gui(SDL *sdl);
    ~Gui();

    void Initilize(SDL *sdl);

    void Draw(SDL *sdl, Chip8 *chip);

    bool FileToLoad();
    const char* GetFilePath();
};