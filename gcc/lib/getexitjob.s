# GetExitJob MMURTL system call
# Takes 2 parameters
# Call gate at 0x2F8
# Returns int

.section .text
.type GetExitJob, @function
.globl GetExitJob
GetExitJob:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x2F8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
