# GetFileLFA MMURTL system call
# Takes 2 parameters
# Call gate at 0x258
# Returns int

.section .text
.type GetFileLFA, @function
.globl GetFileLFA
GetFileLFA:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x258, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
