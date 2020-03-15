# DeviceStat MMURTL system call
# Takes 4 parameters
# Call gate at 0x118
# Returns int

.section .text
.type DeviceStat, @function
.globl DeviceStat
DeviceStat:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	lcall	$0x118, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
