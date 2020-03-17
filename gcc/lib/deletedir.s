# DeleteDir MMURTL system call
# Takes 3 parameters
# Call gate at 0x360
# Returns int

.section .text
.type DeleteDir, @function
.globl DeleteDir
DeleteDir:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	pushl	12(%ebp)
	pushl	16(%ebp)
	lcall	$0x360, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
