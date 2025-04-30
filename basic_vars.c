#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "quirks.h"
#include "env.h"

/* ========================================================================
 * ZX Spectrum BASIC Variable Parser
 * 
 * This module parses ZX Spectrum BASIC variables from memory, displaying
 * their types, names, and values in a readable format.
 * 
 * Variable types (bits 5-7 of first byte):
 * - 0x40 (0100xxxx): String variables (A$)
 * - 0x60 (0110xxxx): Numeric variables (A)
 * - 0x80 (1000xxxx): Numeric arrays (A())
 * - 0xA0 (1010xxxx): Multi-letter numeric variables (ABC)
 * - 0xC0 (1100xxxx): String arrays (A$())
 * - 0xE0 (1110xxxx): FOR loop control variables
 *
 * End marker: 0x80 byte marks the end of variables area
 * ======================================================================== */

// External function to convert ZX Spectrum floating point to double
extern double zx2d(unsigned char *in);

/* ======================================================================== 
 * Type definitions and constants
 * ======================================================================== */

#define MAX_VAR_NAME_LEN 10
#define MAX_HEX_DUMP_LEN 16

// Variable type constants
#define VAR_TYPE_STRING      0x40
#define VAR_TYPE_NUMERIC     0x60
#define VAR_TYPE_NUM_ARRAY   0x80
#define VAR_TYPE_MULTI_LETTER 0xA0
#define VAR_TYPE_STR_ARRAY   0xC0
#define VAR_TYPE_FOR_LOOP    0xE0
#define VAR_END_MARKER       0x80

typedef struct {
    char name[MAX_VAR_NAME_LEN + 1];  // +1 for null terminator
    unsigned char type;                // Type bits (0x40, 0x60, etc.)
    int start_addr;                    // Address where variable starts in memory
    int value_addr;                    // Address of value data
    int total_size;                    // Total size in bytes including name and value
} variable_t;

/* ======================================================================== 
 * Helper functions for memory access and display 
 * ======================================================================== */

/**
 * Safely reads a byte from memory, checking bounds
 */
unsigned char safe_readbyte(int addr) {
    // Check memory bounds (0xFFFF is max ZX Spectrum memory)
    if (addr < 0 || addr > 0xFFFF) {
        return 0;
    }
    return readbyte(addr);
}

/**
 * Safely reads a word (2 bytes) from memory in little-endian format
 */
unsigned int safe_readword(int addr) {
    if (addr < 0 || addr > 0xFFFE) {
        return 0;
    }
    return readbyte(addr) | (readbyte(addr + 1) << 8);
}

/**
 * Print a short hex dump with ASCII representation
 */
void print_hex_dump(int addr, int len, const char *prefix) {
    int display_len = (len > MAX_HEX_DUMP_LEN) ? MAX_HEX_DUMP_LEN : len;
    
    printf("%s[%04X]: ", prefix, addr);
    
    // Print hex values
    for (int i = 0; i < display_len; i++) {
        printf("%02X ", safe_readbyte(addr + i));
    }
    
    // Pad with spaces for alignment if needed
    for (int i = display_len; i < MAX_HEX_DUMP_LEN; i++) {
        printf("   ");
    }
    
    // Print ASCII representation
    printf("| ");
    for (int i = 0; i < display_len; i++) {
        unsigned char c = safe_readbyte(addr + i);
        printf("%c", isprint(c) ? c : '.');
    }
    
    if (len > MAX_HEX_DUMP_LEN) {
        printf(" ... (%d more bytes)", len - MAX_HEX_DUMP_LEN);
    }
    
    printf("\n");
}

/**
 * Print a full hexdump of a memory region
 */
void print_memory_region(int start_addr, int end_addr, const char *title) {
    int num_bytes = end_addr - start_addr;
    
    // Limit to max 320 bytes (20 lines)
    if (num_bytes > 320) {
        num_bytes = 320;
    }
    
    printf("\n%s (%04X to %04X):\n", title, start_addr, end_addr);
    
    // Print hexdump in standard format
    for (int i = 0; i < num_bytes; i += 16) {
        printf(" %04X", start_addr + i);
        
        // Print hex values
        for (int j = 0; j < 16 && (i + j) < num_bytes; j++) {
            printf(" %02X", safe_readbyte(start_addr + i + j));
        }
        
        // Pad with spaces if line is incomplete
        for (int j = ((i + 16 > num_bytes) ? (num_bytes - i) : 16); j < 16; j++) {
            printf("   ");
        }
        
        printf(" ");
        
        // Print ASCII characters
        for (int j = 0; j < 16 && (i + j) < num_bytes; j++) {
            unsigned char c = safe_readbyte(start_addr + i + j);
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        
        printf("\n");
    }
    
    // Indicate if data was truncated
    if (end_addr - start_addr > 320) {
        printf(" ... (additional data truncated)\n");
    }
}

/**
 * Decode a variable header byte into letter and type
 */
void decode_var_header(unsigned char header_byte, char *letter, unsigned char *type) {
    *type = header_byte & 0xE0;  // Upper 3 bits for type
    *letter = ((header_byte & 0x1F) + 'A' - 1);  // Lower 5 bits for letter code
}

/**
 * Decode a variable name from memory
 * Returns the number of bytes used for the name
 */
int decode_variable_name(int addr, char *name_buffer, unsigned char *var_type) {
    unsigned char header_byte = safe_readbyte(addr);
    char first_letter;
    
    // Decode header byte
    decode_var_header(header_byte, &first_letter, var_type);
    
    // Put first letter in buffer
    name_buffer[0] = first_letter;
    name_buffer[1] = '\0';
    
    // Only numeric variables can have multi-letter names
    if (*var_type == VAR_TYPE_NUMERIC || *var_type == VAR_TYPE_MULTI_LETTER) {
        int name_len = 1;
        int pos = 1;
        
        // Look for additional letters (max 9 more)
        while (name_len < MAX_VAR_NAME_LEN) {
            unsigned char next_byte = safe_readbyte(addr + pos);
            
            // End of name detection conditions
            if (next_byte == 0x00 || next_byte == VAR_END_MARKER || 
                addr + pos >= 0xFFFF) {
                break;
            }
            
            // Check if this is the last letter (has high bit set)
            if (next_byte & 0x80) {
                name_buffer[name_len++] = toupper(next_byte & 0x7F);
                pos++;
                break;  // Last letter found
            }
            
            // Regular letter (without high bit)
            if ((next_byte >= 'a' && next_byte <= 'z') || 
                (next_byte >= 'A' && next_byte <= 'Z')) {
                name_buffer[name_len++] = toupper(next_byte);
                pos++;
            } else {
                // Not a letter, end of name
                break;
            }
        }
        
        // Null-terminate the name
        name_buffer[name_len] = '\0';
        
        return pos;
    } else {
        // All other variable types have single-letter names
        return 1;
    }
}

/**
 * Format a numeric value from memory
 */
void format_numeric_value(int addr, char *value_str, size_t size) {
    if (safe_readbyte(addr) == 0) {
        // Small integer format
        if (safe_readbyte(addr + 1) == 0xFF) {
            // Negative number
            unsigned int value = safe_readbyte(addr + 2) | (safe_readbyte(addr + 3) << 8);
            int signed_value = -((~value + 1) & 0xFFFF);
            snprintf(value_str, size, "%d", signed_value);
        } else {
            // Positive number
            int value = safe_readbyte(addr + 2) | (safe_readbyte(addr + 3) << 8);
            snprintf(value_str, size, "%d", value);
        }
    } else {
        // Floating point format
        snprintf(value_str, size, "%lf", zx2d((unsigned char *)(mem + addr)));
    }
}

/**
 * Check if a variable is complete (fits within memory bounds)
 */
bool is_var_complete(int var_addr, int var_size, int end_addr) {
    return (var_addr + var_size <= end_addr);
}

/* ======================================================================== 
 * Variable type-specific processing functions
 * ======================================================================== */

/**
 * Process a numeric variable
 * Returns the total size of the variable in bytes
 */
int process_numeric_variable(variable_t *var, int nxtlin_addr) {
    if (!is_var_complete(var->value_addr, 5, nxtlin_addr)) {
        printf("[%04X] %s = Incomplete numeric variable (reached NXTLIN)\n", 
               var->start_addr, var->name);
        return 0;
    }
    
    // Format and display the value
    char value_str[32];
    format_numeric_value(var->value_addr, value_str, sizeof(value_str));
    printf("[%04X] %s = %s\n", var->start_addr, var->name, value_str);
    
    // Show additional details
    printf("  Variable header: %02X (Type: %02X)\n", safe_readbyte(var->start_addr), var->type);
    
    // Display multi-letter name bytes if applicable
    int name_bytes = var->value_addr - var->start_addr;
    if (name_bytes > 1) {
        printf("  Multi-letter name bytes: ");
        for (int i = 0; i < name_bytes; i++) {
            printf("%02X ", safe_readbyte(var->start_addr + i));
        }
        printf("\n");
    }
    
    printf("  Numeric value (5 bytes):\n");
    print_hex_dump(var->value_addr, 5, "  Memory ");
    
    return name_bytes + 5;  // Name bytes + 5-byte numeric value
}

/**
 * Process a string variable
 * Returns the total size of the variable in bytes
 */
int process_string_variable(variable_t *var, int nxtlin_addr) {
    // Check if we can read the string length (2 bytes)
    if (!is_var_complete(var->value_addr, 2, nxtlin_addr)) {
        printf("[%04X] %s$ = Incomplete string variable (reached NXTLIN)\n", 
               var->start_addr, var->name);
        return 0;
    }
    
    // Read string length (little-endian)
    int str_len = safe_readbyte(var->value_addr) | (safe_readbyte(var->value_addr + 1) << 8);
    
    // Check if the complete string fits within memory
    if (!is_var_complete(var->value_addr + 2, str_len, nxtlin_addr)) {
        printf("[%04X] %s$ = Incomplete string (reached NXTLIN)\n", 
               var->start_addr, var->name);
        return 0;
    }
    
    // Display the string (limit to 20 chars for display)
    printf("[%04X] %s$ = \"", var->start_addr, var->name);
    
    for (int i = 0; i < str_len && i < 20; i++) {
        unsigned char c = safe_readbyte(var->value_addr + 2 + i);
        printf("%c", isprint(c) ? c : '.');
    }
    
    if (str_len > 20) {
        printf("...\" (%d chars)\n", str_len);
    } else {
        printf("\"\n");
    }
    
    // Show additional details
    printf("  Variable header: %02X (Type: %02X)\n", safe_readbyte(var->start_addr), var->type);
    
    // Display multi-letter name bytes if applicable
    int name_bytes = var->value_addr - var->start_addr;
    if (name_bytes > 1) {
        printf("  Multi-letter name bytes: ");
        for (int i = 0; i < name_bytes; i++) {
            printf("%02X ", safe_readbyte(var->start_addr + i));
        }
        printf("\n");
    }
    
    printf("  String length bytes: %02X %02X (Length: %d)\n", 
           safe_readbyte(var->value_addr), safe_readbyte(var->value_addr + 1), str_len);
    printf("  String content:\n");
    print_hex_dump(var->value_addr + 2, str_len, "  Memory ");
    
    return name_bytes + 2 + str_len;  // Name bytes + length (2) + string content
}

/**
 * Process an array (numeric or string)
 * Returns the total size of the variable in bytes
 */
int process_array_variable(variable_t *var, int nxtlin_addr, bool is_string) {
    // Check if we can read the array length (2 bytes)
    if (!is_var_complete(var->value_addr, 2, nxtlin_addr)) {
        printf("[%04X] %s%s() = Incomplete array (reached NXTLIN)\n", 
               var->start_addr, var->name, is_string ? "$" : "");
        return 0;
    }
    
    // Read array length (little-endian)
    int array_len = safe_readbyte(var->value_addr) | (safe_readbyte(var->value_addr + 1) << 8);
    
    // Check if we can read the dimensions byte
    if (!is_var_complete(var->value_addr + 2, 1, nxtlin_addr)) {
        printf("[%04X] %s%s() = Incomplete array dimensions (reached NXTLIN)\n", 
               var->start_addr, var->name, is_string ? "$" : "");
        return 0;
    }
    
    // Read dimensions count
    int dims = safe_readbyte(var->value_addr + 2);
    
    // Check if we can read all dimension sizes
    if (!is_var_complete(var->value_addr + 3, dims * 2, nxtlin_addr)) {
        printf("[%04X] %s%s() = Incomplete array dimensions (reached NXTLIN)\n", 
               var->start_addr, var->name, is_string ? "$" : "");
        return 0;
    }
    
    // Calculate total array elements and format array dimensions
    printf("[%04X] %s%s(", var->start_addr, var->name, is_string ? "$" : "");
    
    int total_elements = 1;
    for (int i = 0; i < dims; i++) {
        // Get dimension size (little-endian)
        int dim_size = safe_readbyte(var->value_addr + 3 + i*2) | 
                       (safe_readbyte(var->value_addr + 4 + i*2) << 8);
        
        total_elements *= dim_size;
        
        printf("%d", dim_size);
        if (i < dims - 1) printf(",");
    }
    
    printf(") = %s array [%d elements]\n", 
           is_string ? "String" : "Numeric", total_elements);
    
    // Show additional details
    printf("  Variable header: %02X (Type: %02X)\n", safe_readbyte(var->start_addr), var->type);
    
    // Display multi-letter name bytes if applicable
    int name_bytes = var->value_addr - var->start_addr;
    if (name_bytes > 1) {
        printf("  Multi-letter name bytes: ");
        for (int i = 0; i < name_bytes; i++) {
            printf("%02X ", safe_readbyte(var->start_addr + i));
        }
        printf("\n");
    }
    
    printf("  Array length bytes: %02X %02X (Length: %d)\n", 
           safe_readbyte(var->value_addr), safe_readbyte(var->value_addr + 1), array_len);
    printf("  Dimensions byte: %02X (Dimensions: %d)\n", safe_readbyte(var->value_addr + 2), dims);
    
    // Print dimensions bytes
    printf("  Dimension sizes:");
    for (int i = 0; i < dims; i++) {
        printf(" %02X%02X", 
               safe_readbyte(var->value_addr + 3 + i*2), 
               safe_readbyte(var->value_addr + 4 + i*2));
    }
    printf("\n");
    
    // Calculate header size (length + dims byte + dimension sizes)
    int header_size = 3 + (dims * 2);
    
    // Show memory contents of header
    printf("  Array header:\n");
    print_hex_dump(var->value_addr, header_size, "  Memory ");
    
    // Check if the entire array fits within the variable area
    if (!is_var_complete(var->value_addr, 2 + array_len, nxtlin_addr)) {
        printf("  Warning: Array data extends beyond NXTLIN boundary\n");
        return 0;  // Can't determine total size safely
    }
    
    return name_bytes + 2 + array_len;  // Name bytes + length (2) + array data
}

/**
 * Process a FOR loop control variable
 * Returns the total size of the variable in bytes
 */
int process_for_variable(variable_t *var, int nxtlin_addr) {
    // FOR loop variables are always 18 bytes of data
    if (!is_var_complete(var->value_addr, 18, nxtlin_addr)) {
        printf("[%04X] %s = Incomplete FOR loop control (reached NXTLIN)\n", 
               var->start_addr, var->name);
        return 0;
    }
    
    printf("[%04X] %s (FOR loop control)\n", var->start_addr, var->name);
    
    // Show details
    printf("  Variable header: %02X (Type: %02X)\n", safe_readbyte(var->start_addr), var->type);
    
    // Display multi-letter name bytes if applicable
    int name_bytes = var->value_addr - var->start_addr;
    if (name_bytes > 1) {
        printf("  Multi-letter name bytes: ");
        for (int i = 0; i < name_bytes; i++) {
            printf("%02X ", safe_readbyte(var->start_addr + i));
        }
        printf("\n");
    }
    
    printf("  Current value: %lf\n", zx2d((unsigned char *)(mem + var->value_addr)));
    printf("  Limit: %lf\n", zx2d((unsigned char *)(mem + var->value_addr + 5)));
    printf("  Step: %lf\n", zx2d((unsigned char *)(mem + var->value_addr + 10)));
    
    // Get loop line (special value 65534 indicates direct command input)
    int loop_line = safe_readbyte(var->value_addr + 15) | 
                    (safe_readbyte(var->value_addr + 16) << 8);
    
    printf("  Loop line: %02X%02X (%d)%s\n", 
           safe_readbyte(var->value_addr + 15), safe_readbyte(var->value_addr + 16), 
           loop_line, (loop_line == 65534) ? " (direct command)" : "");
           
    printf("  Statement: %02X (%d)\n", safe_readbyte(var->value_addr + 17), 
           safe_readbyte(var->value_addr + 17));
    
    // Show memory contents
    printf("  FOR loop data:\n");
    print_hex_dump(var->value_addr, 18, "  Memory ");
    
    return name_bytes + 18;  // Name bytes + 18 bytes of FOR data
}

/* ======================================================================== 
 * Main variable listing function
 * ======================================================================== */

/**
 * Parse and display ZX Spectrum BASIC variables from memory
 */
void list_basic_vars(void) {
    // Get system variables addresses
    int vars_addr = readword(0x5C4B);    // VARS system variable
    int nxtlin_addr = readword(0x5C55);  // NXTLIN system variable
    
    printf("\nBASIC Variables (VARS=%04X to NXTLIN=%04X):\n", vars_addr, nxtlin_addr);
    printf("----------------\n");
    
    // Find 0x80 end marker if it exists before NXTLIN
    int end_marker_addr = -1;
    for (int i = vars_addr; i < nxtlin_addr; i++) {
        if (safe_readbyte(i) == VAR_END_MARKER) {
            end_marker_addr = i;
            break;
        }
    }
    
    // Determine actual end of variables area
    int vars_end = (end_marker_addr != -1) ? end_marker_addr + 1 : nxtlin_addr;
    
    // Show hexdump of the complete variables area
    print_memory_region(vars_addr, vars_end, "Full hexdump of VARS area");
    printf("\n");
    
    // Loop through variables area
    int current_addr = vars_addr;
    
    while (current_addr < nxtlin_addr) {
        // Check for end marker
        if (safe_readbyte(current_addr) == VAR_END_MARKER) {
            printf("End marker at %04X: 80\n", current_addr);
            print_hex_dump(current_addr, 1, "  Memory ");
            break;
        }
        
        // Special case: If we see a 0x00, it's probably an unused area
        if (safe_readbyte(current_addr) == 0x00) {
            printf("Unused byte at %04X: 00\n", current_addr);
            print_hex_dump(current_addr, 1, "  Memory ");
            current_addr++;
            continue;
        }
        
        // Initialize variable structure
        variable_t var;
        var.start_addr = current_addr;
        
        // Decode variable name and type
        unsigned char var_type;
        int name_bytes = decode_variable_name(current_addr, var.name, &var_type);
        var.type = var_type;
        var.value_addr = current_addr + name_bytes;
        
        // Process variable based on type
        int var_size = 0;
        
        if (var_type == VAR_TYPE_NUMERIC || var_type == VAR_TYPE_MULTI_LETTER) {
            var_size = process_numeric_variable(&var, nxtlin_addr);
        }
        else if (var_type == VAR_TYPE_STRING) {
            var_size = process_string_variable(&var, nxtlin_addr);
        }
        else if (var_type == VAR_TYPE_NUM_ARRAY) {
            var_size = process_array_variable(&var, nxtlin_addr, false);
        }
        else if (var_type == VAR_TYPE_STR_ARRAY) {
            var_size = process_array_variable(&var, nxtlin_addr, true);
        }
        else if (var_type == VAR_TYPE_FOR_LOOP) {
            var_size = process_for_variable(&var, nxtlin_addr);
        }
        else {
            // Unknown variable type
            printf("[%04X] %s = Unknown variable type (0x%02X)\n", 
                   var.start_addr, var.name, var_type);
            printf("  Variable header: %02X\n", safe_readbyte(var.start_addr));
            print_hex_dump(var.start_addr, 1, "  Memory ");
            
            // Skip this byte to avoid infinite loop
            var_size = 1;
        }
        
        // Check if variable processing was successful
        if (var_size > 0) {
            // Advance to next variable
            current_addr += var_size;
        } else {
            // Variable processing failed (likely reached NXTLIN)
            // Skip to the end to prevent infinite loop
            current_addr = nxtlin_addr;
        }
        
        printf("\n"); // Extra line between variables for better readability
    }
    
    // Check if we reached NXTLIN without finding an end marker
    if (current_addr >= nxtlin_addr && 
        (current_addr == nxtlin_addr || safe_readbyte(current_addr-1) != VAR_END_MARKER)) {
        printf("Reached NXTLIN at %04X without finding end marker (0x80)\n", nxtlin_addr);
    }
}

