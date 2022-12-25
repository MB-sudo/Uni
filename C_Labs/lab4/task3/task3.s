global main
extern printf
extern malloc
extern free


section .rodata
        format: db '%X', 10, 0 ; 

section .bss
        p_struc: resd 1
                
        

section .data
        x_struct: dd 6
                x_val: db 1,0xf0,1,2,0x44,0x4f

        y_struct: dd 6
                y_val: db 1,1,2,0x44,1, 0xff


        count: db 0


section .text
        main:
                push ebp
                mov ebp, esp
                mov eax, x_struct
                mov ebx, y_struct
                push eax
                push ebx
                call add_multi
                push ecx
                call print_multi
                push ecx
                call free
                add esp, 16
                leave
                ret

        add_multi:
                push ebp
                mov ebp, esp
                mov eax, [ebp+8]             ; get pointers
                mov ebx, [ebp+12]
                call set_max_min             ; set max
                mov edi, dword[eax]          ; get max len
                mov esi, dword[ebx]          ; get min len
                pushad                          ; framing malloc
                add edi,5                       ; accounting for carry + last bit
                call malloc
                mov dword[p_struc], eax
                popad
                mov ecx, dword[p_struc]
                mov dword[ecx], edi
                push edi
                xor edi, edi
                mov edi, 4
        multi_loop1:
                mov dl, byte[ebx+edi]       ; get val in i'th position
                mov byte[ecx+edi], dl
                inc edi
                dec esi
                jnz multi_loop1

                pop edi
                add esi, 4              ; swap - edi\esi porpuses (for shorter code)

        multi_loop2:
                mov dl, byte[ecx+esi]
                adc dl, byte[eax+esi]
                mov byte[ecx+esi], dl
                inc esi
                dec edi
                jnz multi_loop2

                jnc end_multi
                adc dl, 0
                mov byte[ecx+esi], dl
                mov edx, dword[ecx]
                inc edx
                mov dword[ecx], edx
        end_multi:
                leave
                ret


        set_max_min:  ; this is not protected by callee rules 
                mov edi, dword[eax]
                mov esi, dword[ebx]
                cmp edi, esi
                jl switch_regs  ;jump if cmp is negative
                ret
        switch_regs:
                xor ecx, ecx            ; set ecx to zero
                mov ecx, eax
                mov eax, ebx
                mov ebx, ecx
                ret


        print_multi:
                push ebp
                mov ebp, esp
                pushad
                mov esi, [ebp+8]        ; esi recieves pointer
                mov edi, dword[esi]          ; recieves size
                cmp edi, 0              ; if size is 0 dont print - jump out of printing loop
                jz end_loop
                add esi, 4              ; move pointer to x_num
                mov ebx,0
        loop_print:
                xor eax, eax
                mov al, byte[esi+ebx]
                push eax
                push format
                call printf
                add esp,8               ;clear stack
                inc ebx
                cmp ebx, edi            ; when ebx = edi (size) we break
                jne loop_print
        end_loop:
                popad
                mov esp, ebp
                pop ebp
                ret

                