/*
 *			  EmuZ80 v1.0
 *	      (c) 1991,1992 Rui Fernando Ferreira Ribeiro
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

static void Init_labels(void)
{
   FILE * stream;
   unsigned short str_len, pos = 5, tmp;
   char buffer[255];

   // alloc label pointers for the 64K addresses
   if( !(label_table = (char **)calloc(65536, sizeof(char *)) ) )
   {  
      Panic("Insuficient memory");
   }
   if(!(stream = fopen("zx_labels.txt", "r")))
   {  
      Panic("Error: can't read 'zx_labels.txt' file");
   }
   while(fgets(buffer, 199, stream))
   {  
     str_len=strlen(buffer);
     buffer[--str_len] = '\0';

     tmp = (int)strtol(buffer, NULL, 16);

     if (!(*(label_table + tmp) = malloc(str_len-pos+1)))
            Panic("Insuficient memory");
     strcpy(*(label_table + tmp), buffer+pos);
   }
   fclose(stream);
}

/*=========================================================================*
 *                            Init_dissbl                                  *
 *=========================================================================*/
static void Init_dissbl()
{
   FILE * stream;
   unsigned short line = 0, str_len, pos, tmp;
   char buffer[255];

   inited++;
   if( !(maintable = (char **)calloc(256, sizeof(char *)) ) ||
       !(CBtable   = (char **)calloc(256, sizeof(char *)) ) ||
       !(EDtable   = (char **)calloc(256, sizeof(char *)) )  )
   {
      Panic("Insuficient memory");
   }
   if(!(stream = fopen("z80tab.txt", "r")))
   {
      Panic("Error: can't read 'z80tab.txt' file");
   }
   while(fgets(buffer, 199, stream))
   {
      line++;
      str_len=strlen(buffer);
      if( (str_len < 3) || (buffer[0] == ';') )
         continue;
      /* delete space at beginning */
      while(buffer[str_len-1] <= ' ')
      {
         buffer[--str_len] = '\0';
      }
      for(pos = 0; buffer[pos] && (buffer[pos] != '\t') ; pos++);
      if(!buffer[pos] || ((pos != 2) && (pos != 4)) || (str_len >= 30) )
      {
	 sprintf(buffer, "Error : invalid  'z80.tab' line ; %u", line);
         Panic(buffer);
      }
      str_len = str_len - pos++;
      tmp = hextodec(buffer);
      if(pos == 3)
      {
         if (!(*(maintable + tmp) = malloc(str_len)) )
	    Panic("Insuficient memory");
         strcpy(*(maintable + tmp), buffer+pos);
      }
      else
      {
         if(tmp == 0xCB)
         {
            tmp = hextodec(buffer+2);
            if (!(*(CBtable + tmp) = malloc(str_len)) )
	       Panic("Insuficient memory");
            strcpy(*(CBtable + tmp), buffer+pos);
         }
         else
            if(tmp == 0xED)
            {
               tmp = hextodec(buffer+2);
               if (!(*(EDtable + tmp) = malloc(str_len)) )
		  Panic("Insuficient memory");
               strcpy(*(EDtable + tmp), buffer+pos);
            }
            else
            {
               sprintf(buffer, 
	      "Error : invalid extension 'z80.tab', line %u", line);
               Panic(buffer);
            }
      }
   }
   fclose(stream);
   Init_labels();
}

/*=========================================================================*
 *                            Close_dissbl                                 *
 *=========================================================================*/
void Close_dissbl()
{
   short i;

   if(inited)
      inited--;
   for( i = 0 ; i < 256 ; i++)
   {
      if(*(maintable+i))
         free((VOID *)*(maintable+i));
      if(*(CBtable+i))
         free((VOID *)*(CBtable+i));
      if(*(maintable+i))
         free((VOID *)*(EDtable+i));
   }
   free((VOID *)maintable);
   free((VOID *)CBtable);
   free((VOID *)EDtable);
}

/* EOF: DISSBL.C */
