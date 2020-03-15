# SetJobName MMURTL system call
# Takes 2 parameters
# Call gate at 0x388
# Returns int

.section .text
.type SetJobName, @function
.globl SetJobName
SetJobName:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x388, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
