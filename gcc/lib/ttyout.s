# TTYOut MMURTL system call
# Takes 3 parameters
# Call gate at 0x1F8
# Returns int

.section .text
.type TTYOut, @function
.globl TTYOut
TTYOut:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x1F8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
