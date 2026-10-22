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
#اگر_تعریف_شدہ HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# ورنہ
#  include "../auto/config.h"
# endif
#اگر_ختم

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#اگر defined(HAVE_ATTRIBUTE_غیر_مستعمل) || defined(__MINGW32__)
# define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#ورنہ
# define غیر_مستعمل
#اگر_ختم

#اگر defined(_MSC_VER)
# define اندرونی __inline
#اگر_ختم

// Vim replacement for BUG()
خالی xdl_bug(ثابت حرف *پیغام);

#اگر !defined(XINCLUDE_H)
#تعریف_کرو XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#شامل <ctype.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#اگر !defined(_WIN32)
#شامل <unistd.h>
#اگر_ختم
#شامل <string.h>
#شامل <limits.h>
// This include comes from git, so uncomment it
#اگر 0
#شامل "git-compat-util.h"
#اگر_ختم
#شامل "xmacros.h"
#شامل "xdiff.h"
#شامل "xtypes.h"
#شامل "xutils.h"
#شامل "xprepare.h"
#شامل "xdiffi.h"
#شامل "xemit.h"


#اگر_ختم /* #if !defined(XINCLUDE_H) */
