# ReadKBD MMURTL system call
# Takes 2 parameters
# Call gate at 0xC0
# Returns int

.section .text
.type ReadKBD, @function
.globl ReadKBD
ReadKBD:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xC0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
