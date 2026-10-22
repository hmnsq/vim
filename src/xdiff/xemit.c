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

#ضمن "xinclude.h"


ساكن صحيح xdl_emit_رڪارڊ(xdfile_t *xdf, ڊگهو ri, حرف ثابت *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	إذا (xdl_emit_diffrec(rec->ptr, rec->سائيز, pre, (ڊگهو)لڙي_ڊيگهه(pre), ecb) < 0)
		أرجع -1;

	أرجع 0;
}

#وصف_ڪريو signed_جوڙيو_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ساكن ڊگهو saturating_جوڙيو(ڊگهو a, ڊگهو b)
{
	أرجع signed_جوڙيو_overflows(a, b) ? LONG_MAX : a + b;
}

#وصف_رد signed_جوڙيو_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ثابت *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	ڊگهو max_common = saturating_جوڙيو(saturating_جوڙيو(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	ڊگهو max_ignorable = xecfg->ctxlen;
	ڊگهو ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	لكل (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->ايندڙ) {
		xch = xchp->ايندڙ;

		إذا (xch == عدم ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	إذا (!*xscr)
		أرجع عدم;

	lxch = *xscr;

	لكل (xchp = *xscr, xch = xchp->ايندڙ; xch; xchp = xch, xch = xch->ايندڙ) {
		ڊگهو مفاصلو = xch->i1 - (xchp->i1 + xchp->chg1);
		إذا (مفاصلو > max_common)
			اكسر;

		إذا (مفاصلو < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} وإلا إذا (مفاصلو < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} وإلا إذا (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			اكسر;
		} وإلا إذا (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} وإلا {
			ignored += xch->chg2;
		}
	}

	أرجع lxch;
}


#إذا 0
ساكن ڊگهو def_ff(ثابت حرف *rec, ڊگهو len, حرف *buf, ڊگهو sz)
{
	إذا (len > 0 &&
			(اکر_آهي((بنانشان حرف)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		إذا (len > sz)
			len = sz;
		طالما (0 < len && خال_آهي((بنانشان حرف)rec[len - 1]))
			len--;
		يادگيري_نقل(buf, rec, len);
		أرجع len;
	}
	أرجع -1;
}
#جيڪڏهن_پورو

#إذا 0
ساكن ڊگهو ميلاپ_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, ڊگهو ri,
			   حرف *buf, ڊگهو sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	إذا (!xecfg->ڳوليو_func)
		أرجع def_ff(rec->ptr, rec->سائيز, buf, sz);
	أرجع xecfg->ڳوليو_func(rec->ptr, rec->سائيز, buf, sz, xecfg->ڳوليو_func_priv);
}
#جيڪڏهن_پورو

#إذا 0
ساكن صحيح is_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, ڊگهو ri)
{
	حرف dummy[1];
	أرجع ميلاپ_func_rec(xdf, xecfg, ri, dummy, حجم(dummy)) >= 0;
}
#جيڪڏهن_پورو

بنية func_سٽ {
	ڊگهو len;
	حرف buf[80];
};

#إذا 0
ساكن ڊگهو get_func_سٽ(xdfenv_t *xe, xdemitconf_t ثابت *xecfg,
			  بنية func_سٽ *func_سٽ, ڊگهو شروعات, ڊگهو حد_بندي)
{
	ڊگهو l, سائيز, قدم = (شروعات > حد_بندي) ? -1 : 1;
	حرف *buf, dummy[1];

	buf = func_سٽ ? func_سٽ->buf : dummy;
	سائيز = func_سٽ ? حجم(func_سٽ->buf) : حجم(dummy);

	لكل (l = شروعات; l != حد_بندي && 0 <= l && l < xe->xdf1.nrec; l += قدم) {
		ڊگهو len = ميلاپ_func_rec(&xe->xdf1, xecfg, l, buf, سائيز);
		إذا (len >= 0) {
			إذا (func_سٽ)
				func_سٽ->len = len;
			أرجع l;
		}
	}
	أرجع -1;
}
#جيڪڏهن_پورو

#إذا 0
ساكن صحيح is_خالي_جايون_rec(xdfile_t *xdf, ڊگهو ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	ڊگهو i = 0;

	لكل (; i < rec->سائيز && XDL_ISSPACE(rec->ptr[i]); i++);

	أرجع i == rec->سائيز;
}
#جيڪڏهن_پورو

صحيح xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ثابت *xecfg) {
	ڊگهو s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#إذا 0
	ڊگهو funclineprev = -1;
#جيڪڏهن_پورو
	بنية func_سٽ func_سٽ;

	func_سٽ.len = 0;

	لكل (xch = xscr; xch; xch = xche->ايندڙ) {
		xche = xdl_get_hunk(&xch, xecfg);
		إذا (!xch)
			اكسر;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#إذا 0
		إذا (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ڊگهو fs1, i1 = xch->i1;

			/* Appended chunk? */
			إذا (i1 >= xe->xdf1.nrec) {
				ڊگهو i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				طالما (i2 < xe->xdf2.nrec) {
					إذا (is_func_rec(&xe->xdf2, xecfg, i2))
						اذهب post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_سٽ(xe, xecfg, عدم, i1, -1);
			طالما (fs1 > 0 && !is_خالي_جايون_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			إذا (fs1 < 0)
				fs1 = 0;
			إذا (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				طالما (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->ايندڙ;

				/* If so, show it after all. */
				إذا (xchp != xch) {
					xch = xchp;
					اذهب pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#جيڪڏهن_پورو
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#إذا 0
		إذا (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			ڊگهو fe1 = get_func_سٽ(xe, xecfg, عدم,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			طالما (fe1 > 0 && is_خالي_جايون_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			إذا (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			إذا (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			إذا (xche->ايندڙ) {
				ڊگهو l = XDL_MIN(xche->ايندڙ->i1,
						 xe->xdf1.nrec - 1);
				إذا (l - xecfg->ctxlen <= e1 ||
				    get_func_سٽ(xe, xecfg, عدم, l, e1) < 0) {
					xche = xche->ايندڙ;
					اذهب post_context_calculation;
				}
			}
		}
#جيڪڏهن_پورو

		/*
		 * Emit current hunk header.
		 */

#إذا 0
		إذا (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_سٽ(xe, xecfg, &func_سٽ,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#جيڪڏهن_پورو
		إذا (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_سٽ.buf, func_سٽ.len, ecb) < 0)
			أرجع -1;

		/*
		 * Emit pre-context.
		 */
		لكل (; s2 < xch->i2; s2++)
			إذا (xdl_emit_رڪارڊ(&xe->xdf2, s2, " ", ecb) < 0)
				أرجع -1;

		لكل (s1 = xch->i1, s2 = xch->i2;; xch = xch->ايندڙ) {
			/*
			 * Merge previous with current change atom.
			 */
			لكل (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				إذا (xdl_emit_رڪارڊ(&xe->xdf2, s2, " ", ecb) < 0)
					أرجع -1;

			/*
			 * Removes lines from the first file.
			 */
			لكل (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				إذا (xdl_emit_رڪارڊ(&xe->xdf1, s1, "-", ecb) < 0)
					أرجع -1;

			/*
			 * Adds lines from the second file.
			 */
			لكل (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				إذا (xdl_emit_رڪارڊ(&xe->xdf2, s2, "+", ecb) < 0)
					أرجع -1;

			إذا (xch == xche)
				اكسر;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		لكل (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			إذا (xdl_emit_رڪارڊ(&xe->xdf2, s2, " ", ecb) < 0)
				أرجع -1;
	}

	أرجع 0;
}
