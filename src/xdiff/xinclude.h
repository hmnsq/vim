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
# وگرنه
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#اگر defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#وگرنه
# define UNUSED
#endif

#اگر defined(_MSC_VER)
# define درونخطی __inline
#endif

// Vim replacement for BUG()
تهی xdl_bug(ثابت نویسه *پیام);

#اگر !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#شامل <ctype.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#اگر !defined(_WIN32)
#شامل <unistd.h>
#endif
#شامل <string.h>
#شامل <limits.h>
// This include comes from git, so uncomment it
#اگر 0
#شامل "git-compat-util.h"
#endif
#شامل "xmacros.h"
#شامل "xdiff.h"
#شامل "xtypes.h"
#شامل "xutils.h"
#شامل "xprepare.h"
#شامل "xdiffi.h"
#شامل "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
