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

#అన్తర్భావ "xinclude.h"

స్థిర గుర్తులేని దీర్ఘ get_hash(xdfile_t *xdf, దీర్ఘ సూచీ)
{
	తిరిగి xdf->recs[xdf->rindex[సూచీ]].ha;
}

#నిర్వచించు XDL_MAX_ఖర్చు_MIN 256
#నిర్వచించు XDL_HEUR_MIN_ఖర్చు 256
#నిర్వచించు XDL_పంక్తి_MAX (దీర్ఘ)((1UL << (CHAR_BIT * పరిమాణం(దీర్ఘ) - 1)) - 1)
#నిర్వచించు XDL_SNAKE_CNT 20
#నిర్వచించు XDL_K_HEUR 4

రకనిర్వచనం నిర్మాణం s_xdpsplit {
	దీర్ఘ i1, i2;
	పూర్ణాంకం min_lo, min_hi;
} xdpsplit_t;

/*
 * See "An O(ND) Difference Algorithm and its Variations", by Eugene Myers.
 * Basically considers a "box" (off1, off2, lim1, lim2) and scan from both
 * the forward diagonal starting from (off1, off2) and the backward diagonal
 * starting from (lim1, lim2). If the K values on the same diagonal crosses
 * returns the furthest point of reach. We might encounter expensive edge cases
 * using this algorithm, so a little bit of heuristic is needed to cut the
 * search and to return a suboptimal point.
 */
స్థిర దీర్ఘ xdl_విభజించు(xdfile_t *xdf1, దీర్ఘ off1, దీర్ఘ lim1,
		      xdfile_t *xdf2, దీర్ఘ off2, దీర్ఘ lim2,
		      దీర్ఘ *kvdf, దీర్ఘ *kvdb, పూర్ణాంకం need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	దీర్ఘ dmin = off1 - lim2, dmax = lim1 - off2;
	దీర్ఘ fmid = off1 - off2, bmid = lim1 - lim2;
	దీర్ఘ odd = (fmid - bmid) & 1;
	దీర్ఘ fmin = fmid, fmax = fmid;
	దీర్ఘ bmin = bmid, bmax = bmid;
	దీర్ఘ ec, d, i1, i2, prev1, ఉత్తమ, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	కొరకు (ec = 1;; ec++) {
		పూర్ణాంకం got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		ఒకవేళ (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		లేకపోతే
			++fmin;
		ఒకవేళ (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		లేకపోతే
			--fmax;

		కొరకు (d = fmax; d >= fmin; d -= 2) {
			ఒకవేళ (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			లేకపోతే
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			కొరకు (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			ఒకవేళ (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			ఒకవేళ (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				తిరిగి ec;
			}
		}

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		ఒకవేళ (bmin > dmin)
			kvdb[--bmin - 1] = XDL_పంక్తి_MAX;
		లేకపోతే
			++bmin;
		ఒకవేళ (bmax < dmax)
			kvdb[++bmax + 1] = XDL_పంక్తి_MAX;
		లేకపోతే
			--bmax;

		కొరకు (d = bmax; d >= bmin; d -= 2) {
			ఒకవేళ (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			లేకపోతే
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			కొరకు (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			ఒకవేళ (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			ఒకవేళ (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				తిరిగి ec;
			}
		}

		ఒకవేళ (need_min)
			కొనసాగు;

		/*
		 * If the edit cost is above the heuristic trigger and if
		 * we got a good snake, we sample current diagonals to see
		 * if some of them have reached an "interesting" path. Our
		 * measure is a function of the distance from the diagonal
		 * corner (i1 + i2) penalized with the distance from the
		 * mid diagonal itself. If this value is above the current
		 * edit cost times a magic factor (XDL_K_HEUR) we consider
		 * it interesting.
		 */
		ఒకవేళ (got_snake && ec > xenv->heur_min) {
			కొరకు (ఉత్తమ = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				ఒకవేళ (v > XDL_K_HEUR * ec && v > ఉత్తమ &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					కొరకు (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						ఒకవేళ (k == xenv->snake_cnt) {
							ఉత్తమ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ఆపు;
						}
				}
			}
			ఒకవేళ (ఉత్తమ > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				తిరిగి ec;
			}

			కొరకు (ఉత్తమ = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				ఒకవేళ (v > XDL_K_HEUR * ec && v > ఉత్తమ &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					కొరకు (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						ఒకవేళ (k == xenv->snake_cnt - 1) {
							ఉత్తమ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ఆపు;
						}
				}
			}
			ఒకవేళ (ఉత్తమ > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				తిరిగి ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		ఒకవేళ (ec >= xenv->mxcost) {
			దీర్ఘ fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			కొరకు (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				ఒకవేళ (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				ఒకవేళ (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_పంక్తి_MAX;
			కొరకు (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				ఒకవేళ (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				ఒకవేళ (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			ఒకవేళ ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} లేకపోతే {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			తిరిగి ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
పూర్ణాంకం xdl_recs_cmp(xdfile_t *xdf1, దీర్ఘ off1, దీర్ఘ lim1,
		 xdfile_t *xdf2, దీర్ఘ off2, దీర్ఘ lim2,
		 దీర్ఘ *kvdf, దీర్ఘ *kvdb, పూర్ణాంకం need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	కొరకు (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	కొరకు (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	ఒకవేళ (off1 == lim1) {
		కొరకు (; off2 < lim2; off2++)
			xdf2->మారింది[xdf2->rindex[off2]] = true;
	} లేకపోతే ఒకవేళ (off2 == lim2) {
		కొరకు (; off1 < lim1; off1++)
			xdf1->మారింది[xdf1->rindex[off1]] = true;
	} లేకపోతే {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		ఒకవేళ (xdl_విభజించు(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			తిరిగి -1;
		}

		/*
		 * ... et Impera.
		 */
		ఒకవేళ (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			తిరిగి -1;
		}
	}

	తిరిగి 0;
}


పూర్ణాంకం xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t స్థిరరాశి *xpp,
		xdfenv_t *xe) {
	దీర్ఘ ndiags;
	దీర్ఘ *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	పూర్ణాంకం res;

	ఒకవేళ (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		తిరిగి -1;

	ఒకవేళ (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		వెళ్ళు బహిర్గతం;
	}

	ఒకవేళ (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		వెళ్ళు బహిర్గతం;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	ఒకవేళ (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		తిరిగి -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	ఒకవేళ (xenv.mxcost < XDL_MAX_ఖర్చు_MIN)
		xenv.mxcost = XDL_MAX_ఖర్చు_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_ఖర్చు;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_కనీస) != 0,
			   &xenv);
	xdl_free(kvd);
 బహిర్గతం:
	ఒకవేళ (res < 0)
		xdl_free_env(xe);

	తిరిగి res;
}


స్థిర xdchange_t *xdl_కూడు_change(xdchange_t *xscr, దీర్ఘ i1, దీర్ఘ i2, దీర్ఘ chg1, దీర్ఘ chg2) {
	xdchange_t *xch;

	ఒకవేళ (!(xch = (xdchange_t *) xdl_malloc(పరిమాణం(xdchange_t))))
		తిరిగి ఖాళీ;

	xch->తదుపరి = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	తిరిగి xch;
}


స్థిర పూర్ణాంకం recs_సరిపోలిక(xrecord_t *rec1, xrecord_t *rec2)
{
	తిరిగి (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#నిర్వచించు MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
స్థిర పూర్ణాంకం xget_indent(xrecord_t *rec)
{
	దీర్ఘ i;
	పూర్ణాంకం ret = 0;

	కొరకు (i = 0; i < rec->కొలత; i++) {
		అక్షరం c = rec->ptr[i];

		ఒకవేళ (!XDL_ISSPACE(c))
			తిరిగి ret;
		లేకపోతే ఒకవేళ (c == ' ')
			ret += 1;
		లేకపోతే ఒకవేళ (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		ఒకవేళ (ret >= MAX_INDENT)
			తిరిగి MAX_INDENT;
	}

	/* The line contains only whitespace. */
	తిరిగి -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#నిర్వచించు MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
నిర్మాణం విభజించు_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	పూర్ణాంకం చివర_యొక్క_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	పూర్ణాంకం indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	పూర్ణాంకం pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	పూర్ణాంకం pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	పూర్ణాంకం post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	పూర్ణాంకం post_indent;
};

నిర్మాణం విభజించు_score {
	/* The effective indent of this split (smaller is preferred). */
	పూర్ణాంకం effective_indent;

	/* Penalty for this split (smaller is preferred). */
	పూర్ణాంకం penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
స్థిర శూన్య measure_విభజించు(స్థిరరాశి xdfile_t *xdf, దీర్ఘ విభజించు,
			  నిర్మాణం విభజించు_measurement *m)
{
	దీర్ఘ i;

	ఒకవేళ (విభజించు >= xdf->nrec) {
		m->చివర_యొక్క_file = 1;
		m->indent = -1;
	} లేకపోతే {
		m->చివర_యొక్క_file = 0;
		m->indent = xget_indent(&xdf->recs[విభజించు]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	కొరకు (i = విభజించు - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		ఒకవేళ (m->pre_indent != -1)
			ఆపు;
		m->pre_blank += 1;
		ఒకవేళ (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			ఆపు;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	కొరకు (i = విభజించు + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		ఒకవేళ (m->post_indent != -1)
			ఆపు;
		m->post_blank += 1;
		ఒకవేళ (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			ఆపు;
		}
	}
}

/*
 * The empirically-determined weight factors used by score_split() below.
 * Larger values means that the position is a less favorable place to split.
 *
 * Note that scores are only ever compared against each other, so multiplying
 * all of these weight/penalty values by the same factor wouldn't change the
 * heuristic's behavior. Still, we need to set that arbitrary scale *somehow*.
 * In practice, these numbers are chosen to be large enough that they can be
 * adjusted relative to each other with sufficient precision despite using
 * integer math.
 */

/* Penalty if there are no non-blank lines before the split */
#నిర్వచించు ప్రారంభం_యొక్క_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#నిర్వచించు చివర_యొక్క_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#నిర్వచించు మొత్తం_BLANK_బరువు (-30)

/* Multiplier for the number of blank lines after the split */
#నిర్వచించు POST_BLANK_బరువు 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#నిర్వచించు RELATIVE_INDENT_PENALTY (-4)
#నిర్వచించు RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#నిర్వచించు RELATIVE_OUTDENT_PENALTY 24
#నిర్వచించు RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#నిర్వచించు RELATIVE_DEDENT_PENALTY 23
#నిర్వచించు RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#నిర్వచించు INDENT_బరువు 60

/*
 * How far do we slide a hunk at most?
 */
#నిర్వచించు INDENT_HEURISTIC_MAX_SLIDING 100

/*
 * Compute a badness score for the hypothetical split whose measurements are
 * stored in m. The weight factors were determined empirically using the tools
 * and corpus described in
 *
 *     https://github.com/mhagger/diff-slider-tools
 *
 * Also see that project if you want to improve the weights based on, for
 * example, a larger or more diverse corpus.
 */
స్థిర శూన్య score_కూడు_విభజించు(స్థిరరాశి నిర్మాణం విభజించు_measurement *m, నిర్మాణం విభజించు_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	పూర్ణాంకం post_blank, మొత్తం_blank, indent, any_blanks;

	ఒకవేళ (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += ప్రారంభం_యొక్క_FILE_PENALTY;

	ఒకవేళ (m->చివర_యొక్క_file)
		s->penalty += చివర_యొక్క_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	మొత్తం_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += మొత్తం_BLANK_బరువు * మొత్తం_blank;
	s->penalty += POST_BLANK_బరువు * post_blank;

	ఒకవేళ (m->indent != -1)
		indent = m->indent;
	లేకపోతే
		indent = m->post_indent;

	any_blanks = (మొత్తం_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	ఒకవేళ (indent == -1) {
		/* No additional adjustments needed. */
	} లేకపోతే ఒకవేళ (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} లేకపోతే ఒకవేళ (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} లేకపోతే ఒకవేళ (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} లేకపోతే {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		ఒకవేళ (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} లేకపోతే {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

స్థిర పూర్ణాంకం score_cmp(నిర్మాణం విభజించు_score *s1, నిర్మాణం విభజించు_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	పూర్ణాంకం cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	తిరిగి INDENT_బరువు * cmp_indents + (s1->penalty - s2->penalty);
}

/*
 * Represent a group of changed lines in an xdfile_t (i.e., a contiguous group
 * of lines that was inserted or deleted from the corresponding version of the
 * file). We consider there to be such a group at the beginning of the file, at
 * the end of the file, and between any two unchanged lines, though most such
 * groups will usually be empty.
 *
 * If the first line in a group is equal to the line following the group, then
 * the group can be slid down. Similarly, if the last line in a group is equal
 * to the line preceding the group, then the group can be slid up. See
 * group_slide_down() and group_slide_up().
 *
 * Note that loops that are testing for changed lines in xdf->rchg do not need
 * index bounding since the array is prepared with a zero at position -1 and N.
 */
నిర్మాణం xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	దీర్ఘ ప్రారంభం;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	దీర్ఘ చివర;
};

/*
 * Initialize g to point at the first group in xdf.
 */
స్థిర శూన్య సమూహం_init(xdfile_t *xdf, నిర్మాణం xdlgroup *g)
{
	g->ప్రారంభం = g->చివర = 0;
	అయితే (xdf->మారింది[g->చివర])
		g->చివర++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
స్థిర అంతఃవరుస పూర్ణాంకం సమూహం_తదుపరి(xdfile_t *xdf, నిర్మాణం xdlgroup *g)
{
	ఒకవేళ (g->చివర == xdf->nrec)
		తిరిగి -1;

	g->ప్రారంభం = g->చివర + 1;
	కొరకు (g->చివర = g->ప్రారంభం; xdf->మారింది[g->చివర]; g->చివర++)
		;

	తిరిగి 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
స్థిర అంతఃవరుస పూర్ణాంకం సమూహం_మునుపటి(xdfile_t *xdf, నిర్మాణం xdlgroup *g)
{
	ఒకవేళ (g->ప్రారంభం == 0)
		తిరిగి -1;

	g->చివర = g->ప్రారంభం - 1;
	కొరకు (g->ప్రారంభం = g->చివర; xdf->మారింది[g->ప్రారంభం - 1]; g->ప్రారంభం--)
		;

	తిరిగి 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
స్థిర పూర్ణాంకం సమూహం_slide_కిందికి(xdfile_t *xdf, నిర్మాణం xdlgroup *g)
{
	ఒకవేళ (g->చివర < xdf->nrec &&
	    recs_సరిపోలిక(&xdf->recs[g->ప్రారంభం], &xdf->recs[g->చివర])) {
		xdf->మారింది[g->ప్రారంభం++] = false;
		xdf->మారింది[g->చివర++] = true;

		అయితే (xdf->మారింది[g->చివర])
			g->చివర++;

		తిరిగి 0;
	} లేకపోతే {
		తిరిగి -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
స్థిర పూర్ణాంకం సమూహం_slide_పైకి(xdfile_t *xdf, నిర్మాణం xdlgroup *g)
{
	ఒకవేళ (g->ప్రారంభం > 0 &&
	    recs_సరిపోలిక(&xdf->recs[g->ప్రారంభం - 1], &xdf->recs[g->చివర - 1])) {
		xdf->మారింది[--g->ప్రారంభం] = true;
		xdf->మారింది[--g->చివర] = false;

		అయితే (xdf->మారింది[g->ప్రారంభం - 1])
			g->ప్రారంభం--;

		తిరిగి 0;
	} లేకపోతే {
		తిరిగి -1;
	}
}

శూన్య xdl_bug(స్థిరరాశి అక్షరం *సందేశం)
{
	fprintf(stderr, "BUG: %s\n", సందేశం);
	నిష్క్రమించు(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
పూర్ణాంకం xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, దీర్ఘ flags) {
	నిర్మాణం xdlgroup g, go;
	దీర్ఘ earliest_చివర, చివర_matching_other;
	దీర్ఘ groupsize;

	సమూహం_init(xdf, &g);
	సమూహం_init(xdfo, &go);

	అయితే (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		ఒకవేళ (g.చివర == g.ప్రారంభం)
			వెళ్ళు తదుపరి;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		చేయి {
			groupsize = g.చివర - g.ప్రారంభం;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			చివర_matching_other = -1;

			/* Shift the group backward as much as possible: */
			అయితే (!సమూహం_slide_పైకి(xdf, &g))
				ఒకవేళ (సమూహం_మునుపటి(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_చివర = g.చివర;

			ఒకవేళ (go.చివర > go.ప్రారంభం)
				చివర_matching_other = g.చివర;

			/* Now shift the group forward as far as possible: */
			అయితే (1) {
				ఒకవేళ (సమూహం_slide_కిందికి(xdf, &g))
					ఆపు;
				ఒకవేళ (సమూహం_తదుపరి(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				ఒకవేళ (go.చివర > go.ప్రారంభం)
					చివర_matching_other = g.చివర;
			}
		} అయితే (groupsize != g.చివర - g.ప్రారంభం);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		ఒకవేళ (g.చివర == earliest_చివర) {
			/* no shifting was possible */
		} లేకపోతే ఒకవేళ (చివర_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			అయితే (go.చివర == go.ప్రారంభం) {
				ఒకవేళ (సమూహం_slide_పైకి(xdf, &g))
					xdl_bug("match disappeared");
				ఒకవేళ (సమూహం_మునుపటి(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} లేకపోతే ఒకవేళ (flags & XDF_INDENT_HEURISTIC) {
			/*
			 * Indent heuristic: a group of pure add/delete lines
			 * implies two splits, one between the end of the
			 * "before" context and the start of the group, and
			 * another between the end of the group and the
			 * beginning of the "after" context. Some splits are
			 * aesthetically better and some are worse. We compute
			 * a badness "score" for each split, and add the scores
			 * for the two splits to define a "score" for each
			 * position that the group can be shifted to. Then we
			 * pick the shift with the lowest score.
			 */
			దీర్ఘ జరుపు, ఉత్తమ_జరుపు = -1;
			నిర్మాణం విభజించు_score ఉత్తమ_score;

			జరుపు = earliest_చివర;
			ఒకవేళ (g.చివర - groupsize - 1 > జరుపు)
				జరుపు = g.చివర - groupsize - 1;
			ఒకవేళ (g.చివర - INDENT_HEURISTIC_MAX_SLIDING > జరుపు)
				జరుపు = g.చివర - INDENT_HEURISTIC_MAX_SLIDING;
			కొరకు (; జరుపు <= g.చివర; జరుపు++) {
				నిర్మాణం విభజించు_measurement m;
				నిర్మాణం విభజించు_score score = {0, 0};

				measure_విభజించు(xdf, జరుపు, &m);
				score_కూడు_విభజించు(&m, &score);
				measure_విభజించు(xdf, జరుపు - groupsize, &m);
				score_కూడు_విభజించు(&m, &score);
				ఒకవేళ (ఉత్తమ_జరుపు == -1 ||
				    score_cmp(&score, &ఉత్తమ_score) <= 0) {
					ఉత్తమ_score.effective_indent = score.effective_indent;
					ఉత్తమ_score.penalty = score.penalty;
					ఉత్తమ_జరుపు = జరుపు;
				}
			}

			అయితే (g.చివర > ఉత్తమ_జరుపు) {
				ఒకవేళ (సమూహం_slide_పైకి(xdf, &g))
					xdl_bug("best shift unreached");
				ఒకవేళ (సమూహం_మునుపటి(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	తదుపరి:
		/* Move past the just-processed group: */
		ఒకవేళ (సమూహం_తదుపరి(xdf, &g))
			ఆపు;
		ఒకవేళ (సమూహం_తదుపరి(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	ఒకవేళ (!సమూహం_తదుపరి(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	తిరిగి 0;
}


పూర్ణాంకం xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = ఖాళీ, *xch;
	bool *changed1 = xe->xdf1.మారింది, *changed2 = xe->xdf2.మారింది;
	దీర్ఘ i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	కొరకు (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		ఒకవేళ (changed1[i1 - 1] || changed2[i2 - 1]) {
			కొరకు (l1 = i1; changed1[i1 - 1]; i1--);
			కొరకు (l2 = i2; changed2[i2 - 1]; i2--);

			ఒకవేళ (!(xch = xdl_కూడు_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				తిరిగి -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	తిరిగి 0;
}


శూన్య xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	అయితే ((xch = xscr) != ఖాళీ) {
		xscr = xscr->తదుపరి;
		xdl_free(xch);
	}
}

స్థిర పూర్ణాంకం xdl_call_hunk_func(xdfenv_t *xe వాడని, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t స్థిరరాశి *xecfg)
{
	xdchange_t *xch, *xche;

	కొరకు (xch = xscr; xch; xch = xche->తదుపరి) {
		xche = xdl_get_hunk(&xch, xecfg);
		ఒకవేళ (!xch)
			ఆపు;
		ఒకవేళ (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			తిరిగి -1;
	}
	తిరిగి 0;
}

స్థిర శూన్య xdl_mark_ignorable_పంక్తులు(xdchange_t *xscr, xdfenv_t *xe, దీర్ఘ flags)
{
	xdchange_t *xch;

	కొరకు (xch = xscr; xch; xch = xch->తదుపరి) {
		పూర్ణాంకం ignore = 1;
		xrecord_t *rec;
		దీర్ఘ i;

		rec = &xe->xdf1.recs[xch->i1];
		కొరకు (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].కొలత, flags);

		rec = &xe->xdf2.recs[xch->i2];
		కొరకు (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].కొలత, flags);

		xch->ignore = ignore;
	}
}

#ఒకవేళ 0 // unused by Vim
స్థిర పూర్ణాంకం రికార్డు_matches_regex(xrecord_t *rec, xpparam_t స్థిరరాశి *xpp) {
	regmatch_t regmatch;
	size_t i;

	కొరకు (i = 0; i < xpp->ignore_regex_nr; i++)
		ఒకవేళ (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->కొలత, 1,
				 &regmatch, 0))
			తిరిగి 1;

	తిరిగి 0;
}

స్థిర శూన్య xdl_mark_ignorable_regex(xdchange_t *xscr, స్థిరరాశి xdfenv_t *xe,
				     xpparam_t స్థిరరాశి *xpp)
{
	xdchange_t *xch;

	కొరకు (xch = xscr; xch; xch = xch->తదుపరి) {
		xrecord_t *rec;
		పూర్ణాంకం ignore = 1;
		దీర్ఘ i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		ఒకవేళ (xch->ignore)
			కొనసాగు;

		rec = &xe->xdf1.recs[xch->i1];
		కొరకు (i = 0; i < xch->chg1 && ignore; i++)
			ignore = రికార్డు_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		కొరకు (i = 0; i < xch->chg2 && ignore; i++)
			ignore = రికార్డు_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#అయితే_ముగింపు

పూర్ణాంకం xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t స్థిరరాశి *xpp,
	     xdemitconf_t స్థిరరాశి *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	ఒకవేళ (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		తిరిగి -1;
	}
	ఒకవేళ (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		తిరిగి -1;
	}
	ఒకవేళ (xscr) {
		ఒకవేళ (xpp->flags & XDF_IGNORE_BLANK_పంక్తులు)
			xdl_mark_ignorable_పంక్తులు(xscr, &xe, xpp->flags);

#ఒకవేళ 0
		ఒకవేళ (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#అయితే_ముగింపు

		ఒకవేళ (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			తిరిగి -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	తిరిగి 0;
}
