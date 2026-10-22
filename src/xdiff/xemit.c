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


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xdl_emit_record(xdfile_t *xdf, long ri, ವರ್ಣ ಧ್ರುವ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	ಯದಿ (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತ್ಯಾಗಮ 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ಸ್ಥಿರ long saturating_add(long a, long b)
{
	ಪ್ರತ್ಯಾಗಮ signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ಧ್ರುವ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	ಪ್ರತಿ (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		ಯದಿ (xch == ರಿಕ್ತ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	ಯದಿ (!*xscr)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	lxch = *xscr;

	ಪ್ರತಿ (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		ಯದಿ (distance > max_common)
			ಭಙ್ಗ;

		ಯದಿ (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} ಅನ್ಯಥಾ ಯದಿ (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} ಅನ್ಯಥಾ ಯದಿ (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ಭಙ್ಗ;
		} ಅನ್ಯಥಾ ಯದಿ (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} ಅನ್ಯಥಾ {
			ignored += xch->chg2;
		}
	}

	ಪ್ರತ್ಯಾಗಮ lxch;
}


#ಯದಿ 0
ಸ್ಥಿರ long def_ff(ಧ್ರುವ ವರ್ಣ *rec, long len, ವರ್ಣ *buf, long sz)
{
	ಯದಿ (len > 0 &&
			(isalpha((unsigned ವರ್ಣ)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		ಯದಿ (len > sz)
			len = sz;
		ಯಾವತ್ (0 < len && isspace((unsigned ವರ್ಣ)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		ಪ್ರತ್ಯಾಗಮ len;
	}
	ಪ್ರತ್ಯಾಗಮ -1;
}
#endif

#ಯದಿ 0
ಸ್ಥಿರ long match_func_rec(xdfile_t *xdf, xdemitconf_t ಧ್ರುವ *xecfg, long ri,
			   ವರ್ಣ *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	ಯದಿ (!xecfg->find_func)
		ಪ್ರತ್ಯಾಗಮ def_ff(rec->ptr, rec->size, buf, sz);
	ಪ್ರತ್ಯಾಗಮ xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#ಯದಿ 0
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_func_rec(xdfile_t *xdf, xdemitconf_t ಧ್ರುವ *xecfg, long ri)
{
	ವರ್ಣ dummy[1];
	ಪ್ರತ್ಯಾಗಮ match_func_rec(xdf, xecfg, ri, dummy, ಪರಿಮಾಣ(dummy)) >= 0;
}
#endif

ಸಂರಚನಾ func_ಸಾಲು {
	long len;
	ವರ್ಣ buf[80];
};

#ಯದಿ 0
ಸ್ಥಿರ long get_func_ಸಾಲು(xdfenv_t *xe, xdemitconf_t ಧ್ರುವ *xecfg,
			  ಸಂರಚನಾ func_ಸಾಲು *func_ಸಾಲು, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	ವರ್ಣ *buf, dummy[1];

	buf = func_ಸಾಲು ? func_ಸಾಲು->buf : dummy;
	size = func_ಸಾಲು ? ಪರಿಮಾಣ(func_ಸಾಲು->buf) : ಪರಿಮಾಣ(dummy);

	ಪ್ರತಿ (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		ಯದಿ (len >= 0) {
			ಯದಿ (func_ಸಾಲು)
				func_ಸಾಲು->len = len;
			ಪ್ರತ್ಯಾಗಮ l;
		}
	}
	ಪ್ರತ್ಯಾಗಮ -1;
}
#endif

#ಯದಿ 0
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	ಪ್ರತಿ (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	ಪ್ರತ್ಯಾಗಮ i == rec->size;
}
#endif

ಪೂರ್ಣಾಙ್ಕ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ಧ್ರುವ *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#ಯದಿ 0
	long funclineprev = -1;
#endif
	ಸಂರಚನಾ func_ಸಾಲು func_ಸಾಲು;

	func_ಸಾಲು.len = 0;

	ಪ್ರತಿ (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		ಯದಿ (!xch)
			ಭಙ್ಗ;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#ಯದಿ 0
		ಯದಿ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			ಯದಿ (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				ಯಾವತ್ (i2 < xe->xdf2.nrec) {
					ಯದಿ (is_func_rec(&xe->xdf2, xecfg, i2))
						ಗಚ್ಛ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_ಸಾಲು(xe, xecfg, ರಿಕ್ತ, i1, -1);
			ಯಾವತ್ (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			ಯದಿ (fs1 < 0)
				fs1 = 0;
			ಯದಿ (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				ಯಾವತ್ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				ಯದಿ (xchp != xch) {
					xch = xchp;
					ಗಚ್ಛ pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#endif
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#ಯದಿ 0
		ಯದಿ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_ಸಾಲು(xe, xecfg, ರಿಕ್ತ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			ಯಾವತ್ (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			ಯದಿ (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			ಯದಿ (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			ಯದಿ (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				ಯದಿ (l - xecfg->ctxlen <= e1 ||
				    get_func_ಸಾಲು(xe, xecfg, ರಿಕ್ತ, l, e1) < 0) {
					xche = xche->next;
					ಗಚ್ಛ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#ಯದಿ 0
		ಯದಿ (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_ಸಾಲು(xe, xecfg, &func_ಸಾಲು,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		ಯದಿ (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_ಸಾಲು.buf, func_ಸಾಲು.len, ecb) < 0)
			ಪ್ರತ್ಯಾಗಮ -1;

		/*
		 * Emit pre-context.
		 */
		ಪ್ರತಿ (; s2 < xch->i2; s2++)
			ಯದಿ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ಪ್ರತ್ಯಾಗಮ -1;

		ಪ್ರತಿ (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			ಪ್ರತಿ (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				ಯದಿ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					ಪ್ರತ್ಯಾಗಮ -1;

			/*
			 * Removes lines from the first file.
			 */
			ಪ್ರತಿ (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				ಯದಿ (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					ಪ್ರತ್ಯಾಗಮ -1;

			/*
			 * Adds lines from the second file.
			 */
			ಪ್ರತಿ (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				ಯದಿ (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					ಪ್ರತ್ಯಾಗಮ -1;

			ಯದಿ (xch == xche)
				ಭಙ್ಗ;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		ಪ್ರತಿ (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			ಯದಿ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
