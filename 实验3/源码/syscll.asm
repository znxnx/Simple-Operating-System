
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              mysyscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


; 导入全局变量
extern	_save
extern  _readdata
offsetOfUserProg equ 08c00h


;****************************
; SCOPY@                    *
;****************************
;局部字符串带初始化作为实参问题补钉程序
public SCOPY@
SCOPY@ proc 
		arg_0 = dword ptr 6
		arg_4 = dword ptr 0ah
		push bp
			mov bp,sp
		push si
		push di
		push ds
			lds si,[bp+arg_0]
			les di,[bp+arg_4]
			cld
			shr cx,1
			rep movsw
			adc cx,cx
			rep movsb
		pop ds
		pop di
		pop si
		pop bp
		retf 8
SCOPY@ endp



;****************************
; void _cls()               *
;****************************

public _cls
_cls proc 
; 清屏
        push ax
        push bx
        push cx
        push dx		
			mov	ax, 600h	; AH = 6,  AL = 0
			mov	bx, 700h	; 黑底白字(BL = 7)
			mov	cx, 0		; 左上角: (0, 0)
			mov	dx, 184fh	; 右下角: (24, 79)
				int	10h		; 显示中断
		pop dx
		pop cx
		pop bx
		pop ax
        ;mov [_disp_pos],0					;光标问题，清屏之后不知会不会出现问题
		ret
_cls endp

;********************************************************
; void _printChar(char ch)                              *
;********************************************************


public _printChar
_printChar proc 
	push bp
    push es
	push ax
    	mov bp,sp
		;***
        mov ax,0b800h
		mov es,ax
		mov al,byte ptr [bp+2+2+2+2] ;pos\ch\IP\bp\es\ax
		mov ah,00Fh
		mov di,[bp+2+2+2+2+2]									;;;;;pos 寻址方式不知道对不对 !!!!!!!!!!!!!!!!!!!!!!!!!!!
		mov	es:[di], ax
		;***
		mov sp,bp
    pop ax
	pop es
	pop bp
	ret
_printChar endp


public _showChar
_showChar proc
	push bp
    push es
	push ax
	push bx
		mov bp,sp
		mov ax,[bp+2+2+2+2+2] ; ASCII码ch\IP\bp\es\ax\bx
		mov ah,0eh ; 显示字符，推进光标
		mov bh,0	; 页码 ???????????????
		mov bl,0 ; Bl设为0
		int 10H ; 调用中断
		mov sp,bp
	pop bx
    pop ax
	pop es
	pop bp
	ret
_showChar endp


;****************************
; void _getChar()           *
;****************************

public _getChar
_getChar proc
	mov ah,0	;等待键盘输入,字符的ASCII码放入AL中。若AL＝0，则AH为输入的扩展码
	int 16h
	mov byte ptr [_save],al
	ret
_getChar endp



public _loadp
_loadp proc	
	push cx
	push ax
	push dx
	push bp
	push bx
		mov bp,sp

		mov cl,byte ptr[bp+2+2+2+2+2+2] ; int\IP\cx\ax\dx\bp\bx\ex
		add cl,12			  ;13扇区开始，i>=1
		mov bx,offsetOfUserProg
		mov ah,2              ; 功能号
		mov al,1              ;扇区数
		mov dl,0              ;驱动器号 ; 软盘为0，硬盘和U盘为80H
		mov dh,0              ;磁头号 ; 起始编号为0
		mov ch,0              ;柱面号 ; 起始编号为0
		int 13H ;             调用读磁盘BIOS的13h功能				
		
		call bx		
		
	pop bx
	pop bp
	pop dx
	pop ax
	pop cx
	ret
_loadp endp



public _progInfo
_progInfo proc

	push cx
	push ax
	push dx
	push bx
		
		mov ah,2              ; 功能号
		mov bx,9c00h
		mov al,1              ;扇区数
		mov dl,0              ;驱动器号 ; 软盘为0，硬盘和U盘为80H
		mov dh,0              ;磁头号 ; 起始编号为0
		mov ch,0              ;柱面号 ; 起始编号为0
		mov cl,2             ;起始扇区号 ; 起始编号为1
		mov [_readdata],bx 
		int 13H ;             调用读磁盘BIOS的13h功能	

	pop bx
	pop dx
	pop ax
	pop cx
	ret

_progInfo endp
