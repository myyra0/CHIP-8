#ifndef CHIP_8_INSTRUCTIONS_H
#define CHIP_8_INSTRUCTIONS_H

#include "main.h"

void execute_instruction(Chip8 *chip8, const Chip8Config *config, const Instruction *instruction);

#endif // CHIP_8_INSTRUCTIONS_H
