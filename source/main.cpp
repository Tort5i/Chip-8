#include "SDL.hpp"
#include "Chip-8.hpp"

int main() {
    SDL sdl;
    Chip8 chip;
    
    if (!sdl.Init("My chip-8", 64, 32)) {
        return 1;
    }

    chip.Initilize();

    while (!sdl.ShouldGameClose()) {
        sdl.Update(&chip);

        sdl.ClearBackground(Color{0,0,0,255});
        sdl.Draw();
    }

    return 0;
}