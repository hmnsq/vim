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

#ଯଦି !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_diffrec(ଅକ୍ଷର ଅଚଳ *rec, long size, ଅକ୍ଷର ଅଚଳ *pre, long psize,
		     xdemitcb_t *ecb);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_cha_init(chastore_t *cha, long isize, long icount);
ଶୂନ୍ୟ xdl_cha_free(chastore_t *cha);
ଶୂନ୍ୟ *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_ଧାଡ଼ିଗୁଡ଼ିକ(mmfile_t *mf, long sample);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_blankline(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, long size, long flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_recmatch(ଅଚଳ ଅକ୍ଷର *l1, long s1, ଅଚଳ ଅକ୍ଷର *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(ଅକ୍ଷର ଅଚଳ **data, ଅକ୍ଷର ଅଚଳ *ଶୀର୍ଷ);
unsigned long xdl_hash_record_with_whitespace(ଅକ୍ଷର ଅଚଳ **data, ଅକ୍ଷର ଅଚଳ *ଶୀର୍ଷ, long flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned long xdl_hash_record(ଅକ୍ଷର ଅଚଳ **data, ଅକ୍ଷର ଅଚଳ *ଶୀର୍ଷ, long flags)
{
	ଯଦି (flags & XDF_WHITESPACE_FLAGS)
		ଫେରନ୍ତୁ xdl_hash_record_with_whitespace(data, ଶୀର୍ଷ, flags);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ xdl_hash_record_verbatim(data, ଶୀର୍ଷ);
}
unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_hashbits(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_ଅଙ୍କ_out(ଅକ୍ଷର *out, long val);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ଅଚଳ ଅକ୍ଷର *func, long funclen, xdemitcb_t *ecb);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ଅଚଳ *xpp,
		       ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
ଶୂନ୍ୟ* xdl_alloc_grow_helper(ଶୂନ୍ୟ* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
