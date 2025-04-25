/*
 *            (c) 1991-2019 Rui Fernando Ferreira Ribeiro
 *
 * -------------------------------------------------------
 *
 * CPUREGS.C : show CPU registers in debug mode
 *
 */

#include <stdio.h>
#include <stdlib.h>
//#include <alloc.h>

#include "quirks.h"
#include "env.h"

char board[18][29] =
{
   "                            ",
   " AF 0000  s=0  AF' 0000 s=0 ",
   " BC 0000  z=0  BC' 0000 z=0 ",
   " DE 0000  x=0  DE' 0000 x=0 ",
   " HL 0000  h=0  HL' 0000 h=0 ",
   "          y=0           y=0 ",
   "          p=0           p=0 ",
   "          n=0           n=0 ",
   "          c=0           c=0 ",
   "                            ",
   " IX 0000 IFF1=0 (sp+4) 0000 ",
   " IY 0000 IFF2=0 (sp+2) 0000 ",
   " SP 0000 IM  =0 (sp)   0000 ",
   " PC 0000 T=                 ",
   " R  00                      ",   /* HEX opcodes */
   " I  00                      ",
   "                            ",   /* assembly instruction */
   "                            "
};

/* Window definitions --- RIGHT, BOTTOM e NBYTES are autocalculated */
#define LEFT   40
#define RIGHT  (LEFT+28-1) /* 28 -- strlen(board[0]) */
#define TOP    1
#define BOTTOM (TOP+sizeof(board)/sizeof(char *)-1)
#define NBYTES (BOTTOM-TOP+1)*(RIGHT-LEFT+1)*2

/* PC, R not kept (always diferent) */
static UCHAR  OldI = 0;

static USHORT OldAF  = 0, OldBC  = 0, OldDE  = 0, OldHL  = 0,
              OldAF2 = 0, OldBC2 = 0, OldDE2 = 0, OldHL2 = 0,
              OldIX  = 0, OldIY  = 0, OldSP = 0;

void bin2hex8 (char *, UCHAR);
void bin2hex16 (char *, USHORT);
void Init_DebugCpuRegs();

/*=========================================================================*
 *                            bin2hex8                                     *
 *=========================================================================*/
void bin2hex8(char * s, UCHAR value)
{
   const static char * hex = "0123456789ABCDEF";

   *s = *(hex + (value >> 4) );
   *(s + 1) = *(hex + (value & 0x0F));
}

/*=========================================================================*
 *                            bin2hex16                                    *
 *=========================================================================*/
void bin2hex16(char * s, USHORT value)
{
   bin2hex8(s, (UCHAR)(value >> 8));
   bin2hex8(s + 2, (UCHAR)(value & 0xFF));
}

/*=========================================================================*
 *                            draw_cpuregs                                 *
 *=========================================================================*/
unsigned short draw_cpuregs()
{
   short i;
   char *s;
   unsigned short next_PC;

   /* put Z80 var's in array to build window */

   *(board[1] + 12) = '0' + (Z80_S != 0);
   *(board[2] + 12) = '0' + (Z80_Z != 0);
   *(board[3] + 12) = '0' + (Z80_X != 0);
   *(board[4] + 12) = '0' + (Z80_H != 0);
   *(board[5] + 12) = '0' + (Z80_Y != 0);
   *(board[6] + 12) = '0' + (Z80_P != 0);
   *(board[7] + 12) = '0' + (Z80_N != 0);
   *(board[8] + 12) = '0' + (Z80_C != 0);
   *(board[1] + 26) = '0' + ((F2 & (UCHAR)0x80) && (UCHAR)1);
   *(board[2] + 26) = '0' + ((F2 & (UCHAR)0x40) && (UCHAR)1);
   *(board[3] + 26) = '0' + ((F2 & (UCHAR)0x20) && (UCHAR)1);
   *(board[4] + 26) = '0' + ((F2 & (UCHAR)0x10) && (UCHAR)1);
   *(board[5] + 26) = '0' + ((F2 & (UCHAR)0x08) && (UCHAR)1);
   *(board[6] + 26) = '0' + ((F2 & (UCHAR)0x04) && (UCHAR)1);
   *(board[7] + 26) = '0' + ((F2 & (UCHAR)0x02) && (UCHAR)1);
   *(board[8] + 26) = '0' + ((F2 & (UCHAR)0x01) && (UCHAR)1);

   *(board[10] + 14) = '0' + IFF1;
   *(board[11] + 14) = '0' + IFF2;
   *(board[12]+ 14) = '0' + _IM;
   bin2hex16(board[10] + 23, readword((USHORT)SP+4) );
   bin2hex16(board[11] + 23, readword((USHORT)SP+2) );
   bin2hex16(board[12]+ 23, readword((USHORT)SP) );
   build_F();

   if(OldAF != AF)
      bin2hex16(board[1] + 4, OldAF = AF);
   if(OldBC != BC)
      bin2hex16(board[2] + 4, OldBC = BC);
   if(OldDE != DE)
      bin2hex16(board[3] + 4, OldDE = DE);
   if(OldHL != HL)
      bin2hex16(board[4] + 4, OldHL = HL);
   if(OldAF2 != AF2)
      bin2hex16(board[1] + 19, OldAF2 = AF2);
   if(OldBC2 != BC2)
      bin2hex16(board[2] + 19, OldBC2 = BC2);
   if(OldDE2 != DE2)
      bin2hex16(board[3] + 19, OldDE2 = DE2);
   if(OldHL2 != HL2)
      bin2hex16(board[4] + 19, OldHL2 = HL2);
   if(OldIX != IX)
      bin2hex16(board[10] + 4, OldIX = IX);
   if(OldIY != IY)
      bin2hex16(board[11] + 4, OldIY = IY);
   if(OldSP != SP)
      bin2hex16(board[12] + 4, OldSP = SP);
   bin2hex16(board[13] + 4, (USHORT)PC);
   sprintf(board[13] + 11, "%ld ", clock_ticks );
   bin2hex8(board[14] + 4, R);
   if(OldI != I)
      bin2hex8(board[15] + 4, OldI = I);
   s = ldissbl((USHORT)PC);
   next_PC = PC + (unsigned short)*s;
   for(i = 0 ; i < *s ; i++)
   {
      bin2hex8(board[14]+10 + i * 2, readbyte((USHORT)PC+i) );
   }
   while(i < 7)
   {
      *(board[14]+10+i * 2) = ' ';
      *(board[14]+10+i++ * 2+1) = ' ';
   }

   /* Get Z80 intruction */
   s++;

   i = 0;
   while(*s)
   {
      if(*s == '\t')
      {
         while(i < 5)
            *(board[16]+10+ i++) = ' ';
         s++;
      }
      else
         *(board[16]+10+ i++) = *s++;
   }
   while(i < 17)
      *(board[16]+10+ i++) = ' ';
   for(i = 0 ; i < 18  ; i++)
   {
      //gotoxy(LEFT, TOP+i);
      printf("%s\n", board[i]);
   }
   return next_PC;
}

/* EOF: CPUREGS.C */
