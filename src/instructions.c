#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stack.h"

#include "instructions.h"

static uint8_t get_random_uint8()
{
    return (uint8_t)(rand() % 256);
}

void execute_instruction(Chip8 *chip8, const Chip8Config *config, const Instruction *instruction)
{
    switch (instruction->inst_cat)
    {
    case 0x0:
    {
        // 00E0 - Clear the display.
        if (instruction->opcode == 0x00E0)
        {
            memset(chip8->display, 0, sizeof(chip8->display));
        }
        else if (instruction->opcode == 0x00EE)
        {
            // 00EE - Return from a subroutine.
            chip8->pc = pop();
        }
        break;
    }

    case 0x1:
    {
        // 1NNN - Jump to address NNN.
        chip8->pc = instruction->NNN;
        break;
    }

    case 0x2:
    {
        // 2NNN - Call subroutine at address NNN.
        push(chip8->pc);
        chip8->pc = instruction->NNN;
        break;
    }

    case 0x3:
    {
        // 3XNN - Skip next instruction if VX equals NN.
        if (chip8->V[instruction->X] == instruction->NN)
            chip8->pc += 2;
        break;
    }

    case 0x4:
    {
        // 4XNN - Skip next instruction if VX does not equal NN.
        if (chip8->V[instruction->X] != instruction->NN)
            chip8->pc += 2;
        break;
    }

    case 0x5:
    {
        // 5XY0 - Skip next instruction if VX equals VY.
        if (chip8->V[instruction->X] == chip8->V[instruction->Y])
            chip8->pc += 2;
        break;
    }

    case 0x9:
    {
        // 9XY0 - Skip next instruction if VX does not equal VY.
        if (chip8->V[instruction->X] != chip8->V[instruction->Y])
            chip8->pc += 2;
        break;
    }

    case 0x6:
    {
        // 6XNN - Set VX to NN.
        chip8->V[instruction->X] = instruction->NN;
        break;
    }

    case 0x7:
    {
        // 7XNN - Add NN to VX without changing VF.
        chip8->V[instruction->X] += instruction->NN;
        break;
    }

    case 0x8:
        switch (instruction->N)
        {
        case 0x0:
        {
            // 8XY0 - Set VX to VY.
            chip8->V[instruction->X] = chip8->V[instruction->Y];
            break;
        }
        case 0x1:
        {
            // 8XY1 - Set VX to VX OR VY. The original interpreter also leaves VF as 0.
            chip8->V[instruction->X] = chip8->V[instruction->X] | chip8->V[instruction->Y];
            if (!config->super_mode)
                chip8->V[0xF] = 0;
            break;
        }
        case 0x2:
        {
            // 8XY2 - Set VX to VX AND VY.
            chip8->V[instruction->X] = chip8->V[instruction->X] & chip8->V[instruction->Y];
            if (!config->super_mode)
                chip8->V[0xF] = 0;
            break;
        }
        case 0x3:
        {
            // 8XY3 - Set VX to VX XOR VY.
            chip8->V[instruction->X] = chip8->V[instruction->X] ^ chip8->V[instruction->Y];
            if (!config->super_mode)
                chip8->V[0xF] = 0;
            break;
        }
        case 0x4:
        {
            // 8XY4 - Add VY to VX; VF is set on carry.
            uint16_t sum = (uint16_t)chip8->V[instruction->X] + (uint16_t)chip8->V[instruction->Y];
            chip8->V[instruction->X] = (uint8_t)(sum & 0xFF);
            chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
            break;
        }

        case 0x5:
        {
            // 8XY5 - Subtract VY from VX; VF is set when no borrow occurs.
            bool flag = chip8->V[instruction->X] >= chip8->V[instruction->Y];
            chip8->V[instruction->X] = chip8->V[instruction->X] - chip8->V[instruction->Y];
            chip8->V[0xF] = flag;
            break;
        }
        case 0x7:
        {
            // 8XY7 - Set VX to VY minus VX; VF is set when no borrow occurs.
            bool flag = chip8->V[instruction->X] <= chip8->V[instruction->Y];
            chip8->V[instruction->X] = chip8->V[instruction->Y] - chip8->V[instruction->X];
            chip8->V[0xF] = flag;
            break;
        }

        case 0x6:
        {
            // 8XY6 - Shift VX right; VF receives the least significant bit.
            if (!config->super_mode)
                chip8->V[instruction->X] = chip8->V[instruction->Y];
            bool flag = chip8->V[instruction->X] & 1;
            chip8->V[instruction->X] = chip8->V[instruction->X] >> 1;
            chip8->V[0xF] = flag;
            break;
        }
        case 0xE:
        {
            // 8XYE - Shift VX left; VF receives the most significant bit.
            if (!config->super_mode)
                chip8->V[instruction->X] = chip8->V[instruction->Y];
            bool flag = (chip8->V[instruction->X] >> 7) & 1;
            chip8->V[instruction->X] = chip8->V[instruction->X] << 1;
            chip8->V[0xF] = flag;
            break;
        }
        default:
            break;
        }
        break;

    case 0xA:
    {
        // ANNN - Set I to address NNN.
        chip8->I = instruction->NNN;
        break;
    }

    case 0xB:
    {
        // BNNN / BXNN - Jump to NNN plus V0 or VX in SUPER-CHIP mode.
        if (!config->super_mode)
        {
            chip8->pc = instruction->NNN + chip8->V[0];
        }
        else
        {
            chip8->pc = instruction->NNN + chip8->V[instruction->X];
        }
        break;
    }

    case 0xC:
    {
        // CXNN - Set VX to a random byte AND NN.
        chip8->V[instruction->X] = get_random_uint8() & instruction->NN;
        break;
    }

    case 0xD:
    {
        // DXYN - Draw an N-byte sprite at (VX, VY); VF is set on collision.
        uint8_t x_pos = chip8->V[instruction->X] % 64;
        uint8_t y_pos = chip8->V[instruction->Y] % 32;
        uint8_t height = instruction->N;
        chip8->V[0xF] = 0;

        for (int row = 0; row < height; row++)
        {
            if (y_pos + row >= 32)
                break;

            uint8_t sprite_byte = chip8->memory[chip8->I + row];

            for (uint8_t col = 0; col < 8; col++)
            {
                if (x_pos + col >= 64)
                    break;

                if ((sprite_byte & (0x80 >> col)) != 0)
                {
                    if (chip8->display[x_pos + col][y_pos + row])
                    {
                        chip8->V[0xF] = 1;
                        chip8->display[x_pos + col][y_pos + row] = false;
                    }
                    else
                    {
                        chip8->display[x_pos + col][y_pos + row] = true;
                    }
                }
            }
        }
        break;
    }

    case 0xE:
    {
        switch (instruction->NN)
        {
        case 0x9E:
        {
            // EX9E - Skip next instruction if the key in VX is pressed.
            // Only the low nibble names a key; anything more would read past keys[].
            uint8_t key = chip8->V[instruction->X] & 0xF;
            if (chip8->keys[key])
                chip8->pc += 2;
            break;
        }

        case 0xA1:
        {
            // EXA1 - Skip next instruction if the key in VX is not pressed.
            uint8_t key = chip8->V[instruction->X] & 0xF;
            if (!chip8->keys[key])
                chip8->pc += 2;
            break;
        }

        default:
            break;
        }
        break;
    }

    case 0xF:
        switch (instruction->NN)
        {
        case 0x07:
        {
            // FX07 - Set VX to the delay timer.
            chip8->V[instruction->X] = chip8->delay_timer;
            break;
        }
        case 0x15:
        {
            // FX15 - Set the delay timer to VX.
            chip8->delay_timer = chip8->V[instruction->X];
            break;
        }
        case 0x18:
        {
            // FX18 - Set the sound timer to VX.
            chip8->sound_timer = chip8->V[instruction->X];
            break;
        }
        default:
            break;

        case 0x1E:
        {
            // FX1E - Add VX to I; VF is set on a 12-bit address overflow.
            if ((chip8->I += chip8->V[instruction->X]) > 0xFFF)
            {
                chip8->V[0xF] = 1;
                chip8->I &= 0xFFF;
            }
            break;
        }

        case 0x0A:
        {
            // FX0A - Wait for a key to be pressed and released, then store it in VX.
            // The original hardware moves on when the key comes back up, not when it goes down.
            if (chip8->key_pending == 0)
            {
                for (uint8_t key = 0; key < 16; key++)
                {
                    if (chip8->keys[key])
                    {
                        chip8->key_pending = key + 1;
                        break;
                    }
                }
                chip8->pc -= 2;
                break;
            }

            if (chip8->keys[chip8->key_pending - 1])
            {
                chip8->pc -= 2;
                break;
            }

            chip8->V[instruction->X] = chip8->key_pending - 1;
            chip8->key_pending = 0;
            break;
        }

        case 0x29:
        {
            // FX29 - Set I to the font sprite for the digit in VX.
            chip8->I = 0x050 + (chip8->V[instruction->X] * 5);
            break;
        }

        case 0x33:
        {
            // FX33 - Store the BCD digits of VX at memory[I..I+2].
            chip8->memory[chip8->I] = chip8->V[instruction->X] / 100;
            chip8->memory[chip8->I + 1] = (chip8->V[instruction->X] / 10) % 10;
            chip8->memory[chip8->I + 2] = chip8->V[instruction->X] % 10;
            break;
        }

        case 0x55:
        {
            // FX55 - Store V0 through VX in memory starting at I.
            // The original interpreter leaves I pointing past the last byte written.
            for (int i = 0; i <= instruction->X; i++)
            {
                chip8->memory[chip8->I + i] = chip8->V[i];
            }
            if (!config->super_mode)
                chip8->I += instruction->X + 1;
            break;
        }

        case 0x65:
        {
            // FX65 - Load V0 through VX from memory starting at I.
            for (int i = 0; i <= instruction->X; i++)
            {
                chip8->V[i] = chip8->memory[chip8->I + i];
            }
            if (!config->super_mode)
                chip8->I += instruction->X + 1;
            break;
        }
        }

    default:
        break;
    }
}
