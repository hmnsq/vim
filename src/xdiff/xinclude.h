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
#اگر_تعریف_شده HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# وإلا
#  include "../auto/config.h"
# endif
#پایان_اگر

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#إذا defined(HAVE_ATTRIBUTE_استفادهنشده) || defined(__MINGW32__)
# define استفادهنشده __attribute__((استفادهنشده_2))
#وإلا
# define استفادهنشده
#پایان_اگر

#إذا defined(_MSC_VER)
# define مضمن __inline
#پایان_اگر

// Vim replacement for BUG()
فراغ xdl_bug(ثابت حرف *پیام);

#إذا !defined(XINCLUDE_H)
#تعریف_کن XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#ضمن <ctype.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#إذا !defined(_WIN32)
#ضمن <unistd.h>
#پایان_اگر
#ضمن <string.h>
#ضمن <limits.h>
// This include comes from git, so uncomment it
#إذا 0
#ضمن "git-compat-util.h"
#پایان_اگر
#ضمن "xmacros.h"
#ضمن "xdiff.h"
#ضمن "xtypes.h"
#ضمن "xutils.h"
#ضمن "xprepare.h"
#ضمن "xdiffi.h"
#ضمن "xemit.h"


#پایان_اگر /* #if !defined(XINCLUDE_H) */
