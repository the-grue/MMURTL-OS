
.DATA
EXTRN _InitDevDr FWORD
EXTRN _DeviceOp FWORD
EXTRN _DeviceStat FWORD
EXTRN _DeviceInit FWORD
EXTRN _UnMaskIRQ FWORD
EXTRN _MaskIRQ FWORD
EXTRN _SetIRQVector FWORD
EXTRN _EndOfIRQ FWORD
EXTRN _GetpJCB FWORD
EXTRN _GetJobNum FWORD
EXTRN _LoadNewJob FWORD
EXTRN _Chain FWORD
EXTRN _ExitJob FWORD
EXTRN _KillJob FWORD
EXTRN _SetUserName FWORD
EXTRN _GetUserName FWORD
EXTRN _SetCmdLine FWORD
EXTRN _GetCmdLine FWORD
EXTRN _SetPath FWORD
EXTRN _GetPath FWORD
EXTRN _SetExitJob FWORD
EXTRN _GetExitJob FWORD
EXTRN _SetSysIn FWORD
EXTRN _GetSysIn FWORD
EXTRN _SetSysOut FWORD
EXTRN _GetSysOut FWORD
EXTRN _SetJobName FWORD
EXTRN _RegisterSvc FWORD
EXTRN _UnRegisterSvc FWORD
EXTRN _GetSystemDisk FWORD
EXTRN _ReadKbd FWORD
EXTRN _Alarm FWORD
EXTRN _KillAlarm FWORD
EXTRN _Sleep FWORD
EXTRN _MicroDelay FWORD
EXTRN _GetCMOSTime FWORD
EXTRN _GetCMOSDate FWORD
EXTRN _GetTimerTick FWORD
EXTRN _Tone FWORD
EXTRN _Beep FWORD
EXTRN _SetVidOwner FWORD
EXTRN _SetNormVid FWORD
EXTRN _GetNormVid FWORD
EXTRN _ClrScr FWORD
EXTRN _GetVidChar FWORD
EXTRN _SetXY FWORD
EXTRN _PutVidAttrs FWORD
EXTRN _PutVidChars FWORD
EXTRN _GetVidOwner FWORD
EXTRN _GetXY FWORD
EXTRN _ScrollVid FWORD
EXTRN _TTYOut FWORD
EXTRN _EditLine FWORD
L_lit DB 32,32,32,32,32,84,101,114,109,105,110,97,108,108,121,32
 DB 68,85,77,66,44,32,68,117,109,98,32,84,101,114,109,105
 DB 110,97,108,32,80,114,111,103,114,97,109,13,10,0,32,32
 DB 32,32,32,32,40,77,77,85,82,84,76,32,67,111,109,109
 DB 115,32,68,101,118,105,99,101,32,68,114,105,118,101,114,32
 DB 100,101,109,111,41,32,13,10,0,69,114,114,111,114,32,111
 DB 110,32,68,101,118,105,99,101,32,83,116,97,116,58,32,37
 DB 100,13,10,0,73,82,81,78,117,109,58,32,37,100,13,10
 DB 0,73,79,66,97,115,101,58,32,37,100,13,10,0,115,88
 DB 66,117,102,58,32,32,37,100,13,10,0,115,82,66,117,102
 DB 58,32,32,37,100,13,10,0,82,84,105,109,101,79,58,32
 DB 37,100,13,10,0,88,84,105,109,101,79,58,32,37,100,13
 DB 10,0,69,114,114,111,114,32,111,110,32,68,101,118,105,99
 DB 101,32,73,110,105,116,58,32,37,100,13,10,0,79,112,101
 DB 110,67,111,109,109,67,32,69,82,82,79,82,58,32,37,100
 DB 32,13,10,0,67,111,109,109,117,110,105,99,97,116,105,111
 DB 110,115,32,80,111,114,116,32,73,110,105,116,105,97,108,105
 DB 122,101,100,46,13,10,0,87,114,105,116,101,66,121,116,101
 DB 67,69,114,114,111,114,58,32,37,100,32,13,10,0,10,0
PUBLIC _key DD 0h
PUBLIC _com DB 64 DUP(0)


.CODE
EXTRN _fopen: NEAR
EXTRN _fclose: NEAR
EXTRN _remove: NEAR
EXTRN _rename: NEAR
EXTRN _fgetc: NEAR
EXTRN _fgets: NEAR
EXTRN _fputs: NEAR
EXTRN _fputc: NEAR
EXTRN _printf: NEAR
EXTRN _sprintf: NEAR
EXTRN _fprintf: NEAR
EXTRN _ftell: NEAR
EXTRN _rewind: NEAR
EXTRN _fseek: NEAR
EXTRN _fread: NEAR
EXTRN _fwrite: NEAR
EXTRN _iscntrl: NEAR
EXTRN _isspace: NEAR
EXTRN _isdigit: NEAR
EXTRN _isupper: NEAR
EXTRN _islower: NEAR
EXTRN _ispunct: NEAR
EXTRN _isalpha: NEAR
EXTRN _isxdigit: NEAR
EXTRN _isalnum: NEAR
EXTRN _isgraph: NEAR
EXTRN _toupper: NEAR
EXTRN _tolower: NEAR
EXTRN _strcpy: NEAR
EXTRN _strncpy: NEAR
EXTRN _strcat: NEAR
EXTRN _strncat: NEAR
EXTRN _strcmp: NEAR
EXTRN _strncmp: NEAR
EXTRN _strlen: NEAR
EXTRN _strchr: NEAR
PUBLIC _main:
	PUSH EBP
	MOV EBP,ESP
	SUB ESP,7
	PUSH 31
	CALL FWORD PTR _SetNormVid
	CALL FWORD PTR _ClrScr
	PUSH  OFFSET L_lit+0
	MOV EDI, 0
	CALL _printf
	ADD ESP,4
	PUSH  OFFSET L_lit+46
	MOV EDI, 0
	CALL _printf
	ADD ESP,4
	PUSH 6
	PUSH OFFSET _com
	PUSH 64
	LEA EAX,WORD PTR [EBP-4]
	PUSH EAX
	CALL FWORD PTR _DeviceStat
	MOV WORD PTR [EBP-2],AX
	MOVSX EAX,WORD PTR [EBP-2]
	AND EAX,EAX
	JZ L_1
	PUSH 7
	CALL FWORD PTR _SetNormVid
	CALL FWORD PTR _ClrScr
	PUSH  OFFSET L_lit+89
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	CALL FWORD PTR _ExitJob
L_1:
	MOV ESI,OFFSET _com
	MOV EAX,9600
	MOV DWORD PTR [ESI+12],EAX
	MOV ESI,OFFSET _com
	XOR EAX,EAX
	MOV BYTE PTR [ESI+16],AL
	MOV ESI,OFFSET _com
	MOV EAX,8
	MOV BYTE PTR [ESI+17],AL
	MOV ESI,OFFSET _com
	MOV EAX,1
	MOV BYTE PTR [ESI+18],AL
	PUSH  OFFSET L_lit+116
	MOV ESI,OFFSET _com
	XOR EAX,EAX
	MOV AL,BYTE PTR [ESI+19]
	PUSH EAX
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH  OFFSET L_lit+129
	MOV ESI,OFFSET _com
	PUSH DWORD PTR [ESI+20]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH  OFFSET L_lit+142
	MOV ESI,OFFSET _com
	PUSH DWORD PTR [ESI+24]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH  OFFSET L_lit+155
	MOV ESI,OFFSET _com
	PUSH DWORD PTR [ESI+28]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH  OFFSET L_lit+168
	MOV ESI,OFFSET _com
	PUSH DWORD PTR [ESI+36]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH  OFFSET L_lit+181
	MOV ESI,OFFSET _com
	PUSH DWORD PTR [ESI+32]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	PUSH 6
	PUSH OFFSET _com
	PUSH 64
	CALL FWORD PTR _DeviceInit
	MOV WORD PTR [EBP-2],AX
	MOVSX EAX,WORD PTR [EBP-2]
	AND EAX,EAX
	JZ L_2
	PUSH 7
	CALL FWORD PTR _SetNormVid
	CALL FWORD PTR _ClrScr
	PUSH  OFFSET L_lit+194
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	CALL FWORD PTR _ExitJob
L_2:
	PUSH 6
	PUSH 10
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	LEA EAX,WORD PTR [EBP-4]
	PUSH EAX
	CALL FWORD PTR _DeviceOp
	MOV WORD PTR [EBP-2],AX
	MOVSX EAX,WORD PTR [EBP-2]
	AND EAX,EAX
	JZ L_3
	PUSH 7
	CALL FWORD PTR _SetNormVid
	CALL FWORD PTR _ClrScr
	PUSH  OFFSET L_lit+221
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	CALL FWORD PTR _ExitJob
L_3:
	PUSH  OFFSET L_lit+244
	MOV EDI, 0
	CALL _printf
	ADD ESP,4
	MOV EAX,1
	MOV BYTE PTR [EBP-7],AL
L_4:
	MOVSX EAX,BYTE PTR [EBP-7]
	AND EAX,EAX
	JZ L_5
	PUSH OFFSET _key
	XOR EAX,EAX
	PUSH EAX
	CALL FWORD PTR _ReadKbd
	AND EAX,EAX
	JNZ L_6
	MOV EAX,_key
	AND EAX,127
	MOV BYTE PTR [EBP-5],AL
	MOV EAX,_key
	AND EAX,12288
	JZ L_7
	XOR EAX,EAX
	MOV AL,BYTE PTR [EBP-5]
	PUSH EAX
	CALL _toupper
	JMP L_9
L_10:
	PUSH 6
	PUSH 11
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	LEA EAX,WORD PTR [EBP-4]
	PUSH EAX
	CALL FWORD PTR _DeviceOp
	MOV WORD PTR [EBP-2],AX
	PUSH 7
	CALL FWORD PTR _SetNormVid
	CALL FWORD PTR _ClrScr
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	CALL FWORD PTR _ExitJob
	JMP L_8
L_11:
	JMP L_8
L_9:
	CMP EAX,81
	JE L_10
	JMP L_11
L_8:
	JMP L_12
L_7:
	PUSH 6
	PUSH 32
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	LEA EAX,BYTE PTR [EBP-5]
	PUSH EAX
	CALL FWORD PTR _DeviceOp
	MOV WORD PTR [EBP-2],AX
	MOVSX EAX,WORD PTR [EBP-2]
	AND EAX,EAX
	JZ L_13
	PUSH  OFFSET L_lit+279
	MOVSX EAX,WORD PTR [EBP-2]
	PUSH EAX
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	JMP L_14
L_13:
	XOR EAX,EAX
	MOV AL,BYTE PTR [EBP-5]
	CMP EAX,13
	SETE AL
	AND AL,AL
	JZ L_15
	MOV EAX,10
	MOV BYTE PTR [EBP-5],AL
	PUSH 6
	PUSH 32
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	LEA EAX,BYTE PTR [EBP-5]
	PUSH EAX
	CALL FWORD PTR _DeviceOp
	MOV WORD PTR [EBP-2],AX
L_15:
L_14:
L_12:
L_6:
	PUSH 6
	PUSH 31
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	LEA EAX,BYTE PTR [EBP-5]
	PUSH EAX
	CALL FWORD PTR _DeviceOp
	MOV WORD PTR [EBP-2],AX
	MOVSX EAX,WORD PTR [EBP-2]
	AND EAX,EAX
	JNZ L_16
	LEA EAX,BYTE PTR [EBP-5]
	PUSH EAX
	PUSH 1
	PUSH 31
	CALL FWORD PTR _TTYOut
	XOR EAX,EAX
	MOV AL,BYTE PTR [EBP-6]
	CMP EAX,13
	SETE AL
	AND AL,AL
	JZ L_17
	XOR EAX,EAX
	MOV AL,BYTE PTR [EBP-5]
	CMP EAX,10
	SETNE AL
	AND AL,AL
L_17:
	JZ L_18
	PUSH  OFFSET L_lit+302
	PUSH 1
	PUSH 31
	CALL FWORD PTR _TTYOut
L_18:
	XOR EAX,EAX
	MOV AL,BYTE PTR [EBP-5]
	MOV BYTE PTR [EBP-6],AL
L_16:
	JMP L_4
L_5:
	MOV ESP,EBP
	POP EBP
	RETN
