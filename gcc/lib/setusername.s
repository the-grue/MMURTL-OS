# SetUserName MMURTL system call
# Takes 2 parameters
# Call gate at 0x2C0
# Returns int

.section .text
.type SetUserName, @function
.globl SetUserName
SetUserName:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x2C0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
