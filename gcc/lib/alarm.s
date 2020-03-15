# Alarm MMURTL system call
# Takes 2 parameters
# Call gate at 0x98
# Returns int

.section .text
.type Alarm, @function
.globl Alarm
Alarm:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x98, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
