# SetSysOut MMURTL system call
# Takes 2 parameters
# Call gate at 0x308
# Returns int

.section .text
.type SetSysOut, @function
.globl SetSysOut
SetSysOut:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x308, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
