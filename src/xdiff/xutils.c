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

#شامل "xinclude.h"


طویل xdl_bogosqrt(طویل n) {
	طویل i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	برائے (i = 1; n > 0; n >>= 2)
		i <<= 1;

	واپس i;
}


صحیح xdl_emit_diffrec(حرف ثابت *rec, طویل سائز, حرف ثابت *pre, طویل psize,
		     xdemitcb_t *ecb) {
	صحیح i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (حرف *) pre;
	mb[0].سائز = psize;
	mb[1].ptr = (حرف *) rec;
	mb[1].سائز = سائز;
	اگر (سائز > 0 && rec[سائز - 1] != '\n') {
		mb[2].ptr = (حرف *) "\n\\ No newline at end of file\n";
		mb[2].سائز = (طویل)لڑی_لمبائی(mb[2].ptr);
		i++;
	}
	اگر (ecb->برآمد_سطر(ecb->priv, mb, i) < 0) {

		واپس -1;
	}

	واپس 0;
}

خالی *xdl_mmfile_پہلا(mmfile_t *mmf, طویل *سائز)
{
	*سائز = mmf->سائز;
	واپس mmf->ptr;
}


طویل xdl_mmfile_سائز(mmfile_t *mmf)
{
	واپس mmf->سائز;
}


صحیح xdl_cha_init(chastore_t *cha, طویل isize, طویل icount) {

	cha->سر = cha->دم = تہی;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = تہی;
	cha->scurr = 0;

	واپس 0;
}


خالی xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	برائے (cur = cha->سر; (tmp = cur) != تہی;) {
		cur = cur->اگلا;
		xdl_free(tmp);
	}
}


خالی *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	خالی *ڈیٹا;

	اگر (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		اگر (!(ancur = (chanode_t *) xdl_malloc(حجم(chanode_t) + cha->nsize))) {

			واپس تہی;
		}
		ancur->icurr = 0;
		ancur->اگلا = تہی;
		اگر (cha->دم)
			cha->دم->اگلا = ancur;
		اگر (!cha->سر)
			cha->سر = ancur;
		cha->دم = ancur;
		cha->ancur = ancur;
	}

	ڈیٹا = (حرف *) ancur + حجم(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	واپس ڈیٹا;
}

طویل xdl_guess_سطریں(mmfile_t *mf, طویل sample) {
	طویل nl = 0, سائز, tsize = 0;
	حرف ثابت *ڈیٹا, *cur, *چوٹی;

	اگر ((cur = ڈیٹا = xdl_mmfile_پہلا(mf, &سائز))) {
		برائے (چوٹی = ڈیٹا + سائز; nl < sample && cur < چوٹی; ) {
			nl++;
			اگر (!(cur = memchr(cur, '\n', چوٹی - cur)))
				cur = چوٹی;
			ورنہ
				cur++;
		}
		tsize += (طویل) (cur - ڈیٹا);
	}

	اگر (nl && tsize)
		nl = xdl_mmfile_سائز(mf) / (tsize / nl);

	واپس nl + 1;
}

صحیح xdl_blankline(ثابت حرف *سطر, طویل سائز, طویل flags)
{
	طویل i;

	اگر (!(flags & XDF_WHITESPACE_FLAGS))
		واپس (سائز <= 1);

	برائے (i = 0; i < سائز && XDL_ISSPACE(سطر[i]); i++)
		;

	واپس (i == سائز);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ساکن صحیح سرے_with_optional_cr(ثابت حرف *l, طویل s, طویل i)
{
	صحیح complete = s && l[s-1] == '\n';

	اگر (complete)
		s--;
	اگر (s == i)
		واپس 1;
	/* do not ignore CR at the end of an incomplete line */
	اگر (complete && s == i + 1 && l[i] == '\r')
		واپس 1;
	واپس 0;
}

صحیح xdl_recmatch(ثابت حرف *l1, طویل s1, ثابت حرف *l2, طویل s2, طویل flags)
{
	صحیح i1, i2;

	اگر (s1 == s2 && !memcmp(l1, l2, s1))
		واپس 1;
	اگر (!(flags & XDF_WHITESPACE_FLAGS))
		واپس 0;

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
	اگر (flags & XDF_IGNORE_WHITESPACE) {
		جاؤ skip_ws;
		جب (i1 < s1 && i2 < s2) {
			اگر (l1[i1++] != l2[i2++])
				واپس 0;
		skip_ws:
			جب (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			جب (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} ورنہ اگر (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		جب (i1 < s1 && i2 < s2) {
			اگر (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				جب (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				جب (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				جاری;
			}
			اگر (l1[i1++] != l2[i2++])
				واپس 0;
		}
	} ورنہ اگر (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		جب (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} ورنہ اگر (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		جب (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		واپس (سرے_with_optional_cr(l1, s1, i1) &&
			سرے_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	اگر (i1 < s1) {
		جب (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		اگر (s1 != i1)
			واپس 0;
	}
	اگر (i2 < s2) {
		جب (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		واپس (s2 == i2);
	}
	واپس 1;
}

غیرعلامتی طویل xdl_hash_ریکارڈ_with_whitespace(حرف ثابت **ڈیٹا,
		حرف ثابت *چوٹی, طویل flags) {
	غیرعلامتی طویل ha = 5381;
	حرف ثابت *ptr = *ڈیٹا;
	صحیح cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	برائے (; ptr < چوٹی && *ptr != '\n'; ptr++) {
		اگر (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			اگر (*ptr == '\r' &&
			    (ptr + 1 < چوٹی && ptr[1] == '\n'))
				جاری;
		}
		ورنہ اگر (XDL_ISSPACE(*ptr)) {
			ثابت حرف *ptr2 = ptr;
			صحیح at_eol;
			جب (ptr + 1 < چوٹی && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (چوٹی <= ptr + 1 || ptr[1] == '\n');
			اگر (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			ورنہ اگر (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (غیرعلامتی طویل) ' ';
			}
			ورنہ اگر (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				جب (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (غیرعلامتی طویل) *ptr2;
					ptr2++;
				}
			}
			جاری;
		}
		ha += (ha << 5);
		ha ^= (غیرعلامتی طویل) *ptr;
	}
	*ڈیٹا = ptr < چوٹی ? ptr + 1: ptr;

	واپس ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#اگر_تعریف_شدہ __GNUC__
#تعریف_کرو REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#ورنہ
#تعریف_کرو REASSOC_FENCE(x, y)
#اگر_ختم

غیرعلامتی طویل xdl_hash_ریکارڈ_verbatim(حرف ثابت **ڈیٹا, حرف ثابت *چوٹی) {
	غیرعلامتی طویل ha = 5381, c0, c1;
	حرف ثابت *ptr = *ڈیٹا;
#اگر 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	برائے (; ptr < چوٹی && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (غیرعلامتی طویل) *ptr;
	}
	*ڈیٹا = ptr < چوٹی ? ptr + 1: ptr;
#ورنہ
	/* Process two characters per iteration. */
	اگر (چوٹی - ptr >= 2) کرو {
		اگر ((c0 = ptr[0]) == '\n') {
			*ڈیٹا = ptr + 1;
			واپس ha;
		}
		اگر ((c1 = ptr[1]) == '\n') {
			*ڈیٹا = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			واپس ha;
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
	} جب (ptr < چوٹی - 1);
	*ڈیٹا = چوٹی;
	اگر (ptr < چوٹی && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#اگر_ختم
	واپس ha;
}

غیرعلامتی صحیح xdl_hashbits(غیرعلامتی صحیح سائز) {
	غیرعلامتی صحیح val = 1, بٹ = 0;

	برائے (; val < سائز && بٹ < CHAR_BIT * حجم(غیرعلامتی صحیح); val <<= 1, بٹ++);
	واپس بٹ ? بٹ: 1;
}


صحیح xdl_عدد_برآمد(حرف *برآمد, طویل val) {
	حرف *ptr, *str = برآمد;
	حرف buf[32];

	ptr = buf + حجم(buf) - 1;
	*ptr = '\0';
	اگر (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	برائے (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	اگر (*ptr)
		برائے (; *ptr; ptr++, str++)
			*str = *ptr;
	ورنہ
		*str++ = '0';
	*str = '\0';

	واپس str - برآمد;
}

ساکن صحیح xdl_format_hunk_hdr(طویل s1, طویل c1, طویل s2, طویل c2,
			       ثابت حرف *func, طویل funclen,
			       xdemitcb_t *ecb) {
	صحیح nb = 0;
	mmbuffer_t mb;
	حرف buf[128];

	حافظہ_نقل(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_عدد_برآمد(buf + nb, c1 ? s1: s1 - 1);

	اگر (c1 != 1) {
		حافظہ_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_عدد_برآمد(buf + nb, c1);
	}

	حافظہ_نقل(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_عدد_برآمد(buf + nb, c2 ? s2: s2 - 1);

	اگر (c2 != 1) {
		حافظہ_نقل(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_عدد_برآمد(buf + nb, c2);
	}

	حافظہ_نقل(buf + nb, " @@", 3);
	nb += 3;
	اگر (func && funclen) {
		buf[nb++] = ' ';
		اگر ((size_t)funclen > حجم(buf) - nb - 1)
			funclen = حجم(buf) - nb - 1;
		حافظہ_نقل(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.سائز = nb;
	اگر (ecb->برآمد_سطر(ecb->priv, &mb, 1) < 0)
		واپس -1;
	واپس 0;
}

صحیح xdl_emit_hunk_hdr(طویل s1, طویل c1, طویل s2, طویل c2,
		      ثابت حرف *func, طویل funclen,
		      xdemitcb_t *ecb) {
	اگر (!ecb->برآمد_hunk)
		واپس xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	اگر (ecb->برآمد_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		واپس -1;
	واپس 0;
}

صحیح xdl_fall_عقب_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
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
	subfile1.سائز = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].سائز - subfile1.ptr;
	subfile2.ptr = (حرف *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.سائز = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].سائز - subfile2.ptr;
	اگر (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		واپس -1;

	حافظہ_نقل(diff_env->xdf1.بدلا + line1 - 1, env.xdf1.بدلا, count1);
	حافظہ_نقل(diff_env->xdf2.بدلا + line2 - 1, env.xdf2.بدلا, count2);

	xdl_free_env(&env);

	واپس 0;
}

خالی* xdl_alloc_grow_helper(خالی *p, طویل nr, طویل *alloc, size_t سائز)
{
	خالی *tmp = تہی;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	اگر ((size_t)nr > n)
		n = nr;
	اگر (سائز_MAX / سائز >= n)
		tmp = xdl_realloc(p, n * سائز);
	اگر (tmp) {
		*alloc = (طویل)n;
	} ورنہ {
		xdl_free(p);
		*alloc = 0;
	}
	واپس tmp;
}
