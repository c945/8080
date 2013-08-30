	cpu	8080
	org	0
	mvi	d,0ffh		;D=FF
	inr	d		;D=0  S=0 Z=1 P=1 C=0
	inr	d		;D=1  S=0 Z=0 P=0 C=0
	inr	d		;D=2  S=0 Z=0 P=0 C=0
	inr	d		;D=3  S=0 Z=0 P=1 C=0
	lxi	h,8000H
	mvi	m,07eh;		;(HL)=7E
	inr	m		;(HL)=7F S=0 Z=0 P=0 C=0
	inr	m		;(HL)=80 S=1 Z=0 P=0 C=0 (AC=1)

	hlt
