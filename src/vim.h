/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#که_نه_تعریف_شوی VIM__H
#تعریف_کړه VIM__H

#ضمن "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#إذا defined(WIN32) || defined(_WIN32)
# define MSWIN
#د_که_پای

#إذا defined(MSWIN)
# include <io.h>
#د_که_پای

// ============ the header file puzzle: order matters =========

#که_تعریف_شوی HAVE_CONFIG_H	// GNU autoconf (or something else) was here
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
#  error configure did not پرلپسې properly.  Check خپلکاری/config.لوګاریتم.
# endif

# إذا (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_سرچینه
#   define XOPEN_سرچینه    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_سرچینه
#    define BSD_سرچینه 1
#   endif
#   ifndef SVID_سرچینه
#    define SVID_سرچینه 1
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
#د_که_پای

// for INT_MAX, LONG_MAX et al.
#ضمن <limits.h>

#إذا !defined(UINT32_TYPEDEF)
# إذا defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# وإلا
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF بینښه صحيح
# endif
#د_که_پای

// C89 does not define SIZE_MAX
#إذا defined(__hpux) || defined(VMS)
# ifndef اندازه_MAX
#  define اندازه_MAX ((size_t)(-1))
# endif
#د_که_پای

// user ID of root is usually zero, but not for everybody
#که_تعریف_شوی __TANDEM
# ifndef TANDEM_سرچینه
#  define TANDEM_سرچینه
# endif
# include <floss.h>
# define ریښه_UID 65535
# define OLDXAW
# إذا (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#وإلا
# define ریښه_UID 0
#د_که_پای

/* Include MAC_OS_X_VERSION_* macros */
#که_تعریف_شوی HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#د_که_پای

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#إذا defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#د_که_پای
// Unless made through the Makefile enforce GUI on Mac
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#د_که_پای

#إذا defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# إذا !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#د_که_پای

// Check support for rendering options
#که_تعریف_شوی FEAT_GUI
# إذا defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#د_که_پای

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#که_تعریف_شوی MSWIN
# define VIM_SIZEOF_INT 4
#د_که_پای

#که_تعریف_شوی AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  وإلا
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#د_که_پای
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#د_که_پای

#إذا VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only کار_کوي with 32 بټ صحيح or larger
#د_که_پای

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#ضمن "feature.h"

#إذا defined(MACOS_X_DARWIN)
# إذا defined(FEAT_نورمال) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# إذا defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_نسخه_MIN_REQUIRED) && MAC_OS_X_نسخه_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# إذا defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#د_که_پای

// +x11 is only enabled when it's both available and wanted.
#إذا defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#د_که_پای

#إذا defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#د_که_پای

#که_تعریف_شوی NO_X11_INCLUDES
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
#د_که_پای

// The Mac conversion stuff doesn't work under X11.
#إذا defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#د_که_پای

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#که_نه_تعریف_شوی VIMPACKAGE
# define VIMPACKAGE	"vim"
#د_که_پای

/*
 * Find out if function definitions should include argument types
 */
#که_تعریف_شوی AZTEC_C
# include <functions.h>
#د_که_پای

#که_تعریف_شوی SASC
# include <clib/exec_protos.h>
#د_که_پای

#که_تعریف_شوی _DCC
# include <clib/exec_protos.h>
#د_که_پای

#که_نه_تعریف_شوی PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#د_که_پای

#إذا (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#وإلاإذا !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) إشارة(signum, sighandler)
#د_که_پای

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#که_نه_تعریف_شوی ناکارول_شوی
# إذا defined(HAVE_ATTRIBUTE_ناکارول_شوی) || defined(__MINGW32__)
#  define ناکارول_شوی __attribute__((ناکارول_شوی_2))
# وإلا
#  إذا defined __has_attribute
#   إذا __has_attribute(ناکارول_شوی_2)
#    define ناکارول_شوی __attribute__((ناکارول_شوی_2))
#   endif
#  endif
# endif
# ifndef ناکارول_شوی
#  define ناکارول_شوی
# endif
#د_که_پای

// Used to check for "sun", "__sun" is used by newer compilers.
#إذا defined(__sun)
# define SUN_SYSTEM
#د_که_پای

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#إذا !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#د_که_پای

#که_نه_تعریف_شوی PROTO
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
#د_که_پای

#که_تعریف_شوی X_LOCALE
# include <X11/Xlocale.h>
#وإلا
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#د_که_پای

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#که_نه_تعریف_شوی MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# وإلا
#  define MAXPATHL  256
# endif
#د_که_پای
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
# define لار_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#وإلا
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define لار_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# وإلا
#  define لار_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#د_که_پای

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#تعریف_کړه NUMBUFLEN 65

// flags for vim_str2nr()
#تعریف_کړه STR2NR_BIN  0x01
#تعریف_کړه STR2NR_OCT  0x02
#تعریف_کړه STR2NR_HEX  0x04
#تعریف_کړه STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#تعریف_کړه STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#تعریف_کړه STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#تعریف_کړه STR2NR_FORCE 0x80   // only when ONE of the above is used

#تعریف_کړه STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
تعريفنوع بینښه حرف	char_u;
تعريفنوع بینښه لنډ	short_u;
تعريفنوع بینښه صحيح	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#که_تعریف_شوی _WIN64
تعريفنوع بینښه __int64	long_u;
تعريفنوع		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#وإلا
تعريفنوع بینښه اوږد		long_u;
تعريفنوع		 اوږد		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#د_که_پای
#تعریف_کړه PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#إذا defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#د_که_پای

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
#  define vim_fseek(a, b, c)	fseek(a, (اوږد)b, c)
# endif
#د_که_پای

/*
 * The characters and attributes cached for the screen.
 */
تعريفنوع char_u schar_T;
تعريفنوع بینښه لنډ sattr_T;
#تعریف_کړه MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
تعريفنوع بینښه صحيح u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#إذا defined(VMS) && defined(VAX)
// unsupported compiler
تعريفنوع اوږد      vimlong_T;
#وإلا
تعريفنوع اوږد اوږد vimlong_T;
#د_که_پای

#که_نه_تعریف_شوی UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#د_که_پای

#ضمن "ascii.h"
#ضمن "keymap.h"
#ضمن "termdefs.h"
#ضمن "macros.h"

#که_تعریف_شوی LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#د_که_پای
#که_تعریف_شوی _DCC
# include <sys/stat.h>
#د_که_پای
#إذا defined(MSWIN)
# include <sys/stat.h>
#د_که_پای

#إذا defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#د_که_پای

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#که_نه_تعریف_شوی UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# إذا defined(HAVE_مزي_ګانې_H) && !defined(NO_مزي_ګانې_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#د_که_پای // NON-UNIX

#ضمن <assert.h>

#که_تعریف_شوی HAVE_STDINT_H
# include <stdint.h>
#د_که_پای
#که_تعریف_شوی HAVE_INTTYPES_H
# include <inttypes.h>
#د_که_پای
#که_تعریف_شوی HAVE_WCTYPE_H
# include <wctype.h>
#د_که_پای
#ضمن <stdarg.h>
// older compilers do not define va_copy
#که_نه_تعریف_شوی va_copy
# define va_copy(dst, src)	((dst) = (src))
#د_که_پای

// for offsetof()
#ضمن <stddef.h>
#ضمن <stdbool.h>

#إذا defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_وخت_H) || defined(SYS_SELECT_WITH_SYS_وخت))
# include <sys/select.h>
#د_که_پای

#که_نه_تعریف_شوی HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# وإلا
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#د_که_پای

#که_تعریف_شوی HAVE_SODIUM
# include <sodium.h>
#د_که_پای

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#که_تعریف_شوی DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#د_که_پای

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#که_تعریف_شوی DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
خارجي حرف *(*dyn_libintl_gettext)(ثابت حرف *msgid);
خارجي حرف *(*dyn_libintl_ngettext)(ثابت حرف *msgid, ثابت حرف *msgid_plural, بینښه اوږد n);
خارجي حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *domainname, ثابت حرف *dirname);
خارجي حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *domainname, ثابت حرف *codeset);
خارجي حرف *(*dyn_libintl_textdomain)(ثابت حرف *domainname);
خارجي صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *envstring);
خارجي صحيح *dyn_libintl_nl_پیغام_cat_cntr;
#د_که_پای


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#که_تعریف_شوی FEAT_GETTEXT
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
# define bind_textdomain_codeset(x, y) //تشه
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //تشه
#د_که_پای

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#تعریف_کړه UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#تعریف_کړه UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#تعریف_کړه UPD_INVERTED		20  // redisplay inverted part that changed
#تعریف_کړه UPD_INVERTED_ALL	25  // redisplay whole inverted part
#تعریف_کړه UPD_REDRAW_سر		30  // display first w_upd_rows screen lines
#تعریف_کړه UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#تعریف_کړه UPD_NOT_VALID		40  // buffer needs complete redraw
#تعریف_کړه UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#تعریف_کړه SLF_RIGHTLEFT	1
#تعریف_کړه SLF_POPUP	2
#تعریف_کړه SLF_INC_VCOL	4

#تعریف_کړه MB_FILLER_CHAR '<'  // character used when a double-width character
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
#تعریف_کړه VALID_WROW	0x01	// w_wrow (window row) is valid
#تعریف_کړه VALID_WCOL	0x02	// w_wcol (window col) is valid
#تعریف_کړه VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#تعریف_کړه VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#تعریف_کړه VALID_CROW	0x10	// w_cline_row is valid
#تعریف_کړه VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#تعریف_کړه VALID_BOTLINE_AP 0x40	// w_botline is approximated
#تعریف_کړه VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#تعریف_کړه POPF_IS_POPUP	0x01	// this is a popup window
#تعریف_کړه POPF_HIDDEN	0x02	// popup is not displayed
#تعریف_کړه POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#تعریف_کړه POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#تعریف_کړه POPF_ON_CMDLINE	0x10	// popup overlaps command line
#تعریف_کړه POPF_DRAG	0x20	// popup can be moved by dragging border
#تعریف_کړه POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#تعریف_کړه POPF_RESIZE	0x80	// popup can be resized by dragging
#تعریف_کړه POPF_MAPPING	0x100	// mapping keys
#تعریف_کړه POPF_INFO	0x200	// used for info of popup menu
#تعریف_کړه POPF_INFO_MENU	0x400	// align info popup with popup menu
#تعریف_کړه POPF_POSINVERT	0x800	// vertical position can be inverted
#تعریف_کړه POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#تعریف_کړه POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#تعریف_کړه POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#تعریف_کړه POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#تعریف_کړه POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#تعریف_کړه POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#تعریف_کړه POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#تعریف_کړه POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#تعریف_کړه HL_نورمال		0x00
#تعریف_کړه HL_INVERSE		0x01
#تعریف_کړه HL_BOLD			0x02
#تعریف_کړه HL_ITALIC		0x04
#تعریف_کړه HL_UNDERLINE		0x08
#تعریف_کړه HL_UNDERCURL		0x10
#تعریف_کړه HL_UNDERDOUBLE		0x20
#تعریف_کړه HL_UNDERDOTTED		0x40
#تعریف_کړه HL_UNDERDASHED		0x80
#تعریف_کړه HL_STANDOUT		0x100
#تعریف_کړه HL_NOCOMBINE		0x200
#تعریف_کړه HL_STRIKETHROUGH	0x400
#تعریف_کړه HL_ALL			0x7ff

// special attribute addition: Put message in history
#تعریف_کړه پیغام_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#تعریف_کړه ډېرپېښ_نورمال	0x01	// Normal mode, command expected
#تعریف_کړه ډېرپېښ_VISUAL	0x02	// Visual mode - use get_real_state()
#تعریف_کړه ډېرپېښ_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#تعریف_کړه ډېرپېښ_CMDLINE	0x08	// Editing the command line
#تعریف_کړه ډېرپېښ_ورننباسه	0x10	// Insert mode, also for Replace mode
#تعریف_کړه ډېرپېښ_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#تعریف_کړه ډېرپېښ_SELECT	0x40	// Select mode, use get_real_state()
#تعریف_کړه ډېرپېښ_TERMINAL	0x80	// Terminal mode

#تعریف_کړه MAP_ALL_MODES	0xff    // all mode bits used for mapping

#تعریف_کړه REPLACE_FLAG	0x100	// Replace mode flag
#تعریف_کړه ډېرپېښ_REPLACE	(REPLACE_FLAG | ډېرپېښ_ورننباسه)
#تعریف_کړه VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#تعریف_کړه ډېرپېښ_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | ډېرپېښ_ورننباسه)
#تعریف_کړه ډېرپېښ_LREPLACE	(REPLACE_FLAG | ډېرپېښ_LANGMAP)

#تعریف_کړه ډېرپېښ_نورمال_BUSY (0x1000 | ډېرپېښ_نورمال)
				// Normal mode, busy with a command
#تعریف_کړه ډېرپېښ_HITRETURN	(0x2000 | ډېرپېښ_نورمال)
				// waiting for return or command
#تعریف_کړه ډېرپېښ_ASKMORE	0x3000	// Asking if you want --more--
#تعریف_کړه ډېرپېښ_SETWSIZE	0x4000	// window size has changed
#تعریف_کړه ډېرپېښ_EXTERNCMD	0x5000	// executing an external command
#تعریف_کړه ډېرپېښ_SHOWMATCH	(0x6000 | ډېرپېښ_ورننباسه) // show matching paren
#تعریف_کړه ډېرپېښ_CONFIRM	0x7000	// ":confirm" prompt
#تعریف_کړه ډېرپېښ_ALL	0xffff

#تعریف_کړه ډېرپېښ_MAX_اوږدتیا	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#تعریف_کړه FORWARD			1
#تعریف_کړه BACKWARD		(-1)
#تعریف_کړه FORWARD_FILE		3
#تعریف_کړه BACKWARD_FILE		(-3)

// return values for functions
#إذا !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#د_که_پای
#تعریف_کړه FAIL			0
#تعریف_کړه NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#تعریف_کړه BF_RECOVERED	0x01	// buffer has been recovered
#تعریف_کړه BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#تعریف_کړه BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#تعریف_کړه BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#تعریف_کړه BF_NEW		0x10	// file didn't exist when editing started
#تعریف_کړه BF_NEW_W	0x20	// Warned for BF_NEW and file created
#تعریف_کړه BF_READERR	0x40	// got errors while reading the file
#تعریف_کړه BF_DUMMY	0x80	// dummy buffer, only used internally
#تعریف_کړه BF_PRESERVED	0x100	// ":preserve" was used
#تعریف_کړه BF_SYN_سېټ	0x200	// 'syntax' option was set
#تعریف_کړه BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#تعریف_کړه BF_WRITE_ماسک	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#تعریف_کړه EXPAND_UNSUCCESSFUL	(-2)
#تعریف_کړه EXPAND_OK		(-1)
#تعریف_کړه EXPAND_NOTHING		0
#تعریف_کړه EXPAND_COMMANDS		1
#تعریف_کړه EXPAND_FILES		2
#تعریف_کړه EXPAND_DIRECTORIES	3
#تعریف_کړه EXPAND_SETTINGS		4
#تعریف_کړه EXPAND_BOOL_SETTINGS	5
#تعریف_کړه EXPAND_نښانونه		6
#تعریف_کړه EXPAND_OLD_SETTING	7
#تعریف_کړه EXPAND_HELP		8
#تعریف_کړه EXPAND_BUFFERS		9
#تعریف_کړه EXPAND_EVENTS		10
#تعریف_کړه EXPAND_MENUS		11
#تعریف_کړه EXPAND_SYNTAX		12
#تعریف_کړه EXPAND_HIGHLIGHT	13
#تعریف_کړه EXPAND_AUGROUP		14
#تعریف_کړه EXPAND_USER_VARS	15
#تعریف_کړه EXPAND_MAPPINGS		16
#تعریف_کړه EXPAND_نښانونه_LISTFILES	17
#تعریف_کړه EXPAND_FUNCTIONS	18
#تعریف_کړه EXPAND_USER_FUNC	19
#تعریف_کړه EXPAND_افاده	20
#تعریف_کړه EXPAND_MENUNAMES	21
#تعریف_کړه EXPAND_USER_COMMANDS	22
#تعریف_کړه EXPAND_USER_CMD_FLAGS	23
#تعریف_کړه EXPAND_USER_NARGS	24
#تعریف_کړه EXPAND_USER_COMPLETE	25
#تعریف_کړه EXPAND_ENV_VARS		26
#تعریف_کړه EXPAND_LANGUAGE		27
#تعریف_کړه EXPAND_COLORS		28
#تعریف_کړه EXPAND_COMPILER		29
#تعریف_کړه EXPAND_USER_DEFINED	30
#تعریف_کړه EXPAND_USER_LIST	31
#تعریف_کړه EXPAND_SHELLCMD		32
#تعریف_کړه EXPAND_CSCOPE		33
#تعریف_کړه EXPAND_SIGN		34
#تعریف_کړه EXPAND_PROFILE		35
#تعریف_کړه EXPAND_BEHAVE		36
#تعریف_کړه EXPAND_FILETYPE		37
#تعریف_کړه EXPAND_FILES_دننه_لار	38
#تعریف_کړه EXPAND_OWNSYNTAX	39
#تعریف_کړه EXPAND_LOCALES		40
#تعریف_کړه EXPAND_HISTORY		41
#تعریف_کړه EXPAND_USER		42
#تعریف_کړه EXPAND_SYNTIME		43
#تعریف_کړه EXPAND_USER_ADDR_TYPE	44
#تعریف_کړه EXPAND_PACKADD		45
#تعریف_کړه EXPAND_MESSAGES		46
#تعریف_کړه EXPAND_MAPCLEAR		47
#تعریف_کړه EXPAND_ARGLIST		48
#تعریف_کړه EXPAND_DIFF_BUFFERS	49
#تعریف_کړه EXPAND_DISASSEMBLE	50
#تعریف_کړه EXPAND_BREAKPOINT	51
#تعریف_کړه EXPAND_SCRIPTNAMES	52
#تعریف_کړه EXPAND_RUNTIME		53
#تعریف_کړه EXPAND_STRING_SETTING	54
#تعریف_کړه EXPAND_SETTING_کمول	55
#تعریف_کړه EXPAND_ARGOPT		56
#تعریف_کړه EXPAND_TERMINALOPT	57
#تعریف_کړه EXPAND_KEYMAP		58
#تعریف_کړه EXPAND_DIRS_دننه_CDPATH	59
#تعریف_کړه EXPAND_SHELLCMDLINE	60
#تعریف_کړه EXPAND_FINDFUNC		61
#تعریف_کړه EXPAND_HIGHLIGHT_ډله  62
#تعریف_کړه EXPAND_FILETYPECMD	63
#تعریف_کړه EXPAND_نمونه_دننه_BUF	64
#تعریف_کړه EXPAND_RETAB		65
#تعریف_کړه EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#تعریف_کړه EXMODE_نورمال		1
#تعریف_کړه EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#تعریف_کړه WILD_FREE		1
#تعریف_کړه WILD_EXPAND_FREE	2
#تعریف_کړه WILD_EXPAND_KEEP	3
#تعریف_کړه WILD_بل		4
#تعریف_کړه WILD_PREV		5
#تعریف_کړه WILD_ALL		6
#تعریف_کړه WILD_LONGEST		7
#تعریف_کړه WILD_ALL_KEEP		8
#تعریف_کړه WILD_CANCEL		9
#تعریف_کړه WILD_APPLY		10
#تعریف_کړه WILD_PAGEUP		11
#تعریف_کړه WILD_PAGEDOWN		12

#تعریف_کړه WILD_LIST_NOTFOUND	    0x01
#تعریف_کړه WILD_HOME_REPLACE	    0x02
#تعریف_کړه WILD_USE_NL		    0x04
#تعریف_کړه WILD_NO_BEEP		    0x08
#تعریف_کړه WILD_جمع_کړه_SLASH		    0x10
#تعریف_کړه WILD_KEEP_ALL		    0x20
#تعریف_کړه WILD_SILENT		    0x40
#تعریف_کړه WILD_ESCAPE		    0x80
#تعریف_کړه WILD_ICASE		    0x100
#تعریف_کړه WILD_ALLLINKS		    0x200
#تعریف_کړه WILD_IGNORE_COMPLETESLASH   0x400
#تعریف_کړه WILD_NOERROR		    0x800  // sets EW_NOERROR
#تعریف_کړه WILD_BUFLASTUSED	    0x1000
#تعریف_کړه BUF_DIFF_FILTER		    0x2000
#تعریف_کړه WILD_NOSELECT		    0x4000
#تعریف_کړه WILD_MAY_EXPAND_نمونه	    0x8000
#تعریف_کړه WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#تعریف_کړه WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#تعریف_کړه EW_DIR		0x01	// include directory names
#تعریف_کړه EW_FILE		0x02	// include file names
#تعریف_کړه EW_NOTFOUND	0x04	// include not found names
#تعریف_کړه EW_ADDSLASH	0x08	// append slash to directory name
#تعریف_کړه EW_KEEPALL	0x10	// keep all matches
#تعریف_کړه EW_SILENT	0x20	// don't print "1 returned" from shell
#تعریف_کړه EW_EXEC		0x40	// executable files
#تعریف_کړه EW_لار		0x80	// search in 'path' too
#تعریف_کړه EW_ICASE	0x100	// ignore case
#تعریف_کړه EW_NOERROR	0x200	// no error for bad regexp
#تعریف_کړه EW_NOTWILD	0x400	// add match with literal name if exists
#تعریف_کړه EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#تعریف_کړه EW_ALLLINKS	0x1000	// also links not pointing to existing file
#تعریف_کړه EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#تعریف_کړه EW_DODOT	0x4000	// also files starting with a dot
#تعریف_کړه EW_EMPTYOK	0x8000	// no matches is not an error
#تعریف_کړه EW_NOTENV	0x10000	// do not expand environment variables
#تعریف_کړه EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#تعریف_کړه FINDFILE_FILE	0	// only files
#تعریف_کړه FINDFILE_DIR	1	// only directories
#تعریف_کړه FINDFILE_BOTH	2	// files and directories

#إذا defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_پلنوالی())
# define TPL_LCOL()			tabpanel_leftcol()
#وإلا
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#د_که_پای

#تعریف_کړه W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_پلنوالی)
#که_تعریف_شوی FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_لوړوالی)
#وإلا
# define W_WINROW(wp)	(wp->w_winrow)
#د_که_پای

// Values for the find_pattern_in_path() function args 'type' and 'action':
#تعریف_کړه پیدا_کړه_ANY	1
#تعریف_کړه پیدا_کړه_DEFINE	2
#تعریف_کړه CHECK_لار	3

#تعریف_کړه ACTION_SHOW	1
#تعریف_کړه ACTION_GOTO	2
#تعریف_کړه ACTION_وېشل	3
#تعریف_کړه ACTION_SHOW_ALL	4
#تعریف_کړه ACTION_EXPAND	5

#که_تعریف_شوی FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_حالتونه	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_دلته	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_سمون	0x40	// use match ID instead of item ID
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
#د_که_پای

// Values for 'options' argument in do_search() and searchit()
#تعریف_کړه پلټنه_REV    0x01  // go in reverse of previous dir.
#تعریف_کړه پلټنه_ECHO   0x02  // echo the search command and handle options
#تعریف_کړه پلټنه_پیغام    0x0c  // give messages (yes, it's not 0x04)
#تعریف_کړه پلټنه_NFMSG  0x08  // give all messages except not found
#تعریف_کړه پلټنه_OPT    0x10  // interpret optional flags
#تعریف_کړه پلټنه_HIS    0x20  // put search pattern in history
#تعریف_کړه پلټنه_پای_ټکی    0x40  // put cursor at end of match
#تعریف_کړه پلټنه_NOOF   0x80  // don't add offset to position
#تعریف_کړه پلټنه_پیل 0x100  // start search without col offset
#تعریف_کړه پلټنه_MARK  0x200  // set previous context mark
#تعریف_کړه پلټنه_KEEP  0x400  // keep previous search pattern
#تعریف_کړه پلټنه_PEEK  0x800  // peek for typed char, cancel search
#تعریف_کړه پلټنه_ستون  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#تعریف_کړه پیدا_کړه_IDENT	1	// find identifier (word)
#تعریف_کړه پیدا_کړه_STRING	2	// find any string (WORD)
#تعریف_کړه پیدا_کړه_EVAL	4	// include "->", "[]" and "."
#تعریف_کړه پیدا_کړه_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#تعریف_کړه FNAME_MESS	1	// give error message
#تعریف_کړه FNAME_EXP	2	// expand to path
#تعریف_کړه FNAME_HYP	4	// check for hypertext link
#تعریف_کړه FNAME_INCL	8	// apply 'includeexpr'
#تعریف_کړه FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#تعریف_کړه FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#تعریف_کړه GETF_SETMARK	0x01	// set pcmark before jumping
#تعریف_کړه GETF_ALT	0x02	// jumping to alternate file (not buf num)
#تعریف_کړه GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#تعریف_کړه GETFILE_ERROR	    1	// normal error
#تعریف_کړه GETFILE_NOT_WRITTEN 2	// "not written" error
#تعریف_کړه GETFILE_SAME_FILE   0	// success, same file
#تعریف_کړه GETFILE_OPEN_OTHER (-1)	// success, opened another file
#تعریف_کړه GETFILE_ناکارول_شوی	    8
#تعریف_کړه GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#تعریف_کړه BLN_CURBUF	1	// may re-use curbuf for new buffer
#تعریف_کړه BLN_LISTED	2	// put new buffer in buffer list
#تعریف_کړه BLN_DUMMY	4	// allocating dummy buffer
#تعریف_کړه BLN_NEW		8	// create a new buffer
#تعریف_کړه BLN_NOOPT	16	// don't copy options to existing buffer
#تعریف_کړه BLN_DUMMY_OK	32	// also find an existing dummy buffer
#تعریف_کړه BLN_REUSE	64	// may re-use number from buf_reuse
#تعریف_کړه BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#تعریف_کړه کلی_OPEN_FORW	0x101
#تعریف_کړه کلی_OPEN_شا	0x102
#تعریف_کړه کلی_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#تعریف_کړه MAPTYPE_MAP		0
#تعریف_کړه MAPTYPE_UNMAP		1
#تعریف_کړه MAPTYPE_NOREMAP		2
#تعریف_کړه MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#تعریف_کړه REMAP_YES	0	// allow remapping
#تعریف_کړه REMAP_NONE	(-1)	// no remapping
#تعریف_کړه REMAP_SCRIPT	(-2)	// remap script-local mappings only
#تعریف_کړه REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#تعریف_کړه SHELL_FILTER	1	// filtering text
#تعریف_کړه SHELL_EXPAND	2	// expanding wildcards
#تعریف_کړه SHELL_COOKED	4	// set term to cooked mode
#تعریف_کړه SHELL_DOOUT	8	// redirecting output
#تعریف_کړه SHELL_SILENT	16	// don't print error returned by command
#تعریف_کړه SHELL_READ	32	// read lines and insert into buffer
#تعریف_کړه SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#تعریف_کړه غوټه_نورمال	0	// file or directory, check with mch_isdir()
#تعریف_کړه غوټه_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#تعریف_کړه غوټه_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#تعریف_کړه READ_NEW	0x01	// read a file into a new buffer
#تعریف_کړه READ_FILTER	0x02	// read filter output
#تعریف_کړه READ_STDIN	0x04	// read from stdin
#تعریف_کړه READ_بفر	0x08	// read from curbuf (converting stdin)
#تعریف_کړه READ_DUMMY	0x10	// reading into a dummy buffer
#تعریف_کړه READ_KEEP_UNDO	0x20	// keep undo info
#تعریف_کړه READ_FIFO	0x40	// read from fifo or socket
#تعریف_کړه READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#تعریف_کړه READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#تعریف_کړه INDENT_سېټ	1	// set indent
#تعریف_کړه INDENT_INC	2	// increase indent
#تعریف_کړه INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#تعریف_کړه FM_BACKWARD	0x01	// search backwards
#تعریف_کړه FM_FORWARD	0x02	// search forwards
#تعریف_کړه FM_BLOCKSTOP	0x04	// stop at start/end of block
#تعریف_کړه FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#تعریف_کړه DOBUF_GOTO	0	// go to specified buffer
#تعریف_کړه DOBUF_وېشل	1	// split window and go to specified buffer
#تعریف_کړه DOBUF_UNLOAD	2	// unload specified buffer(s)
#تعریف_کړه DOBUF_DEL	3	// delete specified buffer(s) from buflist
#تعریف_کړه DOBUF_WIPE	4	// delete specified buffer(s) really
#تعریف_کړه DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#تعریف_کړه DOBUF_CURRENT	0	// "count" buffer from current buffer
#تعریف_کړه DOBUF_لومړی	1	// "count" buffer from first buffer
#تعریف_کړه DOBUF_وروستی	2	// "count" buffer from last buffer
#تعریف_کړه DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#تعریف_کړه DOBUF_FORCEIT	1	// :cmd!
#تعریف_کړه DOBUF_NOPOPUP	2	// skip popup window buffers
#تعریف_کړه DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#تعریف_کړه RE_پلټنه	0	// save/use pat in/from search_pattern
#تعریف_کړه RE_SUBST	1	// save/use pat in/from subst_pattern
#تعریف_کړه RE_BOTH		2	// save pat in both patterns
#تعریف_کړه RE_وروستی		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#تعریف_کړه RE_MAGIC	1	// 'magic' option
#تعریف_کړه RE_STRING	2	// match in string instead of buffer text
#تعریف_کړه RE_STRICT	4	// don't allow [abc] without ]
#تعریف_کړه RE_AUTO		8	// automatic engine selection

#که_تعریف_شوی FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_سېټ	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_سېټ | REX_USE)
#د_که_پای

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#تعریف_کړه FPC_SAME	1	// both exist and are the same file.
#تعریف_کړه FPC_DIFF	2	// both exist and are different files.
#تعریف_کړه FPC_NOTX	4	// both don't exist.
#تعریف_کړه FPC_DIFFX	6	// one of them doesn't exist.
#تعریف_کړه FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#تعریف_کړه ECMD_HIDE	0x01	// don't free the current buffer
#تعریف_کړه ECMD_سېټ_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#تعریف_کړه ECMD_OLDBUF	0x04	// use existing buffer if it exists
#تعریف_کړه ECMD_FORCEIT	0x08	// ! used in Ex command
#تعریف_کړه ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#تعریف_کړه ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#تعریف_کړه ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#تعریف_کړه ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#تعریف_کړه ECMD_وروستی	((linenr_T)-1)	// use last position in all files
#تعریف_کړه ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#تعریف_کړه DOCMD_VERBOSE	0x01	// included command in error message
#تعریف_کړه DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#تعریف_کړه DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#تعریف_کړه DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#تعریف_کړه DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#تعریف_کړه DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#تعریف_کړه DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#تعریف_کړه BL_WHITE	1	// cursor on first non-white in the line
#تعریف_کړه BL_SOL		2	// use 'sol' option
#تعریف_کړه BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#تعریف_کړه MFS_ALL		1	// also sync blocks with negative numbers
#تعریف_کړه MFS_STOP	2	// stop syncing when a character is available
#تعریف_کړه MFS_FLUSH	4	// flushed file to disk
#تعریف_کړه MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#تعریف_کړه BCO_ENTER	1	// going to enter the buffer
#تعریف_کړه BCO_ALWAYS	2	// always copy the options
#تعریف_کړه BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#تعریف_کړه PUT_FIXINDENT	1	// make indent look nice
#تعریف_کړه PUT_CURSEND	2	// leave cursor after end of new text
#تعریف_کړه PUT_CURSLINE	4	// leave cursor on last line of new text
#تعریف_کړه PUT_کرښه	8	// put register as lines
#تعریف_کړه PUT_کرښه_وېشل	16	// split line for linewise register
#تعریف_کړه PUT_کرښه_FORWARD 32	// put linewise register below Visual sel.
#تعریف_کړه PUT_بلاک_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#تعریف_کړه SIN_بدل_شوی	1	// call changed_bytes() when line changed
#تعریف_کړه SIN_ورننباسه	2	// insert indent before existing text
#تعریف_کړه SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#تعریف_کړه INSCHAR_FORMAT	1	// force formatting
#تعریف_کړه INSCHAR_DO_COM	2	// format comments
#تعریف_کړه INSCHAR_CTRLV	4	// char typed just after CTRL-V
#تعریف_کړه INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#تعریف_کړه INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#تعریف_کړه OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#تعریف_کړه OPENLINE_DO_COM		0x02    // format comments
#تعریف_کړه OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#تعریف_کړه OPENLINE_MARKFIX	0x08    // fix mark positions
#تعریف_کړه OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#تعریف_کړه OPENLINE_FORMAT		0x20    // formatting long comment
#تعریف_کړه OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#تعریف_کړه HIST_CMD	0	// colon commands
#تعریف_کړه HIST_پلټنه	1	// search commands
#تعریف_کړه HIST_بیان	2	// expressions (from entering = register)
#تعریف_کړه HIST_ننوت	3	// input() lines
#تعریف_کړه HIST_DEBUG	4	// debug commands
#تعریف_کړه HIST_شمېر	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#تعریف_کړه BARTYPE_نسخه 1
#تعریف_کړه BARTYPE_HISTORY 2
#تعریف_کړه BARTYPE_ثبتوونکی 3
#تعریف_کړه BARTYPE_MARK 4

#تعریف_کړه VIMINFO_نسخه 4
#تعریف_کړه VIMINFO_نسخه_WITH_HISTORY 2
#تعریف_کړه VIMINFO_نسخه_WITH_REGISTERS 3
#تعریف_کړه VIMINFO_نسخه_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#تعریف_کړه DT_نښان		1	// jump to newer position or same tag again
#تعریف_کړه DT_راوباسه		2	// jump to older position
#تعریف_کړه DT_بل		3	// jump to next match of same tag
#تعریف_کړه DT_PREV		4	// jump to previous match of same tag
#تعریف_کړه DT_لومړی	5	// jump to first match of same tag
#تعریف_کړه DT_وروستی		6	// jump to first match of same tag
#تعریف_کړه DT_SELECT	7	// jump to selection from list
#تعریف_کړه DT_HELP		8	// like DT_TAG, but no wildcards
#تعریف_کړه DT_JUMP		9	// jump to new tag or selection from list
#تعریف_کړه DT_CSCOPE	10	// cscope find command (like tjump)
#تعریف_کړه DT_LTAG		11	// tag using location list
#تعریف_کړه DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#تعریف_کړه نښان_HELP	1	// only search for help tags
#تعریف_کړه نښان_نومونه	2	// only return name of tag
#تعریف_کړه	نښان_REGEXP	4	// use tag pattern as regexp
#تعریف_کړه	نښان_NOIC	8	// don't always ignore case
#که_تعریف_شوی FEAT_CSCOPE
# define نښان_CSCOPE	16	// cscope tag
#د_که_پای
#تعریف_کړه نښان_VERBOSE	32	// message verbosity
#تعریف_کړه نښان_INS_COMP	64	// Currently doing insert completion
#تعریف_کړه نښان_KEEP_LANG	128	// keep current language
#تعریف_کړه نښان_NO_TAGFUNC	256	// do not use 'tagfunc'

#تعریف_کړه نښان_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#تعریف_کړه VIM_GENERIC	0
#تعریف_کړه VIM_ERROR	1
#تعریف_کړه VIM_WARNING	2
#تعریف_کړه VIM_INFO	3
#تعریف_کړه VIM_QUESTION	4
#تعریف_کړه VIM_وروستی_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#تعریف_کړه VIM_YES		2
#تعریف_کړه VIM_NO		3
#تعریف_کړه VIM_CANCEL	4
#تعریف_کړه VIM_ALL		5
#تعریف_کړه VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#تعریف_کړه WSP_ځای	0x01	// require enough room
#تعریف_کړه WSP_VERT	0x02	// split/equalize vertically
#تعریف_کړه WSP_HOR		0x04	// equalize horizontally
#تعریف_کړه WSP_سر		0x08	// window at top-left of shell
#تعریف_کړه WSP_BOT		0x10	// window at bottom-right of shell
#تعریف_کړه WSP_HELP	0x20	// creating the help window
#تعریف_کړه WSP_لاندې	0x40	// put new window below/right
#تعریف_کړه WSP_ABOVE	0x80	// put new window above/left
#تعریف_کړه WSP_NEWLOC	0x100	// don't copy location list
#تعریف_کړه WSP_FORCE_ځای	0x200	// ignore "not enough room" errors
#تعریف_کړه WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#تعریف_کړه RESIZE_VERT	1	// resize vertically
#تعریف_کړه RESIZE_HOR	2	// resize horizontally
#تعریف_کړه RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#تعریف_کړه CCGD_AW		1	// do autowrite if buffer was changed
#تعریف_کړه CCGD_MULTWIN	2	// check also when several wins for the buf
#تعریف_کړه CCGD_FORCEIT	4	// ! used
#تعریف_کړه CCGD_ALLBUF	8	// may write all buffers
#تعریف_کړه CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#تعریف_کړه OPT_FREE	0x01	// free old value if it was allocated
#تعریف_کړه OPT_GLOBAL	0x02	// use global value
#تعریف_کړه OPT_LOCAL	0x04	// use local value
#تعریف_کړه OPT_MODELINE	0x08	// option in modeline
#تعریف_کړه OPT_WINONLY	0x10	// only set window-local options
#تعریف_کړه OPT_NOWIN	0x20	// don't set window-local options
#تعریف_کړه OPT_ONECOLUMN	0x40	// list options one per line
#تعریف_کړه OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#تعریف_کړه OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#تعریف_کړه DLG_BUTTON_SEP	'\n'
#تعریف_کړه DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#تعریف_کړه NO_SCREEN	2	// no screen updating yet
#تعریف_کړه NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#تعریف_کړه SEA_NONE	0	// don't use dialog
#تعریف_کړه SEA_DIALOG	1	// use dialog when possible
#تعریف_کړه SEA_QUIT	2	// quit editing the file
#تعریف_کړه SEA_RECOVER	3	// recover the file
#تعریف_کړه SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#تعریف_کړه MIN_بدلون_پاڼه_اندازه 1048
#تعریف_کړه MAX_بدلون_پاڼه_اندازه 50000

// Special values for current_sctx.sc_sid.
#تعریف_کړه SID_MODELINE	(-1)	// when using a modeline
#تعریف_کړه SID_CMDARG	(-2)	// for "--cmd" argument
#تعریف_کړه SID_CARG	(-3)	// for "-c" argument
#تعریف_کړه SID_ENV		(-4)	// for sourcing environment variable
#تعریف_کړه SID_ERROR	(-5)	// option was reset because of an error
#تعریف_کړه SID_NONE	(-6)	// don't set scriptID
#تعریف_کړه SID_WINLAYOUT	(-7)	// changing window size

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

    عدد_EVENTS,			// MUST be the last one
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
    , HLF_جمع_کړه	    // Added diff line
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
    , HLF_راوباسه	    // popup window body
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
    , HLF_پیغام	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_شمېر	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#تعریف_کړه HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
#که_نه_تعریف_شوی TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#د_که_پای

#تعریف_کړه MAYBE	2	    // sometimes used for a variant on TRUE

#تعریف_کړه LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#که_تعریف_شوی FEAT_دنده_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_منزل_LOG_THIS إذا (ch_log_output == FALSE) ch_log_output = TRUE;
#وإلا
// no logging support
# define MAY_WANT_منزل_LOG_THIS
#د_که_پای

#که_نه_تعریف_شوی UINT32_T
تعريفنوع UINT32_TYPEDEF UINT32_T;
#د_که_پای

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#تعریف_کړه OP_NOP		0	// no pending operation
#تعریف_کړه OP_DELETE	1	// "d"  delete operator
#تعریف_کړه OP_YANK		2	// "y"  yank operator
#تعریف_کړه OP_CHANGE	3	// "c"  change operator
#تعریف_کړه OP_LSHIFT	4	// "<"  left shift operator
#تعریف_کړه OP_RSHIFT	5	// ">"  right shift operator
#تعریف_کړه OP_FILTER	6	// "!"  filter operator
#تعریف_کړه OP_TILDE	7	// "g~" switch case operator
#تعریف_کړه OP_INDENT	8	// "="  indent operator
#تعریف_کړه OP_FORMAT	9	// "gq" format operator
#تعریف_کړه OP_COLON	10	// ":"  colon operator
#تعریف_کړه OP_پاسنی	11	// "gU" make upper case operator
#تعریف_کړه OP_ښکتنی	12	// "gu" make lower case operator
#تعریف_کړه OP_JOIN		13	// "J"  join operator, only for Visual mode
#تعریف_کړه OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#تعریف_کړه OP_ROT13	15	// "g?" rot-13 encoding
#تعریف_کړه OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#تعریف_کړه OP_ورننباسه	17	// "I"  Insert column, only for Visual mode
#تعریف_کړه OP_APPEND	18	// "A"  Append column, only for Visual mode
#تعریف_کړه OP_FOLD		19	// "zf" define a fold
#تعریف_کړه OP_FOLDOPEN	20	// "zo" open folds
#تعریف_کړه OP_FOLDOPENREC	21	// "zO" open folds recursively
#تعریف_کړه OP_FOLDCLOSE	22	// "zc" close folds
#تعریف_کړه OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#تعریف_کړه OP_FOLDDEL	24	// "zd" delete folds
#تعریف_کړه OP_FOLDDELREC	25	// "zD" delete folds recursively
#تعریف_کړه OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#تعریف_کړه OP_FUNCTION	27	// "g@" call 'operatorfunc'
#تعریف_کړه OP_NR_جمع_کړه	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#تعریف_کړه OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#تعریف_کړه MCHAR	0		// character-wise movement/register
#تعریف_کړه MLINE	1		// line-wise movement/register
#تعریف_کړه MBLOCK	2		// block-wise register

#تعریف_کړه MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#تعریف_کړه MIN_COLUMNS	12	// minimal columns for screen
#تعریف_کړه MIN_کرښې	2	// minimal lines for screen
#تعریف_کړه MIN_CMDHEIGHT	1	// minimal height for command line
#تعریف_کړه حالت_لوړوالی_2	1	// default height of a status line under a
				//کړکۍ
#که_تعریف_شوی FEAT_MENU		// height of a status line under a window
# define WINBAR_لوړوالی(wp)	(wp)->w_winbar_لوړوالی
# define VISIBLE_لوړوالی(wp)	((wp)->w_لوړوالی + (wp)->w_winbar_لوړوالی)
#وإلا
# define WINBAR_لوړوالی(wp)	0
# define VISIBLE_لوړوالی(wp)	(wp)->w_لوړوالی
#د_که_پای
#تعریف_کړه QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#که_نه_تعریف_شوی CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#د_که_پای

#تعریف_کړه LSIZE	    512		// max. size of a line in the tags file

#تعریف_کړه IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#تعریف_کړه DIALOG_پیغام_اندازه 1000	// buffer size for dialog_msg()

#تعریف_کړه پیغام_BUF_LEN 480	// length of buffer for small messages
#تعریف_کړه پیغام_BUF_CLEN  (پیغام_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#تعریف_کړه FOLD_متن_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#تعریف_کړه TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#تعریف_کړه MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#تعریف_کړه MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#تعریف_کړه UNDO_HASH_اندازه 32

#که_تعریف_شوی HAVE_FCNTL_H
# include <fcntl.h>
#د_که_پای

#که_تعریف_شوی دوګونی_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#وإلا
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#د_که_پای

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#که_تعریف_شوی __CYGWIN__
# define O_EXTRA    O_دوګونی
#وإلا
# define O_EXTRA    0
#د_که_پای

#که_نه_تعریف_شوی O_NOFOLLOW
# define O_NOFOLLOW 0
#د_که_پای

#که_نه_تعریف_شوی W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#د_که_پای
#که_نه_تعریف_شوی R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#د_که_پای

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#تعریف_کړه ALLOC_ONE(type)  (type *)alloc(حجم(type))
#تعریف_کړه ALLOC_ONE_ID(type, id)  (type *)alloc_id(حجم(type), id)
#تعریف_کړه ALLOC_MULT(type, شمېر)  (type *)alloc(حجم(type) * (شمېر))
#تعریف_کړه ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(حجم(type))
#تعریف_کړه ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(حجم(type), id)
#تعریف_کړه ALLOC_CLEAR_MULT(type, شمېر)  (type *)alloc_clear(حجم(type) * (شمېر))
#تعریف_کړه LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(حجم(type), FALSE)
#تعریف_کړه LALLOC_CLEAR_MULT(type, شمېر)  (type *)lalloc_clear(حجم(type) * (شمېر), FALSE)
#تعریف_کړه LALLOC_MULT(type, شمېر)  (type *)lalloc(حجم(type) * (شمېر), FALSE)

#که_تعریف_شوی HAVE_MEMSET
# define vim_memset(ptr, c, اندازه)   memset((ptr), (c), (اندازه))
#وإلا
فراغ *vim_memset(فراغ *, صحيح, size_t);
#د_که_پای
#تعریف_کړه CLEAR_ډګر(ډګر)  vim_memset(&(ډګر), 0, حجم(ډګر))
#تعریف_کړه CLEAR_اشاره_ګر(ptr)  vim_memset((ptr), 0, حجم(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#تعریف_کړه STRLEN(s)	    مزي_اوږدوالی((حرف *)(s))
#تعریف_کړه STRCPY(d, s)	    strcpy((حرف *)(d), (حرف *)(s))
#تعریف_کړه STRNCPY(d, s, n)    strncpy((حرف *)(d), (حرف *)(s), (n))
#تعریف_کړه STRCMP(d, s)	    strcmp((حرف *)(d), (حرف *)(s))
#تعریف_کړه STRNCMP(d, s, n)    مزي_پرتله((حرف *)(d), (حرف *)(s), (n))
#که_تعریف_شوی HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((حرف *)(d), (حرف *)(s))
#وإلا
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((حرف *)(d), (حرف *)(s))
# وإلا
#  define STRICMP(d, s)	    vim_stricmp((حرف *)(d), (حرف *)(s))
# endif
#د_که_پای
#که_تعریف_شوی HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((حرف *)(d), (حرف *)(s))
#وإلا
# define STRCOLL(d, s)     strcmp((حرف *)(d), (حرف *)(s))
#د_که_پای

// Like strcpy() but allows overlapped source and destination.
#تعریف_کړه STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#که_تعریف_شوی HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((حرف *)(d), (حرف *)(s), (n))
#وإلا
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((حرف *)(d), (حرف *)(s), (n))
# وإلا
#  define STRNICMP(d, s, n) vim_strnicmp((حرف *)(d), (حرف *)(s), (n))
# endif
#د_که_پای

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#تعریف_کړه MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)MAXCOL)
#تعریف_کړه MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)(n))
#تعریف_کړه MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#تعریف_کړه STRCAT(d, s)	    strcat((حرف *)(d), (حرف *)(s))
#تعریف_کړه STRNCAT(d, s, n)    strncat((حرف *)(d), (حرف *)(s), (n))

#که_تعریف_شوی HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((حرف *)(s), (حرف *)(cs))
#د_که_پای

#تعریف_کړه بهر_STR(s)		    بهر_str((char_u *)(s))
#تعریف_کړه بهر_STR_NF(s)		    بهر_str_nf((char_u *)(s))

#که_تعریف_شوی FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.دننه_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.دننه_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_بټ		    (gui.دننه_use || p_tgc)
# وإلا
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_بټ		    gui.دننه_use
# endif
#وإلا
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_بټ		    p_tgc
# endif
#د_که_پای
#که_تعریف_شوی FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#وإلا
# define IS_CTERM		    (t_colors > 1)
#د_که_پای
#که_تعریف_شوی GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#د_که_پای

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#که_تعریف_شوی HAVE_STRERROR
# define PERROR(پیغام)		    (فراغ)semsg("%s: %s", (حرف *)(پیغام), strerror(errno))
#وإلا
# define PERROR(پیغام)		    do_perror(پیغام)
#د_که_پای

تعريفنوع اوږد	linenr_T;		// line number type
تعريفنوع صحيح	colnr_T;		// column number type
تعريفنوع بینښه لنډ disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#که_تعریف_شوی __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#وإلا
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#د_که_پای

#تعریف_کړه SHOWCMD_ستون_شمېر 10			// columns needed by shown command

تعريفنوع فراغ	    *vim_acl_T;		// dummy to pass an ACL to a function

#که_نه_تعریف_شوی mch_memmove
# define mch_memmove(منزل, from, len) memmove((حرف*)(منزل), (حرف*)(from), (len))
#د_که_پای

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#تعریف_کړه fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#تعریف_کړه fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#إذا defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_ننوت_BUF
#د_که_پای

#إذا !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, شمېر) vim_read((fd), (buf), (شمېر))
# define write_eintr(fd, buf, شمېر) vim_write((fd), (buf), (شمېر))
#د_که_پای

#که_تعریف_شوی MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, شمېر)   اقرأ((fd), (حرف *)(buf), (بینښه صحيح)(شمېر))
# define vim_write(fd, buf, شمېر)  اكتب((fd), (حرف *)(buf), (بینښه صحيح)(شمېر))
#وإلا
# define vim_read(fd, buf, شمېر)   اقرأ((fd), (حرف *)(buf), (شمېر))
# define vim_write(fd, buf, شمېر)  اكتب((fd), (حرف *)(buf), (شمېر))
#د_که_پای

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#تعریف_کړه HL_ATTR(n)	highlight_attr[(صحيح)(n)]
#تعریف_کړه غړی_STR(n)	غړی_مزي_ګانې[(صحيح)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#که_نه_تعریف_شوی EXTERN
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
#د_که_پای

#تعریف_کړه MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#تعریف_کړه MB_MAXBYTES	30

#إذا (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
تعريفنوع LARGE_INTEGER proftime_T;
#  define PROF_وخت_BLANK "           "
#  define PROF_TOTALS_سرلیک "count  total (s)   self (s)"
# وإلا
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  إذا defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
تعريفنوع بنية timespec proftime_T;
#   define PROF_GET_وخت(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_وخت_FORMAT "%3ld.%09ld"
#   define PROF_وخت_BLANK "              "
#   define PROF_TOTALS_سرلیک "count     total (s)      self (s)"
#  وإلا
تعريفنوع بنية timeval proftime_T;
#   define PROF_GET_وخت(tm) gettimeofday(tm, عدم)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_وخت_FORMAT "%3ld.%06ld"
#   define PROF_وخت_BLANK "           "
#   define PROF_TOTALS_سرلیک "count  total (s)   self (s)"
#  endif
# endif
#وإلا
تعريفنوع صحيح proftime_T;	    // dummy for function prototypes
#د_که_پای

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
#که_تعریف_شوی MSWIN
تعريفنوع __time64_t  وخت_T;
#وإلا
تعريفنوع time_t	    وخت_T;
#د_که_پای

#که_تعریف_شوی _WIN64
تعريفنوع __int64 sock_T;
#وإلا
تعريفنوع صحيح sock_T;
#د_که_پای

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#د_که_پای

#إذا defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#د_که_پای

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#ضمن "option.h"	// options and default values

#ضمن "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#ضمن "structs.h"	// defines many structures
#ضمن "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#ضمن "alloc.h"

// Values for "do_profiling".
#تعریف_کړه PROF_NONE	0	// profiling not started
#تعریف_کړه PROF_YES	1	// profiling busy
#تعریف_کړه PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#تعریف_کړه MOUSE_کیڼ	0x00
#تعریف_کړه MOUSE_منځ	0x01
#تعریف_کړه MOUSE_ښی	0x02
#تعریف_کړه MOUSE_RELEASE	0x03

// bit masks for modifiers:
#تعریف_کړه MOUSE_ښويدنه	0x04
#تعریف_کړه MOUSE_ALT	0x08
#تعریف_کړه MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#تعریف_کړه MOUSE_4	0x100	// scroll wheel down
#تعریف_کړه MOUSE_5	0x200	// scroll wheel up

#تعریف_کړه MOUSE_X1	0x300 // Mouse-button X1 (6th)
#تعریف_کړه MOUSE_X2	0x400 // Mouse-button X2

#تعریف_کړه MOUSE_6	0x500	// scroll wheel left
#تعریف_کړه MOUSE_7	0x600	// scroll wheel right

#تعریف_کړه MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#تعریف_کړه MOUSE_DRAG_XTERM   0x40

#تعریف_کړه MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#تعریف_کړه MOUSEWHEEL_ټیټ		0x60

// Lowest button code for extra mouse buttons 8-11
#تعریف_کړه MOUSESIDEBUTTONS_ټیټ		0xa0

#تعریف_کړه MOUSE_CLICK_ماسک	0x03

#تعریف_کړه عدد_MOUSE_CLICKS(کوډ) \
    (((بینښه)((کوډ) & 0xC0) >> 6) + 1)

#تعریف_کړه سېټ_عدد_MOUSE_CLICKS(کوډ, عدد) \
    ((کوډ) = ((کوډ) & 0x3f) | ((((عدد) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#تعریف_کړه MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#تعریف_کړه دننه_UNKNOWN		0
#تعریف_کړه دننه_بفر		1
#تعریف_کړه دننه_حالت_کرښه		2	// on status or command line
#تعریف_کړه دننه_SEP_کرښه		4	// on vertical separator line
#تعریف_کړه دننه_OTHER_WIN		8	// in other window but can't go there
#تعریف_کړه CURSOR_MOVED		0x100
#تعریف_کړه MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#تعریف_کړه MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#تعریف_کړه MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#تعریف_کړه MOUSE_FOCUS		0x01	// need to stay in this window
#تعریف_کړه MOUSE_MAY_VIS		0x02	// may start Visual mode
#تعریف_کړه MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#تعریف_کړه MOUSE_SETPOS		0x08	// only set current mouse position
#تعریف_کړه MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#تعریف_کړه MOUSE_RELEASED		0x20	// button was released

#إذا defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وخت_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#د_که_پای


// defines for eval_vars()
#تعریف_کړه VALID_لار		1
#تعریف_کړه VALID_سر		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#تعریف_کړه VV_شمېر	0
#تعریف_کړه VV_COUNT1	1
#تعریف_کړه VV_PREVCOUNT	2
#تعریف_کړه VV_ERRMSG	3
#تعریف_کړه VV_WARNINGMSG	4
#تعریف_کړه VV_STATUSMSG	5
#تعریف_کړه VV_SHELL_ERROR	6
#تعریف_کړه VV_THIS_SESSION	7
#تعریف_کړه VV_نسخه	8
#تعریف_کړه VV_LNUM		9
#تعریف_کړه VV_TERMRESPONSE	10
#تعریف_کړه VV_FNAME	11
#تعریف_کړه VV_LANG		12
#تعریف_کړه VV_LC_وخت	13
#تعریف_کړه VV_CTYPE	14
#تعریف_کړه VV_CC_FROM	15
#تعریف_کړه VV_CC_منزل	16
#تعریف_کړه VV_FNAME_دننه	17
#تعریف_کړه VV_FNAME_بهر	18
#تعریف_کړه VV_FNAME_NEW	19
#تعریف_کړه VV_FNAME_DIFF	20
#تعریف_کړه VV_CMDARG	21
#تعریف_کړه VV_FOLDSTART	22
#تعریف_کړه VV_FOLDEND	23
#تعریف_کړه VV_FOLDDASHES	24
#تعریف_کړه VV_FOLDLEVEL	25
#تعریف_کړه VV_PROGNAME	26
#تعریف_کړه VV_SEND_SERVER	27
#تعریف_کړه VV_DYING	28
#تعریف_کړه VV_EXCEPTION	29
#تعریف_کړه VV_THROWPOINT	30
#تعریف_کړه VV_REG		31
#تعریف_کړه VV_CMDBANG	32
#تعریف_کړه VV_INSERTMODE	33
#تعریف_کړه VV_VAL		34
#تعریف_کړه VV_کلی		35
#تعریف_کړه VV_PROFILING	36
#تعریف_کړه VV_FCS_REASON	37
#تعریف_کړه VV_FCS_CHOICE	38
#تعریف_کړه VV_BEVAL_BUFNR	39
#تعریف_کړه VV_BEVAL_WINNR	40
#تعریف_کړه VV_BEVAL_WINID	41
#تعریف_کړه VV_BEVAL_LNUM	42
#تعریف_کړه VV_BEVAL_ستون	43
#تعریف_کړه VV_BEVAL_متن	44
#تعریف_کړه VV_SCROLLSTART	45
#تعریف_کړه VV_SWAPNAME	46
#تعریف_کړه VV_SWAPCHOICE	47
#تعریف_کړه VV_SWAPCOMMAND	48
#تعریف_کړه VV_CHAR		49
#تعریف_کړه VV_MOUSE_WIN	50
#تعریف_کړه VV_MOUSE_WINID	51
#تعریف_کړه VV_MOUSE_LNUM   52
#تعریف_کړه VV_MOUSE_ستون	53
#تعریف_کړه VV_OP		54
#تعریف_کړه VV_SEARCHFORWARD 55
#تعریف_کړه VV_HLSEARCH	56
#تعریف_کړه VV_OLDFILES	57
#تعریف_کړه VV_WINDOWID	58
#تعریف_کړه VV_PROGPATH	59
#تعریف_کړه VV_COMPLETED_توکی 60
#تعریف_کړه VV_OPTION_NEW   61
#تعریف_کړه VV_OPTION_OLD   62
#تعریف_کړه VV_OPTION_OLDLOCAL 63
#تعریف_کړه VV_OPTION_OLDGLOBAL 64
#تعریف_کړه VV_OPTION_COMMAND 65
#تعریف_کړه VV_OPTION_TYPE  66
#تعریف_کړه VV_ERRORS	67
#تعریف_کړه VV_FALSE	68
#تعریف_کړه VV_TRUE		69
#تعریف_کړه VV_NONE		70
#تعریف_کړه VV_NULL		71
#تعریف_کړه VV_NUMBERMAX	72
#تعریف_کړه VV_NUMBERMIN	73
#تعریف_کړه VV_NUMBERSIZE	74
#تعریف_کړه VV_VIM_DID_ENTER 75
#تعریف_کړه VV_TESTING	76
#تعریف_کړه VV_TYPE_شمېره	77
#تعریف_کړه VV_TYPE_STRING	78
#تعریف_کړه VV_TYPE_FUNC	79
#تعریف_کړه VV_TYPE_LIST	80
#تعریف_کړه VV_TYPE_DICT	81
#تعریف_کړه VV_TYPE_FLOAT	82
#تعریف_کړه VV_TYPE_BOOL	83
#تعریف_کړه VV_TYPE_NONE	84
#تعریف_کړه VV_TYPE_دنده	85
#تعریف_کړه VV_TYPE_CHANNEL	86
#تعریف_کړه VV_TYPE_BLOB	87
#تعریف_کړه VV_TYPE_CLASS	88
#تعریف_کړه VV_TYPE_OBJECT	89
#تعریف_کړه VV_TERMRFGRESP	90
#تعریف_کړه VV_TERMRBGRESP	91
#تعریف_کړه VV_TERMU7RESP	92
#تعریف_کړه VV_TERMSTYLERESP 93
#تعریف_کړه VV_TERMBLINKRESP 94
#تعریف_کړه VV_EVENT	95
#تعریف_کړه VV_VERSIONLONG	96
#تعریف_کړه VV_ECHOSPACE	97
#تعریف_کړه VV_ARGV		98
#تعریف_کړه VV_COLLATE      99
#تعریف_کړه VV_EXITING	100
#تعریف_کړه VV_COLORNAMES   101
#تعریف_کړه VV_SIZEOFINT	102
#تعریف_کړه VV_SIZEOFLONG	103
#تعریف_کړه VV_SIZEOFPOINTER 104
#تعریف_کړه VV_MAXCOL	105
#تعریف_کړه VV_PYTHON3_نسخه 106
#تعریف_کړه VV_TYPE_TYPEALIAS 107
#تعریف_کړه VV_TYPE_ENUM	  108
#تعریف_کړه VV_TYPE_ENUMVALUE 109
#تعریف_کړه VV_STACKTRACE	110
#تعریف_کړه VV_TYPE_TUPLE	111
#تعریف_کړه VV_WAYLAND_DISPLAY 112
#تعریف_کړه VV_CLIPMETHOD 113
#تعریف_کړه VV_TERMDA1 114
#تعریف_کړه VV_TERMOSC 115
#تعریف_کړه VV_VIM_DID_INIT		116
#تعریف_کړه VV_CLIPPROVIDERS 117
#تعریف_کړه VV_IMAGEBACKEND 118
#تعریف_کړه VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#تعریف_کړه VVAL_FALSE	0L	// VAR_BOOL
#تعریف_کړه VVAL_TRUE	1L	// VAR_BOOL
#تعریف_کړه VVAL_NONE	2L	// VAR_SPECIAL
#تعریف_کړه VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#تعریف_کړه VAR_TYPE_شمېره	    0
#تعریف_کړه VAR_TYPE_STRING	    1
#تعریف_کړه VAR_TYPE_FUNC	    2
#تعریف_کړه VAR_TYPE_LIST	    3
#تعریف_کړه VAR_TYPE_DICT	    4
#تعریف_کړه VAR_TYPE_FLOAT	    5
#تعریف_کړه VAR_TYPE_BOOL	    6
#تعریف_کړه VAR_TYPE_NONE	    7
#تعریف_کړه VAR_TYPE_دنده	    8
#تعریف_کړه VAR_TYPE_CHANNEL    9
#تعریف_کړه VAR_TYPE_BLOB	    10
#تعریف_کړه VAR_TYPE_INSTR	    11
#تعریف_کړه VAR_TYPE_CLASS	    12
#تعریف_کړه VAR_TYPE_OBJECT	    13
#تعریف_کړه VAR_TYPE_TYPEALIAS  14
#تعریف_کړه VAR_TYPE_ENUM	    15
#تعریف_کړه VAR_TYPE_ENUMVALUE  16
#تعریف_کړه VAR_TYPE_TUPLE	    17

#تعریف_کړه DICT_MAXNEST 100	// maximum nesting of lists and dicts

#تعریف_کړه TABSTOP_MAX 9999

#که_تعریف_شوی HAVE_CLIPMETHOD
تعريفنوع تعداد {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#د_که_پای

#که_تعریف_شوی FEAT_CLIPBOARD

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
# define SELECT_دننه_پرمختګ	1
# define SELECT_بشپړ		2

# define SELECT_ډېرپېښ_CHAR	0
# define SELECT_ډېرپېښ_کلمه	1
# define SELECT_ډېرپېښ_کرښه	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
تعريفنوع بنية
{
    صحيح		شته;	// Is clipboard available?
    صحيح		owned;		// Flag: do we own the selection?
    pos_T	پیل;		// Start of selected area
    pos_T	پای_ټکی;		// End of selected area
    صحيح		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_لیکه;
    short_u	origin_پیل_ستون;
    short_u	origin_پای_ټکی_ستون;
    short_u	کلمه_پیل_ستون;
    short_u	کلمه_پای_ټکی_ستون;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_ستون;
    short_u	max_ستون;
    short_u	min_لیکه;
    short_u	max_لیکه;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	ډېرپېښ;		// Select by char, word, or line.

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
#د_که_پای

// Use 64-bit stat structure on MS-Windows.
#که_تعریف_شوی MSWIN
تعريفنوع بنية _stat64 stat_T;
#وإلا
تعريفنوع بنية stat stat_T;
#د_که_پای

تعريفنوع بنية soundcb_S soundcb_T;

#إذا (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(چاپ_کړه, fmt_idx, arg_idx)))
#وإلا
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#د_که_پای

#إذا defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#وإلا
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#د_که_پای

تعريفنوع تعداد {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_سمون,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
تعريفنوع تعداد {
    PASTE_ورننباسه,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_ډېرپېښ_T;

// Argument for flush_buffers().
تعريفنوع تعداد {
    FLUSH_اصغري,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_ننوت		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
تعريفنوع تعداد {
    USEPOPUP_NONE,
    USEPOPUP_نورمال,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
تعريفنوع تعداد {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_ډېری,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#تعریف_کړه ENABLE_STL_ډېرپېښ_MULTI_NL

// Argument for build_stl_str_hl_local().
تعريفنوع تعداد {
    STL_ډېرپېښ_SINGLE,	    // Does not accept line breaks "%@"
    STL_ډېرپېښ_MULTI,	    // Accept line breaks "%@"
    STL_ډېرپېښ_GET_RENDERED_لوړوالی,   // Just get stl rendered height
#که_تعریف_شوی ENABLE_STL_ډېرپېښ_MULTI_NL
    STL_ډېرپېښ_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#د_که_پای
} stl_ډېرپېښ_T;

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
    CPT_ډول,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_شمېر,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
تعريفنوع حرف *(*opt_did_سېټ_cb_T)(optset_T *دلیلونه);

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
تعريفنوع صحيح (*opt_expand_cb_T)(optexpand_T *دلیلونه, صحيح *عدد_Matches, char_u ***matches);

// Flags for assignment functions.
#تعریف_کړه ټاکل_VAR	0     // ":var" (nothing special)
#تعریف_کړه ټاکل_FINAL	0x01  // ":final"
#تعریف_کړه ټاکل_CONST	0x02  // ":const"
#تعریف_کړه ټاکل_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#تعریف_کړه ټاکل_DECL	0x08  // may declare variable if it does not exist
#تعریف_کړه ټاکل_UNPACK	0x10  // using [a, b] = list
#تعریف_کړه ټاکل_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#تعریف_کړه ټاکل_FOR_LOOP 0x40 // assigning to loop variable
#تعریف_کړه ټاکل_INIT	0x80 // not assigning a value, just a declaration
#تعریف_کړه ټاکل_UPDATE_بلاک_ID 0x100  // update sav_block_id
#تعریف_کړه ټاکل_COMPOUND_OP 0x200  // compound operator e.g. "+="

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
# define mch_پیغام(str)		چاپ_کړه("%s", (str))
#وإلا
# define USE_MCH_ERRMSG
#د_که_پای

#إذا defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#د_که_پای

#إذا defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_ننوت_لاره
#د_که_پای

#که_نه_تعریف_شوی FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define شمېره_پلنوالی(x) 7
#د_که_پای

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#إذا !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#د_که_پای

#ضمن "globals.h"	    // global variables and messages
#ضمن "errors.h"	    // error messages

#که_نه_تعریف_شوی PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ساكن مضمن صحيح
دننه_vim9script(فراغ)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    أرجع (current_sctx.sc_نسخه == SCRIPT_نسخه_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#د_که_پای

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#إذا defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#د_که_پای

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#که_تعریف_شوی FEAT_BROWSE
# ifdef BACKSLASH_دننه_FILENAME
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
#د_که_پای

#که_تعریف_شوی _MSC_VER
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
#د_که_پای

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#إذا defined(MEM_PROFILE)
# define vim_realloc(ptr, اندازه)  mem_realloc((ptr), (اندازه))
#وإلا
# define vim_realloc(ptr, اندازه)  realloc((ptr), (اندازه))
#د_که_پای

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#تعریف_کړه MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#تعریف_کړه MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#تعریف_کړه ENC_8_بټ	0x01
#تعریف_کړه ENC_DBCS	0x02
#تعریف_کړه ENC_UNICODE	0x04

#تعریف_کړه ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#تعریف_کړه ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#تعریف_کړه ENC_2BYTE	0x40	    // Unicode: UCS-2
#تعریف_کړه ENC_4BYTE	0x80	    // Unicode: UCS-4
#تعریف_کړه ENC_2_کلمه	0x100	    // Unicode: UTF-16

#تعریف_کړه ENC_LATIN1	0x200	    // Latin1
#تعریف_کړه ENC_LATIN9	0x400	    // Latin9
#تعریف_کړه ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#که_تعریف_شوی USE_ICONV
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
#د_که_پای

#تعریف_کړه SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#که_تعریف_شوی FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#د_که_پای

#إذا defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.دننه_use ? (مزدوج)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.دننه_use ? (مزدوج)(p) / gui.scale : (p))
#وإلا
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#د_که_پای

#إذا defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.دننه_use) ? gui_mch_get_display() : xterm_dpy)
# وإلا
#  define X_DISPLAY	(gui.دننه_use ? gui.dpy : xterm_dpy)
# endif
#وإلاإذا defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.دننه_use) ? gui_mch_get_display() : (Display *)عدم)
# وإلا
#  define X_DISPLAY	gui.dpy
# endif
#وإلاإذا defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#وإلا
# define X_DISPLAY	(Display *)عدم
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GTK
# إذا !GTK_CHECK_نسخه(2,14,0)
#  define gtk_widget_get_کړکۍ(wid)	((wid)->کړکۍ_2)
#  define gtk_plug_get_socket_کړکۍ(wid)	((wid)->socket_کړکۍ)
#  define gtk_selection_معلومات_get_معلومات(sel)	((sel)->معلومات)
#  define gtk_selection_معلومات_get_معلومات_type(sel)	((sel)->type)
#  define gtk_selection_معلومات_get_format(sel)	((sel)->format)
#  define gtk_selection_معلومات_get_اوږدتیا(sel)	((sel)->اوږدتیا)
#  define gtk_adjustment_سېټ_ښکتنی(adj, ټیټ) \
    افعل { (adj)->ښکتنی = ټیټ; } طالما (0)
#  define gtk_adjustment_سېټ_پاسنی(adj, پورته) \
    افعل { (adj)->پاسنی_2 = پورته; } طالما (0)
#  define gtk_adjustment_سېټ_پاڼه_اندازه(adj, اندازه) \
    افعل { (adj)->پاڼه_اندازه = اندازه; } طالما (0)
#  define gtk_adjustment_سېټ_پاڼه_increment(adj, inc) \
    افعل { (adj)->پاڼه_increment = inc; } طالما (0)
#  define gtk_adjustment_سېټ_ګام_increment(adj, inc) \
    افعل { (adj)->ګام_increment = inc; } طالما (0)
# endif
# إذا !GTK_CHECK_نسخه(2,16,0)
#  define gtk_selection_معلومات_get_selection(sel)	((sel)->selection)
# endif
# إذا !GTK_CHECK_نسخه(2,18,0)
#  define gtk_widget_get_ویش(wid, alloc) \
    افعل { *(alloc) = (wid)->ویش; } طالما (0)
#  define gtk_widget_سېټ_ویش(wid, alloc) \
    افعل { (wid)->ویش = *(alloc); } طالما (0)
#  define gtk_widget_get_has_کړکۍ(wid)	!GTK_WIDGET_NO_کړکۍ(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_سېټ_کړکۍ(wid, win) \
    افعل { (wid)->کړکۍ_2 = (win); } طالما (0)
#  define gtk_widget_سېټ_can_default(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_سېټ_FLAGS(wid, GTK_CAN_DEFAULT); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } طالما (0)
#  define gtk_widget_سېټ_can_focus(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_سېټ_FLAGS(wid, GTK_CAN_FOCUS); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } طالما (0)
#  define gtk_widget_سېټ_visible(wid, vis) \
    افعل { إذا (vis) \
	    { gtk_widget_show(wid); } \
	وإلا \
	    { gtk_widget_hide(wid); } } طالما (0)
# endif
# إذا !GTK_CHECK_نسخه(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_سېټ_mapped(wid, map) \
    افعل { إذا (map) \
	    { GTK_WIDGET_سېټ_FLAGS(wid, GTK_MAPPED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } طالما (0)
#  define gtk_widget_سېټ_realized(wid, rea) \
    افعل { إذا (rea) \
	    { GTK_WIDGET_سېټ_FLAGS(wid, GTK_REALIZED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } طالما (0)
# endif
#د_که_پای

#إذا defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# إذا GTK_CHECK_نسخه(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#د_که_پای

#که_نه_تعریف_شوی FEAT_NETBEANS_INTG
# undef NBDEBUG
#د_که_پای
#که_تعریف_شوی NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#وإلا
# define nbdebug(a)
#د_که_پای

#که_تعریف_شوی دننه_PERL_FILE
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
# إذا (defined(MACOS_X) && !defined(MAC_OS_X_نسخه_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#د_که_پای

// values for vim_handle_signal() that are not a signal
#تعریف_کړه SIGNAL_بلاک	(-1)
#تعریف_کړه SIGNAL_UNBLOCK  (-2)
#إذا !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#د_که_پای

// flags for skip_vimgrep_pat()
#تعریف_کړه VGR_GLOBAL	1
#تعریف_کړه VGR_NOJUMP	2
#تعریف_کړه VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#تعریف_کړه BAD_REPLACE	'?'	// replace it with '?' (default)
#تعریف_کړه BAD_KEEP	(-1)	// leave it
#تعریف_کړه BAD_DROP	(-2)	// erase it

// last argument for do_source()
#تعریف_کړه DOSO_NONE	0
#تعریف_کړه DOSO_VIMRC	1	// loading vimrc file
#تعریف_کړه DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#تعریف_کړه VIF_WANT_INFO	    1	// load non-mark info
#تعریف_کړه VIF_WANT_MARKS	    2	// load file marks
#تعریف_کړه VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#تعریف_کړه VIF_FORCEIT	    8	// overwrite info already read
#تعریف_کړه VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#تعریف_کړه BFA_DEL		 1	// buffer is going to be deleted
#تعریف_کړه BFA_WIPE	 2	// buffer is going to be wiped out
#تعریف_کړه BFA_KEEP_UNDO	 4	// do not free undo information
#تعریف_کړه BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#تعریف_کړه MSCR_ښکته	0	// DOWN must be FALSE
#تعریف_کړه MSCR_پورته		1
#تعریف_کړه MSCR_کیڼ	(-1)
#تعریف_کړه MSCR_ښی	(-2)

#تعریف_کړه KEYLEN_PART_کلی (-1)	// keylen value for incomplete key-code
#تعریف_کړه KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#تعریف_کړه KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#تعریف_کړه FILEINFO_OK	     0
#تعریف_کړه FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#تعریف_کړه FILEINFO_READ_FAIL   2	// CreateFile() failed
#تعریف_کړه FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#تعریف_کړه SOPT_BOOL	0x01	// Boolean option
#تعریف_کړه SOPT_عدد	0x02	// Number option
#تعریف_کړه SOPT_STRING	0x04	// String option
#تعریف_کړه SOPT_GLOBAL	0x08	// Option has global value
#تعریف_کړه SOPT_WIN	0x10	// Option has window-local value
#تعریف_کړه SOPT_BUF	0x20	// Option has buffer-local value
#تعریف_کړه SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#تعریف_کړه SREQ_GLOBAL	0	// Request global option
#تعریف_کړه SREQ_WIN	1	// Request window-local option
#تعریف_کړه SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#تعریف_کړه GREG_NO_بیان	1	// Do not allow expression register
#تعریف_کړه GREG_بیان_SRC	2	// Return expression itself for "=" register
#تعریف_کړه GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#تعریف_کړه AUTOLOAD_CHAR '#'

#که_تعریف_شوی FEAT_دنده_CHANNEL
# define MAX_OPEN_CHANNELS 10
#وإلا
# define MAX_OPEN_CHANNELS 0
#د_که_پای

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#که_تعریف_شوی FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#وإلا
# define MAX_CLIENT_CHANNELS 0
#د_که_پای

#إذا defined(MSWIN)
# define MAX_NAMED_PIPE_اندازه 65535
#د_که_پای

// Options for json_encode() and json_decode.
#تعریف_کړه JSON_JS		1   // use JS instead of JSON
#تعریف_کړه JSON_NO_NONE	2   // v:none item not allowed
#تعریف_کړه JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#تعریف_کړه DIP_ALL	    0x01	// all matches, not just the first one
#تعریف_کړه DIP_DIR	    0x02	// find directories instead of files.
#تعریف_کړه DIP_ERR	    0x04	// give an error message when none found.
#تعریف_کړه DIP_پیل   0x08	// also use "start" directory in 'packpath'
#تعریف_کړه DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#تعریف_کړه DIP_NORTP   0x20	// do not use 'runtimepath'
#تعریف_کړه DIP_NOAFTER 0x40	// skip "after" directories
#تعریف_کړه DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#تعریف_کړه ټیټ_ترین_WIN_ID 1000

// Used by the garbage collector.
#تعریف_کړه COPYID_INC 2
#تعریف_کړه COPYID_ماسک (~0x1)

// Values for trans_function_name() argument:
#تعریف_کړه TFN_INT		0x01	// internal function name OK
#تعریف_کړه TFN_QUIET	0x02	// no error messages
#تعریف_کړه TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#تعریف_کړه TFN_NO_DEREF	0x08	// do not dereference a Funcref
#تعریف_کړه TFN_READ_ONLY	0x10	// will not change the var
#تعریف_کړه TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#تعریف_کړه TFN_COMPILING	0x40	// only used for GLV_COMPILING
#تعریف_کړه TFN_NEW_FUNC	0x80	// defining a new function
#تعریف_کړه TFN_ټاکل_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#تعریف_کړه TFN_دننه_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#تعریف_کړه GLV_QUIET	TFN_QUIET	// no error messages
#تعریف_کړه GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#تعریف_کړه GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#تعریف_کړه GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#تعریف_کړه GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#تعریف_کړه GLV_ټاکل_WITH_OP TFN_ټاکل_WITH_OP // assignment with operator
#تعریف_کړه GLV_PREFER_FUNC	0x10000		// prefer function above variable
#تعریف_کړه GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#تعریف_کړه DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#تعریف_کړه FLEN_FIXED 40

// flags for find_name_end()
#تعریف_کړه FNE_INCL_BR	1	// include [] in name
#تعریف_کړه FNE_CHECK_پیل	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#إذا (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#د_که_پای

// stat macros
#که_نه_تعریف_شوی S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# وإلا
#  define S_ISDIR(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# وإلا
#  define S_ISREG(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# وإلا
#  define S_ISBLK(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# وإلا
#  define S_ISSOCK(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# وإلا
#  define S_ISFIFO(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# وإلا
#  define S_ISCHR(m)	0
# endif
#د_که_پای
#که_نه_تعریف_شوی S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# وإلا
#  define S_ISLNK(m)	0
# endif
#د_که_پای

#إذا defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وخت_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), عدم)
# define ELAPSED_FUNC(v) elapsed(&(v))
تعريفنوع بنية timeval elapsed_T;
اوږد elapsed(بنية timeval *پیل_tv);
#وإلاإذا defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_شمېر()
# define ELAPSED_FUNC(v) elapsed(v)
تعريفنوع DWORD elapsed_T;
# ifndef PROTO
اوږد elapsed(DWORD پیل_tick);
# endif
#د_که_پای

// Replacement for nchar used by nv_replace().
#تعریف_کړه REPLACE_CR_NCHAR    (-1)
#تعریف_کړه REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#تعریف_کړه غړی_پیل_NOJOB	1
#تعریف_کړه غړی_پیل_FORCEIT	2
#تعریف_کړه غړی_پیل_SYSTEM	4

// Used for icon/title save and restore.
#تعریف_کړه SAVE_RESTORE_TITLE	1
#تعریف_کړه SAVE_RESTORE_ICON	2
#تعریف_کړه SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#تعریف_کړه APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#تعریف_کړه APC_SUBSTITUTE		2   // text is replaced, not inserted
#تعریف_کړه APC_INDENT		4   // changing indent

#تعریف_کړه CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#تعریف_کړه REPTERM_FROM_PART	1
#تعریف_کړه REPTERM_DO_LT		2
#تعریف_کړه REPTERM_SPECIAL		4
#تعریف_کړه REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#تعریف_کړه FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#تعریف_کړه FSK_KEEP_X_کلی	0x02	// don't translate xHome to Home key
#تعریف_کړه FSK_دننه_STRING	0x04	// TRUE in string, double quote is escaped
#تعریف_کړه FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#تعریف_کړه FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#تعریف_کړه READDIR_ترتیبول_NONE	0  // do not sort
#تعریف_کړه READDIR_ترتیبول_BYTE	1  // sort by byte order (strcmp), default
#تعریف_کړه READDIR_ترتیبول_IC		2  // sort ignoring case (strcasecmp)
#تعریف_کړه READDIR_ترتیبول_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#تعریف_کړه MCH_DELAY_IGNOREINPUT	1
#تعریف_کړه MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#تعریف_کړه EVAL_VAR_VERBOSE	1   // may give error message
#تعریف_کړه EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#تعریف_کړه EVAL_VAR_IMPORT		4   // may return special variable for import
#تعریف_کړه EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#تعریف_کړه FUZZY_سمون_MAX_LEN	1024    // max characters that can be matched
#تعریف_کړه FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#تعریف_کړه ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#تعریف_کړه UC_بفر	1	// -buffer: local to current buffer
#تعریف_کړه UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#تعریف_کړه UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#تعریف_کړه VSE_NONE	0
#تعریف_کړه VSE_SHELL	1	// escape for a shell command
#تعریف_کړه VSE_بفر	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#تعریف_کړه FFED_IS_GLOBAL	1	// "g:" was used
#تعریف_کړه FFED_NO_GLOBAL	2	// only check for script-local functions

#تعریف_کړه MAX_LSHIFT_بټونه (varnumber_T)((حجم(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#تعریف_کړه CF_CLASS	1	// inside a class
#تعریف_کړه CF_INTERFACE	2	// inside an interface
#تعریف_کړه CF_ABSTRACT_لاره	4	// inside an abstract class

// Flags used by getvcol()
#تعریف_کړه GETVCOL_پای_ټکی_EXCL_LBR	1
#تعریف_کړه GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#تعریف_کړه GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#که_تعریف_شوی VMS
# define لار_ESC_WILDCARDS	"*?%"
#وإلا
# ifdef MSWIN
#  define لار_ESC_WILDCARDS	"*?["
# وإلا
#  define لار_ESC_WILDCARDS	"*?[{"
# endif
#د_که_پای

#د_که_پای // VIM__H
