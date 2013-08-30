	cpu	8080
	org	0
	mvi	b,1		;B=1
	dcr	b		;B=0  S=0 Z=1 P=1 C=0
	dcr	b		;B=FF S=1 Z=0 P=1 C=0

	hlt
