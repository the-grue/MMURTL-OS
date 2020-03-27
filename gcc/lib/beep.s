# Beep MMURTL system call
# Takes 0 parameters
# Call gate at 0x120
# Returns int

.section .text
.type Beep, @function
.globl Beep
Beep:
	pushl	%ebp
	movl	%esp, %ebp

	lcall	$0x120, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
