# SpawnTask MMURTL system call
# Takes 5 parameters
# Call gate at 0x140
# Returns int

.section .text
.type SpawnTask, @function
.globl SpawnTask
DeviceOp:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	pushl	24(%ebp)
	lcall	$0x140, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
