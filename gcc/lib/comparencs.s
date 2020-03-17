# CompareNCS MMURTL system call
# Takes 3 parameters
# Call gate at 0x1A8
# Returns int

.section .text
.type CompareNCS, @function
.globl CompareNCS
CompareNCS:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x1A8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
