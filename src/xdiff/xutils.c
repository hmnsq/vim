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

#ଅନ୍ତର୍ଭାଵ "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	ପାଇଁ (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ଫେରନ୍ତୁ i;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_diffrec(ଅକ୍ଷର ଅଚଳ *rec, long size, ଅକ୍ଷର ଅଚଳ *pre, long psize,
		     xdemitcb_t *ecb) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (ଅକ୍ଷର *) pre;
	mb[0].size = psize;
	mb[1].ptr = (ଅକ୍ଷର *) rec;
	mb[1].size = size;
	ଯଦି (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (ଅକ୍ଷର *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	ଯଦି (ecb->out_ଧାଡ଼ି(ecb->priv, mb, i) < 0) {

		ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	ଫେରନ୍ତୁ mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	ଫେରନ୍ତୁ mmf->size;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = ଖାଲି;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ଖାଲି;
	cha->scurr = 0;

	ଫେରନ୍ତୁ 0;
}


ଶୂନ୍ୟ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	ପାଇଁ (cur = cha->head; (tmp = cur) != ଖାଲି;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


ଶୂନ୍ୟ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	ଶୂନ୍ୟ *data;

	ଯଦି (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		ଯଦି (!(ancur = (chanode_t *) xdl_malloc(ଆକାର(chanode_t) + cha->nsize))) {

			ଫେରନ୍ତୁ ଖାଲି;
		}
		ancur->icurr = 0;
		ancur->next = ଖାଲି;
		ଯଦି (cha->tail)
			cha->tail->next = ancur;
		ଯଦି (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (ଅକ୍ଷର *) ancur + ଆକାର(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ଫେରନ୍ତୁ data;
}

long xdl_guess_ଧାଡ଼ିଗୁଡ଼ିକ(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	ଅକ୍ଷର ଅଚଳ *data, *cur, *ଶୀର୍ଷ;

	ଯଦି ((cur = data = xdl_mmfile_first(mf, &size))) {
		ପାଇଁ (ଶୀର୍ଷ = data + size; nl < sample && cur < ଶୀର୍ଷ; ) {
			nl++;
			ଯଦି (!(cur = memchr(cur, '\n', ଶୀର୍ଷ - cur)))
				cur = ଶୀର୍ଷ;
			ଅନ୍ୟଥା
				cur++;
		}
		tsize += (long) (cur - data);
	}

	ଯଦି (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	ଫେରନ୍ତୁ nl + 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_blankline(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, long size, long flags)
{
	long i;

	ଯଦି (!(flags & XDF_WHITESPACE_FLAGS))
		ଫେରନ୍ତୁ (size <= 1);

	ପାଇଁ (i = 0; i < size && XDL_ISSPACE(ଧାଡ଼ି[i]); i++)
		;

	ଫେରନ୍ତୁ (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ends_with_optional_cr(ଅଚଳ ଅକ୍ଷର *l, long s, long i)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା complete = s && l[s-1] == '\n';

	ଯଦି (complete)
		s--;
	ଯଦି (s == i)
		ଫେରନ୍ତୁ 1;
	/* do not ignore CR at the end of an incomplete line */
	ଯଦି (complete && s == i + 1 && l[i] == '\r')
		ଫେରନ୍ତୁ 1;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_recmatch(ଅଚଳ ଅକ୍ଷର *l1, long s1, ଅଚଳ ଅକ୍ଷର *l2, long s2, long flags)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i1, i2;

	ଯଦି (s1 == s2 && !memcmp(l1, l2, s1))
		ଫେରନ୍ତୁ 1;
	ଯଦି (!(flags & XDF_WHITESPACE_FLAGS))
		ଫେରନ୍ତୁ 0;

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
	ଯଦି (flags & XDF_IGNORE_WHITESPACE) {
		ଯାଅ skip_ws;
		ଯେତେବେଳେ (i1 < s1 && i2 < s2) {
			ଯଦି (l1[i1++] != l2[i2++])
				ଫେରନ୍ତୁ 0;
		skip_ws:
			ଯେତେବେଳେ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			ଯେତେବେଳେ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} ଅନ୍ୟଥା ଯଦି (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		ଯେତେବେଳେ (i1 < s1 && i2 < s2) {
			ଯଦି (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				ଯେତେବେଳେ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				ଯେତେବେଳେ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ଜାରି;
			}
			ଯଦି (l1[i1++] != l2[i2++])
				ଫେରନ୍ତୁ 0;
		}
	} ଅନ୍ୟଥା ଯଦି (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		ଯେତେବେଳେ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} ଅନ୍ୟଥା ଯଦି (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		ଯେତେବେଳେ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ଫେରନ୍ତୁ (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	ଯଦି (i1 < s1) {
		ଯେତେବେଳେ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		ଯଦି (s1 != i1)
			ଫେରନ୍ତୁ 0;
	}
	ଯଦି (i2 < s2) {
		ଯେତେବେଳେ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ଫେରନ୍ତୁ (s2 == i2);
	}
	ଫେରନ୍ତୁ 1;
}

unsigned long xdl_hash_record_with_whitespace(ଅକ୍ଷର ଅଚଳ **data,
		ଅକ୍ଷର ଅଚଳ *ଶୀର୍ଷ, long flags) {
	unsigned long ha = 5381;
	ଅକ୍ଷର ଅଚଳ *ptr = *data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	ପାଇଁ (; ptr < ଶୀର୍ଷ && *ptr != '\n'; ptr++) {
		ଯଦି (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			ଯଦି (*ptr == '\r' &&
			    (ptr + 1 < ଶୀର୍ଷ && ptr[1] == '\n'))
				ଜାରି;
		}
		ଅନ୍ୟଥା ଯଦି (XDL_ISSPACE(*ptr)) {
			ଅଚଳ ଅକ୍ଷର *ptr2 = ptr;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_eol;
			ଯେତେବେଳେ (ptr + 1 < ଶୀର୍ଷ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (ଶୀର୍ଷ <= ptr + 1 || ptr[1] == '\n');
			ଯଦି (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			ଅନ୍ୟଥା ଯଦି (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			ଅନ୍ୟଥା ଯଦି (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				ଯେତେବେଳେ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			ଜାରି;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < ଶୀର୍ଷ ? ptr + 1: ptr;

	ଫେରନ୍ତୁ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#ଅନ୍ୟଥା
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(ଅକ୍ଷର ଅଚଳ **data, ଅକ୍ଷର ଅଚଳ *ଶୀର୍ଷ) {
	unsigned long ha = 5381, c0, c1;
	ଅକ୍ଷର ଅଚଳ *ptr = *data;
#ଯଦି 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	ପାଇଁ (; ptr < ଶୀର୍ଷ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < ଶୀର୍ଷ ? ptr + 1: ptr;
#ଅନ୍ୟଥା
	/* Process two characters per iteration. */
	ଯଦି (ଶୀର୍ଷ - ptr >= 2) କର {
		ଯଦି ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			ଫେରନ୍ତୁ ha;
		}
		ଯଦି ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ଫେରନ୍ତୁ ha;
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
	} ଯେତେବେଳେ (ptr < ଶୀର୍ଷ - 1);
	*data = ଶୀର୍ଷ;
	ଯଦି (ptr < ଶୀର୍ଷ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	ଫେରନ୍ତୁ ha;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_hashbits(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା size) {
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = 1, bits = 0;

	ପାଇଁ (; val < size && bits < CHAR_BIT * ଆକାର(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା); val <<= 1, bits++);
	ଫେରନ୍ତୁ bits ? bits: 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_ଅଙ୍କ_out(ଅକ୍ଷର *out, long val) {
	ଅକ୍ଷର *ptr, *str = out;
	ଅକ୍ଷର buf[32];

	ptr = buf + ଆକାର(buf) - 1;
	*ptr = '\0';
	ଯଦି (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	ପାଇଁ (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	ଯଦି (*ptr)
		ପାଇଁ (; *ptr; ptr++, str++)
			*str = *ptr;
	ଅନ୍ୟଥା
		*str++ = '0';
	*str = '\0';

	ଫେରନ୍ତୁ str - out;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ଅଚଳ ଅକ୍ଷର *func, long funclen,
			       xdemitcb_t *ecb) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nb = 0;
	mmbuffer_t mb;
	ଅକ୍ଷର buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_ଅଙ୍କ_out(buf + nb, c1 ? s1: s1 - 1);

	ଯଦି (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ଅଙ୍କ_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_ଅଙ୍କ_out(buf + nb, c2 ? s2: s2 - 1);

	ଯଦି (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ଅଙ୍କ_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	ଯଦି (func && funclen) {
		buf[nb++] = ' ';
		ଯଦି ((size_t)funclen > ଆକାର(buf) - nb - 1)
			funclen = ଆକାର(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	ଯଦି (ecb->out_ଧାଡ଼ି(ecb->priv, &mb, 1) < 0)
		ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ଅଚଳ ଅକ୍ଷର *func, long funclen,
		      xdemitcb_t *ecb) {
	ଯଦି (!ecb->out_hunk)
		ଫେରନ୍ତୁ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	ଯଦି (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ଅଚଳ *xpp,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
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

	subfile1.ptr = (ଅକ୍ଷର *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (ଅକ୍ଷର *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	ଯଦି (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ଫେରନ୍ତୁ -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ* xdl_alloc_grow_helper(ଶୂନ୍ୟ *p, long nr, long *alloc, size_t size)
{
	ଶୂନ୍ୟ *tmp = ଖାଲି;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	ଯଦି ((size_t)nr > n)
		n = nr;
	ଯଦି (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	ଯଦି (tmp) {
		*alloc = (long)n;
	} ଅନ୍ୟଥା {
		xdl_free(p);
		*alloc = 0;
	}
	ଫେରନ୍ତୁ tmp;
}
