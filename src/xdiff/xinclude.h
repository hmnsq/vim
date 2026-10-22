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

// The following includes come from Vim:

// defines HAVE_ATTRIBUTE_UNUSED
#ifdef HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# இல்லையெனில்
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#எனில் defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#இல்லையெனில்
# define UNUSED
#endif

#எனில் defined(_MSC_VER)
# define உள்வரி __inline
#endif

// Vim replacement for BUG()
வெறுமை xdl_bug(மாறிலி எழுத்து *செய்தி);

#எனில் !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#எனில் !defined(_WIN32)
#அந்தர்பாவ <unistd.h>
#endif
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <limits.h>
// This include comes from git, so uncomment it
#எனில் 0
#அந்தர்பாவ "git-compat-util.h"
#endif
#அந்தர்பாவ "xmacros.h"
#அந்தர்பாவ "xdiff.h"
#அந்தர்பாவ "xtypes.h"
#அந்தர்பாவ "xutils.h"
#அந்தர்பாவ "xprepare.h"
#அந்தர்பாவ "xdiffi.h"
#அந்தர்பாவ "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
