.equ DDB0, 0x00
.equ DDRB, 0x04
.equ PORTB0, 0
.equ PORTB, 0x05

.org 0x00
	rjmp RESET

RESET:
	ldi r16, (1 << DDB0)
	out DDRB, r16 

LOOP:
	ldi r16, (1 << PORTB0)
	out PORTB, r16
	rjmp LOOP

DELAY:
	ldi r24, 0xFF
	ldi r25, 0xFF

DELAY_LOOP:
	sbiw r24, 1
	brne DELAY_LOOP
	ret	
