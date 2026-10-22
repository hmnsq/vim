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

#যদি !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
পূর্ণসংখ্যা xdl_emit_diffrec(অক্ষর ধ্রুবক *rec, long আকার_2, অক্ষর ধ্রুবক *pre, long psize,
		     xdemitcb_t *ecb);
পূর্ণসংখ্যা xdl_cha_init(chastore_t *cha, long isize, long icount);
শূন্য xdl_cha_free(chastore_t *cha);
শূন্য *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_সারিগুলি(mmfile_t *mf, long sample);
পূর্ণসংখ্যা xdl_blankline(ধ্রুবক অক্ষর *সারি, long আকার_2, long flags);
পূর্ণসংখ্যা xdl_recmatch(ধ্রুবক অক্ষর *l1, long s1, ধ্রুবক অক্ষর *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(অক্ষর ধ্রুবক **তথ্য, অক্ষর ধ্রুবক *শীর্ষ_2);
unsigned long xdl_hash_record_with_whitespace(অক্ষর ধ্রুবক **তথ্য, অক্ষর ধ্রুবক *শীর্ষ_2, long flags);
স্থির অন্তঃরেখ unsigned long xdl_hash_record(অক্ষর ধ্রুবক **তথ্য, অক্ষর ধ্রুবক *শীর্ষ_2, long flags)
{
	যদি (flags & XDF_WHITESPACE_FLAGS)
		ফেরত xdl_hash_record_with_whitespace(তথ্য, শীর্ষ_2, flags);
	অন্যথা
		ফেরত xdl_hash_record_verbatim(তথ্য, শীর্ষ_2);
}
unsigned পূর্ণসংখ্যা xdl_hashbits(unsigned পূর্ণসংখ্যা আকার_2);
পূর্ণসংখ্যা xdl_অঙ্ক_out(অক্ষর *out, long val);
পূর্ণসংখ্যা xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ধ্রুবক অক্ষর *func, long funclen, xdemitcb_t *ecb);
পূর্ণসংখ্যা xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ধ্রুবক *xpp,
		       পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
শূন্য* xdl_alloc_grow_helper(শূন্য* p, long nr, long* alloc, size_t আকার_2);

#endif /* #if !defined(XUTILS_H) */
