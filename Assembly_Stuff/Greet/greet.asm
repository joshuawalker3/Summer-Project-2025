section .data
	prompt db "Please enter your name: "
	prompt_len equ $ - prompt
	greet db "Hello "
	greet_len equ $ - greet

section .bss
	input_buffer resb 128

section .text
	global _start

_start:
	mov rax, 1
	mov rdi, 1
	mov rsi, prompt
	mov rdx, prompt_len
	syscall

	mov rax, 0
	mov rdi, 0
	mov rsi, input_buffer
	mov rdx, 128
	syscall

	mov rbx, rax

	mov rax, 1
	mov rdi, 1
	mov rsi, greet
	mov rdx, greet_len
	syscall

	mov rax, 1
	mov rdi, 1
	mov rsi, input_buffer
	mov rdx, rbx
	syscall

	mov rax, 60
	xor rdi, rdi
	syscall
	
