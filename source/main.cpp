#include "SDL.hpp"
#include "Chip-8.hpp"

int g_scaleFactor{24};
static const Uint32 Target_ticks_per_frame{ 1000 / 60 };

int main() {
    SDL sdl;
    Chip8 chip;
    
    if (!sdl.Init("My chip-8", 64*g_scaleFactor, 32*g_scaleFactor)) {
        return 1;
    }

    chip.Initilize();
    chip.load("Pong.ch8");

    while (!sdl.ShouldGameClose()) {
        Uint64 start{ SDL_GetTicks() };
        
        sdl.Update(&chip);
        chip.EmulateCycle();

        if (chip.shouldDraw()) {
            sdl.ClearBackground(Color{0,0,0,0});
            sdl.Draw(&chip);
            chip.drawn();
        }

        Uint64 end{ SDL_GetTicks() };
        Uint64 elapsed{ end-start };
        if (Target_ticks_per_frame > elapsed) {
            SDL_Delay(Target_ticks_per_frame-(Uint32)elapsed);
        }
    }

    return 0;
}