	cpu	8080
	org	0
	mvi	a,0f5h		;A=F5
	mvi	b,0bfh		;B=Bf
	ana	b		;A=B5
	ani	0fh		;A=05

	hlt
