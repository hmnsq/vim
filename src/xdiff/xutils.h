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

#ಯದಿ !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
ಪೂರ್ಣಾಙ್ಕ xdl_emit_diffrec(ವರ್ಣ ಧ್ರುವ *rec, long size, ವರ್ಣ ಧ್ರುವ *pre, long psize,
		     xdemitcb_t *ecb);
ಪೂರ್ಣಾಙ್ಕ xdl_cha_init(chastore_t *cha, long isize, long icount);
ಶೂನ್ಯ xdl_cha_free(chastore_t *cha);
ಶೂನ್ಯ *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_ಸಾಲುಗಳು(mmfile_t *mf, long sample);
ಪೂರ್ಣಾಙ್ಕ xdl_blankline(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, long size, long flags);
ಪೂರ್ಣಾಙ್ಕ xdl_recmatch(ಧ್ರುವ ವರ್ಣ *l1, long s1, ಧ್ರುವ ವರ್ಣ *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(ವರ್ಣ ಧ್ರುವ **data, ವರ್ಣ ಧ್ರುವ *ತುದಿ);
unsigned long xdl_hash_record_with_whitespace(ವರ್ಣ ಧ್ರುವ **data, ವರ್ಣ ಧ್ರುವ *ತುದಿ, long flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned long xdl_hash_record(ವರ್ಣ ಧ್ರುವ **data, ವರ್ಣ ಧ್ರುವ *ತುದಿ, long flags)
{
	ಯದಿ (flags & XDF_WHITESPACE_FLAGS)
		ಪ್ರತ್ಯಾಗಮ xdl_hash_record_with_whitespace(data, ತುದಿ, flags);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ xdl_hash_record_verbatim(data, ತುದಿ);
}
unsigned ಪೂರ್ಣಾಙ್ಕ xdl_hashbits(unsigned ಪೂರ್ಣಾಙ್ಕ size);
ಪೂರ್ಣಾಙ್ಕ xdl_ಅಂಕೆ_out(ವರ್ಣ *out, long val);
ಪೂರ್ಣಾಙ್ಕ xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ಧ್ರುವ ವರ್ಣ *func, long funclen, xdemitcb_t *ecb);
ಪೂರ್ಣಾಙ್ಕ xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ಧ್ರುವ *xpp,
		       ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
ಶೂನ್ಯ* xdl_alloc_grow_helper(ಶೂನ್ಯ* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
