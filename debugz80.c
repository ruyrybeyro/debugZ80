/*
 *	      (c) 1991-2019 Rui Fernando Ferreira Ribeiro
 *
 * -------------------------------------------------------
 *
 * Z80DEBUG.C : Z80's internal debugger
 *
 */

#include <stdio.h>
#include <stdlib.h>
//#include <process.h>
#include <string.h>

#include "env.h"

/* Trace flag
*/
//static UCHAR running = 0;
// static UCHAR flag_brkp = 0;
// static USHORT brkpoint = 0;

static USHORT last_PC = 0xFFFF; /********/

void draw_cpuregs();

/*=========================================================================*
 *                            trace                                        *
 *=========================================================================*/
void trace()
{
      {
	 draw_cpuregs();
	 last_PC = (USHORT)PC;
      }
   }

static UCHAR inited = 0;

/*=========================================================================*
 *                            Init_Z80debugger                             *
 *=========================================================================*/
void Init_Z80debugger()
{
   if(!inited)
   {
      inited++;
   }
}

/*=========================================================================*
 *                            Close_Z80debugger                            *
 *=========================================================================*/
void Close_Z80debugger()
{
   if(inited)
   {
      inited--;
   }
}

/* EOF: Z80DEBUG.C */
