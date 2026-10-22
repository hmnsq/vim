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

#అన్తర్భావ "xinclude.h"


#నిర్వచించు XDL_KPDIS_నిరంతరం 4
#నిర్వచించు XDL_MAX_EQLIMIT 1024
#నిర్వచించు XDL_SIMSCAN_కిటికీ 100
#నిర్వచించు XDL_GUESS_NLINES1 256
#నిర్వచించు XDL_GUESS_NLINES2 20

#నిర్వచించు DISCARD 0
#నిర్వచించు KEEP 1
#నిర్వచించు INVESTIGATE 2

రకనిర్వచనం నిర్మాణం s_xdlclass {
	నిర్మాణం s_xdlclass *తదుపరి;
	xrecord_t rec;
	దీర్ఘ idx;
	దీర్ఘ len1, len2;
} xdlclass_t;

రకనిర్వచనం నిర్మాణం s_xdlclassifier {
	గుర్తులేని పూర్ణాంకం hbits;
	దీర్ఘ hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	దీర్ఘ alloc;
	దీర్ఘ లెక్క;
	దీర్ఘ flags;
} xdlclassifier_t;




స్థిర పూర్ణాంకం xdl_init_classifier(xdlclassifier_t *cf, దీర్ఘ కొలత, దీర్ఘ flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((గుర్తులేని పూర్ణాంకం) కొలత);
	cf->hsize = 1 << cf->hbits;

	ఒకవేళ (xdl_cha_init(&cf->ncha, పరిమాణం(xdlclass_t), కొలత / 4 + 1) < 0) {

		తిరిగి -1;
	}
	ఒకవేళ (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		తిరిగి -1;
	}

	cf->alloc = కొలత;
	ఒకవేళ (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		తిరిగి -1;
	}

	cf->లెక్క = 0;

	తిరిగి 0;
}


స్థిర శూన్య xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


స్థిర పూర్ణాంకం xdl_classify_రికార్డు(గుర్తులేని పూర్ణాంకం pass, xdlclassifier_t *cf, xrecord_t *rec) {
	దీర్ఘ hi;
	xdlclass_t *rcrec;

	hi = (దీర్ఘ) XDL_HASHLONG(rec->ha, cf->hbits);
	కొరకు (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->తదుపరి)
		ఒకవేళ (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.కొలత,
					rec->ptr, rec->కొలత, cf->flags))
			ఆపు;

	ఒకవేళ (!rcrec) {
		ఒకవేళ (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			తిరిగి -1;
		}
		rcrec->idx = cf->లెక్క++;
		ఒకవేళ (XDL_ALLOC_GROW(cf->rcrecs, cf->లెక్క, cf->alloc))
				తిరిగి -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->తదుపరి = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (గుర్తులేని దీర్ఘ) rcrec->idx;

	తిరిగి 0;
}


స్థిర శూన్య xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->మారింది - 1);
	xdl_free(xdf->recs);
}


స్థిర పూర్ణాంకం xdl_prepare_ctx(గుర్తులేని పూర్ణాంకం pass, mmfile_t *mf, దీర్ఘ narec, xpparam_t స్థిరరాశి *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	దీర్ఘ bsize;
	గుర్తులేని దీర్ఘ hav;
	అక్షరం స్థిరరాశి *blk, *cur, *శిఖరం, *prev;
	xrecord_t *crec;

	xdf->rindex = ఖాళీ;
	xdf->మారింది = ఖాళీ;
	xdf->recs = ఖాళీ;

	ఒకవేళ (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		వెళ్ళు abort;

	xdf->nrec = 0;
	ఒకవేళ ((cur = blk = xdl_mmfile_మొదటి(mf, &bsize))) {
		కొరకు (శిఖరం = blk + bsize; cur < శిఖరం; ) {
			prev = cur;
			hav = xdl_hash_రికార్డు(&cur, శిఖరం, xpp->flags);
			ఒకవేళ (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				వెళ్ళు abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->కొలత = (దీర్ఘ) (cur - prev);
			crec->ha = hav;
			ఒకవేళ (xdl_classify_రికార్డు(pass, cf, crec) < 0)
				వెళ్ళు abort;
		}
	}

	ఒకవేళ (!XDL_CALLOC_ARRAY(xdf->మారింది, xdf->nrec + 2))
		వెళ్ళు abort;

	ఒకవేళ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		ఒకవేళ (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			వెళ్ళు abort;
	}

	xdf->మారింది += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	తిరిగి 0;

abort:
	xdl_free_ctx(xdf);
	తిరిగి -1;
}


శూన్య xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


స్థిర bool xdl_clean_mmatch(uint8_t స్థిరరాశి *action, దీర్ఘ i, దీర్ఘ s, దీర్ఘ e) {
	దీర్ఘ r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	ఒకవేళ (i - s > XDL_SIMSCAN_కిటికీ)
		s = i - XDL_SIMSCAN_కిటికీ;
	ఒకవేళ (e - i > XDL_SIMSCAN_కిటికీ)
		e = i + XDL_SIMSCAN_కిటికీ;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	కొరకు (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		ఒకవేళ (action[i - r] == DISCARD)
			rdis0++;
		లేకపోతే ఒకవేళ (action[i - r] == INVESTIGATE)
			rpdis0++;
		లేకపోతే ఒకవేళ (action[i - r] == KEEP)
			ఆపు;
		లేకపోతే
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	ఒకవేళ (rdis0 == 0)
		తిరిగి 0;
	కొరకు (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		ఒకవేళ (action[i + r] == DISCARD)
			rdis1++;
		లేకపోతే ఒకవేళ (action[i + r] == INVESTIGATE)
			rpdis1++;
		లేకపోతే ఒకవేళ (action[i + r] == KEEP)
			ఆపు;
		లేకపోతే
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	ఒకవేళ (rdis1 == 0)
		తిరిగి false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	తిరిగి rpdis1 * XDL_KPDIS_నిరంతరం < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
స్థిర పూర్ణాంకం xdl_cleanup_రికార్డులు(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	దీర్ఘ i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = ఖాళీ, *action2 = ఖాళీ;
	bool need_min = !!(cf->flags & XDF_NEED_కనీస);
	పూర్ణాంకం ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	ఒకవేళ (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		వెళ్ళు cleanup;
	}
	ఒకవేళ (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		వెళ్ళు cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	ఒకవేళ ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	కొరకు (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	ఒకవేళ ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	కొరకు (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	కొరకు (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		ఒకవేళ (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} లేకపోతే
			xdf1->మారింది[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	కొరకు (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		ఒకవేళ (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} లేకపోతే
			xdf2->మారింది[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	తిరిగి ret;
}


/*
 * Early trim initial and terminal matching records.
 */
స్థిర పూర్ణాంకం xdl_trim_చివర్లు(xdfile_t *xdf1, xdfile_t *xdf2) {
	దీర్ఘ i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	కొరకు (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		ఒకవేళ (recs1->ha != recs2->ha)
			ఆపు;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	కొరకు (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		ఒకవేళ (recs1->ha != recs2->ha)
			ఆపు;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	తిరిగి 0;
}


స్థిర పూర్ణాంకం xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	ఒకవేళ (xdl_trim_చివర్లు(xdf1, xdf2) < 0 ||
	    xdl_cleanup_రికార్డులు(cf, xdf1, xdf2) < 0) {

		తిరిగి -1;
	}

	తిరిగి 0;
}

పూర్ణాంకం xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t స్థిరరాశి *xpp,
		    xdfenv_t *xe) {
	దీర్ఘ enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, పరిమాణం(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_పంక్తులు(mf1, sample) + 1;
	enl2 = xdl_guess_పంక్తులు(mf2, sample) + 1;

	ఒకవేళ (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		తిరిగి -1;

	ఒకవేళ (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		తిరిగి -1;
	}
	ఒకవేళ (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		తిరిగి -1;
	}

	ఒకవేళ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		తిరిగి -1;
	}

	xdl_free_classifier(&cf);

	తిరిగి 0;
}
