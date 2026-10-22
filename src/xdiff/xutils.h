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

#إذا !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
صحيح xdl_emit_diffrec(حرف ثابت *rec, long size, حرف ثابت *pre, long psize,
		     xdemitcb_t *ecb);
صحيح xdl_cha_init(chastore_t *cha, long isize, long icount);
فراغ xdl_cha_free(chastore_t *cha);
فراغ *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_أسطر(mmfile_t *mf, long sample);
صحيح xdl_blankline(ثابت حرف *سطر, long size, long flags);
صحيح xdl_recmatch(ثابت حرف *l1, long s1, ثابت حرف *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(حرف ثابت **data, حرف ثابت *قمة);
unsigned long xdl_hash_record_with_whitespace(حرف ثابت **data, حرف ثابت *قمة, long flags);
ساكن مضمن unsigned long xdl_hash_record(حرف ثابت **data, حرف ثابت *قمة, long flags)
{
	إذا (flags & XDF_WHITESPACE_FLAGS)
		إرجاع xdl_hash_record_with_whitespace(data, قمة, flags);
	وإلا
		إرجاع xdl_hash_record_verbatim(data, قمة);
}
unsigned صحيح xdl_hashbits(unsigned صحيح size);
صحيح xdl_رقم_out(حرف *out, long val);
صحيح xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ثابت حرف *func, long funclen, xdemitcb_t *ecb);
صحيح xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحيح line1, صحيح count1, صحيح line2, صحيح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
فراغ* xdl_alloc_grow_helper(فراغ* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
