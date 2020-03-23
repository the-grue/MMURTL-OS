# GetXY MMURTL system call
# Takes 2 parameters
# Call gate at 0x210
# Returns int

.section .text
.type GetXY, @function
.globl GetXY
GetXY:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x210, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
