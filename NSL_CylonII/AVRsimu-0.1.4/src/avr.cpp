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

#include "stdafx.h"

#include <stdio.h>

#include "avr.h"
#include "instructions.h"

const  char* const PORTS[64] = 
	{ 0		 		// 0
	, 0 			// 1	
	, 0 			// 2
	, 0 			// 3
	, "ADCL" 		// 4
	, "ADCH" 		// 5
	, "ADCSR" 		// 6
	, "ADMUX" 		// 7
	, "ACSR" 		// 8
	, 0 			// 9
	, 0 			// a
	, 0 			// b
	, 0 			// c
	, "USICR" 		// d
	, "USISR" 		// e
	, "USIDR" 		// f
	, 0 			// 10
	, 0 			// 11
	, 0 			// 12
	, 0 			// 13
	, 0 			// 14
	, 0 			// 15
	, "PINB" 		// 16
	, "DDRB" 		// 17
	, "PORTB" 		// 18
	, "PINA" 		// 19
	, "DDRA" 		// 1a
	, "PORTA" 		// 1b
	, "EECR" 		// 1c
	, "EEDR" 		// 1d
	, "EEAR" 		// 1e
	, 0 			// 1f
	, 0 			// 20
	, "WDTCTR" 		// 21
	, 0 			// 22
	, 0 			// 23
	, 0 			// 24
	, 0 			// 25
	, 0 			// 26
	, 0 			// 27
	, 0 			// 28
	, "PLLCSR" 		// 29
	, 0 			// 2a
	, "OCR1C" 		// 2b
	, "OCR1B" 		// 2c
	, "OCR1A" 		// 2d
	, "TCNT1" 		// 2e
	, "TCCR1B" 		// 2f
	, "TCCR1A" 		// 30
	, "OSCCAL" 		// 31
	, "TCNT0" 		// 32
	, "TCCR0" 		// 33
	, "MCUSR" 		// 34
	, "MCUCR" 		// 35
	, 0 			// 36
	, 0 			// 37
	, "TIFR" 		// 38
	, "TIMSK" 		// 39
	, "GIFR" 		// 3a
	, "GIMSK" 		// 3b
	, 0 			// 3c
	, "SPL" 		// 3d
	, "SPH" 		// 3e
	, "SREG" };		// 3f

/* execute the next instruction */
void step(struct avrmcu * avr)
{
	unsigned short int opcode;
	int i;

	opcode=avr->flash[avr->PC]; //FIXME: what if file has not been loaded
	printf("PC=%04X opc %04x ",avr->PC, opcode);
	/*look up the opcode*/
	for (i=0;i<INSTR_MAX;i++){
		//printf("masked: 0x%04x\n",opcode & instructions[i].mask);
		//outmask all data bits and compare with code
		if ((opcode & instructions[i].mask) == instructions[i].code){
			printf("cy[%06d] :  ", avr->cycles );
			//execute instructions (includes parsing parameters from opcode)
			instructions[i].function_ptr(avr, opcode, instructions[i].size);
			break; //leave for
		}
	}
	if (i==INSTR_MAX) { //we haven't found the instruction
			printf("Instruction not yet implemented\n");
	}
}

/**
 * Continue programm execution till next breakpoint
 */
void cont( struct avrmcu* avr ) {
	unsigned long int cyclesStart = avr->cycles;
	step( avr ); // the first step have to be done without breakpoint checking
	while( avr->breakpoint[avr->PC] != 1 ) {
		step( avr );
	}
	printf( "stopped at breakpoint 0x%04x  after %d CPU-cycles\n", avr->PC, avr->cycles - cyclesStart ); 
}

/**
 * Toggle breakpoint for the given line
 */
void toggleBreakpoint( struct avrmcu* avr, unsigned short int line ) 
{
	avr->breakpoint[line] = avr->breakpoint[line] == 0 ? 1 : 0;
}

/* init the structure avrmcu */
struct avrmcu* create_avrmcu(void)
{
	static struct avrmcu avr; //XXX:is static correct?
	avr.registers 	= avr.sram;
	avr.ports	= avr.sram+32;
	avr.ramend          = RAMEND;
	avr.flashend        = FLASHEND;
	avr.lastinstruction = 0;
	clear_flash(&avr);

	return &avr;
}


/* Resets the avr (as if a hardware reset was done) */
void reset_avr(struct avrmcu * avr)
{
	int i;
	/*init I/O regs*/
	for (i=0;i<64;i++){
		avr->ports[i] = 0x00;
	}

	avr->PC = avr->cycles = 0;

	/*avr->SREG->I = avr->SREG.T = avr->SREG.H = avr->SREG.S =0;
	avr->SREG->V = avr->SREG->N = avr->SREG.Z = avr->SREG.C =0;
	*/
}



void clear_flash(struct avrmcu * avr)
{
	int i;
	for (i=0;i<=FLASHEND;i++) {
		avr->flash[i]=0xFFFF; //flash contains words
		avr->breakpoint[i] = 0; // no breakpoint at this line
	}
}


/* Loads <filename> into the AVR's flash
 *
 */
int load_ihex(struct avrmcu * avr, const char * filename)
{
	FILE *f;
	unsigned short int address,length,csum,mode;
	char data[2048];
	int i;
	

	if ( (f = fopen(filename,"r"))==NULL){
		printf("Error opening file %s!\n",filename);
		return 0;
	}else{
		printf("opened: %s \n",filename);
	}


	clear_flash(avr);
	
	while ((i=fscanf(f,":%2hx%4hx%2hx%s\n",&length,&address,&mode,data))!=EOF){
		if (i!=4){
			printf("Error parsing IHEX file!\n");
			return 0;
		}

		switch (mode){
			case 0:	
//				printf("address: %x  data: %s length: %d\n",address, data,length);
				/*get the csum*/
				sscanf(&data[2*length],"%2hx", &csum);
				/*cut the csum off*/
				data[2*length]='\0';
				/* write the data to the avr's flash
				 * we start at the address (from the ihex line)
				 * and write length/2 WORDS!
				 * */
				for (i=address/2;i<(address+length)/2;i++){
					unsigned short int word=0,lowbyte=0,highbyte=0;
					sscanf(&data[2*2*(i-address/2)],"%2hx%2hx",
							&lowbyte,&highbyte);
					word=lowbyte|(highbyte<<8);
//					printf("addr: %04x  h: %x, l:%x -> word:%x\n"
//					      ,i, highbyte, lowbyte,word);
					avr->flash[i]=word;
				}
				avr->lastinstruction = i > avr->lastinstruction ? i : avr->lastinstruction;
				break;
			case 1:
				//printf("IHEX end detected?\n");
				break;
			case 2:
				//printf("I dunno what to do with this -> ignored\n");
				break;
			default:
				//printf("This should never happen!!!\n");
				break;
		}
	}
	return i; //XXX:??
}



