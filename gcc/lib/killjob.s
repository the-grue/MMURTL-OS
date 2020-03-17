# KillJob MMURTL system call
# Takes 1 parameter
# Call gate at 0x390
# Returns int

.section .text
.type KillJob, @function
.globl KillJob
KillJob:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x390, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
