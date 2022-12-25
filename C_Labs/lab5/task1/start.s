
global _start
global system_call

sys_read EQU 3
sys_write EQU 4
stdout EQU 1
stdin EQU 0
sys_close EQU 6
sys_open EQU 5


section .data
    echo_key: db '0'
    key: TIMES 100 db 0 
    key_p: dd 0
    key_len: dd 0
    input_file : dd  0  ; initialized to stdin
    output_file : dd  1  ; initialized to stdin
    read_buf : TIMES 100 db 0
    buf_len : dd 0
    newline : db 10, 0
    io_error_line : db 'io error, please enter valid file names', 10, 0
    io_err_len EQU $ -io_error_line
    ia_error_line : db 'invalid argument\s',10, 0
    ia_err_len EQU $ -ia_error_line
    char : db 0


section .text

_start:     ; ld flag sends argument not in c convention (? look for information)
    pop dword ecx    ; ecx = argc
    mov esi,esp      ; esi = argv
    push dword esi   ; char* argv[]
    push dword ecx   ; int argc
    call main        ; int main( int argc, char *argv[] )
    add esp, 8
    mov ebx,eax
    mov eax,1
    int 0x80
    nop

main:
    push ebp         
    mov ebp, esp
    mov esi, dword[ebp+8]    ; argc -> esi
    mov edi, dword[ebp+12]   ; argv* -> edi
    xor eax, eax             ; eax uses as moving pointer or counter through the whole program
    cmp esi, 1          ; no arguments -> execute prog
    jz _execute
    add edi, 4              ; prepare to set flags
    dec esi

_set_flags:  ; acts as a switch-case loop
    ; handle only if arg is greater or eq 2 
    mov ecx, [edi]
    cmp byte[ecx+1], 0
    jz _ia_err
    ; if not , handles next cmp throws seg fault
    cmp word[ecx], '+'+256*'e'
    jz set_e
    cmp word[ecx], '-'+256*'o'
    jz set_o
    cmp word[ecx], '-'+256*'i'
    jz set_i
    ; else illegal argument:
    jnz _ia_err  
;; set_f and _err function at end of file ;;
_sf_cont:
    add     edi, 4          ; now points to the next arg in argv
    dec     esi             ; argc - 1
    cmp     esi, 0          ; 0 arguments
    jnz _set_flags
_execute:
    cmp dword[key_p], 0   ; if key_p set cont
    jnz _encrypt
    mov dword[key_p],echo_key  ; else - set echo-key
    mov dword[key_len],1
    
_encrypt:  
    ; first set key
    push dword[key_p]   ; get key len in eax
    call strlen
    add esp, 4
    mov [key_len], eax
    mov eax, key
    mov ebx, [key_p]
    mov ecx, [key_len]
fetch_e_vals:   ; loop1
    mov     dl, byte[ebx]
    sub     dl, '0'
    ; check valid e_vals
    cmp     dl, 9
    ja      _ia_err
    cmp     dl, 0
    jb      _ia_err
    ; end check
    mov     byte[eax], dl
    add     ebx, 1
    add     eax, 1
    dec     ecx
    cmp     ecx, 0
    jnz     fetch_e_vals

read_input:     ; loop2 ; contains loop 3
    mov     edx, 100
    mov     ecx, read_buf
    mov     ebx, [input_file] 
    mov     eax, sys_read      
    int     0x80
    cmp eax, 1  ; continue while you can read
    je read_input
    cmp eax,0
    jle end
    ; finished reading input
    dec eax ; remove newline char -> fucks up printing
    mov [buf_len], eax  ; set length accordingly \ checked later
    mov eax, read_buf   ; reverse registers for use in print_output
    mov ebx, key
    mov edx, [key_len]  ; used to end loop

write_output:   ; loop3
    xor ecx, ecx
    mov cl, byte[eax]   ; get char
    add cl, byte[ebx]   ; add offset
    cmp cl, '~'
    ja fix_offset   ; check if rotation is needed (dec char 126 to 32)
    mov [char], cl
    jmp print_char
fix_offset:
    mov cl, 32
    mov [char], cl
    ; print\append char
print_char:
    pushad
    mov     edx, 1
    mov     ecx, char
    mov     ebx, [output_file]
    mov     eax, sys_write 
    int     0x80
    popad
    ; prepare next round
    inc eax
    inc ebx
    ;  check if we reached end of key
    dec edx 
    cmp edx, 0
    jnz check_if_done   ; if not, check if done
    mov edx, [key_len]  ; if yes, resets key and then check if done 
    mov ebx, key

check_if_done:
    dec dword[buf_len]
    cmp dword[buf_len], 0
    jnz write_output               ; if we didnt finish printing cont
    mov     edx, 1             ; if we did, print new line
    mov     ecx, newline
    mov     ebx, [output_file]
    mov     eax, sys_write
    int     0x80
    mov edi, [input_file]      ; check if user wants to continue (only if written in terminal)
    cmp edi, 0
    je read_input

end:
    mov     eax, 0
    mov     esp, ebp
    pop     ebp
    ret


; indpendet fun, protected by callee rules
strlen:                     ; returns in eax, changes edx,ecx
    push    ebp
    mov     ebp, esp
    mov     edx, [ebp+8]    ; get string
    xor     eax, eax        ; eax <- 0, counter
_byte_iter:
    mov     cl, [edx+eax]
    cmp     cl, 0x0
    je      strlen_ret
    inc     eax
    jmp     _byte_iter
strlen_ret:
    Leave
    ret


;; functions used in _set_flags ;;
set_e:
    mov eax, ecx
    add eax, 2
    mov [key_p], eax  
    jmp _sf_cont
set_o:
    mov eax, ecx
    add eax, 2
    mov     edx, 0
    mov     ecx, 1 ; write mode
    mov     ebx, eax
    mov     eax, sys_open 
    int     0x80
    cmp eax, 0
    jle _io_err ; if got errno (all errorno <0 )
    mov [output_file], eax
    jmp _sf_cont
set_i:
    mov eax, ecx
    add eax, 2
    mov     edx, 0
    mov     ecx, 0 ; read mode
    mov     ebx, eax
    mov     eax, sys_open
    int     0x80
    cmp eax, 0
    jle _io_err
    mov [input_file], eax
    jmp _sf_cont
_io_err:
    mov     edx, io_err_len
    mov     ecx, io_error_line
    mov     ebx, stdout
    mov     eax, sys_write 
    int     0x80
    jmp end
_ia_err:
    mov     edx, ia_err_len
    mov     ecx, ia_error_line
    mov     ebx, stdout
    mov     eax, sys_write
    int     0x80
    jmp end




