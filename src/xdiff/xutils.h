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

#ਜੇ !defined(XUTILS_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUTILS_H



ਲੰਮਾ xdl_bogosqrt(ਲੰਮਾ n);
ਪੂਰਨ xdl_emit_diffrec(ਅੱਖਰ ਅਚਲ *rec, ਲੰਮਾ ਮਾਪ, ਅੱਖਰ ਅਚਲ *pre, ਲੰਮਾ psize,
		     xdemitcb_t *ecb);
ਪੂਰਨ xdl_cha_init(chastore_t *cha, ਲੰਮਾ isize, ਲੰਮਾ icount);
ਖਾਲੀ xdl_cha_free(chastore_t *cha);
ਖਾਲੀ *xdl_cha_alloc(chastore_t *cha);
ਲੰਮਾ xdl_guess_ਸਤਰਾਂ(mmfile_t *mf, ਲੰਮਾ sample);
ਪੂਰਨ xdl_blankline(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਲੰਮਾ ਮਾਪ, ਲੰਮਾ flags);
ਪੂਰਨ xdl_recmatch(ਅਚਲ ਅੱਖਰ *l1, ਲੰਮਾ s1, ਅਚਲ ਅੱਖਰ *l2, ਲੰਮਾ s2, ਲੰਮਾ flags);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ xdl_hash_ਰਿਕਾਰਡ_verbatim(ਅੱਖਰ ਅਚਲ **ਅੰਕੜੇ, ਅੱਖਰ ਅਚਲ *ਸਿਖਰ);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ xdl_hash_ਰਿਕਾਰਡ_with_whitespace(ਅੱਖਰ ਅਚਲ **ਅੰਕੜੇ, ਅੱਖਰ ਅਚਲ *ਸਿਖਰ, ਲੰਮਾ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ xdl_hash_ਰਿਕਾਰਡ(ਅੱਖਰ ਅਚਲ **ਅੰਕੜੇ, ਅੱਖਰ ਅਚਲ *ਸਿਖਰ, ਲੰਮਾ flags)
{
	ਜੇ (flags & XDF_WHITESPACE_FLAGS)
		ਵਾਪਸ xdl_hash_ਰਿਕਾਰਡ_with_whitespace(ਅੰਕੜੇ, ਸਿਖਰ, flags);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ xdl_hash_ਰਿਕਾਰਡ_verbatim(ਅੰਕੜੇ, ਸਿਖਰ);
}
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ xdl_hashbits(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ);
ਪੂਰਨ xdl_ਅੰਕ_ਬਾਹਰਲਾ(ਅੱਖਰ *ਬਾਹਰਲਾ, ਲੰਮਾ val);
ਪੂਰਨ xdl_emit_hunk_hdr(ਲੰਮਾ s1, ਲੰਮਾ c1, ਲੰਮਾ s2, ਲੰਮਾ c2,
		      ਅਚਲ ਅੱਖਰ *func, ਲੰਮਾ funclen, xdemitcb_t *ecb);
ਪੂਰਨ xdl_fall_ਪਿੱਠ_diff(xdfenv_t *diff_env, xpparam_t ਅਚਲ *xpp,
		       ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
ਖਾਲੀ* xdl_alloc_grow_helper(ਖਾਲੀ* p, ਲੰਮਾ nr, ਲੰਮਾ* alloc, size_t ਮਾਪ);

#ਜੇ_ਅੰਤ /* #if !defined(XUTILS_H) */
