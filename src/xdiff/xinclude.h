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
# अन्यथा
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#यदि defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#अन्यथा
# define UNUSED
#endif

#यदि defined(_MSC_VER)
# define अन्तःस्थ __inline
#endif

// Vim replacement for BUG()
शून्य xdl_bug(ध्रुव वर्ण *संदेश);

#यदि !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#यदि !defined(_WIN32)
#अन्तर्भाव <unistd.h>
#endif
#अन्तर्भाव <string.h>
#अन्तर्भाव <limits.h>
// This include comes from git, so uncomment it
#यदि 0
#अन्तर्भाव "git-compat-util.h"
#endif
#अन्तर्भाव "xmacros.h"
#अन्तर्भाव "xdiff.h"
#अन्तर्भाव "xtypes.h"
#अन्तर्भाव "xutils.h"
#अन्तर्भाव "xprepare.h"
#अन्तर्भाव "xdiffi.h"
#अन्तर्भाव "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
