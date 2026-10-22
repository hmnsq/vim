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

#जर !defined(XDIFF_H)
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

#define XDF_IGNORE_BLANK_ओळी (1 << 7)

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

प्रकारव्याख्या संरचना s_mmfile {
	अक्षर *ptr;
	दीर्घ size;
} mmfile_t;

प्रकारव्याख्या संरचना s_mmbuffer {
	अक्षर *ptr;
	दीर्घ size;
} mmbuffer_t;

प्रकारव्याख्या संरचना s_xpparam {
	अचिन्हित दीर्घ flags;

	/* -I<regex> */
 #जर 0  // unused by Vim
	regex_t **ignore_regex;
	माप_प्रकार ignore_regex_nr;
#endif

	/* See Documentation/diff-options.adoc. */
	अक्षर **anchors;
	माप_प्रकार anchors_nr;
} xpparam_t;

प्रकारव्याख्या संरचना s_xdemitcb {
	शून्य *priv;
	पूर्णांक (*out_hunk)(शून्य *,
			दीर्घ old_begin, दीर्घ old_nr,
			दीर्घ new_begin, दीर्घ new_nr,
			स्थिरांक अक्षर *func, दीर्घ funclen);
	पूर्णांक (*out_ओळ)(शून्य *, mmbuffer_t *, पूर्णांक);
} xdemitcb_t;

प्रकारव्याख्या दीर्घ (*find_func_t)(स्थिरांक अक्षर *ओळ, दीर्घ ओळ_len, अक्षर *buffer, दीर्घ buffer_size, शून्य *priv);

प्रकारव्याख्या पूर्णांक (*xdl_emit_hunk_consume_func_t)(दीर्घ start_a, दीर्घ मोजणी_a,
					    दीर्घ start_b, दीर्घ मोजणी_b,
					    शून्य *cb_data);

प्रकारव्याख्या संरचना s_xdemitconf {
	दीर्घ ctxlen;
	दीर्घ interhunkctxlen;
	अचिन्हित दीर्घ flags;
	find_func_t find_func;
	शून्य *find_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

प्रकारव्याख्या संरचना s_bdiffparam {
	दीर्घ bsize;
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

शून्य *xdl_mmfile_first(mmfile_t *mmf, दीर्घ *size);
दीर्घ xdl_mmfile_size(mmfile_t *mmf);

पूर्णांक xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t स्थिरांक *xpp,
	     xdemitconf_t स्थिरांक *xecfg, xdemitcb_t *ecb);

प्रकारव्याख्या संरचना s_xmparam {
	xpparam_t xpp;
	पूर्णांक marker_size;
	पूर्णांक level;
	पूर्णांक favor;
	पूर्णांक style;
	स्थिरांक अक्षर *ancestor;	/* label for orig */
	स्थिरांक अक्षर *file1;	/* label for mf1 */
	स्थिरांक अक्षर *file2;	/* label for mf2 */
} xmparam_t;

#define DEFAULT_CONFLICT_MARKER_SIZE 7

पूर्णांक xdl_merge(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t स्थिरांक *xmp, mmbuffer_t *result);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if !defined(XDIFF_H) */
