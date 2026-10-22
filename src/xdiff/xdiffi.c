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

#অন্তর্ভাৱ "xinclude.h"

স্থির unsigned long get_hash(xdfile_t *xdf, long index)
{
	ঘূৰাই xdf->recs[xdf->rindex[index]].ha;
}

#define XDL_MAX_COST_MIN 256
#define XDL_HEUR_MIN_COST 256
#define XDL_সাৰি_MAX (long)((1UL << (CHAR_BIT * পরিমাণ(long) - 1)) - 1)
#define XDL_SNAKE_CNT 20
#define XDL_K_HEUR 4

প্রকারনির্দেশ সংরচনা s_xdpsplit {
	long i1, i2;
	পূর্ণাঙ্ক min_lo, min_hi;
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
স্থির long xdl_split(xdfile_t *xdf1, long off1, long lim1,
		      xdfile_t *xdf2, long off2, long lim2,
		      long *kvdf, long *kvdb, পূর্ণাঙ্ক need_min, xdpsplit_t *spl,
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

	বাবে (ec = 1;; ec++) {
		পূর্ণাঙ্ক got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		যদি (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		অন্যথা
			++fmin;
		যদি (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		অন্যথা
			--fmax;

		বাবে (d = fmax; d >= fmin; d -= 2) {
			যদি (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			অন্যথা
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			বাবে (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			যদি (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			যদি (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ঘূৰাই ec;
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
		যদি (bmin > dmin)
			kvdb[--bmin - 1] = XDL_সাৰি_MAX;
		অন্যথা
			++bmin;
		যদি (bmax < dmax)
			kvdb[++bmax + 1] = XDL_সাৰি_MAX;
		অন্যথা
			--bmax;

		বাবে (d = bmax; d >= bmin; d -= 2) {
			যদি (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			অন্যথা
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			বাবে (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			যদি (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			যদি (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				ঘূৰাই ec;
			}
		}

		যদি (need_min)
			অনুৱর্ত;

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
		যদি (got_snake && ec > xenv->heur_min) {
			বাবে (best = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				যদি (v > XDL_K_HEUR * ec && v > best &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					বাবে (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						যদি (k == xenv->snake_cnt) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ভঙ্গ;
						}
				}
			}
			যদি (best > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				ঘূৰাই ec;
			}

			বাবে (best = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				যদি (v > XDL_K_HEUR * ec && v > best &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					বাবে (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						যদি (k == xenv->snake_cnt - 1) {
							best = v;
							spl->i1 = i1;
							spl->i2 = i2;
							ভঙ্গ;
						}
				}
			}
			যদি (best > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				ঘূৰাই ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		যদি (ec >= xenv->mxcost) {
			long fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			বাবে (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				যদি (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				যদি (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_সাৰি_MAX;
			বাবে (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				যদি (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				যদি (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			যদি ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} অন্যথা {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			ঘূৰাই ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
পূর্ণাঙ্ক xdl_recs_cmp(xdfile_t *xdf1, long off1, long lim1,
		 xdfile_t *xdf2, long off2, long lim2,
		 long *kvdf, long *kvdb, পূর্ণাঙ্ক need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	বাবে (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	বাবে (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	যদি (off1 == lim1) {
		বাবে (; off2 < lim2; off2++)
			xdf2->changed[xdf2->rindex[off2]] = true;
	} অন্যথা যদি (off2 == lim2) {
		বাবে (; off1 < lim1; off1++)
			xdf1->changed[xdf1->rindex[off1]] = true;
	} অন্যথা {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		যদি (xdl_split(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			ঘূৰাই -1;
		}

		/*
		 * ... et Impera.
		 */
		যদি (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			ঘূৰাই -1;
		}
	}

	ঘূৰাই 0;
}


পূর্ণাঙ্ক xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ধ্রুৱ *xpp,
		xdfenv_t *xe) {
	long ndiags;
	long *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	পূর্ণাঙ্ক res;

	যদি (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		ঘূৰাই -1;

	যদি (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		গচ্ছ out;
	}

	যদি (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		গচ্ছ out;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	যদি (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		ঘূৰাই -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	যদি (xenv.mxcost < XDL_MAX_COST_MIN)
		xenv.mxcost = XDL_MAX_COST_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_COST;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_MINIMAL) != 0,
			   &xenv);
	xdl_free(kvd);
 out:
	যদি (res < 0)
		xdl_free_env(xe);

	ঘূৰাই res;
}


স্থির xdchange_t *xdl_add_change(xdchange_t *xscr, long i1, long i2, long chg1, long chg2) {
	xdchange_t *xch;

	যদি (!(xch = (xdchange_t *) xdl_malloc(পরিমাণ(xdchange_t))))
		ঘূৰাই রিক্ত;

	xch->next = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	ঘূৰাই xch;
}


স্থির পূর্ণাঙ্ক recs_match(xrecord_t *rec1, xrecord_t *rec2)
{
	ঘূৰাই (rec1->ha == rec2->ha);
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
স্থির পূর্ণাঙ্ক xget_indent(xrecord_t *rec)
{
	long i;
	পূর্ণাঙ্ক ret = 0;

	বাবে (i = 0; i < rec->size; i++) {
		ৱর্ণ c = rec->ptr[i];

		যদি (!XDL_ISSPACE(c))
			ঘূৰাই ret;
		অন্যথা যদি (c == ' ')
			ret += 1;
		অন্যথা যদি (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		যদি (ret >= MAX_INDENT)
			ঘূৰাই MAX_INDENT;
	}

	/* The line contains only whitespace. */
	ঘূৰাই -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#define MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
সংরচনা split_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	পূর্ণাঙ্ক end_of_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	পূর্ণাঙ্ক indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	পূর্ণাঙ্ক pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	পূর্ণাঙ্ক pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	পূর্ণাঙ্ক post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	পূর্ণাঙ্ক post_indent;
};

সংরচনা split_score {
	/* The effective indent of this split (smaller is preferred). */
	পূর্ণাঙ্ক effective_indent;

	/* Penalty for this split (smaller is preferred). */
	পূর্ণাঙ্ক penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
স্থির শূন্য measure_split(ধ্রুৱ xdfile_t *xdf, long split,
			  সংরচনা split_measurement *m)
{
	long i;

	যদি (split >= xdf->nrec) {
		m->end_of_file = 1;
		m->indent = -1;
	} অন্যথা {
		m->end_of_file = 0;
		m->indent = xget_indent(&xdf->recs[split]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	বাবে (i = split - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		যদি (m->pre_indent != -1)
			ভঙ্গ;
		m->pre_blank += 1;
		যদি (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			ভঙ্গ;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	বাবে (i = split + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		যদি (m->post_indent != -1)
			ভঙ্গ;
		m->post_blank += 1;
		যদি (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			ভঙ্গ;
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
স্থির শূন্য score_add_split(ধ্রুৱ সংরচনা split_measurement *m, সংরচনা split_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	পূর্ণাঙ্ক post_blank, total_blank, indent, any_blanks;

	যদি (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += START_OF_FILE_PENALTY;

	যদি (m->end_of_file)
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

	যদি (m->indent != -1)
		indent = m->indent;
	অন্যথা
		indent = m->post_indent;

	any_blanks = (total_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	যদি (indent == -1) {
		/* No additional adjustments needed. */
	} অন্যথা যদি (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} অন্যথা যদি (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} অন্যথা যদি (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} অন্যথা {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		যদি (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} অন্যথা {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

স্থির পূর্ণাঙ্ক score_cmp(সংরচনা split_score *s1, সংরচনা split_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	পূর্ণাঙ্ক cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	ঘূৰাই INDENT_WEIGHT * cmp_indents + (s1->penalty - s2->penalty);
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
সংরচনা xdlgroup {
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
স্থির শূন্য group_init(xdfile_t *xdf, সংরচনা xdlgroup *g)
{
	g->start = g->end = 0;
	যেতিয়ালৈ (xdf->changed[g->end])
		g->end++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
স্থির অন্তঃস্থ পূর্ণাঙ্ক group_next(xdfile_t *xdf, সংরচনা xdlgroup *g)
{
	যদি (g->end == xdf->nrec)
		ঘূৰাই -1;

	g->start = g->end + 1;
	বাবে (g->end = g->start; xdf->changed[g->end]; g->end++)
		;

	ঘূৰাই 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
স্থির অন্তঃস্থ পূর্ণাঙ্ক group_previous(xdfile_t *xdf, সংরচনা xdlgroup *g)
{
	যদি (g->start == 0)
		ঘূৰাই -1;

	g->end = g->start - 1;
	বাবে (g->start = g->end; xdf->changed[g->start - 1]; g->start--)
		;

	ঘূৰাই 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
স্থির পূর্ণাঙ্ক group_slide_down(xdfile_t *xdf, সংরচনা xdlgroup *g)
{
	যদি (g->end < xdf->nrec &&
	    recs_match(&xdf->recs[g->start], &xdf->recs[g->end])) {
		xdf->changed[g->start++] = false;
		xdf->changed[g->end++] = true;

		যেতিয়ালৈ (xdf->changed[g->end])
			g->end++;

		ঘূৰাই 0;
	} অন্যথা {
		ঘূৰাই -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
স্থির পূর্ণাঙ্ক group_slide_up(xdfile_t *xdf, সংরচনা xdlgroup *g)
{
	যদি (g->start > 0 &&
	    recs_match(&xdf->recs[g->start - 1], &xdf->recs[g->end - 1])) {
		xdf->changed[--g->start] = true;
		xdf->changed[--g->end] = false;

		যেতিয়ালৈ (xdf->changed[g->start - 1])
			g->start--;

		ঘূৰাই 0;
	} অন্যথা {
		ঘূৰাই -1;
	}
}

শূন্য xdl_bug(ধ্রুৱ ৱর্ণ *বাৰ্তা)
{
	fprintf(stderr, "BUG: %s\n", বাৰ্তা);
	exit(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
পূর্ণাঙ্ক xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, long flags) {
	সংরচনা xdlgroup g, go;
	long earliest_end, end_matching_other;
	long groupsize;

	group_init(xdf, &g);
	group_init(xdfo, &go);

	যেতিয়ালৈ (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		যদি (g.end == g.start)
			গচ্ছ next;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		কৰা {
			groupsize = g.end - g.start;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			end_matching_other = -1;

			/* Shift the group backward as much as possible: */
			যেতিয়ালৈ (!group_slide_up(xdf, &g))
				যদি (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_end = g.end;

			যদি (go.end > go.start)
				end_matching_other = g.end;

			/* Now shift the group forward as far as possible: */
			যেতিয়ালৈ (1) {
				যদি (group_slide_down(xdf, &g))
					ভঙ্গ;
				যদি (group_next(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				যদি (go.end > go.start)
					end_matching_other = g.end;
			}
		} যেতিয়ালৈ (groupsize != g.end - g.start);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		যদি (g.end == earliest_end) {
			/* no shifting was possible */
		} অন্যথা যদি (end_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			যেতিয়ালৈ (go.end == go.start) {
				যদি (group_slide_up(xdf, &g))
					xdl_bug("match disappeared");
				যদি (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} অন্যথা যদি (flags & XDF_INDENT_HEURISTIC) {
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
			সংরচনা split_score best_score;

			shift = earliest_end;
			যদি (g.end - groupsize - 1 > shift)
				shift = g.end - groupsize - 1;
			যদি (g.end - INDENT_HEURISTIC_MAX_SLIDING > shift)
				shift = g.end - INDENT_HEURISTIC_MAX_SLIDING;
			বাবে (; shift <= g.end; shift++) {
				সংরচনা split_measurement m;
				সংরচনা split_score score = {0, 0};

				measure_split(xdf, shift, &m);
				score_add_split(&m, &score);
				measure_split(xdf, shift - groupsize, &m);
				score_add_split(&m, &score);
				যদি (best_shift == -1 ||
				    score_cmp(&score, &best_score) <= 0) {
					best_score.effective_indent = score.effective_indent;
					best_score.penalty = score.penalty;
					best_shift = shift;
				}
			}

			যেতিয়ালৈ (g.end > best_shift) {
				যদি (group_slide_up(xdf, &g))
					xdl_bug("best shift unreached");
				যদি (group_previous(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	next:
		/* Move past the just-processed group: */
		যদি (group_next(xdf, &g))
			ভঙ্গ;
		যদি (group_next(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	যদি (!group_next(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	ঘূৰাই 0;
}


পূর্ণাঙ্ক xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = রিক্ত, *xch;
	bool *changed1 = xe->xdf1.changed, *changed2 = xe->xdf2.changed;
	long i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	বাবে (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		যদি (changed1[i1 - 1] || changed2[i2 - 1]) {
			বাবে (l1 = i1; changed1[i1 - 1]; i1--);
			বাবে (l2 = i2; changed2[i2 - 1]; i2--);

			যদি (!(xch = xdl_add_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				ঘূৰাই -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	ঘূৰাই 0;
}


শূন্য xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	যেতিয়ালৈ ((xch = xscr) != রিক্ত) {
		xscr = xscr->next;
		xdl_free(xch);
	}
}

স্থির পূর্ণাঙ্ক xdl_call_hunk_func(xdfenv_t *xe UNUSED, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t ধ্রুৱ *xecfg)
{
	xdchange_t *xch, *xche;

	বাবে (xch = xscr; xch; xch = xche->next) {
		xche = xdl_get_hunk(&xch, xecfg);
		যদি (!xch)
			ভঙ্গ;
		যদি (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			ঘূৰাই -1;
	}
	ঘূৰাই 0;
}

স্থির শূন্য xdl_mark_ignorable_সাৰিগুলি(xdchange_t *xscr, xdfenv_t *xe, long flags)
{
	xdchange_t *xch;

	বাবে (xch = xscr; xch; xch = xch->next) {
		পূর্ণাঙ্ক ignore = 1;
		xrecord_t *rec;
		long i;

		rec = &xe->xdf1.recs[xch->i1];
		বাবে (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].size, flags);

		rec = &xe->xdf2.recs[xch->i2];
		বাবে (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].size, flags);

		xch->ignore = ignore;
	}
}

#যদি 0 // unused by Vim
স্থির পূর্ণাঙ্ক record_matches_regex(xrecord_t *rec, xpparam_t ধ্রুৱ *xpp) {
	regmatch_t regmatch;
	size_t i;

	বাবে (i = 0; i < xpp->ignore_regex_nr; i++)
		যদি (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->size, 1,
				 &regmatch, 0))
			ঘূৰাই 1;

	ঘূৰাই 0;
}

স্থির শূন্য xdl_mark_ignorable_regex(xdchange_t *xscr, ধ্রুৱ xdfenv_t *xe,
				     xpparam_t ধ্রুৱ *xpp)
{
	xdchange_t *xch;

	বাবে (xch = xscr; xch; xch = xch->next) {
		xrecord_t *rec;
		পূর্ণাঙ্ক ignore = 1;
		long i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		যদি (xch->ignore)
			অনুৱর্ত;

		rec = &xe->xdf1.recs[xch->i1];
		বাবে (i = 0; i < xch->chg1 && ignore; i++)
			ignore = record_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		বাবে (i = 0; i < xch->chg2 && ignore; i++)
			ignore = record_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#endif

পূর্ণাঙ্ক xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ধ্রুৱ *xpp,
	     xdemitconf_t ধ্রুৱ *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	যদি (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		ঘূৰাই -1;
	}
	যদি (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		ঘূৰাই -1;
	}
	যদি (xscr) {
		যদি (xpp->flags & XDF_IGNORE_BLANK_সাৰিগুলি)
			xdl_mark_ignorable_সাৰিগুলি(xscr, &xe, xpp->flags);

#যদি 0
		যদি (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#endif

		যদি (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			ঘূৰাই -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	ঘূৰাই 0;
}
