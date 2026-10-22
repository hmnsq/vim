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

#જો !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
પૂર્ણાંક xdl_emit_diffrec(અક્ષર અચલ *rec, long size, અક્ષર અચલ *pre, long psize,
		     xdemitcb_t *ecb);
પૂર્ણાંક xdl_cha_init(chastore_t *cha, long isize, long icount);
શૂન્ય xdl_cha_free(chastore_t *cha);
શૂન્ય *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_લીટીઓ(mmfile_t *mf, long sample);
પૂર્ણાંક xdl_blankline(અચલ અક્ષર *લીટી, long size, long flags);
પૂર્ણાંક xdl_recmatch(અચલ અક્ષર *l1, long s1, અચલ અક્ષર *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(અક્ષર અચલ **data, અક્ષર અચલ *ટોચ);
unsigned long xdl_hash_record_with_whitespace(અક્ષર અચલ **data, અક્ષર અચલ *ટોચ, long flags);
સ્થિર અંતઃરેખ unsigned long xdl_hash_record(અક્ષર અચલ **data, અક્ષર અચલ *ટોચ, long flags)
{
	જો (flags & XDF_WHITESPACE_FLAGS)
		પાછા xdl_hash_record_with_whitespace(data, ટોચ, flags);
	નહીંતર
		પાછા xdl_hash_record_verbatim(data, ટોચ);
}
unsigned પૂર્ણાંક xdl_hashbits(unsigned પૂર્ણાંક size);
પૂર્ણાંક xdl_અંક_out(અક્ષર *out, long val);
પૂર્ણાંક xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      અચલ અક્ષર *func, long funclen, xdemitcb_t *ecb);
પૂર્ણાંક xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t અચલ *xpp,
		       પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
શૂન્ય* xdl_alloc_grow_helper(શૂન્ય* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
