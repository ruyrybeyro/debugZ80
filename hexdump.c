/*
 *	      (c) 1991-2019 Rui Fernando Ferreira Ribeiro
 *
 * -------------------------------------------------------
 *
 * HEXDUMP.C : show hexadecimal values in CPU windows
 *
 */

#define NELEM 16               /* No. of bytes per line */
#define NCOL  30
#define NLINE 1
#define BYTES NELEM*20         /* Total bytes per window */
#define LEN_TABLE NELEM * 3 + NELEM + 9
#define DEPTH_TABLE BYTES/NELEM + 1

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "quirks.h"
#include "env.h"

/*=========================================================================*
 *                            print_hexdump                                *
 *=========================================================================*/
void print_hexdump(USHORT adress)
{
   UCHAR tmp;
   USHORT cadress;
   int i,j;

   cadress = adress;
   for (i = 0 ; i < BYTES ; i+=NELEM)
   {
       putchar(' ');
       printf("%04X ", adress);
       for (j = 0 ; j < NELEM ; j++)
       {
	  if ((j + i) < BYTES)
	      //printf("%02X ", memory?readbyte(cadress++):readport(cadress++));
	      printf("%02X ", readbyte(cadress++));
	  else
	     printf("   ");
       }
       cadress = adress;
       for (j=0 ; j < NELEM ; j++)
       {
	  // tmp = memory?readbyte(cadress):readport(cadress++);
	  tmp = readbyte(cadress);
	  if (isprint(tmp) || (tmp > 0x7f && tmp < 0xff))
	     putchar((j+i<BYTES) ?  tmp  : ' ');
	  else
	     putchar((j+i<BYTES) ?  '.'  : ' ');
	  cadress++;
       }
       putchar(' ');
       putchar('\n');
       adress = cadress;
   }
}

void hexdump(USHORT adress)
{
   print_hexdump(adress);
}

/* EOF : HEXDUMP.C */
