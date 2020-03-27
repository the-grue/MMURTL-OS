# AddIDTGate MMURTL system call
# Takes 0 parameters
# Call gate at 0xD0
# Returns int

.section .text
.type AddIDTGate, @function
.globl AddIDTGate
AddIDTGate:
	pushl	%ebp
	movl	%esp, %ebp

	lcall	$0xD0, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
