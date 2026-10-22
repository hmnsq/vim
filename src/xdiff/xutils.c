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

#அந்தர்பாவ "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	க்கு (i = 1; n > 0; n >>= 2)
		i <<= 1;

	திரும்பு i;
}


முழு xdl_emit_diffrec(எழுத்து மாறிலி *rec, long size, எழுத்து மாறிலி *pre, long psize,
		     xdemitcb_t *ecb) {
	முழு i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (எழுத்து *) pre;
	mb[0].size = psize;
	mb[1].ptr = (எழுத்து *) rec;
	mb[1].size = size;
	எனில் (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (எழுத்து *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	எனில் (ecb->out_வரி(ecb->priv, mb, i) < 0) {

		திரும்பு -1;
	}

	திரும்பு 0;
}

வெறுமை *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	திரும்பு mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	திரும்பு mmf->size;
}


முழு xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = வெற்று;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = வெற்று;
	cha->scurr = 0;

	திரும்பு 0;
}


வெறுமை xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	க்கு (cur = cha->head; (tmp = cur) != வெற்று;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


வெறுமை *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	வெறுமை *data;

	எனில் (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		எனில் (!(ancur = (chanode_t *) xdl_malloc(அளவு(chanode_t) + cha->nsize))) {

			திரும்பு வெற்று;
		}
		ancur->icurr = 0;
		ancur->next = வெற்று;
		எனில் (cha->tail)
			cha->tail->next = ancur;
		எனில் (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (எழுத்து *) ancur + அளவு(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	திரும்பு data;
}

long xdl_guess_வரிகள்(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	எழுத்து மாறிலி *data, *cur, *உச்சி;

	எனில் ((cur = data = xdl_mmfile_first(mf, &size))) {
		க்கு (உச்சி = data + size; nl < sample && cur < உச்சி; ) {
			nl++;
			எனில் (!(cur = memchr(cur, '\n', உச்சி - cur)))
				cur = உச்சி;
			இல்லையெனில்
				cur++;
		}
		tsize += (long) (cur - data);
	}

	எனில் (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	திரும்பு nl + 1;
}

முழு xdl_blankline(மாறிலி எழுத்து *வரி, long size, long flags)
{
	long i;

	எனில் (!(flags & XDF_WHITESPACE_FLAGS))
		திரும்பு (size <= 1);

	க்கு (i = 0; i < size && XDL_ISSPACE(வரி[i]); i++)
		;

	திரும்பு (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
நிலையான முழு ends_with_optional_cr(மாறிலி எழுத்து *l, long s, long i)
{
	முழு complete = s && l[s-1] == '\n';

	எனில் (complete)
		s--;
	எனில் (s == i)
		திரும்பு 1;
	/* do not ignore CR at the end of an incomplete line */
	எனில் (complete && s == i + 1 && l[i] == '\r')
		திரும்பு 1;
	திரும்பு 0;
}

முழு xdl_recmatch(மாறிலி எழுத்து *l1, long s1, மாறிலி எழுத்து *l2, long s2, long flags)
{
	முழு i1, i2;

	எனில் (s1 == s2 && !memcmp(l1, l2, s1))
		திரும்பு 1;
	எனில் (!(flags & XDF_WHITESPACE_FLAGS))
		திரும்பு 0;

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
	எனில் (flags & XDF_IGNORE_WHITESPACE) {
		செல் skip_ws;
		போது (i1 < s1 && i2 < s2) {
			எனில் (l1[i1++] != l2[i2++])
				திரும்பு 0;
		skip_ws:
			போது (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			போது (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} இல்லையெனில் எனில் (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		போது (i1 < s1 && i2 < s2) {
			எனில் (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				போது (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				போது (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				தொடர்;
			}
			எனில் (l1[i1++] != l2[i2++])
				திரும்பு 0;
		}
	} இல்லையெனில் எனில் (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		போது (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} இல்லையெனில் எனில் (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		போது (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		திரும்பு (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	எனில் (i1 < s1) {
		போது (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		எனில் (s1 != i1)
			திரும்பு 0;
	}
	எனில் (i2 < s2) {
		போது (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		திரும்பு (s2 == i2);
	}
	திரும்பு 1;
}

unsigned long xdl_hash_record_with_whitespace(எழுத்து மாறிலி **data,
		எழுத்து மாறிலி *உச்சி, long flags) {
	unsigned long ha = 5381;
	எழுத்து மாறிலி *ptr = *data;
	முழு cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	க்கு (; ptr < உச்சி && *ptr != '\n'; ptr++) {
		எனில் (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			எனில் (*ptr == '\r' &&
			    (ptr + 1 < உச்சி && ptr[1] == '\n'))
				தொடர்;
		}
		இல்லையெனில் எனில் (XDL_ISSPACE(*ptr)) {
			மாறிலி எழுத்து *ptr2 = ptr;
			முழு at_eol;
			போது (ptr + 1 < உச்சி && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (உச்சி <= ptr + 1 || ptr[1] == '\n');
			எனில் (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			இல்லையெனில் எனில் (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			இல்லையெனில் எனில் (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				போது (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			தொடர்;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < உச்சி ? ptr + 1: ptr;

	திரும்பு ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#இல்லையெனில்
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(எழுத்து மாறிலி **data, எழுத்து மாறிலி *உச்சி) {
	unsigned long ha = 5381, c0, c1;
	எழுத்து மாறிலி *ptr = *data;
#எனில் 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	க்கு (; ptr < உச்சி && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < உச்சி ? ptr + 1: ptr;
#இல்லையெனில்
	/* Process two characters per iteration. */
	எனில் (உச்சி - ptr >= 2) செய் {
		எனில் ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			திரும்பு ha;
		}
		எனில் ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			திரும்பு ha;
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
	} போது (ptr < உச்சி - 1);
	*data = உச்சி;
	எனில் (ptr < உச்சி && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	திரும்பு ha;
}

unsigned முழு xdl_hashbits(unsigned முழு size) {
	unsigned முழு val = 1, bits = 0;

	க்கு (; val < size && bits < CHAR_BIT * அளவு(unsigned முழு); val <<= 1, bits++);
	திரும்பு bits ? bits: 1;
}


முழு xdl_இலக்கம்_out(எழுத்து *out, long val) {
	எழுத்து *ptr, *str = out;
	எழுத்து buf[32];

	ptr = buf + அளவு(buf) - 1;
	*ptr = '\0';
	எனில் (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	க்கு (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	எனில் (*ptr)
		க்கு (; *ptr; ptr++, str++)
			*str = *ptr;
	இல்லையெனில்
		*str++ = '0';
	*str = '\0';

	திரும்பு str - out;
}

நிலையான முழு xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       மாறிலி எழுத்து *func, long funclen,
			       xdemitcb_t *ecb) {
	முழு nb = 0;
	mmbuffer_t mb;
	எழுத்து buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_இலக்கம்_out(buf + nb, c1 ? s1: s1 - 1);

	எனில் (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_இலக்கம்_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_இலக்கம்_out(buf + nb, c2 ? s2: s2 - 1);

	எனில் (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_இலக்கம்_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	எனில் (func && funclen) {
		buf[nb++] = ' ';
		எனில் ((size_t)funclen > அளவு(buf) - nb - 1)
			funclen = அளவு(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	எனில் (ecb->out_வரி(ecb->priv, &mb, 1) < 0)
		திரும்பு -1;
	திரும்பு 0;
}

முழு xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      மாறிலி எழுத்து *func, long funclen,
		      xdemitcb_t *ecb) {
	எனில் (!ecb->out_hunk)
		திரும்பு xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	எனில் (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		திரும்பு -1;
	திரும்பு 0;
}

முழு xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t மாறிலி *xpp,
		முழு line1, முழு count1, முழு line2, முழு count2)
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

	subfile1.ptr = (எழுத்து *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (எழுத்து *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	எனில் (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		திரும்பு -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	திரும்பு 0;
}

வெறுமை* xdl_alloc_grow_helper(வெறுமை *p, long nr, long *alloc, size_t size)
{
	வெறுமை *tmp = வெற்று;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	எனில் ((size_t)nr > n)
		n = nr;
	எனில் (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	எனில் (tmp) {
		*alloc = (long)n;
	} இல்லையெனில் {
		xdl_free(p);
		*alloc = 0;
	}
	திரும்பு tmp;
}
