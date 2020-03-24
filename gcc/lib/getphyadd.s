# GetPhyAdd MMURTL system call
# Takes 2 parameters
# Call gate at 0x298
# Returns int

.section .text
.type GetPhyAdd, @function
.globl GetPhyAdd
GetPhyAdd:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x298, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
