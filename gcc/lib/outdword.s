# OutDWord MMURTL system call
# Takes 2 parameters
# Call gate at 0x168
# Returns int

.section .text
.type OutDWord, @function
.globl OutDWord
OutDWord:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x168, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
