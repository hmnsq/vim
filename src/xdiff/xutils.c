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

#অন্তর্ভাব "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	প্রতি (i = 1; n > 0; n >>= 2)
		i <<= 1;

	প্রত্যাগম i;
}


পূর্ণাঙ্ক xdl_emit_diffrec(বর্ণ ধ্রুব *rec, long size, বর্ণ ধ্রুব *pre, long psize,
		     xdemitcb_t *ecb) {
	পূর্ণাঙ্ক i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (বর্ণ *) pre;
	mb[0].size = psize;
	mb[1].ptr = (বর্ণ *) rec;
	mb[1].size = size;
	যদি (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (বর্ণ *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	যদি (ecb->out_সারি(ecb->priv, mb, i) < 0) {

		প্রত্যাগম -1;
	}

	প্রত্যাগম 0;
}

শূন্য *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	প্রত্যাগম mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	প্রত্যাগম mmf->size;
}


পূর্ণাঙ্ক xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = রিক্ত;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = রিক্ত;
	cha->scurr = 0;

	প্রত্যাগম 0;
}


শূন্য xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	প্রতি (cur = cha->head; (tmp = cur) != রিক্ত;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


শূন্য *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	শূন্য *data;

	যদি (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		যদি (!(ancur = (chanode_t *) xdl_malloc(পরিমাণ(chanode_t) + cha->nsize))) {

			প্রত্যাগম রিক্ত;
		}
		ancur->icurr = 0;
		ancur->next = রিক্ত;
		যদি (cha->tail)
			cha->tail->next = ancur;
		যদি (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (বর্ণ *) ancur + পরিমাণ(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	প্রত্যাগম data;
}

long xdl_guess_সারিগুলি(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	বর্ণ ধ্রুব *data, *cur, *শীর্ষ;

	যদি ((cur = data = xdl_mmfile_first(mf, &size))) {
		প্রতি (শীর্ষ = data + size; nl < sample && cur < শীর্ষ; ) {
			nl++;
			যদি (!(cur = memchr(cur, '\n', শীর্ষ - cur)))
				cur = শীর্ষ;
			অন্যথা
				cur++;
		}
		tsize += (long) (cur - data);
	}

	যদি (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	প্রত্যাগম nl + 1;
}

পূর্ণাঙ্ক xdl_blankline(ধ্রুব বর্ণ *সারি, long size, long flags)
{
	long i;

	যদি (!(flags & XDF_WHITESPACE_FLAGS))
		প্রত্যাগম (size <= 1);

	প্রতি (i = 0; i < size && XDL_ISSPACE(সারি[i]); i++)
		;

	প্রত্যাগম (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
স্থির পূর্ণাঙ্ক ends_with_optional_cr(ধ্রুব বর্ণ *l, long s, long i)
{
	পূর্ণাঙ্ক complete = s && l[s-1] == '\n';

	যদি (complete)
		s--;
	যদি (s == i)
		প্রত্যাগম 1;
	/* do not ignore CR at the end of an incomplete line */
	যদি (complete && s == i + 1 && l[i] == '\r')
		প্রত্যাগম 1;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক xdl_recmatch(ধ্রুব বর্ণ *l1, long s1, ধ্রুব বর্ণ *l2, long s2, long flags)
{
	পূর্ণাঙ্ক i1, i2;

	যদি (s1 == s2 && !memcmp(l1, l2, s1))
		প্রত্যাগম 1;
	যদি (!(flags & XDF_WHITESPACE_FLAGS))
		প্রত্যাগম 0;

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
	যদি (flags & XDF_IGNORE_WHITESPACE) {
		গচ্ছ skip_ws;
		যাবত্ (i1 < s1 && i2 < s2) {
			যদি (l1[i1++] != l2[i2++])
				প্রত্যাগম 0;
		skip_ws:
			যাবত্ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			যাবত্ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		যাবত্ (i1 < s1 && i2 < s2) {
			যদি (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				যাবত্ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				যাবত্ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				অনুবর্ত;
			}
			যদি (l1[i1++] != l2[i2++])
				প্রত্যাগম 0;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		যাবত্ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		যাবত্ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		প্রত্যাগম (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	যদি (i1 < s1) {
		যাবত্ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		যদি (s1 != i1)
			প্রত্যাগম 0;
	}
	যদি (i2 < s2) {
		যাবত্ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		প্রত্যাগম (s2 == i2);
	}
	প্রত্যাগম 1;
}

unsigned long xdl_hash_record_with_whitespace(বর্ণ ধ্রুব **data,
		বর্ণ ধ্রুব *শীর্ষ, long flags) {
	unsigned long ha = 5381;
	বর্ণ ধ্রুব *ptr = *data;
	পূর্ণাঙ্ক cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	প্রতি (; ptr < শীর্ষ && *ptr != '\n'; ptr++) {
		যদি (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			যদি (*ptr == '\r' &&
			    (ptr + 1 < শীর্ষ && ptr[1] == '\n'))
				অনুবর্ত;
		}
		অন্যথা যদি (XDL_ISSPACE(*ptr)) {
			ধ্রুব বর্ণ *ptr2 = ptr;
			পূর্ণাঙ্ক at_eol;
			যাবত্ (ptr + 1 < শীর্ষ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (শীর্ষ <= ptr + 1 || ptr[1] == '\n');
			যদি (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				যাবত্ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			অনুবর্ত;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < শীর্ষ ? ptr + 1: ptr;

	প্রত্যাগম ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#অন্যথা
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(বর্ণ ধ্রুব **data, বর্ণ ধ্রুব *শীর্ষ) {
	unsigned long ha = 5381, c0, c1;
	বর্ণ ধ্রুব *ptr = *data;
#যদি 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	প্রতি (; ptr < শীর্ষ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < শীর্ষ ? ptr + 1: ptr;
#অন্যথা
	/* Process two characters per iteration. */
	যদি (শীর্ষ - ptr >= 2) কুরু {
		যদি ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			প্রত্যাগম ha;
		}
		যদি ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			প্রত্যাগম ha;
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
	} যাবত্ (ptr < শীর্ষ - 1);
	*data = শীর্ষ;
	যদি (ptr < শীর্ষ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	প্রত্যাগম ha;
}

unsigned পূর্ণাঙ্ক xdl_hashbits(unsigned পূর্ণাঙ্ক size) {
	unsigned পূর্ণাঙ্ক val = 1, bits = 0;

	প্রতি (; val < size && bits < CHAR_BIT * পরিমাণ(unsigned পূর্ণাঙ্ক); val <<= 1, bits++);
	প্রত্যাগম bits ? bits: 1;
}


পূর্ণাঙ্ক xdl_অঙ্ক_out(বর্ণ *out, long val) {
	বর্ণ *ptr, *str = out;
	বর্ণ buf[32];

	ptr = buf + পরিমাণ(buf) - 1;
	*ptr = '\0';
	যদি (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	প্রতি (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	যদি (*ptr)
		প্রতি (; *ptr; ptr++, str++)
			*str = *ptr;
	অন্যথা
		*str++ = '0';
	*str = '\0';

	প্রত্যাগম str - out;
}

স্থির পূর্ণাঙ্ক xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ধ্রুব বর্ণ *func, long funclen,
			       xdemitcb_t *ecb) {
	পূর্ণাঙ্ক nb = 0;
	mmbuffer_t mb;
	বর্ণ buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_অঙ্ক_out(buf + nb, c1 ? s1: s1 - 1);

	যদি (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_অঙ্ক_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_অঙ্ক_out(buf + nb, c2 ? s2: s2 - 1);

	যদি (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_অঙ্ক_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	যদি (func && funclen) {
		buf[nb++] = ' ';
		যদি ((size_t)funclen > পরিমাণ(buf) - nb - 1)
			funclen = পরিমাণ(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	যদি (ecb->out_সারি(ecb->priv, &mb, 1) < 0)
		প্রত্যাগম -1;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ধ্রুব বর্ণ *func, long funclen,
		      xdemitcb_t *ecb) {
	যদি (!ecb->out_hunk)
		প্রত্যাগম xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	যদি (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		প্রত্যাগম -1;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ধ্রুব *xpp,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
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

	subfile1.ptr = (বর্ণ *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (বর্ণ *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	যদি (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		প্রত্যাগম -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	প্রত্যাগম 0;
}

শূন্য* xdl_alloc_grow_helper(শূন্য *p, long nr, long *alloc, size_t size)
{
	শূন্য *tmp = রিক্ত;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	যদি ((size_t)nr > n)
		n = nr;
	যদি (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	যদি (tmp) {
		*alloc = (long)n;
	} অন্যথা {
		xdl_free(p);
		*alloc = 0;
	}
	প্রত্যাগম tmp;
}
