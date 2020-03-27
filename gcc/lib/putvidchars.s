# PutVidChars MMURTL system call
# Takes 5 parameters
# Call gate at 0x200
# Returns int

.section .text
.type PutVidChars, @function
.globl PutVidChars
PutVidChars:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	lcall	$0x200, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
