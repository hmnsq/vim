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

#ضمن "xinclude.h"


#وصف_ڪريو XDL_KPDIS_لاڳيتو 4
#وصف_ڪريو XDL_MAX_EQLIMIT 1024
#وصف_ڪريو XDL_SIMSCAN_دري 100
#وصف_ڪريو XDL_GUESS_NLINES1 256
#وصف_ڪريو XDL_GUESS_NLINES2 20

#وصف_ڪريو DISCARD 0
#وصف_ڪريو KEEP 1
#وصف_ڪريو INVESTIGATE 2

تعريفنوع بنية s_xdlclass {
	بنية s_xdlclass *ايندڙ;
	xrecord_t rec;
	ڊگهو idx;
	ڊگهو len1, len2;
} xdlclass_t;

تعريفنوع بنية s_xdlclassifier {
	بنانشان صحيح hbits;
	ڊگهو hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	ڊگهو alloc;
	ڊگهو ڳڻپ;
	ڊگهو flags;
} xdlclassifier_t;




ساكن صحيح xdl_init_classifier(xdlclassifier_t *cf, ڊگهو سائيز, ڊگهو flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((بنانشان صحيح) سائيز);
	cf->hsize = 1 << cf->hbits;

	إذا (xdl_cha_init(&cf->ncha, حجم(xdlclass_t), سائيز / 4 + 1) < 0) {

		أرجع -1;
	}
	إذا (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		أرجع -1;
	}

	cf->alloc = سائيز;
	إذا (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		أرجع -1;
	}

	cf->ڳڻپ = 0;

	أرجع 0;
}


ساكن فراغ xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


ساكن صحيح xdl_classify_رڪارڊ(بنانشان صحيح pass, xdlclassifier_t *cf, xrecord_t *rec) {
	ڊگهو hi;
	xdlclass_t *rcrec;

	hi = (ڊگهو) XDL_HASHLONG(rec->ha, cf->hbits);
	لكل (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->ايندڙ)
		إذا (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.سائيز,
					rec->ptr, rec->سائيز, cf->flags))
			اكسر;

	إذا (!rcrec) {
		إذا (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			أرجع -1;
		}
		rcrec->idx = cf->ڳڻپ++;
		إذا (XDL_ALLOC_GROW(cf->rcrecs, cf->ڳڻپ, cf->alloc))
				أرجع -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->ايندڙ = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (بنانشان ڊگهو) rcrec->idx;

	أرجع 0;
}


ساكن فراغ xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->بدليو - 1);
	xdl_free(xdf->recs);
}


ساكن صحيح xdl_prepare_ctx(بنانشان صحيح pass, mmfile_t *mf, ڊگهو narec, xpparam_t ثابت *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	ڊگهو bsize;
	بنانشان ڊگهو hav;
	حرف ثابت *blk, *cur, *چوٽي, *prev;
	xrecord_t *crec;

	xdf->rindex = عدم;
	xdf->بدليو = عدم;
	xdf->recs = عدم;

	إذا (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		اذهب abort;

	xdf->nrec = 0;
	إذا ((cur = blk = xdl_mmfile_پهريون(mf, &bsize))) {
		لكل (چوٽي = blk + bsize; cur < چوٽي; ) {
			prev = cur;
			hav = xdl_hash_رڪارڊ(&cur, چوٽي, xpp->flags);
			إذا (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				اذهب abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->سائيز = (ڊگهو) (cur - prev);
			crec->ha = hav;
			إذا (xdl_classify_رڪارڊ(pass, cf, crec) < 0)
				اذهب abort;
		}
	}

	إذا (!XDL_CALLOC_ARRAY(xdf->بدليو, xdf->nrec + 2))
		اذهب abort;

	إذا ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		إذا (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			اذهب abort;
	}

	xdf->بدليو += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	أرجع 0;

abort:
	xdl_free_ctx(xdf);
	أرجع -1;
}


فراغ xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


ساكن bool xdl_clean_mmatch(uint8_t ثابت *action, ڊگهو i, ڊگهو s, ڊگهو e) {
	ڊگهو r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	إذا (i - s > XDL_SIMSCAN_دري)
		s = i - XDL_SIMSCAN_دري;
	إذا (e - i > XDL_SIMSCAN_دري)
		e = i + XDL_SIMSCAN_دري;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	لكل (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		إذا (action[i - r] == DISCARD)
			rdis0++;
		وإلا إذا (action[i - r] == INVESTIGATE)
			rpdis0++;
		وإلا إذا (action[i - r] == KEEP)
			اكسر;
		وإلا
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	إذا (rdis0 == 0)
		أرجع 0;
	لكل (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		إذا (action[i + r] == DISCARD)
			rdis1++;
		وإلا إذا (action[i + r] == INVESTIGATE)
			rpdis1++;
		وإلا إذا (action[i + r] == KEEP)
			اكسر;
		وإلا
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	إذا (rdis1 == 0)
		أرجع false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	أرجع rpdis1 * XDL_KPDIS_لاڳيتو < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
ساكن صحيح xdl_cleanup_رڪارڊون(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	ڊگهو i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = عدم, *action2 = عدم;
	bool need_min = !!(cf->flags & XDF_NEED_گهٽتر);
	صحيح ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	إذا (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		اذهب cleanup;
	}
	إذا (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		اذهب cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	إذا ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	لكل (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	إذا ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	لكل (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	لكل (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		إذا (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} وإلا
			xdf1->بدليو[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	لكل (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		إذا (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} وإلا
			xdf2->بدليو[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	أرجع ret;
}


/*
 * Early trim initial and terminal matching records.
 */
ساكن صحيح xdl_trim_پڇاڙيون(xdfile_t *xdf1, xdfile_t *xdf2) {
	ڊگهو i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	لكل (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		إذا (recs1->ha != recs2->ha)
			اكسر;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	لكل (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		إذا (recs1->ha != recs2->ha)
			اكسر;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	أرجع 0;
}


ساكن صحيح xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	إذا (xdl_trim_پڇاڙيون(xdf1, xdf2) < 0 ||
	    xdl_cleanup_رڪارڊون(cf, xdf1, xdf2) < 0) {

		أرجع -1;
	}

	أرجع 0;
}

صحيح xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
		    xdfenv_t *xe) {
	ڊگهو enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, حجم(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_سٽون(mf1, sample) + 1;
	enl2 = xdl_guess_سٽون(mf2, sample) + 1;

	إذا (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		أرجع -1;

	إذا (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		أرجع -1;
	}
	إذا (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		أرجع -1;
	}

	إذا ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		أرجع -1;
	}

	xdl_free_classifier(&cf);

	أرجع 0;
}
