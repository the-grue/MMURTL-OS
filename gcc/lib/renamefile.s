# RenameFile MMURTL system call
# Takes 4 parameters
# Call gate at 0x278
# Returns int

.section .text
.type RenameFile, @function
.globl RenameFile
RenameFile:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	lcall	$0x278, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
