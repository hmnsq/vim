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

#അന്തര്ഭാവ "xinclude.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ xdl_emit_record(xdfile_t *xdf, long ri, അക്ഷരം സ്ഥിരം *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	എങ്കിൽ (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		തിരികെ -1;

	തിരികെ 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

സ്ഥിതം long saturating_add(long a, long b)
{
	തിരികെ signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t സ്ഥിരം *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	വേണ്ടി (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		എങ്കിൽ (xch == ശൂന്യ ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	എങ്കിൽ (!*xscr)
		തിരികെ ശൂന്യ;

	lxch = *xscr;

	വേണ്ടി (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		എങ്കിൽ (distance > max_common)
			നിർത്തുക;

		എങ്കിൽ (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} അല്ലെങ്കിൽ എങ്കിൽ (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} അല്ലെങ്കിൽ എങ്കിൽ (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			നിർത്തുക;
		} അല്ലെങ്കിൽ എങ്കിൽ (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} അല്ലെങ്കിൽ {
			ignored += xch->chg2;
		}
	}

	തിരികെ lxch;
}


#എങ്കിൽ 0
സ്ഥിതം long def_ff(സ്ഥിരം അക്ഷരം *rec, long len, അക്ഷരം *buf, long sz)
{
	എങ്കിൽ (len > 0 &&
			(isalpha((unsigned അക്ഷരം)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		എങ്കിൽ (len > sz)
			len = sz;
		ആകുമ്പോൾ (0 < len && isspace((unsigned അക്ഷരം)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		തിരികെ len;
	}
	തിരികെ -1;
}
#endif

#എങ്കിൽ 0
സ്ഥിതം long match_func_rec(xdfile_t *xdf, xdemitconf_t സ്ഥിരം *xecfg, long ri,
			   അക്ഷരം *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	എങ്കിൽ (!xecfg->find_func)
		തിരികെ def_ff(rec->ptr, rec->size, buf, sz);
	തിരികെ xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#എങ്കിൽ 0
സ്ഥിതം പൂർണ്ണസംഖ്യ is_func_rec(xdfile_t *xdf, xdemitconf_t സ്ഥിരം *xecfg, long ri)
{
	അക്ഷരം dummy[1];
	തിരികെ match_func_rec(xdf, xecfg, ri, dummy, വലുപ്പം(dummy)) >= 0;
}
#endif

ഘടന func_വരി {
	long len;
	അക്ഷരം buf[80];
};

#എങ്കിൽ 0
സ്ഥിതം long get_func_വരി(xdfenv_t *xe, xdemitconf_t സ്ഥിരം *xecfg,
			  ഘടന func_വരി *func_വരി, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	അക്ഷരം *buf, dummy[1];

	buf = func_വരി ? func_വരി->buf : dummy;
	size = func_വരി ? വലുപ്പം(func_വരി->buf) : വലുപ്പം(dummy);

	വേണ്ടി (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		എങ്കിൽ (len >= 0) {
			എങ്കിൽ (func_വരി)
				func_വരി->len = len;
			തിരികെ l;
		}
	}
	തിരികെ -1;
}
#endif

#എങ്കിൽ 0
സ്ഥിതം പൂർണ്ണസംഖ്യ is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	വേണ്ടി (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	തിരികെ i == rec->size;
}
#endif

പൂർണ്ണസംഖ്യ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t സ്ഥിരം *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#എങ്കിൽ 0
	long funclineprev = -1;
#endif
	ഘടന func_വരി func_വരി;

	func_വരി.len = 0;

	വേണ്ടി (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		എങ്കിൽ (!xch)
			നിർത്തുക;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#എങ്കിൽ 0
		എങ്കിൽ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			എങ്കിൽ (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				ആകുമ്പോൾ (i2 < xe->xdf2.nrec) {
					എങ്കിൽ (is_func_rec(&xe->xdf2, xecfg, i2))
						പോകുക post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_വരി(xe, xecfg, ശൂന്യ, i1, -1);
			ആകുമ്പോൾ (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			എങ്കിൽ (fs1 < 0)
				fs1 = 0;
			എങ്കിൽ (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				ആകുമ്പോൾ (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				എങ്കിൽ (xchp != xch) {
					xch = xchp;
					പോകുക pre_context_calculation;
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

#എങ്കിൽ 0
		എങ്കിൽ (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_വരി(xe, xecfg, ശൂന്യ,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			ആകുമ്പോൾ (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			എങ്കിൽ (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			എങ്കിൽ (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			എങ്കിൽ (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				എങ്കിൽ (l - xecfg->ctxlen <= e1 ||
				    get_func_വരി(xe, xecfg, ശൂന്യ, l, e1) < 0) {
					xche = xche->next;
					പോകുക post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#എങ്കിൽ 0
		എങ്കിൽ (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_വരി(xe, xecfg, &func_വരി,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		എങ്കിൽ (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_വരി.buf, func_വരി.len, ecb) < 0)
			തിരികെ -1;

		/*
		 * Emit pre-context.
		 */
		വേണ്ടി (; s2 < xch->i2; s2++)
			എങ്കിൽ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				തിരികെ -1;

		വേണ്ടി (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			വേണ്ടി (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				എങ്കിൽ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					തിരികെ -1;

			/*
			 * Removes lines from the first file.
			 */
			വേണ്ടി (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				എങ്കിൽ (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					തിരികെ -1;

			/*
			 * Adds lines from the second file.
			 */
			വേണ്ടി (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				എങ്കിൽ (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					തിരികെ -1;

			എങ്കിൽ (xch == xche)
				നിർത്തുക;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		വേണ്ടി (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			എങ്കിൽ (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				തിരികെ -1;
	}

	തിരികെ 0;
}
