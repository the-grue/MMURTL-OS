# MicroDelay MMURTL system call
# Takes 1 parameter
# Call gate at 0x138
# Returns int

.section .text
.type MicroDelay, @function
.globl MicroDelay
KillAlarm:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x138, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
