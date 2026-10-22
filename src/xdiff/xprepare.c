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

#અન્તર્ભાવ "xinclude.h"


#define XDL_KPDIS_RUN 4
#define XDL_MAX_EQLIMIT 1024
#define XDL_SIMSCAN_WINDOW 100
#define XDL_GUESS_NLINES1 256
#define XDL_GUESS_NLINES2 20

#define DISCARD 0
#define KEEP 1
#define INVESTIGATE 2

પ્રકારવ્યાખ્યા સંરચના s_xdlclass {
	સંરચના s_xdlclass *next;
	xrecord_t rec;
	long idx;
	long len1, len2;
} xdlclass_t;

પ્રકારવ્યાખ્યા સંરચના s_xdlclassifier {
	unsigned પૂર્ણાંક hbits;
	long hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	long alloc;
	long ગણતરી;
	long flags;
} xdlclassifier_t;




સ્થિર પૂર્ણાંક xdl_init_classifier(xdlclassifier_t *cf, long size, long flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((unsigned પૂર્ણાંક) size);
	cf->hsize = 1 << cf->hbits;

	જો (xdl_cha_init(&cf->ncha, કદ(xdlclass_t), size / 4 + 1) < 0) {

		પાછા -1;
	}
	જો (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		પાછા -1;
	}

	cf->alloc = size;
	જો (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		પાછા -1;
	}

	cf->ગણતરી = 0;

	પાછા 0;
}


સ્થિર શૂન્ય xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


સ્થિર પૂર્ણાંક xdl_classify_record(unsigned પૂર્ણાંક pass, xdlclassifier_t *cf, xrecord_t *rec) {
	long hi;
	xdlclass_t *rcrec;

	hi = (long) XDL_HASHLONG(rec->ha, cf->hbits);
	માટે (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->next)
		જો (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.size,
					rec->ptr, rec->size, cf->flags))
			તોડો;

	જો (!rcrec) {
		જો (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			પાછા -1;
		}
		rcrec->idx = cf->ગણતરી++;
		જો (XDL_ALLOC_GROW(cf->rcrecs, cf->ગણતરી, cf->alloc))
				પાછા -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->next = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (unsigned long) rcrec->idx;

	પાછા 0;
}


સ્થિર શૂન્ય xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->changed - 1);
	xdl_free(xdf->recs);
}


સ્થિર પૂર્ણાંક xdl_prepare_ctx(unsigned પૂર્ણાંક pass, mmfile_t *mf, long narec, xpparam_t અચલ *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	long bsize;
	unsigned long hav;
	અક્ષર અચલ *blk, *cur, *ટોચ, *prev;
	xrecord_t *crec;

	xdf->rindex = ખાલી;
	xdf->changed = ખાલી;
	xdf->recs = ખાલી;

	જો (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		જાઓ abort;

	xdf->nrec = 0;
	જો ((cur = blk = xdl_mmfile_first(mf, &bsize))) {
		માટે (ટોચ = blk + bsize; cur < ટોચ; ) {
			prev = cur;
			hav = xdl_hash_record(&cur, ટોચ, xpp->flags);
			જો (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				જાઓ abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = prev;
			crec->size = (long) (cur - prev);
			crec->ha = hav;
			જો (xdl_classify_record(pass, cf, crec) < 0)
				જાઓ abort;
		}
	}

	જો (!XDL_CALLOC_ARRAY(xdf->changed, xdf->nrec + 2))
		જાઓ abort;

	જો ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		જો (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			જાઓ abort;
	}

	xdf->changed += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	પાછા 0;

abort:
	xdl_free_ctx(xdf);
	પાછા -1;
}


શૂન્ય xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


સ્થિર bool xdl_clean_mmatch(uint8_t અચલ *action, long i, long s, long e) {
	long r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	જો (i - s > XDL_SIMSCAN_WINDOW)
		s = i - XDL_SIMSCAN_WINDOW;
	જો (e - i > XDL_SIMSCAN_WINDOW)
		e = i + XDL_SIMSCAN_WINDOW;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	માટે (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		જો (action[i - r] == DISCARD)
			rdis0++;
		નહીંતર જો (action[i - r] == INVESTIGATE)
			rpdis0++;
		નહીંતર જો (action[i - r] == KEEP)
			તોડો;
		નહીંતર
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	જો (rdis0 == 0)
		પાછા 0;
	માટે (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		જો (action[i + r] == DISCARD)
			rdis1++;
		નહીંતર જો (action[i + r] == INVESTIGATE)
			rpdis1++;
		નહીંતર જો (action[i + r] == KEEP)
			તોડો;
		નહીંતર
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	જો (rdis1 == 0)
		પાછા false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	પાછા rpdis1 * XDL_KPDIS_RUN < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
સ્થિર પૂર્ણાંક xdl_cleanup_records(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = ખાલી, *action2 = ખાલી;
	bool need_min = !!(cf->flags & XDF_NEED_MINIMAL);
	પૂર્ણાંક ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	જો (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		જાઓ cleanup;
	}
	જો (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		જાઓ cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	જો ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	માટે (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	જો ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	માટે (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	માટે (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		જો (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} નહીંતર
			xdf1->changed[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	માટે (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		જો (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} નહીંતર
			xdf2->changed[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	પાછા ret;
}


/*
 * Early trim initial and terminal matching records.
 */
સ્થિર પૂર્ણાંક xdl_trim_ends(xdfile_t *xdf1, xdfile_t *xdf2) {
	long i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	માટે (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		જો (recs1->ha != recs2->ha)
			તોડો;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	માટે (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		જો (recs1->ha != recs2->ha)
			તોડો;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	પાછા 0;
}


સ્થિર પૂર્ણાંક xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	જો (xdl_trim_ends(xdf1, xdf2) < 0 ||
	    xdl_cleanup_records(cf, xdf1, xdf2) < 0) {

		પાછા -1;
	}

	પાછા 0;
}

પૂર્ણાંક xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t અચલ *xpp,
		    xdfenv_t *xe) {
	long enl1, enl2, sample;
	xdlclassifier_t cf;

	memset(&cf, 0, કદ(cf));

	/*
	 * For histogram diff, we can afford a smaller sample size and
	 * thus a poorer estimate of the number of lines, as the hash
	 * table (rhash) won't be filled up/grown. The number of lines
	 * (nrecs) will be updated correctly anyway by
	 * xdl_prepare_ctx().
	 */
	sample = (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF
		  ? XDL_GUESS_NLINES2 : XDL_GUESS_NLINES1);

	enl1 = xdl_guess_લીટીઓ(mf1, sample) + 1;
	enl2 = xdl_guess_લીટીઓ(mf2, sample) + 1;

	જો (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		પાછા -1;

	જો (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		પાછા -1;
	}
	જો (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		પાછા -1;
	}

	જો ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		પાછા -1;
	}

	xdl_free_classifier(&cf);

	પાછા 0;
}
