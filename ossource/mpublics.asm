;These are ASSEMBER publics defined so you can use
;CALL FWORD PTR _WaitMsg in your assembly language
;programs with MMURTL.  FAR calls are called indirectly
;to the call gate selector address.
;
;This should also be included in the Assembler ATF file
;for your C programs when you assemble (build the run file).
;Each call takes 6 bytes, so you can taylor this
;to remove calls you don't need to save memory
;if you like.

.DATA
.ALIGN DWORD

PUBLIC _WaitMsg			DF 00000000h:40h
PUBLIC _SendMsg			DF 00000000h:48h
PUBLIC _ISendMsg		DF 00000000h:50h
PUBLIC _SetPriority		DF 00000000h:58h
PUBLIC _Request			DF 00000000h:60h
PUBLIC _Respond			DF 00000000h:68h
PUBLIC _CheckMsg		DF 00000000h:70h
PUBLIC _NewTask			DF 00000000h:78h
PUBLIC _AllocExch		DF 00000000h:80h
PUBLIC _DeAllocExch		DF 00000000h:88h
PUBLIC _Sleep			DF 00000000h:90h
PUBLIC _Alarm			DF 00000000h:98h
PUBLIC _AllocOSPage		DF 00000000h:0A0h
PUBLIC _AllocPage		DF 00000000h:0A8h
PUBLIC _RegisterSvc		DF 00000000h:0B0h
PUBLIC _DMASetUp		DF 00000000h:0B8h
PUBLIC _ReadKBD			DF 00000000h:0C0h
PUBLIC _AddCallGate		DF 00000000h:0C8h
PUBLIC _AddIDTGate		DF 00000000h:0D0h
PUBLIC _EndOfIRQ		DF 00000000h:0D8h
PUBLIC _MaskIRQ			DF 00000000h:0E0h
PUBLIC _UnMaskIRQ		DF 00000000h:0E8h
PUBLIC _SetIRQVector	DF 00000000h:0F0h
PUBLIC _GetIRQVector	DF 00000000h:0F8h
PUBLIC _InitDevDr		DF 00000000h:100h
PUBLIC _DeviceInit		DF 00000000h:108h
PUBLIC _DeviceOp		DF 00000000h:110h
PUBLIC _DeviceStat		DF 00000000h:118h
PUBLIC _Beep			DF 00000000h:120h
PUBLIC _Tone			DF 00000000h:128h
PUBLIC _KillAlarm		DF 00000000h:130h
PUBLIC _MicroDelay		DF 00000000h:138h
PUBLIC _SpawnTask		DF 00000000h:140h
PUBLIC _GetCMOSTime		DF 00000000h:148h
PUBLIC _GetTimerTick	DF 00000000h:150h
PUBLIC _OutByte			DF 00000000h:158h
PUBLIC _OutWord	 		DF 00000000h:160h
PUBLIC _OutDWord		DF 00000000h:168h
PUBLIC _InByte			DF 00000000h:170h
PUBLIC _InWord			DF 00000000h:178h
PUBLIC _InDWord	 		DF 00000000h:180h
PUBLIC _ReadCMOS		DF 00000000h:188h
PUBLIC _CopyData		DF 00000000h:190h
PUBLIC _CopyDataR		DF 00000000h:198h
PUBLIC _FillData		DF 00000000h:1A0h
PUBLIC _CompareNCS		DF 00000000h:1A8h
PUBLIC _Compare	 		DF 00000000h:1B0h
PUBLIC _InWords	 		DF 00000000h:1B8h
PUBLIC _OutWords		DF 00000000h:1C0h
PUBLIC _MoveRequest		DF 00000000h:1C8h
PUBLIC _DeAllocPage		DF 00000000h:1D0h
PUBLIC _LoadNewJob 		DF 00000000h:1D8h
PUBLIC _SetVidOwner		DF 00000000h:1E0h
PUBLIC _GetVidOwner		DF 00000000h:1E8h
PUBLIC _ClrScr			DF 00000000h:1F0h
PUBLIC _TTYOut			DF 00000000h:1F8h
PUBLIC _PutVidChars		DF 00000000h:200h
PUBLIC _SetXY			DF 00000000h:208h
PUBLIC _GetXY	 		DF 00000000h:210h
PUBLIC _EditLine		DF 00000000h:218h
PUBLIC _GetTSSExch		DF 00000000h:220h
PUBLIC _OpenFile		DF 00000000h:228h
PUBLIC _CloseFile		DF 00000000h:230h
PUBLIC _ReadBlock		DF 00000000h:238h
PUBLIC _WriteBlock		DF 00000000h:240h
PUBLIC _ReadBytes		DF 00000000h:248h
PUBLIC _WriteBytes		DF 00000000h:250h
PUBLIC _GetFileLFA		DF 00000000h:258h
PUBLIC _SetFileLFA		DF 00000000h:260h
PUBLIC _GetFileSize		DF 00000000h:268h
PUBLIC _CreateFile		DF 00000000h:270h
PUBLIC _RenameFile		DF 00000000h:278h
PUBLIC _DeleteFile		DF 00000000h:280h
PUBLIC _GetpJCB			DF 00000000h:288h
PUBLIC _QueryPages		DF 00000000h:290h
PUBLIC _GetPhyAdd		DF 00000000h:298h
PUBLIC _ScrollVid		DF 00000000h:2A0h
PUBLIC _GetDirSector	DF 00000000h:2A8h
PUBLIC _GetJobNum		DF 00000000h:2B0h
PUBLIC _ExitJob			DF 00000000h:2B8h
PUBLIC _SetUserName		DF 00000000h:2C0h
PUBLIC _GetUserName		DF 00000000h:2C8h
PUBLIC _SetCmdLine		DF 00000000h:2D0h
PUBLIC _GetCmdLine		DF 00000000h:2D8h
PUBLIC _SetPath			DF 00000000h:2E0h
PUBLIC _GetPath			DF 00000000h:2E8h
PUBLIC _SetExitJob		DF 00000000h:2F0h
PUBLIC _GetExitJob		DF 00000000h:2F8h
PUBLIC _SetSysIn		DF 00000000h:300h
PUBLIC _SetSysOut		DF 00000000h:308h
PUBLIC _GetSysIn		DF 00000000h:310h
PUBLIC _GetSysOut		DF 00000000h:318h
PUBLIC _PutVidAttrs		DF 00000000h:320h
PUBLIC _GetVidChar		DF 00000000h:328h
PUBLIC _SetNormVid		DF 00000000h:330h
PUBLIC _GetNormVid		DF 00000000h:338h
PUBLIC _Chain			DF 00000000h:340h
PUBLIC _SetFileSize		DF 00000000h:348h
PUBLIC _GetCMOSDate		DF 00000000h:350h
PUBLIC _CreateDir		DF 00000000h:358h
PUBLIC _DeleteDir		DF 00000000h:360h
PUBLIC _DeAliasMem		DF 00000000h:368h
PUBLIC _AliasMem		DF 00000000h:370h
PUBLIC _GetDMACount		DF 00000000h:378h
PUBLIC _AllocDMAPage	DF 00000000h:380h
PUBLIC _SetJobName		DF 00000000h:388h
PUBLIC _KillJob			DF 00000000h:390h
PUBLIC _GetSystemDisk	DF 00000000h:398h
PUBLIC _UnRegisterSvc	DF 00000000h:3A0h

;============== End of Module =======================
