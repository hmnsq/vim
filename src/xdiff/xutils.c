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

#شامل "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	برای (i = 1; n > 0; n >>= 2)
		i <<= 1;

	بازگشت i;
}


صحیح xdl_emit_diffrec(نویسه ثابت *rec, long size, نویسه ثابت *pre, long psize,
		     xdemitcb_t *ecb) {
	صحیح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (نویسه *) pre;
	mb[0].size = psize;
	mb[1].ptr = (نویسه *) rec;
	mb[1].size = size;
	اگر (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (نویسه *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	اگر (ecb->out_سطر(ecb->priv, mb, i) < 0) {

		بازگشت -1;
	}

	بازگشت 0;
}

تهی *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	بازگشت mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	بازگشت mmf->size;
}


صحیح xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = پوچ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = پوچ;
	cha->scurr = 0;

	بازگشت 0;
}


تهی xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	برای (cur = cha->head; (tmp = cur) != پوچ;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


تهی *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	تهی *data;

	اگر (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		اگر (!(ancur = (chanode_t *) xdl_malloc(اندازه(chanode_t) + cha->nsize))) {

			بازگشت پوچ;
		}
		ancur->icurr = 0;
		ancur->next = پوچ;
		اگر (cha->tail)
			cha->tail->next = ancur;
		اگر (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (نویسه *) ancur + اندازه(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	بازگشت data;
}

long xdl_guess_سطرها(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	نویسه ثابت *data, *cur, *بالاترین;

	اگر ((cur = data = xdl_mmfile_first(mf, &size))) {
		برای (بالاترین = data + size; nl < sample && cur < بالاترین; ) {
			nl++;
			اگر (!(cur = memchr(cur, '\n', بالاترین - cur)))
				cur = بالاترین;
			وگرنه
				cur++;
		}
		tsize += (long) (cur - data);
	}

	اگر (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	بازگشت nl + 1;
}

صحیح xdl_blankline(ثابت نویسه *سطر, long size, long flags)
{
	long i;

	اگر (!(flags & XDF_WHITESPACE_FLAGS))
		بازگشت (size <= 1);

	برای (i = 0; i < size && XDL_ISSPACE(سطر[i]); i++)
		;

	بازگشت (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ایستا صحیح ends_with_optional_cr(ثابت نویسه *l, long s, long i)
{
	صحیح complete = s && l[s-1] == '\n';

	اگر (complete)
		s--;
	اگر (s == i)
		بازگشت 1;
	/* do not ignore CR at the end of an incomplete line */
	اگر (complete && s == i + 1 && l[i] == '\r')
		بازگشت 1;
	بازگشت 0;
}

صحیح xdl_recmatch(ثابت نویسه *l1, long s1, ثابت نویسه *l2, long s2, long flags)
{
	صحیح i1, i2;

	اگر (s1 == s2 && !memcmp(l1, l2, s1))
		بازگشت 1;
	اگر (!(flags & XDF_WHITESPACE_FLAGS))
		بازگشت 0;

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
	اگر (flags & XDF_IGNORE_WHITESPACE) {
		برو skip_ws;
		وقتی (i1 < s1 && i2 < s2) {
			اگر (l1[i1++] != l2[i2++])
				بازگشت 0;
		skip_ws:
			وقتی (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			وقتی (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} وگرنه اگر (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		وقتی (i1 < s1 && i2 < s2) {
			اگر (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				وقتی (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				وقتی (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ادامه;
			}
			اگر (l1[i1++] != l2[i2++])
				بازگشت 0;
		}
	} وگرنه اگر (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		وقتی (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} وگرنه اگر (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		وقتی (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		بازگشت (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	اگر (i1 < s1) {
		وقتی (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		اگر (s1 != i1)
			بازگشت 0;
	}
	اگر (i2 < s2) {
		وقتی (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		بازگشت (s2 == i2);
	}
	بازگشت 1;
}

unsigned long xdl_hash_record_with_whitespace(نویسه ثابت **data,
		نویسه ثابت *بالاترین, long flags) {
	unsigned long ha = 5381;
	نویسه ثابت *ptr = *data;
	صحیح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	برای (; ptr < بالاترین && *ptr != '\n'; ptr++) {
		اگر (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			اگر (*ptr == '\r' &&
			    (ptr + 1 < بالاترین && ptr[1] == '\n'))
				ادامه;
		}
		وگرنه اگر (XDL_ISSPACE(*ptr)) {
			ثابت نویسه *ptr2 = ptr;
			صحیح at_eol;
			وقتی (ptr + 1 < بالاترین && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (بالاترین <= ptr + 1 || ptr[1] == '\n');
			اگر (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			وگرنه اگر (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			وگرنه اگر (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				وقتی (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			ادامه;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < بالاترین ? ptr + 1: ptr;

	بازگشت ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#وگرنه
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(نویسه ثابت **data, نویسه ثابت *بالاترین) {
	unsigned long ha = 5381, c0, c1;
	نویسه ثابت *ptr = *data;
#اگر 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	برای (; ptr < بالاترین && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < بالاترین ? ptr + 1: ptr;
#وگرنه
	/* Process two characters per iteration. */
	اگر (بالاترین - ptr >= 2) انجامبده {
		اگر ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			بازگشت ha;
		}
		اگر ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			بازگشت ha;
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
	} وقتی (ptr < بالاترین - 1);
	*data = بالاترین;
	اگر (ptr < بالاترین && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	بازگشت ha;
}

unsigned صحیح xdl_hashbits(unsigned صحیح size) {
	unsigned صحیح val = 1, bits = 0;

	برای (; val < size && bits < CHAR_BIT * اندازه(unsigned صحیح); val <<= 1, bits++);
	بازگشت bits ? bits: 1;
}


صحیح xdl_رقم_out(نویسه *out, long val) {
	نویسه *ptr, *str = out;
	نویسه buf[32];

	ptr = buf + اندازه(buf) - 1;
	*ptr = '\0';
	اگر (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	برای (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	اگر (*ptr)
		برای (; *ptr; ptr++, str++)
			*str = *ptr;
	وگرنه
		*str++ = '0';
	*str = '\0';

	بازگشت str - out;
}

ایستا صحیح xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ثابت نویسه *func, long funclen,
			       xdemitcb_t *ecb) {
	صحیح nb = 0;
	mmbuffer_t mb;
	نویسه buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_رقم_out(buf + nb, c1 ? s1: s1 - 1);

	اگر (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_رقم_out(buf + nb, c2 ? s2: s2 - 1);

	اگر (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	اگر (func && funclen) {
		buf[nb++] = ' ';
		اگر ((size_t)funclen > اندازه(buf) - nb - 1)
			funclen = اندازه(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	اگر (ecb->out_سطر(ecb->priv, &mb, 1) < 0)
		بازگشت -1;
	بازگشت 0;
}

صحیح xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ثابت نویسه *func, long funclen,
		      xdemitcb_t *ecb) {
	اگر (!ecb->out_hunk)
		بازگشت xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	اگر (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		بازگشت -1;
	بازگشت 0;
}

صحیح xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
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

	subfile1.ptr = (نویسه *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (نویسه *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	اگر (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		بازگشت -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	بازگشت 0;
}

تهی* xdl_alloc_grow_helper(تهی *p, long nr, long *alloc, size_t size)
{
	تهی *tmp = پوچ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	اگر ((size_t)nr > n)
		n = nr;
	اگر (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	اگر (tmp) {
		*alloc = (long)n;
	} وگرنه {
		xdl_free(p);
		*alloc = 0;
	}
	بازگشت tmp;
}
