# SetNormVid MMURTL system call
# Takes 1 parameter
# Call gate at 0x330
# Returns int

.section .text
.type SetNormVid, @function
.globl SetNormVid
SetNormVid:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x330, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
