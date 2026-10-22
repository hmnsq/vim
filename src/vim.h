/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#నిర్వచితం_కాకపోతే VIM__H
#నిర్వచించు VIM__H

#అన్తర్భావ "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#ఒకవేళ defined(WIN32) || defined(_WIN32)
# define MSWIN
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
# include <io.h>
#అయితే_ముగింపు

// ============ the header file puzzle: order matters =========

#నిర్వచితమైతే HAVE_CONFIG_H	// GNU autoconf (or something else) was here
# ifdef VMS
#  include "config.h"   /* Rely on /INCLUDE to find it. */
# లేకపోతే
#  include "auto/config.h"
# endif /* def VMS [else] */
# define HAVE_PATHDEF

/*
 * Check if configure correctly managed to find sizeof(int).  If this failed,
 * it becomes zero.  This is likely a problem of not being able to run the
 * test program.  Other items from configure may also be wrong then!
 */
# ఒకవేళ (VIM_SIZEOF_INT == 0)
#  error configure did not నిరంతరం properly.  Check స్వయంచాలక/config.సంవర్గమానం.
# endif

# ఒకవేళ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_ఆధారం
#   define XOPEN_ఆధారం    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_ఆధారం
#    define BSD_ఆధారం 1
#   endif
#   ifndef SVID_ఆధారం
#    define SVID_ఆధారం 1
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
# ఒకవేళ defined(__CYGWIN32__) && defined(HAVE_FCHDIR)
#  undef HAVE_FCHDIR
# endif

// We may need to define the uint32_t on non-Unix system, but using the same
// identifier causes conflicts.  Therefore use UINT32_T.
# define UINT32_TYPEDEF uint32_t
#అయితే_ముగింపు

// for INT_MAX, LONG_MAX et al.
#అన్తర్భావ <limits.h>

#ఒకవేళ !defined(UINT32_TYPEDEF)
# ఒకవేళ defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# లేకపోతే
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF గుర్తులేని పూర్ణాంకం
# endif
#అయితే_ముగింపు

// C89 does not define SIZE_MAX
#ఒకవేళ defined(__hpux) || defined(VMS)
# ifndef కొలత_MAX
#  define కొలత_MAX ((size_t)(-1))
# endif
#అయితే_ముగింపు

// user ID of root is usually zero, but not for everybody
#నిర్వచితమైతే __TANDEM
# ifndef TANDEM_ఆధారం
#  define TANDEM_ఆధారం
# endif
# include <floss.h>
# define మూలం_UID 65535
# define OLDXAW
# ఒకవేళ (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#లేకపోతే
# define మూలం_UID 0
#అయితే_ముగింపు

/* Include MAC_OS_X_VERSION_* macros */
#నిర్వచితమైతే HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#అయితే_ముగింపు

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#ఒకవేళ defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#అయితే_ముగింపు
// Unless made through the Makefile enforce GUI on Mac
#ఒకవేళ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# ఒకవేళ !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#అయితే_ముగింపు

// Check support for rendering options
#నిర్వచితమైతే FEAT_GUI
# ఒకవేళ defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#అయితే_ముగింపు

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#నిర్వచితమైతే MSWIN
# define VIM_SIZEOF_INT 4
#అయితే_ముగింపు

#నిర్వచితమైతే AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  లేకపోతే
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#అయితే_ముగింపు

#ఒకవేళ VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only పనిచేస్తుంది with 32 బిట్టు పూర్ణాంకం or larger
#అయితే_ముగింపు

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#అన్తర్భావ "feature.h"

#ఒకవేళ defined(MACOS_X_DARWIN)
# ఒకవేళ defined(FEAT_సాధారణ) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# ఒకవేళ defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_సంస్కరణ_MIN_REQUIRED) && MAC_OS_X_సంస్కరణ_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# ఒకవేళ defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#అయితే_ముగింపు

// +x11 is only enabled when it's both available and wanted.
#ఒకవేళ defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#అయితే_ముగింపు

#నిర్వచితమైతే NO_X11_INCLUDES
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
#అయితే_ముగింపు

// The Mac conversion stuff doesn't work under X11.
#ఒకవేళ defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#అయితే_ముగింపు

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#నిర్వచితం_కాకపోతే VIMPACKAGE
# define VIMPACKAGE	"vim"
#అయితే_ముగింపు

/*
 * Find out if function definitions should include argument types
 */
#నిర్వచితమైతే AZTEC_C
# include <functions.h>
#అయితే_ముగింపు

#నిర్వచితమైతే SASC
# include <clib/exec_protos.h>
#అయితే_ముగింపు

#నిర్వచితమైతే _DCC
# include <clib/exec_protos.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#అయితే_ముగింపు

#ఒకవేళ (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#లేదావేళ !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) సంకేతం(signum, sighandler)
#అయితే_ముగింపు

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#నిర్వచితం_కాకపోతే వాడని
# ఒకవేళ defined(HAVE_ATTRIBUTE_వాడని) || defined(__MINGW32__)
#  define వాడని __attribute__((వాడని_2))
# లేకపోతే
#  ఒకవేళ defined __has_attribute
#   ఒకవేళ __has_attribute(వాడని_2)
#    define వాడని __attribute__((వాడని_2))
#   endif
#  endif
# endif
# ifndef వాడని
#  define వాడని
# endif
#అయితే_ముగింపు

// Used to check for "sun", "__sun" is used by newer compilers.
#ఒకవేళ defined(__sun)
# define SUN_SYSTEM
#అయితే_ముగింపు

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#ఒకవేళ !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PROTO
# ifdef AMIGA
#  include "os_amiga.h"
# endif

# ifdef MSWIN
#  include "os_win32.h"
# endif

# ఒకవేళ defined(MACOS_X)
#  include "os_mac.h"
# endif

# ifdef __QNX__
#  include "os_qnx.h"
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే X_LOCALE
# include <X11/Xlocale.h>
#లేకపోతే
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#అయితే_ముగింపు

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#నిర్వచితం_కాకపోతే MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# లేకపోతే
#  define MAXPATHL  256
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
# define మార్గం_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define బఫరు_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#లేకపోతే
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define మార్గం_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# లేకపోతే
#  define మార్గం_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define బఫరు_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#అయితే_ముగింపు

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#నిర్వచించు NUMBUFLEN 65

// flags for vim_str2nr()
#నిర్వచించు STR2NR_BIN  0x01
#నిర్వచించు STR2NR_OCT  0x02
#నిర్వచించు STR2NR_HEX  0x04
#నిర్వచించు STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#నిర్వచించు STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#నిర్వచించు STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#నిర్వచించు STR2NR_FORCE 0x80   // only when ONE of the above is used

#నిర్వచించు STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
రకనిర్వచనం గుర్తులేని అక్షరం	char_u;
రకనిర్వచనం గుర్తులేని పొట్టి	short_u;
రకనిర్వచనం గుర్తులేని పూర్ణాంకం	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#నిర్వచితమైతే _WIN64
రకనిర్వచనం గుర్తులేని __int64	long_u;
రకనిర్వచనం		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#లేకపోతే
రకనిర్వచనం గుర్తులేని దీర్ఘ		long_u;
రకనిర్వచనం		 దీర్ఘ		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#అయితే_ముగింపు
#నిర్వచించు PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#ఒకవేళ defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#అయితే_ముగింపు

/*
 * We use 64-bit file functions here, if available.  E.g. ftello() returns
 * off_t instead of long, which helps if long is 32 bit and off_t is 64 bit.
 * We assume that when fseeko() is available then ftello() is too.
 * Note that Windows has different function names.
 */
#ఒకవేళ defined(MSWIN) && !defined(PROTO)
రకనిర్వచనం __int64 off_T;
# ifdef __MINGW32__
#  define vim_lseek lseek64
#  define vim_fseek fseeko64
#  define vim_ftell ftello64
# లేకపోతే
#  define vim_lseek _lseeki64
#  define vim_fseek _fseeki64
#  define vim_ftell _ftelli64
# endif
#లేకపోతే
రకనిర్వచనం off_t off_T;
# ifdef HAVE_FSEEKO
#  define vim_lseek lseek
#  define vim_ftell ftello
#  define vim_fseek fseeko
# లేకపోతే
#  define vim_lseek lseek
#  define vim_ftell ftell
#  define vim_fseek(a, b, c)	fseek(a, (దీర్ఘ)b, c)
# endif
#అయితే_ముగింపు

/*
 * The characters and attributes cached for the screen.
 */
రకనిర్వచనం char_u schar_T;
రకనిర్వచనం గుర్తులేని పొట్టి sattr_T;
#నిర్వచించు MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
రకనిర్వచనం గుర్తులేని పూర్ణాంకం u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#ఒకవేళ defined(VMS) && defined(VAX)
// unsupported compiler
రకనిర్వచనం దీర్ఘ      vimlong_T;
#లేకపోతే
రకనిర్వచనం దీర్ఘ దీర్ఘ vimlong_T;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#అయితే_ముగింపు

#అన్తర్భావ "ascii.h"
#అన్తర్భావ "keymap.h"
#అన్తర్భావ "termdefs.h"
#అన్తర్భావ "macros.h"

#నిర్వచితమైతే LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#అయితే_ముగింపు
#నిర్వచితమైతే _DCC
# include <sys/stat.h>
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN)
# include <sys/stat.h>
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#అయితే_ముగింపు

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#నిర్వచితం_కాకపోతే UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# ఒకవేళ defined(HAVE_అక్షరశ్రేణులు_H) && !defined(NO_అక్షరశ్రేణులు_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#అయితే_ముగింపు // NON-UNIX

#అన్తర్భావ <assert.h>

#నిర్వచితమైతే HAVE_STDINT_H
# include <stdint.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_INTTYPES_H
# include <inttypes.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_WCTYPE_H
# include <wctype.h>
#అయితే_ముగింపు
#అన్తర్భావ <stdarg.h>
// older compilers do not define va_copy
#నిర్వచితం_కాకపోతే va_copy
# define va_copy(dst, src)	((dst) = (src))
#అయితే_ముగింపు

// for offsetof()
#అన్తర్భావ <stddef.h>
#అన్తర్భావ <stdbool.h>

#ఒకవేళ defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_సమయం_H) || defined(SYS_SELECT_WITH_SYS_సమయం))
# include <sys/select.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# లేకపోతే
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SODIUM
# include <sodium.h>
#అయితే_ముగింపు

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#నిర్వచితమైతే DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#అయితే_ముగింపు

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#నిర్వచితమైతే DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
బాహ్య అక్షరం *(*dyn_libintl_gettext)(స్థిరరాశి అక్షరం *msgid);
బాహ్య అక్షరం *(*dyn_libintl_ngettext)(స్థిరరాశి అక్షరం *msgid, స్థిరరాశి అక్షరం *msgid_plural, గుర్తులేని దీర్ఘ n);
బాహ్య అక్షరం *(*dyn_libintl_bindtextdomain)(స్థిరరాశి అక్షరం *domainname, స్థిరరాశి అక్షరం *dirname);
బాహ్య అక్షరం *(*dyn_libintl_bind_textdomain_codeset)(స్థిరరాశి అక్షరం *domainname, స్థిరరాశి అక్షరం *codeset);
బాహ్య అక్షరం *(*dyn_libintl_textdomain)(స్థిరరాశి అక్షరం *domainname);
బాహ్య పూర్ణాంకం (*dyn_libintl_wputenv)(స్థిరరాశి wchar_t *envstring);
బాహ్య పూర్ణాంకం *dyn_libintl_nl_సందేశం_cat_cntr;
#అయితే_ముగింపు


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#నిర్వచితమైతే FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
#  define _(x) (*dyn_libintl_gettext)((అక్షరం *)(x))
#  define NGETTEXT(x, xs, n) (*dyn_libintl_ngettext)((అక్షరం *)(x), (అక్షరం *)(xs), (n))
#  define N_(x) x
#  define bindtextdomain(domain, dir) (*dyn_libintl_bindtextdomain)((domain), (dir))
#  define bind_textdomain_codeset(domain, codeset) (*dyn_libintl_bind_textdomain_codeset)((domain), (codeset))
#  ఒకవేళ !defined(HAVE_BIND_TEXTDOMAIN_CODESET)
#   define HAVE_BIND_TEXTDOMAIN_CODESET 1
#  endif
#  define textdomain(domain) (*dyn_libintl_textdomain)(domain)
#  define libintl_wputenv(envstring) (*dyn_libintl_wputenv)(envstring)
# లేకపోతే
#  include <libintl.h>
#  define _(x) gettext((అక్షరం *)(x))
#  define NGETTEXT(x, xs, n) ngettext((x), (xs), (n))
#  ifdef gettext_noop
#   define N_(x) gettext_noop(x)
#  లేకపోతే
#   define N_(x) x
#  endif
# endif
#లేకపోతే
# define _(x) ((అక్షరం *)(x))
# define NGETTEXT(x, xs, n) (((n) == 1) ? (అక్షరం *)(x) : (అక్షరం *)(xs))
# define N_(x) x
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# define bindtextdomain(x, y) ""
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# define bind_textdomain_codeset(x, y) //ఖాళీలు
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //ఖాళీలు
#అయితే_ముగింపు

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#నిర్వచించు UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#నిర్వచించు UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#నిర్వచించు UPD_INVERTED		20  // redisplay inverted part that changed
#నిర్వచించు UPD_INVERTED_ALL	25  // redisplay whole inverted part
#నిర్వచించు UPD_REDRAW_శిఖరం		30  // display first w_upd_rows screen lines
#నిర్వచించు UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#నిర్వచించు UPD_NOT_VALID		40  // buffer needs complete redraw
#నిర్వచించు UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#నిర్వచించు SLF_RIGHTLEFT	1
#నిర్వచించు SLF_POPUP	2
#నిర్వచించు SLF_INC_VCOL	4

#నిర్వచించు MB_FILLER_CHAR '<'  // character used when a double-width character
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
#నిర్వచించు VALID_WROW	0x01	// w_wrow (window row) is valid
#నిర్వచించు VALID_WCOL	0x02	// w_wcol (window col) is valid
#నిర్వచించు VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#నిర్వచించు VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#నిర్వచించు VALID_CROW	0x10	// w_cline_row is valid
#నిర్వచించు VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#నిర్వచించు VALID_BOTLINE_AP 0x40	// w_botline is approximated
#నిర్వచించు VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#నిర్వచించు POPF_IS_POPUP	0x01	// this is a popup window
#నిర్వచించు POPF_HIDDEN	0x02	// popup is not displayed
#నిర్వచించు POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#నిర్వచించు POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#నిర్వచించు POPF_ON_CMDLINE	0x10	// popup overlaps command line
#నిర్వచించు POPF_DRAG	0x20	// popup can be moved by dragging border
#నిర్వచించు POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#నిర్వచించు POPF_RESIZE	0x80	// popup can be resized by dragging
#నిర్వచించు POPF_MAPPING	0x100	// mapping keys
#నిర్వచించు POPF_INFO	0x200	// used for info of popup menu
#నిర్వచించు POPF_INFO_MENU	0x400	// align info popup with popup menu
#నిర్వచించు POPF_POSINVERT	0x800	// vertical position can be inverted
#నిర్వచించు POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#నిర్వచించు POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#నిర్వచించు POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#నిర్వచించు POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#నిర్వచించు POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#నిర్వచించు POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#నిర్వచించు POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#నిర్వచించు POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#నిర్వచించు HL_సాధారణ		0x00
#నిర్వచించు HL_INVERSE		0x01
#నిర్వచించు HL_BOLD			0x02
#నిర్వచించు HL_ITALIC		0x04
#నిర్వచించు HL_UNDERLINE		0x08
#నిర్వచించు HL_UNDERCURL		0x10
#నిర్వచించు HL_UNDERDOUBLE		0x20
#నిర్వచించు HL_UNDERDOTTED		0x40
#నిర్వచించు HL_UNDERDASHED		0x80
#నిర్వచించు HL_STANDOUT		0x100
#నిర్వచించు HL_NOCOMBINE		0x200
#నిర్వచించు HL_STRIKETHROUGH	0x400
#నిర్వచించు HL_ALL			0x7ff

// special attribute addition: Put message in history
#నిర్వచించు సందేశం_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#నిర్వచించు బహుళకం_సాధారణ	0x01	// Normal mode, command expected
#నిర్వచించు బహుళకం_VISUAL	0x02	// Visual mode - use get_real_state()
#నిర్వచించు బహుళకం_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#నిర్వచించు బహుళకం_CMDLINE	0x08	// Editing the command line
#నిర్వచించు బహుళకం_చొప్పించు	0x10	// Insert mode, also for Replace mode
#నిర్వచించు బహుళకం_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#నిర్వచించు బహుళకం_SELECT	0x40	// Select mode, use get_real_state()
#నిర్వచించు బహుళకం_TERMINAL	0x80	// Terminal mode

#నిర్వచించు MAP_ALL_MODES	0xff    // all mode bits used for mapping

#నిర్వచించు REPLACE_FLAG	0x100	// Replace mode flag
#నిర్వచించు బహుళకం_REPLACE	(REPLACE_FLAG | బహుళకం_చొప్పించు)
#నిర్వచించు VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#నిర్వచించు బహుళకం_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | బహుళకం_చొప్పించు)
#నిర్వచించు బహుళకం_LREPLACE	(REPLACE_FLAG | బహుళకం_LANGMAP)

#నిర్వచించు బహుళకం_సాధారణ_BUSY (0x1000 | బహుళకం_సాధారణ)
				// Normal mode, busy with a command
#నిర్వచించు బహుళకం_HITRETURN	(0x2000 | బహుళకం_సాధారణ)
				// waiting for return or command
#నిర్వచించు బహుళకం_ASKMORE	0x3000	// Asking if you want --more--
#నిర్వచించు బహుళకం_SETWSIZE	0x4000	// window size has changed
#నిర్వచించు బహుళకం_EXTERNCMD	0x5000	// executing an external command
#నిర్వచించు బహుళకం_SHOWMATCH	(0x6000 | బహుళకం_చొప్పించు) // show matching paren
#నిర్వచించు బహుళకం_CONFIRM	0x7000	// ":confirm" prompt
#నిర్వచించు బహుళకం_ALL	0xffff

#నిర్వచించు బహుళకం_MAX_నిడివి	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#నిర్వచించు FORWARD			1
#నిర్వచించు BACKWARD		(-1)
#నిర్వచించు FORWARD_FILE		3
#నిర్వచించు BACKWARD_FILE		(-3)

// return values for functions
#ఒకవేళ !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#అయితే_ముగింపు
#నిర్వచించు FAIL			0
#నిర్వచించు NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#నిర్వచించు BF_RECOVERED	0x01	// buffer has been recovered
#నిర్వచించు BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#నిర్వచించు BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#నిర్వచించు BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#నిర్వచించు BF_NEW		0x10	// file didn't exist when editing started
#నిర్వచించు BF_NEW_W	0x20	// Warned for BF_NEW and file created
#నిర్వచించు BF_READERR	0x40	// got errors while reading the file
#నిర్వచించు BF_DUMMY	0x80	// dummy buffer, only used internally
#నిర్వచించు BF_PRESERVED	0x100	// ":preserve" was used
#నిర్వచించు BF_SYN_సమితి	0x200	// 'syntax' option was set
#నిర్వచించు BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#నిర్వచించు BF_WRITE_ముసుగు	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#నిర్వచించు EXPAND_UNSUCCESSFUL	(-2)
#నిర్వచించు EXPAND_OK		(-1)
#నిర్వచించు EXPAND_NOTHING		0
#నిర్వచించు EXPAND_COMMANDS		1
#నిర్వచించు EXPAND_FILES		2
#నిర్వచించు EXPAND_DIRECTORIES	3
#నిర్వచించు EXPAND_SETTINGS		4
#నిర్వచించు EXPAND_BOOL_SETTINGS	5
#నిర్వచించు EXPAND_ట్యాగులు		6
#నిర్వచించు EXPAND_OLD_SETTING	7
#నిర్వచించు EXPAND_HELP		8
#నిర్వచించు EXPAND_BUFFERS		9
#నిర్వచించు EXPAND_EVENTS		10
#నిర్వచించు EXPAND_MENUS		11
#నిర్వచించు EXPAND_SYNTAX		12
#నిర్వచించు EXPAND_HIGHLIGHT	13
#నిర్వచించు EXPAND_AUGROUP		14
#నిర్వచించు EXPAND_USER_VARS	15
#నిర్వచించు EXPAND_MAPPINGS		16
#నిర్వచించు EXPAND_ట్యాగులు_LISTFILES	17
#నిర్వచించు EXPAND_FUNCTIONS	18
#నిర్వచించు EXPAND_USER_FUNC	19
#నిర్వచించు EXPAND_వ్యక్తీకరణ	20
#నిర్వచించు EXPAND_MENUNAMES	21
#నిర్వచించు EXPAND_USER_COMMANDS	22
#నిర్వచించు EXPAND_USER_CMD_FLAGS	23
#నిర్వచించు EXPAND_USER_NARGS	24
#నిర్వచించు EXPAND_USER_COMPLETE	25
#నిర్వచించు EXPAND_ENV_VARS		26
#నిర్వచించు EXPAND_LANGUAGE		27
#నిర్వచించు EXPAND_COLORS		28
#నిర్వచించు EXPAND_COMPILER		29
#నిర్వచించు EXPAND_USER_DEFINED	30
#నిర్వచించు EXPAND_USER_LIST	31
#నిర్వచించు EXPAND_SHELLCMD		32
#నిర్వచించు EXPAND_CSCOPE		33
#నిర్వచించు EXPAND_SIGN		34
#నిర్వచించు EXPAND_PROFILE		35
#నిర్వచించు EXPAND_BEHAVE		36
#నిర్వచించు EXPAND_FILETYPE		37
#నిర్వచించు EXPAND_FILES_లోపల_మార్గం	38
#నిర్వచించు EXPAND_OWNSYNTAX	39
#నిర్వచించు EXPAND_LOCALES		40
#నిర్వచించు EXPAND_HISTORY		41
#నిర్వచించు EXPAND_USER		42
#నిర్వచించు EXPAND_SYNTIME		43
#నిర్వచించు EXPAND_USER_ADDR_TYPE	44
#నిర్వచించు EXPAND_PACKADD		45
#నిర్వచించు EXPAND_MESSAGES		46
#నిర్వచించు EXPAND_MAPCLEAR		47
#నిర్వచించు EXPAND_ARGLIST		48
#నిర్వచించు EXPAND_DIFF_BUFFERS	49
#నిర్వచించు EXPAND_DISASSEMBLE	50
#నిర్వచించు EXPAND_BREAKPOINT	51
#నిర్వచించు EXPAND_SCRIPTNAMES	52
#నిర్వచించు EXPAND_RUNTIME		53
#నిర్వచించు EXPAND_STRING_SETTING	54
#నిర్వచించు EXPAND_SETTING_వ్యవకలనం	55
#నిర్వచించు EXPAND_ARGOPT		56
#నిర్వచించు EXPAND_TERMINALOPT	57
#నిర్వచించు EXPAND_KEYMAP		58
#నిర్వచించు EXPAND_DIRS_లోపల_CDPATH	59
#నిర్వచించు EXPAND_SHELLCMDLINE	60
#నిర్వచించు EXPAND_FINDFUNC		61
#నిర్వచించు EXPAND_HIGHLIGHT_సమూహం  62
#నిర్వచించు EXPAND_FILETYPECMD	63
#నిర్వచించు EXPAND_నమూనా_లోపల_BUF	64
#నిర్వచించు EXPAND_RETAB		65
#నిర్వచించు EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#నిర్వచించు EXMODE_సాధారణ		1
#నిర్వచించు EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#నిర్వచించు WILD_FREE		1
#నిర్వచించు WILD_EXPAND_FREE	2
#నిర్వచించు WILD_EXPAND_KEEP	3
#నిర్వచించు WILD_తదుపరి		4
#నిర్వచించు WILD_PREV		5
#నిర్వచించు WILD_ALL		6
#నిర్వచించు WILD_LONGEST		7
#నిర్వచించు WILD_ALL_KEEP		8
#నిర్వచించు WILD_CANCEL		9
#నిర్వచించు WILD_APPLY		10
#నిర్వచించు WILD_PAGEUP		11
#నిర్వచించు WILD_PAGEDOWN		12

#నిర్వచించు WILD_LIST_NOTFOUND	    0x01
#నిర్వచించు WILD_HOME_REPLACE	    0x02
#నిర్వచించు WILD_USE_NL		    0x04
#నిర్వచించు WILD_NO_BEEP		    0x08
#నిర్వచించు WILD_కూడు_SLASH		    0x10
#నిర్వచించు WILD_KEEP_ALL		    0x20
#నిర్వచించు WILD_SILENT		    0x40
#నిర్వచించు WILD_ESCAPE		    0x80
#నిర్వచించు WILD_ICASE		    0x100
#నిర్వచించు WILD_ALLLINKS		    0x200
#నిర్వచించు WILD_IGNORE_COMPLETESLASH   0x400
#నిర్వచించు WILD_NOERROR		    0x800  // sets EW_NOERROR
#నిర్వచించు WILD_BUFLASTUSED	    0x1000
#నిర్వచించు BUF_DIFF_FILTER		    0x2000
#నిర్వచించు WILD_NOSELECT		    0x4000
#నిర్వచించు WILD_MAY_EXPAND_నమూనా	    0x8000
#నిర్వచించు WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#నిర్వచించు WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#నిర్వచించు EW_DIR		0x01	// include directory names
#నిర్వచించు EW_FILE		0x02	// include file names
#నిర్వచించు EW_NOTFOUND	0x04	// include not found names
#నిర్వచించు EW_ADDSLASH	0x08	// append slash to directory name
#నిర్వచించు EW_KEEPALL	0x10	// keep all matches
#నిర్వచించు EW_SILENT	0x20	// don't print "1 returned" from shell
#నిర్వచించు EW_EXEC		0x40	// executable files
#నిర్వచించు EW_మార్గం		0x80	// search in 'path' too
#నిర్వచించు EW_ICASE	0x100	// ignore case
#నిర్వచించు EW_NOERROR	0x200	// no error for bad regexp
#నిర్వచించు EW_NOTWILD	0x400	// add match with literal name if exists
#నిర్వచించు EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#నిర్వచించు EW_ALLLINKS	0x1000	// also links not pointing to existing file
#నిర్వచించు EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#నిర్వచించు EW_DODOT	0x4000	// also files starting with a dot
#నిర్వచించు EW_EMPTYOK	0x8000	// no matches is not an error
#నిర్వచించు EW_NOTENV	0x10000	// do not expand environment variables
#నిర్వచించు EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#నిర్వచించు FINDFILE_FILE	0	// only files
#నిర్వచించు FINDFILE_DIR	1	// only directories
#నిర్వచించు FINDFILE_BOTH	2	// files and directories

#ఒకవేళ defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_వెడల్పు())
# define TPL_LCOL()			tabpanel_leftcol()
#లేకపోతే
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#అయితే_ముగింపు

#నిర్వచించు W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_వెడల్పు)
#నిర్వచితమైతే FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_ఎత్తు)
#లేకపోతే
# define W_WINROW(wp)	(wp->w_winrow)
#అయితే_ముగింపు

// Values for the find_pattern_in_path() function args 'type' and 'action':
#నిర్వచించు కనుగొను_ANY	1
#నిర్వచించు కనుగొను_DEFINE	2
#నిర్వచించు CHECK_మార్గం	3

#నిర్వచించు ACTION_SHOW	1
#నిర్వచించు ACTION_GOTO	2
#నిర్వచించు ACTION_విభజించు	3
#నిర్వచించు ACTION_SHOW_ALL	4
#నిర్వచించు ACTION_EXPAND	5

#నిర్వచితమైతే FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_స్థితులు	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_ఇక్కడ	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_సరిపోలిక	0x40	// use match ID instead of item ID
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
#అయితే_ముగింపు

// Values for 'options' argument in do_search() and searchit()
#నిర్వచించు శోధన_REV    0x01  // go in reverse of previous dir.
#నిర్వచించు శోధన_ECHO   0x02  // echo the search command and handle options
#నిర్వచించు శోధన_సందేశం    0x0c  // give messages (yes, it's not 0x04)
#నిర్వచించు శోధన_NFMSG  0x08  // give all messages except not found
#నిర్వచించు శోధన_OPT    0x10  // interpret optional flags
#నిర్వచించు శోధన_HIS    0x20  // put search pattern in history
#నిర్వచించు శోధన_చివర    0x40  // put cursor at end of match
#నిర్వచించు శోధన_NOOF   0x80  // don't add offset to position
#నిర్వచించు శోధన_ప్రారంభం 0x100  // start search without col offset
#నిర్వచించు శోధన_MARK  0x200  // set previous context mark
#నిర్వచించు శోధన_KEEP  0x400  // keep previous search pattern
#నిర్వచించు శోధన_PEEK  0x800  // peek for typed char, cancel search
#నిర్వచించు శోధన_నిలువు  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#నిర్వచించు కనుగొను_IDENT	1	// find identifier (word)
#నిర్వచించు కనుగొను_STRING	2	// find any string (WORD)
#నిర్వచించు కనుగొను_EVAL	4	// include "->", "[]" and "."
#నిర్వచించు కనుగొను_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#నిర్వచించు FNAME_MESS	1	// give error message
#నిర్వచించు FNAME_EXP	2	// expand to path
#నిర్వచించు FNAME_HYP	4	// check for hypertext link
#నిర్వచించు FNAME_INCL	8	// apply 'includeexpr'
#నిర్వచించు FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#నిర్వచించు FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#నిర్వచించు GETF_SETMARK	0x01	// set pcmark before jumping
#నిర్వచించు GETF_ALT	0x02	// jumping to alternate file (not buf num)
#నిర్వచించు GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#నిర్వచించు GETFILE_ERROR	    1	// normal error
#నిర్వచించు GETFILE_NOT_WRITTEN 2	// "not written" error
#నిర్వచించు GETFILE_SAME_FILE   0	// success, same file
#నిర్వచించు GETFILE_OPEN_OTHER (-1)	// success, opened another file
#నిర్వచించు GETFILE_వాడని	    8
#నిర్వచించు GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#నిర్వచించు BLN_CURBUF	1	// may re-use curbuf for new buffer
#నిర్వచించు BLN_LISTED	2	// put new buffer in buffer list
#నిర్వచించు BLN_DUMMY	4	// allocating dummy buffer
#నిర్వచించు BLN_NEW		8	// create a new buffer
#నిర్వచించు BLN_NOOPT	16	// don't copy options to existing buffer
#నిర్వచించు BLN_DUMMY_OK	32	// also find an existing dummy buffer
#నిర్వచించు BLN_REUSE	64	// may re-use number from buf_reuse
#నిర్వచించు BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#నిర్వచించు కీలకం_OPEN_FORW	0x101
#నిర్వచించు కీలకం_OPEN_వెనుక	0x102
#నిర్వచించు కీలకం_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#నిర్వచించు MAPTYPE_MAP		0
#నిర్వచించు MAPTYPE_UNMAP		1
#నిర్వచించు MAPTYPE_NOREMAP		2
#నిర్వచించు MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#నిర్వచించు REMAP_YES	0	// allow remapping
#నిర్వచించు REMAP_NONE	(-1)	// no remapping
#నిర్వచించు REMAP_SCRIPT	(-2)	// remap script-local mappings only
#నిర్వచించు REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#నిర్వచించు SHELL_FILTER	1	// filtering text
#నిర్వచించు SHELL_EXPAND	2	// expanding wildcards
#నిర్వచించు SHELL_COOKED	4	// set term to cooked mode
#నిర్వచించు SHELL_DOOUT	8	// redirecting output
#నిర్వచించు SHELL_SILENT	16	// don't print error returned by command
#నిర్వచించు SHELL_READ	32	// read lines and insert into buffer
#నిర్వచించు SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#నిర్వచించు కణుపు_సాధారణ	0	// file or directory, check with mch_isdir()
#నిర్వచించు కణుపు_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#నిర్వచించు కణుపు_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#నిర్వచించు READ_NEW	0x01	// read a file into a new buffer
#నిర్వచించు READ_FILTER	0x02	// read filter output
#నిర్వచించు READ_STDIN	0x04	// read from stdin
#నిర్వచించు READ_బఫరు	0x08	// read from curbuf (converting stdin)
#నిర్వచించు READ_DUMMY	0x10	// reading into a dummy buffer
#నిర్వచించు READ_KEEP_UNDO	0x20	// keep undo info
#నిర్వచించు READ_FIFO	0x40	// read from fifo or socket
#నిర్వచించు READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#నిర్వచించు READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#నిర్వచించు INDENT_సమితి	1	// set indent
#నిర్వచించు INDENT_INC	2	// increase indent
#నిర్వచించు INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#నిర్వచించు FM_BACKWARD	0x01	// search backwards
#నిర్వచించు FM_FORWARD	0x02	// search forwards
#నిర్వచించు FM_BLOCKSTOP	0x04	// stop at start/end of block
#నిర్వచించు FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#నిర్వచించు DOBUF_GOTO	0	// go to specified buffer
#నిర్వచించు DOBUF_విభజించు	1	// split window and go to specified buffer
#నిర్వచించు DOBUF_UNLOAD	2	// unload specified buffer(s)
#నిర్వచించు DOBUF_DEL	3	// delete specified buffer(s) from buflist
#నిర్వచించు DOBUF_WIPE	4	// delete specified buffer(s) really
#నిర్వచించు DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#నిర్వచించు DOBUF_CURRENT	0	// "count" buffer from current buffer
#నిర్వచించు DOBUF_మొదటి	1	// "count" buffer from first buffer
#నిర్వచించు DOBUF_చివరి	2	// "count" buffer from last buffer
#నిర్వచించు DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#నిర్వచించు DOBUF_FORCEIT	1	// :cmd!
#నిర్వచించు DOBUF_NOPOPUP	2	// skip popup window buffers
#నిర్వచించు DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#నిర్వచించు RE_శోధన	0	// save/use pat in/from search_pattern
#నిర్వచించు RE_SUBST	1	// save/use pat in/from subst_pattern
#నిర్వచించు RE_BOTH		2	// save pat in both patterns
#నిర్వచించు RE_చివరి		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#నిర్వచించు RE_MAGIC	1	// 'magic' option
#నిర్వచించు RE_STRING	2	// match in string instead of buffer text
#నిర్వచించు RE_STRICT	4	// don't allow [abc] without ]
#నిర్వచించు RE_AUTO		8	// automatic engine selection

#నిర్వచితమైతే FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_సమితి	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_సమితి | REX_USE)
#అయితే_ముగింపు

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#నిర్వచించు FPC_SAME	1	// both exist and are the same file.
#నిర్వచించు FPC_DIFF	2	// both exist and are different files.
#నిర్వచించు FPC_NOTX	4	// both don't exist.
#నిర్వచించు FPC_DIFFX	6	// one of them doesn't exist.
#నిర్వచించు FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#నిర్వచించు ECMD_HIDE	0x01	// don't free the current buffer
#నిర్వచించు ECMD_సమితి_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#నిర్వచించు ECMD_OLDBUF	0x04	// use existing buffer if it exists
#నిర్వచించు ECMD_FORCEIT	0x08	// ! used in Ex command
#నిర్వచించు ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#నిర్వచించు ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#నిర్వచించు ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#నిర్వచించు ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#నిర్వచించు ECMD_చివరి	((linenr_T)-1)	// use last position in all files
#నిర్వచించు ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#నిర్వచించు DOCMD_VERBOSE	0x01	// included command in error message
#నిర్వచించు DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#నిర్వచించు DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#నిర్వచించు DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#నిర్వచించు DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#నిర్వచించు DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#నిర్వచించు DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#నిర్వచించు BL_WHITE	1	// cursor on first non-white in the line
#నిర్వచించు BL_SOL		2	// use 'sol' option
#నిర్వచించు BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#నిర్వచించు MFS_ALL		1	// also sync blocks with negative numbers
#నిర్వచించు MFS_STOP	2	// stop syncing when a character is available
#నిర్వచించు MFS_FLUSH	4	// flushed file to disk
#నిర్వచించు MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#నిర్వచించు BCO_ENTER	1	// going to enter the buffer
#నిర్వచించు BCO_ALWAYS	2	// always copy the options
#నిర్వచించు BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#నిర్వచించు PUT_FIXINDENT	1	// make indent look nice
#నిర్వచించు PUT_CURSEND	2	// leave cursor after end of new text
#నిర్వచించు PUT_CURSLINE	4	// leave cursor on last line of new text
#నిర్వచించు PUT_పంక్తి	8	// put register as lines
#నిర్వచించు PUT_పంక్తి_విభజించు	16	// split line for linewise register
#నిర్వచించు PUT_పంక్తి_FORWARD 32	// put linewise register below Visual sel.
#నిర్వచించు PUT_ఖండం_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#నిర్వచించు SIN_మారింది	1	// call changed_bytes() when line changed
#నిర్వచించు SIN_చొప్పించు	2	// insert indent before existing text
#నిర్వచించు SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#నిర్వచించు INSCHAR_FORMAT	1	// force formatting
#నిర్వచించు INSCHAR_DO_COM	2	// format comments
#నిర్వచించు INSCHAR_CTRLV	4	// char typed just after CTRL-V
#నిర్వచించు INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#నిర్వచించు INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#నిర్వచించు OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#నిర్వచించు OPENLINE_DO_COM		0x02    // format comments
#నిర్వచించు OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#నిర్వచించు OPENLINE_MARKFIX	0x08    // fix mark positions
#నిర్వచించు OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#నిర్వచించు OPENLINE_FORMAT		0x20    // formatting long comment
#నిర్వచించు OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#నిర్వచించు HIST_CMD	0	// colon commands
#నిర్వచించు HIST_శోధన	1	// search commands
#నిర్వచించు HIST_సమాసం	2	// expressions (from entering = register)
#నిర్వచించు HIST_ఆదానం	3	// input() lines
#నిర్వచించు HIST_DEBUG	4	// debug commands
#నిర్వచించు HIST_లెక్క	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#నిర్వచించు BARTYPE_సంస్కరణ 1
#నిర్వచించు BARTYPE_HISTORY 2
#నిర్వచించు BARTYPE_రిజిస్టరు 3
#నిర్వచించు BARTYPE_MARK 4

#నిర్వచించు VIMINFO_సంస్కరణ 4
#నిర్వచించు VIMINFO_సంస్కరణ_WITH_HISTORY 2
#నిర్వచించు VIMINFO_సంస్కరణ_WITH_REGISTERS 3
#నిర్వచించు VIMINFO_సంస్కరణ_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#నిర్వచించు DT_ట్యాగ్		1	// jump to newer position or same tag again
#నిర్వచించు DT_తీయి		2	// jump to older position
#నిర్వచించు DT_తదుపరి		3	// jump to next match of same tag
#నిర్వచించు DT_PREV		4	// jump to previous match of same tag
#నిర్వచించు DT_మొదటి	5	// jump to first match of same tag
#నిర్వచించు DT_చివరి		6	// jump to first match of same tag
#నిర్వచించు DT_SELECT	7	// jump to selection from list
#నిర్వచించు DT_HELP		8	// like DT_TAG, but no wildcards
#నిర్వచించు DT_JUMP		9	// jump to new tag or selection from list
#నిర్వచించు DT_CSCOPE	10	// cscope find command (like tjump)
#నిర్వచించు DT_LTAG		11	// tag using location list
#నిర్వచించు DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#నిర్వచించు ట్యాగ్_HELP	1	// only search for help tags
#నిర్వచించు ట్యాగ్_పేర్లు	2	// only return name of tag
#నిర్వచించు	ట్యాగ్_REGEXP	4	// use tag pattern as regexp
#నిర్వచించు	ట్యాగ్_NOIC	8	// don't always ignore case
#నిర్వచితమైతే FEAT_CSCOPE
# define ట్యాగ్_CSCOPE	16	// cscope tag
#అయితే_ముగింపు
#నిర్వచించు ట్యాగ్_VERBOSE	32	// message verbosity
#నిర్వచించు ట్యాగ్_INS_COMP	64	// Currently doing insert completion
#నిర్వచించు ట్యాగ్_KEEP_LANG	128	// keep current language
#నిర్వచించు ట్యాగ్_NO_TAGFUNC	256	// do not use 'tagfunc'

#నిర్వచించు ట్యాగ్_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#నిర్వచించు VIM_GENERIC	0
#నిర్వచించు VIM_ERROR	1
#నిర్వచించు VIM_WARNING	2
#నిర్వచించు VIM_INFO	3
#నిర్వచించు VIM_QUESTION	4
#నిర్వచించు VIM_చివరి_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#నిర్వచించు VIM_YES		2
#నిర్వచించు VIM_NO		3
#నిర్వచించు VIM_CANCEL	4
#నిర్వచించు VIM_ALL		5
#నిర్వచించు VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#నిర్వచించు WSP_స్థలం	0x01	// require enough room
#నిర్వచించు WSP_VERT	0x02	// split/equalize vertically
#నిర్వచించు WSP_HOR		0x04	// equalize horizontally
#నిర్వచించు WSP_శిఖరం		0x08	// window at top-left of shell
#నిర్వచించు WSP_BOT		0x10	// window at bottom-right of shell
#నిర్వచించు WSP_HELP	0x20	// creating the help window
#నిర్వచించు WSP_క్రింద	0x40	// put new window below/right
#నిర్వచించు WSP_ABOVE	0x80	// put new window above/left
#నిర్వచించు WSP_NEWLOC	0x100	// don't copy location list
#నిర్వచించు WSP_FORCE_స్థలం	0x200	// ignore "not enough room" errors
#నిర్వచించు WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#నిర్వచించు RESIZE_VERT	1	// resize vertically
#నిర్వచించు RESIZE_HOR	2	// resize horizontally
#నిర్వచించు RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#నిర్వచించు CCGD_AW		1	// do autowrite if buffer was changed
#నిర్వచించు CCGD_MULTWIN	2	// check also when several wins for the buf
#నిర్వచించు CCGD_FORCEIT	4	// ! used
#నిర్వచించు CCGD_ALLBUF	8	// may write all buffers
#నిర్వచించు CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#నిర్వచించు OPT_FREE	0x01	// free old value if it was allocated
#నిర్వచించు OPT_GLOBAL	0x02	// use global value
#నిర్వచించు OPT_LOCAL	0x04	// use local value
#నిర్వచించు OPT_MODELINE	0x08	// option in modeline
#నిర్వచించు OPT_WINONLY	0x10	// only set window-local options
#నిర్వచించు OPT_NOWIN	0x20	// don't set window-local options
#నిర్వచించు OPT_ONECOLUMN	0x40	// list options one per line
#నిర్వచించు OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#నిర్వచించు OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#నిర్వచించు DLG_BUTTON_SEP	'\n'
#నిర్వచించు DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#నిర్వచించు NO_SCREEN	2	// no screen updating yet
#నిర్వచించు NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#నిర్వచించు SEA_NONE	0	// don't use dialog
#నిర్వచించు SEA_DIALOG	1	// use dialog when possible
#నిర్వచించు SEA_QUIT	2	// quit editing the file
#నిర్వచించు SEA_RECOVER	3	// recover the file
#నిర్వచించు SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#నిర్వచించు MIN_మార్పిడి_పుట_కొలత 1048
#నిర్వచించు MAX_మార్పిడి_పుట_కొలత 50000

// Special values for current_sctx.sc_sid.
#నిర్వచించు SID_MODELINE	(-1)	// when using a modeline
#నిర్వచించు SID_CMDARG	(-2)	// for "--cmd" argument
#నిర్వచించు SID_CARG	(-3)	// for "-c" argument
#నిర్వచించు SID_ENV		(-4)	// for sourcing environment variable
#నిర్వచించు SID_ERROR	(-5)	// option was reset because of an error
#నిర్వచించు SID_NONE	(-6)	// don't set scriptID
#నిర్వచించు SID_WINLAYOUT	(-7)	// changing window size

/*
 * Events for autocommands. Must be kept in sync with "event_tab".
 */
గణన auto_event
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

    అంకె_EVENTS,			// MUST be the last one
};

రకనిర్వచనం గణన auto_event event_T;

/*
 * Values for index in highlight_attr[].
 * When making changes, also update HL_FLAGS below!
 * And update the default value of 'highlight': HIGHLIGHT_INIT in optiondefs.h
 */
రకనిర్వచనం గణన
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
    , HLF_కూడు	    // Added diff line
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
    , HLF_తీయి	    // popup window body
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
    , HLF_సందేశం	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_లెక్క	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#నిర్వచించు HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
రకనిర్వచనం గణన
{
    SMT_ALL = 0		    // Move to "all" words
    , SMT_BAD		    // Move to "bad" words only
    , SMT_RARE		    // Move to "rare" words only
} smt_T;
/*
 * Boolean constants
 */
#నిర్వచితం_కాకపోతే TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#అయితే_ముగింపు

#నిర్వచించు MAYBE	2	    // sometimes used for a variant on TRUE

#నిర్వచించు LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#నిర్వచితమైతే FEAT_పని_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_గమ్యం_LOG_THIS ఒకవేళ (ch_log_output == FALSE) ch_log_output = TRUE;
#లేకపోతే
// no logging support
# define MAY_WANT_గమ్యం_LOG_THIS
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే UINT32_T
రకనిర్వచనం UINT32_TYPEDEF UINT32_T;
#అయితే_ముగింపు

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#నిర్వచించు OP_NOP		0	// no pending operation
#నిర్వచించు OP_DELETE	1	// "d"  delete operator
#నిర్వచించు OP_YANK		2	// "y"  yank operator
#నిర్వచించు OP_CHANGE	3	// "c"  change operator
#నిర్వచించు OP_LSHIFT	4	// "<"  left shift operator
#నిర్వచించు OP_RSHIFT	5	// ">"  right shift operator
#నిర్వచించు OP_FILTER	6	// "!"  filter operator
#నిర్వచించు OP_TILDE	7	// "g~" switch case operator
#నిర్వచించు OP_INDENT	8	// "="  indent operator
#నిర్వచించు OP_FORMAT	9	// "gq" format operator
#నిర్వచించు OP_COLON	10	// ":"  colon operator
#నిర్వచించు OP_పై	11	// "gU" make upper case operator
#నిర్వచించు OP_క్రింది	12	// "gu" make lower case operator
#నిర్వచించు OP_JOIN		13	// "J"  join operator, only for Visual mode
#నిర్వచించు OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#నిర్వచించు OP_ROT13	15	// "g?" rot-13 encoding
#నిర్వచించు OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#నిర్వచించు OP_చొప్పించు	17	// "I"  Insert column, only for Visual mode
#నిర్వచించు OP_APPEND	18	// "A"  Append column, only for Visual mode
#నిర్వచించు OP_FOLD		19	// "zf" define a fold
#నిర్వచించు OP_FOLDOPEN	20	// "zo" open folds
#నిర్వచించు OP_FOLDOPENREC	21	// "zO" open folds recursively
#నిర్వచించు OP_FOLDCLOSE	22	// "zc" close folds
#నిర్వచించు OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#నిర్వచించు OP_FOLDDEL	24	// "zd" delete folds
#నిర్వచించు OP_FOLDDELREC	25	// "zD" delete folds recursively
#నిర్వచించు OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#నిర్వచించు OP_FUNCTION	27	// "g@" call 'operatorfunc'
#నిర్వచించు OP_NR_కూడు	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#నిర్వచించు OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#నిర్వచించు MCHAR	0		// character-wise movement/register
#నిర్వచించు MLINE	1		// line-wise movement/register
#నిర్వచించు MBLOCK	2		// block-wise register

#నిర్వచించు MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#నిర్వచించు MIN_COLUMNS	12	// minimal columns for screen
#నిర్వచించు MIN_పంక్తులు	2	// minimal lines for screen
#నిర్వచించు MIN_CMDHEIGHT	1	// minimal height for command line
#నిర్వచించు స్థితి_ఎత్తు_2	1	// default height of a status line under a
				//కిటికీ
#నిర్వచితమైతే FEAT_MENU		// height of a status line under a window
# define WINBAR_ఎత్తు(wp)	(wp)->w_winbar_ఎత్తు
# define VISIBLE_ఎత్తు(wp)	((wp)->w_ఎత్తు + (wp)->w_winbar_ఎత్తు)
#లేకపోతే
# define WINBAR_ఎత్తు(wp)	0
# define VISIBLE_ఎత్తు(wp)	(wp)->w_ఎత్తు
#అయితే_ముగింపు
#నిర్వచించు QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#నిర్వచితం_కాకపోతే CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#అయితే_ముగింపు

#నిర్వచించు LSIZE	    512		// max. size of a line in the tags file

#నిర్వచించు IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#నిర్వచించు DIALOG_సందేశం_కొలత 1000	// buffer size for dialog_msg()

#నిర్వచించు సందేశం_BUF_LEN 480	// length of buffer for small messages
#నిర్వచించు సందేశం_BUF_CLEN  (సందేశం_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#నిర్వచించు FOLD_పాఠ్యం_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#నిర్వచించు TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#నిర్వచించు MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#నిర్వచించు MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#నిర్వచించు UNDO_HASH_కొలత 32

#నిర్వచితమైతే HAVE_FCNTL_H
# include <fcntl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే ద్విమాన_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#లేకపోతే
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#అయితే_ముగింపు

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#నిర్వచితమైతే __CYGWIN__
# define O_EXTRA    O_ద్విమాన
#లేకపోతే
# define O_EXTRA    0
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే O_NOFOLLOW
# define O_NOFOLLOW 0
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#అయితే_ముగింపు

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#నిర్వచించు ALLOC_ONE(type)  (type *)alloc(పరిమాణం(type))
#నిర్వచించు ALLOC_ONE_ID(type, id)  (type *)alloc_id(పరిమాణం(type), id)
#నిర్వచించు ALLOC_MULT(type, లెక్క)  (type *)alloc(పరిమాణం(type) * (లెక్క))
#నిర్వచించు ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(పరిమాణం(type))
#నిర్వచించు ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(పరిమాణం(type), id)
#నిర్వచించు ALLOC_CLEAR_MULT(type, లెక్క)  (type *)alloc_clear(పరిమాణం(type) * (లెక్క))
#నిర్వచించు LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(పరిమాణం(type), FALSE)
#నిర్వచించు LALLOC_CLEAR_MULT(type, లెక్క)  (type *)lalloc_clear(పరిమాణం(type) * (లెక్క), FALSE)
#నిర్వచించు LALLOC_MULT(type, లెక్క)  (type *)lalloc(పరిమాణం(type) * (లెక్క), FALSE)

#నిర్వచితమైతే HAVE_MEMSET
# define vim_memset(ptr, c, కొలత)   memset((ptr), (c), (కొలత))
#లేకపోతే
శూన్య *vim_memset(శూన్య *, పూర్ణాంకం, size_t);
#అయితే_ముగింపు
#నిర్వచించు CLEAR_క్షేత్రం(క్షేత్రం)  vim_memset(&(క్షేత్రం), 0, పరిమాణం(క్షేత్రం))
#నిర్వచించు CLEAR_సూచిక(ptr)  vim_memset((ptr), 0, పరిమాణం(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#నిర్వచించు STRLEN(s)	    అక్షరశ్రేణి_పొడవు((అక్షరం *)(s))
#నిర్వచించు STRCPY(d, s)	    strcpy((అక్షరం *)(d), (అక్షరం *)(s))
#నిర్వచించు STRNCPY(d, s, n)    strncpy((అక్షరం *)(d), (అక్షరం *)(s), (n))
#నిర్వచించు STRCMP(d, s)	    strcmp((అక్షరం *)(d), (అక్షరం *)(s))
#నిర్వచించు STRNCMP(d, s, n)    అక్షరశ్రేణి_పోలిక((అక్షరం *)(d), (అక్షరం *)(s), (n))
#నిర్వచితమైతే HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((అక్షరం *)(d), (అక్షరం *)(s))
#లేకపోతే
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((అక్షరం *)(d), (అక్షరం *)(s))
# లేకపోతే
#  define STRICMP(d, s)	    vim_stricmp((అక్షరం *)(d), (అక్షరం *)(s))
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((అక్షరం *)(d), (అక్షరం *)(s))
#లేకపోతే
# define STRCOLL(d, s)     strcmp((అక్షరం *)(d), (అక్షరం *)(s))
#అయితే_ముగింపు

// Like strcpy() but allows overlapped source and destination.
#నిర్వచించు STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#నిర్వచితమైతే HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((అక్షరం *)(d), (అక్షరం *)(s), (n))
#లేకపోతే
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((అక్షరం *)(d), (అక్షరం *)(s), (n))
# లేకపోతే
#  define STRNICMP(d, s, n) vim_strnicmp((అక్షరం *)(d), (అక్షరం *)(s), (n))
# endif
#అయితే_ముగింపు

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#నిర్వచించు MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (పూర్ణాంకం)MAXCOL)
#నిర్వచించు MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (పూర్ణాంకం)(n))
#నిర్వచించు MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#నిర్వచించు STRCAT(d, s)	    strcat((అక్షరం *)(d), (అక్షరం *)(s))
#నిర్వచించు STRNCAT(d, s, n)    strncat((అక్షరం *)(d), (అక్షరం *)(s), (n))

#నిర్వచితమైతే HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((అక్షరం *)(s), (అక్షరం *)(cs))
#అయితే_ముగింపు

#నిర్వచించు బహిర్గతం_STR(s)		    బహిర్గతం_str((char_u *)(s))
#నిర్వచించు బహిర్గతం_STR_NF(s)		    బహిర్గతం_str_nf((char_u *)(s))

#నిర్వచితమైతే FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.లోపల_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.లోపల_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_బిట్టు		    (gui.లోపల_use || p_tgc)
# లేకపోతే
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_బిట్టు		    gui.లోపల_use
# endif
#లేకపోతే
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_బిట్టు		    p_tgc
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#లేకపోతే
# define IS_CTERM		    (t_colors > 1)
#అయితే_ముగింపు
#నిర్వచితమైతే GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#అయితే_ముగింపు

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#నిర్వచితమైతే HAVE_STRERROR
# define PERROR(సందేశం)		    (శూన్య)semsg("%s: %s", (అక్షరం *)(సందేశం), strerror(errno))
#లేకపోతే
# define PERROR(సందేశం)		    do_perror(సందేశం)
#అయితే_ముగింపు

రకనిర్వచనం దీర్ఘ	linenr_T;		// line number type
రకనిర్వచనం పూర్ణాంకం	colnr_T;		// column number type
రకనిర్వచనం గుర్తులేని పొట్టి disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#నిర్వచితమైతే __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#లేకపోతే
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#అయితే_ముగింపు

#నిర్వచించు SHOWCMD_నిలువుల_సంఖ్య 10			// columns needed by shown command

రకనిర్వచనం శూన్య	    *vim_acl_T;		// dummy to pass an ACL to a function

#నిర్వచితం_కాకపోతే mch_memmove
# define mch_memmove(గమ్యం, from, len) memmove((అక్షరం*)(గమ్యం), (అక్షరం*)(from), (len))
#అయితే_ముగింపు

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#నిర్వచించు fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#నిర్వచించు fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#ఒకవేళ defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_ఆదానం_BUF
#అయితే_ముగింపు

#ఒకవేళ !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, లెక్క) vim_read((fd), (buf), (లెక్క))
# define write_eintr(fd, buf, లెక్క) vim_write((fd), (buf), (లెక్క))
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, లెక్క)   పఠ((fd), (అక్షరం *)(buf), (గుర్తులేని పూర్ణాంకం)(లెక్క))
# define vim_write(fd, buf, లెక్క)  లిఖ((fd), (అక్షరం *)(buf), (గుర్తులేని పూర్ణాంకం)(లెక్క))
#లేకపోతే
# define vim_read(fd, buf, లెక్క)   పఠ((fd), (అక్షరం *)(buf), (లెక్క))
# define vim_write(fd, buf, లెక్క)  లిఖ((fd), (అక్షరం *)(buf), (లెక్క))
#అయితే_ముగింపు

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#నిర్వచించు HL_ATTR(n)	highlight_attr[(పూర్ణాంకం)(n)]
#నిర్వచించు పదం_STR(n)	పదం_అక్షరశ్రేణులు[(పూర్ణాంకం)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#నిర్వచితం_కాకపోతే EXTERN
# define EXTERN బాహ్య
# define INIT(x)
# define INIT2(a, b)
# define INIT3(a, b, c)
# define INIT4(a, b, c, d)
# define INIT5(a, b, c, d, e)
# define INIT6(a, b, c, d, e, f)
#లేకపోతే
# ifndef INIT
#  define INIT(x) x
#  define INIT2(a, b) = {a, b}
#  define INIT3(a, b, c) = {a, b, c}
#  define INIT4(a, b, c, d) = {a, b, c, d}
#  define INIT5(a, b, c, d, e) = {a, b, c, d, e}
#  define INIT6(a, b, c, d, e, f) = {a, b, c, d, e, f}
#  define DO_INIT
# endif
#అయితే_ముగింపు

#నిర్వచించు MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#నిర్వచించు MB_MAXBYTES	30

#ఒకవేళ (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
రకనిర్వచనం LARGE_INTEGER proftime_T;
#  define PROF_సమయం_BLANK "           "
#  define PROF_TOTALS_శీర్షిక "count  total (s)   self (s)"
# లేకపోతే
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  ఒకవేళ defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
రకనిర్వచనం నిర్మాణం timespec proftime_T;
#   define PROF_GET_సమయం(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_సమయం_FORMAT "%3ld.%09ld"
#   define PROF_సమయం_BLANK "              "
#   define PROF_TOTALS_శీర్షిక "count     total (s)      self (s)"
#  లేకపోతే
రకనిర్వచనం నిర్మాణం timeval proftime_T;
#   define PROF_GET_సమయం(tm) gettimeofday(tm, ఖాళీ)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_సమయం_FORMAT "%3ld.%06ld"
#   define PROF_సమయం_BLANK "           "
#   define PROF_TOTALS_శీర్షిక "count  total (s)   self (s)"
#  endif
# endif
#లేకపోతే
రకనిర్వచనం పూర్ణాంకం proftime_T;	    // dummy for function prototypes
#అయితే_ముగింపు

// Type of compilation passed to compile_def_function()
రకనిర్వచనం గణన {
    CT_NONE,	    // use df_instr
    CT_PROFILE,	    // use df_instr_prof
    CT_DEBUG	    // use df_instr_debug, overrules CT_PROFILE
} compiletype_T;

/*
 * When compiling with 32 bit Perl time_t is 32 bits in the Perl code but 64
 * bits elsewhere.  That causes memory corruption.  Define time_T and use it
 * for global variables to avoid that.
 */
#నిర్వచితమైతే MSWIN
రకనిర్వచనం __time64_t  సమయం_T;
#లేకపోతే
రకనిర్వచనం time_t	    సమయం_T;
#అయితే_ముగింపు

#నిర్వచితమైతే _WIN64
రకనిర్వచనం __int64 sock_T;
#లేకపోతే
రకనిర్వచనం పూర్ణాంకం sock_T;
#అయితే_ముగింపు

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#అయితే_ముగింపు

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#అన్తర్భావ "option.h"	// options and default values

#అన్తర్భావ "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#అన్తర్భావ "structs.h"	// defines many structures
#అన్తర్భావ "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#అన్తర్భావ "alloc.h"

// Values for "do_profiling".
#నిర్వచించు PROF_NONE	0	// profiling not started
#నిర్వచించు PROF_YES	1	// profiling busy
#నిర్వచించు PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#నిర్వచించు MOUSE_ఎడమ	0x00
#నిర్వచించు MOUSE_మధ్య	0x01
#నిర్వచించు MOUSE_కుడి	0x02
#నిర్వచించు MOUSE_RELEASE	0x03

// bit masks for modifiers:
#నిర్వచించు MOUSE_జరుపు	0x04
#నిర్వచించు MOUSE_ALT	0x08
#నిర్వచించు MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#నిర్వచించు MOUSE_4	0x100	// scroll wheel down
#నిర్వచించు MOUSE_5	0x200	// scroll wheel up

#నిర్వచించు MOUSE_X1	0x300 // Mouse-button X1 (6th)
#నిర్వచించు MOUSE_X2	0x400 // Mouse-button X2

#నిర్వచించు MOUSE_6	0x500	// scroll wheel left
#నిర్వచించు MOUSE_7	0x600	// scroll wheel right

#నిర్వచించు MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#నిర్వచించు MOUSE_DRAG_XTERM   0x40

#నిర్వచించు MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#నిర్వచించు MOUSEWHEEL_దిగువ		0x60

// Lowest button code for extra mouse buttons 8-11
#నిర్వచించు MOUSESIDEBUTTONS_దిగువ		0xa0

#నిర్వచించు MOUSE_CLICK_ముసుగు	0x03

#నిర్వచించు అంకె_MOUSE_CLICKS(సంకేతం_2) \
    (((గుర్తులేని)((సంకేతం_2) & 0xC0) >> 6) + 1)

#నిర్వచించు సమితి_అంకె_MOUSE_CLICKS(సంకేతం_2, అంకె) \
    ((సంకేతం_2) = ((సంకేతం_2) & 0x3f) | ((((అంకె) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#నిర్వచించు MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#నిర్వచించు లోపల_UNKNOWN		0
#నిర్వచించు లోపల_బఫరు		1
#నిర్వచించు లోపల_స్థితి_పంక్తి		2	// on status or command line
#నిర్వచించు లోపల_SEP_పంక్తి		4	// on vertical separator line
#నిర్వచించు లోపల_OTHER_WIN		8	// in other window but can't go there
#నిర్వచించు CURSOR_MOVED		0x100
#నిర్వచించు MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#నిర్వచించు MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#నిర్వచించు MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#నిర్వచించు MOUSE_FOCUS		0x01	// need to stay in this window
#నిర్వచించు MOUSE_MAY_VIS		0x02	// may start Visual mode
#నిర్వచించు MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#నిర్వచించు MOUSE_SETPOS		0x08	// only set current mouse position
#నిర్వచించు MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#నిర్వచించు MOUSE_RELEASED		0x20	// button was released

#ఒకవేళ defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_సమయం_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#అయితే_ముగింపు


// defines for eval_vars()
#నిర్వచించు VALID_మార్గం		1
#నిర్వచించు VALID_తల		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#నిర్వచించు VV_లెక్క	0
#నిర్వచించు VV_COUNT1	1
#నిర్వచించు VV_PREVCOUNT	2
#నిర్వచించు VV_ERRMSG	3
#నిర్వచించు VV_WARNINGMSG	4
#నిర్వచించు VV_STATUSMSG	5
#నిర్వచించు VV_SHELL_ERROR	6
#నిర్వచించు VV_THIS_SESSION	7
#నిర్వచించు VV_సంస్కరణ	8
#నిర్వచించు VV_LNUM		9
#నిర్వచించు VV_TERMRESPONSE	10
#నిర్వచించు VV_FNAME	11
#నిర్వచించు VV_LANG		12
#నిర్వచించు VV_LC_సమయం	13
#నిర్వచించు VV_CTYPE	14
#నిర్వచించు VV_CC_FROM	15
#నిర్వచించు VV_CC_గమ్యం	16
#నిర్వచించు VV_FNAME_లోపల	17
#నిర్వచించు VV_FNAME_బహిర్గతం	18
#నిర్వచించు VV_FNAME_NEW	19
#నిర్వచించు VV_FNAME_DIFF	20
#నిర్వచించు VV_CMDARG	21
#నిర్వచించు VV_FOLDSTART	22
#నిర్వచించు VV_FOLDEND	23
#నిర్వచించు VV_FOLDDASHES	24
#నిర్వచించు VV_FOLDLEVEL	25
#నిర్వచించు VV_PROGNAME	26
#నిర్వచించు VV_SEND_SERVER	27
#నిర్వచించు VV_DYING	28
#నిర్వచించు VV_EXCEPTION	29
#నిర్వచించు VV_THROWPOINT	30
#నిర్వచించు VV_REG		31
#నిర్వచించు VV_CMDBANG	32
#నిర్వచించు VV_INSERTMODE	33
#నిర్వచించు VV_VAL		34
#నిర్వచించు VV_కీలకం		35
#నిర్వచించు VV_PROFILING	36
#నిర్వచించు VV_FCS_REASON	37
#నిర్వచించు VV_FCS_CHOICE	38
#నిర్వచించు VV_BEVAL_BUFNR	39
#నిర్వచించు VV_BEVAL_WINNR	40
#నిర్వచించు VV_BEVAL_WINID	41
#నిర్వచించు VV_BEVAL_LNUM	42
#నిర్వచించు VV_BEVAL_నిలువు	43
#నిర్వచించు VV_BEVAL_పాఠ్యం	44
#నిర్వచించు VV_SCROLLSTART	45
#నిర్వచించు VV_SWAPNAME	46
#నిర్వచించు VV_SWAPCHOICE	47
#నిర్వచించు VV_SWAPCOMMAND	48
#నిర్వచించు VV_CHAR		49
#నిర్వచించు VV_MOUSE_WIN	50
#నిర్వచించు VV_MOUSE_WINID	51
#నిర్వచించు VV_MOUSE_LNUM   52
#నిర్వచించు VV_MOUSE_నిలువు	53
#నిర్వచించు VV_OP		54
#నిర్వచించు VV_SEARCHFORWARD 55
#నిర్వచించు VV_HLSEARCH	56
#నిర్వచించు VV_OLDFILES	57
#నిర్వచించు VV_WINDOWID	58
#నిర్వచించు VV_PROGPATH	59
#నిర్వచించు VV_COMPLETED_అంశం 60
#నిర్వచించు VV_OPTION_NEW   61
#నిర్వచించు VV_OPTION_OLD   62
#నిర్వచించు VV_OPTION_OLDLOCAL 63
#నిర్వచించు VV_OPTION_OLDGLOBAL 64
#నిర్వచించు VV_OPTION_COMMAND 65
#నిర్వచించు VV_OPTION_TYPE  66
#నిర్వచించు VV_ERRORS	67
#నిర్వచించు VV_FALSE	68
#నిర్వచించు VV_TRUE		69
#నిర్వచించు VV_NONE		70
#నిర్వచించు VV_NULL		71
#నిర్వచించు VV_NUMBERMAX	72
#నిర్వచించు VV_NUMBERMIN	73
#నిర్వచించు VV_NUMBERSIZE	74
#నిర్వచించు VV_VIM_DID_ENTER 75
#నిర్వచించు VV_TESTING	76
#నిర్వచించు VV_TYPE_సంఖ్య	77
#నిర్వచించు VV_TYPE_STRING	78
#నిర్వచించు VV_TYPE_FUNC	79
#నిర్వచించు VV_TYPE_LIST	80
#నిర్వచించు VV_TYPE_DICT	81
#నిర్వచించు VV_TYPE_FLOAT	82
#నిర్వచించు VV_TYPE_BOOL	83
#నిర్వచించు VV_TYPE_NONE	84
#నిర్వచించు VV_TYPE_పని	85
#నిర్వచించు VV_TYPE_CHANNEL	86
#నిర్వచించు VV_TYPE_BLOB	87
#నిర్వచించు VV_TYPE_CLASS	88
#నిర్వచించు VV_TYPE_OBJECT	89
#నిర్వచించు VV_TERMRFGRESP	90
#నిర్వచించు VV_TERMRBGRESP	91
#నిర్వచించు VV_TERMU7RESP	92
#నిర్వచించు VV_TERMSTYLERESP 93
#నిర్వచించు VV_TERMBLINKRESP 94
#నిర్వచించు VV_EVENT	95
#నిర్వచించు VV_VERSIONLONG	96
#నిర్వచించు VV_ECHOSPACE	97
#నిర్వచించు VV_ARGV		98
#నిర్వచించు VV_COLLATE      99
#నిర్వచించు VV_EXITING	100
#నిర్వచించు VV_COLORNAMES   101
#నిర్వచించు VV_SIZEOFINT	102
#నిర్వచించు VV_SIZEOFLONG	103
#నిర్వచించు VV_SIZEOFPOINTER 104
#నిర్వచించు VV_MAXCOL	105
#నిర్వచించు VV_PYTHON3_సంస్కరణ 106
#నిర్వచించు VV_TYPE_TYPEALIAS 107
#నిర్వచించు VV_TYPE_ENUM	  108
#నిర్వచించు VV_TYPE_ENUMVALUE 109
#నిర్వచించు VV_STACKTRACE	110
#నిర్వచించు VV_TYPE_TUPLE	111
#నిర్వచించు VV_WAYLAND_DISPLAY 112
#నిర్వచించు VV_CLIPMETHOD 113
#నిర్వచించు VV_TERMDA1 114
#నిర్వచించు VV_TERMOSC 115
#నిర్వచించు VV_VIM_DID_INIT		116
#నిర్వచించు VV_CLIPPROVIDERS 117
#నిర్వచించు VV_IMAGEBACKEND 118
#నిర్వచించు VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#నిర్వచించు VVAL_FALSE	0L	// VAR_BOOL
#నిర్వచించు VVAL_TRUE	1L	// VAR_BOOL
#నిర్వచించు VVAL_NONE	2L	// VAR_SPECIAL
#నిర్వచించు VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#నిర్వచించు VAR_TYPE_సంఖ్య	    0
#నిర్వచించు VAR_TYPE_STRING	    1
#నిర్వచించు VAR_TYPE_FUNC	    2
#నిర్వచించు VAR_TYPE_LIST	    3
#నిర్వచించు VAR_TYPE_DICT	    4
#నిర్వచించు VAR_TYPE_FLOAT	    5
#నిర్వచించు VAR_TYPE_BOOL	    6
#నిర్వచించు VAR_TYPE_NONE	    7
#నిర్వచించు VAR_TYPE_పని	    8
#నిర్వచించు VAR_TYPE_CHANNEL    9
#నిర్వచించు VAR_TYPE_BLOB	    10
#నిర్వచించు VAR_TYPE_INSTR	    11
#నిర్వచించు VAR_TYPE_CLASS	    12
#నిర్వచించు VAR_TYPE_OBJECT	    13
#నిర్వచించు VAR_TYPE_TYPEALIAS  14
#నిర్వచించు VAR_TYPE_ENUM	    15
#నిర్వచించు VAR_TYPE_ENUMVALUE  16
#నిర్వచించు VAR_TYPE_TUPLE	    17

#నిర్వచించు DICT_MAXNEST 100	// maximum nesting of lists and dicts

#నిర్వచించు TABSTOP_MAX 9999

#నిర్వచితమైతే HAVE_CLIPMETHOD
రకనిర్వచనం గణన {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD

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
# define SELECT_లోపల_పురోగతి	1
# define SELECT_పూర్తి		2

# define SELECT_బహుళకం_CHAR	0
# define SELECT_బహుళకం_మాట	1
# define SELECT_బహుళకం_పంక్తి	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		అందుబాటు;	// Is clipboard available?
    పూర్ణాంకం		owned;		// Flag: do we own the selection?
    pos_T	ప్రారంభం;		// Start of selected area
    pos_T	చివర;		// End of selected area
    పూర్ణాంకం		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_అడ్డువరుస;
    short_u	origin_ప్రారంభం_నిలువు;
    short_u	origin_చివర_నిలువు;
    short_u	మాట_ప్రారంభం_నిలువు;
    short_u	మాట_చివర_నిలువు;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_నిలువు;
    short_u	max_నిలువు;
    short_u	min_అడ్డువరుస;
    short_u	max_అడ్డువరుస;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	బహుళకం;		// Select by char, word, or line.

# ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_XCLIPBOARD)
    Atom	sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ఒకవేళ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
    GdkAtom     gtk_sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ఒకవేళ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    int_u	format;		// Vim's own special clipboard format
    int_u	format_raw;	// Vim's raw text clipboard format
# endif
# ifdef FEAT_GUI_HAIKU
    // No clipboard at the moment. TODO?
# endif
} Clipboard_T;
#లేకపోతే
రకనిర్వచనం పూర్ణాంకం Clipboard_T;	// This is required for the prototypes.
#అయితే_ముగింపు

// Use 64-bit stat structure on MS-Windows.
#నిర్వచితమైతే MSWIN
రకనిర్వచనం నిర్మాణం _stat64 stat_T;
#లేకపోతే
రకనిర్వచనం నిర్మాణం stat stat_T;
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం soundcb_S soundcb_T;

#ఒకవేళ (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(రాయి, fmt_idx, arg_idx)))
#లేకపోతే
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#అయితే_ముగింపు

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#లేకపోతే
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#అయితే_ముగింపు

రకనిర్వచనం గణన {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_సరిపోలిక,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
రకనిర్వచనం గణన {
    PASTE_చొప్పించు,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_బహుళకం_T;

// Argument for flush_buffers().
రకనిర్వచనం గణన {
    FLUSH_కనీస,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_ఆదానం		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
రకనిర్వచనం గణన {
    USEPOPUP_NONE,
    USEPOPUP_సాధారణ,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
రకనిర్వచనం గణన {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_దొంతర,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#నిర్వచించు ENABLE_STL_బహుళకం_MULTI_NL

// Argument for build_stl_str_hl_local().
రకనిర్వచనం గణన {
    STL_బహుళకం_SINGLE,	    // Does not accept line breaks "%@"
    STL_బహుళకం_MULTI,	    // Accept line breaks "%@"
    STL_బహుళకం_GET_RENDERED_ఎత్తు,   // Just get stl rendered height
#నిర్వచితమైతే ENABLE_STL_బహుళకం_MULTI_NL
    STL_బహుళకం_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#అయితే_ముగింపు
} stl_బహుళకం_T;

// Return value of match_keyprotocol()
రకనిర్వచనం గణన {
    KEYPROTOCOL_NONE,
    KEYPROTOCOL_MOK2,
    KEYPROTOCOL_KITTY,
    KEYPROTOCOL_FAIL
} keyprot_T;

// errors for when calling a function
రకనిర్వచనం గణన {
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
రకనిర్వచనం గణన {
    CPT_ABBR,		// "abbr"
    CPT_రకం,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_లెక్క,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
రకనిర్వచనం అక్షరం *(*opt_did_సమితి_cb_T)(optset_T *వాదనలు);

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
రకనిర్వచనం పూర్ణాంకం (*opt_expand_cb_T)(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches);

// Flags for assignment functions.
#నిర్వచించు కేటాయించు_VAR	0     // ":var" (nothing special)
#నిర్వచించు కేటాయించు_FINAL	0x01  // ":final"
#నిర్వచించు కేటాయించు_CONST	0x02  // ":const"
#నిర్వచించు కేటాయించు_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#నిర్వచించు కేటాయించు_DECL	0x08  // may declare variable if it does not exist
#నిర్వచించు కేటాయించు_UNPACK	0x10  // using [a, b] = list
#నిర్వచించు కేటాయించు_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#నిర్వచించు కేటాయించు_FOR_LOOP 0x40 // assigning to loop variable
#నిర్వచించు కేటాయించు_INIT	0x80 // not assigning a value, just a declaration
#నిర్వచించు కేటాయించు_UPDATE_ఖండం_ID 0x100  // update sav_block_id
#నిర్వచించు కేటాయించు_COMPOUND_OP 0x200  // compound operator e.g. "+="

#అన్తర్భావ "ex_cmds.h"	    // Ex command defines
#అన్తర్భావ "spell.h"	    // spell checking stuff

#అన్తర్భావ "proto.h"	    // function prototypes

// This has to go after the include of proto.h, as proto/gui.pro declares
// functions of these names. The declarations would break if the defines had
// been seen at that stage.  But it must be before globals.h, where error_ga
// is declared.
#ఒకవేళ !defined(MSWIN) && !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_HAIKU) \
	&& !defined(FEAT_GUI_GTK) && !defined(PROTO)
# define mch_errmsg(str)	fprintf(stderr, "%s", (str))
# define display_errors()	fflush(stderr)
# define mch_సందేశం(str)		రాయి("%s", (str))
#లేకపోతే
# define USE_MCH_ERRMSG
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_ఆదానం_పద్ధతి
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define సంఖ్య_వెడల్పు(x) 7
#అయితే_ముగింపు

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#ఒకవేళ !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#అయితే_ముగింపు

#అన్తర్భావ "globals.h"	    // global variables and messages
#అన్తర్భావ "errors.h"	    // error messages

#నిర్వచితం_కాకపోతే PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    స్థిర అంతఃవరుస పూర్ణాంకం
లోపల_vim9script(శూన్య)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    తిరిగి (current_sctx.sc_సంస్కరణ == SCRIPT_సంస్కరణ_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#అయితే_ముగింపు

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#ఒకవేళ defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#అయితే_ముగింపు

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#నిర్వచితమైతే FEAT_BROWSE
# ifdef BACKSLASH_లోపల_FILENAME
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*.*)\t*.*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVB code (*.bas, *.frm)\t*.bas;*.frm\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# లేకపోతే
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*)\t*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*)\t*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*)\t*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# endif
# define BROWSE_SAVE 1	    // flag for do_browse()
# define BROWSE_DIR 2	    // flag for do_browse()
#అయితే_ముగింపు

#నిర్వచితమైతే _MSC_VER
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
#అయితే_ముగింపు

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#ఒకవేళ defined(MEM_PROFILE)
# define vim_realloc(ptr, కొలత)  mem_realloc((ptr), (కొలత))
#లేకపోతే
# define vim_realloc(ptr, కొలత)  realloc((ptr), (కొలత))
#అయితే_ముగింపు

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#నిర్వచించు MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#నిర్వచించు MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#నిర్వచించు ENC_8_బిట్టు	0x01
#నిర్వచించు ENC_DBCS	0x02
#నిర్వచించు ENC_UNICODE	0x04

#నిర్వచించు ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#నిర్వచించు ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#నిర్వచించు ENC_2BYTE	0x40	    // Unicode: UCS-2
#నిర్వచించు ENC_4BYTE	0x80	    // Unicode: UCS-4
#నిర్వచించు ENC_2_మాట	0x100	    // Unicode: UTF-16

#నిర్వచించు ENC_LATIN1	0x200	    // Latin1
#నిర్వచించు ENC_LATIN9	0x400	    // Latin9
#నిర్వచించు ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#నిర్వచితమైతే USE_ICONV
# ifndef EILSEQ
#  define EILSEQ 123
# endif
# ifdef DYNAMIC_ICONV
// On Win32 iconv.dll is dynamically loaded.
#  define ICONV_ERRNO (*iconv_errno())
#  define ICONV_E2BIG  7
#  define ICONV_EINVAL 22
#  define ICONV_EILSEQ 42
# లేకపోతే
#  define ICONV_ERRNO errno
#  define ICONV_E2BIG  E2BIG
#  define ICONV_EINVAL EINVAL
#  define ICONV_EILSEQ EILSEQ
# endif
#అయితే_ముగింపు

#నిర్వచించు SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#నిర్వచితమైతే FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.లోపల_use ? (ద్విగుణ)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.లోపల_use ? (ద్విగుణ)(p) / gui.scale : (p))
#లేకపోతే
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.లోపల_use) ? gui_mch_get_display() : xterm_dpy)
# లేకపోతే
#  define X_DISPLAY	(gui.లోపల_use ? gui.dpy : xterm_dpy)
# endif
#లేదావేళ defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.లోపల_use) ? gui_mch_get_display() : (Display *)ఖాళీ)
# లేకపోతే
#  define X_DISPLAY	gui.dpy
# endif
#లేదావేళ defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#లేకపోతే
# define X_DISPLAY	(Display *)ఖాళీ
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
# ఒకవేళ !GTK_CHECK_సంస్కరణ(2,14,0)
#  define gtk_widget_get_కిటికీ(wid)	((wid)->కిటికీ_2)
#  define gtk_plug_get_socket_కిటికీ(wid)	((wid)->socket_కిటికీ)
#  define gtk_selection_దత్తాంశం_get_దత్తాంశం(sel)	((sel)->దత్తాంశం)
#  define gtk_selection_దత్తాంశం_get_దత్తాంశం_type(sel)	((sel)->type)
#  define gtk_selection_దత్తాంశం_get_format(sel)	((sel)->format)
#  define gtk_selection_దత్తాంశం_get_నిడివి(sel)	((sel)->నిడివి)
#  define gtk_adjustment_సమితి_క్రింది(adj, దిగువ) \
    చేయి { (adj)->క్రింది = దిగువ; } అయితే (0)
#  define gtk_adjustment_సమితి_పై(adj, పైకి) \
    చేయి { (adj)->పై_2 = పైకి; } అయితే (0)
#  define gtk_adjustment_సమితి_పుట_కొలత(adj, కొలత) \
    చేయి { (adj)->పుట_కొలత = కొలత; } అయితే (0)
#  define gtk_adjustment_సమితి_పుట_increment(adj, inc) \
    చేయి { (adj)->పుట_increment = inc; } అయితే (0)
#  define gtk_adjustment_సమితి_అడుగు_increment(adj, inc) \
    చేయి { (adj)->అడుగు_increment = inc; } అయితే (0)
# endif
# ఒకవేళ !GTK_CHECK_సంస్కరణ(2,16,0)
#  define gtk_selection_దత్తాంశం_get_selection(sel)	((sel)->selection)
# endif
# ఒకవేళ !GTK_CHECK_సంస్కరణ(2,18,0)
#  define gtk_widget_get_కేటాయింపు(wid, alloc) \
    చేయి { *(alloc) = (wid)->కేటాయింపు; } అయితే (0)
#  define gtk_widget_సమితి_కేటాయింపు(wid, alloc) \
    చేయి { (wid)->కేటాయింపు = *(alloc); } అయితే (0)
#  define gtk_widget_get_has_కిటికీ(wid)	!GTK_WIDGET_NO_కిటికీ(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_సమితి_కిటికీ(wid, win) \
    చేయి { (wid)->కిటికీ_2 = (win); } అయితే (0)
#  define gtk_widget_సమితి_can_default(wid, can) \
    చేయి { ఒకవేళ (can) \
	    { GTK_WIDGET_సమితి_FLAGS(wid, GTK_CAN_DEFAULT); } \
	లేకపోతే \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } అయితే (0)
#  define gtk_widget_సమితి_can_focus(wid, can) \
    చేయి { ఒకవేళ (can) \
	    { GTK_WIDGET_సమితి_FLAGS(wid, GTK_CAN_FOCUS); } \
	లేకపోతే \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } అయితే (0)
#  define gtk_widget_సమితి_visible(wid, vis) \
    చేయి { ఒకవేళ (vis) \
	    { gtk_widget_show(wid); } \
	లేకపోతే \
	    { gtk_widget_hide(wid); } } అయితే (0)
# endif
# ఒకవేళ !GTK_CHECK_సంస్కరణ(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_సమితి_mapped(wid, map) \
    చేయి { ఒకవేళ (map) \
	    { GTK_WIDGET_సమితి_FLAGS(wid, GTK_MAPPED); } \
	లేకపోతే \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } అయితే (0)
#  define gtk_widget_సమితి_realized(wid, rea) \
    చేయి { ఒకవేళ (rea) \
	    { GTK_WIDGET_సమితి_FLAGS(wid, GTK_REALIZED); } \
	లేకపోతే \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } అయితే (0)
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# ఒకవేళ GTK_CHECK_సంస్కరణ(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే FEAT_NETBEANS_INTG
# undef NBDEBUG
#అయితే_ముగింపు
#నిర్వచితమైతే NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#లేకపోతే
# define nbdebug(a)
#అయితే_ముగింపు

#నిర్వచితమైతే లోపల_PERL_FILE
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
# ఒకవేళ (defined(MACOS_X) && !defined(MAC_OS_X_సంస్కరణ_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#అయితే_ముగింపు

// values for vim_handle_signal() that are not a signal
#నిర్వచించు SIGNAL_ఖండం	(-1)
#నిర్వచించు SIGNAL_UNBLOCK  (-2)
#ఒకవేళ !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#అయితే_ముగింపు

// flags for skip_vimgrep_pat()
#నిర్వచించు VGR_GLOBAL	1
#నిర్వచించు VGR_NOJUMP	2
#నిర్వచించు VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#నిర్వచించు BAD_REPLACE	'?'	// replace it with '?' (default)
#నిర్వచించు BAD_KEEP	(-1)	// leave it
#నిర్వచించు BAD_DROP	(-2)	// erase it

// last argument for do_source()
#నిర్వచించు DOSO_NONE	0
#నిర్వచించు DOSO_VIMRC	1	// loading vimrc file
#నిర్వచించు DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#నిర్వచించు VIF_WANT_INFO	    1	// load non-mark info
#నిర్వచించు VIF_WANT_MARKS	    2	// load file marks
#నిర్వచించు VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#నిర్వచించు VIF_FORCEIT	    8	// overwrite info already read
#నిర్వచించు VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#నిర్వచించు BFA_DEL		 1	// buffer is going to be deleted
#నిర్వచించు BFA_WIPE	 2	// buffer is going to be wiped out
#నిర్వచించు BFA_KEEP_UNDO	 4	// do not free undo information
#నిర్వచించు BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#నిర్వచించు MSCR_కిందికి	0	// DOWN must be FALSE
#నిర్వచించు MSCR_పైకి		1
#నిర్వచించు MSCR_ఎడమ	(-1)
#నిర్వచించు MSCR_కుడి	(-2)

#నిర్వచించు KEYLEN_PART_కీలకం (-1)	// keylen value for incomplete key-code
#నిర్వచించు KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#నిర్వచించు KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#నిర్వచించు FILEINFO_OK	     0
#నిర్వచించు FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#నిర్వచించు FILEINFO_READ_FAIL   2	// CreateFile() failed
#నిర్వచించు FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#నిర్వచించు SOPT_BOOL	0x01	// Boolean option
#నిర్వచించు SOPT_అంకె	0x02	// Number option
#నిర్వచించు SOPT_STRING	0x04	// String option
#నిర్వచించు SOPT_GLOBAL	0x08	// Option has global value
#నిర్వచించు SOPT_WIN	0x10	// Option has window-local value
#నిర్వచించు SOPT_BUF	0x20	// Option has buffer-local value
#నిర్వచించు SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#నిర్వచించు SREQ_GLOBAL	0	// Request global option
#నిర్వచించు SREQ_WIN	1	// Request window-local option
#నిర్వచించు SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#నిర్వచించు GREG_NO_సమాసం	1	// Do not allow expression register
#నిర్వచించు GREG_సమాసం_SRC	2	// Return expression itself for "=" register
#నిర్వచించు GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#నిర్వచించు AUTOLOAD_CHAR '#'

#నిర్వచితమైతే FEAT_పని_CHANNEL
# define MAX_OPEN_CHANNELS 10
#లేకపోతే
# define MAX_OPEN_CHANNELS 0
#అయితే_ముగింపు

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#నిర్వచితమైతే FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#లేకపోతే
# define MAX_CLIENT_CHANNELS 0
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
# define MAX_NAMED_PIPE_కొలత 65535
#అయితే_ముగింపు

// Options for json_encode() and json_decode.
#నిర్వచించు JSON_JS		1   // use JS instead of JSON
#నిర్వచించు JSON_NO_NONE	2   // v:none item not allowed
#నిర్వచించు JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#నిర్వచించు DIP_ALL	    0x01	// all matches, not just the first one
#నిర్వచించు DIP_DIR	    0x02	// find directories instead of files.
#నిర్వచించు DIP_ERR	    0x04	// give an error message when none found.
#నిర్వచించు DIP_ప్రారంభం   0x08	// also use "start" directory in 'packpath'
#నిర్వచించు DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#నిర్వచించు DIP_NORTP   0x20	// do not use 'runtimepath'
#నిర్వచించు DIP_NOAFTER 0x40	// skip "after" directories
#నిర్వచించు DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#నిర్వచించు అత్యల్ప_WIN_ID 1000

// Used by the garbage collector.
#నిర్వచించు COPYID_INC 2
#నిర్వచించు COPYID_ముసుగు (~0x1)

// Values for trans_function_name() argument:
#నిర్వచించు TFN_INT		0x01	// internal function name OK
#నిర్వచించు TFN_QUIET	0x02	// no error messages
#నిర్వచించు TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#నిర్వచించు TFN_NO_DEREF	0x08	// do not dereference a Funcref
#నిర్వచించు TFN_READ_ONLY	0x10	// will not change the var
#నిర్వచించు TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#నిర్వచించు TFN_COMPILING	0x40	// only used for GLV_COMPILING
#నిర్వచించు TFN_NEW_FUNC	0x80	// defining a new function
#నిర్వచించు TFN_కేటాయించు_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#నిర్వచించు TFN_లోపల_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#నిర్వచించు GLV_QUIET	TFN_QUIET	// no error messages
#నిర్వచించు GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#నిర్వచించు GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#నిర్వచించు GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#నిర్వచించు GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#నిర్వచించు GLV_కేటాయించు_WITH_OP TFN_కేటాయించు_WITH_OP // assignment with operator
#నిర్వచించు GLV_PREFER_FUNC	0x10000		// prefer function above variable
#నిర్వచించు GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#నిర్వచించు DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#నిర్వచించు FLEN_FIXED 40

// flags for find_name_end()
#నిర్వచించు FNE_INCL_BR	1	// include [] in name
#నిర్వచించు FNE_CHECK_ప్రారంభం	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#ఒకవేళ (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#అయితే_ముగింపు

// stat macros
#నిర్వచితం_కాకపోతే S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# లేకపోతే
#  define S_ISDIR(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# లేకపోతే
#  define S_ISREG(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# లేకపోతే
#  define S_ISBLK(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# లేకపోతే
#  define S_ISSOCK(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# లేకపోతే
#  define S_ISFIFO(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# లేకపోతే
#  define S_ISCHR(m)	0
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# లేకపోతే
#  define S_ISLNK(m)	0
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_సమయం_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), ఖాళీ)
# define ELAPSED_FUNC(v) elapsed(&(v))
రకనిర్వచనం నిర్మాణం timeval elapsed_T;
దీర్ఘ elapsed(నిర్మాణం timeval *ప్రారంభం_tv);
#లేదావేళ defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_లెక్క()
# define ELAPSED_FUNC(v) elapsed(v)
రకనిర్వచనం DWORD elapsed_T;
# ifndef PROTO
దీర్ఘ elapsed(DWORD ప్రారంభం_tick);
# endif
#అయితే_ముగింపు

// Replacement for nchar used by nv_replace().
#నిర్వచించు REPLACE_CR_NCHAR    (-1)
#నిర్వచించు REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#నిర్వచించు పదం_ప్రారంభం_NOJOB	1
#నిర్వచించు పదం_ప్రారంభం_FORCEIT	2
#నిర్వచించు పదం_ప్రారంభం_SYSTEM	4

// Used for icon/title save and restore.
#నిర్వచించు SAVE_RESTORE_TITLE	1
#నిర్వచించు SAVE_RESTORE_ICON	2
#నిర్వచించు SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#నిర్వచించు APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#నిర్వచించు APC_SUBSTITUTE		2   // text is replaced, not inserted
#నిర్వచించు APC_INDENT		4   // changing indent

#నిర్వచించు CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#నిర్వచించు REPTERM_FROM_PART	1
#నిర్వచించు REPTERM_DO_LT		2
#నిర్వచించు REPTERM_SPECIAL		4
#నిర్వచించు REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#నిర్వచించు FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#నిర్వచించు FSK_KEEP_X_కీలకం	0x02	// don't translate xHome to Home key
#నిర్వచించు FSK_లోపల_STRING	0x04	// TRUE in string, double quote is escaped
#నిర్వచించు FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#నిర్వచించు FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#నిర్వచించు READDIR_క్రమీకరించు_NONE	0  // do not sort
#నిర్వచించు READDIR_క్రమీకరించు_BYTE	1  // sort by byte order (strcmp), default
#నిర్వచించు READDIR_క్రమీకరించు_IC		2  // sort ignoring case (strcasecmp)
#నిర్వచించు READDIR_క్రమీకరించు_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#నిర్వచించు MCH_DELAY_IGNOREINPUT	1
#నిర్వచించు MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#నిర్వచించు EVAL_VAR_VERBOSE	1   // may give error message
#నిర్వచించు EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#నిర్వచించు EVAL_VAR_IMPORT		4   // may return special variable for import
#నిర్వచించు EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#నిర్వచించు FUZZY_సరిపోలిక_MAX_LEN	1024    // max characters that can be matched
#నిర్వచించు FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#నిర్వచించు ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#నిర్వచించు UC_బఫరు	1	// -buffer: local to current buffer
#నిర్వచించు UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#నిర్వచించు UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#నిర్వచించు VSE_NONE	0
#నిర్వచించు VSE_SHELL	1	// escape for a shell command
#నిర్వచించు VSE_బఫరు	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#నిర్వచించు FFED_IS_GLOBAL	1	// "g:" was used
#నిర్వచించు FFED_NO_GLOBAL	2	// only check for script-local functions

#నిర్వచించు MAX_LSHIFT_బిట్లు (varnumber_T)((పరిమాణం(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#నిర్వచించు CF_CLASS	1	// inside a class
#నిర్వచించు CF_INTERFACE	2	// inside an interface
#నిర్వచించు CF_ABSTRACT_పద్ధతి	4	// inside an abstract class

// Flags used by getvcol()
#నిర్వచించు GETVCOL_చివర_EXCL_LBR	1
#నిర్వచించు GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#నిర్వచించు GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#నిర్వచితమైతే VMS
# define మార్గం_ESC_WILDCARDS	"*?%"
#లేకపోతే
# ifdef MSWIN
#  define మార్గం_ESC_WILDCARDS	"*?["
# లేకపోతే
#  define మార్గం_ESC_WILDCARDS	"*?[{"
# endif
#అయితే_ముగింపు

#అయితే_ముగింపు // VIM__H
