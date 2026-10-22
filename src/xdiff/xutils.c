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

#include "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	cho (i = 1; n > 0; n >>= 2)
		i <<= 1;

	trả_về i;
}


số_nguyên xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb) {
	số_nguyên i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (char *) pre;
	mb[0].size = psize;
	mb[1].ptr = (char *) rec;
	mb[1].size = size;
	nếu (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (char *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	nếu (ecb->out_line(ecb->priv, mb, i) < 0) {

		trả_về -1;
	}

	trả_về 0;
}

void *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	trả_về mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	trả_về mmf->size;
}


số_nguyên xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = NULL;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = NULL;
	cha->scurr = 0;

	trả_về 0;
}


void xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	cho (cur = cha->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


void *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	void *data;

	nếu (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		nếu (!(ancur = (chanode_t *) xdl_malloc(sizeof(chanode_t) + cha->nsize))) {

			trả_về NULL;
		}
		ancur->icurr = 0;
		ancur->next = NULL;
		nếu (cha->tail)
			cha->tail->next = ancur;
		nếu (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (char *) ancur + sizeof(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	trả_về data;
}

long xdl_guess_lines(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	char const *data, *cur, *top;

	nếu ((cur = data = xdl_mmfile_first(mf, &size))) {
		cho (top = data + size; nl < sample && cur < top; ) {
			nl++;
			nếu (!(cur = memchr(cur, '\n', top - cur)))
				cur = top;
			không_thì
				cur++;
		}
		tsize += (long) (cur - data);
	}

	nếu (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	trả_về nl + 1;
}

số_nguyên xdl_blankline(const char *line, long size, long flags)
{
	long i;

	nếu (!(flags & XDF_WHITESPACE_FLAGS))
		trả_về (size <= 1);

	cho (i = 0; i < size && XDL_ISSPACE(line[i]); i++)
		;

	trả_về (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
static số_nguyên ends_with_optional_cr(const char *l, long s, long i)
{
	số_nguyên complete = s && l[s-1] == '\n';

	nếu (complete)
		s--;
	nếu (s == i)
		trả_về 1;
	/* do not ignore CR at the end of an incomplete line */
	nếu (complete && s == i + 1 && l[i] == '\r')
		trả_về 1;
	trả_về 0;
}

số_nguyên xdl_recmatch(const char *l1, long s1, const char *l2, long s2, long flags)
{
	số_nguyên i1, i2;

	nếu (s1 == s2 && !memcmp(l1, l2, s1))
		trả_về 1;
	nếu (!(flags & XDF_WHITESPACE_FLAGS))
		trả_về 0;

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
	nếu (flags & XDF_IGNORE_WHITESPACE) {
		goto skip_ws;
		trong_khi (i1 < s1 && i2 < s2) {
			nếu (l1[i1++] != l2[i2++])
				trả_về 0;
		skip_ws:
			trong_khi (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			trong_khi (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} không_thì nếu (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		trong_khi (i1 < s1 && i2 < s2) {
			nếu (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				trong_khi (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				trong_khi (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				continue;
			}
			nếu (l1[i1++] != l2[i2++])
				trả_về 0;
		}
	} không_thì nếu (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		trong_khi (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} không_thì nếu (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		trong_khi (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		trả_về (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	nếu (i1 < s1) {
		trong_khi (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		nếu (s1 != i1)
			trả_về 0;
	}
	nếu (i2 < s2) {
		trong_khi (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		trả_về (s2 == i2);
	}
	trả_về 1;
}

unsigned long xdl_hash_record_with_whitespace(char const **data,
		char const *top, long flags) {
	unsigned long ha = 5381;
	char const *ptr = *data;
	số_nguyên cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	cho (; ptr < top && *ptr != '\n'; ptr++) {
		nếu (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			nếu (*ptr == '\r' &&
			    (ptr + 1 < top && ptr[1] == '\n'))
				continue;
		}
		không_thì nếu (XDL_ISSPACE(*ptr)) {
			const char *ptr2 = ptr;
			số_nguyên at_eol;
			trong_khi (ptr + 1 < top && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (top <= ptr + 1 || ptr[1] == '\n');
			nếu (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			không_thì nếu (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			không_thì nếu (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				trong_khi (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			continue;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;

	trả_về ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#không_thì
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(char const **data, char const *top) {
	unsigned long ha = 5381, c0, c1;
	char const *ptr = *data;
#nếu 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	cho (; ptr < top && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;
#không_thì
	/* Process two characters per iteration. */
	nếu (top - ptr >= 2) do {
		nếu ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			trả_về ha;
		}
		nếu ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			trả_về ha;
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
	} trong_khi (ptr < top - 1);
	*data = top;
	nếu (ptr < top && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	trả_về ha;
}

unsigned số_nguyên xdl_hashbits(unsigned số_nguyên size) {
	unsigned số_nguyên val = 1, bits = 0;

	cho (; val < size && bits < CHAR_BIT * sizeof(unsigned số_nguyên); val <<= 1, bits++);
	trả_về bits ? bits: 1;
}


số_nguyên xdl_num_out(char *out, long val) {
	char *ptr, *str = out;
	char buf[32];

	ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';
	nếu (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	cho (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	nếu (*ptr)
		cho (; *ptr; ptr++, str++)
			*str = *ptr;
	không_thì
		*str++ = '0';
	*str = '\0';

	trả_về str - out;
}

static số_nguyên xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       const char *func, long funclen,
			       xdemitcb_t *ecb) {
	số_nguyên nb = 0;
	mmbuffer_t mb;
	char buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_num_out(buf + nb, c1 ? s1: s1 - 1);

	nếu (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_num_out(buf + nb, c2 ? s2: s2 - 1);

	nếu (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	nếu (func && funclen) {
		buf[nb++] = ' ';
		nếu ((size_t)funclen > sizeof(buf) - nb - 1)
			funclen = sizeof(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	nếu (ecb->out_line(ecb->priv, &mb, 1) < 0)
		trả_về -1;
	trả_về 0;
}

số_nguyên xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const char *func, long funclen,
		      xdemitcb_t *ecb) {
	nếu (!ecb->out_hunk)
		trả_về xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	nếu (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		trả_về -1;
	trả_về 0;
}

số_nguyên xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
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

	subfile1.ptr = (char *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (char *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	nếu (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		trả_về -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	trả_về 0;
}

void* xdl_alloc_grow_helper(void *p, long nr, long *alloc, size_t size)
{
	void *tmp = NULL;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	nếu ((size_t)nr > n)
		n = nr;
	nếu (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	nếu (tmp) {
		*alloc = (long)n;
	} không_thì {
		xdl_free(p);
		*alloc = 0;
	}
	trả_về tmp;
}
