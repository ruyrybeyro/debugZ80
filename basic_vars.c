#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "quirks.h"
#include "env.h"

// Function to convert ZX Spectrum floating point to double
extern double zx2d(unsigned char *in);

// Function to print a short hex dump (16 bytes max)
void short_hexdump(int addr, int len) {
    int i;
    
    // Limit to 16 bytes for display
    int display_len = (len > 16) ? 16 : len;
    
    printf("  Memory [%04X]: ", addr);
    
    // Print hex values
    for (i = 0; i < display_len; i++) {
        printf("%02X ", readbyte(addr + i));
    }
    
    // Print ASCII representation
    printf(" | ");
    for (i = 0; i < display_len; i++) {
        unsigned char c = readbyte(addr + i);
        printf("%c", isprint(c) ? c : '.');
    }
    
    if (len > 16) {
        printf(" ... (%d more bytes)", len - 16);
    }
    
    printf("\n");
}

// Function to print hexdump limited to specific range
void limited_hexdump(USHORT start_addr, USHORT end_addr) {
    int i, j;
    int num_bytes = end_addr - start_addr;
    
    // Limit to max 320 bytes (20 lines)
    if (num_bytes > 320) {
        num_bytes = 320;
    }
    
    // Print hexdump in the same format as print_hexdump
    for (i = 0; i < num_bytes; i += 16) {
        printf(" %04X", start_addr + i);
        
        // Print hex values
        for (j = 0; j < 16 && (i + j) < num_bytes; j++) {
            printf(" %02X", readbyte(start_addr + i + j));
        }
        
        // Pad with spaces if line is incomplete
        for (; j < 16; j++) {
            printf("   ");
        }
        
        printf(" ");
        
        // Print ASCII characters
        for (j = 0; j < 16 && (i + j) < num_bytes; j++) {
            unsigned char c = readbyte(start_addr + i + j);
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        
        printf("\n");
    }
    
    // Indicate if data was truncated
    if (end_addr - start_addr > 320) {
        printf(" ... (additional data truncated)\n");
    }
}

void list_basic_vars(void) {
    int vars_addr, nxtlin_addr;
    int var_start_addr;
    unsigned char var_byte;
    char var_name;
    int var_len, dims, i, total_elements;
    
    // Get system variables addresses
    vars_addr = readword(0x5C4B);    // VARS system variable
    nxtlin_addr = readword(0x5C55);  // NXTLIN system variable

    printf("\nBASIC Variables (VARS=%04X to NXTLIN=%04X):\n", vars_addr, nxtlin_addr);
    printf("----------------\n");
    
    // Find 0x80 marker if it exists before NXTLIN
    int end_marker_addr = -1;
    for (i = vars_addr; i < nxtlin_addr; i++) {
        if (readbyte(i) == 0x80) {
            end_marker_addr = i;
            break;
        }
    }
    
    // Determine actual end of variables
    int vars_end = (end_marker_addr != -1) ? end_marker_addr + 1 : nxtlin_addr;
    
    // Show limited hexdump of the actual VARS area
    printf("\nFull hexdump of VARS area (%04X to %04X):\n", 
           vars_addr, vars_end);
    limited_hexdump(vars_addr, vars_end);
    printf("\n");
    
    // Loop through variables area
    while (vars_addr < nxtlin_addr) {
        var_start_addr = vars_addr; // Remember start address for this variable
        
        // Check for end marker (0x80)
        if (readbyte(vars_addr) == 0x80) {
            printf("End marker at %04X: 80\n", vars_addr);
            short_hexdump(vars_addr, 1);
            break;
        }

        var_byte = readbyte(vars_addr++);
        var_name = (var_byte & 0x1F) + 'A' - 1;  // Lower 5 bits for letter code

        // Get type bits (bits 5-7)
        unsigned char type_bits = var_byte & 0xE0;
        
        // Process based on type
        
        // Numeric variable: 0x60 (0110xxxx)
        if (type_bits == 0x60) {
            // Check if we have enough space for a numeric variable
            if (vars_addr + 5 > nxtlin_addr) {
                printf("[%04X] %c = Incomplete numeric variable (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            // Always show the variable header
            printf("[%04X] %c = ", var_start_addr, var_name);
            
            if (readbyte(vars_addr) == 0) {
                // Small integer format
                int sign = readbyte(vars_addr + 1) ? -1 : 1;
                int value = readbyte(vars_addr + 2) | (readbyte(vars_addr + 3) << 8);
                printf("%d\n", sign * value);
            } else {
                // Floating point
                printf("%lf\n", zx2d(mem + vars_addr));
            }
            
            printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
            printf("  Numeric value (5 bytes):\n");
            short_hexdump(vars_addr, 5);
            vars_addr += 5;  // Skip 5-byte numeric value
        }
        // String variable: 0x40 (0100xxxx)
        else if (type_bits == 0x40) {
            // Check if we have enough space for the string length
            if (vars_addr + 2 > nxtlin_addr) {
                printf("[%04X] %c$ = Incomplete string variable (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            // Read length as a word (2 bytes, little-endian)
            var_len = readbyte(vars_addr) | (readbyte(vars_addr + 1) << 8);
            
            // Check if we have enough space for the string content
            if (vars_addr + 2 + var_len > nxtlin_addr) {
                printf("[%04X] %c$ = Incomplete string (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                printf("  String length bytes: %02X %02X (Length: %d)\n", 
                       readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            printf("[%04X] %c$ = \"", var_start_addr, var_name);
            
            // Display string content (up to 20 chars)
            for (i = 0; i < var_len && i < 20; i++) {
                unsigned char c = readbyte(vars_addr + 2 + i);
                printf("%c", isprint(c) ? c : '.');
            }
            
            if (var_len > 20) {
                printf("...\" (%d chars)\n", var_len);
            } else {
                printf("\"\n");
            }
            
            printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
            printf("  String length bytes: %02X %02X (Length: %d)\n", 
                   readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
            printf("  String content:\n");
            short_hexdump(vars_addr + 2, var_len);
            
            vars_addr += 2;        // Skip length word
            vars_addr += var_len;  // Skip string content
        }
        // Numeric array: 0x80 (1000xxxx)
        else if (type_bits == 0x80) {
            // Check if we have enough space for the array length
            if (vars_addr + 2 > nxtlin_addr) {
                printf("[%04X] %c() = Incomplete numeric array (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            var_len = readword(vars_addr);
            
            // Check if we have enough space for dimensions byte
            if (vars_addr + 3 > nxtlin_addr) {
                printf("[%04X] %c() = Incomplete numeric array (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                       readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            dims = readbyte(vars_addr + 2);
            
            // Check if we have enough space for dimensions
            if (vars_addr + 3 + dims * 2 > nxtlin_addr) {
                printf("[%04X] %c() = Incomplete numeric array dimensions (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                       readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
                printf("  Dimensions byte: %02X (Dimensions: %d)\n", dims, dims);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            // Print array header
            printf("[%04X] %c(", var_start_addr, var_name);
            
            // Calculate header size
            int header_size = 3; // Length (2) + Dims (1)
            
            // Print dimensions and calculate total elements
            total_elements = 1;
            for (i = 0; i < dims; i++) {
                int dim_size = readbyte(vars_addr + 3 + i*2) | (readbyte(vars_addr + 4 + i*2) << 8);
                total_elements *= dim_size;
                printf("%d", dim_size);
                if (i < dims - 1) printf(",");
                header_size += 2;
            }
            
            printf(") = Numeric array [%d elements]\n", total_elements);
            
            printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
            printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                   readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
            printf("  Dimensions byte: %02X (Dimensions: %d)\n", readbyte(vars_addr + 2), dims);
            
            // Print dimensions bytes
            printf("  Dimension sizes:");
            for (i = 0; i < dims; i++) {
                printf(" %02X%02X", readbyte(vars_addr + 3 + i*2), readbyte(vars_addr + 4 + i*2));
            }
            printf("\n");
            
            // Show memory contents of full header
            printf("  Array header:\n");
            short_hexdump(vars_addr, header_size);
            
            // Check if the entire array fits within the variable area
            if (vars_addr + 2 + var_len > nxtlin_addr) {
                printf("  Warning: Array data extends beyond NXTLIN boundary\n");
                // Only skip to NXTLIN
                vars_addr = nxtlin_addr;
            } else {
                // Skip to next variable
                vars_addr += 2 + var_len; // Length (2) + Array data
            }
        }
        // String array: 0xC0 (1100xxxx)
        else if (type_bits == 0xC0) {
            // Check if we have enough space for the array length
            if (vars_addr + 2 > nxtlin_addr) {
                printf("[%04X] %c$() = Incomplete string array (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            var_len = readword(vars_addr);
            
            // Check if we have enough space for dimensions byte
            if (vars_addr + 3 > nxtlin_addr) {
                printf("[%04X] %c$() = Incomplete string array (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                       readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            dims = readbyte(vars_addr + 2);
            
            // Check if we have enough space for dimensions
            if (vars_addr + 3 + dims * 2 > nxtlin_addr) {
                printf("[%04X] %c$() = Incomplete string array dimensions (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                       readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
                printf("  Dimensions byte: %02X (Dimensions: %d)\n", dims, dims);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            // Print array header
            printf("[%04X] %c$(", var_start_addr, var_name);
            
            // Calculate header size
            int header_size = 3; // Length (2) + Dims (1)
            
            // Print dimensions and calculate total elements
            total_elements = 1;
            for (i = 0; i < dims; i++) {
                int dim_size = readbyte(vars_addr + 3 + i*2) | (readbyte(vars_addr + 4 + i*2) << 8);
                total_elements *= dim_size;
                printf("%d", dim_size);
                if (i < dims - 1) printf(",");
                header_size += 2;
            }
            
            printf(") = String array [%d elements]\n", total_elements);
            
            printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
            printf("  Array length bytes: %02X %02X (Length: %d)\n", 
                   readbyte(vars_addr), readbyte(vars_addr + 1), var_len);
            printf("  Dimensions byte: %02X (Dimensions: %d)\n", readbyte(vars_addr + 2), dims);
            
            // Print dimensions bytes
            printf("  Dimension sizes:");
            for (i = 0; i < dims; i++) {
                printf(" %02X%02X", readbyte(vars_addr + 3 + i*2), readbyte(vars_addr + 4 + i*2));
            }
            printf("\n");
            
            // Show memory contents of full header
            printf("  Array header:\n");
            short_hexdump(vars_addr, header_size);
            
            // Check if the entire array fits within the variable area
            if (vars_addr + 2 + var_len > nxtlin_addr) {
                printf("  Warning: Array data extends beyond NXTLIN boundary\n");
                // Only skip to NXTLIN
                vars_addr = nxtlin_addr;
            } else {
                // Skip to next variable
                vars_addr += 2 + var_len; // Length (2) + Array data
            }
        }
        // FOR loop control variable: 0xE0 (1110xxxx)
        else if (type_bits == 0xE0) {
            // Check if we have enough space
            if (vars_addr + 18 > nxtlin_addr) {
                printf("[%04X] %c = Incomplete FOR loop control (reached NXTLIN)\n", var_start_addr, var_name);
                printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
                short_hexdump(vars_addr, nxtlin_addr - vars_addr);
                break;
            }
            
            printf("[%04X] %c (FOR loop control)\n", var_start_addr, var_name);
            printf("  Variable header: %02X (Type: %02X)\n", var_byte, type_bits);
            printf("  Current value: %lf\n", zx2d(mem + vars_addr));
            printf("  Limit: %lf\n", zx2d(mem + vars_addr + 5));
            printf("  Step: %lf\n", zx2d(mem + vars_addr + 10));
            printf("  Loop line: %02X%02X (%d)\n", 
                   readbyte(vars_addr + 15), readbyte(vars_addr + 16), 
                   readbyte(vars_addr + 15) | (readbyte(vars_addr + 16) << 8));
            printf("  Statement: %02X (%d)\n", readbyte(vars_addr + 17), readbyte(vars_addr + 17));
            
            // Show memory contents
            short_hexdump(vars_addr, 18);
            
            vars_addr += 18;  // Skip FOR loop control data
        }
        else {
            printf("[%04X] %c = Unknown variable type (0x%02X)\n", var_start_addr, var_name, var_byte);
            printf("  Variable header: %02X\n", var_byte);
            short_hexdump(var_start_addr, 1);
            break;  // Exit to avoid infinite loop
        }
        
        printf("\n"); // Extra line between variables for better readability
    }
    
    // Check if we reached NXTLIN without finding an end marker
    if (vars_addr >= nxtlin_addr && (vars_addr == nxtlin_addr || readbyte(vars_addr-1) != 0x80)) {
        printf("Reached NXTLIN at %04X without finding end marker (0x80)\n", nxtlin_addr);
    }
}

