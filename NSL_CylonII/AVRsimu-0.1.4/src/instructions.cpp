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

struct instruction *instructions; //[INSTR_MAX];


/*
 * Register parsing, maybe this should be outsourced ?!
 *
 * */
//Get the first register d, always located here: 0000 000d dddd 0000
unsigned char get_reg1(unsigned short int opcode)
{
	return (opcode & 0x01F0)>>4;
}

//Get the second register r, always located here: 0000 00r0 0000 rrrr
unsigned char get_reg2(unsigned short int opcode)
{
	return (opcode & 0x000F) | ((opcode & 0x0200)>>5);
}

//get a "fast" register (R16-R31), located here: 0000 0000 dddd 0000
unsigned char get_immediate_reg(unsigned short int opcode)
{
	return 16 + ((opcode & 0x00F0)>>4);
}

//get an 3bit value from 0000 0000 0sss 0000
unsigned char get_3bit_value (unsigned short int opcode)
{
	return (opcode & 0x0070)>>4 ;
}


//get an 8bit value from 0000 KKKK 0000 KKKK
unsigned char get_8bit_value (unsigned short int opcode)
{
	return ((opcode & 0x0F00)>>4) | (opcode & 0x000F);
}
		
//get 0000 KKKK KKKK KKKK
unsigned short int get_12bit_value (unsigned short int opcode)
{
	return (opcode & 0x0FFF);
}

//get 0000 0KKK KKKK K000
unsigned char get_7bit_value (unsigned short int opcode)
{
	return (opcode & 0x03F8)>>3;
}

signed char get_signed_7bit_value (unsigned short int opcode)
{
	unsigned char x = get_7bit_value(opcode);
	//printf("x: 0x%02x, x-0x08: %d\n",x,(signed char)x-0x80);
	if (x & 0x40)
	{
		return (signed char) (x-0x80);
	}else{
		return x;
	}
}

//get 0000 0000 AAAA A000
unsigned char get_io_reg (unsigned short int opcode)
{
	return (opcode & 0x00F8)>>3;
}


//get 0000 0000 0000 0bbb
unsigned char get_sbit (unsigned short int opcode)
{
	return (opcode & 0x0007)>>3;
}



//get 0000 0AA0 0000 AAAA
unsigned short int get_port(unsigned short int opcode)
{
	return ((opcode & 0x0600)>>5) | (opcode & 0x000F);
}

//get 00w0 w0w0 0000 0www
unsigned short int get_offsetword(unsigned short int opcode)
{
	unsigned short int offsetword = opcode & 0x0007;
	offsetword += opcode & 0x0400 ? 0x0008 : 0;
	offsetword += opcode & 0x0800 ? 0x0010 : 0;
	offsetword += opcode & 0x2000 ? 0x0020 : 0;
	return offsetword;
}

/* 
 * Helpers
 *
 */

unsigned char set_bit(unsigned char byte, unsigned char bit, unsigned char value)
{
	if (value==0){
		return byte & ~(1<<bit);
	}else{
		return byte | (1<<bit);
	}
}

// Returns 1 if there was a borrow from bit n
// (~Rdb&Rrb) | (Rrb & Rb) | (Rb & ~Rdb)
unsigned char get_borrow_from_bit(unsigned char Rd, unsigned char Rr,
		unsigned char R, unsigned char bit)
{
	unsigned char 	Rdb= (Rd >> bit) & 0x01,
			Rrb= (Rr >> bit) & 0x01,
			Rb = (R >> bit) & 0x01;

	return (~Rdb & Rrb) | (Rrb & Rb) | (Rb & ~Rdb);
}

// Returns 1 if there was a carry from bit n
unsigned char get_carry_from_bit(unsigned char Rd, unsigned char Rr,
		unsigned char R, unsigned char bit)
{
	unsigned char 	Rdb= (Rd >> bit) & 0x01,
			Rrb= (Rr >> bit) & 0x01,
			Rb = (R >> bit) & 0x01;

	return (Rdb & Rrb) | (Rrb & ~Rb) | (~Rb & Rdb);
}



//Returns 1 if there is an overflow (substraction)
// 
unsigned char get_sub_overflow(unsigned char Rd, unsigned char Rr,
		unsigned char R, unsigned char bit)
{
	unsigned char 	Rdb= (Rd >> bit) & 0x01,
			Rrb= (Rr >> bit) & 0x01,
			Rb = (R >> bit) & 0x01;

	return (Rdb & ~Rrb & ~Rb) | (~Rdb & Rrb & Rb);
}

//Returns 1 if there is an overflow (addition)
// 
unsigned char get_add_overflow(unsigned char Rd, unsigned char Rr,
		unsigned char R, unsigned char bit)
{
	unsigned char 	Rdb= (Rd >> bit) & 0x01,
			Rrb= (Rr >> bit) & 0x01,
			Rb = (R >> bit) & 0x01;

	return (Rdb & Rrb & ~Rb) | (~Rdb & ~Rrb & Rb);
}



//will do the standard updates on the sreg for addition, given Rd, Rr and R
unsigned char get_add_sreg(unsigned char sreg,unsigned char Rd, unsigned char Rr,
		unsigned char R)
{
	unsigned char N,V;
	
	printf("R=0x%02x ",R);
	
	//H: set if there was a carry from bit 3
	sreg = set_bit(sreg,HFLAG,get_carry_from_bit(Rd, Rr, R, 3));
	//V: overflow
	V = get_add_overflow (Rd, Rr, R, 7);
	sreg = set_bit(sreg,VFLAG,V);
	//N: R7
	N = (R >> 7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S: N ^ V for signed tests
	sreg = set_bit(sreg,SFLAG,(N ^ V) );
	//Z: set if Rr and Rd are the same?
	sreg = set_bit(sreg,ZFLAG,R==0x00);
	//C:
	sreg = set_bit(sreg,CFLAG,get_carry_from_bit(Rd,Rr,R,7));

	return sreg;
}


//will do the standard updates on the sreg for substraction, given Rd, Rr and R
unsigned char get_sub_sreg(unsigned char sreg,unsigned char Rd, unsigned char Rr,
		unsigned char R)
{
	unsigned char N,V;
	
	printf("R=0x%02x ",R);
	
	//H: set if there was a borrow from bit 3
	sreg = set_bit(sreg,HFLAG,get_borrow_from_bit(Rd, Rr, R, 3));
	//V: overflow
	V = get_sub_overflow (Rd, Rr, R, 7);
	sreg = set_bit(sreg,VFLAG,V);
	//N: R7
	N = (R >> 7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S: N ^ V for signed tests
	sreg = set_bit(sreg,SFLAG,(N ^ V) );
	//Z: set if Rr and Rd are the same?
	sreg = set_bit(sreg,ZFLAG,R==0x00);
	//C:
	sreg = set_bit(sreg,CFLAG,get_borrow_from_bit(Rd,Rr,R,7));
	
	return sreg;
}


			
/* 
 * instruction handlers
 *
 */

//RJMP: 1100 kkkk kkkk kkkk
void instr_RJMP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	short int k = get_12bit_value(opcode);
	
	if (k & 0x0800){ //test if sign bit is set
		k -= 0x1000;
	}
	avr->PC+=k+1;
	avr->cycles += 2;	

	printf("RJMP %+d to PC 0x%04X\n",k,avr->PC);
}
	
//LDI:  	1110 KKKK dddd KKKK
void instr_LDI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	k=get_8bit_value(opcode); 
	unsigned char 	d = get_immediate_reg(opcode);

	//printf("opcode: 0x%x, %04x\n",opcode,opcode & 0x0F00);
	//printf("klow: 0x%x, khigh: 0x%x -> k: 0x%x\n",klow,khigh,k);
	printf("LDI R%d <- %d \n",d,k);
	avr->registers[d] = k;
	avr->cycles++;
	avr->PC++;	//increase PC
}

//LDS:  	1001 000r rrrr 0000  aaaa aaaa aaaa aaaa
void instr_LDS(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	avr->PC++;
	addr = avr->flash[ avr->PC ];
	avr->PC++;
	avr->registers[ r ] = avr->sram[ addr ];
	avr->cycles += 2;
	printf( "LDS R%d = (SRAM 0x%04x) value: 0x%02X (%d)\n"
	      , r, addr, avr->sram[ addr ], avr->sram[ addr ] );
}

//RCALL:	1101 KKKK KKKK KKKK
void instr_RCALL(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	short int k=get_12bit_value(opcode);
	
	if (k & 0x0800){ //test if sign bit is set
		k -= 0x1000;
	}
		
	//Stack <- PC+1
	//low byte first:
	avr->sram[avr->ports[SP]--] =  (avr->PC+1) & 0x00FF;
	//then the high byte:
	avr->sram[avr->ports[SP]--] =  ((avr->PC+1) & 0xFF00)>>8;
	
	avr->PC+=k+1;
	avr->cycles += 3;
	
	printf("RCALL %+d to PC 0x%04X\n",k, avr->PC);
}

//RET
void instr_RET(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int new_pc;

	//get the High byte first
	//printf("SP: 0x%x, ports[SP]: 0x%x\n",avr->ports[SP],avr->sram[avr->ports[SP]]);
	new_pc = (avr->sram[++avr->ports[SP]] <<8) | avr->sram[++avr->ports[SP]];
	
	printf("RET to PC=0x%04X\n",new_pc);

	avr->PC = new_pc;
	avr->cycles += 4;
	
}

//RETI
void instr_RETI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int new_pc;

	//get the High byte first
	//printf("SP: 0x%x, ports[SP]: 0x%x\n",avr->ports[SP],avr->sram[avr->ports[SP]]);
	new_pc = (avr->sram[++avr->ports[SP]] <<8) | avr->sram[++avr->ports[SP]];
	avr->ports[SREG] = set_bit(avr->ports[SREG],IFLAG,1);
	
	printf("RETI to PC=0x%04X\n",new_pc);

	avr->PC = new_pc;
	avr->cycles += 4;
	
}


void instr_PUSH(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int Rd=opcode & (~instructions[PUSH].mask); 
	Rd >>= 4;

	if( avr->ports[SP] > avr->ramend ) {
		printf( "\nuninitialized Stackpointer %x!\n\n",avr->ports[SP] );
		return;
	}
	printf( "pushing R%d SP=0x%04X value=0x%02X\n"
	      , Rd, avr->ports[SP], avr->registers[Rd] );
	avr->sram[avr->ports[SP]--] = avr->registers[Rd];
	avr->PC++;
	avr->cycles += 2;
}

void instr_POP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int Rd=opcode & (~instructions[POP].mask); 
	Rd >>= 4;
	if( avr->ports[SP] > avr->ramend ) {
		printf( "\nuninitialized Stackpointer %x!\n\n",avr->ports[SP] );
		return;
	}
	avr->registers[Rd] = avr->sram[++avr->ports[SP]];
	avr->PC++;
	avr->cycles += 2;
	printf( "popping R%d SP=0x%04X value=0x%02X\n"
	      , Rd, avr->ports[SP], avr->registers[Rd] );		
}

//OUT:	1011 1AAr rrrr AAAA
void instr_OUT(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int r=get_reg1(opcode);
	unsigned short int port=get_port(opcode);
	const char* szport = PORTS[port] != 0 ? PORTS[port] : "";
	
	printf( "OUT Port %s (0x%x) <- R%d  value: 0x%02X (%d)\n"
	      , szport, port, r, avr->registers[r], avr->registers[r]);

	avr->ports[port] = avr->registers[r];

	avr->PC++;
	avr->cycles++;
	
}

//IN:	1011 0AAd dddd AAAA 
void instr_IN(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int d=get_reg1(opcode);
	unsigned short int port=get_port(opcode);
	const char* szport = PORTS[port] != 0 ? PORTS[port] : "";

	printf( "IN R%d <- Port %s (0x%x)  value: 0x%02X (%d)\n"
	      , d, szport, port, avr->ports[port], avr->ports[port] );
	
	avr->registers[d] = avr->ports[port];
	
	avr->PC++;
	avr->cycles++;	
}

//MOV:
void instr_MOV(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);

	avr->registers[d] = avr->registers[r];
	printf("MOV R%d <- R%d (0x%02x)\n",d,r,avr->registers[r]);
	avr->PC++;
	avr->cycles++;	
}

//MOVW:
void instr_MOVW(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = 2* ( (opcode>>4) & 0x000F),
			r = 2* (opcode & 0x000F);

	avr->registers[d] = avr->registers[r];
	avr->registers[d+1] = avr->registers[r+1];

	printf("MOVW R%d:R%d <- R%d:R%d (0x%02x)\n",d,d+1,r,r+1,
			avr->registers[d] | (avr->registers[d+1]<<8));
	avr->PC++;
	avr->cycles++;	
}

	
//CP: Rd - Rr
void instr_CP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			R  = Rd - Rr,
			sreg = avr->ports[SREG];

	sreg = get_sub_sreg(sreg,Rd,Rr,R);
	
	printf("CP R%d (0x%02x) ==  R%d (0x%02x) Sreg = 0x%02x\n"
	      , d, avr->registers[d], r, avr->registers[r], sreg);

	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//CPC: Rd - Rr -C
void instr_CPC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd - Rr - ((sreg >> CFLAG)&0x01);

	sreg = get_sub_sreg(sreg,Rd,Rr,R);
	//in addition, Z & oldZ
	sreg = set_bit(sreg,ZFLAG, ((avr->ports[SREG]>>ZFLAG)&0x01) & 
			(sreg>>ZFLAG)&0x01);
	
	printf("CPC: Sreg = 0x%02x\n",sreg);

	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//CPI: Rd - K
void instr_CPI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_immediate_reg(opcode),
			K = get_8bit_value(opcode),
			Rd = avr->registers[d],
			R  = Rd - K,
			sreg = avr->ports[SREG];

	sreg = get_sub_sreg(sreg,Rd,K,R);
	
	printf("CPI: Sreg = 0x%02x\n",sreg);

	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//ADC
void instr_ADC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd + Rr + ((sreg >> CFLAG)&0x01);

	sreg = get_add_sreg(sreg,Rd,Rr,R);
	
	printf("ADC:R%d <- R%d + R%d + C = 0x%02x, Sreg = 0x%02x\n",d,d,r,R,sreg);

	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//SBC
void instr_SBC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd - Rr - ((sreg >> CFLAG)&0x01);

	sreg = get_sub_sreg(sreg,Rd,Rr,R);
	//in addition, Z & oldZ
	sreg = set_bit(sreg,ZFLAG, ((avr->ports[SREG]>>ZFLAG)&0x01) & 
			(sreg>>ZFLAG)&0x01);

	printf("SBC:R%d <- R%d - R%d - C = 0x%02x, Sreg = 0x%02x\n",d,d,r,R,sreg);

	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//SBCI
void instr_SBCI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_immediate_reg(opcode),
			K = get_8bit_value(opcode);
	unsigned char	Rd = avr->registers[d],
			sreg = avr->ports[SREG],
			R  = Rd - K - ((sreg >> CFLAG)&0x01);

	sreg = get_sub_sreg(sreg,Rd,K,R);
	//in addition, Z & oldZ
	sreg = set_bit(sreg,ZFLAG, ((avr->ports[SREG]>>ZFLAG)&0x01) & 
			(sreg>>ZFLAG)&0x01);

	printf("SBCI:R%d <- R%d - K (=%d) - C = 0x%02x, Sreg = 0x%02x\n",d,d,K,R,sreg);

	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}


//ADD
void instr_ADD(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd + Rr;
			
	sreg = get_add_sreg(sreg,Rd,Rr,R);
	printf("ADD:R%d <- R%d + R%d = 0x%02x, Sreg = 0x%02x\n",d,d,r,R,sreg);
	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//ADIW
void instr_ADIW(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = 24+((opcode & 0x0030)>>3),   // >>3, da eigentlich >>4 *2 ;)
			K = (opcode & 0x000F) | ((opcode & 0x00C0)>>2),
			sreg = avr->ports[SREG],
			N,V;
	unsigned char	Rdl = avr->registers[d],
			Rdh = avr->registers[d+1];
	unsigned short int 	R  = Rdl + (Rdh<<8) + K;

	/* Set SREG-FLAGS*/
	//N: R15
	N = (R>>15) & 0x01;
	sreg = set_bit(sreg,NFLAG, N);
	//V:
	V = ((Rdh>>7) & 0x01) & N;
	sreg = set_bit(sreg,VFLAG, V);
	//S:
	sreg = set_bit(sreg,SFLAG,(N ^ V) );
	//Z:
	sreg = set_bit(sreg,ZFLAG,R == 0x0000);
	//C:
	sreg = set_bit(sreg,CFLAG, ~N & ((Rdh>>7)&0x01));
	
	printf("ADIW R%d <- R%d + %d = 0x%02x, Sreg = 0x%02x\n",d,d,K,R,sreg);

	avr->registers[d] = R & 0x00FF;
	avr->registers[d+1] = (R & 0xFF00)>>16;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles += 2;
}

//AND
void instr_AND(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd & Rr,N;

	//V: 0
	sreg = set_bit(sreg,VFLAG, 0);
	//N: R7
	N = (R>>7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S:
	sreg = set_bit(sreg,SFLAG,N^0);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	
	printf("AND R%d <- R%d & R%d = 0x%02x, Sreg = 0x%02x\n",d,d,r,R,sreg);

	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//OR
void instr_OR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode);
	unsigned char	Rd = avr->registers[d],
			Rr = avr->registers[r],
			sreg = avr->ports[SREG],
			R  = Rd | Rr,N;

	//V: 0
	sreg = set_bit(sreg,VFLAG, 0);
	//N: R7
	N = (R>>7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S:
	sreg = set_bit(sreg,SFLAG,N^0);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	
	printf("OR R%d <- R%d | R%d = 0x%02x, Sreg = 0x%02x\n",d,d,r,R,sreg);

	avr->registers[d]=R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//ANDI
void instr_ANDI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_immediate_reg(opcode),
			K = get_8bit_value(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R  = Rd & K,N;

	//V: 0
	sreg = set_bit(sreg,VFLAG, 0);
	//N: R7
	N = (R>>7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S:
	sreg = set_bit(sreg,SFLAG,N^0);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	
	printf("ANDI R%d <- R%d & 0x%02x, Sreg = 0x%02x\n",d,d,K,sreg);

	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//ORI
void instr_ORI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_immediate_reg(opcode),
			K = get_8bit_value(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R  = Rd | K,N;

	//V: 0
	sreg = set_bit(sreg,VFLAG, 0);
	//N: R7
	N = (R>>7) & 0x01;
	sreg = set_bit(sreg,NFLAG,N);
	//S:
	sreg = set_bit(sreg,SFLAG,N^0);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	
	printf("ORI:R%d <- R%d & 0x%02x, Sreg = 0x%02x\n",d,d,K,sreg);

	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//ASR
void instr_ASR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R,N,C,V;

	
	//First save Rd0 in C
	sreg = set_bit(sreg,CFLAG, C = Rd & 0x01);
	//Now do the shift
	R = (Rd & 0x80) | (Rd>>1);
	//N: R7
	sreg = set_bit(sreg,NFLAG, N = (R>>7) & 0x01);
	//V: N ^ C
	sreg = set_bit(sreg,VFLAG, V = N ^ C);
	//S:
	sreg = set_bit(sreg,SFLAG, N^V);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	printf("ASR R%d = 0x%02x, Sreg = 0x%02x\n",d,R,sreg);
	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//BCLR
void instr_BCLR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	s = (opcode & 0x0070)>>4;
	unsigned char	sreg = avr->ports[SREG];
			
	
	sreg = set_bit(sreg,s, 0);
	printf("BCLR: SREG(%d), Sreg = 0x%02x\n",s,sreg);
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//BLD
void instr_BLD(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			b = opcode & 0x07;
	
	
	avr->registers[d] = set_bit(avr->registers[d], b, (avr->ports[SREG]>>TFLAG) & 0x01);
	printf("BLD: R%d(%d) <- T\n",d,b);

	avr->PC++;
	avr->cycles++;
}

//BST
void instr_BST(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			b = opcode & 0x07;
	
	avr->ports[SREG] = set_bit(avr->ports[SREG], TFLAG, (avr->registers[d]>>b) & 0x01);	
	printf("BST: T <- R%d(%d)\n",d,b);

	avr->PC++;
	avr->cycles++;
}


//BRBC, Branch if Bit in SREG is Cleared
void instr_BRBC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	signed char 	k = get_signed_7bit_value(opcode);
	unsigned char	s = opcode & 0x07;
	
	//printf("sreg=0x%02x, s=%d, k=%d\n",avr->ports[SREG],s,k);
	if ((avr->ports[SREG] & 0x01<<s)==0){
		printf("BRBC: Branch to PC=0x%04X\n",avr->PC+k+1);
		avr->PC = avr->PC + k + 1;
		avr->cycles+=2;
	}else{
		printf("BRBC: no branch\n");
		avr->PC++;
		avr->cycles++;

	}
}

//BRBC, Branch if Bit in SREG is Set
void instr_BRBS(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	signed char 	k = get_signed_7bit_value(opcode);
	unsigned char	s = opcode & 0x07;
	
	//X printf("sreg=0x%02x, s=%d, k=%d\n",avr->ports[SREG],s,k);
	if ((avr->ports[SREG] & (0x01<<s))!=0){
		printf("BRBS SREG bit%d: Branch to PC=0x%04X\n", s, avr->PC+k+1);
		avr->PC = avr->PC + k + 1;
		avr->cycles+=2;
	}else{
		printf("BRBS SREG bit%d, no branch\n", s);
		avr->PC++;
		avr->cycles++;

	}
}

//BSET
void instr_BSET(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	s = (opcode & 0x0070)>>4;
	unsigned char	sreg = avr->ports[SREG];
			
	sreg = set_bit(sreg,s, 1);
	printf("BSET: SREG(%d), Sreg = 0x%02x\n",s,sreg);
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//CBI
void instr_CBI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	b = get_sbit(opcode),
			A = get_io_reg(opcode);
			
	avr->ports[A] = set_bit(avr->ports[A], b, 0);
	printf("CBI: I/O(%d,%d) <- 0\n",A,b);

	avr->PC++;
	avr->cycles +=2;
}

//SBI
void instr_SBI(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	b = get_sbit(opcode),
			A = get_io_reg(opcode);
			
	avr->ports[A] = set_bit(avr->ports[A], b, 1);
	printf("SBI: I/O(%d,%d) <- 1\n",A,b);

	avr->PC++;
	avr->cycles +=2;
}


//COM
void instr_COM(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R,N,V;
	
	R=0xFF - Rd;
	//C:1
	sreg = set_bit(sreg,CFLAG, 1);
	//N: R7
	sreg = set_bit(sreg,NFLAG, N = (R>>7) & 0x01);
	//V: 0
	sreg = set_bit(sreg,VFLAG, V = 0);
	//S:
	sreg = set_bit(sreg,SFLAG, N^V);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	printf("COM:R%d = 0x%02x, Sreg = 0x%02x\n",d,R,sreg);
	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//CPSE - compare and skip the next instruction if Equal
void instr_CPSE(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode),
			Rd = avr->registers[d],
			Rr = avr->registers[r];
	
	if (Rd == Rr){
		avr->PC += size + getInstructionSize( avr->flash[avr->PC + size] );
		avr->cycles += 2;
		printf("CPSE: skipped one instruction\n");
	}else{
		avr->PC ++;
		avr->cycles++;
		printf("CPSE: Register %d and %d not equal\n", d, r);
	}
}

//DEC
void instr_DEC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			Rd = avr->registers[d],
			R = Rd - 1,
			sreg = avr->ports[SREG],N,V;
	
	//V: sub_overflow
	sreg = set_bit(sreg,VFLAG, V = (R == 0x7F)); //or Rd==0x80
	sreg = set_bit(sreg,NFLAG, N = (R >> 7)&0x01);
	sreg = set_bit(sreg,SFLAG, N ^ V);
	sreg = set_bit(sreg,ZFLAG, R == 0x00);
	
	printf("DEC: R%d=0x%02x, sreg= 0x%02x\n",d,R,sreg);
	
	avr->ports[SREG] = sreg;
	avr->registers[d] = R;
	
	avr->PC ++;
	avr->cycles++;
}

//EOR
void instr_EOR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			r = get_reg2(opcode),
			Rd = avr->registers[d],
			Rr = avr->registers[r],
			R = Rd ^ Rr,
			sreg = avr->ports[SREG],N;
	
	//V: sub_overflow
	sreg = set_bit(sreg,VFLAG, 0); 
	sreg = set_bit(sreg,NFLAG, N = (R >> 7)&0x01);
	sreg = set_bit(sreg,SFLAG, N ^ 0);
	sreg = set_bit(sreg,ZFLAG, R == 0x00);
	
	printf("EOR: R%d <- R%d ^ R%d = 0x%02x, sreg= 0x%02x\n",d,d,r,R,sreg);
	
	avr->ports[SREG] = sreg;
	avr->registers[d] = R;
	
	avr->PC ++;
	avr->cycles++;
}

//IJMP
void instr_IJMP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int addr;
	addr = (avr->registers[31]<<8) | avr->registers[30];
	avr->PC = addr;
	avr->cycles += 2;
	printf("IJMP to addr 0x%04X\n", addr);
}

//INC
void instr_INC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode),
			Rd = avr->registers[d],
			R = Rd + 1,
			sreg = avr->ports[SREG],N,V;
	
	//V: sub_overflow
	sreg = set_bit(sreg,VFLAG, V = (R == 0x80)); //or Rd==0x7F
	sreg = set_bit(sreg,NFLAG, N = (R >> 7)&0x01);
	sreg = set_bit(sreg,SFLAG, N ^ V);
	sreg = set_bit(sreg,ZFLAG, R == 0x00);
	
	printf("INC: R%d=0x%02x, sreg= 0x%02x\n",d,R,sreg);
	
	avr->ports[SREG] = sreg;
	avr->registers[d] = R;
	
	avr->PC ++;
	avr->cycles++;
}

//LDD Load Indirect Data X or Y register with Displacement
void instr_LDD(struct avrmcu *avr, unsigned short int opcode, unsigned short int size) 
{
	unsigned char      d          = get_reg1( opcode );
	unsigned short int offsetword =	get_offsetword( opcode );
	unsigned short int r          = opcode & 0x0008 ? 28 : 30;
	unsigned short int addr;

	addr = (avr->registers[r+1]<<8) | avr->registers[r];
	addr+=offsetword;
	avr->registers[d] = avr->sram[addr];

	printf( "LDD R%d,%c+%d (SRAM 0x%04X)\n",d,opcode & 0x0008 ? 'Y' : 'Z', offsetword, addr );
	avr->PC ++;
	avr->cycles += 2;
}

//LPM with no operands (tested, seems to work)
void instr_LPM(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned short int address,data;
	
	address = (avr->registers[31]<<8) | avr->registers[30];
	data = avr->flash[address/2];
	if (address%2 == 1){
		avr->registers[0] = (data>>8) & 0x00FF;
	}else{
		avr->registers[0] = data & 0x00FF;
	}
		
	printf("LPM: R0 <- (Z), R0 = 0x%02x = %c\n",avr->registers[0],avr->registers[0]);
	
	avr->PC ++;
	avr->cycles +=3;
}

//LPMZ with Rd, Z(+) (tested.)
void instr_LPMZ(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char		d = get_reg1(opcode);
	unsigned short int 	address,data;
	
	address = (avr->registers[31]<<8) | avr->registers[30];
	data = avr->flash[address/2];
	if (address%2 == 1){
		avr->registers[d] = (data>>8) & 0x00FF;
	}else{
		avr->registers[d] = data & 0x00FF;
	}
		
	printf("LPM: R%d <- (Z), R%d = 0x%02x = %c\n",d,d,
			avr->registers[d],avr->registers[d]);
	
	if ((opcode & 0x0001) == 1){
		if (avr->registers[30]++ == 0)  //if ZL flows over...
			avr->registers[31]++;	//...increment ZH	
	}
	
	avr->PC ++;
	avr->cycles +=3;
}

//LSR
void instr_LSR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R,C,V;

	
	//First save Rd0 in C
	sreg = set_bit(sreg,CFLAG, C = Rd & 0x01);
	//Now do the shift
	R = (Rd>>1) & 0x7F;		
	//N: 0
	sreg = set_bit(sreg,NFLAG, 0);
	//V: N ^ C
	sreg = set_bit(sreg,VFLAG, V = 0 ^ C);
	//S:
	sreg = set_bit(sreg,SFLAG, 0^V);
	//Z:
	sreg = set_bit(sreg,ZFLAG, R==0x00);
	
	printf("LSR:R%d = 0x%02x, Sreg = 0x%02x\n",d,R,sreg);
	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}


//NEG
void instr_NEG(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char 	d = get_reg1(opcode);
	unsigned char	sreg = avr->ports[SREG],
			Rd = avr->registers[d],
			R = 0x00 - Rd,
			N,V;

	sreg = set_bit(sreg,HFLAG, (R>>3) | (Rd>>3) );
	sreg = set_bit(sreg,VFLAG, V = (R==0x80));
	sreg = set_bit(sreg,NFLAG, N = (R>>7) & 0x01);
	sreg = set_bit(sreg,SFLAG, V^N);
	sreg = set_bit(sreg,ZFLAG, R==0);
	sreg = set_bit(sreg,CFLAG, R!=0);

	printf("NEG:R%d = 0x%02x, Sreg = 0x%02x\n",d,R,sreg);
	avr->registers[d]= R;
	avr->ports[SREG] = sreg;

	avr->PC++;
	avr->cycles++;
}

//NOP
void instr_NOP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	
	printf("NOP\n");
	avr->PC++;
	avr->cycles++;
}

//ROR
void instr_ROR(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	d = get_reg1(opcode),
			sreg = avr->ports[SREG],
			N,V,C = (sreg>>CFLAG) & 0x01,
			Rd = avr->registers[d],
			R
			;
	//Rd0 -> C
	sreg = set_bit(sreg, CFLAG, Rd & 0x01);		
	R = Rd >> 1;
	//R7 <- C
	R = R | (C<<7);
	
	sreg = set_bit(sreg, NFLAG, N = (R>>7)&0x01);
	sreg = set_bit(sreg, VFLAG, V = N ^ C);
	sreg = set_bit(sreg, SFLAG, N ^ V);
	sreg = set_bit(sreg, ZFLAG, R == 0);

	avr->ports[SREG] = sreg;
	avr->registers[d] = R;
	
	printf("ROR: R%d = 0x%02x, sreg= 0x%02x\n",d,R,sreg);
	avr->PC++;
	avr->cycles++;
}

//SBIC
void instr_SBIC(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	A = get_io_reg(opcode),
			b = get_sbit(opcode);
			
	if ( ((avr->ports[A]>>b)&0x01) == 0){
		avr->PC += size + getInstructionSize( avr->flash[avr->PC + size] );
		avr->cycles += 2;
		printf("SBIC: I/O (0x%02x,%d) == 0 -> skipped 1 instruction\n",
				A, b );
	}else{
		avr->PC++;
		avr->cycles++;
		printf("SBIC: I/O (0x%02x,%d) == 1 -> didn't skip\n",
				A,b);
	}
}

//SBIS
void instr_SBIS(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	A = get_io_reg(opcode),
			b = get_sbit(opcode);
			
	if ( ((avr->ports[A]>>b)&0x01) == 1){
		avr->PC += size + getInstructionSize( avr->flash[avr->PC + size] );
		avr->cycles += 2;
		printf("SBIS: I/O (0x%02x,%d) == 1 -> skipped 1 instruction\n",
				A,b);
	}else{
		avr->PC++;
		avr->cycles++;
		printf("SBIS: I/O (0x%02x,%d) == 0 -> didn't skip\n",
				A,b);
	}
}

//STD Store Indirect Data X or Y register with Displacement
void instr_STD(struct avrmcu *avr, unsigned short int opcode, unsigned short int size) 
{
	unsigned char      d          = get_reg1( opcode );
	unsigned short int offsetword =	get_offsetword( opcode );
	unsigned short int r          = opcode & 0x0008 ? 28 : 30;
	unsigned short int addr;

	addr = (avr->registers[r+1]<<8) | avr->registers[r];
	addr+=offsetword;
	avr->sram[addr] = avr->registers[d];

	printf( "STD %c+%d (SRAM 0x%04X), R%d (value: 0x%02X)\n"
	      , opcode & 0x0008 ? 'Y' : 'Z', offsetword, addr, d, avr->sram[addr] );
	avr->PC ++;
	avr->cycles += 2;
}

//STS:	1001 001r rrrr 0000  aaaa aaaa aaaa aaaa
void instr_STS(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	avr->PC++;
	addr = avr->flash[ avr->PC ];
	avr->PC++;
	avr->sram[ addr ] = avr->registers[ r ];
	avr->cycles += 2;
	printf("STS: SRAM 0x%04x = R%d value: 0x%02X (%d)\n"
		  , addr,r, avr->registers[ r ], avr->registers[ r ]);
}

//ST_X:	1001 001r rrrr 1100 
void instr_ST_X(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned int addr = 0;
	addr = (avr->registers[27]<<8) + avr->registers[26];
	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *X out of available SRAM!");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];
	avr->cycles += 2;
	printf("ST X(SRAM 0x%04x), R%d: \n", addr,r);
}

//ST_XP:1001 001r rrrr 1101 
void instr_ST_XP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	addr = (avr->registers[27]<<8) + avr->registers[26];
	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *X out of available SRAM!");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];

	avr->cycles += 2;
	printf("ST X+(SRAM 0x%04x), R%d: \n", addr,r);
	
	/* do the post increment */
	addr++;
	avr->registers[26] = addr & 0x00ff;
	avr->registers[27] = (addr >> 8) & 0x00ff;
}

//ST_XP:1001 001r rrrr 1110
void instr_ST_MX(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;

	addr = (avr->registers[27]<<8) + avr->registers[26];

	/* do the pre decrement */
	addr--;
	avr->registers[26] = addr & 0x00ff;
	avr->registers[27] = (addr >> 8) & 0x00ff;
	
	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *X out of available SRAM!");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];
	
	avr->cycles += 2;
	printf("ST -X(SRAM 0x%04x), R%d: \n", addr,r);
}

//ST_YP:1001 001r rrrr 1001
void instr_ST_YP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	addr = (avr->registers[29]<<8) + avr->registers[28];
	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *Y out of available SRAM!");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];

	avr->cycles += 2;
	printf("ST Y+(SRAM 0x%04x), R%d: \n", addr,r);

	/* do the post increment */
	addr++;
	avr->registers[28] = addr & 0x00ff;
	avr->registers[29] = (addr >> 8) & 0x00ff;
}

//ST_MY:1001 001r rrrr 1010
void instr_ST_MY(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	addr = (avr->registers[29]<<8) + avr->registers[28];

	/* do the pre decrement */
	addr--;
	avr->registers[28] = addr & 0x00ff;
	avr->registers[29] = (addr >> 8) & 0x00ff;

	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *Y out of available SRAM!");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];
	avr->cycles += 2;
	printf("ST -Y(SRAM 0x%04x), R%d: \n", addr,r);
}

//ST_ZP:1001 001r rrrr 0001
void instr_ST_ZP(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	addr = (avr->registers[31]<<8) + avr->registers[30];
	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *Z out of available SRAM!\n");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];

	avr->cycles += 2;
	printf("ST Z+(SRAM 0x%04x), R%d: \n", addr,r);

	/* do the post increment */
	addr++;
	avr->registers[30] = addr & 0x00ff;
	avr->registers[31] = (addr >> 8) & 0x00ff;
}

//ST_MZ:1001 001r rrrr 0010
void instr_ST_MZ(struct avrmcu *avr, unsigned short int opcode, unsigned short int size)
{
	unsigned char	   r    = get_reg1(opcode);
	unsigned short int addr = 0;
	addr = (avr->registers[31]<<8) + avr->registers[30];

	/* do the pre decrement */
	addr--;
	avr->registers[30] = addr & 0x00ff;
	avr->registers[31] = (addr >> 8) & 0x00ff;

	avr->PC++;
	if( addr > avr->ramend ) {
		printf("ERROR: *Z out of available SRAM!\n");
		return;
	}
	avr->sram[ addr ] = avr->registers[ r ];
	avr->cycles += 2;
	printf("ST -Z(SRAM 0x%04x), R%d: \n", addr,r);
}


/* this function initializes the instruction-array.
 * To do this, 3 steps are done for each instruction:
 * 1) set the instruction mask (which masks out the data bits)
 * 2) set the instruction code (opcode with all data bits set to 0)
 * 3) set up the function pointer (to the function that handles the instruction)
 */

void set_instr(struct instruction* instructions, int instr_num,
		unsigned short int mask, unsigned short int code, 
		void (*function_ptr) (struct avrmcu*, unsigned short int, unsigned short int))
{
	instructions[instr_num].mask=mask;
	instructions[instr_num].code=code;
	instructions[instr_num].size=1;
	instructions[instr_num].function_ptr = function_ptr;
}

/**
 * Initializes 2-word instructions
 */
void set_instr2(struct instruction* instructions, int instr_num,
		unsigned short int mask, unsigned short int code, 
		void (*function_ptr) (struct avrmcu*, unsigned short int, unsigned short int))
{
	instructions[instr_num].mask=mask;
	instructions[instr_num].code=code;
	instructions[instr_num].size=2;
	instructions[instr_num].function_ptr = function_ptr;
}


/**
 * @return size of the given machine instruction.
 */
unsigned short int getInstructionSize( unsigned short int opcode ) {
	int i;
	for (i=0;i<INSTR_MAX;i++){
		if ((opcode & instructions[i].mask) == instructions[i].code){
			return instructions[i].size;
		}
	}
	return 0;
}


struct instruction* init_instructions_array(void){
	static struct instruction instructions[INSTR_MAX];
		
	//RJMP: 1100 kkkk kkkk kkkk
	set_instr(instructions, RJMP, 0xF000, 0xC000, &instr_RJMP);

	//LDI:  1110 KKKK dddd KKKK
	set_instr(instructions, LDI, 0xF000, 0xE000, &instr_LDI);

	//LDS:  1001 000r rrrr 0000
	set_instr(instructions, LDS, 0xFE0F, 0x9000, &instr_LDS);

	//RCALL:1101 KKKK KKKK KKKK
	set_instr(instructions, RCALL, 0xF000, 0xD000, &instr_RCALL);

	//RET:	1001 0101 0000 1000
	set_instr(instructions, RET, 0xFFFF, 0x9508, &instr_RET);
	
	//RETI: 1001 0101 0001 1000
	set_instr(instructions, RETI, 0xFFFF, 0x9518, &instr_RETI);

	//PUSH:	1001 001d dddd 1111 
	set_instr(instructions, PUSH, 0xFE0F, 0x920F, &instr_PUSH);

	//POP:	1001 000d dddd 1111 
	set_instr(instructions,POP, 0xFE0F, 0x900F, &instr_POP);

	//OUT:	1011 1AAr rrrr AAAA 
	set_instr(instructions, OUT, 0xF800, 0xB800, &instr_OUT);

	//IN:	1011 0AAd dddd AAAA 
	set_instr(instructions, IN, 0xF800, 0xB000, &instr_IN );

	//MOV:	0010 11rd dddd rrrr
	set_instr(instructions, MOV, 0xF800, 0x2C00, &instr_MOV);

	//MOVW:	0000 0001 dddd rrrr
	set_instr(instructions, MOVW, 0xFF00, 0x0100, &instr_MOVW);
	
	//CP:	0001 01rd dddd rrrr 
	set_instr(instructions, CP, 0xFC00, 0x1400, &instr_CP);

	//ADC:	0001 11rd dddd rrrr 
	set_instr(instructions, ADC, 0xFC00, 0x1C00,  &instr_ADC);

	//ADD:	0000 11rd dddd rrrr 
	set_instr(instructions, ADD, 0xFC00, 0x0C00, &instr_ADD);

	//ADIW:	1001 0110 KKdd KKKK 
	set_instr(instructions, ADIW, 0xFF00, 0x9600, &instr_ADIW);

	//AND:	0010 00rd dddd rrrr 
	set_instr(instructions, AND, 0xFC00, 0x2000, &instr_AND);

	//ANDI:	0111 KKKK dddd KKKK 
	set_instr(instructions, ANDI, 0xF000, 0x7000, &instr_ANDI);

	//ASR:	1001 010d dddd 0101
	set_instr(instructions, ASR, 0xFE0F, 0x9405, &instr_ASR);

	//BCLR:	1001 0100 1sss 1000
	set_instr(instructions, BCLR, 0xFF8F, 0x9488, &instr_BCLR);

	//BSET:	1001 0100 0sss 1000
	set_instr(instructions, BSET, 0xFF8F, 0x9408, &instr_BSET);

	//BLD:	1111 100d dddd 0bbb
	set_instr(instructions, BLD, 0xFE08, 0xF800, &instr_BLD);

	//BST:	1111 101d dddd 0bbb
	set_instr(instructions, BST, 0xFE08, 0xF800, &instr_BST);

	//BRBC:	1111 01kk kkkk ksss
	set_instr(instructions, BRBC, 0xFC00, 0xF400, &instr_BRBC);

	//BRBS:	1111 00kk kkkk ksss
	set_instr(instructions, BRBS, 0xFC00, 0xF000, &instr_BRBS);

	//CBI: 	1001 1000 AAAA Abbb
	set_instr(instructions, CBI, 0xFF00, 0x9800, &instr_CBI);
	
	//SBI: 	1001 1010 AAAA Abbb
	set_instr(instructions, SBI, 0xFF00, 0x9800, &instr_SBI);
	
	//COM:	1001 010d dddd 0000
	set_instr(instructions, COM, 0xFE0F, 0x9400, &instr_COM);
	
	//CPC:	0000 01rd dddd rrrr
	set_instr(instructions, CPC, 0xFC00, 0x0400, &instr_CPC);
	
	//CPI:	0011 KKKK dddd KKKK
	set_instr(instructions, CPI, 0xF000, 0x3000, &instr_CPI);

	//CPSE:	0001 00rd dddd rrrr
	set_instr(instructions, CPSE, 0xFC00, 0x1000, &instr_CPSE);

	//DEC: 	1001 010d dddd 1010
	set_instr(instructions, DEC, 0xFE0F, 0x940A, &instr_DEC);
	
	//EOR:	0010 01rd dddd rrrr
	set_instr(instructions, EOR, 0xFC00, 0x2400, &instr_EOR);

	//IJMP:	1001 0100 0000 1001
	set_instr(instructions, IJMP, 0xFFFF, 0x9409, &instr_IJMP);

	//INC: 	1001 010d dddd 0011
	set_instr(instructions, INC, 0xFE0F, 0x9403, &instr_INC);

	//LDD: 	
	set_instr(instructions, LDD, 0xD200, 0x8000, &instr_LDD);

	//LPM: 	1001 0101 1100 1000
	set_instr(instructions, LPM, 0xFFFF, 0x95C8, &instr_LPM);

	//LPMZ:	1001 000d dddd 010x if (x==1) {Z postincrement}
	set_instr(instructions, LPMZ, 0xFE0E, 0x9004, &instr_LPMZ);

	//LSR: 	1001 010d dddd 0110
	set_instr(instructions, LSR, 0xFE0F, 0x9405, &instr_LSR);

	//NEG: 	1001 010d dddd 0001
	set_instr(instructions, NEG, 0xFE0F, 0x9401, &instr_NEG);

	//NOP: 	0000 0000 0000 0000
	set_instr(instructions, NOP, 0xFFFF, 0x0000, &instr_NOP);
	
	//OR:	0010 10rd dddd rrrr
	set_instr(instructions, OR, 0xFC00, 0x2800, &instr_OR);

	//ORI:	0110 KKKK dddd KKKK
	set_instr(instructions, ORI, 0xF000, 0x6000, &instr_OR);
	
	//RETI: 1001 0101 0001 1000
	set_instr(instructions, RETI, 0xFFFF, 0x9518, &instr_RETI);

	//ROR: 	1001 010d dddd 0111
	set_instr(instructions, ROR, 0xFE0F, 0x9407, &instr_ROR);

	//SBC: 	0000 10rd dddd rrrr
	set_instr(instructions, SBC, 0xFC00, 0x0800, &instr_SBC);

	//SBCI:	0100 KKKK dddd KKKK
	set_instr(instructions, SBCI, 0xF000, 0x4000, &instr_SBCI);

	//SBIC:	1001 1001 AAAA Abbb
	set_instr(instructions, SBIC, 0xFF00, 0x9900, &instr_SBIC);

	//SBIS:	1001 1011 AAAA Abbb
	set_instr(instructions, SBIS, 0xFF00, 0x9B00, &instr_SBIS);

	//STD:	
	set_instr(instructions, STD, 0xD200, 0x8200, &instr_STD);

	//STS:	1001 001r rrrr 0000  aaaa aaaa aaaa aaaa
	set_instr2(instructions, STS, 0xFE0F, 0x9200, &instr_STS);

	//ST_X:	1001 001r rrrr 1100 
	set_instr(instructions, ST_X, 0xFE0F, 0x920C, &instr_ST_X);

	//ST_XP:1001 001r rrrr 1101 
	set_instr(instructions, ST_XP, 0xFE0F, 0x920D, &instr_ST_XP);

	//ST_MX:1001 001r rrrr 1110 
	set_instr(instructions, ST_MX, 0xFE0F, 0x920E, &instr_ST_MX);

	//ST_YP:1001 001r rrrr 1001
	set_instr(instructions, ST_YP, 0xFE0F, 0x9209, &instr_ST_YP);

	//ST_MY:1001 001r rrrr 1010
	set_instr(instructions, ST_MY, 0xFE0F, 0x920A, &instr_ST_MY);

	//ST_ZP:1001 001r rrrr 0001
	set_instr(instructions, ST_ZP, 0xFE0F, 0x9201, &instr_ST_ZP);

	//ST_MZ:1001 001r rrrr 0010
	set_instr(instructions, ST_MZ, 0xFE0F, 0x9202, &instr_ST_MZ);
	
	return instructions;
};
