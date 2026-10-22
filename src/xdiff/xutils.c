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


ڊگهو xdl_bogosqrt(ڊگهو n) {
	ڊگهو i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	لكل (i = 1; n > 0; n >>= 2)
		i <<= 1;

	أرجع i;
}


صحيح xdl_emit_diffrec(حرف ثابت *rec, ڊگهو سائيز, حرف ثابت *pre, ڊگهو psize,
		     xdemitcb_t *ecb) {
	صحيح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (حرف *) pre;
	mb[0].سائيز = psize;
	mb[1].ptr = (حرف *) rec;
	mb[1].سائيز = سائيز;
	إذا (سائيز > 0 && rec[سائيز - 1] != '\n') {
		mb[2].ptr = (حرف *) "\n\\ No newline at end of file\n";
		mb[2].سائيز = (ڊگهو)لڙي_ڊيگهه(mb[2].ptr);
		i++;
	}
	إذا (ecb->ٻاهريون_سٽ(ecb->priv, mb, i) < 0) {

		أرجع -1;
	}

	أرجع 0;
}

فراغ *xdl_mmfile_پهريون(mmfile_t *mmf, ڊگهو *سائيز)
{
	*سائيز = mmf->سائيز;
	أرجع mmf->ptr;
}


ڊگهو xdl_mmfile_سائيز(mmfile_t *mmf)
{
	أرجع mmf->سائيز;
}


صحيح xdl_cha_init(chastore_t *cha, ڊگهو isize, ڊگهو icount) {

	cha->سر = cha->پڇ = عدم;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = عدم;
	cha->scurr = 0;

	أرجع 0;
}


فراغ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	لكل (cur = cha->سر; (tmp = cur) != عدم;) {
		cur = cur->ايندڙ;
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
		ancur->ايندڙ = عدم;
		إذا (cha->پڇ)
			cha->پڇ->ايندڙ = ancur;
		إذا (!cha->سر)
			cha->سر = ancur;
		cha->پڇ = ancur;
		cha->ancur = ancur;
	}

	معلومات = (حرف *) ancur + حجم(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	أرجع معلومات;
}

ڊگهو xdl_guess_سٽون(mmfile_t *mf, ڊگهو sample) {
	ڊگهو nl = 0, سائيز, tsize = 0;
	حرف ثابت *معلومات, *cur, *چوٽي;

	إذا ((cur = معلومات = xdl_mmfile_پهريون(mf, &سائيز))) {
		لكل (چوٽي = معلومات + سائيز; nl < sample && cur < چوٽي; ) {
			nl++;
			إذا (!(cur = memchr(cur, '\n', چوٽي - cur)))
				cur = چوٽي;
			وإلا
				cur++;
		}
		tsize += (ڊگهو) (cur - معلومات);
	}

	إذا (nl && tsize)
		nl = xdl_mmfile_سائيز(mf) / (tsize / nl);

	أرجع nl + 1;
}

صحيح xdl_blankline(ثابت حرف *سٽ, ڊگهو سائيز, ڊگهو flags)
{
	ڊگهو i;

	إذا (!(flags & XDF_WHITESPACE_FLAGS))
		أرجع (سائيز <= 1);

	لكل (i = 0; i < سائيز && XDL_ISSPACE(سٽ[i]); i++)
		;

	أرجع (i == سائيز);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ساكن صحيح پڇاڙيون_with_optional_cr(ثابت حرف *l, ڊگهو s, ڊگهو i)
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

صحيح xdl_recmatch(ثابت حرف *l1, ڊگهو s1, ثابت حرف *l2, ڊگهو s2, ڊگهو flags)
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
		أرجع (پڇاڙيون_with_optional_cr(l1, s1, i1) &&
			پڇاڙيون_with_optional_cr(l2, s2, i2));
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

بنانشان ڊگهو xdl_hash_رڪارڊ_with_whitespace(حرف ثابت **معلومات,
		حرف ثابت *چوٽي, ڊگهو flags) {
	بنانشان ڊگهو ha = 5381;
	حرف ثابت *ptr = *معلومات;
	صحيح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	لكل (; ptr < چوٽي && *ptr != '\n'; ptr++) {
		إذا (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			إذا (*ptr == '\r' &&
			    (ptr + 1 < چوٽي && ptr[1] == '\n'))
				استمر;
		}
		وإلا إذا (XDL_ISSPACE(*ptr)) {
			ثابت حرف *ptr2 = ptr;
			صحيح at_eol;
			طالما (ptr + 1 < چوٽي && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (چوٽي <= ptr + 1 || ptr[1] == '\n');
			إذا (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (بنانشان ڊگهو) ' ';
			}
			وإلا إذا (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				طالما (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (بنانشان ڊگهو) *ptr2;
					ptr2++;
				}
			}
			استمر;
		}
		ha += (ha << 5);
		ha ^= (بنانشان ڊگهو) *ptr;
	}
	*معلومات = ptr < چوٽي ? ptr + 1: ptr;

	أرجع ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#جيڪڏهن_وصف_ٿيل __GNUC__
#وصف_ڪريو REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#وإلا
#وصف_ڪريو REASSOC_FENCE(x, y)
#جيڪڏهن_پورو

بنانشان ڊگهو xdl_hash_رڪارڊ_verbatim(حرف ثابت **معلومات, حرف ثابت *چوٽي) {
	بنانشان ڊگهو ha = 5381, c0, c1;
	حرف ثابت *ptr = *معلومات;
#إذا 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	لكل (; ptr < چوٽي && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (بنانشان ڊگهو) *ptr;
	}
	*معلومات = ptr < چوٽي ? ptr + 1: ptr;
#وإلا
	/* Process two characters per iteration. */
	إذا (چوٽي - ptr >= 2) افعل {
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
	} طالما (ptr < چوٽي - 1);
	*معلومات = چوٽي;
	إذا (ptr < چوٽي && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#جيڪڏهن_پورو
	أرجع ha;
}

بنانشان صحيح xdl_hashbits(بنانشان صحيح سائيز) {
	بنانشان صحيح val = 1, بٽ = 0;

	لكل (; val < سائيز && بٽ < CHAR_BIT * حجم(بنانشان صحيح); val <<= 1, بٽ++);
	أرجع بٽ ? بٽ: 1;
}


صحيح xdl_انگ_ٻاهريون(حرف *ٻاهريون, ڊگهو val) {
	حرف *ptr, *str = ٻاهريون;
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

	أرجع str - ٻاهريون;
}

ساكن صحيح xdl_format_hunk_hdr(ڊگهو s1, ڊگهو c1, ڊگهو s2, ڊگهو c2,
			       ثابت حرف *func, ڊگهو funclen,
			       xdemitcb_t *ecb) {
	صحيح nb = 0;
	mmbuffer_t mb;
	حرف buf[128];

	يادگيري_نقل(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_انگ_ٻاهريون(buf + nb, c1 ? s1: s1 - 1);

	إذا (c1 != 1) {
		يادگيري_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_انگ_ٻاهريون(buf + nb, c1);
	}

	يادگيري_نقل(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_انگ_ٻاهريون(buf + nb, c2 ? s2: s2 - 1);

	إذا (c2 != 1) {
		يادگيري_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_انگ_ٻاهريون(buf + nb, c2);
	}

	يادگيري_نقل(buf + nb, " @@", 3);
	nb += 3;
	إذا (func && funclen) {
		buf[nb++] = ' ';
		إذا ((size_t)funclen > حجم(buf) - nb - 1)
			funclen = حجم(buf) - nb - 1;
		يادگيري_نقل(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.سائيز = nb;
	إذا (ecb->ٻاهريون_سٽ(ecb->priv, &mb, 1) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_emit_hunk_hdr(ڊگهو s1, ڊگهو c1, ڊگهو s2, ڊگهو c2,
		      ثابت حرف *func, ڊگهو funclen,
		      xdemitcb_t *ecb) {
	إذا (!ecb->ٻاهريون_hunk)
		أرجع xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	إذا (ecb->ٻاهريون_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		أرجع -1;
	أرجع 0;
}

صحيح xdl_fall_پٺ_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
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
	subfile1.سائيز = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].سائيز - subfile1.ptr;
	subfile2.ptr = (حرف *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.سائيز = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].سائيز - subfile2.ptr;
	إذا (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		أرجع -1;

	يادگيري_نقل(diff_env->xdf1.بدليو + line1 - 1, env.xdf1.بدليو, count1);
	يادگيري_نقل(diff_env->xdf2.بدليو + line2 - 1, env.xdf2.بدليو, count2);

	xdl_free_env(&env);

	أرجع 0;
}

فراغ* xdl_alloc_grow_helper(فراغ *p, ڊگهو nr, ڊگهو *alloc, size_t سائيز)
{
	فراغ *tmp = عدم;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	إذا ((size_t)nr > n)
		n = nr;
	إذا (سائيز_MAX / سائيز >= n)
		tmp = xdl_realloc(p, n * سائيز);
	إذا (tmp) {
		*alloc = (ڊگهو)n;
	} وإلا {
		xdl_free(p);
		*alloc = 0;
	}
	أرجع tmp;
}
