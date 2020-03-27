# SendMsg MMURTL system call
# Takes 3 parameters
# Call gate at 0x48
# Returns int

.section .text
.type SendMsg, @function
.globl SendMsg
SendMsg:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x48, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
