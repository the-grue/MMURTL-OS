# DeAllocPage MMURTL system call
# Takes 2 parameters
# Call gate at 0x1D0
# Returns int

.section .text
.type DeAllocPage, @function
.globl DeAllocPage
DeAllocPage:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x1D0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
