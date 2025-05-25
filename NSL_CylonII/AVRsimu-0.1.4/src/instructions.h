/*
 *  AVRsimu
 *
 *  Copyright (C) 2005 Jonas Diemer,  diemer@gmx.de
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#undef OUT
#undef IN

//#define INSTR_MAX	12	//Don't forget to Increase when adding a new instr.
				//this will be 128? when we're done
enum _instructions {
	RJMP,
	LDI,
	LDS,
	RCALL,
	RET,
	PUSH,
	POP,
	OUT,
	IN,
	MOV,	//ab hier noch ungetestet!!!
	CP,	
	ADC,
	ADD,
	ADIW,
	AND,
	ANDI,
	ASR,
	BCLR,
	BLD,
	BRBC,
	BRBS,
	/* BRCC,	implemented by BRBC
	 * BRCS,	implemented by BRBS	
	 * BREAK,	later... :-)
	 * BREQ,	implemented by BRBS (ZFLAG)
	 * BRGE,	implemented by BRBC (SFLAG)
	 * BRHC,	implemented by BRBC (HFLAG)
	 * BRHS,	implemented by BRBS (HFLAG)
	 * BRID,	implemented by BRBC (IFLAG)
	 * BRIE,	implemented by BRBS (IFLAG)
	 * BRLO,	synonym for BRCS
	 * BRLT,	implemented by BRBS (SFLAG)
	 * BRMI,	implemented by BRBS (NFLAG)
	 * BRNE,	implemented by BRBC (ZFLAG)
	 * BRPL,	implemented by BRBC (NFLAG)
	 * BRSH,	synonym for BRCC
	 * BRTC,	implemented by BRBC (TFLAG)
	 * BRTS,	implemented by BRBS (TFLAG)
	 * BRVC,	implemented by BRBC (VFLAG)
	 * BRVS,	implemented by BRBS (VFLAG)
	 */
	BSET,
	BST,
	/* CALL,	later... */
	CBI,
	/* CBR,		see ANDI with K complemented 
	 * CLC,		implemented by BCLR
	 * CLH,		implemented by BCLR
	 * CLI,		implemented by BCLR
	 * CLN,		implemented by BCLR
	 * CLR,		see EOR Rd, Rd
	 * CLS,		implemented by BCLR
	 * CLT,		implemented by BCLR
	 * CLV,		implemented by BCLR
	 * CLZ,		implemented by BCLR
	 * */
	COM,
	CPC,
	CPI,
	CPSE,
	DEC,
	/* EICALL,	later 
	 * EIJMP,	later
	 * ELPM,	later
	 * */
	EOR,
	/* FMUL,	later
	 * FMULS,	later
	 * FMULSU,	later
	 * ICALL,	later */
	IJMP,
	INC,
	/* JMP,		later
	 * LD,		later */
	   LDD,
	/* LDS,		later */
	LPM,		//LPM with no operands
	LPMZ,		//LPM with Rd and Z
	/* LSL,		see ADD Rd, Rd */
	LSR,
	MOVW,
	/* MUL		later
	 * MULS
	 * MULSU
	 */
	NEG,
	NOP,
	OR,
	ORI,
	RETI,
	/* ROL,		see ADC Rd,Rd
	 */
	ROR,
	SBC,
	SBCI,
	SBI,
	SBIC,
	SBIS,
	STD,
	STS,
	ST_X,
	ST_XP,
	ST_MX,
	ST_YP,
	ST_MY,
	ST_ZP,
	ST_MZ,
	INSTR_MAX
};
		

/*
#define RJMP	0
#define	LDI	1
#define RCALL	2
#define RET	3
#define PUSH	4
#define POP	5
#define OUT	6
#define IN	7
#define MOV	8
#define CP	9 
//now that we have the (as I believe) most frequent instructions,
//we will do the rest alphabetically.
#define ADC	10
*/
struct instruction{
	unsigned short int mask; 	//mask masks out the data bits. 
	unsigned short int code;	//code is the opcode with databits set to 0
	unsigned short int size;	//size of the instruction: 1 or 2
	void (*function_ptr) (struct avrmcu*, unsigned short int, unsigned short int);
};

struct instruction* init_instructions_array(void);

unsigned short int getInstructionSize( unsigned short int opcode );


/* Define a global Variable that holds the instructions */
extern struct instruction *instructions; //[INSTR_MAX];



