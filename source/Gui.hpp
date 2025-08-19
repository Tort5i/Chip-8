#include <SDL3/SDL.h>
#include <string>

#include "ImGui/imgui.h"

#include "SDL.hpp"

class Gui {
private:
    ImGuiIO io;
    ImGuiStyle style;
    float mainScale;
    SDL_Window *window;
    SDL_Renderer *renderer;

    bool fileToLoad{false};
    std::string filePath;

public:
    Gui(SDL *sdl);
    ~Gui();

    void Initilize(SDL *sdl);

    void Draw(SDL *sdl);

    bool FileToLoad();
    const char* GetFilePath();
};