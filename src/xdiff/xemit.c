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

#ଅନ୍ତର୍ଭାଵ "xinclude.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_record(xdfile_t *xdf, long ri, ଅକ୍ଷର ଅଚଳ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	ଯଦି (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		ଫେରନ୍ତୁ -1;

	ଫେରନ୍ତୁ 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ସ୍ଥିର long saturating_add(long a, long b)
{
	ଫେରନ୍ତୁ signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ଅଚଳ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	ପାଇଁ (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		ଯଦି (xch == ଖାଲି ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	ଯଦି (!*xscr)
		ଫେରନ୍ତୁ ଖାଲି;

	lxch = *xscr;

	ପାଇଁ (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		ଯଦି (distance > max_common)
			ଭାଙ୍ଗ;

		ଯଦି (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} ଅନ୍ୟଥା ଯଦି (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} ଅନ୍ୟଥା ଯଦି (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ଭାଙ୍ଗ;
		} ଅନ୍ୟଥା ଯଦି (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} ଅନ୍ୟଥା {
			ignored += xch->chg2;
		}
	}

	ଫେରନ୍ତୁ lxch;
}


#ଯଦି 0
ସ୍ଥିର long def_ff(ଅଚଳ ଅକ୍ଷର *rec, long len, ଅକ୍ଷର *buf, long sz)
{
	ଯଦି (len > 0 &&
			(isalpha((unsigned ଅକ୍ଷର)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		ଯଦି (len > sz)
			len = sz;
		ଯେତେବେଳେ (0 < len && isspace((unsigned ଅକ୍ଷର)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		ଫେରନ୍ତୁ len;
	}
	ଫେରନ୍ତୁ -1;
}
#endif

#ଯଦି 0
ସ୍ଥିର long match_func_rec(xdfile_t *xdf, xdemitconf_t ଅଚଳ *xecfg, long ri,
			   ଅକ୍ଷର *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	ଯଦି (!xecfg->find_func)
		ଫେରନ୍ତୁ def_ff(rec->ptr, rec->size, buf, sz);
	ଫେରନ୍ତୁ xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#ଯଦି 0
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_func_rec(xdfile_t *xdf, xdemitconf_t ଅଚଳ *xecfg, long ri)
{
	ଅକ୍ଷର dummy[1];
	ଫେରନ୍ତୁ match_func_rec(xdf, xecfg, ri, dummy, ଆକାର(dummy)) >= 0;
}
#endif

ସଂରଚନା func_ଧାଡ଼ି {
	long len;
	ଅକ୍ଷର buf[80];
};

#ଯଦି 0
ସ୍ଥିର long get_func_ଧାଡ଼ି(xdfenv_t *xe, xdemitconf_t ଅଚଳ *xecfg,
			  ସଂରଚନା func_ଧାଡ଼ି *func_ଧାଡ଼ି, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	ଅକ୍ଷର *buf, dummy[1];

	buf = func_ଧାଡ଼ି ? func_ଧାଡ଼ି->buf : dummy;
	size = func_ଧାଡ଼ି ? ଆକାର(func_ଧାଡ଼ି->buf) : ଆକାର(dummy);

	ପାଇଁ (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		ଯଦି (len >= 0) {
			ଯଦି (func_ଧାଡ଼ି)
				func_ଧାଡ଼ି->len = len;
			ଫେରନ୍ତୁ l;
		}
	}
	ଫେରନ୍ତୁ -1;
}
#endif

#ଯଦି 0
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	ପାଇଁ (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	ଫେରନ୍ତୁ i == rec->size;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ଅଚଳ *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#ଯଦି 0
	long funclineprev = -1;
#endif
	ସଂରଚନା func_ଧାଡ଼ି func_ଧାଡ଼ି;

	func_ଧାଡ଼ି.len = 0;

	ପାଇଁ (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		ଯଦି (!xch)
			ଭାଙ୍ଗ;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#ଯଦି 0
		ଯଦି (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			ଯଦି (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				ଯେତେବେଳେ (i2 < xe->xdf2.nrec) {
					ଯଦି (is_func_rec(&xe->xdf2, xecfg, i2))
						ଯାଅ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_ଧାଡ଼ି(xe, xecfg, ଖାଲି, i1, -1);
			ଯେତେବେଳେ (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			ଯଦି (fs1 < 0)
				fs1 = 0;
			ଯଦି (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				ଯେତେବେଳେ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				ଯଦି (xchp != xch) {
					xch = xchp;
					ଯାଅ pre_context_calculation;
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

#ଯଦି 0
		ଯଦି (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_ଧାଡ଼ି(xe, xecfg, ଖାଲି,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			ଯେତେବେଳେ (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			ଯଦି (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			ଯଦି (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			ଯଦି (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				ଯଦି (l - xecfg->ctxlen <= e1 ||
				    get_func_ଧାଡ଼ି(xe, xecfg, ଖାଲି, l, e1) < 0) {
					xche = xche->next;
					ଯାଅ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#ଯଦି 0
		ଯଦି (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_ଧାଡ଼ି(xe, xecfg, &func_ଧାଡ଼ି,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		ଯଦି (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_ଧାଡ଼ି.buf, func_ଧାଡ଼ି.len, ecb) < 0)
			ଫେରନ୍ତୁ -1;

		/*
		 * Emit pre-context.
		 */
		ପାଇଁ (; s2 < xch->i2; s2++)
			ଯଦି (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ଫେରନ୍ତୁ -1;

		ପାଇଁ (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			ପାଇଁ (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				ଯଦି (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					ଫେରନ୍ତୁ -1;

			/*
			 * Removes lines from the first file.
			 */
			ପାଇଁ (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				ଯଦି (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					ଫେରନ୍ତୁ -1;

			/*
			 * Adds lines from the second file.
			 */
			ପାଇଁ (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				ଯଦି (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					ଫେରନ୍ତୁ -1;

			ଯଦି (xch == xche)
				ଭାଙ୍ଗ;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		ପାଇଁ (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			ଯଦି (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ 0;
}
