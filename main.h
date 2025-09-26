#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> // Required for EXIT_FAILURE
#include <unistd.h> // Required for accesss function

#define CPU_NAME "Silver8 Gen1"

extern uint32_t mem_size;
extern uint32_t vmem_size;
extern uint32_t max_program_size;
extern uint8_t reg_count;
extern uint32_t screen_width;
extern uint32_t screen_height;

extern char characters[];

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

// CPU Structure
typedef struct {
    uint8_t *memory;
    uint8_t *registers;
    uint8_t pc;  // Program Counter
    uint64_t executed_instructions; // Count executed instructions
    char *video_memory; // Video memory for graphics
    bool running;
} CPU;

uint8_t fetch(CPU *cpu);
void execute(CPU *cpu);
void load_program(CPU *cpu, uint8_t *program, size_t size);
int get_opcode(const char *mnemonic);
void run(CPU *cpu);
int main(int argc, char **argv);
