# RegisterSvc MMURTL system call
# Takes 2 parameters
# Call gate at 0xB0
# Returns int

.section .text
.type RegisterSvc, @function
.globl RegisterSvc
RegisterSvc:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0xB0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
