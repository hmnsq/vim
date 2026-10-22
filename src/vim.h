/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIM__H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM__H

#ਅਨ੍ਤਰ੍ਭਾਵ "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#ਜੇ defined(WIN32) || defined(_WIN32)
# define MSWIN
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
# include <io.h>
#ਜੇ_ਅੰਤ

// ============ the header file puzzle: order matters =========

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H	// GNU autoconf (or something else) was here
# ifdef VMS
#  include "config.h"   /* Rely on /INCLUDE to find it. */
# ਨਹੀਂਤਾਂ
#  include "auto/config.h"
# endif /* def VMS [else] */
# define HAVE_PATHDEF

/*
 * Check if configure correctly managed to find sizeof(int).  If this failed,
 * it becomes zero.  This is likely a problem of not being able to run the
 * test program.  Other items from configure may also be wrong then!
 */
# ਜੇ (VIM_SIZEOF_INT == 0)
#  error configure did not ਲੜੀਵਾਰ properly.  Check ਸਵੈਚਾਲਿਤ/config.ਲਘੂਗਣਕ.
# endif

# ਜੇ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_ਸਰੋਤ
#   define XOPEN_ਸਰੋਤ    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_ਸਰੋਤ
#    define BSD_ਸਰੋਤ 1
#   endif
#   ifndef SVID_ਸਰੋਤ
#    define SVID_ਸਰੋਤ 1
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
# ਜੇ defined(__CYGWIN32__) && defined(HAVE_FCHDIR)
#  undef HAVE_FCHDIR
# endif

// We may need to define the uint32_t on non-Unix system, but using the same
// identifier causes conflicts.  Therefore use UINT32_T.
# define UINT32_TYPEDEF uint32_t
#ਜੇ_ਅੰਤ

// for INT_MAX, LONG_MAX et al.
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

#ਜੇ !defined(UINT32_TYPEDEF)
# ਜੇ defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# ਨਹੀਂਤਾਂ
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
# endif
#ਜੇ_ਅੰਤ

// C89 does not define SIZE_MAX
#ਜੇ defined(__hpux) || defined(VMS)
# ifndef ਮਾਪ_MAX
#  define ਮਾਪ_MAX ((size_t)(-1))
# endif
#ਜੇ_ਅੰਤ

// user ID of root is usually zero, but not for everybody
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __TANDEM
# ifndef TANDEM_ਸਰੋਤ
#  define TANDEM_ਸਰੋਤ
# endif
# include <floss.h>
# define ਜੜ੍ਹ_UID 65535
# define OLDXAW
# ਜੇ (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#ਨਹੀਂਤਾਂ
# define ਜੜ੍ਹ_UID 0
#ਜੇ_ਅੰਤ

/* Include MAC_OS_X_VERSION_* macros */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#ਜੇ_ਅੰਤ

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#ਜੇ defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#ਜੇ_ਅੰਤ
// Unless made through the Makefile enforce GUI on Mac
#ਜੇ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# ਜੇ !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#ਜੇ_ਅੰਤ

// Check support for rendering options
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ਜੇ defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#ਜੇ_ਅੰਤ

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# define VIM_SIZEOF_INT 4
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  ਨਹੀਂਤਾਂ
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#ਜੇ_ਅੰਤ

#ਜੇ VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only ਚੱਲਦਾ with 32 ਦੋਆਧਾਰੀ_ਅੰਕ ਪੂਰਨ or larger
#ਜੇ_ਅੰਤ

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "feature.h"

#ਜੇ defined(MACOS_X_DARWIN)
# ਜੇ defined(FEAT_ਸਾਧਾਰਨ) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# ਜੇ defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_ਸੰਸਕਰਣ_MIN_REQUIRED) && MAC_OS_X_ਸੰਸਕਰਣ_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# ਜੇ defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#ਜੇ_ਅੰਤ

// +x11 is only enabled when it's both available and wanted.
#ਜੇ defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NO_X11_INCLUDES
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
#ਜੇ_ਅੰਤ

// The Mac conversion stuff doesn't work under X11.
#ਜੇ defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#ਜੇ_ਅੰਤ

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIMPACKAGE
# define VIMPACKAGE	"vim"
#ਜੇ_ਅੰਤ

/*
 * Find out if function definitions should include argument types
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AZTEC_C
# include <functions.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SASC
# include <clib/exec_protos.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DCC
# include <clib/exec_protos.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#ਜੇ_ਅੰਤ

#ਜੇ (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#ਜਾਂਜੇ !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) ਸੰਕੇਤ(signum, sighandler)
#ਜੇ_ਅੰਤ

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਅਣਵਰਤਿਆ
# ਜੇ defined(HAVE_ATTRIBUTE_ਅਣਵਰਤਿਆ) || defined(__MINGW32__)
#  define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
# ਨਹੀਂਤਾਂ
#  ਜੇ defined __has_attribute
#   ਜੇ __has_attribute(ਅਣਵਰਤਿਆ_2)
#    define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#   endif
#  endif
# endif
# ifndef ਅਣਵਰਤਿਆ
#  define ਅਣਵਰਤਿਆ
# endif
#ਜੇ_ਅੰਤ

// Used to check for "sun", "__sun" is used by newer compilers.
#ਜੇ defined(__sun)
# define SUN_SYSTEM
#ਜੇ_ਅੰਤ

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#ਜੇ !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
# ifdef AMIGA
#  include "os_amiga.h"
# endif

# ifdef MSWIN
#  include "os_win32.h"
# endif

# ਜੇ defined(MACOS_X)
#  include "os_mac.h"
# endif

# ifdef __QNX__
#  include "os_qnx.h"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ X_LOCALE
# include <X11/Xlocale.h>
#ਨਹੀਂਤਾਂ
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#ਜੇ_ਅੰਤ

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# ਨਹੀਂਤਾਂ
#  define MAXPATHL  256
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
# define ਰਸਤਾ_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define ਬਫਰ_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#ਨਹੀਂਤਾਂ
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define ਰਸਤਾ_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# ਨਹੀਂਤਾਂ
#  define ਰਸਤਾ_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define ਬਫਰ_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#ਜੇ_ਅੰਤ

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NUMBUFLEN 65

// flags for vim_str2nr()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_BIN  0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_OCT  0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_HEX  0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_FORCE 0x80   // only when ONE of the above is used

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ	char_u;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ	short_u;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN64
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ __int64	long_u;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ		long_u;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ		 ਲੰਮਾ		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#ਜੇ defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#ਜੇ_ਅੰਤ

/*
 * We use 64-bit file functions here, if available.  E.g. ftello() returns
 * off_t instead of long, which helps if long is 32 bit and off_t is 64 bit.
 * We assume that when fseeko() is available then ftello() is too.
 * Note that Windows has different function names.
 */
#ਜੇ defined(MSWIN) && !defined(PROTO)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ __int64 off_T;
# ifdef __MINGW32__
#  define vim_lseek lseek64
#  define vim_fseek fseeko64
#  define vim_ftell ftello64
# ਨਹੀਂਤਾਂ
#  define vim_lseek _lseeki64
#  define vim_fseek _fseeki64
#  define vim_ftell _ftelli64
# endif
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ off_t off_T;
# ifdef HAVE_FSEEKO
#  define vim_lseek lseek
#  define vim_ftell ftello
#  define vim_fseek fseeko
# ਨਹੀਂਤਾਂ
#  define vim_lseek lseek
#  define vim_ftell ftell
#  define vim_fseek(a, b, c)	fseek(a, (ਲੰਮਾ)b, c)
# endif
#ਜੇ_ਅੰਤ

/*
 * The characters and attributes cached for the screen.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ char_u schar_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ sattr_T;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#ਜੇ defined(VMS) && defined(VAX)
// unsupported compiler
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ      vimlong_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ ਲੰਮਾ vimlong_T;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "ascii.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "keymap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "termdefs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "macros.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DCC
# include <sys/stat.h>
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN)
# include <sys/stat.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#ਜੇ_ਅੰਤ

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# ਜੇ defined(HAVE_ਲੜੀਆਂ_H) && !defined(NO_ਲੜੀਆਂ_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#ਜੇ_ਅੰਤ // NON-UNIX

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STDINT_H
# include <stdint.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_INTTYPES_H
# include <inttypes.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_WCTYPE_H
# include <wctype.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
// older compilers do not define va_copy
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ va_copy
# define va_copy(dst, src)	((dst) = (src))
#ਜੇ_ਅੰਤ

// for offsetof()
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>

#ਜੇ defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_ਸਮਾਂ_H) || defined(SYS_SELECT_WITH_SYS_ਸਮਾਂ))
# include <sys/select.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SODIUM
# include <sodium.h>
#ਜੇ_ਅੰਤ

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#ਜੇ_ਅੰਤ

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
ਬਾਹਰੀ ਅੱਖਰ *(*dyn_libintl_gettext)(ਅਚਲ ਅੱਖਰ *msgid);
ਬਾਹਰੀ ਅੱਖਰ *(*dyn_libintl_ngettext)(ਅਚਲ ਅੱਖਰ *msgid, ਅਚਲ ਅੱਖਰ *msgid_plural, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n);
ਬਾਹਰੀ ਅੱਖਰ *(*dyn_libintl_bindtextdomain)(ਅਚਲ ਅੱਖਰ *domainname, ਅਚਲ ਅੱਖਰ *dirname);
ਬਾਹਰੀ ਅੱਖਰ *(*dyn_libintl_bind_textdomain_codeset)(ਅਚਲ ਅੱਖਰ *domainname, ਅਚਲ ਅੱਖਰ *codeset);
ਬਾਹਰੀ ਅੱਖਰ *(*dyn_libintl_textdomain)(ਅਚਲ ਅੱਖਰ *domainname);
ਬਾਹਰੀ ਪੂਰਨ (*dyn_libintl_wputenv)(ਅਚਲ wchar_t *envstring);
ਬਾਹਰੀ ਪੂਰਨ *dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr;
#ਜੇ_ਅੰਤ


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
#  define _(x) (*dyn_libintl_gettext)((ਅੱਖਰ *)(x))
#  define NGETTEXT(x, xs, n) (*dyn_libintl_ngettext)((ਅੱਖਰ *)(x), (ਅੱਖਰ *)(xs), (n))
#  define N_(x) x
#  define bindtextdomain(domain, dir) (*dyn_libintl_bindtextdomain)((domain), (dir))
#  define bind_textdomain_codeset(domain, codeset) (*dyn_libintl_bind_textdomain_codeset)((domain), (codeset))
#  ਜੇ !defined(HAVE_BIND_TEXTDOMAIN_CODESET)
#   define HAVE_BIND_TEXTDOMAIN_CODESET 1
#  endif
#  define textdomain(domain) (*dyn_libintl_textdomain)(domain)
#  define libintl_wputenv(envstring) (*dyn_libintl_wputenv)(envstring)
# ਨਹੀਂਤਾਂ
#  include <libintl.h>
#  define _(x) gettext((ਅੱਖਰ *)(x))
#  define NGETTEXT(x, xs, n) ngettext((x), (xs), (n))
#  ifdef gettext_noop
#   define N_(x) gettext_noop(x)
#  ਨਹੀਂਤਾਂ
#   define N_(x) x
#  endif
# endif
#ਨਹੀਂਤਾਂ
# define _(x) ((ਅੱਖਰ *)(x))
# define NGETTEXT(x, xs, n) (((n) == 1) ? (ਅੱਖਰ *)(x) : (ਅੱਖਰ *)(xs))
# define N_(x) x
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# define bindtextdomain(x, y) ""
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# define bind_textdomain_codeset(x, y) //ਖਾਲੀ_ਥਾਵਾਂ
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_INVERTED		20  // redisplay inverted part that changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_INVERTED_ALL	25  // redisplay whole inverted part
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_REDRAW_ਸਿਖਰ		30  // display first w_upd_rows screen lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_NOT_VALID		40  // buffer needs complete redraw
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SLF_RIGHTLEFT	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SLF_POPUP	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SLF_INC_VCOL	4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_FILLER_CHAR '<'  // character used when a double-width character
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_WROW	0x01	// w_wrow (window row) is valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_WCOL	0x02	// w_wcol (window col) is valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_CROW	0x10	// w_cline_row is valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_BOTLINE_AP 0x40	// w_botline is approximated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_IS_POPUP	0x01	// this is a popup window
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_HIDDEN	0x02	// popup is not displayed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_ON_CMDLINE	0x10	// popup overlaps command line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_DRAG	0x20	// popup can be moved by dragging border
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_RESIZE	0x80	// popup can be resized by dragging
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_MAPPING	0x100	// mapping keys
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_INFO	0x200	// used for info of popup menu
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_INFO_MENU	0x400	// align info popup with popup menu
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_POSINVERT	0x800	// vertical position can be inverted
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_ਸਾਧਾਰਨ		0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_INVERSE		0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_BOLD			0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_ITALIC		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_UNDERLINE		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_UNDERCURL		0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_UNDERDOUBLE		0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_UNDERDOTTED		0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_UNDERDASHED		0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_STANDOUT		0x100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_NOCOMBINE		0x200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_STRIKETHROUGH	0x400
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_ALL			0x7ff

// special attribute addition: Put message in history
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_ਸਾਧਾਰਨ	0x01	// Normal mode, command expected
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_VISUAL	0x02	// Visual mode - use get_real_state()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_CMDLINE	0x08	// Editing the command line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_ਪਾਓ	0x10	// Insert mode, also for Replace mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_SELECT	0x40	// Select mode, use get_real_state()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_TERMINAL	0x80	// Terminal mode

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_ALL_MODES	0xff    // all mode bits used for mapping

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPLACE_FLAG	0x100	// Replace mode flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_REPLACE	(REPLACE_FLAG | ਬਹੁਲਕ_ਪਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | ਬਹੁਲਕ_ਪਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_LREPLACE	(REPLACE_FLAG | ਬਹੁਲਕ_LANGMAP)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY (0x1000 | ਬਹੁਲਕ_ਸਾਧਾਰਨ)
				// Normal mode, busy with a command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_HITRETURN	(0x2000 | ਬਹੁਲਕ_ਸਾਧਾਰਨ)
				// waiting for return or command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_ASKMORE	0x3000	// Asking if you want --more--
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_SETWSIZE	0x4000	// window size has changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_EXTERNCMD	0x5000	// executing an external command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_SHOWMATCH	(0x6000 | ਬਹੁਲਕ_ਪਾਓ) // show matching paren
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_CONFIRM	0x7000	// ":confirm" prompt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_ALL	0xffff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_MAX_ਲੰਮਾਈ	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORWARD			1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKWARD		(-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORWARD_FILE		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKWARD_FILE		(-3)

// return values for functions
#ਜੇ !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAIL			0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_RECOVERED	0x01	// buffer has been recovered
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_NEW		0x10	// file didn't exist when editing started
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_NEW_W	0x20	// Warned for BF_NEW and file created
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_READERR	0x40	// got errors while reading the file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_DUMMY	0x80	// dummy buffer, only used internally
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_PRESERVED	0x100	// ":preserve" was used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_SYN_ਸੈੱਟ	0x200	// 'syntax' option was set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BF_WRITE_ਨਕਾਬ	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_UNSUCCESSFUL	(-2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_OK		(-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_NOTHING		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_COMMANDS		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILES		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_DIRECTORIES	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SETTINGS		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_BOOL_SETTINGS	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ਟੈਗ_ਸੂਚੀ		6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_OLD_SETTING	7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_HELP		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_BUFFERS		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_EVENTS		10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_MENUS		11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SYNTAX		12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_HIGHLIGHT	13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_AUGROUP		14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_VARS	15
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_MAPPINGS		16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES	17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FUNCTIONS	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_FUNC	19
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ਵਿਅੰਜਕ	20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_MENUNAMES	21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_COMMANDS	22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_CMD_FLAGS	23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_NARGS	24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_COMPLETE	25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ENV_VARS		26
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_LANGUAGE		27
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_COLORS		28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_COMPILER		29
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_DEFINED	30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_LIST	31
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SHELLCMD		32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_CSCOPE		33
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SIGN		34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_PROFILE		35
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_BEHAVE		36
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILETYPE		37
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ	38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_OWNSYNTAX	39
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_LOCALES		40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_HISTORY		41
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER		42
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SYNTIME		43
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_ADDR_TYPE	44
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_PACKADD		45
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_MESSAGES		46
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_MAPCLEAR		47
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ARGLIST		48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_DIFF_BUFFERS	49
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_DISASSEMBLE	50
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_BREAKPOINT	51
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SCRIPTNAMES	52
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_RUNTIME		53
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_STRING_SETTING	54
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SETTING_ਘਟਾਓ	55
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ARGOPT		56
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_TERMINALOPT	57
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_KEYMAP		58
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_DIRS_ਵਿੱਚ_CDPATH	59
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_SHELLCMDLINE	60
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FINDFUNC		61
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_HIGHLIGHT_ਸਮੂਹ  62
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILETYPECMD	63
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF	64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_RETAB		65
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXMODE_ਸਾਧਾਰਨ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_FREE		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_EXPAND_FREE	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_EXPAND_KEEP	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ਅਗਲਾ		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_PREV		5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ALL		6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_LONGEST		7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ALL_KEEP		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_CANCEL		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_APPLY		10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_PAGEUP		11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_PAGEDOWN		12

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_LIST_NOTFOUND	    0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_HOME_REPLACE	    0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_USE_NL		    0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_NO_BEEP		    0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ਜੋੜੋ_SLASH		    0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_KEEP_ALL		    0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_SILENT		    0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ESCAPE		    0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ICASE		    0x100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_ALLLINKS		    0x200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_IGNORE_COMPLETESLASH   0x400
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_NOERROR		    0x800  // sets EW_NOERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_BUFLASTUSED	    0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF_DIFF_FILTER		    0x2000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_NOSELECT		    0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_MAY_EXPAND_ਨਮੂਨਾ	    0x8000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_DIR		0x01	// include directory names
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_FILE		0x02	// include file names
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_NOTFOUND	0x04	// include not found names
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_ADDSLASH	0x08	// append slash to directory name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_KEEPALL	0x10	// keep all matches
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_SILENT	0x20	// don't print "1 returned" from shell
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_EXEC		0x40	// executable files
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_ਰਸਤਾ		0x80	// search in 'path' too
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_ICASE	0x100	// ignore case
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_NOERROR	0x200	// no error for bad regexp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_NOTWILD	0x400	// add match with literal name if exists
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_ALLLINKS	0x1000	// also links not pointing to existing file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_DODOT	0x4000	// also files starting with a dot
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_EMPTYOK	0x8000	// no matches is not an error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_NOTENV	0x10000	// do not expand environment variables
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FINDFILE_FILE	0	// only files
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FINDFILE_DIR	1	// only directories
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FINDFILE_BOTH	2	// files and directories

#ਜੇ defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_ਚੌੜਾਈ())
# define TPL_LCOL()			tabpanel_leftcol()
#ਨਹੀਂਤਾਂ
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_ਚੌੜਾਈ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_ਉਚਾਈ)
#ਨਹੀਂਤਾਂ
# define W_WINROW(wp)	(wp->w_winrow)
#ਜੇ_ਅੰਤ

// Values for the find_pattern_in_path() function args 'type' and 'action':
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_ANY	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_DEFINE	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_ਰਸਤਾ	3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACTION_SHOW	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACTION_GOTO	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACTION_ਵੰਡੋ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACTION_SHOW_ALL	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACTION_EXPAND	5

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_ਹਾਲਤਾਂ	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_ਇੱਥੇ	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_ਮੇਲ	0x40	// use match ID instead of item ID
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
#ਜੇ_ਅੰਤ

// Values for 'options' argument in do_search() and searchit()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_REV    0x01  // go in reverse of previous dir.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ECHO   0x02  // echo the search command and handle options
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ਸੁਨੇਹਾ    0x0c  // give messages (yes, it's not 0x04)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_NFMSG  0x08  // give all messages except not found
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_OPT    0x10  // interpret optional flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_HIS    0x20  // put search pattern in history
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ਸਿਰਾ    0x40  // put cursor at end of match
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_NOOF   0x80  // don't add offset to position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ਸ਼ੁਰੂ 0x100  // start search without col offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_MARK  0x200  // set previous context mark
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_KEEP  0x400  // keep previous search pattern
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_PEEK  0x800  // peek for typed char, cancel search
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_ਖਾਨਾ  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_IDENT	1	// find identifier (word)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_STRING	2	// find any string (WORD)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_EVAL	4	// include "->", "[]" and "."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_MESS	1	// give error message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_EXP	2	// expand to path
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_HYP	4	// check for hypertext link
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_INCL	8	// apply 'includeexpr'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETF_SETMARK	0x01	// set pcmark before jumping
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETF_ALT	0x02	// jumping to alternate file (not buf num)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_ERROR	    1	// normal error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_NOT_WRITTEN 2	// "not written" error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_SAME_FILE   0	// success, same file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_OPEN_OTHER (-1)	// success, opened another file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_ਅਣਵਰਤਿਆ	    8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_CURBUF	1	// may re-use curbuf for new buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_LISTED	2	// put new buffer in buffer list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_DUMMY	4	// allocating dummy buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_NEW		8	// create a new buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_NOOPT	16	// don't copy options to existing buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_DUMMY_OK	32	// also find an existing dummy buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_REUSE	64	// may re-use number from buf_reuse
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੰਜੀ_OPEN_FORW	0x101
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੰਜੀ_OPEN_ਪਿੱਠ	0x102
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੰਜੀ_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAPTYPE_MAP		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAPTYPE_UNMAP		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAPTYPE_NOREMAP		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REMAP_YES	0	// allow remapping
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REMAP_NONE	(-1)	// no remapping
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REMAP_SCRIPT	(-2)	// remap script-local mappings only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_FILTER	1	// filtering text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_EXPAND	2	// expanding wildcards
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_COOKED	4	// set term to cooked mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_DOOUT	8	// redirecting output
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_SILENT	16	// don't print error returned by command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_READ	32	// read lines and insert into buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਗੰਢ_ਸਾਧਾਰਨ	0	// file or directory, check with mch_isdir()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਗੰਢ_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਗੰਢ_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_NEW	0x01	// read a file into a new buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_FILTER	0x02	// read filter output
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_STDIN	0x04	// read from stdin
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_ਬਫਰ	0x08	// read from curbuf (converting stdin)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_DUMMY	0x10	// reading into a dummy buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_KEEP_UNDO	0x20	// keep undo info
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_FIFO	0x40	// read from fifo or socket
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENT_ਸੈੱਟ	1	// set indent
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENT_INC	2	// increase indent
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FM_BACKWARD	0x01	// search backwards
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FM_FORWARD	0x02	// search forwards
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FM_BLOCKSTOP	0x04	// stop at start/end of block
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_GOTO	0	// go to specified buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_ਵੰਡੋ	1	// split window and go to specified buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_UNLOAD	2	// unload specified buffer(s)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_DEL	3	// delete specified buffer(s) from buflist
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_WIPE	4	// delete specified buffer(s) really
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_CURRENT	0	// "count" buffer from current buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_ਪਹਿਲਾ	1	// "count" buffer from first buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_ਆਖਰੀ	2	// "count" buffer from last buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_FORCEIT	1	// :cmd!
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_NOPOPUP	2	// skip popup window buffers
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_ਖੋਜ	0	// save/use pat in/from search_pattern
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_SUBST	1	// save/use pat in/from subst_pattern
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_BOTH		2	// save pat in both patterns
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_ਆਖਰੀ		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_MAGIC	1	// 'magic' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_STRING	2	// match in string instead of buffer text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_STRICT	4	// don't allow [abc] without ]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_AUTO		8	// automatic engine selection

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_ਸੈੱਟ	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_ਸੈੱਟ | REX_USE)
#ਜੇ_ਅੰਤ

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPC_SAME	1	// both exist and are the same file.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPC_DIFF	2	// both exist and are different files.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPC_NOTX	4	// both don't exist.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPC_DIFFX	6	// one of them doesn't exist.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_HIDE	0x01	// don't free the current buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_ਸੈੱਟ_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_OLDBUF	0x04	// use existing buffer if it exists
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_FORCEIT	0x08	// ! used in Ex command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_ਆਖਰੀ	((linenr_T)-1)	// use last position in all files
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_VERBOSE	0x01	// included command in error message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BL_WHITE	1	// cursor on first non-white in the line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BL_SOL		2	// use 'sol' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MFS_ALL		1	// also sync blocks with negative numbers
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MFS_STOP	2	// stop syncing when a character is available
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MFS_FLUSH	4	// flushed file to disk
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BCO_ENTER	1	// going to enter the buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BCO_ALWAYS	2	// always copy the options
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_FIXINDENT	1	// make indent look nice
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_CURSEND	2	// leave cursor after end of new text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_CURSLINE	4	// leave cursor on last line of new text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_ਸਤਰ	8	// put register as lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_ਸਤਰ_ਵੰਡੋ	16	// split line for linewise register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_ਸਤਰ_FORWARD 32	// put linewise register below Visual sel.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUT_ਖੰਡ_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIN_ਬਦਲਿਆ	1	// call changed_bytes() when line changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIN_ਪਾਓ	2	// insert indent before existing text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INSCHAR_FORMAT	1	// force formatting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INSCHAR_DO_COM	2	// format comments
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INSCHAR_CTRLV	4	// char typed just after CTRL-V
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_DO_COM		0x02    // format comments
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_MARKFIX	0x08    // fix mark positions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_FORMAT		0x20    // formatting long comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_CMD	0	// colon commands
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_ਖੋਜ	1	// search commands
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_ਵਿਅੰਜਕ	2	// expressions (from entering = register)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_ਨਿਵੇਸ਼	3	// input() lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_DEBUG	4	// debug commands
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIST_ਗਿਣਤੀ	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BARTYPE_ਸੰਸਕਰਣ 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BARTYPE_HISTORY 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BARTYPE_ਰਜਿਸਟਰ 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BARTYPE_MARK 4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMINFO_ਸੰਸਕਰਣ 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMINFO_ਸੰਸਕਰਣ_WITH_HISTORY 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMINFO_ਸੰਸਕਰਣ_WITH_REGISTERS 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMINFO_ਸੰਸਕਰਣ_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_ਟੈਗ		1	// jump to newer position or same tag again
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_ਕੱਢੋ		2	// jump to older position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_ਅਗਲਾ		3	// jump to next match of same tag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_PREV		4	// jump to previous match of same tag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_ਪਹਿਲਾ	5	// jump to first match of same tag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_ਆਖਰੀ		6	// jump to first match of same tag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_SELECT	7	// jump to selection from list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_HELP		8	// like DT_TAG, but no wildcards
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_JUMP		9	// jump to new tag or selection from list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_CSCOPE	10	// cscope find command (like tjump)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_LTAG		11	// tag using location list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_HELP	1	// only search for help tags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_ਨਾਂ	2	// only return name of tag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	ਟੈਗ_REGEXP	4	// use tag pattern as regexp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	ਟੈਗ_NOIC	8	// don't always ignore case
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
# define ਟੈਗ_CSCOPE	16	// cscope tag
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_VERBOSE	32	// message verbosity
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_INS_COMP	64	// Currently doing insert completion
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_KEEP_LANG	128	// keep current language
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_NO_TAGFUNC	256	// do not use 'tagfunc'

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_GENERIC	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ERROR	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_WARNING	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_INFO	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_QUESTION	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ਆਖਰੀ_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_YES		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_NO		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_CANCEL	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ALL		5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_ਗੁੰਜਾਇਸ਼	0x01	// require enough room
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_VERT	0x02	// split/equalize vertically
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_HOR		0x04	// equalize horizontally
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_ਸਿਖਰ		0x08	// window at top-left of shell
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_BOT		0x10	// window at bottom-right of shell
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_HELP	0x20	// creating the help window
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_ਹੇਠਾਂ	0x40	// put new window below/right
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_ABOVE	0x80	// put new window above/left
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_NEWLOC	0x100	// don't copy location list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_FORCE_ਗੁੰਜਾਇਸ਼	0x200	// ignore "not enough room" errors
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESIZE_VERT	1	// resize vertically
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESIZE_HOR	2	// resize horizontally
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCGD_AW		1	// do autowrite if buffer was changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCGD_MULTWIN	2	// check also when several wins for the buf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCGD_FORCEIT	4	// ! used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCGD_ALLBUF	8	// may write all buffers
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_FREE	0x01	// free old value if it was allocated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_GLOBAL	0x02	// use global value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_LOCAL	0x04	// use local value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_MODELINE	0x08	// option in modeline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_WINONLY	0x10	// only set window-local options
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_NOWIN	0x20	// don't set window-local options
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_ONECOLUMN	0x40	// list options one per line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_BUTTON_SEP	'\n'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_SCREEN	2	// no screen updating yet
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEA_NONE	0	// don't use dialog
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEA_DIALOG	1	// use dialog when possible
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEA_QUIT	2	// quit editing the file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEA_RECOVER	3	// recover the file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_ਅਦਲਾ_ਬਦਲੀ_ਪੰਨਾ_ਮਾਪ 1048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਅਦਲਾ_ਬਦਲੀ_ਪੰਨਾ_ਮਾਪ 50000

// Special values for current_sctx.sc_sid.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_MODELINE	(-1)	// when using a modeline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_CMDARG	(-2)	// for "--cmd" argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_CARG	(-3)	// for "-c" argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_ENV		(-4)	// for sourcing environment variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_ERROR	(-5)	// option was reset because of an error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_NONE	(-6)	// don't set scriptID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_WINLAYOUT	(-7)	// changing window size

/*
 * Events for autocommands. Must be kept in sync with "event_tab".
 */
ਗਿਣਤੀ auto_event
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

    ਅੰਕ_EVENTS,			// MUST be the last one
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ auto_event event_T;

/*
 * Values for index in highlight_attr[].
 * When making changes, also update HL_FLAGS below!
 * And update the default value of 'highlight': HIGHLIGHT_INIT in optiondefs.h
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
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
    , HLF_ਜੋੜੋ	    // Added diff line
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
    , HLF_ਕੱਢੋ	    // popup window body
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
    , HLF_ਸੁਨੇਹਾ	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_ਗਿਣਤੀ	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    SMT_ALL = 0		    // Move to "all" words
    , SMT_BAD		    // Move to "bad" words only
    , SMT_RARE		    // Move to "rare" words only
} smt_T;
/*
 * Boolean constants
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAYBE	2	    // sometimes used for a variant on TRUE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS ਜੇ (ch_log_output == FALSE) ch_log_output = TRUE;
#ਨਹੀਂਤਾਂ
// no logging support
# define MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UINT32_T
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UINT32_TYPEDEF UINT32_T;
#ਜੇ_ਅੰਤ

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_NOP		0	// no pending operation
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_DELETE	1	// "d"  delete operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_YANK		2	// "y"  yank operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_CHANGE	3	// "c"  change operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_LSHIFT	4	// "<"  left shift operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_RSHIFT	5	// ">"  right shift operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FILTER	6	// "!"  filter operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_TILDE	7	// "g~" switch case operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_INDENT	8	// "="  indent operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FORMAT	9	// "gq" format operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_COLON	10	// ":"  colon operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_ਉੱਤਲਾ	11	// "gU" make upper case operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_ਹੇਠਲਾ	12	// "gu" make lower case operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_JOIN		13	// "J"  join operator, only for Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_ROT13	15	// "g?" rot-13 encoding
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_ਪਾਓ	17	// "I"  Insert column, only for Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_APPEND	18	// "A"  Append column, only for Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLD		19	// "zf" define a fold
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDOPEN	20	// "zo" open folds
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDOPENREC	21	// "zO" open folds recursively
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDCLOSE	22	// "zc" close folds
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDDEL	24	// "zd" delete folds
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FOLDDELREC	25	// "zD" delete folds recursively
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_FUNCTION	27	// "g@" call 'operatorfunc'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_NR_ਜੋੜੋ	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCHAR	0		// character-wise movement/register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MLINE	1		// line-wise movement/register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MBLOCK	2		// block-wise register

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_COLUMNS	12	// minimal columns for screen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_ਸਤਰਾਂ	2	// minimal lines for screen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_CMDHEIGHT	1	// minimal height for command line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਥਿਤੀ_ਉਚਾਈ_2	1	// default height of a status line under a
				//ਖਿੜਕੀ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU		// height of a status line under a window
# define WINBAR_ਉਚਾਈ(wp)	(wp)->w_winbar_ਉਚਾਈ
# define VISIBLE_ਉਚਾਈ(wp)	((wp)->w_ਉਚਾਈ + (wp)->w_winbar_ਉਚਾਈ)
#ਨਹੀਂਤਾਂ
# define WINBAR_ਉਚਾਈ(wp)	0
# define VISIBLE_ਉਚਾਈ(wp)	(wp)->w_ਉਚਾਈ
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSIZE	    512		// max. size of a line in the tags file

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIALOG_ਸੁਨੇਹਾ_ਮਾਪ 1000	// buffer size for dialog_msg()

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_BUF_LEN 480	// length of buffer for small messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_BUF_CLEN  (ਸੁਨੇਹਾ_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOLD_ਪਾਠ_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDO_HASH_ਮਾਪ 32

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
# include <fcntl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਦੁਆਧਾਰੀ_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#ਨਹੀਂਤਾਂ
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#ਜੇ_ਅੰਤ

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
# define O_EXTRA    O_ਦੁਆਧਾਰੀ
#ਨਹੀਂਤਾਂ
# define O_EXTRA    0
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ O_NOFOLLOW
# define O_NOFOLLOW 0
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#ਜੇ_ਅੰਤ

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_ONE(type)  (type *)alloc(ਆਕਾਰ(type))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_ONE_ID(type, id)  (type *)alloc_id(ਆਕਾਰ(type), id)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_MULT(type, ਗਿਣਤੀ_2)  (type *)alloc(ਆਕਾਰ(type) * (ਗਿਣਤੀ_2))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(ਆਕਾਰ(type))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(ਆਕਾਰ(type), id)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOC_CLEAR_MULT(type, ਗਿਣਤੀ_2)  (type *)alloc_clear(ਆਕਾਰ(type) * (ਗਿਣਤੀ_2))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(ਆਕਾਰ(type), FALSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LALLOC_CLEAR_MULT(type, ਗਿਣਤੀ_2)  (type *)lalloc_clear(ਆਕਾਰ(type) * (ਗਿਣਤੀ_2), FALSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LALLOC_MULT(type, ਗਿਣਤੀ_2)  (type *)lalloc(ਆਕਾਰ(type) * (ਗਿਣਤੀ_2), FALSE)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_MEMSET
# define vim_memset(ptr, c, ਮਾਪ)   memset((ptr), (c), (ਮਾਪ))
#ਨਹੀਂਤਾਂ
ਖਾਲੀ *vim_memset(ਖਾਲੀ *, ਪੂਰਨ, size_t);
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEAR_ਖੇਤਰ(ਖੇਤਰ)  vim_memset(&(ਖੇਤਰ), 0, ਆਕਾਰ(ਖੇਤਰ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEAR_ਸੂਚਕ(ptr)  vim_memset((ptr), 0, ਆਕਾਰ(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRLEN(s)	    ਲੜੀ_ਲੰਬਾਈ((ਅੱਖਰ *)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCPY(d, s)	    strcpy((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRNCPY(d, s, n)    strncpy((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCMP(d, s)	    strcmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRNCMP(d, s, n)    ਲੜੀ_ਤੁਲਨਾ((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਨਹੀਂਤਾਂ
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
# ਨਹੀਂਤਾਂ
#  define STRICMP(d, s)	    vim_stricmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਨਹੀਂਤਾਂ
# define STRCOLL(d, s)     strcmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਜੇ_ਅੰਤ

// Like strcpy() but allows overlapped source and destination.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))
#ਨਹੀਂਤਾਂ
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))
# ਨਹੀਂਤਾਂ
#  define STRNICMP(d, s, n) vim_strnicmp((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))
# endif
#ਜੇ_ਅੰਤ

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (ਪੂਰਨ)MAXCOL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (ਪੂਰਨ)(n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCAT(d, s)	    strcat((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRNCAT(d, s, n)    strncat((ਅੱਖਰ *)(d), (ਅੱਖਰ *)(s), (n))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((ਅੱਖਰ *)(s), (ਅੱਖਰ *)(cs))
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਾਹਰਲਾ_STR(s)		    ਬਾਹਰਲਾ_str((char_u *)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਾਹਰਲਾ_STR_NF(s)		    ਬਾਹਰਲਾ_str_nf((char_u *)(s))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.ਵਿੱਚ_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.ਵਿੱਚ_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ		    (gui.ਵਿੱਚ_use || p_tgc)
# ਨਹੀਂਤਾਂ
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ		    gui.ਵਿੱਚ_use
# endif
#ਨਹੀਂਤਾਂ
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ		    p_tgc
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#ਨਹੀਂਤਾਂ
# define IS_CTERM		    (t_colors > 1)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#ਜੇ_ਅੰਤ

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRERROR
# define PERROR(ਸੁਨੇਹਾ)		    (ਖਾਲੀ)semsg("%s: %s", (ਅੱਖਰ *)(ਸੁਨੇਹਾ), strerror(errno))
#ਨਹੀਂਤਾਂ
# define PERROR(ਸੁਨੇਹਾ)		    do_perror(ਸੁਨੇਹਾ)
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ	linenr_T;		// line number type
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ	colnr_T;		// column number type
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#ਨਹੀਂਤਾਂ
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHOWCMD_ਖਾਨੇ_ਗਿਣਤੀ 10			// columns needed by shown command

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ	    *vim_acl_T;		// dummy to pass an ACL to a function

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ mch_memmove
# define mch_memmove(ਮੰਜ਼ਿਲ, from, len) memmove((ਅੱਖਰ*)(ਮੰਜ਼ਿਲ), (ਅੱਖਰ*)(from), (len))
#ਜੇ_ਅੰਤ

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#ਜੇ defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_ਨਿਵੇਸ਼_BUF
#ਜੇ_ਅੰਤ

#ਜੇ !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, ਗਿਣਤੀ_2) vim_read((fd), (buf), (ਗਿਣਤੀ_2))
# define write_eintr(fd, buf, ਗਿਣਤੀ_2) vim_write((fd), (buf), (ਗਿਣਤੀ_2))
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, ਗਿਣਤੀ_2)   ਪਠ((fd), (ਅੱਖਰ *)(buf), (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(ਗਿਣਤੀ_2))
# define vim_write(fd, buf, ਗਿਣਤੀ_2)  ਲਿਖ((fd), (ਅੱਖਰ *)(buf), (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(ਗਿਣਤੀ_2))
#ਨਹੀਂਤਾਂ
# define vim_read(fd, buf, ਗਿਣਤੀ_2)   ਪਠ((fd), (ਅੱਖਰ *)(buf), (ਗਿਣਤੀ_2))
# define vim_write(fd, buf, ਗਿਣਤੀ_2)  ਲਿਖ((fd), (ਅੱਖਰ *)(buf), (ਗਿਣਤੀ_2))
#ਜੇ_ਅੰਤ

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_ATTR(n)	highlight_attr[(ਪੂਰਨ)(n)]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਦ_STR(n)	ਪਦ_ਲੜੀਆਂ[(ਪੂਰਨ)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EXTERN
# define EXTERN ਬਾਹਰੀ
# define INIT(x)
# define INIT2(a, b)
# define INIT3(a, b, c)
# define INIT4(a, b, c, d)
# define INIT5(a, b, c, d, e)
# define INIT6(a, b, c, d, e, f)
#ਨਹੀਂਤਾਂ
# ifndef INIT
#  define INIT(x) x
#  define INIT2(a, b) = {a, b}
#  define INIT3(a, b, c) = {a, b, c}
#  define INIT4(a, b, c, d) = {a, b, c, d}
#  define INIT5(a, b, c, d, e) = {a, b, c, d, e}
#  define INIT6(a, b, c, d, e, f) = {a, b, c, d, e, f}
#  define DO_INIT
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MAXBYTES	30

#ਜੇ (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
ਕਿਸਮਪਰਿਭਾਸ਼ਾ LARGE_INTEGER proftime_T;
#  define PROF_ਸਮਾਂ_BLANK "           "
#  define PROF_TOTALS_ਸਿਰਲੇਖ "count  total (s)   self (s)"
# ਨਹੀਂਤਾਂ
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  ਜੇ defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ timespec proftime_T;
#   define PROF_GET_ਸਮਾਂ(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_ਸਮਾਂ_FORMAT "%3ld.%09ld"
#   define PROF_ਸਮਾਂ_BLANK "              "
#   define PROF_TOTALS_ਸਿਰਲੇਖ "count     total (s)      self (s)"
#  ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ timeval proftime_T;
#   define PROF_GET_ਸਮਾਂ(tm) gettimeofday(tm, ਨਿਰਾ)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_ਸਮਾਂ_FORMAT "%3ld.%06ld"
#   define PROF_ਸਮਾਂ_BLANK "           "
#   define PROF_TOTALS_ਸਿਰਲੇਖ "count  total (s)   self (s)"
#  endif
# endif
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ proftime_T;	    // dummy for function prototypes
#ਜੇ_ਅੰਤ

// Type of compilation passed to compile_def_function()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CT_NONE,	    // use df_instr
    CT_PROFILE,	    // use df_instr_prof
    CT_DEBUG	    // use df_instr_debug, overrules CT_PROFILE
} compiletype_T;

/*
 * When compiling with 32 bit Perl time_t is 32 bits in the Perl code but 64
 * bits elsewhere.  That causes memory corruption.  Define time_T and use it
 * for global variables to avoid that.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
ਕਿਸਮਪਰਿਭਾਸ਼ਾ __time64_t  ਸਮਾਂ_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ time_t	    ਸਮਾਂ_T;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN64
ਕਿਸਮਪਰਿਭਾਸ਼ਾ __int64 sock_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ sock_T;
#ਜੇ_ਅੰਤ

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#ਜੇ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#ਜੇ_ਅੰਤ

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#ਅਨ੍ਤਰ੍ਭਾਵ "option.h"	// options and default values

#ਅਨ੍ਤਰ੍ਭਾਵ "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#ਅਨ੍ਤਰ੍ਭਾਵ "structs.h"	// defines many structures
#ਅਨ੍ਤਰ੍ਭਾਵ "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc.h"

// Values for "do_profiling".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROF_NONE	0	// profiling not started
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROF_YES	1	// profiling busy
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਖੱਬਾ	0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਵਿਚਕਾਰ	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਸੱਜਾ	0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_RELEASE	0x03

// bit masks for modifiers:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਖਿਸਕਾਓ	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ALT	0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_4	0x100	// scroll wheel down
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_5	0x200	// scroll wheel up

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_X1	0x300 // Mouse-button X1 (6th)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_X2	0x400 // Mouse-button X2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_6	0x500	// scroll wheel left
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_7	0x600	// scroll wheel right

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_DRAG_XTERM   0x40

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSEWHEEL_ਨੀਵਾਂ		0x60

// Lowest button code for extra mouse buttons 8-11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSESIDEBUTTONS_ਨੀਵਾਂ		0xa0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_CLICK_ਨਕਾਬ	0x03

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_MOUSE_CLICKS(ਕੋਡ) \
    (((ਬਿਨਾਨਿਸ਼ਾਨੀ)((ਕੋਡ) & 0xC0) >> 6) + 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਅੰਕ_MOUSE_CLICKS(ਕੋਡ, ਅੰਕ) \
    ((ਕੋਡ) = ((ਕੋਡ) & 0x3f) | ((((ਅੰਕ) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_UNKNOWN		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ਬਫਰ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ		2	// on status or command line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_SEP_ਸਤਰ		4	// on vertical separator line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_OTHER_WIN		8	// in other window but can't go there
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURSOR_MOVED		0x100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_FOCUS		0x01	// need to stay in this window
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_MAY_VIS		0x02	// may start Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_SETPOS		0x08	// only set current mouse position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_RELEASED		0x20	// button was released

#ਜੇ defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ਸਮਾਂ_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#ਜੇ_ਅੰਤ


// defines for eval_vars()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_ਰਸਤਾ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_ਸਿਰ		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ਗਿਣਤੀ	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_COUNT1	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_PREVCOUNT	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ERRMSG	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_WARNINGMSG	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_STATUSMSG	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SHELL_ERROR	6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_THIS_SESSION	7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ਸੰਸਕਰਣ	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_LNUM		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMRESPONSE	10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FNAME	11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_LANG		12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_LC_ਸਮਾਂ	13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CTYPE	14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CC_FROM	15
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CC_ਮੰਜ਼ਿਲ	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FNAME_ਵਿੱਚ	17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FNAME_ਬਾਹਰਲਾ	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FNAME_NEW	19
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FNAME_DIFF	20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CMDARG	21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FOLDSTART	22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FOLDEND	23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FOLDDASHES	24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FOLDLEVEL	25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_PROGNAME	26
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SEND_SERVER	27
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_DYING	28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_EXCEPTION	29
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_THROWPOINT	30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_REG		31
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CMDBANG	32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_INSERTMODE	33
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_VAL		34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ਕੁੰਜੀ		35
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_PROFILING	36
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FCS_REASON	37
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FCS_CHOICE	38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_BUFNR	39
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_WINNR	40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_WINID	41
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_LNUM	42
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_ਖਾਨਾ	43
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_BEVAL_ਪਾਠ	44
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SCROLLSTART	45
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SWAPNAME	46
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SWAPCHOICE	47
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SWAPCOMMAND	48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CHAR		49
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_MOUSE_WIN	50
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_MOUSE_WINID	51
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_MOUSE_LNUM   52
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_MOUSE_ਖਾਨਾ	53
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OP		54
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SEARCHFORWARD 55
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_HLSEARCH	56
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OLDFILES	57
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_WINDOWID	58
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_PROGPATH	59
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_COMPLETED_ਵਸਤੂ 60
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_NEW   61
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_OLD   62
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_OLDLOCAL 63
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_OLDGLOBAL 64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_COMMAND 65
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_OPTION_TYPE  66
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ERRORS	67
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_FALSE	68
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TRUE		69
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NONE		70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NULL		71
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NUMBERMAX	72
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NUMBERMIN	73
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NUMBERSIZE	74
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_VIM_DID_ENTER 75
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TESTING	76
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_ਸੰਖਿਆ	77
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_STRING	78
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_FUNC	79
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_LIST	80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_DICT	81
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_FLOAT	82
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_BOOL	83
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_NONE	84
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_ਕੰਮ	85
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_CHANNEL	86
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_BLOB	87
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_CLASS	88
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_OBJECT	89
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMRFGRESP	90
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMRBGRESP	91
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMU7RESP	92
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMSTYLERESP 93
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMBLINKRESP 94
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_EVENT	95
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_VERSIONLONG	96
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ECHOSPACE	97
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_ARGV		98
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_COLLATE      99
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_EXITING	100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_COLORNAMES   101
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SIZEOFINT	102
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SIZEOFLONG	103
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_SIZEOFPOINTER 104
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_MAXCOL	105
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_PYTHON3_ਸੰਸਕਰਣ 106
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_TYPEALIAS 107
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_ENUM	  108
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_ENUMVALUE 109
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_STACKTRACE	110
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TYPE_TUPLE	111
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_WAYLAND_DISPLAY 112
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CLIPMETHOD 113
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMDA1 114
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_TERMOSC 115
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_VIM_DID_INIT		116
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_CLIPPROVIDERS 117
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_IMAGEBACKEND 118
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VVAL_FALSE	0L	// VAR_BOOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VVAL_TRUE	1L	// VAR_BOOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VVAL_NONE	2L	// VAR_SPECIAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_ਸੰਖਿਆ	    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_STRING	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_FUNC	    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_LIST	    3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_DICT	    4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_FLOAT	    5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_BOOL	    6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_NONE	    7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_ਕੰਮ	    8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_CHANNEL    9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_BLOB	    10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_INSTR	    11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_CLASS	    12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_OBJECT	    13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_TYPEALIAS  14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_ENUM	    15
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_ENUMVALUE  16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_TYPE_TUPLE	    17

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_MAXNEST 100	// maximum nesting of lists and dicts

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TABSTOP_MAX 9999

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD

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
# define SELECT_ਵਿੱਚ_ਤਰੱਕੀ	1
# define SELECT_ਪੂਰਾ		2

# define SELECT_ਬਹੁਲਕ_CHAR	0
# define SELECT_ਬਹੁਲਕ_ਸ਼ਬਦ	1
# define SELECT_ਬਹੁਲਕ_ਸਤਰ	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		ਉਪਲਬਧ;	// Is clipboard available?
    ਪੂਰਨ		owned;		// Flag: do we own the selection?
    pos_T	ਸ਼ੁਰੂ;		// Start of selected area
    pos_T	ਸਿਰਾ;		// End of selected area
    ਪੂਰਨ		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_ਪੰਕਤੀ;
    short_u	origin_ਸ਼ੁਰੂ_ਖਾਨਾ;
    short_u	origin_ਸਿਰਾ_ਖਾਨਾ;
    short_u	ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ;
    short_u	ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_ਖਾਨਾ;
    short_u	max_ਖਾਨਾ;
    short_u	min_ਪੰਕਤੀ;
    short_u	max_ਪੰਕਤੀ;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	ਬਹੁਲਕ;		// Select by char, word, or line.

# ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_XCLIPBOARD)
    Atom	sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ਜੇ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
    GdkAtom     gtk_sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ਜੇ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    int_u	format;		// Vim's own special clipboard format
    int_u	format_raw;	// Vim's raw text clipboard format
# endif
# ifdef FEAT_GUI_HAIKU
    // No clipboard at the moment. TODO?
# endif
} Clipboard_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ Clipboard_T;	// This is required for the prototypes.
#ਜੇ_ਅੰਤ

// Use 64-bit stat structure on MS-Windows.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _stat64 stat_T;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ stat stat_T;
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ soundcb_S soundcb_T;

#ਜੇ (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(ਲਿਖੋ, fmt_idx, arg_idx)))
#ਨਹੀਂਤਾਂ
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#ਜੇ_ਅੰਤ

#ਜੇ defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#ਨਹੀਂਤਾਂ
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_ਮੇਲ,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    PASTE_ਪਾਓ,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_ਬਹੁਲਕ_T;

// Argument for flush_buffers().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    FLUSH_ਨਿਊਨਤਮ,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_ਨਿਵੇਸ਼		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    USEPOPUP_NONE,
    USEPOPUP_ਸਾਧਾਰਨ,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_ਢੇਰ,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENABLE_STL_ਬਹੁਲਕ_MULTI_NL

// Argument for build_stl_str_hl_local().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    STL_ਬਹੁਲਕ_SINGLE,	    // Does not accept line breaks "%@"
    STL_ਬਹੁਲਕ_MULTI,	    // Accept line breaks "%@"
    STL_ਬਹੁਲਕ_GET_RENDERED_ਉਚਾਈ,   // Just get stl rendered height
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
    STL_ਬਹੁਲਕ_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#ਜੇ_ਅੰਤ
} stl_ਬਹੁਲਕ_T;

// Return value of match_keyprotocol()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    KEYPROTOCOL_NONE,
    KEYPROTOCOL_MOK2,
    KEYPROTOCOL_KITTY,
    KEYPROTOCOL_FAIL
} keyprot_T;

// errors for when calling a function
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CPT_ABBR,		// "abbr"
    CPT_ਕਿਸਮ,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_ਗਿਣਤੀ,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ *(*opt_did_ਸੈੱਟ_cb_T)(optset_T *ਦਲੀਲਾਂ);

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*opt_expand_cb_T)(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches);

// Flags for assignment functions.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_VAR	0     // ":var" (nothing special)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_FINAL	0x01  // ":final"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_CONST	0x02  // ":const"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_DECL	0x08  // may declare variable if it does not exist
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_UNPACK	0x10  // using [a, b] = list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_FOR_LOOP 0x40 // assigning to loop variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_INIT	0x80 // not assigning a value, just a declaration
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_UPDATE_ਖੰਡ_ID 0x100  // update sav_block_id
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੌਂਪੋ_COMPOUND_OP 0x200  // compound operator e.g. "+="

#ਅਨ੍ਤਰ੍ਭਾਵ "ex_cmds.h"	    // Ex command defines
#ਅਨ੍ਤਰ੍ਭਾਵ "spell.h"	    // spell checking stuff

#ਅਨ੍ਤਰ੍ਭਾਵ "proto.h"	    // function prototypes

// This has to go after the include of proto.h, as proto/gui.pro declares
// functions of these names. The declarations would break if the defines had
// been seen at that stage.  But it must be before globals.h, where error_ga
// is declared.
#ਜੇ !defined(MSWIN) && !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_HAIKU) \
	&& !defined(FEAT_GUI_GTK) && !defined(PROTO)
# define mch_errmsg(str)	fprintf(stderr, "%s", (str))
# define display_errors()	fflush(stderr)
# define mch_ਸੁਨੇਹਾ(str)		ਲਿਖੋ("%s", (str))
#ਨਹੀਂਤਾਂ
# define USE_MCH_ERRMSG
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define ਸੰਖਿਆ_ਚੌੜਾਈ(x) 7
#ਜੇ_ਅੰਤ

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#ਜੇ !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "globals.h"	    // global variables and messages
#ਅਨ੍ਤਰ੍ਭਾਵ "errors.h"	    // error messages

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
ਵਿੱਚ_vim9script(ਖਾਲੀ)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    ਵਾਪਸ (current_sctx.sc_ਸੰਸਕਰਣ == SCRIPT_ਸੰਸਕਰਣ_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#ਜੇ_ਅੰਤ

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#ਜੇ defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#ਜੇ_ਅੰਤ

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*.*)\t*.*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVB code (*.bas, *.frm)\t*.bas;*.frm\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# ਨਹੀਂਤਾਂ
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*)\t*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*)\t*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*)\t*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# endif
# define BROWSE_SAVE 1	    // flag for do_browse()
# define BROWSE_DIR 2	    // flag for do_browse()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
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
#ਜੇ_ਅੰਤ

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#ਜੇ defined(MEM_PROFILE)
# define vim_realloc(ptr, ਮਾਪ)  mem_realloc((ptr), (ਮਾਪ))
#ਨਹੀਂਤਾਂ
# define vim_realloc(ptr, ਮਾਪ)  realloc((ptr), (ਮਾਪ))
#ਜੇ_ਅੰਤ

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_8_ਦੋਆਧਾਰੀ_ਅੰਕ	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_DBCS	0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_UNICODE	0x04

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_2BYTE	0x40	    // Unicode: UCS-2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_4BYTE	0x80	    // Unicode: UCS-4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_2_ਸ਼ਬਦ	0x100	    // Unicode: UTF-16

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_LATIN1	0x200	    // Latin1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_LATIN9	0x400	    // Latin9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
# ifndef EILSEQ
#  define EILSEQ 123
# endif
# ifdef DYNAMIC_ICONV
// On Win32 iconv.dll is dynamically loaded.
#  define ICONV_ERRNO (*iconv_errno())
#  define ICONV_E2BIG  7
#  define ICONV_EINVAL 22
#  define ICONV_EILSEQ 42
# ਨਹੀਂਤਾਂ
#  define ICONV_ERRNO errno
#  define ICONV_E2BIG  E2BIG
#  define ICONV_EINVAL EINVAL
#  define ICONV_EILSEQ EILSEQ
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.ਵਿੱਚ_use ? (ਦੁੱਗਣਾ)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.ਵਿੱਚ_use ? (ਦੁੱਗਣਾ)(p) / gui.scale : (p))
#ਨਹੀਂਤਾਂ
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.ਵਿੱਚ_use) ? gui_mch_get_display() : xterm_dpy)
# ਨਹੀਂਤਾਂ
#  define X_DISPLAY	(gui.ਵਿੱਚ_use ? gui.dpy : xterm_dpy)
# endif
#ਜਾਂਜੇ defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.ਵਿੱਚ_use) ? gui_mch_get_display() : (Display *)ਨਿਰਾ)
# ਨਹੀਂਤਾਂ
#  define X_DISPLAY	gui.dpy
# endif
#ਜਾਂਜੇ defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#ਨਹੀਂਤਾਂ
# define X_DISPLAY	(Display *)ਨਿਰਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(2,14,0)
#  define gtk_widget_get_ਖਿੜਕੀ(wid)	((wid)->ਖਿੜਕੀ_2)
#  define gtk_plug_get_socket_ਖਿੜਕੀ(wid)	((wid)->socket_ਖਿੜਕੀ)
#  define gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ(sel)	((sel)->ਅੰਕੜੇ)
#  define gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(sel)	((sel)->type)
#  define gtk_selection_ਅੰਕੜੇ_get_format(sel)	((sel)->format)
#  define gtk_selection_ਅੰਕੜੇ_get_ਲੰਮਾਈ(sel)	((sel)->ਲੰਮਾਈ)
#  define gtk_adjustment_ਸੈੱਟ_ਹੇਠਲਾ(adj, ਨੀਵਾਂ) \
    ਕਰੋ { (adj)->ਹੇਠਲਾ = ਨੀਵਾਂ; } ਜਦੋਂ (0)
#  define gtk_adjustment_ਸੈੱਟ_ਉੱਤਲਾ(adj, ਉੱਪਰ) \
    ਕਰੋ { (adj)->ਉੱਤਲਾ_2 = ਉੱਪਰ; } ਜਦੋਂ (0)
#  define gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_ਮਾਪ(adj, ਮਾਪ) \
    ਕਰੋ { (adj)->ਪੰਨਾ_ਮਾਪ = ਮਾਪ; } ਜਦੋਂ (0)
#  define gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_increment(adj, inc) \
    ਕਰੋ { (adj)->ਪੰਨਾ_increment = inc; } ਜਦੋਂ (0)
#  define gtk_adjustment_ਸੈੱਟ_ਕਦਮ_increment(adj, inc) \
    ਕਰੋ { (adj)->ਕਦਮ_increment = inc; } ਜਦੋਂ (0)
# endif
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(2,16,0)
#  define gtk_selection_ਅੰਕੜੇ_get_selection(sel)	((sel)->selection)
# endif
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(2,18,0)
#  define gtk_widget_get_ਵੰਡ(wid, alloc) \
    ਕਰੋ { *(alloc) = (wid)->ਵੰਡ; } ਜਦੋਂ (0)
#  define gtk_widget_ਸੈੱਟ_ਵੰਡ(wid, alloc) \
    ਕਰੋ { (wid)->ਵੰਡ = *(alloc); } ਜਦੋਂ (0)
#  define gtk_widget_get_has_ਖਿੜਕੀ(wid)	!GTK_WIDGET_NO_ਖਿੜਕੀ(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_ਸੈੱਟ_ਖਿੜਕੀ(wid, win) \
    ਕਰੋ { (wid)->ਖਿੜਕੀ_2 = (win); } ਜਦੋਂ (0)
#  define gtk_widget_ਸੈੱਟ_can_default(wid, can) \
    ਕਰੋ { ਜੇ (can) \
	    { GTK_WIDGET_ਸੈੱਟ_FLAGS(wid, GTK_CAN_DEFAULT); } \
	ਨਹੀਂਤਾਂ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } ਜਦੋਂ (0)
#  define gtk_widget_ਸੈੱਟ_can_focus(wid, can) \
    ਕਰੋ { ਜੇ (can) \
	    { GTK_WIDGET_ਸੈੱਟ_FLAGS(wid, GTK_CAN_FOCUS); } \
	ਨਹੀਂਤਾਂ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } ਜਦੋਂ (0)
#  define gtk_widget_ਸੈੱਟ_visible(wid, vis) \
    ਕਰੋ { ਜੇ (vis) \
	    { gtk_widget_show(wid); } \
	ਨਹੀਂਤਾਂ \
	    { gtk_widget_hide(wid); } } ਜਦੋਂ (0)
# endif
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_ਸੈੱਟ_mapped(wid, map) \
    ਕਰੋ { ਜੇ (map) \
	    { GTK_WIDGET_ਸੈੱਟ_FLAGS(wid, GTK_MAPPED); } \
	ਨਹੀਂਤਾਂ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } ਜਦੋਂ (0)
#  define gtk_widget_ਸੈੱਟ_realized(wid, rea) \
    ਕਰੋ { ਜੇ (rea) \
	    { GTK_WIDGET_ਸੈੱਟ_FLAGS(wid, GTK_REALIZED); } \
	ਨਹੀਂਤਾਂ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } ਜਦੋਂ (0)
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
# undef NBDEBUG
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#ਨਹੀਂਤਾਂ
# define nbdebug(a)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਵਿੱਚ_PERL_FILE
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
# ਜੇ (defined(MACOS_X) && !defined(MAC_OS_X_ਸੰਸਕਰਣ_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#ਜੇ_ਅੰਤ

// values for vim_handle_signal() that are not a signal
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGNAL_ਖੰਡ	(-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGNAL_UNBLOCK  (-2)
#ਜੇ !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#ਜੇ_ਅੰਤ

// flags for skip_vimgrep_pat()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VGR_GLOBAL	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VGR_NOJUMP	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BAD_REPLACE	'?'	// replace it with '?' (default)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BAD_KEEP	(-1)	// leave it
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BAD_DROP	(-2)	// erase it

// last argument for do_source()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOSO_NONE	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOSO_VIMRC	1	// loading vimrc file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIF_WANT_INFO	    1	// load non-mark info
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIF_WANT_MARKS	    2	// load file marks
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIF_FORCEIT	    8	// overwrite info already read
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BFA_DEL		 1	// buffer is going to be deleted
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BFA_WIPE	 2	// buffer is going to be wiped out
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BFA_KEEP_UNDO	 4	// do not free undo information
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSCR_ਥੱਲੇ	0	// DOWN must be FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSCR_ਉੱਪਰ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSCR_ਖੱਬਾ	(-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSCR_ਸੱਜਾ	(-2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYLEN_PART_ਕੁੰਜੀ (-1)	// keylen value for incomplete key-code
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILEINFO_OK	     0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILEINFO_READ_FAIL   2	// CreateFile() failed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_BOOL	0x01	// Boolean option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_ਅੰਕ	0x02	// Number option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_STRING	0x04	// String option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_GLOBAL	0x08	// Option has global value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_WIN	0x10	// Option has window-local value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_BUF	0x20	// Option has buffer-local value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SREQ_GLOBAL	0	// Request global option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SREQ_WIN	1	// Request window-local option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREG_NO_ਵਿਅੰਜਕ	1	// Do not allow expression register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREG_ਵਿਅੰਜਕ_SRC	2	// Return expression itself for "=" register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUTOLOAD_CHAR '#'

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
# define MAX_OPEN_CHANNELS 10
#ਨਹੀਂਤਾਂ
# define MAX_OPEN_CHANNELS 0
#ਜੇ_ਅੰਤ

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#ਨਹੀਂਤਾਂ
# define MAX_CLIENT_CHANNELS 0
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
# define MAX_NAMED_PIPE_ਮਾਪ 65535
#ਜੇ_ਅੰਤ

// Options for json_encode() and json_decode.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JSON_JS		1   // use JS instead of JSON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JSON_NO_NONE	2   // v:none item not allowed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_ALL	    0x01	// all matches, not just the first one
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_DIR	    0x02	// find directories instead of files.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_ERR	    0x04	// give an error message when none found.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_ਸ਼ੁਰੂ   0x08	// also use "start" directory in 'packpath'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_NORTP   0x20	// do not use 'runtimepath'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_NOAFTER 0x40	// skip "after" directories
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਭ_ਤੋਂ_ਨੀਵਾਂ_WIN_ID 1000

// Used by the garbage collector.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPYID_INC 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPYID_ਨਕਾਬ (~0x1)

// Values for trans_function_name() argument:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_INT		0x01	// internal function name OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_QUIET	0x02	// no error messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_NO_DEREF	0x08	// do not dereference a Funcref
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_READ_ONLY	0x10	// will not change the var
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_COMPILING	0x40	// only used for GLV_COMPILING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_NEW_FUNC	0x80	// defining a new function
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_ਸੌਂਪੋ_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TFN_ਵਿੱਚ_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_QUIET	TFN_QUIET	// no error messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_ਸੌਂਪੋ_WITH_OP TFN_ਸੌਂਪੋ_WITH_OP // assignment with operator
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_PREFER_FUNC	0x10000		// prefer function above variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLEN_FIXED 40

// flags for find_name_end()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNE_INCL_BR	1	// include [] in name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNE_CHECK_ਸ਼ੁਰੂ	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#ਜੇ (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#ਜੇ_ਅੰਤ

// stat macros
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# ਨਹੀਂਤਾਂ
#  define S_ISDIR(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# ਨਹੀਂਤਾਂ
#  define S_ISREG(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# ਨਹੀਂਤਾਂ
#  define S_ISBLK(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# ਨਹੀਂਤਾਂ
#  define S_ISSOCK(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# ਨਹੀਂਤਾਂ
#  define S_ISFIFO(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# ਨਹੀਂਤਾਂ
#  define S_ISCHR(m)	0
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# ਨਹੀਂਤਾਂ
#  define S_ISLNK(m)	0
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ਸਮਾਂ_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), ਨਿਰਾ)
# define ELAPSED_FUNC(v) elapsed(&(v))
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ timeval elapsed_T;
ਲੰਮਾ elapsed(ਬਣਤਰ timeval *ਸ਼ੁਰੂ_tv);
#ਜਾਂਜੇ defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_ਗਿਣਤੀ()
# define ELAPSED_FUNC(v) elapsed(v)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ DWORD elapsed_T;
# ifndef PROTO
ਲੰਮਾ elapsed(DWORD ਸ਼ੁਰੂ_tick);
# endif
#ਜੇ_ਅੰਤ

// Replacement for nchar used by nv_replace().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPLACE_CR_NCHAR    (-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਦ_ਸ਼ੁਰੂ_NOJOB	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਦ_ਸ਼ੁਰੂ_FORCEIT	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਦ_ਸ਼ੁਰੂ_SYSTEM	4

// Used for icon/title save and restore.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_RESTORE_TITLE	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_RESTORE_ICON	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APC_SUBSTITUTE		2   // text is replaced, not inserted
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APC_INDENT		4   // changing indent

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPTERM_FROM_PART	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPTERM_DO_LT		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPTERM_SPECIAL		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSK_KEEP_X_ਕੁੰਜੀ	0x02	// don't translate xHome to Home key
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSK_ਵਿੱਚ_STRING	0x04	// TRUE in string, double quote is escaped
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READDIR_ਛਾਂਟੋ_NONE	0  // do not sort
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READDIR_ਛਾਂਟੋ_BYTE	1  // sort by byte order (strcmp), default
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READDIR_ਛਾਂਟੋ_IC		2  // sort ignoring case (strcasecmp)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READDIR_ਛਾਂਟੋ_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCH_DELAY_IGNOREINPUT	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_VAR_VERBOSE	1   // may give error message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_VAR_IMPORT		4   // may return special variable for import
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUZZY_ਮੇਲ_MAX_LEN	1024    // max characters that can be matched
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UC_ਬਫਰ	1	// -buffer: local to current buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VSE_NONE	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VSE_SHELL	1	// escape for a shell command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VSE_ਬਫਰ	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FFED_IS_GLOBAL	1	// "g:" was used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FFED_NO_GLOBAL	2	// only check for script-local functions

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_LSHIFT_ਬਿੱਟ (varnumber_T)((ਆਕਾਰ(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CLASS	1	// inside a class
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_INTERFACE	2	// inside an interface
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_ABSTRACT_ਵਿਧੀ	4	// inside an abstract class

// Flags used by getvcol()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETVCOL_ਸਿਰਾ_EXCL_LBR	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# define ਰਸਤਾ_ESC_WILDCARDS	"*?%"
#ਨਹੀਂਤਾਂ
# ifdef MSWIN
#  define ਰਸਤਾ_ESC_WILDCARDS	"*?["
# ਨਹੀਂਤਾਂ
#  define ਰਸਤਾ_ESC_WILDCARDS	"*?[{"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // VIM__H
