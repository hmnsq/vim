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

#nếu !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
số_nguyên xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb);
số_nguyên xdl_cha_init(chastore_t *cha, long isize, long icount);
void xdl_cha_free(chastore_t *cha);
void *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_lines(mmfile_t *mf, long sample);
số_nguyên xdl_blankline(const char *line, long size, long flags);
số_nguyên xdl_recmatch(const char *l1, long s1, const char *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(char const **data, char const *top);
unsigned long xdl_hash_record_with_whitespace(char const **data, char const *top, long flags);
static inline unsigned long xdl_hash_record(char const **data, char const *top, long flags)
{
	nếu (flags & XDF_WHITESPACE_FLAGS)
		trả_về xdl_hash_record_with_whitespace(data, top, flags);
	không_thì
		trả_về xdl_hash_record_verbatim(data, top);
}
unsigned số_nguyên xdl_hashbits(unsigned số_nguyên size);
số_nguyên xdl_num_out(char *out, long val);
số_nguyên xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const char *func, long funclen, xdemitcb_t *ecb);
số_nguyên xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		       số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
void* xdl_alloc_grow_helper(void* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
