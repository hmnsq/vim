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


اوږد xdl_bogosqrt(اوږد n) {
	اوږد i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	لكل (i = 1; n > 0; n >>= 2)
		i <<= 1;

	أرجع i;
}


صحيح xdl_emit_diffrec(حرف ثابت *rec, اوږد اندازه, حرف ثابت *pre, اوږد psize,
		     xdemitcb_t *ecb) {
	صحيح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (حرف *) pre;
	mb[0].اندازه = psize;
	mb[1].ptr = (حرف *) rec;
	mb[1].اندازه = اندازه;
	إذا (اندازه > 0 && rec[اندازه - 1] != '\n') {
		mb[2].ptr = (حرف *) "\n\\ No newline at end of file\n";
		mb[2].اندازه = (اوږد)مزي_اوږدوالی(mb[2].ptr);
		i++;
	}
	إذا (ecb->بهر_کرښه(ecb->priv, mb, i) < 0) {

		أرجع -1;
	}

	أرجع 0;
}

فراغ *xdl_mmfile_لومړی(mmfile_t *mmf, اوږد *اندازه)
{
	*اندازه = mmf->اندازه;
	أرجع mmf->ptr;
}


اوږد xdl_mmfile_اندازه(mmfile_t *mmf)
{
	أرجع mmf->اندازه;
}


صحيح xdl_cha_init(chastore_t *cha, اوږد isize, اوږد icount) {

	cha->سر = cha->لکۍ = عدم;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = عدم;
	cha->scurr = 0;

	أرجع 0;
}


فراغ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	لكل (cur = cha->سر; (tmp = cur) != عدم;) {
		cur = cur->بل;
		xdl_free(tmp);
	}
}


فراغ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	فراغ *معلومات;

	إذا (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		إذا (!(ancur = (chanode_t *) xdl_malloc(حجم(chanode_t) + cha->nsize))) {

			أرجع عدم;
		}
		ancur->icurr = 0;
		ancur->بل = عدم;
		إذا (cha->لکۍ)
			cha->لکۍ->بل = ancur;
		إذا (!cha->سر)
			cha->سر = ancur;
		cha->لکۍ = ancur;
		cha->ancur = ancur;
	}

	معلومات = (حرف *) ancur + حجم(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	أرجع معلومات;
}

اوږد xdl_guess_کرښې(mmfile_t *mf, اوږد sample) {
	اوږد nl = 0, اندازه, tsize = 0;
	حرف ثابت *معلومات, *cur, *سر_2;

	إذا ((cur = معلومات = xdl_mmfile_لومړی(mf, &اندازه))) {
		لكل (سر_2 = معلومات + اندازه; nl < sample && cur < سر_2; ) {
			nl++;
			إذا (!(cur = memchr(cur, '\n', سر_2 - cur)))
				cur = سر_2;
			وإلا
				cur++;
		}
		tsize += (اوږد) (cur - معلومات);
	}

	إذا (nl && tsize)
		nl = xdl_mmfile_اندازه(mf) / (tsize / nl);

	أرجع nl + 1;
}

صحيح xdl_blankline(ثابت حرف *کرښه, اوږد اندازه, اوږد flags)
{
	اوږد i;

	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		أرجع (اندازه <= 1);

	لكل (i = 0; i < اندازه && XDL_ISSPACE(کرښه[i]); i++)
		;

	أرجع (i == اندازه);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ساكن صحيح پایونه_with_optional_cr(ثابت حرف *l, اوږد s, اوږد i)
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

صحيح xdl_recmatch(ثابت حرف *l1, اوږد s1, ثابت حرف *l2, اوږد s2, اوږد flags)
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
		أرجع (پایونه_with_optional_cr(l1, s1, i1) &&
			پایونه_with_optional_cr(l2, s2, i2));
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

بینښه اوږد xdl_hash_ریکارډ_with_whitespace(حرف ثابت **معلومات,
		حرف ثابت *سر_2, اوږد flags) {
	بینښه اوږد ha = 5381;
	حرف ثابت *ptr = *معلومات;
	صحيح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	لكل (; ptr < سر_2 && *ptr != '\n'; ptr++) {
		إذا (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			إذا (*ptr == '\r' &&
			    (ptr + 1 < سر_2 && ptr[1] == '\n'))
				استمر;
		}
		وإلا إذا (XDL_ISSPACE(*ptr)) {
			ثابت حرف *ptr2 = ptr;
			صحيح at_eol;
			طالما (ptr + 1 < سر_2 && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (سر_2 <= ptr + 1 || ptr[1] == '\n');
			إذا (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (بینښه اوږد) ' ';
			}
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				طالما (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (بینښه اوږد) *ptr2;
					ptr2++;
				}
			}
			استمر;
		}
		ha += (ha << 5);
		ha ^= (بینښه اوږد) *ptr;
	}
	*معلومات = ptr < سر_2 ? ptr + 1: ptr;

	أرجع ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#که_تعریف_شوی __GNUC__
#تعریف_کړه REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#وإلا
#تعریف_کړه REASSOC_FENCE(x, y)
#د_که_پای

بینښه اوږد xdl_hash_ریکارډ_verbatim(حرف ثابت **معلومات, حرف ثابت *سر_2) {
	بینښه اوږد ha = 5381, c0, c1;
	حرف ثابت *ptr = *معلومات;
#إذا 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	لكل (; ptr < سر_2 && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (بینښه اوږد) *ptr;
	}
	*معلومات = ptr < سر_2 ? ptr + 1: ptr;
#وإلا
	/* Process two characters per iteration. */
	إذا (سر_2 - ptr >= 2) افعل {
		إذا ((c0 = ptr[0]) == '\n') {
			*معلومات = ptr + 1;
			أرجع ha;
		}
		إذا ((c1 = ptr[1]) == '\n') {
			*معلومات = ptr + 2;
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
	} طالما (ptr < سر_2 - 1);
	*معلومات = سر_2;
	إذا (ptr < سر_2 && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#د_که_پای
	أرجع ha;
}

بینښه صحيح xdl_hashbits(بینښه صحيح اندازه) {
	بینښه صحيح val = 1, بټونه = 0;

	لكل (; val < اندازه && بټونه < CHAR_BIT * حجم(بینښه صحيح); val <<= 1, بټونه++);
	أرجع بټونه ? بټونه: 1;
}


صحيح xdl_عدد_بهر(حرف *بهر, اوږد val) {
	حرف *ptr, *str = بهر;
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

	أرجع str - بهر;
}

ساكن صحيح xdl_format_hunk_hdr(اوږد s1, اوږد c1, اوږد s2, اوږد c2,
			       ثابت حرف *func, اوږد funclen,
			       xdemitcb_t *ecb) {
	صحيح nb = 0;
	mmbuffer_t mb;
	حرف buf[128];

	حافظه_نقل(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_عدد_بهر(buf + nb, c1 ? s1: s1 - 1);

	إذا (c1 != 1) {
		حافظه_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_عدد_بهر(buf + nb, c1);
	}

	حافظه_نقل(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_عدد_بهر(buf + nb, c2 ? s2: s2 - 1);

	إذا (c2 != 1) {
		حافظه_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_عدد_بهر(buf + nb, c2);
	}

	حافظه_نقل(buf + nb, " @@", 3);
	nb += 3;
	إذا (func && funclen) {
		buf[nb++] = ' ';
		إذا ((size_t)funclen > حجم(buf) - nb - 1)
			funclen = حجم(buf) - nb - 1;
		حافظه_نقل(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.اندازه = nb;
	إذا (ecb->بهر_کرښه(ecb->priv, &mb, 1) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_emit_hunk_hdr(اوږد s1, اوږد c1, اوږد s2, اوږد c2,
		      ثابت حرف *func, اوږد funclen,
		      xdemitcb_t *ecb) {
	إذا (!ecb->بهر_hunk)
		أرجع xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	إذا (ecb->بهر_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_fall_شا_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
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

	حافظه_نقل(diff_env->xdf1.بدل_شوی + line1 - 1, env.xdf1.بدل_شوی, count1);
	حافظه_نقل(diff_env->xdf2.بدل_شوی + line2 - 1, env.xdf2.بدل_شوی, count2);

	xdl_free_env(&env);

	أرجع 0;
}

فراغ* xdl_alloc_grow_helper(فراغ *p, اوږد nr, اوږد *alloc, size_t اندازه)
{
	فراغ *tmp = عدم;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	إذا ((size_t)nr > n)
		n = nr;
	إذا (اندازه_MAX / اندازه >= n)
		tmp = xdl_realloc(p, n * اندازه);
	إذا (tmp) {
		*alloc = (اوږد)n;
	} وإلا {
		xdl_free(p);
		*alloc = 0;
	}
	أرجع tmp;
}
