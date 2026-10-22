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

#ਜੇ !defined(XDIFF_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDIFF_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ /* #ifdef __cplusplus */

/* xpparm_t.flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_NEED_ਨਿਊਨਤਮ (1 << 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_IGNORE_WHITESPACE (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_IGNORE_CR_AT_EOL (1 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_IGNORE_BLANK_ਸਤਰਾਂ (1 << 7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_PATIENCE_DIFF (1 << 14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_HISTOGRAM_DIFF (1 << 15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_DIFF_ALGORITHM_ਨਕਾਬ (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_ਨਕਾਬ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_EMIT_FUNCNAMES (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_EMIT_NO_HUNK_HDR (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_ਨਿਊਨਤਮ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_EAGER 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_ZEALOUS 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_ZEALOUS_ALNUM 3

/* merge favor modes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_FAVOR_OURS 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_FAVOR_THEIRS 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_FAVOR_UNION 3

/* merge output styles */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_DIFF3 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ਮਿਲਾਓ_ZEALOUS_DIFF3 2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_mmfile {
	ਅੱਖਰ *ptr;
	ਲੰਮਾ ਮਾਪ;
} mmfile_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_mmbuffer {
	ਅੱਖਰ *ptr;
	ਲੰਮਾ ਮਾਪ;
} mmbuffer_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xpparam {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags;

	/* -I<regex> */
 #ਜੇ 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#ਜੇ_ਅੰਤ

	/* See Documentation/diff-options.adoc. */
	ਅੱਖਰ **anchors;
	size_t anchors_nr;
} xpparam_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdemitcb {
	ਖਾਲੀ *priv;
	ਪੂਰਨ (*ਬਾਹਰਲਾ_hunk)(ਖਾਲੀ *,
			ਲੰਮਾ old_begin, ਲੰਮਾ old_nr,
			ਲੰਮਾ new_begin, ਲੰਮਾ new_nr,
			ਅਚਲ ਅੱਖਰ *func, ਲੰਮਾ funclen);
	ਪੂਰਨ (*ਬਾਹਰਲਾ_ਸਤਰ)(ਖਾਲੀ *, mmbuffer_t *, ਪੂਰਨ);
} xdemitcb_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ (*ਲੱਭੋ_func_t)(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਲੰਮਾ ਸਤਰ_len, ਅੱਖਰ *ਬਫਰ, ਲੰਮਾ ਬਫਰ_ਮਾਪ_3, ਖਾਲੀ *priv);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*xdl_emit_hunk_consume_func_t)(ਲੰਮਾ ਸ਼ੁਰੂ_a, ਲੰਮਾ ਗਿਣਤੀ_a,
					    ਲੰਮਾ ਸ਼ੁਰੂ_b, ਲੰਮਾ ਗਿਣਤੀ_b,
					    ਖਾਲੀ *cb_ਅੰਕੜੇ_2);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdemitconf {
	ਲੰਮਾ ctxlen;
	ਲੰਮਾ interhunkctxlen;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags;
	ਲੱਭੋ_func_t ਲੱਭੋ_func;
	ਖਾਲੀ *ਲੱਭੋ_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_bdiffparam {
	ਲੰਮਾ bsize;
} bdiffparam_t;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# include "[]vim.h"
#ਨਹੀਂਤਾਂ
# include "../vim.h"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xdl_malloc(x) lalloc((x), TRUE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xdl_free(ptr) vim_free(ptr)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xdl_realloc(ptr,x) vim_realloc((ptr),(x))

ਖਾਲੀ *xdl_mmfile_ਪਹਿਲਾ(mmfile_t *mmf, ਲੰਮਾ *ਮਾਪ);
ਲੰਮਾ xdl_mmfile_ਮਾਪ(mmfile_t *mmf);

ਪੂਰਨ xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ਅਚਲ *xpp,
	     xdemitconf_t ਅਚਲ *xecfg, xdemitcb_t *ecb);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xmparam {
	xpparam_t xpp;
	ਪੂਰਨ marker_ਮਾਪ;
	ਪੂਰਨ level;
	ਪੂਰਨ favor;
	ਪੂਰਨ style;
	ਅਚਲ ਅੱਖਰ *ancestor;	/* label for orig */
	ਅਚਲ ਅੱਖਰ *file1;	/* label for mf1 */
	ਅਚਲ ਅੱਖਰ *file2;	/* label for mf2 */
} xmparam_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_CONFLICT_MARKER_ਮਾਪ 7

ਪੂਰਨ xdl_ਮਿਲਾਓ(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ਅਚਲ *xmp, mmbuffer_t *ਨਤੀਜਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ /* #ifdef __cplusplus */

#ਜੇ_ਅੰਤ /* #if !defined(XDIFF_H) */
