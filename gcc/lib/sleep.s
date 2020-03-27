# Sleep MMURTL system call
# Takes 1 parameters
# Call gate at 0x90
# Returns int

.section .text
.type Sleep, @function
.globl Sleep
Sleep:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x90, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
