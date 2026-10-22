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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ get_hash(xdfile_t *xdf, ਲੰਮਾ ਸੂਚਕਾਂਕ)
{
	ਵਾਪਸ xdf->recs[xdf->rindex[ਸੂਚਕਾਂਕ]].ha;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_MAX_ਲਾਗਤ_MIN 256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_HEUR_MIN_ਲਾਗਤ 256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਸਤਰ_MAX (ਲੰਮਾ)((1UL << (CHAR_BIT * ਆਕਾਰ(ਲੰਮਾ) - 1)) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_SNAKE_CNT 20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_K_HEUR 4

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdpsplit {
	ਲੰਮਾ i1, i2;
	ਪੂਰਨ min_lo, min_hi;
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
ਸਥਿਰ ਲੰਮਾ xdl_ਵੰਡੋ(xdfile_t *xdf1, ਲੰਮਾ off1, ਲੰਮਾ lim1,
		      xdfile_t *xdf2, ਲੰਮਾ off2, ਲੰਮਾ lim2,
		      ਲੰਮਾ *kvdf, ਲੰਮਾ *kvdb, ਪੂਰਨ need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	ਲੰਮਾ dmin = off1 - lim2, dmax = lim1 - off2;
	ਲੰਮਾ fmid = off1 - off2, bmid = lim1 - lim2;
	ਲੰਮਾ odd = (fmid - bmid) & 1;
	ਲੰਮਾ fmin = fmid, fmax = fmid;
	ਲੰਮਾ bmin = bmid, bmax = bmid;
	ਲੰਮਾ ec, d, i1, i2, prev1, ਉੱਤਮ, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	ਲਈ (ec = 1;; ec++) {
		ਪੂਰਨ got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		ਜੇ (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		ਨਹੀਂਤਾਂ
			++fmin;
		ਜੇ (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		ਨਹੀਂਤਾਂ
			--fmax;

		ਲਈ (d = fmax; d >= fmin; d -= 2) {
			ਜੇ (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			ਨਹੀਂਤਾਂ
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			ਲਈ (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			ਜੇ (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			ਜੇ (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ਵਾਪਸ ec;
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
		ਜੇ (bmin > dmin)
			kvdb[--bmin - 1] = XDL_ਸਤਰ_MAX;
		ਨਹੀਂਤਾਂ
			++bmin;
		ਜੇ (bmax < dmax)
			kvdb[++bmax + 1] = XDL_ਸਤਰ_MAX;
		ਨਹੀਂਤਾਂ
			--bmax;

		ਲਈ (d = bmax; d >= bmin; d -= 2) {
			ਜੇ (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			ਨਹੀਂਤਾਂ
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			ਲਈ (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			ਜੇ (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			ਜੇ (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ਵਾਪਸ ec;
			}
		}

		ਜੇ (need_min)
			ਜਾਰੀ;

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
		ਜੇ (got_snake && ec > xenv->heur_min) {
			ਲਈ (ਉੱਤਮ = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				ਜੇ (v > XDL_K_HEUR * ec && v > ਉੱਤਮ &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					ਲਈ (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						ਜੇ (k == xenv->snake_cnt) {
							ਉੱਤਮ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ਰੋਕੋ;
						}
				}
			}
			ਜੇ (ਉੱਤਮ > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				ਵਾਪਸ ec;
			}

			ਲਈ (ਉੱਤਮ = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				ਜੇ (v > XDL_K_HEUR * ec && v > ਉੱਤਮ &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					ਲਈ (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						ਜੇ (k == xenv->snake_cnt - 1) {
							ਉੱਤਮ = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ਰੋਕੋ;
						}
				}
			}
			ਜੇ (ਉੱਤਮ > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				ਵਾਪਸ ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		ਜੇ (ec >= xenv->mxcost) {
			ਲੰਮਾ fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			ਲਈ (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				ਜੇ (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				ਜੇ (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_ਸਤਰ_MAX;
			ਲਈ (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				ਜੇ (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				ਜੇ (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			ਜੇ ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} ਨਹੀਂਤਾਂ {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			ਵਾਪਸ ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
ਪੂਰਨ xdl_recs_cmp(xdfile_t *xdf1, ਲੰਮਾ off1, ਲੰਮਾ lim1,
		 xdfile_t *xdf2, ਲੰਮਾ off2, ਲੰਮਾ lim2,
		 ਲੰਮਾ *kvdf, ਲੰਮਾ *kvdb, ਪੂਰਨ need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	ਲਈ (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	ਲਈ (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	ਜੇ (off1 == lim1) {
		ਲਈ (; off2 < lim2; off2++)
			xdf2->ਬਦਲਿਆ[xdf2->rindex[off2]] = true;
	} ਨਹੀਂਤਾਂ ਜੇ (off2 == lim2) {
		ਲਈ (; off1 < lim1; off1++)
			xdf1->ਬਦਲਿਆ[xdf1->rindex[off1]] = true;
	} ਨਹੀਂਤਾਂ {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		ਜੇ (xdl_ਵੰਡੋ(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			ਵਾਪਸ -1;
		}

		/*
		 * ... et Impera.
		 */
		ਜੇ (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			ਵਾਪਸ -1;
		}
	}

	ਵਾਪਸ 0;
}


ਪੂਰਨ xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ਅਚਲ *xpp,
		xdfenv_t *xe) {
	ਲੰਮਾ ndiags;
	ਲੰਮਾ *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	ਪੂਰਨ res;

	ਜੇ (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		ਵਾਪਸ -1;

	ਜੇ (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	ਜੇ (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	ਜੇ (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		ਵਾਪਸ -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	ਜੇ (xenv.mxcost < XDL_MAX_ਲਾਗਤ_MIN)
		xenv.mxcost = XDL_MAX_ਲਾਗਤ_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_ਲਾਗਤ;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_ਨਿਊਨਤਮ) != 0,
			   &xenv);
	xdl_free(kvd);
 ਬਾਹਰਲਾ:
	ਜੇ (res < 0)
		xdl_free_env(xe);

	ਵਾਪਸ res;
}


ਸਥਿਰ xdchange_t *xdl_ਜੋੜੋ_change(xdchange_t *xscr, ਲੰਮਾ i1, ਲੰਮਾ i2, ਲੰਮਾ chg1, ਲੰਮਾ chg2) {
	xdchange_t *xch;

	ਜੇ (!(xch = (xdchange_t *) xdl_malloc(ਆਕਾਰ(xdchange_t))))
		ਵਾਪਸ ਨਿਰਾ;

	xch->ਅਗਲਾ = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	ਵਾਪਸ xch;
}


ਸਥਿਰ ਪੂਰਨ recs_ਮੇਲ(xrecord_t *rec1, xrecord_t *rec2)
{
	ਵਾਪਸ (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
ਸਥਿਰ ਪੂਰਨ xget_indent(xrecord_t *rec)
{
	ਲੰਮਾ i;
	ਪੂਰਨ ret = 0;

	ਲਈ (i = 0; i < rec->ਮਾਪ; i++) {
		ਅੱਖਰ c = rec->ptr[i];

		ਜੇ (!XDL_ISSPACE(c))
			ਵਾਪਸ ret;
		ਨਹੀਂਤਾਂ ਜੇ (c == ' ')
			ret += 1;
		ਨਹੀਂਤਾਂ ਜੇ (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		ਜੇ (ret >= MAX_INDENT)
			ਵਾਪਸ MAX_INDENT;
	}

	/* The line contains only whitespace. */
	ਵਾਪਸ -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
ਬਣਤਰ ਵੰਡੋ_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	ਪੂਰਨ ਸਿਰਾ_ਦਾ_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	ਪੂਰਨ indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	ਪੂਰਨ pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	ਪੂਰਨ pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	ਪੂਰਨ post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	ਪੂਰਨ post_indent;
};

ਬਣਤਰ ਵੰਡੋ_score {
	/* The effective indent of this split (smaller is preferred). */
	ਪੂਰਨ effective_indent;

	/* Penalty for this split (smaller is preferred). */
	ਪੂਰਨ penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
ਸਥਿਰ ਖਾਲੀ measure_ਵੰਡੋ(ਅਚਲ xdfile_t *xdf, ਲੰਮਾ ਵੰਡੋ,
			  ਬਣਤਰ ਵੰਡੋ_measurement *m)
{
	ਲੰਮਾ i;

	ਜੇ (ਵੰਡੋ >= xdf->nrec) {
		m->ਸਿਰਾ_ਦਾ_file = 1;
		m->indent = -1;
	} ਨਹੀਂਤਾਂ {
		m->ਸਿਰਾ_ਦਾ_file = 0;
		m->indent = xget_indent(&xdf->recs[ਵੰਡੋ]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	ਲਈ (i = ਵੰਡੋ - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		ਜੇ (m->pre_indent != -1)
			ਰੋਕੋ;
		m->pre_blank += 1;
		ਜੇ (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			ਰੋਕੋ;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	ਲਈ (i = ਵੰਡੋ + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		ਜੇ (m->post_indent != -1)
			ਰੋਕੋ;
		m->post_blank += 1;
		ਜੇ (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			ਰੋਕੋ;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸ਼ੁਰੂ_ਦਾ_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_ਦਾ_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੱਲ_BLANK_ਭਾਰ (-30)

/* Multiplier for the number of blank lines after the split */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POST_BLANK_ਭਾਰ 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_INDENT_PENALTY (-4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_OUTDENT_PENALTY 24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_DEDENT_PENALTY 23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENT_ਭਾਰ 60

/*
 * How far do we slide a hunk at most?
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENT_HEURISTIC_MAX_SLIDING 100

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
ਸਥਿਰ ਖਾਲੀ score_ਜੋੜੋ_ਵੰਡੋ(ਅਚਲ ਬਣਤਰ ਵੰਡੋ_measurement *m, ਬਣਤਰ ਵੰਡੋ_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	ਪੂਰਨ post_blank, ਕੁੱਲ_blank, indent, any_blanks;

	ਜੇ (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += ਸ਼ੁਰੂ_ਦਾ_FILE_PENALTY;

	ਜੇ (m->ਸਿਰਾ_ਦਾ_file)
		s->penalty += ਸਿਰਾ_ਦਾ_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	ਕੁੱਲ_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += ਕੁੱਲ_BLANK_ਭਾਰ * ਕੁੱਲ_blank;
	s->penalty += POST_BLANK_ਭਾਰ * post_blank;

	ਜੇ (m->indent != -1)
		indent = m->indent;
	ਨਹੀਂਤਾਂ
		indent = m->post_indent;

	any_blanks = (ਕੁੱਲ_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	ਜੇ (indent == -1) {
		/* No additional adjustments needed. */
	} ਨਹੀਂਤਾਂ ਜੇ (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} ਨਹੀਂਤਾਂ ਜੇ (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} ਨਹੀਂਤਾਂ ਜੇ (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} ਨਹੀਂਤਾਂ {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		ਜੇ (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} ਨਹੀਂਤਾਂ {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

ਸਥਿਰ ਪੂਰਨ score_cmp(ਬਣਤਰ ਵੰਡੋ_score *s1, ਬਣਤਰ ਵੰਡੋ_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	ਪੂਰਨ cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	ਵਾਪਸ INDENT_ਭਾਰ * cmp_indents + (s1->penalty - s2->penalty);
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
ਬਣਤਰ xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	ਲੰਮਾ ਸ਼ੁਰੂ;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	ਲੰਮਾ ਸਿਰਾ;
};

/*
 * Initialize g to point at the first group in xdf.
 */
ਸਥਿਰ ਖਾਲੀ ਸਮੂਹ_init(xdfile_t *xdf, ਬਣਤਰ xdlgroup *g)
{
	g->ਸ਼ੁਰੂ = g->ਸਿਰਾ = 0;
	ਜਦੋਂ (xdf->ਬਦਲਿਆ[g->ਸਿਰਾ])
		g->ਸਿਰਾ++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸਮੂਹ_ਅਗਲਾ(xdfile_t *xdf, ਬਣਤਰ xdlgroup *g)
{
	ਜੇ (g->ਸਿਰਾ == xdf->nrec)
		ਵਾਪਸ -1;

	g->ਸ਼ੁਰੂ = g->ਸਿਰਾ + 1;
	ਲਈ (g->ਸਿਰਾ = g->ਸ਼ੁਰੂ; xdf->ਬਦਲਿਆ[g->ਸਿਰਾ]; g->ਸਿਰਾ++)
		;

	ਵਾਪਸ 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸਮੂਹ_ਪਿਛਲਾ(xdfile_t *xdf, ਬਣਤਰ xdlgroup *g)
{
	ਜੇ (g->ਸ਼ੁਰੂ == 0)
		ਵਾਪਸ -1;

	g->ਸਿਰਾ = g->ਸ਼ੁਰੂ - 1;
	ਲਈ (g->ਸ਼ੁਰੂ = g->ਸਿਰਾ; xdf->ਬਦਲਿਆ[g->ਸ਼ੁਰੂ - 1]; g->ਸ਼ੁਰੂ--)
		;

	ਵਾਪਸ 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
ਸਥਿਰ ਪੂਰਨ ਸਮੂਹ_slide_ਥੱਲੇ(xdfile_t *xdf, ਬਣਤਰ xdlgroup *g)
{
	ਜੇ (g->ਸਿਰਾ < xdf->nrec &&
	    recs_ਮੇਲ(&xdf->recs[g->ਸ਼ੁਰੂ], &xdf->recs[g->ਸਿਰਾ])) {
		xdf->ਬਦਲਿਆ[g->ਸ਼ੁਰੂ++] = false;
		xdf->ਬਦਲਿਆ[g->ਸਿਰਾ++] = true;

		ਜਦੋਂ (xdf->ਬਦਲਿਆ[g->ਸਿਰਾ])
			g->ਸਿਰਾ++;

		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
ਸਥਿਰ ਪੂਰਨ ਸਮੂਹ_slide_ਉੱਪਰ(xdfile_t *xdf, ਬਣਤਰ xdlgroup *g)
{
	ਜੇ (g->ਸ਼ੁਰੂ > 0 &&
	    recs_ਮੇਲ(&xdf->recs[g->ਸ਼ੁਰੂ - 1], &xdf->recs[g->ਸਿਰਾ - 1])) {
		xdf->ਬਦਲਿਆ[--g->ਸ਼ੁਰੂ] = true;
		xdf->ਬਦਲਿਆ[--g->ਸਿਰਾ] = false;

		ਜਦੋਂ (xdf->ਬਦਲਿਆ[g->ਸ਼ੁਰੂ - 1])
			g->ਸ਼ੁਰੂ--;

		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -1;
	}
}

ਖਾਲੀ xdl_bug(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
	fprintf(stderr, "BUG: %s\n", ਸੁਨੇਹਾ);
	ਬਾਹਰ_ਨਿਕਲੋ(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
ਪੂਰਨ xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, ਲੰਮਾ flags) {
	ਬਣਤਰ xdlgroup g, go;
	ਲੰਮਾ earliest_ਸਿਰਾ, ਸਿਰਾ_matching_other;
	ਲੰਮਾ groupsize;

	ਸਮੂਹ_init(xdf, &g);
	ਸਮੂਹ_init(xdfo, &go);

	ਜਦੋਂ (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		ਜੇ (g.ਸਿਰਾ == g.ਸ਼ੁਰੂ)
			ਜਾਓ ਅਗਲਾ;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		ਕਰੋ {
			groupsize = g.ਸਿਰਾ - g.ਸ਼ੁਰੂ;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			ਸਿਰਾ_matching_other = -1;

			/* Shift the group backward as much as possible: */
			ਜਦੋਂ (!ਸਮੂਹ_slide_ਉੱਪਰ(xdf, &g))
				ਜੇ (ਸਮੂਹ_ਪਿਛਲਾ(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_ਸਿਰਾ = g.ਸਿਰਾ;

			ਜੇ (go.ਸਿਰਾ > go.ਸ਼ੁਰੂ)
				ਸਿਰਾ_matching_other = g.ਸਿਰਾ;

			/* Now shift the group forward as far as possible: */
			ਜਦੋਂ (1) {
				ਜੇ (ਸਮੂਹ_slide_ਥੱਲੇ(xdf, &g))
					ਰੋਕੋ;
				ਜੇ (ਸਮੂਹ_ਅਗਲਾ(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				ਜੇ (go.ਸਿਰਾ > go.ਸ਼ੁਰੂ)
					ਸਿਰਾ_matching_other = g.ਸਿਰਾ;
			}
		} ਜਦੋਂ (groupsize != g.ਸਿਰਾ - g.ਸ਼ੁਰੂ);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		ਜੇ (g.ਸਿਰਾ == earliest_ਸਿਰਾ) {
			/* no shifting was possible */
		} ਨਹੀਂਤਾਂ ਜੇ (ਸਿਰਾ_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			ਜਦੋਂ (go.ਸਿਰਾ == go.ਸ਼ੁਰੂ) {
				ਜੇ (ਸਮੂਹ_slide_ਉੱਪਰ(xdf, &g))
					xdl_bug("match disappeared");
				ਜੇ (ਸਮੂਹ_ਪਿਛਲਾ(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_INDENT_HEURISTIC) {
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
			ਲੰਮਾ ਖਿਸਕਾਓ, ਉੱਤਮ_ਖਿਸਕਾਓ = -1;
			ਬਣਤਰ ਵੰਡੋ_score ਉੱਤਮ_score;

			ਖਿਸਕਾਓ = earliest_ਸਿਰਾ;
			ਜੇ (g.ਸਿਰਾ - groupsize - 1 > ਖਿਸਕਾਓ)
				ਖਿਸਕਾਓ = g.ਸਿਰਾ - groupsize - 1;
			ਜੇ (g.ਸਿਰਾ - INDENT_HEURISTIC_MAX_SLIDING > ਖਿਸਕਾਓ)
				ਖਿਸਕਾਓ = g.ਸਿਰਾ - INDENT_HEURISTIC_MAX_SLIDING;
			ਲਈ (; ਖਿਸਕਾਓ <= g.ਸਿਰਾ; ਖਿਸਕਾਓ++) {
				ਬਣਤਰ ਵੰਡੋ_measurement m;
				ਬਣਤਰ ਵੰਡੋ_score score = {0, 0};

				measure_ਵੰਡੋ(xdf, ਖਿਸਕਾਓ, &m);
				score_ਜੋੜੋ_ਵੰਡੋ(&m, &score);
				measure_ਵੰਡੋ(xdf, ਖਿਸਕਾਓ - groupsize, &m);
				score_ਜੋੜੋ_ਵੰਡੋ(&m, &score);
				ਜੇ (ਉੱਤਮ_ਖਿਸਕਾਓ == -1 ||
				    score_cmp(&score, &ਉੱਤਮ_score) <= 0) {
					ਉੱਤਮ_score.effective_indent = score.effective_indent;
					ਉੱਤਮ_score.penalty = score.penalty;
					ਉੱਤਮ_ਖਿਸਕਾਓ = ਖਿਸਕਾਓ;
				}
			}

			ਜਦੋਂ (g.ਸਿਰਾ > ਉੱਤਮ_ਖਿਸਕਾਓ) {
				ਜੇ (ਸਮੂਹ_slide_ਉੱਪਰ(xdf, &g))
					xdl_bug("best shift unreached");
				ਜੇ (ਸਮੂਹ_ਪਿਛਲਾ(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	ਅਗਲਾ:
		/* Move past the just-processed group: */
		ਜੇ (ਸਮੂਹ_ਅਗਲਾ(xdf, &g))
			ਰੋਕੋ;
		ਜੇ (ਸਮੂਹ_ਅਗਲਾ(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	ਜੇ (!ਸਮੂਹ_ਅਗਲਾ(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	ਵਾਪਸ 0;
}


ਪੂਰਨ xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = ਨਿਰਾ, *xch;
	bool *changed1 = xe->xdf1.ਬਦਲਿਆ, *changed2 = xe->xdf2.ਬਦਲਿਆ;
	ਲੰਮਾ i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	ਲਈ (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		ਜੇ (changed1[i1 - 1] || changed2[i2 - 1]) {
			ਲਈ (l1 = i1; changed1[i1 - 1]; i1--);
			ਲਈ (l2 = i2; changed2[i2 - 1]; i2--);

			ਜੇ (!(xch = xdl_ਜੋੜੋ_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				ਵਾਪਸ -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	ਵਾਪਸ 0;
}


ਖਾਲੀ xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	ਜਦੋਂ ((xch = xscr) != ਨਿਰਾ) {
		xscr = xscr->ਅਗਲਾ;
		xdl_free(xch);
	}
}

ਸਥਿਰ ਪੂਰਨ xdl_call_hunk_func(xdfenv_t *xe ਅਣਵਰਤਿਆ, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t ਅਚਲ *xecfg)
{
	xdchange_t *xch, *xche;

	ਲਈ (xch = xscr; xch; xch = xche->ਅਗਲਾ) {
		xche = xdl_get_hunk(&xch, xecfg);
		ਜੇ (!xch)
			ਰੋਕੋ;
		ਜੇ (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			ਵਾਪਸ -1;
	}
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ xdl_mark_ignorable_ਸਤਰਾਂ(xdchange_t *xscr, xdfenv_t *xe, ਲੰਮਾ flags)
{
	xdchange_t *xch;

	ਲਈ (xch = xscr; xch; xch = xch->ਅਗਲਾ) {
		ਪੂਰਨ ignore = 1;
		xrecord_t *rec;
		ਲੰਮਾ i;

		rec = &xe->xdf1.recs[xch->i1];
		ਲਈ (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].ਮਾਪ, flags);

		rec = &xe->xdf2.recs[xch->i2];
		ਲਈ (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].ਮਾਪ, flags);

		xch->ignore = ignore;
	}
}

#ਜੇ 0 // unused by Vim
ਸਥਿਰ ਪੂਰਨ ਰਿਕਾਰਡ_matches_regex(xrecord_t *rec, xpparam_t ਅਚਲ *xpp) {
	regmatch_t regmatch;
	size_t i;

	ਲਈ (i = 0; i < xpp->ignore_regex_nr; i++)
		ਜੇ (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->ਮਾਪ, 1,
				 &regmatch, 0))
			ਵਾਪਸ 1;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ xdl_mark_ignorable_regex(xdchange_t *xscr, ਅਚਲ xdfenv_t *xe,
				     xpparam_t ਅਚਲ *xpp)
{
	xdchange_t *xch;

	ਲਈ (xch = xscr; xch; xch = xch->ਅਗਲਾ) {
		xrecord_t *rec;
		ਪੂਰਨ ignore = 1;
		ਲੰਮਾ i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		ਜੇ (xch->ignore)
			ਜਾਰੀ;

		rec = &xe->xdf1.recs[xch->i1];
		ਲਈ (i = 0; i < xch->chg1 && ignore; i++)
			ignore = ਰਿਕਾਰਡ_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		ਲਈ (i = 0; i < xch->chg2 && ignore; i++)
			ignore = ਰਿਕਾਰਡ_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ਅਚਲ *xpp,
	     xdemitconf_t ਅਚਲ *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	ਜੇ (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		ਵਾਪਸ -1;
	}
	ਜੇ (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		ਵਾਪਸ -1;
	}
	ਜੇ (xscr) {
		ਜੇ (xpp->flags & XDF_IGNORE_BLANK_ਸਤਰਾਂ)
			xdl_mark_ignorable_ਸਤਰਾਂ(xscr, &xe, xpp->flags);

#ਜੇ 0
		ਜੇ (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#ਜੇ_ਅੰਤ

		ਜੇ (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			ਵਾਪਸ -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	ਵਾਪਸ 0;
}
