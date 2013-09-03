#include	<stdio.h>
#include	<stdlib.h>
#include	"i8080.h"

// internal funcion
static int	mov( i8080 *, u8 *, u8);
static int	mvi( i8080 *, u8 *, u8);
static int	lxi( i8080 *, u8 *, u8);
static int	hlt( i8080 *, u8 *, u8);
static int	ldax(i8080 *, u8 *, u8);
static int	stax(i8080 *, u8 *, u8);
static int	dcr( i8080 *, u8 *, u8);
static int	inx( i8080 *, u8 *, u8);
static int	dcx( i8080 *, u8 *, u8);
static int	dad( i8080 *, u8 *, u8);
static int	add( i8080 *, u8 *, u8, int);
static int	adi( i8080 *, u8 *, u8, int);
static int	sub( i8080 *, u8 *, u8, int);
static int	sbi( i8080 *, u8 *, u8, int);
static int	inr( i8080 *, u8 *, u8);
static u8	*cvtregs(i8080 *, int);
static u16	*cvtregp_sp(i8080 *, int);
static u16	*cvtregp_af(i8080 *, int);
static u16	getword(i8080 *, u8 *);
static u8	getbyte(i8080 *, u8 *);
static u16	popword(i8080 *, u8 *);
static u16	pushword(i8080 *, u8 *, u16);
static u8	getmem(u8 *, u16);
static void	setmem(u8 *, u16, u8);
static void	illegal_ins(u8);
static void	setflags_szp(i8080 *, u16);
static void	setflags_add(i8080 *, u16, u8);
static void	setflags_sub(i8080 *, u16, u8);
static u16	u16_parity(u16);

int	i8080_run(i8080 *cpu, u8 *mem)
{
	u8 	op, sop, wop, wop2;
	u16	word, *regp;
	int	ret;

	ret = 0;
	/* op = getmem(mem, cpu->PC.W); */
	op = mem[cpu->PC.W];
	sop = (op & 0xc7);	/* OPCODE for Reg A/B/C/D/E/H/L/M	 */
	wop = (op & 0xcf);	/* OPCODE for Reg Pair BC/DE/HL/SP	 */
	wop2 = (op & 0xef);	/* OPCODE for Reg Pair BC/DE		 */
	if(op == 0) {			/* NOP */
		cpu->clocks += 4;
		cpu->PC.W++;
	}
	else if((op & 0xc0) == 0x40) {	/* MOV r,r / MOV M,r / MOV r,M / HLT */
		ret = mov(cpu, mem, op);
	}
	else if(sop == 0x06) {	/* MVI r,data / MOV M,data */
		ret = mvi(cpu, mem, op);
	}
	else if(wop == 0x01) { 	/* LXI rp, data data */
		ret = lxi(cpu, mem, op);
	}
	else if(op == 0x3a) {		/* LDA addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		cpu->i8080ACC = getmem(mem,word);
		cpu->clocks += 13;
	}
	else if(op == 0x2a) {		/* LHLD addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		cpu->HL.B.l = getmem(mem, word);
		cpu->HL.B.h = getmem(mem, word+1);
		cpu->clocks += 16;
	}
	else if(op == 0x32) {		/* STA addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		setmem(mem, word, cpu->i8080ACC);
		cpu->clocks += 13;
	}
	else if(op == 0x22) {		/* SHLD addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		setmem(mem, word, cpu->HL.B.l);
		setmem(mem, word+1, cpu->HL.B.h);
		cpu->clocks += 16;
	}
	else if(wop2 == 0x0a) { 	/* LDAX rp */
		ret = ldax(cpu, mem, op);
	}
	else if(wop2 == 0x02) { 	/* STAX rp */
		ret = stax(cpu, mem, op);
	}
	else if(op == 0xeb) {		/* XCHG */
		word = cpu->HL.W;
		cpu->HL.W = cpu->DE.W;
		cpu->DE.W = word;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(wop == 0x03) {	/* INX rp */
		ret = inx(cpu, mem, op);
	}
	else if(wop == 0x0b) {	/* DCX rp */
		ret = dcx(cpu, mem, op);
	}
	else if(wop == 0x09) {	/* dad rp */
		ret = dad(cpu, mem, op);
	}
	else if(op == 0x2f) {		/* CMA */
		cpu->i8080ACC =  ~cpu->i8080ACC;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(op == 0x3f) {		/* CMC */
		cpu->i8080FLAGS ^= i8080F_CY;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(op == 0x37) {		/* STC */
		cpu->i8080FLAGS |= i8080F_CY;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(op == 0xc3) {		/* JMP adr,adr */
		cpu->PC.W++;
		cpu->PC.W = getword(cpu, mem);
		cpu->clocks += 10;
	}
	else if(op == 0xcd) {		/* CALL adr,adr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		pushword(cpu, mem, cpu->PC.W);
		cpu->PC.W = word;
		cpu->clocks += 17;
	}
	else if(op == 0xc9) {		/* RET */
		cpu->PC.W++;
		word = popword(cpu, mem);
		cpu->PC.W = word;
		cpu->clocks += 10;
	}
	else if((op & 0xf8) == 0x80 ) {	/* ADD r */
		ret = add(cpu, mem, op, 0);
	}
	else if((op & 0xf8) == 0x88 ) {	/* ADC r */
		ret = add(cpu, mem, op, 1);
	}
	else if(op == 0xc6) {		/* ADI byte */
		ret = adi(cpu, mem, op, 0);
	}
	else if(op == 0xce) {		/* ACI byte */
		ret = adi(cpu, mem, op, 1);
	}
	else if((op & 0xf8) == 0x90 ) {	/* SUB r */
		ret = sub(cpu, mem, op, 0);
	}
	else if((op & 0xf8) == 0x98 ) {	/* SBB r */
		ret = sub(cpu, mem, op, 1);
	}
	else if(op == 0xd6) {		/* SUI byte */
		ret = sbi(cpu, mem, op, 0);
	}
	else if(op == 0xde) {		/* SBI byte */
		ret = sbi(cpu, mem, op, 1);
	}
	else if((op & 0xc7) == 0x4 ) {	/* INR r(M) */
		ret = inr(cpu, mem, op);
	}
	else if((op & 0xc7) == 0x5 ) {	/* DCR r(M) */
		ret = dcr(cpu, mem, op);
	}
	else if((op & 0xcf) == 0xc5 ) {	/* PUSH rp */
		regp = cvtregp_af(cpu, (op & 0x30) >> 4);
		pushword(cpu, mem, *regp);
		cpu->PC.W++;
		cpu->clocks += 11;
		ret = 0;
	}
	else if((op & 0xcf) == 0xc1 ) {	/* POP rp */
		regp = cvtregp_af(cpu, (op & 0x30) >> 4);
		*regp = popword(cpu, mem);
		cpu->PC.W++;
		cpu->clocks += 11;
		ret = 0;
	}
	else if(op == 0xfb) {		/* EI */
		cpu->int_f = 1;
		cpu->PC.W++;
		cpu->clocks += 4;
		ret = 0;
	}
	else if(op == 0xf3) {		/* DI */
		cpu->int_f = 0;
		cpu->PC.W++;
		cpu->clocks += 4;
		ret = 0;
	}
	else {
		illegal_ins(op);
		ret = -1;
	}
	return ret;
}

static int	dcr(i8080 *cpu, u8 *mem, u8 op)
{
	u8	*sss, ans;

	sss = cvtregs(cpu, ((op >>3) & 0x07));
	if(sss == NULL) {	/* DCR r(M) */
		ans = getmem(mem, cpu->HL.W) - 1;
		setmem(mem, cpu->HL.W, ans);
		cpu->clocks += 10;
	} else {
		ans = --*sss;
		cpu->clocks += 5;
	}
	setflags_szp(cpu, ans);
	if((ans & 0x0f) == 0x0f) {
		cpu->i8080FLAGS |= i8080F_AC;
	} else {
		cpu->i8080FLAGS &= ~i8080F_AC;
	}
	cpu->PC.W++;
	return 0;
}

static int	inr(i8080 *cpu, u8 *mem, u8 op)
{
	u8	*sss, ans;

	sss = cvtregs(cpu, ((op >>3) & 0x07));
	if(sss == NULL) {	/* INR r(M) */
		ans = getmem(mem, cpu->HL.W) + 1;
		setmem(mem, cpu->HL.W, ans);
		cpu->clocks += 10;
	} else {
		ans = ++*sss;
		cpu->clocks += 5;
	}

	setflags_szp(cpu, ans);
	if((ans & 0x0f) == 0) {
		cpu->i8080FLAGS |= i8080F_AC;
	} else {
		cpu->i8080FLAGS &= ~i8080F_AC;
	}
	cpu->PC.W++;
	return 0;
}

static int	sbi(i8080 *cpu, u8 *mem, u8 op, int with_carry)
{
	u8	imm, CY=0, old;
	u16	ans;

	if((with_carry) && cpu->i8080FLAGS & i8080F_CY) {
		CY = 1;
	} else {
		CY = 0;
	}
	imm = getbyte(cpu, mem);
	ans = cpu->i8080ACC - imm - CY;
	old = cpu->i8080ACC;	/* save old ACC */
	cpu->i8080ACC = ans;
	setflags_sub(cpu, ans, old);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
}
static int	sub(i8080 *cpu, u8 *mem, u8 op, int with_carry)
{
	u8	*sss, CY=0, old;
	u16	ans;

	sss = cvtregs(cpu, (op & 0x07));
	if((with_carry) && cpu->i8080FLAGS & i8080F_CY) {
		CY = 1;
	} else {
		CY = 0;
	}
	if(sss == NULL) {	/* ADD(C) M */
		ans = cpu->i8080ACC - getmem(mem, cpu->HL.W) - CY;
		old = cpu->i8080ACC;	/* save old ACC */
		cpu->i8080ACC = ans;
		cpu->clocks += 7;
	} else {
		ans = cpu->i8080ACC - *sss - CY;
		old = cpu->i8080ACC;	/* save old ACC */
		cpu->i8080ACC = ans;
		cpu->clocks += 4;
	}
	setflags_sub(cpu, ans, old);
	cpu->PC.W++;
	return 0;
}

static int	adi(i8080 *cpu, u8 *mem, u8 op, int with_carry)
{
	u8	imm, CY=0, old;
	u16	ans;

	if((with_carry) && cpu->i8080FLAGS & i8080F_CY) {
		CY = 1;
	} else {
		CY = 0;
	}
	imm = getbyte(cpu, mem);
	ans = cpu->i8080ACC + imm + CY;
	old = cpu->i8080ACC;	/* save old ACC */
	cpu->i8080ACC = ans;
	setflags_add(cpu, ans, old);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
}
static int	add(i8080 *cpu, u8 *mem, u8 op, int with_carry)
{
	u8	*sss, CY=0, old;
	u16	ans;

	sss = cvtregs(cpu, (op & 0x07));
	if((with_carry) && cpu->i8080FLAGS & i8080F_CY) {
		CY = 1;
	} else {
		CY = 0;
	}
	if(sss == NULL) {	/* ADD(C) M */
		ans = cpu->i8080ACC + getmem(mem, cpu->HL.W) + CY;
		old = cpu->i8080ACC;	/* save old ACC */
		cpu->i8080ACC = ans;
		cpu->clocks += 7;
	} else {
		ans = cpu->i8080ACC + *sss + CY;
		old = cpu->i8080ACC;	/* save old ACC */
		cpu->i8080ACC = ans;
		cpu->clocks += 4;
	}
	setflags_add(cpu, ans, old);
	cpu->PC.W++;
	cpu->clocks += 5;
	return 0;
}

static int	dad(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd, hl;

	ddd = cvtregp_sp(cpu, (op & 0x30) >> 4);
	hl = cpu->HL.W;
	cpu->HL.W += *ddd;
	cpu->PC.W++;
	cpu->clocks += 10;
	if(cpu->HL.W >= hl && cpu->HL.W >= *ddd) {
 		cpu->i8080FLAGS &= ~i8080F_CY;
	} else {
 		cpu->i8080FLAGS |= i8080F_CY;
	}
	return 0;
}
static int	inx(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp_sp(cpu, (op & 0x30) >> 4);
	(*ddd)++;
	cpu->PC.W++;
	cpu->clocks += 5;
	return 0;
}
static int	dcx(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp_sp(cpu, (op & 0x30) >> 4);
	(*ddd)--;
	cpu->PC.W++;
	cpu->clocks += 5;
	return 0;
}
static int	ldax(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp_sp(cpu, (op & 0x10) >> 4);
	cpu->i8080ACC = getmem(mem, *ddd);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
	
}
static int	stax(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp_sp(cpu, (op & 0x10) >> 4);
	setmem(mem, *ddd, cpu->i8080ACC);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
}
static int	lxi(i8080 *cpu, u8 *mem, u8 op)
{
	u16	*ddd, word;

	ddd = cvtregp_sp(cpu, (op & 0x30) >> 4);
	cpu->PC.W++;
	word = getword(cpu, mem);
	*ddd = word;
	cpu->clocks += 10;
	return 0;
}

static int	mvi(i8080 *cpu, u8 *mem, u8 op)
{
	u8	*ddd, imm;

	ddd = cvtregs(cpu, (op & 0x38) >> 3);
	imm = getbyte(cpu, mem);
	if(ddd == NULL) {		/* mvi M,data */
		setmem(mem, cpu->HL.W, imm);
		cpu->clocks += 10;
	} else {
		*ddd = imm;		/* mvi r,data */
		cpu->clocks += 7;
	}
	cpu->PC.W++;
	return 0;
}
static int	mov(i8080 *cpu, u8 *mem, u8 op)
{
	u8	*ddd, *sss;

	ddd = cvtregs(cpu, (op & 0x38) >> 3);
	sss = cvtregs(cpu, (op & 0x07));
	if(ddd == NULL && sss == NULL) return hlt(cpu, mem, op);
	if(ddd == NULL) {		/* mov M,r */
		setmem(mem, cpu->HL.W, *sss);
		cpu->clocks += 7;
		cpu->PC.W++;
	} else if(sss == NULL) {	/* mov r,M */
		*ddd = getmem(mem, cpu->HL.W);
		cpu->clocks += 7;
		cpu->PC.W++;
	} else {
		*ddd = *sss;		/* mov r1,r2 */
		cpu->clocks += 5;
		cpu->PC.W++;
	}
	return 0;
}
static int	hlt(i8080 *cpu, u8 *mem, u8 op)
{
	// cpu->PC.W++;
	cpu->clocks += 7;
	return i8080_HLT;
}

/*
 * return register pointer by instruction DDD no.
 */
static u8	*cvtregs(i8080 *cpu, int ddd)
{
	switch(ddd) {
		case 0:
			return &cpu->BC.B.h;
		case 1:
			return &cpu->BC.B.l;
		case 2:
			return &cpu->DE.B.h;
		case 3:
			return &cpu->DE.B.l;
		case 4:
			return &cpu->HL.B.h;
		case 5:
			return &cpu->HL.B.l;
		case 6:
			return NULL;
		case 7:
			return &cpu->i8080ACC;
		default:
			return NULL;
	}
	return NULL;
}
/*
 * return register pointer by instruction DDD no. 11=PSW
 */
static u16	*cvtregp_af(i8080 *cpu, int ddd)
{
	switch(ddd) {
		case 0:
			return &cpu->BC.W;
		case 1:
			return &cpu->DE.W;
		case 2:
			return &cpu->HL.W;
		case 3:
			return &cpu->AF.W;
		default:
			return NULL;
	}
	return NULL;
}
/*
 * return register pointer by instruction DDD no. 11=SP
 */
static u16	*cvtregp_sp(i8080 *cpu, int ddd)
{
	switch(ddd) {
		case 0:
			return &cpu->BC.W;
		case 1:
			return &cpu->DE.W;
		case 2:
			return &cpu->HL.W;
		case 3:
			return &cpu->SP.W;
		default:
			return NULL;
	}
	return NULL;
}
/*
 * return stack word by sp and sp+1. after sp += 2
 */
static u16	popword(i8080 *cpu, u8 *mem)
{
	u8	h,l;
	l = getmem(mem, cpu->SP.W++);
	h = getmem(mem, cpu->SP.W++);
	return h<<8 | l;
}
/*
 * push stack by sp-1 and sp-2. after sp -= 2
 */
static u16	pushword(i8080 *cpu, u8 *mem, u16 word)
{
	u8	h,l;
	h = word >> 8;
	l = word & 0xff;
	setmem(mem, --cpu->SP.W, h);
	setmem(mem, --cpu->SP.W, l);
	return h<<8 | l;
}
/*
 * return immedate word by pc and pc+1. after pc += 2
 */
static u16	getword(i8080 *cpu, u8 *mem)
{
	u8	h,l;
	l = mem[cpu->PC.W++];
	h = mem[cpu->PC.W++];
	return h<<8 | l;
}
/*
 * return immedate next instruction byte by pc. after pc += 1
 */
static u8	getbyte(i8080 *cpu, u8 *mem)
{
	u8	l;
	l = mem[++cpu->PC.W];
	return l;
}

/*
 * get memory byte and print debug
 */
static u8	getmem(u8 *mem, u16 offset)
{
	printf("%04X:%02X\n", offset, mem[offset]);
	return mem[offset];
}
/*
 * write memory byte and print debug
 */
void	setmem(u8 *mem, u16 offset, u8 byte)
{
	printf("%04X:%02X -> %02X\n", offset, mem[offset], byte);
	mem[offset] = byte;
}

i8080	*i8080_new()
{
	i8080 *p;
	p = (i8080 *)malloc(sizeof(i8080)); 

	p->SP.W = p->PC.W = 0;
	p->clocks = 0;
	p->int_f  = 0;
	p->i8080FLAGS = i8080F_B1;
	return p;
}

void	i8080_dump(i8080 *p, u8 *mem)
{
	u8	op;
	if(p == NULL) {
		printf("     -PC- OP --FLAG-- -AF- -BC- -DE- -HL- -SP-\n");
		return;
	}
	op = mem[p->PC.W];
	printf("%4ld ", p->clocks);
	printf("%04X ", p->PC.W);
	printf("%02X ", op);
	printf("%c", (p->i8080FLAGS & i8080F_S  ? 'S' : 's'));
	printf("%c", (p->i8080FLAGS & i8080F_Z  ? 'Z' : 'z'));
	printf("%c", (p->i8080FLAGS & i8080F_B5 ? '+' : '-')); /* not use */
	printf("%c", (p->i8080FLAGS & i8080F_AC ? 'A' : 'a'));
	printf("%c", (p->i8080FLAGS & i8080F_B3 ? '+' : '-')); /* not use */
	printf("%c", (p->i8080FLAGS & i8080F_P  ? 'P' : 'p'));
	printf("%c", (p->i8080FLAGS & i8080F_B1 ? '+' : '-')); /* not use */
	printf("%c ",(p->i8080FLAGS & i8080F_CY ? 'C' : 'c'));
	printf("%04X ", p->AF.W);
	printf("%04X ", p->BC.W);
	printf("%04X ", p->DE.W);
	printf("%04X ", p->HL.W);
	printf("%04X ", p->SP.W);
	printf("\n");
}

static void	illegal_ins(u8 op)
{
	printf("%02X illegal opecode\n", op);
}

/*
 * Sign Zero Parity Flags set by ans.
 */
static void	setflags_szp(i8080 *cpu, u16 ans)
{
	if(ans == 0) {
		cpu->i8080FLAGS |= i8080F_Z;
	} else {
		cpu->i8080FLAGS &= ~i8080F_Z;
	}
	if(ans & (u16)0x0080) {
		cpu->i8080FLAGS |= i8080F_S;
	} else {
		cpu->i8080FLAGS &= ~i8080F_S;
	}
	if(u16_parity(ans)) {
		cpu->i8080FLAGS &= ~i8080F_P;
	} else {
		cpu->i8080FLAGS |= i8080F_P;
	}
}
static void	setflags_add(i8080 *cpu, u16 ans, u8 old)
{
	setflags_szp(cpu, ans);
	if(ans > (u16)0x00ff) {
		cpu->i8080FLAGS |= i8080F_CY;
	} else {
		cpu->i8080FLAGS &= ~i8080F_CY;
	}
	if((ans & 0x000f) < (old & 0x0f)) {
		cpu->i8080FLAGS |= i8080F_AC;
	} else {
		cpu->i8080FLAGS &= ~i8080F_AC;
	}
}
static void	setflags_sub(i8080 *cpu, u16 ans, u8 old)
{
	setflags_add(cpu, ans, old);
	if((ans & 0x000f) > (old & 0x0f)) {
		cpu->i8080FLAGS |= i8080F_AC;
	} else {
		cpu->i8080FLAGS &= ~i8080F_AC;
	}
}

/*
 * odd parity check, true if odd.
 */
static u16	u16_parity(u16 x)
{
  x = x ^ (x >> 8);
  x = x ^ (x >> 4);
  x = x ^ (x >> 2);
  x = x ^ (x >> 1);

  return x & 1;
}
