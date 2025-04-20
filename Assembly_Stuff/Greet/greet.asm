section .bss
	buffer resb 128

section .text
	global _start

_start:
	mov rax, 0
	mov rdi, 0
	mov rsi, buffer
	mov rdx, 128
	syscall

	mov rbx, rax

	mov rax, 1
	mov rdi, 1
	mov rsi, buffer
	mov rdx, rbx
	syscall

	mov rax, 60
	xor rdi, rdi
	syscall
	
