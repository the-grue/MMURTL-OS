# AliasMem MMURTL system call
# Takes 4 parameters
# Call gate at 0x370
# Returns int

.section .text
.type AliasMem, @function
.globl AliasMem
AliasMem:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	pushl	20(%ebp)
	lcall	$0x370, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
