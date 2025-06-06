
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

unsigned short draw_cpuregs();
void list_basic(char * s);

#include "z80.h"
#include "iglobal.h"

void print_hexdump(USHORT adress,int memory);

#include <string.h>

void build_F();

extern char ** label_table;

unsigned char * attach_speccy_shared_ram(void);
unsigned char * attach_speccy_shared_vars(void);
void dealloc_shared(unsigned char * mem, unsigned char * vars);

unsigned char        bFlashOn = 1;  /* Flash on or off      */

/* Main registers   */
union Z80Regs * Z80Regs;

/* Alternative registers */
union Z80Regs * Z80Regs2;

union Z80IX * Z80IX;
union Z80IY * Z80IY;

struct CPU_flags * flags;
struct Z80vars * Z80vars;

/* Interrupt mode - 0, 1 or 2 */
// UCHAR _IM;

/* Interrupt flag & copy
 */
// UCHAR IFF1, IFF2;

/* Used in DDCB and FDCB to keep a indice for IX and IY */
UCHAR lastbyte;

/* Memory 64k */
UCHAR * mem;
UCHAR * vars;

USHORT addr;
USHORT daddr;

int proceed_mode = 0;
int return_mode = 0;
USHORT old_PC;
USHORT old_SP;
USHORT next_PC;
USHORT pSP;

void show_help(void)
{
   printf(" H 99999             - Convert decimal number to hexa\n");
   printf(" T [XXXX]            - Trace\n");
   printf(" P [XXXX]            - Proceed\n");
   printf(" E                   - Execute until RET\n");
   printf(" B XXXX              - Break point\n");
   printf(" B 1BD1              - Break point - next BASIC line\n");
   printf(" B 1B76              - Break point - STMT_RET\n");
   printf(" B 1BB3              - Break point - LINE END\n");
   printf(" B 34BB              - Break point - USR no (end)\n");
   printf(" B 0556              - Break point - LD_BYTES (start)\n");
   printf(" B 05E2              - Break point - LD_BYTES (end)\n");
   printf(" G [XXXX]            - Go\n");
   printf(" Q                   - Quit\n");
   printf(" U [XXXX]            - Dissassembly\n");
   printf(" D [XXXX]            - Dump\n");
   printf(" R                   - Show registers\n");
   printf(" R YY XXXX           - Load 16-bit vars\n");
   printf(" R Y XX              - Load 8-bit vars\n");
   printf(" L                   - List ZX BASIC program\n");
   printf(" L list              - List ZX BASIC program without reveal numbers\n");
   printf(" L addr              - List ZX BASIC with addresses\n");
   printf(" L sysvars           - List system variables\n");
   printf(" L basicvars         - List BASIC variables\n");
   printf(" L linebuffer        - Print BASIC linebuffer\n");
   printf(" L noheaders         - Disable loading header printing\n");
   printf(" L nosaldret         - Disable LD_BYTES falling into SA_LD_RET\n");
   printf(" L noautorun         - Disable BASIC LOAD\"\" auto run\n");
   printf(" L autorun           - enable auto run back\n");
   printf(" K XXXX XX           - Poke hexa addr with hexa byte\n");
   printf(" W XXXX XXXX [file]  - Write raw Spectrum memory into /tmp/a or file, init len\n");
   printf(" W XXXX XXXX file.tap [name] - Write Spectrum memory into TAP, with name in header\n");
   printf(" O XXXX [file]       - Open-load /tmp/a into Spectrum memory address\n");
   printf(" A XXXX assembly     - assembly into XXXX address\n");
   printf(" N path              - Change active directory\n");
   printf(" ?                   - Help\n");
   printf(" ENTER               - Repeats last command\n");
   printf("\n");
   printf(" --- XXXX four hexa digits\n");
   printf(" --- XX   two  hexa digits\n");
   printf(" --- YY   Z80 AF BC DE HL IX IY SP or PC\n");
   printf(" --- Y    Z80 A B C D E H L\n");
}

// print dissassembly in address
void unassemble(int addr_arg)
{
   char * s, *p = NULL, *var_label, *address_label;
   int i, cnt;

   // addr address to disassembly from
   if ( addr_arg != -1 )
      addr = addr_arg;

   // we want 20 dissassembly lines
   for (i = 0; i < 20 ; i++)
   {
      int j;

      // call the dissassembler
      // *s = len of opcode in bytes 
      s = ldissbl(addr);
      // print address
      printf("%04X ", addr);

      // print bytes in hexa
      for(j = 0; j < *s ; j++ )
         printf("%02X ", readbyte(addr+j));

      // formatting - max 5 bytes opcode
      for( ; j < 5 ; j++)
         printf("   ");

      // print disassembled mmemonic
      printf("%s", s + 1);

      // get var label, if any
      cnt = 0;      // hexa counter
      var_label = NULL;

      // find 4 hexa number pos p in s
      if ( strlen(s+1) > 6 )
      {
         int c;

         p = s+1; // take out byte len

	 // search string. If we found 4 consecutive hexa digits
         // we've found it -> break out of for.
         for ( j = 0 ; j < (int)strlen(s+1) && (cnt < 4) ; j++)
         {
            c = *p++;
            if ( isxdigit(c) )
               cnt++;
            else
               cnt = 0;
         }

	 // if found
         if (cnt == 4 )
         {
            p = p - 4; // p to beginging of hex number
           
            var_label =  *(label_table + (int)strtol(p, NULL, 16) );                             
         }
      }

      // get address label, if any
      address_label = NULL;
      address_label = *(label_table + addr);

      // do we need to print a comment?
      if ( ( (var_label != NULL ) || (address_label != NULL ) ) && (p != NULL) )
      {
         printf(" ; ");          // comments in Z80 assembly begin with ;

         if (address_label != NULL )          // if the address has a label
            printf("%s   ", address_label);   // print it


         if (var_label != NULL )           // if the var has a label
         {
            char * z = s +1;          // point to disassembly string

            while ( z < (p-1) )           // print it, up to find hexa number
               putchar(*z++);
            z++;
            printf("%s", var_label);       // print label
            z += 4;		      // and skip hex string
            printf("%s", z);          // print the rest of the string
         }
      }
      // end of line
      printf("\n");

      // incremment address to start of next mmemonic
      addr += s[0];
   }
}

void register_value(char * reg, char * value)
{
   int addr_arg;
   int i;
      char reg_z80 [][3] =
      {
       "AF",
       "BC",
       "DE",
       "HL",
       "SP",
       "PC",
       "IX",
       "IY",

       "A",
       "B",
       "C",
       "D",
       "E",
       "H",
       "L",
       "IM",
       "" 
      };

   if ( reg == NULL || value == NULL )
   {
      draw_cpuregs();
      return;
   }
   addr_arg = (int)strtol(value, NULL, 16);
   
   for (i = 0 ; strcmp(reg_z80[i], reg) && strcmp(reg_z80[i], "") ; i++ );

   // As variables are not allocated compile time
   // Cannot optimize this with a compile-time pointer array
   switch(i)
   {
      case  0:
             AF = addr_arg;
             build_F();
             break;
      case  1:
             BC = addr_arg;
             break;
      case  2:
             DE = addr_arg;
             break;
      case  3:
             HL = addr_arg;
             break;
      case  4:
             SP = addr_arg;
             break;
      case  5:
             PC = addr_arg;
             break;
      case  6:
             IX = addr_arg;
             break;
      case  7:
             IY = addr_arg;
             break;

      case  8:
             A = addr_arg;
             break;
      case  9:
             B = addr_arg;
             break;
      case 10:
             C = addr_arg;
             break;
      case 11:
             D = addr_arg;
             break;
      case 12:
             E = addr_arg;
             break;
      case 13:
             H = addr_arg;
             break;
      case 14:
             L = addr_arg;
             break;
      case 15:
	     _IM = 1;
	     break;

      default:
             printf("Invalid args\n");
             break;
      }
}

void poke(char * byte, char * v)
{
   int addr, value;

   addr  = (int)strtol(byte, NULL, 16);
   value = (int)strtol(v, NULL, 16);
   writebyte(addr, value);
}

// parse user line command
void do_commands(char * str)
{
    char command;

    char * pch, * token[8];
    int i = 0;
    int  addr_arg;
    FILE * f;

    // field separator por extracting tokens is " "
    pch = strtok (str," ");

    // create array of tokens
    while (pch != NULL)
    {
       token[i++] = pch;
       pch = strtok (NULL, " ");
    }
   
    // two to simply testing / parameter passing
    token[i++] = NULL;
    token[i++] = NULL;

    if (i == 2)
    {
       printf("No commands\n");
       return; 
    }

    if (token[1] != NULL)
    {
       addr_arg = (int)strtol(token[1], NULL, 16);
    }
    else
       addr_arg = -1;

    command = toupper(*token[0]);
    switch(command)
    {
        case 'N':
                if ( token[0] != NULL )
                   chdir(token[0]);
                break;
        case 'T':	// Trace
                if ( addr_arg != -1 )
                   PC = addr_arg;
                addr = PC;
                build_F();
		TraceOn = 3;
                {
                   volatile UCHAR Trace;

                   while(1)
                   {
                      Trace = TraceOn;

                      // fool optimizer compilers, Trace is never 4
                      if (Trace == 4)
                         printf("%d", Trace);

                      if (Trace != 3)
                         break;
                      // need it to overcome compiler optimizations
		      printf("");
                      usleep(1000);  // 1ms delay instead of busy wait
                   }
                }
                draw_cpuregs();
		break;
        case 'P':	// Proceed
               if ( addr_arg != -1 )
                   PC = addr_arg;
                proceed_mode = 1;
                old_PC = PC;
                old_SP = SP;
		addr = PC;
		build_F();
		next_PC = draw_cpuregs();
		TraceOn = 3;
		break;
        case 'E':	// Execute Until Return
                return_mode = 1;
                pSP = readword(SP);
		addr = PC;
		build_F();
		draw_cpuregs();
                TraceOn = 2;
		break;
        case 'G':	// Go
               if ( addr_arg != -1 )
                   PC = addr_arg;
                printf("Running...\n");
                TraceOn = 0;
		break;
        case 'Q':	// Quit
                printf("Quitting. Disabling trace.\n");
                TraceOn = 0;
                exit(0);
	        break;
	case 'D':	// Dump
                if ( addr_arg != -1 )
                   daddr = addr_arg;
                print_hexdump(daddr, 0);
                daddr += 0x140;
                break;
        case 'U':
                unassemble(addr_arg);
		break;

	case 'H':
                printf("%X\n\n", atoi(token[1]) );
                break;

        case 'B':
                BreakPoint = strtol(token[1], NULL, 16);;
                break;

        case 'R':
                register_value(token[1], token[2]);
                break;

        case 'K':
		poke(token[1], token[2]);
                break;

	case 'L':
		list_basic(token[1]);
		break;

        case 'W':
                {
                unsigned int i, i_begin, i_end;
                int tap = 0;

                i_begin = strtol(token[1], NULL, 16);
                i_end   = i_begin + strtol(token[2], NULL, 16);

                if ( token[3] != NULL )
                {
                   if ( ( strlen(token[3] ) > 4 ) )
                   {
                      char * p = token[3] + (strlen(token[3]) -4);
                   
                      if ( !strcasecmp(p, ".tap") )
                         tap = 1;
                   }

                   if (tap == 1)
                      f = fopen ("/tmp/a", "w" );
                   else
                      f = fopen (token[3], "w" );
                }
                else
                {
                   f = fopen ("/tmp/a", "w" );
                }

		for (i= i_begin ;  i < i_end ; i++ )
                   fputc(readbyte(i), f);
                fclose(f);

                if (tap == 1)
                {
                    char s[300];

                    if (token[4] != NULL )
                       sprintf(s, "file2tap /tmp/a %s %d %s", token[3], i_begin, token[4]);
                    else
                       sprintf(s, "file2tap /tmp/a %s %d", token[3], i_begin);
                    printf("\n%s\n", s);
                    system(s);
                }
                }
                break;
                   
        case 'O':
                i = (int)strtol(token[1], NULL, 16);
                if ( token[2] != NULL )
                   f= fopen(token[2], "r" );
                else
                   f= fopen("/tmp/a", "r" );
                while(!feof(f))
                {
                   char c;
                   
                   c=fgetc(f);
                   if (!feof(f))
                      *(mem+i++)=c;
                      //writebyte(i++, c );
                }
                fclose(f);
                break;

        case 'A':
                {
                int i;
                char temp[200];
                char string[100];
                int status;
                FILE * f;
                char c;
                unsigned address;
                int first = 1;
                int do_it_again;

                do
                {
                sprintf(temp, "echo \"ORG 0x%04X\\n", addr_arg);
                if (first)
                {
                   for(i=2;token[i] != NULL; i++)
                   {
                      strcat(temp, token[i]);
                      strcat(temp, " "); 
                   }
                   first=0;
                }
                else
                {
                   strcat(temp, string);
                }
                strcat(temp, "\" | pasmo - /tmp/a ");
                status=system(temp);
                if (!status)
                {
                   address=addr_arg;
                   f=fopen("/tmp/a", "r");
                   if (f != NULL )
                   {
                      while(!feof(f))
                      {
                         c=fgetc(f);
                         if(!feof(f))
                            /* writebyte(address++, c); */
                            *(mem+address++)=c;                   
                      }
                      do_it_again = 0;
                      if( address > (unsigned)addr_arg )
                      {
                         printf("- A %04X ", address);
                         addr_arg = address;
                         fgets (string, 100, stdin);   

                         if ( strcmp(string, "\n") )      // if not empty
                         {
                            string[strlen(string)-1] = '\0'; 
                            do_it_again = 1; 
                         }
                      }
                   }
                }
                } while (!status && do_it_again);
                }
                break; 

	case '?':
                show_help();
		break;

	default:
		printf(" Unknown command\n");
		break;
    }
}

void destroy_shm_client()
{
   dealloc_shared(mem, vars);
}

void init_shm_client()
{

   mem = attach_speccy_shared_ram();
   vars = attach_speccy_shared_vars();

   atexit(destroy_shm_client);

   Z80vars  = (struct Z80vars *)  vars;
   Z80Regs  = (union Z80Regs *)   vars + sizeof(struct Z80vars);
   Z80Regs2 = (union Z80Regs *)   vars + sizeof(struct Z80vars) + sizeof(union Z80Regs);
   flags    = (struct CPU_flags *)vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs);
   Z80IX    = (union Z80IX *)     vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs) + sizeof(struct CPU_flags);
   Z80IY    = (union Z80IY *)     vars + sizeof(struct Z80vars) + sizeof(union Z80Regs) + sizeof(union Z80Regs) + sizeof(struct CPU_flags) + sizeof(union Z80IX);
}

int main()
{
   char string[256];      // actual command line
   char old_string[256];  // copy of command line for replaying it

   init_shm_client();	  // attach to memory of QtSpecem 

   strcpy(old_string, "");

   // address for debugging pointing to the corrent Z80 PC
   addr = PC;

   while (1)
   {
      // if not in debugging mode
      if (!proceed_mode && !return_mode)
      {
         // draw_cpuregs();
         printf("- ");                    // print prompt

         fgets (string, 255, stdin);      // wait for command line
  
         if ( strcmp(string, "\n") )      // if not empty
         {
             strcpy(old_string, string);  // copy new command line
             string[strlen(string)-1] = '\0';
             do_commands(string); 	  // execute it
         }
         else
            do_commands(old_string);     // if empty, repeast last line
      }
      else
      {
         // handle P/proceed mode
         if (proceed_mode)
         {
            if (TraceOn == 2)
            {
               // if PC the same as last time we were here
               if ( PC == old_PC )
                 TraceOn = 3;   
               else
                  // if not next instruct and SP lower in stack than we need
                  if ( ( PC != next_PC ) && (SP < old_SP ) )
                     TraceOn = 3;
                  else
                  {
                     proceed_mode = 0; // disable proceed mode
                     draw_cpuregs();
                  }
            }
         }

	 // handle E/return after RET
         if (return_mode)
         {
            if (TraceOn == 2)
            {
               // if Z80 PC was in the address in the stack
               if ( PC == pSP )
               {
                  draw_cpuregs();

		  // disable E mode
                  return_mode = 0;
               }
               else
               {
                  // save the top address on stack
                  pSP = readword(SP);
                  // continue debugging
                  TraceOn = 3;
               }
            }
         }
      }
   }
}

