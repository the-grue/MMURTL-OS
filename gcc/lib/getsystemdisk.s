# GetSystemDisk MMURTL system call
# Takes 1 parameter
# Call gate at 0x398
# Returns int

.section .text
.type GetSystemDisk, @function
.globl GetSystemDisk
GetSystemDisk:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	8(%ebp)
	lcall	$0x398, $0x00000000

	movl	%ebp, %esp
	popl	%ebp
	ret
