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
#وصف_ڪريو XUTILS_H



ڊگهو xdl_bogosqrt(ڊگهو n);
صحيح xdl_emit_diffrec(حرف ثابت *rec, ڊگهو سائيز, حرف ثابت *pre, ڊگهو psize,
		     xdemitcb_t *ecb);
صحيح xdl_cha_init(chastore_t *cha, ڊگهو isize, ڊگهو icount);
فراغ xdl_cha_free(chastore_t *cha);
فراغ *xdl_cha_alloc(chastore_t *cha);
ڊگهو xdl_guess_سٽون(mmfile_t *mf, ڊگهو sample);
صحيح xdl_blankline(ثابت حرف *سٽ, ڊگهو سائيز, ڊگهو flags);
صحيح xdl_recmatch(ثابت حرف *l1, ڊگهو s1, ثابت حرف *l2, ڊگهو s2, ڊگهو flags);
بنانشان ڊگهو xdl_hash_رڪارڊ_verbatim(حرف ثابت **معلومات, حرف ثابت *چوٽي);
بنانشان ڊگهو xdl_hash_رڪارڊ_with_whitespace(حرف ثابت **معلومات, حرف ثابت *چوٽي, ڊگهو flags);
ساكن مضمن بنانشان ڊگهو xdl_hash_رڪارڊ(حرف ثابت **معلومات, حرف ثابت *چوٽي, ڊگهو flags)
{
	إذا (flags & XDF_WHITESPACE_FLAGS)
		أرجع xdl_hash_رڪارڊ_with_whitespace(معلومات, چوٽي, flags);
	وإلا
		أرجع xdl_hash_رڪارڊ_verbatim(معلومات, چوٽي);
}
بنانشان صحيح xdl_hashbits(بنانشان صحيح سائيز);
صحيح xdl_انگ_ٻاهريون(حرف *ٻاهريون, ڊگهو val);
صحيح xdl_emit_hunk_hdr(ڊگهو s1, ڊگهو c1, ڊگهو s2, ڊگهو c2,
		      ثابت حرف *func, ڊگهو funclen, xdemitcb_t *ecb);
صحيح xdl_fall_پٺ_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحيح line1, صحيح count1, صحيح line2, صحيح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
فراغ* xdl_alloc_grow_helper(فراغ* p, ڊگهو nr, ڊگهو* alloc, size_t سائيز);

#جيڪڏهن_پورو /* #if !defined(XUTILS_H) */
