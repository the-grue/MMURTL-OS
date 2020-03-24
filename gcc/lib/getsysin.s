# GetSysIn MMURTL system call
# Takes 2 parameters
# Call gate at 0x310
# Returns int

.section .text
.type GetSysIn, @function
.globl GetSysIn
GetSysIn:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x310, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
