#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "main.h"

typedef struct {
    uint32_t mem_size;
    uint32_t vmem_size;
    uint32_t max_program_size;
    uint8_t  reg_count;
    uint32_t screen_width;
    uint32_t screen_height;
} Config;

Config load_config();

size_t parse_s8_file(const char *filename, uint8_t *program, size_t max_size);
bool includes_extension(const char *file);
bool file_exists(const char *file);
