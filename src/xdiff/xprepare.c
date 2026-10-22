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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_KPDIS_ਲੜੀਵਾਰ 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_MAX_EQLIMIT 1024
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_SIMSCAN_ਖਿੜਕੀ 100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_GUESS_NLINES1 256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_GUESS_NLINES2 20

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DISCARD 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEEP 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVESTIGATE 2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdlclass {
	ਬਣਤਰ s_xdlclass *ਅਗਲਾ;
	xrecord_t rec;
	ਲੰਮਾ idx;
	ਲੰਮਾ len1, len2;
} xdlclass_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdlclassifier {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hbits;
	ਲੰਮਾ hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	ਲੰਮਾ alloc;
	ਲੰਮਾ ਗਿਣਤੀ_2;
	ਲੰਮਾ flags;
} xdlclassifier_t;




ਸਥਿਰ ਪੂਰਨ xdl_init_classifier(xdlclassifier_t *cf, ਲੰਮਾ ਮਾਪ, ਲੰਮਾ flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) ਮਾਪ);
	cf->hsize = 1 << cf->hbits;

	ਜੇ (xdl_cha_init(&cf->ncha, ਆਕਾਰ(xdlclass_t), ਮਾਪ / 4 + 1) < 0) {

		ਵਾਪਸ -1;
	}
	ਜੇ (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		ਵਾਪਸ -1;
	}

	cf->alloc = ਮਾਪ;
	ਜੇ (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		ਵਾਪਸ -1;
	}

	cf->ਗਿਣਤੀ_2 = 0;

	ਵਾਪਸ 0;
}


ਸਥਿਰ ਖਾਲੀ xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


ਸਥਿਰ ਪੂਰਨ xdl_classify_ਰਿਕਾਰਡ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pass, xdlclassifier_t *cf, xrecord_t *rec) {
	ਲੰਮਾ hi;
	xdlclass_t *rcrec;

	hi = (ਲੰਮਾ) XDL_HASHLONG(rec->ha, cf->hbits);
	ਲਈ (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->ਅਗਲਾ)
		ਜੇ (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.ਮਾਪ,
					rec->ptr, rec->ਮਾਪ, cf->flags))
			ਰੋਕੋ;

	ਜੇ (!rcrec) {
		ਜੇ (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			ਵਾਪਸ -1;
		}
		rcrec->idx = cf->ਗਿਣਤੀ_2++;
		ਜੇ (XDL_ALLOC_GROW(cf->rcrecs, cf->ਗਿਣਤੀ_2, cf->alloc))
				ਵਾਪਸ -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->ਅਗਲਾ = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) rcrec->idx;

	ਵਾਪਸ 0;
}


ਸਥਿਰ ਖਾਲੀ xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->ਬਦਲਿਆ - 1);
	xdl_free(xdf->recs);
}


ਸਥਿਰ ਪੂਰਨ xdl_prepare_ctx(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pass, mmfile_t *mf, ਲੰਮਾ narec, xpparam_t ਅਚਲ *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	ਲੰਮਾ bsize;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hav;
	ਅੱਖਰ ਅਚਲ *blk, *cur, *ਸਿਖਰ, *prev;
	xrecord_t *crec;

	xdf->rindex = ਨਿਰਾ;
	xdf->ਬਦਲਿਆ = ਨਿਰਾ;
	xdf->recs = ਨਿਰਾ;

	ਜੇ (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		ਜਾਓ abort;

	xdf->nrec = 0;
	ਜੇ ((cur = blk = xdl_mmfile_ਪਹਿਲਾ(mf, &bsize))) {
		ਲਈ (ਸਿਖਰ = blk + bsize; cur < ਸਿਖਰ; ) {
			prev = cur;
			hav = xdl_hash_ਰਿਕਾਰਡ(&cur, ਸਿਖਰ, xpp->flags);
			ਜੇ (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				ਜਾਓ abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->ਮਾਪ = (ਲੰਮਾ) (cur - prev);
			crec->ha = hav;
			ਜੇ (xdl_classify_ਰਿਕਾਰਡ(pass, cf, crec) < 0)
				ਜਾਓ abort;
		}
	}

	ਜੇ (!XDL_CALLOC_ARRAY(xdf->ਬਦਲਿਆ, xdf->nrec + 2))
		ਜਾਓ abort;

	ਜੇ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		ਜੇ (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			ਜਾਓ abort;
	}

	xdf->ਬਦਲਿਆ += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	ਵਾਪਸ 0;

abort:
	xdl_free_ctx(xdf);
	ਵਾਪਸ -1;
}


ਖਾਲੀ xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


ਸਥਿਰ bool xdl_clean_mmatch(uint8_t ਅਚਲ *action, ਲੰਮਾ i, ਲੰਮਾ s, ਲੰਮਾ e) {
	ਲੰਮਾ r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	ਜੇ (i - s > XDL_SIMSCAN_ਖਿੜਕੀ)
		s = i - XDL_SIMSCAN_ਖਿੜਕੀ;
	ਜੇ (e - i > XDL_SIMSCAN_ਖਿੜਕੀ)
		e = i + XDL_SIMSCAN_ਖਿੜਕੀ;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	ਲਈ (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		ਜੇ (action[i - r] == DISCARD)
			rdis0++;
		ਨਹੀਂਤਾਂ ਜੇ (action[i - r] == INVESTIGATE)
			rpdis0++;
		ਨਹੀਂਤਾਂ ਜੇ (action[i - r] == KEEP)
			ਰੋਕੋ;
		ਨਹੀਂਤਾਂ
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	ਜੇ (rdis0 == 0)
		ਵਾਪਸ 0;
	ਲਈ (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		ਜੇ (action[i + r] == DISCARD)
			rdis1++;
		ਨਹੀਂਤਾਂ ਜੇ (action[i + r] == INVESTIGATE)
			rpdis1++;
		ਨਹੀਂਤਾਂ ਜੇ (action[i + r] == KEEP)
			ਰੋਕੋ;
		ਨਹੀਂਤਾਂ
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	ਜੇ (rdis1 == 0)
		ਵਾਪਸ false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	ਵਾਪਸ rpdis1 * XDL_KPDIS_ਲੜੀਵਾਰ < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
ਸਥਿਰ ਪੂਰਨ xdl_cleanup_ਰਿਕਾਰਡਾਂ(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	ਲੰਮਾ i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = ਨਿਰਾ, *action2 = ਨਿਰਾ;
	bool need_min = !!(cf->flags & XDF_NEED_ਨਿਊਨਤਮ);
	ਪੂਰਨ ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	ਜੇ (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		ਜਾਓ cleanup;
	}
	ਜੇ (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		ਜਾਓ cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	ਜੇ ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	ਲਈ (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	ਜੇ ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	ਲਈ (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	ਲਈ (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		ਜੇ (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ਨਹੀਂਤਾਂ
			xdf1->ਬਦਲਿਆ[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	ਲਈ (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		ਜੇ (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ਨਹੀਂਤਾਂ
			xdf2->ਬਦਲਿਆ[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	ਵਾਪਸ ret;
}


/*
 * Early trim initial and terminal matching records.
 */
ਸਥਿਰ ਪੂਰਨ xdl_trim_ਸਿਰੇ(xdfile_t *xdf1, xdfile_t *xdf2) {
	ਲੰਮਾ i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	ਲਈ (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		ਜੇ (recs1->ha != recs2->ha)
			ਰੋਕੋ;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	ਲਈ (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		ਜੇ (recs1->ha != recs2->ha)
			ਰੋਕੋ;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	ਵਾਪਸ 0;
}


ਸਥਿਰ ਪੂਰਨ xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	ਜੇ (xdl_trim_ਸਿਰੇ(xdf1, xdf2) < 0 ||
	    xdl_cleanup_ਰਿਕਾਰਡਾਂ(cf, xdf1, xdf2) < 0) {

		ਵਾਪਸ -1;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ਅਚਲ *xpp,
		    xdfenv_t *xe) {
	ਲੰਮਾ enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, ਆਕਾਰ(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_ਸਤਰਾਂ(mf1, sample) + 1;
	enl2 = xdl_guess_ਸਤਰਾਂ(mf2, sample) + 1;

	ਜੇ (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		ਵਾਪਸ -1;

	ਜੇ (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		ਵਾਪਸ -1;
	}
	ਜੇ (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ਵਾਪਸ -1;
	}

	ਜੇ ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ਵਾਪਸ -1;
	}

	xdl_free_classifier(&cf);

	ਵਾਪਸ 0;
}
