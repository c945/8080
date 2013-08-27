#include	<stdio.h>
#include	<stdlib.h>
#include	"i8080.h"

int	i8080_run(cpui8080_t cpu, u8 *mem)
{
	u8 	op, sop, wop, wop2;
	u16	word;
	int	ret;

	ret = 0;
	/* op = getmem(mem, cpu->PC.W); */
	op = mem[cpu->PC.W];
	sop = (op & 0xc7);	/* OPCODE for Reg A/B/C/D/E/H/L/M	 */
	wop = (op & 0xcf);	/* OPCODE for Reg Pair BC/DE/HL/SP	 */
	wop2 = (op & 0xef);	/* OPCODE for Reg Pair BC/DE		 */
	if(op == 0) {			/* nop */
		cpu->clocks += 4;
		cpu->PC.W++;
	}
	else if((op & 0xc0) == 0x40) {	/* mov r,r / mov m,r / mov r,m / hlt */
		ret = mov(cpu, mem, op);
	}
	else if(sop == 0x06) {	/* mvi r,data / mov m,data */
		ret = mvi(cpu, mem, op);
	}
	else if(wop == 0x01) { 	/* lxi rp, data data */
		ret = lxi(cpu, mem, op);
	}
	else if(op == 0x3a) {		/* lda addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		cpu->AF.B.h = getmem(mem,word);
		cpu->clocks += 13;
	}
	else if(op == 0x2a) {		/* lhld addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		cpu->HL.B.l = getmem(mem, word);
		cpu->HL.B.h = getmem(mem, word+1);
		cpu->clocks += 16;
	}
	else if(op == 0x32) {		/* sta addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		setmem(mem, word, cpu->AF.B.h);
		cpu->clocks += 13;
	}
	else if(op == 0x22) {		/* shld addr addr */
		cpu->PC.W++;
		word = getword(cpu, mem);
		setmem(mem, word, cpu->HL.B.l);
		setmem(mem, word+1, cpu->HL.B.h);
		cpu->clocks += 16;
	}
	else if(wop2 == 0x0a) { 	/* ldax rp */
		ret = ldax(cpu, mem, op);
	}
	else if(wop2 == 0x02) { 	/* stax rp */
		ret = stax(cpu, mem, op);
	}
	else if(op == 0xeb) {		/* xchg */
		word = cpu->HL.W;
		cpu->HL.W = cpu->DE.W;
		cpu->DE.W = word;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(wop == 0x03) {	/* inx rp */
		ret = inx(cpu, mem, op);
	}
	else if(wop == 0x0b) {	/* dcx rp */
		ret = dcx(cpu, mem, op);
	}
	else if(wop == 0x09) {	/* dad rp */
		ret = dad(cpu, mem, op);
	}
	else if(op == 0x3f) {		/* cmc */
		cpu->AF.B.l ^= i8080F_CY;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(op == 0x37) {		/* stc */
		cpu->AF.B.l |= i8080F_CY;
		cpu->PC.W++;
		cpu->clocks += 4;
	}
	else if(op == 0xc3) {		/* jmp */
		cpu->PC.W++;
		cpu->PC.W = getword(cpu, mem);
		cpu->clocks += 10;
	}
	else if(op == 0xcd) {		/* call */
		cpu->PC.W++;
		word = getword(cpu, mem);
		pushword(cpu, mem, cpu->PC.W);
		cpu->PC.W = word;
		cpu->clocks += 17;
	}
	else if(op == 0xc9) {		/* ret */
		cpu->PC.W++;
		word = popword(cpu, mem);
		cpu->PC.W = word;
		cpu->clocks += 10;
	}
	else if(op == 0x76) {		/* hlt */
		cpu->clocks += 7;
	}
	else {
		illegal_ins(op);
		ret = -1;
	}
	return ret;
}

int	dad(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd, hl;

	ddd = cvtregp(cpu, (op & 0x30) >> 4);
	hl = cpu->HL.W;
	cpu->HL.W += *ddd;
	cpu->PC.W++;
	cpu->clocks += 10;
	if(cpu->HL.W >= hl && cpu->HL.W >= *ddd) {
 		cpu->AF.B.l &= ~i8080F_CY;
	} else {
 		cpu->AF.B.l |= i8080F_CY;
	}
	return 0;
}
int	inx(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp(cpu, (op & 0x30) >> 4);
	(*ddd)++;
	cpu->PC.W++;
	cpu->clocks += 5;
	return 0;
}
int	dcx(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp(cpu, (op & 0x30) >> 4);
	(*ddd)--;
	cpu->PC.W++;
	cpu->clocks += 5;
	return 0;
}
int	ldax(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp(cpu, (op & 0x10) >> 4);
	cpu->AF.B.h = getmem(mem, *ddd);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
	
}
int	stax(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd;

	ddd = cvtregp(cpu, (op & 0x10) >> 4);
	setmem(mem, *ddd, cpu->AF.B.h);
	cpu->PC.W++;
	cpu->clocks += 7;
	return 0;
}
int	lxi(cpui8080_t cpu, u8 *mem, u8 op)
{
	u16	*ddd, word;

	ddd = cvtregp(cpu, (op & 0x30) >> 4);
	cpu->PC.W++;
	word = getword(cpu, mem);
	*ddd = word;
	cpu->clocks += 10;
	return 0;
}

int	mvi(cpui8080_t cpu, u8 *mem, u8 op)
{
	u8	*ddd, imm;

	cpu->PC.W++;
	ddd = cvtregs(cpu, (op & 0x38) >> 3);
	imm = getbyte(cpu, mem);
	if(ddd == NULL) {		/* mvi M,data */
		setmem(mem, cpu->HL.W, imm);
		cpu->clocks += 10;
	} else {
		*ddd = imm;		/* mvi r,data */
		cpu->clocks += 7;
	}
	return 0;
}
int	mov(cpui8080_t cpu, u8 *mem, u8 op)
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
int	hlt(cpui8080_t cpu, u8 *mem, u8 op)
{
	cpu->PC.W++;
	return i8080_HLT;
}

/*
 * return register pointer by instruction DDD no.
 */
u8	*cvtregs(cpui8080_t cpu, int ddd)
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
			return &cpu->AF.B.h;
		default:
			return NULL;
	}
	return NULL;
}
/*
 * return register pointer by instruction DDD no.
 */
u16	*cvtregp(cpui8080_t cpu, int ddd)
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
u16	popword(cpui8080_t cpu, u8 *mem)
{
	u8	h,l;
	l = mem[cpu->SP.W++];
	h = mem[cpu->SP.W++];
	return h<<8 | l;
}
/*
 * push stack by sp-1 and sp-2. after sp -= 2
 */
u16	pushword(cpui8080_t cpu, u8 *mem, u16 word)
{
	u8	h,l;
	h = word >> 8;
	l = word & 0xff;
	mem[--cpu->SP.W] = h;
	mem[--cpu->SP.W] = l;
	return h<<8 | l;
}
/*
 * return immedate word by pc and pc+1. after pc += 2
 */
u16	getword(cpui8080_t cpu, u8 *mem)
{
	u8	h,l;
	l = mem[cpu->PC.W++];
	h = mem[cpu->PC.W++];
	return h<<8 | l;
}
/*
 * return immedate byte by pc. after pc += 1
 */
u8	getbyte(cpui8080_t cpu, u8 *mem)
{
	u8	l;
	l = mem[cpu->PC.W++];
	return l;
}

/*
 * get memory byte
 */
u8	getmem(u8 *mem, u16 offset)
{
	printf("%04X:%02X\n", offset, mem[offset]);
	return mem[offset];
}
/*
 * write memory byte
 */
void	setmem(u8 *mem, u16 offset, u8 byte)
{
	printf("%04X:%02X -> %02X\n", offset, mem[offset], byte);
	mem[offset] = byte;
}

cpui8080_t	i8080_new()
{
	cpui8080_t p;
	p = (cpui8080_t)malloc(sizeof(cpui8080)); 

	p->SP.W = p->PC.W = 0;
	p->clocks = 0;
	p->int_f  = 0;
	return p;
}

void	i8080_dump(cpui8080_t p, u8 *mem)
{
	u8	op;
	if(p == NULL) {
		printf("     -PC- OP FLAG -AF- -BC- -DE- -HL- -SP-\n");
		return;
	}
	op = mem[p->PC.W];
	printf("%4ld ", p->clocks);
	printf("%04X ", p->PC.W);
	printf("%02X ", op);
	printf("%c", (p->AF.B.l & i8080F_CY ? 'C' : 'c'));
	printf("%c", (p->AF.B.l & i8080F_S  ? 'S' : 's'));
	printf("%c", (p->AF.B.l & i8080F_P  ? 'P' : 'p'));
	printf("%c ", (p->AF.B.l & i8080F_Z  ? 'Z' : 'z'));
	printf("%02X", p->AF.B.h);
	printf("%02X ", p->AF.B.l);
	printf("%02X", p->BC.B.h);
	printf("%02X ", p->BC.B.l);
	printf("%02X", p->DE.B.h);
	printf("%02X ", p->DE.B.l);
	printf("%02X", p->HL.B.h);
	printf("%02X ", p->HL.B.l);
	printf("%04X", p->SP.W);
	printf("\n");
}

void	illegal_ins(u8 op)
{
	printf("%02X illegal opecode\n", op);
}

