# OutByte MMURTL system call
# Takes 2 parameters
# Call gate at 0x158
# Returns int

.section .text
.type OutByte, @function
.globl OutByte
OutByte:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x158, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
