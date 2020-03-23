# EditLine MMURTL system call
# Takes 6 parameters
# Call gate at 0x218
# Returns int

.section .text
.type EditLine, @function
.globl EditLine
EditLine:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	pushl	28(%ebp)
	lcall	$0x218, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
