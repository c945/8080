	cpu	8080

	org	0

	lxi	sp,0ffffh
	dcx	sp		;SP=fffe
	call	test1
	hlt

test1:
	lxi	b,1234h		;BC=1234
	mov	h,b
	mov	l,c		;HL=1234
	mov	e,m		;E=55
;
	mvi	d,0aah		;D=AA
	mvi	m,44h		;$1234 = 44
	lda	1234h		;A=44
	sta	2000h		;$2000 = 44
	lxi	h,2000h		;HL=2000
	mov	e,m		;E=44
;
	mvi	a,0		;A=0
	lxi	b,2000h		;BC=2000
	ldax	b		;A=44
;
	dcx	b		;BC=1FFF
	inx	b		;BC=2000
;
	dad	d		;HL=CA44
	dad	b		;HL=EA44
	dad	b		;HL=0A44 CY=1
	ret
;
$$loop
	cmc			;CY=0
	stc			;CY=1
	jmp	$$loop

	ret

	org	1234h
	db	55h

