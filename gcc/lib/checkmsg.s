# CheckMsg MMURTL system call
# Takes 2 parameters
# Call gate at 0x70
# Returns int

.section .text
.type CheckMsg, @function
.globl CheckMsg
CheckMsg:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x70, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
