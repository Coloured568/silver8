#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MEM_SIZE 256
#define VMEM_SIZE 64
#define REG_COUNT 4

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
    DIV // 0x08 division
};

// CPU Structure
typedef struct {
    uint8_t memory[MEM_SIZE];
    uint8_t registers[REG_COUNT];
    uint8_t pc;  // Program Counter
    bool running;
    uint64_t executed_instructions; // Count executed instructions
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
            uint8_t reg = fetch(cpu);
            uint8_t value = fetch(cpu);
            cpu->registers[reg] = value;
            break;
        }
        case ADD: {
            uint8_t reg1 = fetch(cpu);
            uint8_t reg2 = fetch(cpu);
            cpu->registers[reg1] += cpu->registers[reg2];
            break;
        }
        case SUB: {
            uint8_t reg1 = fetch(cpu);
            uint8_t reg2 = fetch(cpu);
            cpu->registers[reg1] -= cpu->registers[reg2];
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
    printf("Emulated CPU speed: %.2f million instructions per second (MIPS)\n", mips);
}

int main() {
    CPU cpu = {0};

    // Sample program: Multiply R0 by R1
    uint8_t program[] = {
        LOAD, 0, 10,  // Load 10 into R0
        LOAD, 1, 1,   // Load 5 into R1
        MUL, 0, 1,    // R0 = R0 * R1
        HALT          // Stop execution
    };

    load_program(&cpu, program, sizeof(program));
    run(&cpu);

    printf("Final Value in R0: %d\n", cpu.registers[0]); // Should print 50
    return 0;
}
