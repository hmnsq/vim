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

#অন্তর্ভাব "xinclude.h"


#define XDL_KPDIS_RUN 4
#define XDL_MAX_EQLIMIT 1024
#define XDL_SIMSCAN_WINDOW 100
#define XDL_GUESS_NLINES1 256
#define XDL_GUESS_NLINES2 20

#define DISCARD 0
#define KEEP 1
#define INVESTIGATE 2

প্রকারলেখ কাঠামো s_xdlclass {
	কাঠামো s_xdlclass *পরবর্তী;
	xrecord_t rec;
	long idx;
	long len1, len2;
} xdlclass_t;

প্রকারলেখ কাঠামো s_xdlclassifier {
	unsigned পূর্ণসংখ্যা hbits;
	long hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	long alloc;
	long গুনতি;
	long flags;
} xdlclassifier_t;




স্থির পূর্ণসংখ্যা xdl_init_classifier(xdlclassifier_t *cf, long আকার_2, long flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((unsigned পূর্ণসংখ্যা) আকার_2);
	cf->hsize = 1 << cf->hbits;

	যদি (xdl_cha_init(&cf->ncha, আকার(xdlclass_t), আকার_2 / 4 + 1) < 0) {

		ফেরত -1;
	}
	যদি (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		ফেরত -1;
	}

	cf->alloc = আকার_2;
	যদি (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		ফেরত -1;
	}

	cf->গুনতি = 0;

	ফেরত 0;
}


স্থির শূন্য xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


স্থির পূর্ণসংখ্যা xdl_classify_record(unsigned পূর্ণসংখ্যা pass, xdlclassifier_t *cf, xrecord_t *rec) {
	long hi;
	xdlclass_t *rcrec;

	hi = (long) XDL_HASHLONG(rec->ha, cf->hbits);
	জন্য (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->পরবর্তী)
		যদি (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.আকার_2,
					rec->ptr, rec->আকার_2, cf->flags))
			বিরতি;

	যদি (!rcrec) {
		যদি (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			ফেরত -1;
		}
		rcrec->idx = cf->গুনতি++;
		যদি (XDL_ALLOC_GROW(cf->rcrecs, cf->গুনতি, cf->alloc))
				ফেরত -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->পরবর্তী = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (unsigned long) rcrec->idx;

	ফেরত 0;
}


স্থির শূন্য xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->changed - 1);
	xdl_free(xdf->recs);
}


স্থির পূর্ণসংখ্যা xdl_prepare_ctx(unsigned পূর্ণসংখ্যা pass, mmfile_t *mf, long narec, xpparam_t ধ্রুবক *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	long bsize;
	unsigned long hav;
	অক্ষর ধ্রুবক *blk, *cur, *শীর্ষ_2, *পূর্ব;
	xrecord_t *crec;

	xdf->rindex = ফাঁকা;
	xdf->changed = ফাঁকা;
	xdf->recs = ফাঁকা;

	যদি (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		যাও abort;

	xdf->nrec = 0;
	যদি ((cur = blk = xdl_mmfile_first(mf, &bsize))) {
		জন্য (শীর্ষ_2 = blk + bsize; cur < শীর্ষ_2; ) {
			পূর্ব = cur;
			hav = xdl_hash_record(&cur, শীর্ষ_2, xpp->flags);
			যদি (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				যাও abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = পূর্ব;
			crec->আকার_2 = (long) (cur - পূর্ব);
			crec->ha = hav;
			যদি (xdl_classify_record(pass, cf, crec) < 0)
				যাও abort;
		}
	}

	যদি (!XDL_CALLOC_ARRAY(xdf->changed, xdf->nrec + 2))
		যাও abort;

	যদি ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		যদি (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			যাও abort;
	}

	xdf->changed += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	ফেরত 0;

abort:
	xdl_free_ctx(xdf);
	ফেরত -1;
}


শূন্য xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


স্থির bool xdl_clean_mmatch(uint8_t ধ্রুবক *action, long i, long s, long e) {
	long r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	যদি (i - s > XDL_SIMSCAN_WINDOW)
		s = i - XDL_SIMSCAN_WINDOW;
	যদি (e - i > XDL_SIMSCAN_WINDOW)
		e = i + XDL_SIMSCAN_WINDOW;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	জন্য (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		যদি (action[i - r] == DISCARD)
			rdis0++;
		অন্যথা যদি (action[i - r] == INVESTIGATE)
			rpdis0++;
		অন্যথা যদি (action[i - r] == KEEP)
			বিরতি;
		অন্যথা
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	যদি (rdis0 == 0)
		ফেরত 0;
	জন্য (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		যদি (action[i + r] == DISCARD)
			rdis1++;
		অন্যথা যদি (action[i + r] == INVESTIGATE)
			rpdis1++;
		অন্যথা যদি (action[i + r] == KEEP)
			বিরতি;
		অন্যথা
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	যদি (rdis1 == 0)
		ফেরত false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	ফেরত rpdis1 * XDL_KPDIS_RUN < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
স্থির পূর্ণসংখ্যা xdl_cleanup_records(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = ফাঁকা, *action2 = ফাঁকা;
	bool need_min = !!(cf->flags & XDF_NEED_MINIMAL);
	পূর্ণসংখ্যা ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	যদি (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		যাও cleanup;
	}
	যদি (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		যাও cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	যদি ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	জন্য (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	যদি ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	জন্য (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	জন্য (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		যদি (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} অন্যথা
			xdf1->changed[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	জন্য (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		যদি (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} অন্যথা
			xdf2->changed[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	ফেরত ret;
}


/*
 * Early trim initial and terminal matching records.
 */
স্থির পূর্ণসংখ্যা xdl_trim_ends(xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	জন্য (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		যদি (recs1->ha != recs2->ha)
			বিরতি;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	জন্য (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		যদি (recs1->ha != recs2->ha)
			বিরতি;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	ফেরত 0;
}


স্থির পূর্ণসংখ্যা xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	যদি (xdl_trim_ends(xdf1, xdf2) < 0 ||
	    xdl_cleanup_records(cf, xdf1, xdf2) < 0) {

		ফেরত -1;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ধ্রুবক *xpp,
		    xdfenv_t *xe) {
	long enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, আকার(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_সারিগুলি(mf1, sample) + 1;
	enl2 = xdl_guess_সারিগুলি(mf2, sample) + 1;

	যদি (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		ফেরত -1;

	যদি (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		ফেরত -1;
	}
	যদি (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ফেরত -1;
	}

	যদি ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		ফেরত -1;
	}

	xdl_free_classifier(&cf);

	ফেরত 0;
}
