#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

Config load_config() {
    Config config = {0};
    FILE *config_file = fopen("config.json", "r");
    if (!config_file) {
        printf("Warning: Could not open config.json, using defaults\n");
        // Set default values
        config.mem_size = 4196;
        config.vmem_size = 1024;
        config.max_program_size = 512;
        config.reg_count = 2;
        config.screen_width = 16;
        config.screen_height = 16;
        return config;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        // Skip empty lines and lines with only whitespace
        if (line[0] == '\n' || line[0] == '\r') continue;
        
        // Remove newline and carriage return
        line[strcspn(line, "\r\n")] = 0;
        
        // Skip lines that don't contain quotes (not JSON key-value pairs)
        if (strchr(line, '"') == NULL) continue;
        
        // Find the key (between first set of quotes)
        char *key_start = strchr(line, '"');
        if (!key_start) continue;
        key_start++; // Skip the opening quote
        
        char *key_end = strchr(key_start, '"');
        if (!key_end) continue;
        *key_end = '\0'; // Null-terminate the key
        
        // Find the value (after the colon)
        char *value_start = strchr(key_end + 1, ':');
        if (!value_start) continue;
        value_start++; // Skip the colon
        
        // Skip whitespace
        while (*value_start == ' ' || *value_start == '\t') value_start++;
        
        // Remove trailing comma if present
        char *comma = strchr(value_start, ',');
        if (comma) *comma = '\0';
        
        // Remove trailing whitespace
        while (value_start[strlen(value_start) - 1] == ' ' || 
               value_start[strlen(value_start) - 1] == '\t') {
            value_start[strlen(value_start) - 1] = '\0';
        }
        
        // Parse the value
        int value = atoi(value_start);
        
        // Assign to appropriate config field
        if (strcmp(key_start, "MEM_SIZE") == 0) {
            config.mem_size = value;
        } else if (strcmp(key_start, "VMEM_SIZE") == 0) {
            config.vmem_size = value;
        } else if (strcmp(key_start, "MAX_PROGRAM_SIZE") == 0) {
            config.max_program_size = value;
        } else if (strcmp(key_start, "REG_COUNT") == 0) {
            config.reg_count = value;
        } else if (strcmp(key_start, "SCREEN_WIDTH") == 0) {
            config.screen_width = value;
        } else if (strcmp(key_start, "SCREEN_HEIGHT") == 0) {
            config.screen_height = value;
        }
    }
    fclose(config_file);
    
    return config;
}

// Parse the .s8 file and populate the program array
size_t parse_s8_file(const char *filename, uint8_t *program, size_t max_size) {
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
            if (program_size >= max_size) {
                fprintf(stderr, "Program size exceeds maximum limit (size: %zu, max: %zu)\n", program_size, max_size);
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

bool includes_extension(const char *file) {
    const char *ext = strrchr(file, '.');
    if (ext != NULL && strcmp(ext, ".s8") == 0) {
        return true;
    }
    return false;
}

bool file_exists(const char *file) {
    return access(file, F_OK) == 0;
}
