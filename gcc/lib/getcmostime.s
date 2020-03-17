# GetCMOSTime MMURTL system call
# Takes 1 parameter
# Call gate at 0x148
# Returns int

.section .text
.type GetCMOSTime, @function
.globl GetCMOSTime
GetCMOSTime:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x148, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
