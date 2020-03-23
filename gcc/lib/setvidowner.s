# SetVidOwner MMURTL system call
# Takes 1 parameters
# Call gate at 0x1E0
# Returns int

.section .text
.type SetVidOwner, @function
.globl SetVidOwner
SetVidOwner:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x1E0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
