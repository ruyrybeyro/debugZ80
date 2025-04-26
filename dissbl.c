/*
 *			  EmuZ80 v1.0
 *	      (c) 1991-2025 Rui Fernando Ferreira Ribeiro
 *
 * -------------------------------------------------------
 *
 * DISSBL.C : Z80's opcode line disassembler
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "env.h"
#include "z80tab_data.h"   /* Added for embedded tables */
#include "zx_labels_data.h" /* Added for embedded labels */
#undef toupper

static void Init_dissbl();
static char ** maintable, ** CBtable, ** EDtable;
char ** label_table;
static UCHAR inited = 0;

/* class = 1 - CB
 * class = 2 - ED
 *
 * _index = 1 - DD
 * _index = 2 - FD
 */
static short class = 0, _index = 0;

static UCHAR dys; /* Used for displacement in IX and IY */
static UCHAR byte;
static char buffer[80];
static unsigned short IP_loc;
/* H, L, HL detection flag */
static UCHAR h_l;

/*=========================================================================*
 *                            Dnextbyte                                    *
 *=========================================================================*/
static UCHAR Dnextbyte()
{
   return readbyte(IP_loc++);
}

/*=========================================================================*
 *                            Dnextword                                    *
 *=========================================================================*/
static USHORT Dnextword()
{
   IP_loc += 2;
   return (readword(IP_loc - 2));
}

/*=========================================================================*
 *                            my_strcpy                                    *
 *=========================================================================*/
static void my_strcpy(char * s, char * t)
{
   static char *hilo[] = { "HL", "IX", "IY" };
   static char *high[] = { "H",  "Ix",  "Iy"  };
   static char *low[]  = { "L",  "iX",  "iY"  };
   UCHAR tmp, flag = 0;

   h_l = 0;
   while(*t)
   {
      if(islower(*t))
      {
         switch(*t)
         {
            case 'x': // subst by HL, IX+b, IY+b
                 h_l = 1;
                 strcpy(s, hilo[_index]);
                 if(_index)
                 {
                    s += 2;
                    flag = 1;
                 }
                 else
                 {
                    s++;
                    break;
                 }

            case 'b': // byte
                 tmp = ((class==1) && _index)?dys:Dnextbyte();
                 if(flag)
                 {
                   *s++ = (tmp < (UCHAR)0x80 )? '+' : '-';
                   if(tmp > (UCHAR)0x80)
                      tmp = 256 - tmp; /* tmp = NEG tmp */
                 }
                 sprintf(s++, "$%02X", tmp);
		 s++;
                 flag = 0;
               break;

            case 'w': // word
                 sprintf(s, "$%04X", Dnextword());
                 s += 4;
               break;

            case 'd': // displacement (relative jump -- [ PC + signed byte])
                 {
                    USHORT tmp;

                    tmp = IP_loc + 1;
                    sprintf(s, "$%04X", tmp+ucharToUshort(Dnextbyte()) );
                    s += 4;
                 }
               break;

            case 'h': // subst by H,  Ix or Iy
                 h_l = 1;
                 if(*(t+1)!='l')
                 {
                    strcpy(s, high[_index]);
                    if(_index)
                       s++;
                 }
                 else
                 {
                    strcpy(s++, hilo[_index]);
                    /* skip 'l' */
                    t++;
                 }
               break;

            case 'l': // subst by L,  iX or iY
                 h_l = 1;
                 strcpy(s, low[_index]);
                 if(_index)
                    s++;
         }
      }
      else
         *s = *t;
      s++;
      t++;
   }
   *s = '\0';
}

/*=========================================================================*
 *                            ldissbl                                      *
 *=========================================================================*/
char * ldissbl(USHORT adress)
{
   if(!inited)
      Init_dissbl();

   class = _index = 0;
   buffer[1] = '\0';
   IP_loc = adress;
   switch(byte = Dnextbyte() )
   {
      case (UCHAR)0xDD:
               _index = 1;
            break;
      case (UCHAR)0xFD:
               _index = 2;
            break;
   };
   if(_index)
      byte = Dnextbyte();
   switch(byte)
   {
      case (UCHAR)0xCB:
               class = 1;
               if(_index)
                  dys = Dnextbyte();
               byte = Dnextbyte();
               if(*(CBtable+byte))
                  my_strcpy(buffer + 1, *(CBtable+byte));
            break;
      case (UCHAR)0xED:
               class = 2;
               byte = Dnextbyte();
               if(*(EDtable+byte))
                  my_strcpy(buffer + 1, *(EDtable+byte));
            break;
      default:
               if(*(maintable+byte))
                  my_strcpy(buffer + 1, *(maintable+byte));
   }
   if(IP_loc > adress)
      buffer[0] = IP_loc - adress;
   else
      buffer[0] = ((USHORT)0xFFFF - adress) + IP_loc;
   if(!strcmp(buffer+1, "") )
   {
      buffer[0] = (class)? 2 : 1;
      if(_index && class)
         buffer[0] = 3;
      strcpy(buffer + 1, "???");
   }
   if(buffer[0] > 8)
      Panic("Error : overflow of line disassembler");
   return(buffer);
}

/*=========================================================================*
 *                            Hextodec                                     *
 *=========================================================================*/
unsigned short hextodec(char * s)
{
   unsigned char v1, v2;

   v1 = isdigit(*s)?(*s)-'0':toupper(*s)-'A'+10;
   v2 = isdigit(*(s+1))?(*(s+1))-'0':toupper(*(s+1))-'A'+10;
   return((((unsigned short)v1)<<4)+v2);
}

/*=========================================================================*
 *                            Init_dissbl                                  *
 *=========================================================================*/
static void Init_dissbl()
{
   inited++;
   
   /* Use embedded instruction tables instead of reading from z80tab.txt */
   init_z80tab_data(&maintable, &CBtable, &EDtable);
   
   /* Use embedded label data instead of reading from zx_labels.txt */
   init_zx_labels_data(&label_table);
}

/*=========================================================================*
 *                            Close_dissbl                                 *
 *=========================================================================*/
void Close_dissbl()
{
   unsigned short i;

   if(inited)
      inited--;
   for(i = 0; i < 256; i++)
   {
      if(*(maintable+i))
         free((VOID *)*(maintable+i));
      if(*(CBtable+i))
         free((VOID *)*(CBtable+i));
      if(*(EDtable+i))
         free((VOID *)*(EDtable+i));
   }
   free((VOID *)maintable);
   free((VOID *)CBtable);
   free((VOID *)EDtable);
   
   /* Free labels memory */
   for(i = 0; i <= 65535; i++) {
      if(*(label_table+i))
         free((VOID *)*(label_table+i));
   }
   free((VOID *)label_table);
}

/* EOF: DISSBL.C */

