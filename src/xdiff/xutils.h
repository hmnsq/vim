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

#اگر !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
صحیح xdl_emit_diffrec(نویسه ثابت *rec, long size, نویسه ثابت *pre, long psize,
		     xdemitcb_t *ecb);
صحیح xdl_cha_init(chastore_t *cha, long isize, long icount);
تهی xdl_cha_free(chastore_t *cha);
تهی *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_سطرها(mmfile_t *mf, long sample);
صحیح xdl_blankline(ثابت نویسه *سطر, long size, long flags);
صحیح xdl_recmatch(ثابت نویسه *l1, long s1, ثابت نویسه *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(نویسه ثابت **data, نویسه ثابت *بالاترین);
unsigned long xdl_hash_record_with_whitespace(نویسه ثابت **data, نویسه ثابت *بالاترین, long flags);
ایستا درونخطی unsigned long xdl_hash_record(نویسه ثابت **data, نویسه ثابت *بالاترین, long flags)
{
	اگر (flags & XDF_WHITESPACE_FLAGS)
		بازگشت xdl_hash_record_with_whitespace(data, بالاترین, flags);
	وگرنه
		بازگشت xdl_hash_record_verbatim(data, بالاترین);
}
unsigned صحیح xdl_hashbits(unsigned صحیح size);
صحیح xdl_رقم_out(نویسه *out, long val);
صحیح xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ثابت نویسه *func, long funclen, xdemitcb_t *ecb);
صحیح xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحیح line1, صحیح count1, صحیح line2, صحیح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
تهی* xdl_alloc_grow_helper(تهی* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
