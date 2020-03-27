# ExitJob MMURTL system call
# Takes 1 parameters
# Call gate at 0x2B8
# Returns int

.section .text
.type ExitJob, @function
.globl ExitJob
ExitJob:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x2B8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
