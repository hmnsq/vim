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

#include "xinclude.h"


static ཧྲིལ xdl_emit_record(xdfile_t *xdf, long ri, var_na const *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	གལ་སྲིད (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		ལོག -1;

	ལོག 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

static long saturating_add(long a, long b)
{
	ལོག signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t const *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	དོན་ལུ (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		གལ་སྲིད (xch == NULL ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	གལ་སྲིད (!*xscr)
		ལོག NULL;

	lxch = *xscr;

	དོན་ལུ (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		གལ་སྲིད (distance > max_common)
			bha_nga;

		གལ་སྲིད (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} དེ་མིན གལ་སྲིད (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} དེ་མིན གལ་སྲིད (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			bha_nga;
		} དེ་མིན གལ་སྲིད (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} དེ་མིན {
			ignored += xch->chg2;
		}
	}

	ལོག lxch;
}


#གལ་སྲིད 0
static long def_ff(const var_na *rec, long len, var_na *buf, long sz)
{
	གལ་སྲིད (len > 0 &&
			(isalpha((unsigned var_na)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		གལ་སྲིད (len > sz)
			len = sz;
		སྐབས (0 < len && isspace((unsigned var_na)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		ལོག len;
	}
	ལོག -1;
}
#endif

#གལ་སྲིད 0
static long match_func_rec(xdfile_t *xdf, xdemitconf_t const *xecfg, long ri,
			   var_na *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	གལ་སྲིད (!xecfg->find_func)
		ལོག def_ff(rec->ptr, rec->size, buf, sz);
	ལོག xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#གལ་སྲིད 0
static ཧྲིལ is_func_rec(xdfile_t *xdf, xdemitconf_t const *xecfg, long ri)
{
	var_na dummy[1];
	ལོག match_func_rec(xdf, xecfg, ri, dummy, parimaa_na(dummy)) >= 0;
}
#endif

sa_nracanaa func_line {
	long len;
	var_na buf[80];
};

#གལ་སྲིད 0
static long get_func_line(xdfenv_t *xe, xdemitconf_t const *xecfg,
			  sa_nracanaa func_line *func_line, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	var_na *buf, dummy[1];

	buf = func_line ? func_line->buf : dummy;
	size = func_line ? parimaa_na(func_line->buf) : parimaa_na(dummy);

	དོན་ལུ (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		གལ་སྲིད (len >= 0) {
			གལ་སྲིད (func_line)
				func_line->len = len;
			ལོག l;
		}
	}
	ལོག -1;
}
#endif

#གལ་སྲིད 0
static ཧྲིལ is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	དོན་ལུ (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	ལོག i == rec->size;
}
#endif

ཧྲིལ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t const *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#གལ་སྲིད 0
	long funclineprev = -1;
#endif
	sa_nracanaa func_line func_line;

	func_line.len = 0;

	དོན་ལུ (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		གལ་སྲིད (!xch)
			bha_nga;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#གལ་སྲིད 0
		གལ་སྲིད (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			གལ་སྲིད (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				སྐབས (i2 < xe->xdf2.nrec) {
					གལ་སྲིད (is_func_rec(&xe->xdf2, xecfg, i2))
						goto post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_line(xe, xecfg, NULL, i1, -1);
			སྐབས (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			གལ་སྲིད (fs1 < 0)
				fs1 = 0;
			གལ་སྲིད (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				སྐབས (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				གལ་སྲིད (xchp != xch) {
					xch = xchp;
					goto pre_context_calculation;
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

#གལ་སྲིད 0
		གལ་སྲིད (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_line(xe, xecfg, NULL,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			སྐབས (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			གལ་སྲིད (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			གལ་སྲིད (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			གལ་སྲིད (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				གལ་སྲིད (l - xecfg->ctxlen <= e1 ||
				    get_func_line(xe, xecfg, NULL, l, e1) < 0) {
					xche = xche->next;
					goto post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#གལ་སྲིད 0
		གལ་སྲིད (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_line(xe, xecfg, &func_line,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		གལ་སྲིད (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_line.buf, func_line.len, ecb) < 0)
			ལོག -1;

		/*
		 * Emit pre-context.
		 */
		དོན་ལུ (; s2 < xch->i2; s2++)
			གལ་སྲིད (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ལོག -1;

		དོན་ལུ (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			དོན་ལུ (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				གལ་སྲིད (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					ལོག -1;

			/*
			 * Removes lines from the first file.
			 */
			དོན་ལུ (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				གལ་སྲིད (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					ལོག -1;

			/*
			 * Adds lines from the second file.
			 */
			དོན་ལུ (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				གལ་སྲིད (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					ལོག -1;

			གལ་སྲིད (xch == xche)
				bha_nga;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		དོན་ལུ (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			གལ་སྲིད (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ལོག -1;
	}

	ལོག 0;
}
