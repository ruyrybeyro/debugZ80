#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "quirks.h"
#include "env.h"

// array of system variables
struct sys_vars {
   int addr;		// Address in RAM
   int n_bytes;		// number of bytes
   char name[10];	// memmonic name
   char desc[80];	// description
} sys_vars [] = 
{
{ 0x5C00,8,"KSTATE","Used in reading the keyboard" },
{ 0x5C08,1,"LAST-K","Last key pressed" },
{ 0x5C09,1,"REPDEL","Time that a key must be held down before it repeats" },
{ 0x5C0A,1,"REPPER","Delay between successive repeats of a key held down" },
{ 0x5C0B,2,"DEFADD","Address of arguments of user defined function" },
{ 0x5C0D,1,"K-DATA","Second byte of colour controls entered from keyboard" },
{ 0x5C0E,2,"TVDATA","Colour, AT and TAB controls going to television" },
{ 0x5C10,38,"STRMS","Addresses of channels attached to streams" },
{ 0x5C36,2,"CHARS","256 less than address of character set" },
{ 0x5C38,1,"RASP","Length of warning buzz" },
{ 0x5C39,1,"PIP","Length of keyboard click" },
{ 0x5C3A,1,"ERR-NR","One less than the error report code" },
{ 0x5C3B,1,"FLAGS","Various flags to control the BASIC system" },
{ 0x5C3C,1,"TV-FLAG","Flags associated with the television" },
{ 0x5C3D,2,"ERR-SP","Address of item on machine stack to use as error return" },
{ 0x5C3F,2,"LIST-SP","Return address from automatic listing" },
{ 0x5C41,1,"MODE","Specifies K, L, C, E or G cursor" },
{ 0x5C42,2,"NEWPPC","Line to be jumped to" },
{ 0x5C44,1,"NSPPC","Statement number in line to be jumped to" },
{ 0x5C45,2,"PPC","Line number of statement being executed" },
{ 0x5C47,1,"SUBPPC","Number within line of statement being executed" },
{ 0x5C48,1,"BORDCR","Border colour" },
{ 0x5C49,2,"E-PPC","Number of current line" },
{ 0x5C4B,2,"VARS","Address of variables" },
{ 0x5C4D,2,"DEST","Address of variable in assignment" },
{ 0x5C4F,2,"CHANS","Address of channel data" },
{ 0x5C51,2,"CURCHL","Address of information used for input and output" },
{ 0x5C53,2,"PROG","Address of BASIC program" },
{ 0x5C55,2,"NXTLIN","Address of next line in program" },
{ 0x5C57,2,"DATADD","Address of terminator of last DATA item" },
{ 0x5C59,2,"E-LINE","Address of command being typed in" },
{ 0x5C5B,2,"K-CUR","Address of cursor" },
{ 0x5C5D,2,"CH-ADD","Address of the next character to be interpreted" },
{ 0x5C5F,2,"X-PTR","Address of the character after the '?' marker" },
{ 0x5C61,2,"WORKSP","Address of temporary work space" },
{ 0x5C63,2,"STKBOT","Address of bottom of calculator stack" },
{ 0x5C65,2,"STKEND","Address of start of spare space" },
{ 0x5C67,1,"BREG","Calculator's B register" },
{ 0x5C68,2,"MEM","Address of area used for calculator's memory" },
{ 0x5C6A,1,"FLAGS2","More flags" },
{ 0x5C6B,1,"DF-SZ","The number of lines in the lower part of the screen" },
{ 0x5C6C,2,"S-TOP","The number of the top program line in automatic listings" },
{ 0x5C6E,2,"OLDPPC","Line number to which CONTINUE jumps" },
{ 0x5C70,1,"OSPCC","Number within line of statement to which CONTINUE jumps" },
{ 0x5C71,1,"FLAGX","Various flags" },
{ 0x5C72,2,"STRLEN","Length of string type destination in assignment" },
{ 0x5C74,2,"T-ADDR","Address of next item in parameter table" },
{ 0x5C76,2,"SEED","The seed for RND" },
{ 0x5C78,3,"FRAMES","Frame counter" },
{ 0x5C7B,2,"UDG","Address of first user defined graphic" },
{ 0x5C7D,2,"COORDS","Coordinates of last point plotted" },
{ 0x5C7F,1,"P-POSN","Column number of printer position" },
{ 0x5C80,2,"PR-CC","Address of next position for LPRINT to print at" },
{ 0x5C82,2,"ECHO-E","Column and line number of end of input buffer" },
{ 0x5C84,2,"DF-CC","Address in display file of PRINT position" },
{ 0x5C86,2,"DF-CCL","Like DF-CC for lower part of screen" },
{ 0x5C88,2,"S-POSN","Column and line number for PRINT position" },
{ 0x5C8A,2,"S-POSNL","Like S-POSN for lower part of screen" },
{ 0x5C8C,1,"SCR-CT","Scroll counter" },
{ 0x5C8D,1,"ATTR-P","Permanent current colours" },
{ 0x5C8E,1,"MASK-P","Used for transparent colours" },
{ 0x5C8F,1,"ATTR-T","Temporary current colours" },
{ 0x5C90,1,"MASK-T","Temporary transparent colours" },
{ 0x5C91,1,"P-FLAG","More flags" },
{ 0x5C92,30,"MEMBOT","Calculator's memory area" },
{ 0x5CB0,2,"NMIADD","Non-maskable interrupt address" },
{ 0x5CB2,2,"RAMTOP","Address of last byte of BASIC system area" },
{ 0x5CB4,2,"P-RAMT","Address of last byte of physical RAM" },
{ 0,0,"","" }
};

void show_system_vars() {
   int i, j, pos;

   // Loop through system variables
   for (i = 0; sys_vars[i].addr != 0; i++) {
      printf("%04X %-7s ", sys_vars[i].addr, sys_vars[i].name);

      // Handle display of system variables based on size
      switch (sys_vars[i].n_bytes) {
         case 1:  // 1 byte
            printf("  %02X ", readbyte(sys_vars[i].addr));
            break;

         case 2:  // 2 bytes (word)
            printf("%04X ", readword(sys_vars[i].addr));
            break;

         default:  // Multiple bytes
            pos = sys_vars[i].addr;
            for (j = 0; j < sys_vars[i].n_bytes; j++) {
               printf("%02X ", readbyte(pos++));
            }
            break;
      }

      // Print description
      printf("%s\n", sys_vars[i].desc);
   }
}

