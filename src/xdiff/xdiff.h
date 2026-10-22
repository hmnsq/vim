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
#تعریف_کرو XDIFF_H

#جے_تعریف_شدہ __cplusplus
خارجي "C" {
#جے_انت /* #ifdef __cplusplus */

/* xpparm_t.flags */
#تعریف_کرو XDF_NEED_اقل (1 << 0)

#تعریف_کرو XDF_IGNORE_WHITESPACE (1 << 1)
#تعریف_کرو XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#تعریف_کرو XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#تعریف_کرو XDF_IGNORE_CR_AT_EOL (1 << 4)
#تعریف_کرو XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#تعریف_کرو XDF_IGNORE_BLANK_سطریں (1 << 7)

#تعریف_کرو XDF_PATIENCE_DIFF (1 << 14)
#تعریف_کرو XDF_HISTOGRAM_DIFF (1 << 15)
#تعریف_کرو XDF_DIFF_ALGORITHM_نقاب (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#تعریف_کرو XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_نقاب)

#تعریف_کرو XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#تعریف_کرو XDL_EMIT_FUNCNAMES (1 << 0)
#تعریف_کرو XDL_EMIT_NO_HUNK_HDR (1 << 1)
#تعریف_کرو XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#تعریف_کرو XDL_ملاؤ_اقل 0
#تعریف_کرو XDL_ملاؤ_EAGER 1
#تعریف_کرو XDL_ملاؤ_ZEALOUS 2
#تعریف_کرو XDL_ملاؤ_ZEALOUS_ALNUM 3

/* merge favor modes */
#تعریف_کرو XDL_ملاؤ_FAVOR_OURS 1
#تعریف_کرو XDL_ملاؤ_FAVOR_THEIRS 2
#تعریف_کرو XDL_ملاؤ_FAVOR_UNION 3

/* merge output styles */
#تعریف_کرو XDL_ملاؤ_DIFF3 1
#تعریف_کرو XDL_ملاؤ_ZEALOUS_DIFF3 2

تعريفنوع بنية s_mmfile {
	حرف *ptr;
	لما ناپ;
} mmfile_t;

تعريفنوع بنية s_mmbuffer {
	حرف *ptr;
	لما ناپ;
} mmbuffer_t;

تعريفنوع بنية s_xpparam {
	بنانشانی لما flags;

	/* -I<regex> */
 #إذا 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#جے_انت

	/* See Documentation/diff-options.adoc. */
	حرف **anchors;
	size_t anchors_nr;
} xpparam_t;

تعريفنوع بنية s_xdemitcb {
	فراغ *priv;
	صحيح (*باہرلا_hunk)(فراغ *,
			لما old_begin, لما old_nr,
			لما new_begin, لما new_nr,
			ثابت حرف *func, لما funclen);
	صحيح (*باہرلا_سطر)(فراغ *, mmbuffer_t *, صحيح);
} xdemitcb_t;

تعريفنوع لما (*لبھو_func_t)(ثابت حرف *سطر, لما سطر_len, حرف *بفر, لما بفر_ناپ_3, فراغ *priv);

تعريفنوع صحيح (*xdl_emit_hunk_consume_func_t)(لما شروع_a, لما گنتی_a,
					    لما شروع_b, لما گنتی_b,
					    فراغ *cb_انکڑے_2);

تعريفنوع بنية s_xdemitconf {
	لما ctxlen;
	لما interhunkctxlen;
	بنانشانی لما flags;
	لبھو_func_t لبھو_func;
	فراغ *لبھو_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

تعريفنوع بنية s_bdiffparam {
	لما bsize;
} bdiffparam_t;

#جے_تعریف_شدہ VMS
# include "[]vim.h"
#وإلا
# include "../vim.h"
#جے_انت

#تعریف_کرو xdl_malloc(x) lalloc((x), TRUE)
#تعریف_کرو xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#تعریف_کرو xdl_free(ptr) vim_free(ptr)
#تعریف_کرو xdl_realloc(ptr,x) vim_realloc((ptr),(x))

فراغ *xdl_mmfile_پہلا(mmfile_t *mmf, لما *ناپ);
لما xdl_mmfile_ناپ(mmfile_t *mmf);

صحيح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb);

تعريفنوع بنية s_xmparam {
	xpparam_t xpp;
	صحيح marker_ناپ;
	صحيح level;
	صحيح favor;
	صحيح style;
	ثابت حرف *ancestor;	/* label for orig */
	ثابت حرف *file1;	/* label for mf1 */
	ثابت حرف *file2;	/* label for mf2 */
} xmparam_t;

#تعریف_کرو DEFAULT_CONFLICT_MARKER_ناپ 7

صحيح xdl_ملاؤ(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ثابت *xmp, mmbuffer_t *نتیجہ);

#جے_تعریف_شدہ __cplusplus
}
#جے_انت /* #ifdef __cplusplus */

#جے_انت /* #if !defined(XDIFF_H) */
