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


स्थिर पूर्णाङ्क xdl_emit_record(xdfile_t *xdf, long ri, वर्ण ध्रुव *pre, xdemitcb_t *ecb)
{
	xrecord_t *rec = &xdf->recs[ri];

	सचे (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		पटिनिवत्त -1;

	पटिनिवत्त 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

स्थिर long saturating_add(long a, long b)
{
	पटिनिवत्त signed_add_overflows(a, b) ? LONG_MAX : a + b;
}

#undef signed_add_overflows

/*
 * Starting at the passed change atom, find the latest change atom to be included
 * inside the differential hunk according to the specified configuration.
 * Also advance xscr if the first changes must be discarded.
 */
xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t ध्रुव *xecfg)
{
	xdchange_t *xch, *xchp, *lxch;
	long max_common = saturating_add(saturating_add(xecfg->ctxlen,
							xecfg->ctxlen),
					 xecfg->interhunkctxlen);
	long max_ignorable = xecfg->ctxlen;
	long ignored = 0; /* number of ignored blank lines */

	/* remove ignorable changes that are too far before other changes */
	अत्थाय (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		सचे (xch == रिक्त ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	सचे (!*xscr)
		पटिनिवत्त रिक्त;

	lxch = *xscr;

	अत्थाय (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		सचे (distance > max_common)
			भङ्ग;

		सचे (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} अञ्ञथा सचे (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} अञ्ञथा सचे (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			भङ्ग;
		} अञ्ञथा सचे (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} अञ्ञथा {
			ignored += xch->chg2;
		}
	}

	पटिनिवत्त lxch;
}


#सचे 0
स्थिर long def_ff(ध्रुव वर्ण *rec, long len, वर्ण *buf, long sz)
{
	सचे (len > 0 &&
			(isalpha((unsigned वर्ण)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		सचे (len > sz)
			len = sz;
		याव (0 < len && isspace((unsigned वर्ण)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		पटिनिवत्त len;
	}
	पटिनिवत्त -1;
}
#endif

#सचे 0
स्थिर long match_func_rec(xdfile_t *xdf, xdemitconf_t ध्रुव *xecfg, long ri,
			   वर्ण *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	सचे (!xecfg->find_func)
		पटिनिवत्त def_ff(rec->ptr, rec->size, buf, sz);
	पटिनिवत्त xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#सचे 0
स्थिर पूर्णाङ्क is_func_rec(xdfile_t *xdf, xdemitconf_t ध्रुव *xecfg, long ri)
{
	वर्ण dummy[1];
	पटिनिवत्त match_func_rec(xdf, xecfg, ri, dummy, परिमाण(dummy)) >= 0;
}
#endif

संरचना func_पङ्क्तिः {
	long len;
	वर्ण buf[80];
};

#सचे 0
स्थिर long get_func_पङ्क्तिः(xdfenv_t *xe, xdemitconf_t ध्रुव *xecfg,
			  संरचना func_पङ्क्तिः *func_पङ्क्तिः, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	वर्ण *buf, dummy[1];

	buf = func_पङ्क्तिः ? func_पङ्क्तिः->buf : dummy;
	size = func_पङ्क्तिः ? परिमाण(func_पङ्क्तिः->buf) : परिमाण(dummy);

	अत्थाय (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		सचे (len >= 0) {
			सचे (func_पङ्क्तिः)
				func_पङ्क्तिः->len = len;
			पटिनिवत्त l;
		}
	}
	पटिनिवत्त -1;
}
#endif

#सचे 0
स्थिर पूर्णाङ्क is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	अत्थाय (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	पटिनिवत्त i == rec->size;
}
#endif

पूर्णाङ्क xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ध्रुव *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#सचे 0
	long funclineprev = -1;
#endif
	संरचना func_पङ्क्तिः func_पङ्क्तिः;

	func_पङ्क्तिः.len = 0;

	अत्थाय (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		सचे (!xch)
			भङ्ग;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#सचे 0
		सचे (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			सचे (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				याव (i2 < xe->xdf2.nrec) {
					सचे (is_func_rec(&xe->xdf2, xecfg, i2))
						गच्छ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_पङ्क्तिः(xe, xecfg, रिक्त, i1, -1);
			याव (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			सचे (fs1 < 0)
				fs1 = 0;
			सचे (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				याव (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				सचे (xchp != xch) {
					xch = xchp;
					गच्छ pre_context_calculation;
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

#सचे 0
		सचे (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_पङ्क्तिः(xe, xecfg, रिक्त,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			याव (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			सचे (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			सचे (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			सचे (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				सचे (l - xecfg->ctxlen <= e1 ||
				    get_func_पङ्क्तिः(xe, xecfg, रिक्त, l, e1) < 0) {
					xche = xche->next;
					गच्छ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#सचे 0
		सचे (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_पङ्क्तिः(xe, xecfg, &func_पङ्क्तिः,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		सचे (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_पङ्क्तिः.buf, func_पङ्क्तिः.len, ecb) < 0)
			पटिनिवत्त -1;

		/*
		 * Emit pre-context.
		 */
		अत्थाय (; s2 < xch->i2; s2++)
			सचे (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				पटिनिवत्त -1;

		अत्थाय (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			अत्थाय (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				सचे (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					पटिनिवत्त -1;

			/*
			 * Removes lines from the first file.
			 */
			अत्थाय (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				सचे (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					पटिनिवत्त -1;

			/*
			 * Adds lines from the second file.
			 */
			अत्थाय (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				सचे (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					पटिनिवत्त -1;

			सचे (xch == xche)
				भङ्ग;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		अत्थाय (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			सचे (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				पटिनिवत्त -1;
	}

	पटिनिवत्त 0;
}
