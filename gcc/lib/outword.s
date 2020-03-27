# OutWord MMURTL system call
# Takes 2 parameters
# Call gate at 0x160
# Returns int

.section .text
.type OutWord, @function
.globl OutWord
OutWord:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x160, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
