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
#تعریف_کرو XUTILS_H



لما xdl_bogosqrt(لما n);
صحيح xdl_emit_diffrec(حرف ثابت *rec, لما ناپ, حرف ثابت *pre, لما psize,
		     xdemitcb_t *ecb);
صحيح xdl_cha_init(chastore_t *cha, لما isize, لما icount);
فراغ xdl_cha_free(chastore_t *cha);
فراغ *xdl_cha_alloc(chastore_t *cha);
لما xdl_guess_سطریں(mmfile_t *mf, لما sample);
صحيح xdl_blankline(ثابت حرف *سطر, لما ناپ, لما flags);
صحيح xdl_recmatch(ثابت حرف *l1, لما s1, ثابت حرف *l2, لما s2, لما flags);
بنانشانی لما xdl_hash_ریکارڈ_verbatim(حرف ثابت **انکڑے, حرف ثابت *چوٹی);
بنانشانی لما xdl_hash_ریکارڈ_with_whitespace(حرف ثابت **انکڑے, حرف ثابت *چوٹی, لما flags);
ساكن مضمن بنانشانی لما xdl_hash_ریکارڈ(حرف ثابت **انکڑے, حرف ثابت *چوٹی, لما flags)
{
	إذا (flags & XDF_WHITESPACE_FLAGS)
		أرجع xdl_hash_ریکارڈ_with_whitespace(انکڑے, چوٹی, flags);
	وإلا
		أرجع xdl_hash_ریکارڈ_verbatim(انکڑے, چوٹی);
}
بنانشانی صحيح xdl_hashbits(بنانشانی صحيح ناپ);
صحيح xdl_عدد_باہرلا(حرف *باہرلا, لما val);
صحيح xdl_emit_hunk_hdr(لما s1, لما c1, لما s2, لما c2,
		      ثابت حرف *func, لما funclen, xdemitcb_t *ecb);
صحيح xdl_fall_پٹھ_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحيح line1, صحيح count1, صحيح line2, صحيح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
فراغ* xdl_alloc_grow_helper(فراغ* p, لما nr, لما* alloc, size_t ناپ);

#جے_انت /* #if !defined(XUTILS_H) */
