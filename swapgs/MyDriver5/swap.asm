.code 

my_readmsr proc
	rdmsr 
	shl rdx, 20h
	or rax, rdx
	ret 
my_readmsr endp



my_swapgs proc
	push rbx 
	push rsi 

	mov rbx, rcx 
	mov rsi, rdx 

	mov rcx, 0C0000101h;
	call my_readmsr
	mov [rbx], rax 


	mov rcx, 0C0000101h;
	call my_readmsr
	mov [rsi], rax 

	swapgs
	
	pop rsi
	pop rbx
	ret
my_swapgs endp


end

