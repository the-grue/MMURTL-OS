# GetVidOwner MMURTL system call
# Takes 1 parameters
# Call gate at 0x1E8
# Returns int

.section .text
.type GetVidOwner, @function
.globl GetVidOwner
GetVidOwner:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x1E8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
