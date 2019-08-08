; 实验8
public _p
_p proc
	mov ah, 12
	int 21h
	ret
_p endp

public _v
_v proc
	mov ah, 13
	int 21h
	ret
_v endp

public _getsem
_getsem proc
	mov ah, 15
	int 21h
	ret
_getsem endp


;  实验七部分
public _fork
_fork proc
    mov ah, 9
    int 21h
    ret
_fork endp

public _wait
_wait proc
    mov ah, 10
    int 21h
    ret
_wait endp

public _exit
_exit proc
    push bp
    mov bp, sp
    mov bx, word ptr [bp+4]
    mov ah, 11
    int 21h
    pop bp
    ret
_exit endp

public _printchar
_printchar proc
    push ax
    push bp
    mov bp, sp
    mov al, byte ptr [bp+6]
    mov ah, 0eh
    mov bl, 0
    int 10h
    pop bp
    pop ax
    ret 
_printchar endp

_test proc
    push ax
    push bx
    mov al, '*'
    mov ah, 0eh
    mov bl, 0
    int 10h
    pop bx
    pop ax
    ret
_test endp
