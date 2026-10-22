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

#ضمن "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	لكل (i = 1; n > 0; n >>= 2)
		i <<= 1;

	إرجاع i;
}


صحيح xdl_emit_diffrec(حرف ثابت *rec, long size, حرف ثابت *pre, long psize,
		     xdemitcb_t *ecb) {
	صحيح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (حرف *) pre;
	mb[0].size = psize;
	mb[1].ptr = (حرف *) rec;
	mb[1].size = size;
	إذا (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (حرف *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	إذا (ecb->out_سطر(ecb->priv, mb, i) < 0) {

		إرجاع -1;
	}

	إرجاع 0;
}

فراغ *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	إرجاع mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	إرجاع mmf->size;
}


صحيح xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = عدم;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = عدم;
	cha->scurr = 0;

	إرجاع 0;
}


فراغ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	لكل (cur = cha->head; (tmp = cur) != عدم;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


فراغ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	فراغ *data;

	إذا (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		إذا (!(ancur = (chanode_t *) xdl_malloc(حجم(chanode_t) + cha->nsize))) {

			إرجاع عدم;
		}
		ancur->icurr = 0;
		ancur->next = عدم;
		إذا (cha->tail)
			cha->tail->next = ancur;
		إذا (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (حرف *) ancur + حجم(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	إرجاع data;
}

long xdl_guess_أسطر(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	حرف ثابت *data, *cur, *قمة;

	إذا ((cur = data = xdl_mmfile_first(mf, &size))) {
		لكل (قمة = data + size; nl < sample && cur < قمة; ) {
			nl++;
			إذا (!(cur = memchr(cur, '\n', قمة - cur)))
				cur = قمة;
			وإلا
				cur++;
		}
		tsize += (long) (cur - data);
	}

	إذا (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	إرجاع nl + 1;
}

صحيح xdl_blankline(ثابت حرف *سطر, long size, long flags)
{
	long i;

	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		إرجاع (size <= 1);

	لكل (i = 0; i < size && XDL_ISSPACE(سطر[i]); i++)
		;

	إرجاع (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ساكن صحيح ends_with_optional_cr(ثابت حرف *l, long s, long i)
{
	صحيح complete = s && l[s-1] == '\n';

	إذا (complete)
		s--;
	إذا (s == i)
		إرجاع 1;
	/* do not ignore CR at the end of an incomplete line */
	إذا (complete && s == i + 1 && l[i] == '\r')
		إرجاع 1;
	إرجاع 0;
}

صحيح xdl_recmatch(ثابت حرف *l1, long s1, ثابت حرف *l2, long s2, long flags)
{
	صحيح i1, i2;

	إذا (s1 == s2 && !memcmp(l1, l2, s1))
		إرجاع 1;
	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		إرجاع 0;

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
	إذا (flags & XDF_IGNORE_WHITESPACE) {
		اذهب skip_ws;
		بينما (i1 < s1 && i2 < s2) {
			إذا (l1[i1++] != l2[i2++])
				إرجاع 0;
		skip_ws:
			بينما (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			بينما (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		بينما (i1 < s1 && i2 < s2) {
			إذا (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				بينما (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				بينما (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				استمر;
			}
			إذا (l1[i1++] != l2[i2++])
				إرجاع 0;
		}
	} وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		بينما (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} وإلا إذا (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		بينما (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		إرجاع (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	إذا (i1 < s1) {
		بينما (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		إذا (s1 != i1)
			إرجاع 0;
	}
	إذا (i2 < s2) {
		بينما (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		إرجاع (s2 == i2);
	}
	إرجاع 1;
}

unsigned long xdl_hash_record_with_whitespace(حرف ثابت **data,
		حرف ثابت *قمة, long flags) {
	unsigned long ha = 5381;
	حرف ثابت *ptr = *data;
	صحيح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	لكل (; ptr < قمة && *ptr != '\n'; ptr++) {
		إذا (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			إذا (*ptr == '\r' &&
			    (ptr + 1 < قمة && ptr[1] == '\n'))
				استمر;
		}
		وإلا إذا (XDL_ISSPACE(*ptr)) {
			ثابت حرف *ptr2 = ptr;
			صحيح at_eol;
			بينما (ptr + 1 < قمة && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (قمة <= ptr + 1 || ptr[1] == '\n');
			إذا (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				بينما (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			استمر;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < قمة ? ptr + 1: ptr;

	إرجاع ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#وإلا
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(حرف ثابت **data, حرف ثابت *قمة) {
	unsigned long ha = 5381, c0, c1;
	حرف ثابت *ptr = *data;
#إذا 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	لكل (; ptr < قمة && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < قمة ? ptr + 1: ptr;
#وإلا
	/* Process two characters per iteration. */
	إذا (قمة - ptr >= 2) افعل {
		إذا ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			إرجاع ha;
		}
		إذا ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			إرجاع ha;
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
	} بينما (ptr < قمة - 1);
	*data = قمة;
	إذا (ptr < قمة && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	إرجاع ha;
}

unsigned صحيح xdl_hashbits(unsigned صحيح size) {
	unsigned صحيح val = 1, bits = 0;

	لكل (; val < size && bits < CHAR_BIT * حجم(unsigned صحيح); val <<= 1, bits++);
	إرجاع bits ? bits: 1;
}


صحيح xdl_رقم_out(حرف *out, long val) {
	حرف *ptr, *str = out;
	حرف buf[32];

	ptr = buf + حجم(buf) - 1;
	*ptr = '\0';
	إذا (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	لكل (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	إذا (*ptr)
		لكل (; *ptr; ptr++, str++)
			*str = *ptr;
	وإلا
		*str++ = '0';
	*str = '\0';

	إرجاع str - out;
}

ساكن صحيح xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ثابت حرف *func, long funclen,
			       xdemitcb_t *ecb) {
	صحيح nb = 0;
	mmbuffer_t mb;
	حرف buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_رقم_out(buf + nb, c1 ? s1: s1 - 1);

	إذا (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_رقم_out(buf + nb, c2 ? s2: s2 - 1);

	إذا (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	إذا (func && funclen) {
		buf[nb++] = ' ';
		إذا ((size_t)funclen > حجم(buf) - nb - 1)
			funclen = حجم(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	إذا (ecb->out_سطر(ecb->priv, &mb, 1) < 0)
		إرجاع -1;
	إرجاع 0;
}

صحيح xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ثابت حرف *func, long funclen,
		      xdemitcb_t *ecb) {
	إذا (!ecb->out_hunk)
		إرجاع xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	إذا (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		إرجاع -1;
	إرجاع 0;
}

صحيح xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
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

	subfile1.ptr = (حرف *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (حرف *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	إذا (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		إرجاع -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	إرجاع 0;
}

فراغ* xdl_alloc_grow_helper(فراغ *p, long nr, long *alloc, size_t size)
{
	فراغ *tmp = عدم;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	إذا ((size_t)nr > n)
		n = nr;
	إذا (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	إذا (tmp) {
		*alloc = (long)n;
	} وإلا {
		xdl_free(p);
		*alloc = 0;
	}
	إرجاع tmp;
}
