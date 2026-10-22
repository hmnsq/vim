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

#إذا !defined(XDIFF_H)
#تعریف_کړه XDIFF_H

#که_تعریف_شوی __cplusplus
خارجي "C" {
#د_که_پای /* #ifdef __cplusplus */

/* xpparm_t.flags */
#تعریف_کړه XDF_NEED_اصغري (1 << 0)

#تعریف_کړه XDF_IGNORE_WHITESPACE (1 << 1)
#تعریف_کړه XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#تعریف_کړه XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#تعریف_کړه XDF_IGNORE_CR_AT_EOL (1 << 4)
#تعریف_کړه XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#تعریف_کړه XDF_IGNORE_BLANK_کرښې (1 << 7)

#تعریف_کړه XDF_PATIENCE_DIFF (1 << 14)
#تعریف_کړه XDF_HISTOGRAM_DIFF (1 << 15)
#تعریف_کړه XDF_DIFF_ALGORITHM_ماسک (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#تعریف_کړه XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_ماسک)

#تعریف_کړه XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#تعریف_کړه XDL_EMIT_FUNCNAMES (1 << 0)
#تعریف_کړه XDL_EMIT_NO_HUNK_HDR (1 << 1)
#تعریف_کړه XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#تعریف_کړه XDL_یوځای_اصغري 0
#تعریف_کړه XDL_یوځای_EAGER 1
#تعریف_کړه XDL_یوځای_ZEALOUS 2
#تعریف_کړه XDL_یوځای_ZEALOUS_ALNUM 3

/* merge favor modes */
#تعریف_کړه XDL_یوځای_FAVOR_OURS 1
#تعریف_کړه XDL_یوځای_FAVOR_THEIRS 2
#تعریف_کړه XDL_یوځای_FAVOR_UNION 3

/* merge output styles */
#تعریف_کړه XDL_یوځای_DIFF3 1
#تعریف_کړه XDL_یوځای_ZEALOUS_DIFF3 2

تعريفنوع بنية s_mmfile {
	حرف *ptr;
	اوږد اندازه;
} mmfile_t;

تعريفنوع بنية s_mmbuffer {
	حرف *ptr;
	اوږد اندازه;
} mmbuffer_t;

تعريفنوع بنية s_xpparam {
	بینښه اوږد flags;

	/* -I<regex> */
 #إذا 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#د_که_پای

	/* See Documentation/diff-options.adoc. */
	حرف **anchors;
	size_t anchors_nr;
} xpparam_t;

تعريفنوع بنية s_xdemitcb {
	فراغ *priv;
	صحيح (*بهر_hunk)(فراغ *,
			اوږد old_begin, اوږد old_nr,
			اوږد new_begin, اوږد new_nr,
			ثابت حرف *func, اوږد funclen);
	صحيح (*بهر_کرښه)(فراغ *, mmbuffer_t *, صحيح);
} xdemitcb_t;

تعريفنوع اوږد (*پیدا_کړه_func_t)(ثابت حرف *کرښه, اوږد کرښه_len, حرف *بفر, اوږد بفر_اندازه_3, فراغ *priv);

تعريفنوع صحيح (*xdl_emit_hunk_consume_func_t)(اوږد پیل_a, اوږد شمېر_a,
					    اوږد پیل_b, اوږد شمېر_b,
					    فراغ *cb_معلومات_2);

تعريفنوع بنية s_xdemitconf {
	اوږد ctxlen;
	اوږد interhunkctxlen;
	بینښه اوږد flags;
	پیدا_کړه_func_t پیدا_کړه_func;
	فراغ *پیدا_کړه_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

تعريفنوع بنية s_bdiffparam {
	اوږد bsize;
} bdiffparam_t;

#که_تعریف_شوی VMS
# include "[]vim.h"
#وإلا
# include "../vim.h"
#د_که_پای

#تعریف_کړه xdl_malloc(x) lalloc((x), TRUE)
#تعریف_کړه xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#تعریف_کړه xdl_free(ptr) vim_free(ptr)
#تعریف_کړه xdl_realloc(ptr,x) vim_realloc((ptr),(x))

فراغ *xdl_mmfile_لومړی(mmfile_t *mmf, اوږد *اندازه);
اوږد xdl_mmfile_اندازه(mmfile_t *mmf);

صحيح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb);

تعريفنوع بنية s_xmparam {
	xpparam_t xpp;
	صحيح marker_اندازه;
	صحيح level;
	صحيح favor;
	صحيح style;
	ثابت حرف *ancestor;	/* label for orig */
	ثابت حرف *file1;	/* label for mf1 */
	ثابت حرف *file2;	/* label for mf2 */
} xmparam_t;

#تعریف_کړه DEFAULT_CONFLICT_MARKER_اندازه 7

صحيح xdl_یوځای(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ثابت *xmp, mmbuffer_t *پایله);

#که_تعریف_شوی __cplusplus
}
#د_که_پای /* #ifdef __cplusplus */

#د_که_پای /* #if !defined(XDIFF_H) */
