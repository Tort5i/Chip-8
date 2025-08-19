#include "ImGui/imgui.h"

#include "SDL.hpp"

#include <SDL3/SDL.h>

class Gui {
private:
    ImGuiIO io;
    ImGuiStyle style;
    float mainScale;
    SDL_Window *window;
    SDL_Renderer *renderer;

public:
    Gui(SDL *sdl);
    ~Gui();

    void Initilize(SDL *sdl);

    void Draw(SDL *sdl);
};