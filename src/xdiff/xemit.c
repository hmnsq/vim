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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"


ਸਥਿਰ ਪੂਰਨ xdl_emit_ਰਿਕਾਰਡ(xdfile_t *xdf, ਲੰਮਾ ri, ਅੱਖਰ ਅਚਲ *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	ਜੇ (xdl_emit_diffrec(rec->ptr, rec->ਮਾਪ, pre, (ਲੰਮਾ)ਲੜੀ_ਲੰਬਾਈ(pre), ecb) < 0)
		ਵਾਪਸ -1;

	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ signed_ਜੋੜੋ_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ਸਥਿਰ ਲੰਮਾ saturating_ਜੋੜੋ(ਲੰਮਾ a, ਲੰਮਾ b)
{
	ਵਾਪਸ signed_ਜੋੜੋ_overflows(a, b) ? LONG_MAX : a + b;
}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ signed_ਜੋੜੋ_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ਅਚਲ *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	ਲੰਮਾ max_common = saturating_ਜੋੜੋ(saturating_ਜੋੜੋ(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	ਲੰਮਾ max_ignorable = xecfg->ctxlen;
	ਲੰਮਾ ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	ਲਈ (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->ਅਗਲਾ) {
		xch = xchp->ਅਗਲਾ;

		ਜੇ (xch == ਨਿਰਾ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	ਜੇ (!*xscr)
		ਵਾਪਸ ਨਿਰਾ;

	lxch = *xscr;

	ਲਈ (xchp = *xscr, xch = xchp->ਅਗਲਾ; xch; xchp = xch, xch = xch->ਅਗਲਾ) {
		ਲੰਮਾ ਦੂਰੀ = xch->i1 - (xchp->i1 + xchp->chg1);
		ਜੇ (ਦੂਰੀ > max_common)
			ਰੋਕੋ;

		ਜੇ (ਦੂਰੀ < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} ਨਹੀਂਤਾਂ ਜੇ (ਦੂਰੀ < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} ਨਹੀਂਤਾਂ ਜੇ (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			ਰੋਕੋ;
		} ਨਹੀਂਤਾਂ ਜੇ (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} ਨਹੀਂਤਾਂ {
			ignored += xch->chg2;
		}
	}

	ਵਾਪਸ lxch;
}


#ਜੇ 0
ਸਥਿਰ ਲੰਮਾ def_ff(ਅਚਲ ਅੱਖਰ *rec, ਲੰਮਾ len, ਅੱਖਰ *buf, ਲੰਮਾ sz)
{
	ਜੇ (len > 0 &&
			(ਅੱਖਰ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		ਜੇ (len > sz)
			len = sz;
		ਜਦੋਂ (0 < len && ਖਾਲੀ_ਥਾਂ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)rec[len - 1]))
			len--;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, rec, len);
		ਵਾਪਸ len;
	}
	ਵਾਪਸ -1;
}
#ਜੇ_ਅੰਤ

#ਜੇ 0
ਸਥਿਰ ਲੰਮਾ ਮੇਲ_func_rec(xdfile_t *xdf, xdemitconf_t ਅਚਲ *xecfg, ਲੰਮਾ ri,
			   ਅੱਖਰ *buf, ਲੰਮਾ sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	ਜੇ (!xecfg->ਲੱਭੋ_func)
		ਵਾਪਸ def_ff(rec->ptr, rec->ਮਾਪ, buf, sz);
	ਵਾਪਸ xecfg->ਲੱਭੋ_func(rec->ptr, rec->ਮਾਪ, buf, sz, xecfg->ਲੱਭੋ_func_priv);
}
#ਜੇ_ਅੰਤ

#ਜੇ 0
ਸਥਿਰ ਪੂਰਨ is_func_rec(xdfile_t *xdf, xdemitconf_t ਅਚਲ *xecfg, ਲੰਮਾ ri)
{
	ਅੱਖਰ dummy[1];
	ਵਾਪਸ ਮੇਲ_func_rec(xdf, xecfg, ri, dummy, ਆਕਾਰ(dummy)) >= 0;
}
#ਜੇ_ਅੰਤ

ਬਣਤਰ func_ਸਤਰ {
	ਲੰਮਾ len;
	ਅੱਖਰ buf[80];
};

#ਜੇ 0
ਸਥਿਰ ਲੰਮਾ get_func_ਸਤਰ(xdfenv_t *xe, xdemitconf_t ਅਚਲ *xecfg,
			  ਬਣਤਰ func_ਸਤਰ *func_ਸਤਰ, ਲੰਮਾ ਸ਼ੁਰੂ, ਲੰਮਾ ਹੱਦਬੰਦੀ)
{
	ਲੰਮਾ l, ਮਾਪ, ਕਦਮ = (ਸ਼ੁਰੂ > ਹੱਦਬੰਦੀ) ? -1 : 1;
	ਅੱਖਰ *buf, dummy[1];

	buf = func_ਸਤਰ ? func_ਸਤਰ->buf : dummy;
	ਮਾਪ = func_ਸਤਰ ? ਆਕਾਰ(func_ਸਤਰ->buf) : ਆਕਾਰ(dummy);

	ਲਈ (l = ਸ਼ੁਰੂ; l != ਹੱਦਬੰਦੀ && 0 <= l && l < xe->xdf1.nrec; l += ਕਦਮ) {
		ਲੰਮਾ len = ਮੇਲ_func_rec(&xe->xdf1, xecfg, l, buf, ਮਾਪ);
		ਜੇ (len >= 0) {
			ਜੇ (func_ਸਤਰ)
				func_ਸਤਰ->len = len;
			ਵਾਪਸ l;
		}
	}
	ਵਾਪਸ -1;
}
#ਜੇ_ਅੰਤ

#ਜੇ 0
ਸਥਿਰ ਪੂਰਨ is_ਖਾਲੀ_ਥਾਵਾਂ_rec(xdfile_t *xdf, ਲੰਮਾ ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	ਲੰਮਾ i = 0;

	ਲਈ (; i < rec->ਮਾਪ && XDL_ISSPACE(rec->ptr[i]); i++);

	ਵਾਪਸ i == rec->ਮਾਪ;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ਅਚਲ *xecfg) {
	ਲੰਮਾ s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#ਜੇ 0
	ਲੰਮਾ funclineprev = -1;
#ਜੇ_ਅੰਤ
	ਬਣਤਰ func_ਸਤਰ func_ਸਤਰ;

	func_ਸਤਰ.len = 0;

	ਲਈ (xch = xscr; xch; xch = xche->ਅਗਲਾ) {
		xche = xdl_get_hunk(&xch, xecfg);
		ਜੇ (!xch)
			ਰੋਕੋ;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#ਜੇ 0
		ਜੇ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ਲੰਮਾ fs1, i1 = xch->i1;

			/* Appended chunk? */
			ਜੇ (i1 >= xe->xdf1.nrec) {
				ਲੰਮਾ i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				ਜਦੋਂ (i2 < xe->xdf2.nrec) {
					ਜੇ (is_func_rec(&xe->xdf2, xecfg, i2))
						ਜਾਓ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_ਸਤਰ(xe, xecfg, ਨਿਰਾ, i1, -1);
			ਜਦੋਂ (fs1 > 0 && !is_ਖਾਲੀ_ਥਾਵਾਂ_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			ਜੇ (fs1 < 0)
				fs1 = 0;
			ਜੇ (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				ਜਦੋਂ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->ਅਗਲਾ;

				/* If so, show it after all. */
				ਜੇ (xchp != xch) {
					xch = xchp;
					ਜਾਓ pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#ਜੇ_ਅੰਤ
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#ਜੇ 0
		ਜੇ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ਲੰਮਾ fe1 = get_func_ਸਤਰ(xe, xecfg, ਨਿਰਾ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			ਜਦੋਂ (fe1 > 0 && is_ਖਾਲੀ_ਥਾਵਾਂ_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			ਜੇ (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			ਜੇ (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			ਜੇ (xche->ਅਗਲਾ) {
				ਲੰਮਾ l = XDL_MIN(xche->ਅਗਲਾ->i1,
						 xe->xdf1.nrec - 1);
				ਜੇ (l - xecfg->ctxlen <= e1 ||
				    get_func_ਸਤਰ(xe, xecfg, ਨਿਰਾ, l, e1) < 0) {
					xche = xche->ਅਗਲਾ;
					ਜਾਓ post_context_calculation;
				}
			}
		}
#ਜੇ_ਅੰਤ

		/*
		 * Emit current hunk header.
		 */

#ਜੇ 0
		ਜੇ (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_ਸਤਰ(xe, xecfg, &func_ਸਤਰ,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#ਜੇ_ਅੰਤ
		ਜੇ (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_ਸਤਰ.buf, func_ਸਤਰ.len, ecb) < 0)
			ਵਾਪਸ -1;

		/*
		 * Emit pre-context.
		 */
		ਲਈ (; s2 < xch->i2; s2++)
			ਜੇ (xdl_emit_ਰਿਕਾਰਡ(&xe->xdf2, s2, " ", ecb) < 0)
				ਵਾਪਸ -1;

		ਲਈ (s1 = xch->i1, s2 = xch->i2;; xch = xch->ਅਗਲਾ) {
			/*
			 * Merge previous with current change atom.
			 */
			ਲਈ (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				ਜੇ (xdl_emit_ਰਿਕਾਰਡ(&xe->xdf2, s2, " ", ecb) < 0)
					ਵਾਪਸ -1;

			/*
			 * Removes lines from the first file.
			 */
			ਲਈ (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				ਜੇ (xdl_emit_ਰਿਕਾਰਡ(&xe->xdf1, s1, "-", ecb) < 0)
					ਵਾਪਸ -1;

			/*
			 * Adds lines from the second file.
			 */
			ਲਈ (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				ਜੇ (xdl_emit_ਰਿਕਾਰਡ(&xe->xdf2, s2, "+", ecb) < 0)
					ਵਾਪਸ -1;

			ਜੇ (xch == xche)
				ਰੋਕੋ;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		ਲਈ (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			ਜੇ (xdl_emit_ਰਿਕਾਰਡ(&xe->xdf2, s2, " ", ecb) < 0)
				ਵਾਪਸ -1;
	}

	ਵਾਪਸ 0;
}
