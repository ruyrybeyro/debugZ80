/* Mem.c: Z80 memory - basic support routines.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

//#include "quirks.h"
#include "env.h"


#if !defined(NDEBUG)
extern UCHAR running;
#endif

/*=========================================================================*
 *                            writebyte                                    *
 *=========================================================================*/
void writebyte(USHORT addr, UCHAR value) {
	 UINT hiAddr = addr >> 14;

	 if (hiAddr) {               // Not in ROM?
		  *(mem+addr)  = value;
	 }
}

/*=========================================================================*
 *                            writeword                                    *
 *=========================================================================*/
void writeword(USHORT addr, USHORT value)
{
	/* Remember: Z80 word is in reversed order */
	writebyte(addr, (UCHAR) (value & 0xff) );
	writebyte(addr + 1, (UCHAR) ( (value >> 8) & 0xff) );
}

#undef readbyte

/*=========================================================================*
 *                            readbyte                                     *
 *=========================================================================*/
UCHAR readbyte(USHORT addr)
{
	/* definide as a macro at z80.h */
	return((UCHAR) *(mem+addr) );
}

#undef readword

/*=========================================================================*
 *                            readword                                     *
 *=========================================================================*/
USHORT readword(USHORT addr)
{
	/* Remember: Z80 word is in reversed order */
	return( ((USHORT) *(mem+addr) ) |
			  ( ( (USHORT) *(mem+addr + 1) ) << 8 ) );
}

/* EOF: Mem.c */
