section .data
sysvar0 db "hello", 0



section .text
global _start

_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, sysvar0
    mov rdx, 6
    syscall

    mov rax, 60
    mov rdi, 1
    syscall
