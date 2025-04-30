;Interrupt Initialization
.equ PCICR, 0x68
.equ PCIE0, 0x00
.equ PCMSK0, 0x6B
.equ PCINT0, 0x00

;Initialize PORTB
.equ DDB5, 0x05
.equ DDRB, 0x04
.equ PORTB5, 0x05
.equ PORTB, 0x05
.equ PORTB0, 0x00
.equ PINB, 0x03

;Initialize Counters
.equ TCCR0A, 0x24
.equ TCCR0B, 0x25
.equ WGM01, 0x01
.equ CS00, 0x00
.equ CS02, 0x04
.equ OCR0A, 0x27
.equ TIMSK0, 0x6E
.equ OCIE0A, 0x01

;Actual counter
.equ TCNT0, 0x26

;Checking Flags
.equ TIFR0, 0x15
.equ OCF0A, 0x01

;delay
.equ DELAY, 0x9c

;Stack initialization
.equ RAMENDH, 0x08
.equ RAMENDL, 0xFF
.equ SPH, 0x3E
.equ SPL, 0x3D

;Sleep initialization
.equ SE, 0x00
.equ SMCR, 0x33
