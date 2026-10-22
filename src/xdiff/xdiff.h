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
#تعریف_کن XDIFF_H

#اگر_تعریف_شده __cplusplus
خارجي "C" {
#پایان_اگر /* #ifdef __cplusplus */

/* xpparm_t.flags */
#تعریف_کن XDF_NEED_کمینه (1 << 0)

#تعریف_کن XDF_IGNORE_WHITESPACE (1 << 1)
#تعریف_کن XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#تعریف_کن XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#تعریف_کن XDF_IGNORE_CR_AT_EOL (1 << 4)
#تعریف_کن XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#تعریف_کن XDF_IGNORE_BLANK_سطرها (1 << 7)

#تعریف_کن XDF_PATIENCE_DIFF (1 << 14)
#تعریف_کن XDF_HISTOGRAM_DIFF (1 << 15)
#تعریف_کن XDF_DIFF_ALGORITHM_ماسک (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#تعریف_کن XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_ماسک)

#تعریف_کن XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#تعریف_کن XDL_EMIT_FUNCNAMES (1 << 0)
#تعریف_کن XDL_EMIT_NO_HUNK_HDR (1 << 1)
#تعریف_کن XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#تعریف_کن XDL_ادغام_کمینه 0
#تعریف_کن XDL_ادغام_EAGER 1
#تعریف_کن XDL_ادغام_ZEALOUS 2
#تعریف_کن XDL_ادغام_ZEALOUS_ALNUM 3

/* merge favor modes */
#تعریف_کن XDL_ادغام_FAVOR_OURS 1
#تعریف_کن XDL_ادغام_FAVOR_THEIRS 2
#تعریف_کن XDL_ادغام_FAVOR_UNION 3

/* merge output styles */
#تعریف_کن XDL_ادغام_DIFF3 1
#تعریف_کن XDL_ادغام_ZEALOUS_DIFF3 2

تعريفنوع بنية s_mmfile {
	حرف *ptr;
	دراز اندازه;
} mmfile_t;

تعريفنوع بنية s_mmbuffer {
	حرف *ptr;
	دراز اندازه;
} mmbuffer_t;

تعريفنوع بنية s_xpparam {
	بیعلامت دراز flags;

	/* -I<regex> */
 #إذا 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#پایان_اگر

	/* See Documentation/diff-options.adoc. */
	حرف **anchors;
	size_t anchors_nr;
} xpparam_t;

تعريفنوع بنية s_xdemitcb {
	فراغ *priv;
	صحيح (*بیرون_hunk)(فراغ *,
			دراز old_begin, دراز old_nr,
			دراز new_begin, دراز new_nr,
			ثابت حرف *func, دراز funclen);
	صحيح (*بیرون_سطر)(فراغ *, mmbuffer_t *, صحيح);
} xdemitcb_t;

تعريفنوع دراز (*پیدا_کن_func_t)(ثابت حرف *سطر, دراز سطر_len, حرف *میانگیر, دراز میانگیر_اندازه_3, فراغ *priv);

تعريفنوع صحيح (*xdl_emit_hunk_consume_func_t)(دراز شروع_a, دراز شمار_a,
					    دراز شروع_b, دراز شمار_b,
					    فراغ *cb_داده_2);

تعريفنوع بنية s_xdemitconf {
	دراز ctxlen;
	دراز interhunkctxlen;
	بیعلامت دراز flags;
	پیدا_کن_func_t پیدا_کن_func;
	فراغ *پیدا_کن_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

تعريفنوع بنية s_bdiffparam {
	دراز bsize;
} bdiffparam_t;

#اگر_تعریف_شده VMS
# include "[]vim.h"
#وإلا
# include "../vim.h"
#پایان_اگر

#تعریف_کن xdl_malloc(x) lalloc((x), TRUE)
#تعریف_کن xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#تعریف_کن xdl_free(ptr) vim_free(ptr)
#تعریف_کن xdl_realloc(ptr,x) vim_realloc((ptr),(x))

فراغ *xdl_mmfile_اولی(mmfile_t *mmf, دراز *اندازه);
دراز xdl_mmfile_اندازه(mmfile_t *mmf);

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

#تعریف_کن DEFAULT_CONFLICT_MARKER_اندازه 7

صحيح xdl_ادغام(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ثابت *xmp, mmbuffer_t *نتیجه);

#اگر_تعریف_شده __cplusplus
}
#پایان_اگر /* #ifdef __cplusplus */

#پایان_اگر /* #if !defined(XDIFF_H) */
