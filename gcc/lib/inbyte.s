# InByte MMURTL system call
# Takes 1 parameter
# Call gate at 0x170
# Returns int

.section .text
.type InByte, @function
.globl InByte
InByte:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x170, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
