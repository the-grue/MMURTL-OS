# UnRegisterSvc MMURTL system call
# Takes 1 parameter
# Call gate at 0x3A0
# Returns int

.section .text
.type UnRegisterSvc, @function
.globl UnRegisterSvc
UnRegisterSvc:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x3A0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
