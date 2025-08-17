#pragma once

// Opcodes
#define SET_INDEX_REG_OPCODE 0xA000
#define CLS_SCREEN_OPCODE 0x0000
#define RET_FROM_SUBRTN_OPCODE 0x000E
#define CALL_SUBRTN_OPCODE 0x2000
#define ADD_VX_VY_OPCODE 0x0004
#define VX_BCD_IN_MEM_OPCODE 0x0033
#define JMP_OPCODE 0x1000
#define INC_PC_VK_EQUAL 0x3000
#define INC_PC_VK_NEQUAL 0x4000
#define INC_PC_VK_VY_EQUAL 0x5000
#define MOV_KK_TO_VX 0x6000
#define ADD_VX_KK 0x7000
#define MOV_VY_TO_VX 0x8000
#define OR_VX_VY 0x8001
#define AND_VX_VY 0x8002
#define XOR_VX_VY 0x8003
#define SUB_VX_VY 0x8005
#define SHR_VX 0x8006
#define SUB_VY_VX 0x8007
#define MULT_VX_2 0x800E
#define INC_PC_VK_VY_NEQUAL 0x9000
#define JMP_NNN_PLUS_V0 0xB000
#define RAND 0xC000
#define DRAW 0xD000

class Chip8 {
private:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short indexRegister;
    unsigned short pc;
    unsigned char gfx[64 * 32];
    unsigned char delayTimer;
    unsigned char soundTimer;
    unsigned short stack[16];
    unsigned short stackPointer;
    bool drawFlag;

public:
    unsigned char key[16];

    void Initilize();
    void EmulateCycle();
};