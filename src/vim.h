/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_تعریف_نشده VIM__H
#تعریف_کن VIM__H

#ضمن "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#إذا defined(WIN32) || defined(_WIN32)
# define MSWIN
#پایان_اگر

#إذا defined(MSWIN)
# include <io.h>
#پایان_اگر

// ============ the header file puzzle: order matters =========

#اگر_تعریف_شده HAVE_CONFIG_H	// GNU autoconf (or something else) was here
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
#  error configure did not پیاپی properly.  Check خودکار/config.لگاریتم.
# endif

# إذا (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_منبع
#   define XOPEN_منبع    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_منبع
#    define BSD_منبع 1
#   endif
#   ifndef SVID_منبع
#    define SVID_منبع 1
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
#پایان_اگر

// for INT_MAX, LONG_MAX et al.
#ضمن <limits.h>

#إذا !defined(UINT32_TYPEDEF)
# إذا defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# وإلا
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF بیعلامت صحيح
# endif
#پایان_اگر

// C89 does not define SIZE_MAX
#إذا defined(__hpux) || defined(VMS)
# ifndef اندازه_MAX
#  define اندازه_MAX ((size_t)(-1))
# endif
#پایان_اگر

// user ID of root is usually zero, but not for everybody
#اگر_تعریف_شده __TANDEM
# ifndef TANDEM_منبع
#  define TANDEM_منبع
# endif
# include <floss.h>
# define ریشه_UID 65535
# define OLDXAW
# إذا (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#وإلا
# define ریشه_UID 0
#پایان_اگر

/* Include MAC_OS_X_VERSION_* macros */
#اگر_تعریف_شده HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#پایان_اگر

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#إذا defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#پایان_اگر
// Unless made through the Makefile enforce GUI on Mac
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#پایان_اگر

#إذا defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# إذا !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#پایان_اگر

// Check support for rendering options
#اگر_تعریف_شده FEAT_GUI
# إذا defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#پایان_اگر

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#اگر_تعریف_شده MSWIN
# define VIM_SIZEOF_INT 4
#پایان_اگر

#اگر_تعریف_شده AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  وإلا
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#پایان_اگر
#إذا defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#پایان_اگر

#إذا VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only کار_میکند with 32 بیت صحيح or larger
#پایان_اگر

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#ضمن "feature.h"

#إذا defined(MACOS_X_DARWIN)
# إذا defined(FEAT_نرمال) && !defined(FEAT_CLIPBOARD)
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
#پایان_اگر

// +x11 is only enabled when it's both available and wanted.
#إذا defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#پایان_اگر

#إذا defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#پایان_اگر

#اگر_تعریف_شده NO_X11_INCLUDES
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
#پایان_اگر

// The Mac conversion stuff doesn't work under X11.
#إذا defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#پایان_اگر

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#اگر_تعریف_نشده VIMPACKAGE
# define VIMPACKAGE	"vim"
#پایان_اگر

/*
 * Find out if function definitions should include argument types
 */
#اگر_تعریف_شده AZTEC_C
# include <functions.h>
#پایان_اگر

#اگر_تعریف_شده SASC
# include <clib/exec_protos.h>
#پایان_اگر

#اگر_تعریف_شده _DCC
# include <clib/exec_protos.h>
#پایان_اگر

#اگر_تعریف_نشده PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#پایان_اگر

#إذا (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#وإلاإذا !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) إشارة(signum, sighandler)
#پایان_اگر

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#اگر_تعریف_نشده استفادهنشده
# إذا defined(HAVE_ATTRIBUTE_استفادهنشده) || defined(__MINGW32__)
#  define استفادهنشده __attribute__((استفادهنشده_2))
# وإلا
#  إذا defined __has_attribute
#   إذا __has_attribute(استفادهنشده_2)
#    define استفادهنشده __attribute__((استفادهنشده_2))
#   endif
#  endif
# endif
# ifndef استفادهنشده
#  define استفادهنشده
# endif
#پایان_اگر

// Used to check for "sun", "__sun" is used by newer compilers.
#إذا defined(__sun)
# define SUN_SYSTEM
#پایان_اگر

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#إذا !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#پایان_اگر

#اگر_تعریف_نشده PROTO
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
#پایان_اگر

#اگر_تعریف_شده X_LOCALE
# include <X11/Xlocale.h>
#وإلا
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#پایان_اگر

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#اگر_تعریف_نشده MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# وإلا
#  define MAXPATHL  256
# endif
#پایان_اگر
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
# define مسیر_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define میانگیر_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#وإلا
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define مسیر_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# وإلا
#  define مسیر_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define میانگیر_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#پایان_اگر

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#تعریف_کن NUMBUFLEN 65

// flags for vim_str2nr()
#تعریف_کن STR2NR_BIN  0x01
#تعریف_کن STR2NR_OCT  0x02
#تعریف_کن STR2NR_HEX  0x04
#تعریف_کن STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#تعریف_کن STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#تعریف_کن STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#تعریف_کن STR2NR_FORCE 0x80   // only when ONE of the above is used

#تعریف_کن STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
تعريفنوع بیعلامت حرف	char_u;
تعريفنوع بیعلامت کوتاه	short_u;
تعريفنوع بیعلامت صحيح	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#اگر_تعریف_شده _WIN64
تعريفنوع بیعلامت __int64	long_u;
تعريفنوع		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#وإلا
تعريفنوع بیعلامت دراز		long_u;
تعريفنوع		 دراز		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#پایان_اگر
#تعریف_کن PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#إذا defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#پایان_اگر

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
#  define vim_fseek(a, b, c)	fseek(a, (دراز)b, c)
# endif
#پایان_اگر

/*
 * The characters and attributes cached for the screen.
 */
تعريفنوع char_u schar_T;
تعريفنوع بیعلامت کوتاه sattr_T;
#تعریف_کن MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
تعريفنوع بیعلامت صحيح u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#إذا defined(VMS) && defined(VAX)
// unsupported compiler
تعريفنوع دراز      vimlong_T;
#وإلا
تعريفنوع دراز دراز vimlong_T;
#پایان_اگر

#اگر_تعریف_نشده UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#پایان_اگر

#ضمن "ascii.h"
#ضمن "keymap.h"
#ضمن "termdefs.h"
#ضمن "macros.h"

#اگر_تعریف_شده LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#پایان_اگر
#اگر_تعریف_شده _DCC
# include <sys/stat.h>
#پایان_اگر
#إذا defined(MSWIN)
# include <sys/stat.h>
#پایان_اگر

#إذا defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#پایان_اگر

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#اگر_تعریف_نشده UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# إذا defined(HAVE_رشتهها_H) && !defined(NO_رشتهها_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#پایان_اگر // NON-UNIX

#ضمن <assert.h>

#اگر_تعریف_شده HAVE_STDINT_H
# include <stdint.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_INTTYPES_H
# include <inttypes.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_WCTYPE_H
# include <wctype.h>
#پایان_اگر
#ضمن <stdarg.h>
// older compilers do not define va_copy
#اگر_تعریف_نشده va_copy
# define va_copy(dst, src)	((dst) = (src))
#پایان_اگر

// for offsetof()
#ضمن <stddef.h>
#ضمن <stdbool.h>

#إذا defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_زمان_H) || defined(SYS_SELECT_WITH_SYS_زمان))
# include <sys/select.h>
#پایان_اگر

#اگر_تعریف_نشده HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# وإلا
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#پایان_اگر

#اگر_تعریف_شده HAVE_SODIUM
# include <sodium.h>
#پایان_اگر

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#اگر_تعریف_شده DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#پایان_اگر

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#اگر_تعریف_شده DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
خارجي حرف *(*dyn_libintl_gettext)(ثابت حرف *msgid);
خارجي حرف *(*dyn_libintl_ngettext)(ثابت حرف *msgid, ثابت حرف *msgid_plural, بیعلامت دراز n);
خارجي حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *domainname, ثابت حرف *dirname);
خارجي حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *domainname, ثابت حرف *codeset);
خارجي حرف *(*dyn_libintl_textdomain)(ثابت حرف *domainname);
خارجي صحيح (*dyn_libintl_wputenv)(ثابت wchar_t *envstring);
خارجي صحيح *dyn_libintl_nl_پیام_cat_cntr;
#پایان_اگر


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#اگر_تعریف_شده FEAT_GETTEXT
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
# define bind_textdomain_codeset(x, y) //خالی
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //خالی
#پایان_اگر

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#تعریف_کن UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#تعریف_کن UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#تعریف_کن UPD_INVERTED		20  // redisplay inverted part that changed
#تعریف_کن UPD_INVERTED_ALL	25  // redisplay whole inverted part
#تعریف_کن UPD_REDRAW_بالاترین		30  // display first w_upd_rows screen lines
#تعریف_کن UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#تعریف_کن UPD_NOT_VALID		40  // buffer needs complete redraw
#تعریف_کن UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#تعریف_کن SLF_RIGHTLEFT	1
#تعریف_کن SLF_POPUP	2
#تعریف_کن SLF_INC_VCOL	4

#تعریف_کن MB_FILLER_CHAR '<'  // character used when a double-width character
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
#تعریف_کن VALID_WROW	0x01	// w_wrow (window row) is valid
#تعریف_کن VALID_WCOL	0x02	// w_wcol (window col) is valid
#تعریف_کن VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#تعریف_کن VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#تعریف_کن VALID_CROW	0x10	// w_cline_row is valid
#تعریف_کن VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#تعریف_کن VALID_BOTLINE_AP 0x40	// w_botline is approximated
#تعریف_کن VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#تعریف_کن POPF_IS_POPUP	0x01	// this is a popup window
#تعریف_کن POPF_HIDDEN	0x02	// popup is not displayed
#تعریف_کن POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#تعریف_کن POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#تعریف_کن POPF_ON_CMDLINE	0x10	// popup overlaps command line
#تعریف_کن POPF_DRAG	0x20	// popup can be moved by dragging border
#تعریف_کن POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#تعریف_کن POPF_RESIZE	0x80	// popup can be resized by dragging
#تعریف_کن POPF_MAPPING	0x100	// mapping keys
#تعریف_کن POPF_INFO	0x200	// used for info of popup menu
#تعریف_کن POPF_INFO_MENU	0x400	// align info popup with popup menu
#تعریف_کن POPF_POSINVERT	0x800	// vertical position can be inverted
#تعریف_کن POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#تعریف_کن POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#تعریف_کن POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#تعریف_کن POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#تعریف_کن POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#تعریف_کن POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#تعریف_کن POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#تعریف_کن POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#تعریف_کن HL_نرمال		0x00
#تعریف_کن HL_INVERSE		0x01
#تعریف_کن HL_BOLD			0x02
#تعریف_کن HL_ITALIC		0x04
#تعریف_کن HL_UNDERLINE		0x08
#تعریف_کن HL_UNDERCURL		0x10
#تعریف_کن HL_UNDERDOUBLE		0x20
#تعریف_کن HL_UNDERDOTTED		0x40
#تعریف_کن HL_UNDERDASHED		0x80
#تعریف_کن HL_STANDOUT		0x100
#تعریف_کن HL_NOCOMBINE		0x200
#تعریف_کن HL_STRIKETHROUGH	0x400
#تعریف_کن HL_ALL			0x7ff

// special attribute addition: Put message in history
#تعریف_کن پیام_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#تعریف_کن نما_نرمال	0x01	// Normal mode, command expected
#تعریف_کن نما_VISUAL	0x02	// Visual mode - use get_real_state()
#تعریف_کن نما_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#تعریف_کن نما_CMDLINE	0x08	// Editing the command line
#تعریف_کن نما_درج	0x10	// Insert mode, also for Replace mode
#تعریف_کن نما_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#تعریف_کن نما_SELECT	0x40	// Select mode, use get_real_state()
#تعریف_کن نما_TERMINAL	0x80	// Terminal mode

#تعریف_کن MAP_ALL_MODES	0xff    // all mode bits used for mapping

#تعریف_کن REPLACE_FLAG	0x100	// Replace mode flag
#تعریف_کن نما_REPLACE	(REPLACE_FLAG | نما_درج)
#تعریف_کن VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#تعریف_کن نما_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | نما_درج)
#تعریف_کن نما_LREPLACE	(REPLACE_FLAG | نما_LANGMAP)

#تعریف_کن نما_نرمال_BUSY (0x1000 | نما_نرمال)
				// Normal mode, busy with a command
#تعریف_کن نما_HITRETURN	(0x2000 | نما_نرمال)
				// waiting for return or command
#تعریف_کن نما_ASKMORE	0x3000	// Asking if you want --more--
#تعریف_کن نما_SETWSIZE	0x4000	// window size has changed
#تعریف_کن نما_EXTERNCMD	0x5000	// executing an external command
#تعریف_کن نما_SHOWMATCH	(0x6000 | نما_درج) // show matching paren
#تعریف_کن نما_CONFIRM	0x7000	// ":confirm" prompt
#تعریف_کن نما_ALL	0xffff

#تعریف_کن نما_MAX_درازی	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#تعریف_کن FORWARD			1
#تعریف_کن BACKWARD		(-1)
#تعریف_کن FORWARD_FILE		3
#تعریف_کن BACKWARD_FILE		(-3)

// return values for functions
#إذا !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#پایان_اگر
#تعریف_کن FAIL			0
#تعریف_کن NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#تعریف_کن BF_RECOVERED	0x01	// buffer has been recovered
#تعریف_کن BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#تعریف_کن BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#تعریف_کن BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#تعریف_کن BF_NEW		0x10	// file didn't exist when editing started
#تعریف_کن BF_NEW_W	0x20	// Warned for BF_NEW and file created
#تعریف_کن BF_READERR	0x40	// got errors while reading the file
#تعریف_کن BF_DUMMY	0x80	// dummy buffer, only used internally
#تعریف_کن BF_PRESERVED	0x100	// ":preserve" was used
#تعریف_کن BF_SYN_مجموعه	0x200	// 'syntax' option was set
#تعریف_کن BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#تعریف_کن BF_WRITE_ماسک	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#تعریف_کن EXPAND_UNSUCCESSFUL	(-2)
#تعریف_کن EXPAND_OK		(-1)
#تعریف_کن EXPAND_NOTHING		0
#تعریف_کن EXPAND_COMMANDS		1
#تعریف_کن EXPAND_FILES		2
#تعریف_کن EXPAND_DIRECTORIES	3
#تعریف_کن EXPAND_SETTINGS		4
#تعریف_کن EXPAND_BOOL_SETTINGS	5
#تعریف_کن EXPAND_برچسبها		6
#تعریف_کن EXPAND_OLD_SETTING	7
#تعریف_کن EXPAND_HELP		8
#تعریف_کن EXPAND_BUFFERS		9
#تعریف_کن EXPAND_EVENTS		10
#تعریف_کن EXPAND_MENUS		11
#تعریف_کن EXPAND_SYNTAX		12
#تعریف_کن EXPAND_HIGHLIGHT	13
#تعریف_کن EXPAND_AUGROUP		14
#تعریف_کن EXPAND_USER_VARS	15
#تعریف_کن EXPAND_MAPPINGS		16
#تعریف_کن EXPAND_برچسبها_LISTFILES	17
#تعریف_کن EXPAND_FUNCTIONS	18
#تعریف_کن EXPAND_USER_FUNC	19
#تعریف_کن EXPAND_عبارت	20
#تعریف_کن EXPAND_MENUNAMES	21
#تعریف_کن EXPAND_USER_COMMANDS	22
#تعریف_کن EXPAND_USER_CMD_FLAGS	23
#تعریف_کن EXPAND_USER_NARGS	24
#تعریف_کن EXPAND_USER_COMPLETE	25
#تعریف_کن EXPAND_ENV_VARS		26
#تعریف_کن EXPAND_LANGUAGE		27
#تعریف_کن EXPAND_COLORS		28
#تعریف_کن EXPAND_COMPILER		29
#تعریف_کن EXPAND_USER_DEFINED	30
#تعریف_کن EXPAND_USER_LIST	31
#تعریف_کن EXPAND_SHELLCMD		32
#تعریف_کن EXPAND_CSCOPE		33
#تعریف_کن EXPAND_SIGN		34
#تعریف_کن EXPAND_PROFILE		35
#تعریف_کن EXPAND_BEHAVE		36
#تعریف_کن EXPAND_FILETYPE		37
#تعریف_کن EXPAND_FILES_درون_مسیر	38
#تعریف_کن EXPAND_OWNSYNTAX	39
#تعریف_کن EXPAND_LOCALES		40
#تعریف_کن EXPAND_HISTORY		41
#تعریف_کن EXPAND_USER		42
#تعریف_کن EXPAND_SYNTIME		43
#تعریف_کن EXPAND_USER_ADDR_TYPE	44
#تعریف_کن EXPAND_PACKADD		45
#تعریف_کن EXPAND_MESSAGES		46
#تعریف_کن EXPAND_MAPCLEAR		47
#تعریف_کن EXPAND_ARGLIST		48
#تعریف_کن EXPAND_DIFF_BUFFERS	49
#تعریف_کن EXPAND_DISASSEMBLE	50
#تعریف_کن EXPAND_BREAKPOINT	51
#تعریف_کن EXPAND_SCRIPTNAMES	52
#تعریف_کن EXPAND_RUNTIME		53
#تعریف_کن EXPAND_STRING_SETTING	54
#تعریف_کن EXPAND_SETTING_تفریق	55
#تعریف_کن EXPAND_ARGOPT		56
#تعریف_کن EXPAND_TERMINALOPT	57
#تعریف_کن EXPAND_KEYMAP		58
#تعریف_کن EXPAND_DIRS_درون_CDPATH	59
#تعریف_کن EXPAND_SHELLCMDLINE	60
#تعریف_کن EXPAND_FINDFUNC		61
#تعریف_کن EXPAND_HIGHLIGHT_گروه  62
#تعریف_کن EXPAND_FILETYPECMD	63
#تعریف_کن EXPAND_الگو_درون_BUF	64
#تعریف_کن EXPAND_RETAB		65
#تعریف_کن EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#تعریف_کن EXMODE_نرمال		1
#تعریف_کن EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#تعریف_کن WILD_FREE		1
#تعریف_کن WILD_EXPAND_FREE	2
#تعریف_کن WILD_EXPAND_KEEP	3
#تعریف_کن WILD_بعدی		4
#تعریف_کن WILD_PREV		5
#تعریف_کن WILD_ALL		6
#تعریف_کن WILD_LONGEST		7
#تعریف_کن WILD_ALL_KEEP		8
#تعریف_کن WILD_CANCEL		9
#تعریف_کن WILD_APPLY		10
#تعریف_کن WILD_PAGEUP		11
#تعریف_کن WILD_PAGEDOWN		12

#تعریف_کن WILD_LIST_NOTFOUND	    0x01
#تعریف_کن WILD_HOME_REPLACE	    0x02
#تعریف_کن WILD_USE_NL		    0x04
#تعریف_کن WILD_NO_BEEP		    0x08
#تعریف_کن WILD_جمع_کن_SLASH		    0x10
#تعریف_کن WILD_KEEP_ALL		    0x20
#تعریف_کن WILD_SILENT		    0x40
#تعریف_کن WILD_ESCAPE		    0x80
#تعریف_کن WILD_ICASE		    0x100
#تعریف_کن WILD_ALLLINKS		    0x200
#تعریف_کن WILD_IGNORE_COMPLETESLASH   0x400
#تعریف_کن WILD_NOERROR		    0x800  // sets EW_NOERROR
#تعریف_کن WILD_BUFLASTUSED	    0x1000
#تعریف_کن BUF_DIFF_FILTER		    0x2000
#تعریف_کن WILD_NOSELECT		    0x4000
#تعریف_کن WILD_MAY_EXPAND_الگو	    0x8000
#تعریف_کن WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#تعریف_کن WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#تعریف_کن EW_DIR		0x01	// include directory names
#تعریف_کن EW_FILE		0x02	// include file names
#تعریف_کن EW_NOTFOUND	0x04	// include not found names
#تعریف_کن EW_ADDSLASH	0x08	// append slash to directory name
#تعریف_کن EW_KEEPALL	0x10	// keep all matches
#تعریف_کن EW_SILENT	0x20	// don't print "1 returned" from shell
#تعریف_کن EW_EXEC		0x40	// executable files
#تعریف_کن EW_مسیر		0x80	// search in 'path' too
#تعریف_کن EW_ICASE	0x100	// ignore case
#تعریف_کن EW_NOERROR	0x200	// no error for bad regexp
#تعریف_کن EW_NOTWILD	0x400	// add match with literal name if exists
#تعریف_کن EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#تعریف_کن EW_ALLLINKS	0x1000	// also links not pointing to existing file
#تعریف_کن EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#تعریف_کن EW_DODOT	0x4000	// also files starting with a dot
#تعریف_کن EW_EMPTYOK	0x8000	// no matches is not an error
#تعریف_کن EW_NOTENV	0x10000	// do not expand environment variables
#تعریف_کن EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#تعریف_کن FINDFILE_FILE	0	// only files
#تعریف_کن FINDFILE_DIR	1	// only directories
#تعریف_کن FINDFILE_BOTH	2	// files and directories

#إذا defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_عرض())
# define TPL_LCOL()			tabpanel_leftcol()
#وإلا
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#پایان_اگر

#تعریف_کن W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_عرض)
#اگر_تعریف_شده FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_ارتفاع)
#وإلا
# define W_WINROW(wp)	(wp->w_winrow)
#پایان_اگر

// Values for the find_pattern_in_path() function args 'type' and 'action':
#تعریف_کن پیدا_کن_ANY	1
#تعریف_کن پیدا_کن_DEFINE	2
#تعریف_کن CHECK_مسیر	3

#تعریف_کن ACTION_SHOW	1
#تعریف_کن ACTION_GOTO	2
#تعریف_کن ACTION_تقسیم	3
#تعریف_کن ACTION_SHOW_ALL	4
#تعریف_کن ACTION_EXPAND	5

#اگر_تعریف_شده FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_حالتها	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_اینجا	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_تطابق	0x40	// use match ID instead of item ID
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
#پایان_اگر

// Values for 'options' argument in do_search() and searchit()
#تعریف_کن جستجو_REV    0x01  // go in reverse of previous dir.
#تعریف_کن جستجو_ECHO   0x02  // echo the search command and handle options
#تعریف_کن جستجو_پیام    0x0c  // give messages (yes, it's not 0x04)
#تعریف_کن جستجو_NFMSG  0x08  // give all messages except not found
#تعریف_کن جستجو_OPT    0x10  // interpret optional flags
#تعریف_کن جستجو_HIS    0x20  // put search pattern in history
#تعریف_کن جستجو_انتها    0x40  // put cursor at end of match
#تعریف_کن جستجو_NOOF   0x80  // don't add offset to position
#تعریف_کن جستجو_شروع 0x100  // start search without col offset
#تعریف_کن جستجو_MARK  0x200  // set previous context mark
#تعریف_کن جستجو_KEEP  0x400  // keep previous search pattern
#تعریف_کن جستجو_PEEK  0x800  // peek for typed char, cancel search
#تعریف_کن جستجو_ستون  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#تعریف_کن پیدا_کن_IDENT	1	// find identifier (word)
#تعریف_کن پیدا_کن_STRING	2	// find any string (WORD)
#تعریف_کن پیدا_کن_EVAL	4	// include "->", "[]" and "."
#تعریف_کن پیدا_کن_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#تعریف_کن FNAME_MESS	1	// give error message
#تعریف_کن FNAME_EXP	2	// expand to path
#تعریف_کن FNAME_HYP	4	// check for hypertext link
#تعریف_کن FNAME_INCL	8	// apply 'includeexpr'
#تعریف_کن FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#تعریف_کن FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#تعریف_کن GETF_SETMARK	0x01	// set pcmark before jumping
#تعریف_کن GETF_ALT	0x02	// jumping to alternate file (not buf num)
#تعریف_کن GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#تعریف_کن GETFILE_ERROR	    1	// normal error
#تعریف_کن GETFILE_NOT_WRITTEN 2	// "not written" error
#تعریف_کن GETFILE_SAME_FILE   0	// success, same file
#تعریف_کن GETFILE_OPEN_OTHER (-1)	// success, opened another file
#تعریف_کن GETFILE_استفادهنشده	    8
#تعریف_کن GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#تعریف_کن BLN_CURBUF	1	// may re-use curbuf for new buffer
#تعریف_کن BLN_LISTED	2	// put new buffer in buffer list
#تعریف_کن BLN_DUMMY	4	// allocating dummy buffer
#تعریف_کن BLN_NEW		8	// create a new buffer
#تعریف_کن BLN_NOOPT	16	// don't copy options to existing buffer
#تعریف_کن BLN_DUMMY_OK	32	// also find an existing dummy buffer
#تعریف_کن BLN_REUSE	64	// may re-use number from buf_reuse
#تعریف_کن BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#تعریف_کن کلید_OPEN_FORW	0x101
#تعریف_کن کلید_OPEN_پشت	0x102
#تعریف_کن کلید_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#تعریف_کن MAPTYPE_MAP		0
#تعریف_کن MAPTYPE_UNMAP		1
#تعریف_کن MAPTYPE_NOREMAP		2
#تعریف_کن MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#تعریف_کن REMAP_YES	0	// allow remapping
#تعریف_کن REMAP_NONE	(-1)	// no remapping
#تعریف_کن REMAP_SCRIPT	(-2)	// remap script-local mappings only
#تعریف_کن REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#تعریف_کن SHELL_FILTER	1	// filtering text
#تعریف_کن SHELL_EXPAND	2	// expanding wildcards
#تعریف_کن SHELL_COOKED	4	// set term to cooked mode
#تعریف_کن SHELL_DOOUT	8	// redirecting output
#تعریف_کن SHELL_SILENT	16	// don't print error returned by command
#تعریف_کن SHELL_READ	32	// read lines and insert into buffer
#تعریف_کن SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#تعریف_کن گره_نرمال	0	// file or directory, check with mch_isdir()
#تعریف_کن گره_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#تعریف_کن گره_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#تعریف_کن READ_NEW	0x01	// read a file into a new buffer
#تعریف_کن READ_FILTER	0x02	// read filter output
#تعریف_کن READ_STDIN	0x04	// read from stdin
#تعریف_کن READ_میانگیر	0x08	// read from curbuf (converting stdin)
#تعریف_کن READ_DUMMY	0x10	// reading into a dummy buffer
#تعریف_کن READ_KEEP_UNDO	0x20	// keep undo info
#تعریف_کن READ_FIFO	0x40	// read from fifo or socket
#تعریف_کن READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#تعریف_کن READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#تعریف_کن INDENT_مجموعه	1	// set indent
#تعریف_کن INDENT_INC	2	// increase indent
#تعریف_کن INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#تعریف_کن FM_BACKWARD	0x01	// search backwards
#تعریف_کن FM_FORWARD	0x02	// search forwards
#تعریف_کن FM_BLOCKSTOP	0x04	// stop at start/end of block
#تعریف_کن FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#تعریف_کن DOBUF_GOTO	0	// go to specified buffer
#تعریف_کن DOBUF_تقسیم	1	// split window and go to specified buffer
#تعریف_کن DOBUF_UNLOAD	2	// unload specified buffer(s)
#تعریف_کن DOBUF_DEL	3	// delete specified buffer(s) from buflist
#تعریف_کن DOBUF_WIPE	4	// delete specified buffer(s) really
#تعریف_کن DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#تعریف_کن DOBUF_CURRENT	0	// "count" buffer from current buffer
#تعریف_کن DOBUF_اولی	1	// "count" buffer from first buffer
#تعریف_کن DOBUF_آخرین	2	// "count" buffer from last buffer
#تعریف_کن DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#تعریف_کن DOBUF_FORCEIT	1	// :cmd!
#تعریف_کن DOBUF_NOPOPUP	2	// skip popup window buffers
#تعریف_کن DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#تعریف_کن RE_جستجو	0	// save/use pat in/from search_pattern
#تعریف_کن RE_SUBST	1	// save/use pat in/from subst_pattern
#تعریف_کن RE_BOTH		2	// save pat in both patterns
#تعریف_کن RE_آخرین		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#تعریف_کن RE_MAGIC	1	// 'magic' option
#تعریف_کن RE_STRING	2	// match in string instead of buffer text
#تعریف_کن RE_STRICT	4	// don't allow [abc] without ]
#تعریف_کن RE_AUTO		8	// automatic engine selection

#اگر_تعریف_شده FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_مجموعه	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_مجموعه | REX_USE)
#پایان_اگر

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#تعریف_کن FPC_SAME	1	// both exist and are the same file.
#تعریف_کن FPC_DIFF	2	// both exist and are different files.
#تعریف_کن FPC_NOTX	4	// both don't exist.
#تعریف_کن FPC_DIFFX	6	// one of them doesn't exist.
#تعریف_کن FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#تعریف_کن ECMD_HIDE	0x01	// don't free the current buffer
#تعریف_کن ECMD_مجموعه_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#تعریف_کن ECMD_OLDBUF	0x04	// use existing buffer if it exists
#تعریف_کن ECMD_FORCEIT	0x08	// ! used in Ex command
#تعریف_کن ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#تعریف_کن ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#تعریف_کن ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#تعریف_کن ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#تعریف_کن ECMD_آخرین	((linenr_T)-1)	// use last position in all files
#تعریف_کن ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#تعریف_کن DOCMD_VERBOSE	0x01	// included command in error message
#تعریف_کن DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#تعریف_کن DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#تعریف_کن DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#تعریف_کن DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#تعریف_کن DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#تعریف_کن DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#تعریف_کن BL_WHITE	1	// cursor on first non-white in the line
#تعریف_کن BL_SOL		2	// use 'sol' option
#تعریف_کن BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#تعریف_کن MFS_ALL		1	// also sync blocks with negative numbers
#تعریف_کن MFS_STOP	2	// stop syncing when a character is available
#تعریف_کن MFS_FLUSH	4	// flushed file to disk
#تعریف_کن MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#تعریف_کن BCO_ENTER	1	// going to enter the buffer
#تعریف_کن BCO_ALWAYS	2	// always copy the options
#تعریف_کن BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#تعریف_کن PUT_FIXINDENT	1	// make indent look nice
#تعریف_کن PUT_CURSEND	2	// leave cursor after end of new text
#تعریف_کن PUT_CURSLINE	4	// leave cursor on last line of new text
#تعریف_کن PUT_سطر	8	// put register as lines
#تعریف_کن PUT_سطر_تقسیم	16	// split line for linewise register
#تعریف_کن PUT_سطر_FORWARD 32	// put linewise register below Visual sel.
#تعریف_کن PUT_بلوک_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#تعریف_کن SIN_تغییریافته	1	// call changed_bytes() when line changed
#تعریف_کن SIN_درج	2	// insert indent before existing text
#تعریف_کن SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#تعریف_کن INSCHAR_FORMAT	1	// force formatting
#تعریف_کن INSCHAR_DO_COM	2	// format comments
#تعریف_کن INSCHAR_CTRLV	4	// char typed just after CTRL-V
#تعریف_کن INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#تعریف_کن INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#تعریف_کن OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#تعریف_کن OPENLINE_DO_COM		0x02    // format comments
#تعریف_کن OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#تعریف_کن OPENLINE_MARKFIX	0x08    // fix mark positions
#تعریف_کن OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#تعریف_کن OPENLINE_FORMAT		0x20    // formatting long comment
#تعریف_کن OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#تعریف_کن HIST_CMD	0	// colon commands
#تعریف_کن HIST_جستجو	1	// search commands
#تعریف_کن HIST_عبارت	2	// expressions (from entering = register)
#تعریف_کن HIST_درون_داد	3	// input() lines
#تعریف_کن HIST_DEBUG	4	// debug commands
#تعریف_کن HIST_شمار	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#تعریف_کن BARTYPE_نسخه 1
#تعریف_کن BARTYPE_HISTORY 2
#تعریف_کن BARTYPE_ثبات 3
#تعریف_کن BARTYPE_MARK 4

#تعریف_کن VIMINFO_نسخه 4
#تعریف_کن VIMINFO_نسخه_WITH_HISTORY 2
#تعریف_کن VIMINFO_نسخه_WITH_REGISTERS 3
#تعریف_کن VIMINFO_نسخه_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#تعریف_کن DT_برچسب		1	// jump to newer position or same tag again
#تعریف_کن DT_برداشت		2	// jump to older position
#تعریف_کن DT_بعدی		3	// jump to next match of same tag
#تعریف_کن DT_PREV		4	// jump to previous match of same tag
#تعریف_کن DT_اولی	5	// jump to first match of same tag
#تعریف_کن DT_آخرین		6	// jump to first match of same tag
#تعریف_کن DT_SELECT	7	// jump to selection from list
#تعریف_کن DT_HELP		8	// like DT_TAG, but no wildcards
#تعریف_کن DT_JUMP		9	// jump to new tag or selection from list
#تعریف_کن DT_CSCOPE	10	// cscope find command (like tjump)
#تعریف_کن DT_LTAG		11	// tag using location list
#تعریف_کن DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#تعریف_کن برچسب_HELP	1	// only search for help tags
#تعریف_کن برچسب_نامها	2	// only return name of tag
#تعریف_کن	برچسب_REGEXP	4	// use tag pattern as regexp
#تعریف_کن	برچسب_NOIC	8	// don't always ignore case
#اگر_تعریف_شده FEAT_CSCOPE
# define برچسب_CSCOPE	16	// cscope tag
#پایان_اگر
#تعریف_کن برچسب_VERBOSE	32	// message verbosity
#تعریف_کن برچسب_INS_COMP	64	// Currently doing insert completion
#تعریف_کن برچسب_KEEP_LANG	128	// keep current language
#تعریف_کن برچسب_NO_TAGFUNC	256	// do not use 'tagfunc'

#تعریف_کن برچسب_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#تعریف_کن VIM_GENERIC	0
#تعریف_کن VIM_ERROR	1
#تعریف_کن VIM_WARNING	2
#تعریف_کن VIM_INFO	3
#تعریف_کن VIM_QUESTION	4
#تعریف_کن VIM_آخرین_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#تعریف_کن VIM_YES		2
#تعریف_کن VIM_NO		3
#تعریف_کن VIM_CANCEL	4
#تعریف_کن VIM_ALL		5
#تعریف_کن VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#تعریف_کن WSP_جا	0x01	// require enough room
#تعریف_کن WSP_VERT	0x02	// split/equalize vertically
#تعریف_کن WSP_HOR		0x04	// equalize horizontally
#تعریف_کن WSP_بالاترین		0x08	// window at top-left of shell
#تعریف_کن WSP_BOT		0x10	// window at bottom-right of shell
#تعریف_کن WSP_HELP	0x20	// creating the help window
#تعریف_کن WSP_زیر	0x40	// put new window below/right
#تعریف_کن WSP_ABOVE	0x80	// put new window above/left
#تعریف_کن WSP_NEWLOC	0x100	// don't copy location list
#تعریف_کن WSP_FORCE_جا	0x200	// ignore "not enough room" errors
#تعریف_کن WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#تعریف_کن RESIZE_VERT	1	// resize vertically
#تعریف_کن RESIZE_HOR	2	// resize horizontally
#تعریف_کن RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#تعریف_کن CCGD_AW		1	// do autowrite if buffer was changed
#تعریف_کن CCGD_MULTWIN	2	// check also when several wins for the buf
#تعریف_کن CCGD_FORCEIT	4	// ! used
#تعریف_کن CCGD_ALLBUF	8	// may write all buffers
#تعریف_کن CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#تعریف_کن OPT_FREE	0x01	// free old value if it was allocated
#تعریف_کن OPT_GLOBAL	0x02	// use global value
#تعریف_کن OPT_LOCAL	0x04	// use local value
#تعریف_کن OPT_MODELINE	0x08	// option in modeline
#تعریف_کن OPT_WINONLY	0x10	// only set window-local options
#تعریف_کن OPT_NOWIN	0x20	// don't set window-local options
#تعریف_کن OPT_ONECOLUMN	0x40	// list options one per line
#تعریف_کن OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#تعریف_کن OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#تعریف_کن DLG_BUTTON_SEP	'\n'
#تعریف_کن DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#تعریف_کن NO_SCREEN	2	// no screen updating yet
#تعریف_کن NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#تعریف_کن SEA_NONE	0	// don't use dialog
#تعریف_کن SEA_DIALOG	1	// use dialog when possible
#تعریف_کن SEA_QUIT	2	// quit editing the file
#تعریف_کن SEA_RECOVER	3	// recover the file
#تعریف_کن SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#تعریف_کن MIN_جابجا_صفحه_اندازه 1048
#تعریف_کن MAX_جابجا_صفحه_اندازه 50000

// Special values for current_sctx.sc_sid.
#تعریف_کن SID_MODELINE	(-1)	// when using a modeline
#تعریف_کن SID_CMDARG	(-2)	// for "--cmd" argument
#تعریف_کن SID_CARG	(-3)	// for "-c" argument
#تعریف_کن SID_ENV		(-4)	// for sourcing environment variable
#تعریف_کن SID_ERROR	(-5)	// option was reset because of an error
#تعریف_کن SID_NONE	(-6)	// don't set scriptID
#تعریف_کن SID_WINLAYOUT	(-7)	// changing window size

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

    رقم_EVENTS,			// MUST be the last one
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
    , HLF_جمع_کن	    // Added diff line
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
    , HLF_برداشت	    // popup window body
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
    , HLF_پیام	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_شمار	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#تعریف_کن HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
#اگر_تعریف_نشده TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#پایان_اگر

#تعریف_کن MAYBE	2	    // sometimes used for a variant on TRUE

#تعریف_کن LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#اگر_تعریف_شده FEAT_کار_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_مقصد_LOG_THIS إذا (ch_log_output == FALSE) ch_log_output = TRUE;
#وإلا
// no logging support
# define MAY_WANT_مقصد_LOG_THIS
#پایان_اگر

#اگر_تعریف_نشده UINT32_T
تعريفنوع UINT32_TYPEDEF UINT32_T;
#پایان_اگر

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#تعریف_کن OP_NOP		0	// no pending operation
#تعریف_کن OP_DELETE	1	// "d"  delete operator
#تعریف_کن OP_YANK		2	// "y"  yank operator
#تعریف_کن OP_CHANGE	3	// "c"  change operator
#تعریف_کن OP_LSHIFT	4	// "<"  left shift operator
#تعریف_کن OP_RSHIFT	5	// ">"  right shift operator
#تعریف_کن OP_FILTER	6	// "!"  filter operator
#تعریف_کن OP_TILDE	7	// "g~" switch case operator
#تعریف_کن OP_INDENT	8	// "="  indent operator
#تعریف_کن OP_FORMAT	9	// "gq" format operator
#تعریف_کن OP_COLON	10	// ":"  colon operator
#تعریف_کن OP_فوقانی	11	// "gU" make upper case operator
#تعریف_کن OP_پایینی	12	// "gu" make lower case operator
#تعریف_کن OP_JOIN		13	// "J"  join operator, only for Visual mode
#تعریف_کن OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#تعریف_کن OP_ROT13	15	// "g?" rot-13 encoding
#تعریف_کن OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#تعریف_کن OP_درج	17	// "I"  Insert column, only for Visual mode
#تعریف_کن OP_APPEND	18	// "A"  Append column, only for Visual mode
#تعریف_کن OP_FOLD		19	// "zf" define a fold
#تعریف_کن OP_FOLDOPEN	20	// "zo" open folds
#تعریف_کن OP_FOLDOPENREC	21	// "zO" open folds recursively
#تعریف_کن OP_FOLDCLOSE	22	// "zc" close folds
#تعریف_کن OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#تعریف_کن OP_FOLDDEL	24	// "zd" delete folds
#تعریف_کن OP_FOLDDELREC	25	// "zD" delete folds recursively
#تعریف_کن OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#تعریف_کن OP_FUNCTION	27	// "g@" call 'operatorfunc'
#تعریف_کن OP_NR_جمع_کن	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#تعریف_کن OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#تعریف_کن MCHAR	0		// character-wise movement/register
#تعریف_کن MLINE	1		// line-wise movement/register
#تعریف_کن MBLOCK	2		// block-wise register

#تعریف_کن MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#تعریف_کن MIN_COLUMNS	12	// minimal columns for screen
#تعریف_کن MIN_سطرها	2	// minimal lines for screen
#تعریف_کن MIN_CMDHEIGHT	1	// minimal height for command line
#تعریف_کن وضعیت_ارتفاع_2	1	// default height of a status line under a
				//پنجره
#اگر_تعریف_شده FEAT_MENU		// height of a status line under a window
# define WINBAR_ارتفاع(wp)	(wp)->w_winbar_ارتفاع
# define VISIBLE_ارتفاع(wp)	((wp)->w_ارتفاع + (wp)->w_winbar_ارتفاع)
#وإلا
# define WINBAR_ارتفاع(wp)	0
# define VISIBLE_ارتفاع(wp)	(wp)->w_ارتفاع
#پایان_اگر
#تعریف_کن QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#اگر_تعریف_نشده CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#پایان_اگر

#تعریف_کن LSIZE	    512		// max. size of a line in the tags file

#تعریف_کن IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#تعریف_کن DIALOG_پیام_اندازه 1000	// buffer size for dialog_msg()

#تعریف_کن پیام_BUF_LEN 480	// length of buffer for small messages
#تعریف_کن پیام_BUF_CLEN  (پیام_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#تعریف_کن FOLD_متن_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#تعریف_کن TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#تعریف_کن MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#تعریف_کن MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#تعریف_کن UNDO_HASH_اندازه 32

#اگر_تعریف_شده HAVE_FCNTL_H
# include <fcntl.h>
#پایان_اگر

#اگر_تعریف_شده دودویی_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#وإلا
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#پایان_اگر

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#اگر_تعریف_شده __CYGWIN__
# define O_EXTRA    O_دودویی
#وإلا
# define O_EXTRA    0
#پایان_اگر

#اگر_تعریف_نشده O_NOFOLLOW
# define O_NOFOLLOW 0
#پایان_اگر

#اگر_تعریف_نشده W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#پایان_اگر
#اگر_تعریف_نشده R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#پایان_اگر

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#تعریف_کن ALLOC_ONE(type)  (type *)alloc(حجم(type))
#تعریف_کن ALLOC_ONE_ID(type, id)  (type *)alloc_id(حجم(type), id)
#تعریف_کن ALLOC_MULT(type, شمار)  (type *)alloc(حجم(type) * (شمار))
#تعریف_کن ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(حجم(type))
#تعریف_کن ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(حجم(type), id)
#تعریف_کن ALLOC_CLEAR_MULT(type, شمار)  (type *)alloc_clear(حجم(type) * (شمار))
#تعریف_کن LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(حجم(type), FALSE)
#تعریف_کن LALLOC_CLEAR_MULT(type, شمار)  (type *)lalloc_clear(حجم(type) * (شمار), FALSE)
#تعریف_کن LALLOC_MULT(type, شمار)  (type *)lalloc(حجم(type) * (شمار), FALSE)

#اگر_تعریف_شده HAVE_MEMSET
# define vim_memset(ptr, c, اندازه)   memset((ptr), (c), (اندازه))
#وإلا
فراغ *vim_memset(فراغ *, صحيح, size_t);
#پایان_اگر
#تعریف_کن CLEAR_فیلد(فیلد)  vim_memset(&(فیلد), 0, حجم(فیلد))
#تعریف_کن CLEAR_اشاره_گر(ptr)  vim_memset((ptr), 0, حجم(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#تعریف_کن STRLEN(s)	    طول_رشته((حرف *)(s))
#تعریف_کن STRCPY(d, s)	    strcpy((حرف *)(d), (حرف *)(s))
#تعریف_کن STRNCPY(d, s, n)    strncpy((حرف *)(d), (حرف *)(s), (n))
#تعریف_کن STRCMP(d, s)	    strcmp((حرف *)(d), (حرف *)(s))
#تعریف_کن STRNCMP(d, s, n)    مقایسه_رشته((حرف *)(d), (حرف *)(s), (n))
#اگر_تعریف_شده HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((حرف *)(d), (حرف *)(s))
#وإلا
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((حرف *)(d), (حرف *)(s))
# وإلا
#  define STRICMP(d, s)	    vim_stricmp((حرف *)(d), (حرف *)(s))
# endif
#پایان_اگر
#اگر_تعریف_شده HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((حرف *)(d), (حرف *)(s))
#وإلا
# define STRCOLL(d, s)     strcmp((حرف *)(d), (حرف *)(s))
#پایان_اگر

// Like strcpy() but allows overlapped source and destination.
#تعریف_کن STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#اگر_تعریف_شده HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((حرف *)(d), (حرف *)(s), (n))
#وإلا
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((حرف *)(d), (حرف *)(s), (n))
# وإلا
#  define STRNICMP(d, s, n) vim_strnicmp((حرف *)(d), (حرف *)(s), (n))
# endif
#پایان_اگر

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#تعریف_کن MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)MAXCOL)
#تعریف_کن MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحيح)(n))
#تعریف_کن MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#تعریف_کن STRCAT(d, s)	    strcat((حرف *)(d), (حرف *)(s))
#تعریف_کن STRNCAT(d, s, n)    strncat((حرف *)(d), (حرف *)(s), (n))

#اگر_تعریف_شده HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((حرف *)(s), (حرف *)(cs))
#پایان_اگر

#تعریف_کن بیرون_STR(s)		    بیرون_str((char_u *)(s))
#تعریف_کن بیرون_STR_NF(s)		    بیرون_str_nf((char_u *)(s))

#اگر_تعریف_شده FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.درون_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.درون_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_بیت		    (gui.درون_use || p_tgc)
# وإلا
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_بیت		    gui.درون_use
# endif
#وإلا
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_بیت		    p_tgc
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#وإلا
# define IS_CTERM		    (t_colors > 1)
#پایان_اگر
#اگر_تعریف_شده GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#پایان_اگر

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#اگر_تعریف_شده HAVE_STRERROR
# define PERROR(پیام)		    (فراغ)semsg("%s: %s", (حرف *)(پیام), strerror(errno))
#وإلا
# define PERROR(پیام)		    do_perror(پیام)
#پایان_اگر

تعريفنوع دراز	linenr_T;		// line number type
تعريفنوع صحيح	colnr_T;		// column number type
تعريفنوع بیعلامت کوتاه disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#اگر_تعریف_شده __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#وإلا
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#پایان_اگر

#تعریف_کن SHOWCMD_تعداد_ستون 10			// columns needed by shown command

تعريفنوع فراغ	    *vim_acl_T;		// dummy to pass an ACL to a function

#اگر_تعریف_نشده mch_memmove
# define mch_memmove(مقصد, from, len) memmove((حرف*)(مقصد), (حرف*)(from), (len))
#پایان_اگر

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#تعریف_کن fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#تعریف_کن fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#إذا defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_درون_داد_BUF
#پایان_اگر

#إذا !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, شمار) vim_read((fd), (buf), (شمار))
# define write_eintr(fd, buf, شمار) vim_write((fd), (buf), (شمار))
#پایان_اگر

#اگر_تعریف_شده MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, شمار)   اقرأ((fd), (حرف *)(buf), (بیعلامت صحيح)(شمار))
# define vim_write(fd, buf, شمار)  اكتب((fd), (حرف *)(buf), (بیعلامت صحيح)(شمار))
#وإلا
# define vim_read(fd, buf, شمار)   اقرأ((fd), (حرف *)(buf), (شمار))
# define vim_write(fd, buf, شمار)  اكتب((fd), (حرف *)(buf), (شمار))
#پایان_اگر

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#تعریف_کن HL_ATTR(n)	highlight_attr[(صحيح)(n)]
#تعریف_کن جمله_STR(n)	جمله_رشتهها[(صحيح)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#اگر_تعریف_نشده EXTERN
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
#پایان_اگر

#تعریف_کن MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#تعریف_کن MB_MAXBYTES	30

#إذا (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
تعريفنوع LARGE_INTEGER proftime_T;
#  define PROF_زمان_BLANK "           "
#  define PROF_TOTALS_سرآیند "count  total (s)   self (s)"
# وإلا
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  إذا defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
تعريفنوع بنية timespec proftime_T;
#   define PROF_GET_زمان(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_زمان_FORMAT "%3ld.%09ld"
#   define PROF_زمان_BLANK "              "
#   define PROF_TOTALS_سرآیند "count     total (s)      self (s)"
#  وإلا
تعريفنوع بنية timeval proftime_T;
#   define PROF_GET_زمان(tm) gettimeofday(tm, عدم)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_زمان_FORMAT "%3ld.%06ld"
#   define PROF_زمان_BLANK "           "
#   define PROF_TOTALS_سرآیند "count  total (s)   self (s)"
#  endif
# endif
#وإلا
تعريفنوع صحيح proftime_T;	    // dummy for function prototypes
#پایان_اگر

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
#اگر_تعریف_شده MSWIN
تعريفنوع __time64_t  زمان_T;
#وإلا
تعريفنوع time_t	    زمان_T;
#پایان_اگر

#اگر_تعریف_شده _WIN64
تعريفنوع __int64 sock_T;
#وإلا
تعريفنوع صحيح sock_T;
#پایان_اگر

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#پایان_اگر

#إذا defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#پایان_اگر

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#ضمن "option.h"	// options and default values

#ضمن "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#ضمن "structs.h"	// defines many structures
#ضمن "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#ضمن "alloc.h"

// Values for "do_profiling".
#تعریف_کن PROF_NONE	0	// profiling not started
#تعریف_کن PROF_YES	1	// profiling busy
#تعریف_کن PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#تعریف_کن MOUSE_چپ	0x00
#تعریف_کن MOUSE_میانه	0x01
#تعریف_کن MOUSE_راست	0x02
#تعریف_کن MOUSE_RELEASE	0x03

// bit masks for modifiers:
#تعریف_کن MOUSE_لغزش	0x04
#تعریف_کن MOUSE_ALT	0x08
#تعریف_کن MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#تعریف_کن MOUSE_4	0x100	// scroll wheel down
#تعریف_کن MOUSE_5	0x200	// scroll wheel up

#تعریف_کن MOUSE_X1	0x300 // Mouse-button X1 (6th)
#تعریف_کن MOUSE_X2	0x400 // Mouse-button X2

#تعریف_کن MOUSE_6	0x500	// scroll wheel left
#تعریف_کن MOUSE_7	0x600	// scroll wheel right

#تعریف_کن MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#تعریف_کن MOUSE_DRAG_XTERM   0x40

#تعریف_کن MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#تعریف_کن MOUSEWHEEL_کم		0x60

// Lowest button code for extra mouse buttons 8-11
#تعریف_کن MOUSESIDEBUTTONS_کم		0xa0

#تعریف_کن MOUSE_CLICK_ماسک	0x03

#تعریف_کن رقم_MOUSE_CLICKS(کد) \
    (((بیعلامت)((کد) & 0xC0) >> 6) + 1)

#تعریف_کن مجموعه_رقم_MOUSE_CLICKS(کد, رقم) \
    ((کد) = ((کد) & 0x3f) | ((((رقم) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#تعریف_کن MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#تعریف_کن درون_UNKNOWN		0
#تعریف_کن درون_میانگیر		1
#تعریف_کن درون_وضعیت_سطر		2	// on status or command line
#تعریف_کن درون_SEP_سطر		4	// on vertical separator line
#تعریف_کن درون_OTHER_WIN		8	// in other window but can't go there
#تعریف_کن CURSOR_MOVED		0x100
#تعریف_کن MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#تعریف_کن MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#تعریف_کن MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#تعریف_کن MOUSE_FOCUS		0x01	// need to stay in this window
#تعریف_کن MOUSE_MAY_VIS		0x02	// may start Visual mode
#تعریف_کن MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#تعریف_کن MOUSE_SETPOS		0x08	// only set current mouse position
#تعریف_کن MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#تعریف_کن MOUSE_RELEASED		0x20	// button was released

#إذا defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_زمان_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#پایان_اگر


// defines for eval_vars()
#تعریف_کن VALID_مسیر		1
#تعریف_کن VALID_سر		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#تعریف_کن VV_شمار	0
#تعریف_کن VV_COUNT1	1
#تعریف_کن VV_PREVCOUNT	2
#تعریف_کن VV_ERRMSG	3
#تعریف_کن VV_WARNINGMSG	4
#تعریف_کن VV_STATUSMSG	5
#تعریف_کن VV_SHELL_ERROR	6
#تعریف_کن VV_THIS_SESSION	7
#تعریف_کن VV_نسخه	8
#تعریف_کن VV_LNUM		9
#تعریف_کن VV_TERMRESPONSE	10
#تعریف_کن VV_FNAME	11
#تعریف_کن VV_LANG		12
#تعریف_کن VV_LC_زمان	13
#تعریف_کن VV_CTYPE	14
#تعریف_کن VV_CC_FROM	15
#تعریف_کن VV_CC_مقصد	16
#تعریف_کن VV_FNAME_درون	17
#تعریف_کن VV_FNAME_بیرون	18
#تعریف_کن VV_FNAME_NEW	19
#تعریف_کن VV_FNAME_DIFF	20
#تعریف_کن VV_CMDARG	21
#تعریف_کن VV_FOLDSTART	22
#تعریف_کن VV_FOLDEND	23
#تعریف_کن VV_FOLDDASHES	24
#تعریف_کن VV_FOLDLEVEL	25
#تعریف_کن VV_PROGNAME	26
#تعریف_کن VV_SEND_SERVER	27
#تعریف_کن VV_DYING	28
#تعریف_کن VV_EXCEPTION	29
#تعریف_کن VV_THROWPOINT	30
#تعریف_کن VV_REG		31
#تعریف_کن VV_CMDBANG	32
#تعریف_کن VV_INSERTMODE	33
#تعریف_کن VV_VAL		34
#تعریف_کن VV_کلید		35
#تعریف_کن VV_PROFILING	36
#تعریف_کن VV_FCS_REASON	37
#تعریف_کن VV_FCS_CHOICE	38
#تعریف_کن VV_BEVAL_BUFNR	39
#تعریف_کن VV_BEVAL_WINNR	40
#تعریف_کن VV_BEVAL_WINID	41
#تعریف_کن VV_BEVAL_LNUM	42
#تعریف_کن VV_BEVAL_ستون	43
#تعریف_کن VV_BEVAL_متن	44
#تعریف_کن VV_SCROLLSTART	45
#تعریف_کن VV_SWAPNAME	46
#تعریف_کن VV_SWAPCHOICE	47
#تعریف_کن VV_SWAPCOMMAND	48
#تعریف_کن VV_CHAR		49
#تعریف_کن VV_MOUSE_WIN	50
#تعریف_کن VV_MOUSE_WINID	51
#تعریف_کن VV_MOUSE_LNUM   52
#تعریف_کن VV_MOUSE_ستون	53
#تعریف_کن VV_OP		54
#تعریف_کن VV_SEARCHFORWARD 55
#تعریف_کن VV_HLSEARCH	56
#تعریف_کن VV_OLDFILES	57
#تعریف_کن VV_WINDOWID	58
#تعریف_کن VV_PROGPATH	59
#تعریف_کن VV_COMPLETED_قلم 60
#تعریف_کن VV_OPTION_NEW   61
#تعریف_کن VV_OPTION_OLD   62
#تعریف_کن VV_OPTION_OLDLOCAL 63
#تعریف_کن VV_OPTION_OLDGLOBAL 64
#تعریف_کن VV_OPTION_COMMAND 65
#تعریف_کن VV_OPTION_TYPE  66
#تعریف_کن VV_ERRORS	67
#تعریف_کن VV_FALSE	68
#تعریف_کن VV_TRUE		69
#تعریف_کن VV_NONE		70
#تعریف_کن VV_NULL		71
#تعریف_کن VV_NUMBERMAX	72
#تعریف_کن VV_NUMBERMIN	73
#تعریف_کن VV_NUMBERSIZE	74
#تعریف_کن VV_VIM_DID_ENTER 75
#تعریف_کن VV_TESTING	76
#تعریف_کن VV_TYPE_عدد	77
#تعریف_کن VV_TYPE_STRING	78
#تعریف_کن VV_TYPE_FUNC	79
#تعریف_کن VV_TYPE_LIST	80
#تعریف_کن VV_TYPE_DICT	81
#تعریف_کن VV_TYPE_FLOAT	82
#تعریف_کن VV_TYPE_BOOL	83
#تعریف_کن VV_TYPE_NONE	84
#تعریف_کن VV_TYPE_کار	85
#تعریف_کن VV_TYPE_CHANNEL	86
#تعریف_کن VV_TYPE_BLOB	87
#تعریف_کن VV_TYPE_CLASS	88
#تعریف_کن VV_TYPE_OBJECT	89
#تعریف_کن VV_TERMRFGRESP	90
#تعریف_کن VV_TERMRBGRESP	91
#تعریف_کن VV_TERMU7RESP	92
#تعریف_کن VV_TERMSTYLERESP 93
#تعریف_کن VV_TERMBLINKRESP 94
#تعریف_کن VV_EVENT	95
#تعریف_کن VV_VERSIONLONG	96
#تعریف_کن VV_ECHOSPACE	97
#تعریف_کن VV_ARGV		98
#تعریف_کن VV_COLLATE      99
#تعریف_کن VV_EXITING	100
#تعریف_کن VV_COLORNAMES   101
#تعریف_کن VV_SIZEOFINT	102
#تعریف_کن VV_SIZEOFLONG	103
#تعریف_کن VV_SIZEOFPOINTER 104
#تعریف_کن VV_MAXCOL	105
#تعریف_کن VV_PYTHON3_نسخه 106
#تعریف_کن VV_TYPE_TYPEALIAS 107
#تعریف_کن VV_TYPE_ENUM	  108
#تعریف_کن VV_TYPE_ENUMVALUE 109
#تعریف_کن VV_STACKTRACE	110
#تعریف_کن VV_TYPE_TUPLE	111
#تعریف_کن VV_WAYLAND_DISPLAY 112
#تعریف_کن VV_CLIPMETHOD 113
#تعریف_کن VV_TERMDA1 114
#تعریف_کن VV_TERMOSC 115
#تعریف_کن VV_VIM_DID_INIT		116
#تعریف_کن VV_CLIPPROVIDERS 117
#تعریف_کن VV_IMAGEBACKEND 118
#تعریف_کن VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#تعریف_کن VVAL_FALSE	0L	// VAR_BOOL
#تعریف_کن VVAL_TRUE	1L	// VAR_BOOL
#تعریف_کن VVAL_NONE	2L	// VAR_SPECIAL
#تعریف_کن VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#تعریف_کن VAR_TYPE_عدد	    0
#تعریف_کن VAR_TYPE_STRING	    1
#تعریف_کن VAR_TYPE_FUNC	    2
#تعریف_کن VAR_TYPE_LIST	    3
#تعریف_کن VAR_TYPE_DICT	    4
#تعریف_کن VAR_TYPE_FLOAT	    5
#تعریف_کن VAR_TYPE_BOOL	    6
#تعریف_کن VAR_TYPE_NONE	    7
#تعریف_کن VAR_TYPE_کار	    8
#تعریف_کن VAR_TYPE_CHANNEL    9
#تعریف_کن VAR_TYPE_BLOB	    10
#تعریف_کن VAR_TYPE_INSTR	    11
#تعریف_کن VAR_TYPE_CLASS	    12
#تعریف_کن VAR_TYPE_OBJECT	    13
#تعریف_کن VAR_TYPE_TYPEALIAS  14
#تعریف_کن VAR_TYPE_ENUM	    15
#تعریف_کن VAR_TYPE_ENUMVALUE  16
#تعریف_کن VAR_TYPE_TUPLE	    17

#تعریف_کن DICT_MAXNEST 100	// maximum nesting of lists and dicts

#تعریف_کن TABSTOP_MAX 9999

#اگر_تعریف_شده HAVE_CLIPMETHOD
تعريفنوع تعداد {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#پایان_اگر

#اگر_تعریف_شده FEAT_CLIPBOARD

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
# define SELECT_درون_پیشرفت	1
# define SELECT_انجامشده		2

# define SELECT_نما_CHAR	0
# define SELECT_نما_کلمه	1
# define SELECT_نما_سطر	2

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
    pos_T	شروع;		// Start of selected area
    pos_T	انتها;		// End of selected area
    صحيح		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_ردیف;
    short_u	origin_شروع_ستون;
    short_u	origin_انتها_ستون;
    short_u	کلمه_شروع_ستون;
    short_u	کلمه_انتها_ستون;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_ستون;
    short_u	max_ستون;
    short_u	min_ردیف;
    short_u	max_ردیف;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	نما;		// Select by char, word, or line.

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
#پایان_اگر

// Use 64-bit stat structure on MS-Windows.
#اگر_تعریف_شده MSWIN
تعريفنوع بنية _stat64 stat_T;
#وإلا
تعريفنوع بنية stat stat_T;
#پایان_اگر

تعريفنوع بنية soundcb_S soundcb_T;

#إذا (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(چاپ_کن, fmt_idx, arg_idx)))
#وإلا
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#پایان_اگر

#إذا defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#وإلا
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#پایان_اگر

تعريفنوع تعداد {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_تطابق,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
تعريفنوع تعداد {
    PASTE_درج,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_نما_T;

// Argument for flush_buffers().
تعريفنوع تعداد {
    FLUSH_کمینه,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_درون_داد		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
تعريفنوع تعداد {
    USEPOPUP_NONE,
    USEPOPUP_نرمال,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
تعريفنوع تعداد {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_پشته,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#تعریف_کن ENABLE_STL_نما_MULTI_NL

// Argument for build_stl_str_hl_local().
تعريفنوع تعداد {
    STL_نما_SINGLE,	    // Does not accept line breaks "%@"
    STL_نما_MULTI,	    // Accept line breaks "%@"
    STL_نما_GET_RENDERED_ارتفاع,   // Just get stl rendered height
#اگر_تعریف_شده ENABLE_STL_نما_MULTI_NL
    STL_نما_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#پایان_اگر
} stl_نما_T;

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
    CPT_نوع,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_شمار,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
تعريفنوع حرف *(*opt_did_مجموعه_cb_T)(optset_T *آرگومانها);

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
تعريفنوع صحيح (*opt_expand_cb_T)(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches);

// Flags for assignment functions.
#تعریف_کن تخصیص_بده_VAR	0     // ":var" (nothing special)
#تعریف_کن تخصیص_بده_FINAL	0x01  // ":final"
#تعریف_کن تخصیص_بده_CONST	0x02  // ":const"
#تعریف_کن تخصیص_بده_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#تعریف_کن تخصیص_بده_DECL	0x08  // may declare variable if it does not exist
#تعریف_کن تخصیص_بده_UNPACK	0x10  // using [a, b] = list
#تعریف_کن تخصیص_بده_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#تعریف_کن تخصیص_بده_FOR_LOOP 0x40 // assigning to loop variable
#تعریف_کن تخصیص_بده_INIT	0x80 // not assigning a value, just a declaration
#تعریف_کن تخصیص_بده_UPDATE_بلوک_ID 0x100  // update sav_block_id
#تعریف_کن تخصیص_بده_COMPOUND_OP 0x200  // compound operator e.g. "+="

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
# define mch_پیام(str)		چاپ_کن("%s", (str))
#وإلا
# define USE_MCH_ERRMSG
#پایان_اگر

#إذا defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#پایان_اگر

#إذا defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_درون_داد_روش
#پایان_اگر

#اگر_تعریف_نشده FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define عدد_عرض(x) 7
#پایان_اگر

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#إذا !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#پایان_اگر

#ضمن "globals.h"	    // global variables and messages
#ضمن "errors.h"	    // error messages

#اگر_تعریف_نشده PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ساكن مضمن صحيح
درون_vim9script(فراغ)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    أرجع (current_sctx.sc_نسخه == SCRIPT_نسخه_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#پایان_اگر

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#إذا defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#پایان_اگر

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#اگر_تعریف_شده FEAT_BROWSE
# ifdef BACKSLASH_درون_FILENAME
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
#پایان_اگر

#اگر_تعریف_شده _MSC_VER
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
#پایان_اگر

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#إذا defined(MEM_PROFILE)
# define vim_realloc(ptr, اندازه)  mem_realloc((ptr), (اندازه))
#وإلا
# define vim_realloc(ptr, اندازه)  realloc((ptr), (اندازه))
#پایان_اگر

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#تعریف_کن MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#تعریف_کن MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#تعریف_کن ENC_8_بیت	0x01
#تعریف_کن ENC_DBCS	0x02
#تعریف_کن ENC_UNICODE	0x04

#تعریف_کن ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#تعریف_کن ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#تعریف_کن ENC_2BYTE	0x40	    // Unicode: UCS-2
#تعریف_کن ENC_4BYTE	0x80	    // Unicode: UCS-4
#تعریف_کن ENC_2_کلمه	0x100	    // Unicode: UTF-16

#تعریف_کن ENC_LATIN1	0x200	    // Latin1
#تعریف_کن ENC_LATIN9	0x400	    // Latin9
#تعریف_کن ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#اگر_تعریف_شده USE_ICONV
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
#پایان_اگر

#تعریف_کن SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#اگر_تعریف_شده FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#پایان_اگر

#إذا defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.درون_use ? (مزدوج)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.درون_use ? (مزدوج)(p) / gui.scale : (p))
#وإلا
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#پایان_اگر

#إذا defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.درون_use) ? gui_mch_get_display() : xterm_dpy)
# وإلا
#  define X_DISPLAY	(gui.درون_use ? gui.dpy : xterm_dpy)
# endif
#وإلاإذا defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.درون_use) ? gui_mch_get_display() : (Display *)عدم)
# وإلا
#  define X_DISPLAY	gui.dpy
# endif
#وإلاإذا defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#وإلا
# define X_DISPLAY	(Display *)عدم
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_GTK
# إذا !GTK_CHECK_نسخه(2,14,0)
#  define gtk_widget_get_پنجره(wid)	((wid)->پنجره_2)
#  define gtk_plug_get_socket_پنجره(wid)	((wid)->socket_پنجره)
#  define gtk_selection_داده_get_داده(sel)	((sel)->داده)
#  define gtk_selection_داده_get_داده_type(sel)	((sel)->type)
#  define gtk_selection_داده_get_format(sel)	((sel)->format)
#  define gtk_selection_داده_get_درازی(sel)	((sel)->درازی)
#  define gtk_adjustment_مجموعه_پایینی(adj, کم) \
    افعل { (adj)->پایینی = کم; } طالما (0)
#  define gtk_adjustment_مجموعه_فوقانی(adj, به_بالا) \
    افعل { (adj)->فوقانی_2 = به_بالا; } طالما (0)
#  define gtk_adjustment_مجموعه_صفحه_اندازه(adj, اندازه) \
    افعل { (adj)->صفحه_اندازه = اندازه; } طالما (0)
#  define gtk_adjustment_مجموعه_صفحه_increment(adj, inc) \
    افعل { (adj)->صفحه_increment = inc; } طالما (0)
#  define gtk_adjustment_مجموعه_گام_increment(adj, inc) \
    افعل { (adj)->گام_increment = inc; } طالما (0)
# endif
# إذا !GTK_CHECK_نسخه(2,16,0)
#  define gtk_selection_داده_get_selection(sel)	((sel)->selection)
# endif
# إذا !GTK_CHECK_نسخه(2,18,0)
#  define gtk_widget_get_تخصیص(wid, alloc) \
    افعل { *(alloc) = (wid)->تخصیص; } طالما (0)
#  define gtk_widget_مجموعه_تخصیص(wid, alloc) \
    افعل { (wid)->تخصیص = *(alloc); } طالما (0)
#  define gtk_widget_get_has_پنجره(wid)	!GTK_WIDGET_NO_پنجره(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_مجموعه_پنجره(wid, win) \
    افعل { (wid)->پنجره_2 = (win); } طالما (0)
#  define gtk_widget_مجموعه_can_default(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_مجموعه_FLAGS(wid, GTK_CAN_DEFAULT); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } طالما (0)
#  define gtk_widget_مجموعه_can_focus(wid, can) \
    افعل { إذا (can) \
	    { GTK_WIDGET_مجموعه_FLAGS(wid, GTK_CAN_FOCUS); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } طالما (0)
#  define gtk_widget_مجموعه_visible(wid, vis) \
    افعل { إذا (vis) \
	    { gtk_widget_show(wid); } \
	وإلا \
	    { gtk_widget_hide(wid); } } طالما (0)
# endif
# إذا !GTK_CHECK_نسخه(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_مجموعه_mapped(wid, map) \
    افعل { إذا (map) \
	    { GTK_WIDGET_مجموعه_FLAGS(wid, GTK_MAPPED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } طالما (0)
#  define gtk_widget_مجموعه_realized(wid, rea) \
    افعل { إذا (rea) \
	    { GTK_WIDGET_مجموعه_FLAGS(wid, GTK_REALIZED); } \
	وإلا \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } طالما (0)
# endif
#پایان_اگر

#إذا defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# إذا GTK_CHECK_نسخه(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#پایان_اگر

#اگر_تعریف_نشده FEAT_NETBEANS_INTG
# undef NBDEBUG
#پایان_اگر
#اگر_تعریف_شده NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#وإلا
# define nbdebug(a)
#پایان_اگر

#اگر_تعریف_شده درون_PERL_FILE
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

#پایان_اگر

// values for vim_handle_signal() that are not a signal
#تعریف_کن SIGNAL_بلوک	(-1)
#تعریف_کن SIGNAL_UNBLOCK  (-2)
#إذا !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#پایان_اگر

// flags for skip_vimgrep_pat()
#تعریف_کن VGR_GLOBAL	1
#تعریف_کن VGR_NOJUMP	2
#تعریف_کن VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#تعریف_کن BAD_REPLACE	'?'	// replace it with '?' (default)
#تعریف_کن BAD_KEEP	(-1)	// leave it
#تعریف_کن BAD_DROP	(-2)	// erase it

// last argument for do_source()
#تعریف_کن DOSO_NONE	0
#تعریف_کن DOSO_VIMRC	1	// loading vimrc file
#تعریف_کن DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#تعریف_کن VIF_WANT_INFO	    1	// load non-mark info
#تعریف_کن VIF_WANT_MARKS	    2	// load file marks
#تعریف_کن VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#تعریف_کن VIF_FORCEIT	    8	// overwrite info already read
#تعریف_کن VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#تعریف_کن BFA_DEL		 1	// buffer is going to be deleted
#تعریف_کن BFA_WIPE	 2	// buffer is going to be wiped out
#تعریف_کن BFA_KEEP_UNDO	 4	// do not free undo information
#تعریف_کن BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#تعریف_کن MSCR_پایین	0	// DOWN must be FALSE
#تعریف_کن MSCR_به_بالا		1
#تعریف_کن MSCR_چپ	(-1)
#تعریف_کن MSCR_راست	(-2)

#تعریف_کن KEYLEN_PART_کلید (-1)	// keylen value for incomplete key-code
#تعریف_کن KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#تعریف_کن KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#تعریف_کن FILEINFO_OK	     0
#تعریف_کن FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#تعریف_کن FILEINFO_READ_FAIL   2	// CreateFile() failed
#تعریف_کن FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#تعریف_کن SOPT_BOOL	0x01	// Boolean option
#تعریف_کن SOPT_رقم	0x02	// Number option
#تعریف_کن SOPT_STRING	0x04	// String option
#تعریف_کن SOPT_GLOBAL	0x08	// Option has global value
#تعریف_کن SOPT_WIN	0x10	// Option has window-local value
#تعریف_کن SOPT_BUF	0x20	// Option has buffer-local value
#تعریف_کن SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#تعریف_کن SREQ_GLOBAL	0	// Request global option
#تعریف_کن SREQ_WIN	1	// Request window-local option
#تعریف_کن SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#تعریف_کن GREG_NO_عبارت	1	// Do not allow expression register
#تعریف_کن GREG_عبارت_SRC	2	// Return expression itself for "=" register
#تعریف_کن GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#تعریف_کن AUTOLOAD_CHAR '#'

#اگر_تعریف_شده FEAT_کار_CHANNEL
# define MAX_OPEN_CHANNELS 10
#وإلا
# define MAX_OPEN_CHANNELS 0
#پایان_اگر

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#اگر_تعریف_شده FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#وإلا
# define MAX_CLIENT_CHANNELS 0
#پایان_اگر

#إذا defined(MSWIN)
# define MAX_NAMED_PIPE_اندازه 65535
#پایان_اگر

// Options for json_encode() and json_decode.
#تعریف_کن JSON_JS		1   // use JS instead of JSON
#تعریف_کن JSON_NO_NONE	2   // v:none item not allowed
#تعریف_کن JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#تعریف_کن DIP_ALL	    0x01	// all matches, not just the first one
#تعریف_کن DIP_DIR	    0x02	// find directories instead of files.
#تعریف_کن DIP_ERR	    0x04	// give an error message when none found.
#تعریف_کن DIP_شروع   0x08	// also use "start" directory in 'packpath'
#تعریف_کن DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#تعریف_کن DIP_NORTP   0x20	// do not use 'runtimepath'
#تعریف_کن DIP_NOAFTER 0x40	// skip "after" directories
#تعریف_کن DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#تعریف_کن پایینترین_WIN_ID 1000

// Used by the garbage collector.
#تعریف_کن COPYID_INC 2
#تعریف_کن COPYID_ماسک (~0x1)

// Values for trans_function_name() argument:
#تعریف_کن TFN_INT		0x01	// internal function name OK
#تعریف_کن TFN_QUIET	0x02	// no error messages
#تعریف_کن TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#تعریف_کن TFN_NO_DEREF	0x08	// do not dereference a Funcref
#تعریف_کن TFN_READ_ONLY	0x10	// will not change the var
#تعریف_کن TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#تعریف_کن TFN_COMPILING	0x40	// only used for GLV_COMPILING
#تعریف_کن TFN_NEW_FUNC	0x80	// defining a new function
#تعریف_کن TFN_تخصیص_بده_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#تعریف_کن TFN_درون_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#تعریف_کن GLV_QUIET	TFN_QUIET	// no error messages
#تعریف_کن GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#تعریف_کن GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#تعریف_کن GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#تعریف_کن GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#تعریف_کن GLV_تخصیص_بده_WITH_OP TFN_تخصیص_بده_WITH_OP // assignment with operator
#تعریف_کن GLV_PREFER_FUNC	0x10000		// prefer function above variable
#تعریف_کن GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#تعریف_کن DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#تعریف_کن FLEN_FIXED 40

// flags for find_name_end()
#تعریف_کن FNE_INCL_BR	1	// include [] in name
#تعریف_کن FNE_CHECK_شروع	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#إذا (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#پایان_اگر

// stat macros
#اگر_تعریف_نشده S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# وإلا
#  define S_ISDIR(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# وإلا
#  define S_ISREG(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# وإلا
#  define S_ISBLK(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# وإلا
#  define S_ISSOCK(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# وإلا
#  define S_ISFIFO(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# وإلا
#  define S_ISCHR(m)	0
# endif
#پایان_اگر
#اگر_تعریف_نشده S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# وإلا
#  define S_ISLNK(m)	0
# endif
#پایان_اگر

#إذا defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_زمان_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), عدم)
# define ELAPSED_FUNC(v) elapsed(&(v))
تعريفنوع بنية timeval elapsed_T;
دراز elapsed(بنية timeval *شروع_tv);
#وإلاإذا defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_شمار()
# define ELAPSED_FUNC(v) elapsed(v)
تعريفنوع DWORD elapsed_T;
# ifndef PROTO
دراز elapsed(DWORD شروع_tick);
# endif
#پایان_اگر

// Replacement for nchar used by nv_replace().
#تعریف_کن REPLACE_CR_NCHAR    (-1)
#تعریف_کن REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#تعریف_کن جمله_شروع_NOJOB	1
#تعریف_کن جمله_شروع_FORCEIT	2
#تعریف_کن جمله_شروع_SYSTEM	4

// Used for icon/title save and restore.
#تعریف_کن SAVE_RESTORE_TITLE	1
#تعریف_کن SAVE_RESTORE_ICON	2
#تعریف_کن SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#تعریف_کن APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#تعریف_کن APC_SUBSTITUTE		2   // text is replaced, not inserted
#تعریف_کن APC_INDENT		4   // changing indent

#تعریف_کن CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#تعریف_کن REPTERM_FROM_PART	1
#تعریف_کن REPTERM_DO_LT		2
#تعریف_کن REPTERM_SPECIAL		4
#تعریف_کن REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#تعریف_کن FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#تعریف_کن FSK_KEEP_X_کلید	0x02	// don't translate xHome to Home key
#تعریف_کن FSK_درون_STRING	0x04	// TRUE in string, double quote is escaped
#تعریف_کن FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#تعریف_کن FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#تعریف_کن READDIR_مرتب_سازی_NONE	0  // do not sort
#تعریف_کن READDIR_مرتب_سازی_BYTE	1  // sort by byte order (strcmp), default
#تعریف_کن READDIR_مرتب_سازی_IC		2  // sort ignoring case (strcasecmp)
#تعریف_کن READDIR_مرتب_سازی_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#تعریف_کن MCH_DELAY_IGNOREINPUT	1
#تعریف_کن MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#تعریف_کن EVAL_VAR_VERBOSE	1   // may give error message
#تعریف_کن EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#تعریف_کن EVAL_VAR_IMPORT		4   // may return special variable for import
#تعریف_کن EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#تعریف_کن FUZZY_تطابق_MAX_LEN	1024    // max characters that can be matched
#تعریف_کن FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#تعریف_کن ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#تعریف_کن UC_میانگیر	1	// -buffer: local to current buffer
#تعریف_کن UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#تعریف_کن UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#تعریف_کن VSE_NONE	0
#تعریف_کن VSE_SHELL	1	// escape for a shell command
#تعریف_کن VSE_میانگیر	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#تعریف_کن FFED_IS_GLOBAL	1	// "g:" was used
#تعریف_کن FFED_NO_GLOBAL	2	// only check for script-local functions

#تعریف_کن MAX_LSHIFT_بیتها (varnumber_T)((حجم(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#تعریف_کن CF_CLASS	1	// inside a class
#تعریف_کن CF_INTERFACE	2	// inside an interface
#تعریف_کن CF_ABSTRACT_روش	4	// inside an abstract class

// Flags used by getvcol()
#تعریف_کن GETVCOL_انتها_EXCL_LBR	1
#تعریف_کن GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#تعریف_کن GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#اگر_تعریف_شده VMS
# define مسیر_ESC_WILDCARDS	"*?%"
#وإلا
# ifdef MSWIN
#  define مسیر_ESC_WILDCARDS	"*?["
# وإلا
#  define مسیر_ESC_WILDCARDS	"*?[{"
# endif
#پایان_اگر

#پایان_اگر // VIM__H
