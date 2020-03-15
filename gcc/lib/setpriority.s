# SetPriority MMURTL system call
# Takes 1 parameter
# Call gate at 0x58
# Returns int

.section .text
.type SetPriority, @function
.globl SetPriority
SetPriority:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x58, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
