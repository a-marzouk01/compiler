section .data
xter dd 17
section .text
global _start

_start:
    mov rax, 60
    mov edi, dword [xter]
    syscall