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

#ಆದರೆ !defined(XUTILS_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XUTILS_H



ದೀರ್ಘ xdl_bogosqrt(ದೀರ್ಘ n);
ಪೂರ್ಣಾಂಕ xdl_emit_diffrec(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *rec, ದೀರ್ಘ ಗಾತ್ರ_2, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *pre, ದೀರ್ಘ psize,
		     xdemitcb_t *ecb);
ಪೂರ್ಣಾಂಕ xdl_cha_init(chastore_t *cha, ದೀರ್ಘ isize, ದೀರ್ಘ icount);
ಶೂನ್ಯ xdl_cha_free(chastore_t *cha);
ಶೂನ್ಯ *xdl_cha_alloc(chastore_t *cha);
ದೀರ್ಘ xdl_guess_ಸಾಲುಗಳು(mmfile_t *mf, ದೀರ್ಘ sample);
ಪೂರ್ಣಾಂಕ xdl_blankline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ದೀರ್ಘ ಗಾತ್ರ_2, ದೀರ್ಘ flags);
ಪೂರ್ಣಾಂಕ xdl_recmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l1, ದೀರ್ಘ s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l2, ದೀರ್ಘ s2, ದೀರ್ಘ flags);
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ xdl_hash_ದಾಖಲೆ_verbatim(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **ದತ್ತಾಂಶ, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ತುದಿ);
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ xdl_hash_ದಾಖಲೆ_with_whitespace(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **ದತ್ತಾಂಶ, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ತುದಿ, ದೀರ್ಘ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ xdl_hash_ದಾಖಲೆ(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **ದತ್ತಾಂಶ, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ತುದಿ, ದೀರ್ಘ flags)
{
	ಆದರೆ (flags & XDF_WHITESPACE_FLAGS)
		ಹಿಂತಿರುಗಿ xdl_hash_ದಾಖಲೆ_with_whitespace(ದತ್ತಾಂಶ, ತುದಿ, flags);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ xdl_hash_ದಾಖಲೆ_verbatim(ದತ್ತಾಂಶ, ತುದಿ);
}
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ xdl_hashbits(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2);
ಪೂರ್ಣಾಂಕ xdl_ಅಂಕೆ_ಹೊರಹೋಗು(ಅಕ್ಷರ *ಹೊರಹೋಗು, ದೀರ್ಘ val);
ಪೂರ್ಣಾಂಕ xdl_emit_hunk_hdr(ದೀರ್ಘ s1, ದೀರ್ಘ c1, ದೀರ್ಘ s2, ದೀರ್ಘ c2,
		      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func, ದೀರ್ಘ funclen, xdemitcb_t *ecb);
ಪೂರ್ಣಾಂಕ xdl_fall_ಹಿಂಭಾಗ_diff(xdfenv_t *diff_env, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
		       ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
ಶೂನ್ಯ* xdl_alloc_grow_helper(ಶೂನ್ಯ* p, ದೀರ್ಘ nr, ದೀರ್ಘ* alloc, size_t ಗಾತ್ರ_2);

#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XUTILS_H) */
