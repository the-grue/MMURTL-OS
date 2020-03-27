# GetCMOSDate MMURTL system call
# Takes 1 parameter
# Call gate at 0x350
# Returns int

.section .text
.type GetCMOSDate, @function
.globl GetCMOSDate
GetCMOSDate:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x350, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
