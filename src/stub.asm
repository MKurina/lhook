SYS_WRITE	equ 1
SYS_EXIT	equ 0x3c

global stub


struc elem_str
	.lib_str:	RESQ 1
	.fn_str:	RESQ 1
endstruc

elem_struct_sz	equ 0x32
struc elem
	.str_info:	RESQ	2
	.src:		RESQ	1
	.hook:		RESQ	1
	.do_ret:	RESB	1
	.active:	RESB	1
	.in_off:	RESQ	1
	.in_sz:		RESQ	1
endstruc

%macro pusha 0
	push rax
	push rbx
	push rcx
	push rdx
	push rsp
	push rbp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popa 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rbp
	pop rsp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

stub:
	jmp hook

call_orig:
	call [rsp - 0x18]	; call orig fn
	; jmp post_hook
	ret

hook:
	call get_tbl_open
	mov rax, 0x0123456789abcdef

	mov rbx, QWORD [rax+0x18]	; access struct tbl
	add rbx, rcx
	mov rdx, QWORD [rbx + elem.hook]

call_hook_fn:
	push rdx
	pusha

	mov rdi, QWORD [rsp+0x88]
	mov rdx, QWORD [rsp+0x90]
	mov rcx, QWORD [rsp+0x98]
	mov rbx, QWORD [rsp+0xa0]
	mov rax, QWORD [rsp+0xa8]

	call QWORD [rsp+0x80]			; Call hook fn

	push rax
	add rsp, 0x8
	popa
	add rsp, 0x8

hook_return:
	;; Shall be returned
	mov dl, BYTE [rbx + elem.do_ret]
	test dl, dl
	jne ret_rax

	;; Geet orig fn
	mov rdx, QWORD [rbx + elem.in_off]
	mov rax, QWORD [rax]
	add rax, rdx		; orig fn addr
	call get_tbl_close

	add rsp, 0x18
	mov rax, [rsp - 0x10]
	jmp call_orig

ret_rax:
	call get_tbl_close
	add rsp, 0x18
	mov rax, QWORD [rsp-0xc0]
	ret

get_tbl_open:
	sub rsp, 0x20			;
	push QWORD [rsp+0x20]	; So return is possible
	add rsp, 0x30			;

	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	mov rcx, QWORD [rsp+8*5]	; ndx
	imul rcx, elem_struct_sz	; off
	sub rsp, 0x8
	ret

get_tbl_close:
	push QWORD [rsp]	; So return is possible
	add rsp, 0x10		;

	pop rdi
	pop rdx
	pop rcx
	pop rbx
	push rax

	mov rax, [rsp - 0x10]

	sub rsp, 0x20
	pop QWORD [rsp+0x10]
	add rsp, 0x10
	ret
