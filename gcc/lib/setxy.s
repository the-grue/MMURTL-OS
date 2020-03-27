# SetXY MMURTL system call
# Takes 2 parameters
# Call gate at 0x208
# Returns int

.section .text
.type SetXY, @function
.globl SetXY
SetXY:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x208, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
