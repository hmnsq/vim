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

#ಅನ್ತರ್ಭಾವ "xinclude.h"


#ವ್ಯಾಖ್ಯಾನಿಸು XDL_KPDIS_ಸತತ 4
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_MAX_EQLIMIT 1024
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_SIMSCAN_ಕಿಟಕಿ 100
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_GUESS_NLINES1 256
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_GUESS_NLINES2 20

#ವ್ಯಾಖ್ಯಾನಿಸು DISCARD 0
#ವ್ಯಾಖ್ಯಾನಿಸು KEEP 1
#ವ್ಯಾಖ್ಯಾನಿಸು INVESTIGATE 2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdlclass {
	ರಚನೆ s_xdlclass *ಮುಂದಿನ;
	xrecord_t rec;
	ದೀರ್ಘ idx;
	ದೀರ್ಘ len1, len2;
} xdlclass_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdlclassifier {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hbits;
	ದೀರ್ಘ hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	ದೀರ್ಘ alloc;
	ದೀರ್ಘ ಎಣಿಕೆ_2;
	ದೀರ್ಘ flags;
} xdlclassifier_t;




ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_init_classifier(xdlclassifier_t *cf, ದೀರ್ಘ ಗಾತ್ರ_2, ದೀರ್ಘ flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ_2);
	cf->hsize = 1 << cf->hbits;

	ಆದರೆ (xdl_cha_init(&cf->ncha, ಗಾತ್ರ(xdlclass_t), ಗಾತ್ರ_2 / 4 + 1) < 0) {

		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		ಹಿಂತಿರುಗಿ -1;
	}

	cf->alloc = ಗಾತ್ರ_2;
	ಆದರೆ (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		ಹಿಂತಿರುಗಿ -1;
	}

	cf->ಎಣಿಕೆ_2 = 0;

	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಶೂನ್ಯ xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_classify_ದಾಖಲೆ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pass, xdlclassifier_t *cf, xrecord_t *rec) {
	ದೀರ್ಘ hi;
	xdlclass_t *rcrec;

	hi = (ದೀರ್ಘ) XDL_HASHLONG(rec->ha, cf->hbits);
	ಗಾಗಿ (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->ಮುಂದಿನ)
		ಆದರೆ (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.ಗಾತ್ರ_2,
					rec->ptr, rec->ಗಾತ್ರ_2, cf->flags))
			ನಿಲ್ಲಿಸು;

	ಆದರೆ (!rcrec) {
		ಆದರೆ (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			ಹಿಂತಿರುಗಿ -1;
		}
		rcrec->idx = cf->ಎಣಿಕೆ_2++;
		ಆದರೆ (XDL_ALLOC_GROW(cf->rcrecs, cf->ಎಣಿಕೆ_2, cf->alloc))
				ಹಿಂತಿರುಗಿ -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->ಮುಂದಿನ = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) rcrec->idx;

	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಶೂನ್ಯ xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->ಬದಲಾಯಿತು - 1);
	xdl_free(xdf->recs);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_prepare_ctx(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pass, mmfile_t *mf, ದೀರ್ಘ narec, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	ದೀರ್ಘ bsize;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hav;
	ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *blk, *cur, *ತುದಿ, *prev;
	xrecord_t *crec;

	xdf->rindex = ಖಾಲಿ;
	xdf->ಬದಲಾಯಿತು = ಖಾಲಿ;
	xdf->recs = ಖಾಲಿ;

	ಆದರೆ (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		ಹೋಗು abort;

	xdf->nrec = 0;
	ಆದರೆ ((cur = blk = xdl_mmfile_ಮೊದಲ(mf, &bsize))) {
		ಗಾಗಿ (ತುದಿ = blk + bsize; cur < ತುದಿ; ) {
			prev = cur;
			hav = xdl_hash_ದಾಖಲೆ(&cur, ತುದಿ, xpp->flags);
			ಆದರೆ (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				ಹೋಗು abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->ಗಾತ್ರ_2 = (ದೀರ್ಘ) (cur - prev);
			crec->ha = hav;
			ಆದರೆ (xdl_classify_ದಾಖಲೆ(pass, cf, crec) < 0)
				ಹೋಗು abort;
		}
	}

	ಆದರೆ (!XDL_CALLOC_ARRAY(xdf->ಬದಲಾಯಿತು, xdf->nrec + 2))
		ಹೋಗು abort;

	ಆದರೆ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		ಆದರೆ (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			ಹೋಗು abort;
	}

	xdf->ಬದಲಾಯಿತು += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	ಹಿಂತಿರುಗಿ 0;

abort:
	xdl_free_ctx(xdf);
	ಹಿಂತಿರುಗಿ -1;
}


ಶೂನ್ಯ xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


ಸ್ಥಿರ bool xdl_clean_mmatch(uint8_t ಸ್ಥಿರಾಂಕ *action, ದೀರ್ಘ i, ದೀರ್ಘ s, ದೀರ್ಘ e) {
	ದೀರ್ಘ r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	ಆದರೆ (i - s > XDL_SIMSCAN_ಕಿಟಕಿ)
		s = i - XDL_SIMSCAN_ಕಿಟಕಿ;
	ಆದರೆ (e - i > XDL_SIMSCAN_ಕಿಟಕಿ)
		e = i + XDL_SIMSCAN_ಕಿಟಕಿ;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	ಗಾಗಿ (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		ಆದರೆ (action[i - r] == DISCARD)
			rdis0++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action[i - r] == INVESTIGATE)
			rpdis0++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action[i - r] == KEEP)
			ನಿಲ್ಲಿಸು;
		ಇಲ್ಲದಿದ್ದರೆ
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	ಆದರೆ (rdis0 == 0)
		ಹಿಂತಿರುಗಿ 0;
	ಗಾಗಿ (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		ಆದರೆ (action[i + r] == DISCARD)
			rdis1++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action[i + r] == INVESTIGATE)
			rpdis1++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action[i + r] == KEEP)
			ನಿಲ್ಲಿಸು;
		ಇಲ್ಲದಿದ್ದರೆ
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	ಆದರೆ (rdis1 == 0)
		ಹಿಂತಿರುಗಿ false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	ಹಿಂತಿರುಗಿ rpdis1 * XDL_KPDIS_ಸತತ < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_cleanup_ದಾಖಲೆಗಳು(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	ದೀರ್ಘ i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = ಖಾಲಿ, *action2 = ಖಾಲಿ;
	bool need_min = !!(cf->flags & XDF_NEED_ಕನಿಷ್ಠತಮ);
	ಪೂರ್ಣಾಂಕ ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	ಆದರೆ (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		ಹೋಗು cleanup;
	}
	ಆದರೆ (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		ಹೋಗು cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	ಆದರೆ ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	ಗಾಗಿ (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	ಆದರೆ ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	ಗಾಗಿ (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	ಗಾಗಿ (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		ಆದರೆ (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ಇಲ್ಲದಿದ್ದರೆ
			xdf1->ಬದಲಾಯಿತು[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	ಗಾಗಿ (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		ಆದರೆ (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ಇಲ್ಲದಿದ್ದರೆ
			xdf2->ಬದಲಾಯಿತು[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	ಹಿಂತಿರುಗಿ ret;
}


/*
 * Early trim initial and terminal matching records.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_trim_ತುದಿಗಳು(xdfile_t *xdf1, xdfile_t *xdf2) {
	ದೀರ್ಘ i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	ಗಾಗಿ (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		ಆದರೆ (recs1->ha != recs2->ha)
			ನಿಲ್ಲಿಸು;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	ಗಾಗಿ (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		ಆದರೆ (recs1->ha != recs2->ha)
			ನಿಲ್ಲಿಸು;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	ಆದರೆ (xdl_trim_ತುದಿಗಳು(xdf1, xdf2) < 0 ||
	    xdl_cleanup_ದಾಖಲೆಗಳು(cf, xdf1, xdf2) < 0) {

		ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
		    xdfenv_t *xe) {
	ದೀರ್ಘ enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, ಗಾತ್ರ(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_ಸಾಲುಗಳು(mf1, sample) + 1;
	enl2 = xdl_guess_ಸಾಲುಗಳು(mf2, sample) + 1;

	ಆದರೆ (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ಹಿಂತಿರುಗಿ -1;
	}

	xdl_free_classifier(&cf);

	ಹಿಂತಿರುಗಿ 0;
}
