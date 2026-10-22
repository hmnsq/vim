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
# ଅନ୍ୟଥା
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ଯଦି defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#ଅନ୍ୟଥା
# define UNUSED
#endif

#ଯଦି defined(_MSC_VER)
# define ଧାଡ଼ିଭିତରେ __inline
#endif

// Vim replacement for BUG()
ଶୂନ୍ୟ xdl_bug(ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ);

#ଯଦି !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଯଦି !defined(_WIN32)
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
// This include comes from git, so uncomment it
#ଯଦି 0
#ଅନ୍ତର୍ଭାଵ "git-compat-util.h"
#endif
#ଅନ୍ତର୍ଭାଵ "xmacros.h"
#ଅନ୍ତର୍ଭାଵ "xdiff.h"
#ଅନ୍ତର୍ଭାଵ "xtypes.h"
#ଅନ୍ତର୍ଭାଵ "xutils.h"
#ଅନ୍ତର୍ଭାଵ "xprepare.h"
#ଅନ୍ତର୍ଭାଵ "xdiffi.h"
#ଅନ୍ତର୍ଭାଵ "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
