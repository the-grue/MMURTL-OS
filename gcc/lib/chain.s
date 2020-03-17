# Chain MMURTL system call
# Takes 3 parameters
# Call gate at 0x340
# Returns int

.section .text
.type Chain, @function
.globl Chain
Chain:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x340, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
