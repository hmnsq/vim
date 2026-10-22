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
#جيڪڏهن_وصف_ٿيل HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# وإلا
#  include "../auto/config.h"
# endif
#جيڪڏهن_پورو

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#إذا defined(HAVE_ATTRIBUTE_اڻ_استعمال_ٿيل) || defined(__MINGW32__)
# define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#وإلا
# define اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#إذا defined(_MSC_VER)
# define مضمن __inline
#جيڪڏهن_پورو

// Vim replacement for BUG()
فراغ xdl_bug(ثابت حرف *نياپو);

#إذا !defined(XINCLUDE_H)
#وصف_ڪريو XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#ضمن <ctype.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#إذا !defined(_WIN32)
#ضمن <unistd.h>
#جيڪڏهن_پورو
#ضمن <string.h>
#ضمن <limits.h>
// This include comes from git, so uncomment it
#إذا 0
#ضمن "git-compat-util.h"
#جيڪڏهن_پورو
#ضمن "xmacros.h"
#ضمن "xdiff.h"
#ضمن "xtypes.h"
#ضمن "xutils.h"
#ضمن "xprepare.h"
#ضمن "xdiffi.h"
#ضمن "xemit.h"


#جيڪڏهن_پورو /* #if !defined(XINCLUDE_H) */
