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

#अन्तर्भाव "xinclude.h"


स्थिर पूर्णांक xdl_emit_record(xdfile_t *xdf, दीर्घ ri, अक्षर स्थिरांक *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	जर (xdl_emit_diffrec(rec->ptr, rec->size, pre, (दीर्घ)म_माप(pre), ecb) < 0)
		परत -1;

	परत 0;
}

#define signed_add_overflows(a, b) \
    ((b) > दीर्घ_उच्च - (a))

स्थिर दीर्घ saturating_add(दीर्घ a, दीर्घ b)
{
	परत signed_add_overflows(a, b) ? दीर्घ_उच्च : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t स्थिरांक *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	दीर्घ max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	दीर्घ max_ignorable = xecfg->ctxlen;
	दीर्घ ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	साठी (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		जर (xch == रिक्त ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	जर (!*xscr)
		परत रिक्त;

	lxch = *xscr;

	साठी (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		दीर्घ distance = xch->i1 - (xchp->i1 + xchp->chg1);
		जर (distance > max_common)
			थांबा;

		जर (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} अन्यथा जर (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} अन्यथा जर (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			थांबा;
		} अन्यथा जर (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} अन्यथा {
			ignored += xch->chg2;
		}
	}

	परत lxch;
}


#जर 0
स्थिर दीर्घ def_ff(स्थिरांक अक्षर *rec, दीर्घ len, अक्षर *buf, दीर्घ sz)
{
	जर (len > 0 &&
			(है_अक्षर((अचिन्हित अक्षर)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		जर (len > sz)
			len = sz;
		पर्यंत (0 < len && है_खाली((अचिन्हित अक्षर)rec[len - 1]))
			len--;
		स_नकल(buf, rec, len);
		परत len;
	}
	परत -1;
}
#endif

#जर 0
स्थिर दीर्घ match_func_rec(xdfile_t *xdf, xdemitconf_t स्थिरांक *xecfg, दीर्घ ri,
			   अक्षर *buf, दीर्घ sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	जर (!xecfg->find_func)
		परत def_ff(rec->ptr, rec->size, buf, sz);
	परत xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#जर 0
स्थिर पूर्णांक is_func_rec(xdfile_t *xdf, xdemitconf_t स्थिरांक *xecfg, दीर्घ ri)
{
	अक्षर dummy[1];
	परत match_func_rec(xdf, xecfg, ri, dummy, आकार(dummy)) >= 0;
}
#endif

संरचना func_ओळ {
	दीर्घ len;
	अक्षर buf[80];
};

#जर 0
स्थिर दीर्घ get_func_ओळ(xdfenv_t *xe, xdemitconf_t स्थिरांक *xecfg,
			  संरचना func_ओळ *func_ओळ, दीर्घ start, दीर्घ limit)
{
	दीर्घ l, size, step = (start > limit) ? -1 : 1;
	अक्षर *buf, dummy[1];

	buf = func_ओळ ? func_ओळ->buf : dummy;
	size = func_ओळ ? आकार(func_ओळ->buf) : आकार(dummy);

	साठी (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		दीर्घ len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		जर (len >= 0) {
			जर (func_ओळ)
				func_ओळ->len = len;
			परत l;
		}
	}
	परत -1;
}
#endif

#जर 0
स्थिर पूर्णांक is_empty_rec(xdfile_t *xdf, दीर्घ ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	दीर्घ i = 0;

	साठी (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	परत i == rec->size;
}
#endif

पूर्णांक xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t स्थिरांक *xecfg) {
	दीर्घ s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#जर 0
	दीर्घ funclineprev = -1;
#endif
	संरचना func_ओळ func_ओळ;

	func_ओळ.len = 0;

	साठी (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		जर (!xch)
			थांबा;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#जर 0
		जर (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			दीर्घ fs1, i1 = xch->i1;

			/* Appended chunk? */
			जर (i1 >= xe->xdf1.nrec) {
				दीर्घ i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				पर्यंत (i2 < xe->xdf2.nrec) {
					जर (is_func_rec(&xe->xdf2, xecfg, i2))
						जा post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_ओळ(xe, xecfg, रिक्त, i1, -1);
			पर्यंत (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			जर (fs1 < 0)
				fs1 = 0;
			जर (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				पर्यंत (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				जर (xchp != xch) {
					xch = xchp;
					जा pre_context_calculation;
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

#जर 0
		जर (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			दीर्घ fe1 = get_func_ओळ(xe, xecfg, रिक्त,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			पर्यंत (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			जर (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			जर (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			जर (xche->next) {
				दीर्घ l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				जर (l - xecfg->ctxlen <= e1 ||
				    get_func_ओळ(xe, xecfg, रिक्त, l, e1) < 0) {
					xche = xche->next;
					जा post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#जर 0
		जर (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_ओळ(xe, xecfg, &func_ओळ,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		जर (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_ओळ.buf, func_ओळ.len, ecb) < 0)
			परत -1;

		/*
		 * Emit pre-context.
		 */
		साठी (; s2 < xch->i2; s2++)
			जर (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				परत -1;

		साठी (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			साठी (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				जर (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					परत -1;

			/*
			 * Removes lines from the first file.
			 */
			साठी (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				जर (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					परत -1;

			/*
			 * Adds lines from the second file.
			 */
			साठी (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				जर (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					परत -1;

			जर (xch == xche)
				थांबा;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		साठी (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			जर (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				परत -1;
	}

	परत 0;
}
