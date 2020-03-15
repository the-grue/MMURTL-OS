# DMASetUp MMURTL system call
# Takes 5 parameters
# Call gate at 0xB8
# Returns int

.section .text
.type DMASetUp, @function
.globl DMASetUp
DMASetUp:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	lcall	$0xB8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
