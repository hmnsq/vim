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

#ଯଦି !defined(XDIFF_H)
#define XDIFF_H

#ifdef __cplusplus
ବାହ୍ୟ "C" {
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

#define XDF_IGNORE_BLANK_ଧାଡ଼ିଗୁଡ଼ିକ (1 << 7)

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_mmfile {
	ଅକ୍ଷର *ptr;
	long size;
} mmfile_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_mmbuffer {
	ଅକ୍ଷର *ptr;
	long size;
} mmbuffer_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_xpparam {
	unsigned long flags;

	/* -I<regex> */
 #ଯଦି 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#endif

	/* See Documentation/diff-options.adoc. */
	ଅକ୍ଷର **anchors;
	size_t anchors_nr;
} xpparam_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_xdemitcb {
	ଶୂନ୍ୟ *priv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*out_hunk)(ଶୂନ୍ୟ *,
			long old_begin, long old_nr,
			long new_begin, long new_nr,
			ଅଚଳ ଅକ୍ଷର *func, long funclen);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*out_ଧାଡ଼ି)(ଶୂନ୍ୟ *, mmbuffer_t *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
} xdemitcb_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା long (*find_func_t)(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, long ଧାଡ଼ି_len, ଅକ୍ଷର *buffer, long buffer_size, ଶୂନ୍ୟ *priv);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*xdl_emit_hunk_consume_func_t)(long start_a, long ଗଣନା_a,
					    long start_b, long ଗଣନା_b,
					    ଶୂନ୍ୟ *cb_data);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_xdemitconf {
	long ctxlen;
	long interhunkctxlen;
	unsigned long flags;
	find_func_t find_func;
	ଶୂନ୍ୟ *find_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_bdiffparam {
	long bsize;
} bdiffparam_t;

#ifdef VMS
# include "[]vim.h"
#ଅନ୍ୟଥା
# include "../vim.h"
#endif

#define xdl_malloc(x) lalloc((x), TRUE)
#define xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#define xdl_free(ptr) vim_free(ptr)
#define xdl_realloc(ptr,x) vim_realloc((ptr),(x))

ଶୂନ୍ୟ *xdl_mmfile_first(mmfile_t *mmf, long *size);
long xdl_mmfile_size(mmfile_t *mmf);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ଅଚଳ *xpp,
	     xdemitconf_t ଅଚଳ *xecfg, xdemitcb_t *ecb);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା s_xmparam {
	xpparam_t xpp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା marker_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା level;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା favor;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା style;
	ଅଚଳ ଅକ୍ଷର *ancestor;	/* label for orig */
	ଅଚଳ ଅକ୍ଷର *file1;	/* label for mf1 */
	ଅଚଳ ଅକ୍ଷର *file2;	/* label for mf2 */
} xmparam_t;

#define DEFAULT_CONFLICT_MARKER_SIZE 7

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_merge(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ଅଚଳ *xmp, mmbuffer_t *result);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if !defined(XDIFF_H) */
