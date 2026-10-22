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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# ਨਹੀਂਤਾਂ
#  include "../auto/config.h"
# endif
#ਜੇ_ਅੰਤ

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ਜੇ defined(HAVE_ATTRIBUTE_ਅਣਵਰਤਿਆ) || defined(__MINGW32__)
# define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#ਨਹੀਂਤਾਂ
# define ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਜੇ defined(_MSC_VER)
# define ਸਤਰਅੰਦਰ __inline
#ਜੇ_ਅੰਤ

// Vim replacement for BUG()
ਖਾਲੀ xdl_bug(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ);

#ਜੇ !defined(XINCLUDE_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਜੇ !defined(_WIN32)
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
// This include comes from git, so uncomment it
#ਜੇ 0
#ਅਨ੍ਤਰ੍ਭਾਵ "git-compat-util.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "xmacros.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xdiff.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xtypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xutils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xprepare.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xdiffi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xemit.h"


#ਜੇ_ਅੰਤ /* #if !defined(XINCLUDE_H) */
