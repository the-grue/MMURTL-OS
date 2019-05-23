sin() asm	/*Compute the sin of the angle in fpacc1, assumes radians*/
{
		lcall	mov1to2
		lcall	pshf2		Save angle
		lcall	fltmul		Compute angle squared
		lcall	mov1to2
		lcall	pshf2		Save angle squared
		mov		A,#c1d72
		mov		B,#=c1d72
		lcall	getfpc2		Load 1/72 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltsub
		lcall	popf2		Get angle squared
		lcall	pshf2
		lcall	fltmul
		mov		A,#c1d42
		mov		B,#=c1d42
		lcall	getfpc2		Load 1/42 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltadd
		lcall	popf2		Get angle squared
		lcall	pshf2
		lcall	fltmul
		mov		A,#c1d20
		mov		B,#=c1d20
		lcall	getfpc2		Load 1/20 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltsub
		lcall	popf2		Get angle squared
		lcall	fltmul
		mov		A,#c1d6
		mov		B,#=c1d6
		lcall	getfpc2		Load 1/6 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltadd
		lcall	popf2		Get angle
		lcall	fltmul
}

asm {
c1d72	DB $3D,$63,$8E,$39
c1d42	DB $3D,$43,$0C,$31
c1d20	DB $3E,$4C,$CC,$CD
c1d6	DB $3F,$2A,$AA,$AB
c1		DB $40,$80,$00,$00
}

cos() asm	/*Compute the cos of the angle in fpacc1, assumes radians*/
{
		lcall	mov1to2
		lcall	fltmul		Compute angle squared
		lcall	mov1to2
		lcall	pshf2		Save angle squared
		mov		A,#c1d56
		mov		B,#=c1d56
		lcall	getfpc2		Load 1/56 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltsub
		lcall	popf2		Get angle squared
		lcall	pshf2
		lcall	fltmul
		mov		A,#c1d30
		mov		B,#=c1d30
		lcall	getfpc2		Load 1/30 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltadd
		lcall	popf2		Get angle squared
		lcall	pshf2
		lcall	fltmul
		mov		A,#c1d12
		mov		B,#=c1d12
		lcall	getfpc2		Load 1/12 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltsub
		lcall	popf2		Get angle squared
		lcall	fltmul
		mov		A,#c1d2
		mov		B,#=c1d2
		lcall	getfpc2		Load 1/2 into fpacc2
		lcall	fltmul
		mov		A,#c1
		mov		B,#=c1
		lcall	getfpc2		Load 1 into fpacc2
		lcall	fltadd
}

asm {
c1d56	DB $3D,$92,$49,$25
c1d30	DB $3E,$08,$88,$89
c1d12	DB $3E,$AA,$AA,$AB
c1d2	DB $40,$00,$00,$00
}
