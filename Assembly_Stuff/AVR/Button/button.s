.include "constants.inc"

.section .text
.global __start

.org 0x0000
	rjmp __start

.org 0x000C
	rjmp BUTTON_INTERRUPT


__start:
	ldi r16, RAMENDH
	ldi r17, RAMENDL
	out SPH, r16
	out SPL, r17

	sei 

	ldi r16, (1 << PCIE0)
	sts PCICR, r16

	ldi r16, (1 << PCINT0)
	sts PCMSK0, r16

	ldi r16, (1 << DDB5)
	out DDRB, r16

	ldi r16, (1 << PORTB0)
	out PORTB, r16

	ldi r16, (1 << SE)
	sts SMCR, r16
	
LOOP:
	sleep
	rjmp LOOP

BUTTON_INTERRUPT:
	in r16, PORTB
	ldi r17, (1 << PORTB5)
	eor r16, r17
	out PORTB, r16

	reti 	


	
