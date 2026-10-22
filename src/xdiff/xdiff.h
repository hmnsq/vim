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

#ఒకవేళ !defined(XDIFF_H)
#నిర్వచించు XDIFF_H

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు /* #ifdef __cplusplus */

/* xpparm_t.flags */
#నిర్వచించు XDF_NEED_కనీస (1 << 0)

#నిర్వచించు XDF_IGNORE_WHITESPACE (1 << 1)
#నిర్వచించు XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#నిర్వచించు XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#నిర్వచించు XDF_IGNORE_CR_AT_EOL (1 << 4)
#నిర్వచించు XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#నిర్వచించు XDF_IGNORE_BLANK_పంక్తులు (1 << 7)

#నిర్వచించు XDF_PATIENCE_DIFF (1 << 14)
#నిర్వచించు XDF_HISTOGRAM_DIFF (1 << 15)
#నిర్వచించు XDF_DIFF_ALGORITHM_ముసుగు (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#నిర్వచించు XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_ముసుగు)

#నిర్వచించు XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#నిర్వచించు XDL_EMIT_FUNCNAMES (1 << 0)
#నిర్వచించు XDL_EMIT_NO_HUNK_HDR (1 << 1)
#నిర్వచించు XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#నిర్వచించు XDL_విలీనం_కనీస 0
#నిర్వచించు XDL_విలీనం_EAGER 1
#నిర్వచించు XDL_విలీనం_ZEALOUS 2
#నిర్వచించు XDL_విలీనం_ZEALOUS_ALNUM 3

/* merge favor modes */
#నిర్వచించు XDL_విలీనం_FAVOR_OURS 1
#నిర్వచించు XDL_విలీనం_FAVOR_THEIRS 2
#నిర్వచించు XDL_విలీనం_FAVOR_UNION 3

/* merge output styles */
#నిర్వచించు XDL_విలీనం_DIFF3 1
#నిర్వచించు XDL_విలీనం_ZEALOUS_DIFF3 2

రకనిర్వచనం నిర్మాణం s_mmfile {
	అక్షరం *ptr;
	దీర్ఘ కొలత;
} mmfile_t;

రకనిర్వచనం నిర్మాణం s_mmbuffer {
	అక్షరం *ptr;
	దీర్ఘ కొలత;
} mmbuffer_t;

రకనిర్వచనం నిర్మాణం s_xpparam {
	గుర్తులేని దీర్ఘ flags;

	/* -I<regex> */
 #ఒకవేళ 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#అయితే_ముగింపు

	/* See Documentation/diff-options.adoc. */
	అక్షరం **anchors;
	size_t anchors_nr;
} xpparam_t;

రకనిర్వచనం నిర్మాణం s_xdemitcb {
	శూన్య *priv;
	పూర్ణాంకం (*బహిర్గతం_hunk)(శూన్య *,
			దీర్ఘ old_begin, దీర్ఘ old_nr,
			దీర్ఘ new_begin, దీర్ఘ new_nr,
			స్థిరరాశి అక్షరం *func, దీర్ఘ funclen);
	పూర్ణాంకం (*బహిర్గతం_పంక్తి)(శూన్య *, mmbuffer_t *, పూర్ణాంకం);
} xdemitcb_t;

రకనిర్వచనం దీర్ఘ (*కనుగొను_func_t)(స్థిరరాశి అక్షరం *పంక్తి, దీర్ఘ పంక్తి_len, అక్షరం *బఫరు, దీర్ఘ బఫరు_కొలత_3, శూన్య *priv);

రకనిర్వచనం పూర్ణాంకం (*xdl_emit_hunk_consume_func_t)(దీర్ఘ ప్రారంభం_a, దీర్ఘ లెక్క_a,
					    దీర్ఘ ప్రారంభం_b, దీర్ఘ లెక్క_b,
					    శూన్య *cb_దత్తాంశం_2);

రకనిర్వచనం నిర్మాణం s_xdemitconf {
	దీర్ఘ ctxlen;
	దీర్ఘ interhunkctxlen;
	గుర్తులేని దీర్ఘ flags;
	కనుగొను_func_t కనుగొను_func;
	శూన్య *కనుగొను_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

రకనిర్వచనం నిర్మాణం s_bdiffparam {
	దీర్ఘ bsize;
} bdiffparam_t;

#నిర్వచితమైతే VMS
# include "[]vim.h"
#లేకపోతే
# include "../vim.h"
#అయితే_ముగింపు

#నిర్వచించు xdl_malloc(x) lalloc((x), TRUE)
#నిర్వచించు xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#నిర్వచించు xdl_free(ptr) vim_free(ptr)
#నిర్వచించు xdl_realloc(ptr,x) vim_realloc((ptr),(x))

శూన్య *xdl_mmfile_మొదటి(mmfile_t *mmf, దీర్ఘ *కొలత);
దీర్ఘ xdl_mmfile_కొలత(mmfile_t *mmf);

పూర్ణాంకం xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t స్థిరరాశి *xpp,
	     xdemitconf_t స్థిరరాశి *xecfg, xdemitcb_t *ecb);

రకనిర్వచనం నిర్మాణం s_xmparam {
	xpparam_t xpp;
	పూర్ణాంకం marker_కొలత;
	పూర్ణాంకం level;
	పూర్ణాంకం favor;
	పూర్ణాంకం style;
	స్థిరరాశి అక్షరం *ancestor;	/* label for orig */
	స్థిరరాశి అక్షరం *file1;	/* label for mf1 */
	స్థిరరాశి అక్షరం *file2;	/* label for mf2 */
} xmparam_t;

#నిర్వచించు DEFAULT_CONFLICT_MARKER_కొలత 7

పూర్ణాంకం xdl_విలీనం(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t స్థిరరాశి *xmp, mmbuffer_t *ఫలితం);

#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు /* #ifdef __cplusplus */

#అయితే_ముగింపు /* #if !defined(XDIFF_H) */
