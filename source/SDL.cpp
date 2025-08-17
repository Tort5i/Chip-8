#include "SDL.hpp"

bool SDL::Init(const char* title, int sWidth, int sHeight) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Could not init! %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer(title, sWidth, sHeight, 0, &window, &renderer)) {
        SDL_Log("Failed to create window and renderer, %s", SDL_GetError());
        return false;
    }

    return true;
}

bool SDL::ShouldGameClose() {
    return gameShouldClose;
}

void SDL::Update(Chip8 *chip) {
    SDL_PollEvent(&events);

    switch (events.type) {
        case SDL_EVENT_QUIT:
            gameShouldClose = true;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (events.key.key == SDLK_ESCAPE) {
                gameShouldClose = true;
            }
            
            for (int i{0}; i < 16; i++) {
                if (events.key.key == keymap[i]) {
                    chip->key[i] = 1;
                }
            }
            break;

        case SDL_EVENT_KEY_UP:
            for (int i{0}; i < 16; i++) {
                if (events.key.key == keymap[i]) {
                    chip->key[i] = 0;
                }
            }

    }
    
}

void SDL::ClearBackground(Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void SDL::Draw() {
    SDL_RenderPresent(renderer);
}