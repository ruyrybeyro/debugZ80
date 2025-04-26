
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


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

int main(int argc, char *argv[])
{
   int i;

   char string[256];      // actual command line
   char old_string[256];  // copy of command line for replaying it

   init_shm_client();	  // attach to memory of QtSpecem 
   for (i = 0x5B00 ; i <= 0xFFFF ; i++ )
      if (*(mem+i) == atoi(argv[1]) )
         printf("%X\n", i); 
}

