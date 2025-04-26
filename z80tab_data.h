/*
 * Z80 instruction table data header
 * (C) 1991-2025 Rui Fernando F. Ribeiro
 */

#ifndef Z80TAB_DATA_H
#define Z80TAB_DATA_H

/*
 * Initialize the Z80 instruction tables with embedded data
 * Used instead of reading from z80tab.txt
 */
void init_z80tab_data(char ***maintable, char ***CBtable, char ***EDtable);

#endif /* Z80TAB_DATA_H */
