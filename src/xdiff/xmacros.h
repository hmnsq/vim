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

#ఒకవేళ !defined(XMACROS_H)
#నిర్వచించు XMACROS_H


// SIZE_MAX may not be defined on older platforms without <stdint.h>.
#నిర్వచితం_కాకపోతే కొలత_MAX
# define కొలత_MAX ((size_t)(-1))
#అయితే_ముగింపు

#నిర్వచించు XDL_MIN(a, b) ((a) < (b) ? (a): (b))
#నిర్వచించు XDL_MAX(a, b) ((a) > (b) ? (a): (b))
#నిర్వచించు XDL_ABS(v) ((v) >= 0 ? (v): -(v))
#నిర్వచించు XDL_ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#నిర్వచించు XDL_ISSPACE(c) (ఖాళీనా((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు XDL_ADDBITS(v,b)	((v) + ((v) >> (b)))
#నిర్వచించు XDL_MASKBITS(b)		((1UL << (b)) - 1)
#నిర్వచించు XDL_HASHLONG(v,b)	(XDL_ADDBITS((గుర్తులేని దీర్ఘ)(v), b) & XDL_MASKBITS(b))
#నిర్వచించు XDL_LE32_PUT(p, v) \
చేయి { \
	గుర్తులేని అక్షరం *__p = (గుర్తులేని అక్షరం *) (p); \
	*__p++ = (గుర్తులేని అక్షరం) (v); \
	*__p++ = (గుర్తులేని అక్షరం) ((v) >> 8); \
	*__p++ = (గుర్తులేని అక్షరం) ((v) >> 16); \
	*__p = (గుర్తులేని అక్షరం) ((v) >> 24); \
} అయితే (0)
#నిర్వచించు XDL_LE32_GET(p, v) \
చేయి { \
	గుర్తులేని అక్షరం స్థిరరాశి *__p = (గుర్తులేని అక్షరం స్థిరరాశి *) (p); \
	(v) = (గుర్తులేని దీర్ఘ) __p[0] | ((గుర్తులేని దీర్ఘ) __p[1]) << 8 | \
		((గుర్తులేని దీర్ఘ) __p[2]) << 16 | ((గుర్తులేని దీర్ఘ) __p[3]) << 24; \
} అయితే (0)

/* Allocate an array of nr elements, returns NULL on failure */
#నిర్వచించు XDL_ALLOC_ARRAY(p, nr)				\
	((p) = కొలత_MAX / పరిమాణం(*(p)) >= (size_t)(nr)	\
		? xdl_malloc((nr) * పరిమాణం(*(p)))	\
		: ఖాళీ)

/* Allocate an array of nr zeroed out elements, returns NULL on failure */
#నిర్వచించు XDL_CALLOC_ARRAY(p, nr)	((p) = xdl_calloc(nr, పరిమాణం(*(p))))

/*
 * Ensure array p can accommodate at least nr elements, growing the
 * array and updating alloc (which is the number of allocated
 * elements) as necessary. Frees p and returns -1 on failure, returns
 * 0 on success
 */
#నిర్వచించు XDL_ALLOC_GROW(p, nr, alloc)	\
	(-!((nr) <= (alloc) ||		\
	    ((p) = xdl_alloc_grow_helper((p), (nr), &(alloc), పరిమాణం(*(p))))))

#అయితే_ముగింపు /* #if !defined(XMACROS_H) */
