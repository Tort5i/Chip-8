#include "SDL.hpp"
#include "Chip-8.hpp"

int g_scaleFactor{24};

int main() {
    SDL sdl;
    Chip8 chip;
    
    if (!sdl.Init("My chip-8", 64*g_scaleFactor, 32*g_scaleFactor)) {
        return 1;
    }

    chip.Initilize();
    chip.load("Pong.ch8");

    while (!sdl.ShouldGameClose()) {
        sdl.Update(&chip);
        chip.EmulateCycle();

        if (chip.shouldDraw()) {
            sdl.ClearBackground(Color{0,0,0,0});
            sdl.Draw(&chip);
            chip.drawn();
        }
    }

    return 0;
}