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

#ಅನ್ತರ್ಭಾವ "xinclude.h"

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ get_hash(xdfile_t *xdf, ದೀರ್ಘ ಸೂಚ್ಯಂಕ)
{
	ಹಿಂತಿರುಗಿ xdf->recs[xdf->rindex[ಸೂಚ್ಯಂಕ]].ha;
}

#ವ್ಯಾಖ್ಯಾನಿಸು XDL_MAX_ವೆಚ್ಚ_MIN 256
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_HEUR_MIN_ವೆಚ್ಚ 256
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ಸಾಲು_MAX (ದೀರ್ಘ)((1UL << (CHAR_BIT * ಗಾತ್ರ(ದೀರ್ಘ) - 1)) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_SNAKE_CNT 20
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_K_HEUR 4

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdpsplit {
	ದೀರ್ಘ i1, i2;
	ಪೂರ್ಣಾಂಕ min_lo, min_hi;
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
ಸ್ಥಿರ ದೀರ್ಘ xdl_ವಿಭಜಿಸು(xdfile_t *xdf1, ದೀರ್ಘ off1, ದೀರ್ಘ lim1,
		      xdfile_t *xdf2, ದೀರ್ಘ off2, ದೀರ್ಘ lim2,
		      ದೀರ್ಘ *kvdf, ದೀರ್ಘ *kvdb, ಪೂರ್ಣಾಂಕ need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	ದೀರ್ಘ dmin = off1 - lim2, dmax = lim1 - off2;
	ದೀರ್ಘ fmid = off1 - off2, bmid = lim1 - lim2;
	ದೀರ್ಘ odd = (fmid - bmid) & 1;
	ದೀರ್ಘ fmin = fmid, fmax = fmid;
	ದೀರ್ಘ bmin = bmid, bmax = bmid;
	ದೀರ್ಘ ec, d, i1, i2, prev1, ಉತ್ತಮ, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	ಗಾಗಿ (ec = 1;; ec++) {
		ಪೂರ್ಣಾಂಕ got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		ಆದರೆ (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		ಇಲ್ಲದಿದ್ದರೆ
			++fmin;
		ಆದರೆ (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		ಇಲ್ಲದಿದ್ದರೆ
			--fmax;

		ಗಾಗಿ (d = fmax; d >= fmin; d -= 2) {
			ಆದರೆ (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			ಇಲ್ಲದಿದ್ದರೆ
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			ಗಾಗಿ (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			ಆದರೆ (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			ಆದರೆ (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ಹಿಂತಿರುಗಿ ec;
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
		ಆದರೆ (bmin > dmin)
			kvdb[--bmin - 1] = XDL_ಸಾಲು_MAX;
		ಇಲ್ಲದಿದ್ದರೆ
			++bmin;
		ಆದರೆ (bmax < dmax)
			kvdb[++bmax + 1] = XDL_ಸಾಲು_MAX;
		ಇಲ್ಲದಿದ್ದರೆ
			--bmax;

		ಗಾಗಿ (d = bmax; d >= bmin; d -= 2) {
			ಆದರೆ (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			ಇಲ್ಲದಿದ್ದರೆ
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			ಗಾಗಿ (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			ಆದರೆ (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			ಆದರೆ (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ಹಿಂತಿರುಗಿ ec;
			}
		}

		ಆದರೆ (need_min)
			ಮುಂದುವರಿ;

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
		ಆದರೆ (got_snake && ec > xenv->heur_min) {
			ಗಾಗಿ (ಉತ್ತಮ = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				ಆದರೆ (v > XDL_K_HEUR * ec && v > ಉತ್ತಮ &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					ಗಾಗಿ (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						ಆದರೆ (k == xenv->snake_cnt) {
							ಉತ್ತಮ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ನಿಲ್ಲಿಸು;
						}
				}
			}
			ಆದರೆ (ಉತ್ತಮ > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				ಹಿಂತಿರುಗಿ ec;
			}

			ಗಾಗಿ (ಉತ್ತಮ = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				ಆದರೆ (v > XDL_K_HEUR * ec && v > ಉತ್ತಮ &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					ಗಾಗಿ (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						ಆದರೆ (k == xenv->snake_cnt - 1) {
							ಉತ್ತಮ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ನಿಲ್ಲಿಸು;
						}
				}
			}
			ಆದರೆ (ಉತ್ತಮ > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				ಹಿಂತಿರುಗಿ ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		ಆದರೆ (ec >= xenv->mxcost) {
			ದೀರ್ಘ fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			ಗಾಗಿ (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				ಆದರೆ (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				ಆದರೆ (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_ಸಾಲು_MAX;
			ಗಾಗಿ (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				ಆದರೆ (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				ಆದರೆ (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			ಆದರೆ ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} ಇಲ್ಲದಿದ್ದರೆ {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			ಹಿಂತಿರುಗಿ ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
ಪೂರ್ಣಾಂಕ xdl_recs_cmp(xdfile_t *xdf1, ದೀರ್ಘ off1, ದೀರ್ಘ lim1,
		 xdfile_t *xdf2, ದೀರ್ಘ off2, ದೀರ್ಘ lim2,
		 ದೀರ್ಘ *kvdf, ದೀರ್ಘ *kvdb, ಪೂರ್ಣಾಂಕ need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	ಗಾಗಿ (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	ಗಾಗಿ (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	ಆದರೆ (off1 == lim1) {
		ಗಾಗಿ (; off2 < lim2; off2++)
			xdf2->ಬದಲಾಯಿತು[xdf2->rindex[off2]] = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (off2 == lim2) {
		ಗಾಗಿ (; off1 < lim1; off1++)
			xdf1->ಬದಲಾಯಿತು[xdf1->rindex[off1]] = true;
	} ಇಲ್ಲದಿದ್ದರೆ {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		ಆದರೆ (xdl_ವಿಭಜಿಸು(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			ಹಿಂತಿರುಗಿ -1;
		}

		/*
		 * ... et Impera.
		 */
		ಆದರೆ (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			ಹಿಂತಿರುಗಿ -1;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}


ಪೂರ್ಣಾಂಕ xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
		xdfenv_t *xe) {
	ದೀರ್ಘ ndiags;
	ದೀರ್ಘ *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	ಪೂರ್ಣಾಂಕ res;

	ಆದರೆ (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		ಹೋಗು ಹೊರಹೋಗು;
	}

	ಆದರೆ (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		ಹೋಗು ಹೊರಹೋಗು;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	ಆದರೆ (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		ಹಿಂತಿರುಗಿ -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	ಆದರೆ (xenv.mxcost < XDL_MAX_ವೆಚ್ಚ_MIN)
		xenv.mxcost = XDL_MAX_ವೆಚ್ಚ_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_ವೆಚ್ಚ;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_ಕನಿಷ್ಠತಮ) != 0,
			   &xenv);
	xdl_free(kvd);
 ಹೊರಹೋಗು:
	ಆದರೆ (res < 0)
		xdl_free_env(xe);

	ಹಿಂತಿರುಗಿ res;
}


ಸ್ಥಿರ xdchange_t *xdl_ಕೂಡಿಸು_change(xdchange_t *xscr, ದೀರ್ಘ i1, ದೀರ್ಘ i2, ದೀರ್ಘ chg1, ದೀರ್ಘ chg2) {
	xdchange_t *xch;

	ಆದರೆ (!(xch = (xdchange_t *) xdl_malloc(ಗಾತ್ರ(xdchange_t))))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	xch->ಮುಂದಿನ = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	ಹಿಂತಿರುಗಿ xch;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recs_ಹೊಂದಿಕೆ(xrecord_t *rec1, xrecord_t *rec2)
{
	ಹಿಂತಿರುಗಿ (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xget_indent(xrecord_t *rec)
{
	ದೀರ್ಘ i;
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಗಾಗಿ (i = 0; i < rec->ಗಾತ್ರ_2; i++) {
		ಅಕ್ಷರ c = rec->ptr[i];

		ಆದರೆ (!XDL_ISSPACE(c))
			ಹಿಂತಿರುಗಿ ret;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == ' ')
			ret += 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		ಆದರೆ (ret >= MAX_INDENT)
			ಹಿಂತಿರುಗಿ MAX_INDENT;
	}

	/* The line contains only whitespace. */
	ಹಿಂತಿರುಗಿ -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
ರಚನೆ ವಿಭಜಿಸು_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	ಪೂರ್ಣಾಂಕ ಕೊನೆ_ದ_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	ಪೂರ್ಣಾಂಕ indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	ಪೂರ್ಣಾಂಕ pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	ಪೂರ್ಣಾಂಕ pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	ಪೂರ್ಣಾಂಕ post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	ಪೂರ್ಣಾಂಕ post_indent;
};

ರಚನೆ ವಿಭಜಿಸು_score {
	/* The effective indent of this split (smaller is preferred). */
	ಪೂರ್ಣಾಂಕ effective_indent;

	/* Penalty for this split (smaller is preferred). */
	ಪೂರ್ಣಾಂಕ penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
ಸ್ಥಿರ ಶೂನ್ಯ measure_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ xdfile_t *xdf, ದೀರ್ಘ ವಿಭಜಿಸು,
			  ರಚನೆ ವಿಭಜಿಸು_measurement *m)
{
	ದೀರ್ಘ i;

	ಆದರೆ (ವಿಭಜಿಸು >= xdf->nrec) {
		m->ಕೊನೆ_ದ_file = 1;
		m->indent = -1;
	} ಇಲ್ಲದಿದ್ದರೆ {
		m->ಕೊನೆ_ದ_file = 0;
		m->indent = xget_indent(&xdf->recs[ವಿಭಜಿಸು]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	ಗಾಗಿ (i = ವಿಭಜಿಸು - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		ಆದರೆ (m->pre_indent != -1)
			ನಿಲ್ಲಿಸು;
		m->pre_blank += 1;
		ಆದರೆ (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			ನಿಲ್ಲಿಸು;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	ಗಾಗಿ (i = ವಿಭಜಿಸು + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		ಆದರೆ (m->post_indent != -1)
			ನಿಲ್ಲಿಸು;
		m->post_blank += 1;
		ಆದರೆ (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			ನಿಲ್ಲಿಸು;
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
#ವ್ಯಾಖ್ಯಾನಿಸು ಆರಂಭ_ದ_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_ದ_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಟ್ಟು_BLANK_ತೂಕ (-30)

/* Multiplier for the number of blank lines after the split */
#ವ್ಯಾಖ್ಯಾನಿಸು POST_BLANK_ತೂಕ 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_INDENT_PENALTY (-4)
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_OUTDENT_PENALTY 24
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_DEDENT_PENALTY 23
#ವ್ಯಾಖ್ಯಾನಿಸು RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು INDENT_ತೂಕ 60

/*
 * How far do we slide a hunk at most?
 */
#ವ್ಯಾಖ್ಯಾನಿಸು INDENT_HEURISTIC_MAX_SLIDING 100

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
ಸ್ಥಿರ ಶೂನ್ಯ score_ಕೂಡಿಸು_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ರಚನೆ ವಿಭಜಿಸು_measurement *m, ರಚನೆ ವಿಭಜಿಸು_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	ಪೂರ್ಣಾಂಕ post_blank, ಒಟ್ಟು_blank, indent, any_blanks;

	ಆದರೆ (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += ಆರಂಭ_ದ_FILE_PENALTY;

	ಆದರೆ (m->ಕೊನೆ_ದ_file)
		s->penalty += ಕೊನೆ_ದ_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	ಒಟ್ಟು_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += ಒಟ್ಟು_BLANK_ತೂಕ * ಒಟ್ಟು_blank;
	s->penalty += POST_BLANK_ತೂಕ * post_blank;

	ಆದರೆ (m->indent != -1)
		indent = m->indent;
	ಇಲ್ಲದಿದ್ದರೆ
		indent = m->post_indent;

	any_blanks = (ಒಟ್ಟು_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	ಆದರೆ (indent == -1) {
		/* No additional adjustments needed. */
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} ಇಲ್ಲದಿದ್ದರೆ {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		ಆದರೆ (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} ಇಲ್ಲದಿದ್ದರೆ {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ score_cmp(ರಚನೆ ವಿಭಜಿಸು_score *s1, ರಚನೆ ವಿಭಜಿಸು_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	ಪೂರ್ಣಾಂಕ cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	ಹಿಂತಿರುಗಿ INDENT_ತೂಕ * cmp_indents + (s1->penalty - s2->penalty);
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
ರಚನೆ xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	ದೀರ್ಘ ಆರಂಭ;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	ದೀರ್ಘ ಕೊನೆ;
};

/*
 * Initialize g to point at the first group in xdf.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ಗುಂಪು_init(xdfile_t *xdf, ರಚನೆ xdlgroup *g)
{
	g->ಆರಂಭ = g->ಕೊನೆ = 0;
	ಆಗ (xdf->ಬದಲಾಯಿತು[g->ಕೊನೆ])
		g->ಕೊನೆ++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಮುಂದಿನ(xdfile_t *xdf, ರಚನೆ xdlgroup *g)
{
	ಆದರೆ (g->ಕೊನೆ == xdf->nrec)
		ಹಿಂತಿರುಗಿ -1;

	g->ಆರಂಭ = g->ಕೊನೆ + 1;
	ಗಾಗಿ (g->ಕೊನೆ = g->ಆರಂಭ; xdf->ಬದಲಾಯಿತು[g->ಕೊನೆ]; g->ಕೊನೆ++)
		;

	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಹಿಂದಿನ(xdfile_t *xdf, ರಚನೆ xdlgroup *g)
{
	ಆದರೆ (g->ಆರಂಭ == 0)
		ಹಿಂತಿರುಗಿ -1;

	g->ಕೊನೆ = g->ಆರಂಭ - 1;
	ಗಾಗಿ (g->ಆರಂಭ = g->ಕೊನೆ; xdf->ಬದಲಾಯಿತು[g->ಆರಂಭ - 1]; g->ಆರಂಭ--)
		;

	ಹಿಂತಿರುಗಿ 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗುಂಪು_slide_ಕೆಳಕ್ಕೆ(xdfile_t *xdf, ರಚನೆ xdlgroup *g)
{
	ಆದರೆ (g->ಕೊನೆ < xdf->nrec &&
	    recs_ಹೊಂದಿಕೆ(&xdf->recs[g->ಆರಂಭ], &xdf->recs[g->ಕೊನೆ])) {
		xdf->ಬದಲಾಯಿತು[g->ಆರಂಭ++] = false;
		xdf->ಬದಲಾಯಿತು[g->ಕೊನೆ++] = true;

		ಆಗ (xdf->ಬದಲಾಯಿತು[g->ಕೊನೆ])
			g->ಕೊನೆ++;

		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗುಂಪು_slide_ಮೇಲೆ(xdfile_t *xdf, ರಚನೆ xdlgroup *g)
{
	ಆದರೆ (g->ಆರಂಭ > 0 &&
	    recs_ಹೊಂದಿಕೆ(&xdf->recs[g->ಆರಂಭ - 1], &xdf->recs[g->ಕೊನೆ - 1])) {
		xdf->ಬದಲಾಯಿತು[--g->ಆರಂಭ] = true;
		xdf->ಬದಲಾಯಿತು[--g->ಕೊನೆ] = false;

		ಆಗ (xdf->ಬದಲಾಯಿತು[g->ಆರಂಭ - 1])
			g->ಆರಂಭ--;

		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -1;
	}
}

ಶೂನ್ಯ xdl_bug(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
	fprintf(stderr, "BUG: %s\n", ಸಂದೇಶ);
	ನಿರ್ಗಮಿಸು(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
ಪೂರ್ಣಾಂಕ xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, ದೀರ್ಘ flags) {
	ರಚನೆ xdlgroup g, go;
	ದೀರ್ಘ earliest_ಕೊನೆ, ಕೊನೆ_matching_other;
	ದೀರ್ಘ groupsize;

	ಗುಂಪು_init(xdf, &g);
	ಗುಂಪು_init(xdfo, &go);

	ಆಗ (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		ಆದರೆ (g.ಕೊನೆ == g.ಆರಂಭ)
			ಹೋಗು ಮುಂದಿನ;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		ಮಾಡು {
			groupsize = g.ಕೊನೆ - g.ಆರಂಭ;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			ಕೊನೆ_matching_other = -1;

			/* Shift the group backward as much as possible: */
			ಆಗ (!ಗುಂಪು_slide_ಮೇಲೆ(xdf, &g))
				ಆದರೆ (ಗುಂಪು_ಹಿಂದಿನ(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_ಕೊನೆ = g.ಕೊನೆ;

			ಆದರೆ (go.ಕೊನೆ > go.ಆರಂಭ)
				ಕೊನೆ_matching_other = g.ಕೊನೆ;

			/* Now shift the group forward as far as possible: */
			ಆಗ (1) {
				ಆದರೆ (ಗುಂಪು_slide_ಕೆಳಕ್ಕೆ(xdf, &g))
					ನಿಲ್ಲಿಸು;
				ಆದರೆ (ಗುಂಪು_ಮುಂದಿನ(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				ಆದರೆ (go.ಕೊನೆ > go.ಆರಂಭ)
					ಕೊನೆ_matching_other = g.ಕೊನೆ;
			}
		} ಆಗ (groupsize != g.ಕೊನೆ - g.ಆರಂಭ);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		ಆದರೆ (g.ಕೊನೆ == earliest_ಕೊನೆ) {
			/* no shifting was possible */
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆ_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			ಆಗ (go.ಕೊನೆ == go.ಆರಂಭ) {
				ಆದರೆ (ಗುಂಪು_slide_ಮೇಲೆ(xdf, &g))
					xdl_bug("match disappeared");
				ಆದರೆ (ಗುಂಪು_ಹಿಂದಿನ(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_INDENT_HEURISTIC) {
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
			ದೀರ್ಘ ಸರಿಸು, ಉತ್ತಮ_ಸರಿಸು = -1;
			ರಚನೆ ವಿಭಜಿಸು_score ಉತ್ತಮ_score;

			ಸರಿಸು = earliest_ಕೊನೆ;
			ಆದರೆ (g.ಕೊನೆ - groupsize - 1 > ಸರಿಸು)
				ಸರಿಸು = g.ಕೊನೆ - groupsize - 1;
			ಆದರೆ (g.ಕೊನೆ - INDENT_HEURISTIC_MAX_SLIDING > ಸರಿಸು)
				ಸರಿಸು = g.ಕೊನೆ - INDENT_HEURISTIC_MAX_SLIDING;
			ಗಾಗಿ (; ಸರಿಸು <= g.ಕೊನೆ; ಸರಿಸು++) {
				ರಚನೆ ವಿಭಜಿಸು_measurement m;
				ರಚನೆ ವಿಭಜಿಸು_score score = {0, 0};

				measure_ವಿಭಜಿಸು(xdf, ಸರಿಸು, &m);
				score_ಕೂಡಿಸು_ವಿಭಜಿಸು(&m, &score);
				measure_ವಿಭಜಿಸು(xdf, ಸರಿಸು - groupsize, &m);
				score_ಕೂಡಿಸು_ವಿಭಜಿಸು(&m, &score);
				ಆದರೆ (ಉತ್ತಮ_ಸರಿಸು == -1 ||
				    score_cmp(&score, &ಉತ್ತಮ_score) <= 0) {
					ಉತ್ತಮ_score.effective_indent = score.effective_indent;
					ಉತ್ತಮ_score.penalty = score.penalty;
					ಉತ್ತಮ_ಸರಿಸು = ಸರಿಸು;
				}
			}

			ಆಗ (g.ಕೊನೆ > ಉತ್ತಮ_ಸರಿಸು) {
				ಆದರೆ (ಗುಂಪು_slide_ಮೇಲೆ(xdf, &g))
					xdl_bug("best shift unreached");
				ಆದರೆ (ಗುಂಪು_ಹಿಂದಿನ(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	ಮುಂದಿನ:
		/* Move past the just-processed group: */
		ಆದರೆ (ಗುಂಪು_ಮುಂದಿನ(xdf, &g))
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (ಗುಂಪು_ಮುಂದಿನ(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	ಆದರೆ (!ಗುಂಪು_ಮುಂದಿನ(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	ಹಿಂತಿರುಗಿ 0;
}


ಪೂರ್ಣಾಂಕ xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = ಖಾಲಿ, *xch;
	bool *changed1 = xe->xdf1.ಬದಲಾಯಿತು, *changed2 = xe->xdf2.ಬದಲಾಯಿತು;
	ದೀರ್ಘ i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	ಗಾಗಿ (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		ಆದರೆ (changed1[i1 - 1] || changed2[i2 - 1]) {
			ಗಾಗಿ (l1 = i1; changed1[i1 - 1]; i1--);
			ಗಾಗಿ (l2 = i2; changed2[i2 - 1]; i2--);

			ಆದರೆ (!(xch = xdl_ಕೂಡಿಸು_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				ಹಿಂತಿರುಗಿ -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	ಹಿಂತಿರುಗಿ 0;
}


ಶೂನ್ಯ xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	ಆಗ ((xch = xscr) != ಖಾಲಿ) {
		xscr = xscr->ಮುಂದಿನ;
		xdl_free(xch);
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_call_hunk_func(xdfenv_t *xe ಬಳಸದ, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg)
{
	xdchange_t *xch, *xche;

	ಗಾಗಿ (xch = xscr; xch; xch = xche->ಮುಂದಿನ) {
		xche = xdl_get_hunk(&xch, xecfg);
		ಆದರೆ (!xch)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ xdl_mark_ignorable_ಸಾಲುಗಳು(xdchange_t *xscr, xdfenv_t *xe, ದೀರ್ಘ flags)
{
	xdchange_t *xch;

	ಗಾಗಿ (xch = xscr; xch; xch = xch->ಮುಂದಿನ) {
		ಪೂರ್ಣಾಂಕ ignore = 1;
		xrecord_t *rec;
		ದೀರ್ಘ i;

		rec = &xe->xdf1.recs[xch->i1];
		ಗಾಗಿ (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].ಗಾತ್ರ_2, flags);

		rec = &xe->xdf2.recs[xch->i2];
		ಗಾಗಿ (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].ಗಾತ್ರ_2, flags);

		xch->ignore = ignore;
	}
}

#ಆದರೆ 0 // unused by Vim
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ದಾಖಲೆ_matches_regex(xrecord_t *rec, xpparam_t ಸ್ಥಿರಾಂಕ *xpp) {
	regmatch_t regmatch;
	size_t i;

	ಗಾಗಿ (i = 0; i < xpp->ignore_regex_nr; i++)
		ಆದರೆ (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->ಗಾತ್ರ_2, 1,
				 &regmatch, 0))
			ಹಿಂತಿರುಗಿ 1;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ xdl_mark_ignorable_regex(xdchange_t *xscr, ಸ್ಥಿರಾಂಕ xdfenv_t *xe,
				     xpparam_t ಸ್ಥಿರಾಂಕ *xpp)
{
	xdchange_t *xch;

	ಗಾಗಿ (xch = xscr; xch; xch = xch->ಮುಂದಿನ) {
		xrecord_t *rec;
		ಪೂರ್ಣಾಂಕ ignore = 1;
		ದೀರ್ಘ i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		ಆದರೆ (xch->ignore)
			ಮುಂದುವರಿ;

		rec = &xe->xdf1.recs[xch->i1];
		ಗಾಗಿ (i = 0; i < xch->chg1 && ignore; i++)
			ignore = ದಾಖಲೆ_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		ಗಾಗಿ (i = 0; i < xch->chg2 && ignore; i++)
			ignore = ದಾಖಲೆ_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
	     xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	ಆದರೆ (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (xscr) {
		ಆದರೆ (xpp->flags & XDF_IGNORE_BLANK_ಸಾಲುಗಳು)
			xdl_mark_ignorable_ಸಾಲುಗಳು(xscr, &xe, xpp->flags);

#ಆದರೆ 0
		ಆದರೆ (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			ಹಿಂತಿರುಗಿ -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	ಹಿಂತಿರುಗಿ 0;
}
