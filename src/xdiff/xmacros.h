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

#اگر !defined(XMACROS_H)
#تعریف_کرو XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#اگر_غیر_تعریف_شدہ سائز_MAX
# define سائز_MAX ((size_t)(-1))
#اگر_ختم

#تعریف_کرو XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#تعریف_کرو XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#تعریف_کرو XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#تعریف_کرو XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#تعریف_کرو XDL_ISSPACE(c) (خلا_ہے((غیرعلامتی حرف)(c)))
#تعریف_کرو XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#تعریف_کرو XDL_MASKBITS(b)		((1UL << (b)) - 1)
#تعریف_کرو XDL_HASHLONG(v,b)	(XDL_ADDBITS((غیرعلامتی طویل)(v), b) & XDL_MASKBITS(b))
#تعریف_کرو XDL_LE32_PUT(p, v) \
کرو { \
	غیرعلامتی حرف *__p = (غیرعلامتی حرف *) (p); \
	*__p++ = (غیرعلامتی حرف) (v); \
	*__p++ = (غیرعلامتی حرف) ((v) >> 8); \
	*__p++ = (غیرعلامتی حرف) ((v) >> 16); \
	*__p = (غیرعلامتی حرف) ((v) >> 24); \
} جب (0)
#تعریف_کرو XDL_LE32_GET(p, v) \
کرو { \
	غیرعلامتی حرف ثابت *__p = (غیرعلامتی حرف ثابت *) (p); \
	(v) = (غیرعلامتی طویل) __p[0] | ((غیرعلامتی طویل) __p[1]) << 8 | \
		((غیرعلامتی طویل) __p[2]) << 16 | ((غیرعلامتی طویل) __p[3]) << 24; \
} جب (0)

/* Allocate an array of nr elements, returns NULL on failure */
#تعریف_کرو XDL_ALLOC_ARRAY(p, nr)				\
	((p) = سائز_MAX / حجم(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * حجم(*(p)))	\
		: تہی)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#تعریف_کرو XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, حجم(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#تعریف_کرو XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), حجم(*(p))))))

#اگر_ختم /* #if !defined(XMACROS_H) */
