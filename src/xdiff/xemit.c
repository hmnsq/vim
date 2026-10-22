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


স্থির পূর্ণসংখ্যা xdl_emit_record(xdfile_t *xdf, long ri, অক্ষর ধ্রুবক *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	যদি (xdl_emit_diffrec(rec->ptr, rec->আকার_2, pre, (long)strlen(pre), ecb) < 0)
		ফেরত -1;

	ফেরত 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

স্থির long saturating_add(long a, long b)
{
	ফেরত signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ধ্রুবক *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	জন্য (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->পরবর্তী) {
		xch = xchp->পরবর্তী;

		যদি (xch == ফাঁকা ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	যদি (!*xscr)
		ফেরত ফাঁকা;

	lxch = *xscr;

	জন্য (xchp = *xscr, xch = xchp->পরবর্তী; xch; xchp = xch, xch = xch->পরবর্তী) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		যদি (distance > max_common)
			বিরতি;

		যদি (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} অন্যথা যদি (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} অন্যথা যদি (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			বিরতি;
		} অন্যথা যদি (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} অন্যথা {
			ignored += xch->chg2;
		}
	}

	ফেরত lxch;
}


#যদি 0
স্থির long def_ff(ধ্রুবক অক্ষর *rec, long len, অক্ষর *buf, long sz)
{
	যদি (len > 0 &&
			(isalpha((unsigned অক্ষর)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		যদি (len > sz)
			len = sz;
		যখন (0 < len && isspace((unsigned অক্ষর)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		ফেরত len;
	}
	ফেরত -1;
}
#endif

#যদি 0
স্থির long match_func_rec(xdfile_t *xdf, xdemitconf_t ধ্রুবক *xecfg, long ri,
			   অক্ষর *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	যদি (!xecfg->find_func)
		ফেরত def_ff(rec->ptr, rec->আকার_2, buf, sz);
	ফেরত xecfg->find_func(rec->ptr, rec->আকার_2, buf, sz, xecfg->find_func_priv);
}
#endif

#যদি 0
স্থির পূর্ণসংখ্যা is_func_rec(xdfile_t *xdf, xdemitconf_t ধ্রুবক *xecfg, long ri)
{
	অক্ষর dummy[1];
	ফেরত match_func_rec(xdf, xecfg, ri, dummy, আকার(dummy)) >= 0;
}
#endif

কাঠামো func_সারি {
	long len;
	অক্ষর buf[80];
};

#যদি 0
স্থির long get_func_সারি(xdfenv_t *xe, xdemitconf_t ধ্রুবক *xecfg,
			  কাঠামো func_সারি *func_সারি, long start, long limit)
{
	long l, আকার_2, step = (start > limit) ? -1 : 1;
	অক্ষর *buf, dummy[1];

	buf = func_সারি ? func_সারি->buf : dummy;
	আকার_2 = func_সারি ? আকার(func_সারি->buf) : আকার(dummy);

	জন্য (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, আকার_2);
		যদি (len >= 0) {
			যদি (func_সারি)
				func_সারি->len = len;
			ফেরত l;
		}
	}
	ফেরত -1;
}
#endif

#যদি 0
স্থির পূর্ণসংখ্যা is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	জন্য (; i < rec->আকার_2 && XDL_ISSPACE(rec->ptr[i]); i++);

	ফেরত i == rec->আকার_2;
}
#endif

পূর্ণসংখ্যা xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ধ্রুবক *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#যদি 0
	long funclineprev = -1;
#endif
	কাঠামো func_সারি func_সারি;

	func_সারি.len = 0;

	জন্য (xch = xscr; xch; xch = xche->পরবর্তী) {
		xche = xdl_get_hunk(&xch, xecfg);
		যদি (!xch)
			বিরতি;

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
				যখন (i2 < xe->xdf2.nrec) {
					যদি (is_func_rec(&xe->xdf2, xecfg, i2))
						যাও post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_সারি(xe, xecfg, ফাঁকা, i1, -1);
			যখন (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
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
				যখন (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->পরবর্তী;

				/* If so, show it after all. */
				যদি (xchp != xch) {
					xch = xchp;
					যাও pre_context_calculation;
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
			long fe1 = get_func_সারি(xe, xecfg, ফাঁকা,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			যখন (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
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
			যদি (xche->পরবর্তী) {
				long l = XDL_MIN(xche->পরবর্তী->i1,
						 xe->xdf1.nrec - 1);
				যদি (l - xecfg->ctxlen <= e1 ||
				    get_func_সারি(xe, xecfg, ফাঁকা, l, e1) < 0) {
					xche = xche->পরবর্তী;
					যাও post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#যদি 0
		যদি (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_সারি(xe, xecfg, &func_সারি,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		যদি (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_সারি.buf, func_সারি.len, ecb) < 0)
			ফেরত -1;

		/*
		 * Emit pre-context.
		 */
		জন্য (; s2 < xch->i2; s2++)
			যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ফেরত -1;

		জন্য (s1 = xch->i1, s2 = xch->i2;; xch = xch->পরবর্তী) {
			/*
			 * Merge previous with current change atom.
			 */
			জন্য (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					ফেরত -1;

			/*
			 * Removes lines from the first file.
			 */
			জন্য (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				যদি (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					ফেরত -1;

			/*
			 * Adds lines from the second file.
			 */
			জন্য (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				যদি (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					ফেরত -1;

			যদি (xch == xche)
				বিরতি;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		জন্য (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			যদি (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ফেরত -1;
	}

	ফেরত 0;
}
