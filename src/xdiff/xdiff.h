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

#यदि !defined(XDIFF_H)
#define XDIFF_H

#ifdef __cplusplus
बाह्य "C" {
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

#define XDF_IGNORE_BLANK_पंक्तियाँ (1 << 7)

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

प्रकारनिर्देश संरचना s_mmfile {
	वर्ण *ptr;
	long size;
} mmfile_t;

प्रकारनिर्देश संरचना s_mmbuffer {
	वर्ण *ptr;
	long size;
} mmbuffer_t;

प्रकारनिर्देश संरचना s_xpparam {
	unsigned long flags;

	/* -I<regex> */
 #यदि 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#endif

	/* See Documentation/diff-options.adoc. */
	वर्ण **anchors;
	size_t anchors_nr;
} xpparam_t;

प्रकारनिर्देश संरचना s_xdemitcb {
	शून्य *priv;
	पूर्णाङ्क (*out_hunk)(शून्य *,
			long old_begin, long old_nr,
			long new_begin, long new_nr,
			ध्रुव वर्ण *func, long funclen);
	पूर्णाङ्क (*out_पंक्ति)(शून्य *, mmbuffer_t *, पूर्णाङ्क);
} xdemitcb_t;

प्रकारनिर्देश long (*find_func_t)(ध्रुव वर्ण *पंक्ति, long पंक्ति_len, वर्ण *buffer, long buffer_size, शून्य *priv);

प्रकारनिर्देश पूर्णाङ्क (*xdl_emit_hunk_consume_func_t)(long start_a, long गिनती_a,
					    long start_b, long गिनती_b,
					    शून्य *cb_data);

प्रकारनिर्देश संरचना s_xdemitconf {
	long ctxlen;
	long interhunkctxlen;
	unsigned long flags;
	find_func_t find_func;
	शून्य *find_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

प्रकारनिर्देश संरचना s_bdiffparam {
	long bsize;
} bdiffparam_t;

#ifdef VMS
# include "[]vim.h"
#अन्यथा
# include "../vim.h"
#endif

#define xdl_malloc(x) lalloc((x), TRUE)
#define xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#define xdl_free(ptr) vim_free(ptr)
#define xdl_realloc(ptr,x) vim_realloc((ptr),(x))

शून्य *xdl_mmfile_first(mmfile_t *mmf, long *size);
long xdl_mmfile_size(mmfile_t *mmf);

पूर्णाङ्क xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ध्रुव *xpp,
	     xdemitconf_t ध्रुव *xecfg, xdemitcb_t *ecb);

प्रकारनिर्देश संरचना s_xmparam {
	xpparam_t xpp;
	पूर्णाङ्क marker_size;
	पूर्णाङ्क level;
	पूर्णाङ्क favor;
	पूर्णाङ्क style;
	ध्रुव वर्ण *ancestor;	/* label for orig */
	ध्रुव वर्ण *file1;	/* label for mf1 */
	ध्रुव वर्ण *file2;	/* label for mf2 */
} xmparam_t;

#define DEFAULT_CONFLICT_MARKER_SIZE 7

पूर्णाङ्क xdl_merge(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ध्रुव *xmp, mmbuffer_t *result);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if !defined(XDIFF_H) */
