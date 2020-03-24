# GetJobNum MMURTL system call
# Takes 1 parameter
# Call gate at 0x2B0
# Returns int

.section .text
.type GetJobNum, @function
.globl GetJobNum
GetJobNum:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x2B0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
