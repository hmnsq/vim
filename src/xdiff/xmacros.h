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

#ಆದರೆ !defined(XMACROS_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಗಾತ್ರ_MAX
# define ಗಾತ್ರ_MAX ((size_t)(-1))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ISSPACE(c) (ಖಾಲಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_MASKBITS(b)		((1UL << (b)) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_HASHLONG(v,b)	(XDL_ADDBITS((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(v), b) & XDL_MASKBITS(b))
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_LE32_PUT(p, v) \
ಮಾಡು { \
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *__p = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *) (p); \
	*__p++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) (v); \
	*__p++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) ((v) >> 8); \
	*__p++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) ((v) >> 16); \
	*__p = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) ((v) >> 24); \
} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_LE32_GET(p, v) \
ಮಾಡು { \
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *__p = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *) (p); \
	(v) = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) __p[0] | ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) __p[1]) << 8 | \
		((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) __p[2]) << 16 | ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) __p[3]) << 24; \
} ಆಗ (0)

/* Allocate an array of nr elements, returns NULL on failure */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ALLOC_ARRAY(p, nr)				\
	((p) = ಗಾತ್ರ_MAX / ಗಾತ್ರ(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * ಗಾತ್ರ(*(p)))	\
		: ಖಾಲಿ)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, ಗಾತ್ರ(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#ವ್ಯಾಖ್ಯಾನಿಸು XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), ಗಾತ್ರ(*(p))))))

#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XMACROS_H) */
