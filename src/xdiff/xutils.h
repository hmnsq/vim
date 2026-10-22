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

#എങ്കിൽ !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
പൂർണ്ണസംഖ്യ xdl_emit_diffrec(അക്ഷരം സ്ഥിരം *rec, long size, അക്ഷരം സ്ഥിരം *pre, long psize,
		     xdemitcb_t *ecb);
പൂർണ്ണസംഖ്യ xdl_cha_init(chastore_t *cha, long isize, long icount);
ശൂന്യം xdl_cha_free(chastore_t *cha);
ശൂന്യം *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_വരികൾ(mmfile_t *mf, long sample);
പൂർണ്ണസംഖ്യ xdl_blankline(സ്ഥിരം അക്ഷരം *വരി, long size, long flags);
പൂർണ്ണസംഖ്യ xdl_recmatch(സ്ഥിരം അക്ഷരം *l1, long s1, സ്ഥിരം അക്ഷരം *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(അക്ഷരം സ്ഥിരം **data, അക്ഷരം സ്ഥിരം *മുകൾ);
unsigned long xdl_hash_record_with_whitespace(അക്ഷരം സ്ഥിരം **data, അക്ഷരം സ്ഥിരം *മുകൾ, long flags);
സ്ഥിതം വരിയിൽ unsigned long xdl_hash_record(അക്ഷരം സ്ഥിരം **data, അക്ഷരം സ്ഥിരം *മുകൾ, long flags)
{
	എങ്കിൽ (flags & XDF_WHITESPACE_FLAGS)
		തിരികെ xdl_hash_record_with_whitespace(data, മുകൾ, flags);
	അല്ലെങ്കിൽ
		തിരികെ xdl_hash_record_verbatim(data, മുകൾ);
}
unsigned പൂർണ്ണസംഖ്യ xdl_hashbits(unsigned പൂർണ്ണസംഖ്യ size);
പൂർണ്ണസംഖ്യ xdl_അക്കം_out(അക്ഷരം *out, long val);
പൂർണ്ണസംഖ്യ xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      സ്ഥിരം അക്ഷരം *func, long funclen, xdemitcb_t *ecb);
പൂർണ്ണസംഖ്യ xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t സ്ഥിരം *xpp,
		       പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
ശൂന്യം* xdl_alloc_grow_helper(ശൂന്യം* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
