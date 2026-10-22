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


ایستا صحیح xdl_emit_record(xdfile_t *xdf, long ri, نویسه ثابت *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	اگر (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		بازگشت -1;

	بازگشت 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

ایستا long saturating_add(long a, long b)
{
	بازگشت signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ثابت *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	برای (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		اگر (xch == پوچ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	اگر (!*xscr)
		بازگشت پوچ;

	lxch = *xscr;

	برای (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		اگر (distance > max_common)
			بشکن;

		اگر (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} وگرنه اگر (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} وگرنه اگر (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			بشکن;
		} وگرنه اگر (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} وگرنه {
			ignored += xch->chg2;
		}
	}

	بازگشت lxch;
}


#اگر 0
ایستا long def_ff(ثابت نویسه *rec, long len, نویسه *buf, long sz)
{
	اگر (len > 0 &&
			(isalpha((unsigned نویسه)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		اگر (len > sz)
			len = sz;
		وقتی (0 < len && isspace((unsigned نویسه)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		بازگشت len;
	}
	بازگشت -1;
}
#endif

#اگر 0
ایستا long match_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, long ri,
			   نویسه *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	اگر (!xecfg->find_func)
		بازگشت def_ff(rec->ptr, rec->size, buf, sz);
	بازگشت xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#اگر 0
ایستا صحیح is_func_rec(xdfile_t *xdf, xdemitconf_t ثابت *xecfg, long ri)
{
	نویسه dummy[1];
	بازگشت match_func_rec(xdf, xecfg, ri, dummy, اندازه(dummy)) >= 0;
}
#endif

ساختار func_سطر {
	long len;
	نویسه buf[80];
};

#اگر 0
ایستا long get_func_سطر(xdfenv_t *xe, xdemitconf_t ثابت *xecfg,
			  ساختار func_سطر *func_سطر, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	نویسه *buf, dummy[1];

	buf = func_سطر ? func_سطر->buf : dummy;
	size = func_سطر ? اندازه(func_سطر->buf) : اندازه(dummy);

	برای (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		اگر (len >= 0) {
			اگر (func_سطر)
				func_سطر->len = len;
			بازگشت l;
		}
	}
	بازگشت -1;
}
#endif

#اگر 0
ایستا صحیح is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	برای (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	بازگشت i == rec->size;
}
#endif

صحیح xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ثابت *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#اگر 0
	long funclineprev = -1;
#endif
	ساختار func_سطر func_سطر;

	func_سطر.len = 0;

	برای (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		اگر (!xch)
			بشکن;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			اگر (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				وقتی (i2 < xe->xdf2.nrec) {
					اگر (is_func_rec(&xe->xdf2, xecfg, i2))
						برو post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_سطر(xe, xecfg, پوچ, i1, -1);
			وقتی (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
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
				وقتی (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				اگر (xchp != xch) {
					xch = xchp;
					برو pre_context_calculation;
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

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_سطر(xe, xecfg, پوچ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			وقتی (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
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
			اگر (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				اگر (l - xecfg->ctxlen <= e1 ||
				    get_func_سطر(xe, xecfg, پوچ, l, e1) < 0) {
					xche = xche->next;
					برو post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#اگر 0
		اگر (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_سطر(xe, xecfg, &func_سطر,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		اگر (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_سطر.buf, func_سطر.len, ecb) < 0)
			بازگشت -1;

		/*
		 * Emit pre-context.
		 */
		برای (; s2 < xch->i2; s2++)
			اگر (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				بازگشت -1;

		برای (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			برای (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				اگر (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					بازگشت -1;

			/*
			 * Removes lines from the first file.
			 */
			برای (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				اگر (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					بازگشت -1;

			/*
			 * Adds lines from the second file.
			 */
			برای (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				اگر (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					بازگشت -1;

			اگر (xch == xche)
				بشکن;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		برای (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			اگر (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				بازگشت -1;
	}

	بازگشت 0;
}
