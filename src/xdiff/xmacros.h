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

#ਜੇ !defined(XMACROS_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਮਾਪ_MAX
# define ਮਾਪ_MAX ((size_t)(-1))
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ISSPACE(c) (ਖਾਲੀ_ਥਾਂ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_MASKBITS(b)		((1UL << (b)) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_HASHLONG(v,b)	(XDL_ADDBITS((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(v), b) & XDL_MASKBITS(b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_LE32_PUT(p, v) \
ਕਰੋ { \
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *__p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *) (p); \
	*__p++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) (v); \
	*__p++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) ((v) >> 8); \
	*__p++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) ((v) >> 16); \
	*__p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) ((v) >> 24); \
} ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_LE32_GET(p, v) \
ਕਰੋ { \
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅਚਲ *__p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅਚਲ *) (p); \
	(v) = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) __p[0] | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) __p[1]) << 8 | \
		((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) __p[2]) << 16 | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) __p[3]) << 24; \
} ਜਦੋਂ (0)

/* Allocate an array of nr elements, returns NULL on failure */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ALLOC_ARRAY(p, nr)				\
	((p) = ਮਾਪ_MAX / ਆਕਾਰ(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * ਆਕਾਰ(*(p)))	\
		: ਨਿਰਾ)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, ਆਕਾਰ(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), ਆਕਾਰ(*(p))))))

#ਜੇ_ਅੰਤ /* #if !defined(XMACROS_H) */
