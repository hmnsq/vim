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

	यदि (xdl_emit_diffrec(rec->ptr, rec->size, pre, (long)strlen(pre), ecb) < 0)
		प्रत्यागम -1;

	प्रत्यागम 0;
}

#define signed_add_overflows(a, b) \
    ((b) > LONG_MAX - (a))

स्थिर long saturating_add(long a, long b)
{
	प्रत्यागम signed_add_overflows(a, b) ? LONG_MAX : a + b;
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
	प्रति (xchp = *xscr; xchp && xchp->ignore; xchp = xchp->next) {
		xch = xchp->next;

		यदि (xch == रिक्त ||
		    xch->i1 - (xchp->i1 + xchp->chg1) >= max_ignorable)
			*xscr = xch;
	}

	यदि (!*xscr)
		प्रत्यागम रिक्त;

	lxch = *xscr;

	प्रति (xchp = *xscr, xch = xchp->next; xch; xchp = xch, xch = xch->next) {
		long distance = xch->i1 - (xchp->i1 + xchp->chg1);
		यदि (distance > max_common)
			भङ्ग;

		यदि (distance < max_ignorable && (!xch->ignore || lxch == xchp)) {
			lxch = xch;
			ignored = 0;
		} अन्यथा यदि (distance < max_ignorable && xch->ignore) {
			ignored += xch->chg2;
		} अन्यथा यदि (lxch != xchp &&
			   xch->i1 + ignored - (lxch->i1 + lxch->chg1) > max_common) {
			भङ्ग;
		} अन्यथा यदि (!xch->ignore) {
			lxch = xch;
			ignored = 0;
		} अन्यथा {
			ignored += xch->chg2;
		}
	}

	प्रत्यागम lxch;
}


#यदि 0
स्थिर long def_ff(ध्रुव वर्ण *rec, long len, वर्ण *buf, long sz)
{
	यदि (len > 0 &&
			(isalpha((unsigned वर्ण)*rec) || /* identifier? */
			 *rec == '_' || /* also identifier? */
			 *rec == '$')) { /* identifiers from VMS and other esoterico */
		यदि (len > sz)
			len = sz;
		यावत् (0 < len && isspace((unsigned वर्ण)rec[len - 1]))
			len--;
		memcpy(buf, rec, len);
		प्रत्यागम len;
	}
	प्रत्यागम -1;
}
#endif

#यदि 0
स्थिर long match_func_rec(xdfile_t *xdf, xdemitconf_t ध्रुव *xecfg, long ri,
			   वर्ण *buf, long sz)
{
	xrecord_t *rec = &xdf->recs[ri];

	यदि (!xecfg->find_func)
		प्रत्यागम def_ff(rec->ptr, rec->size, buf, sz);
	प्रत्यागम xecfg->find_func(rec->ptr, rec->size, buf, sz, xecfg->find_func_priv);
}
#endif

#यदि 0
स्थिर पूर्णाङ्क is_func_rec(xdfile_t *xdf, xdemitconf_t ध्रुव *xecfg, long ri)
{
	वर्ण dummy[1];
	प्रत्यागम match_func_rec(xdf, xecfg, ri, dummy, परिमाण(dummy)) >= 0;
}
#endif

संरचना func_पंक्ति {
	long len;
	वर्ण buf[80];
};

#यदि 0
स्थिर long get_func_पंक्ति(xdfenv_t *xe, xdemitconf_t ध्रुव *xecfg,
			  संरचना func_पंक्ति *func_पंक्ति, long start, long limit)
{
	long l, size, step = (start > limit) ? -1 : 1;
	वर्ण *buf, dummy[1];

	buf = func_पंक्ति ? func_पंक्ति->buf : dummy;
	size = func_पंक्ति ? परिमाण(func_पंक्ति->buf) : परिमाण(dummy);

	प्रति (l = start; l != limit && 0 <= l && l < xe->xdf1.nrec; l += step) {
		long len = match_func_rec(&xe->xdf1, xecfg, l, buf, size);
		यदि (len >= 0) {
			यदि (func_पंक्ति)
				func_पंक्ति->len = len;
			प्रत्यागम l;
		}
	}
	प्रत्यागम -1;
}
#endif

#यदि 0
स्थिर पूर्णाङ्क is_empty_rec(xdfile_t *xdf, long ri)
{
	xrecord_t *rec = &xdf->recs[ri];
	long i = 0;

	प्रति (; i < rec->size && XDL_ISSPACE(rec->ptr[i]); i++);

	प्रत्यागम i == rec->size;
}
#endif

पूर्णाङ्क xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ध्रुव *xecfg) {
	long s1, s2, e1, e2, lctx;
	xdchange_t *xch, *xche;
#यदि 0
	long funclineprev = -1;
#endif
	संरचना func_पंक्ति func_पंक्ति;

	func_पंक्ति.len = 0;

	प्रति (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		यदि (!xch)
			भङ्ग;

		s1 = XDL_MAX(xch->i1 - xecfg->ctxlen, 0);
		s2 = XDL_MAX(xch->i2 - xecfg->ctxlen, 0);

#यदि 0
		यदि (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fs1, i1 = xch->i1;

			/* Appended chunk? */
			यदि (i1 >= xe->xdf1.nrec) {
				long i2 = xch->i2;

				/*
				 * We don't need additional context if
				 * a whole function was added.
				 */
				यावत् (i2 < xe->xdf2.nrec) {
					यदि (is_func_rec(&xe->xdf2, xecfg, i2))
						गच्छ post_context_calculation;
					i2++;
				}

				/*
				 * Otherwise get more context from the
				 * pre-image.
				 */
				i1 = xe->xdf1.nrec - 1;
			}

			fs1 = get_func_पंक्ति(xe, xecfg, रिक्त, i1, -1);
			यावत् (fs1 > 0 && !is_empty_rec(&xe->xdf1, fs1 - 1) &&
			       !is_func_rec(&xe->xdf1, xecfg, fs1 - 1))
				fs1--;
			यदि (fs1 < 0)
				fs1 = 0;
			यदि (fs1 < s1) {
				s2 = XDL_MAX(s2 - (s1 - fs1), 0);
				s1 = fs1;

				/*
				 * Did we extend context upwards into an
				 * ignored change?
				 */
				यावत् (xchp != xch &&
				       xchp->i1 + xchp->chg1 <= s1 &&
				       xchp->i2 + xchp->chg2 <= s2)
					xchp = xchp->next;

				/* If so, show it after all. */
				यदि (xchp != xch) {
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

#यदि 0
		यदि (xecfg->flags & XDL_EMIT_FUNCCONTEXT) {
			long fe1 = get_func_पंक्ति(xe, xecfg, रिक्त,
						 xche->i1 + xche->chg1,
						 xe->xdf1.nrec);
			यावत् (fe1 > 0 && is_empty_rec(&xe->xdf1, fe1 - 1))
				fe1--;
			यदि (fe1 < 0)
				fe1 = xe->xdf1.nrec;
			यदि (fe1 > e1) {
				e2 = XDL_MIN(e2 + (fe1 - e1), xe->xdf2.nrec);
				e1 = fe1;
			}

			/*
			 * Overlap with next change?  Then include it
			 * in the current hunk and start over to find
			 * its new end.
			 */
			यदि (xche->next) {
				long l = XDL_MIN(xche->next->i1,
						 xe->xdf1.nrec - 1);
				यदि (l - xecfg->ctxlen <= e1 ||
				    get_func_पंक्ति(xe, xecfg, रिक्त, l, e1) < 0) {
					xche = xche->next;
					गच्छ post_context_calculation;
				}
			}
		}
#endif

		/*
		 * Emit current hunk header.
		 */

#यदि 0
		यदि (xecfg->flags & XDL_EMIT_FUNCNAMES) {
			get_func_पंक्ति(xe, xecfg, &func_पंक्ति,
				      s1 - 1, funclineprev);
			funclineprev = s1 - 1;
		}
#endif
		यदि (!(xecfg->flags & XDL_EMIT_NO_HUNK_HDR) &&
		    xdl_emit_hunk_hdr(s1 + 1, e1 - s1, s2 + 1, e2 - s2,
				      func_पंक्ति.buf, func_पंक्ति.len, ecb) < 0)
			प्रत्यागम -1;

		/*
		 * Emit pre-context.
		 */
		प्रति (; s2 < xch->i2; s2++)
			यदि (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				प्रत्यागम -1;

		प्रति (s1 = xch->i1, s2 = xch->i2;; xch = xch->next) {
			/*
			 * Merge previous with current change atom.
			 */
			प्रति (; s1 < xch->i1 && s2 < xch->i2; s1++, s2++)
				यदि (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
					प्रत्यागम -1;

			/*
			 * Removes lines from the first file.
			 */
			प्रति (s1 = xch->i1; s1 < xch->i1 + xch->chg1; s1++)
				यदि (xdl_emit_record(&xe->xdf1, s1, "-", ecb) < 0)
					प्रत्यागम -1;

			/*
			 * Adds lines from the second file.
			 */
			प्रति (s2 = xch->i2; s2 < xch->i2 + xch->chg2; s2++)
				यदि (xdl_emit_record(&xe->xdf2, s2, "+", ecb) < 0)
					प्रत्यागम -1;

			यदि (xch == xche)
				भङ्ग;
			s1 = xch->i1 + xch->chg1;
			s2 = xch->i2 + xch->chg2;
		}

		/*
		 * Emit post-context.
		 */
		प्रति (s2 = xche->i2 + xche->chg2; s2 < e2; s2++)
			यदि (xdl_emit_record(&xe->xdf2, s2, " ", ecb) < 0)
				प्रत्यागम -1;
	}

	प्रत्यागम 0;
}
