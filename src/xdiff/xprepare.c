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

#شامل "xinclude.h"


#تعریف_کرو XDL_KPDIS_تسلسل 4
#تعریف_کرو XDL_MAX_EQLIMIT 1024
#تعریف_کرو XDL_SIMSCAN_کھڑکی 100
#تعریف_کرو XDL_GUESS_NLINES1 256
#تعریف_کرو XDL_GUESS_NLINES2 20

#تعریف_کرو DISCARD 0
#تعریف_کرو KEEP 1
#تعریف_کرو INVESTIGATE 2

قسمتعریف ساخت s_xdlclass {
	ساخت s_xdlclass *اگلا;
	xrecord_t rec;
	طویل idx;
	طویل len1, len2;
} xdlclass_t;

قسمتعریف ساخت s_xdlclassifier {
	غیرعلامتی صحیح hbits;
	طویل hsize;
	xdlclass_t **rchash;
	chastore_t ncha;
	xdlclass_t **rcrecs;
	طویل alloc;
	طویل گنتی;
	طویل flags;
} xdlclassifier_t;




ساکن صحیح xdl_init_classifier(xdlclassifier_t *cf, طویل سائز, طویل flags) {
	cf->flags = flags;

	cf->hbits = xdl_hashbits((غیرعلامتی صحیح) سائز);
	cf->hsize = 1 << cf->hbits;

	اگر (xdl_cha_init(&cf->ncha, حجم(xdlclass_t), سائز / 4 + 1) < 0) {

		واپس -1;
	}
	اگر (!XDL_CALLOC_ARRAY(cf->rchash, cf->hsize)) {

		xdl_cha_free(&cf->ncha);
		واپس -1;
	}

	cf->alloc = سائز;
	اگر (!XDL_ALLOC_ARRAY(cf->rcrecs, cf->alloc)) {

		xdl_free(cf->rchash);
		xdl_cha_free(&cf->ncha);
		واپس -1;
	}

	cf->گنتی = 0;

	واپس 0;
}


ساکن خالی xdl_free_classifier(xdlclassifier_t *cf) {

	xdl_free(cf->rcrecs);
	xdl_free(cf->rchash);
	xdl_cha_free(&cf->ncha);
}


ساکن صحیح xdl_classify_ریکارڈ(غیرعلامتی صحیح pass, xdlclassifier_t *cf, xrecord_t *rec) {
	طویل hi;
	xdlclass_t *rcrec;

	hi = (طویل) XDL_HASHLONG(rec->ha, cf->hbits);
	برائے (rcrec = cf->rchash[hi]; rcrec; rcrec = rcrec->اگلا)
		اگر (rcrec->rec.ha == rec->ha &&
				xdl_recmatch(rcrec->rec.ptr, rcrec->rec.سائز,
					rec->ptr, rec->سائز, cf->flags))
			توڑو;

	اگر (!rcrec) {
		اگر (!(rcrec = xdl_cha_alloc(&cf->ncha))) {

			واپس -1;
		}
		rcrec->idx = cf->گنتی++;
		اگر (XDL_ALLOC_GROW(cf->rcrecs, cf->گنتی, cf->alloc))
				واپس -1;
		cf->rcrecs[rcrec->idx] = rcrec;
		rcrec->rec = *rec;
		rcrec->len1 = rcrec->len2 = 0;
		rcrec->اگلا = cf->rchash[hi];
		cf->rchash[hi] = rcrec;
	}

	(pass == 1) ? rcrec->len1++ : rcrec->len2++;

	rec->ha = (غیرعلامتی طویل) rcrec->idx;

	واپس 0;
}


ساکن خالی xdl_free_ctx(xdfile_t *xdf)
{
	xdl_free(xdf->rindex);
	xdl_free(xdf->بدلا - 1);
	xdl_free(xdf->recs);
}


ساکن صحیح xdl_prepare_ctx(غیرعلامتی صحیح pass, mmfile_t *mf, طویل narec, xpparam_t ثابت *xpp,
			   xdlclassifier_t *cf, xdfile_t *xdf) {
	طویل bsize;
	غیرعلامتی طویل hav;
	حرف ثابت *blk, *cur, *چوٹی, *پچھلا;
	xrecord_t *crec;

	xdf->rindex = تہی;
	xdf->بدلا = تہی;
	xdf->recs = تہی;

	اگر (!XDL_ALLOC_ARRAY(xdf->recs, narec))
		جاؤ abort;

	xdf->nrec = 0;
	اگر ((cur = blk = xdl_mmfile_پہلا(mf, &bsize))) {
		برائے (چوٹی = blk + bsize; cur < چوٹی; ) {
			پچھلا = cur;
			hav = xdl_hash_ریکارڈ(&cur, چوٹی, xpp->flags);
			اگر (XDL_ALLOC_GROW(xdf->recs, xdf->nrec + 1, narec))
				جاؤ abort;
			crec = &xdf->recs[xdf->nrec++];
			crec->ptr = پچھلا;
			crec->سائز = (طویل) (cur - پچھلا);
			crec->ha = hav;
			اگر (xdl_classify_ریکارڈ(pass, cf, crec) < 0)
				جاؤ abort;
		}
	}

	اگر (!XDL_CALLOC_ARRAY(xdf->بدلا, xdf->nrec + 2))
		جاؤ abort;

	اگر ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF)) {
		اگر (!XDL_ALLOC_ARRAY(xdf->rindex, xdf->nrec + 1))
			جاؤ abort;
	}

	xdf->بدلا += 1;
	xdf->nreff = 0;
	xdf->dstart = 0;
	xdf->dend = xdf->nrec - 1;

	واپس 0;

abort:
	xdl_free_ctx(xdf);
	واپس -1;
}


خالی xdl_free_env(xdfenv_t *xe) {

	xdl_free_ctx(&xe->xdf2);
	xdl_free_ctx(&xe->xdf1);
}


ساکن bool xdl_clean_mmatch(uint8_t ثابت *action, طویل i, طویل s, طویل e) {
	طویل r, rdis0, rpdis0, rdis1, rpdis1;

	/*
	 * Limits the window that is examined during the similar-lines
	 * scan. The loops below stops when action[i - r] == KEEP
	 * (line that has no match), but there are corner cases where
	 * the loop proceed all the way to the extremities by causing
	 * huge performance penalties in case of big files.
	 */
	اگر (i - s > XDL_SIMSCAN_کھڑکی)
		s = i - XDL_SIMSCAN_کھڑکی;
	اگر (e - i > XDL_SIMSCAN_کھڑکی)
		e = i + XDL_SIMSCAN_کھڑکی;

	/*
	 * Scans the lines before 'i' to find a run of lines that either
	 * have no match (action[j] == DISCARD) or have multiple matches
	 * (action[j] == INVESTIGATE). Note that we always call this
	 * function with action[i] == INVESTIGATE, so the current line
	 * (i) is already a multimatch line.
	 */
	برائے (r = 1, rdis0 = 0, rpdis0 = 1; (i - r) >= s; r++) {
		اگر (action[i - r] == DISCARD)
			rdis0++;
		ورنہ اگر (action[i - r] == INVESTIGATE)
			rpdis0++;
		ورنہ اگر (action[i - r] == KEEP)
			توڑو;
		ورنہ
			xdl_bug("Illegal value for action[i - r]");
	}
	/*
	 * If the run before the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded. We want to discard multimatch lines only when
	 * they appear in the middle of runs with nomatch lines
	 * (action[j] == DISCARD).
	 */
	اگر (rdis0 == 0)
		واپس 0;
	برائے (r = 1, rdis1 = 0, rpdis1 = 1; (i + r) <= e; r++) {
		اگر (action[i + r] == DISCARD)
			rdis1++;
		ورنہ اگر (action[i + r] == INVESTIGATE)
			rpdis1++;
		ورنہ اگر (action[i + r] == KEEP)
			توڑو;
		ورنہ
			xdl_bug("Illegal value for action[i + r]");
	}
	/*
	 * If the run after the line 'i' found only multimatch lines,
	 * we return false and hence we don't make the current line (i)
	 * discarded.
	 */
	اگر (rdis1 == 0)
		واپس false;
	rdis1 += rdis0;
	rpdis1 += rpdis0;

	واپس rpdis1 * XDL_KPDIS_تسلسل < (rpdis1 + rdis1);
}


/*
 * Try to reduce the problem complexity, discard records that have no
 * matches on the other file. Also, lines that have multiple matches
 * might be potentially discarded if they appear in a run of discardable.
 */
ساکن صحیح xdl_cleanup_اندراجات(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {
	طویل i, nm, nreff, mlim;
	xrecord_t *recs;
	xdlclass_t *rcrec;
	uint8_t *action1 = تہی, *action2 = تہی;
	bool need_min = !!(cf->flags & XDF_NEED_اقل);
	صحیح ret = 0;

	/*
	 * Create temporary arrays that will help us decide if
	 * changed[i] should remain false, or become true.
	 */
	اگر (!XDL_CALLOC_ARRAY(action1, xdf1->nrec + 1)) {
		ret = -1;
		جاؤ cleanup;
	}
	اگر (!XDL_CALLOC_ARRAY(action2, xdf2->nrec + 1)) {
		ret = -1;
		جاؤ cleanup;
	}

	/*
	 * Initialize temporary arrays with DISCARD, KEEP, or INVESTIGATE.
	 */
	اگر ((mlim = xdl_bogosqrt(xdf1->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	برائے (i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart]; i <= xdf1->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len2 : 0;
		action1[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	اگر ((mlim = xdl_bogosqrt(xdf2->nrec)) > XDL_MAX_EQLIMIT)
		mlim = XDL_MAX_EQLIMIT;
	برائے (i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart]; i <= xdf2->dend; i++, recs++) {
		rcrec = cf->rcrecs[recs->ha];
		nm = rcrec ? rcrec->len1 : 0;
		action2[i] = (nm == 0) ? DISCARD: (nm >= mlim && !need_min) ? INVESTIGATE: KEEP;
	}

	/*
	 * Use temporary arrays to decide if changed[i] should remain
	 * false, or become true.
	 */
	برائے (nreff = 0, i = xdf1->dstart, recs = &xdf1->recs[xdf1->dstart];
	     i <= xdf1->dend; i++, recs++) {
		اگر (action1[i] == KEEP ||
		    (action1[i] == INVESTIGATE && !xdl_clean_mmatch(action1, i, xdf1->dstart, xdf1->dend))) {
			xdf1->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ورنہ
			xdf1->بدلا[i] = true;
			/* i.e. discard */
	}
	xdf1->nreff = nreff;

	برائے (nreff = 0, i = xdf2->dstart, recs = &xdf2->recs[xdf2->dstart];
	     i <= xdf2->dend; i++, recs++) {
		اگر (action2[i] == KEEP ||
		    (action2[i] == INVESTIGATE && !xdl_clean_mmatch(action2, i, xdf2->dstart, xdf2->dend))) {
			xdf2->rindex[nreff++] = i;
			/* changed[i] remains false, i.e. keep */
		} ورنہ
			xdf2->بدلا[i] = true;
			/* i.e. discard */
	}
	xdf2->nreff = nreff;

cleanup:
	xdl_free(action1);
	xdl_free(action2);

	واپس ret;
}


/*
 * Early trim initial and terminal matching records.
 */
ساکن صحیح xdl_trim_سرے(xdfile_t *xdf1, xdfile_t *xdf2) {
	طویل i, lim;
	xrecord_t *recs1, *recs2;

	recs1 = xdf1->recs;
	recs2 = xdf2->recs;
	برائے (i = 0, lim = XDL_MIN(xdf1->nrec, xdf2->nrec); i < lim;
	     i++, recs1++, recs2++)
		اگر (recs1->ha != recs2->ha)
			توڑو;

	xdf1->dstart = xdf2->dstart = i;

	recs1 = xdf1->recs + xdf1->nrec - 1;
	recs2 = xdf2->recs + xdf2->nrec - 1;
	برائے (lim -= i, i = 0; i < lim; i++, recs1--, recs2--)
		اگر (recs1->ha != recs2->ha)
			توڑو;

	xdf1->dend = xdf1->nrec - i - 1;
	xdf2->dend = xdf2->nrec - i - 1;

	واپس 0;
}


ساکن صحیح xdl_optimize_ctxs(xdlclassifier_t *cf, xdfile_t *xdf1, xdfile_t *xdf2) {

	اگر (xdl_trim_سرے(xdf1, xdf2) < 0 ||
	    xdl_cleanup_اندراجات(cf, xdf1, xdf2) < 0) {

		واپس -1;
	}

	واپس 0;
}

صحیح xdl_prepare_env(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
		    xdfenv_t *xe) {
	طویل enl1, enl2, sample;
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

	enl1 = xdl_guess_سطریں(mf1, sample) + 1;
	enl2 = xdl_guess_سطریں(mf2, sample) + 1;

	اگر (xdl_init_classifier(&cf, enl1 + enl2 + 1, xpp->flags) < 0)
		واپس -1;

	اگر (xdl_prepare_ctx(1, mf1, enl1, xpp, &cf, &xe->xdf1) < 0) {

		xdl_free_classifier(&cf);
		واپس -1;
	}
	اگر (xdl_prepare_ctx(2, mf2, enl2, xpp, &cf, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		واپس -1;
	}

	اگر ((XDF_DIFF_ALG(xpp->flags) != XDF_PATIENCE_DIFF) &&
	    (XDF_DIFF_ALG(xpp->flags) != XDF_HISTOGRAM_DIFF) &&
	    xdl_optimize_ctxs(&cf, &xe->xdf1, &xe->xdf2) < 0) {

		xdl_free_ctx(&xe->xdf2);
		xdl_free_ctx(&xe->xdf1);
		xdl_free_classifier(&cf);
		واپس -1;
	}

	xdl_free_classifier(&cf);

	واپس 0;
}
