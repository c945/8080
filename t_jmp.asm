	cpu 8080
	lxi b,0ffffh
	push b
	pop psw
	jc $+4
	hlt
	jm $+4
	hlt
	jpe $+4
	hlt
	jz  $+4
	hlt
	nop

	push b
	pop psw
	jnz  $+4
	hlt
	jpo $+4
	hlt
	jp $+4
	hlt
	jnc $+4
	hlt
	nop
	hlt
