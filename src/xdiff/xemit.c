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

#شامل "xinclude.h"


ساکن صحیح xdl_emit_ریکارڈ(xdfile_t *xdf, طویل ri, حرف ثابت *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	اگر (xdl_emit_diffrec(rec->ptr, rec->سائز, pre, (طویل)لڑی_لمبائی(pre), ecb) < 0)
		واپس -1;

	واپس 0;
}

#تعریف_کرو signed_جوڑو_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ساکن طویل saturating_جوڑو(طویل a, طویل b)
{
	واپس signed_جوڑو_overflows(a, b) ? LONG_MAX : a + b;
}

#تعریف_منسوخ signed_جوڑو_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ثابت *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	طویل max_common = saturating_جوڑو(saturating_جوڑو(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	طویل max_ignorable = xecfg->ctxlen;
	طویل ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	برائے (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->اگلا) {
		xch = xchp->اگلا;

		اگر (xch == تہی ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	اگر (!*xscr)
		واپس تہی;

	lxch = *xscr;

	برائے (xchp = *xscr, xch = xchp->اگلا; xch; xchp = xch, xch = xch->اگلا) {
		طویل فاصلہ = xch->i1 - (xchp->i1 + xchp->chg1);
		اگر (فاصلہ > max_common)
			توڑو;

		اگر (فاصلہ < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} ورنہ اگر (فاصلہ < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} ورنہ اگر (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			توڑو;
		} ورنہ اگر (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} ورنہ {
			ignored += xch->chg2;
		}
	}

	واپس lxch;
}


#اگر 0
ساکن طویل def_ff(ثابت حرف *rec, طویل len, حرف *buf, طویل sz)
{
	اگر (len > 0 &&
			(حرف_ہے((غیرعلامتی حرف)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		اگر (len > sz)
			len = sz;
		جب (0 < len && خلا_ہے((غیرعلامتی حرف)rec[len - 1]))
			len--;
		حافظہ_نقل(buf, rec, len);
		واپس len;
	}
	واپس -1;
}
#اگر_ختم

#اگر 0
ساکن طویل مطابقت_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, طویل ri,
			   حرف *buf, طویل sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	اگر (!xecfg->ڈھونڈو_func)
		واپس def_ff(rec->ptr, rec->سائز, buf, sz);
	واپس xecfg->ڈھونڈو_func(rec->ptr, rec->سائز, buf, sz, xecfg->ڈھونڈو_func_priv);
}
#اگر_ختم

#اگر 0
ساکن صحیح is_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, طویل ri)
{
	حرف dummy[1];
	واپس مطابقت_func_rec(xdf, xecfg, ri, dummy, حجم(dummy)) >= 0;
}
#اگر_ختم

ساخت func_سطر {
	طویل len;
	حرف buf[80];
};

#اگر 0
ساکن طویل get_func_سطر(xdfenv_t *xe, xdemitconf_t ثابت *xecfg,
			  ساخت func_سطر *func_سطر, طویل آغاز, طویل حد_بندی)
{
	طویل l, سائز, قدم = (آغاز > حد_بندی) ? -1 : 1;
	حرف *buf, dummy[1];

	buf = func_سطر ? func_سطر->buf : dummy;
	سائز = func_سطر ? حجم(func_سطر->buf) : حجم(dummy);

	برائے (l = آغاز; l != حد_بندی && 0 <= l && l < xe->xdf1.nrec; l += قدم) {
		طویل len = مطابقت_func_rec(&xe->xdf1, xecfg, l, buf, سائز);
		اگر (len >= 0) {
			اگر (func_سطر)
				func_سطر->len = len;
			واپس l;
		}
	}
	واپس -1;
}
#اگر_ختم

#اگر 0
ساکن صحیح is_خالی_خانے_rec(xdfile_t *xdf, طویل ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	طویل i = 0;

	برائے (; i < rec->سائز && XDL_ISSPACE(rec->ptr[i]); i++);

	واپس i == rec->سائز;
}
#اگر_ختم

صحیح xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ثابت *xecfg) {
	طویل s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#اگر 0
	طویل funclineprev = -1;
#اگر_ختم
	ساخت func_سطر func_سطر;

	func_سطر.len = 0;

	برائے (xch = xscr; xch; xch = xche->اگلا) {
		xche = xdl_get_hunk(&xch, xecfg);
		اگر (!xch)
			توڑو;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			طویل fs1, i1 = xch->i1;

			/* Appended chunk? */
			اگر (i1 >= xe->xdf1.nrec) {
				طویل i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				جب (i2 < xe->xdf2.nrec) {
					اگر (is_func_rec(&xe->xdf2, xecfg, i2))
						جاؤ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_سطر(xe, xecfg, تہی, i1, -1);
			جب (fs1 > 0 && !is_خالی_خانے_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			اگر (fs1 < 0)
				fs1 = 0;
			اگر (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				جب (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->اگلا;

				/* If so, show it after all. */
				اگر (xchp != xch) {
					xch = xchp;
					جاؤ pre_context_calculation;
				}
			}
		}

 post_context_calculation:
#اگر_ختم
		lctx = xecfg->ctxlen;
		lctx = XDL_MIN(lctx, xe->xdf1.nrec - (xche->i1 + xche->chg1));
		lctx = XDL_MIN(lctx, xe->xdf2.nrec - (xche->i2 + xche->chg2));

		e1 = xche->i1 + xche->chg1 + lctx;
		e2 = xche->i2 + xche->chg2 + lctx;

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			طویل fe1 = get_func_سطر(xe, xecfg, تہی,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			جب (fe1 > 0 && is_خالی_خانے_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			اگر (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			اگر (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			اگر (xche->اگلا) {
				طویل l = XDL_MIN(xche->اگلا->i1,
						 xe->xdf1.nrec - 1);
				اگر (l - xecfg->ctxlen <= e1 ||
				    get_func_سطر(xe, xecfg, تہی, l, e1) < 0) {
					xche = xche->اگلا;
					جاؤ post_context_calculation;
				}
			}
		}
#اگر_ختم

		/*
		 * Emit current hunk header.
		 */

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_سطر(xe, xecfg, &func_سطر,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#اگر_ختم
		اگر (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_سطر.buf, func_سطر.len, ecb) < 0)
			واپس -1;

		/*
		 * Emit pre-context.
		 */
		برائے (; s2 < xch->i2; s2++)
			اگر (xdl_emit_ریکارڈ(&xe->xdf2, s2, " ", ecb) < 0)
				واپس -1;

		برائے (s1 = xch->i1, s2 = xch->i2;; xch = xch->اگلا) {
			/*
			 * Merge previous with current change atom.
			 */
			برائے (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				اگر (xdl_emit_ریکارڈ(&xe->xdf2, s2, " ", ecb) < 0)
					واپس -1;

			/*
			 * Removes lines from the first file.
			 */
			برائے (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				اگر (xdl_emit_ریکارڈ(&xe->xdf1, s1, "-", ecb) < 0)
					واپس -1;

			/*
			 * Adds lines from the second file.
			 */
			برائے (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				اگر (xdl_emit_ریکارڈ(&xe->xdf2, s2, "+", ecb) < 0)
					واپس -1;

			اگر (xch == xche)
				توڑو;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		برائے (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			اگر (xdl_emit_ریکارڈ(&xe->xdf2, s2, " ", ecb) < 0)
				واپس -1;
	}

	واپس 0;
}
