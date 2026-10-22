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

#شامل "xinclude.h"

ساکن غیرعلامتی طویل get_hash(xdfile_t *xdf, طویل اشاریہ)
{
	واپس xdf->recs[xdf->rindex[اشاریہ]].ha;
}

#تعریف_کرو XDL_MAX_لاگت_MIN 256
#تعریف_کرو XDL_HEUR_MIN_لاگت 256
#تعریف_کرو XDL_سطر_MAX (طویل)((1UL << (CHAR_BIT * حجم(طویل) - 1)) - 1)
#تعریف_کرو XDL_SNAKE_CNT 20
#تعریف_کرو XDL_K_HEUR 4

قسمتعریف ساخت s_xdpsplit {
	طویل i1, i2;
	صحیح min_lo, min_hi;
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
ساکن طویل xdl_بانٹو(xdfile_t *xdf1, طویل off1, طویل lim1,
		      xdfile_t *xdf2, طویل off2, طویل lim2,
		      طویل *kvdf, طویل *kvdb, صحیح need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	طویل dmin = off1 - lim2, dmax = lim1 - off2;
	طویل fmid = off1 - off2, bmid = lim1 - lim2;
	طویل odd = (fmid - bmid) & 1;
	طویل fmin = fmid, fmax = fmid;
	طویل bmin = bmid, bmax = bmid;
	طویل ec, d, i1, i2, prev1, بہترین, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	برائے (ec = 1;; ec++) {
		صحیح got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		اگر (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		ورنہ
			++fmin;
		اگر (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		ورنہ
			--fmax;

		برائے (d = fmax; d >= fmin; d -= 2) {
			اگر (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			ورنہ
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			برائے (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			اگر (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			اگر (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				واپس ec;
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
		اگر (bmin > dmin)
			kvdb[--bmin - 1] = XDL_سطر_MAX;
		ورنہ
			++bmin;
		اگر (bmax < dmax)
			kvdb[++bmax + 1] = XDL_سطر_MAX;
		ورنہ
			--bmax;

		برائے (d = bmax; d >= bmin; d -= 2) {
			اگر (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			ورنہ
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			برائے (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			اگر (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			اگر (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				واپس ec;
			}
		}

		اگر (need_min)
			جاری;

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
		اگر (got_snake && ec > xenv->heur_min) {
			برائے (بہترین = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				اگر (v > XDL_K_HEUR * ec && v > بہترین &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					برائے (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						اگر (k == xenv->snake_cnt) {
							بہترین = v;
							spl->i1 = i1;
							spl->i2 = i2;
							توڑو;
						}
				}
			}
			اگر (بہترین > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				واپس ec;
			}

			برائے (بہترین = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				اگر (v > XDL_K_HEUR * ec && v > بہترین &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					برائے (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						اگر (k == xenv->snake_cnt - 1) {
							بہترین = v;
							spl->i1 = i1;
							spl->i2 = i2;
							توڑو;
						}
				}
			}
			اگر (بہترین > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				واپس ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		اگر (ec >= xenv->mxcost) {
			طویل fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			برائے (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				اگر (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				اگر (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_سطر_MAX;
			برائے (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				اگر (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				اگر (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			اگر ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} ورنہ {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			واپس ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
صحیح xdl_recs_cmp(xdfile_t *xdf1, طویل off1, طویل lim1,
		 xdfile_t *xdf2, طویل off2, طویل lim2,
		 طویل *kvdf, طویل *kvdb, صحیح need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	برائے (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	برائے (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	اگر (off1 == lim1) {
		برائے (; off2 < lim2; off2++)
			xdf2->بدلا[xdf2->rindex[off2]] = true;
	} ورنہ اگر (off2 == lim2) {
		برائے (; off1 < lim1; off1++)
			xdf1->بدلا[xdf1->rindex[off1]] = true;
	} ورنہ {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		اگر (xdl_بانٹو(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			واپس -1;
		}

		/*
		 * ... et Impera.
		 */
		اگر (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			واپس -1;
		}
	}

	واپس 0;
}


صحیح xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
		xdfenv_t *xe) {
	طویل ndiags;
	طویل *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	صحیح res;

	اگر (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		واپس -1;

	اگر (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		جاؤ برآمد;
	}

	اگر (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		جاؤ برآمد;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	اگر (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		واپس -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	اگر (xenv.mxcost < XDL_MAX_لاگت_MIN)
		xenv.mxcost = XDL_MAX_لاگت_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_لاگت;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_اقل) != 0,
			   &xenv);
	xdl_free(kvd);
 برآمد:
	اگر (res < 0)
		xdl_free_env(xe);

	واپس res;
}


ساکن xdchange_t *xdl_جوڑو_change(xdchange_t *xscr, طویل i1, طویل i2, طویل chg1, طویل chg2) {
	xdchange_t *xch;

	اگر (!(xch = (xdchange_t *) xdl_malloc(حجم(xdchange_t))))
		واپس تہی;

	xch->اگلا = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	واپس xch;
}


ساکن صحیح recs_مطابقت(xrecord_t *rec1, xrecord_t *rec2)
{
	واپس (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#تعریف_کرو MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
ساکن صحیح xget_indent(xrecord_t *rec)
{
	طویل i;
	صحیح ret = 0;

	برائے (i = 0; i < rec->سائز; i++) {
		حرف c = rec->ptr[i];

		اگر (!XDL_ISSPACE(c))
			واپس ret;
		ورنہ اگر (c == ' ')
			ret += 1;
		ورنہ اگر (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		اگر (ret >= MAX_INDENT)
			واپس MAX_INDENT;
	}

	/* The line contains only whitespace. */
	واپس -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#تعریف_کرو MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
ساخت بانٹو_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	صحیح انتہا_کا_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	صحیح indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	صحیح pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	صحیح pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	صحیح post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	صحیح post_indent;
};

ساخت بانٹو_score {
	/* The effective indent of this split (smaller is preferred). */
	صحیح effective_indent;

	/* Penalty for this split (smaller is preferred). */
	صحیح penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
ساکن خالی measure_بانٹو(ثابت xdfile_t *xdf, طویل بانٹو,
			  ساخت بانٹو_measurement *m)
{
	طویل i;

	اگر (بانٹو >= xdf->nrec) {
		m->انتہا_کا_file = 1;
		m->indent = -1;
	} ورنہ {
		m->انتہا_کا_file = 0;
		m->indent = xget_indent(&xdf->recs[بانٹو]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	برائے (i = بانٹو - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		اگر (m->pre_indent != -1)
			توڑو;
		m->pre_blank += 1;
		اگر (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			توڑو;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	برائے (i = بانٹو + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		اگر (m->post_indent != -1)
			توڑو;
		m->post_blank += 1;
		اگر (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			توڑو;
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
#تعریف_کرو آغاز_کا_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#تعریف_کرو انتہا_کا_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#تعریف_کرو کل_BLANK_وزن (-30)

/* Multiplier for the number of blank lines after the split */
#تعریف_کرو POST_BLANK_وزن 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#تعریف_کرو RELATIVE_INDENT_PENALTY (-4)
#تعریف_کرو RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#تعریف_کرو RELATIVE_OUTDENT_PENALTY 24
#تعریف_کرو RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#تعریف_کرو RELATIVE_DEDENT_PENALTY 23
#تعریف_کرو RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#تعریف_کرو INDENT_وزن 60

/*
 * How far do we slide a hunk at most?
 */
#تعریف_کرو INDENT_HEURISTIC_MAX_SLIDING 100

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
ساکن خالی score_جوڑو_بانٹو(ثابت ساخت بانٹو_measurement *m, ساخت بانٹو_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	صحیح post_blank, کل_blank, indent, any_blanks;

	اگر (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += آغاز_کا_FILE_PENALTY;

	اگر (m->انتہا_کا_file)
		s->penalty += انتہا_کا_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	کل_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += کل_BLANK_وزن * کل_blank;
	s->penalty += POST_BLANK_وزن * post_blank;

	اگر (m->indent != -1)
		indent = m->indent;
	ورنہ
		indent = m->post_indent;

	any_blanks = (کل_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	اگر (indent == -1) {
		/* No additional adjustments needed. */
	} ورنہ اگر (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} ورنہ اگر (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} ورنہ اگر (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} ورنہ {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		اگر (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} ورنہ {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

ساکن صحیح score_cmp(ساخت بانٹو_score *s1, ساخت بانٹو_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	صحیح cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	واپس INDENT_وزن * cmp_indents + (s1->penalty - s2->penalty);
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
ساخت xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	طویل آغاز;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	طویل انتہا;
};

/*
 * Initialize g to point at the first group in xdf.
 */
ساکن خالی گروہ_init(xdfile_t *xdf, ساخت xdlgroup *g)
{
	g->آغاز = g->انتہا = 0;
	جب (xdf->بدلا[g->انتہا])
		g->انتہا++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
ساکن اندرونی صحیح گروہ_اگلا(xdfile_t *xdf, ساخت xdlgroup *g)
{
	اگر (g->انتہا == xdf->nrec)
		واپس -1;

	g->آغاز = g->انتہا + 1;
	برائے (g->انتہا = g->آغاز; xdf->بدلا[g->انتہا]; g->انتہا++)
		;

	واپس 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
ساکن اندرونی صحیح گروہ_پچھلا(xdfile_t *xdf, ساخت xdlgroup *g)
{
	اگر (g->آغاز == 0)
		واپس -1;

	g->انتہا = g->آغاز - 1;
	برائے (g->آغاز = g->انتہا; xdf->بدلا[g->آغاز - 1]; g->آغاز--)
		;

	واپس 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
ساکن صحیح گروہ_slide_نیچے_کو(xdfile_t *xdf, ساخت xdlgroup *g)
{
	اگر (g->انتہا < xdf->nrec &&
	    recs_مطابقت(&xdf->recs[g->آغاز], &xdf->recs[g->انتہا])) {
		xdf->بدلا[g->آغاز++] = false;
		xdf->بدلا[g->انتہا++] = true;

		جب (xdf->بدلا[g->انتہا])
			g->انتہا++;

		واپس 0;
	} ورنہ {
		واپس -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
ساکن صحیح گروہ_slide_اوپر(xdfile_t *xdf, ساخت xdlgroup *g)
{
	اگر (g->آغاز > 0 &&
	    recs_مطابقت(&xdf->recs[g->آغاز - 1], &xdf->recs[g->انتہا - 1])) {
		xdf->بدلا[--g->آغاز] = true;
		xdf->بدلا[--g->انتہا] = false;

		جب (xdf->بدلا[g->آغاز - 1])
			g->آغاز--;

		واپس 0;
	} ورنہ {
		واپس -1;
	}
}

خالی xdl_bug(ثابت حرف *پیغام)
{
	fprintf(stderr, "BUG: %s\n", پیغام);
	باہر_نکلو(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
صحیح xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, طویل flags) {
	ساخت xdlgroup g, go;
	طویل earliest_انتہا, انتہا_matching_other;
	طویل groupsize;

	گروہ_init(xdf, &g);
	گروہ_init(xdfo, &go);

	جب (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		اگر (g.انتہا == g.آغاز)
			جاؤ اگلا;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		کرو {
			groupsize = g.انتہا - g.آغاز;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			انتہا_matching_other = -1;

			/* Shift the group backward as much as possible: */
			جب (!گروہ_slide_اوپر(xdf, &g))
				اگر (گروہ_پچھلا(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_انتہا = g.انتہا;

			اگر (go.انتہا > go.آغاز)
				انتہا_matching_other = g.انتہا;

			/* Now shift the group forward as far as possible: */
			جب (1) {
				اگر (گروہ_slide_نیچے_کو(xdf, &g))
					توڑو;
				اگر (گروہ_اگلا(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				اگر (go.انتہا > go.آغاز)
					انتہا_matching_other = g.انتہا;
			}
		} جب (groupsize != g.انتہا - g.آغاز);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		اگر (g.انتہا == earliest_انتہا) {
			/* no shifting was possible */
		} ورنہ اگر (انتہا_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			جب (go.انتہا == go.آغاز) {
				اگر (گروہ_slide_اوپر(xdf, &g))
					xdl_bug("match disappeared");
				اگر (گروہ_پچھلا(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} ورنہ اگر (flags & XDF_INDENT_HEURISTIC) {
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
			طویل سرکاؤ, بہترین_سرکاؤ = -1;
			ساخت بانٹو_score بہترین_score;

			سرکاؤ = earliest_انتہا;
			اگر (g.انتہا - groupsize - 1 > سرکاؤ)
				سرکاؤ = g.انتہا - groupsize - 1;
			اگر (g.انتہا - INDENT_HEURISTIC_MAX_SLIDING > سرکاؤ)
				سرکاؤ = g.انتہا - INDENT_HEURISTIC_MAX_SLIDING;
			برائے (; سرکاؤ <= g.انتہا; سرکاؤ++) {
				ساخت بانٹو_measurement m;
				ساخت بانٹو_score score = {0, 0};

				measure_بانٹو(xdf, سرکاؤ, &m);
				score_جوڑو_بانٹو(&m, &score);
				measure_بانٹو(xdf, سرکاؤ - groupsize, &m);
				score_جوڑو_بانٹو(&m, &score);
				اگر (بہترین_سرکاؤ == -1 ||
				    score_cmp(&score, &بہترین_score) <= 0) {
					بہترین_score.effective_indent = score.effective_indent;
					بہترین_score.penalty = score.penalty;
					بہترین_سرکاؤ = سرکاؤ;
				}
			}

			جب (g.انتہا > بہترین_سرکاؤ) {
				اگر (گروہ_slide_اوپر(xdf, &g))
					xdl_bug("best shift unreached");
				اگر (گروہ_پچھلا(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	اگلا:
		/* Move past the just-processed group: */
		اگر (گروہ_اگلا(xdf, &g))
			توڑو;
		اگر (گروہ_اگلا(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	اگر (!گروہ_اگلا(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	واپس 0;
}


صحیح xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = تہی, *xch;
	bool *changed1 = xe->xdf1.بدلا, *changed2 = xe->xdf2.بدلا;
	طویل i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	برائے (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		اگر (changed1[i1 - 1] || changed2[i2 - 1]) {
			برائے (l1 = i1; changed1[i1 - 1]; i1--);
			برائے (l2 = i2; changed2[i2 - 1]; i2--);

			اگر (!(xch = xdl_جوڑو_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				واپس -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	واپس 0;
}


خالی xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	جب ((xch = xscr) != تہی) {
		xscr = xscr->اگلا;
		xdl_free(xch);
	}
}

ساکن صحیح xdl_call_hunk_func(xdfenv_t *xe غیر_مستعمل, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t ثابت *xecfg)
{
	xdchange_t *xch, *xche;

	برائے (xch = xscr; xch; xch = xche->اگلا) {
		xche = xdl_get_hunk(&xch, xecfg);
		اگر (!xch)
			توڑو;
		اگر (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			واپس -1;
	}
	واپس 0;
}

ساکن خالی xdl_mark_ignorable_سطریں(xdchange_t *xscr, xdfenv_t *xe, طویل flags)
{
	xdchange_t *xch;

	برائے (xch = xscr; xch; xch = xch->اگلا) {
		صحیح ignore = 1;
		xrecord_t *rec;
		طویل i;

		rec = &xe->xdf1.recs[xch->i1];
		برائے (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].سائز, flags);

		rec = &xe->xdf2.recs[xch->i2];
		برائے (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].سائز, flags);

		xch->ignore = ignore;
	}
}

#اگر 0 // unused by Vim
ساکن صحیح ریکارڈ_matches_regex(xrecord_t *rec, xpparam_t ثابت *xpp) {
	regmatch_t regmatch;
	size_t i;

	برائے (i = 0; i < xpp->ignore_regex_nr; i++)
		اگر (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->سائز, 1,
				 &regmatch, 0))
			واپس 1;

	واپس 0;
}

ساکن خالی xdl_mark_ignorable_regex(xdchange_t *xscr, ثابت xdfenv_t *xe,
				     xpparam_t ثابت *xpp)
{
	xdchange_t *xch;

	برائے (xch = xscr; xch; xch = xch->اگلا) {
		xrecord_t *rec;
		صحیح ignore = 1;
		طویل i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		اگر (xch->ignore)
			جاری;

		rec = &xe->xdf1.recs[xch->i1];
		برائے (i = 0; i < xch->chg1 && ignore; i++)
			ignore = ریکارڈ_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		برائے (i = 0; i < xch->chg2 && ignore; i++)
			ignore = ریکارڈ_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#اگر_ختم

صحیح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	اگر (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		واپس -1;
	}
	اگر (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		واپس -1;
	}
	اگر (xscr) {
		اگر (xpp->flags & XDF_IGNORE_BLANK_سطریں)
			xdl_mark_ignorable_سطریں(xscr, &xe, xpp->flags);

#اگر 0
		اگر (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#اگر_ختم

		اگر (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			واپس -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	واپس 0;
}
