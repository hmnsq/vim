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
	জন্য (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ফেরত i;
}


পূর্ণসংখ্যা xdl_emit_diffrec(অক্ষর ধ্রুবক *rec, long আকার_2, অক্ষর ধ্রুবক *pre, long psize,
		     xdemitcb_t *ecb) {
	পূর্ণসংখ্যা i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (অক্ষর *) pre;
	mb[0].আকার_2 = psize;
	mb[1].ptr = (অক্ষর *) rec;
	mb[1].আকার_2 = আকার_2;
	যদি (আকার_2 > 0 && rec[আকার_2 - 1] != '\n') {
		mb[2].ptr = (অক্ষর *) "\n\\ No newline at end of file\n";
		mb[2].আকার_2 = (long)strlen(mb[2].ptr);
		i++;
	}
	যদি (ecb->out_সারি(ecb->priv, mb, i) < 0) {

		ফেরত -1;
	}

	ফেরত 0;
}

শূন্য *xdl_mmfile_first(mmfile_t *mmf, long *আকার_2)
{
	*আকার_2 = mmf->আকার_2;
	ফেরত mmf->ptr;
}


long xdl_mmfile_আকার(mmfile_t *mmf)
{
	ফেরত mmf->আকার_2;
}


পূর্ণসংখ্যা xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->শীর্ষ = cha->লেজ = ফাঁকা;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ফাঁকা;
	cha->scurr = 0;

	ফেরত 0;
}


শূন্য xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	জন্য (cur = cha->শীর্ষ; (tmp = cur) != ফাঁকা;) {
		cur = cur->পরবর্তী;
		xdl_free(tmp);
	}
}


শূন্য *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	শূন্য *তথ্য;

	যদি (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		যদি (!(ancur = (chanode_t *) xdl_malloc(আকার(chanode_t) + cha->nsize))) {

			ফেরত ফাঁকা;
		}
		ancur->icurr = 0;
		ancur->পরবর্তী = ফাঁকা;
		যদি (cha->লেজ)
			cha->লেজ->পরবর্তী = ancur;
		যদি (!cha->শীর্ষ)
			cha->শীর্ষ = ancur;
		cha->লেজ = ancur;
		cha->ancur = ancur;
	}

	তথ্য = (অক্ষর *) ancur + আকার(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ফেরত তথ্য;
}

long xdl_guess_সারিগুলি(mmfile_t *mf, long sample) {
	long nl = 0, আকার_2, tsize = 0;
	অক্ষর ধ্রুবক *তথ্য, *cur, *শীর্ষ_2;

	যদি ((cur = তথ্য = xdl_mmfile_first(mf, &আকার_2))) {
		জন্য (শীর্ষ_2 = তথ্য + আকার_2; nl < sample && cur < শীর্ষ_2; ) {
			nl++;
			যদি (!(cur = memchr(cur, '\n', শীর্ষ_2 - cur)))
				cur = শীর্ষ_2;
			অন্যথা
				cur++;
		}
		tsize += (long) (cur - তথ্য);
	}

	যদি (nl && tsize)
		nl = xdl_mmfile_আকার(mf) / (tsize / nl);

	ফেরত nl + 1;
}

পূর্ণসংখ্যা xdl_blankline(ধ্রুবক অক্ষর *সারি, long আকার_2, long flags)
{
	long i;

	যদি (!(flags & XDF_WHITESPACE_FLAGS))
		ফেরত (আকার_2 <= 1);

	জন্য (i = 0; i < আকার_2 && XDL_ISSPACE(সারি[i]); i++)
		;

	ফেরত (i == আকার_2);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
স্থির পূর্ণসংখ্যা ends_with_optional_cr(ধ্রুবক অক্ষর *l, long s, long i)
{
	পূর্ণসংখ্যা complete = s && l[s-1] == '\n';

	যদি (complete)
		s--;
	যদি (s == i)
		ফেরত 1;
	/* do not ignore CR at the end of an incomplete line */
	যদি (complete && s == i + 1 && l[i] == '\r')
		ফেরত 1;
	ফেরত 0;
}

পূর্ণসংখ্যা xdl_recmatch(ধ্রুবক অক্ষর *l1, long s1, ধ্রুবক অক্ষর *l2, long s2, long flags)
{
	পূর্ণসংখ্যা i1, i2;

	যদি (s1 == s2 && !memcmp(l1, l2, s1))
		ফেরত 1;
	যদি (!(flags & XDF_WHITESPACE_FLAGS))
		ফেরত 0;

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
		যাও skip_ws;
		যখন (i1 < s1 && i2 < s2) {
			যদি (l1[i1++] != l2[i2++])
				ফেরত 0;
		skip_ws:
			যখন (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			যখন (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		যখন (i1 < s1 && i2 < s2) {
			যদি (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				যখন (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				যখন (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				চালু;
			}
			যদি (l1[i1++] != l2[i2++])
				ফেরত 0;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		যখন (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} অন্যথা যদি (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		যখন (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ফেরত (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	যদি (i1 < s1) {
		যখন (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		যদি (s1 != i1)
			ফেরত 0;
	}
	যদি (i2 < s2) {
		যখন (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ফেরত (s2 == i2);
	}
	ফেরত 1;
}

unsigned long xdl_hash_record_with_whitespace(অক্ষর ধ্রুবক **তথ্য,
		অক্ষর ধ্রুবক *শীর্ষ_2, long flags) {
	unsigned long ha = 5381;
	অক্ষর ধ্রুবক *ptr = *তথ্য;
	পূর্ণসংখ্যা cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	জন্য (; ptr < শীর্ষ_2 && *ptr != '\n'; ptr++) {
		যদি (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			যদি (*ptr == '\r' &&
			    (ptr + 1 < শীর্ষ_2 && ptr[1] == '\n'))
				চালু;
		}
		অন্যথা যদি (XDL_ISSPACE(*ptr)) {
			ধ্রুবক অক্ষর *ptr2 = ptr;
			পূর্ণসংখ্যা at_eol;
			যখন (ptr + 1 < শীর্ষ_2 && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (শীর্ষ_2 <= ptr + 1 || ptr[1] == '\n');
			যদি (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			অন্যথা যদি (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				যখন (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			চালু;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*তথ্য = ptr < শীর্ষ_2 ? ptr + 1: ptr;

	ফেরত ha;
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

unsigned long xdl_hash_record_verbatim(অক্ষর ধ্রুবক **তথ্য, অক্ষর ধ্রুবক *শীর্ষ_2) {
	unsigned long ha = 5381, c0, c1;
	অক্ষর ধ্রুবক *ptr = *তথ্য;
#যদি 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	জন্য (; ptr < শীর্ষ_2 && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*তথ্য = ptr < শীর্ষ_2 ? ptr + 1: ptr;
#অন্যথা
	/* Process two characters per iteration. */
	যদি (শীর্ষ_2 - ptr >= 2) করো {
		যদি ((c0 = ptr[0]) == '\n') {
			*তথ্য = ptr + 1;
			ফেরত ha;
		}
		যদি ((c1 = ptr[1]) == '\n') {
			*তথ্য = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ফেরত ha;
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
	} যখন (ptr < শীর্ষ_2 - 1);
	*তথ্য = শীর্ষ_2;
	যদি (ptr < শীর্ষ_2 && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	ফেরত ha;
}

unsigned পূর্ণসংখ্যা xdl_hashbits(unsigned পূর্ণসংখ্যা আকার_2) {
	unsigned পূর্ণসংখ্যা val = 1, bits = 0;

	জন্য (; val < আকার_2 && bits < CHAR_BIT * আকার(unsigned পূর্ণসংখ্যা); val <<= 1, bits++);
	ফেরত bits ? bits: 1;
}


পূর্ণসংখ্যা xdl_অঙ্ক_out(অক্ষর *out, long val) {
	অক্ষর *ptr, *str = out;
	অক্ষর buf[32];

	ptr = buf + আকার(buf) - 1;
	*ptr = '\0';
	যদি (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	জন্য (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	যদি (*ptr)
		জন্য (; *ptr; ptr++, str++)
			*str = *ptr;
	অন্যথা
		*str++ = '0';
	*str = '\0';

	ফেরত str - out;
}

স্থির পূর্ণসংখ্যা xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ধ্রুবক অক্ষর *func, long funclen,
			       xdemitcb_t *ecb) {
	পূর্ণসংখ্যা nb = 0;
	mmbuffer_t mb;
	অক্ষর buf[128];

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
		যদি ((size_t)funclen > আকার(buf) - nb - 1)
			funclen = আকার(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.আকার_2 = nb;
	যদি (ecb->out_সারি(ecb->priv, &mb, 1) < 0)
		ফেরত -1;
	ফেরত 0;
}

পূর্ণসংখ্যা xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ধ্রুবক অক্ষর *func, long funclen,
		      xdemitcb_t *ecb) {
	যদি (!ecb->out_hunk)
		ফেরত xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	যদি (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ফেরত -1;
	ফেরত 0;
}

পূর্ণসংখ্যা xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ধ্রুবক *xpp,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
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

	subfile1.ptr = (অক্ষর *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.আকার_2 = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].আকার_2 - subfile1.ptr;
	subfile2.ptr = (অক্ষর *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.আকার_2 = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].আকার_2 - subfile2.ptr;
	যদি (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ফেরত -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	ফেরত 0;
}

শূন্য* xdl_alloc_grow_helper(শূন্য *p, long nr, long *alloc, size_t আকার_2)
{
	শূন্য *tmp = ফাঁকা;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	যদি ((size_t)nr > n)
		n = nr;
	যদি (আকার_MAX / আকার_2 >= n)
		tmp = xdl_realloc(p, n * আকার_2);
	যদি (tmp) {
		*alloc = (long)n;
	} অন্যথা {
		xdl_free(p);
		*alloc = 0;
	}
	ফেরত tmp;
}
