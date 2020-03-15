# UnMaskIRQ MMURTL system call
# Takes 1 parameter
# Call gate at 0xE8
# Returns int

.section .text
.type UnMaskIRQ, @function
.globl UnMaskIRQ
UnMaskIRQ:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0xE8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
