#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "quirks.h"
#include "env.h"

// Function to convert ZX Spectrum floating point to double
extern double zx2d(unsigned char *in);

void list_basic_vars(void) {
    int vars_addr, e_line_addr;
    unsigned char var_byte;
    char var_name;
    int var_len, dims, i, total_elements;
    
    // Get system variables addresses
    vars_addr = readword(0x5C4B);   // VARS system variable
    e_line_addr = readword(0x5C59); // E_LINE system variable

    printf("\nBASIC Variables:\n");
    printf("----------------\n");
    
    // Loop through variables area
    while (vars_addr < e_line_addr) {
        // Check for end marker (0x80)
        if (readbyte(vars_addr) == 0x80) {
            break;
        }

        var_byte = readbyte(vars_addr++);
        var_name = (var_byte & 0x1F) + 'A' - 1;  // Lower 5 bits for letter code

        // Get type bits (bits 5-7)
        unsigned char type_bits = var_byte & 0xE0;
        
        // Process based on type
        
        // Numeric variable: 0x60 (0110xxxx)
        if (type_bits == 0x60) {
            if (readbyte(vars_addr) == 0) {
                // Small integer format
                int sign = readbyte(vars_addr + 1) ? -1 : 1;
                int value = readbyte(vars_addr + 2) | (readbyte(vars_addr + 3) << 8);
                printf("%c = %d\n", var_name, sign * value);
            } else {
                // Floating point
                printf("%c = %lf\n", var_name, zx2d(mem + vars_addr));
            }
            vars_addr += 5;  // Skip 5-byte numeric value
        }
        // String variable: 0x40 (0100xxxx)
        else if (type_bits == 0x40) {
            // Read length as a word (2 bytes, little-endian)
            var_len = readbyte(vars_addr) | (readbyte(vars_addr + 1) << 8);
            vars_addr += 2;
            
            // Display string content (up to 20 chars)
            printf("%c$ = \"", var_name);
            
            for (i = 0; i < var_len && i < 20; i++) {
                unsigned char c = readbyte(vars_addr + i);
                printf("%c", isprint(c) ? c : '.');
            }
            
            if (var_len > 20) {
                printf("...\" (%d chars)\n", var_len);
            } else {
                printf("\"\n");
            }
            
            vars_addr += var_len;  // Skip string content
        }
        // Numeric array: 0x80 (1000xxxx)
        else if (type_bits == 0x80) {
            var_len = readword(vars_addr);
            vars_addr += 2;
            
            dims = readbyte(vars_addr++);
            
            printf("%c(", var_name);
            
            // Print dimensions
            total_elements = 1;
            for (i = 0; i < dims; i++) {
                int dim_size = readword(vars_addr);
                total_elements *= dim_size;
                printf("%d", dim_size);
                vars_addr += 2;
                if (i < dims - 1) printf(",");
            }
            
            printf(") = Numeric array [%d elements]\n", total_elements);
            
            // Skip array contents
            vars_addr += var_len - dims * 2 - 1;
        }
        // String array: 0xC0 (1100xxxx)
        else if (type_bits == 0xC0) {
            var_len = readword(vars_addr);
            vars_addr += 2;
            
            dims = readbyte(vars_addr++);
            
            printf("%c$(", var_name);
            
            // Print dimensions
            total_elements = 1;
            for (i = 0; i < dims; i++) {
                int dim_size = readword(vars_addr);
                total_elements *= dim_size;
                printf("%d", dim_size);
                vars_addr += 2;
                if (i < dims - 1) printf(",");
            }
            
            printf(") = String array [%d elements]\n", total_elements);
            
            // Skip array contents
            vars_addr += var_len - dims * 2 - 1;
        }
        // FOR loop control variable: 0xE0 (1110xxxx)
        else if (type_bits == 0xE0) {
            printf("%c (FOR loop control)\n", var_name);
            printf("  Current value: %lf\n", zx2d(mem + vars_addr));
            vars_addr += 5;
            printf("  Limit: %lf\n", zx2d(mem + vars_addr));
            vars_addr += 5;
            printf("  Step: %lf\n", zx2d(mem + vars_addr));
            vars_addr += 5;
            printf("  Loop line: %d\n", readword(vars_addr));
            vars_addr += 2;
            printf("  Statement: %d\n", readbyte(vars_addr++));
        }
        else {
            printf("%c = Unknown variable type (0x%02X)\n", var_name, var_byte);
            break;  // Exit to avoid infinite loop
        }
    }
}

