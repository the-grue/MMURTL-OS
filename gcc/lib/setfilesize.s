# SetFileSize MMURTL system call
# Takes 2 parameters
# Call gate at 0x348
# Returns int

.section .text
.type SetFileSize, @function
.globl SetFileSize
SetFileSize:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x348, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
