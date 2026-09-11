#ifndef CHIP_8_MAIN_H
#define CHIP_8_MAIN_H

#include <stdint.h>

#define CPU_INTERVAL_NS (1000000000ULL / 700)  // = 1,428,571 ns per tick
#define TIMER_INTERVAL_NS (1000000000ULL / 60) // = 16,666,666 ns per tick

extern const uint8_t fontset[80];

typedef struct
{
    uint8_t memory[4096];
    uint16_t pc;
    uint16_t I;
    uint8_t V[16];
    bool keys[16];
    bool display[64][32];
    uint8_t delay_timer;
    uint8_t sound_timer;
} Chip8;

typedef struct
{
    bool super_mode;
} Chip8Config;

typedef struct
{
    uint16_t opcode;
    uint8_t inst_cat;
    uint8_t X;
    uint8_t Y;
    uint8_t N;
    uint8_t NN;
    uint16_t NNN;
} Instruction;

#endif // CHIP_8_MAIN_H
