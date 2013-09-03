	cpu	8080
	org	0
	mvi	a,80h		;A=80
	cpi	0
	cpi	7fh
	cpi	80h
	cpi	81h
	mvi	a,0
	cpi	0
	cpi	7fh
	cpi	80h
	cpi	0ffh

	hlt
