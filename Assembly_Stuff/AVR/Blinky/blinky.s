.include "constants.inc"

.section .text
.global __start

.org 0x0000 ;RESET interrupt vector
	rjmp __start 

.org 0x002C ;TIMER1 COMPA Interrupt Register. Manual gives word addresses not byte addresses
	rjmp PB5_TOGGLE

__start:
	;initialize stack
	ldi r16, RAMENDH
	out SPH, r16
	ldi r16, RAMENDL
	out SPL, r16	

	sei ;Initialize global interrupts

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

	;enable TIMER1_COMPA Interrupt
	ldi r16, (1 << OCIE1A)
	sts TIMSK1, r16 
	
	ldi r16, (1 << SE)
	out SMCR, r16	

	;start LED in on
	ldi r16, (1 << PORTB5)
	out PORTB, r16

LOOP:
	sleep
	rjmp LOOP ;OCRA1 interrupt controls flow

PB5_TOGGLE:
	;toggle pin PB5
	in r17, PORTB
	ldi r16, (1 << PORTB5)
	eor r17, r16
	out PORTB, r17	
	reti
	
