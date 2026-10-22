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

#அந்தர்பாவ "xinclude.h"


நிலையான முழு xdl_emit_record(xdfile_t *xdf, long ri, எழுத்து மாறிலி *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	எனில் (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		திரும்பு -1;

	திரும்பு 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

நிலையான long saturating_add(long a, long b)
{
	திரும்பு signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t மாறிலி *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	க்கு (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		எனில் (xch == வெற்று ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	எனில் (!*xscr)
		திரும்பு வெற்று;

	lxch = *xscr;

	க்கு (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		எனில் (distance > max_common)
			நிறுத்து;

		எனில் (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} இல்லையெனில் எனில் (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} இல்லையெனில் எனில் (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			நிறுத்து;
		} இல்லையெனில் எனில் (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} இல்லையெனில் {
			ignored += xch->chg2;
		}
	}

	திரும்பு lxch;
}


#எனில் 0
நிலையான long def_ff(மாறிலி எழுத்து *rec, long len, எழுத்து *buf, long sz)
{
	எனில் (len > 0 &&
			(isalpha((unsigned எழுத்து)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		எனில் (len > sz)
			len = sz;
		போது (0 < len && isspace((unsigned எழுத்து)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		திரும்பு len;
	}
	திரும்பு -1;
}
#endif

#எனில் 0
நிலையான long match_func_rec(xdfile_t *xdf, xdemitconf_t மாறிலி *xecfg, long ri,
			   எழுத்து *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	எனில் (!xecfg->find_func)
		திரும்பு def_ff(rec->ptr, rec->size, buf, sz);
	திரும்பு xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#எனில் 0
நிலையான முழு is_func_rec(xdfile_t *xdf, xdemitconf_t மாறிலி *xecfg, long ri)
{
	எழுத்து dummy[1];
	திரும்பு match_func_rec(xdf, xecfg, ri, dummy, அளவு(dummy)) >= 0;
}
#endif

கட்டமைப்பு func_வரி {
	long len;
	எழுத்து buf[80];
};

#எனில் 0
நிலையான long get_func_வரி(xdfenv_t *xe, xdemitconf_t மாறிலி *xecfg,
			  கட்டமைப்பு func_வரி *func_வரி, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	எழுத்து *buf, dummy[1];

	buf = func_வரி ? func_வரி->buf : dummy;
	size = func_வரி ? அளவு(func_வரி->buf) : அளவு(dummy);

	க்கு (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		எனில் (len >= 0) {
			எனில் (func_வரி)
				func_வரி->len = len;
			திரும்பு l;
		}
	}
	திரும்பு -1;
}
#endif

#எனில் 0
நிலையான முழு is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	க்கு (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	திரும்பு i == rec->size;
}
#endif

முழு xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t மாறிலி *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#எனில் 0
	long funclineprev = -1;
#endif
	கட்டமைப்பு func_வரி func_வரி;

	func_வரி.len = 0;

	க்கு (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		எனில் (!xch)
			நிறுத்து;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#எனில் 0
		எனில் (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			எனில் (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				போது (i2 < xe->xdf2.nrec) {
					எனில் (is_func_rec(&xe->xdf2, xecfg, i2))
						செல் post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_வரி(xe, xecfg, வெற்று, i1, -1);
			போது (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			எனில் (fs1 < 0)
				fs1 = 0;
			எனில் (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				போது (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				எனில் (xchp != xch) {
					xch = xchp;
					செல் pre_context_calculation;
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

#எனில் 0
		எனில் (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_வரி(xe, xecfg, வெற்று,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			போது (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			எனில் (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			எனில் (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			எனில் (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				எனில் (l - xecfg->ctxlen <= e1 ||
				    get_func_வரி(xe, xecfg, வெற்று, l, e1) < 0) {
					xche = xche->next;
					செல் post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#எனில் 0
		எனில் (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_வரி(xe, xecfg, &func_வரி,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		எனில் (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_வரி.buf, func_வரி.len, ecb) < 0)
			திரும்பு -1;

		/*
		 * Emit pre-context.
		 */
		க்கு (; s2 < xch->i2; s2++)
			எனில் (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				திரும்பு -1;

		க்கு (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			க்கு (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				எனில் (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					திரும்பு -1;

			/*
			 * Removes lines from the first file.
			 */
			க்கு (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				எனில் (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					திரும்பு -1;

			/*
			 * Adds lines from the second file.
			 */
			க்கு (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				எனில் (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					திரும்பு -1;

			எனில் (xch == xche)
				நிறுத்து;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		க்கு (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			எனில் (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				திரும்பு -1;
	}

	திரும்பு 0;
}
