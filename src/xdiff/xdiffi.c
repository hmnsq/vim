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

#അന്തര്ഭാവ "xinclude.h"

സ്ഥിതം unsigned long get_hash(xdfile_t *xdf, long index)
{
	തിരികെ xdf->recs[xdf->rindex[index]].ha;
}

#define XDL_MAX_COST_MIN 256
#define XDL_HEUR_MIN_COST 256
#define XDL_വരി_MAX (long)((1UL << (CHAR_BIT * വലുപ്പം(long) - 1)) - 1)
#define XDL_SNAKE_CNT 20
#define XDL_K_HEUR 4

തരനിർവചനം ഘടന s_xdpsplit {
	long i1, i2;
	പൂർണ്ണസംഖ്യ min_lo, min_hi;
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
സ്ഥിതം long xdl_split(xdfile_t *xdf1, long off1, long lim1,
		      xdfile_t *xdf2, long off2, long lim2,
		      long *kvdf, long *kvdb, പൂർണ്ണസംഖ്യ need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	long dmin = off1 - lim2, dmax = lim1 - off2;
	long fmid = off1 - off2, bmid = lim1 - lim2;
	long odd = (fmid - bmid) & 1;
	long fmin = fmid, fmax = fmid;
	long bmin = bmid, bmax = bmid;
	long ec, d, i1, i2, prev1, best, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	വേണ്ടി (ec = 1;; ec++) {
		പൂർണ്ണസംഖ്യ got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		എങ്കിൽ (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		അല്ലെങ്കിൽ
			++fmin;
		എങ്കിൽ (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		അല്ലെങ്കിൽ
			--fmax;

		വേണ്ടി (d = fmax; d >= fmin; d -= 2) {
			എങ്കിൽ (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			അല്ലെങ്കിൽ
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			വേണ്ടി (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			എങ്കിൽ (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			എങ്കിൽ (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				തിരികെ ec;
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
		എങ്കിൽ (bmin > dmin)
			kvdb[--bmin - 1] = XDL_വരി_MAX;
		അല്ലെങ്കിൽ
			++bmin;
		എങ്കിൽ (bmax < dmax)
			kvdb[++bmax + 1] = XDL_വരി_MAX;
		അല്ലെങ്കിൽ
			--bmax;

		വേണ്ടി (d = bmax; d >= bmin; d -= 2) {
			എങ്കിൽ (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			അല്ലെങ്കിൽ
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			വേണ്ടി (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			എങ്കിൽ (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			എങ്കിൽ (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				തിരികെ ec;
			}
		}

		എങ്കിൽ (need_min)
			തുടരുക;

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
		എങ്കിൽ (got_snake && ec > xenv->heur_min) {
			വേണ്ടി (best = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				എങ്കിൽ (v > XDL_K_HEUR * ec && v > best &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					വേണ്ടി (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						എങ്കിൽ (k == xenv->snake_cnt) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							നിർത്തുക;
						}
				}
			}
			എങ്കിൽ (best > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				തിരികെ ec;
			}

			വേണ്ടി (best = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				എങ്കിൽ (v > XDL_K_HEUR * ec && v > best &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					വേണ്ടി (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						എങ്കിൽ (k == xenv->snake_cnt - 1) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							നിർത്തുക;
						}
				}
			}
			എങ്കിൽ (best > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				തിരികെ ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		എങ്കിൽ (ec >= xenv->mxcost) {
			long fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			വേണ്ടി (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				എങ്കിൽ (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				എങ്കിൽ (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_വരി_MAX;
			വേണ്ടി (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				എങ്കിൽ (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				എങ്കിൽ (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			എങ്കിൽ ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} അല്ലെങ്കിൽ {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			തിരികെ ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
പൂർണ്ണസംഖ്യ xdl_recs_cmp(xdfile_t *xdf1, long off1, long lim1,
		 xdfile_t *xdf2, long off2, long lim2,
		 long *kvdf, long *kvdb, പൂർണ്ണസംഖ്യ need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	വേണ്ടി (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	വേണ്ടി (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	എങ്കിൽ (off1 == lim1) {
		വേണ്ടി (; off2 < lim2; off2++)
			xdf2->changed[xdf2->rindex[off2]] = true;
	} അല്ലെങ്കിൽ എങ്കിൽ (off2 == lim2) {
		വേണ്ടി (; off1 < lim1; off1++)
			xdf1->changed[xdf1->rindex[off1]] = true;
	} അല്ലെങ്കിൽ {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		എങ്കിൽ (xdl_split(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			തിരികെ -1;
		}

		/*
		 * ... et Impera.
		 */
		എങ്കിൽ (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			തിരികെ -1;
		}
	}

	തിരികെ 0;
}


പൂർണ്ണസംഖ്യ xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t സ്ഥിരം *xpp,
		xdfenv_t *xe) {
	long ndiags;
	long *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	പൂർണ്ണസംഖ്യ res;

	എങ്കിൽ (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		തിരികെ -1;

	എങ്കിൽ (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		പോകുക out;
	}

	എങ്കിൽ (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		പോകുക out;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	എങ്കിൽ (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		തിരികെ -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	എങ്കിൽ (xenv.mxcost < XDL_MAX_COST_MIN)
		xenv.mxcost = XDL_MAX_COST_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_COST;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_MINIMAL) != 0,
			   &xenv);
	xdl_free(kvd);
 out:
	എങ്കിൽ (res < 0)
		xdl_free_env(xe);

	തിരികെ res;
}


സ്ഥിതം xdchange_t *xdl_add_change(xdchange_t *xscr, long i1, long i2, long chg1, long chg2) {
	xdchange_t *xch;

	എങ്കിൽ (!(xch = (xdchange_t *) xdl_malloc(വലുപ്പം(xdchange_t))))
		തിരികെ ശൂന്യ;

	xch->next = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	തിരികെ xch;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ recs_match(xrecord_t *rec1, xrecord_t *rec2)
{
	തിരികെ (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#define MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ xget_indent(xrecord_t *rec)
{
	long i;
	പൂർണ്ണസംഖ്യ ret = 0;

	വേണ്ടി (i = 0; i < rec->size; i++) {
		അക്ഷരം c = rec->ptr[i];

		എങ്കിൽ (!XDL_ISSPACE(c))
			തിരികെ ret;
		അല്ലെങ്കിൽ എങ്കിൽ (c == ' ')
			ret += 1;
		അല്ലെങ്കിൽ എങ്കിൽ (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		എങ്കിൽ (ret >= MAX_INDENT)
			തിരികെ MAX_INDENT;
	}

	/* The line contains only whitespace. */
	തിരികെ -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#define MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
ഘടന split_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	പൂർണ്ണസംഖ്യ end_of_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	പൂർണ്ണസംഖ്യ indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	പൂർണ്ണസംഖ്യ pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	പൂർണ്ണസംഖ്യ pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	പൂർണ്ണസംഖ്യ post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	പൂർണ്ണസംഖ്യ post_indent;
};

ഘടന split_score {
	/* The effective indent of this split (smaller is preferred). */
	പൂർണ്ണസംഖ്യ effective_indent;

	/* Penalty for this split (smaller is preferred). */
	പൂർണ്ണസംഖ്യ penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
സ്ഥിതം ശൂന്യം measure_split(സ്ഥിരം xdfile_t *xdf, long split,
			  ഘടന split_measurement *m)
{
	long i;

	എങ്കിൽ (split >= xdf->nrec) {
		m->end_of_file = 1;
		m->indent = -1;
	} അല്ലെങ്കിൽ {
		m->end_of_file = 0;
		m->indent = xget_indent(&xdf->recs[split]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	വേണ്ടി (i = split - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		എങ്കിൽ (m->pre_indent != -1)
			നിർത്തുക;
		m->pre_blank += 1;
		എങ്കിൽ (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			നിർത്തുക;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	വേണ്ടി (i = split + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		എങ്കിൽ (m->post_indent != -1)
			നിർത്തുക;
		m->post_blank += 1;
		എങ്കിൽ (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			നിർത്തുക;
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
#define START_OF_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#define END_OF_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#define TOTAL_BLANK_WEIGHT (-30)

/* Multiplier for the number of blank lines after the split */
#define POST_BLANK_WEIGHT 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#define RELATIVE_INDENT_PENALTY (-4)
#define RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#define RELATIVE_OUTDENT_PENALTY 24
#define RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#define RELATIVE_DEDENT_PENALTY 23
#define RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#define INDENT_WEIGHT 60

/*
 * How far do we slide a hunk at most?
 */
#define INDENT_HEURISTIC_MAX_SLIDING 100

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
സ്ഥിതം ശൂന്യം score_add_split(സ്ഥിരം ഘടന split_measurement *m, ഘടന split_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	പൂർണ്ണസംഖ്യ post_blank, total_blank, indent, any_blanks;

	എങ്കിൽ (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += START_OF_FILE_PENALTY;

	എങ്കിൽ (m->end_of_file)
		s->penalty += END_OF_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	total_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += TOTAL_BLANK_WEIGHT * total_blank;
	s->penalty += POST_BLANK_WEIGHT * post_blank;

	എങ്കിൽ (m->indent != -1)
		indent = m->indent;
	അല്ലെങ്കിൽ
		indent = m->post_indent;

	any_blanks = (total_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	എങ്കിൽ (indent == -1) {
		/* No additional adjustments needed. */
	} അല്ലെങ്കിൽ എങ്കിൽ (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} അല്ലെങ്കിൽ എങ്കിൽ (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} അല്ലെങ്കിൽ എങ്കിൽ (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} അല്ലെങ്കിൽ {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		എങ്കിൽ (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} അല്ലെങ്കിൽ {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ score_cmp(ഘടന split_score *s1, ഘടന split_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	പൂർണ്ണസംഖ്യ cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	തിരികെ INDENT_WEIGHT * cmp_indents + (s1->penalty - s2->penalty);
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
ഘടന xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	long start;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	long end;
};

/*
 * Initialize g to point at the first group in xdf.
 */
സ്ഥിതം ശൂന്യം group_init(xdfile_t *xdf, ഘടന xdlgroup *g)
{
	g->start = g->end = 0;
	ആകുമ്പോൾ (xdf->changed[g->end])
		g->end++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ group_next(xdfile_t *xdf, ഘടന xdlgroup *g)
{
	എങ്കിൽ (g->end == xdf->nrec)
		തിരികെ -1;

	g->start = g->end + 1;
	വേണ്ടി (g->end = g->start; xdf->changed[g->end]; g->end++)
		;

	തിരികെ 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ group_previous(xdfile_t *xdf, ഘടന xdlgroup *g)
{
	എങ്കിൽ (g->start == 0)
		തിരികെ -1;

	g->end = g->start - 1;
	വേണ്ടി (g->start = g->end; xdf->changed[g->start - 1]; g->start--)
		;

	തിരികെ 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ group_slide_down(xdfile_t *xdf, ഘടന xdlgroup *g)
{
	എങ്കിൽ (g->end < xdf->nrec &&
	    recs_match(&xdf->recs[g->start], &xdf->recs[g->end])) {
		xdf->changed[g->start++] = false;
		xdf->changed[g->end++] = true;

		ആകുമ്പോൾ (xdf->changed[g->end])
			g->end++;

		തിരികെ 0;
	} അല്ലെങ്കിൽ {
		തിരികെ -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ group_slide_up(xdfile_t *xdf, ഘടന xdlgroup *g)
{
	എങ്കിൽ (g->start > 0 &&
	    recs_match(&xdf->recs[g->start - 1], &xdf->recs[g->end - 1])) {
		xdf->changed[--g->start] = true;
		xdf->changed[--g->end] = false;

		ആകുമ്പോൾ (xdf->changed[g->start - 1])
			g->start--;

		തിരികെ 0;
	} അല്ലെങ്കിൽ {
		തിരികെ -1;
	}
}

ശൂന്യം xdl_bug(സ്ഥിരം അക്ഷരം *സന്ദേശം)
{
	fprintf(stderr, "BUG: %s\n", സന്ദേശം);
	exit(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
പൂർണ്ണസംഖ്യ xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, long flags) {
	ഘടന xdlgroup g, go;
	long earliest_end, end_matching_other;
	long groupsize;

	group_init(xdf, &g);
	group_init(xdfo, &go);

	ആകുമ്പോൾ (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		എങ്കിൽ (g.end == g.start)
			പോകുക next;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		ചെയ്യുക {
			groupsize = g.end - g.start;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			end_matching_other = -1;

			/* Shift the group backward as much as possible: */
			ആകുമ്പോൾ (!group_slide_up(xdf, &g))
				എങ്കിൽ (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_end = g.end;

			എങ്കിൽ (go.end > go.start)
				end_matching_other = g.end;

			/* Now shift the group forward as far as possible: */
			ആകുമ്പോൾ (1) {
				എങ്കിൽ (group_slide_down(xdf, &g))
					നിർത്തുക;
				എങ്കിൽ (group_next(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				എങ്കിൽ (go.end > go.start)
					end_matching_other = g.end;
			}
		} ആകുമ്പോൾ (groupsize != g.end - g.start);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		എങ്കിൽ (g.end == earliest_end) {
			/* no shifting was possible */
		} അല്ലെങ്കിൽ എങ്കിൽ (end_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			ആകുമ്പോൾ (go.end == go.start) {
				എങ്കിൽ (group_slide_up(xdf, &g))
					xdl_bug("match disappeared");
				എങ്കിൽ (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_INDENT_HEURISTIC) {
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
			long shift, best_shift = -1;
			ഘടന split_score best_score;

			shift = earliest_end;
			എങ്കിൽ (g.end - groupsize - 1 > shift)
				shift = g.end - groupsize - 1;
			എങ്കിൽ (g.end - INDENT_HEURISTIC_MAX_SLIDING > shift)
				shift = g.end - INDENT_HEURISTIC_MAX_SLIDING;
			വേണ്ടി (; shift <= g.end; shift++) {
				ഘടന split_measurement m;
				ഘടന split_score score = {0, 0};

				measure_split(xdf, shift, &m);
				score_add_split(&m, &score);
				measure_split(xdf, shift - groupsize, &m);
				score_add_split(&m, &score);
				എങ്കിൽ (best_shift == -1 ||
				    score_cmp(&score, &best_score) <= 0) {
					best_score.effective_indent = score.effective_indent;
					best_score.penalty = score.penalty;
					best_shift = shift;
				}
			}

			ആകുമ്പോൾ (g.end > best_shift) {
				എങ്കിൽ (group_slide_up(xdf, &g))
					xdl_bug("best shift unreached");
				എങ്കിൽ (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	next:
		/* Move past the just-processed group: */
		എങ്കിൽ (group_next(xdf, &g))
			നിർത്തുക;
		എങ്കിൽ (group_next(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	എങ്കിൽ (!group_next(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	തിരികെ 0;
}


പൂർണ്ണസംഖ്യ xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = ശൂന്യ, *xch;
	bool *changed1 = xe->xdf1.changed, *changed2 = xe->xdf2.changed;
	long i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	വേണ്ടി (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		എങ്കിൽ (changed1[i1 - 1] || changed2[i2 - 1]) {
			വേണ്ടി (l1 = i1; changed1[i1 - 1]; i1--);
			വേണ്ടി (l2 = i2; changed2[i2 - 1]; i2--);

			എങ്കിൽ (!(xch = xdl_add_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				തിരികെ -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	തിരികെ 0;
}


ശൂന്യം xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	ആകുമ്പോൾ ((xch = xscr) != ശൂന്യ) {
		xscr = xscr->next;
		xdl_free(xch);
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xdl_call_hunk_func(xdfenv_t *xe UNUSED, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t സ്ഥിരം *xecfg)
{
	xdchange_t *xch, *xche;

	വേണ്ടി (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		എങ്കിൽ (!xch)
			നിർത്തുക;
		എങ്കിൽ (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			തിരികെ -1;
	}
	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം xdl_mark_ignorable_വരികൾ(xdchange_t *xscr, xdfenv_t *xe, long flags)
{
	xdchange_t *xch;

	വേണ്ടി (xch = xscr; xch; xch = xch->next) {
		പൂർണ്ണസംഖ്യ ignore = 1;
		xrecord_t *rec;
		long i;

		rec = &xe->xdf1.recs[xch->i1];
		വേണ്ടി (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].size, flags);

		rec = &xe->xdf2.recs[xch->i2];
		വേണ്ടി (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].size, flags);

		xch->ignore = ignore;
	}
}

#എങ്കിൽ 0 // unused by Vim
സ്ഥിതം പൂർണ്ണസംഖ്യ record_matches_regex(xrecord_t *rec, xpparam_t സ്ഥിരം *xpp) {
	regmatch_t regmatch;
	size_t i;

	വേണ്ടി (i = 0; i < xpp->ignore_regex_nr; i++)
		എങ്കിൽ (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->size, 1,
				 &regmatch, 0))
			തിരികെ 1;

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം xdl_mark_ignorable_regex(xdchange_t *xscr, സ്ഥിരം xdfenv_t *xe,
				     xpparam_t സ്ഥിരം *xpp)
{
	xdchange_t *xch;

	വേണ്ടി (xch = xscr; xch; xch = xch->next) {
		xrecord_t *rec;
		പൂർണ്ണസംഖ്യ ignore = 1;
		long i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		എങ്കിൽ (xch->ignore)
			തുടരുക;

		rec = &xe->xdf1.recs[xch->i1];
		വേണ്ടി (i = 0; i < xch->chg1 && ignore; i++)
			ignore = record_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		വേണ്ടി (i = 0; i < xch->chg2 && ignore; i++)
			ignore = record_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#endif

പൂർണ്ണസംഖ്യ xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t സ്ഥിരം *xpp,
	     xdemitconf_t സ്ഥിരം *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	എങ്കിൽ (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		തിരികെ -1;
	}
	എങ്കിൽ (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		തിരികെ -1;
	}
	എങ്കിൽ (xscr) {
		എങ്കിൽ (xpp->flags & XDF_IGNORE_BLANK_വരികൾ)
			xdl_mark_ignorable_വരികൾ(xscr, &xe, xpp->flags);

#എങ്കിൽ 0
		എങ്കിൽ (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#endif

		എങ്കിൽ (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			തിരികെ -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	തിരികെ 0;
}
