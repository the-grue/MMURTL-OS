# ClrScr MMURTL system call
# Takes 0 parameters
# Call gate at 0x1F0
# Returns int

.section .text
.type ClrScr, @function
.globl ClrScr
ClrScr:
	pushl	%ebp
	movl	%esp, %ebp

	lcall	$0x1F0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
