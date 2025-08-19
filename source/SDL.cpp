#include "SDL.hpp"
#include "Globals.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"

const Color WHITE{255, 255, 255, 255};
const Color BLACK{0, 0, 0, 255};

SDL::SDL() {}

SDL::~SDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

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

void SDL::CloseGame() {
    gameShouldClose = true;
}

void SDL::Update(Chip8 *chip) {
    while (SDL_PollEvent(&events)) {
        ImGui_ImplSDL3_ProcessEvent(&events);

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
    
}

void SDL::ClearBackground(Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void SDL::Draw(Chip8 *chip) {
    int i{0};
    SDL_FRect rect{0,0,0,0};
    for (int j{0}; j < 64; j++) {
        for (int k{0}; k < 32; k++) {
            //SDL_Log("I: %d", i);
            i = j + k * 64;
            if (chip->gfx[i] == 1) {
                SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
                rect.x = j*g_scaleFactor;
                rect.y = k*g_scaleFactor+g_menuBarOffset;
                rect.w = 1*g_scaleFactor;
                rect.h = 1*g_scaleFactor;
                //SDL_Log("Drawing, X: %f, Y: %f, W: %f, H: %f, I: %d", rect.x, rect.y, rect.w, rect.h, i);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
            } 
        }
    }
    
    //SDL_RenderPresent(renderer);
}

SDL_Window* SDL::GetWindow() {
    return window;
}

SDL_Renderer* SDL::GetRenderer() {
    return renderer;
}