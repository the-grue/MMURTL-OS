# CreateDir MMURTL system call
# Takes 2 parameters
# Call gate at 0x358
# Returns int

.section .text
.type CreateDir, @function
.globl CreateDir
CreateDir:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x358, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
