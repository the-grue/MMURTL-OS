# DeleteFile MMURTL system call
# Takes 1 parameter
# Call gate at 0x280
# Returns int

.section .text
.type DeleteFile, @function
.globl DeleteFile
DeleteFile:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x280, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
