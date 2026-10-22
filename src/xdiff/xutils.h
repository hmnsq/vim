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

#जर !defined(XUTILS_H)
#define XUTILS_H



दीर्घ xdl_bogosqrt(दीर्घ n);
पूर्णांक xdl_emit_diffrec(अक्षर स्थिरांक *rec, दीर्घ size, अक्षर स्थिरांक *pre, दीर्घ psize,
		     xdemitcb_t *ecb);
पूर्णांक xdl_cha_init(chastore_t *cha, दीर्घ isize, दीर्घ icount);
शून्य xdl_cha_free(chastore_t *cha);
शून्य *xdl_cha_alloc(chastore_t *cha);
दीर्घ xdl_guess_ओळी(mmfile_t *mf, दीर्घ sample);
पूर्णांक xdl_blankline(स्थिरांक अक्षर *ओळ, दीर्घ size, दीर्घ flags);
पूर्णांक xdl_recmatch(स्थिरांक अक्षर *l1, दीर्घ s1, स्थिरांक अक्षर *l2, दीर्घ s2, दीर्घ flags);
अचिन्हित दीर्घ xdl_hash_record_verbatim(अक्षर स्थिरांक **data, अक्षर स्थिरांक *शीर्ष);
अचिन्हित दीर्घ xdl_hash_record_with_whitespace(अक्षर स्थिरांक **data, अक्षर स्थिरांक *शीर्ष, दीर्घ flags);
स्थिर ओळीत अचिन्हित दीर्घ xdl_hash_record(अक्षर स्थिरांक **data, अक्षर स्थिरांक *शीर्ष, दीर्घ flags)
{
	जर (flags & XDF_WHITESPACE_FLAGS)
		परत xdl_hash_record_with_whitespace(data, शीर्ष, flags);
	अन्यथा
		परत xdl_hash_record_verbatim(data, शीर्ष);
}
अचिन्हित पूर्णांक xdl_hashbits(अचिन्हित पूर्णांक size);
पूर्णांक xdl_अंक_out(अक्षर *out, दीर्घ val);
पूर्णांक xdl_emit_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
		      स्थिरांक अक्षर *func, दीर्घ funclen, xdemitcb_t *ecb);
पूर्णांक xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t स्थिरांक *xpp,
		       पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
शून्य* xdl_alloc_grow_helper(शून्य* p, दीर्घ nr, दीर्घ* alloc, माप_प्रकार size);

#endif /* #if !defined(XUTILS_H) */
