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

#ضمن "xinclude.h"

ساكن بنانشان ڊگهو get_hash(xdfile_t *xdf, ڊگهو اشاريو)
{
	أرجع xdf->recs[xdf->rindex[اشاريو]].ha;
}

#وصف_ڪريو XDL_MAX_لاڳت_MIN 256
#وصف_ڪريو XDL_HEUR_MIN_لاڳت 256
#وصف_ڪريو XDL_سٽ_MAX (ڊگهو)((1UL << (CHAR_BIT * حجم(ڊگهو) - 1)) - 1)
#وصف_ڪريو XDL_SNAKE_CNT 20
#وصف_ڪريو XDL_K_HEUR 4

تعريفنوع بنية s_xdpsplit {
	ڊگهو i1, i2;
	صحيح min_lo, min_hi;
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
ساكن ڊگهو xdl_ورهايو(xdfile_t *xdf1, ڊگهو off1, ڊگهو lim1,
		      xdfile_t *xdf2, ڊگهو off2, ڊگهو lim2,
		      ڊگهو *kvdf, ڊگهو *kvdb, صحيح need_min, xdpsplit_t *spl,
		      xdalgoenv_t *xenv) {
	ڊگهو dmin = off1 - lim2, dmax = lim1 - off2;
	ڊگهو fmid = off1 - off2, bmid = lim1 - lim2;
	ڊگهو odd = (fmid - bmid) & 1;
	ڊگهو fmin = fmid, fmax = fmid;
	ڊگهو bmin = bmid, bmax = bmid;
	ڊگهو ec, d, i1, i2, prev1, بهترين, dd, v, k;

	/*
	 * Set initial diagonal values for both forward and backward path.
	 */
	kvdf[fmid] = off1;
	kvdb[bmid] = lim1;

	لكل (ec = 1;; ec++) {
		صحيح got_snake = 0;

		/*
		 * We need to extend the diagonal "domain" by one. If the next
		 * values exits the box boundaries we need to change it in the
		 * opposite direction because (max - min) must be a power of
		 * two.
		 *
		 * Also we initialize the external K value to -1 so that we can
		 * avoid extra conditions in the check inside the core loop.
		 */
		إذا (fmin > dmin)
			kvdf[--fmin - 1] = -1;
		وإلا
			++fmin;
		إذا (fmax < dmax)
			kvdf[++fmax + 1] = -1;
		وإلا
			--fmax;

		لكل (d = fmax; d >= fmin; d -= 2) {
			إذا (kvdf[d - 1] >= kvdf[d + 1])
				i1 = kvdf[d - 1] + 1;
			وإلا
				i1 = kvdf[d + 1];
			prev1 = i1;
			i2 = i1 - d;
			لكل (; i1 < lim1 && i2 < lim2 && get_hash(xdf1, i1) == get_hash(xdf2, i2); i1++, i2++);
			إذا (i1 - prev1 > xenv->snake_cnt)
				got_snake = 1;
			kvdf[d] = i1;
			إذا (odd && bmin <= d && d <= bmax && kvdb[d] <= i1) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				أرجع ec;
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
		إذا (bmin > dmin)
			kvdb[--bmin - 1] = XDL_سٽ_MAX;
		وإلا
			++bmin;
		إذا (bmax < dmax)
			kvdb[++bmax + 1] = XDL_سٽ_MAX;
		وإلا
			--bmax;

		لكل (d = bmax; d >= bmin; d -= 2) {
			إذا (kvdb[d - 1] < kvdb[d + 1])
				i1 = kvdb[d - 1];
			وإلا
				i1 = kvdb[d + 1] - 1;
			prev1 = i1;
			i2 = i1 - d;
			لكل (; i1 > off1 && i2 > off2 && get_hash(xdf1, i1 - 1) == get_hash(xdf2, i2 - 1); i1--, i2--);
			إذا (prev1 - i1 > xenv->snake_cnt)
				got_snake = 1;
			kvdb[d] = i1;
			إذا (!odd && fmin <= d && d <= fmax && i1 <= kvdf[d]) {
				spl->i1 = i1;
				spl->i2 = i2;
				spl->min_lo = spl->min_hi = 1;
				أرجع ec;
			}
		}

		إذا (need_min)
			استمر;

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
		إذا (got_snake && ec > xenv->heur_min) {
			لكل (بهترين = 0, d = fmax; d >= fmin; d -= 2) {
				dd = d > fmid ? d - fmid: fmid - d;
				i1 = kvdf[d];
				i2 = i1 - d;
				v = (i1 - off1) + (i2 - off2) - dd;

				إذا (v > XDL_K_HEUR * ec && v > بهترين &&
				    off1 + xenv->snake_cnt <= i1 && i1 < lim1 &&
				    off2 + xenv->snake_cnt <= i2 && i2 < lim2) {
					لكل (k = 1; get_hash(xdf1, i1 - k) == get_hash(xdf2, i2 - k); k++)
						إذا (k == xenv->snake_cnt) {
							بهترين = v;
							spl->i1 = i1;
							spl->i2 = i2;
							اكسر;
						}
				}
			}
			إذا (بهترين > 0) {
				spl->min_lo = 1;
				spl->min_hi = 0;
				أرجع ec;
			}

			لكل (بهترين = 0, d = bmax; d >= bmin; d -= 2) {
				dd = d > bmid ? d - bmid: bmid - d;
				i1 = kvdb[d];
				i2 = i1 - d;
				v = (lim1 - i1) + (lim2 - i2) - dd;

				إذا (v > XDL_K_HEUR * ec && v > بهترين &&
				    off1 < i1 && i1 <= lim1 - xenv->snake_cnt &&
				    off2 < i2 && i2 <= lim2 - xenv->snake_cnt) {
					لكل (k = 0; get_hash(xdf1, i1 + k) == get_hash(xdf2, i2 + k); k++)
						إذا (k == xenv->snake_cnt - 1) {
							بهترين = v;
							spl->i1 = i1;
							spl->i2 = i2;
							اكسر;
						}
				}
			}
			إذا (بهترين > 0) {
				spl->min_lo = 0;
				spl->min_hi = 1;
				أرجع ec;
			}
		}

		/*
		 * Enough is enough. We spent too much time here and now we
		 * collect the furthest reaching path using the (i1 + i2)
		 * measure.
		 */
		إذا (ec >= xenv->mxcost) {
			ڊگهو fbest, fbest1, bbest, bbest1;

			fbest = fbest1 = -1;
			لكل (d = fmax; d >= fmin; d -= 2) {
				i1 = XDL_MIN(kvdf[d], lim1);
				i2 = i1 - d;
				إذا (lim2 < i2) {
					i1 = lim2 + d;
					i2 = lim2;
				}
				إذا (fbest < i1 + i2) {
					fbest = i1 + i2;
					fbest1 = i1;
				}
			}

			bbest = bbest1 = XDL_سٽ_MAX;
			لكل (d = bmax; d >= bmin; d -= 2) {
				i1 = XDL_MAX(off1, kvdb[d]);
				i2 = i1 - d;
				إذا (i2 < off2) {
					i1 = off2 + d;
					i2 = off2;
				}
				إذا (i1 + i2 < bbest) {
					bbest = i1 + i2;
					bbest1 = i1;
				}
			}

			إذا ((lim1 + lim2) - bbest < fbest - (off1 + off2)) {
				spl->i1 = fbest1;
				spl->i2 = fbest - fbest1;
				spl->min_lo = 1;
				spl->min_hi = 0;
			} وإلا {
				spl->i1 = bbest1;
				spl->i2 = bbest - bbest1;
				spl->min_lo = 0;
				spl->min_hi = 1;
			}
			أرجع ec;
		}
	}
}


/*
 * Rule: "Divide et Impera" (divide & conquer). Recursively split the box in
 * sub-boxes by calling the box splitting function. Note that the real job
 * (marking changed lines) is done in the two boundary reaching checks.
 */
صحيح xdl_recs_cmp(xdfile_t *xdf1, ڊگهو off1, ڊگهو lim1,
		 xdfile_t *xdf2, ڊگهو off2, ڊگهو lim2,
		 ڊگهو *kvdf, ڊگهو *kvdb, صحيح need_min, xdalgoenv_t *xenv) {

	/*
	 * Shrink the box by walking through each diagonal snake (SW and NE).
	 */
	لكل (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, off1) == get_hash(xdf2, off2); off1++, off2++);
	لكل (; off1 < lim1 && off2 < lim2 && get_hash(xdf1, lim1 - 1) == get_hash(xdf2, lim2 - 1); lim1--, lim2--);

	/*
	 * If one dimension is empty, then all records on the other one must
	 * be obviously changed.
	 */
	إذا (off1 == lim1) {
		لكل (; off2 < lim2; off2++)
			xdf2->بدليو[xdf2->rindex[off2]] = true;
	} وإلا إذا (off2 == lim2) {
		لكل (; off1 < lim1; off1++)
			xdf1->بدليو[xdf1->rindex[off1]] = true;
	} وإلا {
		xdpsplit_t spl;
		spl.i1 = spl.i2 = 0;

		/*
		 * Divide ...
		 */
		إذا (xdl_ورهايو(xdf1, off1, lim1, xdf2, off2, lim2, kvdf, kvdb,
			      need_min, &spl, xenv) < 0) {

			أرجع -1;
		}

		/*
		 * ... et Impera.
		 */
		إذا (xdl_recs_cmp(xdf1, off1, spl.i1, xdf2, off2, spl.i2,
				 kvdf, kvdb, spl.min_lo, xenv) < 0 ||
		    xdl_recs_cmp(xdf1, spl.i1, lim1, xdf2, spl.i2, lim2,
				 kvdf, kvdb, spl.min_hi, xenv) < 0) {

			أرجع -1;
		}
	}

	أرجع 0;
}


صحيح xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
		xdfenv_t *xe) {
	ڊگهو ndiags;
	ڊگهو *kvd, *kvdf, *kvdb;
	xdalgoenv_t xenv;
	صحيح res;

	إذا (xdl_prepare_env(mf1, mf2, xpp, xe) < 0)
		أرجع -1;

	إذا (XDF_DIFF_ALG(xpp->flags) == XDF_PATIENCE_DIFF) {
		res = xdl_do_patience_diff(xpp, xe);
		اذهب ٻاهريون;
	}

	إذا (XDF_DIFF_ALG(xpp->flags) == XDF_HISTOGRAM_DIFF) {
		res = xdl_do_histogram_diff(xpp, xe);
		اذهب ٻاهريون;
	}

	/*
	 * Allocate and setup K vectors to be used by the differential
	 * algorithm.
	 *
	 * One is to store the forward path and one to store the backward path.
	 */
	ndiags = xe->xdf1.nreff + xe->xdf2.nreff + 3;
	إذا (!XDL_ALLOC_ARRAY(kvd, 2 * ndiags + 2)) {

		xdl_free_env(xe);
		أرجع -1;
	}
	kvdf = kvd;
	kvdb = kvdf + ndiags;
	kvdf += xe->xdf2.nreff + 1;
	kvdb += xe->xdf2.nreff + 1;

	xenv.mxcost = xdl_bogosqrt(ndiags);
	إذا (xenv.mxcost < XDL_MAX_لاڳت_MIN)
		xenv.mxcost = XDL_MAX_لاڳت_MIN;
	xenv.snake_cnt = XDL_SNAKE_CNT;
	xenv.heur_min = XDL_HEUR_MIN_لاڳت;

	res = xdl_recs_cmp(&xe->xdf1, 0, xe->xdf1.nreff, &xe->xdf2, 0, xe->xdf2.nreff,
			   kvdf, kvdb, (xpp->flags & XDF_NEED_گهٽتر) != 0,
			   &xenv);
	xdl_free(kvd);
 ٻاهريون:
	إذا (res < 0)
		xdl_free_env(xe);

	أرجع res;
}


ساكن xdchange_t *xdl_جوڙيو_change(xdchange_t *xscr, ڊگهو i1, ڊگهو i2, ڊگهو chg1, ڊگهو chg2) {
	xdchange_t *xch;

	إذا (!(xch = (xdchange_t *) xdl_malloc(حجم(xdchange_t))))
		أرجع عدم;

	xch->ايندڙ = xscr;
	xch->i1 = i1;
	xch->i2 = i2;
	xch->chg1 = chg1;
	xch->chg2 = chg2;
	xch->ignore = 0;

	أرجع xch;
}


ساكن صحيح recs_ميلاپ(xrecord_t *rec1, xrecord_t *rec2)
{
	أرجع (rec1->ha == rec2->ha);
}

/*
 * If a line is indented more than this, xget_indent() just returns this value.
 * This avoids having to do absurd amounts of work for data that are not
 * human-readable text, and also ensures that the output of xget_indent fits within
 * an int.
 */
#وصف_ڪريو MAX_INDENT 200

/*
 * Return the amount of indentation of the specified line, treating TAB as 8
 * columns. Return -1 if line is empty or contains only whitespace. Clamp the
 * output value at MAX_INDENT.
 */
ساكن صحيح xget_indent(xrecord_t *rec)
{
	ڊگهو i;
	صحيح ret = 0;

	لكل (i = 0; i < rec->سائيز; i++) {
		حرف c = rec->ptr[i];

		إذا (!XDL_ISSPACE(c))
			أرجع ret;
		وإلا إذا (c == ' ')
			ret += 1;
		وإلا إذا (c == '\t')
			ret += 8 - ret % 8;
		/* ignore other whitespace characters */

		إذا (ret >= MAX_INDENT)
			أرجع MAX_INDENT;
	}

	/* The line contains only whitespace. */
	أرجع -1;
}

/*
 * If more than this number of consecutive blank rows are found, just return
 * this value. This avoids requiring O(N^2) work for pathological cases, and
 * also ensures that the output of score_split fits in an int.
 */
#وصف_ڪريو MAX_BLANKS 20

/* Characteristics measured about a hypothetical split position. */
بنية ورهايو_measurement {
	/*
	 * Is the split at the end of the file (aside from any blank lines)?
	 */
	صحيح پڇاڙي_جو_file;

	/*
	 * How much is the line immediately following the split indented (or -1
	 * if the line is blank):
	 */
	صحيح indent;

	/*
	 * How many consecutive lines above the split are blank?
	 */
	صحيح pre_blank;

	/*
	 * How much is the nearest non-blank line above the split indented (or
	 * -1 if there is no such line)?
	 */
	صحيح pre_indent;

	/*
	 * How many lines after the line following the split are blank?
	 */
	صحيح post_blank;

	/*
	 * How much is the nearest non-blank line after the line following the
	 * split indented (or -1 if there is no such line)?
	 */
	صحيح post_indent;
};

بنية ورهايو_score {
	/* The effective indent of this split (smaller is preferred). */
	صحيح effective_indent;

	/* Penalty for this split (smaller is preferred). */
	صحيح penalty;
};

/*
 * Fill m with information about a hypothetical split of xdf above line split.
 */
ساكن فراغ measure_ورهايو(ثابت xdfile_t *xdf, ڊگهو ورهايو,
			  بنية ورهايو_measurement *m)
{
	ڊگهو i;

	إذا (ورهايو >= xdf->nrec) {
		m->پڇاڙي_جو_file = 1;
		m->indent = -1;
	} وإلا {
		m->پڇاڙي_جو_file = 0;
		m->indent = xget_indent(&xdf->recs[ورهايو]);
	}

	m->pre_blank = 0;
	m->pre_indent = -1;
	لكل (i = ورهايو - 1; i >= 0; i--) {
		m->pre_indent = xget_indent(&xdf->recs[i]);
		إذا (m->pre_indent != -1)
			اكسر;
		m->pre_blank += 1;
		إذا (m->pre_blank == MAX_BLANKS) {
			m->pre_indent = 0;
			اكسر;
		}
	}

	m->post_blank = 0;
	m->post_indent = -1;
	لكل (i = ورهايو + 1; i < xdf->nrec; i++) {
		m->post_indent = xget_indent(&xdf->recs[i]);
		إذا (m->post_indent != -1)
			اكسر;
		m->post_blank += 1;
		إذا (m->post_blank == MAX_BLANKS) {
			m->post_indent = 0;
			اكسر;
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
#وصف_ڪريو شروعات_جو_FILE_PENALTY 1

/* Penalty if there are no non-blank lines after the split */
#وصف_ڪريو پڇاڙي_جو_FILE_PENALTY 21

/* Multiplier for the number of blank lines around the split */
#وصف_ڪريو ڪل_BLANK_وزن (-30)

/* Multiplier for the number of blank lines after the split */
#وصف_ڪريو POST_BLANK_وزن 6

/*
 * Penalties applied if the line is indented more than its predecessor
 */
#وصف_ڪريو RELATIVE_INDENT_PENALTY (-4)
#وصف_ڪريو RELATIVE_INDENT_WITH_BLANK_PENALTY 10

/*
 * Penalties applied if the line is indented less than both its predecessor and
 * its successor
 */
#وصف_ڪريو RELATIVE_OUTDENT_PENALTY 24
#وصف_ڪريو RELATIVE_OUTDENT_WITH_BLANK_PENALTY 17

/*
 * Penalties applied if the line is indented less than its predecessor but not
 * less than its successor
 */
#وصف_ڪريو RELATIVE_DEDENT_PENALTY 23
#وصف_ڪريو RELATIVE_DEDENT_WITH_BLANK_PENALTY 17

/*
 * We only consider whether the sum of the effective indents for splits are
 * less than (-1), equal to (0), or greater than (+1) each other. The resulting
 * value is multiplied by the following weight and combined with the penalty to
 * determine the better of two scores.
 */
#وصف_ڪريو INDENT_وزن 60

/*
 * How far do we slide a hunk at most?
 */
#وصف_ڪريو INDENT_HEURISTIC_MAX_SLIDING 100

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
ساكن فراغ score_جوڙيو_ورهايو(ثابت بنية ورهايو_measurement *m, بنية ورهايو_score *s)
{
	/*
	 * A place to accumulate penalty factors (positive makes this index more
	 * favored):
	 */
	صحيح post_blank, ڪل_blank, indent, any_blanks;

	إذا (m->pre_indent == -1 && m->pre_blank == 0)
		s->penalty += شروعات_جو_FILE_PENALTY;

	إذا (m->پڇاڙي_جو_file)
		s->penalty += پڇاڙي_جو_FILE_PENALTY;

	/*
	 * Set post_blank to the number of blank lines following the split,
	 * including the line immediately after the split:
	 */
	post_blank = (m->indent == -1) ? 1 + m->post_blank : 0;
	ڪل_blank = m->pre_blank + post_blank;

	/* Penalties based on nearby blank lines: */
	s->penalty += ڪل_BLANK_وزن * ڪل_blank;
	s->penalty += POST_BLANK_وزن * post_blank;

	إذا (m->indent != -1)
		indent = m->indent;
	وإلا
		indent = m->post_indent;

	any_blanks = (ڪل_blank != 0);

	/* Note that the effective indent is -1 at the end of the file: */
	s->effective_indent += indent;

	إذا (indent == -1) {
		/* No additional adjustments needed. */
	} وإلا إذا (m->pre_indent == -1) {
		/* No additional adjustments needed. */
	} وإلا إذا (indent > m->pre_indent) {
		/*
		 * The line is indented more than its predecessor.
		 */
		s->penalty += any_blanks ?
			RELATIVE_INDENT_WITH_BLANK_PENALTY :
			RELATIVE_INDENT_PENALTY;
	} وإلا إذا (indent == m->pre_indent) {
		/*
		 * The line has the same indentation level as its predecessor.
		 * No additional adjustments needed.
		 */
	} وإلا {
		/*
		 * The line is indented less than its predecessor. It could be
		 * the block terminator of the previous block, but it could
		 * also be the start of a new block (e.g., an "else" block, or
		 * maybe the previous block didn't have a block terminator).
		 * Try to distinguish those cases based on what comes next:
		 */
		إذا (m->post_indent != -1 && m->post_indent > indent) {
			/*
			 * The following line is indented more. So it is likely
			 * that this line is the start of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_OUTDENT_WITH_BLANK_PENALTY :
				RELATIVE_OUTDENT_PENALTY;
		} وإلا {
			/*
			 * That was probably the end of a block.
			 */
			s->penalty += any_blanks ?
				RELATIVE_DEDENT_WITH_BLANK_PENALTY :
				RELATIVE_DEDENT_PENALTY;
		}
	}
}

ساكن صحيح score_cmp(بنية ورهايو_score *s1, بنية ورهايو_score *s2)
{
	/* -1 if s1.effective_indent < s2->effective_indent, etc. */
	صحيح cmp_indents = ((s1->effective_indent > s2->effective_indent) -
			   (s1->effective_indent < s2->effective_indent));

	أرجع INDENT_وزن * cmp_indents + (s1->penalty - s2->penalty);
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
بنية xdlgroup {
	/*
	 * The index of the first changed line in the group, or the index of
	 * the unchanged line above which the (empty) group is located.
	 */
	ڊگهو شروعات;

	/*
	 * The index of the first unchanged line after the group. For an empty
	 * group, end is equal to start.
	 */
	ڊگهو پڇاڙي;
};

/*
 * Initialize g to point at the first group in xdf.
 */
ساكن فراغ گروهه_init(xdfile_t *xdf, بنية xdlgroup *g)
{
	g->شروعات = g->پڇاڙي = 0;
	طالما (xdf->بدليو[g->پڇاڙي])
		g->پڇاڙي++;
}

/*
 * Move g to describe the next (possibly empty) group in xdf and return 0. If g
 * is already at the end of the file, do nothing and return -1.
 */
ساكن مضمن صحيح گروهه_ايندڙ(xdfile_t *xdf, بنية xdlgroup *g)
{
	إذا (g->پڇاڙي == xdf->nrec)
		أرجع -1;

	g->شروعات = g->پڇاڙي + 1;
	لكل (g->پڇاڙي = g->شروعات; xdf->بدليو[g->پڇاڙي]; g->پڇاڙي++)
		;

	أرجع 0;
}

/*
 * Move g to describe the previous (possibly empty) group in xdf and return 0.
 * If g is already at the beginning of the file, do nothing and return -1.
 */
ساكن مضمن صحيح گروهه_اڳيون(xdfile_t *xdf, بنية xdlgroup *g)
{
	إذا (g->شروعات == 0)
		أرجع -1;

	g->پڇاڙي = g->شروعات - 1;
	لكل (g->شروعات = g->پڇاڙي; xdf->بدليو[g->شروعات - 1]; g->شروعات--)
		;

	أرجع 0;
}

/*
 * If g can be slid toward the end of the file, do so, and if it bumps into a
 * following group, expand this group to include it. Return 0 on success or -1
 * if g cannot be slid down.
 */
ساكن صحيح گروهه_slide_هيٺ_ڏانهن(xdfile_t *xdf, بنية xdlgroup *g)
{
	إذا (g->پڇاڙي < xdf->nrec &&
	    recs_ميلاپ(&xdf->recs[g->شروعات], &xdf->recs[g->پڇاڙي])) {
		xdf->بدليو[g->شروعات++] = false;
		xdf->بدليو[g->پڇاڙي++] = true;

		طالما (xdf->بدليو[g->پڇاڙي])
			g->پڇاڙي++;

		أرجع 0;
	} وإلا {
		أرجع -1;
	}
}

/*
 * If g can be slid toward the beginning of the file, do so, and if it bumps
 * into a previous group, expand this group to include it. Return 0 on success
 * or -1 if g cannot be slid up.
 */
ساكن صحيح گروهه_slide_مٿي(xdfile_t *xdf, بنية xdlgroup *g)
{
	إذا (g->شروعات > 0 &&
	    recs_ميلاپ(&xdf->recs[g->شروعات - 1], &xdf->recs[g->پڇاڙي - 1])) {
		xdf->بدليو[--g->شروعات] = true;
		xdf->بدليو[--g->پڇاڙي] = false;

		طالما (xdf->بدليو[g->شروعات - 1])
			g->شروعات--;

		أرجع 0;
	} وإلا {
		أرجع -1;
	}
}

فراغ xdl_bug(ثابت حرف *نياپو)
{
	fprintf(stderr, "BUG: %s\n", نياپو);
	ٻاهر_نڪرو(1);
}

/*
 * Move back and forward change groups for a consistent and pretty diff output.
 * This also helps in finding joinable change groups and reducing the diff
 * size.
 */
صحيح xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, ڊگهو flags) {
	بنية xdlgroup g, go;
	ڊگهو earliest_پڇاڙي, پڇاڙي_matching_other;
	ڊگهو groupsize;

	گروهه_init(xdf, &g);
	گروهه_init(xdfo, &go);

	طالما (1) {
		/*
		 * If the group is empty in the to-be-compacted file, skip it:
		 */
		إذا (g.پڇاڙي == g.شروعات)
			اذهب ايندڙ;

		/*
		 * Now shift the change up and then down as far as possible in
		 * each direction. If it bumps into any other changes, merge
		 * them.
		 */
		افعل {
			groupsize = g.پڇاڙي - g.شروعات;

			/*
			 * Keep track of the last "end" index that causes this
			 * group to align with a group of changed lines in the
			 * other file. -1 indicates that we haven't found such
			 * a match yet:
			 */
			پڇاڙي_matching_other = -1;

			/* Shift the group backward as much as possible: */
			طالما (!گروهه_slide_مٿي(xdf, &g))
				إذا (گروهه_اڳيون(xdfo, &go))
					xdl_bug("group sync broken sliding up");

			/*
			 * This is this highest that this group can be shifted.
			 * Record its end index:
			 */
			earliest_پڇاڙي = g.پڇاڙي;

			إذا (go.پڇاڙي > go.شروعات)
				پڇاڙي_matching_other = g.پڇاڙي;

			/* Now shift the group forward as far as possible: */
			طالما (1) {
				إذا (گروهه_slide_هيٺ_ڏانهن(xdf, &g))
					اكسر;
				إذا (گروهه_ايندڙ(xdfo, &go))
					xdl_bug("group sync broken sliding down");

				إذا (go.پڇاڙي > go.شروعات)
					پڇاڙي_matching_other = g.پڇاڙي;
			}
		} طالما (groupsize != g.پڇاڙي - g.شروعات);

		/*
		 * If the group can be shifted, then we can possibly use this
		 * freedom to produce a more intuitive diff.
		 *
		 * The group is currently shifted as far down as possible, so
		 * the heuristics below only have to handle upwards shifts.
		 */

		إذا (g.پڇاڙي == earliest_پڇاڙي) {
			/* no shifting was possible */
		} وإلا إذا (پڇاڙي_matching_other != -1) {
			/*
			 * Move the possibly merged group of changes back to
			 * line up with the last group of changes from the
			 * other file that it can align with.
			 */
			طالما (go.پڇاڙي == go.شروعات) {
				إذا (گروهه_slide_مٿي(xdf, &g))
					xdl_bug("match disappeared");
				إذا (گروهه_اڳيون(xdfo, &go))
					xdl_bug("group sync broken sliding to match");
			}
		} وإلا إذا (flags & XDF_INDENT_HEURISTIC) {
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
			ڊگهو سرڪايو, بهترين_سرڪايو = -1;
			بنية ورهايو_score بهترين_score;

			سرڪايو = earliest_پڇاڙي;
			إذا (g.پڇاڙي - groupsize - 1 > سرڪايو)
				سرڪايو = g.پڇاڙي - groupsize - 1;
			إذا (g.پڇاڙي - INDENT_HEURISTIC_MAX_SLIDING > سرڪايو)
				سرڪايو = g.پڇاڙي - INDENT_HEURISTIC_MAX_SLIDING;
			لكل (; سرڪايو <= g.پڇاڙي; سرڪايو++) {
				بنية ورهايو_measurement m;
				بنية ورهايو_score score = {0, 0};

				measure_ورهايو(xdf, سرڪايو, &m);
				score_جوڙيو_ورهايو(&m, &score);
				measure_ورهايو(xdf, سرڪايو - groupsize, &m);
				score_جوڙيو_ورهايو(&m, &score);
				إذا (بهترين_سرڪايو == -1 ||
				    score_cmp(&score, &بهترين_score) <= 0) {
					بهترين_score.effective_indent = score.effective_indent;
					بهترين_score.penalty = score.penalty;
					بهترين_سرڪايو = سرڪايو;
				}
			}

			طالما (g.پڇاڙي > بهترين_سرڪايو) {
				إذا (گروهه_slide_مٿي(xdf, &g))
					xdl_bug("best shift unreached");
				إذا (گروهه_اڳيون(xdfo, &go))
					xdl_bug("group sync broken sliding to blank line");
			}
		}

	ايندڙ:
		/* Move past the just-processed group: */
		إذا (گروهه_ايندڙ(xdf, &g))
			اكسر;
		إذا (گروهه_ايندڙ(xdfo, &go))
			xdl_bug("group sync broken moving to next group");
	}

	إذا (!گروهه_ايندڙ(xdfo, &go))
		xdl_bug("group sync broken at end of file");

	أرجع 0;
}


صحيح xdl_build_script(xdfenv_t *xe, xdchange_t **xscr) {
	xdchange_t *cscr = عدم, *xch;
	bool *changed1 = xe->xdf1.بدليو, *changed2 = xe->xdf2.بدليو;
	ڊگهو i1, i2, l1, l2;

	/*
	 * Trivial. Collects "groups" of changes and creates an edit script.
	 */
	لكل (i1 = xe->xdf1.nrec, i2 = xe->xdf2.nrec; i1 >= 0 || i2 >= 0; i1--, i2--)
		إذا (changed1[i1 - 1] || changed2[i2 - 1]) {
			لكل (l1 = i1; changed1[i1 - 1]; i1--);
			لكل (l2 = i2; changed2[i2 - 1]; i2--);

			إذا (!(xch = xdl_جوڙيو_change(cscr, i1, i2, l1 - i1, l2 - i2))) {
				xdl_free_script(cscr);
				أرجع -1;
			}
			cscr = xch;
		}

	*xscr = cscr;

	أرجع 0;
}


فراغ xdl_free_script(xdchange_t *xscr) {
	xdchange_t *xch;

	طالما ((xch = xscr) != عدم) {
		xscr = xscr->ايندڙ;
		xdl_free(xch);
	}
}

ساكن صحيح xdl_call_hunk_func(xdfenv_t *xe اڻ_استعمال_ٿيل, xdchange_t *xscr, xdemitcb_t *ecb,
			      xdemitconf_t ثابت *xecfg)
{
	xdchange_t *xch, *xche;

	لكل (xch = xscr; xch; xch = xche->ايندڙ) {
		xche = xdl_get_hunk(&xch, xecfg);
		إذا (!xch)
			اكسر;
		إذا (xecfg->hunk_func(xch->i1, xche->i1 + xche->chg1 - xch->i1,
				     xch->i2, xche->i2 + xche->chg2 - xch->i2,
				     ecb->priv) < 0)
			أرجع -1;
	}
	أرجع 0;
}

ساكن فراغ xdl_mark_ignorable_سٽون(xdchange_t *xscr, xdfenv_t *xe, ڊگهو flags)
{
	xdchange_t *xch;

	لكل (xch = xscr; xch; xch = xch->ايندڙ) {
		صحيح ignore = 1;
		xrecord_t *rec;
		ڊگهو i;

		rec = &xe->xdf1.recs[xch->i1];
		لكل (i = 0; i < xch->chg1 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].سائيز, flags);

		rec = &xe->xdf2.recs[xch->i2];
		لكل (i = 0; i < xch->chg2 && ignore; i++)
			ignore = xdl_blankline(rec[i].ptr, rec[i].سائيز, flags);

		xch->ignore = ignore;
	}
}

#إذا 0 // unused by Vim
ساكن صحيح رڪارڊ_matches_regex(xrecord_t *rec, xpparam_t ثابت *xpp) {
	regmatch_t regmatch;
	size_t i;

	لكل (i = 0; i < xpp->ignore_regex_nr; i++)
		إذا (!regexec_buf(xpp->ignore_regex[i], rec->ptr, rec->سائيز, 1,
				 &regmatch, 0))
			أرجع 1;

	أرجع 0;
}

ساكن فراغ xdl_mark_ignorable_regex(xdchange_t *xscr, ثابت xdfenv_t *xe,
				     xpparam_t ثابت *xpp)
{
	xdchange_t *xch;

	لكل (xch = xscr; xch; xch = xch->ايندڙ) {
		xrecord_t *rec;
		صحيح ignore = 1;
		ڊگهو i;

		/*
		 * Do not override --ignore-blank-lines.
		 */
		إذا (xch->ignore)
			استمر;

		rec = &xe->xdf1.recs[xch->i1];
		لكل (i = 0; i < xch->chg1 && ignore; i++)
			ignore = رڪارڊ_matches_regex(&rec[i], xpp);

		rec = &xe->xdf2.recs[xch->i2];
		لكل (i = 0; i < xch->chg2 && ignore; i++)
			ignore = رڪارڊ_matches_regex(&rec[i], xpp);

		xch->ignore = ignore;
	}
}
#جيڪڏهن_پورو

صحيح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb) {
	xdchange_t *xscr;
	xdfenv_t xe;
	emit_func_t ef = xecfg->hunk_func ? xdl_call_hunk_func : xdl_emit_diff;

	إذا (xdl_do_diff(mf1, mf2, xpp, &xe) < 0) {

		أرجع -1;
	}
	إذا (xdl_change_compact(&xe.xdf1, &xe.xdf2, xpp->flags) < 0 ||
	    xdl_change_compact(&xe.xdf2, &xe.xdf1, xpp->flags) < 0 ||
	    xdl_build_script(&xe, &xscr) < 0) {

		xdl_free_env(&xe);
		أرجع -1;
	}
	إذا (xscr) {
		إذا (xpp->flags & XDF_IGNORE_BLANK_سٽون)
			xdl_mark_ignorable_سٽون(xscr, &xe, xpp->flags);

#إذا 0
		إذا (xpp->ignore_regex)
			xdl_mark_ignorable_regex(xscr, &xe, xpp);
#جيڪڏهن_پورو

		إذا (ef(&xe, xscr, ecb, xecfg) < 0) {

			xdl_free_script(xscr);
			xdl_free_env(&xe);
			أرجع -1;
		}
		xdl_free_script(xscr);
	}
	xdl_free_env(&xe);

	أرجع 0;
}
