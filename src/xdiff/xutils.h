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
#تعریف_کن XUTILS_H



دراز xdl_bogosqrt(دراز n);
صحيح xdl_emit_diffrec(حرف ثابت *rec, دراز اندازه, حرف ثابت *pre, دراز psize,
		     xdemitcb_t *ecb);
صحيح xdl_cha_init(chastore_t *cha, دراز isize, دراز icount);
فراغ xdl_cha_free(chastore_t *cha);
فراغ *xdl_cha_alloc(chastore_t *cha);
دراز xdl_guess_سطرها(mmfile_t *mf, دراز sample);
صحيح xdl_blankline(ثابت حرف *سطر, دراز اندازه, دراز flags);
صحيح xdl_recmatch(ثابت حرف *l1, دراز s1, ثابت حرف *l2, دراز s2, دراز flags);
بیعلامت دراز xdl_hash_رکورد_verbatim(حرف ثابت **داده, حرف ثابت *بالاترین);
بیعلامت دراز xdl_hash_رکورد_with_whitespace(حرف ثابت **داده, حرف ثابت *بالاترین, دراز flags);
ساكن مضمن بیعلامت دراز xdl_hash_رکورد(حرف ثابت **داده, حرف ثابت *بالاترین, دراز flags)
{
	إذا (flags & XDF_WHITESPACE_FLAGS)
		أرجع xdl_hash_رکورد_with_whitespace(داده, بالاترین, flags);
	وإلا
		أرجع xdl_hash_رکورد_verbatim(داده, بالاترین);
}
بیعلامت صحيح xdl_hashbits(بیعلامت صحيح اندازه);
صحيح xdl_رقم_بیرون(حرف *بیرون, دراز val);
صحيح xdl_emit_hunk_hdr(دراز s1, دراز c1, دراز s2, دراز c2,
		      ثابت حرف *func, دراز funclen, xdemitcb_t *ecb);
صحيح xdl_fall_پشت_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحيح line1, صحيح count1, صحيح line2, صحيح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
فراغ* xdl_alloc_grow_helper(فراغ* p, دراز nr, دراز* alloc, size_t اندازه);

#پایان_اگر /* #if !defined(XUTILS_H) */
