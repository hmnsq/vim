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
# অন্যথা
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#যদি defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#যদি defined(_MSC_VER)
# define অন্তঃস্থ __inline
#endif

// Vim replacement for BUG()
শূন্য xdl_bug(ধ্রুৱ ৱর্ণ *বাৰ্তা);

#যদি !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#যদি !defined(_WIN32)
#অন্তর্ভাৱ <unistd.h>
#endif
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <limits.h>
// This include comes from git, so uncomment it
#যদি 0
#অন্তর্ভাৱ "git-compat-util.h"
#endif
#অন্তর্ভাৱ "xmacros.h"
#অন্তর্ভাৱ "xdiff.h"
#অন্তর্ভাৱ "xtypes.h"
#অন্তর্ভাৱ "xutils.h"
#অন্তর্ভাৱ "xprepare.h"
#অন্তর্ভাৱ "xdiffi.h"
#অন্তর্ভাৱ "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
