# OutWords MMURTL system call
# Takes 3 parameters
# Call gate at 0x1C0
# Returns int

.section .text
.type OutWords, @function
.globl OutWords
OutWords:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x1C0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
