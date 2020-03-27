# GetpJCB MMURTL system call
# Takes 2 parameters
# Call gate at 0x288
# Returns int

.section .text
.type GetpJCB, @function
.globl GetpJCB
GetpJCB:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x288, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
