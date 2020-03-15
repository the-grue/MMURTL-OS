# AllocExch MMURTL system call
# Takes 1 parameter
# Call gate at 0x80
# Returns int

.section .text
.type AllocExch, @function
.globl AllocExch
AllocExch:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x80, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
