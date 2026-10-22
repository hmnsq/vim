/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003	Davide Libenzi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#અન્તર્ભાવ "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	માટે (i = 1; n > 0; n >>= 2)
		i <<= 1;

	પાછા i;
}


પૂર્ણાંક xdl_emit_diffrec(અક્ષર અચલ *rec, long size, અક્ષર અચલ *pre, long psize,
		     xdemitcb_t *ecb) {
	પૂર્ણાંક i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (અક્ષર *) pre;
	mb[0].size = psize;
	mb[1].ptr = (અક્ષર *) rec;
	mb[1].size = size;
	જો (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (અક્ષર *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	જો (ecb->out_લીટી(ecb->priv, mb, i) < 0) {

		પાછા -1;
	}

	પાછા 0;
}

શૂન્ય *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	પાછા mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	પાછા mmf->size;
}


પૂર્ણાંક xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = ખાલી;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ખાલી;
	cha->scurr = 0;

	પાછા 0;
}


શૂન્ય xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	માટે (cur = cha->head; (tmp = cur) != ખાલી;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


શૂન્ય *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	શૂન્ય *data;

	જો (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		જો (!(ancur = (chanode_t *) xdl_malloc(કદ(chanode_t) + cha->nsize))) {

			પાછા ખાલી;
		}
		ancur->icurr = 0;
		ancur->next = ખાલી;
		જો (cha->tail)
			cha->tail->next = ancur;
		જો (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (અક્ષર *) ancur + કદ(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	પાછા data;
}

long xdl_guess_લીટીઓ(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	અક્ષર અચલ *data, *cur, *ટોચ;

	જો ((cur = data = xdl_mmfile_first(mf, &size))) {
		માટે (ટોચ = data + size; nl < sample && cur < ટોચ; ) {
			nl++;
			જો (!(cur = memchr(cur, '\n', ટોચ - cur)))
				cur = ટોચ;
			નહીંતર
				cur++;
		}
		tsize += (long) (cur - data);
	}

	જો (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	પાછા nl + 1;
}

પૂર્ણાંક xdl_blankline(અચલ અક્ષર *લીટી, long size, long flags)
{
	long i;

	જો (!(flags & XDF_WHITESPACE_FLAGS))
		પાછા (size <= 1);

	માટે (i = 0; i < size && XDL_ISSPACE(લીટી[i]); i++)
		;

	પાછા (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
સ્થિર પૂર્ણાંક ends_with_optional_cr(અચલ અક્ષર *l, long s, long i)
{
	પૂર્ણાંક complete = s && l[s-1] == '\n';

	જો (complete)
		s--;
	જો (s == i)
		પાછા 1;
	/* do not ignore CR at the end of an incomplete line */
	જો (complete && s == i + 1 && l[i] == '\r')
		પાછા 1;
	પાછા 0;
}

પૂર્ણાંક xdl_recmatch(અચલ અક્ષર *l1, long s1, અચલ અક્ષર *l2, long s2, long flags)
{
	પૂર્ણાંક i1, i2;

	જો (s1 == s2 && !memcmp(l1, l2, s1))
		પાછા 1;
	જો (!(flags & XDF_WHITESPACE_FLAGS))
		પાછા 0;

	i1 = 0;
	i2 = 0;

	/*
	 * -w matches everything that matches with -b, and -b in turn
	 * matches everything that matches with --ignore-space-at-eol,
	 * which in turn matches everything that matches with --ignore-cr-at-eol.
	 *
	 * Each flavor of ignoring needs different logic to skip whitespaces
	 * while we have both sides to compare.
	 */
	જો (flags & XDF_IGNORE_WHITESPACE) {
		જાઓ skip_ws;
		જ્યારે (i1 < s1 && i2 < s2) {
			જો (l1[i1++] != l2[i2++])
				પાછા 0;
		skip_ws:
			જ્યારે (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			જ્યારે (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} નહીંતર જો (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		જ્યારે (i1 < s1 && i2 < s2) {
			જો (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				જ્યારે (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				જ્યારે (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ચાલુ;
			}
			જો (l1[i1++] != l2[i2++])
				પાછા 0;
		}
	} નહીંતર જો (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		જ્યારે (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} નહીંતર જો (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		જ્યારે (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		પાછા (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	જો (i1 < s1) {
		જ્યારે (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		જો (s1 != i1)
			પાછા 0;
	}
	જો (i2 < s2) {
		જ્યારે (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		પાછા (s2 == i2);
	}
	પાછા 1;
}

unsigned long xdl_hash_record_with_whitespace(અક્ષર અચલ **data,
		અક્ષર અચલ *ટોચ, long flags) {
	unsigned long ha = 5381;
	અક્ષર અચલ *ptr = *data;
	પૂર્ણાંક cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	માટે (; ptr < ટોચ && *ptr != '\n'; ptr++) {
		જો (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			જો (*ptr == '\r' &&
			    (ptr + 1 < ટોચ && ptr[1] == '\n'))
				ચાલુ;
		}
		નહીંતર જો (XDL_ISSPACE(*ptr)) {
			અચલ અક્ષર *ptr2 = ptr;
			પૂર્ણાંક at_eol;
			જ્યારે (ptr + 1 < ટોચ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (ટોચ <= ptr + 1 || ptr[1] == '\n');
			જો (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			નહીંતર જો (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			નહીંતર જો (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				જ્યારે (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			ચાલુ;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < ટોચ ? ptr + 1: ptr;

	પાછા ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#નહીંતર
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(અક્ષર અચલ **data, અક્ષર અચલ *ટોચ) {
	unsigned long ha = 5381, c0, c1;
	અક્ષર અચલ *ptr = *data;
#જો 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	માટે (; ptr < ટોચ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < ટોચ ? ptr + 1: ptr;
#નહીંતર
	/* Process two characters per iteration. */
	જો (ટોચ - ptr >= 2) કરો {
		જો ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			પાછા ha;
		}
		જો ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			પાછા ha;
		}
		/*
		 * Combine characters C0 and C1 into the hash HA. We have
		 * HA = (HA * 33 + C0) * 33 + C1, and we want to ensure
		 * that dependency chain over HA is just one multiplication
		 * and one addition, i.e. we want to evaluate this as
		 * HA = HA * 33 * 33 + (C0 * 33 + C1), and likewise prefer
		 * (C0 * 32 + (C0 + C1)) for the expression in parenthesis.
		 */
		ha *= 33 * 33;
		c1 += c0;
		REASSOC_FENCE(c1, c0);
		c1 += c0 * 32;
		REASSOC_FENCE(c1, ha);
		ha += c1;

		ptr += 2;
	} જ્યારે (ptr < ટોચ - 1);
	*data = ટોચ;
	જો (ptr < ટોચ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	પાછા ha;
}

unsigned પૂર્ણાંક xdl_hashbits(unsigned પૂર્ણાંક size) {
	unsigned પૂર્ણાંક val = 1, bits = 0;

	માટે (; val < size && bits < CHAR_BIT * કદ(unsigned પૂર્ણાંક); val <<= 1, bits++);
	પાછા bits ? bits: 1;
}


પૂર્ણાંક xdl_અંક_out(અક્ષર *out, long val) {
	અક્ષર *ptr, *str = out;
	અક્ષર buf[32];

	ptr = buf + કદ(buf) - 1;
	*ptr = '\0';
	જો (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	માટે (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	જો (*ptr)
		માટે (; *ptr; ptr++, str++)
			*str = *ptr;
	નહીંતર
		*str++ = '0';
	*str = '\0';

	પાછા str - out;
}

સ્થિર પૂર્ણાંક xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       અચલ અક્ષર *func, long funclen,
			       xdemitcb_t *ecb) {
	પૂર્ણાંક nb = 0;
	mmbuffer_t mb;
	અક્ષર buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_અંક_out(buf + nb, c1 ? s1: s1 - 1);

	જો (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_અંક_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_અંક_out(buf + nb, c2 ? s2: s2 - 1);

	જો (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_અંક_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	જો (func && funclen) {
		buf[nb++] = ' ';
		જો ((size_t)funclen > કદ(buf) - nb - 1)
			funclen = કદ(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	જો (ecb->out_લીટી(ecb->priv, &mb, 1) < 0)
		પાછા -1;
	પાછા 0;
}

પૂર્ણાંક xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      અચલ અક્ષર *func, long funclen,
		      xdemitcb_t *ecb) {
	જો (!ecb->out_hunk)
		પાછા xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	જો (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		પાછા -1;
	પાછા 0;
}

પૂર્ણાંક xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t અચલ *xpp,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	/*
	 * This probably does not work outside Git, since
	 * we have a very simple mmfile structure.
	 *
	 * Note: ideally, we would reuse the prepared environment, but
	 * the libxdiff interface does not (yet) allow for diffing only
	 * ranges of lines instead of the whole files.
	 */
	mmfile_t subfile1, subfile2;
	xdfenv_t env;

	subfile1.ptr = (અક્ષર *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (અક્ષર *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	જો (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		પાછા -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	પાછા 0;
}

શૂન્ય* xdl_alloc_grow_helper(શૂન્ય *p, long nr, long *alloc, size_t size)
{
	શૂન્ય *tmp = ખાલી;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	જો ((size_t)nr > n)
		n = nr;
	જો (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	જો (tmp) {
		*alloc = (long)n;
	} નહીંતર {
		xdl_free(p);
		*alloc = 0;
	}
	પાછા tmp;
}
