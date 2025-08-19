#include <cstring>
#include <stdio.h>
#include <random>
#include <SDL3/SDL_log.h>
#include "Chip-8.hpp"

void Chip8::Initilize() {
    pc = 0x200;
    opcode = 0;
    indexRegister = 0;
    stackPointer = 0;

    for (int i{0}; i < (64 * 32); i++) {
        gfx[i] = 0;
    }

    for (int i{0}; i < 16; i++) {
        stack[i] = 0;
        V[i] = 0;
    }

    for (int i{0}; i < 4096; i++) {
        memory[i] = 0;
    }

    // load fontset
    for (int i{0}; i<80; i++) {
        memory[i] = fontset[i];
    }

    // reset timers
    delayTimer = 0;
    soundTimer = 0;

    gameLoaded = false;
}

void Chip8::load(const char *path) {
    SDL_Log("Loading ROM: %s", path);

    FILE* rom{ fopen(path, "rb") };
    if (!rom) {
        SDL_LogError(0, "Could not open ROM %s", path);
        return;
    }

    fseek(rom, 0, SEEK_END);
    long romSize{ftell(rom)};
    rewind(rom);

    char* romBuffer{ (char*) malloc(sizeof(char) * romSize) };
    if (!romBuffer) {
        SDL_LogError(0, "Failed to create log buffer");
        return;
    }

    size_t result = fread(romBuffer, sizeof(char), (size_t)romSize, rom);
    if (result != romSize) {
        SDL_LogError(0, "Failed to read ROM");
        return;
    }

    if ((4096-512) > romSize){
        for (int i = 0; i < romSize; ++i) {
            memory[i + 512] = (uint8_t)romBuffer[i];   
        }
    }
    else {
        SDL_LogError(0, "ROM to larget to fit in memory, %li", romSize);
        return;
    }

    gameLoaded = true;
    fclose(rom);
    free(romBuffer);
}

void Chip8::EmulateCycle() {
    /* Fetch 
    Data is stored in 2 bytes, each array index is one 
    The first byte is shifted to the left, then the secnond byte is OR'ed to merge the two opcodes */
    opcode = memory[pc] << 8 | memory[pc+1];
    SDL_Log("Opcode: %X", opcode);

    /* Decode
    when the opcode is AND'ed against 0xF000 the first byte remains but the remainin are set to 0 */
    switch (opcode & 0xF000) {

        case 0x0000:
            switch (opcode & 0x00FF) {
                case CLS_SCREEN_OPCODE:
                    SDL_Log("Screen cleared");
                    for (int i{0}; i < (64*32); i++) {
                        gfx[i] = 0;
                        drawFlag = true;
                    }
                    pc += 2;
                    break;

                case RET_FROM_SUBRTN_OPCODE:
                    SDL_Log("ret from subrtn");
                    stackPointer--;
                    pc = stack[stackPointer] + 2;
                    break;
            }
            break;
            

        

        case CALL_SUBRTN_OPCODE:
            stack[stackPointer] = pc;
            stackPointer++;
            pc = opcode & 0x0FFF;
            SDL_Log("Subrouteen called");
            break;

        case JMP_OPCODE:
            pc = opcode & 0x0FFF;
            SDL_Log("Jumped too %u", pc);
            break;

        case INC_PC_VK_EQUAL:
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
                SDL_Log("VK equal too kk skipping");
            } else {
                pc += 2;
                SDL_Log("VK not equal to kk");
            }
            break;

        case INC_PC_VK_NEQUAL:
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
                SDL_Log("VK not equal to kk skipping");
            } else {
                pc += 2;
                SDL_Log("VK equal too kk");
            }
            break;

        case INC_PC_VK_VY_EQUAL:
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
                SDL_Log("VK and VY equal, skipping");
            } else {
                pc += 2;
                SDL_Log("VK and VY not equal");
            }
            break;

        case MOV_KK_TO_VX:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            SDL_Log("%X moved to Vk", (opcode & 0x00FF));
            break;

        case ADD_VX_KK:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            SDL_Log("VK added to KK");
            break;

        case 0x8000:
            switch (opcode & 0xF00F) {
                case MOV_VY_TO_VX:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    SDL_Log("VY moved to Vk");
                    break;

                case OR_VX_VY:
                    SDL_Log("VX AND VY OR");
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                
                case AND_VX_VY:
                    SDL_Log("VX and VY AND");
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case XOR_VX_VY:
                    SDL_Log("VX and VY XOR");
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case ADD_VX_VY_OPCODE:
                    SDL_Log("Added vx and vy");
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode &0x0F00) >> 8])) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case SUB_VX_VY:
                    SDL_Log("VX and VY sub");
                    if (V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case SHR_VX:
                    SDL_Log("SHR VX");
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode &0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                case SUB_VY_VX:
                    SDL_Log("Sub VY and VX");
                    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case MULT_VX_2:
                    SDL_Log("Mult VX 2");
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
            }
            break;
            

        case INC_PC_VK_VY_NEQUAL:
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
                SDL_Log("VK and VY nequal skipping");
            } else {
                pc += 2;
                SDL_Log("VK and VY equal");
            }
            break;

        case SET_INDEX_REG_OPCODE:
            indexRegister = opcode & 0x0FFF;
            pc += 2;
            SDL_Log("Setting index reg too: %i", indexRegister);
            break;

        case JMP_NNN_PLUS_V0:
            pc = (opcode & 0x0FFF) + V[0];
            SDL_Log("Jumped to nnn plus v0");
            break;

        case RAND:
            V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
            pc += 2;
            SDL_Log("Rand number generated");
            break;

        case DRAW:
        {
            SDL_Log("Draw opcode called");
            unsigned short Vx{ V[(opcode & 0x0F00) >> 8] };
            unsigned short Vy{ V[(opcode & 0x00F0) >> 4] };
            unsigned short height{ static_cast<unsigned short>(opcode & 0x000F) };
            unsigned short pixel;

            V[0xF] = 0;
            for (int yLine{0}; yLine < height; yLine++) {
                pixel = memory[indexRegister + yLine];
                for (int xLine{0}; xLine < 8; xLine++) {
                    if ((pixel & (0x80 >> xLine)) != 0) {
                        if (gfx[(Vx + xLine + ((Vy + yLine) * 64))] == 1) {
                            V[0xF] = 1;
                        }
                        gfx[Vx + xLine + ((Vy + yLine) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
        }
        break;

        case 0xE000:
            switch (opcode & 0x000F) {
                case IS_KEY_DOWN:
                    SDL_Log("Key down check");
                    if (key[V[(opcode & 0x0F00) >> 8]] == 1) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;

                case IS_KEY_UP:
                    SDL_Log("Key up check");
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
            }
            break;
            
        case 0xF000:
            switch (opcode & 0x00FF) {
                case VX_DELAY_TIMER:
                    SDL_Log("Delay timer mov to VK");
                    V[(opcode & 0x0F00) >> 8] = delayTimer;
                    pc += 2;
                    break;

                case IS_KEY_PRESSED:
                {
                    SDL_Log("Is key pressed loop");
                    bool isKeyPressed{false};

                    for (int i{0}; i < 16; i++) {
                        if (key[i] == 1) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            isKeyPressed = true;
                        }
                    }

                    if (!isKeyPressed) {
                        return;
                    }

                    pc += 2;
                    break;
                }

                case SOUND_EQUALS_VX:
                    SDL_Log("Sound equals VX");
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case ADD_I_VX:
                    SDL_Log("I added to VX");
                    indexRegister += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case I_EQUALS_SPITE:
                    SDL_Log("I equals sprite");
                    indexRegister = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                case DELAY_EQUALS_VX:
                    SDL_Log("Delay equals VX");
                    delayTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case VX_BCD_IN_MEM_OPCODE:
                    SDL_Log("VX BCD MEM");
                    memory[indexRegister] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[indexRegister + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[indexRegister + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;

                case COPY_V0_TO_VX_TO_MEM:
                {
                    SDL_Log("V0 to VX copied to mem");
                    for (int i{0}; i <= ((opcode & 0x0F00) >> 8); i++) {
                        memory[indexRegister+i] = V[i];
                    }

                   // indexRegister += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                }

                case READ_V0_TO_VX_FROM_MEM:
                {
                    SDL_Log("V0 to VX read from mme");
                    for (int i{0}; i <= ((opcode & 0x0F00) >> 8); i++) {
                        V[i] = memory[indexRegister + i];
                    }

                    //indexRegister = ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                }
            } 
            break;

        default:
            SDL_LogError(0, "Unknown opcode 0x%X", opcode);
            break;
    }

    //update timers
    if (delayTimer > 0) {
        delayTimer--;
    }

    if (soundTimer > 0) {
        SDL_Log("buzzer, soundTimer: %i", soundTimer);
        soundTimer--;
    }
}

bool Chip8::shouldDraw() {
    return drawFlag;
}

bool Chip8::GameLoaded() {
    return gameLoaded;
}

void Chip8::drawn() {
    drawFlag = false;
}


