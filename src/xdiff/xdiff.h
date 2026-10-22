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

#যদি !defined(XDIFF_H)
#define XDIFF_H

#ifdef __cplusplus
বাহ্য "C" {
#endif /* #ifdef __cplusplus */

/* xpparm_t.flags */
#define XDF_NEED_MINIMAL (1 << 0)

#define XDF_IGNORE_WHITESPACE (1 << 1)
#define XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#define XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#define XDF_IGNORE_CR_AT_EOL (1 << 4)
#define XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#define XDF_IGNORE_BLANK_সারিগুলি (1 << 7)

#define XDF_PATIENCE_DIFF (1 << 14)
#define XDF_HISTOGRAM_DIFF (1 << 15)
#define XDF_DIFF_ALGORITHM_MASK (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#define XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_MASK)

#define XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#define XDL_EMIT_FUNCNAMES (1 << 0)
#define XDL_EMIT_NO_HUNK_HDR (1 << 1)
#define XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#define XDL_MERGE_MINIMAL 0
#define XDL_MERGE_EAGER 1
#define XDL_MERGE_ZEALOUS 2
#define XDL_MERGE_ZEALOUS_ALNUM 3

/* merge favor modes */
#define XDL_MERGE_FAVOR_OURS 1
#define XDL_MERGE_FAVOR_THEIRS 2
#define XDL_MERGE_FAVOR_UNION 3

/* merge output styles */
#define XDL_MERGE_DIFF3 1
#define XDL_MERGE_ZEALOUS_DIFF3 2

প্রকারনির্দেশ সংরচনা s_mmfile {
	বর্ণ *ptr;
	long size;
} mmfile_t;

প্রকারনির্দেশ সংরচনা s_mmbuffer {
	বর্ণ *ptr;
	long size;
} mmbuffer_t;

প্রকারনির্দেশ সংরচনা s_xpparam {
	unsigned long flags;

	/* -I<regex> */
 #যদি 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#endif

	/* See Documentation/diff-options.adoc. */
	বর্ণ **anchors;
	size_t anchors_nr;
} xpparam_t;

প্রকারনির্দেশ সংরচনা s_xdemitcb {
	শূন্য *priv;
	পূর্ণাঙ্ক (*out_hunk)(শূন্য *,
			long old_begin, long old_nr,
			long new_begin, long new_nr,
			ধ্রুব বর্ণ *func, long funclen);
	পূর্ণাঙ্ক (*out_সারি)(শূন্য *, mmbuffer_t *, পূর্ণাঙ্ক);
} xdemitcb_t;

প্রকারনির্দেশ long (*find_func_t)(ধ্রুব বর্ণ *সারি, long সারি_len, বর্ণ *buffer, long buffer_size, শূন্য *priv);

প্রকারনির্দেশ পূর্ণাঙ্ক (*xdl_emit_hunk_consume_func_t)(long start_a, long গুনতি_a,
					    long start_b, long গুনতি_b,
					    শূন্য *cb_data);

প্রকারনির্দেশ সংরচনা s_xdemitconf {
	long ctxlen;
	long interhunkctxlen;
	unsigned long flags;
	find_func_t find_func;
	শূন্য *find_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

প্রকারনির্দেশ সংরচনা s_bdiffparam {
	long bsize;
} bdiffparam_t;

#ifdef VMS
# include "[]vim.h"
#অন্যথা
# include "../vim.h"
#endif

#define xdl_malloc(x) lalloc((x), TRUE)
#define xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#define xdl_free(ptr) vim_free(ptr)
#define xdl_realloc(ptr,x) vim_realloc((ptr),(x))

শূন্য *xdl_mmfile_first(mmfile_t *mmf, long *size);
long xdl_mmfile_size(mmfile_t *mmf);

পূর্ণাঙ্ক xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ধ্রুব *xpp,
	     xdemitconf_t ধ্রুব *xecfg, xdemitcb_t *ecb);

প্রকারনির্দেশ সংরচনা s_xmparam {
	xpparam_t xpp;
	পূর্ণাঙ্ক marker_size;
	পূর্ণাঙ্ক level;
	পূর্ণাঙ্ক favor;
	পূর্ণাঙ্ক style;
	ধ্রুব বর্ণ *ancestor;	/* label for orig */
	ধ্রুব বর্ণ *file1;	/* label for mf1 */
	ধ্রুব বর্ণ *file2;	/* label for mf2 */
} xmparam_t;

#define DEFAULT_CONFLICT_MARKER_SIZE 7

পূর্ণাঙ্ক xdl_merge(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ধ্রুব *xmp, mmbuffer_t *result);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if !defined(XDIFF_H) */
