#include "../include/uapi.h"

__u32 get_inst_type(instr_dat_t *ret, operand *p) {
	if (p->is_imm) {
		if (p->rip) return RIP|IMM;
		return IMM;
	}
	if (p->reg && !p->disp_sz) {
		if (p->is_ptr) return PTR|REG;
		return REG;
	}
	if (p->is_ptr) {
		if (p->sib.on) return SIB|PTR;
		if (ret->modrm_on && p->disp_sz) {
			if (p->rip) return PTR|MODRM|RIP;
			return PTR|MODRM;
		}
	}
}

const __u8 *get_instr_name(instr_dat_t *in) {
	return (!in->op_ndx) ? in->instr->mnemonic : in->instr->ops[in->op_ndx].mnemonic;
}

__u8 in_cmp_mnemo(instr_dat_t *in, __u8 *s) {
	return (!strncmp(get_instr_name(in), s, strlen(s)+1));
}

ok_t get_imm(instr_dat_t *in) {
	foreach_operand(p, in) {
		if (p->is_imm)
			return _OK(1, *(__u64*)p->imm);
	}
	return _OK(0, 0);
}

__u8 get_oper_sz(instr_dat_t *in, operand *p) {
	switch (p->x_origin.t) {
		case X_IMM:		return p->sz;
		case X_MODRM:	return p->disp_sz;
	}
	return 0;
}

__u8 get_operand_ptr(instr_dat_t *in, operand *p, __u64 *v) {
	__u64 imm=0, sz=0;
	switch (p->x_origin.t) {
		case X_IMM:
			imm = *(__u64*)p->imm;
			sz = p->sz;
			break;

		case X_MODRM:
			imm = *(__u64*)p->disp;
			sz = p->disp_sz;
			break;
		default:
			return 0;
	}

	__u8 is_min = _is_signed(imm, sz);
	if (is_min) {
		imm = _signed(imm, sz);
		imm ^= ~((__u64)0|(_pow(2, sz)));
	}

	imm = imm & N_BITS_MAX(sz);
	*v = (is_min) ? -(imm) : imm;
	return (is_min) ? '-' : '+';
}

__s8 get_rip_oper(instr_dat_t *in) {
	__u8 c = 0;
	foreach_operand(p, in) {
		if (!!p->rip) return c;
		c++;
	}
	return -1;
}

err_t get_dst_ptr(instr_dat_t *in) {
	__s8 c = get_rip_oper(in);
	if (c == -1) goto fucked;
	operand *p = &in->oper[c];

	__s64 v = 0;
	__u8 plmin = get_operand_ptr(in, p, &v);
	if (!plmin) goto fucked;

	v += ((plmin == '-') ? in->in_sz : 0);
	return ((err_t){ .ok=1, .v=v });

	fucked:
		return NEW_ERROR(-1, 0);
}

__u32 change_ptr(instr_dat_t *in, __u64 diff, __u8 f) {
	__s8 c = get_rip_oper(in);
	if (c == -1) return -1;
	operand *p = &in->oper[c];

	__s64 v = 0;
	__u8 plmin = get_operand_ptr(in, p, &v);
	if (!plmin) return -1;

	switch (f) {
		case CHNG_REL:	v = diff;		break;
		case CHNG_FXD:	v = v + diff;	break;
		default:
			return -1;
	}

	switch (p->x_origin.t) {
		case X_IMM:
			change_imm(in, p, v);
			break;
		case X_MODRM:
			__u8 mod	= MODRM_RM(in->modrm);
			__u8 rm		= MODRM_REG(in->modrm);
			if (!in->sib_on) {
				change_disp(in, p, v);
				break;
			}
		default:
			pr_in_str(in);
			DIE("FUCK!");
	}
	return 0;
}


__u8 get_rip_val(instr_dat_t *in, __u64 *v) {
	__s8 c = get_rip_oper(in);
	if (c == -1) return 0;

	return get_operand_ptr(in, &in->oper[c], v);
}

__s8 get_rip_ptr_addr(instr_dat_t *in, void *off, __u64 *v) {
	__u8 plmin = get_rip_val(in, v);
	if (!plmin) return -1;
	*v = (off + in->in_sz + ((plmin == u'-') ? (__s64)( -( ~*v + 1 ) ) : *v));
	// *v = off + *v + in->in_sz;//((plmin == '+') ? in->in_sz : 0);
	return 0;
}

__u8 _build_instr(instr *in, __u8 sc[15]) {
	__u64 i = 0;
	if (in->prefix)		sc[i++] = in->prefix;
	if (in->two_byte)	sc[i++] = 0x0f;

	sc[i++] = in->opcode1;
	if (in->opcode2) sc[i++] = in->opcode2;

	if (in->reg_op && in->reg_op != F_MODRM) {
		sc[i-1] = (__u8)( ((in->reg_op == 0xb0) ? 0 : in->reg_op) << 3);
	}
	return i;
}