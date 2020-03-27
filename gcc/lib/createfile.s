# CreateFile MMURTL system call
# Takes 3 parameters
# Call gate at 0x270
# Returns int

.section .text
.type CreateFile, @function
.globl CreateFile
CreateFile:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x270, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
