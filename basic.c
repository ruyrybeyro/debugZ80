#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "quirks.h"
#include "env.h"

#define MAX_LINES 20

void show_system_vars();
void list_basic_vars(void);

// ZX Spectrum BASIC tokens as they are in ROM
char tokens[][10] =
{
   "RND",
   "INKEY$",
   "PI",
   "FN",
   "POINT",
   "SCREEN$",
   "ATTR",
   "AT",
   "TAB",
   "VAL$",
   "CODE",
   "VAL",
   "LEN",
   "SIN",
   "COS",
   "TAN",
   "ASN",
   "ACS",
   "ATN",
   "LN",
   "EXP",
   "INT",
   "SQR",
   "SGN",
   "ABS",
   "PEEK",
   "IN",
   "USR",
   "STR$",
   "CHR$",
   "NOT",
   "BIN",
   "OR",
   "AND",
   "<=",
   ">=",
   "<>",
   "LINE",
   "THEN",
   "TO",
   "STEP",
   "DEF FN",
   "CAT",
   "FORMAT",
   "MOVE",
   "ERASE",
   "OPEN #",
   "CLOSE #",
   "MERGE",
   "VERIFY",
   "BEEP",
   "CIRCLE",
   "INK",
   "PAPER",
   "FLASH",
   "BRIGHT",
   "INVERSE",
   "OVER",
   "OUT",
   "LPRINT",
   "LLIST",
   "STOP",
   "READ",
   "DATA",
   "RESTORE",
   "NEW",
   "BORDER",
   "CONTINUE",
   "DIM",
   "REM",
   "FOR",
   "GO TO",
   "GO SUB",
   "INPUT",
   "LOAD",
   "LIST",
   "LET",
   "PAUSE",
   "NEXT",
   "POKE",
   "PRINT",
   "PLOT",
   "RUN",
   "SAVE",
   "RANDOMIZE",
   "IF",
   "CLS",
   "DRAW",
   "CLEAR",
   "RETURN",
   "COPY"
};

// "Small integers have a special representation in which the first byte is 0, the second is a sign byte (0 or FFh) and the third and fourth are the integer in twos complement form, the less significant byte first."

// Routine: Daniel A. Nagy
double zx2d(unsigned char *in) {
	int e = in[0];
	unsigned long m[] = {
		in[1], in[2], in[3], in[4]
	};
	return e ?
		(pow(2, e - 160)) * (
			0x80000000
			+ ((m[0] & 0x7f) << 24)
			+ (m[1] << 16)
			+ (m[2] << 8)
			+ m[3]
			) * ((in[1] & 0x80) ? -1.0 : 1.0) :
		(in[1] ? -0x10000 : 0 ) + m[1] + (m[2] << 8);
}

// Routine: Slavo Labsky 
//double zx2d(unsigned char *in) {
//	int e = in[0];
//	long m[] = {
//		in[1], in[2], in[3], in[4]
//	};
//	return e ?
//		 pow(2,e - 128) * ( .5
//			+ ((m[0]&0x7F) / 256.0 )
//			+ (m[1] / 65536.0 )
//			+ (m[2] / 16777216.0 )
//			+ (m[3] / 4294967296.0 )
//			) * ((in[1] & 0x80) ? -1.0 : 1.0) :
//
//		(in[1] ? -0x10000 : 0 ) + m[1] + (m[2] << 8);
//}

// 2 byte line number (in big-endian format)
// 2 byte length of text including NEWLINE (in little endian format, length "excludes" the line number and length, i.e. to skip between lines you add "length of text" +4 bytes.
// text (BASIC tokens)
// NEWLINE (0x76)
// When a numeric constant is included in the text of a BASIC line, an ASCII string displaying the constant value will be inserted, followed by the token 0x7E, and the next 5 bytes are the value of the constant in floating point format.


void list_basic(char * s)
{
   int prog, vars, nxtlin, e_line, worksp;
   int line, len, cnt_line = 0;
   static int pos = 0;
   unsigned char c, oldc;
   int line_buffer = 0, list = 0, show_addr = 0;

   if ( (s != NULL) && !strncmp(s, "sysvars", 7) )
   {
      show_system_vars();
      return;
   }

   if ( (s != NULL) && !strncmp(s, "basicvars", 9) )
   {
      list_basic_vars();
      return;
   }

   if ( (s != NULL) && !strncmp(s, "noautorun", 9) )
   {
      *(mem+0x08A1) = 0x18;   // replace JR NZ, with JR
      return;
   }

   if ( (s != NULL) && !strncmp(s, "noheaders", 9) )
   {
      *(mem+0x0792) = 0x00;   // replace PO_MSG, with NOP
      *(mem+0x0793) = 0x00;   // 
      *(mem+0x0794) = 0x00;   // 
      *(mem+0x07AD) = 0x00;
      *(mem+0x07B6) = 0x00;
      return;
   }

   if ( (s != NULL) && !strncmp(s, "nosaldret", 9) )
   {
      *(mem+0x055E) = 0;   // delete LD HL,$053F
      *(mem+0x055F) = 0;   // 
      *(mem+0x0560) = 0;   // 
      *(mem+0x0561) = 0;   // delete PUSH HL
      return;
   }

   if ( (s != NULL) && !strncmp(s, "autorun", 7) )
   {
      *(mem+0x08A1) = 0x20; // write JR NZ,
      return;
   }

   if ( (s != NULL) && !strncmp(s, "linebuffer", 10)  )
   {
      line_buffer = 1;
   }

   if ( (s != NULL) && !strncmp(s, "list", 4)  )
   {
      list = 1;
   }

    if ( (s != NULL) && !strncmp(s, "addr", 4)  )
   {
      show_addr = 1;
   }
   
   // PROG is the special system variable that tells us where the BASIC program starts in memory 
   prog   = readword(23635);
   // VARS is the special system variable for storing where BASIC ends and program variables
   // start 
   vars   = readword(23627);
   // NEXTLIN points to the BASIC line being ran
   nxtlin = readword(23637);
   // E_LINE points to temporary line buffer
   e_line = readword(0x5C59);
   // WORKSP to the end of it
   worksp = readword(0x5C61);

   printf("(PROG)   %04X (VARS)   %04X (NXTLIN) %04X\n", prog, vars, nxtlin);
   printf("(E_LINE) %04X (WORKSP) %04X\n\n", e_line, worksp);

   if ( !pos || pos > vars)
      pos = prog;

   if (line_buffer)
      pos = e_line;

   // show BASIC lines until reaching (VARS) or MAX_LINES
   while ( line_buffer?(pos < worksp - 1):(pos < vars) && (cnt_line != MAX_LINES ) )
   {

       if (!line_buffer)
       {
       // line is in high-endian format
       line = readbyte(pos++) * 256;
       line += readbyte(pos++);

       // print memory position of start of line and line number
       if (!list && show_addr)
          printf("[mem:$%04X]\n%d ", pos-2, line);
       else
          printf("%d ", line);

       len = readword(pos++);
       pos++;
       }
       else
          len = worksp - e_line - 1;

       c = 0;
       // while (--len > 0 )
       while (1)
       {
          oldc = c;
          c = readbyte(pos++);

          if ( c == 0x0D )
             break;

          // If we find the marker for integer/float internal representation
          if ( (c == 0x0E) && (oldc >='0' && oldc <= '9'))
          {
             if (!list)
             {
             // positive integers are often messed up in games
             if ( (readbyte(pos) == 0) && (readbyte(pos+1) == 0) )
             {
                printf("[int:%d,$%04X,addr:$%04X]", readword(pos+2), readword(pos+2), pos);
             }
             else
             {
                printf("[num:%lf,addr:$%04X]", zx2d(mem+pos), pos);
             }
             }

	     // skip 5 bytes of BASIC's internal number representation
             pos += 5;
             len -= 5;
             
             // failsafe for a mal-formed BASIC line
             //if ( len < 0 )
             //   break;
          }
          else
          {
             // from 0xA5 to 0xFF we are printing tokens
             if (c >= 0xA5)
                printf("%s ", tokens[c - 0xA5]);
             else
             {
                // bellow " ", we do not want to print unprintable
                // control characters
                if (c >= 32)
                {
                   putchar(c);
                   
                   if (show_addr && (c == ':'))
                      printf("\n[mem:$%04X]", pos);

                   if (c == 32)
                      c=oldc;
                }
                else
                {
                   if (!list)
                      putchar('.');
                   c=oldc;
                }
             }
          }
       }
       
       //pos++; // point to next byte
       cnt_line++;
       printf("\n"); // end of BASIC line
   }  

   // if we reached the end of the BASIC program
   // point it to the beggining again
   if ( (cnt_line != MAX_LINES ) || (prog == vars) )
      pos = prog;
}

