# CloseFile MMURTL system call
# Takes 1 parameter
# Call gate at 0x230
# Returns int

.section .text
.type CloseFile, @function
.globl CloseFile
CloseFile:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x230, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
