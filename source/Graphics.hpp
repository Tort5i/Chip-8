#pragma once

#include <SDL3/SDL.h>

#include "types.hpp"

class Graphics {
private:
    const unsigned short targetFrameRate{60};
    Uint64 frameRate{0};
    bool gameShouldClose{false};

    SDL_Window *window{NULL};
    SDL_Renderer *renderer{NULL};
    SDL_Event events{0};

public:
    bool Init(const char* title, int sWidth, int sHeight);
    bool ShouldGameClose();
    void Update();

    void ClearBackground(Color color);
    void Draw();
};