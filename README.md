# debugZ80 v0.84

© 2019-2025 Rui Fernando Ferreira Ribeiro

## Overview

debugZ80 is a command-line Z80 debugger for Linux/Mac/BSD systems, designed with a user interface similar to MS-DOS DEBUG.EXE but specifically for Z80/ZX Spectrum assembly.

This tool serves as a companion to [QtSpecem](https://github.com/ruyrybeyro/QtSpecem). For successful compilation, the debugZ80 directory must be at the same root level as the QtSpecem source code.

debugZ80 is a real-time console mode debugger. While still in alpha stage, it provides a functional demonstration of its capabilities. The code will undergo further improvements and restructuring in future releases.

Tested on:
- macOS Catalina and Sequoia
- Debian 10 and Debian 13

## Key Features

- Breakpoint management
- ZX BASIC program listing
- Trace/running functionality (handled by debugZ80, not the emulator)

Note: The current implementation processes trace and running logic within debugZ80 itself, which results in slower performance compared to emulator-side processing.

## Compilation Instructions

### Linux/GCC:
```
cp Makefile.linux Makefile
make
```

### macOS/Clang:
```
cp Makefile.MacOS Makefile
make
```

## Usage

Launch debugZ80 using:
```
./debugz80
```

Important: debugZ80 must be launched *after* QtSpecem. The files `z80tab.txt` and `zx_labels.txt` must be present in the current directory.

## Command Reference

| Command | Description |
|---------|-------------|
| `T [XXXX]` | Trace (step by step, including interrupt routines) |
| `P [XXXX]` | Proceed (step over calls, without showing interrupt routines) |
| `E` | Execute until return from subroutine |
| `G [XXXX]` | Go (execute from PC or specified address) |
| `Q` | Quit |
| `U [XXXX]` | Disassemble |
| `D [XXXX]` | Memory dump (hexadecimal) |
| `R` | Show Z80 registers |
| `R YY XXXX` | Load 16-bit registers |
| `R Y XX` | Load 8-bit registers |
| `L` | List ZX BASIC program |
| `L sysvars` | List system variables |
| `K XXXX XX` | Poke address with byte |
| `?` | Display help |
| `ENTER` | Repeat last command |

Parameters:
- `XXXX`: Four hexadecimal digits (address)
- `XX`: Two hexadecimal digits (byte value)
- `YY`: Z80 16-bit registers (AF, BC, DE, HL, IX, IY, SP, PC)
- `Y`: Z80 8-bit registers (A, B, C, D, E, H, L)

## Example Session

```
./debugz80 

- P
                            
 AF 005C  s=0  AF' 0044 s=0 

 BC 1721  z=1  BC' 171F z=1 

 DE 5CB9  h=1  DE' 00B8 h=0 

 HL 10A8  p=1  HL' 0038 p=1 

          n=0           n=0 

          c=0           c=0 
                            
 IX 03D4 IFF1=0 (sp+4) 15E1 

 IY 5C3A IFF2=0 (sp+2) FFFF 

 SP FF48 IM  =1 (sp)   15FE 

 PC 0038                    

 R  D3    F5                

 I  3F                      

          PUSH AF           
                            
- U

0038 F5             PUSH	AF ; MASK_INT   

0039 E5             PUSH	HL

003A 2A 78 5C       LD	HL,(5C78) ; LD	HL,(FRAMES1)

003D 23             INC	HL

003E 22 78 5C       LD	(5C78),HL ; LD	(FRAMES1),HL

0041 7C             LD	A,H

0042 B5             OR	L

0043 20 03          JR	NZ,0048 ; JR	NZ,KEY_INT

0045 FD 34 40       INC	(IY+40)

0048 C5             PUSH	BC ; KEY_INT   

0049 D5             PUSH	DE

004A CD BF 02       CALL	02BF ; CALL	KEYBOARD

004D D1             POP	DE

004E C1             POP	BC

004F E1             POP	HL

0050 F1             POP	AF

0051 FB             EI

0052 C9             RET

0053 E1             POP	HL ; ERROR_2   

0054 6E             LD	L,(HL)
```

## Future Development Plans

- Code refactoring and creation of proper subroutines
- Potential adoption of the `=XXXX` syntax from DEBUG.EXE
- Moving breakpoints/proceed/move functionality to the emulation side to improve performance
- Integration of external files into the binary

## License

debugZ80 © 2019-2025 Rui Fernando Ferreira Ribeiro
