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
#وصف_ڪريو XDIFF_H

#جيڪڏهن_وصف_ٿيل __cplusplus
خارجي "C" {
#جيڪڏهن_پورو /* #ifdef __cplusplus */

/* xpparm_t.flags */
#وصف_ڪريو XDF_NEED_گهٽتر (1 << 0)

#وصف_ڪريو XDF_IGNORE_WHITESPACE (1 << 1)
#وصف_ڪريو XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#وصف_ڪريو XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#وصف_ڪريو XDF_IGNORE_CR_AT_EOL (1 << 4)
#وصف_ڪريو XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#وصف_ڪريو XDF_IGNORE_BLANK_سٽون (1 << 7)

#وصف_ڪريو XDF_PATIENCE_DIFF (1 << 14)
#وصف_ڪريو XDF_HISTOGRAM_DIFF (1 << 15)
#وصف_ڪريو XDF_DIFF_ALGORITHM_نقاب (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#وصف_ڪريو XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_نقاب)

#وصف_ڪريو XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#وصف_ڪريو XDL_EMIT_FUNCNAMES (1 << 0)
#وصف_ڪريو XDL_EMIT_NO_HUNK_HDR (1 << 1)
#وصف_ڪريو XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#وصف_ڪريو XDL_ملايو_گهٽتر 0
#وصف_ڪريو XDL_ملايو_EAGER 1
#وصف_ڪريو XDL_ملايو_ZEALOUS 2
#وصف_ڪريو XDL_ملايو_ZEALOUS_ALNUM 3

/* merge favor modes */
#وصف_ڪريو XDL_ملايو_FAVOR_OURS 1
#وصف_ڪريو XDL_ملايو_FAVOR_THEIRS 2
#وصف_ڪريو XDL_ملايو_FAVOR_UNION 3

/* merge output styles */
#وصف_ڪريو XDL_ملايو_DIFF3 1
#وصف_ڪريو XDL_ملايو_ZEALOUS_DIFF3 2

تعريفنوع بنية s_mmfile {
	حرف *ptr;
	ڊگهو سائيز;
} mmfile_t;

تعريفنوع بنية s_mmbuffer {
	حرف *ptr;
	ڊگهو سائيز;
} mmbuffer_t;

تعريفنوع بنية s_xpparam {
	بنانشان ڊگهو flags;

	/* -I<regex> */
 #إذا 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#جيڪڏهن_پورو

	/* See Documentation/diff-options.adoc. */
	حرف **anchors;
	size_t anchors_nr;
} xpparam_t;

تعريفنوع بنية s_xdemitcb {
	فراغ *priv;
	صحيح (*ٻاهريون_hunk)(فراغ *,
			ڊگهو old_begin, ڊگهو old_nr,
			ڊگهو new_begin, ڊگهو new_nr,
			ثابت حرف *func, ڊگهو funclen);
	صحيح (*ٻاهريون_سٽ)(فراغ *, mmbuffer_t *, صحيح);
} xdemitcb_t;

تعريفنوع ڊگهو (*ڳوليو_func_t)(ثابت حرف *سٽ, ڊگهو سٽ_len, حرف *بفر, ڊگهو بفر_سائيز_3, فراغ *priv);

تعريفنوع صحيح (*xdl_emit_hunk_consume_func_t)(ڊگهو شروعات_a, ڊگهو ڳڻپ_a,
					    ڊگهو شروعات_b, ڊگهو ڳڻپ_b,
					    فراغ *cb_معلومات_2);

تعريفنوع بنية s_xdemitconf {
	ڊگهو ctxlen;
	ڊگهو interhunkctxlen;
	بنانشان ڊگهو flags;
	ڳوليو_func_t ڳوليو_func;
	فراغ *ڳوليو_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

تعريفنوع بنية s_bdiffparam {
	ڊگهو bsize;
} bdiffparam_t;

#جيڪڏهن_وصف_ٿيل VMS
# include "[]vim.h"
#وإلا
# include "../vim.h"
#جيڪڏهن_پورو

#وصف_ڪريو xdl_malloc(x) lalloc((x), TRUE)
#وصف_ڪريو xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#وصف_ڪريو xdl_free(ptr) vim_free(ptr)
#وصف_ڪريو xdl_realloc(ptr,x) vim_realloc((ptr),(x))

فراغ *xdl_mmfile_پهريون(mmfile_t *mmf, ڊگهو *سائيز);
ڊگهو xdl_mmfile_سائيز(mmfile_t *mmf);

صحيح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb);

تعريفنوع بنية s_xmparam {
	xpparam_t xpp;
	صحيح marker_سائيز;
	صحيح level;
	صحيح favor;
	صحيح style;
	ثابت حرف *ancestor;	/* label for orig */
	ثابت حرف *file1;	/* label for mf1 */
	ثابت حرف *file2;	/* label for mf2 */
} xmparam_t;

#وصف_ڪريو DEFAULT_CONFLICT_MARKER_سائيز 7

صحيح xdl_ملايو(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ثابت *xmp, mmbuffer_t *نتيجو);

#جيڪڏهن_وصف_ٿيل __cplusplus
}
#جيڪڏهن_پورو /* #ifdef __cplusplus */

#جيڪڏهن_پورو /* #if !defined(XDIFF_H) */
