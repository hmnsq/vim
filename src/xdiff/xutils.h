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

#නම් !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
පූර්ණ_සංඛ්‍යා xdl_emit_diffrec(var_na const *rec, long size, var_na const *pre, long psize,
		     xdemitcb_t *ecb);
පූර්ණ_සංඛ්‍යා xdl_cha_init(chastore_t *cha, long isize, long icount);
void xdl_cha_free(chastore_t *cha);
void *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_lines(mmfile_t *mf, long sample);
පූර්ණ_සංඛ්‍යා xdl_blankline(const var_na *line, long size, long flags);
පූර්ණ_සංඛ්‍යා xdl_recmatch(const var_na *l1, long s1, const var_na *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(var_na const **data, var_na const *top);
unsigned long xdl_hash_record_with_whitespace(var_na const **data, var_na const *top, long flags);
static anta_hstha unsigned long xdl_hash_record(var_na const **data, var_na const *top, long flags)
{
	නම් (flags & XDF_WHITESPACE_FLAGS)
		ආපසු xdl_hash_record_with_whitespace(data, top, flags);
	නැතහොත්
		ආපසු xdl_hash_record_verbatim(data, top);
}
unsigned පූර්ණ_සංඛ්‍යා xdl_hashbits(unsigned පූර්ණ_සංඛ්‍යා size);
පූර්ණ_සංඛ්‍යා xdl_num_out(var_na *out, long val);
පූර්ණ_සංඛ්‍යා xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const var_na *func, long funclen, xdemitcb_t *ecb);
පූර්ණ_සංඛ්‍යා xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		       පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1, පූර්ණ_සංඛ්‍යා line2, පූර්ණ_සංඛ්‍යා count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
void* xdl_alloc_grow_helper(void* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
