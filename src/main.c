#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL.h>

#include "display.h"
#include "instructions.h"
#include "timer.h"
#include "audio.h"

#include "main.h"

// The fontset is a predefined set of sprites for the hexadecimal digits 0-F. Each digit is represented by a 5-byte sprite, where each byte corresponds to a row of pixels in a 4x5 grid
const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static char *parse_arguments(const int argc, char *argv[], Chip8Config *config)
{
    if (argc < 2)
    {
        printf("Usage: %s <rom_file>\n", argv[0]);
        return nullptr;
    }

    char *filename = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-super") == 0)
        {
            config->super_mode = true;
        }
        else if (filename == NULL)
        {
            filename = argv[i];
        }
        else
        {
            printf("Error: Unknown argument '%s'\n", argv[i]);
            return nullptr;
        }
    }

    return filename;
}

static bool load_rom(Chip8 *chip8, const char *filename)
{
    if (filename == nullptr)
    {
        fprintf(stderr, "Error: No ROM file specified.\n");
        return false;
    }

    FILE *rom = fopen(filename, "rb");
     if (!rom)
    {
        fprintf(stderr, "Error: Could not open file: %s\n", filename);
        return false;
    }

    // Determine absolute file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    if (rom_size <= 0)
    {
        fprintf(stderr, "Error: ROM file is empty or invalid.\n");
        fclose(rom);
        return false;
    }

    const size_t max_size = sizeof(chip8->memory) - 0x200;
    if (rom_size > (long)max_size)
    {
        fprintf(stderr, "Error: ROM exceeds maximum memory (%zu bytes).\n", max_size);
        fclose(rom);
        return false;
    }

    size_t bytes_read = fread(chip8->memory + 0x200, 1, rom_size, rom);
    fclose(rom);

    if (bytes_read != (size_t)rom_size)
    {
        fprintf(stderr, "Error: Failed to read the entire ROM.\n");
        return false;
    }

    return true;
}

static void fetch_instruction(Chip8 *chip8, Instruction *instruction)
{
    instruction->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    instruction->inst_cat = instruction->opcode >> 12;
    instruction->X = (instruction->opcode >> 8) & 0x0F;
    instruction->Y = (instruction->opcode >> 4) & 0x0F;
    instruction->N = instruction->opcode & 0x0F;
    instruction->NN = (instruction->Y << 4) | instruction->N;
    instruction->NNN = (instruction->X << 8) | (instruction->Y << 4) | instruction->N;
    chip8->pc += 2;
}

static int chip8_key_from_scancode(const SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_1: return 0x1;
    case SDL_SCANCODE_2: return 0x2;
    case SDL_SCANCODE_3: return 0x3;
    case SDL_SCANCODE_4: return 0xC;
    case SDL_SCANCODE_Q: return 0x4;
    case SDL_SCANCODE_W: return 0x5;
    case SDL_SCANCODE_E: return 0x6;
    case SDL_SCANCODE_R: return 0xD;
    case SDL_SCANCODE_A: return 0x7;
    case SDL_SCANCODE_S: return 0x8;
    case SDL_SCANCODE_D: return 0x9;
    case SDL_SCANCODE_F: return 0xE;
    case SDL_SCANCODE_Z: return 0xA;
    case SDL_SCANCODE_X: return 0x0;
    case SDL_SCANCODE_C: return 0xB;
    case SDL_SCANCODE_V: return 0xF;
    default: return -1;
    }
}

int main(const int argc, char *argv[])
{
    srand((unsigned int)time(nullptr));

    Chip8 chip8 = {.pc = 0x200};
    Chip8Config config = {0};
    Instruction instruction;

    char *filename = parse_arguments(argc, argv, &config);

    if (!load_rom(&chip8, filename))
    {
        return EXIT_FAILURE;
    }

    memcpy(&chip8.memory[0x050], fontset, sizeof(fontset));

    // SDL Initializations
    Display disp = {0};
    if (!display_init(&disp))
    {
        fprintf(stderr, "Fatal Error: Failed to initialize display subsystem.\n");
        return EXIT_FAILURE;
    }

    if (!audio_init())
    {
        fprintf(stderr, "Fatal Error: Failed to initialize audio subsystem.\n");
        display_cleanup(&disp);
        return EXIT_FAILURE;
    }

    SDL_Event event;
    
    uint64_t cpu_timestamp = SDL_GetTicksNS();
    uint64_t timer_timestamp = SDL_GetTicksNS();

    // Main execution loop
    bool running = true;
    while (running)
    {
        // Watch for program counter out-of-bounds
        if (chip8.pc < 0x200 || chip8.pc >= sizeof(chip8.memory) - 1)
        {
            fprintf(stderr, "Fatal Error: PC out of bounds (0x%04X)\n", chip8.pc);
            running = false;
            break;
        }

        // Watch for keyboard inputs
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
                break;
            }

            if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
            {
                int chip8_key = chip8_key_from_scancode(event.key.scancode);

                if (chip8_key >= 0)
                    chip8.keys[chip8_key] = event.type == SDL_EVENT_KEY_DOWN;
            }
        }

        uint64_t current_timestamp = SDL_GetTicksNS();

        // 500Hz for the timers and display
        while (current_timestamp - cpu_timestamp >= CPU_INTERVAL_NS)
        {
            cpu_timestamp += CPU_INTERVAL_NS;
            fetch_instruction(&chip8, &instruction);
            execute_instruction(&chip8, &config, &instruction);
        }

        // 60Hz for the timers and display
        while (current_timestamp - timer_timestamp >= TIMER_INTERVAL_NS)
        {
            timer_timestamp += TIMER_INTERVAL_NS;
            display_render(&disp, chip8.display);
            decrement_delaytimer(&chip8);
            decrement_soundtimer(&chip8);
        }

        SDL_Delay(1);
    }

    // SDL Cleanup
    audio_cleanup();
    display_cleanup(&disp);

    return EXIT_SUCCESS;
}
