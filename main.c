#include "main.h"
#include "parse.h"

uint32_t mem_size = 4196;
uint32_t vmem_size = 1024;
uint32_t max_program_size = 512;
uint8_t reg_count = 2;
uint32_t screen_width = 16;
uint32_t screen_height = 16;

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
            if (char_index < mem_size) {
                char character = cpu->memory[char_index]; // Get the character from video memory
                printf("%c", character); // Print the character to the console
            } else {
                printf("Invalid character index: %d\n", char_index);
            }
            break;
        }
        case PRNTREG: {
            uint8_t reg = fetch(cpu); // fetches the register to print
            if (reg < reg_count) {
                printf("%d %d", reg, cpu->registers[reg]);
            } else {
                printf("Invalid register index: %d\n", reg);
            }
            break;
        }
        case PRNTVMEM: {
            printf("Video Memory: ");
            for (int i = 0; i < vmem_size; i++) {
                if (cpu->video_memory[i] != '\0') { // Only print non-empty characters
                    printf("%c", cpu->video_memory[i]);
                } else {
                    printf("."); // Placeholder for empty slots
                }
            }
            printf("\n");
            printf("Video Memory Size: %d bytes\n", vmem_size);
            break;
        }
        case PRNTMEM: {
            printf("Memory: ");
            for (int i = 0; i < mem_size; i++) {
                if (cpu->memory[i] != 0) { // Only print non-zero values
                    printf("%c", cpu->memory[i]);
                } else {
                    printf("."); // Placeholder for empty slots
                }
            }
            printf("\n");
            printf("Memory Size: %d bytes\n", mem_size);
            break;
        }
        case IF: {
            uint8_t condition = fetch(cpu); // Fetch the condition (0 for false, 1 for true)
            uint8_t true_addr = fetch(cpu); // Fetch the address to jump if condition is true
            uint8_t false_addr = fetch(cpu); // Fetch the address to jump if condition is false

            if (condition == 1) {
                if (true_addr < mem_size) {
                    cpu->pc = true_addr; // Jump to true address if condition is true
                } else {
                    printf("Invalid true address: %d\n", true_addr);
                    cpu->running = false; // Stop execution if the address is invalid
                }
            } else {
                if (false_addr < mem_size) {
                    cpu->pc = false_addr; // Jump to false address if condition is false
                } else {
                    printf("Invalid false address: %d\n", false_addr);
                    cpu->running = false; // Stop execution if the address is invalid
                }
            }
            break;
        }
        case STORE: {
            uint8_t reg = fetch(cpu);  // Fetch the register containing the value to store
            uint8_t addr = fetch(cpu); // Fetch the memory address to store the value
            if (addr < mem_size) {
                cpu->memory[addr] = cpu->registers[reg]; // Store the value in system memory
            } else if (addr < mem_size + screen_width * screen_height) {
                addr -= mem_size; // Adjust address for video memory
                cpu->video_memory[addr] = cpu->registers[reg]; // Store the value in video memory
            } else {
                printf("Invalid memory address: %d\n", addr);
            }
            break;
        }
        case PRNTFREE: {
            int free_memory = 0;
            for (int i = 0; i < mem_size; i++) {
                if (cpu->memory[i] == 0) { // Count empty memory slots
                    free_memory++;
                }
            }
            printf("Free Memory: %d bytes\n", free_memory);
            break;
        }
        case PRNTFREEV: {
            int free_memory = 0;
            for (int i = 0; i < vmem_size; i++) {
                if (cpu->video_memory[i] == 0) { // Count empty memory slots
                    free_memory++;
                }
            }
            printf("Free Video Memory: %d bytes\n", free_memory);
            break;
        }
        case RENDER: {
            printf("\n---- Output ----\n");
            for (int y = 0; y < screen_height; y++) {
                for (int x = 0; x < screen_width; x++) {
                    char c = cpu->video_memory[y * screen_width + x];
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
            if (x < screen_width && y < screen_height) {
                int addr = y * screen_width + x; // Calculate the address in video memory
                cpu->video_memory[addr] = cpu->registers[reg]; // Store the value in video memory
            } else {
                printf("Invalid video memory coordinates: (%d, %d)\n", x, y);
            }
            break;
        }
        case CLR: { // quite literally just clears vram
            for (int i = 0; i < mem_size; i++) {
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
    for (size_t i = 0; i < size && i < mem_size; i++) {
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
    printf("CPU executed %llu instructions in %.2f seconds\n", cpu->executed_instructions, elapsed_time);
    printf("%s @ %.2f MIPS\n", CPU_NAME, mips); // Print the CPU name and MIPS
}

int main(int argc, char **argv) {
    Config config = load_config();
    mem_size = config.mem_size;
    vmem_size = config.vmem_size;
    max_program_size = config.max_program_size;
    reg_count = config.reg_count;
    screen_width = config.screen_width;
    screen_height = config.screen_height;

    CPU cpu = {0};

    // Allocate memory for CPU components
    cpu.memory = malloc(mem_size);
    cpu.registers = malloc(reg_count);
    cpu.video_memory = malloc(screen_width * screen_height);
    
    if (!cpu.memory || !cpu.registers || !cpu.video_memory) {
        perror("Failed to allocate CPU memory");
        exit(EXIT_FAILURE);
    }
    
    // Initialize memory to zero
    memset(cpu.memory, 0, mem_size);
    memset(cpu.registers, 0, reg_count);
    memset(cpu.video_memory, 0, screen_width * screen_height);

    // Initialize video memory with some characters
    for (int i = 0; i < mem_size; i++) {
        if (i < sizeof(characters)) {
            cpu.memory[i] = characters[i]; // Fill video memory with characters
        } else {
            cpu.memory[i] = '\0'; // Fill remaining with null characters
        }
    }

    // Load the program from a .s8 file
    uint8_t *program = malloc(max_program_size); // Dynamic allocation
    if (!program) {
        perror("Failed to allocate program memory");
        exit(EXIT_FAILURE);
    }
    size_t program_size = 0;
    for (int i = 1; i < argc; i++) {
        if (includes_extension(argv[i])) {
            if (file_exists(argv[i])) {
                program_size = parse_s8_file(argv[i], program, max_program_size); // Loads the program into array from command line argument
            } else {
                perror("File doesn't exist!\nDefaulting to program.s8\n");
            }
        } else {
            char *filename_with_ext = (char *)malloc(strlen(argv[i]) + strlen(".s8") + 1);
            if (filename_with_ext == NULL) {
                perror("Failed to allocate memory\n");
                exit(EXIT_FAILURE);
            }
            strcpy(filename_with_ext, argv[i]);
            strcat(filename_with_ext, ".s8");
            if (file_exists(filename_with_ext)) {
                program_size = parse_s8_file(filename_with_ext, program, max_program_size);
                // program_size = parse_s8_file((char[]) {argv[i], '.s8', '\0'}, program);
            } else {
                perror("File doesn't exist!\nDefaulting to program.s8\n");
            }
            free(filename_with_ext); // Don't forget to free the allocated memory
        }
    }
    if (program_size == 0) {
        program_size = parse_s8_file("program.s8", program, max_program_size);
    }

    // Load the program into memory
    load_program(&cpu, program, program_size);

    // Run the CPU
    run(&cpu);

    // Clean up memory
    free(program);
    free(cpu.memory);
    free(cpu.registers);
    free(cpu.video_memory);
    
    printf("\n");
    return 0;
}