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

#ifndef AVR_H
#define AVR_H

#define FLASHEND	511//1023
#define SRAMSIZE	128
#define RAMEND		SRAMSIZE+95	//96bytes for Registers and IO-ports

/*IO-Ports */
#define	SREG		0x3F
#define SP		0x3D

extern const char * const PORTS[64];


#define CFLAG		0
#define ZFLAG		1
#define NFLAG		2
#define VFLAG		3
#define SFLAG		4
#define HFLAG		5
#define TFLAG		6
#define IFLAG		7
/*
struct sreg{
	int C:1;
	int Z:1;
	int N:1;
	int V:1;
	int S:1;
	int H:1;
	int T:1;
	int I:1;
};*/
struct avrmcu* create_avrmcu(void);
int load_ihex(struct avrmcu *, const char *);
void reset_avr(struct avrmcu *);
void clear_flash(struct avrmcu *);
void toggleBreakpoint( struct avrmcu* avr, unsigned short int line );

void step(struct avrmcu* avr );
void cont( struct avrmcu* avr );


struct avrmcu{
	char name[10];
	int PC;
	long int cycles;
	unsigned int ramend;
	unsigned int flashend;
	unsigned int lastinstruction;
	/*struct sreg * sreg; this isn't kosha!*/
	unsigned char*  registers;
	unsigned char*  ports;
	unsigned char sram[RAMEND+1];
	unsigned short int flash[FLASHEND+1]; //flash is organized in words
	unsigned char breakpoint[FLASHEND+1];  // 1 means a breakpoint is set
};


extern struct avrmcu * avr;

#endif
