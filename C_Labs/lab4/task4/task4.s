global main
extern printf

section .data
        format: db '%hX', 10, 0


section .text

        main:
                push ebp
                mov ebp, esp
                mov ax, 0x002D ;mask
                mov bx, 0x5555 ;state
                xor di, di
                call print_state

        loop:
                xor bx, ax
                jpe add_carry
                shr bx, 1
                call print_state
                call check_num
        add_carry:
                stc
                rcr bx, 1
                call print_state 
                call check_num
        check_num:
                add di, 1
                cmp di, 20
                jne loop
                xor eax,eax
                leave
                ret

        print_state:
                push bx
                push format
                call printf
                add esp, 6
                ret
