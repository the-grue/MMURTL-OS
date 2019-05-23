
.DATA
EXTRN _AllocExch FWORD
EXTRN _DeAllocExch FWORD
EXTRN _GetTSSExch FWORD
EXTRN _SetPriority FWORD
EXTRN _NewTask FWORD
EXTRN _SpawnTask FWORD
EXTRN _SendMsg FWORD
EXTRN _ISendMsg FWORD
EXTRN _WaitMsg FWORD
EXTRN _CheckMsg FWORD
EXTRN _Request FWORD
EXTRN _MoveRequest FWORD
EXTRN _Respond FWORD
L_lit DB 69,114,114,111,114,32,37,100,32,97,108,108,111,99,97,116
 DB 105,110,103,32,69,120,99,104,97,110,103,101,46,13,10,0
 DB 78,85,77,66,69,82,83,32,0,69,114,114,111,114,32,37
 DB 100,32,102,114,111,109,32,82,101,113,117,101,115,116,46,13
 DB 10,0,69,114,114,111,114,32,37,100,32,102,114,111,109,32
 DB 87,97,105,116,77,115,103,46,13,10,0,69,114,114,111,114
 DB 32,37,100,32,102,114,111,109,32,78,85,77,66,69,82,83
 DB 32,83,101,114,118,105,99,101,46,13,10,0,78,85,77,66
 DB 69,82,83,32,83,101,114,118,105,99,101,32,103,97,118,101
 DB 32,111,117,116,32,110,117,109,98,101,114,58,32,37,100,46
 DB 13,10,0
PUBLIC _Number DD 0h
PUBLIC _Exch DD 0h
PUBLIC _Message DD 2 DUP(0)


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
PUBLIC _main:
	PUSH EBP
	MOV EBP,ESP
	SUB ESP,8
	PUSH OFFSET _Exch
	CALL FWORD PTR _AllocExch
	MOV DWORD PTR [EBP-4], EAX
	MOV EAX,DWORD PTR [EBP-4]
	AND EAX,EAX
	JZ L_1
	PUSH OFFSET L_lit+0
	PUSH DWORD PTR [EBP-4]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
L_1:
	PUSH OFFSET L_lit+32
	PUSH 1
	PUSH _Exch
	LEA EAX,DWORD PTR [EBP-8]
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	PUSH OFFSET _Number
	PUSH 4
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	XOR EAX,EAX
	PUSH EAX
	CALL FWORD PTR _Request
	MOV DWORD PTR [EBP-4], EAX
	MOV EAX,DWORD PTR [EBP-4]
	AND EAX,EAX
	JZ L_2
	PUSH OFFSET L_lit+41
	PUSH DWORD PTR [EBP-4]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
L_2:
	MOV EAX,DWORD PTR [EBP-4]
	AND EAX,EAX
	JNZ L_3
	PUSH _Exch
	PUSH OFFSET _Message
	CALL FWORD PTR _WaitMsg
	MOV DWORD PTR [EBP-4], EAX
	MOV EAX,DWORD PTR [EBP-4]
	AND EAX,EAX
	JZ L_4
	PUSH OFFSET L_lit+66
	PUSH DWORD PTR [EBP-4]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	JMP L_5
L_4:
	MOV EAX,4
	MOV ESI,OFFSET _Message
	ADD ESI,EAX
	MOV EAX,DWORD PTR [ESI]
	AND EAX,EAX
	JZ L_6
	PUSH OFFSET L_lit+91
	MOV EAX,4
	MOV ESI,OFFSET _Message
	ADD ESI,EAX
	PUSH DWORD PTR [ESI]
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
	JMP L_7
L_6:
	PUSH OFFSET L_lit+124
	PUSH _Number
	MOV EDI, 4
	CALL _printf
	ADD ESP,8
L_7:
L_5:
L_3:
	PUSH _Exch
	CALL FWORD PTR _DeAllocExch
	MOV ESP,EBP
	POP EBP
	RETN
