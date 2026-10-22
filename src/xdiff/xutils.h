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

#सचे !defined(XUTILS_H)
#define XUTILS_H



long xdl_bogosqrt(long n);
पूर्णाङ्क xdl_emit_diffrec(वर्ण ध्रुव *rec, long size, वर्ण ध्रुव *pre, long psize,
		     xdemitcb_t *ecb);
पूर्णाङ्क xdl_cha_init(chastore_t *cha, long isize, long icount);
शून्य xdl_cha_free(chastore_t *cha);
शून्य *xdl_cha_alloc(chastore_t *cha);
long xdl_guess_पङ्क्तयः(mmfile_t *mf, long sample);
पूर्णाङ्क xdl_blankline(ध्रुव वर्ण *पङ्क्तिः, long size, long flags);
पूर्णाङ्क xdl_recmatch(ध्रुव वर्ण *l1, long s1, ध्रुव वर्ण *l2, long s2, long flags);
unsigned long xdl_hash_record_verbatim(वर्ण ध्रुव **data, वर्ण ध्रुव *शिखरम्);
unsigned long xdl_hash_record_with_whitespace(वर्ण ध्रुव **data, वर्ण ध्रुव *शिखरम्, long flags);
स्थिर अन्तःस्थ unsigned long xdl_hash_record(वर्ण ध्रुव **data, वर्ण ध्रुव *शिखरम्, long flags)
{
	सचे (flags & XDF_WHITESPACE_FLAGS)
		पटिनिवत्त xdl_hash_record_with_whitespace(data, शिखरम्, flags);
	अञ्ञथा
		पटिनिवत्त xdl_hash_record_verbatim(data, शिखरम्);
}
unsigned पूर्णाङ्क xdl_hashbits(unsigned पूर्णाङ्क size);
पूर्णाङ्क xdl_अङ्कः_out(वर्ण *out, long val);
पूर्णाङ्क xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ध्रुव वर्ण *func, long funclen, xdemitcb_t *ecb);
पूर्णाङ्क xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ध्रुव *xpp,
		       पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
शून्य* xdl_alloc_grow_helper(शून्य* p, long nr, long* alloc, size_t size);

#endif /* #if !defined(XUTILS_H) */
