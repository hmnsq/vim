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

#எனில் !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
முழு xdl_emit_diffrec(எழுத்து மாறிலி *rec, long size, எழுத்து மாறிலி *pre, long psize,
		     xdemitcb_t *ecb);
முழு xdl_cha_init(chastore_t *cha, long isize, long icount);
வெறுமை xdl_cha_free(chastore_t *cha);
வெறுமை *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_வரிகள்(mmfile_t *mf, long sample);
முழு xdl_blankline(மாறிலி எழுத்து *வரி, long size, long flags);
முழு xdl_recmatch(மாறிலி எழுத்து *l1, long s1, மாறிலி எழுத்து *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(எழுத்து மாறிலி **data, எழுத்து மாறிலி *உச்சி);
unsigned long xdl_hash_record_with_whitespace(எழுத்து மாறிலி **data, எழுத்து மாறிலி *உச்சி, long flags);
நிலையான உள்வரி unsigned long xdl_hash_record(எழுத்து மாறிலி **data, எழுத்து மாறிலி *உச்சி, long flags)
{
	எனில் (flags & XDF_WHITESPACE_FLAGS)
		திரும்பு xdl_hash_record_with_whitespace(data, உச்சி, flags);
	இல்லையெனில்
		திரும்பு xdl_hash_record_verbatim(data, உச்சி);
}
unsigned முழு xdl_hashbits(unsigned முழு size);
முழு xdl_இலக்கம்_out(எழுத்து *out, long val);
முழு xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      மாறிலி எழுத்து *func, long funclen, xdemitcb_t *ecb);
முழு xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t மாறிலி *xpp,
		       முழு line1, முழு count1, முழு line2, முழு count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
வெறுமை* xdl_alloc_grow_helper(வெறுமை* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
