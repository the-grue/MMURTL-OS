# GetCmdLine MMURTL system call
# Takes 2 parameters
# Call gate at 0x2D8
# Returns int

.section .text
.type GetCmdLine, @function
.globl GetCmdLine
GetCmdLine:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x2D8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
