#include <linux/types.h>
#include <stdint.h>


typedef __u8 bool;
#define true	1
#define false	0

/** Named as X - 0xF0 prefix || FPU additional push/pop/2pop */
typedef enum {
	X_LOCK,			/* Has 0xF0 Lock prefix / Is valid Lock pf */
	/** These apply only to FPU instr **/
	X_FPU_PUSH,		/* opcode does additional ; push */
	X_FPU_POP,		/* opcode does additional ; pop */
	X_FPU_POPOP,	/* Same as FPU_POP, but   ; pop' twice */
} x_enum;


/** Introduced with instr extention... **/
// typedef enum {
//     MMX,    /* MMX Technology */
//     SSE1,   /* Streaming SIMD Extensions (1) */
//     SSE2,   /* Streaming SIMD Extensions 2 */
//     SSE3,   /* Streaming SIMD Extensions 3 */
//     SSSE3,  /* Supplemental Streaming SIMD Extensions 3 */
//     SSE41,  /* Streaming SIMD Extensions 4.1 */
//     SSE42,  /* Streaming SIMD Extensions 4.2 */
//     VMX,    /* Virtualization Technology Extensions */
//     SMX,    /* Safer Mode Extensions */
// } iext_enum;



/* Register flags */
#define F_OVERFLOW		(1 << 7)	/* b10000000 */
#define F_DIRECTION		(1 << 6)	/* b01000000 */
#define F_INTERRUPT		(1 << 5)	/* b00100000 */
#define F_SIGN			(1 << 4)	/* b00010000 */
#define F_ZERO			(1 << 3)	/* b00001000 */
#define F_AUXCARRY		(1 << 2)	/* b00000100 */
#define F_PARITY		(1 << 1)	/* b00000010 */
#define F_CARRY			(1 << 0)	/* b00000001 */

/* x87 FPU flags */
#define F_X86_FPU_0 	(1 << 0)	/* b0001 */
#define F_X86_FPU_1 	(1 << 1)	/* b0010 */
#define F_X86_FPU_2 	(1 << 2)	/* b0100 */
#define F_X86_FPU_3 	(1 << 3)	/* b1000 */

typedef struct {
	enum {
		ODISZAPC_FLAG,
		x86_FPU_FLAG,
	} 		f_type;
	__u8 	f_val;
} flags_enum;

typedef enum {
	GRP_F_PREFIX,
	GRP_F_BRANCH,
	GRP_F_OBSOL,				/* obsolete */
	GRP_F_GEN,					/* general */
	GRP_F_DATAMOV,				/* data movement */
	GRP_F_STACK,
	GRP_F_ARITH,				/* arithmetic */
	GRP_F_BINARY,						
	GRP_F_DECIMAL,
	GRP_F_LOGICAL,
	GRP_F_SHFTROT,				/* shift&rotate */
	GRP_F_BIT,					/* bit manipulation */
	GRP_F_COND,					/* conditional */
	GRP_F_BREAK,				/* interrupt */
	GRP_F_STRING,				/* (means that the instruction can make use of the REP family prefixes) */
	GRP_F_INOUT,				/* I/O */
	GRP_F_FLGCTRL,				/* flag control */
	GRP_F_SEGREG,				/* segment register manipulation */
	GRP_F_SYSTEM,
	GRP_F_TRANS,				/* transitional (implies sensitivity to operand-size attribute) */
	GRP_F_X87FPU,
	GRP_F_COMPAR,				/* comparison */
	GRP_F_LDCONST,				/* load constant */
	GRP_F_CONTROL,
	GRP_F_CONV,					/* conversion */
	GRP_F_SM,					/* x87 FPU and SIMD state management */
} grp_enum;

/** @MOOP - Mode of Operation **/
#define MODOP_R 0x1     /* applies for real, protected and 64-bit mode. SMM is not taken into account. */
#define MODOP_P 0x2     /* applies for protected and 64-bit mode. SMM is not taken into account. group 0F00 */
#define MODOP_E 0x4     /* applies for 64-bit mode. for 64-bit, specific to 64-bit mode SMM is not taken into account. 63 MOVSXD */
#define MODOP_S 0x8     /* applies for SMM. 0FAA RSM */


#define F_MODRM 0xff



#define X86_FPU_ANY 0x8334
// #define MEMORY 		0x8333
#define REG_ANY 	0x8332
#define REG_I_ANY 	0x8331
#define GEN_REG 	0x8330

#define NO 0xcafe


/** f indicates that the level depends on a flag(s) and it should contain a reference to the description of that flag,
 * if the flag is not too complex. If this column is empty, it means ring 3
**/
#define RING_LEVEL_IS_FLAGS_R3 0xf3
#define RING_LEVEL_IS_FLAGS_R2 0xf2
#define RING_LEVEL_IS_FLAGS_R1 0xf1
#define RING_LEVEL_IS_FLAGS_R0 0xf0



#define ENUM_ASM_X86_64_REGS_LIMIT 0x44
////////////////////


#define GET_F_ADDR(a) (a & 0xff00)
#define GET_F_OPER(a) (a & 0x00ff)

/* Original */
#define F_ADDR_A 0x0100  	/* Direct address. The instruction has no ModR/M byte; the address of the operand is encoded in the  instruction; no base register, index register, or scaling factor can be applied (for example, far JMP (EA)). */
#define F_ADDR_C 0x0200  	/* The reg field of the ModR/M byte selects a control register (only MOV (0F20, 0F22)).  */
#define F_ADDR_D 0x0300  	/* The reg field of the ModR/M byte selects a debug register (only MOV (0F21, 0F23)).  */
#define F_ADDR_E 0x0400  	/* A ModR/M byte follows the opcode and specifies the operand. The operand is either a  general-purpose register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, or a displacement. */
#define F_ADDR_F 0x0500  	/* rFLAGS register.  */
#define F_ADDR_G 0x0600  	/* The reg field of the ModR/M byte selects a general register (for example, AX (000)).  */
#define F_ADDR_I 0x0700  	/* Immediate data. The operand value is encoded in subsequent bytes of the instruction.  */
#define F_ADDR_J 0x0800  	/* The instruction contains a relative offset to be added to the instruction pointer register (for  example, JMP (E9), LOOP)). */
#define F_ADDR_M 0x0900  	/* The ModR/M byte may refer only to memory: mod != 11bin (BOUND, LEA, CALLF, JMPF, LES, LDS, LSS,  LFS, LGS, CMPXCHG8B, CMPXCHG16B, F20FF0 LDDQU). */
#define F_ADDR_N 0x1000  	/* The R/M field of the ModR/M byte selects a packed quadword MMX technology register.  */
#define F_ADDR_O 0x1100  	/* The instruction has no ModR/M byte; the offset of the operand is coded as a word, double word or  quad word (depending on address size attribute) in the instruction. No base register, index register, or scaling factor can be applied (only MOV  (A0, A1, A2, A3)). */
#define F_ADDR_P 0x1200  	/* The reg field of the ModR/M byte selects a packed quadword MMX technology register.  */
#define F_ADDR_Q 0x1300  	/* A ModR/M byte follows the opcode and specifies the operand. The operand is either an MMX  technology register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement. */
#define F_ADDR_R 0x1400  	/* The mod field of the ModR/M byte may refer only to a general register (only MOV (0F20-0F24,  0F26)). */
#define F_ADDR_S 0x1500  	/* The reg field of the ModR/M byte selects a segment register (only MOV (8C, 8E)).  
instructions are, for example, POP, RET, IRET, LEAVE. Push-like are, for example, PUSH, CALL, INT. No Operand type is provided along with this method because it depends on source/destination operand(s). */
#define F_ADDR_T 0x1600  	/* The reg field of the ModR/M byte selects a test register (only MOV (0F24, 0F26)).  */
#define F_ADDR_U 0x1700  	/* The R/M field of the ModR/M byte selects a 128-bit XMM register.  */
#define F_ADDR_V 0x1800  	/* The reg field of the ModR/M byte selects a 128-bit XMM register.  */
#define F_ADDR_W 0x1900  	/* A ModR/M byte follows the opcode and specifies the operand. The operand is either a 128-bit XMM  register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement */
#define F_ADDR_X 0x2000  	/* Memory addressed by the DS:eSI or by RSI (only MOVS, CMPS, OUTS, and LODS). In 64-bit mode, only  64-bit (RSI) and 32-bit (ESI) address sizes are supported. In non-64-bit modes, only 32-bit (ESI) and 16-bit (SI) address sizes are supported. */
#define F_ADDR_Y 0x2100  	/* Memory addressed by the ES:eDI or by RDI (only MOVS, CMPS, INS, STOS, and SCAS). In 64-bit mode,  only 64-bit (RDI) and 32-bit (EDI) address sizes are supported. In non-64-bit modes, only 32-bit (EDI) and 16-bit (DI) address sizes are supported. The implicit ES segment register cannot be overriden by a segment prefix. */

/* Added */
#define F_ADDR_BA	0x5000 /* Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 MONITOR).  */
#define F_ADDR_BB	0x5100 /* Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT). (This code changed from  single B in revision 1.00) */
#define F_ADDR_BD	0x5200 /* Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 MASKMOVDQU) (This code changed  from YD (introduced in 1.02)  */
#define F_ADDR_ES 	0x5300 /* (Implies original E). A ModR/M byte follows the opcode and specifies the operand. The operand is  either a x87 FPU stack register  */
#define F_ADDR_EST	0x5400 /* (Implies original E). A ModR/M byte follows the opcode and specifies the x87 FPU stack register.  */
#define F_ADDR_H	0x5500 /* The r/m field of the ModR/M byte always selects a general register, regardless of the mod field (for  example, MOV (0F20)). */
#define F_ADDR_SC	0x5600 /* Stack operand, used by instructions which either push an operand to the stack or pop an operand from  the stack. Pop-like  */
#define F_ADDR_Z	0x5700 /* The instruction has no ModR/M byte; the three least-significant bits of the opcode byte selects a  general-purpose register */

/* operators */
#define F_OPERAND_A	 	0x0001 	/*  	Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute ( only BOUND).*/
#define F_OPERAND_B 	0x0002 	/*  	Byte, regardless of operand-size attribute. */
#define F_OPERAND_BSQ	0x0003 	/* ; replaced by bs 	(Byte, sign-extended to 64 bits.) */
#define F_OPERAND_BSS 	0x0004 	/*  	Byte, sign-extended to the size of the stack pointer (for example, PUSH (6A)). */
#define F_OPERAND_C 	0x0005 	/*  	Byte or word, depending on operand-size attribute. (unused even by Intel?) */
#define F_OPERAND_D		0x0006 	/*  	Doubleword, regardless of operand-size attribute. */
#define F_OPERAND_DQ 	0x0007 	/*  	Double-quadword, regardless of operand-size attribute (for example, CMPXCHG16B). */
#define F_OPERAND_DS 	0x0008	/*  	Doubleword, sign-extended to 64 bits (for example, CALL (E8). */
#define F_OPERAND_P 	0x0009 	/*  	32-bit or 48-bit pointer, depending on operand-size attribute (for example, CALLF (9A). */
#define F_OPERAND_PI 	0x0010 	/*  	Quadword MMX technology data. */
#define F_OPERAND_PD 	0x0011 	/*  	128-bit packed double-precision floating-point data. */
#define F_OPERAND_PS	0x0012 	/*  	128-bit packed single-precision floating-point data. */
#define F_OPERAND_PT 	0x0013 	/* ; replaced by ptp 	(80-bit far pointer.) example, CALLF (FF /3)). */
#define F_OPERAND_Q 	0x0014	/*  	Quadword, regardless of operand-size attribute (for example, CALL (FF /2)). */
#define F_OPERAND_QP 	0x0015 	/*  	Quadword, promoted by REX.W (for example, IRETQ). */
#define F_OPERAND_S 	0x0016 	/* Changed to 	6-byte pseudo-descriptor, or 10-byte pseudo-descriptor in 64-bit mode (for example, SGDT). */
#define F_OPERAND_SD 	0x0017 	/*  	Scalar element of a 128-bit packed double-precision floating data. */
#define F_OPERAND_SI	0x0018 	/*  	Doubleword integer register (e. g., eax). (unused even by Intel?) */
#define F_OPERAND_SS 	0x0019 	/*  	Scalar element of a 128-bit packed single-precision floating data. */
#define F_OPERAND_T 	0x0020 	/* ; replaced by ptp 	10-byte far pointer. */
#define F_OPERAND_V 	0x0021 	/*  	Word or doubleword, depending on operand-size attribute (for example, INC (40), PUSH (50)). */
#define F_OPERAND_VQ 	0x0022 	/*  	Quadword (default) or word if operand-size prefix is used (for example, PUSH (50)). */
#define F_OPERAND_VS 	0x0023 	/*  	Word or doubleword sign extended to the size of the stack pointer (for example, PUSH (68)). */
#define F_OPERAND_W		0x0024 	/*  	Word, regardless of operand-size attribute (for example, ENTER). */


/* Added */
#define F_OPERAND_BCD	0x0050 	/*  	Packed-BCD. Only x87 FPU instructions (for example, FBLD).*/
#define F_OPERAND_BS 	0x0051	/* ; simplified bsq 	Byte, sign-extended to the size of the destination operand.*/
#define F_OPERAND_DI 	0x0052	/*  	Doubleword-integer. Only x87 FPU instructions (for example, FIADD).*/
#define F_OPERAND_DQP 	0x0053	/* ; combines d and qp 	Doubleword, or quadword, promoted by REX.W in 64-bit mode (for example, MOVSXD).*/
#define F_OPERAND_DR 	0x0054	/*  	Double-real. Only x87 FPU instructions (for example, FADD).*/
#define F_OPERAND_E 	0x0055	/*  	x87 FPU environment (for example, FSTENV).*/
#define F_OPERAND_ER 	0x0056	/*  	Extended-real. Only x87 FPU instructions (for example, FLD).*/
#define F_OPERAND_PSQ 	0x0057	/* 	64-bit packed single-precision floating-point data.*/
#define F_OPERAND_PTP 	0x0058	/*  32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit far pointer, promoted by REX.W in 64-bit mode (for */
#define F_OPERAND_QI	0x0059	/*  	Qword-integer. Only x87 FPU instructions (for example, FILD).*/
#define F_OPERAND_SR	0x0061	/*  	Single-real. Only x87 FPU instructions (for example, FADD).*/
#define F_OPERAND_ST	0x0062	/*  	x87 FPU state (for example, FSAVE).*/
#define F_OPERAND_STX	0x0063	/*  	x87 FPU and SIMD state (FXSAVE and FXRSTOR).*/
#define F_OPERAND_VDS	0x0064	/* ; combines v and ds 	Word or doubleword, depending on operand-size attribute, or doubleword, sign-extended to 64 bits for */
#define F_OPERAND_VQP	0x0065	/* ; combines v and qp 	Word or doubleword, depending on operand-size attribute, or quadword, promoted by REX.W in 64-bit mode.*/
#define F_OPERAND_WI	0x0066	/*  	Word-integer. Only x87 FPU instructions (for example, FIADD).*/
#define F_OPERAND_VA	0x0067
#define F_OPERAND_DQA	0x0068
#define F_OPERAND_WA	0x0069
#define F_OPERAND_WO	0x0070
#define F_OPERAND_WS	0x0071
#define F_OPERAND_DA 	0x0072
#define F_OPERAND_DO 	0x0073
#define F_OPERAND_QA 	0x0074
#define F_OPERAND_QS 	0x0075


// >= 4
#define BAD_OPS_INDEX(x) ((x<0||x>4) ? 1 : 0)

typedef struct {
	// ONLY 1 will be used
	// why not union - because it is shorter, not in memory
	unsigned char *reg;	// "RAX"
	__u16	f;
	__u64	v;
} oper_st;

#define foreach_oper_st(iter, ops, ndx)		\
			for (iter = &ops[0]; iter <= &ops[3]; iter++, ndx++)
				
// struct instr_ops *iter = NULL;
#define for_each_ops(l, inst, ndx)	\
		for (l = &inst->ops[0]; l <= &inst->ops[inst->ops_count-1]; l++, ndx++)


typedef struct {
	__u8 		prefix;			/* Prefix	*/
	bool 		two_byte;		/* 0F - 0/1 */
	__u8	 	opcode1;		/* 1st opcode	*/
	__u8	 	opcode2;		/* 2nd opcode	*/
	unsigned char *flds;
	__u8		reg_op;			/* MODr\m (use F_MODRM)	/ 0..7 - inside REG */
	__u8		m;			/* Real, Prot, 64bit... See @MOOP 	*/
	// __u8 		ringlvl;		/* Ring - 0,1,2,3 - 0xf 			*/
	x_enum		x;				/* USE Lock pf / FPU(& cannot use Lock pf) */

	union {
		struct {
			unsigned char 	*mnemonic;	/* instr */
			bool 			alias;
			oper_st op[4];
			// when parsing this - golang - gotta add the ops_count at the end 1
		};
		/*	OR	*/
		struct instr_ops {
			unsigned char 	*mnemonic;	/* instr */
			bool 			alias;
			oper_st op[4];
		} *ops;			// strictly [n] -Sure does work!
	};

	__u8 	ops_count;			// how many of struct instr_ops are there
	// iext_enum 	iext;

	// grp_enum	grp1;
	// grp_enum	grp2;
	// grp_enum	grp3;
	
	// flags_enum 	tested_f;
	// flags_enum 	modif_f;
	// flags_enum 	def_f;
	// flags_enum 	undef_f;
	// flags_enum 	f_values;
} instr;

#ifndef INSTR_TABLE_H
	extern instr tabl[941];
#endif