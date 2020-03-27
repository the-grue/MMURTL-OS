# Tone MMURTL system call
# Takes 2 parameters
# Call gate at 0x128
# Returns int

.section .text
.type Tone, @function
.globl Tone
Tone:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x128, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
