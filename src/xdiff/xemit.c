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


స్థిర పూర్ణాంకం xdl_emit_రికార్డు(xdfile_t *xdf, దీర్ఘ ri, అక్షరం స్థిరరాశి *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	ఒకవేళ (xdl_emit_diffrec(rec->ptr, rec->కొలత, pre, (దీర్ఘ)అక్షరశ్రేణి_పొడవు(pre), ecb) < 0)
		తిరిగి -1;

	తిరిగి 0;
}

#నిర్వచించు signed_కూడు_overflows(a, b) \
    ((b) > LONG_MAX - (a))

స్థిర దీర్ఘ saturating_కూడు(దీర్ఘ a, దీర్ఘ b)
{
	తిరిగి signed_కూడు_overflows(a, b) ? LONG_MAX : a + b;
}

#నిర్వచనం_రద్దు signed_కూడు_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t స్థిరరాశి *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	దీర్ఘ max_common = saturating_కూడు(saturating_కూడు(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	దీర్ఘ max_ignorable = xecfg->ctxlen;
	దీర్ఘ ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	కొరకు (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->తదుపరి) {
		xch = xchp->తదుపరి;

		ఒకవేళ (xch == ఖాళీ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	ఒకవేళ (!*xscr)
		తిరిగి ఖాళీ;

	lxch = *xscr;

	కొరకు (xchp = *xscr, xch = xchp->తదుపరి; xch; xchp = xch, xch = xch->తదుపరి) {
		దీర్ఘ దూరం = xch->i1 - (xchp->i1 + xchp->chg1);
		ఒకవేళ (దూరం > max_common)
			ఆపు;

		ఒకవేళ (దూరం < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} లేకపోతే ఒకవేళ (దూరం < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} లేకపోతే ఒకవేళ (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ఆపు;
		} లేకపోతే ఒకవేళ (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} లేకపోతే {
			ignored += xch->chg2;
		}
	}

	తిరిగి lxch;
}


#ఒకవేళ 0
స్థిర దీర్ఘ def_ff(స్థిరరాశి అక్షరం *rec, దీర్ఘ len, అక్షరం *buf, దీర్ఘ sz)
{
	ఒకవేళ (len > 0 &&
			(అక్షరమా((గుర్తులేని అక్షరం)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		ఒకవేళ (len > sz)
			len = sz;
		అయితే (0 < len && ఖాళీనా((గుర్తులేని అక్షరం)rec[len - 1]))
			len--;
		జ్ఞాపకం_నకలు(buf, rec, len);
		తిరిగి len;
	}
	తిరిగి -1;
}
#అయితే_ముగింపు

#ఒకవేళ 0
స్థిర దీర్ఘ సరిపోలిక_func_rec(xdfile_t *xdf, xdemitconf_t స్థిరరాశి *xecfg, దీర్ఘ ri,
			   అక్షరం *buf, దీర్ఘ sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	ఒకవేళ (!xecfg->కనుగొను_func)
		తిరిగి def_ff(rec->ptr, rec->కొలత, buf, sz);
	తిరిగి xecfg->కనుగొను_func(rec->ptr, rec->కొలత, buf, sz, xecfg->కనుగొను_func_priv);
}
#అయితే_ముగింపు

#ఒకవేళ 0
స్థిర పూర్ణాంకం is_func_rec(xdfile_t *xdf, xdemitconf_t స్థిరరాశి *xecfg, దీర్ఘ ri)
{
	అక్షరం dummy[1];
	తిరిగి సరిపోలిక_func_rec(xdf, xecfg, ri, dummy, పరిమాణం(dummy)) >= 0;
}
#అయితే_ముగింపు

నిర్మాణం func_పంక్తి {
	దీర్ఘ len;
	అక్షరం buf[80];
};

#ఒకవేళ 0
స్థిర దీర్ఘ get_func_పంక్తి(xdfenv_t *xe, xdemitconf_t స్థిరరాశి *xecfg,
			  నిర్మాణం func_పంక్తి *func_పంక్తి, దీర్ఘ ప్రారంభం, దీర్ఘ హద్దు)
{
	దీర్ఘ l, కొలత, అడుగు = (ప్రారంభం > హద్దు) ? -1 : 1;
	అక్షరం *buf, dummy[1];

	buf = func_పంక్తి ? func_పంక్తి->buf : dummy;
	కొలత = func_పంక్తి ? పరిమాణం(func_పంక్తి->buf) : పరిమాణం(dummy);

	కొరకు (l = ప్రారంభం; l != హద్దు && 0 <= l && l < xe->xdf1.nrec; l += అడుగు) {
		దీర్ఘ len = సరిపోలిక_func_rec(&xe->xdf1, xecfg, l, buf, కొలత);
		ఒకవేళ (len >= 0) {
			ఒకవేళ (func_పంక్తి)
				func_పంక్తి->len = len;
			తిరిగి l;
		}
	}
	తిరిగి -1;
}
#అయితే_ముగింపు

#ఒకవేళ 0
స్థిర పూర్ణాంకం is_ఖాళీలు_rec(xdfile_t *xdf, దీర్ఘ ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	దీర్ఘ i = 0;

	కొరకు (; i < rec->కొలత && XDL_ISSPACE(rec->ptr[i]); i++);

	తిరిగి i == rec->కొలత;
}
#అయితే_ముగింపు

పూర్ణాంకం xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t స్థిరరాశి *xecfg) {
	దీర్ఘ s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#ఒకవేళ 0
	దీర్ఘ funclineprev = -1;
#అయితే_ముగింపు
	నిర్మాణం func_పంక్తి func_పంక్తి;

	func_పంక్తి.len = 0;

	కొరకు (xch = xscr; xch; xch = xche->తదుపరి) {
		xche = xdl_get_hunk(&xch, xecfg);
		ఒకవేళ (!xch)
			ఆపు;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#ఒకవేళ 0
		ఒకవేళ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			దీర్ఘ fs1, i1 = xch->i1;

			/* Appended chunk? */
			ఒకవేళ (i1 >= xe->xdf1.nrec) {
				దీర్ఘ i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				అయితే (i2 < xe->xdf2.nrec) {
					ఒకవేళ (is_func_rec(&xe->xdf2, xecfg, i2))
						వెళ్ళు post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_పంక్తి(xe, xecfg, ఖాళీ, i1, -1);
			అయితే (fs1 > 0 && !is_ఖాళీలు_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			ఒకవేళ (fs1 < 0)
				fs1 = 0;
			ఒకవేళ (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				అయితే (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->తదుపరి;

				/* If so, show it after all. */
				ఒకవేళ (xchp != xch) {
					xch = xchp;
					వెళ్ళు pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#అయితే_ముగింపు
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#ఒకవేళ 0
		ఒకవేళ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			దీర్ఘ fe1 = get_func_పంక్తి(xe, xecfg, ఖాళీ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			అయితే (fe1 > 0 && is_ఖాళీలు_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			ఒకవేళ (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			ఒకవేళ (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			ఒకవేళ (xche->తదుపరి) {
				దీర్ఘ l = XDL_MIN(xche->తదుపరి->i1,
						 xe->xdf1.nrec - 1);
				ఒకవేళ (l - xecfg->ctxlen <= e1 ||
				    get_func_పంక్తి(xe, xecfg, ఖాళీ, l, e1) < 0) {
					xche = xche->తదుపరి;
					వెళ్ళు post_context_calculation;
				}
			}
		}
#అయితే_ముగింపు

		/*
		 * Emit current hunk header.
		 */

#ఒకవేళ 0
		ఒకవేళ (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_పంక్తి(xe, xecfg, &func_పంక్తి,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#అయితే_ముగింపు
		ఒకవేళ (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_పంక్తి.buf, func_పంక్తి.len, ecb) < 0)
			తిరిగి -1;

		/*
		 * Emit pre-context.
		 */
		కొరకు (; s2 < xch->i2; s2++)
			ఒకవేళ (xdl_emit_రికార్డు(&xe->xdf2, s2, " ", ecb) < 0)
				తిరిగి -1;

		కొరకు (s1 = xch->i1, s2 = xch->i2;; xch = xch->తదుపరి) {
			/*
			 * Merge previous with current change atom.
			 */
			కొరకు (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				ఒకవేళ (xdl_emit_రికార్డు(&xe->xdf2, s2, " ", ecb) < 0)
					తిరిగి -1;

			/*
			 * Removes lines from the first file.
			 */
			కొరకు (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				ఒకవేళ (xdl_emit_రికార్డు(&xe->xdf1, s1, "-", ecb) < 0)
					తిరిగి -1;

			/*
			 * Adds lines from the second file.
			 */
			కొరకు (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				ఒకవేళ (xdl_emit_రికార్డు(&xe->xdf2, s2, "+", ecb) < 0)
					తిరిగి -1;

			ఒకవేళ (xch == xche)
				ఆపు;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		కొరకు (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			ఒకవేళ (xdl_emit_రికార్డు(&xe->xdf2, s2, " ", ecb) < 0)
				తిరిగి -1;
	}

	తిరిగి 0;
}
