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

#اگر !defined(XUTILS_H)
#تعریف_کرو XUTILS_H



طویل xdl_bogosqrt(طویل n);
صحیح xdl_emit_diffrec(حرف ثابت *rec, طویل سائز, حرف ثابت *pre, طویل psize,
		     xdemitcb_t *ecb);
صحیح xdl_cha_init(chastore_t *cha, طویل isize, طویل icount);
خالی xdl_cha_free(chastore_t *cha);
خالی *xdl_cha_alloc(chastore_t *cha);
طویل xdl_guess_سطریں(mmfile_t *mf, طویل sample);
صحیح xdl_blankline(ثابت حرف *سطر, طویل سائز, طویل flags);
صحیح xdl_recmatch(ثابت حرف *l1, طویل s1, ثابت حرف *l2, طویل s2, طویل flags);
غیرعلامتی طویل xdl_hash_ریکارڈ_verbatim(حرف ثابت **ڈیٹا, حرف ثابت *چوٹی);
غیرعلامتی طویل xdl_hash_ریکارڈ_with_whitespace(حرف ثابت **ڈیٹا, حرف ثابت *چوٹی, طویل flags);
ساکن اندرونی غیرعلامتی طویل xdl_hash_ریکارڈ(حرف ثابت **ڈیٹا, حرف ثابت *چوٹی, طویل flags)
{
	اگر (flags & XDF_WHITESPACE_FLAGS)
		واپس xdl_hash_ریکارڈ_with_whitespace(ڈیٹا, چوٹی, flags);
	ورنہ
		واپس xdl_hash_ریکارڈ_verbatim(ڈیٹا, چوٹی);
}
غیرعلامتی صحیح xdl_hashbits(غیرعلامتی صحیح سائز);
صحیح xdl_عدد_برآمد(حرف *برآمد, طویل val);
صحیح xdl_emit_hunk_hdr(طویل s1, طویل c1, طویل s2, طویل c2,
		      ثابت حرف *func, طویل funclen, xdemitcb_t *ecb);
صحیح xdl_fall_عقب_diff(xdfenv_t *diff_env, xpparam_t ثابت *xpp,
		       صحیح line1, صحیح count1, صحیح line2, صحیح count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
خالی* xdl_alloc_grow_helper(خالی* p, طویل nr, طویل* alloc, size_t سائز);

#اگر_ختم /* #if !defined(XUTILS_H) */
