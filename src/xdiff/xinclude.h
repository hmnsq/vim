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
# അല്ലെങ്കിൽ
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#എങ്കിൽ defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#അല്ലെങ്കിൽ
# define UNUSED
#endif

#എങ്കിൽ defined(_MSC_VER)
# define വരിയിൽ __inline
#endif

// Vim replacement for BUG()
ശൂന്യം xdl_bug(സ്ഥിരം അക്ഷരം *സന്ദേശം);

#എങ്കിൽ !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#എങ്കിൽ !defined(_WIN32)
#അന്തര്ഭാവ <unistd.h>
#endif
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <limits.h>
// This include comes from git, so uncomment it
#എങ്കിൽ 0
#അന്തര്ഭാവ "git-compat-util.h"
#endif
#അന്തര്ഭാവ "xmacros.h"
#അന്തര്ഭാവ "xdiff.h"
#അന്തര്ഭാവ "xtypes.h"
#അന്തര്ഭാവ "xutils.h"
#അന്തര്ഭാവ "xprepare.h"
#അന്തര്ഭാവ "xdiffi.h"
#അന്തര്ഭാവ "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
