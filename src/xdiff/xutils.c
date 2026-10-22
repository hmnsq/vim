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


دراز xdl_bogosqrt(دراز n) {
	دراز i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	لكل (i = 1; n > 0; n >>= 2)
		i <<= 1;

	أرجع i;
}


صحيح xdl_emit_diffrec(حرف ثابت *rec, دراز اندازه, حرف ثابت *pre, دراز psize,
		     xdemitcb_t *ecb) {
	صحيح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (حرف *) pre;
	mb[0].اندازه = psize;
	mb[1].ptr = (حرف *) rec;
	mb[1].اندازه = اندازه;
	إذا (اندازه > 0 && rec[اندازه - 1] != '\n') {
		mb[2].ptr = (حرف *) "\n\\ No newline at end of file\n";
		mb[2].اندازه = (دراز)طول_رشته(mb[2].ptr);
		i++;
	}
	إذا (ecb->بیرون_سطر(ecb->priv, mb, i) < 0) {

		أرجع -1;
	}

	أرجع 0;
}

فراغ *xdl_mmfile_اولی(mmfile_t *mmf, دراز *اندازه)
{
	*اندازه = mmf->اندازه;
	أرجع mmf->ptr;
}


دراز xdl_mmfile_اندازه(mmfile_t *mmf)
{
	أرجع mmf->اندازه;
}


صحيح xdl_cha_init(chastore_t *cha, دراز isize, دراز icount) {

	cha->سر = cha->دم = عدم;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = عدم;
	cha->scurr = 0;

	أرجع 0;
}


فراغ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	لكل (cur = cha->سر; (tmp = cur) != عدم;) {
		cur = cur->بعدی;
		xdl_free(tmp);
	}
}


فراغ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	فراغ *داده;

	إذا (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		إذا (!(ancur = (chanode_t *) xdl_malloc(حجم(chanode_t) + cha->nsize))) {

			أرجع عدم;
		}
		ancur->icurr = 0;
		ancur->بعدی = عدم;
		إذا (cha->دم)
			cha->دم->بعدی = ancur;
		إذا (!cha->سر)
			cha->سر = ancur;
		cha->دم = ancur;
		cha->ancur = ancur;
	}

	داده = (حرف *) ancur + حجم(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	أرجع داده;
}

دراز xdl_guess_سطرها(mmfile_t *mf, دراز sample) {
	دراز nl = 0, اندازه, tsize = 0;
	حرف ثابت *داده, *cur, *بالاترین;

	إذا ((cur = داده = xdl_mmfile_اولی(mf, &اندازه))) {
		لكل (بالاترین = داده + اندازه; nl < sample && cur < بالاترین; ) {
			nl++;
			إذا (!(cur = memchr(cur, '\n', بالاترین - cur)))
				cur = بالاترین;
			وإلا
				cur++;
		}
		tsize += (دراز) (cur - داده);
	}

	إذا (nl && tsize)
		nl = xdl_mmfile_اندازه(mf) / (tsize / nl);

	أرجع nl + 1;
}

صحيح xdl_blankline(ثابت حرف *سطر, دراز اندازه, دراز flags)
{
	دراز i;

	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		أرجع (اندازه <= 1);

	لكل (i = 0; i < اندازه && XDL_ISSPACE(سطر[i]); i++)
		;

	أرجع (i == اندازه);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ساكن صحيح دوسر_with_optional_cr(ثابت حرف *l, دراز s, دراز i)
{
	صحيح complete = s && l[s-1] == '\n';

	إذا (complete)
		s--;
	إذا (s == i)
		أرجع 1;
	/* do not ignore CR at the end of an incomplete line */
	إذا (complete && s == i + 1 && l[i] == '\r')
		أرجع 1;
	أرجع 0;
}

صحيح xdl_recmatch(ثابت حرف *l1, دراز s1, ثابت حرف *l2, دراز s2, دراز flags)
{
	صحيح i1, i2;

	إذا (s1 == s2 && !memcmp(l1, l2, s1))
		أرجع 1;
	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		أرجع 0;

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
		طالما (i1 < s1 && i2 < s2) {
			إذا (l1[i1++] != l2[i2++])
				أرجع 0;
		skip_ws:
			طالما (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			طالما (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		طالما (i1 < s1 && i2 < s2) {
			إذا (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				طالما (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				طالما (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				استمر;
			}
			إذا (l1[i1++] != l2[i2++])
				أرجع 0;
		}
	} وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		طالما (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} وإلا إذا (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		طالما (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		أرجع (دوسر_with_optional_cr(l1, s1, i1) &&
			دوسر_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	إذا (i1 < s1) {
		طالما (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		إذا (s1 != i1)
			أرجع 0;
	}
	إذا (i2 < s2) {
		طالما (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		أرجع (s2 == i2);
	}
	أرجع 1;
}

بیعلامت دراز xdl_hash_رکورد_with_whitespace(حرف ثابت **داده,
		حرف ثابت *بالاترین, دراز flags) {
	بیعلامت دراز ha = 5381;
	حرف ثابت *ptr = *داده;
	صحيح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	لكل (; ptr < بالاترین && *ptr != '\n'; ptr++) {
		إذا (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			إذا (*ptr == '\r' &&
			    (ptr + 1 < بالاترین && ptr[1] == '\n'))
				استمر;
		}
		وإلا إذا (XDL_ISSPACE(*ptr)) {
			ثابت حرف *ptr2 = ptr;
			صحيح at_eol;
			طالما (ptr + 1 < بالاترین && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (بالاترین <= ptr + 1 || ptr[1] == '\n');
			إذا (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (بیعلامت دراز) ' ';
			}
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				طالما (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (بیعلامت دراز) *ptr2;
					ptr2++;
				}
			}
			استمر;
		}
		ha += (ha << 5);
		ha ^= (بیعلامت دراز) *ptr;
	}
	*داده = ptr < بالاترین ? ptr + 1: ptr;

	أرجع ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#اگر_تعریف_شده __GNUC__
#تعریف_کن REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#وإلا
#تعریف_کن REASSOC_FENCE(x, y)
#پایان_اگر

بیعلامت دراز xdl_hash_رکورد_verbatim(حرف ثابت **داده, حرف ثابت *بالاترین) {
	بیعلامت دراز ha = 5381, c0, c1;
	حرف ثابت *ptr = *داده;
#إذا 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	لكل (; ptr < بالاترین && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (بیعلامت دراز) *ptr;
	}
	*داده = ptr < بالاترین ? ptr + 1: ptr;
#وإلا
	/* Process two characters per iteration. */
	إذا (بالاترین - ptr >= 2) افعل {
		إذا ((c0 = ptr[0]) == '\n') {
			*داده = ptr + 1;
			أرجع ha;
		}
		إذا ((c1 = ptr[1]) == '\n') {
			*داده = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			أرجع ha;
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
	} طالما (ptr < بالاترین - 1);
	*داده = بالاترین;
	إذا (ptr < بالاترین && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#پایان_اگر
	أرجع ha;
}

بیعلامت صحيح xdl_hashbits(بیعلامت صحيح اندازه) {
	بیعلامت صحيح val = 1, بیتها = 0;

	لكل (; val < اندازه && بیتها < CHAR_BIT * حجم(بیعلامت صحيح); val <<= 1, بیتها++);
	أرجع بیتها ? بیتها: 1;
}


صحيح xdl_رقم_بیرون(حرف *بیرون, دراز val) {
	حرف *ptr, *str = بیرون;
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

	أرجع str - بیرون;
}

ساكن صحيح xdl_format_hunk_hdr(دراز s1, دراز c1, دراز s2, دراز c2,
			       ثابت حرف *func, دراز funclen,
			       xdemitcb_t *ecb) {
	صحيح nb = 0;
	mmbuffer_t mb;
	حرف buf[128];

	کپی_حافظه(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_رقم_بیرون(buf + nb, c1 ? s1: s1 - 1);

	إذا (c1 != 1) {
		کپی_حافظه(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_بیرون(buf + nb, c1);
	}

	کپی_حافظه(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_رقم_بیرون(buf + nb, c2 ? s2: s2 - 1);

	إذا (c2 != 1) {
		کپی_حافظه(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_رقم_بیرون(buf + nb, c2);
	}

	کپی_حافظه(buf + nb, " @@", 3);
	nb += 3;
	إذا (func && funclen) {
		buf[nb++] = ' ';
		إذا ((size_t)funclen > حجم(buf) - nb - 1)
			funclen = حجم(buf) - nb - 1;
		کپی_حافظه(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.اندازه = nb;
	إذا (ecb->بیرون_سطر(ecb->priv, &mb, 1) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_emit_hunk_hdr(دراز s1, دراز c1, دراز s2, دراز c2,
		      ثابت حرف *func, دراز funclen,
		      xdemitcb_t *ecb) {
	إذا (!ecb->بیرون_hunk)
		أرجع xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	إذا (ecb->بیرون_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_fall_پشت_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
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
	subfile1.اندازه = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].اندازه - subfile1.ptr;
	subfile2.ptr = (حرف *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.اندازه = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].اندازه - subfile2.ptr;
	إذا (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		أرجع -1;

	کپی_حافظه(diff_env->xdf1.تغییریافته + line1 - 1, env.xdf1.تغییریافته, count1);
	کپی_حافظه(diff_env->xdf2.تغییریافته + line2 - 1, env.xdf2.تغییریافته, count2);

	xdl_free_env(&env);

	أرجع 0;
}

فراغ* xdl_alloc_grow_helper(فراغ *p, دراز nr, دراز *alloc, size_t اندازه)
{
	فراغ *tmp = عدم;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	إذا ((size_t)nr > n)
		n = nr;
	إذا (اندازه_MAX / اندازه >= n)
		tmp = xdl_realloc(p, n * اندازه);
	إذا (tmp) {
		*alloc = (دراز)n;
	} وإلا {
		xdl_free(p);
		*alloc = 0;
	}
	أرجع tmp;
}
