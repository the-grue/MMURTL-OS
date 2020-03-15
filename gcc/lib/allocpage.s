# AllocPage MMURTL system call
# Takes 2 parameters
# Call gate at 0xA8
# Returns int

.section .text
.type AllocPage, @function
.globl AllocPage
AllocPage:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xA8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
