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
	ཆེད་དུ (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ཕྱིར i;
}


ཧྲིལ xdl_emit_diffrec(var_na const *rec, long size, var_na const *pre, long psize,
		     xdemitcb_t *ecb) {
	ཧྲིལ i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (var_na *) pre;
	mb[0].size = psize;
	mb[1].ptr = (var_na *) rec;
	mb[1].size = size;
	གལ་ཏེ (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (var_na *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	གལ་ཏེ (ecb->out_line(ecb->priv, mb, i) < 0) {

		ཕྱིར -1;
	}

	ཕྱིར 0;
}

void *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	ཕྱིར mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	ཕྱིར mmf->size;
}


ཧྲིལ xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = NULL;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = NULL;
	cha->scurr = 0;

	ཕྱིར 0;
}


void xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	ཆེད་དུ (cur = cha->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


void *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	void *data;

	གལ་ཏེ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		གལ་ཏེ (!(ancur = (chanode_t *) xdl_malloc(parimaa_na(chanode_t) + cha->nsize))) {

			ཕྱིར NULL;
		}
		ancur->icurr = 0;
		ancur->next = NULL;
		གལ་ཏེ (cha->tail)
			cha->tail->next = ancur;
		གལ་ཏེ (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (var_na *) ancur + parimaa_na(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ཕྱིར data;
}

long xdl_guess_lines(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	var_na const *data, *cur, *top;

	གལ་ཏེ ((cur = data = xdl_mmfile_first(mf, &size))) {
		ཆེད་དུ (top = data + size; nl < sample && cur < top; ) {
			nl++;
			གལ་ཏེ (!(cur = memchr(cur, '\n', top - cur)))
				cur = top;
			དེ་མིན
				cur++;
		}
		tsize += (long) (cur - data);
	}

	གལ་ཏེ (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	ཕྱིར nl + 1;
}

ཧྲིལ xdl_blankline(const var_na *line, long size, long flags)
{
	long i;

	གལ་ཏེ (!(flags & XDF_WHITESPACE_FLAGS))
		ཕྱིར (size <= 1);

	ཆེད་དུ (i = 0; i < size && XDL_ISSPACE(line[i]); i++)
		;

	ཕྱིར (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
static ཧྲིལ ends_with_optional_cr(const var_na *l, long s, long i)
{
	ཧྲིལ complete = s && l[s-1] == '\n';

	གལ་ཏེ (complete)
		s--;
	གལ་ཏེ (s == i)
		ཕྱིར 1;
	/* do not ignore CR at the end of an incomplete line */
	གལ་ཏེ (complete && s == i + 1 && l[i] == '\r')
		ཕྱིར 1;
	ཕྱིར 0;
}

ཧྲིལ xdl_recmatch(const var_na *l1, long s1, const var_na *l2, long s2, long flags)
{
	ཧྲིལ i1, i2;

	གལ་ཏེ (s1 == s2 && !memcmp(l1, l2, s1))
		ཕྱིར 1;
	གལ་ཏེ (!(flags & XDF_WHITESPACE_FLAGS))
		ཕྱིར 0;

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
	གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE) {
		goto skip_ws;
		སྐབས (i1 < s1 && i2 < s2) {
			གལ་ཏེ (l1[i1++] != l2[i2++])
				ཕྱིར 0;
		skip_ws:
			སྐབས (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			སྐབས (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} དེ་མིན གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		སྐབས (i1 < s1 && i2 < s2) {
			གལ་ཏེ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				སྐབས (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				སྐབས (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				continue;
			}
			གལ་ཏེ (l1[i1++] != l2[i2++])
				ཕྱིར 0;
		}
	} དེ་མིན གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		སྐབས (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} དེ་མིན གལ་ཏེ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		སྐབས (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ཕྱིར (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	གལ་ཏེ (i1 < s1) {
		སྐབས (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		གལ་ཏེ (s1 != i1)
			ཕྱིར 0;
	}
	གལ་ཏེ (i2 < s2) {
		སྐབས (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ཕྱིར (s2 == i2);
	}
	ཕྱིར 1;
}

unsigned long xdl_hash_record_with_whitespace(var_na const **data,
		var_na const *top, long flags) {
	unsigned long ha = 5381;
	var_na const *ptr = *data;
	ཧྲིལ cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	ཆེད་དུ (; ptr < top && *ptr != '\n'; ptr++) {
		གལ་ཏེ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			གལ་ཏེ (*ptr == '\r' &&
			    (ptr + 1 < top && ptr[1] == '\n'))
				continue;
		}
		དེ་མིན གལ་ཏེ (XDL_ISSPACE(*ptr)) {
			const var_na *ptr2 = ptr;
			ཧྲིལ at_eol;
			སྐབས (ptr + 1 < top && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (top <= ptr + 1 || ptr[1] == '\n');
			གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			དེ་མིན གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			དེ་མིན གལ་ཏེ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				སྐབས (ptr2 != ptr + 1) {
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

	ཕྱིར ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#དེ་མིན
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(var_na const **data, var_na const *top) {
	unsigned long ha = 5381, c0, c1;
	var_na const *ptr = *data;
#གལ་ཏེ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	ཆེད་དུ (; ptr < top && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;
#དེ་མིན
	/* Process two characters per iteration. */
	གལ་ཏེ (top - ptr >= 2) do {
		གལ་ཏེ ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			ཕྱིར ha;
		}
		གལ་ཏེ ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ཕྱིར ha;
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
	} སྐབས (ptr < top - 1);
	*data = top;
	གལ་ཏེ (ptr < top && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	ཕྱིར ha;
}

unsigned ཧྲིལ xdl_hashbits(unsigned ཧྲིལ size) {
	unsigned ཧྲིལ val = 1, bits = 0;

	ཆེད་དུ (; val < size && bits < CHAR_BIT * parimaa_na(unsigned ཧྲིལ); val <<= 1, bits++);
	ཕྱིར bits ? bits: 1;
}


ཧྲིལ xdl_num_out(var_na *out, long val) {
	var_na *ptr, *str = out;
	var_na buf[32];

	ptr = buf + parimaa_na(buf) - 1;
	*ptr = '\0';
	གལ་ཏེ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	ཆེད་དུ (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	གལ་ཏེ (*ptr)
		ཆེད་དུ (; *ptr; ptr++, str++)
			*str = *ptr;
	དེ་མིན
		*str++ = '0';
	*str = '\0';

	ཕྱིར str - out;
}

static ཧྲིལ xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       const var_na *func, long funclen,
			       xdemitcb_t *ecb) {
	ཧྲིལ nb = 0;
	mmbuffer_t mb;
	var_na buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_num_out(buf + nb, c1 ? s1: s1 - 1);

	གལ་ཏེ (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_num_out(buf + nb, c2 ? s2: s2 - 1);

	གལ་ཏེ (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	གལ་ཏེ (func && funclen) {
		buf[nb++] = ' ';
		གལ་ཏེ ((size_t)funclen > parimaa_na(buf) - nb - 1)
			funclen = parimaa_na(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	གལ་ཏེ (ecb->out_line(ecb->priv, &mb, 1) < 0)
		ཕྱིར -1;
	ཕྱིར 0;
}

ཧྲིལ xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const var_na *func, long funclen,
		      xdemitcb_t *ecb) {
	གལ་ཏེ (!ecb->out_hunk)
		ཕྱིར xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	གལ་ཏེ (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ཕྱིར -1;
	ཕྱིར 0;
}

ཧྲིལ xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		ཧྲིལ line1, ཧྲིལ count1, ཧྲིལ line2, ཧྲིལ count2)
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

	subfile1.ptr = (var_na *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (var_na *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	གལ་ཏེ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ཕྱིར -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	ཕྱིར 0;
}

void* xdl_alloc_grow_helper(void *p, long nr, long *alloc, size_t size)
{
	void *tmp = NULL;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	གལ་ཏེ ((size_t)nr > n)
		n = nr;
	གལ་ཏེ (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	གལ་ཏེ (tmp) {
		*alloc = (long)n;
	} དེ་མིན {
		xdl_free(p);
		*alloc = 0;
	}
	ཕྱིར tmp;
}
