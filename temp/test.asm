section .data
    my_int dd 14

section .text
global _start

_start:
    mov rax, 60
    mov edi, dword [my_int]
    syscall
