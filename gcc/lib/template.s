.section .text
.type FUNCNAME, @function
.globl FUNCNAME
FUNCNAME:
	pushl	%ebp
	movl	%esp, %ebp

	; Subtract space for local variables if necessary
	; subl	$4, %esp

	; Code here

	movl	%ebp, %esp
	popl	%ebp
	ret
