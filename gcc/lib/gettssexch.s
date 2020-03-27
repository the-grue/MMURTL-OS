# GetTSSExch MMURTL system call
# Takes 1 parameter
# Call gate at 0x220
# Returns int

.section .text
.type GetTSSExch, @function
.globl GetTSSExch
GetTSSExch:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x220, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
