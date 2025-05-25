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

int load_ihex (struct avrmcu * avr, const char * filename);
void printHelp();
void print_registers( struct avrmcu* avr );
void set_register( struct avrmcu* avr );
void print_sram(struct avrmcu * avr);
void set_sram(struct avrmcu * avr);
void print_flash(struct avrmcu * avr);
void set_flash(struct avrmcu * avr);
void print_breakpoints( struct avrmcu * avr);
void set_breakpoint( struct avrmcu * avr);
int run(struct avrmcu * avr);

int main(int argc, const char* argv[])
{
	struct avrmcu * avr;
	int    bCont = 1;
	unsigned char   cCmd  = 0;
	
	avr = create_avrmcu();
	instructions = init_instructions_array();	
	//init_opcodes();
	
	if( argc != 2 ) {
		printf( "usage: %s hexfilename\n", argv[0] );
		return 0;
	}	
	clear_flash(avr);
	load_ihex(avr, argv[1]);
	//print_flash(&avr);
	
	reset_avr(avr);
	//print_sram(&avr);
	
	while( bCont ) {
		cCmd = getc( stdin );
		switch( cCmd) {
			case '?': 
				printHelp();
				break;
			case 's':
				print_sram(avr);
				break;
			case 'S':
				set_sram(avr);
				break;
			case 'r':
				print_registers(avr);
				break;
			case 'R':
				set_register(avr);
				break;
			case 'f':
				print_flash(avr);
				break;
			case 'F':
				set_flash(avr);
				break;
			case 'n':
				step(avr);
				break;
			case 'c':
				cont(avr);
				break;
			case 'b':
				print_breakpoints(avr);
				break;
			case 'B':
				set_breakpoint(avr);
				break;
			case 'x':
				bCont=0;
				break;
		}
	}
	
	printf("\n");
	
}

void printHelp() {
	printf( "?   help\n");
	printf( "s   show Sram\n");
	printf( "S   set value in sram Sram\n");
	printf( "r   show Registers\n");
	printf( "R   set Registers value\n");
	printf( "f   show Flash\n");
	printf( "F   set value in Flash\n");
	printf( "n   Next step\n");
	printf( "c   Continue execution till next breakpoint\n");
	printf( "b   show Breakpoints\n");
	printf( "B   toggle Breakpoint\n");
	printf( "x   eXit\n");
}


void print_sram(struct avrmcu * avr)
{
	unsigned int i;
	printf( " SRAM:    +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F" );
	for (i=0;i<=RAMEND;i++){
		if (i%16==0){
			printf("\n %06x: ",i);
		}
		printf(" %02x",avr->sram[i]);
	}
	printf("\n");
}

void set_sram(struct avrmcu * avr) 
{
	long int addr = -1;
	int value = -1;
	printf( "Enter SRAM address to change in hex: " );
	scanf( "%x", &addr );
	if( addr == -1 || addr > avr->ramend ) {
		printf( "\nERROR: wrong SRAM address %d!\n\n", addr );
		return;
	}
	printf( "Old Value of SRAM[0x%04X]: 0x%04X - insert new value in hex: ", addr, avr->sram[addr] );
	scanf( "%x", &value );
	if( value == -1 || value > 0xff) {
		printf( "\nERROR: wrong value %d for SRAM[0x%04X]!\n\n", value, addr );
		return;
	}		
	avr->sram[addr] = value;
}


void print_registers( struct avrmcu* avr ) 
{
	unsigned int i;
	for (i=0;i<=31;i++){
		printf( "R%02d =%02X(%03d)%c", i, avr->sram[i], avr->sram[i], (i+1)%8?' ':'\n' );
	}
	{
	unsigned short int x = (avr->registers[27]<<8) | avr->registers[26];
	unsigned short int y = (avr->registers[29]<<8) | avr->registers[28];
	unsigned short int z = (avr->registers[31]<<8) | avr->registers[30];
	printf( "X: %04X (%d)   Y: %04X (%d)   Z: %04X (%d)  SREG: 0x%02X  SP:0x%04X\n"
	      ,  x, x, y, y, z, z, avr->ports[SREG], avr->ports[SP] );
	}
}

void set_register( struct avrmcu* avr ) 
{
	int reg = -1;
	int value = -1;
	printf( "Enter Register to change: " );
	scanf( "%i", &reg );
	if( reg == -1 || reg > 31 ) {
		printf( "\nERROR: wrong register number %d!\n\n", reg );
		return;
	}
	printf( "Old Value of R%02d: %02X - insert new value in hex: ", reg, avr->registers[reg] );
	scanf( "%x", &value );
	if( value == -1 || value > 0xff) {
		printf( "\nERROR: wrong value %X for Register %d!\n\n", value, reg );
		return;
	}		
	avr->registers[reg] = value;
}

void print_flash(struct avrmcu * avr)
{
	unsigned int i;
	for (i=0;i<=avr->lastinstruction;i++){
		if (i%8==0){
			printf("\n %06x: ",i);
		}
		printf(" %04x",avr->flash[i]);
	}
	printf("\n");
}

void set_flash(struct avrmcu * avr) 
{
	long int addr = -1;
	long int value = -1;
	printf( "Enter Flash address to change in hex: " );
	scanf( "%x", &addr );
	if( addr == -1 || addr > avr->flashend ) {
		printf( "\nERROR: wrong flash address %d!\n\n", addr );
		return;
	}
	printf( "Old Value of FLASH[%04X]: %04X - insert new value in hex: ", addr, avr->flash[addr] );
	scanf( "%x", &value );
	if( value == -1 || value > 0xffff) {
		printf( "\nERROR: wrong value %X for Flash!\n\n", value );
		return;
	}		
	avr->flash[addr] = value;
}

/**
 * Print all given breakpoints
 */
void print_breakpoints( struct avrmcu * avr) 
{
	unsigned int i;
	for(i=0;i<=avr->lastinstruction;i++){
		if( avr->breakpoint[i] == 1 ){
			printf("breakpoint at addr 0x%04x - instruction: 0x%04x\n", i, avr->flash[i] );
		}
	}
	printf("\n");
}
	
/**
 * toggle breakpoint at the given line
 */
void set_breakpoint( struct avrmcu * avr) 
{
	int addr = -1;
	printf( "enter breakpoint address in hex: " );
	scanf( "%X", &addr );
	if( addr != -1 ) {
		if( addr > avr->lastinstruction ) {
			printf( "\nERROR! given address behind last instruction!\n\n" );
		} else {
			printf( "toggle breakpoint at addr 0x%04X\n", addr );
			toggleBreakpoint( avr, addr );
		}
	}
}

