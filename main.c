#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> // Required for EXIT_FAILURE

#define CPU_NAME "Silver8 Gen1"
static int MEM_SIZE = 1024; // system memory in bytes
static int VMEM_SIZE = 256; // video memory for graphics in bytes
#define MAX_PROGRAM_SIZE 512 // maximum size of the programs
#define REG_COUNT 2 // cpu registers
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 16

static bool debugging = true;

// Opcodes
enum {
    NOP, // 0x00 - no operation
    LOAD, // 0x01 - load value
    ADD,  // 0x02 - addition
    SUB, // 0x03 - subtraction
    JMP, // 0x04 - jump to address
    JZ, // 0x05 - jump if zero
    HALT, // 0x06 - halt execution of programs
    MUL, // 0x07 - multiplicaton
    DIV,// 0x08 division
    PRNTCH, // 0x09 - print character from video memory
    PRNTREG, // 0x0A - register value
    PRNTVMEM, // 0x0B - print video memory
    IF, // 0x0C - if statement
    PRNTMEM, // 0x0D - print memory
    STORE, // 0x0E - store value
    PRNTFREE, // 0x0F - print free memory
    PRNTFREEV, // 0x10 - print free video memory
    RENDER, // 0x11 - render graphics
    STOREVMEM, // 0x12 - store value in video memory
    CLR, // 0x13 - clear screen
};

char characters[] = {
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', 
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 
    '-', '_', '=', '+', '[', ']', '{', '}', '\\', '|', 
    ';', ':', '\'', '"', ',', '<', '.', '>', '/', '?', 
    '`', '~', 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 
    'z', 'x', 'c', 'v', 'b', 'n', 'm', '\n', 
    ' '
};

// CPU Structure
typedef struct {
    uint8_t memory[MEM_SIZE];
    uint8_t registers[REG_COUNT];
    uint8_t pc;  // Program Counter
    uint64_t executed_instructions; // Count executed instructions
    char video_memory[SCREEN_WIDTH * SCREEN_HEIGHT]; // Video memory for graphics
    bool running;
} CPU;

// Fetch instruction
uint8_t fetch(CPU *cpu) {
    return cpu->memory[cpu->pc++];
}

// Decode & Execute instruction
void execute(CPU *cpu) {
    uint8_t opcode = fetch(cpu);
    cpu->executed_instructions++; // Count each instruction executed

    switch (opcode) {
        case NOP: break;
        case LOAD: {
            uint8_t reg = fetch(cpu); // the cpu has 4 registers, you can assign values to a register 0-3.
            uint8_t value = fetch(cpu); // input any 8 bit value
            cpu->registers[reg] = value; // assigns value to register
            break;
        }
        case ADD: {
            uint8_t reg1 = fetch(cpu); // fetches register 0
            uint8_t reg2 = fetch(cpu); // fetches register 1
            cpu->registers[reg1] += cpu->registers[reg2]; // adds the values of each register together
            break;
        }
        case SUB: {
            uint8_t reg1 = fetch(cpu); // fetches register 0
            uint8_t reg2 = fetch(cpu); // fetches register 1
            cpu->registers[reg1] -= cpu->registers[reg2]; // subtracts the values of each register
            break;
        }
        case JMP: {
            uint8_t addr = fetch(cpu);
            cpu->pc = addr;
            break;
        }
        case JZ: {
            uint8_t addr = fetch(cpu);
            if (cpu->registers[0] == 0)
                cpu->pc = addr;
            break;
        }
        case HALT:
            cpu->running = false;
            break;
        case MUL: {
            uint8_t reg1 = fetch(cpu);
            uint8_t reg2 = fetch(cpu);
            cpu->registers[reg1] *= cpu->registers[reg2];
            break;
        }
        case DIV: {
            uint8_t reg1 = fetch(cpu);
            uint8_t reg2 = fetch(cpu);
            if (cpu->registers[reg2] != 0) {
                cpu->registers[reg1] /= cpu->registers[reg2];
            }
            break;
        }
        case PRNTCH: // syntax: PRNTCH, index 
        {
            uint8_t char_index = fetch(cpu); // Fetch the index of the character to print
            if (char_index < MEM_SIZE) {
                char character = cpu->memory[char_index]; // Get the character from video memory
                // printf("PRNTCH: Fetching character '%c' from video memory index %d\n", character, char_index);
                printf("%c", character); // Print the character to the console
            } else {
                printf("Invalid character index: %d\n", char_index);
            }
            break;
        }
        case PRNTREG: {
            uint8_t reg = fetch(cpu); // fetches the register to print
            if (reg < REG_COUNT) {
                printf("%d %d", reg, cpu->registers[reg]);
            } else {
                printf("Invalid register index: %d\n", reg);
            }
            break;
        }
        case PRNTVMEM: {
            printf("Video Memory: ");
            for (int i = 0; i < VMEM_SIZE; i++) {
                if (cpu->video_memory[i] != '\0') { // Only print non-empty characters
                    printf("%c", cpu->video_memory[i]);
                } else {
                    printf("."); // Placeholder for empty slots
                }
            }
            printf("\n");
            printf("Video Memory Size: %d bytes\n", VMEM_SIZE);
            break;
        }
        case PRNTMEM: {
            printf("Memory: ");
            for (int i = 0; i < MEM_SIZE; i++) {
                if (cpu->memory[i] != 0) { // Only print non-zero values
                    printf("%c", cpu->memory[i]);
                } else {
                    printf("."); // Placeholder for empty slots
                }
            }
            printf("\n");
            printf("Memory Size: %d bytes\n", MEM_SIZE);
            break;
        }
        case IF: {
            uint8_t condition = fetch(cpu); // Fetch the condition (0 for false, 1 for true)
            uint8_t true_addr = fetch(cpu); // Fetch the address to jump if condition is true
            uint8_t false_addr = fetch(cpu); // Fetch the address to jump if condition is false
            if (condition == 1) {
                cpu->pc = true_addr; // Jump to true address if condition is true
            } else {
                cpu->pc = false_addr; // Jump to false address if condition is false
            }
            break;
        }
        case STORE: {
            uint8_t reg = fetch(cpu);  // Fetch the register containing the value to store
            uint8_t addr = fetch(cpu); // Fetch the memory address to store the value
            if (addr < MEM_SIZE) {
                cpu->memory[addr] = cpu->registers[reg]; // Store the value in system memory
            } else if (addr < MEM_SIZE + SCREEN_WIDTH * SCREEN_HEIGHT) {
                addr -= MEM_SIZE; // Adjust address for video memory
                cpu->video_memory[addr] = cpu->registers[reg]; // Store the value in video memory
            } else {
                printf("Invalid memory address: %d\n", addr);
            }
            break;
        }
        case PRNTFREE: {
            int free_memory = 0;
            for (int i = 0; i < MEM_SIZE; i++) {
                if (cpu->memory[i] == 0) { // Count empty memory slots
                    free_memory++;
                }
            }
            printf("Free Memory: %d bytes\n", free_memory);
            break;
        }
        case PRNTFREEV: {
            int free_memory = 0;
            for (int i = 0; i < VMEM_SIZE; i++) {
                if (cpu->video_memory[i] == 0) { // Count empty memory slots
                    free_memory++;
                }
            }
            printf("Free Video Memory: %d bytes\n", free_memory);
            break;
        }
        case RENDER: {
            printf("\n---- Output ----\n");
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    char c = cpu->video_memory[y * SCREEN_WIDTH + x];
                    if (c == '\0') {
                        printf("."); // Empty cells are displayed as dots
                    } else {
                        printf("%c", c); // Print the character
                    }
                }
                printf("\n"); // Newline after each row
            }
            printf("----------------\n");
            break;
        }
        case STOREVMEM: {
            uint8_t reg = fetch(cpu);  // Fetch the register containing the value to store
            uint8_t x = fetch(cpu);    // Fetch the x coordinate for video memory
            uint8_t y = fetch(cpu);    // Fetch the y coordinate for video memory
            if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
                int addr = y * SCREEN_WIDTH + x; // Calculate the address in video memory
                cpu->video_memory[addr] = cpu->registers[reg]; // Store the value in video memory
            } else {
                printf("Invalid video memory coordinates: (%d, %d)\n", x, y);
            }
            break;
        }
        case CLR: { // quite literally just clears vram
            for (int i = 0; i < MEM_SIZE; i++) {
                cpu->memory[i] = '\0'; // clear system memory
            }
            printf("memory cleared.\n");
            break;
        }
        default:
            printf("Unknown instruction: %02X\n", opcode);
            cpu->running = false;
            break;
    }
}

// Load program into memory
void load_program(CPU *cpu, uint8_t *program, size_t size) {
    for (size_t i = 0; i < size && i < MEM_SIZE; i++) {
        cpu->memory[i] = program[i];
    }
}

// Map opcode names to their numeric values
int get_opcode(const char *mnemonic) {
    if (strcmp(mnemonic, "NOP") == 0) return NOP;
    if (strcmp(mnemonic, "LOAD") == 0) return LOAD;
    if (strcmp(mnemonic, "ADD") == 0) return ADD;
    if (strcmp(mnemonic, "SUB") == 0) return SUB;
    if (strcmp(mnemonic, "JMP") == 0) return JMP;
    if (strcmp(mnemonic, "JZ") == 0) return JZ;
    if (strcmp(mnemonic, "HALT") == 0) return HALT;
    if (strcmp(mnemonic, "MUL") == 0) return MUL;
    if (strcmp(mnemonic, "DIV") == 0) return DIV;
    if (strcmp(mnemonic, "PRNTCH") == 0) return PRNTCH;
    if (strcmp(mnemonic, "PRNTREG") == 0) return PRNTREG;
    if (strcmp(mnemonic, "PRNTVMEM") == 0) return PRNTVMEM;
    if (strcmp(mnemonic, "IF") == 0) return IF;
    if (strcmp(mnemonic, "PRNTMEM") == 0) return PRNTMEM;
    if (strcmp(mnemonic, "STORE") == 0) return STORE;
    if (strcmp(mnemonic, "PRNTFREE") == 0) return PRNTFREE;
    if (strcmp(mnemonic, "PRNTFREEV") == 0) return PRNTFREEV;  
    if (strcmp(mnemonic, "RENDER") == 0) return RENDER;
    if (strcmp(mnemonic, "STOREVMEM") == 0) return STOREVMEM;
    if (strcmp(mnemonic, "CLR") == 0) return CLR;
    return -1; // Invalid opcode
}

// Parse the .s8 file and populate the program array
size_t parse_s8_file(const char *filename, uint8_t *program) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    size_t program_size = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ", \n");
        while (token) {
            if (program_size >= MAX_PROGRAM_SIZE) {
                fprintf(stderr, "Program size exceeds maximum limit\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // Get the opcode or operand
            int value = get_opcode(token);
            if (value == -1) {
                // If not an opcode, treat it as an operand
                value = atoi(token);
            }

            program[program_size++] = (uint8_t)value;
            token = strtok(NULL, ", \n");
        }
    }

    fclose(file);
    return program_size;
}

// Run the CPU and measure speed
void run(CPU *cpu) {
    cpu->running = true;
    cpu->executed_instructions = 0;
    
    clock_t start_time = clock(); // Start timing

    while (cpu->running) {
        execute(cpu);
    }

    clock_t end_time = clock(); // End timing
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Calculate instructions per second (IPS)
    double mips = (cpu->executed_instructions / elapsed_time)/1000000;
    printf("CPU executed %lu instructions in %.2f seconds\n", cpu->executed_instructions, elapsed_time);
    printf("%s @ %.2f MIPS\n", CPU_NAME, mips); // Print the CPU name and MIPS
}

int main() {
    CPU cpu = {0};
    // Initialize video memory with some characters
    for (int i = 0; i < MEM_SIZE; i++) {
        if (i < sizeof(characters)) {
            cpu.memory[i] = characters[i]; // Fill video memory with characters
        } else {
            cpu.memory[i] = '\0'; // Fill remaining with null characters
        }
    }

    // Load the program from a .s8 file
    uint8_t program[MAX_PROGRAM_SIZE]; // defines program array
    size_t program_size = parse_s8_file("program.s8", program); // loads program into array (if it'd work)

    // Load the program into memory
    load_program(&cpu, program, program_size);

    // Run the CPU
    run(&cpu);
    if(debugging) {
        printf("Executed Instructions: %lu\n", cpu.executed_instructions);
        for(int i = 0; i < program_size; i++) {
            printf("%02X ", program[i]); // Print the program in hex format
        }
    }
    
    printf("\n");
    return 0;
}