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

#إذا !defined(XMACROS_H)
#تعریف_کن XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#اگر_تعریف_نشده اندازه_MAX
# define اندازه_MAX ((size_t)(-1))
#پایان_اگر

#تعریف_کن XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#تعریف_کن XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#تعریف_کن XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#تعریف_کن XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#تعریف_کن XDL_ISSPACE(c) (آیا_فاصله((بیعلامت حرف)(c)))
#تعریف_کن XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#تعریف_کن XDL_MASKBITS(b)		((1UL << (b)) - 1)
#تعریف_کن XDL_HASHLONG(v,b)	(XDL_ADDBITS((بیعلامت دراز)(v), b) & XDL_MASKBITS(b))
#تعریف_کن XDL_LE32_PUT(p, v) \
افعل { \
	بیعلامت حرف *__p = (بیعلامت حرف *) (p); \
	*__p++ = (بیعلامت حرف) (v); \
	*__p++ = (بیعلامت حرف) ((v) >> 8); \
	*__p++ = (بیعلامت حرف) ((v) >> 16); \
	*__p = (بیعلامت حرف) ((v) >> 24); \
} طالما (0)
#تعریف_کن XDL_LE32_GET(p, v) \
افعل { \
	بیعلامت حرف ثابت *__p = (بیعلامت حرف ثابت *) (p); \
	(v) = (بیعلامت دراز) __p[0] | ((بیعلامت دراز) __p[1]) << 8 | \
		((بیعلامت دراز) __p[2]) << 16 | ((بیعلامت دراز) __p[3]) << 24; \
} طالما (0)

/* Allocate an array of nr elements, returns NULL on failure */
#تعریف_کن XDL_ALLOC_ARRAY(p, nr)				\
	((p) = اندازه_MAX / حجم(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * حجم(*(p)))	\
		: عدم)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#تعریف_کن XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, حجم(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#تعریف_کن XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), حجم(*(p))))))

#پایان_اگر /* #if !defined(XMACROS_H) */
