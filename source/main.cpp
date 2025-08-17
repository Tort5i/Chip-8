#include "Graphics.hpp"
#include "Chip-8.hpp"

static Graphics graphics;
//static Chip8 chip8;

int main() {
    if (!graphics.Init("My chip-8", 64, 32)) {
        return 1;
    }

    while (!graphics.ShouldGameClose()) {
        graphics.Update();

        graphics.ClearBackground(Color{0,0,0,255});
        graphics.Draw();
    }

    return 0;
}