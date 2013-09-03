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
} i8080;

#define	i8080ACC	AF.B.h	/* pair of Acc */
#define	i8080FLAGS	AF.B.l	/* pair of Flags */

/*
 * Flag Bit Pattern
 *	Set  :	i8080.f |= i8080F_CY;
 *	Reset:  i8080.f &= ~i8080F_Z;
 *	Cmp. :	i8080.f ^= i8080F_P;
 */
#define	i8080F_CY	0x01	/* Carry	*/
#define	i8080F_B1	0x02	/* not use 1	*/
#define	i8080F_P	0x04	/* Parity	*/
#define	i8080F_B3	0x08	/* not use 0	*/
#define	i8080F_AC	0x10	/* Aux Carry	*/
#define	i8080F_B5	0x20	/* not use 0	*/
#define	i8080F_Z	0x40	/* Zero		*/
#define	i8080F_S	0x80	/* Sign		*/

#define	i8080_HLT	1	/* hlt instruction	*/
#define	i8080_ILG	-2	/* illegal opcode	*/

extern	i8080	*i8080_new();
extern	int	i8080_run(i8080 *, u8 *);
extern	void	i8080_dump(i8080 *, u8 *);

#endif
