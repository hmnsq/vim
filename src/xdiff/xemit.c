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

#ಅನ್ತರ್ಭಾವ "xinclude.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_emit_ದಾಖಲೆ(xdfile_t *xdf, ದೀರ್ಘ ri, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	ಆದರೆ (xdl_emit_diffrec(rec->ptr, rec->ಗಾತ್ರ_2, pre, (ದೀರ್ಘ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pre), ecb) < 0)
		ಹಿಂತಿರುಗಿ -1;

	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು signed_ಕೂಡಿಸು_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ಸ್ಥಿರ ದೀರ್ಘ saturating_ಕೂಡಿಸು(ದೀರ್ಘ a, ದೀರ್ಘ b)
{
	ಹಿಂತಿರುಗಿ signed_ಕೂಡಿಸು_overflows(a, b) ? LONG_MAX : a + b;
}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು signed_ಕೂಡಿಸು_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	ದೀರ್ಘ max_common = saturating_ಕೂಡಿಸು(saturating_ಕೂಡಿಸು(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	ದೀರ್ಘ max_ignorable = xecfg->ctxlen;
	ದೀರ್ಘ ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	ಗಾಗಿ (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->ಮುಂದಿನ) {
		xch = xchp->ಮುಂದಿನ;

		ಆದರೆ (xch == ಖಾಲಿ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	ಆದರೆ (!*xscr)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	lxch = *xscr;

	ಗಾಗಿ (xchp = *xscr, xch = xchp->ಮುಂದಿನ; xch; xchp = xch, xch = xch->ಮುಂದಿನ) {
		ದೀರ್ಘ ದೂರ = xch->i1 - (xchp->i1 + xchp->chg1);
		ಆದರೆ (ದೂರ > max_common)
			ನಿಲ್ಲಿಸು;

		ಆದರೆ (ದೂರ < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದೂರ < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ನಿಲ್ಲಿಸು;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ignored += xch->chg2;
		}
	}

	ಹಿಂತಿರುಗಿ lxch;
}


#ಆದರೆ 0
ಸ್ಥಿರ ದೀರ್ಘ def_ff(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rec, ದೀರ್ಘ len, ಅಕ್ಷರ *buf, ದೀರ್ಘ sz)
{
	ಆದರೆ (len > 0 &&
			(ಅಕ್ಷರವೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		ಆದರೆ (len > sz)
			len = sz;
		ಆಗ (0 < len && ಖಾಲಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)rec[len - 1]))
			len--;
		ಸ್ಮರಣೆ_ನಕಲು(buf, rec, len);
		ಹಿಂತಿರುಗಿ len;
	}
	ಹಿಂತಿರುಗಿ -1;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0
ಸ್ಥಿರ ದೀರ್ಘ ಹೊಂದಿಕೆ_func_rec(xdfile_t *xdf, xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg, ದೀರ್ಘ ri,
			   ಅಕ್ಷರ *buf, ದೀರ್ಘ sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	ಆದರೆ (!xecfg->ಕಂಡುಹಿಡಿ_func)
		ಹಿಂತಿರುಗಿ def_ff(rec->ptr, rec->ಗಾತ್ರ_2, buf, sz);
	ಹಿಂತಿರುಗಿ xecfg->ಕಂಡುಹಿಡಿ_func(rec->ptr, rec->ಗಾತ್ರ_2, buf, sz, xecfg->ಕಂಡುಹಿಡಿ_func_priv);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_func_rec(xdfile_t *xdf, xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg, ದೀರ್ಘ ri)
{
	ಅಕ್ಷರ dummy[1];
	ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_func_rec(xdf, xecfg, ri, dummy, ಗಾತ್ರ(dummy)) >= 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ func_ಸಾಲು {
	ದೀರ್ಘ len;
	ಅಕ್ಷರ buf[80];
};

#ಆದರೆ 0
ಸ್ಥಿರ ದೀರ್ಘ get_func_ಸಾಲು(xdfenv_t *xe, xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg,
			  ರಚನೆ func_ಸಾಲು *func_ಸಾಲು, ದೀರ್ಘ ಆರಂಭ, ದೀರ್ಘ ಎಲ್ಲೆ)
{
	ದೀರ್ಘ l, ಗಾತ್ರ_2, ಹೆಜ್ಜೆ = (ಆರಂಭ > ಎಲ್ಲೆ) ? -1 : 1;
	ಅಕ್ಷರ *buf, dummy[1];

	buf = func_ಸಾಲು ? func_ಸಾಲು->buf : dummy;
	ಗಾತ್ರ_2 = func_ಸಾಲು ? ಗಾತ್ರ(func_ಸಾಲು->buf) : ಗಾತ್ರ(dummy);

	ಗಾಗಿ (l = ಆರಂಭ; l != ಎಲ್ಲೆ && 0 <= l && l < xe->xdf1.nrec; l += ಹೆಜ್ಜೆ) {
		ದೀರ್ಘ len = ಹೊಂದಿಕೆ_func_rec(&xe->xdf1, xecfg, l, buf, ಗಾತ್ರ_2);
		ಆದರೆ (len >= 0) {
			ಆದರೆ (func_ಸಾಲು)
				func_ಸಾಲು->len = len;
			ಹಿಂತಿರುಗಿ l;
		}
	}
	ಹಿಂತಿರುಗಿ -1;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_ಖಾಲಿಗಳು_rec(xdfile_t *xdf, ದೀರ್ಘ ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	ದೀರ್ಘ i = 0;

	ಗಾಗಿ (; i < rec->ಗಾತ್ರ_2 && XDL_ISSPACE(rec->ptr[i]); i++);

	ಹಿಂತಿರುಗಿ i == rec->ಗಾತ್ರ_2;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg) {
	ದೀರ್ಘ s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#ಆದರೆ 0
	ದೀರ್ಘ funclineprev = -1;
#ಆದರೆ_ಅಂತ್ಯ
	ರಚನೆ func_ಸಾಲು func_ಸಾಲು;

	func_ಸಾಲು.len = 0;

	ಗಾಗಿ (xch = xscr; xch; xch = xche->ಮುಂದಿನ) {
		xche = xdl_get_hunk(&xch, xecfg);
		ಆದರೆ (!xch)
			ನಿಲ್ಲಿಸು;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#ಆದರೆ 0
		ಆದರೆ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ದೀರ್ಘ fs1, i1 = xch->i1;

			/* Appended chunk? */
			ಆದರೆ (i1 >= xe->xdf1.nrec) {
				ದೀರ್ಘ i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				ಆಗ (i2 < xe->xdf2.nrec) {
					ಆದರೆ (is_func_rec(&xe->xdf2, xecfg, i2))
						ಹೋಗು post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_ಸಾಲು(xe, xecfg, ಖಾಲಿ, i1, -1);
			ಆಗ (fs1 > 0 && !is_ಖಾಲಿಗಳು_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			ಆದರೆ (fs1 < 0)
				fs1 = 0;
			ಆದರೆ (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				ಆಗ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->ಮುಂದಿನ;

				/* If so, show it after all. */
				ಆದರೆ (xchp != xch) {
					xch = xchp;
					ಹೋಗು pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#ಆದರೆ_ಅಂತ್ಯ
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#ಆದರೆ 0
		ಆದರೆ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ದೀರ್ಘ fe1 = get_func_ಸಾಲು(xe, xecfg, ಖಾಲಿ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			ಆಗ (fe1 > 0 && is_ಖಾಲಿಗಳು_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			ಆದರೆ (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			ಆದರೆ (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			ಆದರೆ (xche->ಮುಂದಿನ) {
				ದೀರ್ಘ l = XDL_MIN(xche->ಮುಂದಿನ->i1,
						 xe->xdf1.nrec - 1);
				ಆದರೆ (l - xecfg->ctxlen <= e1 ||
				    get_func_ಸಾಲು(xe, xecfg, ಖಾಲಿ, l, e1) < 0) {
					xche = xche->ಮುಂದಿನ;
					ಹೋಗು post_context_calculation;
				}
			}
		}
#ಆದರೆ_ಅಂತ್ಯ

		/*
		 * Emit current hunk header.
		 */

#ಆದರೆ 0
		ಆದರೆ (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_ಸಾಲು(xe, xecfg, &func_ಸಾಲು,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_ಸಾಲು.buf, func_ಸಾಲು.len, ecb) < 0)
			ಹಿಂತಿರುಗಿ -1;

		/*
		 * Emit pre-context.
		 */
		ಗಾಗಿ (; s2 < xch->i2; s2++)
			ಆದರೆ (xdl_emit_ದಾಖಲೆ(&xe->xdf2, s2, " ", ecb) < 0)
				ಹಿಂತಿರುಗಿ -1;

		ಗಾಗಿ (s1 = xch->i1, s2 = xch->i2;; xch = xch->ಮುಂದಿನ) {
			/*
			 * Merge previous with current change atom.
			 */
			ಗಾಗಿ (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				ಆದರೆ (xdl_emit_ದಾಖಲೆ(&xe->xdf2, s2, " ", ecb) < 0)
					ಹಿಂತಿರುಗಿ -1;

			/*
			 * Removes lines from the first file.
			 */
			ಗಾಗಿ (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				ಆದರೆ (xdl_emit_ದಾಖಲೆ(&xe->xdf1, s1, "-", ecb) < 0)
					ಹಿಂತಿರುಗಿ -1;

			/*
			 * Adds lines from the second file.
			 */
			ಗಾಗಿ (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				ಆದರೆ (xdl_emit_ದಾಖಲೆ(&xe->xdf2, s2, "+", ecb) < 0)
					ಹಿಂತಿರುಗಿ -1;

			ಆದರೆ (xch == xche)
				ನಿಲ್ಲಿಸು;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		ಗಾಗಿ (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			ಆದರೆ (xdl_emit_ದಾಖಲೆ(&xe->xdf2, s2, " ", ecb) < 0)
				ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ 0;
}
