# Request MMURTL system call
# Takes 12 parameters
# Call gate at 0x60
# Returns int

.section .text
.type Request, @function
.globl Request
Request:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	pushl	28(%ebp)
	pushl	32(%ebp)
	pushl	36(%ebp)
	pushl	40(%ebp)
	pushl	44(%ebp)
	pushl	48(%ebp)
	pushl	52(%ebp)
	lcall	$0x60, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
