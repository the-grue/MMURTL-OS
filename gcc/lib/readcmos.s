# ReadCMOS MMURTL system call
# Takes 1 parameter
# Call gate at 0x188
# Returns int

.section .text
.type ReadCMOS, @function
.globl ReadCMOS
ReadCMOS:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x188, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
