.include "constants.inc"

.section .text
.global __start

.org 0x0000
	rjmp __start

__start:
	ldi r16, RAMENDH
	ldi r17, RAMENDL
	out SPH, r16
	out SPL, r17

	sei

	sbi DDRD, DDB6 ;pin 6 on board

	ldi r16, ((1 << COM0A1) | (1 << WGM01) | (1 << WGM00))
	out TCCR0A, r16

	ldi r16, ((1 << CS01) | (1 << CS00))
	out TCCR0B, r16

	ldi r16, 10
	out OCR0A, r16

	ldi r16, (1 << SE)
	out SMCR, r16

LOOP:
	rjmp LOOP
