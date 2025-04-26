/*
 * ZX Spectrum memory address labels header
 * (C) 1991-2025 Rui Fernando F. Ribeiro
 */

#ifndef ZX_LABELS_DATA_H
#define ZX_LABELS_DATA_H

/*
 * Initialize the ZX Spectrum ROM labels table with embedded data
 * Used instead of reading from zx_labels.txt
 */
void init_zx_labels_data(char ***label_table);

#endif /* ZX_LABELS_DATA_H */
