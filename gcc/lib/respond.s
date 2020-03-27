# Respond MMURTL system call
# Takes 2 parameters
# Call gate at 0x68
# Returns int

.section .text
.type Respond, @function
.globl Respond
Respond:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x68, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
