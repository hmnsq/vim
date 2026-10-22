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

#यदि !defined(XMACROS_H)
#define XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#ifndef SIZE_MAX
# define SIZE_MAX ((माप_प्रकार)(-1))
#endif

#define XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#define XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#define XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#define XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define XDL_ISSPACE(c) (है_खाली((अचिन्हित वर्ण)(c)))
#define XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#define XDL_MASKBITS(b)		((1UL << (b)) - 1)
#define XDL_HASHLONG(v,b)	(XDL_ADDBITS((अचिन्हित दीर्घ)(v), b) & XDL_MASKBITS(b))
#define XDL_LE32_PUT(p, v) \
गर { \
	अचिन्हित वर्ण *__p = (अचिन्हित वर्ण *) (p); \
	*__p++ = (अचिन्हित वर्ण) (v); \
	*__p++ = (अचिन्हित वर्ण) ((v) >> 8); \
	*__p++ = (अचिन्हित वर्ण) ((v) >> 16); \
	*__p = (अचिन्हित वर्ण) ((v) >> 24); \
} जबसम्म (0)
#define XDL_LE32_GET(p, v) \
गर { \
	अचिन्हित वर्ण अचर *__p = (अचिन्हित वर्ण अचर *) (p); \
	(v) = (अचिन्हित दीर्घ) __p[0] | ((अचिन्हित दीर्घ) __p[1]) << 8 | \
		((अचिन्हित दीर्घ) __p[2]) << 16 | ((अचिन्हित दीर्घ) __p[3]) << 24; \
} जबसम्म (0)

/* Allocate an array of nr elements, returns NULL on failure */
#define XDL_ALLOC_ARRAY(p, nr)				\
	((p) = SIZE_MAX / परिमाण(*(p)) >= (माप_प्रकार)(nr)	\
		? xdl_malloc((nr) * परिमाण(*(p)))	\
		: रिक्त)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#define XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, परिमाण(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#define XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), परिमाण(*(p))))))

#endif /* #if !defined(XMACROS_H) */
