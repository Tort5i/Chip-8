#include "SDL.hpp"
#include "Chip-8.hpp"
#include "Gui.hpp"

int g_scaleFactor{24};
int g_menuBarOffset{20};
static const Uint32 Target_ticks_per_frame{ 1000 / 60 };

int main(int argc, char* argv[]) {
    /*if (argc != 2) {
        SDL_LogError(0, "Usage error");
        SDL_Log("./Chip-8 [Chip-8 ROM]");
        return -1;
    }*/
    
    SDL sdl;
    Chip8 chip;
    
    if (!sdl.Init("My chip-8", 64*g_scaleFactor, 32*g_scaleFactor+g_menuBarOffset)) {
        return 1;
    }

    Gui gui{&sdl};

    chip.Initilize();
    if (argc > 1) {
        chip.load(argv[1]);
    }

    while (!sdl.ShouldGameClose()) {
        Uint64 start{ SDL_GetTicks() };
        
        sdl.Update(&chip);
        if (chip.GameLoaded()) {
            chip.EmulateCycle();
        }

        if (true) {
            sdl.ClearBackground(Color{0,0,0,0});
            sdl.Draw(&chip);
            gui.Draw(&sdl);
            chip.drawn();
            auto* renderer{ sdl.GetRenderer()};
            SDL_RenderPresent(renderer);
        }
        
        Uint64 end{ SDL_GetTicks() };
        Uint64 elapsed{ end-start };
        if (Target_ticks_per_frame > elapsed) {
            SDL_Delay(Target_ticks_per_frame-(Uint32)elapsed);
        }
    }

    return 0;
}