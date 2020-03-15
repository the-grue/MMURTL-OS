# AddCallGate MMURTL system call
# Takes 0 parameters
# Call gate at 0xC8
# Returns int

.section .text
.type AddCallGate, @function
.globl AddCallGate
AddCallGate:
	pushl	%ebp
	movl	%esp, %ebp

	lcall	$0xC8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
