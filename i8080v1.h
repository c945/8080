#ifdef	_i8080_h_
#else
#define	_i8080_h_
typedef unsigned char  u8;
typedef unsigned short u16;

typedef struct _cpui8080 {
	union {
		struct {
			u8 b;
			u8 c;
			u8 d;
			u8 e;
			u8 h;
			u8 l;
			u8 a;
			u8 f;
			u8 sp_h;
			u8 sp_l;
			u8 pc_h;
			u8 pc_l;
		} h;
		struct {
			u16 bc;
			u16 de;
			u16 hl;
			u16 psw;
			u16 sp;
			u16 pc;
		} w;
	};
} cpui8080_t, *cpui8080;
#endif
