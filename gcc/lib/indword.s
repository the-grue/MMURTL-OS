# InDWord MMURTL system call
# Takes 1 parameter
# Call gate at 0x180
# Returns int

.section .text
.type InDWord, @function
.globl InDWord
InDWord:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x180, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
