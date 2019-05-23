*
* Read PC joystick (game) port: int joystick(int results[4], char flags)
*   results = Integer array to receive the AX, AY, BX, and BY values
*   flags   = Bits 0,1,2,3 = Disable AX,AY,BX,BY       (1 = Disable)
*   Returns : Bits 0,1,2,3 = Error reading AX,AY,BX,BY (1 = Error)
*             Bits 4,5,6,7 = Buttons A1,A2,B1,B2       (0 = Pressed)
*
* Values placed in 'results' will range from 0 to 'n', where 'n' is
* dependant on the speed of the hardware. Experiment with your system
* to determine the actual maximum values, and scale the results to
* the range of measurements you are taking.
*
* To add to DDS MICROC-C 8086 library: CD to LIB86 subdirectory, copy in
* this file, and issue the commands: (omit memory models you don't want)
*		slib i=tiny    a=joystick
*		slib i=small   a=joystick
*
* Copyright 1989-1995 Dave Dunfield
* All rights reserved.
*
* Permission granted for personal (non-commercial) use only.
*
joystick MOV	BX,SP			Address parms
		MOV		AH,2[BX]		Get mask flags
		MOV		BX,4[BX]		Get array address
		MOV		DX,#$0201		Address game port
		XOR		CX,CX			Zero initial count
		AND		AX,#$0F00		Mask AH, AL=0
		OUT		DX,AL			Write to I/O port
?1		IN		AL,DX			Read I/O port
		OR		AL,AH			Mask already received ports
		TEST	AL,#$01			AX bit set
		JNZ		?2				No, its not
		MOV		[BX],CX			Save position
		OR		AH,#$01			Disable further AX
?2		TEST	AL,#$02			AY bit set
		JNZ		?3				No, try next
		MOV		2[BX],CX		Save position
		OR		AH,#$02			Disable further AY
?3		TEST	AL,#$04			BX bit set?
		JNZ		?4				No, try next
		MOV		4[BX],CX		Save position
		OR		AH,#$04			Disable further BX
?4		TEST	AL,#$08			BY bit set?
		JNZ		?5				No, try next
		MOV		6[BX],CX		Save position
		OR		AH,#$08			Disable further BY
?5		INC		CX				Advance count
		JZ		?6				Overflow...
		CMP		AH,#$0F			All bits set?
		JNZ		?1				No, keep looking
?6		NOT		AH				Invert bits
		AND		AL,AH			Set flags
		XOR		AH,AH			Zero high
		RET
