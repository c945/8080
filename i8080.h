#ifdef	_i8080_h_
#else
#define	_i8080_h_
typedef unsigned char  u8;
typedef unsigned short u16;

typedef union {
	struct {
		u8  l;
		u8  h;
	} B;
	u16 W;
} pair;
typedef struct {
	pair	AF,BC,DE,HL,SP,PC;
	long	clocks;
	int	int_f;
} cpui8080, *cpui8080_t;

/*
 * Flag Bit Pattern
 *	Set  :	cpui8080.f |= i8080F_CY;
 *	Reset:  cpui8080.f &= ~i8080F_Z;
 *	Cmp. :	cpui8080.f ^= i8080F_P;
 */
#define	i8080F_CY	0x01	/* Carry	*/
#define	i8080F_P	0x04	/* Parity	*/
#define	i8080F_AC	0x10	/* Aux Carry	*/
#define	i8080F_Z	0x40	/* Zero		*/
#define	i8080F_S	0x80	/* Sign		*/

#define	i8080_HLT	1	/* hlt instruction	*/
#define	i8080_ILG	-2	/* illegal opcode	*/

extern	cpui8080_t	i8080_new();
extern	int	i8080_run(cpui8080_t, u8 *);
extern	void	i8080_dump(cpui8080_t, u8 *);
extern	int	mov( cpui8080_t, u8 *, u8);
extern	int	mvi( cpui8080_t, u8 *, u8);
extern	int	lxi( cpui8080_t, u8 *, u8);
extern	int	hlt( cpui8080_t, u8 *, u8);
extern	int	ldax(cpui8080_t, u8 *, u8);
extern	int	stax(cpui8080_t, u8 *, u8);
extern	int	inx( cpui8080_t, u8 *, u8);
extern	int	dcx( cpui8080_t, u8 *, u8);
extern	int	dad( cpui8080_t, u8 *, u8);
extern	u8	*cvtregs(cpui8080_t, int);
extern	u16	*cvtregp(cpui8080_t, int);
extern	u16	getword(cpui8080_t, u8 *);
extern	u8	getbyte(cpui8080_t, u8 *);
extern	u16	popword(cpui8080_t, u8 *);
extern	u16	pushword(cpui8080_t, u8 *, u16);
extern	u8	getmem(u8 *, u16);
extern	void	setmem(u8 *, u16, u8);

#endif
