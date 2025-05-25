

.include "inc/tn2313def.inc"

.def Temp = R25
.def row = R24
.def col = R23

.def cathode = R22
.def anode = R21
	
.def del1 = R20
.def del2 = R19
.def del3 = R18

.def bits = R17
	
.org  0x0000

init:
	sub temp, temp		; assume all pins are inputs
	out ddrb, temp		; turn off all LEDs
	


;;;	call tranlateinit	; set up translation tables.
	
.macro ldl
	ldi temp, @1
	mov @0,temp
.endmacro
	
	ldl R0,$10
	out ddrd,R0

main:
	ldl R0,0b0110			;A as dots...
	ldl R1,0b1001
	ldl R2,0b1111
	ldl R3,0b1001
	ldl R4,0b1001
;	rcall displayfield
	;rcall chaseall
	ldl R0,0b1110			;B
	ldl R1,0b1001
	ldl R2,0b1110
	ldl R3,0b1001
	ldl R4,0b1110
;	rcall displayfield
;	rcall chaseall
	ldl R0,0b0110			;C
	ldl R1,0b1001
	ldl R2,0b1000
	ldl R3,0b1001
	ldl R4,0b0110
	;rcall displayfield
	rcall chaseall
	rjmp main


		;; Generic code to chase through all the LEDs
chaseall:	
	ldi	row, 5
rowlp:				; for each row
	rcall translaterow
	ldi col, 4		; Do all columns
collp:	
	rcall translatecol
	
	sub temp, temp		; assume all pins are inputs
	out ddrb, temp		; turn off all LEDs by setting pins to inputs
	or temp, cathode	; cathode is an output
	or temp, anode		; anode is an output
	out portb, anode	; send outputs - selects a particular LED
	out ddrb, temp		; set pin directions- actually turn it on
	
	out portd, anode

	;rcall delay
	dec col			; next column
	
	brne collp
	
	dec row
	brne rowlp
	
	ret

;;; displayone
;;; light one led for the configured delay.
;;; enter with row and col set appropriately
displayone:
	rcall translaterow
	rcall translatecol
	sub temp, temp		; assume all pins are inputs
	out ddrb, temp		; turn off all LEDs
	or temp, cathode	; cathode is an output
	or temp, anode		; anode is an output
	out portb, anode	; send outputs - selects a particular LED
	out ddrb, temp		; set pin directions- actually turn it on
	rjmp delay		; delay and return
	
;;; displayfield
;;; display a field of bits at R0 and thereon (5 row of 4 LSBs.)
;;; loops fast enough to look like they're all on (?)
;;; Does this for a while :-)  (approx == delay?)
;;; 
displayfield:	
	ldi del1, 255		; how long to delay
dispflp1:		
	ldi del2, 4

dispflp2:		
	ldi ZL,4		;  turn upside down to match board
	sub ZH,ZH
	ldi row, 5		;  do five rows
fldcollp:
	rcall translaterow	
	ld bits, Z		; get one row
	ldi col, 4		;  do four columns
fldbitlp:		
	sbrs bits,3		; test bit 3
	 rjmp nextbit		;  clear, look for next one.
	rcall translatecol	; need to display it.  Get column to anode
	sub temp, temp		; assume all pins are inputs
	out ddrb, temp		; turn off all LEDs
	or temp, cathode	; cathode is an output
	or temp, anode		; anode is an output
	out portb, anode	; send outputs - selects a particular LED
	out ddrb, temp		; set pin directions- actually turn it on
nextbit:
	lsl bits		; look at next bit
	dec col
	brne fldbitlp		; maybe display next bit

	dec R30			; next row of bits from caller
	dec row			; next row to display
	brne fldcollp		; done with rows?

	dec del2		; implement delaying
	brne dispflp2
	dec del1		; loop for a while
	brne dispflp1
	ret

;;; support routines
	
;;; Translaterow
;;; translate a normal row number to an appropriate bit value in cathode.
;;; This is necessary because the LEDs are arranged to make the PCB layout
;;; easy, not to exactly match internal numbers.
;;; 
translaterow:
	ldi cathode, 0b00100
	cpi row,1
	breq doret
	ldi cathode, 0b00010
	cpi row,2
	breq doret
	ldi cathode, 0b00001
	cpi row,3
	breq doret
	ldi cathode, 0b10000
	cpi row,4
	breq doret
	ldi cathode, 0b01000	; must be?
doret:	ret	
	
translatecol:
	mov temp,col
	cp col,row
	brlt noadjust
	 inc temp
noadjust:
	ldi anode, 0b00100
	cpi temp,1
	breq doret
	ldi anode, 0b00010
	cpi temp,2
	breq doret
	ldi anode, 0b00001
	cpi temp,3
	breq doret
	ldi anode, 0b10000
	cpi temp,4
	breq doret
	ldi anode, 0b01000	; must be?
	ret


delay:      ; provides some delay so that the LED is visible
; =============================
;    delay loop
; -----------------------------
;;;	ret

	ldi  DEL1, $10

WGLOOP0:  ldi  DEL2, $10

WGLOOP1:  		ldi  DEL3, $c9

WGLOOP2:  			dec  DEL3
          			brne WGLOOP2

          	 	dec  DEL2
           		brne WGLOOP1

           dec  DEL1
           brne WGLOOP0
; -----------------------------
	ret

