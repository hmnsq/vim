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
পূর্ণাঙ্ক xdl_emit_diffrec(বর্ণ ধ্রুব *rec, long size, বর্ণ ধ্রুব *pre, long psize,
		     xdemitcb_t *ecb);
পূর্ণাঙ্ক xdl_cha_init(chastore_t *cha, long isize, long icount);
শূন্য xdl_cha_free(chastore_t *cha);
শূন্য *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_সারিগুলি(mmfile_t *mf, long sample);
পূর্ণাঙ্ক xdl_blankline(ধ্রুব বর্ণ *সারি, long size, long flags);
পূর্ণাঙ্ক xdl_recmatch(ধ্রুব বর্ণ *l1, long s1, ধ্রুব বর্ণ *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(বর্ণ ধ্রুব **data, বর্ণ ধ্রুব *শীর্ষ);
unsigned long xdl_hash_record_with_whitespace(বর্ণ ধ্রুব **data, বর্ণ ধ্রুব *শীর্ষ, long flags);
স্থির অন্তঃস্থ unsigned long xdl_hash_record(বর্ণ ধ্রুব **data, বর্ণ ধ্রুব *শীর্ষ, long flags)
{
	যদি (flags & XDF_WHITESPACE_FLAGS)
		প্রত্যাগম xdl_hash_record_with_whitespace(data, শীর্ষ, flags);
	অন্যথা
		প্রত্যাগম xdl_hash_record_verbatim(data, শীর্ষ);
}
unsigned পূর্ণাঙ্ক xdl_hashbits(unsigned পূর্ণাঙ্ক size);
পূর্ণাঙ্ক xdl_অঙ্ক_out(বর্ণ *out, long val);
পূর্ণাঙ্ক xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ধ্রুব বর্ণ *func, long funclen, xdemitcb_t *ecb);
পূর্ণাঙ্ক xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ধ্রুব *xpp,
		       পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
শূন্য* xdl_alloc_grow_helper(শূন্য* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
