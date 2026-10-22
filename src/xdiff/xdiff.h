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

#ಆದರೆ !defined(XDIFF_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XDIFF_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ /* #ifdef __cplusplus */

/* xpparm_t.flags */
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_NEED_ಕನಿಷ್ಠತಮ (1 << 0)

#ವ್ಯಾಖ್ಯಾನಿಸು XDF_IGNORE_WHITESPACE (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_IGNORE_WHITESPACE_CHANGE (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_IGNORE_WHITESPACE_AT_EOL (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_IGNORE_CR_AT_EOL (1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_WHITESPACE_FLAGS (XDF_IGNORE_WHITESPACE | \
			      XDF_IGNORE_WHITESPACE_CHANGE | \
			      XDF_IGNORE_WHITESPACE_AT_EOL | \
			      XDF_IGNORE_CR_AT_EOL)

#ವ್ಯಾಖ್ಯಾನಿಸು XDF_IGNORE_BLANK_ಸಾಲುಗಳು (1 << 7)

#ವ್ಯಾಖ್ಯಾನಿಸು XDF_PATIENCE_DIFF (1 << 14)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_HISTOGRAM_DIFF (1 << 15)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_DIFF_ALGORITHM_ಮುಖವಾಡ (XDF_PATIENCE_DIFF | XDF_HISTOGRAM_DIFF)
#ವ್ಯಾಖ್ಯಾನಿಸು XDF_DIFF_ALG(x) ((x) & XDF_DIFF_ALGORITHM_ಮುಖವಾಡ)

#ವ್ಯಾಖ್ಯಾನಿಸು XDF_INDENT_HEURISTIC (1 << 23)

/* xdemitconf_t.flags */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_EMIT_FUNCNAMES (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_EMIT_NO_HUNK_HDR (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_EMIT_FUNCCONTEXT (1 << 2)

/* merge simplification levels */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_ಕನಿಷ್ಠತಮ 0
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_EAGER 1
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_ZEALOUS 2
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_ZEALOUS_ALNUM 3

/* merge favor modes */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_FAVOR_OURS 1
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_FAVOR_THEIRS 2
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_FAVOR_UNION 3

/* merge output styles */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_DIFF3 1
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ವಿಲೀನ_ZEALOUS_DIFF3 2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_mmfile {
	ಅಕ್ಷರ *ptr;
	ದೀರ್ಘ ಗಾತ್ರ_2;
} mmfile_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_mmbuffer {
	ಅಕ್ಷರ *ptr;
	ದೀರ್ಘ ಗಾತ್ರ_2;
} mmbuffer_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xpparam {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags;

	/* -I<regex> */
 #ಆದರೆ 0  // unused by Vim
	regex_t **ignore_regex;
	size_t ignore_regex_nr;
#ಆದರೆ_ಅಂತ್ಯ

	/* See Documentation/diff-options.adoc. */
	ಅಕ್ಷರ **anchors;
	size_t anchors_nr;
} xpparam_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdemitcb {
	ಶೂನ್ಯ *priv;
	ಪೂರ್ಣಾಂಕ (*ಹೊರಹೋಗು_hunk)(ಶೂನ್ಯ *,
			ದೀರ್ಘ old_begin, ದೀರ್ಘ old_nr,
			ದೀರ್ಘ new_begin, ದೀರ್ಘ new_nr,
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func, ದೀರ್ಘ funclen);
	ಪೂರ್ಣಾಂಕ (*ಹೊರಹೋಗು_ಸಾಲು)(ಶೂನ್ಯ *, mmbuffer_t *, ಪೂರ್ಣಾಂಕ);
} xdemitcb_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ (*ಕಂಡುಹಿಡಿ_func_t)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ದೀರ್ಘ ಸಾಲು_len, ಅಕ್ಷರ *ಬಫರ್, ದೀರ್ಘ ಬಫರ್_ಗಾತ್ರ_3, ಶೂನ್ಯ *priv);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*xdl_emit_hunk_consume_func_t)(ದೀರ್ಘ ಆರಂಭ_a, ದೀರ್ಘ ಎಣಿಕೆ_a,
					    ದೀರ್ಘ ಆರಂಭ_b, ದೀರ್ಘ ಎಣಿಕೆ_b,
					    ಶೂನ್ಯ *cb_ದತ್ತಾಂಶ_2);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdemitconf {
	ದೀರ್ಘ ctxlen;
	ದೀರ್ಘ interhunkctxlen;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags;
	ಕಂಡುಹಿಡಿ_func_t ಕಂಡುಹಿಡಿ_func;
	ಶೂನ್ಯ *ಕಂಡುಹಿಡಿ_func_priv;
	xdl_emit_hunk_consume_func_t hunk_func;
} xdemitconf_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_bdiffparam {
	ದೀರ್ಘ bsize;
} bdiffparam_t;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include "[]vim.h"
#ಇಲ್ಲದಿದ್ದರೆ
# include "../vim.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು xdl_malloc(x) lalloc((x), TRUE)
#ವ್ಯಾಖ್ಯಾನಿಸು xdl_calloc(n, sz) lalloc_clear(n*sz, TRUE)
#ವ್ಯಾಖ್ಯಾನಿಸು xdl_free(ptr) vim_free(ptr)
#ವ್ಯಾಖ್ಯಾನಿಸು xdl_realloc(ptr,x) vim_realloc((ptr),(x))

ಶೂನ್ಯ *xdl_mmfile_ಮೊದಲ(mmfile_t *mmf, ದೀರ್ಘ *ಗಾತ್ರ_2);
ದೀರ್ಘ xdl_mmfile_ಗಾತ್ರ(mmfile_t *mmf);

ಪೂರ್ಣಾಂಕ xdl_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
	     xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg, xdemitcb_t *ecb);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xmparam {
	xpparam_t xpp;
	ಪೂರ್ಣಾಂಕ marker_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ level;
	ಪೂರ್ಣಾಂಕ favor;
	ಪೂರ್ಣಾಂಕ style;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ancestor;	/* label for orig */
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file1;	/* label for mf1 */
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file2;	/* label for mf2 */
} xmparam_t;

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_CONFLICT_MARKER_ಗಾತ್ರ 7

ಪೂರ್ಣಾಂಕ xdl_ವಿಲೀನ(mmfile_t *orig, mmfile_t *mf1, mmfile_t *mf2,
		xmparam_t ಸ್ಥಿರಾಂಕ *xmp, mmbuffer_t *ಫಲಿತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ /* #ifdef __cplusplus */

#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XDIFF_H) */
