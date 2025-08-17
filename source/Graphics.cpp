#include "Graphics.hpp"

bool Graphics::Init(const char* title, int sWidth, int sHeight) {
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

bool Graphics::ShouldGameClose() {
    return gameShouldClose;
}

void Graphics::Update() {
    SDL_PollEvent(&events);

    switch (events.type) {
        case SDL_EVENT_QUIT :
            gameShouldClose = true;
            break;
    }
    
}

void Graphics::ClearBackground(Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void Graphics::Draw() {
    SDL_RenderPresent(renderer);
}