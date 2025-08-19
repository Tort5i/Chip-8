#pragma once

#include <SDL3/SDL.h>

#include "Chip-8.hpp"
#include "types.hpp"

class SDL {
private:
    const unsigned short targetFrameRate{60};
    Uint64 frameRate{0};
    bool gameShouldClose{false};

    SDL_Window *window{NULL};
    SDL_Renderer *renderer{NULL};

    SDL_Event events{0};

    unsigned short keymap[16] {
        SDLK_X,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_Q,
        SDLK_W,
        SDLK_E,
        SDLK_A,
        SDLK_S,
        SDLK_D,
        SDLK_Z,
        SDLK_C,
        SDLK_4,
        SDLK_R,
        SDLK_F,
        SDLK_V,
    };

public:

    SDL();
    ~SDL();

    bool Init(const char* title, int sWidth, int sHeight);
    bool ShouldGameClose();
    void CloseGame();
    void Update(Chip8 *chip8);

    void ClearBackground(Color color);
    void Draw(Chip8 *chip);

    SDL_Window* GetWindow();
    SDL_Renderer* GetRenderer();
};