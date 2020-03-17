# KillAlarm MMURTL system call
# Takes 1 parameter
# Call gate at 0x130
# Returns int

.section .text
.type KillAlarm, @function
.globl KillAlarm
KillAlarm:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x130, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
