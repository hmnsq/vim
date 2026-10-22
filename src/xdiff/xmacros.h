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
#تعریف_کړه XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#که_نه_تعریف_شوی اندازه_MAX
# define اندازه_MAX ((size_t)(-1))
#د_که_پای

#تعریف_کړه XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#تعریف_کړه XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#تعریف_کړه XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#تعریف_کړه XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#تعریف_کړه XDL_ISSPACE(c) (که_تشه((بینښه حرف)(c)))
#تعریف_کړه XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#تعریف_کړه XDL_MASKBITS(b)		((1UL << (b)) - 1)
#تعریف_کړه XDL_HASHLONG(v,b)	(XDL_ADDBITS((بینښه اوږد)(v), b) & XDL_MASKBITS(b))
#تعریف_کړه XDL_LE32_PUT(p, v) \
افعل { \
	بینښه حرف *__p = (بینښه حرف *) (p); \
	*__p++ = (بینښه حرف) (v); \
	*__p++ = (بینښه حرف) ((v) >> 8); \
	*__p++ = (بینښه حرف) ((v) >> 16); \
	*__p = (بینښه حرف) ((v) >> 24); \
} طالما (0)
#تعریف_کړه XDL_LE32_GET(p, v) \
افعل { \
	بینښه حرف ثابت *__p = (بینښه حرف ثابت *) (p); \
	(v) = (بینښه اوږد) __p[0] | ((بینښه اوږد) __p[1]) << 8 | \
		((بینښه اوږد) __p[2]) << 16 | ((بینښه اوږد) __p[3]) << 24; \
} طالما (0)

/* Allocate an array of nr elements, returns NULL on failure */
#تعریف_کړه XDL_ALLOC_ARRAY(p, nr)				\
	((p) = اندازه_MAX / حجم(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * حجم(*(p)))	\
		: عدم)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#تعریف_کړه XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, حجم(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#تعریف_کړه XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), حجم(*(p))))))

#د_که_پای /* #if !defined(XMACROS_H) */
