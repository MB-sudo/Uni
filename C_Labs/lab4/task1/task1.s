global main
extern printf
extern puts

section .rodata
        format1: db 'argc: %d',10, 0

section .text
        main:
                push ebp             ; prepare stack frame
                mov ebp, esp
                mov edi, dword[ebp+8]    ; get argc into edi
                mov esi, dword[ebp+12]   ; get first argv string into esi
                push edi
                push format1
                call printf
                add esp, 8
        argv_loop:
                push    dword[esi]     ; dereference esi; points to argv
                call    puts
                add     esp, 4
                add     esi, 4          ; advance to the next pointer in argv
                dec     edi             ; decrement edi from argc to 0
                cmp     edi, 0          ; when it hits 0, we're done
                jnz     argv_loop      ; end with NULL pointer
        end_loop:
                xor     eax, eax
                leave
                ret

