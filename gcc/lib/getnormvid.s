# GetNormVid MMURTL system call
# Takes 1 parameter
# Call gate at 0x338
# Returns int

.section .text
.type GetNormVid, @function
.globl GetNormVid
GetNormVid:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x338, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
