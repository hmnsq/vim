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

#અન્તર્ભાવ "xinclude.h"


સ્થિર પૂર્ણાંક xdl_emit_record(xdfile_t *xdf, long ri, અક્ષર અચલ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	જો (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		પાછા -1;

	પાછા 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

સ્થિર long saturating_add(long a, long b)
{
	પાછા signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t અચલ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	માટે (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		જો (xch == ખાલી ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	જો (!*xscr)
		પાછા ખાલી;

	lxch = *xscr;

	માટે (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		જો (distance > max_common)
			તોડો;

		જો (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} નહીંતર જો (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} નહીંતર જો (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			તોડો;
		} નહીંતર જો (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} નહીંતર {
			ignored += xch->chg2;
		}
	}

	પાછા lxch;
}


#જો 0
સ્થિર long def_ff(અચલ અક્ષર *rec, long len, અક્ષર *buf, long sz)
{
	જો (len > 0 &&
			(isalpha((unsigned અક્ષર)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		જો (len > sz)
			len = sz;
		જ્યારે (0 < len && isspace((unsigned અક્ષર)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		પાછા len;
	}
	પાછા -1;
}
#endif

#જો 0
સ્થિર long match_func_rec(xdfile_t *xdf, xdemitconf_t અચલ *xecfg, long ri,
			   અક્ષર *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	જો (!xecfg->find_func)
		પાછા def_ff(rec->ptr, rec->size, buf, sz);
	પાછા xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#જો 0
સ્થિર પૂર્ણાંક is_func_rec(xdfile_t *xdf, xdemitconf_t અચલ *xecfg, long ri)
{
	અક્ષર dummy[1];
	પાછા match_func_rec(xdf, xecfg, ri, dummy, કદ(dummy)) >= 0;
}
#endif

સંરચના func_લીટી {
	long len;
	અક્ષર buf[80];
};

#જો 0
સ્થિર long get_func_લીટી(xdfenv_t *xe, xdemitconf_t અચલ *xecfg,
			  સંરચના func_લીટી *func_લીટી, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	અક્ષર *buf, dummy[1];

	buf = func_લીટી ? func_લીટી->buf : dummy;
	size = func_લીટી ? કદ(func_લીટી->buf) : કદ(dummy);

	માટે (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		જો (len >= 0) {
			જો (func_લીટી)
				func_લીટી->len = len;
			પાછા l;
		}
	}
	પાછા -1;
}
#endif

#જો 0
સ્થિર પૂર્ણાંક is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	માટે (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	પાછા i == rec->size;
}
#endif

પૂર્ણાંક xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t અચલ *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#જો 0
	long funclineprev = -1;
#endif
	સંરચના func_લીટી func_લીટી;

	func_લીટી.len = 0;

	માટે (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		જો (!xch)
			તોડો;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#જો 0
		જો (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			જો (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				જ્યારે (i2 < xe->xdf2.nrec) {
					જો (is_func_rec(&xe->xdf2, xecfg, i2))
						જાઓ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_લીટી(xe, xecfg, ખાલી, i1, -1);
			જ્યારે (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			જો (fs1 < 0)
				fs1 = 0;
			જો (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				જ્યારે (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				જો (xchp != xch) {
					xch = xchp;
					જાઓ pre_context_calculation;
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

#જો 0
		જો (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_લીટી(xe, xecfg, ખાલી,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			જ્યારે (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			જો (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			જો (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			જો (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				જો (l - xecfg->ctxlen <= e1 ||
				    get_func_લીટી(xe, xecfg, ખાલી, l, e1) < 0) {
					xche = xche->next;
					જાઓ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#જો 0
		જો (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_લીટી(xe, xecfg, &func_લીટી,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		જો (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_લીટી.buf, func_લીટી.len, ecb) < 0)
			પાછા -1;

		/*
		 * Emit pre-context.
		 */
		માટે (; s2 < xch->i2; s2++)
			જો (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				પાછા -1;

		માટે (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			માટે (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				જો (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					પાછા -1;

			/*
			 * Removes lines from the first file.
			 */
			માટે (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				જો (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					પાછા -1;

			/*
			 * Adds lines from the second file.
			 */
			માટે (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				જો (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					પાછા -1;

			જો (xch == xche)
				તોડો;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		માટે (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			જો (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				પાછા -1;
	}

	પાછા 0;
}
