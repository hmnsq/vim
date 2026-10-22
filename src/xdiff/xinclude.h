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
# ᯀᯮᯝᯂᯪ
#  include "../auto/config.h"
# endif
#endif

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ᯀᯮᯝ defined(HAVE_ATTRIBUTE_UNUSED) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#ᯀᯮᯝᯂᯪ
# define UNUSED
#endif

#ᯀᯮᯝ defined(_MSC_VER)
# define anta_hstha __inline
#endif

// Vim replacement for BUG()
void xdl_bug(const var_na *msg);

#ᯀᯮᯝ !defined(XINCLUDE_H)
#define XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ᯀᯮᯝ !defined(_WIN32)
#include <unistd.h>
#endif
#include <string.h>
#include <limits.h>
// This include comes from git, so uncomment it
#ᯀᯮᯝ 0
#include "git-compat-util.h"
#endif
#include "xmacros.h"
#include "xdiff.h"
#include "xtypes.h"
#include "xutils.h"
#include "xprepare.h"
#include "xdiffi.h"
#include "xemit.h"


#endif /* #if !defined(XINCLUDE_H) */
