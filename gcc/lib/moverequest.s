# MoveRequest MMURTL system call
# Takes 2 parameters
# Call gate at 0x1C8
# Returns int

.section .text
.type MoveRequest, @function
.globl MoveRequest
MoveRequest:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x1C8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
