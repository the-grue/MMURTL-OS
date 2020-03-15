# EndOfIRQ MMURTL system call
# Takes 1 parameter
# Call gate at 0xD8
# Returns int

.section .text
.type EndOfIRQ, @function
.globl EndOfIRQ
EndOfIRQ:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0xD8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
