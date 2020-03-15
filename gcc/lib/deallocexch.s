# DeAllocExch MMURTL system call
# Takes 1 parameter
# Call gate at 0x88
# Returns int

.section .text
.type DeAllocExch, @function
.globl DeAllocExch
DeAllocExch:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x88, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
