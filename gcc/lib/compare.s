# Compare MMURTL system call
# Takes 3 parameters
# Call gate at 0x1B0
# Returns int

.section .text
.type Compare, @function
.globl Compare
Compare:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x1B0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
