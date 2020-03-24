# QueryPages MMURTL system call
# Takes 1 parameter
# Call gate at 0x290
# Returns int

.section .text
.type QueryPages, @function
.globl QueryPages
QueryPages:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x290, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
