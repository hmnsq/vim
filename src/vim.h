/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#جيڪڏهن_وصف_نه_ٿيل VIM__H
#وصف_ڪريو VIM__H

#ضمن "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#إذا defined(WIN32) || defined(_WIN32)
# define MSWIN
#جيڪڏهن_پورو

#إذا defined(MSWIN)
# include <io.h>
#جيڪڏهن_پورو

// ============ the header file puzzle: order matters =========

#جيڪڏهن_وصف_ٿيل HAVE_CONFIG_H	// GNU autoconf (or something else) was here
# ifdef VMS
#  include "config.h"   /* Rely on /INCLUDE to find it. */
# وإلا
#  include "auto/config.h"
# endif /* def VMS [else] */
# define HAVE_PATHDEF

/*
 * Check if configure correctly managed to find sizeof(int).  If this failed,
 * it becomes zero.  This is likely a problem of not being able to run the
 * test program.  Other items from configure may also be wrong then!
 */
# إذا (VIM_SIZEOF_INT == 0)
#  error configure did not لاڳيتو properly.  Check خودڪار/config.لاگرٿم.
# endif

# إذا (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_ذريعو
#   define XOPEN_ذريعو    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_ذريعو
#    define BSD_ذريعو 1
#   endif
#   ifndef SVID_ذريعو
#    define SVID_ذريعو 1
#   endif
#   ifndef _DEFAULT_SOURCE
#    define _DEFAULT_SOURCE 1
#   endif
#  endif
# endif

/*
 * Cygwin may have fchdir() in a newer release, but in most versions it
 * doesn't work well and avoiding it keeps the binary backward compatible.
 */
# إذا defined(__CYGWIN32__) && defined(HAVE_FCHDIR)
#  undef HAVE_FCHDIR
# endif

// We may need to define the uint32_t on non-Unix system, but using the same
// identifier causes conflicts.  Therefore use UINT32_T.
# define UINT32_TYPEDEF uint32_t
#جيڪڏهن_پورو

// for INT_MAX, LONG_MAX et al.
#ضمن <limits.h>

#إذا !defined(UINT32_TYPEDEF)
# إذا defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# وإلا
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF بنانشان صحيح
# endif
#جيڪڏهن_پورو

// C89 does not define SIZE_MAX
#إذا defined(__hpux) || defined(VMS)
# ifndef سائيز_MAX
#  define سائيز_MAX ((size_t)(-1))
# endif
#جيڪڏهن_پورو

// user ID of root is usually zero, but not for everybody
#جيڪڏهن_وصف_ٿيل __TANDEM
# ifndef TANDEM_ذريعو
#  define TANDEM_ذريعو
# endif
# include <floss.h>
# define پاڙ_UID 65535
# define OLDXAW
# إذا (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#وإلا
# define پاڙ_UID 0
#جيڪڏهن_پورو

/* Include MAC_OS_X_VERSION_* macros */
#جيڪڏهن_وصف_ٿيل HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#جيڪڏهن_پورو

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#إذا defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#جيڪڏهن_پورو
// Unless made through the Makefile enforce GUI on Mac
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# إذا !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#جيڪڏهن_پورو

// Check support for rendering options
#جيڪڏهن_وصف_ٿيل FEAT_GUI
# إذا defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#جيڪڏهن_پورو

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#جيڪڏهن_وصف_ٿيل MSWIN
# define VIM_SIZEOF_INT 4
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  وإلا
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#جيڪڏهن_پورو
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#جيڪڏهن_پورو

#إذا VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only هلي with 32 ٻٽو_انگ صحيح or larger
#جيڪڏهن_پورو

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#ضمن "feature.h"

#إذا defined(MACOS_X_DARWIN)
# إذا defined(FEAT_عام) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# إذا defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_نسخو_MIN_REQUIRED) && MAC_OS_X_نسخو_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# إذا defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#جيڪڏهن_پورو

// +x11 is only enabled when it's both available and wanted.
#إذا defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#جيڪڏهن_پورو

#إذا defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل NO_X11_INCLUDES
    // In os_mac_conv.c and os_macosx.m NO_X11_INCLUDES is defined to avoid
    // X11 headers.  Disable all X11 related things to avoid conflicts.
# ifdef FEAT_X11
#  undef FEAT_X11
# endif
# ifdef FEAT_GUI_X11
#  undef FEAT_GUI_X11
# endif
# ifdef FEAT_XCLIPBOARD
#  undef FEAT_XCLIPBOARD
# endif
# ifdef FEAT_GUI_MOTIF
#  undef FEAT_GUI_MOTIF
# endif
# ifdef FEAT_GUI_GTK
#  undef FEAT_GUI_GTK
# endif
# ifdef FEAT_BEVAL_TIP
#  undef FEAT_BEVAL_TIP
# endif
# ifdef FEAT_XIM
#  undef FEAT_XIM
# endif
# ifdef FEAT_CLIENTSERVER
#  undef FEAT_CLIENTSERVER
# endif
#جيڪڏهن_پورو

// The Mac conversion stuff doesn't work under X11.
#إذا defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#جيڪڏهن_پورو

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#جيڪڏهن_وصف_نه_ٿيل VIMPACKAGE
# define VIMPACKAGE	"vim"
#جيڪڏهن_پورو

/*
 * Find out if function definitions should include argument types
 */
#جيڪڏهن_وصف_ٿيل AZTEC_C
# include <functions.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SASC
# include <clib/exec_protos.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _DCC
# include <clib/exec_protos.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#جيڪڏهن_پورو

#إذا (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#وإلاإذا !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) إشارة(signum, sighandler)
#جيڪڏهن_پورو

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#جيڪڏهن_وصف_نه_ٿيل اڻ_استعمال_ٿيل
# إذا defined(HAVE_ATTRIBUTE_اڻ_استعمال_ٿيل) || defined(__MINGW32__)
#  define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
# وإلا
#  إذا defined __has_attribute
#   إذا __has_attribute(اڻ_استعمال_ٿيل_2)
#    define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#   endif
#  endif
# endif
# ifndef اڻ_استعمال_ٿيل
#  define اڻ_استعمال_ٿيل
# endif
#جيڪڏهن_پورو

// Used to check for "sun", "__sun" is used by newer compilers.
#إذا defined(__sun)
# define SUN_SYSTEM
#جيڪڏهن_پورو

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#إذا !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل PROTO
# ifdef AMIGA
#  include "os_amiga.h"
# endif

# ifdef MSWIN
#  include "os_win32.h"
# endif

# إذا defined(MACOS_X)
#  include "os_mac.h"
# endif

# ifdef __QNX__
#  include "os_qnx.h"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل X_LOCALE
# include <X11/Xlocale.h>
#وإلا
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#جيڪڏهن_پورو

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#جيڪڏهن_وصف_نه_ٿيل MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# وإلا
#  define MAXPATHL  256
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
# define رستو_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#وإلا
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define رستو_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# وإلا
#  define رستو_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#جيڪڏهن_پورو

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#وصف_ڪريو NUMBUFLEN 65

// flags for vim_str2nr()
#وصف_ڪريو STR2NR_BIN  0x01
#وصف_ڪريو STR2NR_OCT  0x02
#وصف_ڪريو STR2NR_HEX  0x04
#وصف_ڪريو STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#وصف_ڪريو STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#وصف_ڪريو STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#وصف_ڪريو STR2NR_FORCE 0x80   // only when ONE of the above is used

#وصف_ڪريو STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
تعريفنوع بنانشان حرف	char_u;
تعريفنوع بنانشان ننڍو	short_u;
تعريفنوع بنانشان صحيح	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#جيڪڏهن_وصف_ٿيل _WIN64
تعريفنوع بنانشان __int64	long_u;
تعريفنوع		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#وإلا
تعريفنوع بنانشان ڊگهو		long_u;
تعريفنوع		 ڊگهو		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#جيڪڏهن_پورو
#وصف_ڪريو PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#إذا defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#جيڪڏهن_پورو

/*
 * We use 64-bit file functions here, if available.  E.g. ftello() returns
 * off_t instead of long, which helps if long is 32 bit and off_t is 64 bit.
 * We assume that when fseeko() is available then ftello() is too.
 * Note that Windows has different function names.
 */
#إذا defined(MSWIN) && !defined(PROTO)
تعريفنوع __int64 off_T;
# ifdef __MINGW32__
#  define vim_lseek lseek64
#  define vim_fseek fseeko64
#  define vim_ftell ftello64
# وإلا
#  define vim_lseek _lseeki64
#  define vim_fseek _fseeki64
#  define vim_ftell _ftelli64
# endif
#وإلا
تعريفنوع off_t off_T;
# ifdef HAVE_FSEEKO
#  define vim_lseek lseek
#  define vim_ftell ftello
#  define vim_fseek fseeko
# وإلا
#  define vim_lseek lseek
#  define vim_ftell ftell
#  define vim_fseek(a, b, c)	fseek(a, (ڊگهو)b, c)
# endif
#جيڪڏهن_پورو

/*
 * The characters and attributes cached for the screen.
 */
تعريفنوع char_u schar_T;
تعريفنوع بنانشان ننڍو sattr_T;
#وصف_ڪريو MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
تعريفنوع بنانشان صحيح u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#إذا defined(VMS) && defined(VAX)
// unsupported compiler
تعريفنوع ڊگهو      vimlong_T;
#وإلا
تعريفنوع ڊگهو ڊگهو vimlong_T;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#جيڪڏهن_پورو

#ضمن "ascii.h"
#ضمن "keymap.h"
#ضمن "termdefs.h"
#ضمن "macros.h"

#جيڪڏهن_وصف_ٿيل LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل _DCC
# include <sys/stat.h>
#جيڪڏهن_پورو
#إذا defined(MSWIN)
# include <sys/stat.h>
#جيڪڏهن_پورو

#إذا defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#جيڪڏهن_پورو

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#جيڪڏهن_وصف_نه_ٿيل UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# إذا defined(HAVE_لڙيون_H) && !defined(NO_لڙيون_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#جيڪڏهن_پورو // NON-UNIX

#ضمن <assert.h>

#جيڪڏهن_وصف_ٿيل HAVE_STDINT_H
# include <stdint.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_INTTYPES_H
# include <inttypes.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_WCTYPE_H
# include <wctype.h>
#جيڪڏهن_پورو
#ضمن <stdarg.h>
// older compilers do not define va_copy
#جيڪڏهن_وصف_نه_ٿيل va_copy
# define va_copy(dst, src)	((dst) = (src))
#جيڪڏهن_پورو

// for offsetof()
#ضمن <stddef.h>
#ضمن <stdbool.h>

#إذا defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_وقت_H) || defined(SYS_SELECT_WITH_SYS_وقت))
# include <sys/select.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# وإلا
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SODIUM
# include <sodium.h>
#جيڪڏهن_پورو

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#جيڪڏهن_وصف_ٿيل DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#جيڪڏهن_پورو

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#جيڪڏهن_وصف_ٿيل DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
خارجي حرف *(*dyn_libintl_gettext)(ثابت حرف *msgid);
خارجي حرف *(*dyn_libintl_ngettext)(ثابت حرف *msgid, ثابت حرف *msgid_plural, بنانشان ڊگهو n);
خارجي حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *domainname, ثابت حرف *dirname);
خارجي حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *domainname, ثابت حرف *codeset);
خارجي حرف *(*dyn_libintl_textdomain)(ثابت حرف *domainname);
خارجي صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *envstring);
خارجي صحيح *dyn_libintl_nl_نياپو_cat_cntr;
#جيڪڏهن_پورو


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#جيڪڏهن_وصف_ٿيل FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
#  define _(x) (*dyn_libintl_gettext)((حرف *)(x))
#  define NGETTEXT(x, xs, n) (*dyn_libintl_ngettext)((حرف *)(x), (حرف *)(xs), (n))
#  define N_(x) x
#  define bindtextdomain(domain, dir) (*dyn_libintl_bindtextdomain)((domain), (dir))
#  define bind_textdomain_codeset(domain, codeset) (*dyn_libintl_bind_textdomain_codeset)((domain), (codeset))
#  إذا !defined(HAVE_BIND_TEXTDOMAIN_CODESET)
#   define HAVE_BIND_TEXTDOMAIN_CODESET 1
#  endif
#  define textdomain(domain) (*dyn_libintl_textdomain)(domain)
#  define libintl_wputenv(envstring) (*dyn_libintl_wputenv)(envstring)
# وإلا
#  include <libintl.h>
#  define _(x) gettext((حرف *)(x))
#  define NGETTEXT(x, xs, n) ngettext((x), (xs), (n))
#  ifdef gettext_noop
#   define N_(x) gettext_noop(x)
#  وإلا
#   define N_(x) x
#  endif
# endif
#وإلا
# define _(x) ((حرف *)(x))
# define NGETTEXT(x, xs, n) (((n) == 1) ? (حرف *)(x) : (حرف *)(xs))
# define N_(x) x
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# define bindtextdomain(x, y) ""
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# define bind_textdomain_codeset(x, y) //خالي_جايون
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //خالي_جايون
#جيڪڏهن_پورو

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#وصف_ڪريو UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#وصف_ڪريو UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#وصف_ڪريو UPD_INVERTED		20  // redisplay inverted part that changed
#وصف_ڪريو UPD_INVERTED_ALL	25  // redisplay whole inverted part
#وصف_ڪريو UPD_REDRAW_چوٽي		30  // display first w_upd_rows screen lines
#وصف_ڪريو UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#وصف_ڪريو UPD_NOT_VALID		40  // buffer needs complete redraw
#وصف_ڪريو UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#وصف_ڪريو SLF_RIGHTLEFT	1
#وصف_ڪريو SLF_POPUP	2
#وصف_ڪريو SLF_INC_VCOL	4

#وصف_ڪريو MB_FILLER_CHAR '<'  // character used when a double-width character
			    // doesn't fit.

/*
 * Flags for w_valid.
 * These are set when something in a window structure becomes invalid, except
 * when the cursor is moved.  Call check_cursor_moved() before testing one of
 * the flags.
 * These are reset when that thing has been updated and is valid again.
 *
 * Every function that invalidates one of these must call one of the
 * invalidate_* functions.
 *
 * w_valid is supposed to be used only in screen.c.  From other files, use the
 * functions that set or reset the flags.
 *
 * VALID_BOTLINE    VALID_BOTLINE_AP
 *     on		on		w_botline valid
 *     off		on		w_botline approximated
 *     off		off		w_botline not valid
 *     on		off		not possible
 */
#وصف_ڪريو VALID_WROW	0x01	// w_wrow (window row) is valid
#وصف_ڪريو VALID_WCOL	0x02	// w_wcol (window col) is valid
#وصف_ڪريو VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#وصف_ڪريو VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#وصف_ڪريو VALID_CROW	0x10	// w_cline_row is valid
#وصف_ڪريو VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#وصف_ڪريو VALID_BOTLINE_AP 0x40	// w_botline is approximated
#وصف_ڪريو VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#وصف_ڪريو POPF_IS_POPUP	0x01	// this is a popup window
#وصف_ڪريو POPF_HIDDEN	0x02	// popup is not displayed
#وصف_ڪريو POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#وصف_ڪريو POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#وصف_ڪريو POPF_ON_CMDLINE	0x10	// popup overlaps command line
#وصف_ڪريو POPF_DRAG	0x20	// popup can be moved by dragging border
#وصف_ڪريو POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#وصف_ڪريو POPF_RESIZE	0x80	// popup can be resized by dragging
#وصف_ڪريو POPF_MAPPING	0x100	// mapping keys
#وصف_ڪريو POPF_INFO	0x200	// used for info of popup menu
#وصف_ڪريو POPF_INFO_MENU	0x400	// align info popup with popup menu
#وصف_ڪريو POPF_POSINVERT	0x800	// vertical position can be inverted
#وصف_ڪريو POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#وصف_ڪريو POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#وصف_ڪريو POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#وصف_ڪريو POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#وصف_ڪريو POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#وصف_ڪريو POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#وصف_ڪريو POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#وصف_ڪريو POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#وصف_ڪريو HL_عام		0x00
#وصف_ڪريو HL_INVERSE		0x01
#وصف_ڪريو HL_BOLD			0x02
#وصف_ڪريو HL_ITALIC		0x04
#وصف_ڪريو HL_UNDERLINE		0x08
#وصف_ڪريو HL_UNDERCURL		0x10
#وصف_ڪريو HL_UNDERDOUBLE		0x20
#وصف_ڪريو HL_UNDERDOTTED		0x40
#وصف_ڪريو HL_UNDERDASHED		0x80
#وصف_ڪريو HL_STANDOUT		0x100
#وصف_ڪريو HL_NOCOMBINE		0x200
#وصف_ڪريو HL_STRIKETHROUGH	0x400
#وصف_ڪريو HL_ALL			0x7ff

// special attribute addition: Put message in history
#وصف_ڪريو نياپو_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#وصف_ڪريو گهڻائي_عام	0x01	// Normal mode, command expected
#وصف_ڪريو گهڻائي_VISUAL	0x02	// Visual mode - use get_real_state()
#وصف_ڪريو گهڻائي_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#وصف_ڪريو گهڻائي_CMDLINE	0x08	// Editing the command line
#وصف_ڪريو گهڻائي_وجهو	0x10	// Insert mode, also for Replace mode
#وصف_ڪريو گهڻائي_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#وصف_ڪريو گهڻائي_SELECT	0x40	// Select mode, use get_real_state()
#وصف_ڪريو گهڻائي_TERMINAL	0x80	// Terminal mode

#وصف_ڪريو MAP_ALL_MODES	0xff    // all mode bits used for mapping

#وصف_ڪريو REPLACE_FLAG	0x100	// Replace mode flag
#وصف_ڪريو گهڻائي_REPLACE	(REPLACE_FLAG | گهڻائي_وجهو)
#وصف_ڪريو VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#وصف_ڪريو گهڻائي_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | گهڻائي_وجهو)
#وصف_ڪريو گهڻائي_LREPLACE	(REPLACE_FLAG | گهڻائي_LANGMAP)

#وصف_ڪريو گهڻائي_عام_BUSY (0x1000 | گهڻائي_عام)
				// Normal mode, busy with a command
#وصف_ڪريو گهڻائي_HITRETURN	(0x2000 | گهڻائي_عام)
				// waiting for return or command
#وصف_ڪريو گهڻائي_ASKMORE	0x3000	// Asking if you want --more--
#وصف_ڪريو گهڻائي_SETWSIZE	0x4000	// window size has changed
#وصف_ڪريو گهڻائي_EXTERNCMD	0x5000	// executing an external command
#وصف_ڪريو گهڻائي_SHOWMATCH	(0x6000 | گهڻائي_وجهو) // show matching paren
#وصف_ڪريو گهڻائي_CONFIRM	0x7000	// ":confirm" prompt
#وصف_ڪريو گهڻائي_ALL	0xffff

#وصف_ڪريو گهڻائي_MAX_ڊيگهائي	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#وصف_ڪريو FORWARD			1
#وصف_ڪريو BACKWARD		(-1)
#وصف_ڪريو FORWARD_FILE		3
#وصف_ڪريو BACKWARD_FILE		(-3)

// return values for functions
#إذا !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#جيڪڏهن_پورو
#وصف_ڪريو FAIL			0
#وصف_ڪريو NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#وصف_ڪريو BF_RECOVERED	0x01	// buffer has been recovered
#وصف_ڪريو BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#وصف_ڪريو BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#وصف_ڪريو BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#وصف_ڪريو BF_NEW		0x10	// file didn't exist when editing started
#وصف_ڪريو BF_NEW_W	0x20	// Warned for BF_NEW and file created
#وصف_ڪريو BF_READERR	0x40	// got errors while reading the file
#وصف_ڪريو BF_DUMMY	0x80	// dummy buffer, only used internally
#وصف_ڪريو BF_PRESERVED	0x100	// ":preserve" was used
#وصف_ڪريو BF_SYN_سيٽ	0x200	// 'syntax' option was set
#وصف_ڪريو BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#وصف_ڪريو BF_WRITE_نقاب	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#وصف_ڪريو EXPAND_UNSUCCESSFUL	(-2)
#وصف_ڪريو EXPAND_OK		(-1)
#وصف_ڪريو EXPAND_NOTHING		0
#وصف_ڪريو EXPAND_COMMANDS		1
#وصف_ڪريو EXPAND_FILES		2
#وصف_ڪريو EXPAND_DIRECTORIES	3
#وصف_ڪريو EXPAND_SETTINGS		4
#وصف_ڪريو EXPAND_BOOL_SETTINGS	5
#وصف_ڪريو EXPAND_ٽيگ_فهرست		6
#وصف_ڪريو EXPAND_OLD_SETTING	7
#وصف_ڪريو EXPAND_HELP		8
#وصف_ڪريو EXPAND_BUFFERS		9
#وصف_ڪريو EXPAND_EVENTS		10
#وصف_ڪريو EXPAND_MENUS		11
#وصف_ڪريو EXPAND_SYNTAX		12
#وصف_ڪريو EXPAND_HIGHLIGHT	13
#وصف_ڪريو EXPAND_AUGROUP		14
#وصف_ڪريو EXPAND_USER_VARS	15
#وصف_ڪريو EXPAND_MAPPINGS		16
#وصف_ڪريو EXPAND_ٽيگ_فهرست_LISTFILES	17
#وصف_ڪريو EXPAND_FUNCTIONS	18
#وصف_ڪريو EXPAND_USER_FUNC	19
#وصف_ڪريو EXPAND_اظهار	20
#وصف_ڪريو EXPAND_MENUNAMES	21
#وصف_ڪريو EXPAND_USER_COMMANDS	22
#وصف_ڪريو EXPAND_USER_CMD_FLAGS	23
#وصف_ڪريو EXPAND_USER_NARGS	24
#وصف_ڪريو EXPAND_USER_COMPLETE	25
#وصف_ڪريو EXPAND_ENV_VARS		26
#وصف_ڪريو EXPAND_LANGUAGE		27
#وصف_ڪريو EXPAND_COLORS		28
#وصف_ڪريو EXPAND_COMPILER		29
#وصف_ڪريو EXPAND_USER_DEFINED	30
#وصف_ڪريو EXPAND_USER_LIST	31
#وصف_ڪريو EXPAND_SHELLCMD		32
#وصف_ڪريو EXPAND_CSCOPE		33
#وصف_ڪريو EXPAND_SIGN		34
#وصف_ڪريو EXPAND_PROFILE		35
#وصف_ڪريو EXPAND_BEHAVE		36
#وصف_ڪريو EXPAND_FILETYPE		37
#وصف_ڪريو EXPAND_FILES_اندريون_رستو	38
#وصف_ڪريو EXPAND_OWNSYNTAX	39
#وصف_ڪريو EXPAND_LOCALES		40
#وصف_ڪريو EXPAND_HISTORY		41
#وصف_ڪريو EXPAND_USER		42
#وصف_ڪريو EXPAND_SYNTIME		43
#وصف_ڪريو EXPAND_USER_ADDR_TYPE	44
#وصف_ڪريو EXPAND_PACKADD		45
#وصف_ڪريو EXPAND_MESSAGES		46
#وصف_ڪريو EXPAND_MAPCLEAR		47
#وصف_ڪريو EXPAND_ARGLIST		48
#وصف_ڪريو EXPAND_DIFF_BUFFERS	49
#وصف_ڪريو EXPAND_DISASSEMBLE	50
#وصف_ڪريو EXPAND_BREAKPOINT	51
#وصف_ڪريو EXPAND_SCRIPTNAMES	52
#وصف_ڪريو EXPAND_RUNTIME		53
#وصف_ڪريو EXPAND_STRING_SETTING	54
#وصف_ڪريو EXPAND_SETTING_ڪٽيو	55
#وصف_ڪريو EXPAND_ARGOPT		56
#وصف_ڪريو EXPAND_TERMINALOPT	57
#وصف_ڪريو EXPAND_KEYMAP		58
#وصف_ڪريو EXPAND_DIRS_اندريون_CDPATH	59
#وصف_ڪريو EXPAND_SHELLCMDLINE	60
#وصف_ڪريو EXPAND_FINDFUNC		61
#وصف_ڪريو EXPAND_HIGHLIGHT_گروهه  62
#وصف_ڪريو EXPAND_FILETYPECMD	63
#وصف_ڪريو EXPAND_نمونو_اندريون_BUF	64
#وصف_ڪريو EXPAND_RETAB		65
#وصف_ڪريو EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#وصف_ڪريو EXMODE_عام		1
#وصف_ڪريو EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#وصف_ڪريو WILD_FREE		1
#وصف_ڪريو WILD_EXPAND_FREE	2
#وصف_ڪريو WILD_EXPAND_KEEP	3
#وصف_ڪريو WILD_ايندڙ		4
#وصف_ڪريو WILD_PREV		5
#وصف_ڪريو WILD_ALL		6
#وصف_ڪريو WILD_LONGEST		7
#وصف_ڪريو WILD_ALL_KEEP		8
#وصف_ڪريو WILD_CANCEL		9
#وصف_ڪريو WILD_APPLY		10
#وصف_ڪريو WILD_PAGEUP		11
#وصف_ڪريو WILD_PAGEDOWN		12

#وصف_ڪريو WILD_LIST_NOTFOUND	    0x01
#وصف_ڪريو WILD_HOME_REPLACE	    0x02
#وصف_ڪريو WILD_USE_NL		    0x04
#وصف_ڪريو WILD_NO_BEEP		    0x08
#وصف_ڪريو WILD_جوڙيو_SLASH		    0x10
#وصف_ڪريو WILD_KEEP_ALL		    0x20
#وصف_ڪريو WILD_SILENT		    0x40
#وصف_ڪريو WILD_ESCAPE		    0x80
#وصف_ڪريو WILD_ICASE		    0x100
#وصف_ڪريو WILD_ALLLINKS		    0x200
#وصف_ڪريو WILD_IGNORE_COMPLETESLASH   0x400
#وصف_ڪريو WILD_NOERROR		    0x800  // sets EW_NOERROR
#وصف_ڪريو WILD_BUFLASTUSED	    0x1000
#وصف_ڪريو BUF_DIFF_FILTER		    0x2000
#وصف_ڪريو WILD_NOSELECT		    0x4000
#وصف_ڪريو WILD_MAY_EXPAND_نمونو	    0x8000
#وصف_ڪريو WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#وصف_ڪريو WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#وصف_ڪريو EW_DIR		0x01	// include directory names
#وصف_ڪريو EW_FILE		0x02	// include file names
#وصف_ڪريو EW_NOTFOUND	0x04	// include not found names
#وصف_ڪريو EW_ADDSLASH	0x08	// append slash to directory name
#وصف_ڪريو EW_KEEPALL	0x10	// keep all matches
#وصف_ڪريو EW_SILENT	0x20	// don't print "1 returned" from shell
#وصف_ڪريو EW_EXEC		0x40	// executable files
#وصف_ڪريو EW_رستو		0x80	// search in 'path' too
#وصف_ڪريو EW_ICASE	0x100	// ignore case
#وصف_ڪريو EW_NOERROR	0x200	// no error for bad regexp
#وصف_ڪريو EW_NOTWILD	0x400	// add match with literal name if exists
#وصف_ڪريو EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#وصف_ڪريو EW_ALLLINKS	0x1000	// also links not pointing to existing file
#وصف_ڪريو EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#وصف_ڪريو EW_DODOT	0x4000	// also files starting with a dot
#وصف_ڪريو EW_EMPTYOK	0x8000	// no matches is not an error
#وصف_ڪريو EW_NOTENV	0x10000	// do not expand environment variables
#وصف_ڪريو EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#وصف_ڪريو FINDFILE_FILE	0	// only files
#وصف_ڪريو FINDFILE_DIR	1	// only directories
#وصف_ڪريو FINDFILE_BOTH	2	// files and directories

#إذا defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_ويڪر())
# define TPL_LCOL()			tabpanel_leftcol()
#وإلا
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#جيڪڏهن_پورو

#وصف_ڪريو W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_ويڪر)
#جيڪڏهن_وصف_ٿيل FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_اوچائي)
#وإلا
# define W_WINROW(wp)	(wp->w_winrow)
#جيڪڏهن_پورو

// Values for the find_pattern_in_path() function args 'type' and 'action':
#وصف_ڪريو ڳوليو_ANY	1
#وصف_ڪريو ڳوليو_DEFINE	2
#وصف_ڪريو CHECK_رستو	3

#وصف_ڪريو ACTION_SHOW	1
#وصف_ڪريو ACTION_GOTO	2
#وصف_ڪريو ACTION_ورهايو	3
#وصف_ڪريو ACTION_SHOW_ALL	4
#وصف_ڪريو ACTION_EXPAND	5

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_حالتون	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_هتي	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_ميلاپ	0x40	// use match ID instead of item ID
# define HL_SKIPNL	0x80	// nextgroup can skip newlines
# define HL_SKIPWHITE	0x100	// nextgroup can skip white space
# define HL_SKIPEMPTY	0x200	// nextgroup can skip empty lines
# define HL_KEEPEND	0x400	// end match always kept
# define HL_EXCLUDENL	0x800	// exclude NL from match
# define HL_DISPLAY	0x1000	// only used for displaying, not syncing
# define HL_FOLD	0x2000	// define fold
# define HL_EXTEND	0x4000	// ignore a keepend
# define HL_MATCHCONT	0x8000	// match continued from previous line
# define HL_TRANS_CONT	0x10000 // transparent item without contains arg
# define HL_CONCEAL	0x20000 // can be concealed
# define HL_CONCEALENDS	0x40000 // can be concealed
# define HL_INCLUDED_TOPLEVEL 0x80000 // toplevel item in included syntax, allowed by contains=TOP
#جيڪڏهن_پورو

// Values for 'options' argument in do_search() and searchit()
#وصف_ڪريو ڳولا_REV    0x01  // go in reverse of previous dir.
#وصف_ڪريو ڳولا_ECHO   0x02  // echo the search command and handle options
#وصف_ڪريو ڳولا_نياپو    0x0c  // give messages (yes, it's not 0x04)
#وصف_ڪريو ڳولا_NFMSG  0x08  // give all messages except not found
#وصف_ڪريو ڳولا_OPT    0x10  // interpret optional flags
#وصف_ڪريو ڳولا_HIS    0x20  // put search pattern in history
#وصف_ڪريو ڳولا_پڇاڙي    0x40  // put cursor at end of match
#وصف_ڪريو ڳولا_NOOF   0x80  // don't add offset to position
#وصف_ڪريو ڳولا_شروعات 0x100  // start search without col offset
#وصف_ڪريو ڳولا_MARK  0x200  // set previous context mark
#وصف_ڪريو ڳولا_KEEP  0x400  // keep previous search pattern
#وصف_ڪريو ڳولا_PEEK  0x800  // peek for typed char, cancel search
#وصف_ڪريو ڳولا_کانو  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#وصف_ڪريو ڳوليو_IDENT	1	// find identifier (word)
#وصف_ڪريو ڳوليو_STRING	2	// find any string (WORD)
#وصف_ڪريو ڳوليو_EVAL	4	// include "->", "[]" and "."
#وصف_ڪريو ڳوليو_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#وصف_ڪريو FNAME_MESS	1	// give error message
#وصف_ڪريو FNAME_EXP	2	// expand to path
#وصف_ڪريو FNAME_HYP	4	// check for hypertext link
#وصف_ڪريو FNAME_INCL	8	// apply 'includeexpr'
#وصف_ڪريو FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#وصف_ڪريو FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#وصف_ڪريو GETF_SETMARK	0x01	// set pcmark before jumping
#وصف_ڪريو GETF_ALT	0x02	// jumping to alternate file (not buf num)
#وصف_ڪريو GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#وصف_ڪريو GETFILE_ERROR	    1	// normal error
#وصف_ڪريو GETFILE_NOT_WRITTEN 2	// "not written" error
#وصف_ڪريو GETFILE_SAME_FILE   0	// success, same file
#وصف_ڪريو GETFILE_OPEN_OTHER (-1)	// success, opened another file
#وصف_ڪريو GETFILE_اڻ_استعمال_ٿيل	    8
#وصف_ڪريو GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#وصف_ڪريو BLN_CURBUF	1	// may re-use curbuf for new buffer
#وصف_ڪريو BLN_LISTED	2	// put new buffer in buffer list
#وصف_ڪريو BLN_DUMMY	4	// allocating dummy buffer
#وصف_ڪريو BLN_NEW		8	// create a new buffer
#وصف_ڪريو BLN_NOOPT	16	// don't copy options to existing buffer
#وصف_ڪريو BLN_DUMMY_OK	32	// also find an existing dummy buffer
#وصف_ڪريو BLN_REUSE	64	// may re-use number from buf_reuse
#وصف_ڪريو BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#وصف_ڪريو ڪنجي_OPEN_FORW	0x101
#وصف_ڪريو ڪنجي_OPEN_پٺ	0x102
#وصف_ڪريو ڪنجي_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#وصف_ڪريو MAPTYPE_MAP		0
#وصف_ڪريو MAPTYPE_UNMAP		1
#وصف_ڪريو MAPTYPE_NOREMAP		2
#وصف_ڪريو MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#وصف_ڪريو REMAP_YES	0	// allow remapping
#وصف_ڪريو REMAP_NONE	(-1)	// no remapping
#وصف_ڪريو REMAP_SCRIPT	(-2)	// remap script-local mappings only
#وصف_ڪريو REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#وصف_ڪريو SHELL_FILTER	1	// filtering text
#وصف_ڪريو SHELL_EXPAND	2	// expanding wildcards
#وصف_ڪريو SHELL_COOKED	4	// set term to cooked mode
#وصف_ڪريو SHELL_DOOUT	8	// redirecting output
#وصف_ڪريو SHELL_SILENT	16	// don't print error returned by command
#وصف_ڪريو SHELL_READ	32	// read lines and insert into buffer
#وصف_ڪريو SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#وصف_ڪريو ڳنڍ_عام	0	// file or directory, check with mch_isdir()
#وصف_ڪريو ڳنڍ_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#وصف_ڪريو ڳنڍ_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#وصف_ڪريو READ_NEW	0x01	// read a file into a new buffer
#وصف_ڪريو READ_FILTER	0x02	// read filter output
#وصف_ڪريو READ_STDIN	0x04	// read from stdin
#وصف_ڪريو READ_بفر	0x08	// read from curbuf (converting stdin)
#وصف_ڪريو READ_DUMMY	0x10	// reading into a dummy buffer
#وصف_ڪريو READ_KEEP_UNDO	0x20	// keep undo info
#وصف_ڪريو READ_FIFO	0x40	// read from fifo or socket
#وصف_ڪريو READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#وصف_ڪريو READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#وصف_ڪريو INDENT_سيٽ	1	// set indent
#وصف_ڪريو INDENT_INC	2	// increase indent
#وصف_ڪريو INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#وصف_ڪريو FM_BACKWARD	0x01	// search backwards
#وصف_ڪريو FM_FORWARD	0x02	// search forwards
#وصف_ڪريو FM_BLOCKSTOP	0x04	// stop at start/end of block
#وصف_ڪريو FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#وصف_ڪريو DOBUF_GOTO	0	// go to specified buffer
#وصف_ڪريو DOBUF_ورهايو	1	// split window and go to specified buffer
#وصف_ڪريو DOBUF_UNLOAD	2	// unload specified buffer(s)
#وصف_ڪريو DOBUF_DEL	3	// delete specified buffer(s) from buflist
#وصف_ڪريو DOBUF_WIPE	4	// delete specified buffer(s) really
#وصف_ڪريو DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#وصف_ڪريو DOBUF_CURRENT	0	// "count" buffer from current buffer
#وصف_ڪريو DOBUF_پهريون	1	// "count" buffer from first buffer
#وصف_ڪريو DOBUF_آخري	2	// "count" buffer from last buffer
#وصف_ڪريو DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#وصف_ڪريو DOBUF_FORCEIT	1	// :cmd!
#وصف_ڪريو DOBUF_NOPOPUP	2	// skip popup window buffers
#وصف_ڪريو DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#وصف_ڪريو RE_ڳولا	0	// save/use pat in/from search_pattern
#وصف_ڪريو RE_SUBST	1	// save/use pat in/from subst_pattern
#وصف_ڪريو RE_BOTH		2	// save pat in both patterns
#وصف_ڪريو RE_آخري		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#وصف_ڪريو RE_MAGIC	1	// 'magic' option
#وصف_ڪريو RE_STRING	2	// match in string instead of buffer text
#وصف_ڪريو RE_STRICT	4	// don't allow [abc] without ]
#وصف_ڪريو RE_AUTO		8	// automatic engine selection

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_سيٽ	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_سيٽ | REX_USE)
#جيڪڏهن_پورو

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#وصف_ڪريو FPC_SAME	1	// both exist and are the same file.
#وصف_ڪريو FPC_DIFF	2	// both exist and are different files.
#وصف_ڪريو FPC_NOTX	4	// both don't exist.
#وصف_ڪريو FPC_DIFFX	6	// one of them doesn't exist.
#وصف_ڪريو FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#وصف_ڪريو ECMD_HIDE	0x01	// don't free the current buffer
#وصف_ڪريو ECMD_سيٽ_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#وصف_ڪريو ECMD_OLDBUF	0x04	// use existing buffer if it exists
#وصف_ڪريو ECMD_FORCEIT	0x08	// ! used in Ex command
#وصف_ڪريو ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#وصف_ڪريو ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#وصف_ڪريو ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#وصف_ڪريو ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#وصف_ڪريو ECMD_آخري	((linenr_T)-1)	// use last position in all files
#وصف_ڪريو ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#وصف_ڪريو DOCMD_VERBOSE	0x01	// included command in error message
#وصف_ڪريو DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#وصف_ڪريو DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#وصف_ڪريو DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#وصف_ڪريو DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#وصف_ڪريو DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#وصف_ڪريو DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#وصف_ڪريو BL_WHITE	1	// cursor on first non-white in the line
#وصف_ڪريو BL_SOL		2	// use 'sol' option
#وصف_ڪريو BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#وصف_ڪريو MFS_ALL		1	// also sync blocks with negative numbers
#وصف_ڪريو MFS_STOP	2	// stop syncing when a character is available
#وصف_ڪريو MFS_FLUSH	4	// flushed file to disk
#وصف_ڪريو MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#وصف_ڪريو BCO_ENTER	1	// going to enter the buffer
#وصف_ڪريو BCO_ALWAYS	2	// always copy the options
#وصف_ڪريو BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#وصف_ڪريو PUT_FIXINDENT	1	// make indent look nice
#وصف_ڪريو PUT_CURSEND	2	// leave cursor after end of new text
#وصف_ڪريو PUT_CURSLINE	4	// leave cursor on last line of new text
#وصف_ڪريو PUT_سٽ	8	// put register as lines
#وصف_ڪريو PUT_سٽ_ورهايو	16	// split line for linewise register
#وصف_ڪريو PUT_سٽ_FORWARD 32	// put linewise register below Visual sel.
#وصف_ڪريو PUT_بلاڪ_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#وصف_ڪريو SIN_بدليو	1	// call changed_bytes() when line changed
#وصف_ڪريو SIN_وجهو	2	// insert indent before existing text
#وصف_ڪريو SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#وصف_ڪريو INSCHAR_FORMAT	1	// force formatting
#وصف_ڪريو INSCHAR_DO_COM	2	// format comments
#وصف_ڪريو INSCHAR_CTRLV	4	// char typed just after CTRL-V
#وصف_ڪريو INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#وصف_ڪريو INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#وصف_ڪريو OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#وصف_ڪريو OPENLINE_DO_COM		0x02    // format comments
#وصف_ڪريو OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#وصف_ڪريو OPENLINE_MARKFIX	0x08    // fix mark positions
#وصف_ڪريو OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#وصف_ڪريو OPENLINE_FORMAT		0x20    // formatting long comment
#وصف_ڪريو OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#وصف_ڪريو HIST_CMD	0	// colon commands
#وصف_ڪريو HIST_ڳولا	1	// search commands
#وصف_ڪريو HIST_اظهار	2	// expressions (from entering = register)
#وصف_ڪريو HIST_داخلا	3	// input() lines
#وصف_ڪريو HIST_DEBUG	4	// debug commands
#وصف_ڪريو HIST_ڳڻپ	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#وصف_ڪريو BARTYPE_نسخو 1
#وصف_ڪريو BARTYPE_HISTORY 2
#وصف_ڪريو BARTYPE_رجسٽر 3
#وصف_ڪريو BARTYPE_MARK 4

#وصف_ڪريو VIMINFO_نسخو 4
#وصف_ڪريو VIMINFO_نسخو_WITH_HISTORY 2
#وصف_ڪريو VIMINFO_نسخو_WITH_REGISTERS 3
#وصف_ڪريو VIMINFO_نسخو_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#وصف_ڪريو DT_ٽيگ		1	// jump to newer position or same tag again
#وصف_ڪريو DT_ڪڍو		2	// jump to older position
#وصف_ڪريو DT_ايندڙ		3	// jump to next match of same tag
#وصف_ڪريو DT_PREV		4	// jump to previous match of same tag
#وصف_ڪريو DT_پهريون	5	// jump to first match of same tag
#وصف_ڪريو DT_آخري		6	// jump to first match of same tag
#وصف_ڪريو DT_SELECT	7	// jump to selection from list
#وصف_ڪريو DT_HELP		8	// like DT_TAG, but no wildcards
#وصف_ڪريو DT_JUMP		9	// jump to new tag or selection from list
#وصف_ڪريو DT_CSCOPE	10	// cscope find command (like tjump)
#وصف_ڪريو DT_LTAG		11	// tag using location list
#وصف_ڪريو DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#وصف_ڪريو ٽيگ_HELP	1	// only search for help tags
#وصف_ڪريو ٽيگ_نالا	2	// only return name of tag
#وصف_ڪريو	ٽيگ_REGEXP	4	// use tag pattern as regexp
#وصف_ڪريو	ٽيگ_NOIC	8	// don't always ignore case
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
# define ٽيگ_CSCOPE	16	// cscope tag
#جيڪڏهن_پورو
#وصف_ڪريو ٽيگ_VERBOSE	32	// message verbosity
#وصف_ڪريو ٽيگ_INS_COMP	64	// Currently doing insert completion
#وصف_ڪريو ٽيگ_KEEP_LANG	128	// keep current language
#وصف_ڪريو ٽيگ_NO_TAGFUNC	256	// do not use 'tagfunc'

#وصف_ڪريو ٽيگ_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#وصف_ڪريو VIM_GENERIC	0
#وصف_ڪريو VIM_ERROR	1
#وصف_ڪريو VIM_WARNING	2
#وصف_ڪريو VIM_INFO	3
#وصف_ڪريو VIM_QUESTION	4
#وصف_ڪريو VIM_آخري_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#وصف_ڪريو VIM_YES		2
#وصف_ڪريو VIM_NO		3
#وصف_ڪريو VIM_CANCEL	4
#وصف_ڪريو VIM_ALL		5
#وصف_ڪريو VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#وصف_ڪريو WSP_گنجائش	0x01	// require enough room
#وصف_ڪريو WSP_VERT	0x02	// split/equalize vertically
#وصف_ڪريو WSP_HOR		0x04	// equalize horizontally
#وصف_ڪريو WSP_چوٽي		0x08	// window at top-left of shell
#وصف_ڪريو WSP_BOT		0x10	// window at bottom-right of shell
#وصف_ڪريو WSP_HELP	0x20	// creating the help window
#وصف_ڪريو WSP_هيٺ	0x40	// put new window below/right
#وصف_ڪريو WSP_ABOVE	0x80	// put new window above/left
#وصف_ڪريو WSP_NEWLOC	0x100	// don't copy location list
#وصف_ڪريو WSP_FORCE_گنجائش	0x200	// ignore "not enough room" errors
#وصف_ڪريو WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#وصف_ڪريو RESIZE_VERT	1	// resize vertically
#وصف_ڪريو RESIZE_HOR	2	// resize horizontally
#وصف_ڪريو RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#وصف_ڪريو CCGD_AW		1	// do autowrite if buffer was changed
#وصف_ڪريو CCGD_MULTWIN	2	// check also when several wins for the buf
#وصف_ڪريو CCGD_FORCEIT	4	// ! used
#وصف_ڪريو CCGD_ALLBUF	8	// may write all buffers
#وصف_ڪريو CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#وصف_ڪريو OPT_FREE	0x01	// free old value if it was allocated
#وصف_ڪريو OPT_GLOBAL	0x02	// use global value
#وصف_ڪريو OPT_LOCAL	0x04	// use local value
#وصف_ڪريو OPT_MODELINE	0x08	// option in modeline
#وصف_ڪريو OPT_WINONLY	0x10	// only set window-local options
#وصف_ڪريو OPT_NOWIN	0x20	// don't set window-local options
#وصف_ڪريو OPT_ONECOLUMN	0x40	// list options one per line
#وصف_ڪريو OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#وصف_ڪريو OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#وصف_ڪريو DLG_BUTTON_SEP	'\n'
#وصف_ڪريو DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#وصف_ڪريو NO_SCREEN	2	// no screen updating yet
#وصف_ڪريو NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#وصف_ڪريو SEA_NONE	0	// don't use dialog
#وصف_ڪريو SEA_DIALOG	1	// use dialog when possible
#وصف_ڪريو SEA_QUIT	2	// quit editing the file
#وصف_ڪريو SEA_RECOVER	3	// recover the file
#وصف_ڪريو SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#وصف_ڪريو MIN_مٽا_سٽا_صفحو_سائيز 1048
#وصف_ڪريو MAX_مٽا_سٽا_صفحو_سائيز 50000

// Special values for current_sctx.sc_sid.
#وصف_ڪريو SID_MODELINE	(-1)	// when using a modeline
#وصف_ڪريو SID_CMDARG	(-2)	// for "--cmd" argument
#وصف_ڪريو SID_CARG	(-3)	// for "-c" argument
#وصف_ڪريو SID_ENV		(-4)	// for sourcing environment variable
#وصف_ڪريو SID_ERROR	(-5)	// option was reset because of an error
#وصف_ڪريو SID_NONE	(-6)	// don't set scriptID
#وصف_ڪريو SID_WINLAYOUT	(-7)	// changing window size

/*
 * Events for autocommands. Must be kept in sync with "event_tab".
 */
تعداد auto_event
{
    EVENT_BUFADD = 0,		// after adding a buffer to the buffer list
    EVENT_BUFCREATE,		// UNUSED: BufCreate == BufAdd
    EVENT_BUFDELETE,		// deleting a buffer from the buffer list
    EVENT_BUFENTER,		// after entering a buffer
    EVENT_BUFFILEPOST,		// after renaming a buffer
    EVENT_BUFFILEPRE,		// before renaming a buffer
    EVENT_BUFHIDDEN,		// just after buffer becomes hidden
    EVENT_BUFLEAVE,		// before leaving a buffer
    EVENT_BUFNEW,		// after creating any buffer
    EVENT_BUFNEWFILE,		// when creating a buffer for a new file
    EVENT_BUFREAD,		// UNUSED: BufRead == BufReadPost
    EVENT_BUFREADCMD,		// read buffer using command
    EVENT_BUFREADPOST,		// after reading a buffer
    EVENT_BUFREADPRE,		// before reading a buffer
    EVENT_BUFUNLOAD,		// just before unloading a buffer
    EVENT_BUFWINENTER,		// after showing a buffer in a window
    EVENT_BUFWINLEAVE,		// just after buffer removed from window
    EVENT_BUFWIPEOUT,		// just before really deleting a buffer
    EVENT_BUFWRITE,		// UNUSED: BufWrite == BufWritePost
    EVENT_BUFWRITECMD,		// write buffer using command
    EVENT_BUFWRITEPOST,		// after writing a buffer
    EVENT_BUFWRITEPRE,		// before writing a buffer
    EVENT_CMDLINECHANGED,	// command line was modified
    EVENT_CMDLINEENTER,		// after entering the command line
    EVENT_CMDLINELEAVEPRE,	// just before leaving the command line
    EVENT_CMDLINELEAVE,		// before leaving the command line
    EVENT_CMDUNDEFINED,		// command undefined
    EVENT_CMDWINENTER,		// after entering the cmdline window
    EVENT_CMDWINLEAVE,		// before leaving the cmdline window
    EVENT_COLORSCHEME,		// after loading a colorscheme
    EVENT_COLORSCHEMEPRE,	// before loading a colorscheme
    EVENT_COMPLETECHANGED,	// after completion popup menu changed
    EVENT_COMPLETEDONE,		// after finishing insert complete
    EVENT_COMPLETEDONEPRE,	// idem, before clearing info
    EVENT_CURSORHOLD,		// cursor in same position for a while
    EVENT_CURSORHOLDI,		// idem, in Insert mode
    EVENT_CURSORMOVED,		// cursor was moved
    EVENT_CURSORMOVEDC,		// cursor was moved in Command line mode
    EVENT_CURSORMOVEDI,		// cursor was moved in Insert mode
    EVENT_DIFFUPDATED,		// after diffs were updated
    EVENT_DIRCHANGED,		// after user changed directory
    EVENT_DIRCHANGEDPRE,	// before directory changes
    EVENT_ENCODINGCHANGED,	// after changing the 'encoding' option
    EVENT_EXITPRE,		// before exiting
    EVENT_FILEAPPENDCMD,	// append to a file using command
    EVENT_FILEAPPENDPOST,	// after appending to a file
    EVENT_FILEAPPENDPRE,	// before appending to a file
    EVENT_FILECHANGEDRO,	// before first change to read-only file
    EVENT_FILECHANGEDSHELL,	// after shell command that changed file
    EVENT_FILECHANGEDSHELLPOST,	// after (not) reloading changed file
    EVENT_FILEENCODING,		// UNUSED: FileEncoding == EncodingChanged
    EVENT_FILEREADCMD,		// read from a file using command
    EVENT_FILEREADPOST,		// after reading a file
    EVENT_FILEREADPRE,		// before reading a file
    EVENT_FILETYPE,		// new file type detected (user defined)
    EVENT_FILEWRITECMD,		// write to a file using command
    EVENT_FILEWRITEPOST,	// after writing a file
    EVENT_FILEWRITEPRE,		// before writing a file
    EVENT_FILTERREADPOST,	// after reading from a filter
    EVENT_FILTERREADPRE,	// before reading from a filter
    EVENT_FILTERWRITEPOST,	// after writing to a filter
    EVENT_FILTERWRITEPRE,	// before writing to a filter
    EVENT_FOCUSGAINED,		// got the focus
    EVENT_FOCUSLOST,		// lost the focus to another app
    EVENT_FUNCUNDEFINED,	// if calling a function which doesn't exist
    EVENT_GUIENTER,		// after starting the GUI
    EVENT_GUIFAILED,		// after starting the GUI failed
    EVENT_INSERTCHANGE,		// when changing Insert/Replace mode
    EVENT_INSERTCHARPRE,	// before inserting a char
    EVENT_INSERTENTER,		// when entering Insert mode
    EVENT_INSERTLEAVE,		// just after leaving Insert mode
    EVENT_INSERTLEAVEPRE,	// just before leaving Insert mode
    EVENT_KEYINPUTPRE,		// before key input
    EVENT_MENUPOPUP,		// just before popup menu is displayed
    EVENT_MODECHANGED,		// after changing the mode
    EVENT_OPTIONSET,		// option was set
    EVENT_QUICKFIXCMDPOST,	// after :make, :grep etc.
    EVENT_QUICKFIXCMDPRE,	// before :make, :grep etc.
    EVENT_QUITPRE,		// before :quit
    EVENT_REMOTEREPLY,		// upon string reception from a remote vim
    EVENT_SAFESTATE,		// going to wait for a character
    EVENT_SAFESTATEAGAIN,	// still waiting for a character
    EVENT_SESSIONLOADPOST,	// after loading a session file
    EVENT_SESSIONLOADPRE,	// before loading a session file
    EVENT_SESSIONWRITEPOST,	// after writing a session file
    EVENT_SHELLCMDPOST,		// after ":!cmd"
    EVENT_SHELLFILTERPOST,	// after ":1,2!cmd", ":w !cmd", ":r !cmd".
    EVENT_SIGUSR1,		// after the SIGUSR1 signal
    EVENT_SOURCECMD,		// sourcing a Vim script using command
    EVENT_SOURCEPOST,		// after sourcing a Vim script
    EVENT_SOURCEPRE,		// before sourcing a Vim script
    EVENT_SPELLFILEMISSING,	// spell file missing
    EVENT_STDINREADPOST,	// after reading from stdin
    EVENT_STDINREADPRE,		// before reading from stdin
    EVENT_SWAPEXISTS,		// found existing swap file
    EVENT_SYNTAX,		// syntax selected
    EVENT_TABCLOSED,		// after closing a tab page
    EVENT_TABCLOSEDPRE,		// before closing a tab page
    EVENT_TABENTER,		// after entering a tab page
    EVENT_TABLEAVE,		// before leaving a tab page
    EVENT_TABNEW,		// when entering a new tab page
    EVENT_TERMCHANGED,		// after changing 'term'
    EVENT_TERMINALOPEN,		// after a terminal buffer was created
    EVENT_TERMINALWINOPEN,	// after a terminal buffer was created and
				// entering its window
    EVENT_TERMRESPONSE,		// after setting "v:termresponse"
    EVENT_TERMRESPONSEALL,	// after setting terminal response vars
    EVENT_TEXTCHANGED,		// text was modified not in Insert mode
    EVENT_TEXTCHANGEDI,		// text was modified in Insert mode
    EVENT_TEXTCHANGEDP,		// TextChangedI with popup menu visible
    EVENT_TEXTCHANGEDT,		// text was modified in Terminal mode
    EVENT_TEXTPUTPOST,		// after some text was put
    EVENT_TEXTPUTPRE,		// before some text was put
    EVENT_TEXTYANKPOST,		// after some text was yanked
    EVENT_USER,			// user defined autocommand
    EVENT_VIMENTER,		// after starting Vim
    EVENT_VIMLEAVE,		// before exiting Vim
    EVENT_VIMLEAVEPRE,		// before exiting Vim and writing .viminfo
    EVENT_VIMRESIZED,		// after Vim window was resized
    EVENT_VIMRESUME,		// after Vim is resumed
    EVENT_VIMSUSPEND,		// before Vim is suspended
    EVENT_WINCLOSED,		// after closing a window
    EVENT_WINENTER,		// after entering a window
    EVENT_WINLEAVE,		// before leaving a window
    EVENT_WINNEW,		// after creating a new window
    EVENT_WINNEWPRE,		// before creating a new window
    EVENT_WINRESIZED,		// after a window was resized
    EVENT_WINSCROLLED,		// after a window was scrolled or resized

    انگ_EVENTS,			// MUST be the last one
};

تعريفنوع تعداد auto_event event_T;

/*
 * Values for index in highlight_attr[].
 * When making changes, also update HL_FLAGS below!
 * And update the default value of 'highlight': HIGHLIGHT_INIT in optiondefs.h
 */
تعريفنوع تعداد
{
    HLF_8 = 0	    // Meta & special keys listed with ":map", text that is
		    // displayed different from what it is
    , HLF_EOB	    // after the last line in the buffer
    , HLF_AT	    // @ characters at end of screen, characters that
		    // don't really exist in the text
    , HLF_D	    // directories in CTRL-D listing
    , HLF_E	    // error messages
    , HLF_H	    // obsolete, ignored
    , HLF_I	    // incremental search
    , HLF_L	    // last search string
    , HLF_LC	    // last search string under cursor
    , HLF_M	    // "--More--" message
    , HLF_CM	    // Mode (e.g., "-- INSERT --")
    , HLF_N	    // line number for ":number" and ":#" commands
    , HLF_LNA	    // LineNrAbove
    , HLF_LNB	    // LineNrBelow
    , HLF_CLN	    // current line number
    , HLF_CLS	    // current line sign column
    , HLF_CLF	    // current line fold
    , HLF_R	    // return to continue message and yes/no questions
    , HLF_S	    // status lines
    , HLF_SNC	    // status lines of not-current windows
    , HLF_C	    // column to separate vertically split windows
    , HLF_CNC	    // column to separate vertically split non-current windows
    , HLF_T	    // Titles for output from ":set all", ":autocmd" etc.
    , HLF_V	    // Visual mode
    , HLF_VNC	    // Visual mode, autoselecting and not clipboard owner
    , HLF_W	    // warning messages
    , HLF_WM	    // Wildmenu highlight
    , HLF_FL	    // Folded line
    , HLF_FC	    // Fold column
    , HLF_جوڙيو	    // Added diff line
    , HLF_CHD	    // Changed diff line
    , HLF_DED	    // Deleted diff line
    , HLF_TXD	    // Text Changed in changed diff line
    , HLF_TXA	    // Text Added in changed diff line
    , HLF_CONCEAL   // Concealed text
    , HLF_SC	    // Sign column
    , HLF_SPB	    // SpellBad
    , HLF_SPC	    // SpellCap
    , HLF_SPR	    // SpellRare
    , HLF_SPL	    // SpellLocal
    , HLF_PNI	    // popup menu normal item
    , HLF_PSI	    // popup menu selected item
    , HLF_PMNI	    // popup menu matched text in normal item
    , HLF_PMSI	    // popup menu matched text in selected item
    , HLF_PNK	    // popup menu normal item "kind"
    , HLF_PSK	    // popup menu selected item "kind"
    , HLF_PNX	    // popup menu normal item "menu" (extra text)
    , HLF_PSX	    // popup menu selected item "menu" (extra text)
    , HLF_PSB	    // popup menu scrollbar
    , HLF_PST	    // popup menu scrollbar thumb
    , HLF_PMB	    // popup menu border
    , HLF_PMS	    // popup menu shadow
    , HLF_ڪڍو	    // popup window body
    , HLF_POPB	    // popup window border
    , HLF_POPT	    // popup window title
    , HLF_TP	    // tabpage line
    , HLF_TPS	    // tabpage line selected
    , HLF_TPF	    // tabpage line filler
    , HLF_CUC	    // 'cursorcolumn'
    , HLF_CUL	    // 'cursorline'
    , HLF_MC	    // 'colorcolumn'
    , HLF_QFL	    // quickfix window line currently selected
    , HLF_ST	    // status lines of terminal windows
    , HLF_STNC	    // status lines of not-current terminal windows
    , HLF_نياپو	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_ڳڻپ	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#وصف_ڪريو HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
		  'n', 'a', 'b', 'N', 'G', 'O', 'r', 's', 'S', 'c', '|', 't', 'v', 'V', \
		  'w', 'W', 'f', 'F', 'A', 'C', 'D', 'T', 'E', '-', '>', \
		  'B', 'P', 'R', 'L', \
		  '+', '=', 'k', '<','[', ']', '{', '}', 'x', 'X', 'j', 'H', \
		  'p', 'J', 'Q', \
		  '*', '#', '_', '!', '.', 'o', 'q', \
		  'z', 'Z', 'g', \
		  '%', '^', '&', 'I', '('}

/*
 * Values for behaviour in spell_move_to
 */
تعريفنوع تعداد
{
    SMT_ALL = 0		    // Move to "all" words
    , SMT_BAD		    // Move to "bad" words only
    , SMT_RARE		    // Move to "rare" words only
} smt_T;
/*
 * Boolean constants
 */
#جيڪڏهن_وصف_نه_ٿيل TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#جيڪڏهن_پورو

#وصف_ڪريو MAYBE	2	    // sometimes used for a variant on TRUE

#وصف_ڪريو LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_منزل_LOG_THIS إذا (ch_log_output == FALSE) ch_log_output = TRUE;
#وإلا
// no logging support
# define MAY_WANT_منزل_LOG_THIS
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل UINT32_T
تعريفنوع UINT32_TYPEDEF UINT32_T;
#جيڪڏهن_پورو

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#وصف_ڪريو OP_NOP		0	// no pending operation
#وصف_ڪريو OP_DELETE	1	// "d"  delete operator
#وصف_ڪريو OP_YANK		2	// "y"  yank operator
#وصف_ڪريو OP_CHANGE	3	// "c"  change operator
#وصف_ڪريو OP_LSHIFT	4	// "<"  left shift operator
#وصف_ڪريو OP_RSHIFT	5	// ">"  right shift operator
#وصف_ڪريو OP_FILTER	6	// "!"  filter operator
#وصف_ڪريو OP_TILDE	7	// "g~" switch case operator
#وصف_ڪريو OP_INDENT	8	// "="  indent operator
#وصف_ڪريو OP_FORMAT	9	// "gq" format operator
#وصف_ڪريو OP_COLON	10	// ":"  colon operator
#وصف_ڪريو OP_مٿانهون	11	// "gU" make upper case operator
#وصف_ڪريو OP_هيٺاهون	12	// "gu" make lower case operator
#وصف_ڪريو OP_JOIN		13	// "J"  join operator, only for Visual mode
#وصف_ڪريو OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#وصف_ڪريو OP_ROT13	15	// "g?" rot-13 encoding
#وصف_ڪريو OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#وصف_ڪريو OP_وجهو	17	// "I"  Insert column, only for Visual mode
#وصف_ڪريو OP_APPEND	18	// "A"  Append column, only for Visual mode
#وصف_ڪريو OP_FOLD		19	// "zf" define a fold
#وصف_ڪريو OP_FOLDOPEN	20	// "zo" open folds
#وصف_ڪريو OP_FOLDOPENREC	21	// "zO" open folds recursively
#وصف_ڪريو OP_FOLDCLOSE	22	// "zc" close folds
#وصف_ڪريو OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#وصف_ڪريو OP_FOLDDEL	24	// "zd" delete folds
#وصف_ڪريو OP_FOLDDELREC	25	// "zD" delete folds recursively
#وصف_ڪريو OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#وصف_ڪريو OP_FUNCTION	27	// "g@" call 'operatorfunc'
#وصف_ڪريو OP_NR_جوڙيو	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#وصف_ڪريو OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#وصف_ڪريو MCHAR	0		// character-wise movement/register
#وصف_ڪريو MLINE	1		// line-wise movement/register
#وصف_ڪريو MBLOCK	2		// block-wise register

#وصف_ڪريو MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#وصف_ڪريو MIN_COLUMNS	12	// minimal columns for screen
#وصف_ڪريو MIN_سٽون	2	// minimal lines for screen
#وصف_ڪريو MIN_CMDHEIGHT	1	// minimal height for command line
#وصف_ڪريو حالت_اوچائي_2	1	// default height of a status line under a
				//دري
#جيڪڏهن_وصف_ٿيل FEAT_MENU		// height of a status line under a window
# define WINBAR_اوچائي(wp)	(wp)->w_winbar_اوچائي
# define VISIBLE_اوچائي(wp)	((wp)->w_اوچائي + (wp)->w_winbar_اوچائي)
#وإلا
# define WINBAR_اوچائي(wp)	0
# define VISIBLE_اوچائي(wp)	(wp)->w_اوچائي
#جيڪڏهن_پورو
#وصف_ڪريو QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#جيڪڏهن_وصف_نه_ٿيل CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#جيڪڏهن_پورو

#وصف_ڪريو LSIZE	    512		// max. size of a line in the tags file

#وصف_ڪريو IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#وصف_ڪريو DIALOG_نياپو_سائيز 1000	// buffer size for dialog_msg()

#وصف_ڪريو نياپو_BUF_LEN 480	// length of buffer for small messages
#وصف_ڪريو نياپو_BUF_CLEN  (نياپو_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#وصف_ڪريو FOLD_متن_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#وصف_ڪريو TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#وصف_ڪريو MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#وصف_ڪريو MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#وصف_ڪريو UNDO_HASH_سائيز 32

#جيڪڏهن_وصف_ٿيل HAVE_FCNTL_H
# include <fcntl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل ٻٽو_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#وإلا
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#جيڪڏهن_پورو

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#جيڪڏهن_وصف_ٿيل __CYGWIN__
# define O_EXTRA    O_ٻٽو
#وإلا
# define O_EXTRA    0
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل O_NOFOLLOW
# define O_NOFOLLOW 0
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#جيڪڏهن_پورو

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#وصف_ڪريو ALLOC_ONE(type)  (type *)alloc(حجم(type))
#وصف_ڪريو ALLOC_ONE_ID(type, id)  (type *)alloc_id(حجم(type), id)
#وصف_ڪريو ALLOC_MULT(type, ڳڻپ)  (type *)alloc(حجم(type) * (ڳڻپ))
#وصف_ڪريو ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(حجم(type))
#وصف_ڪريو ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(حجم(type), id)
#وصف_ڪريو ALLOC_CLEAR_MULT(type, ڳڻپ)  (type *)alloc_clear(حجم(type) * (ڳڻپ))
#وصف_ڪريو LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(حجم(type), FALSE)
#وصف_ڪريو LALLOC_CLEAR_MULT(type, ڳڻپ)  (type *)lalloc_clear(حجم(type) * (ڳڻپ), FALSE)
#وصف_ڪريو LALLOC_MULT(type, ڳڻپ)  (type *)lalloc(حجم(type) * (ڳڻپ), FALSE)

#جيڪڏهن_وصف_ٿيل HAVE_MEMSET
# define vim_memset(ptr, c, سائيز)   memset((ptr), (c), (سائيز))
#وإلا
فراغ *vim_memset(فراغ *, صحيح, size_t);
#جيڪڏهن_پورو
#وصف_ڪريو CLEAR_ميدان(ميدان)  vim_memset(&(ميدان), 0, حجم(ميدان))
#وصف_ڪريو CLEAR_اشارو(ptr)  vim_memset((ptr), 0, حجم(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#وصف_ڪريو STRLEN(s)	    لڙي_ڊيگهه((حرف *)(s))
#وصف_ڪريو STRCPY(d, s)	    strcpy((حرف *)(d), (حرف *)(s))
#وصف_ڪريو STRNCPY(d, s, n)    strncpy((حرف *)(d), (حرف *)(s), (n))
#وصف_ڪريو STRCMP(d, s)	    strcmp((حرف *)(d), (حرف *)(s))
#وصف_ڪريو STRNCMP(d, s, n)    لڙي_ڀيٽ((حرف *)(d), (حرف *)(s), (n))
#جيڪڏهن_وصف_ٿيل HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((حرف *)(d), (حرف *)(s))
#وإلا
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((حرف *)(d), (حرف *)(s))
# وإلا
#  define STRICMP(d, s)	    vim_stricmp((حرف *)(d), (حرف *)(s))
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((حرف *)(d), (حرف *)(s))
#وإلا
# define STRCOLL(d, s)     strcmp((حرف *)(d), (حرف *)(s))
#جيڪڏهن_پورو

// Like strcpy() but allows overlapped source and destination.
#وصف_ڪريو STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#جيڪڏهن_وصف_ٿيل HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((حرف *)(d), (حرف *)(s), (n))
#وإلا
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((حرف *)(d), (حرف *)(s), (n))
# وإلا
#  define STRNICMP(d, s, n) vim_strnicmp((حرف *)(d), (حرف *)(s), (n))
# endif
#جيڪڏهن_پورو

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#وصف_ڪريو MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)MAXCOL)
#وصف_ڪريو MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)(n))
#وصف_ڪريو MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#وصف_ڪريو STRCAT(d, s)	    strcat((حرف *)(d), (حرف *)(s))
#وصف_ڪريو STRNCAT(d, s, n)    strncat((حرف *)(d), (حرف *)(s), (n))

#جيڪڏهن_وصف_ٿيل HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((حرف *)(s), (حرف *)(cs))
#جيڪڏهن_پورو

#وصف_ڪريو ٻاهريون_STR(s)		    ٻاهريون_str((char_u *)(s))
#وصف_ڪريو ٻاهريون_STR_NF(s)		    ٻاهريون_str_nf((char_u *)(s))

#جيڪڏهن_وصف_ٿيل FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.اندريون_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.اندريون_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_ٻٽو_انگ		    (gui.اندريون_use || p_tgc)
# وإلا
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_ٻٽو_انگ		    gui.اندريون_use
# endif
#وإلا
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_ٻٽو_انگ		    p_tgc
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#وإلا
# define IS_CTERM		    (t_colors > 1)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#جيڪڏهن_پورو

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#جيڪڏهن_وصف_ٿيل HAVE_STRERROR
# define PERROR(نياپو)		    (فراغ)semsg("%s: %s", (حرف *)(نياپو), strerror(errno))
#وإلا
# define PERROR(نياپو)		    do_perror(نياپو)
#جيڪڏهن_پورو

تعريفنوع ڊگهو	linenr_T;		// line number type
تعريفنوع صحيح	colnr_T;		// column number type
تعريفنوع بنانشان ننڍو disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#جيڪڏهن_وصف_ٿيل __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#وإلا
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#جيڪڏهن_پورو

#وصف_ڪريو SHOWCMD_کانن_ڳڻپ 10			// columns needed by shown command

تعريفنوع فراغ	    *vim_acl_T;		// dummy to pass an ACL to a function

#جيڪڏهن_وصف_نه_ٿيل mch_memmove
# define mch_memmove(منزل, from, len) memmove((حرف*)(منزل), (حرف*)(from), (len))
#جيڪڏهن_پورو

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#وصف_ڪريو fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#وصف_ڪريو fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#إذا defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_داخلا_BUF
#جيڪڏهن_پورو

#إذا !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, ڳڻپ) vim_read((fd), (buf), (ڳڻپ))
# define write_eintr(fd, buf, ڳڻپ) vim_write((fd), (buf), (ڳڻپ))
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, ڳڻپ)   اقرأ((fd), (حرف *)(buf), (بنانشان صحيح)(ڳڻپ))
# define vim_write(fd, buf, ڳڻپ)  اكتب((fd), (حرف *)(buf), (بنانشان صحيح)(ڳڻپ))
#وإلا
# define vim_read(fd, buf, ڳڻپ)   اقرأ((fd), (حرف *)(buf), (ڳڻپ))
# define vim_write(fd, buf, ڳڻپ)  اكتب((fd), (حرف *)(buf), (ڳڻپ))
#جيڪڏهن_پورو

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#وصف_ڪريو HL_ATTR(n)	highlight_attr[(صحيح)(n)]
#وصف_ڪريو رڪن_STR(n)	رڪن_لڙيون[(صحيح)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#جيڪڏهن_وصف_نه_ٿيل EXTERN
# define EXTERN خارجي
# define INIT(x)
# define INIT2(a, b)
# define INIT3(a, b, c)
# define INIT4(a, b, c, d)
# define INIT5(a, b, c, d, e)
# define INIT6(a, b, c, d, e, f)
#وإلا
# ifndef INIT
#  define INIT(x) x
#  define INIT2(a, b) = {a, b}
#  define INIT3(a, b, c) = {a, b, c}
#  define INIT4(a, b, c, d) = {a, b, c, d}
#  define INIT5(a, b, c, d, e) = {a, b, c, d, e}
#  define INIT6(a, b, c, d, e, f) = {a, b, c, d, e, f}
#  define DO_INIT
# endif
#جيڪڏهن_پورو

#وصف_ڪريو MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#وصف_ڪريو MB_MAXBYTES	30

#إذا (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
تعريفنوع LARGE_INTEGER proftime_T;
#  define PROF_وقت_BLANK "           "
#  define PROF_TOTALS_سرو "count  total (s)   self (s)"
# وإلا
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  إذا defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
تعريفنوع بنية timespec proftime_T;
#   define PROF_GET_وقت(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_وقت_FORMAT "%3ld.%09ld"
#   define PROF_وقت_BLANK "              "
#   define PROF_TOTALS_سرو "count     total (s)      self (s)"
#  وإلا
تعريفنوع بنية timeval proftime_T;
#   define PROF_GET_وقت(tm) gettimeofday(tm, عدم)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_وقت_FORMAT "%3ld.%06ld"
#   define PROF_وقت_BLANK "           "
#   define PROF_TOTALS_سرو "count  total (s)   self (s)"
#  endif
# endif
#وإلا
تعريفنوع صحيح proftime_T;	    // dummy for function prototypes
#جيڪڏهن_پورو

// Type of compilation passed to compile_def_function()
تعريفنوع تعداد {
    CT_NONE,	    // use df_instr
    CT_PROFILE,	    // use df_instr_prof
    CT_DEBUG	    // use df_instr_debug, overrules CT_PROFILE
} compiletype_T;

/*
 * When compiling with 32 bit Perl time_t is 32 bits in the Perl code but 64
 * bits elsewhere.  That causes memory corruption.  Define time_T and use it
 * for global variables to avoid that.
 */
#جيڪڏهن_وصف_ٿيل MSWIN
تعريفنوع __time64_t  وقت_T;
#وإلا
تعريفنوع time_t	    وقت_T;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _WIN64
تعريفنوع __int64 sock_T;
#وإلا
تعريفنوع صحيح sock_T;
#جيڪڏهن_پورو

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#جيڪڏهن_پورو

#إذا defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#جيڪڏهن_پورو

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#ضمن "option.h"	// options and default values

#ضمن "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#ضمن "structs.h"	// defines many structures
#ضمن "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#ضمن "alloc.h"

// Values for "do_profiling".
#وصف_ڪريو PROF_NONE	0	// profiling not started
#وصف_ڪريو PROF_YES	1	// profiling busy
#وصف_ڪريو PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#وصف_ڪريو MOUSE_کاٻو	0x00
#وصف_ڪريو MOUSE_وچ	0x01
#وصف_ڪريو MOUSE_ساڄو	0x02
#وصف_ڪريو MOUSE_RELEASE	0x03

// bit masks for modifiers:
#وصف_ڪريو MOUSE_سرڪايو	0x04
#وصف_ڪريو MOUSE_ALT	0x08
#وصف_ڪريو MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#وصف_ڪريو MOUSE_4	0x100	// scroll wheel down
#وصف_ڪريو MOUSE_5	0x200	// scroll wheel up

#وصف_ڪريو MOUSE_X1	0x300 // Mouse-button X1 (6th)
#وصف_ڪريو MOUSE_X2	0x400 // Mouse-button X2

#وصف_ڪريو MOUSE_6	0x500	// scroll wheel left
#وصف_ڪريو MOUSE_7	0x600	// scroll wheel right

#وصف_ڪريو MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#وصف_ڪريو MOUSE_DRAG_XTERM   0x40

#وصف_ڪريو MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#وصف_ڪريو MOUSEWHEEL_هيٺيون		0x60

// Lowest button code for extra mouse buttons 8-11
#وصف_ڪريو MOUSESIDEBUTTONS_هيٺيون		0xa0

#وصف_ڪريو MOUSE_CLICK_نقاب	0x03

#وصف_ڪريو انگ_MOUSE_CLICKS(ڪوڊ) \
    (((بنانشان)((ڪوڊ) & 0xC0) >> 6) + 1)

#وصف_ڪريو سيٽ_انگ_MOUSE_CLICKS(ڪوڊ, انگ) \
    ((ڪوڊ) = ((ڪوڊ) & 0x3f) | ((((انگ) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#وصف_ڪريو MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#وصف_ڪريو اندريون_UNKNOWN		0
#وصف_ڪريو اندريون_بفر		1
#وصف_ڪريو اندريون_حالت_سٽ		2	// on status or command line
#وصف_ڪريو اندريون_SEP_سٽ		4	// on vertical separator line
#وصف_ڪريو اندريون_OTHER_WIN		8	// in other window but can't go there
#وصف_ڪريو CURSOR_MOVED		0x100
#وصف_ڪريو MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#وصف_ڪريو MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#وصف_ڪريو MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#وصف_ڪريو MOUSE_FOCUS		0x01	// need to stay in this window
#وصف_ڪريو MOUSE_MAY_VIS		0x02	// may start Visual mode
#وصف_ڪريو MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#وصف_ڪريو MOUSE_SETPOS		0x08	// only set current mouse position
#وصف_ڪريو MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#وصف_ڪريو MOUSE_RELEASED		0x20	// button was released

#إذا defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وقت_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#جيڪڏهن_پورو


// defines for eval_vars()
#وصف_ڪريو VALID_رستو		1
#وصف_ڪريو VALID_سر		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#وصف_ڪريو VV_ڳڻپ	0
#وصف_ڪريو VV_COUNT1	1
#وصف_ڪريو VV_PREVCOUNT	2
#وصف_ڪريو VV_ERRMSG	3
#وصف_ڪريو VV_WARNINGMSG	4
#وصف_ڪريو VV_STATUSMSG	5
#وصف_ڪريو VV_SHELL_ERROR	6
#وصف_ڪريو VV_THIS_SESSION	7
#وصف_ڪريو VV_نسخو	8
#وصف_ڪريو VV_LNUM		9
#وصف_ڪريو VV_TERMRESPONSE	10
#وصف_ڪريو VV_FNAME	11
#وصف_ڪريو VV_LANG		12
#وصف_ڪريو VV_LC_وقت	13
#وصف_ڪريو VV_CTYPE	14
#وصف_ڪريو VV_CC_FROM	15
#وصف_ڪريو VV_CC_منزل	16
#وصف_ڪريو VV_FNAME_اندريون	17
#وصف_ڪريو VV_FNAME_ٻاهريون	18
#وصف_ڪريو VV_FNAME_NEW	19
#وصف_ڪريو VV_FNAME_DIFF	20
#وصف_ڪريو VV_CMDARG	21
#وصف_ڪريو VV_FOLDSTART	22
#وصف_ڪريو VV_FOLDEND	23
#وصف_ڪريو VV_FOLDDASHES	24
#وصف_ڪريو VV_FOLDLEVEL	25
#وصف_ڪريو VV_PROGNAME	26
#وصف_ڪريو VV_SEND_SERVER	27
#وصف_ڪريو VV_DYING	28
#وصف_ڪريو VV_EXCEPTION	29
#وصف_ڪريو VV_THROWPOINT	30
#وصف_ڪريو VV_REG		31
#وصف_ڪريو VV_CMDBANG	32
#وصف_ڪريو VV_INSERTMODE	33
#وصف_ڪريو VV_VAL		34
#وصف_ڪريو VV_ڪنجي		35
#وصف_ڪريو VV_PROFILING	36
#وصف_ڪريو VV_FCS_REASON	37
#وصف_ڪريو VV_FCS_CHOICE	38
#وصف_ڪريو VV_BEVAL_BUFNR	39
#وصف_ڪريو VV_BEVAL_WINNR	40
#وصف_ڪريو VV_BEVAL_WINID	41
#وصف_ڪريو VV_BEVAL_LNUM	42
#وصف_ڪريو VV_BEVAL_کانو	43
#وصف_ڪريو VV_BEVAL_متن	44
#وصف_ڪريو VV_SCROLLSTART	45
#وصف_ڪريو VV_SWAPNAME	46
#وصف_ڪريو VV_SWAPCHOICE	47
#وصف_ڪريو VV_SWAPCOMMAND	48
#وصف_ڪريو VV_CHAR		49
#وصف_ڪريو VV_MOUSE_WIN	50
#وصف_ڪريو VV_MOUSE_WINID	51
#وصف_ڪريو VV_MOUSE_LNUM   52
#وصف_ڪريو VV_MOUSE_کانو	53
#وصف_ڪريو VV_OP		54
#وصف_ڪريو VV_SEARCHFORWARD 55
#وصف_ڪريو VV_HLSEARCH	56
#وصف_ڪريو VV_OLDFILES	57
#وصف_ڪريو VV_WINDOWID	58
#وصف_ڪريو VV_PROGPATH	59
#وصف_ڪريو VV_COMPLETED_شي 60
#وصف_ڪريو VV_OPTION_NEW   61
#وصف_ڪريو VV_OPTION_OLD   62
#وصف_ڪريو VV_OPTION_OLDLOCAL 63
#وصف_ڪريو VV_OPTION_OLDGLOBAL 64
#وصف_ڪريو VV_OPTION_COMMAND 65
#وصف_ڪريو VV_OPTION_TYPE  66
#وصف_ڪريو VV_ERRORS	67
#وصف_ڪريو VV_FALSE	68
#وصف_ڪريو VV_TRUE		69
#وصف_ڪريو VV_NONE		70
#وصف_ڪريو VV_NULL		71
#وصف_ڪريو VV_NUMBERMAX	72
#وصف_ڪريو VV_NUMBERMIN	73
#وصف_ڪريو VV_NUMBERSIZE	74
#وصف_ڪريو VV_VIM_DID_ENTER 75
#وصف_ڪريو VV_TESTING	76
#وصف_ڪريو VV_TYPE_عدد	77
#وصف_ڪريو VV_TYPE_STRING	78
#وصف_ڪريو VV_TYPE_FUNC	79
#وصف_ڪريو VV_TYPE_LIST	80
#وصف_ڪريو VV_TYPE_DICT	81
#وصف_ڪريو VV_TYPE_FLOAT	82
#وصف_ڪريو VV_TYPE_BOOL	83
#وصف_ڪريو VV_TYPE_NONE	84
#وصف_ڪريو VV_TYPE_ڪم	85
#وصف_ڪريو VV_TYPE_CHANNEL	86
#وصف_ڪريو VV_TYPE_BLOB	87
#وصف_ڪريو VV_TYPE_CLASS	88
#وصف_ڪريو VV_TYPE_OBJECT	89
#وصف_ڪريو VV_TERMRFGRESP	90
#وصف_ڪريو VV_TERMRBGRESP	91
#وصف_ڪريو VV_TERMU7RESP	92
#وصف_ڪريو VV_TERMSTYLERESP 93
#وصف_ڪريو VV_TERMBLINKRESP 94
#وصف_ڪريو VV_EVENT	95
#وصف_ڪريو VV_VERSIONLONG	96
#وصف_ڪريو VV_ECHOSPACE	97
#وصف_ڪريو VV_ARGV		98
#وصف_ڪريو VV_COLLATE      99
#وصف_ڪريو VV_EXITING	100
#وصف_ڪريو VV_COLORNAMES   101
#وصف_ڪريو VV_SIZEOFINT	102
#وصف_ڪريو VV_SIZEOFLONG	103
#وصف_ڪريو VV_SIZEOFPOINTER 104
#وصف_ڪريو VV_MAXCOL	105
#وصف_ڪريو VV_PYTHON3_نسخو 106
#وصف_ڪريو VV_TYPE_TYPEALIAS 107
#وصف_ڪريو VV_TYPE_ENUM	  108
#وصف_ڪريو VV_TYPE_ENUMVALUE 109
#وصف_ڪريو VV_STACKTRACE	110
#وصف_ڪريو VV_TYPE_TUPLE	111
#وصف_ڪريو VV_WAYLAND_DISPLAY 112
#وصف_ڪريو VV_CLIPMETHOD 113
#وصف_ڪريو VV_TERMDA1 114
#وصف_ڪريو VV_TERMOSC 115
#وصف_ڪريو VV_VIM_DID_INIT		116
#وصف_ڪريو VV_CLIPPROVIDERS 117
#وصف_ڪريو VV_IMAGEBACKEND 118
#وصف_ڪريو VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#وصف_ڪريو VVAL_FALSE	0L	// VAR_BOOL
#وصف_ڪريو VVAL_TRUE	1L	// VAR_BOOL
#وصف_ڪريو VVAL_NONE	2L	// VAR_SPECIAL
#وصف_ڪريو VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#وصف_ڪريو VAR_TYPE_عدد	    0
#وصف_ڪريو VAR_TYPE_STRING	    1
#وصف_ڪريو VAR_TYPE_FUNC	    2
#وصف_ڪريو VAR_TYPE_LIST	    3
#وصف_ڪريو VAR_TYPE_DICT	    4
#وصف_ڪريو VAR_TYPE_FLOAT	    5
#وصف_ڪريو VAR_TYPE_BOOL	    6
#وصف_ڪريو VAR_TYPE_NONE	    7
#وصف_ڪريو VAR_TYPE_ڪم	    8
#وصف_ڪريو VAR_TYPE_CHANNEL    9
#وصف_ڪريو VAR_TYPE_BLOB	    10
#وصف_ڪريو VAR_TYPE_INSTR	    11
#وصف_ڪريو VAR_TYPE_CLASS	    12
#وصف_ڪريو VAR_TYPE_OBJECT	    13
#وصف_ڪريو VAR_TYPE_TYPEALIAS  14
#وصف_ڪريو VAR_TYPE_ENUM	    15
#وصف_ڪريو VAR_TYPE_ENUMVALUE  16
#وصف_ڪريو VAR_TYPE_TUPLE	    17

#وصف_ڪريو DICT_MAXNEST 100	// maximum nesting of lists and dicts

#وصف_ڪريو TABSTOP_MAX 9999

#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
تعريفنوع تعداد {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD

// VIM_ATOM_NAME is the older Vim-specific selection type for X11.  Still
// supported for when a mix of Vim versions is used. VIMENC_ATOM_NAME includes
// the encoding to support Vims using different 'encoding' values.
# define VIM_ATOM_NAME "_VIM_TEXT"
# define VIMENC_ATOM_NAME "_VIMENC_TEXT"

// These are used for the GTK4 GUI, since GTK4 only supports conforming mime
// types, see gui_gtk4_cb.c for more information.
# define VIM_MIMETYPE_NAME "application/x-vim-text"
# define VIMENC_MIMETYPE_NAME "application/x-vim-enc-text"

// Selection states for modeless selection
# define SELECT_CLEARED		0
# define SELECT_اندريون_ترقي	1
# define SELECT_پورو		2

# define SELECT_گهڻائي_CHAR	0
# define SELECT_گهڻائي_لفظ	1
# define SELECT_گهڻائي_سٽ	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
تعريفنوع بنية
{
    صحيح		موجود;	// Is clipboard available?
    صحيح		owned;		// Flag: do we own the selection?
    pos_T	شروعات;		// Start of selected area
    pos_T	پڇاڙي;		// End of selected area
    صحيح		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_صف;
    short_u	origin_شروعات_کانو;
    short_u	origin_پڇاڙي_کانو;
    short_u	لفظ_شروعات_کانو;
    short_u	لفظ_پڇاڙي_کانو;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_کانو;
    short_u	max_کانو;
    short_u	min_صف;
    short_u	max_صف;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	گهڻائي;		// Select by char, word, or line.

# إذا defined(FEAT_GUI_X11) || defined(FEAT_XCLIPBOARD)
    Atom	sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# إذا defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
    GdkAtom     gtk_sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# إذا defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    int_u	format;		// Vim's own special clipboard format
    int_u	format_raw;	// Vim's raw text clipboard format
# endif
# ifdef FEAT_GUI_HAIKU
    // No clipboard at the moment. TODO?
# endif
} Clipboard_T;
#وإلا
تعريفنوع صحيح Clipboard_T;	// This is required for the prototypes.
#جيڪڏهن_پورو

// Use 64-bit stat structure on MS-Windows.
#جيڪڏهن_وصف_ٿيل MSWIN
تعريفنوع بنية _stat64 stat_T;
#وإلا
تعريفنوع بنية stat stat_T;
#جيڪڏهن_پورو

تعريفنوع بنية soundcb_S soundcb_T;

#إذا (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(لکو, fmt_idx, arg_idx)))
#وإلا
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#جيڪڏهن_پورو

#إذا defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#وإلا
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#جيڪڏهن_پورو

تعريفنوع تعداد {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_ميلاپ,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
تعريفنوع تعداد {
    PASTE_وجهو,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_گهڻائي_T;

// Argument for flush_buffers().
تعريفنوع تعداد {
    FLUSH_گهٽتر,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_داخلا		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
تعريفنوع تعداد {
    USEPOPUP_NONE,
    USEPOPUP_عام,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
تعريفنوع تعداد {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_ڍير,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#وصف_ڪريو ENABLE_STL_گهڻائي_MULTI_NL

// Argument for build_stl_str_hl_local().
تعريفنوع تعداد {
    STL_گهڻائي_SINGLE,	    // Does not accept line breaks "%@"
    STL_گهڻائي_MULTI,	    // Accept line breaks "%@"
    STL_گهڻائي_GET_RENDERED_اوچائي,   // Just get stl rendered height
#جيڪڏهن_وصف_ٿيل ENABLE_STL_گهڻائي_MULTI_NL
    STL_گهڻائي_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#جيڪڏهن_پورو
} stl_گهڻائي_T;

// Return value of match_keyprotocol()
تعريفنوع تعداد {
    KEYPROTOCOL_NONE,
    KEYPROTOCOL_MOK2,
    KEYPROTOCOL_KITTY,
    KEYPROTOCOL_FAIL
} keyprot_T;

// errors for when calling a function
تعريفنوع تعداد {
    FCERR_NONE,		// no error
    FCERR_UNKNOWN,	// unknown function
    FCERR_TOOMANY,	// too many arguments
    FCERR_TOOFEW,	// too few arguments
    FCERR_SCRIPT,	// missing script context
    FCERR_DICT,		// missing dict
    FCERR_OTHER,	// another kind of error
    FCERR_DELETED,	// function was deleted
    FCERR_NOTMETHOD,	// function cannot be used as a method
    FCERR_FAILED,	// error while executing the function
} funcerror_T;

/*
 * Array indexes used for cp_text[].
 */
تعريفنوع تعداد {
    CPT_ABBR,		// "abbr"
    CPT_قسم,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_ڳڻپ,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
تعريفنوع حرف *(*opt_did_سيٽ_cb_T)(optset_T *دليلون);

/*
 * Type for the callback function that is invoked when expanding possible
 * string option values during cmdline completion.
 *
 * Strings in returned matches will be managed and freed by caller.
 *
 * Returns OK if the expansion succeeded (numMatches and matches have to be
 * set). Otherwise returns FAIL.
 *
 * Note: If returned FAIL or *numMatches is 0, *matches will NOT be freed by
 * caller.
 */
تعريفنوع صحيح (*opt_expand_cb_T)(optexpand_T *دليلون, صحيح *انگ_Matches, char_u ***matches);

// Flags for assignment functions.
#وصف_ڪريو سونپيو_VAR	0     // ":var" (nothing special)
#وصف_ڪريو سونپيو_FINAL	0x01  // ":final"
#وصف_ڪريو سونپيو_CONST	0x02  // ":const"
#وصف_ڪريو سونپيو_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#وصف_ڪريو سونپيو_DECL	0x08  // may declare variable if it does not exist
#وصف_ڪريو سونپيو_UNPACK	0x10  // using [a, b] = list
#وصف_ڪريو سونپيو_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#وصف_ڪريو سونپيو_FOR_LOOP 0x40 // assigning to loop variable
#وصف_ڪريو سونپيو_INIT	0x80 // not assigning a value, just a declaration
#وصف_ڪريو سونپيو_UPDATE_بلاڪ_ID 0x100  // update sav_block_id
#وصف_ڪريو سونپيو_COMPOUND_OP 0x200  // compound operator e.g. "+="

#ضمن "ex_cmds.h"	    // Ex command defines
#ضمن "spell.h"	    // spell checking stuff

#ضمن "proto.h"	    // function prototypes

// This has to go after the include of proto.h, as proto/gui.pro declares
// functions of these names. The declarations would break if the defines had
// been seen at that stage.  But it must be before globals.h, where error_ga
// is declared.
#إذا !defined(MSWIN) && !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_HAIKU) \
	&& !defined(FEAT_GUI_GTK) && !defined(PROTO)
# define mch_errmsg(str)	fprintf(stderr, "%s", (str))
# define display_errors()	fflush(stderr)
# define mch_نياپو(str)		لکو("%s", (str))
#وإلا
# define USE_MCH_ERRMSG
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#جيڪڏهن_پورو

#إذا defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_داخلا_طريقو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define عدد_ويڪر(x) 7
#جيڪڏهن_پورو

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#إذا !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#جيڪڏهن_پورو

#ضمن "globals.h"	    // global variables and messages
#ضمن "errors.h"	    // error messages

#جيڪڏهن_وصف_نه_ٿيل PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ساكن مضمن صحيح
اندريون_vim9script(فراغ)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    أرجع (current_sctx.sc_نسخو == SCRIPT_نسخو_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#جيڪڏهن_پورو

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#إذا defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#جيڪڏهن_پورو

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
# ifdef BACKSLASH_اندريون_FILENAME
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*.*)\t*.*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVB code (*.bas, *.frm)\t*.bas;*.frm\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# وإلا
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*)\t*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*)\t*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*)\t*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# endif
# define BROWSE_SAVE 1	    // flag for do_browse()
# define BROWSE_DIR 2	    // flag for do_browse()
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _MSC_VER
// Avoid useless warning "conversion from X to Y of greater size".
# pragma warning(disable : 4312)
// Avoid warning for old style function declarators
# pragma warning(disable : 4131)
// Avoid warning for conversion to type with smaller range
# pragma warning(disable : 4244)
// Avoid warning for conversion to larger size
# pragma warning(disable : 4306)
// Avoid warning for unreferenced formal parameter
# pragma warning(disable : 4100)
// Avoid warning for differs in indirection to slightly different base type
# pragma warning(disable : 4057)
// Avoid warning for constant conditional expression
# pragma warning(disable : 4127)
// Avoid warning for assignment within conditional
# pragma warning(disable : 4706)
#جيڪڏهن_پورو

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#إذا defined(MEM_PROFILE)
# define vim_realloc(ptr, سائيز)  mem_realloc((ptr), (سائيز))
#وإلا
# define vim_realloc(ptr, سائيز)  realloc((ptr), (سائيز))
#جيڪڏهن_پورو

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#وصف_ڪريو MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#وصف_ڪريو MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#وصف_ڪريو ENC_8_ٻٽو_انگ	0x01
#وصف_ڪريو ENC_DBCS	0x02
#وصف_ڪريو ENC_UNICODE	0x04

#وصف_ڪريو ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#وصف_ڪريو ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#وصف_ڪريو ENC_2BYTE	0x40	    // Unicode: UCS-2
#وصف_ڪريو ENC_4BYTE	0x80	    // Unicode: UCS-4
#وصف_ڪريو ENC_2_لفظ	0x100	    // Unicode: UTF-16

#وصف_ڪريو ENC_LATIN1	0x200	    // Latin1
#وصف_ڪريو ENC_LATIN9	0x400	    // Latin9
#وصف_ڪريو ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#جيڪڏهن_وصف_ٿيل USE_ICONV
# ifndef EILSEQ
#  define EILSEQ 123
# endif
# ifdef DYNAMIC_ICONV
// On Win32 iconv.dll is dynamically loaded.
#  define ICONV_ERRNO (*iconv_errno())
#  define ICONV_E2BIG  7
#  define ICONV_EINVAL 22
#  define ICONV_EILSEQ 42
# وإلا
#  define ICONV_ERRNO errno
#  define ICONV_E2BIG  E2BIG
#  define ICONV_EINVAL EINVAL
#  define ICONV_EILSEQ EILSEQ
# endif
#جيڪڏهن_پورو

#وصف_ڪريو SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.اندريون_use ? (مزدوج)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.اندريون_use ? (مزدوج)(p) / gui.scale : (p))
#وإلا
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.اندريون_use) ? gui_mch_get_display() : xterm_dpy)
# وإلا
#  define X_DISPLAY	(gui.اندريون_use ? gui.dpy : xterm_dpy)
# endif
#وإلاإذا defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.اندريون_use) ? gui_mch_get_display() : (Display *)عدم)
# وإلا
#  define X_DISPLAY	gui.dpy
# endif
#وإلاإذا defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#وإلا
# define X_DISPLAY	(Display *)عدم
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
# إذا !GTK_CHECK_نسخو(2,14,0)
#  define gtk_widget_get_دري(wid)	((wid)->دري_2)
#  define gtk_plug_get_socket_دري(wid)	((wid)->socket_دري)
#  define gtk_selection_معلومات_get_معلومات(sel)	((sel)->معلومات)
#  define gtk_selection_معلومات_get_معلومات_type(sel)	((sel)->type)
#  define gtk_selection_معلومات_get_format(sel)	((sel)->format)
#  define gtk_selection_معلومات_get_ڊيگهائي(sel)	((sel)->ڊيگهائي)
#  define gtk_adjustment_سيٽ_هيٺاهون(adj, هيٺيون) \
    افعل { (adj)->هيٺاهون = هيٺيون; } طالما (0)
#  define gtk_adjustment_سيٽ_مٿانهون(adj, مٿي) \
    افعل { (adj)->مٿانهون_2 = مٿي; } طالما (0)
#  define gtk_adjustment_سيٽ_صفحو_سائيز(adj, سائيز) \
    افعل { (adj)->صفحو_سائيز = سائيز; } طالما (0)
#  define gtk_adjustment_سيٽ_صفحو_increment(adj, inc) \
    افعل { (adj)->صفحو_increment = inc; } طالما (0)
#  define gtk_adjustment_سيٽ_قدم_increment(adj, inc) \
    افعل { (adj)->قدم_increment = inc; } طالما (0)
# endif
# إذا !GTK_CHECK_نسخو(2,16,0)
#  define gtk_selection_معلومات_get_selection(sel)	((sel)->selection)
# endif
# إذا !GTK_CHECK_نسخو(2,18,0)
#  define gtk_widget_get_ورهاست(wid, alloc) \
    افعل { *(alloc) = (wid)->ورهاست; } طالما (0)
#  define gtk_widget_سيٽ_ورهاست(wid, alloc) \
    افعل { (wid)->ورهاست = *(alloc); } طالما (0)
#  define gtk_widget_get_has_دري(wid)	!GTK_WIDGET_NO_دري(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_سيٽ_دري(wid, win) \
    افعل { (wid)->دري_2 = (win); } طالما (0)
#  define gtk_widget_سيٽ_can_default(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_سيٽ_FLAGS(wid, GTK_CAN_DEFAULT); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } طالما (0)
#  define gtk_widget_سيٽ_can_focus(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_سيٽ_FLAGS(wid, GTK_CAN_FOCUS); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } طالما (0)
#  define gtk_widget_سيٽ_visible(wid, vis) \
    افعل { إذا (vis) \
	    { gtk_widget_show(wid); } \
	وإلا \
	    { gtk_widget_hide(wid); } } طالما (0)
# endif
# إذا !GTK_CHECK_نسخو(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_سيٽ_mapped(wid, map) \
    افعل { إذا (map) \
	    { GTK_WIDGET_سيٽ_FLAGS(wid, GTK_MAPPED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } طالما (0)
#  define gtk_widget_سيٽ_realized(wid, rea) \
    افعل { إذا (rea) \
	    { GTK_WIDGET_سيٽ_FLAGS(wid, GTK_REALIZED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } طالما (0)
# endif
#جيڪڏهن_پورو

#إذا defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# إذا GTK_CHECK_نسخو(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل FEAT_NETBEANS_INTG
# undef NBDEBUG
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#وإلا
# define nbdebug(a)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل اندريون_PERL_FILE
  /*
   * Avoid clashes between Perl and Vim namespace.
   */
# undef STRLEN
# undef FF
# undef OP_DELETE
# undef OP_JOIN
  // remove MAX and MIN, included by glib.h, redefined by sys/param.h
# ifdef MAX
#  undef MAX
# endif
# ifdef MIN
#  undef MIN
# endif
  // We use _() for gettext(), Perl uses it for function prototypes...
# ifdef _
#  undef _
# endif
# ifdef DEBUG
#  undef DEBUG
# endif
# ifdef _DEBUG
#  undef _DEBUG
# endif
# ifdef instr
#  undef instr
# endif
  // bool may cause trouble on some old versions of Mac OS X but is required
  // on a few other systems and for Perl
# إذا (defined(MACOS_X) && !defined(MAC_OS_X_نسخو_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#جيڪڏهن_پورو

// values for vim_handle_signal() that are not a signal
#وصف_ڪريو SIGNAL_بلاڪ	(-1)
#وصف_ڪريو SIGNAL_UNBLOCK  (-2)
#إذا !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#جيڪڏهن_پورو

// flags for skip_vimgrep_pat()
#وصف_ڪريو VGR_GLOBAL	1
#وصف_ڪريو VGR_NOJUMP	2
#وصف_ڪريو VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#وصف_ڪريو BAD_REPLACE	'?'	// replace it with '?' (default)
#وصف_ڪريو BAD_KEEP	(-1)	// leave it
#وصف_ڪريو BAD_DROP	(-2)	// erase it

// last argument for do_source()
#وصف_ڪريو DOSO_NONE	0
#وصف_ڪريو DOSO_VIMRC	1	// loading vimrc file
#وصف_ڪريو DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#وصف_ڪريو VIF_WANT_INFO	    1	// load non-mark info
#وصف_ڪريو VIF_WANT_MARKS	    2	// load file marks
#وصف_ڪريو VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#وصف_ڪريو VIF_FORCEIT	    8	// overwrite info already read
#وصف_ڪريو VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#وصف_ڪريو BFA_DEL		 1	// buffer is going to be deleted
#وصف_ڪريو BFA_WIPE	 2	// buffer is going to be wiped out
#وصف_ڪريو BFA_KEEP_UNDO	 4	// do not free undo information
#وصف_ڪريو BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#وصف_ڪريو MSCR_هيٺ_ڏانهن	0	// DOWN must be FALSE
#وصف_ڪريو MSCR_مٿي		1
#وصف_ڪريو MSCR_کاٻو	(-1)
#وصف_ڪريو MSCR_ساڄو	(-2)

#وصف_ڪريو KEYLEN_PART_ڪنجي (-1)	// keylen value for incomplete key-code
#وصف_ڪريو KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#وصف_ڪريو KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#وصف_ڪريو FILEINFO_OK	     0
#وصف_ڪريو FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#وصف_ڪريو FILEINFO_READ_FAIL   2	// CreateFile() failed
#وصف_ڪريو FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#وصف_ڪريو SOPT_BOOL	0x01	// Boolean option
#وصف_ڪريو SOPT_انگ	0x02	// Number option
#وصف_ڪريو SOPT_STRING	0x04	// String option
#وصف_ڪريو SOPT_GLOBAL	0x08	// Option has global value
#وصف_ڪريو SOPT_WIN	0x10	// Option has window-local value
#وصف_ڪريو SOPT_BUF	0x20	// Option has buffer-local value
#وصف_ڪريو SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#وصف_ڪريو SREQ_GLOBAL	0	// Request global option
#وصف_ڪريو SREQ_WIN	1	// Request window-local option
#وصف_ڪريو SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#وصف_ڪريو GREG_NO_اظهار	1	// Do not allow expression register
#وصف_ڪريو GREG_اظهار_SRC	2	// Return expression itself for "=" register
#وصف_ڪريو GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#وصف_ڪريو AUTOLOAD_CHAR '#'

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
# define MAX_OPEN_CHANNELS 10
#وإلا
# define MAX_OPEN_CHANNELS 0
#جيڪڏهن_پورو

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#وإلا
# define MAX_CLIENT_CHANNELS 0
#جيڪڏهن_پورو

#إذا defined(MSWIN)
# define MAX_NAMED_PIPE_سائيز 65535
#جيڪڏهن_پورو

// Options for json_encode() and json_decode.
#وصف_ڪريو JSON_JS		1   // use JS instead of JSON
#وصف_ڪريو JSON_NO_NONE	2   // v:none item not allowed
#وصف_ڪريو JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#وصف_ڪريو DIP_ALL	    0x01	// all matches, not just the first one
#وصف_ڪريو DIP_DIR	    0x02	// find directories instead of files.
#وصف_ڪريو DIP_ERR	    0x04	// give an error message when none found.
#وصف_ڪريو DIP_شروعات   0x08	// also use "start" directory in 'packpath'
#وصف_ڪريو DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#وصف_ڪريو DIP_NORTP   0x20	// do not use 'runtimepath'
#وصف_ڪريو DIP_NOAFTER 0x40	// skip "after" directories
#وصف_ڪريو DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#وصف_ڪريو سڀ_کان_هيٺ_WIN_ID 1000

// Used by the garbage collector.
#وصف_ڪريو COPYID_INC 2
#وصف_ڪريو COPYID_نقاب (~0x1)

// Values for trans_function_name() argument:
#وصف_ڪريو TFN_INT		0x01	// internal function name OK
#وصف_ڪريو TFN_QUIET	0x02	// no error messages
#وصف_ڪريو TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#وصف_ڪريو TFN_NO_DEREF	0x08	// do not dereference a Funcref
#وصف_ڪريو TFN_READ_ONLY	0x10	// will not change the var
#وصف_ڪريو TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#وصف_ڪريو TFN_COMPILING	0x40	// only used for GLV_COMPILING
#وصف_ڪريو TFN_NEW_FUNC	0x80	// defining a new function
#وصف_ڪريو TFN_سونپيو_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#وصف_ڪريو TFN_اندريون_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#وصف_ڪريو GLV_QUIET	TFN_QUIET	// no error messages
#وصف_ڪريو GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#وصف_ڪريو GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#وصف_ڪريو GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#وصف_ڪريو GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#وصف_ڪريو GLV_سونپيو_WITH_OP TFN_سونپيو_WITH_OP // assignment with operator
#وصف_ڪريو GLV_PREFER_FUNC	0x10000		// prefer function above variable
#وصف_ڪريو GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#وصف_ڪريو DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#وصف_ڪريو FLEN_FIXED 40

// flags for find_name_end()
#وصف_ڪريو FNE_INCL_BR	1	// include [] in name
#وصف_ڪريو FNE_CHECK_شروعات	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#إذا (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#جيڪڏهن_پورو

// stat macros
#جيڪڏهن_وصف_نه_ٿيل S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# وإلا
#  define S_ISDIR(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# وإلا
#  define S_ISREG(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# وإلا
#  define S_ISBLK(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# وإلا
#  define S_ISSOCK(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# وإلا
#  define S_ISFIFO(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# وإلا
#  define S_ISCHR(m)	0
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# وإلا
#  define S_ISLNK(m)	0
# endif
#جيڪڏهن_پورو

#إذا defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وقت_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), عدم)
# define ELAPSED_FUNC(v) elapsed(&(v))
تعريفنوع بنية timeval elapsed_T;
ڊگهو elapsed(بنية timeval *شروعات_tv);
#وإلاإذا defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_ڳڻپ()
# define ELAPSED_FUNC(v) elapsed(v)
تعريفنوع DWORD elapsed_T;
# ifndef PROTO
ڊگهو elapsed(DWORD شروعات_tick);
# endif
#جيڪڏهن_پورو

// Replacement for nchar used by nv_replace().
#وصف_ڪريو REPLACE_CR_NCHAR    (-1)
#وصف_ڪريو REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#وصف_ڪريو رڪن_شروعات_NOJOB	1
#وصف_ڪريو رڪن_شروعات_FORCEIT	2
#وصف_ڪريو رڪن_شروعات_SYSTEM	4

// Used for icon/title save and restore.
#وصف_ڪريو SAVE_RESTORE_TITLE	1
#وصف_ڪريو SAVE_RESTORE_ICON	2
#وصف_ڪريو SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#وصف_ڪريو APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#وصف_ڪريو APC_SUBSTITUTE		2   // text is replaced, not inserted
#وصف_ڪريو APC_INDENT		4   // changing indent

#وصف_ڪريو CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#وصف_ڪريو REPTERM_FROM_PART	1
#وصف_ڪريو REPTERM_DO_LT		2
#وصف_ڪريو REPTERM_SPECIAL		4
#وصف_ڪريو REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#وصف_ڪريو FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#وصف_ڪريو FSK_KEEP_X_ڪنجي	0x02	// don't translate xHome to Home key
#وصف_ڪريو FSK_اندريون_STRING	0x04	// TRUE in string, double quote is escaped
#وصف_ڪريو FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#وصف_ڪريو FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#وصف_ڪريو READDIR_ترتيب_ڏيو_NONE	0  // do not sort
#وصف_ڪريو READDIR_ترتيب_ڏيو_BYTE	1  // sort by byte order (strcmp), default
#وصف_ڪريو READDIR_ترتيب_ڏيو_IC		2  // sort ignoring case (strcasecmp)
#وصف_ڪريو READDIR_ترتيب_ڏيو_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#وصف_ڪريو MCH_DELAY_IGNOREINPUT	1
#وصف_ڪريو MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#وصف_ڪريو EVAL_VAR_VERBOSE	1   // may give error message
#وصف_ڪريو EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#وصف_ڪريو EVAL_VAR_IMPORT		4   // may return special variable for import
#وصف_ڪريو EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#وصف_ڪريو FUZZY_ميلاپ_MAX_LEN	1024    // max characters that can be matched
#وصف_ڪريو FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#وصف_ڪريو ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#وصف_ڪريو UC_بفر	1	// -buffer: local to current buffer
#وصف_ڪريو UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#وصف_ڪريو UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#وصف_ڪريو VSE_NONE	0
#وصف_ڪريو VSE_SHELL	1	// escape for a shell command
#وصف_ڪريو VSE_بفر	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#وصف_ڪريو FFED_IS_GLOBAL	1	// "g:" was used
#وصف_ڪريو FFED_NO_GLOBAL	2	// only check for script-local functions

#وصف_ڪريو MAX_LSHIFT_بٽ (varnumber_T)((حجم(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#وصف_ڪريو CF_CLASS	1	// inside a class
#وصف_ڪريو CF_INTERFACE	2	// inside an interface
#وصف_ڪريو CF_ABSTRACT_طريقو	4	// inside an abstract class

// Flags used by getvcol()
#وصف_ڪريو GETVCOL_پڇاڙي_EXCL_LBR	1
#وصف_ڪريو GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#وصف_ڪريو GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#جيڪڏهن_وصف_ٿيل VMS
# define رستو_ESC_WILDCARDS	"*?%"
#وإلا
# ifdef MSWIN
#  define رستو_ESC_WILDCARDS	"*?["
# وإلا
#  define رستو_ESC_WILDCARDS	"*?[{"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_پورو // VIM__H
