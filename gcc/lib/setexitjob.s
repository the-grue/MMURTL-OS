# SetExitJob MMURTL system call
# Takes 2 parameters
# Call gate at 0x2F0
# Returns int

.section .text
.type SetExitJob, @function
.globl SetExitJob
SetExitJob:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x2F0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
