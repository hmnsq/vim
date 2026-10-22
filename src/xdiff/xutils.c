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

#ಅನ್ತರ್ಭಾವ "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	ಪ್ರತಿ (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ಪ್ರತ್ಯಾಗಮ i;
}


ಪೂರ್ಣಾಙ್ಕ xdl_emit_diffrec(ವರ್ಣ ಧ್ರುವ *rec, long size, ವರ್ಣ ಧ್ರುವ *pre, long psize,
		     xdemitcb_t *ecb) {
	ಪೂರ್ಣಾಙ್ಕ i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (ವರ್ಣ *) pre;
	mb[0].size = psize;
	mb[1].ptr = (ವರ್ಣ *) rec;
	mb[1].size = size;
	ಯದಿ (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (ವರ್ಣ *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	ಯದಿ (ecb->out_ಸಾಲು(ecb->priv, mb, i) < 0) {

		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	ಪ್ರತ್ಯಾಗಮ mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	ಪ್ರತ್ಯಾಗಮ mmf->size;
}


ಪೂರ್ಣಾಙ್ಕ xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = ರಿಕ್ತ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ರಿಕ್ತ;
	cha->scurr = 0;

	ಪ್ರತ್ಯಾಗಮ 0;
}


ಶೂನ್ಯ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	ಪ್ರತಿ (cur = cha->head; (tmp = cur) != ರಿಕ್ತ;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


ಶೂನ್ಯ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	ಶೂನ್ಯ *data;

	ಯದಿ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		ಯದಿ (!(ancur = (chanode_t *) xdl_malloc(ಪರಿಮಾಣ(chanode_t) + cha->nsize))) {

			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ancur->icurr = 0;
		ancur->next = ರಿಕ್ತ;
		ಯದಿ (cha->tail)
			cha->tail->next = ancur;
		ಯದಿ (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (ವರ್ಣ *) ancur + ಪರಿಮಾಣ(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ಪ್ರತ್ಯಾಗಮ data;
}

long xdl_guess_ಸಾಲುಗಳು(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	ವರ್ಣ ಧ್ರುವ *data, *cur, *ತುದಿ;

	ಯದಿ ((cur = data = xdl_mmfile_first(mf, &size))) {
		ಪ್ರತಿ (ತುದಿ = data + size; nl < sample && cur < ತುದಿ; ) {
			nl++;
			ಯದಿ (!(cur = memchr(cur, '\n', ತುದಿ - cur)))
				cur = ತುದಿ;
			ಅನ್ಯಥಾ
				cur++;
		}
		tsize += (long) (cur - data);
	}

	ಯದಿ (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	ಪ್ರತ್ಯಾಗಮ nl + 1;
}

ಪೂರ್ಣಾಙ್ಕ xdl_blankline(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, long size, long flags)
{
	long i;

	ಯದಿ (!(flags & XDF_WHITESPACE_FLAGS))
		ಪ್ರತ್ಯಾಗಮ (size <= 1);

	ಪ್ರತಿ (i = 0; i < size && XDL_ISSPACE(ಸಾಲು[i]); i++)
		;

	ಪ್ರತ್ಯಾಗಮ (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ends_with_optional_cr(ಧ್ರುವ ವರ್ಣ *l, long s, long i)
{
	ಪೂರ್ಣಾಙ್ಕ complete = s && l[s-1] == '\n';

	ಯದಿ (complete)
		s--;
	ಯದಿ (s == i)
		ಪ್ರತ್ಯಾಗಮ 1;
	/* do not ignore CR at the end of an incomplete line */
	ಯದಿ (complete && s == i + 1 && l[i] == '\r')
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ xdl_recmatch(ಧ್ರುವ ವರ್ಣ *l1, long s1, ಧ್ರುವ ವರ್ಣ *l2, long s2, long flags)
{
	ಪೂರ್ಣಾಙ್ಕ i1, i2;

	ಯದಿ (s1 == s2 && !memcmp(l1, l2, s1))
		ಪ್ರತ್ಯಾಗಮ 1;
	ಯದಿ (!(flags & XDF_WHITESPACE_FLAGS))
		ಪ್ರತ್ಯಾಗಮ 0;

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
	ಯದಿ (flags & XDF_IGNORE_WHITESPACE) {
		ಗಚ್ಛ skip_ws;
		ಯಾವತ್ (i1 < s1 && i2 < s2) {
			ಯದಿ (l1[i1++] != l2[i2++])
				ಪ್ರತ್ಯಾಗಮ 0;
		skip_ws:
			ಯಾವತ್ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			ಯಾವತ್ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} ಅನ್ಯಥಾ ಯದಿ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		ಯಾವತ್ (i1 < s1 && i2 < s2) {
			ಯದಿ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				ಯಾವತ್ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				ಯಾವತ್ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ಅನುವರ್ತ;
			}
			ಯದಿ (l1[i1++] != l2[i2++])
				ಪ್ರತ್ಯಾಗಮ 0;
		}
	} ಅನ್ಯಥಾ ಯದಿ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		ಯಾವತ್ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} ಅನ್ಯಥಾ ಯದಿ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		ಯಾವತ್ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ಪ್ರತ್ಯಾಗಮ (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	ಯದಿ (i1 < s1) {
		ಯಾವತ್ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		ಯದಿ (s1 != i1)
			ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (i2 < s2) {
		ಯಾವತ್ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ಪ್ರತ್ಯಾಗಮ (s2 == i2);
	}
	ಪ್ರತ್ಯಾಗಮ 1;
}

unsigned long xdl_hash_record_with_whitespace(ವರ್ಣ ಧ್ರುವ **data,
		ವರ್ಣ ಧ್ರುವ *ತುದಿ, long flags) {
	unsigned long ha = 5381;
	ವರ್ಣ ಧ್ರುವ *ptr = *data;
	ಪೂರ್ಣಾಙ್ಕ cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	ಪ್ರತಿ (; ptr < ತುದಿ && *ptr != '\n'; ptr++) {
		ಯದಿ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			ಯದಿ (*ptr == '\r' &&
			    (ptr + 1 < ತುದಿ && ptr[1] == '\n'))
				ಅನುವರ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ (XDL_ISSPACE(*ptr)) {
			ಧ್ರುವ ವರ್ಣ *ptr2 = ptr;
			ಪೂರ್ಣಾಙ್ಕ at_eol;
			ಯಾವತ್ (ptr + 1 < ತುದಿ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (ತುದಿ <= ptr + 1 || ptr[1] == '\n');
			ಯದಿ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			ಅನ್ಯಥಾ ಯದಿ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			ಅನ್ಯಥಾ ಯದಿ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				ಯಾವತ್ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			ಅನುವರ್ತ;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < ತುದಿ ? ptr + 1: ptr;

	ಪ್ರತ್ಯಾಗಮ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#ಅನ್ಯಥಾ
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(ವರ್ಣ ಧ್ರುವ **data, ವರ್ಣ ಧ್ರುವ *ತುದಿ) {
	unsigned long ha = 5381, c0, c1;
	ವರ್ಣ ಧ್ರುವ *ptr = *data;
#ಯದಿ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	ಪ್ರತಿ (; ptr < ತುದಿ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < ತುದಿ ? ptr + 1: ptr;
#ಅನ್ಯಥಾ
	/* Process two characters per iteration. */
	ಯದಿ (ತುದಿ - ptr >= 2) ಕುರು {
		ಯದಿ ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			ಪ್ರತ್ಯಾಗಮ ha;
		}
		ಯದಿ ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ಪ್ರತ್ಯಾಗಮ ha;
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
	} ಯಾವತ್ (ptr < ತುದಿ - 1);
	*data = ತುದಿ;
	ಯದಿ (ptr < ತುದಿ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	ಪ್ರತ್ಯಾಗಮ ha;
}

unsigned ಪೂರ್ಣಾಙ್ಕ xdl_hashbits(unsigned ಪೂರ್ಣಾಙ್ಕ size) {
	unsigned ಪೂರ್ಣಾಙ್ಕ val = 1, bits = 0;

	ಪ್ರತಿ (; val < size && bits < CHAR_BIT * ಪರಿಮಾಣ(unsigned ಪೂರ್ಣಾಙ್ಕ); val <<= 1, bits++);
	ಪ್ರತ್ಯಾಗಮ bits ? bits: 1;
}


ಪೂರ್ಣಾಙ್ಕ xdl_ಅಂಕೆ_out(ವರ್ಣ *out, long val) {
	ವರ್ಣ *ptr, *str = out;
	ವರ್ಣ buf[32];

	ptr = buf + ಪರಿಮಾಣ(buf) - 1;
	*ptr = '\0';
	ಯದಿ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	ಪ್ರತಿ (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	ಯದಿ (*ptr)
		ಪ್ರತಿ (; *ptr; ptr++, str++)
			*str = *ptr;
	ಅನ್ಯಥಾ
		*str++ = '0';
	*str = '\0';

	ಪ್ರತ್ಯಾಗಮ str - out;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ಧ್ರುವ ವರ್ಣ *func, long funclen,
			       xdemitcb_t *ecb) {
	ಪೂರ್ಣಾಙ್ಕ nb = 0;
	mmbuffer_t mb;
	ವರ್ಣ buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_ಅಂಕೆ_out(buf + nb, c1 ? s1: s1 - 1);

	ಯದಿ (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ಅಂಕೆ_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_ಅಂಕೆ_out(buf + nb, c2 ? s2: s2 - 1);

	ಯದಿ (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ಅಂಕೆ_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	ಯದಿ (func && funclen) {
		buf[nb++] = ' ';
		ಯದಿ ((size_t)funclen > ಪರಿಮಾಣ(buf) - nb - 1)
			funclen = ಪರಿಮಾಣ(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	ಯದಿ (ecb->out_ಸಾಲು(ecb->priv, &mb, 1) < 0)
		ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ಧ್ರುವ ವರ್ಣ *func, long funclen,
		      xdemitcb_t *ecb) {
	ಯದಿ (!ecb->out_hunk)
		ಪ್ರತ್ಯಾಗಮ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	ಯದಿ (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ಧ್ರುವ *xpp,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
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

	subfile1.ptr = (ವರ್ಣ *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (ವರ್ಣ *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	ಯದಿ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ಪ್ರತ್ಯಾಗಮ -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ* xdl_alloc_grow_helper(ಶೂನ್ಯ *p, long nr, long *alloc, size_t size)
{
	ಶೂನ್ಯ *tmp = ರಿಕ್ತ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	ಯದಿ ((size_t)nr > n)
		n = nr;
	ಯದಿ (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	ಯದಿ (tmp) {
		*alloc = (long)n;
	} ಅನ್ಯಥಾ {
		xdl_free(p);
		*alloc = 0;
	}
	ಪ್ರತ್ಯಾಗಮ tmp;
}
