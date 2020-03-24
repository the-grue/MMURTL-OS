# SetPath MMURTL system call
# Takes 2 parameters
# Call gate at 0x2E0
# Returns int

.section .text
.type SetPath, @function
.globl SetPath
SetPath:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x2E0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
