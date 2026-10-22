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

#অন্তর্ভাৱ "xinclude.h"


স্থির পূর্ণাঙ্ক xdl_emit_record(xdfile_t *xdf, long ri, ৱর্ণ ধ্রুৱ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	যদি (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		ঘূৰাই -1;

	ঘূৰাই 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

স্থির long saturating_add(long a, long b)
{
	ঘূৰাই signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ধ্রুৱ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	বাবে (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		যদি (xch == রিক্ত ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	যদি (!*xscr)
		ঘূৰাই রিক্ত;

	lxch = *xscr;

	বাবে (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		যদি (distance > max_common)
			ভঙ্গ;

		যদি (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} অন্যথা যদি (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} অন্যথা যদি (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ভঙ্গ;
		} অন্যথা যদি (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} অন্যথা {
			ignored += xch->chg2;
		}
	}

	ঘূৰাই lxch;
}


#যদি 0
স্থির long def_ff(ধ্রুৱ ৱর্ণ *rec, long len, ৱর্ণ *buf, long sz)
{
	যদি (len > 0 &&
			(isalpha((unsigned ৱর্ণ)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		যদি (len > sz)
			len = sz;
		যেতিয়ালৈ (0 < len && isspace((unsigned ৱর্ণ)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		ঘূৰাই len;
	}
	ঘূৰাই -1;
}
#endif

#যদি 0
স্থির long match_func_rec(xdfile_t *xdf, xdemitconf_t ধ্রুৱ *xecfg, long ri,
			   ৱর্ণ *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	যদি (!xecfg->find_func)
		ঘূৰাই def_ff(rec->ptr, rec->size, buf, sz);
	ঘূৰাই xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#যদি 0
স্থির পূর্ণাঙ্ক is_func_rec(xdfile_t *xdf, xdemitconf_t ধ্রুৱ *xecfg, long ri)
{
	ৱর্ণ dummy[1];
	ঘূৰাই match_func_rec(xdf, xecfg, ri, dummy, পরিমাণ(dummy)) >= 0;
}
#endif

সংরচনা func_সাৰি {
	long len;
	ৱর্ণ buf[80];
};

#যদি 0
স্থির long get_func_সাৰি(xdfenv_t *xe, xdemitconf_t ধ্রুৱ *xecfg,
			  সংরচনা func_সাৰি *func_সাৰি, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	ৱর্ণ *buf, dummy[1];

	buf = func_সাৰি ? func_সাৰি->buf : dummy;
	size = func_সাৰি ? পরিমাণ(func_সাৰি->buf) : পরিমাণ(dummy);

	বাবে (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		যদি (len >= 0) {
			যদি (func_সাৰি)
				func_সাৰি->len = len;
			ঘূৰাই l;
		}
	}
	ঘূৰাই -1;
}
#endif

#যদি 0
স্থির পূর্ণাঙ্ক is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	বাবে (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	ঘূৰাই i == rec->size;
}
#endif

পূর্ণাঙ্ক xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ধ্রুৱ *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#যদি 0
	long funclineprev = -1;
#endif
	সংরচনা func_সাৰি func_সাৰি;

	func_সাৰি.len = 0;

	বাবে (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		যদি (!xch)
			ভঙ্গ;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#যদি 0
		যদি (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			যদি (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				যেতিয়ালৈ (i2 < xe->xdf2.nrec) {
					যদি (is_func_rec(&xe->xdf2, xecfg, i2))
						গচ্ছ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_সাৰি(xe, xecfg, রিক্ত, i1, -1);
			যেতিয়ালৈ (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			যদি (fs1 < 0)
				fs1 = 0;
			যদি (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				যেতিয়ালৈ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				যদি (xchp != xch) {
					xch = xchp;
					গচ্ছ pre_context_calculation;
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

#যদি 0
		যদি (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_সাৰি(xe, xecfg, রিক্ত,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			যেতিয়ালৈ (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			যদি (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			যদি (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			যদি (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				যদি (l - xecfg->ctxlen <= e1 ||
				    get_func_সাৰি(xe, xecfg, রিক্ত, l, e1) < 0) {
					xche = xche->next;
					গচ্ছ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#যদি 0
		যদি (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_সাৰি(xe, xecfg, &func_সাৰি,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		যদি (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_সাৰি.buf, func_সাৰি.len, ecb) < 0)
			ঘূৰাই -1;

		/*
		 * Emit pre-context.
		 */
		বাবে (; s2 < xch->i2; s2++)
			যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ঘূৰাই -1;

		বাবে (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			বাবে (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					ঘূৰাই -1;

			/*
			 * Removes lines from the first file.
			 */
			বাবে (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				যদি (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					ঘূৰাই -1;

			/*
			 * Adds lines from the second file.
			 */
			বাবে (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				যদি (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					ঘূৰাই -1;

			যদি (xch == xche)
				ভঙ্গ;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		বাবে (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ঘূৰাই -1;
	}

	ঘূৰাই 0;
}
