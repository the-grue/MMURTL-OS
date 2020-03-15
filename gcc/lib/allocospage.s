# AllocOSPage MMURTL system call
# Takes 2 parameters
# Call gate at 0xA0
# Returns int

.section .text
.type AllocOSPage, @function
.globl AllocOSPage
AllocOSPage:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xA0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
