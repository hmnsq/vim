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

#यदि !defined(XUTILS_H)
#define XUTILS_H



दीर्घ xdl_bogosqrt(दीर्घ n);
पूर्णाङ्क xdl_emit_diffrec(वर्ण अचर *rec, दीर्घ size, वर्ण अचर *pre, दीर्घ psize,
		     xdemitcb_t *ecb);
पूर्णाङ्क xdl_cha_init(chastore_t *cha, दीर्घ isize, दीर्घ icount);
शून्य xdl_cha_free(chastore_t *cha);
शून्य *xdl_cha_alloc(chastore_t *cha);
दीर्घ xdl_guess_पङ्क्तिहरू(mmfile_t *mf, दीर्घ sample);
पूर्णाङ्क xdl_blankline(अचर वर्ण *पङ्क्ति, दीर्घ size, दीर्घ flags);
पूर्णाङ्क xdl_recmatch(अचर वर्ण *l1, दीर्घ s1, अचर वर्ण *l2, दीर्घ s2, दीर्घ flags);
अचिन्हित दीर्घ xdl_hash_record_verbatim(वर्ण अचर **data, वर्ण अचर *शीर्ष);
अचिन्हित दीर्घ xdl_hash_record_with_whitespace(वर्ण अचर **data, वर्ण अचर *शीर्ष, दीर्घ flags);
स्थिर अन्तःस्थ अचिन्हित दीर्घ xdl_hash_record(वर्ण अचर **data, वर्ण अचर *शीर्ष, दीर्घ flags)
{
	यदि (flags & XDF_WHITESPACE_FLAGS)
		फर्काऊ xdl_hash_record_with_whitespace(data, शीर्ष, flags);
	अन्यथा
		फर्काऊ xdl_hash_record_verbatim(data, शीर्ष);
}
अचिन्हित पूर्णाङ्क xdl_hashbits(अचिन्हित पूर्णाङ्क size);
पूर्णाङ्क xdl_अङ्क_out(वर्ण *out, दीर्घ val);
पूर्णाङ्क xdl_emit_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
		      अचर वर्ण *func, दीर्घ funclen, xdemitcb_t *ecb);
पूर्णाङ्क xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t अचर *xpp,
		       पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
शून्य* xdl_alloc_grow_helper(शून्य* p, दीर्घ nr, दीर्घ* alloc, माप_प्रकार size);

#endif /* #if !defined(XUTILS_H) */
