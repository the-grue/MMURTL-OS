# GetPath MMURTL system call
# Takes 3 parameters
# Call gate at 0x2E8
# Returns int

.section .text
.type GetPath, @function
.globl GetPath
GetPath:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
    pushl   16(%ebp)
	lcall	$0x2E8, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
