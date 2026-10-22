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
#నిర్వచితమైతే HAVE_CONFIG_H
# ifdef VMS
#  include "config.h"
# లేకపోతే
#  include "../auto/config.h"
# endif
#అయితే_ముగింపు

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ఒకవేళ defined(HAVE_ATTRIBUTE_వాడని) || defined(__MINGW32__)
# define వాడని __attribute__((వాడని_2))
#లేకపోతే
# define వాడని
#అయితే_ముగింపు

#ఒకవేళ defined(_MSC_VER)
# define అంతఃవరుస __inline
#అయితే_ముగింపు

// Vim replacement for BUG()
శూన్య xdl_bug(స్థిరరాశి అక్షరం *సందేశం);

#ఒకవేళ !defined(XINCLUDE_H)
#నిర్వచించు XINCLUDE_H

// This effectively re-verts b46054b3746271d23feab0 from git
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#ఒకవేళ !defined(_WIN32)
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు
#అన్తర్భావ <string.h>
#అన్తర్భావ <limits.h>
// This include comes from git, so uncomment it
#ఒకవేళ 0
#అన్తర్భావ "git-compat-util.h"
#అయితే_ముగింపు
#అన్తర్భావ "xmacros.h"
#అన్తర్భావ "xdiff.h"
#అన్తర్భావ "xtypes.h"
#అన్తర్భావ "xutils.h"
#అన్తర్భావ "xprepare.h"
#అన్తర్భావ "xdiffi.h"
#అన్తర్భావ "xemit.h"


#అయితే_ముగింపు /* #if !defined(XINCLUDE_H) */
