global main
extern printf

section .rodata
        toHex: db '%hhX', 10, 0 ; print by taking only half of half of size
        

section .data
        x_struct: dd 6 ; 4 byte val
                x_num: db 0xaa,1,2,0x44,0x4f,1 ; each is 1 byte


section .text

        main:
                push ebp
                mov ebp, esp
                mov eax, x_struct ; setting as excplicit pointer
                push eax ; pushing as argument
                call print_multi
                leave
                ret

        ; tot 19 lines
        print_multi:
                push ebp
                mov ebp, esp
                mov esi, [ebp+8] ; eax recieves pointer
                mov edi, [esi] ; recieves size
                add esi, 4 ; move pointer to x_num
                mov ebx, 0
        loop_print:
                xor eax, eax
                mov ecx, [esi+ebx]
                push ecx
                push toHex
                call printf
                add ebx,1
                cmp ebx, edi   ; when ebx = edi (size) we break
                jne loop_print
        end_loop:
                xor eax, eax
                leave
                ret
