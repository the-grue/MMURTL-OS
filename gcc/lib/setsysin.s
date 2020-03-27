# SetSysIn MMURTL system call
# Takes 2 parameters
# Call gate at 0x300
# Returns int

.section .text
.type SetSysIn, @function
.globl SetSysIn
SetSysIn:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x300, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
