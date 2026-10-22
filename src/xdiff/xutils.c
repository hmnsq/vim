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

#అన్తర్భావ "xinclude.h"


దీర్ఘ xdl_bogosqrt(దీర్ఘ n) {
	దీర్ఘ i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	కొరకు (i = 1; n > 0; n >>= 2)
		i <<= 1;

	తిరిగి i;
}


పూర్ణాంకం xdl_emit_diffrec(అక్షరం స్థిరరాశి *rec, దీర్ఘ కొలత, అక్షరం స్థిరరాశి *pre, దీర్ఘ psize,
		     xdemitcb_t *ecb) {
	పూర్ణాంకం i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (అక్షరం *) pre;
	mb[0].కొలత = psize;
	mb[1].ptr = (అక్షరం *) rec;
	mb[1].కొలత = కొలత;
	ఒకవేళ (కొలత > 0 && rec[కొలత - 1] != '\n') {
		mb[2].ptr = (అక్షరం *) "\n\\ No newline at end of file\n";
		mb[2].కొలత = (దీర్ఘ)అక్షరశ్రేణి_పొడవు(mb[2].ptr);
		i++;
	}
	ఒకవేళ (ecb->బహిర్గతం_పంక్తి(ecb->priv, mb, i) < 0) {

		తిరిగి -1;
	}

	తిరిగి 0;
}

శూన్య *xdl_mmfile_మొదటి(mmfile_t *mmf, దీర్ఘ *కొలత)
{
	*కొలత = mmf->కొలత;
	తిరిగి mmf->ptr;
}


దీర్ఘ xdl_mmfile_కొలత(mmfile_t *mmf)
{
	తిరిగి mmf->కొలత;
}


పూర్ణాంకం xdl_cha_init(chastore_t *cha, దీర్ఘ isize, దీర్ఘ icount) {

	cha->తల = cha->తోక = ఖాళీ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ఖాళీ;
	cha->scurr = 0;

	తిరిగి 0;
}


శూన్య xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	కొరకు (cur = cha->తల; (tmp = cur) != ఖాళీ;) {
		cur = cur->తదుపరి;
		xdl_free(tmp);
	}
}


శూన్య *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	శూన్య *దత్తాంశం;

	ఒకవేళ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		ఒకవేళ (!(ancur = (chanode_t *) xdl_malloc(పరిమాణం(chanode_t) + cha->nsize))) {

			తిరిగి ఖాళీ;
		}
		ancur->icurr = 0;
		ancur->తదుపరి = ఖాళీ;
		ఒకవేళ (cha->తోక)
			cha->తోక->తదుపరి = ancur;
		ఒకవేళ (!cha->తల)
			cha->తల = ancur;
		cha->తోక = ancur;
		cha->ancur = ancur;
	}

	దత్తాంశం = (అక్షరం *) ancur + పరిమాణం(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	తిరిగి దత్తాంశం;
}

దీర్ఘ xdl_guess_పంక్తులు(mmfile_t *mf, దీర్ఘ sample) {
	దీర్ఘ nl = 0, కొలత, tsize = 0;
	అక్షరం స్థిరరాశి *దత్తాంశం, *cur, *శిఖరం;

	ఒకవేళ ((cur = దత్తాంశం = xdl_mmfile_మొదటి(mf, &కొలత))) {
		కొరకు (శిఖరం = దత్తాంశం + కొలత; nl < sample && cur < శిఖరం; ) {
			nl++;
			ఒకవేళ (!(cur = memchr(cur, '\n', శిఖరం - cur)))
				cur = శిఖరం;
			లేకపోతే
				cur++;
		}
		tsize += (దీర్ఘ) (cur - దత్తాంశం);
	}

	ఒకవేళ (nl && tsize)
		nl = xdl_mmfile_కొలత(mf) / (tsize / nl);

	తిరిగి nl + 1;
}

పూర్ణాంకం xdl_blankline(స్థిరరాశి అక్షరం *పంక్తి, దీర్ఘ కొలత, దీర్ఘ flags)
{
	దీర్ఘ i;

	ఒకవేళ (!(flags & XDF_WHITESPACE_FLAGS))
		తిరిగి (కొలత <= 1);

	కొరకు (i = 0; i < కొలత && XDL_ISSPACE(పంక్తి[i]); i++)
		;

	తిరిగి (i == కొలత);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
స్థిర పూర్ణాంకం చివర్లు_with_optional_cr(స్థిరరాశి అక్షరం *l, దీర్ఘ s, దీర్ఘ i)
{
	పూర్ణాంకం complete = s && l[s-1] == '\n';

	ఒకవేళ (complete)
		s--;
	ఒకవేళ (s == i)
		తిరిగి 1;
	/* do not ignore CR at the end of an incomplete line */
	ఒకవేళ (complete && s == i + 1 && l[i] == '\r')
		తిరిగి 1;
	తిరిగి 0;
}

పూర్ణాంకం xdl_recmatch(స్థిరరాశి అక్షరం *l1, దీర్ఘ s1, స్థిరరాశి అక్షరం *l2, దీర్ఘ s2, దీర్ఘ flags)
{
	పూర్ణాంకం i1, i2;

	ఒకవేళ (s1 == s2 && !memcmp(l1, l2, s1))
		తిరిగి 1;
	ఒకవేళ (!(flags & XDF_WHITESPACE_FLAGS))
		తిరిగి 0;

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
	ఒకవేళ (flags & XDF_IGNORE_WHITESPACE) {
		వెళ్ళు skip_ws;
		అయితే (i1 < s1 && i2 < s2) {
			ఒకవేళ (l1[i1++] != l2[i2++])
				తిరిగి 0;
		skip_ws:
			అయితే (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			అయితే (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} లేకపోతే ఒకవేళ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		అయితే (i1 < s1 && i2 < s2) {
			ఒకవేళ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				అయితే (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				అయితే (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				కొనసాగు;
			}
			ఒకవేళ (l1[i1++] != l2[i2++])
				తిరిగి 0;
		}
	} లేకపోతే ఒకవేళ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		అయితే (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} లేకపోతే ఒకవేళ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		అయితే (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		తిరిగి (చివర్లు_with_optional_cr(l1, s1, i1) &&
			చివర్లు_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	ఒకవేళ (i1 < s1) {
		అయితే (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		ఒకవేళ (s1 != i1)
			తిరిగి 0;
	}
	ఒకవేళ (i2 < s2) {
		అయితే (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		తిరిగి (s2 == i2);
	}
	తిరిగి 1;
}

గుర్తులేని దీర్ఘ xdl_hash_రికార్డు_with_whitespace(అక్షరం స్థిరరాశి **దత్తాంశం,
		అక్షరం స్థిరరాశి *శిఖరం, దీర్ఘ flags) {
	గుర్తులేని దీర్ఘ ha = 5381;
	అక్షరం స్థిరరాశి *ptr = *దత్తాంశం;
	పూర్ణాంకం cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	కొరకు (; ptr < శిఖరం && *ptr != '\n'; ptr++) {
		ఒకవేళ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			ఒకవేళ (*ptr == '\r' &&
			    (ptr + 1 < శిఖరం && ptr[1] == '\n'))
				కొనసాగు;
		}
		లేకపోతే ఒకవేళ (XDL_ISSPACE(*ptr)) {
			స్థిరరాశి అక్షరం *ptr2 = ptr;
			పూర్ణాంకం at_eol;
			అయితే (ptr + 1 < శిఖరం && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (శిఖరం <= ptr + 1 || ptr[1] == '\n');
			ఒకవేళ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			లేకపోతే ఒకవేళ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (గుర్తులేని దీర్ఘ) ' ';
			}
			లేకపోతే ఒకవేళ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				అయితే (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (గుర్తులేని దీర్ఘ) *ptr2;
					ptr2++;
				}
			}
			కొనసాగు;
		}
		ha += (ha << 5);
		ha ^= (గుర్తులేని దీర్ఘ) *ptr;
	}
	*దత్తాంశం = ptr < శిఖరం ? ptr + 1: ptr;

	తిరిగి ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#నిర్వచితమైతే __GNUC__
#నిర్వచించు REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#లేకపోతే
#నిర్వచించు REASSOC_FENCE(x, y)
#అయితే_ముగింపు

గుర్తులేని దీర్ఘ xdl_hash_రికార్డు_verbatim(అక్షరం స్థిరరాశి **దత్తాంశం, అక్షరం స్థిరరాశి *శిఖరం) {
	గుర్తులేని దీర్ఘ ha = 5381, c0, c1;
	అక్షరం స్థిరరాశి *ptr = *దత్తాంశం;
#ఒకవేళ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	కొరకు (; ptr < శిఖరం && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (గుర్తులేని దీర్ఘ) *ptr;
	}
	*దత్తాంశం = ptr < శిఖరం ? ptr + 1: ptr;
#లేకపోతే
	/* Process two characters per iteration. */
	ఒకవేళ (శిఖరం - ptr >= 2) చేయి {
		ఒకవేళ ((c0 = ptr[0]) == '\n') {
			*దత్తాంశం = ptr + 1;
			తిరిగి ha;
		}
		ఒకవేళ ((c1 = ptr[1]) == '\n') {
			*దత్తాంశం = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			తిరిగి ha;
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
	} అయితే (ptr < శిఖరం - 1);
	*దత్తాంశం = శిఖరం;
	ఒకవేళ (ptr < శిఖరం && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#అయితే_ముగింపు
	తిరిగి ha;
}

గుర్తులేని పూర్ణాంకం xdl_hashbits(గుర్తులేని పూర్ణాంకం కొలత) {
	గుర్తులేని పూర్ణాంకం val = 1, బిట్లు = 0;

	కొరకు (; val < కొలత && బిట్లు < CHAR_BIT * పరిమాణం(గుర్తులేని పూర్ణాంకం); val <<= 1, బిట్లు++);
	తిరిగి బిట్లు ? బిట్లు: 1;
}


పూర్ణాంకం xdl_అంకె_బహిర్గతం(అక్షరం *బహిర్గతం, దీర్ఘ val) {
	అక్షరం *ptr, *str = బహిర్గతం;
	అక్షరం buf[32];

	ptr = buf + పరిమాణం(buf) - 1;
	*ptr = '\0';
	ఒకవేళ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	కొరకు (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	ఒకవేళ (*ptr)
		కొరకు (; *ptr; ptr++, str++)
			*str = *ptr;
	లేకపోతే
		*str++ = '0';
	*str = '\0';

	తిరిగి str - బహిర్గతం;
}

స్థిర పూర్ణాంకం xdl_format_hunk_hdr(దీర్ఘ s1, దీర్ఘ c1, దీర్ఘ s2, దీర్ఘ c2,
			       స్థిరరాశి అక్షరం *func, దీర్ఘ funclen,
			       xdemitcb_t *ecb) {
	పూర్ణాంకం nb = 0;
	mmbuffer_t mb;
	అక్షరం buf[128];

	జ్ఞాపకం_నకలు(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_అంకె_బహిర్గతం(buf + nb, c1 ? s1: s1 - 1);

	ఒకవేళ (c1 != 1) {
		జ్ఞాపకం_నకలు(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_అంకె_బహిర్గతం(buf + nb, c1);
	}

	జ్ఞాపకం_నకలు(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_అంకె_బహిర్గతం(buf + nb, c2 ? s2: s2 - 1);

	ఒకవేళ (c2 != 1) {
		జ్ఞాపకం_నకలు(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_అంకె_బహిర్గతం(buf + nb, c2);
	}

	జ్ఞాపకం_నకలు(buf + nb, " @@", 3);
	nb += 3;
	ఒకవేళ (func && funclen) {
		buf[nb++] = ' ';
		ఒకవేళ ((size_t)funclen > పరిమాణం(buf) - nb - 1)
			funclen = పరిమాణం(buf) - nb - 1;
		జ్ఞాపకం_నకలు(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.కొలత = nb;
	ఒకవేళ (ecb->బహిర్గతం_పంక్తి(ecb->priv, &mb, 1) < 0)
		తిరిగి -1;
	తిరిగి 0;
}

పూర్ణాంకం xdl_emit_hunk_hdr(దీర్ఘ s1, దీర్ఘ c1, దీర్ఘ s2, దీర్ఘ c2,
		      స్థిరరాశి అక్షరం *func, దీర్ఘ funclen,
		      xdemitcb_t *ecb) {
	ఒకవేళ (!ecb->బహిర్గతం_hunk)
		తిరిగి xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	ఒకవేళ (ecb->బహిర్గతం_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		తిరిగి -1;
	తిరిగి 0;
}

పూర్ణాంకం xdl_fall_వెనుక_diff(xdfenv_t *diff_env, xpparam_t స్థిరరాశి *xpp,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
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

	subfile1.ptr = (అక్షరం *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.కొలత = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].కొలత - subfile1.ptr;
	subfile2.ptr = (అక్షరం *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.కొలత = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].కొలత - subfile2.ptr;
	ఒకవేళ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		తిరిగి -1;

	జ్ఞాపకం_నకలు(diff_env->xdf1.మారింది + line1 - 1, env.xdf1.మారింది, count1);
	జ్ఞాపకం_నకలు(diff_env->xdf2.మారింది + line2 - 1, env.xdf2.మారింది, count2);

	xdl_free_env(&env);

	తిరిగి 0;
}

శూన్య* xdl_alloc_grow_helper(శూన్య *p, దీర్ఘ nr, దీర్ఘ *alloc, size_t కొలత)
{
	శూన్య *tmp = ఖాళీ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	ఒకవేళ ((size_t)nr > n)
		n = nr;
	ఒకవేళ (కొలత_MAX / కొలత >= n)
		tmp = xdl_realloc(p, n * కొలత);
	ఒకవేళ (tmp) {
		*alloc = (దీర్ఘ)n;
	} లేకపోతే {
		xdl_free(p);
		*alloc = 0;
	}
	తిరిగి tmp;
}
