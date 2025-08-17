#include <cstring>
#include <random>
#include <SDL3/SDL_log.h>
#include "Chip-8.hpp"

void Chip8::Initilize() {
    pc = 0x200;
    opcode = 0;
    indexRegister = 0;
    stackPointer = 0;

    for (unsigned char pixel : gfx) {
        pixel = 0;
    }

    for (unsigned short stackObj : stack) {
        stackObj = 0;
    }

    for (unsigned char reg : V) {
        reg = 0;
    }

    for (unsigned char mem : memory) {
        mem = 0;
    }

    // load fontset

    // reset timers
    delayTimer = 0;
    soundTimer = 0;
}

void Chip8::EmulateCycle() {
    /* Fetch 
    Data is stored in 2 bytes, each array index is one 
    The first byte is shifted to the left, then the secnond byte is OR'ed to merge the two opcodes */
    opcode = memory[pc] << 8 | memory[pc+1];

    /* Decode
    when the opcode is AND'ed against 0xF000 the first byte remains but the remainin are set to 0 */
    switch (opcode & 0xF000) {
        case SET_INDEX_REG_OPCODE:
            indexRegister = opcode & 0x0FFF;
            pc += 2;
            SDL_Log("Setting index reg too: %i", indexRegister);
            break;

        case CALL_SUBRTN_OPCODE:
            stack[stackPointer] = pc;
            stackPointer++;
            pc = opcode & 0x0FFF;
            break;

        case JMP_OPCODE:
            pc = opcode & 0x0FFF;
            break;

        case INC_PC_VK_EQUAL:
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case INC_PC_VK_NEQUAL:
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case INC_PC_VK_VY_EQUAL:
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case MOV_KK_TO_VX:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        case ADD_VX_KK:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        case MOV_VY_TO_VX:
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case INC_PC_VK_VY_NEQUAL:
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case JMP_NNN_PLUS_V0:
            pc = (opcode & 0x0FFF) + V[0];
            break;

        case RAND:
            V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
            pc += 2;
            break;

        case DRAW:
            unsigned short Vx{ V[(opcode & 0x0F00) >> 8] };
            unsigned short Vy{ V[(opcode & 0x00F0) >> 4] };
            unsigned short height{ opcode * 0x000F };
            unsigned short pixel;

            V[0xF] = 0;
            for (int yLine{0}; yLine < height; yLine++) {
                pixel = memory[indexRegister + yLine];
                for (int xLine{0}; xLine < 8; xLine++) {
                    if (gfx[(Vx + xLine + ((Vy + yLine) * 64))]) {
                        V[0xF] = 1;
                    }
                    gfx[Vx + xLine + ((Vy + yLine) * 64)] ^= 1;

                    drawFlag = true;
                    pc += 2;
                    break;
                }
            }

        case 0x0000:
            switch (opcode & 0x000F) {
                case CLS_SCREEN_OPCODE:
                    break;

                case RET_FROM_SUBRTN_OPCODE:
                    stackPointer--;
                    pc = stack[stackPointer];
                    break;

                case ADD_VX_VY_OPCODE:
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode &0x0F00)] >> 8)) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
            }
            break;

            switch (opcode & 0x00FF) {
                case VX_BCD_IN_MEM_OPCODE:
                    memory[indexRegister] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[indexRegister + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[indexRegister + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
            }
            break;

            switch (opcode & 0xF00F) {
                case OR_VX_VY:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                
                case AND_VX_VY:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case XOR_VX_VY:
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case SUB_VX_VY:
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case SHR_VX:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode &0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                case SUB_VY_VX:
                    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case MULT_VX_2:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
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
