*
* Allocate a block of memory: char *malloc(size)
*
malloc	LDI	#?heap		Point to beginning of heap
* Search for free block of memory
?1	LDB	I		Get flag
	SJZ	?4		End of list, allocate here
	DEC			Un-allocated?
	JNZ	?2		No, try next
	LD	1,I		Get size
* Found free block, see if its large enough
	CMP	2,S		Large enough?
	UGE			Complete compare
	SJNZ	?3		Yes, its ok
* This block not suitable, advance to next
?2	LD	1,I		Get size
	ADDB	#3		Adjust for overhead
	ADAI			Advance to next
	SJMP	?1		And keep looking
* This block is OK to re-allocate
?3	LD	1,I		Get size back
	SUB	2,S		Calculate remaining
	ST	?temp		Save new size
	CMPB	#3		Is it worth splitting?
	ULE			Complete compare
	SJNZ	?6		No, skip it
* Split this block into two blocks
	LD	2,S		Get size of block
	PUSHI			Save index
	ADAI			Offset to next
	LDB	#1		Get released flag
	STB	3,I		Set it free
	LD	?temp		Get size back
	SUBB	#3		Convert
	ST	4,I		Save size of block
	LDI	S+		Restore INDEX
	SJMP	?5		And proceed
* Allocate on end of memory
?4	TSA			Get stack pointer
	SUB	2,S		Adjust for buffer size
	SUB	#256		Adjust for margin
	PUSHI			Save pointer
	CMP	S+		Test it
	UGT			Complete compare
	SJZ	?7		No, fail
* Ok to proceed, allocate memory
	LD	2,S		Get size
	PUSHI			Save index
	ADAI			Offset to new area
	CLR			Get zero
	STB	3,I		Indicate end of list
	LDI	S+		Restore index
?5	LD	2,S		Get block size
	ST	1,I		Save block size
?6	LDB	#2		Get 'Allocated' flag
	STB	I		Set it
	LEAI	3,I		Point to address
	TIA			In ACC for return
?7	RET
*
* Release a block of memory: free(ptr)
*
free	LD	2,S		Get address
	SUBB	#3		Backup to "real" beginning
	PUSHA			Save for later
* Search the allocation list for this block
	LDI	#?heap		Point to beginning of heap
?10	LDB	I		Get address
	SJZ	?12		End of list
	TIA			Get address
	CMP	,S		Is this it?
	SJNZ	?11		Yes, handle it
	LD	1,I		Get size
	ADAI			Advance for size
	LEAI	3,I		Include overhead
	SJMP	?10		And keep looking
* Mark this block as un-allocated
?11	LDB	#1		Get 'deallocated' flag
	STB	I
?12	LD	S+		Clean up stack
* Garbage collection, scan allocation list and join any
* contiguous de-allocated blocks into single areas.
* Also, truncate list at last allocated block.
	LDI	#?heap		Point to beginning of heap
?20	LDB	I		Get allocation flag
	SJZ	?25		End, quit
	DEC			Test for de-allocated
	SJNZ	?23		No, its not
	LD	1,I		Get size of block
	STI	?temp		Save for later
* This block is free, check following blocks
?21	ADAI			Point to next block
	LDB	3,I		Get next flag
	SJZ	?24		End of list, its ok
	DEC			Test for allocated?
	SJNZ	?22		Yes, stop looking
* Next block is also free
	LD	4,I		Get size of next block
	ADDB	#3		Adjust for three
	SJMP	?21		And keep looking
* Resave this block size
?22	TIA			Get end address
	SUB	?temp		Convert to actual size
	LDI	?temp		Get original pointer
	ST	1,I		Save new size
* Advance to next block in list
?23	LD	1,I		Get length
	ADAI			Offset to next
	LEAI	3,I		Include overhead
	SJMP	?20		And keep looking
* Mark this block as end of list
?24	LD	?temp		Get pointer
	CLR			Get zero
	STB	I		Indicate end of list
?25	RET
