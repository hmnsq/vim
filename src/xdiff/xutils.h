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
#تعریف_کړه XUTILS_H



اوږد xdl_bogosqrt(اوږد n);
صحيح xdl_emit_diffrec(حرف ثابت *rec, اوږد اندازه, حرف ثابت *pre, اوږد psize,
		     xdemitcb_t *ecb);
صحيح xdl_cha_init(chastore_t *cha, اوږد isize, اوږد icount);
فراغ xdl_cha_free(chastore_t *cha);
فراغ *xdl_cha_alloc(chastore_t *cha);
اوږد xdl_guess_کرښې(mmfile_t *mf, اوږد sample);
صحيح xdl_blankline(ثابت حرف *کرښه, اوږد اندازه, اوږد flags);
صحيح xdl_recmatch(ثابت حرف *l1, اوږد s1, ثابت حرف *l2, اوږد s2, اوږد flags);
بینښه اوږد xdl_hash_ریکارډ_verbatim(حرف ثابت **معلومات, حرف ثابت *سر_2);
بینښه اوږد xdl_hash_ریکارډ_with_whitespace(حرف ثابت **معلومات, حرف ثابت *سر_2, اوږد flags);
ساكن مضمن بینښه اوږد xdl_hash_ریکارډ(حرف ثابت **معلومات, حرف ثابت *سر_2, اوږد flags)
{
	إذا (flags & XDF_WHITESPACE_FLAGS)
		أرجع xdl_hash_ریکارډ_with_whitespace(معلومات, سر_2, flags);
	وإلا
		أرجع xdl_hash_ریکارډ_verbatim(معلومات, سر_2);
}
بینښه صحيح xdl_hashbits(بینښه صحيح اندازه);
صحيح xdl_عدد_بهر(حرف *بهر, اوږد val);
صحيح xdl_emit_hunk_hdr(اوږد s1, اوږد c1, اوږد s2, اوږد c2,
		      ثابت حرف *func, اوږد funclen, xdemitcb_t *ecb);
صحيح xdl_fall_شا_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحيح line1, صحيح count1, صحيح line2, صحيح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
فراغ* xdl_alloc_grow_helper(فراغ* p, اوږد nr, اوږد* alloc, size_t اندازه);

#د_که_پای /* #if !defined(XUTILS_H) */
