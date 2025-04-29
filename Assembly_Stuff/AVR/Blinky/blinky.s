.equ DDB5, 0x05
.equ DDRB, 0x04
.equ PORTB5, 5 
.equ PORTB, 0x05
.equ TCNT1L, 0x84
.equ TCNT1H, 0x85
.equ OCR1AL, 0x88
.equ OCR1AH, 0x89
.equ TCCR1A, 0x80
.equ TCCR1B, 0x81
.equ CS_BITS, 0x05
.equ WGM_BITS, 0x08
.equ TIFR1, 0x16
.equ OCF1A, 0x01

.org 0x00 ;instruction start
	rjmp RESET 

RESET:
	;uinitialize pin PB5 as output
	ldi r16, (1 << DDB5) 
	out DDRB, r16	
	
	;Initialize timer in CTC mode with 1024 prescaler
	ldi r16, (CS_BITS | WGM_BITS)
	sts TCCR1B, r16

	;nothing needed for this register
	ldi r16, 0x00
	sts TCCR1A, r16	

	;ensure timer 1 set to 0
	ldi r17, 0x00
	ldi r16, 0x00
	sts TCNT1H, r17
	sts TCNT1L, r16	
	
	;set compare register
	ldi r17, 0x3D
	ldi r16, 0x09
	sts OCR1AH, r17
	sts OCR1AL, r16 

LOOP:	
	;check if flag set. Loop if not, continue if set
	sbis TIFR1, OCF1A
	rjmp LOOP	

	;reset flag by writing 1 to it (weird)
	ldi r16, (1 << OCF1A)
	out TIFR1, r16

	;toggle pin PB5
	in r17, PORTB
	ldi r16, (1 << PORTB5)
	eor r17, r16
	out PORTB, r17	

	rjmp LOOP
	
