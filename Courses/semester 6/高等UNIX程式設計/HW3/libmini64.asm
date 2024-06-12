
%macro gensys 2
	global sys_%2:function
sys_%2:
	push	r10
	mov	r10, rcx
	mov	rax, %1
	syscall
	pop	r10
	ret
%endmacro

; RDI, RSI, RDX, RCX, R8, R9

extern	errno

	section .data

	section .text

	gensys   0, read
	gensys   1, write
	gensys   2, open
	gensys   3, close
	gensys   9, mmap
	gensys  10, mprotect
	gensys  11, munmap
	gensys	13,	rt_sigaction
	gensys	14, rt_sigprocmask
	gensys	15, rt_sigreturn
	gensys  22, pipe
	gensys  32, dup
	gensys  33, dup2
	gensys  34, pause
	gensys  35, nanosleep
	gensys	37, alarm
	gensys	39, getpid
	gensys  57, fork
	gensys	62, kill
	gensys  231, exit
	gensys  79, getcwd
	gensys  80, chdir
	gensys  82, rename
	gensys  83, mkdir
	gensys  84, rmdir
	gensys  85, creat
	gensys  86, link
	gensys  88, unlink
	gensys  89, readlink
	gensys  90, chmod
	gensys  92, chown
	gensys  95, umask
	gensys  96, gettimeofday
	gensys 102, getuid
	gensys 104, getgid
	gensys 105, setuid
	gensys 106, setgid
	gensys 107, geteuid
	gensys 108, getegid
	gensys 127, rt_sigpending

;long sys_open(const char *filename, int flags, ... /*mode*/);
;int	open(const char *filename, int flags, ... /*mode*/);
	global open:function
open:
	call	sys_open
	cmp	rax, 0
	jge	open_success	; no error :)
open_error:
	neg	rax
%ifdef NASM
	mov	rdi, [rel errno wrt ..gotpc]
%else
	mov	rdi, [rel errno wrt ..gotpcrel]
%endif
	mov	[rdi], rax	; errno = -rax
	mov	rax, -1
	jmp	open_quit
open_success:
%ifdef NASM
	mov	rdi, [rel errno wrt ..gotpc]
%else
	mov	rdi, [rel errno wrt ..gotpcrel]
%endif
	mov	QWORD [rdi], 0	; errno = 0
open_quit:
	ret

;int	nanosleep(struct timespec *rqtp, struct timespec *rmtp);
;unsigned int sleep(unsigned int s);
	global sleep:function
sleep:
	sub	rsp, 32		; allocate timespec * 2
	mov	[rsp], rdi		; req.tv_sec
	mov	QWORD [rsp+8], 0	; req.tv_nsec
	mov	rdi, rsp	; rdi = req @ rsp
	lea	rsi, [rsp+16]	; rsi = rem @ rsp+16
	call	sys_nanosleep
	cmp	rax, 0
	jge	sleep_quit	; no error :)
sleep_error:
	neg	rax
	cmp	rax, 4		; rax == EINTR?
	jne	sleep_failed
sleep_interrupted:
	lea	rsi, [rsp+16]
	mov	rax, [rsi]	; return rem.tv_sec
	jmp	sleep_quit
sleep_failed:
	mov	rax, 0		; return 0 on error
sleep_quit:
	add	rsp, 32
	ret


	global setjmp:function
setjmp:
	pop	rcx
	mov	[rdi]	, rcx ; return address
	mov	[rdi+ 8], rbx
	mov	[rdi+16], rsp
	mov	[rdi+24], rbp
	mov	[rdi+32], r12
	mov	[rdi+40], r13
	mov	[rdi+48], r14
	mov	[rdi+56], r15
	; +++++++++++++++++++++
	push rcx
	push rdi
	push rsi
	push rdx
	push rax
	push r10
	;
	mov rax,	14
	mov rdi,	1
	xor	rsi,	rsi
	sub rsp,	8
	mov rdx, rsp
	mov r10, 	8
	syscall
	mov r11,	[rsp]
	add rsp,	8
	;
	pop r10
	pop rax
	pop rdx
	pop rsi
	pop rdi
	pop rcx
	mov [rdi+64], r11 ; set env->mask(sigset_t)
	; +++++++++++++++++++++
	xor rax, rax ; returns 0
	jmp rcx


	global longjmp:function
longjmp:
	mov	rcx,	[rdi]	
	mov	rbx,	[rdi+ 8]
	mov	rsp,	[rdi+16]
	mov	rbp,	[rdi+24]
	mov	r12,	[rdi+32]
	mov	r13,	[rdi+40]
	mov	r14,	[rdi+48]
	mov	r15,	[rdi+56]
	mov r11,	[rdi+64]
	; +++++++++++++++++++++
	push rcx
	push rdi
	push rsi
	push rdx
	push rax
	push r10
	;
	mov rax,	14
	mov rdi,	2	;SIG_SETMASK   2
	mov	rsi,	r11 ;oset = env->mask
	xor	rdx,	rdx
	mov r10, 	8
	syscall
	;
	pop r10
	pop rax
	pop rdx
	pop rsi
	pop rdi
	pop rcx
	; +++++++++++++++++++++
	mov rax, rsi
	cmp rax, 0
	jne	longjmp_quit
	inc rax

longjmp_quit:
	jmp rcx

	global myrt:function
myrt:
	mov rax, 15
	syscall







