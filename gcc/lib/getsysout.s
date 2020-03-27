# GetSysOut MMURTL system call
# Takes 2 parameters
# Call gate at 0x318
# Returns int

.section .text
.type GetSysOut, @function
.globl GetSysOut
GetSysOut:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	lcall	$0x318, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
