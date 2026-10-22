/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_غیر_تعریف_شدہ VIM__H
#تعریف_کرو VIM__H

#شامل "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#اگر defined(WIN32) || defined(_WIN32)
# define MSWIN
#اگر_ختم

#اگر defined(MSWIN)
# include <io.h>
#اگر_ختم

// ============ the header file puzzle: order matters =========

#اگر_تعریف_شدہ HAVE_CONFIG_H	// GNU autoconf (or something else) was here
# ifdef VMS
#  include "config.h"   /* Rely on /INCLUDE to find it. */
# ورنہ
#  include "auto/config.h"
# endif /* def VMS [else] */
# define HAVE_PATHDEF

/*
 * Check if configure correctly managed to find sizeof(int).  If this failed,
 * it becomes zero.  This is likely a problem of not being able to run the
 * test program.  Other items from configure may also be wrong then!
 */
# اگر (VIM_SIZEOF_INT == 0)
#  error configure did not تسلسل properly.  Check خودکار/config.لوگارتھم.
# endif

# اگر (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_ماخذ
#   define XOPEN_ماخذ    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_ماخذ
#    define BSD_ماخذ 1
#   endif
#   ifndef SVID_ماخذ
#    define SVID_ماخذ 1
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
# اگر defined(__CYGWIN32__) && defined(HAVE_FCHDIR)
#  undef HAVE_FCHDIR
# endif

// We may need to define the uint32_t on non-Unix system, but using the same
// identifier causes conflicts.  Therefore use UINT32_T.
# define UINT32_TYPEDEF uint32_t
#اگر_ختم

// for INT_MAX, LONG_MAX et al.
#شامل <limits.h>

#اگر !defined(UINT32_TYPEDEF)
# اگر defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# ورنہ
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF غیرعلامتی صحیح
# endif
#اگر_ختم

// C89 does not define SIZE_MAX
#اگر defined(__hpux) || defined(VMS)
# ifndef سائز_MAX
#  define سائز_MAX ((size_t)(-1))
# endif
#اگر_ختم

// user ID of root is usually zero, but not for everybody
#اگر_تعریف_شدہ __TANDEM
# ifndef TANDEM_ماخذ
#  define TANDEM_ماخذ
# endif
# include <floss.h>
# define جڑ_UID 65535
# define OLDXAW
# اگر (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#ورنہ
# define جڑ_UID 0
#اگر_ختم

/* Include MAC_OS_X_VERSION_* macros */
#اگر_تعریف_شدہ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#اگر_ختم

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#اگر defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#اگر_ختم
// Unless made through the Makefile enforce GUI on Mac
#اگر defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#اگر_ختم

#اگر defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# اگر !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#اگر_ختم

// Check support for rendering options
#اگر_تعریف_شدہ FEAT_GUI
# اگر defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#اگر_ختم

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#اگر_تعریف_شدہ MSWIN
# define VIM_SIZEOF_INT 4
#اگر_ختم

#اگر_تعریف_شدہ AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  ورنہ
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#اگر_ختم
#اگر defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#اگر_ختم

#اگر VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only چلتا with 32 ثنائی_ہندسہ صحیح or larger
#اگر_ختم

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#شامل "feature.h"

#اگر defined(MACOS_X_DARWIN)
# اگر defined(FEAT_معمول) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# اگر defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_نسخہ_MIN_REQUIRED) && MAC_OS_X_نسخہ_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# اگر defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#اگر_ختم

// +x11 is only enabled when it's both available and wanted.
#اگر defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#اگر_ختم

#اگر defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#اگر_ختم

#اگر_تعریف_شدہ NO_X11_INCLUDES
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
#اگر_ختم

// The Mac conversion stuff doesn't work under X11.
#اگر defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#اگر_ختم

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#اگر_غیر_تعریف_شدہ VIMPACKAGE
# define VIMPACKAGE	"vim"
#اگر_ختم

/*
 * Find out if function definitions should include argument types
 */
#اگر_تعریف_شدہ AZTEC_C
# include <functions.h>
#اگر_ختم

#اگر_تعریف_شدہ SASC
# include <clib/exec_protos.h>
#اگر_ختم

#اگر_تعریف_شدہ _DCC
# include <clib/exec_protos.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#اگر_ختم

#اگر (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#وگرنہ_اگر !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) اشارہ(signum, sighandler)
#اگر_ختم

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#اگر_غیر_تعریف_شدہ غیر_مستعمل
# اگر defined(HAVE_ATTRIBUTE_غیر_مستعمل) || defined(__MINGW32__)
#  define غیر_مستعمل __attribute__((غیر_مستعمل_2))
# ورنہ
#  اگر defined __has_attribute
#   اگر __has_attribute(غیر_مستعمل_2)
#    define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#   endif
#  endif
# endif
# ifndef غیر_مستعمل
#  define غیر_مستعمل
# endif
#اگر_ختم

// Used to check for "sun", "__sun" is used by newer compilers.
#اگر defined(__sun)
# define SUN_SYSTEM
#اگر_ختم

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#اگر !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#اگر_ختم

#اگر_غیر_تعریف_شدہ PROTO
# ifdef AMIGA
#  include "os_amiga.h"
# endif

# ifdef MSWIN
#  include "os_win32.h"
# endif

# اگر defined(MACOS_X)
#  include "os_mac.h"
# endif

# ifdef __QNX__
#  include "os_qnx.h"
# endif
#اگر_ختم

#اگر_تعریف_شدہ X_LOCALE
# include <X11/Xlocale.h>
#ورنہ
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#اگر_ختم

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#اگر_غیر_تعریف_شدہ MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# ورنہ
#  define MAXPATHL  256
# endif
#اگر_ختم
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
# define راستہ_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#ورنہ
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define راستہ_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# ورنہ
#  define راستہ_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define بفر_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#اگر_ختم

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#تعریف_کرو NUMBUFLEN 65

// flags for vim_str2nr()
#تعریف_کرو STR2NR_BIN  0x01
#تعریف_کرو STR2NR_OCT  0x02
#تعریف_کرو STR2NR_HEX  0x04
#تعریف_کرو STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#تعریف_کرو STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#تعریف_کرو STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#تعریف_کرو STR2NR_FORCE 0x80   // only when ONE of the above is used

#تعریف_کرو STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
قسمتعریف غیرعلامتی حرف	char_u;
قسمتعریف غیرعلامتی مختصر	short_u;
قسمتعریف غیرعلامتی صحیح	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#اگر_تعریف_شدہ _WIN64
قسمتعریف غیرعلامتی __int64	long_u;
قسمتعریف		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#ورنہ
قسمتعریف غیرعلامتی طویل		long_u;
قسمتعریف		 طویل		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#اگر_ختم
#تعریف_کرو PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#اگر defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#اگر_ختم

/*
 * We use 64-bit file functions here, if available.  E.g. ftello() returns
 * off_t instead of long, which helps if long is 32 bit and off_t is 64 bit.
 * We assume that when fseeko() is available then ftello() is too.
 * Note that Windows has different function names.
 */
#اگر defined(MSWIN) && !defined(PROTO)
قسمتعریف __int64 off_T;
# ifdef __MINGW32__
#  define vim_lseek lseek64
#  define vim_fseek fseeko64
#  define vim_ftell ftello64
# ورنہ
#  define vim_lseek _lseeki64
#  define vim_fseek _fseeki64
#  define vim_ftell _ftelli64
# endif
#ورنہ
قسمتعریف off_t off_T;
# ifdef HAVE_FSEEKO
#  define vim_lseek lseek
#  define vim_ftell ftello
#  define vim_fseek fseeko
# ورنہ
#  define vim_lseek lseek
#  define vim_ftell ftell
#  define vim_fseek(a, b, c)	fseek(a, (طویل)b, c)
# endif
#اگر_ختم

/*
 * The characters and attributes cached for the screen.
 */
قسمتعریف char_u schar_T;
قسمتعریف غیرعلامتی مختصر sattr_T;
#تعریف_کرو MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
قسمتعریف غیرعلامتی صحیح u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#اگر defined(VMS) && defined(VAX)
// unsupported compiler
قسمتعریف طویل      vimlong_T;
#ورنہ
قسمتعریف طویل طویل vimlong_T;
#اگر_ختم

#اگر_غیر_تعریف_شدہ UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#اگر_ختم

#شامل "ascii.h"
#شامل "keymap.h"
#شامل "termdefs.h"
#شامل "macros.h"

#اگر_تعریف_شدہ LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#اگر_ختم
#اگر_تعریف_شدہ _DCC
# include <sys/stat.h>
#اگر_ختم
#اگر defined(MSWIN)
# include <sys/stat.h>
#اگر_ختم

#اگر defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#اگر_ختم

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#اگر_غیر_تعریف_شدہ UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# اگر defined(HAVE_لڑیاں_H) && !defined(NO_لڑیاں_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#اگر_ختم // NON-UNIX

#شامل <assert.h>

#اگر_تعریف_شدہ HAVE_STDINT_H
# include <stdint.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_INTTYPES_H
# include <inttypes.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_WCTYPE_H
# include <wctype.h>
#اگر_ختم
#شامل <stdarg.h>
// older compilers do not define va_copy
#اگر_غیر_تعریف_شدہ va_copy
# define va_copy(dst, src)	((dst) = (src))
#اگر_ختم

// for offsetof()
#شامل <stddef.h>
#شامل <stdbool.h>

#اگر defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_وقت_H) || defined(SYS_SELECT_WITH_SYS_وقت))
# include <sys/select.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# ورنہ
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SODIUM
# include <sodium.h>
#اگر_ختم

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#اگر_تعریف_شدہ DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#اگر_ختم

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#اگر_تعریف_شدہ DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
خارجی حرف *(*dyn_libintl_gettext)(ثابت حرف *msgid);
خارجی حرف *(*dyn_libintl_ngettext)(ثابت حرف *msgid, ثابت حرف *msgid_plural, غیرعلامتی طویل n);
خارجی حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *domainname, ثابت حرف *dirname);
خارجی حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *domainname, ثابت حرف *codeset);
خارجی حرف *(*dyn_libintl_textdomain)(ثابت حرف *domainname);
خارجی صحیح (*dyn_libintl_wputenv)(ثابت wchar_t *envstring);
خارجی صحیح *dyn_libintl_nl_پیغام_cat_cntr;
#اگر_ختم


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#اگر_تعریف_شدہ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
#  define _(x) (*dyn_libintl_gettext)((حرف *)(x))
#  define NGETTEXT(x, xs, n) (*dyn_libintl_ngettext)((حرف *)(x), (حرف *)(xs), (n))
#  define N_(x) x
#  define bindtextdomain(domain, dir) (*dyn_libintl_bindtextdomain)((domain), (dir))
#  define bind_textdomain_codeset(domain, codeset) (*dyn_libintl_bind_textdomain_codeset)((domain), (codeset))
#  اگر !defined(HAVE_BIND_TEXTDOMAIN_CODESET)
#   define HAVE_BIND_TEXTDOMAIN_CODESET 1
#  endif
#  define textdomain(domain) (*dyn_libintl_textdomain)(domain)
#  define libintl_wputenv(envstring) (*dyn_libintl_wputenv)(envstring)
# ورنہ
#  include <libintl.h>
#  define _(x) gettext((حرف *)(x))
#  define NGETTEXT(x, xs, n) ngettext((x), (xs), (n))
#  ifdef gettext_noop
#   define N_(x) gettext_noop(x)
#  ورنہ
#   define N_(x) x
#  endif
# endif
#ورنہ
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
# define bind_textdomain_codeset(x, y) //خالی_خانے
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //خالی_خانے
#اگر_ختم

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#تعریف_کرو UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#تعریف_کرو UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#تعریف_کرو UPD_INVERTED		20  // redisplay inverted part that changed
#تعریف_کرو UPD_INVERTED_ALL	25  // redisplay whole inverted part
#تعریف_کرو UPD_REDRAW_چوٹی		30  // display first w_upd_rows screen lines
#تعریف_کرو UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#تعریف_کرو UPD_NOT_VALID		40  // buffer needs complete redraw
#تعریف_کرو UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#تعریف_کرو SLF_RIGHTLEFT	1
#تعریف_کرو SLF_POPUP	2
#تعریف_کرو SLF_INC_VCOL	4

#تعریف_کرو MB_FILLER_CHAR '<'  // character used when a double-width character
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
#تعریف_کرو VALID_WROW	0x01	// w_wrow (window row) is valid
#تعریف_کرو VALID_WCOL	0x02	// w_wcol (window col) is valid
#تعریف_کرو VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#تعریف_کرو VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#تعریف_کرو VALID_CROW	0x10	// w_cline_row is valid
#تعریف_کرو VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#تعریف_کرو VALID_BOTLINE_AP 0x40	// w_botline is approximated
#تعریف_کرو VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#تعریف_کرو POPF_IS_POPUP	0x01	// this is a popup window
#تعریف_کرو POPF_HIDDEN	0x02	// popup is not displayed
#تعریف_کرو POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#تعریف_کرو POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#تعریف_کرو POPF_ON_CMDLINE	0x10	// popup overlaps command line
#تعریف_کرو POPF_DRAG	0x20	// popup can be moved by dragging border
#تعریف_کرو POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#تعریف_کرو POPF_RESIZE	0x80	// popup can be resized by dragging
#تعریف_کرو POPF_MAPPING	0x100	// mapping keys
#تعریف_کرو POPF_INFO	0x200	// used for info of popup menu
#تعریف_کرو POPF_INFO_MENU	0x400	// align info popup with popup menu
#تعریف_کرو POPF_POSINVERT	0x800	// vertical position can be inverted
#تعریف_کرو POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#تعریف_کرو POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#تعریف_کرو POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#تعریف_کرو POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#تعریف_کرو POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#تعریف_کرو POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#تعریف_کرو POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#تعریف_کرو POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#تعریف_کرو HL_معمول		0x00
#تعریف_کرو HL_INVERSE		0x01
#تعریف_کرو HL_BOLD			0x02
#تعریف_کرو HL_ITALIC		0x04
#تعریف_کرو HL_UNDERLINE		0x08
#تعریف_کرو HL_UNDERCURL		0x10
#تعریف_کرو HL_UNDERDOUBLE		0x20
#تعریف_کرو HL_UNDERDOTTED		0x40
#تعریف_کرو HL_UNDERDASHED		0x80
#تعریف_کرو HL_STANDOUT		0x100
#تعریف_کرو HL_NOCOMBINE		0x200
#تعریف_کرو HL_STRIKETHROUGH	0x400
#تعریف_کرو HL_ALL			0x7ff

// special attribute addition: Put message in history
#تعریف_کرو پیغام_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#تعریف_کرو منوال_معمول	0x01	// Normal mode, command expected
#تعریف_کرو منوال_VISUAL	0x02	// Visual mode - use get_real_state()
#تعریف_کرو منوال_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#تعریف_کرو منوال_CMDLINE	0x08	// Editing the command line
#تعریف_کرو منوال_داخل_کرو	0x10	// Insert mode, also for Replace mode
#تعریف_کرو منوال_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#تعریف_کرو منوال_SELECT	0x40	// Select mode, use get_real_state()
#تعریف_کرو منوال_TERMINAL	0x80	// Terminal mode

#تعریف_کرو MAP_ALL_MODES	0xff    // all mode bits used for mapping

#تعریف_کرو REPLACE_FLAG	0x100	// Replace mode flag
#تعریف_کرو منوال_REPLACE	(REPLACE_FLAG | منوال_داخل_کرو)
#تعریف_کرو VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#تعریف_کرو منوال_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | منوال_داخل_کرو)
#تعریف_کرو منوال_LREPLACE	(REPLACE_FLAG | منوال_LANGMAP)

#تعریف_کرو منوال_معمول_BUSY (0x1000 | منوال_معمول)
				// Normal mode, busy with a command
#تعریف_کرو منوال_HITRETURN	(0x2000 | منوال_معمول)
				// waiting for return or command
#تعریف_کرو منوال_ASKMORE	0x3000	// Asking if you want --more--
#تعریف_کرو منوال_SETWSIZE	0x4000	// window size has changed
#تعریف_کرو منوال_EXTERNCMD	0x5000	// executing an external command
#تعریف_کرو منوال_SHOWMATCH	(0x6000 | منوال_داخل_کرو) // show matching paren
#تعریف_کرو منوال_CONFIRM	0x7000	// ":confirm" prompt
#تعریف_کرو منوال_ALL	0xffff

#تعریف_کرو منوال_MAX_لمبائی	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#تعریف_کرو FORWARD			1
#تعریف_کرو BACKWARD		(-1)
#تعریف_کرو FORWARD_FILE		3
#تعریف_کرو BACKWARD_FILE		(-3)

// return values for functions
#اگر !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#اگر_ختم
#تعریف_کرو FAIL			0
#تعریف_کرو NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#تعریف_کرو BF_RECOVERED	0x01	// buffer has been recovered
#تعریف_کرو BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#تعریف_کرو BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#تعریف_کرو BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#تعریف_کرو BF_NEW		0x10	// file didn't exist when editing started
#تعریف_کرو BF_NEW_W	0x20	// Warned for BF_NEW and file created
#تعریف_کرو BF_READERR	0x40	// got errors while reading the file
#تعریف_کرو BF_DUMMY	0x80	// dummy buffer, only used internally
#تعریف_کرو BF_PRESERVED	0x100	// ":preserve" was used
#تعریف_کرو BF_SYN_سیٹ	0x200	// 'syntax' option was set
#تعریف_کرو BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#تعریف_کرو BF_WRITE_نقاب	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#تعریف_کرو EXPAND_UNSUCCESSFUL	(-2)
#تعریف_کرو EXPAND_OK		(-1)
#تعریف_کرو EXPAND_NOTHING		0
#تعریف_کرو EXPAND_COMMANDS		1
#تعریف_کرو EXPAND_FILES		2
#تعریف_کرو EXPAND_DIRECTORIES	3
#تعریف_کرو EXPAND_SETTINGS		4
#تعریف_کرو EXPAND_BOOL_SETTINGS	5
#تعریف_کرو EXPAND_لیبل_فہرست		6
#تعریف_کرو EXPAND_OLD_SETTING	7
#تعریف_کرو EXPAND_HELP		8
#تعریف_کرو EXPAND_BUFFERS		9
#تعریف_کرو EXPAND_EVENTS		10
#تعریف_کرو EXPAND_MENUS		11
#تعریف_کرو EXPAND_SYNTAX		12
#تعریف_کرو EXPAND_HIGHLIGHT	13
#تعریف_کرو EXPAND_AUGROUP		14
#تعریف_کرو EXPAND_USER_VARS	15
#تعریف_کرو EXPAND_MAPPINGS		16
#تعریف_کرو EXPAND_لیبل_فہرست_LISTFILES	17
#تعریف_کرو EXPAND_FUNCTIONS	18
#تعریف_کرو EXPAND_USER_FUNC	19
#تعریف_کرو EXPAND_عبارت	20
#تعریف_کرو EXPAND_MENUNAMES	21
#تعریف_کرو EXPAND_USER_COMMANDS	22
#تعریف_کرو EXPAND_USER_CMD_FLAGS	23
#تعریف_کرو EXPAND_USER_NARGS	24
#تعریف_کرو EXPAND_USER_COMPLETE	25
#تعریف_کرو EXPAND_ENV_VARS		26
#تعریف_کرو EXPAND_LANGUAGE		27
#تعریف_کرو EXPAND_COLORS		28
#تعریف_کرو EXPAND_COMPILER		29
#تعریف_کرو EXPAND_USER_DEFINED	30
#تعریف_کرو EXPAND_USER_LIST	31
#تعریف_کرو EXPAND_SHELLCMD		32
#تعریف_کرو EXPAND_CSCOPE		33
#تعریف_کرو EXPAND_SIGN		34
#تعریف_کرو EXPAND_PROFILE		35
#تعریف_کرو EXPAND_BEHAVE		36
#تعریف_کرو EXPAND_FILETYPE		37
#تعریف_کرو EXPAND_FILES_اندر_راستہ	38
#تعریف_کرو EXPAND_OWNSYNTAX	39
#تعریف_کرو EXPAND_LOCALES		40
#تعریف_کرو EXPAND_HISTORY		41
#تعریف_کرو EXPAND_USER		42
#تعریف_کرو EXPAND_SYNTIME		43
#تعریف_کرو EXPAND_USER_ADDR_TYPE	44
#تعریف_کرو EXPAND_PACKADD		45
#تعریف_کرو EXPAND_MESSAGES		46
#تعریف_کرو EXPAND_MAPCLEAR		47
#تعریف_کرو EXPAND_ARGLIST		48
#تعریف_کرو EXPAND_DIFF_BUFFERS	49
#تعریف_کرو EXPAND_DISASSEMBLE	50
#تعریف_کرو EXPAND_BREAKPOINT	51
#تعریف_کرو EXPAND_SCRIPTNAMES	52
#تعریف_کرو EXPAND_RUNTIME		53
#تعریف_کرو EXPAND_STRING_SETTING	54
#تعریف_کرو EXPAND_SETTING_تفریق	55
#تعریف_کرو EXPAND_ARGOPT		56
#تعریف_کرو EXPAND_TERMINALOPT	57
#تعریف_کرو EXPAND_KEYMAP		58
#تعریف_کرو EXPAND_DIRS_اندر_CDPATH	59
#تعریف_کرو EXPAND_SHELLCMDLINE	60
#تعریف_کرو EXPAND_FINDFUNC		61
#تعریف_کرو EXPAND_HIGHLIGHT_گروہ  62
#تعریف_کرو EXPAND_FILETYPECMD	63
#تعریف_کرو EXPAND_نمونہ_اندر_BUF	64
#تعریف_کرو EXPAND_RETAB		65
#تعریف_کرو EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#تعریف_کرو EXMODE_معمول		1
#تعریف_کرو EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#تعریف_کرو WILD_FREE		1
#تعریف_کرو WILD_EXPAND_FREE	2
#تعریف_کرو WILD_EXPAND_KEEP	3
#تعریف_کرو WILD_اگلا		4
#تعریف_کرو WILD_پچھلا		5
#تعریف_کرو WILD_ALL		6
#تعریف_کرو WILD_LONGEST		7
#تعریف_کرو WILD_ALL_KEEP		8
#تعریف_کرو WILD_CANCEL		9
#تعریف_کرو WILD_APPLY		10
#تعریف_کرو WILD_PAGEUP		11
#تعریف_کرو WILD_PAGEDOWN		12

#تعریف_کرو WILD_LIST_NOTFOUND	    0x01
#تعریف_کرو WILD_HOME_REPLACE	    0x02
#تعریف_کرو WILD_USE_NL		    0x04
#تعریف_کرو WILD_NO_BEEP		    0x08
#تعریف_کرو WILD_جوڑو_SLASH		    0x10
#تعریف_کرو WILD_KEEP_ALL		    0x20
#تعریف_کرو WILD_SILENT		    0x40
#تعریف_کرو WILD_ESCAPE		    0x80
#تعریف_کرو WILD_ICASE		    0x100
#تعریف_کرو WILD_ALLLINKS		    0x200
#تعریف_کرو WILD_IGNORE_COMPLETESLASH   0x400
#تعریف_کرو WILD_NOERROR		    0x800  // sets EW_NOERROR
#تعریف_کرو WILD_BUFLASTUSED	    0x1000
#تعریف_کرو BUF_DIFF_FILTER		    0x2000
#تعریف_کرو WILD_NOSELECT		    0x4000
#تعریف_کرو WILD_MAY_EXPAND_نمونہ	    0x8000
#تعریف_کرو WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#تعریف_کرو WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#تعریف_کرو EW_DIR		0x01	// include directory names
#تعریف_کرو EW_FILE		0x02	// include file names
#تعریف_کرو EW_NOTFOUND	0x04	// include not found names
#تعریف_کرو EW_ADDSLASH	0x08	// append slash to directory name
#تعریف_کرو EW_KEEPALL	0x10	// keep all matches
#تعریف_کرو EW_SILENT	0x20	// don't print "1 returned" from shell
#تعریف_کرو EW_EXEC		0x40	// executable files
#تعریف_کرو EW_راستہ		0x80	// search in 'path' too
#تعریف_کرو EW_ICASE	0x100	// ignore case
#تعریف_کرو EW_NOERROR	0x200	// no error for bad regexp
#تعریف_کرو EW_NOTWILD	0x400	// add match with literal name if exists
#تعریف_کرو EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#تعریف_کرو EW_ALLLINKS	0x1000	// also links not pointing to existing file
#تعریف_کرو EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#تعریف_کرو EW_DODOT	0x4000	// also files starting with a dot
#تعریف_کرو EW_EMPTYOK	0x8000	// no matches is not an error
#تعریف_کرو EW_NOTENV	0x10000	// do not expand environment variables
#تعریف_کرو EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#تعریف_کرو FINDFILE_FILE	0	// only files
#تعریف_کرو FINDFILE_DIR	1	// only directories
#تعریف_کرو FINDFILE_BOTH	2	// files and directories

#اگر defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_چوڑائی())
# define TPL_LCOL()			tabpanel_leftcol()
#ورنہ
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#اگر_ختم

#تعریف_کرو W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_چوڑائی)
#اگر_تعریف_شدہ FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_اونچائی)
#ورنہ
# define W_WINROW(wp)	(wp->w_winrow)
#اگر_ختم

// Values for the find_pattern_in_path() function args 'type' and 'action':
#تعریف_کرو ڈھونڈو_ANY	1
#تعریف_کرو ڈھونڈو_DEFINE	2
#تعریف_کرو CHECK_راستہ	3

#تعریف_کرو ACTION_SHOW	1
#تعریف_کرو ACTION_GOTO	2
#تعریف_کرو ACTION_بانٹو	3
#تعریف_کرو ACTION_SHOW_ALL	4
#تعریف_کرو ACTION_EXPAND	5

#اگر_تعریف_شدہ FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_حالتیں	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_یہاں	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_مطابقت	0x40	// use match ID instead of item ID
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
#اگر_ختم

// Values for 'options' argument in do_search() and searchit()
#تعریف_کرو تلاش_REV    0x01  // go in reverse of previous dir.
#تعریف_کرو تلاش_ECHO   0x02  // echo the search command and handle options
#تعریف_کرو تلاش_پیغام    0x0c  // give messages (yes, it's not 0x04)
#تعریف_کرو تلاش_NFMSG  0x08  // give all messages except not found
#تعریف_کرو تلاش_OPT    0x10  // interpret optional flags
#تعریف_کرو تلاش_HIS    0x20  // put search pattern in history
#تعریف_کرو تلاش_انتہا    0x40  // put cursor at end of match
#تعریف_کرو تلاش_NOOF   0x80  // don't add offset to position
#تعریف_کرو تلاش_آغاز 0x100  // start search without col offset
#تعریف_کرو تلاش_MARK  0x200  // set previous context mark
#تعریف_کرو تلاش_KEEP  0x400  // keep previous search pattern
#تعریف_کرو تلاش_PEEK  0x800  // peek for typed char, cancel search
#تعریف_کرو تلاش_ستون  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#تعریف_کرو ڈھونڈو_IDENT	1	// find identifier (word)
#تعریف_کرو ڈھونڈو_STRING	2	// find any string (WORD)
#تعریف_کرو ڈھونڈو_EVAL	4	// include "->", "[]" and "."
#تعریف_کرو ڈھونڈو_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#تعریف_کرو FNAME_MESS	1	// give error message
#تعریف_کرو FNAME_EXP	2	// expand to path
#تعریف_کرو FNAME_HYP	4	// check for hypertext link
#تعریف_کرو FNAME_INCL	8	// apply 'includeexpr'
#تعریف_کرو FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#تعریف_کرو FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#تعریف_کرو GETF_SETMARK	0x01	// set pcmark before jumping
#تعریف_کرو GETF_ALT	0x02	// jumping to alternate file (not buf num)
#تعریف_کرو GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#تعریف_کرو GETFILE_ERROR	    1	// normal error
#تعریف_کرو GETFILE_NOT_WRITTEN 2	// "not written" error
#تعریف_کرو GETFILE_SAME_FILE   0	// success, same file
#تعریف_کرو GETFILE_OPEN_OTHER (-1)	// success, opened another file
#تعریف_کرو GETFILE_غیر_مستعمل	    8
#تعریف_کرو GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#تعریف_کرو BLN_CURBUF	1	// may re-use curbuf for new buffer
#تعریف_کرو BLN_LISTED	2	// put new buffer in buffer list
#تعریف_کرو BLN_DUMMY	4	// allocating dummy buffer
#تعریف_کرو BLN_NEW		8	// create a new buffer
#تعریف_کرو BLN_NOOPT	16	// don't copy options to existing buffer
#تعریف_کرو BLN_DUMMY_OK	32	// also find an existing dummy buffer
#تعریف_کرو BLN_REUSE	64	// may re-use number from buf_reuse
#تعریف_کرو BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#تعریف_کرو کلید_OPEN_FORW	0x101
#تعریف_کرو کلید_OPEN_عقب	0x102
#تعریف_کرو کلید_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#تعریف_کرو MAPTYPE_MAP		0
#تعریف_کرو MAPTYPE_UNMAP		1
#تعریف_کرو MAPTYPE_NOREMAP		2
#تعریف_کرو MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#تعریف_کرو REMAP_YES	0	// allow remapping
#تعریف_کرو REMAP_NONE	(-1)	// no remapping
#تعریف_کرو REMAP_SCRIPT	(-2)	// remap script-local mappings only
#تعریف_کرو REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#تعریف_کرو SHELL_FILTER	1	// filtering text
#تعریف_کرو SHELL_EXPAND	2	// expanding wildcards
#تعریف_کرو SHELL_COOKED	4	// set term to cooked mode
#تعریف_کرو SHELL_DOOUT	8	// redirecting output
#تعریف_کرو SHELL_SILENT	16	// don't print error returned by command
#تعریف_کرو SHELL_READ	32	// read lines and insert into buffer
#تعریف_کرو SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#تعریف_کرو گرہ_معمول	0	// file or directory, check with mch_isdir()
#تعریف_کرو گرہ_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#تعریف_کرو گرہ_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#تعریف_کرو READ_NEW	0x01	// read a file into a new buffer
#تعریف_کرو READ_FILTER	0x02	// read filter output
#تعریف_کرو READ_STDIN	0x04	// read from stdin
#تعریف_کرو READ_بفر	0x08	// read from curbuf (converting stdin)
#تعریف_کرو READ_DUMMY	0x10	// reading into a dummy buffer
#تعریف_کرو READ_KEEP_UNDO	0x20	// keep undo info
#تعریف_کرو READ_FIFO	0x40	// read from fifo or socket
#تعریف_کرو READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#تعریف_کرو READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#تعریف_کرو INDENT_سیٹ	1	// set indent
#تعریف_کرو INDENT_INC	2	// increase indent
#تعریف_کرو INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#تعریف_کرو FM_BACKWARD	0x01	// search backwards
#تعریف_کرو FM_FORWARD	0x02	// search forwards
#تعریف_کرو FM_BLOCKSTOP	0x04	// stop at start/end of block
#تعریف_کرو FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#تعریف_کرو DOBUF_GOTO	0	// go to specified buffer
#تعریف_کرو DOBUF_بانٹو	1	// split window and go to specified buffer
#تعریف_کرو DOBUF_UNLOAD	2	// unload specified buffer(s)
#تعریف_کرو DOBUF_DEL	3	// delete specified buffer(s) from buflist
#تعریف_کرو DOBUF_WIPE	4	// delete specified buffer(s) really
#تعریف_کرو DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#تعریف_کرو DOBUF_CURRENT	0	// "count" buffer from current buffer
#تعریف_کرو DOBUF_پہلا	1	// "count" buffer from first buffer
#تعریف_کرو DOBUF_آخری	2	// "count" buffer from last buffer
#تعریف_کرو DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#تعریف_کرو DOBUF_FORCEIT	1	// :cmd!
#تعریف_کرو DOBUF_NOPOPUP	2	// skip popup window buffers
#تعریف_کرو DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#تعریف_کرو RE_تلاش	0	// save/use pat in/from search_pattern
#تعریف_کرو RE_SUBST	1	// save/use pat in/from subst_pattern
#تعریف_کرو RE_BOTH		2	// save pat in both patterns
#تعریف_کرو RE_آخری		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#تعریف_کرو RE_MAGIC	1	// 'magic' option
#تعریف_کرو RE_STRING	2	// match in string instead of buffer text
#تعریف_کرو RE_STRICT	4	// don't allow [abc] without ]
#تعریف_کرو RE_AUTO		8	// automatic engine selection

#اگر_تعریف_شدہ FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_سیٹ	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_سیٹ | REX_USE)
#اگر_ختم

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#تعریف_کرو FPC_SAME	1	// both exist and are the same file.
#تعریف_کرو FPC_DIFF	2	// both exist and are different files.
#تعریف_کرو FPC_NOTX	4	// both don't exist.
#تعریف_کرو FPC_DIFFX	6	// one of them doesn't exist.
#تعریف_کرو FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#تعریف_کرو ECMD_HIDE	0x01	// don't free the current buffer
#تعریف_کرو ECMD_سیٹ_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#تعریف_کرو ECMD_OLDBUF	0x04	// use existing buffer if it exists
#تعریف_کرو ECMD_FORCEIT	0x08	// ! used in Ex command
#تعریف_کرو ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#تعریف_کرو ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#تعریف_کرو ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#تعریف_کرو ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#تعریف_کرو ECMD_آخری	((linenr_T)-1)	// use last position in all files
#تعریف_کرو ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#تعریف_کرو DOCMD_VERBOSE	0x01	// included command in error message
#تعریف_کرو DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#تعریف_کرو DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#تعریف_کرو DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#تعریف_کرو DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#تعریف_کرو DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#تعریف_کرو DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#تعریف_کرو BL_WHITE	1	// cursor on first non-white in the line
#تعریف_کرو BL_SOL		2	// use 'sol' option
#تعریف_کرو BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#تعریف_کرو MFS_ALL		1	// also sync blocks with negative numbers
#تعریف_کرو MFS_STOP	2	// stop syncing when a character is available
#تعریف_کرو MFS_FLUSH	4	// flushed file to disk
#تعریف_کرو MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#تعریف_کرو BCO_ENTER	1	// going to enter the buffer
#تعریف_کرو BCO_ALWAYS	2	// always copy the options
#تعریف_کرو BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#تعریف_کرو PUT_FIXINDENT	1	// make indent look nice
#تعریف_کرو PUT_CURSEND	2	// leave cursor after end of new text
#تعریف_کرو PUT_CURSLINE	4	// leave cursor on last line of new text
#تعریف_کرو PUT_سطر	8	// put register as lines
#تعریف_کرو PUT_سطر_بانٹو	16	// split line for linewise register
#تعریف_کرو PUT_سطر_FORWARD 32	// put linewise register below Visual sel.
#تعریف_کرو PUT_بلاک_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#تعریف_کرو SIN_بدلا	1	// call changed_bytes() when line changed
#تعریف_کرو SIN_داخل_کرو	2	// insert indent before existing text
#تعریف_کرو SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#تعریف_کرو INSCHAR_FORMAT	1	// force formatting
#تعریف_کرو INSCHAR_DO_COM	2	// format comments
#تعریف_کرو INSCHAR_CTRLV	4	// char typed just after CTRL-V
#تعریف_کرو INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#تعریف_کرو INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#تعریف_کرو OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#تعریف_کرو OPENLINE_DO_COM		0x02    // format comments
#تعریف_کرو OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#تعریف_کرو OPENLINE_MARKFIX	0x08    // fix mark positions
#تعریف_کرو OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#تعریف_کرو OPENLINE_FORMAT		0x20    // formatting long comment
#تعریف_کرو OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#تعریف_کرو HIST_CMD	0	// colon commands
#تعریف_کرو HIST_تلاش	1	// search commands
#تعریف_کرو HIST_اظہاریہ	2	// expressions (from entering = register)
#تعریف_کرو HIST_داخلہ	3	// input() lines
#تعریف_کرو HIST_DEBUG	4	// debug commands
#تعریف_کرو HIST_گنتی	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#تعریف_کرو BARTYPE_نسخہ 1
#تعریف_کرو BARTYPE_HISTORY 2
#تعریف_کرو BARTYPE_رجسٹر 3
#تعریف_کرو BARTYPE_MARK 4

#تعریف_کرو VIMINFO_نسخہ 4
#تعریف_کرو VIMINFO_نسخہ_WITH_HISTORY 2
#تعریف_کرو VIMINFO_نسخہ_WITH_REGISTERS 3
#تعریف_کرو VIMINFO_نسخہ_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#تعریف_کرو DT_لیبل		1	// jump to newer position or same tag again
#تعریف_کرو DT_نکالو		2	// jump to older position
#تعریف_کرو DT_اگلا		3	// jump to next match of same tag
#تعریف_کرو DT_پچھلا		4	// jump to previous match of same tag
#تعریف_کرو DT_پہلا	5	// jump to first match of same tag
#تعریف_کرو DT_آخری		6	// jump to first match of same tag
#تعریف_کرو DT_SELECT	7	// jump to selection from list
#تعریف_کرو DT_HELP		8	// like DT_TAG, but no wildcards
#تعریف_کرو DT_JUMP		9	// jump to new tag or selection from list
#تعریف_کرو DT_CSCOPE	10	// cscope find command (like tjump)
#تعریف_کرو DT_LTAG		11	// tag using location list
#تعریف_کرو DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#تعریف_کرو لیبل_HELP	1	// only search for help tags
#تعریف_کرو لیبل_نام	2	// only return name of tag
#تعریف_کرو	لیبل_REGEXP	4	// use tag pattern as regexp
#تعریف_کرو	لیبل_NOIC	8	// don't always ignore case
#اگر_تعریف_شدہ FEAT_CSCOPE
# define لیبل_CSCOPE	16	// cscope tag
#اگر_ختم
#تعریف_کرو لیبل_VERBOSE	32	// message verbosity
#تعریف_کرو لیبل_INS_COMP	64	// Currently doing insert completion
#تعریف_کرو لیبل_KEEP_LANG	128	// keep current language
#تعریف_کرو لیبل_NO_TAGFUNC	256	// do not use 'tagfunc'

#تعریف_کرو لیبل_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#تعریف_کرو VIM_GENERIC	0
#تعریف_کرو VIM_ERROR	1
#تعریف_کرو VIM_WARNING	2
#تعریف_کرو VIM_INFO	3
#تعریف_کرو VIM_QUESTION	4
#تعریف_کرو VIM_آخری_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#تعریف_کرو VIM_YES		2
#تعریف_کرو VIM_NO		3
#تعریف_کرو VIM_CANCEL	4
#تعریف_کرو VIM_ALL		5
#تعریف_کرو VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#تعریف_کرو WSP_گنجائش	0x01	// require enough room
#تعریف_کرو WSP_VERT	0x02	// split/equalize vertically
#تعریف_کرو WSP_HOR		0x04	// equalize horizontally
#تعریف_کرو WSP_چوٹی		0x08	// window at top-left of shell
#تعریف_کرو WSP_BOT		0x10	// window at bottom-right of shell
#تعریف_کرو WSP_HELP	0x20	// creating the help window
#تعریف_کرو WSP_نیچے	0x40	// put new window below/right
#تعریف_کرو WSP_ABOVE	0x80	// put new window above/left
#تعریف_کرو WSP_NEWLOC	0x100	// don't copy location list
#تعریف_کرو WSP_FORCE_گنجائش	0x200	// ignore "not enough room" errors
#تعریف_کرو WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#تعریف_کرو RESIZE_VERT	1	// resize vertically
#تعریف_کرو RESIZE_HOR	2	// resize horizontally
#تعریف_کرو RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#تعریف_کرو CCGD_AW		1	// do autowrite if buffer was changed
#تعریف_کرو CCGD_MULTWIN	2	// check also when several wins for the buf
#تعریف_کرو CCGD_FORCEIT	4	// ! used
#تعریف_کرو CCGD_ALLBUF	8	// may write all buffers
#تعریف_کرو CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#تعریف_کرو OPT_FREE	0x01	// free old value if it was allocated
#تعریف_کرو OPT_GLOBAL	0x02	// use global value
#تعریف_کرو OPT_LOCAL	0x04	// use local value
#تعریف_کرو OPT_MODELINE	0x08	// option in modeline
#تعریف_کرو OPT_WINONLY	0x10	// only set window-local options
#تعریف_کرو OPT_NOWIN	0x20	// don't set window-local options
#تعریف_کرو OPT_ONECOLUMN	0x40	// list options one per line
#تعریف_کرو OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#تعریف_کرو OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#تعریف_کرو DLG_BUTTON_SEP	'\n'
#تعریف_کرو DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#تعریف_کرو NO_SCREEN	2	// no screen updating yet
#تعریف_کرو NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#تعریف_کرو SEA_NONE	0	// don't use dialog
#تعریف_کرو SEA_DIALOG	1	// use dialog when possible
#تعریف_کرو SEA_QUIT	2	// quit editing the file
#تعریف_کرو SEA_RECOVER	3	// recover the file
#تعریف_کرو SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#تعریف_کرو MIN_تبادلہ_صفحہ_سائز 1048
#تعریف_کرو MAX_تبادلہ_صفحہ_سائز 50000

// Special values for current_sctx.sc_sid.
#تعریف_کرو SID_MODELINE	(-1)	// when using a modeline
#تعریف_کرو SID_CMDARG	(-2)	// for "--cmd" argument
#تعریف_کرو SID_CARG	(-3)	// for "-c" argument
#تعریف_کرو SID_ENV		(-4)	// for sourcing environment variable
#تعریف_کرو SID_ERROR	(-5)	// option was reset because of an error
#تعریف_کرو SID_NONE	(-6)	// don't set scriptID
#تعریف_کرو SID_WINLAYOUT	(-7)	// changing window size

/*
 * Events for autocommands. Must be kept in sync with "event_tab".
 */
شمار auto_event
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

قسمتعریف شمار auto_event event_T;

/*
 * Values for index in highlight_attr[].
 * When making changes, also update HL_FLAGS below!
 * And update the default value of 'highlight': HIGHLIGHT_INIT in optiondefs.h
 */
قسمتعریف شمار
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
    , HLF_جوڑو	    // Added diff line
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
    , HLF_نکالو	    // popup window body
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
    , HLF_گنتی	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#تعریف_کرو HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
قسمتعریف شمار
{
    SMT_ALL = 0		    // Move to "all" words
    , SMT_BAD		    // Move to "bad" words only
    , SMT_RARE		    // Move to "rare" words only
} smt_T;
/*
 * Boolean constants
 */
#اگر_غیر_تعریف_شدہ TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#اگر_ختم

#تعریف_کرو MAYBE	2	    // sometimes used for a variant on TRUE

#تعریف_کرو LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_مقصد_LOG_THIS اگر (ch_log_output == FALSE) ch_log_output = TRUE;
#ورنہ
// no logging support
# define MAY_WANT_مقصد_LOG_THIS
#اگر_ختم

#اگر_غیر_تعریف_شدہ UINT32_T
قسمتعریف UINT32_TYPEDEF UINT32_T;
#اگر_ختم

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#تعریف_کرو OP_NOP		0	// no pending operation
#تعریف_کرو OP_DELETE	1	// "d"  delete operator
#تعریف_کرو OP_YANK		2	// "y"  yank operator
#تعریف_کرو OP_CHANGE	3	// "c"  change operator
#تعریف_کرو OP_LSHIFT	4	// "<"  left shift operator
#تعریف_کرو OP_RSHIFT	5	// ">"  right shift operator
#تعریف_کرو OP_FILTER	6	// "!"  filter operator
#تعریف_کرو OP_TILDE	7	// "g~" switch case operator
#تعریف_کرو OP_INDENT	8	// "="  indent operator
#تعریف_کرو OP_FORMAT	9	// "gq" format operator
#تعریف_کرو OP_COLON	10	// ":"  colon operator
#تعریف_کرو OP_بالائی	11	// "gU" make upper case operator
#تعریف_کرو OP_زیریں	12	// "gu" make lower case operator
#تعریف_کرو OP_JOIN		13	// "J"  join operator, only for Visual mode
#تعریف_کرو OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#تعریف_کرو OP_ROT13	15	// "g?" rot-13 encoding
#تعریف_کرو OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#تعریف_کرو OP_داخل_کرو	17	// "I"  Insert column, only for Visual mode
#تعریف_کرو OP_APPEND	18	// "A"  Append column, only for Visual mode
#تعریف_کرو OP_FOLD		19	// "zf" define a fold
#تعریف_کرو OP_FOLDOPEN	20	// "zo" open folds
#تعریف_کرو OP_FOLDOPENREC	21	// "zO" open folds recursively
#تعریف_کرو OP_FOLDCLOSE	22	// "zc" close folds
#تعریف_کرو OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#تعریف_کرو OP_FOLDDEL	24	// "zd" delete folds
#تعریف_کرو OP_FOLDDELREC	25	// "zD" delete folds recursively
#تعریف_کرو OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#تعریف_کرو OP_FUNCTION	27	// "g@" call 'operatorfunc'
#تعریف_کرو OP_NR_جوڑو	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#تعریف_کرو OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#تعریف_کرو MCHAR	0		// character-wise movement/register
#تعریف_کرو MLINE	1		// line-wise movement/register
#تعریف_کرو MBLOCK	2		// block-wise register

#تعریف_کرو MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#تعریف_کرو MIN_COLUMNS	12	// minimal columns for screen
#تعریف_کرو MIN_سطریں	2	// minimal lines for screen
#تعریف_کرو MIN_CMDHEIGHT	1	// minimal height for command line
#تعریف_کرو حالت_اونچائی_2	1	// default height of a status line under a
				//کھڑکی
#اگر_تعریف_شدہ FEAT_MENU		// height of a status line under a window
# define WINBAR_اونچائی(wp)	(wp)->w_winbar_اونچائی
# define VISIBLE_اونچائی(wp)	((wp)->w_اونچائی + (wp)->w_winbar_اونچائی)
#ورنہ
# define WINBAR_اونچائی(wp)	0
# define VISIBLE_اونچائی(wp)	(wp)->w_اونچائی
#اگر_ختم
#تعریف_کرو QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#اگر_غیر_تعریف_شدہ CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#اگر_ختم

#تعریف_کرو LSIZE	    512		// max. size of a line in the tags file

#تعریف_کرو IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#تعریف_کرو DIALOG_پیغام_سائز 1000	// buffer size for dialog_msg()

#تعریف_کرو پیغام_BUF_LEN 480	// length of buffer for small messages
#تعریف_کرو پیغام_BUF_CLEN  (پیغام_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#تعریف_کرو FOLD_متن_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#تعریف_کرو TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#تعریف_کرو MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#تعریف_کرو MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#تعریف_کرو UNDO_HASH_سائز 32

#اگر_تعریف_شدہ HAVE_FCNTL_H
# include <fcntl.h>
#اگر_ختم

#اگر_تعریف_شدہ ثنائی_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#ورنہ
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#اگر_ختم

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#اگر_تعریف_شدہ __CYGWIN__
# define O_EXTRA    O_ثنائی
#ورنہ
# define O_EXTRA    0
#اگر_ختم

#اگر_غیر_تعریف_شدہ O_NOFOLLOW
# define O_NOFOLLOW 0
#اگر_ختم

#اگر_غیر_تعریف_شدہ W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#اگر_ختم
#اگر_غیر_تعریف_شدہ R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#اگر_ختم

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#تعریف_کرو ALLOC_ONE(type)  (type *)alloc(حجم(type))
#تعریف_کرو ALLOC_ONE_ID(type, id)  (type *)alloc_id(حجم(type), id)
#تعریف_کرو ALLOC_MULT(type, گنتی)  (type *)alloc(حجم(type) * (گنتی))
#تعریف_کرو ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(حجم(type))
#تعریف_کرو ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(حجم(type), id)
#تعریف_کرو ALLOC_CLEAR_MULT(type, گنتی)  (type *)alloc_clear(حجم(type) * (گنتی))
#تعریف_کرو LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(حجم(type), FALSE)
#تعریف_کرو LALLOC_CLEAR_MULT(type, گنتی)  (type *)lalloc_clear(حجم(type) * (گنتی), FALSE)
#تعریف_کرو LALLOC_MULT(type, گنتی)  (type *)lalloc(حجم(type) * (گنتی), FALSE)

#اگر_تعریف_شدہ HAVE_MEMSET
# define vim_memset(ptr, c, سائز)   memset((ptr), (c), (سائز))
#ورنہ
خالی *vim_memset(خالی *, صحیح, size_t);
#اگر_ختم
#تعریف_کرو CLEAR_میدان(میدان)  vim_memset(&(میدان), 0, حجم(میدان))
#تعریف_کرو CLEAR_اشارہ_گر(ptr)  vim_memset((ptr), 0, حجم(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#تعریف_کرو STRLEN(s)	    لڑی_لمبائی((حرف *)(s))
#تعریف_کرو STRCPY(d, s)	    strcpy((حرف *)(d), (حرف *)(s))
#تعریف_کرو STRNCPY(d, s, n)    strncpy((حرف *)(d), (حرف *)(s), (n))
#تعریف_کرو STRCMP(d, s)	    strcmp((حرف *)(d), (حرف *)(s))
#تعریف_کرو STRNCMP(d, s, n)    لڑی_موازنہ((حرف *)(d), (حرف *)(s), (n))
#اگر_تعریف_شدہ HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((حرف *)(d), (حرف *)(s))
#ورنہ
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((حرف *)(d), (حرف *)(s))
# ورنہ
#  define STRICMP(d, s)	    vim_stricmp((حرف *)(d), (حرف *)(s))
# endif
#اگر_ختم
#اگر_تعریف_شدہ HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((حرف *)(d), (حرف *)(s))
#ورنہ
# define STRCOLL(d, s)     strcmp((حرف *)(d), (حرف *)(s))
#اگر_ختم

// Like strcpy() but allows overlapped source and destination.
#تعریف_کرو STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#اگر_تعریف_شدہ HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((حرف *)(d), (حرف *)(s), (n))
#ورنہ
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((حرف *)(d), (حرف *)(s), (n))
# ورنہ
#  define STRNICMP(d, s, n) vim_strnicmp((حرف *)(d), (حرف *)(s), (n))
# endif
#اگر_ختم

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#تعریف_کرو MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحیح)MAXCOL)
#تعریف_کرو MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (صحیح)(n))
#تعریف_کرو MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#تعریف_کرو STRCAT(d, s)	    strcat((حرف *)(d), (حرف *)(s))
#تعریف_کرو STRNCAT(d, s, n)    strncat((حرف *)(d), (حرف *)(s), (n))

#اگر_تعریف_شدہ HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((حرف *)(s), (حرف *)(cs))
#اگر_ختم

#تعریف_کرو برآمد_STR(s)		    برآمد_str((char_u *)(s))
#تعریف_کرو برآمد_STR_NF(s)		    برآمد_str_nf((char_u *)(s))

#اگر_تعریف_شدہ FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.اندر_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.اندر_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_ثنائی_ہندسہ		    (gui.اندر_use || p_tgc)
# ورنہ
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_ثنائی_ہندسہ		    gui.اندر_use
# endif
#ورنہ
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_ثنائی_ہندسہ		    p_tgc
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#ورنہ
# define IS_CTERM		    (t_colors > 1)
#اگر_ختم
#اگر_تعریف_شدہ GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#اگر_ختم

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#اگر_تعریف_شدہ HAVE_STRERROR
# define PERROR(پیغام)		    (خالی)semsg("%s: %s", (حرف *)(پیغام), strerror(errno))
#ورنہ
# define PERROR(پیغام)		    do_perror(پیغام)
#اگر_ختم

قسمتعریف طویل	linenr_T;		// line number type
قسمتعریف صحیح	colnr_T;		// column number type
قسمتعریف غیرعلامتی مختصر disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#اگر_تعریف_شدہ __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#ورنہ
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#اگر_ختم

#تعریف_کرو SHOWCMD_ستون_تعداد 10			// columns needed by shown command

قسمتعریف خالی	    *vim_acl_T;		// dummy to pass an ACL to a function

#اگر_غیر_تعریف_شدہ mch_memmove
# define mch_memmove(مقصد, from, len) memmove((حرف*)(مقصد), (حرف*)(from), (len))
#اگر_ختم

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#تعریف_کرو fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#تعریف_کرو fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#اگر defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_داخلہ_BUF
#اگر_ختم

#اگر !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, گنتی) vim_read((fd), (buf), (گنتی))
# define write_eintr(fd, buf, گنتی) vim_write((fd), (buf), (گنتی))
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, گنتی)   اقرأ((fd), (حرف *)(buf), (غیرعلامتی صحیح)(گنتی))
# define vim_write(fd, buf, گنتی)  اكتب((fd), (حرف *)(buf), (غیرعلامتی صحیح)(گنتی))
#ورنہ
# define vim_read(fd, buf, گنتی)   اقرأ((fd), (حرف *)(buf), (گنتی))
# define vim_write(fd, buf, گنتی)  اكتب((fd), (حرف *)(buf), (گنتی))
#اگر_ختم

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#تعریف_کرو HL_ATTR(n)	highlight_attr[(صحیح)(n)]
#تعریف_کرو رکن_STR(n)	رکن_لڑیاں[(صحیح)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#اگر_غیر_تعریف_شدہ EXTERN
# define EXTERN خارجی
# define INIT(x)
# define INIT2(a, b)
# define INIT3(a, b, c)
# define INIT4(a, b, c, d)
# define INIT5(a, b, c, d, e)
# define INIT6(a, b, c, d, e, f)
#ورنہ
# ifndef INIT
#  define INIT(x) x
#  define INIT2(a, b) = {a, b}
#  define INIT3(a, b, c) = {a, b, c}
#  define INIT4(a, b, c, d) = {a, b, c, d}
#  define INIT5(a, b, c, d, e) = {a, b, c, d, e}
#  define INIT6(a, b, c, d, e, f) = {a, b, c, d, e, f}
#  define DO_INIT
# endif
#اگر_ختم

#تعریف_کرو MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#تعریف_کرو MB_MAXBYTES	30

#اگر (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
قسمتعریف LARGE_INTEGER proftime_T;
#  define PROF_وقت_BLANK "           "
#  define PROF_TOTALS_سرخی "count  total (s)   self (s)"
# ورنہ
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  اگر defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
قسمتعریف ساخت timespec proftime_T;
#   define PROF_GET_وقت(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_وقت_FORMAT "%3ld.%09ld"
#   define PROF_وقت_BLANK "              "
#   define PROF_TOTALS_سرخی "count     total (s)      self (s)"
#  ورنہ
قسمتعریف ساخت timeval proftime_T;
#   define PROF_GET_وقت(tm) gettimeofday(tm, تہی)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_وقت_FORMAT "%3ld.%06ld"
#   define PROF_وقت_BLANK "           "
#   define PROF_TOTALS_سرخی "count  total (s)   self (s)"
#  endif
# endif
#ورنہ
قسمتعریف صحیح proftime_T;	    // dummy for function prototypes
#اگر_ختم

// Type of compilation passed to compile_def_function()
قسمتعریف شمار {
    CT_NONE,	    // use df_instr
    CT_PROFILE,	    // use df_instr_prof
    CT_DEBUG	    // use df_instr_debug, overrules CT_PROFILE
} compiletype_T;

/*
 * When compiling with 32 bit Perl time_t is 32 bits in the Perl code but 64
 * bits elsewhere.  That causes memory corruption.  Define time_T and use it
 * for global variables to avoid that.
 */
#اگر_تعریف_شدہ MSWIN
قسمتعریف __time64_t  وقت_T;
#ورنہ
قسمتعریف time_t	    وقت_T;
#اگر_ختم

#اگر_تعریف_شدہ _WIN64
قسمتعریف __int64 sock_T;
#ورنہ
قسمتعریف صحیح sock_T;
#اگر_ختم

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#اگر_ختم

#اگر defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#اگر_ختم

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#شامل "option.h"	// options and default values

#شامل "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#شامل "structs.h"	// defines many structures
#شامل "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#شامل "alloc.h"

// Values for "do_profiling".
#تعریف_کرو PROF_NONE	0	// profiling not started
#تعریف_کرو PROF_YES	1	// profiling busy
#تعریف_کرو PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#تعریف_کرو MOUSE_بایاں	0x00
#تعریف_کرو MOUSE_درمیان	0x01
#تعریف_کرو MOUSE_دایاں	0x02
#تعریف_کرو MOUSE_RELEASE	0x03

// bit masks for modifiers:
#تعریف_کرو MOUSE_سرکاؤ	0x04
#تعریف_کرو MOUSE_ALT	0x08
#تعریف_کرو MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#تعریف_کرو MOUSE_4	0x100	// scroll wheel down
#تعریف_کرو MOUSE_5	0x200	// scroll wheel up

#تعریف_کرو MOUSE_X1	0x300 // Mouse-button X1 (6th)
#تعریف_کرو MOUSE_X2	0x400 // Mouse-button X2

#تعریف_کرو MOUSE_6	0x500	// scroll wheel left
#تعریف_کرو MOUSE_7	0x600	// scroll wheel right

#تعریف_کرو MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#تعریف_کرو MOUSE_DRAG_XTERM   0x40

#تعریف_کرو MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#تعریف_کرو MOUSEWHEEL_کم		0x60

// Lowest button code for extra mouse buttons 8-11
#تعریف_کرو MOUSESIDEBUTTONS_کم		0xa0

#تعریف_کرو MOUSE_CLICK_نقاب	0x03

#تعریف_کرو عدد_MOUSE_CLICKS(رمز) \
    (((غیرعلامتی)((رمز) & 0xC0) >> 6) + 1)

#تعریف_کرو سیٹ_عدد_MOUSE_CLICKS(رمز, عدد) \
    ((رمز) = ((رمز) & 0x3f) | ((((عدد) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#تعریف_کرو MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#تعریف_کرو اندر_UNKNOWN		0
#تعریف_کرو اندر_بفر		1
#تعریف_کرو اندر_حالت_سطر		2	// on status or command line
#تعریف_کرو اندر_SEP_سطر		4	// on vertical separator line
#تعریف_کرو اندر_OTHER_WIN		8	// in other window but can't go there
#تعریف_کرو CURSOR_MOVED		0x100
#تعریف_کرو MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#تعریف_کرو MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#تعریف_کرو MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#تعریف_کرو MOUSE_FOCUS		0x01	// need to stay in this window
#تعریف_کرو MOUSE_MAY_VIS		0x02	// may start Visual mode
#تعریف_کرو MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#تعریف_کرو MOUSE_SETPOS		0x08	// only set current mouse position
#تعریف_کرو MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#تعریف_کرو MOUSE_RELEASED		0x20	// button was released

#اگر defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وقت_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#اگر_ختم


// defines for eval_vars()
#تعریف_کرو VALID_راستہ		1
#تعریف_کرو VALID_سر		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#تعریف_کرو VV_گنتی	0
#تعریف_کرو VV_COUNT1	1
#تعریف_کرو VV_PREVCOUNT	2
#تعریف_کرو VV_ERRMSG	3
#تعریف_کرو VV_WARNINGMSG	4
#تعریف_کرو VV_STATUSMSG	5
#تعریف_کرو VV_SHELL_ERROR	6
#تعریف_کرو VV_THIS_SESSION	7
#تعریف_کرو VV_نسخہ	8
#تعریف_کرو VV_LNUM		9
#تعریف_کرو VV_TERMRESPONSE	10
#تعریف_کرو VV_FNAME	11
#تعریف_کرو VV_LANG		12
#تعریف_کرو VV_LC_وقت	13
#تعریف_کرو VV_CTYPE	14
#تعریف_کرو VV_CC_FROM	15
#تعریف_کرو VV_CC_مقصد	16
#تعریف_کرو VV_FNAME_اندر	17
#تعریف_کرو VV_FNAME_برآمد	18
#تعریف_کرو VV_FNAME_NEW	19
#تعریف_کرو VV_FNAME_DIFF	20
#تعریف_کرو VV_CMDARG	21
#تعریف_کرو VV_FOLDSTART	22
#تعریف_کرو VV_FOLDEND	23
#تعریف_کرو VV_FOLDDASHES	24
#تعریف_کرو VV_FOLDLEVEL	25
#تعریف_کرو VV_PROGNAME	26
#تعریف_کرو VV_SEND_SERVER	27
#تعریف_کرو VV_DYING	28
#تعریف_کرو VV_EXCEPTION	29
#تعریف_کرو VV_THROWPOINT	30
#تعریف_کرو VV_REG		31
#تعریف_کرو VV_CMDBANG	32
#تعریف_کرو VV_INSERTMODE	33
#تعریف_کرو VV_VAL		34
#تعریف_کرو VV_کلید		35
#تعریف_کرو VV_PROFILING	36
#تعریف_کرو VV_FCS_REASON	37
#تعریف_کرو VV_FCS_CHOICE	38
#تعریف_کرو VV_BEVAL_BUFNR	39
#تعریف_کرو VV_BEVAL_WINNR	40
#تعریف_کرو VV_BEVAL_WINID	41
#تعریف_کرو VV_BEVAL_LNUM	42
#تعریف_کرو VV_BEVAL_ستون	43
#تعریف_کرو VV_BEVAL_متن	44
#تعریف_کرو VV_SCROLLSTART	45
#تعریف_کرو VV_SWAPNAME	46
#تعریف_کرو VV_SWAPCHOICE	47
#تعریف_کرو VV_SWAPCOMMAND	48
#تعریف_کرو VV_CHAR		49
#تعریف_کرو VV_MOUSE_WIN	50
#تعریف_کرو VV_MOUSE_WINID	51
#تعریف_کرو VV_MOUSE_LNUM   52
#تعریف_کرو VV_MOUSE_ستون	53
#تعریف_کرو VV_OP		54
#تعریف_کرو VV_SEARCHFORWARD 55
#تعریف_کرو VV_HLSEARCH	56
#تعریف_کرو VV_OLDFILES	57
#تعریف_کرو VV_WINDOWID	58
#تعریف_کرو VV_PROGPATH	59
#تعریف_کرو VV_COMPLETED_شے 60
#تعریف_کرو VV_OPTION_NEW   61
#تعریف_کرو VV_OPTION_OLD   62
#تعریف_کرو VV_OPTION_OLDLOCAL 63
#تعریف_کرو VV_OPTION_OLDGLOBAL 64
#تعریف_کرو VV_OPTION_COMMAND 65
#تعریف_کرو VV_OPTION_TYPE  66
#تعریف_کرو VV_ERRORS	67
#تعریف_کرو VV_FALSE	68
#تعریف_کرو VV_TRUE		69
#تعریف_کرو VV_NONE		70
#تعریف_کرو VV_NULL		71
#تعریف_کرو VV_NUMBERMAX	72
#تعریف_کرو VV_NUMBERMIN	73
#تعریف_کرو VV_NUMBERSIZE	74
#تعریف_کرو VV_VIM_DID_ENTER 75
#تعریف_کرو VV_TESTING	76
#تعریف_کرو VV_TYPE_نمبر	77
#تعریف_کرو VV_TYPE_STRING	78
#تعریف_کرو VV_TYPE_FUNC	79
#تعریف_کرو VV_TYPE_LIST	80
#تعریف_کرو VV_TYPE_DICT	81
#تعریف_کرو VV_TYPE_FLOAT	82
#تعریف_کرو VV_TYPE_BOOL	83
#تعریف_کرو VV_TYPE_NONE	84
#تعریف_کرو VV_TYPE_کام	85
#تعریف_کرو VV_TYPE_CHANNEL	86
#تعریف_کرو VV_TYPE_BLOB	87
#تعریف_کرو VV_TYPE_CLASS	88
#تعریف_کرو VV_TYPE_OBJECT	89
#تعریف_کرو VV_TERMRFGRESP	90
#تعریف_کرو VV_TERMRBGRESP	91
#تعریف_کرو VV_TERMU7RESP	92
#تعریف_کرو VV_TERMSTYLERESP 93
#تعریف_کرو VV_TERMBLINKRESP 94
#تعریف_کرو VV_EVENT	95
#تعریف_کرو VV_VERSIONLONG	96
#تعریف_کرو VV_ECHOSPACE	97
#تعریف_کرو VV_ARGV		98
#تعریف_کرو VV_COLLATE      99
#تعریف_کرو VV_EXITING	100
#تعریف_کرو VV_COLORNAMES   101
#تعریف_کرو VV_SIZEOFINT	102
#تعریف_کرو VV_SIZEOFLONG	103
#تعریف_کرو VV_SIZEOFPOINTER 104
#تعریف_کرو VV_MAXCOL	105
#تعریف_کرو VV_PYTHON3_نسخہ 106
#تعریف_کرو VV_TYPE_TYPEALIAS 107
#تعریف_کرو VV_TYPE_ENUM	  108
#تعریف_کرو VV_TYPE_ENUMVALUE 109
#تعریف_کرو VV_STACKTRACE	110
#تعریف_کرو VV_TYPE_TUPLE	111
#تعریف_کرو VV_WAYLAND_DISPLAY 112
#تعریف_کرو VV_CLIPMETHOD 113
#تعریف_کرو VV_TERMDA1 114
#تعریف_کرو VV_TERMOSC 115
#تعریف_کرو VV_VIM_DID_INIT		116
#تعریف_کرو VV_CLIPPROVIDERS 117
#تعریف_کرو VV_IMAGEBACKEND 118
#تعریف_کرو VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#تعریف_کرو VVAL_FALSE	0L	// VAR_BOOL
#تعریف_کرو VVAL_TRUE	1L	// VAR_BOOL
#تعریف_کرو VVAL_NONE	2L	// VAR_SPECIAL
#تعریف_کرو VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#تعریف_کرو VAR_TYPE_نمبر	    0
#تعریف_کرو VAR_TYPE_STRING	    1
#تعریف_کرو VAR_TYPE_FUNC	    2
#تعریف_کرو VAR_TYPE_LIST	    3
#تعریف_کرو VAR_TYPE_DICT	    4
#تعریف_کرو VAR_TYPE_FLOAT	    5
#تعریف_کرو VAR_TYPE_BOOL	    6
#تعریف_کرو VAR_TYPE_NONE	    7
#تعریف_کرو VAR_TYPE_کام	    8
#تعریف_کرو VAR_TYPE_CHANNEL    9
#تعریف_کرو VAR_TYPE_BLOB	    10
#تعریف_کرو VAR_TYPE_INSTR	    11
#تعریف_کرو VAR_TYPE_CLASS	    12
#تعریف_کرو VAR_TYPE_OBJECT	    13
#تعریف_کرو VAR_TYPE_TYPEALIAS  14
#تعریف_کرو VAR_TYPE_ENUM	    15
#تعریف_کرو VAR_TYPE_ENUMVALUE  16
#تعریف_کرو VAR_TYPE_TUPLE	    17

#تعریف_کرو DICT_MAXNEST 100	// maximum nesting of lists and dicts

#تعریف_کرو TABSTOP_MAX 9999

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
قسمتعریف شمار {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD

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
# define SELECT_اندر_پیش_رفت	1
# define SELECT_مکمل		2

# define SELECT_منوال_CHAR	0
# define SELECT_منوال_لفظ	1
# define SELECT_منوال_سطر	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
قسمتعریف ساخت
{
    صحیح		دستیاب;	// Is clipboard available?
    صحیح		owned;		// Flag: do we own the selection?
    pos_T	آغاز;		// Start of selected area
    pos_T	انتہا;		// End of selected area
    صحیح		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_صف;
    short_u	origin_آغاز_ستون;
    short_u	origin_انتہا_ستون;
    short_u	لفظ_آغاز_ستون;
    short_u	لفظ_انتہا_ستون;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_ستون;
    short_u	max_ستون;
    short_u	min_صف;
    short_u	max_صف;
# endif

    pos_T	پچھلا;		// Previous position
    short_u	state;		// Current selection state
    short_u	منوال;		// Select by char, word, or line.

# اگر defined(FEAT_GUI_X11) || defined(FEAT_XCLIPBOARD)
    Atom	sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# اگر defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
    GdkAtom     gtk_sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# اگر defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    int_u	format;		// Vim's own special clipboard format
    int_u	format_raw;	// Vim's raw text clipboard format
# endif
# ifdef FEAT_GUI_HAIKU
    // No clipboard at the moment. TODO?
# endif
} Clipboard_T;
#ورنہ
قسمتعریف صحیح Clipboard_T;	// This is required for the prototypes.
#اگر_ختم

// Use 64-bit stat structure on MS-Windows.
#اگر_تعریف_شدہ MSWIN
قسمتعریف ساخت _stat64 stat_T;
#ورنہ
قسمتعریف ساخت stat stat_T;
#اگر_ختم

قسمتعریف ساخت soundcb_S soundcb_T;

#اگر (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(لکھو, fmt_idx, arg_idx)))
#ورنہ
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#اگر_ختم

#اگر defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#ورنہ
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#اگر_ختم

قسمتعریف شمار {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_مطابقت,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
قسمتعریف شمار {
    PASTE_داخل_کرو,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_منوال_T;

// Argument for flush_buffers().
قسمتعریف شمار {
    FLUSH_اقل,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_داخلہ		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
قسمتعریف شمار {
    USEPOPUP_NONE,
    USEPOPUP_معمول,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
قسمتعریف شمار {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_ڈھیر,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#تعریف_کرو ENABLE_STL_منوال_MULTI_NL

// Argument for build_stl_str_hl_local().
قسمتعریف شمار {
    STL_منوال_SINGLE,	    // Does not accept line breaks "%@"
    STL_منوال_MULTI,	    // Accept line breaks "%@"
    STL_منوال_GET_RENDERED_اونچائی,   // Just get stl rendered height
#اگر_تعریف_شدہ ENABLE_STL_منوال_MULTI_NL
    STL_منوال_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#اگر_ختم
} stl_منوال_T;

// Return value of match_keyprotocol()
قسمتعریف شمار {
    KEYPROTOCOL_NONE,
    KEYPROTOCOL_MOK2,
    KEYPROTOCOL_KITTY,
    KEYPROTOCOL_FAIL
} keyprot_T;

// errors for when calling a function
قسمتعریف شمار {
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
قسمتعریف شمار {
    CPT_ABBR,		// "abbr"
    CPT_قسم,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_گنتی,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
قسمتعریف حرف *(*opt_did_سیٹ_cb_T)(optset_T *دلائل);

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
قسمتعریف صحیح (*opt_expand_cb_T)(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches);

// Flags for assignment functions.
#تعریف_کرو تفویض_VAR	0     // ":var" (nothing special)
#تعریف_کرو تفویض_FINAL	0x01  // ":final"
#تعریف_کرو تفویض_CONST	0x02  // ":const"
#تعریف_کرو تفویض_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#تعریف_کرو تفویض_DECL	0x08  // may declare variable if it does not exist
#تعریف_کرو تفویض_UNPACK	0x10  // using [a, b] = list
#تعریف_کرو تفویض_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#تعریف_کرو تفویض_FOR_LOOP 0x40 // assigning to loop variable
#تعریف_کرو تفویض_INIT	0x80 // not assigning a value, just a declaration
#تعریف_کرو تفویض_UPDATE_بلاک_ID 0x100  // update sav_block_id
#تعریف_کرو تفویض_COMPOUND_OP 0x200  // compound operator e.g. "+="

#شامل "ex_cmds.h"	    // Ex command defines
#شامل "spell.h"	    // spell checking stuff

#شامل "proto.h"	    // function prototypes

// This has to go after the include of proto.h, as proto/gui.pro declares
// functions of these names. The declarations would break if the defines had
// been seen at that stage.  But it must be before globals.h, where error_ga
// is declared.
#اگر !defined(MSWIN) && !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_HAIKU) \
	&& !defined(FEAT_GUI_GTK) && !defined(PROTO)
# define mch_errmsg(str)	fprintf(stderr, "%s", (str))
# define display_errors()	fflush(stderr)
# define mch_پیغام(str)		لکھو("%s", (str))
#ورنہ
# define USE_MCH_ERRMSG
#اگر_ختم

#اگر defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#اگر_ختم

#اگر defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_داخلہ_طریقہ
#اگر_ختم

#اگر_غیر_تعریف_شدہ FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define نمبر_چوڑائی(x) 7
#اگر_ختم

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#اگر !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#اگر_ختم

#شامل "globals.h"	    // global variables and messages
#شامل "errors.h"	    // error messages

#اگر_غیر_تعریف_شدہ PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ساکن اندرونی صحیح
اندر_vim9script(خالی)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    واپس (current_sctx.sc_نسخہ == SCRIPT_نسخہ_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#اگر_ختم

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#اگر defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#اگر_ختم

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#اگر_تعریف_شدہ FEAT_BROWSE
# ifdef BACKSLASH_اندر_FILENAME
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*.*)\t*.*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVB code (*.bas, *.frm)\t*.bas;*.frm\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# ورنہ
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*)\t*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*)\t*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*)\t*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# endif
# define BROWSE_SAVE 1	    // flag for do_browse()
# define BROWSE_DIR 2	    // flag for do_browse()
#اگر_ختم

#اگر_تعریف_شدہ _MSC_VER
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
#اگر_ختم

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#اگر defined(MEM_PROFILE)
# define vim_realloc(ptr, سائز)  mem_realloc((ptr), (سائز))
#ورنہ
# define vim_realloc(ptr, سائز)  realloc((ptr), (سائز))
#اگر_ختم

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#تعریف_کرو MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#تعریف_کرو MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#تعریف_کرو ENC_8_ثنائی_ہندسہ	0x01
#تعریف_کرو ENC_DBCS	0x02
#تعریف_کرو ENC_UNICODE	0x04

#تعریف_کرو ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#تعریف_کرو ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#تعریف_کرو ENC_2BYTE	0x40	    // Unicode: UCS-2
#تعریف_کرو ENC_4BYTE	0x80	    // Unicode: UCS-4
#تعریف_کرو ENC_2_لفظ	0x100	    // Unicode: UTF-16

#تعریف_کرو ENC_LATIN1	0x200	    // Latin1
#تعریف_کرو ENC_LATIN9	0x400	    // Latin9
#تعریف_کرو ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#اگر_تعریف_شدہ USE_ICONV
# ifndef EILSEQ
#  define EILSEQ 123
# endif
# ifdef DYNAMIC_ICONV
// On Win32 iconv.dll is dynamically loaded.
#  define ICONV_ERRNO (*iconv_errno())
#  define ICONV_E2BIG  7
#  define ICONV_EINVAL 22
#  define ICONV_EILSEQ 42
# ورنہ
#  define ICONV_ERRNO errno
#  define ICONV_E2BIG  E2BIG
#  define ICONV_EINVAL EINVAL
#  define ICONV_EILSEQ EILSEQ
# endif
#اگر_ختم

#تعریف_کرو SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#اگر_ختم

#اگر defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.اندر_use ? (دوہرا)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.اندر_use ? (دوہرا)(p) / gui.scale : (p))
#ورنہ
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#اگر_ختم

#اگر defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.اندر_use) ? gui_mch_get_display() : xterm_dpy)
# ورنہ
#  define X_DISPLAY	(gui.اندر_use ? gui.dpy : xterm_dpy)
# endif
#وگرنہ_اگر defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.اندر_use) ? gui_mch_get_display() : (Display *)تہی)
# ورنہ
#  define X_DISPLAY	gui.dpy
# endif
#وگرنہ_اگر defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#ورنہ
# define X_DISPLAY	(Display *)تہی
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GTK
# اگر !GTK_CHECK_نسخہ(2,14,0)
#  define gtk_widget_get_کھڑکی(wid)	((wid)->کھڑکی_2)
#  define gtk_plug_get_socket_کھڑکی(wid)	((wid)->socket_کھڑکی)
#  define gtk_selection_ڈیٹا_get_ڈیٹا(sel)	((sel)->ڈیٹا)
#  define gtk_selection_ڈیٹا_get_ڈیٹا_type(sel)	((sel)->type)
#  define gtk_selection_ڈیٹا_get_format(sel)	((sel)->format)
#  define gtk_selection_ڈیٹا_get_لمبائی(sel)	((sel)->لمبائی)
#  define gtk_adjustment_سیٹ_زیریں(adj, کم) \
    کرو { (adj)->زیریں = کم; } جب (0)
#  define gtk_adjustment_سیٹ_بالائی(adj, اوپر) \
    کرو { (adj)->بالائی_2 = اوپر; } جب (0)
#  define gtk_adjustment_سیٹ_صفحہ_سائز(adj, سائز) \
    کرو { (adj)->صفحہ_سائز = سائز; } جب (0)
#  define gtk_adjustment_سیٹ_صفحہ_increment(adj, inc) \
    کرو { (adj)->صفحہ_increment = inc; } جب (0)
#  define gtk_adjustment_سیٹ_قدم_increment(adj, inc) \
    کرو { (adj)->قدم_increment = inc; } جب (0)
# endif
# اگر !GTK_CHECK_نسخہ(2,16,0)
#  define gtk_selection_ڈیٹا_get_selection(sel)	((sel)->selection)
# endif
# اگر !GTK_CHECK_نسخہ(2,18,0)
#  define gtk_widget_get_تخصیص(wid, alloc) \
    کرو { *(alloc) = (wid)->تخصیص; } جب (0)
#  define gtk_widget_سیٹ_تخصیص(wid, alloc) \
    کرو { (wid)->تخصیص = *(alloc); } جب (0)
#  define gtk_widget_get_has_کھڑکی(wid)	!GTK_WIDGET_NO_کھڑکی(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_سیٹ_کھڑکی(wid, win) \
    کرو { (wid)->کھڑکی_2 = (win); } جب (0)
#  define gtk_widget_سیٹ_can_default(wid, can) \
    کرو { اگر (can) \
	    { GTK_WIDGET_سیٹ_FLAGS(wid, GTK_CAN_DEFAULT); } \
	ورنہ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } جب (0)
#  define gtk_widget_سیٹ_can_focus(wid, can) \
    کرو { اگر (can) \
	    { GTK_WIDGET_سیٹ_FLAGS(wid, GTK_CAN_FOCUS); } \
	ورنہ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } جب (0)
#  define gtk_widget_سیٹ_visible(wid, vis) \
    کرو { اگر (vis) \
	    { gtk_widget_show(wid); } \
	ورنہ \
	    { gtk_widget_hide(wid); } } جب (0)
# endif
# اگر !GTK_CHECK_نسخہ(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_سیٹ_mapped(wid, map) \
    کرو { اگر (map) \
	    { GTK_WIDGET_سیٹ_FLAGS(wid, GTK_MAPPED); } \
	ورنہ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } جب (0)
#  define gtk_widget_سیٹ_realized(wid, rea) \
    کرو { اگر (rea) \
	    { GTK_WIDGET_سیٹ_FLAGS(wid, GTK_REALIZED); } \
	ورنہ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } جب (0)
# endif
#اگر_ختم

#اگر defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# اگر GTK_CHECK_نسخہ(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ FEAT_NETBEANS_INTG
# undef NBDEBUG
#اگر_ختم
#اگر_تعریف_شدہ NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#ورنہ
# define nbdebug(a)
#اگر_ختم

#اگر_تعریف_شدہ اندر_PERL_FILE
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
# اگر (defined(MACOS_X) && !defined(MAC_OS_X_نسخہ_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#اگر_ختم

// values for vim_handle_signal() that are not a signal
#تعریف_کرو SIGNAL_بلاک	(-1)
#تعریف_کرو SIGNAL_UNBLOCK  (-2)
#اگر !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#اگر_ختم

// flags for skip_vimgrep_pat()
#تعریف_کرو VGR_GLOBAL	1
#تعریف_کرو VGR_NOJUMP	2
#تعریف_کرو VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#تعریف_کرو BAD_REPLACE	'?'	// replace it with '?' (default)
#تعریف_کرو BAD_KEEP	(-1)	// leave it
#تعریف_کرو BAD_DROP	(-2)	// erase it

// last argument for do_source()
#تعریف_کرو DOSO_NONE	0
#تعریف_کرو DOSO_VIMRC	1	// loading vimrc file
#تعریف_کرو DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#تعریف_کرو VIF_WANT_INFO	    1	// load non-mark info
#تعریف_کرو VIF_WANT_MARKS	    2	// load file marks
#تعریف_کرو VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#تعریف_کرو VIF_FORCEIT	    8	// overwrite info already read
#تعریف_کرو VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#تعریف_کرو BFA_DEL		 1	// buffer is going to be deleted
#تعریف_کرو BFA_WIPE	 2	// buffer is going to be wiped out
#تعریف_کرو BFA_KEEP_UNDO	 4	// do not free undo information
#تعریف_کرو BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#تعریف_کرو MSCR_نیچے_کو	0	// DOWN must be FALSE
#تعریف_کرو MSCR_اوپر		1
#تعریف_کرو MSCR_بایاں	(-1)
#تعریف_کرو MSCR_دایاں	(-2)

#تعریف_کرو KEYLEN_PART_کلید (-1)	// keylen value for incomplete key-code
#تعریف_کرو KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#تعریف_کرو KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#تعریف_کرو FILEINFO_OK	     0
#تعریف_کرو FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#تعریف_کرو FILEINFO_READ_FAIL   2	// CreateFile() failed
#تعریف_کرو FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#تعریف_کرو SOPT_BOOL	0x01	// Boolean option
#تعریف_کرو SOPT_عدد	0x02	// Number option
#تعریف_کرو SOPT_STRING	0x04	// String option
#تعریف_کرو SOPT_GLOBAL	0x08	// Option has global value
#تعریف_کرو SOPT_WIN	0x10	// Option has window-local value
#تعریف_کرو SOPT_BUF	0x20	// Option has buffer-local value
#تعریف_کرو SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#تعریف_کرو SREQ_GLOBAL	0	// Request global option
#تعریف_کرو SREQ_WIN	1	// Request window-local option
#تعریف_کرو SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#تعریف_کرو GREG_NO_اظہاریہ	1	// Do not allow expression register
#تعریف_کرو GREG_اظہاریہ_SRC	2	// Return expression itself for "=" register
#تعریف_کرو GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#تعریف_کرو AUTOLOAD_CHAR '#'

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
# define MAX_OPEN_CHANNELS 10
#ورنہ
# define MAX_OPEN_CHANNELS 0
#اگر_ختم

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#ورنہ
# define MAX_CLIENT_CHANNELS 0
#اگر_ختم

#اگر defined(MSWIN)
# define MAX_NAMED_PIPE_سائز 65535
#اگر_ختم

// Options for json_encode() and json_decode.
#تعریف_کرو JSON_JS		1   // use JS instead of JSON
#تعریف_کرو JSON_NO_NONE	2   // v:none item not allowed
#تعریف_کرو JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#تعریف_کرو DIP_ALL	    0x01	// all matches, not just the first one
#تعریف_کرو DIP_DIR	    0x02	// find directories instead of files.
#تعریف_کرو DIP_ERR	    0x04	// give an error message when none found.
#تعریف_کرو DIP_آغاز   0x08	// also use "start" directory in 'packpath'
#تعریف_کرو DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#تعریف_کرو DIP_NORTP   0x20	// do not use 'runtimepath'
#تعریف_کرو DIP_NOAFTER 0x40	// skip "after" directories
#تعریف_کرو DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#تعریف_کرو سب_سے_نیچا_WIN_ID 1000

// Used by the garbage collector.
#تعریف_کرو COPYID_INC 2
#تعریف_کرو COPYID_نقاب (~0x1)

// Values for trans_function_name() argument:
#تعریف_کرو TFN_INT		0x01	// internal function name OK
#تعریف_کرو TFN_QUIET	0x02	// no error messages
#تعریف_کرو TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#تعریف_کرو TFN_NO_DEREF	0x08	// do not dereference a Funcref
#تعریف_کرو TFN_READ_ONLY	0x10	// will not change the var
#تعریف_کرو TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#تعریف_کرو TFN_COMPILING	0x40	// only used for GLV_COMPILING
#تعریف_کرو TFN_NEW_FUNC	0x80	// defining a new function
#تعریف_کرو TFN_تفویض_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#تعریف_کرو TFN_اندر_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#تعریف_کرو GLV_QUIET	TFN_QUIET	// no error messages
#تعریف_کرو GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#تعریف_کرو GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#تعریف_کرو GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#تعریف_کرو GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#تعریف_کرو GLV_تفویض_WITH_OP TFN_تفویض_WITH_OP // assignment with operator
#تعریف_کرو GLV_PREFER_FUNC	0x10000		// prefer function above variable
#تعریف_کرو GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#تعریف_کرو DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#تعریف_کرو FLEN_FIXED 40

// flags for find_name_end()
#تعریف_کرو FNE_INCL_BR	1	// include [] in name
#تعریف_کرو FNE_CHECK_آغاز	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#اگر (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#اگر_ختم

// stat macros
#اگر_غیر_تعریف_شدہ S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# ورنہ
#  define S_ISDIR(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# ورنہ
#  define S_ISREG(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# ورنہ
#  define S_ISBLK(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# ورنہ
#  define S_ISSOCK(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# ورنہ
#  define S_ISFIFO(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# ورنہ
#  define S_ISCHR(m)	0
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# ورنہ
#  define S_ISLNK(m)	0
# endif
#اگر_ختم

#اگر defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وقت_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), تہی)
# define ELAPSED_FUNC(v) elapsed(&(v))
قسمتعریف ساخت timeval elapsed_T;
طویل elapsed(ساخت timeval *آغاز_tv);
#وگرنہ_اگر defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_گنتی()
# define ELAPSED_FUNC(v) elapsed(v)
قسمتعریف DWORD elapsed_T;
# ifndef PROTO
طویل elapsed(DWORD آغاز_tick);
# endif
#اگر_ختم

// Replacement for nchar used by nv_replace().
#تعریف_کرو REPLACE_CR_NCHAR    (-1)
#تعریف_کرو REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#تعریف_کرو رکن_آغاز_NOJOB	1
#تعریف_کرو رکن_آغاز_FORCEIT	2
#تعریف_کرو رکن_آغاز_SYSTEM	4

// Used for icon/title save and restore.
#تعریف_کرو SAVE_RESTORE_TITLE	1
#تعریف_کرو SAVE_RESTORE_ICON	2
#تعریف_کرو SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#تعریف_کرو APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#تعریف_کرو APC_SUBSTITUTE		2   // text is replaced, not inserted
#تعریف_کرو APC_INDENT		4   // changing indent

#تعریف_کرو CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#تعریف_کرو REPTERM_FROM_PART	1
#تعریف_کرو REPTERM_DO_LT		2
#تعریف_کرو REPTERM_SPECIAL		4
#تعریف_کرو REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#تعریف_کرو FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#تعریف_کرو FSK_KEEP_X_کلید	0x02	// don't translate xHome to Home key
#تعریف_کرو FSK_اندر_STRING	0x04	// TRUE in string, double quote is escaped
#تعریف_کرو FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#تعریف_کرو FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#تعریف_کرو READDIR_ترتیب_دو_NONE	0  // do not sort
#تعریف_کرو READDIR_ترتیب_دو_BYTE	1  // sort by byte order (strcmp), default
#تعریف_کرو READDIR_ترتیب_دو_IC		2  // sort ignoring case (strcasecmp)
#تعریف_کرو READDIR_ترتیب_دو_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#تعریف_کرو MCH_DELAY_IGNOREINPUT	1
#تعریف_کرو MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#تعریف_کرو EVAL_VAR_VERBOSE	1   // may give error message
#تعریف_کرو EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#تعریف_کرو EVAL_VAR_IMPORT		4   // may return special variable for import
#تعریف_کرو EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#تعریف_کرو FUZZY_مطابقت_MAX_LEN	1024    // max characters that can be matched
#تعریف_کرو FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#تعریف_کرو ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#تعریف_کرو UC_بفر	1	// -buffer: local to current buffer
#تعریف_کرو UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#تعریف_کرو UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#تعریف_کرو VSE_NONE	0
#تعریف_کرو VSE_SHELL	1	// escape for a shell command
#تعریف_کرو VSE_بفر	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#تعریف_کرو FFED_IS_GLOBAL	1	// "g:" was used
#تعریف_کرو FFED_NO_GLOBAL	2	// only check for script-local functions

#تعریف_کرو MAX_LSHIFT_بٹ (varnumber_T)((حجم(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#تعریف_کرو CF_CLASS	1	// inside a class
#تعریف_کرو CF_INTERFACE	2	// inside an interface
#تعریف_کرو CF_ABSTRACT_طریقہ	4	// inside an abstract class

// Flags used by getvcol()
#تعریف_کرو GETVCOL_انتہا_EXCL_LBR	1
#تعریف_کرو GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#تعریف_کرو GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#اگر_تعریف_شدہ VMS
# define راستہ_ESC_WILDCARDS	"*?%"
#ورنہ
# ifdef MSWIN
#  define راستہ_ESC_WILDCARDS	"*?["
# ورنہ
#  define راستہ_ESC_WILDCARDS	"*?[{"
# endif
#اگر_ختم

#اگر_ختم // VIM__H
