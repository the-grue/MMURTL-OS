# NewTask MMURTL system call
# Takes 7 parameters
# Call gate at 0x78
# Returns int

.section .text
.type NewTask, @function
.globl NewTask
NewTask:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	pushl	28(%ebp)
	pushl	32(%ebp)
	lcall	$0x78, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
