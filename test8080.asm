# vim:set ts=4:
#
	cpu 8080

	nop
	lxi	b,0
	stax	b
	inx	b
	inr	b
	dcr	b
	mvi	b,0
	rlc
	nop		;08 ex AF,AF'
	dad b
	ldax	b
	dcx	b
	inr	c
	dcr	c
	mvi	c,0
	rrc
	nop		;10 djnz e
	lxi	d,0
	stax	d
	inx	d
	inr	d
	dcr	d
	mvi	d,0
	ral
	nop		;18 jr e
	dad	d
	ldax	d
	dcx	d
	inr	e
	dcr	e
	mvi	e,0
	rar
	nop		;20 jr nz,e
	lxi	h,0
	shld	0
	inx	h
	inr	h
	dcr	h
	mvi	h,0
	daa
	nop		;28 jr z,e
	dad	h
	lhld	0
	dcx	h
	inr	l
	dcr	l
	mvi	l,0
	cma
	nop		;30 jr nc,e
	lxi	sp,0
	sta	0
	inx	sp
	inr	m
	dcr	m
	mvi	m,0
	stc
	nop		;38 jr c,e
	dad	sp
	lda	0
	dcx	sp
	inr	a
	dcr	a
	mvi	a,0
	cmc
	mov	b,b
	mov	b,c
	mov	b,d
	mov	b,e
	mov	b,h
	mov	b,l
	mov	b,m
	mov	b,a
	mov	c,b
	mov	c,c
	mov	c,d
	mov	c,e
	mov	c,h
	mov	c,l
	mov	c,m
	mov	c,a
	mov	d,b
	mov	d,c
	mov	d,d
	mov	d,e
	mov	d,h
	mov	d,l
	mov	d,m
	mov	d,a
	mov	e,b
	mov	e,c
	mov	e,d
	mov	e,e
	mov	e,h
	mov	e,l
	mov	e,m
	mov	e,a
	mov	h,b
	mov	h,c
	mov	h,d
	mov	h,e
	mov	h,h
	mov	h,l
	mov	h,m
	mov	h,a
	mov	l,b
	mov	l,c
	mov	l,d
	mov	l,e
	mov	l,h
	mov	l,l
	mov	l,m
	mov	l,a
	mov	m,b
	mov	m,c
	mov	m,d
	mov	m,e
	mov	m,h
	mov	m,l
	hlt
	mov	m,a
	mov	a,b
	mov	a,c
	mov	a,d
	mov	a,e
	mov	a,h
	mov	a,l
	mov	a,m
	mov	a,a
	add	b
	add	c
	add	d
	add	e
	add	h
	add	l
	add	m
	add	a
	adc	b
	adc	c
	adc	d
	adc	e
	adc	h
	adc	l
	adc	m
	adc	a
	sub	b
	sub	c
	sub	d
	sub	e
	sub	h
	sub	l
	sub	m
	sub	a
	sbb	b
	sbb	c
	sbb	d
	sbb	e
	sbb	h
	sbb	l
	sbb	m
	sbb	a
	ana	b
	ana	c
	ana	d
	ana	e
	ana	h
	ana	l
	ana	m
	ana	a
	xra	b
	xra	c
	xra	d
	xra	e
	xra	h
	xra	l
	xra	m
	xra	a
	ora	b
	ora	c
	ora	d
	ora	e
	ora	h
	ora	l
	ora	m
	ora	a
	cmp	b
	cmp	c
	cmp	d
	cmp	e
	cmp	h
	cmp	l
	cmp	m
	cmp	a
	rnz
	pop	b
	jnz	0
	jmp	0
	cnz	0
	push	b
	adi	0
	rst	0
	rz
	ret
	jz	0
	nop		;CB xx
	cz	0
	call	0
	aci	0
	rst	1
	rnc
	pop	d
	jnc	0
	out	0
	cnc	0
	push	d
	sui	0
	rst	2
	rc
	nop		;D9 EXX
	jc	0
	in	0
	cc	0
	nop		;DD xx or DD CB xx
	sbi	0
	rst	3
	rpo
	pop	h
	jpo	0
	xthl
	cpo	0
	push	h
	ani	0
	rst	4
	rpe
	pchl
	jpe	0
	xchg
	cpe	0
	nop		;ED xx
	xri	0
	rst	5
	rp
	pop	psw
	jp	0
	di
	cp	0
	push	psw
	ori	0
	rst	6
	rm
	sphl
	jm	0
	ei
	cm	0
	nop		;FD xx
	cpi	0
	rst	7

