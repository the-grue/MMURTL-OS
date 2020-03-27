# GetTimerTick MMURTL system call
# Takes 1 parameter
# Call gate at 0x150
# Returns int

.section .text
.type GetTimerTick, @function
.globl GetTimerTick
GetTimerTick:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x150, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
