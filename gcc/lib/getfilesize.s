# GetFileSize MMURTL system call
# Takes 2 parameters
# Call gate at 0x268
# Returns int

.section .text
.type GetFileSize, @function
.globl GetFileSize
GetFileSize:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x268, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
