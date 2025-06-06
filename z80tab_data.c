/*
 * Z80 instruction table data
 * Derived from z80tab.txt
 * (C) 1991-2025 Rui Fernando F. Ribeiro
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialize the Z80 instruction tables with embedded data
 * Used instead of reading from z80tab.txt
 */
void init_z80tab_data(char ***maintable, char ***CBtable, char ***EDtable) {
    // Allocate memory for the tables
    if (!(*maintable = (char **)calloc(256, sizeof(char *))) ||
        !(*CBtable = (char **)calloc(256, sizeof(char *))) ||
        !(*EDtable = (char **)calloc(256, sizeof(char *)))) {
        fprintf(stderr, "Error: Insufficient memory for instruction tables\n");
        exit(1);
    }

    /* Main Z80 instructions */
    (*maintable)[0x00] = strdup("NOP");
    (*maintable)[0x01] = strdup("LD\tBC,w");
    (*maintable)[0x02] = strdup("LD\t(BC),A");
    (*maintable)[0x03] = strdup("INC\tBC");
    (*maintable)[0x04] = strdup("INC\tB");
    (*maintable)[0x05] = strdup("DEC\tB");
    (*maintable)[0x06] = strdup("LD\tB,b");
    (*maintable)[0x07] = strdup("RLCA");
    (*maintable)[0x08] = strdup("EX\tAF,AF'");
    (*maintable)[0x09] = strdup("ADD\thl,BC");
    (*maintable)[0x0A] = strdup("LD\tA,(BC)");
    (*maintable)[0x0B] = strdup("DEC\tBC");
    (*maintable)[0x0C] = strdup("INC\tC");
    (*maintable)[0x0D] = strdup("DEC\tC");
    (*maintable)[0x0E] = strdup("LD\tC,b");
    (*maintable)[0x0F] = strdup("RRCA");
    (*maintable)[0x10] = strdup("DJNZ\td");
    (*maintable)[0x11] = strdup("LD\tDE,w");
    (*maintable)[0x12] = strdup("LD\t(DE),A");
    (*maintable)[0x13] = strdup("INC\tDE");
    (*maintable)[0x14] = strdup("INC\tD");
    (*maintable)[0x15] = strdup("DEC\tD");
    (*maintable)[0x16] = strdup("LD\tD,b");
    (*maintable)[0x17] = strdup("RLA");
    (*maintable)[0x18] = strdup("JR\td");
    (*maintable)[0x19] = strdup("ADD\thl,DE");
    (*maintable)[0x1A] = strdup("LD\tA,(DE)");
    (*maintable)[0x1B] = strdup("DEC\tDE");
    (*maintable)[0x1C] = strdup("INC\tE");
    (*maintable)[0x1D] = strdup("DEC\tE");
    (*maintable)[0x1E] = strdup("LD\tE,b");
    (*maintable)[0x1F] = strdup("RRA");
    (*maintable)[0x20] = strdup("JR\tNZ,d");
    (*maintable)[0x21] = strdup("LD\thl,w");
    (*maintable)[0x22] = strdup("LD\t(w),hl");
    (*maintable)[0x23] = strdup("INC\thl");
    (*maintable)[0x24] = strdup("INC\th");
    (*maintable)[0x25] = strdup("DEC\th");
    (*maintable)[0x26] = strdup("LD\th,b");
    (*maintable)[0x27] = strdup("DAA");
    (*maintable)[0x28] = strdup("JR\tZ,d");
    (*maintable)[0x29] = strdup("ADD\thl,hl");
    (*maintable)[0x2A] = strdup("LD\thl,(w)");
    (*maintable)[0x2B] = strdup("DEC\thl");
    (*maintable)[0x2C] = strdup("INC\tl");
    (*maintable)[0x2D] = strdup("DEC\tl");
    (*maintable)[0x2E] = strdup("LD\tl,b");
    (*maintable)[0x2F] = strdup("CPL");
    (*maintable)[0x30] = strdup("JR\tNC,d");
    (*maintable)[0x31] = strdup("LD\tSP,w");
    (*maintable)[0x32] = strdup("LD\t(w),A");
    (*maintable)[0x33] = strdup("INC\tSP");
    (*maintable)[0x34] = strdup("INC\t(x)");
    (*maintable)[0x35] = strdup("DEC\t(x)");
    (*maintable)[0x36] = strdup("LD\t(x),b");
    (*maintable)[0x37] = strdup("SCF");
    (*maintable)[0x38] = strdup("JR \tC,d");
    (*maintable)[0x39] = strdup("ADD\thl,SP");
    (*maintable)[0x3A] = strdup("LD\tA,(w)");
    (*maintable)[0x3B] = strdup("DEC \tSP");
    (*maintable)[0x3C] = strdup("INC\tA");
    (*maintable)[0x3D] = strdup("DEC\tA");
    (*maintable)[0x3E] = strdup("LD\tA,b");
    (*maintable)[0x3F] = strdup("CCF");
    (*maintable)[0x40] = strdup("LD\tB,B");
    (*maintable)[0x41] = strdup("LD\tB,C");
    (*maintable)[0x42] = strdup("LD\tB,D");
    (*maintable)[0x43] = strdup("LD\tB,E");
    (*maintable)[0x44] = strdup("LD\tB,h");
    (*maintable)[0x45] = strdup("LD\tB,l");
    (*maintable)[0x46] = strdup("LD\tB,(x)");
    (*maintable)[0x47] = strdup("LD\tB,A");
    (*maintable)[0x48] = strdup("LD\tC,B");
    (*maintable)[0x49] = strdup("LD\tC,C");
    (*maintable)[0x4A] = strdup("LD\tC,D");
    (*maintable)[0x4B] = strdup("LD\tC,E");
    (*maintable)[0x4C] = strdup("LD\tC,h");
    (*maintable)[0x4D] = strdup("LD\tC,l");
    (*maintable)[0x4E] = strdup("LD\tC,(x)");
    (*maintable)[0x4F] = strdup("LD\tC,A");
    (*maintable)[0x50] = strdup("LD\tD,B");
    (*maintable)[0x51] = strdup("LD\tD,C");
    (*maintable)[0x52] = strdup("LD\tD,D");
    (*maintable)[0x53] = strdup("LD\tD,E");
    (*maintable)[0x54] = strdup("LD\tD,h");
    (*maintable)[0x55] = strdup("LD\tD,l");
    (*maintable)[0x56] = strdup("LD\tD,(x)");
    (*maintable)[0x57] = strdup("LD\tD,A");
    (*maintable)[0x58] = strdup("LD\tE,B");
    (*maintable)[0x59] = strdup("LD\tE,C");
    (*maintable)[0x5A] = strdup("LD\tE,D");
    (*maintable)[0x5B] = strdup("LD\tE,E");
    (*maintable)[0x5C] = strdup("LD\tE,h");
    (*maintable)[0x5D] = strdup("LD\tE,l");
    (*maintable)[0x5E] = strdup("LD\tE,(x)");
    (*maintable)[0x5F] = strdup("LD\tE,A");
    (*maintable)[0x60] = strdup("LD\th,B");
    (*maintable)[0x61] = strdup("LD\th,C");
    (*maintable)[0x62] = strdup("LD\th,D");
    (*maintable)[0x63] = strdup("LD\th,E");
    (*maintable)[0x64] = strdup("LD\th,h");
    (*maintable)[0x65] = strdup("LD\th,l");
    (*maintable)[0x66] = strdup("LD\tH,(x)");
    (*maintable)[0x67] = strdup("LD\th,A");
    (*maintable)[0x68] = strdup("LD\tl,B");
    (*maintable)[0x69] = strdup("LD\tl,C");
    (*maintable)[0x6A] = strdup("LD\tl,D");
    (*maintable)[0x6B] = strdup("LD\tl,E");
    (*maintable)[0x6C] = strdup("LD\tl,h");
    (*maintable)[0x6D] = strdup("LD\tl,l");
    (*maintable)[0x6E] = strdup("LD\tL,(x)");
    (*maintable)[0x6F] = strdup("LD\tl,A");
    (*maintable)[0x70] = strdup("LD\t(x),B");
    (*maintable)[0x71] = strdup("LD\t(x),C");
    (*maintable)[0x72] = strdup("LD\t(x),D");
    (*maintable)[0x73] = strdup("LD\t(x),E");
    (*maintable)[0x74] = strdup("LD\t(x),H");
    (*maintable)[0x75] = strdup("LD\t(x),L");
    (*maintable)[0x76] = strdup("HALT");
    (*maintable)[0x77] = strdup("LD\t(x),A");
    (*maintable)[0x78] = strdup("LD\tA,B");
    (*maintable)[0x79] = strdup("LD\tA,C");
    (*maintable)[0x7A] = strdup("LD\tA,D");
    (*maintable)[0x7B] = strdup("LD\tA,E");
    (*maintable)[0x7C] = strdup("LD\tA,h");
    (*maintable)[0x7D] = strdup("LD\tA,l");
    (*maintable)[0x7E] = strdup("LD\tA,(x)");
    (*maintable)[0x7F] = strdup("LD\tA,A");
    (*maintable)[0x80] = strdup("ADD\tA,B");
    (*maintable)[0x81] = strdup("ADD\tA,C");
    (*maintable)[0x82] = strdup("ADD\tA,D");
    (*maintable)[0x83] = strdup("ADD\tA,E");
    (*maintable)[0x84] = strdup("ADD\tA,h");
    (*maintable)[0x85] = strdup("ADD\tA,l");
    (*maintable)[0x86] = strdup("ADD\tA,(x)");
    (*maintable)[0x87] = strdup("ADD\tA,A");
    (*maintable)[0x88] = strdup("ADC\tA,B");
    (*maintable)[0x89] = strdup("ADC\tA,C");
    (*maintable)[0x8A] = strdup("ADC\tA,D");
    (*maintable)[0x8B] = strdup("ADC\tA,E");
    (*maintable)[0x8C] = strdup("ADC\tA,h");
    (*maintable)[0x8D] = strdup("ADC\tA,l");
    (*maintable)[0x8E] = strdup("ADC\tA,(x)");
    (*maintable)[0x8F] = strdup("ADC\tA,A");
    (*maintable)[0x90] = strdup("SUB\tB");
    (*maintable)[0x91] = strdup("SUB\tC");
    (*maintable)[0x92] = strdup("SUB\tD");
    (*maintable)[0x93] = strdup("SUB\tE");
    (*maintable)[0x94] = strdup("SUB\th");
    (*maintable)[0x95] = strdup("SUB\tl");
    (*maintable)[0x96] = strdup("SUB\t(x)");
    (*maintable)[0x97] = strdup("SUB\tA");
    (*maintable)[0x98] = strdup("SBC\tA,B");
    (*maintable)[0x99] = strdup("SBC\tA,C");
    (*maintable)[0x9A] = strdup("SBC\tA,D");
    (*maintable)[0x9B] = strdup("SBC\tA,E");
    (*maintable)[0x9C] = strdup("SBC\tA,h");
    (*maintable)[0x9D] = strdup("SBC\tA,l");
    (*maintable)[0x9E] = strdup("SBC\tA,(x)");
    (*maintable)[0x9F] = strdup("SBC\tA,A");
    (*maintable)[0xA0] = strdup("AND\tB");
    (*maintable)[0xA1] = strdup("AND\tC");
    (*maintable)[0xA2] = strdup("AND\tD");
    (*maintable)[0xA3] = strdup("AND\tE");
    (*maintable)[0xA4] = strdup("AND\th");
    (*maintable)[0xA5] = strdup("AND\tl");
    (*maintable)[0xA6] = strdup("AND\t(x)");
    (*maintable)[0xA7] = strdup("AND\tA");
    (*maintable)[0xA8] = strdup("XOR\tB");
    (*maintable)[0xA9] = strdup("XOR\tC");
    (*maintable)[0xAA] = strdup("XOR\tD");
    (*maintable)[0xAB] = strdup("XOR\tE");
    (*maintable)[0xAC] = strdup("XOR\th");
    (*maintable)[0xAD] = strdup("XOR\tl");
    (*maintable)[0xAE] = strdup("XOR\t(x)");
    (*maintable)[0xAF] = strdup("XOR\tA");
    (*maintable)[0xB0] = strdup("OR\tB");
    (*maintable)[0xB1] = strdup("OR\tC");
    (*maintable)[0xB2] = strdup("OR\tD");
    (*maintable)[0xB3] = strdup("OR\tE");
    (*maintable)[0xB4] = strdup("OR\th");
    (*maintable)[0xB5] = strdup("OR\tl");
    (*maintable)[0xB6] = strdup("OR\t(x)");
    (*maintable)[0xB7] = strdup("OR\tA");
    (*maintable)[0xB8] = strdup("CP\tB");
    (*maintable)[0xB9] = strdup("CP\tC");
    (*maintable)[0xBA] = strdup("CP\tD");
    (*maintable)[0xBB] = strdup("CP\tE");
    (*maintable)[0xBC] = strdup("CP\th");
    (*maintable)[0xBD] = strdup("CP\tl");
    (*maintable)[0xBE] = strdup("CP\t(x)");
    (*maintable)[0xBF] = strdup("CP\tA");
    (*maintable)[0xC0] = strdup("RET\tNZ");
    (*maintable)[0xC1] = strdup("POP\tBC");
    (*maintable)[0xC2] = strdup("JP\tNZ,w");
    (*maintable)[0xC3] = strdup("JP\tw");
    (*maintable)[0xC4] = strdup("CALL\tNZ,w");
    (*maintable)[0xC5] = strdup("PUSH\tBC");
    (*maintable)[0xC6] = strdup("ADD\tA,b");
    (*maintable)[0xC7] = strdup("RST\t$00");
    (*maintable)[0xC8] = strdup("RET\tZ");
    (*maintable)[0xC9] = strdup("RET");
    (*maintable)[0xCA] = strdup("JP\tZ,w");
    (*maintable)[0xCC] = strdup("CALL\tZ,w");
    (*maintable)[0xCD] = strdup("CALL\tw");
    (*maintable)[0xCE] = strdup("ADC\tA,b");
    (*maintable)[0xCF] = strdup("RST\t$08");
    (*maintable)[0xD0] = strdup("RET\tNC");
    (*maintable)[0xD1] = strdup("POP\tDE");
    (*maintable)[0xD2] = strdup("JP\tNC,w");
    (*maintable)[0xD3] = strdup("OUT\t(b),A");
    (*maintable)[0xD4] = strdup("CALL\tNC,w");
    (*maintable)[0xD5] = strdup("PUSH\tDE");
    (*maintable)[0xD6] = strdup("SUB\tb");
    (*maintable)[0xD7] = strdup("RST\t$10");
    (*maintable)[0xD8] = strdup("RET\tC");
    (*maintable)[0xD9] = strdup("EXX");
    (*maintable)[0xDA] = strdup("JP\tC,w");
    (*maintable)[0xDB] = strdup("IN A,(b)");
    (*maintable)[0xDC] = strdup("CALL\tC,w");
    (*maintable)[0xDE] = strdup("SBC\tA,b");
    (*maintable)[0xDF] = strdup("RST\t$18");
    (*maintable)[0xE0] = strdup("RET\tPO");
    (*maintable)[0xE1] = strdup("POP\thl");
    (*maintable)[0xE2] = strdup("JP\tPO,w");
    (*maintable)[0xE3] = strdup("EX\t(SP),hl");
    (*maintable)[0xE4] = strdup("CALL\tPO,w");
    (*maintable)[0xE5] = strdup("PUSH\thl");
    (*maintable)[0xE6] = strdup("AND\tb");
    (*maintable)[0xE7] = strdup("RST\t$20");
    (*maintable)[0xE8] = strdup("RET\tPE");
    (*maintable)[0xE9] = strdup("JP\t(hl)");
    (*maintable)[0xEA] = strdup("JP\tPE,w");
    (*maintable)[0xEB] = strdup("EX\tDE,hl");
    (*maintable)[0xEC] = strdup("CALL\tPE,w");
    (*maintable)[0xEE] = strdup("XOR\tb");
    (*maintable)[0xEF] = strdup("RST\t$28");
    (*maintable)[0xF0] = strdup("RET\tP");
    (*maintable)[0xF1] = strdup("POP\tAF");
    (*maintable)[0xF2] = strdup("JP\tP,w");
    (*maintable)[0xF3] = strdup("DI");
    (*maintable)[0xF4] = strdup("CALL\tP,w");
    (*maintable)[0xF5] = strdup("PUSH\tAF");
    (*maintable)[0xF6] = strdup("OR\tb");
    (*maintable)[0xF7] = strdup("RST\t$30");
    (*maintable)[0xF8] = strdup("RET\tM");
    (*maintable)[0xF9] = strdup("LD\tSP,hl");
    (*maintable)[0xFA] = strdup("JP\tM,w");
    (*maintable)[0xFB] = strdup("EI");
    (*maintable)[0xFC] = strdup("CALL\tM,w");
    (*maintable)[0xFE] = strdup("CP\tb");
    (*maintable)[0xFF] = strdup("RST\t$38");

    /* CB-prefixed instructions */
    (*CBtable)[0x00] = strdup("RLC\tB");
    (*CBtable)[0x01] = strdup("RLC\tC");
    (*CBtable)[0x02] = strdup("RLC\tD");
    (*CBtable)[0x03] = strdup("RLC\tE");
    (*CBtable)[0x04] = strdup("RLC\th");
    (*CBtable)[0x05] = strdup("RLC\tl");
    (*CBtable)[0x06] = strdup("RLC\t(x)");
    (*CBtable)[0x07] = strdup("RLC\tA");
    (*CBtable)[0x08] = strdup("RRC\tB");
    (*CBtable)[0x09] = strdup("RRC\tC");
    (*CBtable)[0x0A] = strdup("RRC\tD");
    (*CBtable)[0x0B] = strdup("RRC\tE");
    (*CBtable)[0x0C] = strdup("RRC\th");
    (*CBtable)[0x0D] = strdup("RRC\tl");
    (*CBtable)[0x0E] = strdup("RRC\t(x)");
    (*CBtable)[0x0F] = strdup("RRC\tA");
    (*CBtable)[0x10] = strdup("RL\tB");
    (*CBtable)[0x11] = strdup("RL\tC");
    (*CBtable)[0x12] = strdup("RL\tD");
    (*CBtable)[0x13] = strdup("RL\tE");
    (*CBtable)[0x14] = strdup("RL\th");
    (*CBtable)[0x15] = strdup("RL\tl");
    (*CBtable)[0x16] = strdup("RL\t(x)");
    (*CBtable)[0x17] = strdup("RL\tA");
    (*CBtable)[0x18] = strdup("RR\tB");
    (*CBtable)[0x19] = strdup("RR\tC");
    (*CBtable)[0x1A] = strdup("RR\tD");
    (*CBtable)[0x1B] = strdup("RR\tE");
    (*CBtable)[0x1C] = strdup("RR\th");
    (*CBtable)[0x1D] = strdup("RR\tl");
    (*CBtable)[0x1E] = strdup("RR\t(x)");
    (*CBtable)[0x1F] = strdup("RR\tA");
    (*CBtable)[0x20] = strdup("SLA\tB");
    (*CBtable)[0x21] = strdup("SLA\tC");
    (*CBtable)[0x22] = strdup("SLA\tD");
    (*CBtable)[0x23] = strdup("SLA\tE");
    (*CBtable)[0x24] = strdup("SLA\th");
    (*CBtable)[0x25] = strdup("SLA\tl");
    (*CBtable)[0x26] = strdup("SLA\t(x)");
    (*CBtable)[0x27] = strdup("SLA\tA");
    (*CBtable)[0x28] = strdup("SRA\tB");
    (*CBtable)[0x29] = strdup("SRA\tC");
    (*CBtable)[0x2A] = strdup("SRA\tD");
    (*CBtable)[0x2B] = strdup("SRA\tE");
    (*CBtable)[0x2C] = strdup("SRA\th");
    (*CBtable)[0x2D] = strdup("SRA\tl");
    (*CBtable)[0x2E] = strdup("SRA\t(x)");
    (*CBtable)[0x2F] = strdup("SRA\tA");
    (*CBtable)[0x30] = strdup("SLL\tB");
    (*CBtable)[0x31] = strdup("SLL\tC");
    (*CBtable)[0x32] = strdup("SLL\tD");
    (*CBtable)[0x33] = strdup("SLL\tE");
    (*CBtable)[0x34] = strdup("SLL\th");
    (*CBtable)[0x35] = strdup("SLL\tl");
    (*CBtable)[0x36] = strdup("SLL\t(x)");
    (*CBtable)[0x37] = strdup("SLL\tA");
    (*CBtable)[0x38] = strdup("SRL\tB");
    (*CBtable)[0x39] = strdup("SRL\tC");
    (*CBtable)[0x3A] = strdup("SRL\tD");
    (*CBtable)[0x3B] = strdup("SRL\tE");
    (*CBtable)[0x3C] = strdup("SRL\th");
    (*CBtable)[0x3D] = strdup("SRL\tl");
    (*CBtable)[0x3E] = strdup("SRL\t(x)");
    (*CBtable)[0x3F] = strdup("SRL\tA");
    (*CBtable)[0x40] = strdup("BIT\t0,B");
    (*CBtable)[0x41] = strdup("BIT\t0,C");
    (*CBtable)[0x42] = strdup("BIT\t0,D");
    (*CBtable)[0x43] = strdup("BIT\t0,E");
    (*CBtable)[0x44] = strdup("BIT\t0,h");
    (*CBtable)[0x45] = strdup("BIT\t0,l");
    (*CBtable)[0x46] = strdup("BIT\t0,(x)");
    (*CBtable)[0x47] = strdup("BIT\t0,A");
    (*CBtable)[0x48] = strdup("BIT\t1,B");
    (*CBtable)[0x49] = strdup("BIT\t1,C");
    (*CBtable)[0x4A] = strdup("BIT\t1,D");
    (*CBtable)[0x4B] = strdup("BIT\t1,E");
    (*CBtable)[0x4C] = strdup("BIT\t1,h");
    (*CBtable)[0x4D] = strdup("BIT\t1,l");
    (*CBtable)[0x4E] = strdup("BIT\t1,(x)");
    (*CBtable)[0x4F] = strdup("BIT\t1,A");
    (*CBtable)[0x50] = strdup("BIT\t2,B");
    (*CBtable)[0x51] = strdup("BIT\t2,C");
    (*CBtable)[0x52] = strdup("BIT\t2,D");
    (*CBtable)[0x53] = strdup("BIT\t2,E");
    (*CBtable)[0x54] = strdup("BIT\t2,h");
    (*CBtable)[0x55] = strdup("BIT\t2,l");
    (*CBtable)[0x56] = strdup("BIT\t2,(x)");
    (*CBtable)[0x57] = strdup("BIT\t2,A");
    (*CBtable)[0x58] = strdup("BIT\t3,B");
    (*CBtable)[0x59] = strdup("BIT\t3,C");
    (*CBtable)[0x5A] = strdup("BIT\t3,D");
    (*CBtable)[0x5B] = strdup("BIT\t3,E");
    (*CBtable)[0x5C] = strdup("BIT\t3,h");
    (*CBtable)[0x5D] = strdup("BIT\t3,l");
    (*CBtable)[0x5E] = strdup("BIT\t3,(x)");
    (*CBtable)[0x5F] = strdup("BIT\t3,A");
    (*CBtable)[0x60] = strdup("BIT\t4,B");
    (*CBtable)[0x61] = strdup("BIT\t4,C");
    (*CBtable)[0x62] = strdup("BIT\t4,D");
    (*CBtable)[0x63] = strdup("BIT\t4,E");
    (*CBtable)[0x64] = strdup("BIT\t4,h");
    (*CBtable)[0x65] = strdup("BIT\t4,l");
    (*CBtable)[0x66] = strdup("BIT\t4,(x)");
    (*CBtable)[0x67] = strdup("BIT\t4,A");
    (*CBtable)[0x68] = strdup("BIT\t5,B");
    (*CBtable)[0x69] = strdup("BIT\t5,C");
    (*CBtable)[0x6A] = strdup("BIT\t5,D");
    (*CBtable)[0x6B] = strdup("BIT\t5,E");
    (*CBtable)[0x6C] = strdup("BIT\t5,h");
    (*CBtable)[0x6D] = strdup("BIT\t5,l");
    (*CBtable)[0x6E] = strdup("BIT\t5,(x)");
    (*CBtable)[0x6F] = strdup("BIT\t5,A");
    (*CBtable)[0x70] = strdup("BIT\t6,B");
    (*CBtable)[0x71] = strdup("BIT\t6,C");
    (*CBtable)[0x72] = strdup("BIT\t6,D");
    (*CBtable)[0x73] = strdup("BIT\t6,E");
    (*CBtable)[0x74] = strdup("BIT\t6,h");
    (*CBtable)[0x75] = strdup("BIT\t6,l");
    (*CBtable)[0x76] = strdup("BIT\t6,(x)");
    (*CBtable)[0x77] = strdup("BIT\t6,A");
    (*CBtable)[0x78] = strdup("BIT\t7,B");
    (*CBtable)[0x79] = strdup("BIT\t7,C");
    (*CBtable)[0x7A] = strdup("BIT\t7,D");
    (*CBtable)[0x7B] = strdup("BIT\t7,E");
    (*CBtable)[0x7C] = strdup("BIT\t7,h");
    (*CBtable)[0x7D] = strdup("BIT\t7,l");
    (*CBtable)[0x7E] = strdup("BIT\t7,(x)");
    (*CBtable)[0x7F] = strdup("BIT\t7,A");
    (*CBtable)[0x80] = strdup("RES\t0,B");
    (*CBtable)[0x81] = strdup("RES\t0,C");
    (*CBtable)[0x82] = strdup("RES\t0,D");
    (*CBtable)[0x83] = strdup("RES\t0,E");
    (*CBtable)[0x84] = strdup("RES\t0,h");
    (*CBtable)[0x85] = strdup("RES\t0,l");
    (*CBtable)[0x86] = strdup("RES\t0,(x)");
    (*CBtable)[0x87] = strdup("RES\t0,A");
    (*CBtable)[0x88] = strdup("RES\t1,B");
    (*CBtable)[0x89] = strdup("RES\t1,C");
    (*CBtable)[0x8A] = strdup("RES\t1,D");
    (*CBtable)[0x8B] = strdup("RES\t1,E");
    (*CBtable)[0x8C] = strdup("RES\t1,h");
    (*CBtable)[0x8D] = strdup("RES\t1,l");
    (*CBtable)[0x8E] = strdup("RES\t1,(x)");
    (*CBtable)[0x8F] = strdup("RES\t1,A");
    (*CBtable)[0x90] = strdup("RES\t2,B");
    (*CBtable)[0x91] = strdup("RES\t2,C");
    (*CBtable)[0x92] = strdup("RES\t2,D");
    (*CBtable)[0x93] = strdup("RES\t2,E");
    (*CBtable)[0x94] = strdup("RES\t2,h");
    (*CBtable)[0x95] = strdup("RES\t2,l");
    (*CBtable)[0x96] = strdup("RES\t2,(x)");
    (*CBtable)[0x97] = strdup("RES\t2,A");
    (*CBtable)[0x98] = strdup("RES\t3,B");
    (*CBtable)[0x99] = strdup("RES\t3,C");
    (*CBtable)[0x9A] = strdup("RES\t3,D");
    (*CBtable)[0x9B] = strdup("RES\t3,E");
    (*CBtable)[0x9C] = strdup("RES\t3,h");
    (*CBtable)[0x9D] = strdup("RES\t3,l");
    (*CBtable)[0x9E] = strdup("RES\t3,(x)");
    (*CBtable)[0x9F] = strdup("RES\t3,A");
    (*CBtable)[0xA0] = strdup("RES\t4,B");
    (*CBtable)[0xA1] = strdup("RES\t4,C");
    (*CBtable)[0xA2] = strdup("RES\t4,D");
    (*CBtable)[0xA3] = strdup("RES\t4,E");
    (*CBtable)[0xA4] = strdup("RES\t4,h");
    (*CBtable)[0xA5] = strdup("RES\t4,l");
    (*CBtable)[0xA6] = strdup("RES\t4,(x)");
    (*CBtable)[0xA7] = strdup("RES\t4,A");
    (*CBtable)[0xA8] = strdup("RES\t5,B");
    (*CBtable)[0xA9] = strdup("RES\t5,C");
    (*CBtable)[0xAA] = strdup("RES\t5,D");
    (*CBtable)[0xAB] = strdup("RES\t5,E");
    (*CBtable)[0xAC] = strdup("RES\t5,h");
    (*CBtable)[0xAD] = strdup("RES\t5,l");
    (*CBtable)[0xAE] = strdup("RES\t5,(x)");
    (*CBtable)[0xAF] = strdup("RES\t5,A");
    (*CBtable)[0xB0] = strdup("RES\t6,B");
    (*CBtable)[0xB1] = strdup("RES\t6,C");
    (*CBtable)[0xB2] = strdup("RES\t6,D");
    (*CBtable)[0xB3] = strdup("RES\t6,E");
    (*CBtable)[0xB4] = strdup("RES\t6,h");
    (*CBtable)[0xB5] = strdup("RES\t6,l");
    (*CBtable)[0xB6] = strdup("RES\t6,(x)");
    (*CBtable)[0xB7] = strdup("RES\t6,A");
    (*CBtable)[0xB8] = strdup("RES\t7,B");
    (*CBtable)[0xB9] = strdup("RES\t7,C");
    (*CBtable)[0xBA] = strdup("RES\t7,D");
    (*CBtable)[0xBB] = strdup("RES\t7,E");
    (*CBtable)[0xBC] = strdup("RES\t7,h");
    (*CBtable)[0xBD] = strdup("RES\t7,l");
    (*CBtable)[0xBE] = strdup("RES\t7,(x)");
    (*CBtable)[0xBF] = strdup("RES\t7,A");
    (*CBtable)[0xC0] = strdup("SET\t0,B");
    (*CBtable)[0xC1] = strdup("SET\t0,C");
    (*CBtable)[0xC2] = strdup("SET\t0,D");
    (*CBtable)[0xC3] = strdup("SET\t0,E");
    (*CBtable)[0xC4] = strdup("SET\t0,h");
    (*CBtable)[0xC5] = strdup("SET\t0,l");
    (*CBtable)[0xC6] = strdup("SET\t0,(x)");
    (*CBtable)[0xC7] = strdup("SET\t0,A");
    (*CBtable)[0xC8] = strdup("SET\t1,B");
    (*CBtable)[0xC9] = strdup("SET\t1,C");
    (*CBtable)[0xCA] = strdup("SET\t1,D");
    (*CBtable)[0xCB] = strdup("SET\t1,E");
    (*CBtable)[0xCC] = strdup("SET\t1,h");
    (*CBtable)[0xCD] = strdup("SET\t1,l");
    (*CBtable)[0xCE] = strdup("SET\t1,(x)");
    (*CBtable)[0xCF] = strdup("SET\t1,A");
    (*CBtable)[0xD0] = strdup("SET\t2,B");
    (*CBtable)[0xD1] = strdup("SET\t2,C");
    (*CBtable)[0xD2] = strdup("SET\t2,D");
    (*CBtable)[0xD3] = strdup("SET\t2,E");
    (*CBtable)[0xD4] = strdup("SET\t2,h");
    (*CBtable)[0xD5] = strdup("SET\t2,l");
    (*CBtable)[0xD6] = strdup("SET\t2,(x)");
    (*CBtable)[0xD7] = strdup("SET\t2,A");
    (*CBtable)[0xD8] = strdup("SET\t3,B");
    (*CBtable)[0xD9] = strdup("SET\t3,C");
    (*CBtable)[0xDA] = strdup("SET\t3,D");
    (*CBtable)[0xDB] = strdup("SET\t3,E");
    (*CBtable)[0xDC] = strdup("SET\t3,h");
    (*CBtable)[0xDD] = strdup("SET\t3,l");
    (*CBtable)[0xDE] = strdup("SET\t3,(x)");
    (*CBtable)[0xDF] = strdup("SET\t3,A");
    (*CBtable)[0xE0] = strdup("SET\t4,B");
    (*CBtable)[0xE1] = strdup("SET\t4,C");
    (*CBtable)[0xE2] = strdup("SET\t4,D");
    (*CBtable)[0xE3] = strdup("SET\t4,E");
    (*CBtable)[0xE4] = strdup("SET\t4,h");
    (*CBtable)[0xE5] = strdup("SET\t4,l");
    (*CBtable)[0xE6] = strdup("SET\t4,(x)");
    (*CBtable)[0xE7] = strdup("SET\t4,A");
    (*CBtable)[0xE8] = strdup("SET\t5,B");
    (*CBtable)[0xE9] = strdup("SET\t5,C");
    (*CBtable)[0xEA] = strdup("SET\t5,D");
    (*CBtable)[0xEB] = strdup("SET\t5,E");
    (*CBtable)[0xEC] = strdup("SET\t5,h");
    (*CBtable)[0xED] = strdup("SET\t5,l");
    (*CBtable)[0xEE] = strdup("SET\t5,(x)");
    (*CBtable)[0xEF] = strdup("SET\t5,A");
    (*CBtable)[0xF0] = strdup("SET\t6,B");
    (*CBtable)[0xF1] = strdup("SET\t6,C");
    (*CBtable)[0xF2] = strdup("SET\t6,D");
    (*CBtable)[0xF3] = strdup("SET\t6,E");
    (*CBtable)[0xF4] = strdup("SET\t6,h");
    (*CBtable)[0xF5] = strdup("SET\t6,l");
    (*CBtable)[0xF6] = strdup("SET\t6,(x)");
    (*CBtable)[0xF7] = strdup("SET\t6,A");
    (*CBtable)[0xF8] = strdup("SET\t7,B");
    (*CBtable)[0xF9] = strdup("SET\t7,C");
    (*CBtable)[0xFA] = strdup("SET\t7,D");
    (*CBtable)[0xFB] = strdup("SET\t7,E");
    (*CBtable)[0xFC] = strdup("SET\t7,h");
    (*CBtable)[0xFD] = strdup("SET\t7,l");
    (*CBtable)[0xFE] = strdup("SET\t7,(x)");
    (*CBtable)[0xFF] = strdup("SET\t7,A");

    /* ED-prefixed instructions */
    (*EDtable)[0x40] = strdup("IN\tB,(C)");
    (*EDtable)[0x41] = strdup("OUT\t(C),B");
    (*EDtable)[0x42] = strdup("SBC\thl,BC");
    (*EDtable)[0x43] = strdup("LD\t(w),BC");
    (*EDtable)[0x44] = strdup("NEG");
    (*EDtable)[0x45] = strdup("RETN");
    (*EDtable)[0x46] = strdup("IM\t0");
    (*EDtable)[0x47] = strdup("LD\tI,A");
    (*EDtable)[0x48] = strdup("IN\tC,(C)");
    (*EDtable)[0x49] = strdup("OUT\t(C),C");
    (*EDtable)[0x4A] = strdup("ADC\thl,BC");
    (*EDtable)[0x4B] = strdup("LD\tBC,(w)");
    (*EDtable)[0x4D] = strdup("RETI");
    (*EDtable)[0x4F] = strdup("LD\tR,A");
    (*EDtable)[0x50] = strdup("IN\tD,(C)");
    (*EDtable)[0x51] = strdup("OUT\t(C),D");
    (*EDtable)[0x52] = strdup("SBC\thl,DE");
    (*EDtable)[0x53] = strdup("LD\t(w),DE");
    (*EDtable)[0x56] = strdup("IM\t1");
    (*EDtable)[0x57] = strdup("LD\tA,I");
    (*EDtable)[0x58] = strdup("IN\tE,(C)");
    (*EDtable)[0x59] = strdup("OUT\t(C),E");
    (*EDtable)[0x5A] = strdup("ADC\thl,DE");
    (*EDtable)[0x5B] = strdup("LD\tDE,(w)");
    (*EDtable)[0x5E] = strdup("IM\t2");
    (*EDtable)[0x5F] = strdup("LD\tA,R");
    (*EDtable)[0x60] = strdup("IN\th,(C)");
    (*EDtable)[0x61] = strdup("OUT\t(C),h");
    (*EDtable)[0x62] = strdup("SBC\thl,hl");
    (*EDtable)[0x63] = strdup("LD\t(w),hl");
    (*EDtable)[0x67] = strdup("RRD");
    (*EDtable)[0x68] = strdup("IN\tl,(C)");
    (*EDtable)[0x69] = strdup("OUT\t(C),l");
    (*EDtable)[0x6A] = strdup("ADC\thl,hl");
    (*EDtable)[0x6B] = strdup("LD\thl,(w)");
    (*EDtable)[0x6F] = strdup("RLD");
    (*EDtable)[0x72] = strdup("SBC\thl,SP");
    (*EDtable)[0x73] = strdup("LD\t(w),SP");
    (*EDtable)[0x78] = strdup("IN\tA,(C)");
    (*EDtable)[0x79] = strdup("OUT\t(C),A");
    (*EDtable)[0x7A] = strdup("ADC\thl,SP");
    (*EDtable)[0x7B] = strdup("LD\tSP,(w)");
    (*EDtable)[0xA0] = strdup("LDI");
    (*EDtable)[0xA1] = strdup("CPI");
    (*EDtable)[0xA2] = strdup("INI");
    (*EDtable)[0xA3] = strdup("OUTI");
    (*EDtable)[0xA8] = strdup("LDD");
    (*EDtable)[0xA9] = strdup("CPD");
    (*EDtable)[0xAA] = strdup("IND");
    (*EDtable)[0xAB] = strdup("OUTD");
    (*EDtable)[0xB0] = strdup("LDIR");
    (*EDtable)[0xB1] = strdup("CPIR");
    (*EDtable)[0xB2] = strdup("INIR");
    (*EDtable)[0xB3] = strdup("OTIR");
    (*EDtable)[0xB8] = strdup("LDDR");
    (*EDtable)[0xB9] = strdup("CPDR");
    (*EDtable)[0xBA] = strdup("INDR");
    (*EDtable)[0xBB] = strdup("OTDR");
}

