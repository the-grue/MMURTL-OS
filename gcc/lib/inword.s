# InWord MMURTL system call
# Takes 1 parameter
# Call gate at 0x178
# Returns int

.section .text
.type InWord, @function
.globl InWord
InWord:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x178, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
