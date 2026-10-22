/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003  Davide Libenzi
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


#define XDL_KPDIS_RUN 4
#define XDL_MAX_EQLIMIT 1024
#define XDL_SIMSCAN_WINDOW 100
#define XDL_GUESS_NLINES1 256
#define XDL_GUESS_NLINES2 20

#define DISCARD 0
#define KEEP 1
#define INVESTIGATE 2

प्रकारव्याख्या संरचना s_xdlclass {
	संरचना s_xdlclass *next;
	xrecord_t rec;
	दीर्घ idx;
	दीर्घ len1, len2;
} xdlclass_t;

प्रकारव्याख्या संरचना s_xdlclassifier {
	अचिन्हित पूर्णांक hbits;
	दीर्घ hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	दीर्घ alloc;
	दीर्घ मोजणी;
	दीर्घ flags;
} xdlclassifier_t;




स्थिर पूर्णांक xdl_init_classifier(xdlclassifier_t *cf, दीर्घ size, दीर्घ flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((अचिन्हित पूर्णांक) size);
	cf->hsize = 1 << cf->hbits;

	जर (xdl_cha_init(&cf->ncha, आकार(xdlclass_t), size / 4 + 1) < 0) {

		परत -1;
	}
	जर (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		परत -1;
	}

	cf->alloc = size;
	जर (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		परत -1;
	}

	cf->मोजणी = 0;

	परत 0;
}


स्थिर शून्य xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


स्थिर पूर्णांक xdl_classify_record(अचिन्हित पूर्णांक pass, xdlclassifier_t *cf, xrecord_t *rec) {
	दीर्घ hi;
	xdlclass_t *rcrec;

	hi = (दीर्घ) XDL_HASHLONG(rec->ha, cf->hbits);
	साठी (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->next)
		जर (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.size,
					rec->ptr, rec->size, cf->flags))
			थांबा;

	जर (!rcrec) {
		जर (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			परत -1;
		}
		rcrec->idx = cf->मोजणी++;
		जर (XDL_ALLOC_GROW(cf->rcrecs, cf->मोजणी, cf->alloc))
				परत -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->next = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (अचिन्हित दीर्घ) rcrec->idx;

	परत 0;
}


स्थिर शून्य xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->changed - 1);
	xdl_free(xdf->recs);
}


स्थिर पूर्णांक xdl_prepare_ctx(अचिन्हित पूर्णांक pass, mmfile_t *mf, दीर्घ narec, xpparam_t स्थिरांक *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	दीर्घ bsize;
	अचिन्हित दीर्घ hav;
	अक्षर स्थिरांक *blk, *cur, *शीर्ष, *prev;
	xrecord_t *crec;

	xdf->rindex = रिक्त;
	xdf->changed = रिक्त;
	xdf->recs = रिक्त;

	जर (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		जा पात;

	xdf->nrec = 0;
	जर ((cur = blk = xdl_mmfile_first(mf, &bsize))) {
		साठी (शीर्ष = blk + bsize; cur < शीर्ष; ) {
			prev = cur;
			hav = xdl_hash_record(&cur, शीर्ष, xpp->flags);
			जर (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				जा पात;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->size = (दीर्घ) (cur - prev);
			crec->ha = hav;
			जर (xdl_classify_record(pass, cf, crec) < 0)
				जा पात;
		}
	}

	जर (!XDL_CALLOC_ARRAY(xdf->changed, xdf->nrec + 2))
		जा पात;

	जर ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		जर (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			जा पात;
	}

	xdf->changed += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	परत 0;

पात:
	xdl_free_ctx(xdf);
	परत -1;
}


शून्य xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


स्थिर bool xdl_clean_mmatch(uint8_t स्थिरांक *action, दीर्घ i, दीर्घ s, दीर्घ e) {
	दीर्घ r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	जर (i - s > XDL_SIMSCAN_WINDOW)
		s = i - XDL_SIMSCAN_WINDOW;
	जर (e - i > XDL_SIMSCAN_WINDOW)
		e = i + XDL_SIMSCAN_WINDOW;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	साठी (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		जर (action[i - r] == DISCARD)
			rdis0++;
		अन्यथा जर (action[i - r] == INVESTIGATE)
			rpdis0++;
		अन्यथा जर (action[i - r] == KEEP)
			थांबा;
		अन्यथा
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	जर (rdis0 == 0)
		परत 0;
	साठी (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		जर (action[i + r] == DISCARD)
			rdis1++;
		अन्यथा जर (action[i + r] == INVESTIGATE)
			rpdis1++;
		अन्यथा जर (action[i + r] == KEEP)
			थांबा;
		अन्यथा
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	जर (rdis1 == 0)
		परत false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	परत rpdis1 * XDL_KPDIS_RUN < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
स्थिर पूर्णांक xdl_cleanup_records(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	दीर्घ i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = रिक्त, *action2 = रिक्त;
	bool need_min = !!(cf->flags & XDF_NEED_MINIMAL);
	पूर्णांक ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	जर (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		जा cleanup;
	}
	जर (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		जा cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	जर ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	साठी (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	जर ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	साठी (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	साठी (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		जर (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} अन्यथा
			xdf1->changed[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	साठी (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		जर (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} अन्यथा
			xdf2->changed[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	परत ret;
}


/*
 * Early trim initial and terminal matching records.
 */
स्थिर पूर्णांक xdl_trim_ends(xdfile_t *xdf1, xdfile_t *xdf2) {
	दीर्घ i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	साठी (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		जर (recs1->ha != recs2->ha)
			थांबा;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	साठी (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		जर (recs1->ha != recs2->ha)
			थांबा;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	परत 0;
}


स्थिर पूर्णांक xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	जर (xdl_trim_ends(xdf1, xdf2) < 0 ||
	    xdl_cleanup_records(cf, xdf1, xdf2) < 0) {

		परत -1;
	}

	परत 0;
}

पूर्णांक xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t स्थिरांक *xpp,
		    xdfenv_t *xe) {
	दीर्घ enl1, enl2, sample;
	xdlclassifier_t cf;

	स_रखो(&cf, 0, आकार(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_ओळी(mf1, sample) + 1;
	enl2 = xdl_guess_ओळी(mf2, sample) + 1;

	जर (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		परत -1;

	जर (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		परत -1;
	}
	जर (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		परत -1;
	}

	जर ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		परत -1;
	}

	xdl_free_classifier(&cf);

	परत 0;
}
