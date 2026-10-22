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

#اگر !defined(XDIFF_H)
#تعریف_کرو XDIFF_H

#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم /* #ifdef __cplusplus */

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

قسمتعریف ساخت s_mmfile {
	حرف *ptr;
	طویل سائز;
} mmfile_t;

قسمتعریف ساخت s_mmbuffer {
	حرف *ptr;
	طویل سائز;
} mmbuffer_t;

قسمتعریف ساخت s_xpparam {
	غیرعلامتی طویل flags;

	/* -I<regex> */
 #اگر 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#اگر_ختم

	/* See Documentation/diff-options.adoc. */
	حرف **anchors;
	size_t anchors_nr;
} xpparam_t;

قسمتعریف ساخت s_xdemitcb {
	خالی *priv;
	صحیح (*برآمد_hunk)(خالی *,
			طویل old_begin, طویل old_nr,
			طویل new_begin, طویل new_nr,
			ثابت حرف *func, طویل funclen);
	صحیح (*برآمد_سطر)(خالی *, mmbuffer_t *, صحیح);
} xdemitcb_t;

قسمتعریف طویل (*ڈھونڈو_func_t)(ثابت حرف *سطر, طویل سطر_len, حرف *بفر, طویل بفر_سائز_3, خالی *priv);

قسمتعریف صحیح (*xdl_emit_hunk_consume_func_t)(طویل آغاز_a, طویل گنتی_a,
					    طویل آغاز_b, طویل گنتی_b,
					    خالی *cb_ڈیٹا_2);

قسمتعریف ساخت s_xdemitconf {
	طویل ctxlen;
	طویل interhunkctxlen;
	غیرعلامتی طویل flags;
	ڈھونڈو_func_t ڈھونڈو_func;
	خالی *ڈھونڈو_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

قسمتعریف ساخت s_bdiffparam {
	طویل bsize;
} bdiffparam_t;

#اگر_تعریف_شدہ VMS
# include "[]vim.h"
#ورنہ
# include "../vim.h"
#اگر_ختم

#تعریف_کرو xdl_malloc(x) lalloc((x), TRUE)
#تعریف_کرو xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#تعریف_کرو xdl_free(ptr) vim_free(ptr)
#تعریف_کرو xdl_realloc(ptr,x) vim_realloc((ptr),(x))

خالی *xdl_mmfile_پہلا(mmfile_t *mmf, طویل *سائز);
طویل xdl_mmfile_سائز(mmfile_t *mmf);

صحیح xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
	     xdemitconf_t ثابت *xecfg, xdemitcb_t *ecb);

قسمتعریف ساخت s_xmparam {
	xpparam_t xpp;
	صحیح marker_سائز;
	صحیح level;
	صحیح favor;
	صحیح style;
	ثابت حرف *ancestor;	/* label for orig */
	ثابت حرف *file1;	/* label for mf1 */
	ثابت حرف *file2;	/* label for mf2 */
} xmparam_t;

#تعریف_کرو DEFAULT_CONFLICT_MARKER_سائز 7

صحیح xdl_ملاؤ(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ثابت *xmp, mmbuffer_t *نتیجہ);

#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم /* #ifdef __cplusplus */

#اگر_ختم /* #if !defined(XDIFF_H) */
