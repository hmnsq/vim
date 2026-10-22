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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# ಇಲ್ಲದಿದ್ದರೆ
#  include "../auto/config.h"
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ಆದರೆ defined(HAVE_ATTRIBUTE_ಬಳಸದ) || defined(__MINGW32__)
# define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_MSC_VER)
# define ಸಾಲೊಳಗೆ __inline
#ಆದರೆ_ಅಂತ್ಯ

// Vim replacement for BUG()
ಶೂನ್ಯ xdl_bug(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ);

#ಆದರೆ !defined(XINCLUDE_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಆದರೆ !defined(_WIN32)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
// This include comes from git, so uncomment it
#ಆದರೆ 0
#ಅನ್ತರ್ಭಾವ "git-compat-util.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "xmacros.h"
#ಅನ್ತರ್ಭಾವ "xdiff.h"
#ಅನ್ತರ್ಭಾವ "xtypes.h"
#ಅನ್ತರ್ಭಾವ "xutils.h"
#ಅನ್ತರ್ಭಾವ "xprepare.h"
#ಅನ್ತರ್ಭಾವ "xdiffi.h"
#ಅನ್ತರ್ಭಾವ "xemit.h"


#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XINCLUDE_H) */
