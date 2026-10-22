/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIM__H
#ವ್ಯಾಖ್ಯಾನಿಸು VIM__H

#ಅನ್ತರ್ಭಾವ "protodef.h"

// _WIN32 is defined as 1 when the compilation target is 32-bit or 64-bit.
// Note: If you want to check for 64-bit use the _WIN64 macro.
#ಆದರೆ defined(WIN32) || defined(_WIN32)
# define MSWIN
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
# include <io.h>
#ಆದರೆ_ಅಂತ್ಯ

// ============ the header file puzzle: order matters =========

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CONFIG_H	// GNU autoconf (or something else) was here
# ifdef VMS
#  include "config.h"   /* Rely on /INCLUDE to find it. */
# ಇಲ್ಲದಿದ್ದರೆ
#  include "auto/config.h"
# endif /* def VMS [else] */
# define HAVE_PATHDEF

/*
 * Check if configure correctly managed to find sizeof(int).  If this failed,
 * it becomes zero.  This is likely a problem of not being able to run the
 * test program.  Other items from configure may also be wrong then!
 */
# ಆದರೆ (VIM_SIZEOF_INT == 0)
#  error configure did not ಸತತ properly.  Check ಸ್ವಯಂಚಾಲಿತ/config.ಲಘುಗಣಕ.
# endif

# ಆದರೆ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__) || defined(__GNU__)
// Needed for strptime().  Needs to be done early, since header files can
// include other header files and end up including time.h, where these symbols
// matter for Vim.
// 700 is needed for mkdtemp().
#  ifndef XOPEN_ಮೂಲ
#   define XOPEN_ಮೂಲ    700

// On old systems, defining _XOPEN_SOURCE causes _BSD_SOURCE, _SVID_SOURCE
// and/or // _DEFAULT_SOURCE not to be defined, so do that here.  Those are
// needed to include nanosecond-resolution timestamps in struct stat.  On new
// systems, _DEFAULT_SOURCE is needed to avoid warning messages about using
// deprecated _BSD_SOURCE or _SVID_SOURCE.
#   ifndef BSD_ಮೂಲ
#    define BSD_ಮೂಲ 1
#   endif
#   ifndef SVID_ಮೂಲ
#    define SVID_ಮೂಲ 1
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
# ಆದರೆ defined(__CYGWIN32__) && defined(HAVE_FCHDIR)
#  undef HAVE_FCHDIR
# endif

// We may need to define the uint32_t on non-Unix system, but using the same
// identifier causes conflicts.  Therefore use UINT32_T.
# define UINT32_TYPEDEF uint32_t
#ಆದರೆ_ಅಂತ್ಯ

// for INT_MAX, LONG_MAX et al.
#ಅನ್ತರ್ಭಾವ <limits.h>

#ಆದರೆ !defined(UINT32_TYPEDEF)
# ಆದರೆ defined(uint32_t)  // this doesn't catch typedefs, unfortunately
#  define UINT32_TYPEDEF uint32_t
# ಇಲ್ಲದಿದ್ದರೆ
  // Fall back to assuming unsigned int is 32 bit.  If this is wrong then the
  // test in blowfish.c will fail.
#  define UINT32_TYPEDEF ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
# endif
#ಆದರೆ_ಅಂತ್ಯ

// C89 does not define SIZE_MAX
#ಆದರೆ defined(__hpux) || defined(VMS)
# ifndef ಗಾತ್ರ_MAX
#  define ಗಾತ್ರ_MAX ((size_t)(-1))
# endif
#ಆದರೆ_ಅಂತ್ಯ

// user ID of root is usually zero, but not for everybody
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __TANDEM
# ifndef TANDEM_ಮೂಲ
#  define TANDEM_ಮೂಲ
# endif
# include <floss.h>
# define ಬೇರು_UID 65535
# define OLDXAW
# ಆದರೆ (_TANDEM_ARCH_ == 2 && __H_Series_RVU >= 621)
#  define SA_ONSTACK_COMPATIBILITY
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬೇರು_UID 0
#ಆದರೆ_ಅಂತ್ಯ

/* Include MAC_OS_X_VERSION_* macros */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * MACOS_X	    compiling for Mac OS X
 * MACOS_X_DARWIN   integrating the darwin feature into MACOS_X
 */
#ಆದರೆ defined(MACOS_X_DARWIN) && !defined(MACOS_X)
# define MACOS_X
#ಆದರೆ_ಅಂತ್ಯ
// Unless made through the Makefile enforce GUI on Mac
#ಆದರೆ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define UNIX
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) \
    || defined(FEAT_GUI_HAIKU) \
    || defined(FEAT_GUI_MSWIN) \
    || defined(FEAT_GUI_PHOTON)
# ಆದರೆ !defined(FEAT_GUI) && !defined(NO_X11_INCLUDES)
#  define FEAT_GUI
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Check support for rendering options
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ಆದರೆ defined(FEAT_DIRECTX)
#  define FEAT_RENDER_OPTIONS
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * VIM_SIZEOF_INT is used in feature.h, and the system-specific included files
 * need items from feature.h.  Therefore define VIM_SIZEOF_INT here.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# define VIM_SIZEOF_INT 4
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
  // Be conservative about sizeof(int). It could be 4 too.
# ifndef FEAT_GUI_GTK	// avoid problems when generating prototypes
#  ifdef __GNUC__
#   define VIM_SIZEOF_INT	4
#  ಇಲ್ಲದಿದ್ದರೆ
#   define VIM_SIZEOF_INT	2
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MACOS_X) && !defined(HAVE_CONFIG_H)
# define VIM_SIZEOF_INT __SIZEOF_INT__
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ VIM_SIZEOF_INT < 4 && !defined(PROTO)
# error Vim only ಕೆಲಸಮಾಡುತ್ತದೆ with 32 ಬಿಟ್ ಪೂರ್ಣಾಂಕ or larger
#ಆದರೆ_ಅಂತ್ಯ

/*
 * #defines for optionals and features
 * Also defines FEAT_TINY, FEAT_NORMAL, etc. when FEAT_HUGE is defined.
 */
#ಅನ್ತರ್ಭಾವ "feature.h"

#ಆದರೆ defined(MACOS_X_DARWIN)
# ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && !defined(FEAT_CLIPBOARD)
#  define FEAT_CLIPBOARD
# endif
# ಆದರೆ defined(FEAT_HUGE) && !defined(FEAT_SOUND) && \
	defined(__clang_major__) && __clang_major__ >= 7 && \
	defined(MAC_OS_X_ಆವೃತ್ತಿ_MIN_REQUIRED) && MAC_OS_X_ಆವೃತ್ತಿ_MIN_REQUIRED >= 1060
#  define FEAT_SOUND
# endif
# ಆದರೆ defined(FEAT_SOUND)
#  define FEAT_SOUND_MACOSX
# endif
#ಆದರೆ_ಅಂತ್ಯ

// +x11 is only enabled when it's both available and wanted.
#ಆದರೆ defined(HAVE_X11) && defined(WANT_X11)
# define FEAT_X11
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NO_X11_INCLUDES
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
#ಆದರೆ_ಅಂತ್ಯ

// The Mac conversion stuff doesn't work under X11.
#ಆದರೆ defined(MACOS_X_DARWIN)
# define MACOS_CONVERT
#ಆದರೆ_ಅಂತ್ಯ

// Can't use "PACKAGE" here, conflicts with a Perl include file.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIMPACKAGE
# define VIMPACKAGE	"vim"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Find out if function definitions should include argument types
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AZTEC_C
# include <functions.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SASC
# include <clib/exec_protos.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DCC
# include <clib/exec_protos.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
# ifdef __HAIKU__
#  include "os_haiku.h"
#  define __ARGS(x)  x
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(UNIX) || defined(VMS)) \
	&& (!defined(MACOS_X) || defined(HAVE_CONFIG_H))
# include "os_unix.h"	    // bring lots of system header files
#ಅಥವಾಆದರೆ !defined(PROTO)
  // For all non-Unix systems: use old-fashioned signal().
# define mch_signal(signum, sighandler) ಸಂಕೇತ(signum, sighandler)
#ಆದರೆ_ಅಂತ್ಯ

// Mark unused function arguments with UNUSED, so that gcc -Wunused-parameter
// can be used to check for mistakes.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಬಳಸದ
# ಆದರೆ defined(HAVE_ATTRIBUTE_ಬಳಸದ) || defined(__MINGW32__)
#  define ಬಳಸದ __attribute__((ಬಳಸದ_2))
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined __has_attribute
#   ಆದರೆ __has_attribute(ಬಳಸದ_2)
#    define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#   endif
#  endif
# endif
# ifndef ಬಳಸದ
#  define ಬಳಸದ
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Used to check for "sun", "__sun" is used by newer compilers.
#ಆದರೆ defined(__sun)
# define SUN_SYSTEM
#ಆದರೆ_ಅಂತ್ಯ

// If we're compiling in C++ (currently only KVim), the system
// headers must have the correct prototypes or nothing will build.
// Conversely, our prototypes might clash due to throw() specifiers and
// cause compilation failures even though the headers are correct.  For
// a concrete example, gcc-3.2 enforces exception specifications, and
// glibc-2.2.5 has them in their system headers.
#ಆದರೆ !defined(__cplusplus) && defined(UNIX) \
       && !defined(MACOS_X) // MACOS_X doesn't yet support osdef.h
# include "auto/osdef.h"	// bring missing declarations in
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
# ifdef AMIGA
#  include "os_amiga.h"
# endif

# ifdef MSWIN
#  include "os_win32.h"
# endif

# ಆದರೆ defined(MACOS_X)
#  include "os_mac.h"
# endif

# ifdef __QNX__
#  include "os_qnx.h"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ X_LOCALE
# include <X11/Xlocale.h>
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Maximum length of a path (for non-unix systems) Make it a bit long, to stay
 * on the safe side.  But not too long to put on the stack.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAXPATHL
# ifdef MAXPATHLEN
#  define MAXPATHL  MAXPATHLEN
# ಇಲ್ಲದಿದ್ದರೆ
#  define MAXPATHL  256
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
# define ಮಾರ್ಗ_ESC_CHARS ((char_u *)" \t\n*?[{`%#'\"|!<")
// '%' and '#' are not escaped for ":buffer": it has no EX_XFILE, so they are
// not expanded, and escaping them as "\%"/"\#" breaks buffer name matching
// when '%'/'#' is in 'isfname' (backslash treated as a path separator).
# define ಬಫರ್_ESC_CHARS ((char_u *)" \t\n*?[`'\"|!<")
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VMS
    // VMS allows a lot of characters in the file name
#  define ಮಾರ್ಗ_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'\"|!")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?{`\\%#'|!()&")
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಮಾರ್ಗ_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<")
#  define SHELL_ESC_CHARS ((char_u *)" \t\n*?[{`$\\%#'\"|!<>();&")
# endif
# define ಬಫರ್_ESC_CHARS ((char_u *)" \t\n*?[`$\\%#'\"|!<")
#ಆದರೆ_ಅಂತ್ಯ

// length of a buffer to store a number in ASCII (64 bits binary + NUL)
#ವ್ಯಾಖ್ಯಾನಿಸು NUMBUFLEN 65

// flags for vim_str2nr()
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_BIN  0x01
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_OCT  0x02
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_HEX  0x04
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_OOCT 0x08    // Octal with prefix "0o": 0o777
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_ALL (STR2NR_BIN + STR2NR_OCT + STR2NR_HEX + STR2NR_OOCT)
#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_NO_OCT (STR2NR_BIN + STR2NR_HEX + STR2NR_OOCT)

#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_FORCE 0x80   // only when ONE of the above is used

#ವ್ಯಾಖ್ಯಾನಿಸು STR2NR_QUOTE 0x10   // ignore embedded single quotes

/*
 * Shorthand for unsigned variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ	char_u;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ	short_u;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	int_u;

// Make sure long_u is big enough to hold a pointer.
// On Win64, longs are 32 bits and pointers are 64 bits.
// For printf() and scanf(), we need to take care of long_u specifically.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN64
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ __int64	long_u;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ		 __int64	long_i;
# define SCANF_HEX_LONG_U       "%llx"
# define SCANF_DECIMAL_LONG_U   "%llu"
# define PRINTF_HEX_LONG_U      "0x%llx"
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ		long_u;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ		 ದೀರ್ಘ		long_i;
# define SCANF_HEX_LONG_U       "%lx"
# define SCANF_DECIMAL_LONG_U   "%lu"
# define PRINTF_HEX_LONG_U      "0x%lx"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PRINTF_DECIMAL_LONG_U SCANF_DECIMAL_LONG_U

/*
 * Only systems which use configure will have SIZEOF_OFF_T and VIM_SIZEOF_LONG
 * defined, which is ok since those are the same systems which can have
 * varying sizes for off_t.  The other systems will continue to use "%ld" to
 * print off_t since off_t is simply a typedef to long for them.
 */
#ಆದರೆ defined(SIZEOF_OFF_T) && (SIZEOF_OFF_T > VIM_SIZEOF_LONG)
# define LONG_LONG_OFF_T
#ಆದರೆ_ಅಂತ್ಯ

/*
 * We use 64-bit file functions here, if available.  E.g. ftello() returns
 * off_t instead of long, which helps if long is 32 bit and off_t is 64 bit.
 * We assume that when fseeko() is available then ftello() is too.
 * Note that Windows has different function names.
 */
#ಆದರೆ defined(MSWIN) && !defined(PROTO)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ __int64 off_T;
# ifdef __MINGW32__
#  define vim_lseek lseek64
#  define vim_fseek fseeko64
#  define vim_ftell ftello64
# ಇಲ್ಲದಿದ್ದರೆ
#  define vim_lseek _lseeki64
#  define vim_fseek _fseeki64
#  define vim_ftell _ftelli64
# endif
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ off_t off_T;
# ifdef HAVE_FSEEKO
#  define vim_lseek lseek
#  define vim_ftell ftello
#  define vim_fseek fseeko
# ಇಲ್ಲದಿದ್ದರೆ
#  define vim_lseek lseek
#  define vim_ftell ftell
#  define vim_fseek(a, b, c)	fseek(a, (ದೀರ್ಘ)b, c)
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The characters and attributes cached for the screen.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ char_u schar_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ sattr_T;
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_TYPENR 65535

/*
 * The u8char_T can hold one decoded UTF-8 character.
 * We use 32 bits, since some Asian characters don't fit in 16 bits.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ u8char_T;	// int is 32 bits or more

/*
 * The vimlong_T has sizeof(vimlong_T) >= 2 * sizeof(int).
 * One use is simple handling of overflow in int calculations.
 */
#ಆದರೆ defined(VMS) && defined(VAX)
// unsupported compiler
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ      vimlong_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ ದೀರ್ಘ vimlong_T;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX		    // For Unix this is included in os_unix.h
# include <stdio.h>
# include <ctype.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "ascii.h"
#ಅನ್ತರ್ಭಾವ "keymap.h"
#ಅನ್ತರ್ಭಾವ "termdefs.h"
#ಅನ್ತರ್ಭಾವ "macros.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DCC
# include <sys/stat.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN)
# include <sys/stat.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_ERRNO_H) || defined(MSWIN)
# include <errno.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Allow other (non-unix) systems to configure themselves now
 * These are also in os_unix.h, because osdef.sh needs them there.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
// Note: Some systems need both string.h and strings.h (Savage).  If the
// system can't handle this, define NO_STRINGS_WITH_STRING_H.
# ifdef HAVE_STRING_H
#  include <string.h>
# endif
# ಆದರೆ defined(HAVE_ಅಕ್ಷರಸರಣಿಗಳು_H) && !defined(NO_ಅಕ್ಷರಸರಣಿಗಳು_WITH_STRING_H)
#  include <strings.h>
# endif
# ifdef HAVE_STAT_H
#  include <stat.h>
# endif
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ // NON-UNIX

#ಅನ್ತರ್ಭಾವ <assert.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STDINT_H
# include <stdint.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_INTTYPES_H
# include <inttypes.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_WCTYPE_H
# include <wctype.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <stdarg.h>
// older compilers do not define va_copy
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ va_copy
# define va_copy(dst, src)	((dst) = (src))
#ಆದರೆ_ಅಂತ್ಯ

// for offsetof()
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>

#ಆದರೆ defined(HAVE_SYS_SELECT_H) && \
	(!defined(HAVE_SYS_ಸಮಯ_H) || defined(SYS_SELECT_WITH_SYS_ಸಮಯ))
# include <sys/select.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SELECT
# ifdef HAVE_SYS_POLL_H
#  include <sys/poll.h>
# elif defined(MSWIN)
#  define HAVE_SELECT
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_POLL_H
#   include <poll.h>
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SODIUM
# include <sodium.h>
#ಆದರೆ_ಅಂತ್ಯ

// ================ end of the header file puzzle ===============

/*
 * For dynamically loaded imm library. Currently, only for Win32.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_IME
# ifndef FEAT_MBYTE_IME
#  define FEAT_MBYTE_IME
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * For dynamically loaded gettext library.  Currently, only for Win32.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_GETTEXT
# ifndef FEAT_GETTEXT
#  define FEAT_GETTEXT
# endif
// These are in os_win32.c
ಬಾಹ್ಯ ಅಕ್ಷರ *(*dyn_libintl_gettext)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid);
ಬಾಹ್ಯ ಅಕ್ಷರ *(*dyn_libintl_ngettext)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid_plural, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n);
ಬಾಹ್ಯ ಅಕ್ಷರ *(*dyn_libintl_bindtextdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirname);
ಬಾಹ್ಯ ಅಕ್ಷರ *(*dyn_libintl_bind_textdomain_codeset)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codeset);
ಬಾಹ್ಯ ಅಕ್ಷರ *(*dyn_libintl_textdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domainname);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ (*dyn_libintl_wputenv)(ಸ್ಥಿರಾಂಕ wchar_t *envstring);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ *dyn_libintl_nl_ಸಂದೇಶ_cat_cntr;
#ಆದರೆ_ಅಂತ್ಯ


/*
 * The _() stuff is for using gettext().  It is a no-op when libintl.h is not
 * found or the +multilang feature is disabled.
 * Use NGETTEXT(single, multi, number) to get plural behavior:
 * - single - message for singular form
 * - multi  - message for plural form
 * - number - the count
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
# ifdef DYNAMIC_GETTEXT
#  define _(x) (*dyn_libintl_gettext)((ಅಕ್ಷರ *)(x))
#  define NGETTEXT(x, xs, n) (*dyn_libintl_ngettext)((ಅಕ್ಷರ *)(x), (ಅಕ್ಷರ *)(xs), (n))
#  define N_(x) x
#  define bindtextdomain(domain, dir) (*dyn_libintl_bindtextdomain)((domain), (dir))
#  define bind_textdomain_codeset(domain, codeset) (*dyn_libintl_bind_textdomain_codeset)((domain), (codeset))
#  ಆದರೆ !defined(HAVE_BIND_TEXTDOMAIN_CODESET)
#   define HAVE_BIND_TEXTDOMAIN_CODESET 1
#  endif
#  define textdomain(domain) (*dyn_libintl_textdomain)(domain)
#  define libintl_wputenv(envstring) (*dyn_libintl_wputenv)(envstring)
# ಇಲ್ಲದಿದ್ದರೆ
#  include <libintl.h>
#  define _(x) gettext((ಅಕ್ಷರ *)(x))
#  define NGETTEXT(x, xs, n) ngettext((x), (xs), (n))
#  ifdef gettext_noop
#   define N_(x) gettext_noop(x)
#  ಇಲ್ಲದಿದ್ದರೆ
#   define N_(x) x
#  endif
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define _(x) ((ಅಕ್ಷರ *)(x))
# define NGETTEXT(x, xs, n) (((n) == 1) ? (ಅಕ್ಷರ *)(x) : (ಅಕ್ಷರ *)(xs))
# define N_(x) x
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# define bindtextdomain(x, y) ""
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# define bind_textdomain_codeset(x, y) //ಖಾಲಿಗಳು
# ifdef textdomain
#  undef textdomain
# endif
# define textdomain(x) //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Flags for update_screen().
 * The higher the value, the higher the priority.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_VALID_NO_UPDATE	 5  // no new changes, keep the command line if
				    // possible
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_VALID		10  // buffer not changed, or changes marked
				    // with b_mod_*
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_INVERTED		20  // redisplay inverted part that changed
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_INVERTED_ALL	25  // redisplay whole inverted part
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_REDRAW_ತುದಿ		30  // display first w_upd_rows screen lines
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_SOME_VALID		35  // like UPD_NOT_VALID but may scroll
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_NOT_VALID		40  // buffer needs complete redraw
#ವ್ಯಾಖ್ಯಾನಿಸು UPD_CLEAR		50  // screen messed up, clear it

// flags for screen_line()
#ವ್ಯಾಖ್ಯಾನಿಸು SLF_RIGHTLEFT	1
#ವ್ಯಾಖ್ಯಾನಿಸು SLF_POPUP	2
#ವ್ಯಾಖ್ಯಾನಿಸು SLF_INC_VCOL	4

#ವ್ಯಾಖ್ಯಾನಿಸು MB_FILLER_CHAR '<'  // character used when a double-width character
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
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_WROW	0x01	// w_wrow (window row) is valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_WCOL	0x02	// w_wcol (window col) is valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_VIRTCOL	0x04	// w_virtcol (file col) is valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CHEIGHT	0x08	// w_cline_height and w_cline_folded valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_CROW	0x10	// w_cline_row is valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_BOTLINE	0x20	// w_botline and w_empty_rows are valid
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_BOTLINE_AP 0x40	// w_botline is approximated
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_TOPLINE	0x80	// w_topline is valid (for cursor position)

// Values for w_popup_flags.
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_IS_POPUP	0x01	// this is a popup window
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_HIDDEN	0x02	// popup is not displayed
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_HIDDEN_FORCE 0x04	// popup is explicitly set to not be displayed
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_CURSORLINE	0x08	// popup is highlighting at the cursorline
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_ON_CMDLINE	0x10	// popup overlaps command line
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_DRAG	0x20	// popup can be moved by dragging border
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_DRAGALL	0x40	// popup can be moved by dragging everywhere
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_RESIZE	0x80	// popup can be resized by dragging
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_MAPPING	0x100	// mapping keys
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_INFO	0x200	// used for info of popup menu
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_INFO_MENU	0x400	// align info popup with popup menu
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_POSINVERT	0x800	// vertical position can be inverted
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_OPACITY 0x1000	// popup has opacity/transparency setting
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_CLIPWINDOW	0x2000	// confine popup to its host window's rect
#ವ್ಯಾಖ್ಯಾನಿಸು POPF_GLOBAL	0x4000	// popup is global (not tabpage local)

// flags used in w_popup_handled
#ವ್ಯಾಖ್ಯಾನಿಸು POPUP_HANDLED_1	    0x01    // used by mouse_find_win()
#ವ್ಯಾಖ್ಯಾನಿಸು POPUP_HANDLED_2	    0x02    // used by popup_do_filter()
#ವ್ಯಾಖ್ಯಾನಿಸು POPUP_HANDLED_3	    0x04    // used by popup_check_cursor_pos()
#ವ್ಯಾಖ್ಯಾನಿಸು POPUP_HANDLED_4	    0x08    // used by may_update_popup_mask()
#ವ್ಯಾಖ್ಯಾನಿಸು POPUP_HANDLED_5	    0x10    // used by update_popups() and
				    // update_popup_images()

/*
 * Terminal highlighting attribute bits.
 * Attributes above HL_ALL are used for syntax highlighting.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HL_ಸಾಮಾನ್ಯ		0x00
#ವ್ಯಾಖ್ಯಾನಿಸು HL_INVERSE		0x01
#ವ್ಯಾಖ್ಯಾನಿಸು HL_BOLD			0x02
#ವ್ಯಾಖ್ಯಾನಿಸು HL_ITALIC		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು HL_UNDERLINE		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು HL_UNDERCURL		0x10
#ವ್ಯಾಖ್ಯಾನಿಸು HL_UNDERDOUBLE		0x20
#ವ್ಯಾಖ್ಯಾನಿಸು HL_UNDERDOTTED		0x40
#ವ್ಯಾಖ್ಯಾನಿಸು HL_UNDERDASHED		0x80
#ವ್ಯಾಖ್ಯಾನಿಸು HL_STANDOUT		0x100
#ವ್ಯಾಖ್ಯಾನಿಸು HL_NOCOMBINE		0x200
#ವ್ಯಾಖ್ಯಾನಿಸು HL_STRIKETHROUGH	0x400
#ವ್ಯಾಖ್ಯಾನಿಸು HL_ALL			0x7ff

// special attribute addition: Put message in history
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_HIST		0x1000

/*
 * Values for State.
 *
 * The lower bits up to 0x80 are used to distinguish normal/visual/op_pending
 * /cmdline/insert/replace/terminal mode.  This is used for mapping.  If none
 * of these bits are set, no mapping is done.  See the comment above do_map().
 * The upper bits are used to distinguish between other states and variants of
 * the base modes.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_ಸಾಮಾನ್ಯ	0x01	// Normal mode, command expected
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_VISUAL	0x02	// Visual mode - use get_real_state()
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_OP_PENDING	0x04	// Normal mode, operator is pending - use
				// get_real_state()
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_CMDLINE	0x08	// Editing the command line
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_ಸೇರಿಸು	0x10	// Insert mode, also for Replace mode
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_LANGMAP	0x20	// Language mapping, can be combined with
				// MODE_INSERT and MODE_CMDLINE
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_SELECT	0x40	// Select mode, use get_real_state()
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_TERMINAL	0x80	// Terminal mode

#ವ್ಯಾಖ್ಯಾನಿಸು MAP_ALL_MODES	0xff    // all mode bits used for mapping

#ವ್ಯಾಖ್ಯಾನಿಸು REPLACE_FLAG	0x100	// Replace mode flag
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_REPLACE	(REPLACE_FLAG | ಬಹುಲಕ_ಸೇರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು VREPLACE_FLAG	0x200	// Virtual-replace mode flag
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_VREPLACE	(REPLACE_FLAG | VREPLACE_FLAG | ಬಹುಲಕ_ಸೇರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_LREPLACE	(REPLACE_FLAG | ಬಹುಲಕ_LANGMAP)

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY (0x1000 | ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
				// Normal mode, busy with a command
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_HITRETURN	(0x2000 | ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
				// waiting for return or command
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_ASKMORE	0x3000	// Asking if you want --more--
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_SETWSIZE	0x4000	// window size has changed
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_EXTERNCMD	0x5000	// executing an external command
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_SHOWMATCH	(0x6000 | ಬಹುಲಕ_ಸೇರಿಸು) // show matching paren
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_CONFIRM	0x7000	// ":confirm" prompt
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_ALL	0xffff

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ	4	// max mode length used by get_mode(),
				// including the terminating NUL

// directions
#ವ್ಯಾಖ್ಯಾನಿಸು FORWARD			1
#ವ್ಯಾಖ್ಯಾನಿಸು BACKWARD		(-1)
#ವ್ಯಾಖ್ಯಾನಿಸು FORWARD_FILE		3
#ವ್ಯಾಖ್ಯಾನಿಸು BACKWARD_FILE		(-3)

// return values for functions
#ಆದರೆ !(defined(OK) && (OK == 1))
// OK already defined to 1 in MacOS X curses, skip this
# define OK			1
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು FAIL			0
#ವ್ಯಾಖ್ಯಾನಿಸು NOTDONE			2   // not OK or FAIL but skipped

// flags for b_flags
#ವ್ಯಾಖ್ಯಾನಿಸು BF_RECOVERED	0x01	// buffer has been recovered
#ವ್ಯಾಖ್ಯಾನಿಸು BF_CHECK_RO	0x02	// need to check readonly when loading file
				// into buffer (set by ":e", may be reset by
				// ":buf"
#ವ್ಯಾಖ್ಯಾನಿಸು BF_NEVERLOADED	0x04	// file has never been loaded into buffer,
				// many variables still need to be set
#ವ್ಯಾಖ್ಯಾನಿಸು BF_NOTEDITED	0x08	// Set when file name is changed after
				// starting to edit, reset when file is
				// written out.
#ವ್ಯಾಖ್ಯಾನಿಸು BF_NEW		0x10	// file didn't exist when editing started
#ವ್ಯಾಖ್ಯಾನಿಸು BF_NEW_W	0x20	// Warned for BF_NEW and file created
#ವ್ಯಾಖ್ಯಾನಿಸು BF_READERR	0x40	// got errors while reading the file
#ವ್ಯಾಖ್ಯಾನಿಸು BF_DUMMY	0x80	// dummy buffer, only used internally
#ವ್ಯಾಖ್ಯಾನಿಸು BF_PRESERVED	0x100	// ":preserve" was used
#ವ್ಯಾಖ್ಯಾನಿಸು BF_SYN_ಗಣ	0x200	// 'syntax' option was set
#ವ್ಯಾಖ್ಯಾನಿಸು BF_NO_SEA	0x400	// no swap_exists_action (ATTENTION prompt)

// Mask to check for flags that prevent normal writing
#ವ್ಯಾಖ್ಯಾನಿಸು BF_WRITE_ಮುಖವಾಡ	(BF_NOTEDITED + BF_NEW + BF_READERR)

/*
 * values for xp_context when doing command line completion
 */
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_UNSUCCESSFUL	(-2)
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_OK		(-1)
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_NOTHING		0
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_COMMANDS		1
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILES		2
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_DIRECTORIES	3
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SETTINGS		4
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_BOOL_SETTINGS	5
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ಟ್ಯಾಗುಗಳು		6
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_OLD_SETTING	7
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_HELP		8
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_BUFFERS		9
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_EVENTS		10
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_MENUS		11
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SYNTAX		12
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_HIGHLIGHT	13
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_AUGROUP		14
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_VARS	15
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_MAPPINGS		16
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES	17
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FUNCTIONS	18
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_FUNC	19
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ಅಭಿವ್ಯಕ್ತಿ	20
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_MENUNAMES	21
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_COMMANDS	22
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_CMD_FLAGS	23
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_NARGS	24
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_COMPLETE	25
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ENV_VARS		26
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_LANGUAGE		27
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_COLORS		28
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_COMPILER		29
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_DEFINED	30
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_LIST	31
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SHELLCMD		32
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_CSCOPE		33
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SIGN		34
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_PROFILE		35
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_BEHAVE		36
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILETYPE		37
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILES_ಒಳ_ಮಾರ್ಗ	38
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_OWNSYNTAX	39
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_LOCALES		40
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_HISTORY		41
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER		42
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SYNTIME		43
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_ADDR_TYPE	44
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_PACKADD		45
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_MESSAGES		46
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_MAPCLEAR		47
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ARGLIST		48
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_DIFF_BUFFERS	49
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_DISASSEMBLE	50
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_BREAKPOINT	51
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SCRIPTNAMES	52
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_RUNTIME		53
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_STRING_SETTING	54
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SETTING_ಕಳೆ	55
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ARGOPT		56
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_TERMINALOPT	57
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_KEYMAP		58
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_DIRS_ಒಳ_CDPATH	59
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_SHELLCMDLINE	60
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FINDFUNC		61
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_HIGHLIGHT_ಗುಂಪು  62
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILETYPECMD	63
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ನಮೂನೆ_ಒಳ_BUF	64
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_RETAB		65
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_USER_COMPLETEOPT	66


// Values for exmode_active (0 is no exmode)
#ವ್ಯಾಖ್ಯಾನಿಸು EXMODE_ಸಾಮಾನ್ಯ		1
#ವ್ಯಾಖ್ಯಾನಿಸು EXMODE_VIM		2

// Values for nextwild() and ExpandOne().  See ExpandOne() for meaning.
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_FREE		1
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_EXPAND_FREE	2
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_EXPAND_KEEP	3
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ಮುಂದಿನ		4
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_PREV		5
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ALL		6
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_LONGEST		7
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ALL_KEEP		8
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_CANCEL		9
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_APPLY		10
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_PAGEUP		11
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_PAGEDOWN		12

#ವ್ಯಾಖ್ಯಾನಿಸು WILD_LIST_NOTFOUND	    0x01
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_HOME_REPLACE	    0x02
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_USE_NL		    0x04
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_NO_BEEP		    0x08
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ಕೂಡಿಸು_SLASH		    0x10
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_KEEP_ALL		    0x20
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_SILENT		    0x40
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ESCAPE		    0x80
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ICASE		    0x100
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_ALLLINKS		    0x200
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_IGNORE_COMPLETESLASH   0x400
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_NOERROR		    0x800  // sets EW_NOERROR
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_BUFLASTUSED	    0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು BUF_DIFF_FILTER		    0x2000
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_NOSELECT		    0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_MAY_EXPAND_ನಮೂನೆ	    0x8000
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_FUNC_TRIGGER	    0x10000 // called from wildtrigger()
#ವ್ಯಾಖ್ಯಾನಿಸು WILD_NOINSERT		    0x20000

// Flags for expand_wildcards()
#ವ್ಯಾಖ್ಯಾನಿಸು EW_DIR		0x01	// include directory names
#ವ್ಯಾಖ್ಯಾನಿಸು EW_FILE		0x02	// include file names
#ವ್ಯಾಖ್ಯಾನಿಸು EW_NOTFOUND	0x04	// include not found names
#ವ್ಯಾಖ್ಯಾನಿಸು EW_ADDSLASH	0x08	// append slash to directory name
#ವ್ಯಾಖ್ಯಾನಿಸು EW_KEEPALL	0x10	// keep all matches
#ವ್ಯಾಖ್ಯಾನಿಸು EW_SILENT	0x20	// don't print "1 returned" from shell
#ವ್ಯಾಖ್ಯಾನಿಸು EW_EXEC		0x40	// executable files
#ವ್ಯಾಖ್ಯಾನಿಸು EW_ಮಾರ್ಗ		0x80	// search in 'path' too
#ವ್ಯಾಖ್ಯಾನಿಸು EW_ICASE	0x100	// ignore case
#ವ್ಯಾಖ್ಯಾನಿಸು EW_NOERROR	0x200	// no error for bad regexp
#ವ್ಯಾಖ್ಯಾನಿಸು EW_NOTWILD	0x400	// add match with literal name if exists
#ವ್ಯಾಖ್ಯಾನಿಸು EW_KEEPDOLLAR	0x800	// do not escape $, $var is expanded
// Note: mostly EW_NOTFOUND and EW_SILENT are mutually exclusive: EW_NOTFOUND
// is used when executing commands and EW_SILENT for interactive expanding.
#ವ್ಯಾಖ್ಯಾನಿಸು EW_ALLLINKS	0x1000	// also links not pointing to existing file
#ವ್ಯಾಖ್ಯಾನಿಸು EW_SHELLCMD	0x2000	// called from expand_shellcmd(), don't check
				// if executable is in $PATH
#ವ್ಯಾಖ್ಯಾನಿಸು EW_DODOT	0x4000	// also files starting with a dot
#ವ್ಯಾಖ್ಯಾನಿಸು EW_EMPTYOK	0x8000	// no matches is not an error
#ವ್ಯಾಖ್ಯಾನಿಸು EW_NOTENV	0x10000	// do not expand environment variables
#ವ್ಯಾಖ್ಯಾನಿಸು EW_CDPATH	0x20000	// search in 'cdpath' too

// Flags for find_file_*() functions.
#ವ್ಯಾಖ್ಯಾನಿಸು FINDFILE_FILE	0	// only files
#ವ್ಯಾಖ್ಯಾನಿಸು FINDFILE_DIR	1	// only directories
#ವ್ಯಾಖ್ಯಾನಿಸು FINDFILE_BOTH	2	// files and directories

#ಆದರೆ defined(FEAT_TABPANEL)
# define COLUMNS_WITHOUT_TPL()		(Columns - tabpanel_ಅಗಲ())
# define TPL_LCOL()			tabpanel_leftcol()
#ಇಲ್ಲದಿದ್ದರೆ
# define COLUMNS_WITHOUT_TPL()		Columns
# define TPL_LCOL()			0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು W_ENDCOL(wp)	((wp)->w_wincol + (wp)->w_ಅಗಲ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# define W_WINROW(wp)	((wp)->w_winrow + (wp)->w_winbar_ಎತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
# define W_WINROW(wp)	(wp->w_winrow)
#ಆದರೆ_ಅಂತ್ಯ

// Values for the find_pattern_in_path() function args 'type' and 'action':
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_ANY	1
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_DEFINE	2
#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_ಮಾರ್ಗ	3

#ವ್ಯಾಖ್ಯಾನಿಸು ACTION_SHOW	1
#ವ್ಯಾಖ್ಯಾನಿಸು ACTION_GOTO	2
#ವ್ಯಾಖ್ಯಾನಿಸು ACTION_ವಿಭಜಿಸು	3
#ವ್ಯಾಖ್ಯಾನಿಸು ACTION_SHOW_ALL	4
#ವ್ಯಾಖ್ಯಾನಿಸು ACTION_EXPAND	5

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
# define SST_MIN_ENTRIES 150	// minimal size for state stack array
# define SST_MAX_ENTRIES 1000	// maximal size for state stack array
# define SST_FIX_ಸ್ಥಿತಿಗಳು	 7	// size of sst_stack[].
# define SST_DIST	 16	// normal distance between entries
# define SST_INVALID	((synstate_T *)-1)	// invalid syn_state pointer

# define HL_CONTAINED	0x01	// not used on toplevel
# define HL_TRANSP	0x02	// has no highlighting
# define HL_ONELINE	0x04	// match within one line only
# define HL_HAS_EOL	0x08	// end pattern that matches with $
# define HL_SYNC_ಇಲ್ಲಿ	0x10	// sync point after this item (syncing only)
# define HL_SYNC_THERE	0x20	// sync point at current line (syncing only)
# define HL_ಹೊಂದಿಕೆ	0x40	// use match ID instead of item ID
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
#ಆದರೆ_ಅಂತ್ಯ

// Values for 'options' argument in do_search() and searchit()
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_REV    0x01  // go in reverse of previous dir.
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ECHO   0x02  // echo the search command and handle options
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ಸಂದೇಶ    0x0c  // give messages (yes, it's not 0x04)
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_NFMSG  0x08  // give all messages except not found
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_OPT    0x10  // interpret optional flags
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_HIS    0x20  // put search pattern in history
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ಕೊನೆ    0x40  // put cursor at end of match
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_NOOF   0x80  // don't add offset to position
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ಆರಂಭ 0x100  // start search without col offset
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_MARK  0x200  // set previous context mark
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_KEEP  0x400  // keep previous search pattern
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_PEEK  0x800  // peek for typed char, cancel search
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_ಲಂಬಸಾಲು  0x1000  // start at specified column instead of zero

// Values for find_ident_under_cursor()
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_IDENT	1	// find identifier (word)
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_STRING	2	// find any string (WORD)
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_EVAL	4	// include "->", "[]" and "."
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_NOERROR	8	// no error when no word found

// Values for file_name_in_line()
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_MESS	1	// give error message
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_EXP	2	// expand to path
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_HYP	4	// check for hypertext link
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_INCL	8	// apply 'includeexpr'
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_REL	16	// ".." and "./" are relative to the (current)
				// file instead of the current directory
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_UNESC	32	// remove backslashes used for escaping

// Values for buflist_getfile()
#ವ್ಯಾಖ್ಯಾನಿಸು GETF_SETMARK	0x01	// set pcmark before jumping
#ವ್ಯಾಖ್ಯಾನಿಸು GETF_ALT	0x02	// jumping to alternate file (not buf num)
#ವ್ಯಾಖ್ಯಾನಿಸು GETF_SWITCH	0x04	// respect 'switchbuf' settings when jumping

// Return values of getfile()
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_ERROR	    1	// normal error
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_NOT_WRITTEN 2	// "not written" error
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_SAME_FILE   0	// success, same file
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_OPEN_OTHER (-1)	// success, opened another file
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_ಬಳಸದ	    8
#ವ್ಯಾಖ್ಯಾನಿಸು GETFILE_SUCCESS(x)  ((x) <= 0)

// Values for buflist_new() flags
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_CURBUF	1	// may re-use curbuf for new buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_LISTED	2	// put new buffer in buffer list
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_DUMMY	4	// allocating dummy buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_NEW		8	// create a new buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_NOOPT	16	// don't copy options to existing buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_DUMMY_OK	32	// also find an existing dummy buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_REUSE	64	// may re-use number from buf_reuse
#ವ್ಯಾಖ್ಯಾನಿಸು BLN_NOCURWIN	128	// buffer is not associated with curwin

// Values for in_cinkeys()
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೀಲಿ_OPEN_FORW	0x101
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೀಲಿ_OPEN_ಹಿಂಭಾಗ	0x102
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೀಲಿ_COMPLETE	0x103	// end of completion

// Used for the first argument of do_map()
#ವ್ಯಾಖ್ಯಾನಿಸು MAPTYPE_MAP		0
#ವ್ಯಾಖ್ಯಾನಿಸು MAPTYPE_UNMAP		1
#ವ್ಯಾಖ್ಯಾನಿಸು MAPTYPE_NOREMAP		2
#ವ್ಯಾಖ್ಯಾನಿಸು MAPTYPE_UNMAP_LHS	3

// Values for "noremap" argument of ins_typebuf().  Also used for
// map->m_noremap and menu->noremap[].
#ವ್ಯಾಖ್ಯಾನಿಸು REMAP_YES	0	// allow remapping
#ವ್ಯಾಖ್ಯಾನಿಸು REMAP_NONE	(-1)	// no remapping
#ವ್ಯಾಖ್ಯಾನಿಸು REMAP_SCRIPT	(-2)	// remap script-local mappings only
#ವ್ಯಾಖ್ಯಾನಿಸು REMAP_SKIP	(-3)	// no remapping for first char

// Values for mch_call_shell() second argument
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_FILTER	1	// filtering text
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_EXPAND	2	// expanding wildcards
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_COOKED	4	// set term to cooked mode
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_DOOUT	8	// redirecting output
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_SILENT	16	// don't print error returned by command
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_READ	32	// read lines and insert into buffer
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_WRITE	64	// write lines from buffer

// Values returned by mch_nodetype()
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಂಟು_ಸಾಮಾನ್ಯ	0	// file or directory, check with mch_isdir()
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಂಟು_WRITABLE	1	// something we can write to (character
				// device, fifo, socket, ..)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಂಟು_OTHER	2	// non-writable thing (e.g., block device)

// Values for readfile() flags
#ವ್ಯಾಖ್ಯಾನಿಸು READ_NEW	0x01	// read a file into a new buffer
#ವ್ಯಾಖ್ಯಾನಿಸು READ_FILTER	0x02	// read filter output
#ವ್ಯಾಖ್ಯಾನಿಸು READ_STDIN	0x04	// read from stdin
#ವ್ಯಾಖ್ಯಾನಿಸು READ_ಬಫರ್	0x08	// read from curbuf (converting stdin)
#ವ್ಯಾಖ್ಯಾನಿಸು READ_DUMMY	0x10	// reading into a dummy buffer
#ವ್ಯಾಖ್ಯಾನಿಸು READ_KEEP_UNDO	0x20	// keep undo info
#ವ್ಯಾಖ್ಯಾನಿಸು READ_FIFO	0x40	// read from fifo or socket
#ವ್ಯಾಖ್ಯಾನಿಸು READ_NOWINENTER 0x80	// do not trigger BufWinEnter
#ವ್ಯಾಖ್ಯಾನಿಸು READ_NOFILE	0x100	// do not read a file, do trigger BufReadCmd

// Values for change_indent()
#ವ್ಯಾಖ್ಯಾನಿಸು INDENT_ಗಣ	1	// set indent
#ವ್ಯಾಖ್ಯಾನಿಸು INDENT_INC	2	// increase indent
#ವ್ಯಾಖ್ಯಾನಿಸು INDENT_DEC	3	// decrease indent

// Values for flags argument for findmatchlimit()
#ವ್ಯಾಖ್ಯಾನಿಸು FM_BACKWARD	0x01	// search backwards
#ವ್ಯಾಖ್ಯಾನಿಸು FM_FORWARD	0x02	// search forwards
#ವ್ಯಾಖ್ಯಾನಿಸು FM_BLOCKSTOP	0x04	// stop at start/end of block
#ವ್ಯಾಖ್ಯಾನಿಸು FM_SKIPCOMM	0x08	// skip comments (cursor must start outside)

// Values for action argument for do_buffer() and close_buffer()
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_GOTO	0	// go to specified buffer
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_ವಿಭಜಿಸು	1	// split window and go to specified buffer
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_UNLOAD	2	// unload specified buffer(s)
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_DEL	3	// delete specified buffer(s) from buflist
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_WIPE	4	// delete specified buffer(s) really
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_WIPE_REUSE 5	// like DOBUF_WIPE and keep number for reuse

// Values for start argument for do_buffer()
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_CURRENT	0	// "count" buffer from current buffer
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_ಮೊದಲ	1	// "count" buffer from first buffer
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_ಕೊನೆಯ	2	// "count" buffer from last buffer
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_MOD	3	// "count" mod. buffer from current buffer

// Values for flags argument of do_buffer()
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_FORCEIT	1	// :cmd!
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_NOPOPUP	2	// skip popup window buffers
#ವ್ಯಾಖ್ಯಾನಿಸು DOBUF_SKIPHELP	4	// skip or keep help buffers depending on b_help of the
				// starting buffer

// Values for sub_cmd and which_pat argument for search_regcomp()
// Also used for which_pat argument for searchit()
#ವ್ಯಾಖ್ಯಾನಿಸು RE_ಹುಡುಕು	0	// save/use pat in/from search_pattern
#ವ್ಯಾಖ್ಯಾನಿಸು RE_SUBST	1	// save/use pat in/from subst_pattern
#ವ್ಯಾಖ್ಯಾನಿಸು RE_BOTH		2	// save pat in both patterns
#ವ್ಯಾಖ್ಯಾನಿಸು RE_ಕೊನೆಯ		2	// use last used pattern if "pat" is NULL

// Second argument for vim_regcomp().
#ವ್ಯಾಖ್ಯಾನಿಸು RE_MAGIC	1	// 'magic' option
#ವ್ಯಾಖ್ಯಾನಿಸು RE_STRING	2	// match in string instead of buffer text
#ವ್ಯಾಖ್ಯಾನಿಸು RE_STRICT	4	// don't allow [abc] without ]
#ವ್ಯಾಖ್ಯಾನಿಸು RE_AUTO		8	// automatic engine selection

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
// values for reg_do_extmatch
# define REX_ಗಣ	1	// to allow \z\(...\),
# define REX_USE	2	// to allow \z\1 et al.
# define REX_ALL	(REX_ಗಣ | REX_USE)
#ಆದರೆ_ಅಂತ್ಯ

// Return values for fullpathcmp()
// Note: can use (fullpathcmp() & FPC_SAME) to check for equal files
#ವ್ಯಾಖ್ಯಾನಿಸು FPC_SAME	1	// both exist and are the same file.
#ವ್ಯಾಖ್ಯಾನಿಸು FPC_DIFF	2	// both exist and are different files.
#ವ್ಯಾಖ್ಯಾನಿಸು FPC_NOTX	4	// both don't exist.
#ವ್ಯಾಖ್ಯಾನಿಸು FPC_DIFFX	6	// one of them doesn't exist.
#ವ್ಯಾಖ್ಯಾನಿಸು FPC_SAMEX	7	// both don't exist and file names are same.

// flags for do_ecmd()
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_HIDE	0x01	// don't free the current buffer
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_ಗಣ_HELP	0x02	// set b_help flag of (new) buffer before
				// opening file
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_OLDBUF	0x04	// use existing buffer if it exists
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_FORCEIT	0x08	// ! used in Ex command
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_ADDBUF	0x10	// don't edit, just add to buffer list
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_ALTBUF	0x20	// like ECMD_ADDBUF and set the alternate file
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_NOWINENTER	0x40	// do not trigger BufWinEnter

// for lnum argument in do_ecmd()
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_LASTL	(linenr_T)0	// use last position in loaded file
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_ಕೊನೆಯ	((linenr_T)-1)	// use last position in all files
#ವ್ಯಾಖ್ಯಾನಿಸು ECMD_ONE	(linenr_T)1	// use first line

// flags for do_cmdline()
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_VERBOSE	0x01	// included command in error message
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_NOWAIT	0x02	// don't call wait_return() and friends
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_REPEAT	0x04	// repeat exec. until getline() returns NULL
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_KEYTYPED	0x08	// don't reset KeyTyped
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_EXCRESET	0x10	// reset exception environment (for debugging)
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_KEEPLINE  0x20	// keep typed line for repeating with "."
#ವ್ಯಾಖ್ಯಾನಿಸು DOCMD_RANGEOK	0x40	// can use a range without ":" in Vim9 script

// flags for beginline()
#ವ್ಯಾಖ್ಯಾನಿಸು BL_WHITE	1	// cursor on first non-white in the line
#ವ್ಯಾಖ್ಯಾನಿಸು BL_SOL		2	// use 'sol' option
#ವ್ಯಾಖ್ಯಾನಿಸು BL_FIX		4	// don't leave cursor on a NUL

// flags for mf_sync()
#ವ್ಯಾಖ್ಯಾನಿಸು MFS_ALL		1	// also sync blocks with negative numbers
#ವ್ಯಾಖ್ಯಾನಿಸು MFS_STOP	2	// stop syncing when a character is available
#ವ್ಯಾಖ್ಯಾನಿಸು MFS_FLUSH	4	// flushed file to disk
#ವ್ಯಾಖ್ಯಾನಿಸು MFS_ZERO	8	// only write block 0

// flags for buf_copy_options()
#ವ್ಯಾಖ್ಯಾನಿಸು BCO_ENTER	1	// going to enter the buffer
#ವ್ಯಾಖ್ಯಾನಿಸು BCO_ALWAYS	2	// always copy the options
#ವ್ಯಾಖ್ಯಾನಿಸು BCO_NOHELP	4	// don't touch the help related options

// flags for do_put()
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_FIXINDENT	1	// make indent look nice
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_CURSEND	2	// leave cursor after end of new text
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_CURSLINE	4	// leave cursor on last line of new text
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_ಸಾಲು	8	// put register as lines
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_ಸಾಲು_ವಿಭಜಿಸು	16	// split line for linewise register
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_ಸಾಲು_FORWARD 32	// put linewise register below Visual sel.
#ವ್ಯಾಖ್ಯಾನಿಸು PUT_ಖಂಡ_INNER 64      // in block mode, do not add trailing spaces

// flags for set_indent()
#ವ್ಯಾಖ್ಯಾನಿಸು SIN_ಬದಲಾಯಿತು	1	// call changed_bytes() when line changed
#ವ್ಯಾಖ್ಯಾನಿಸು SIN_ಸೇರಿಸು	2	// insert indent before existing text
#ವ್ಯಾಖ್ಯಾನಿಸು SIN_UNDO	4	// save line for undo before changing it

// flags for insertchar()
#ವ್ಯಾಖ್ಯಾನಿಸು INSCHAR_FORMAT	1	// force formatting
#ವ್ಯಾಖ್ಯಾನಿಸು INSCHAR_DO_COM	2	// format comments
#ವ್ಯಾಖ್ಯಾನಿಸು INSCHAR_CTRLV	4	// char typed just after CTRL-V
#ವ್ಯಾಖ್ಯಾನಿಸು INSCHAR_NO_FEX	8	// don't use 'formatexpr'
#ವ್ಯಾಖ್ಯಾನಿಸು INSCHAR_COM_LIST 16	// format comments with list/2nd line indent

// flags for open_line()
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_DELSPACES	0x01    // delete spaces after cursor
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_DO_COM		0x02    // format comments
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_KEEPTRAIL	0x04    // keep trailing spaces
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_MARKFIX	0x08    // fix mark positions
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_COM_LIST	0x10    // format comments with list/2nd line indent
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_FORMAT		0x20    // formatting long comment
#ವ್ಯಾಖ್ಯಾನಿಸು OPENLINE_FORCE_INDENT	0x40    // use second_line_indent without indent logic

// There are five history tables:
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_CMD	0	// colon commands
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_ಹುಡುಕು	1	// search commands
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_ಅಭಿವ್ಯಕ್ತಿ	2	// expressions (from entering = register)
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_ಒಳಹರಿವು	3	// input() lines
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_DEBUG	4	// debug commands
#ವ್ಯಾಖ್ಯಾನಿಸು HIST_ಎಣಿಕೆ	5	// number of history tables

// The type numbers are fixed for backwards compatibility.
#ವ್ಯಾಖ್ಯಾನಿಸು BARTYPE_ಆವೃತ್ತಿ 1
#ವ್ಯಾಖ್ಯಾನಿಸು BARTYPE_HISTORY 2
#ವ್ಯಾಖ್ಯಾನಿಸು BARTYPE_ರಿಜಿಸ್ಟರ್ 3
#ವ್ಯಾಖ್ಯಾನಿಸು BARTYPE_MARK 4

#ವ್ಯಾಖ್ಯಾನಿಸು VIMINFO_ಆವೃತ್ತಿ 4
#ವ್ಯಾಖ್ಯಾನಿಸು VIMINFO_ಆವೃತ್ತಿ_WITH_HISTORY 2
#ವ್ಯಾಖ್ಯಾನಿಸು VIMINFO_ಆವೃತ್ತಿ_WITH_REGISTERS 3
#ವ್ಯಾಖ್ಯಾನಿಸು VIMINFO_ಆವೃತ್ತಿ_WITH_MARKS 4

/*
 * Values for do_tag().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DT_ಟ್ಯಾಗ್		1	// jump to newer position or same tag again
#ವ್ಯಾಖ್ಯಾನಿಸು DT_ತೆಗೆ		2	// jump to older position
#ವ್ಯಾಖ್ಯಾನಿಸು DT_ಮುಂದಿನ		3	// jump to next match of same tag
#ವ್ಯಾಖ್ಯಾನಿಸು DT_PREV		4	// jump to previous match of same tag
#ವ್ಯಾಖ್ಯಾನಿಸು DT_ಮೊದಲ	5	// jump to first match of same tag
#ವ್ಯಾಖ್ಯಾನಿಸು DT_ಕೊನೆಯ		6	// jump to first match of same tag
#ವ್ಯಾಖ್ಯಾನಿಸು DT_SELECT	7	// jump to selection from list
#ವ್ಯಾಖ್ಯಾನಿಸು DT_HELP		8	// like DT_TAG, but no wildcards
#ವ್ಯಾಖ್ಯಾನಿಸು DT_JUMP		9	// jump to new tag or selection from list
#ವ್ಯಾಖ್ಯಾನಿಸು DT_CSCOPE	10	// cscope find command (like tjump)
#ವ್ಯಾಖ್ಯಾನಿಸು DT_LTAG		11	// tag using location list
#ವ್ಯಾಖ್ಯಾನಿಸು DT_FREE		99	// free cached matches

/*
 * flags for find_tags().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_HELP	1	// only search for help tags
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_ಹೆಸರುಗಳು	2	// only return name of tag
#ವ್ಯಾಖ್ಯಾನಿಸು	ಟ್ಯಾಗ್_REGEXP	4	// use tag pattern as regexp
#ವ್ಯಾಖ್ಯಾನಿಸು	ಟ್ಯಾಗ್_NOIC	8	// don't always ignore case
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
# define ಟ್ಯಾಗ್_CSCOPE	16	// cscope tag
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_VERBOSE	32	// message verbosity
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_INS_COMP	64	// Currently doing insert completion
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_KEEP_LANG	128	// keep current language
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_NO_TAGFUNC	256	// do not use 'tagfunc'

#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_MANY	300	// When finding many tags (for completion),
				// find up to this many tags

/*
 * Types of dialogs passed to do_vim_dialog().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_GENERIC	0
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ERROR	1
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_WARNING	2
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_INFO	3
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_QUESTION	4
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ಕೊನೆಯ_TYPE	4	// sentinel value

/*
 * Return values for functions like gui_yesnocancel()
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_YES		2
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_NO		3
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_CANCEL	4
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ALL		5
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_DISCARDALL  6

/*
 * arguments for win_split()
 */
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_ಸ್ಥಳ	0x01	// require enough room
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_VERT	0x02	// split/equalize vertically
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_HOR		0x04	// equalize horizontally
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_ತುದಿ		0x08	// window at top-left of shell
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_BOT		0x10	// window at bottom-right of shell
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_HELP	0x20	// creating the help window
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_ಕೆಳಗೆ	0x40	// put new window below/right
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_ABOVE	0x80	// put new window above/left
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_NEWLOC	0x100	// don't copy location list
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_FORCE_ಸ್ಥಳ	0x200	// ignore "not enough room" errors
#ವ್ಯಾಖ್ಯಾನಿಸು WSP_QUICKFIX	0x400	// creating the quickfix window

/*
 * arguments for gui_set_shellsize()
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RESIZE_VERT	1	// resize vertically
#ವ್ಯಾಖ್ಯಾನಿಸು RESIZE_HOR	2	// resize horizontally
#ವ್ಯಾಖ್ಯಾನಿಸು RESIZE_BOTH	15	// resize in both directions

/*
 * flags for check_changed()
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CCGD_AW		1	// do autowrite if buffer was changed
#ವ್ಯಾಖ್ಯಾನಿಸು CCGD_MULTWIN	2	// check also when several wins for the buf
#ವ್ಯಾಖ್ಯಾನಿಸು CCGD_FORCEIT	4	// ! used
#ವ್ಯಾಖ್ಯಾನಿಸು CCGD_ALLBUF	8	// may write all buffers
#ವ್ಯಾಖ್ಯಾನಿಸು CCGD_EXCMD	16	// may suggest using !

/*
 * "flags" values for option-setting functions.
 * When OPT_GLOBAL and OPT_LOCAL are both missing, set both local and global
 * values, get local value.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_FREE	0x01	// free old value if it was allocated
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_GLOBAL	0x02	// use global value
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_LOCAL	0x04	// use local value
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_MODELINE	0x08	// option in modeline
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_WINONLY	0x10	// only set window-local options
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_NOWIN	0x20	// don't set window-local options
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_ONECOLUMN	0x40	// list options one per line
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_NO_REDRAW	0x80	// ignore redraw flags on option
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_SKIPRTP	0x100	// "skiprtp" in 'sessionoptions'

// Magic chars used in confirm dialog strings
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_BUTTON_SEP	'\n'
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_HOTKEY_CHAR	'&'

// Values for "starting"
#ವ್ಯಾಖ್ಯಾನಿಸು NO_SCREEN	2	// no screen updating yet
#ವ್ಯಾಖ್ಯಾನಿಸು NO_BUFFERS	1	// not all buffers loaded yet
//			0	   not starting anymore

// Values for swap_exists_action: what to do when swap file already exists
#ವ್ಯಾಖ್ಯಾನಿಸು SEA_NONE	0	// don't use dialog
#ವ್ಯಾಖ್ಯಾನಿಸು SEA_DIALOG	1	// use dialog when possible
#ವ್ಯಾಖ್ಯಾನಿಸು SEA_QUIT	2	// quit editing the file
#ವ್ಯಾಖ್ಯಾನಿಸು SEA_RECOVER	3	// recover the file
#ವ್ಯಾಖ್ಯಾನಿಸು SEA_READONLY	4	// no dialog, mark buffer as read-only

/*
 * Minimal size for block 0 of a swap file.
 * NOTE: This depends on size of struct block0! It's not done with a sizeof(),
 * because struct block0 is defined in memline.c (Sorry).
 * The maximal block size is arbitrary.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MIN_ಅದಲುಬದಲು_ಪುಟ_ಗಾತ್ರ 1048
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಅದಲುಬದಲು_ಪುಟ_ಗಾತ್ರ 50000

// Special values for current_sctx.sc_sid.
#ವ್ಯಾಖ್ಯಾನಿಸು SID_MODELINE	(-1)	// when using a modeline
#ವ್ಯಾಖ್ಯಾನಿಸು SID_CMDARG	(-2)	// for "--cmd" argument
#ವ್ಯಾಖ್ಯಾನಿಸು SID_CARG	(-3)	// for "-c" argument
#ವ್ಯಾಖ್ಯಾನಿಸು SID_ENV		(-4)	// for sourcing environment variable
#ವ್ಯಾಖ್ಯಾನಿಸು SID_ERROR	(-5)	// option was reset because of an error
#ವ್ಯಾಖ್ಯಾನಿಸು SID_NONE	(-6)	// don't set scriptID
#ವ್ಯಾಖ್ಯಾನಿಸು SID_WINLAYOUT	(-7)	// changing window size

/*
 * Events for autocommands. Must be kept in sync with "event_tab".
 */
ಎಣಿಕೆ auto_event
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

    ಅಂಕೆ_EVENTS,			// MUST be the last one
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ auto_event event_T;

/*
 * Values for index in highlight_attr[].
 * When making changes, also update HL_FLAGS below!
 * And update the default value of 'highlight': HIGHLIGHT_INIT in optiondefs.h
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
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
    , HLF_ಕೂಡಿಸು	    // Added diff line
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
    , HLF_ತೆಗೆ	    // popup window body
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
    , HLF_ಸಂದೇಶ	    // message area
    , HLF_TPL	    // tabpanel
    , HLF_TPLS	    // tabpanel selected
    , HLF_TPLF	    // tabpanel filler
    , HLF_PRI	    // "preinsert" in 'completeopt'
    , HLF_WIN	    // window colour
    , HLF_ಎಣಿಕೆ	    // MUST be the last one
} hlf_T;

// The HL_FLAGS must be in the same order as the HLF_ enums!
// When changing this also adjust the default for 'highlight'.
#ವ್ಯಾಖ್ಯಾನಿಸು HL_FLAGS {'8', '~', '@', 'd', 'e', 'h', 'i', 'l', 'y', 'm', 'M', \
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    SMT_ALL = 0		    // Move to "all" words
    , SMT_BAD		    // Move to "bad" words only
    , SMT_RARE		    // Move to "rare" words only
} smt_T;
/*
 * Boolean constants
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TRUE
# define FALSE	0	    // note: this is an int, not a long!
# define TRUE	1
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAYBE	2	    // sometimes used for a variant on TRUE

#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ALWAYS 9	    // must be different from TRUE and FALSE

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
// If "--log logfile" was used or ch_logfile() was called then log some or all
// terminal output.
# define MAY_WANT_ಗಮ್ಯ_LOG_THIS ಆದರೆ (ch_log_output == FALSE) ch_log_output = TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
// no logging support
# define MAY_WANT_ಗಮ್ಯ_LOG_THIS
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UINT32_T
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UINT32_TYPEDEF UINT32_T;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Operator IDs; The order must correspond to opchars[] in ops.c!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು OP_NOP		0	// no pending operation
#ವ್ಯಾಖ್ಯಾನಿಸು OP_DELETE	1	// "d"  delete operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_YANK		2	// "y"  yank operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_CHANGE	3	// "c"  change operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_LSHIFT	4	// "<"  left shift operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_RSHIFT	5	// ">"  right shift operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FILTER	6	// "!"  filter operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_TILDE	7	// "g~" switch case operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_INDENT	8	// "="  indent operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FORMAT	9	// "gq" format operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_COLON	10	// ":"  colon operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_ಊರ್ಧ್ವ	11	// "gU" make upper case operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_ಅಧೋ	12	// "gu" make lower case operator
#ವ್ಯಾಖ್ಯಾನಿಸು OP_JOIN		13	// "J"  join operator, only for Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು OP_JOIN_NS	14	// "gJ"  join operator, only for Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು OP_ROT13	15	// "g?" rot-13 encoding
#ವ್ಯಾಖ್ಯಾನಿಸು OP_REPLACE	16	// "r"  replace chars, only for Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು OP_ಸೇರಿಸು	17	// "I"  Insert column, only for Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು OP_APPEND	18	// "A"  Append column, only for Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLD		19	// "zf" define a fold
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDOPEN	20	// "zo" open folds
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDOPENREC	21	// "zO" open folds recursively
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDCLOSE	22	// "zc" close folds
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDCLOSEREC	23	// "zC" close folds recursively
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDDEL	24	// "zd" delete folds
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FOLDDELREC	25	// "zD" delete folds recursively
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FORMAT2	26	// "gw" format operator, keeps cursor pos
#ವ್ಯಾಖ್ಯಾನಿಸು OP_FUNCTION	27	// "g@" call 'operatorfunc'
#ವ್ಯಾಖ್ಯಾನಿಸು OP_NR_ಕೂಡಿಸು	28	// "<C-A>" Add to the number or alphabetic
				// character (OP_ADD conflicts with Perl)
#ವ್ಯಾಖ್ಯಾನಿಸು OP_NR_SUB	29	// "<C-X>" Subtract from the number or
				// alphabetic character

/*
 * Motion types, used for operators and for yank/delete registers.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MCHAR	0		// character-wise movement/register
#ವ್ಯಾಖ್ಯಾನಿಸು MLINE	1		// line-wise movement/register
#ವ್ಯಾಖ್ಯಾನಿಸು MBLOCK	2		// block-wise register

#ವ್ಯಾಖ್ಯಾನಿಸು MAUTO	0xff		// Decide between MLINE/MCHAR

/*
 * Minimum screen size
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MIN_COLUMNS	12	// minimal columns for screen
#ವ್ಯಾಖ್ಯಾನಿಸು MIN_ಸಾಲುಗಳು	2	// minimal lines for screen
#ವ್ಯಾಖ್ಯಾನಿಸು MIN_CMDHEIGHT	1	// minimal height for command line
#ವ್ಯಾಖ್ಯಾನಿಸು ಸ್ಥಿತಿ_ಎತ್ತರ_2	1	// default height of a status line under a
				//ಕಿಟಕಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU		// height of a status line under a window
# define WINBAR_ಎತ್ತರ(wp)	(wp)->w_winbar_ಎತ್ತರ
# define VISIBLE_ಎತ್ತರ(wp)	((wp)->w_ಎತ್ತರ + (wp)->w_winbar_ಎತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
# define WINBAR_ಎತ್ತರ(wp)	0
# define VISIBLE_ಎತ್ತರ(wp)	(wp)->w_ಎತ್ತರ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು QF_WINHEIGHT	10	// default height for quickfix window

/*
 * Buffer sizes
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CMDBUFFSIZE
# define CMDBUFFSIZE	256	// size of the command processing buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು LSIZE	    512		// max. size of a line in the tags file

#ವ್ಯಾಖ್ಯಾನಿಸು IOSIZE	   (1024+1)	// file i/o and sprintf buffer size

#ವ್ಯಾಖ್ಯಾನಿಸು DIALOG_ಸಂದೇಶ_ಗಾತ್ರ 1000	// buffer size for dialog_msg()

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_BUF_LEN 480	// length of buffer for small messages
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_BUF_CLEN  (ಸಂದೇಶ_BUF_LEN / 6)    // cell length (worst case: utf-8
					   // takes 6 bytes for one cell)

#ವ್ಯಾಖ್ಯಾನಿಸು FOLD_ಪಠ್ಯ_LEN  51	// buffer size for get_foldtext()

// Size of the buffer used for tgetent().  Unfortunately this is largely
// undocumented, some systems use 1024.  Using a buffer that is too small
// causes a buffer overrun and a crash.  Use the maximum known value to stay
// on the safe side.
#ವ್ಯಾಖ್ಯಾನಿಸು TBUFSZ 2048		// buffer size for termcap entry

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAXMAPLEN   50

// maximum length of a function name, including SID and NUL
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FUNC_NAME_LEN   200

// Size in bytes of the hash used in the undo file.
#ವ್ಯಾಖ್ಯಾನಿಸು UNDO_HASH_ಗಾತ್ರ 32

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_H
# include <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ದ್ವಿಮಾನ_FILE_IO
# define WRITEBIN   "wb"	// no CR-LF translation
# define READBIN    "rb"
# define APPENDBIN  "ab"
#ಇಲ್ಲದಿದ್ದರೆ
# define WRITEBIN   "w"
# define READBIN    "r"
# define APPENDBIN  "a"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Cygwin doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
# define O_EXTRA    O_ದ್ವಿಮಾನ
#ಇಲ್ಲದಿದ್ದರೆ
# define O_EXTRA    0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ O_NOFOLLOW
# define O_NOFOLLOW 0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ W_OK
# define W_OK 2		// for systems that don't have W_OK in unistd.h
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ R_OK
# define R_OK 4		// for systems that don't have R_OK in unistd.h
#ಆದರೆ_ಅಂತ್ಯ

// Allocate memory for one type and cast the returned pointer to have the
// compiler check the types.
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_ONE(type)  (type *)alloc(ಗಾತ್ರ(type))
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_ONE_ID(type, id)  (type *)alloc_id(ಗಾತ್ರ(type), id)
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_MULT(type, ಎಣಿಕೆ_2)  (type *)alloc(ಗಾತ್ರ(type) * (ಎಣಿಕೆ_2))
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_CLEAR_ONE(type)  (type *)alloc_clear(ಗಾತ್ರ(type))
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_CLEAR_ONE_ID(type, id)  (type *)alloc_clear_id(ಗಾತ್ರ(type), id)
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOC_CLEAR_MULT(type, ಎಣಿಕೆ_2)  (type *)alloc_clear(ಗಾತ್ರ(type) * (ಎಣಿಕೆ_2))
#ವ್ಯಾಖ್ಯಾನಿಸು LALLOC_CLEAR_ONE(type)  (type *)lalloc_clear(ಗಾತ್ರ(type), FALSE)
#ವ್ಯಾಖ್ಯಾನಿಸು LALLOC_CLEAR_MULT(type, ಎಣಿಕೆ_2)  (type *)lalloc_clear(ಗಾತ್ರ(type) * (ಎಣಿಕೆ_2), FALSE)
#ವ್ಯಾಖ್ಯಾನಿಸು LALLOC_MULT(type, ಎಣಿಕೆ_2)  (type *)lalloc(ಗಾತ್ರ(type) * (ಎಣಿಕೆ_2), FALSE)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_MEMSET
# define vim_memset(ptr, c, ಗಾತ್ರ_2)   memset((ptr), (c), (ಗಾತ್ರ_2))
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ *vim_memset(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, size_t);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು CLEAR_ಕ್ಷೇತ್ರ(ಕ್ಷೇತ್ರ)  vim_memset(&(ಕ್ಷೇತ್ರ), 0, ಗಾತ್ರ(ಕ್ಷೇತ್ರ))
#ವ್ಯಾಖ್ಯಾನಿಸು CLEAR_ಸೂಚಕ(ptr)  vim_memset((ptr), 0, ಗಾತ್ರ(*(ptr)))

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 * (vim_strchr() and vim_strrchr() are now in strings.c)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು STRLEN(s)	    ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಅಕ್ಷರ *)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು STRCPY(d, s)	    strcpy((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು STRNCPY(d, s, n)    strncpy((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))
#ವ್ಯಾಖ್ಯಾನಿಸು STRCMP(d, s)	    strcmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು STRNCMP(d, s, n)    ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRCASECMP
# define STRICMP(d, s)	    strcasecmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_STRICMP
#  define STRICMP(d, s)	    stricmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
# ಇಲ್ಲದಿದ್ದರೆ
#  define STRICMP(d, s)	    vim_stricmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRCOLL
# define STRCOLL(d, s)     strcoll((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ಇಲ್ಲದಿದ್ದರೆ
# define STRCOLL(d, s)     strcmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ಆದರೆ_ಅಂತ್ಯ

// Like strcpy() but allows overlapped source and destination.
#ವ್ಯಾಖ್ಯಾನಿಸು STRMOVE(d, s)	    mch_memmove((d), (s), STRLEN(s) + 1)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRNCASECMP
# define STRNICMP(d, s, n)  strncasecmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_STRNICMP
#  define STRNICMP(d, s, n) strnicmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))
# ಇಲ್ಲದಿದ್ದರೆ
#  define STRNICMP(d, s, n) vim_strnicmp((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))
# endif
#ಆದರೆ_ಅಂತ್ಯ

// We need to call mb_stricmp() even when we aren't dealing with a multi-byte
// encoding because mb_stricmp() takes care of all ascii and non-ascii
// encodings, including characters with umlauts in latin1, etc., while
// STRICMP() only handles the system locale version, which often does not
// handle non-ascii properly.

#ವ್ಯಾಖ್ಯಾನಿಸು MB_STRICMP(d, s)	mb_strnicmp((char_u *)(d), (char_u *)(s), (ಪೂರ್ಣಾಂಕ)MAXCOL)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_STRNICMP(d, s, n)	mb_strnicmp((char_u *)(d), (char_u *)(s), (ಪೂರ್ಣಾಂಕ)(n))
#ವ್ಯಾಖ್ಯಾನಿಸು MB_STRNICMP2(d, s, n1, n2)	mb_strnicmp2((char_u *)(d), (char_u *)(s), (n1), (n2))

#ವ್ಯಾಖ್ಯಾನಿಸು STRCAT(d, s)	    strcat((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು STRNCAT(d, s, n)    strncat((ಅಕ್ಷರ *)(d), (ಅಕ್ಷರ *)(s), (n))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRPBRK
# define vim_strpbrk(s, cs) (char_u *)strpbrk((ಅಕ್ಷರ *)(s), (ಅಕ್ಷರ *)(cs))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊರಹೋಗು_STR(s)		    ಹೊರಹೋಗು_str((char_u *)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊರಹೋಗು_STR_NF(s)		    ಹೊರಹೋಗು_str_nf((char_u *)(s))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    (gui.ಒಳ_use ? gui_##f : termgui_##f)
#  define GUI_FUNCTION2(f, pixel)   (gui.ಒಳ_use \
				    ?  ((pixel) != INVALCOLOR \
					? gui_##f((pixel)) \
					: INVALCOLOR) \
				    : termgui_##f((pixel)))
#  define USE_24_ಬಿಟ್		    (gui.ಒಳ_use || p_tgc)
# ಇಲ್ಲದಿದ್ದರೆ
#  define GUI_FUNCTION(f)	    gui_##f
#  define GUI_FUNCTION2(f,pixel)    ((pixel) != INVALCOLOR \
				     ? gui_##f((pixel)) \
				     : INVALCOLOR)
#  define USE_24_ಬಿಟ್		    gui.ಒಳ_use
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_TERMGUICOLORS
#  define GUI_FUNCTION(f)	    termgui_##f
#  define GUI_FUNCTION2(f, pixel)   termgui_##f((pixel))
#  define USE_24_ಬಿಟ್		    p_tgc
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
# define IS_CTERM		    (t_colors > 1 || p_tgc)
#ಇಲ್ಲದಿದ್ದರೆ
# define IS_CTERM		    (t_colors > 1)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_FUNCTION
# define GUI_MCH_GET_RGB	    GUI_FUNCTION(mch_get_rgb)
# define GUI_MCH_GET_RGB2(pixel)    GUI_FUNCTION2(mch_get_rgb, (pixel))
# define GUI_MCH_GET_COLOR	    GUI_FUNCTION(mch_get_color)
# define GUI_GET_COLOR		    GUI_FUNCTION(get_color)
#ಆದರೆ_ಅಂತ್ಯ

// Prefer using emsgf(), because perror() may send the output to the wrong
// destination and mess up the screen.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRERROR
# define PERROR(ಸಂದೇಶ)		    (ಶೂನ್ಯ)semsg("%s: %s", (ಅಕ್ಷರ *)(ಸಂದೇಶ), strerror(errno))
#ಇಲ್ಲದಿದ್ದರೆ
# define PERROR(ಸಂದೇಶ)		    do_perror(ಸಂದೇಶ)
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ	linenr_T;		// line number type
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ	colnr_T;		// column number type
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ disptick_T;	// display tick type

/*
 * Well, you won't believe it, but some S/390 machines ("host", now also known
 * as zServer) use 31 bit pointers. There are also some newer machines, that
 * use 64 bit pointers. I don't know how to distinguish between 31 and 64 bit
 * machines, so the best way is to assume 31 bits whenever we detect OS/390
 * Unix.
 * With this we restrict the maximum line length to 1073741823. I guess this is
 * not a real problem. BTW:  Longer lines are split.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MVS__
# define MAXCOL (0x3fffffffL)		// maximum column number, 30 bits
# define MAXLNUM (0x3fffffffL)		// maximum (invalid) line number
#ಇಲ್ಲದಿದ್ದರೆ
  // MAXCOL used to be INT_MAX, but with 64 bit ints that results in running
  // out of memory when trying to allocate a very long line.
# define MAXCOL  0x7fffffffL		// maximum column number
# define MAXLNUM LONG_MAX		// maximum (invalid) line number
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು SHOWCMD_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ 10			// columns needed by shown command

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ	    *vim_acl_T;		// dummy to pass an ACL to a function

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ mch_memmove
# define mch_memmove(ಗಮ್ಯ, from, len) memmove((ಅಕ್ಷರ*)(ಗಮ್ಯ), (ಅಕ್ಷರ*)(from), (len))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * fnamecmp() is used to compare file names.
 * On some systems case in a file name does not matter, on others it does.
 * (this does not account for maximum name lengths and things like "../dir",
 * thus it is not 100% accurate!)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು fnamecmp(x, y) vim_fnamecmp((char_u *)(x), (char_u *)(y))
#ವ್ಯಾಖ್ಯಾನಿಸು fnamencmp(x, y, n) vim_fnamencmp((char_u *)(x), (char_u *)(y), (n))

#ಆದರೆ defined(UNIX) || defined(FEAT_GUI) || defined(VMS) \
	|| defined(FEAT_CLIENTSERVER)
# define USE_ಒಳಹರಿವು_BUF
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(EINTR) && !defined(PROTO)
# define read_eintr(fd, buf, ಎಣಿಕೆ_2) vim_read((fd), (buf), (ಎಣಿಕೆ_2))
# define write_eintr(fd, buf, ಎಣಿಕೆ_2) vim_write((fd), (buf), (ಎಣಿಕೆ_2))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
// On MS-Windows the third argument isn't size_t.  This matters for Win64,
// where sizeof(size_t)==8, not 4
# define vim_read(fd, buf, ಎಣಿಕೆ_2)   ಪಠ((fd), (ಅಕ್ಷರ *)(buf), (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(ಎಣಿಕೆ_2))
# define vim_write(fd, buf, ಎಣಿಕೆ_2)  ಲಿಖ((fd), (ಅಕ್ಷರ *)(buf), (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(ಎಣಿಕೆ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define vim_read(fd, buf, ಎಣಿಕೆ_2)   ಪಠ((fd), (ಅಕ್ಷರ *)(buf), (ಎಣಿಕೆ_2))
# define vim_write(fd, buf, ಎಣಿಕೆ_2)  ಲಿಖ((fd), (ಅಕ್ಷರ *)(buf), (ಎಣಿಕೆ_2))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Enums need a typecast to be used as array index (for Ultrix).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HL_ATTR(n)	highlight_attr[(ಪೂರ್ಣಾಂಕ)(n)]
#ವ್ಯಾಖ್ಯಾನಿಸು ಪದ_STR(n)	ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[(ಪೂರ್ಣಾಂಕ)(n)]

/*
 * EXTERN is only defined in main.c.  That's where global variables are
 * actually defined and initialized.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EXTERN
# define EXTERN ಬಾಹ್ಯ
# define INIT(x)
# define INIT2(a, b)
# define INIT3(a, b, c)
# define INIT4(a, b, c, d)
# define INIT5(a, b, c, d, e)
# define INIT6(a, b, c, d, e, f)
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef INIT
#  define INIT(x) x
#  define INIT2(a, b) = {a, b}
#  define INIT3(a, b, c) = {a, b, c}
#  define INIT4(a, b, c, d) = {a, b, c, d}
#  define INIT5(a, b, c, d, e) = {a, b, c, d, e}
#  define INIT6(a, b, c, d, e, f) = {a, b, c, d, e, f}
#  define DO_INIT
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_MCO	6	// maximum value for 'maxcombine'

// Maximum number of bytes in the multi-byte character of one screen cell.
// It can be one character of up to six bytes, plus MAX_MCO following composing
// characters of up to four bytes each.
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MAXBYTES	30

#ಆದರೆ (defined(FEAT_PROFILE) || defined(FEAT_RELTIME)) && !defined(PROTO)
# ifdef MSWIN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ LARGE_INTEGER proftime_T;
#  define PROF_ಸಮಯ_BLANK "           "
#  define PROF_TOTALS_ಶೀರ್ಷಿಕೆ "count  total (s)   self (s)"
# ಇಲ್ಲದಿದ್ದರೆ
   // Use tv_fsec for fraction of second (micro or nano) of proftime_T
#  ಆದರೆ defined(HAVE_TIMER_CREATE) && defined(HAVE_CLOCK_GETTIME)
#   define PROF_NSEC 1
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ timespec proftime_T;
#   define PROF_GET_ಸಮಯ(tm) clock_gettime(CLOCK_MONOTONIC, tm)
#   define tv_fsec tv_nsec
#   define TV_FSEC_SEC 1000000000L
#   define PROF_ಸಮಯ_FORMAT "%3ld.%09ld"
#   define PROF_ಸಮಯ_BLANK "              "
#   define PROF_TOTALS_ಶೀರ್ಷಿಕೆ "count     total (s)      self (s)"
#  ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ timeval proftime_T;
#   define PROF_GET_ಸಮಯ(tm) gettimeofday(tm, ಖಾಲಿ)
#   define tv_fsec tv_usec
#   define TV_FSEC_SEC 1000000
#   define PROF_ಸಮಯ_FORMAT "%3ld.%06ld"
#   define PROF_ಸಮಯ_BLANK "           "
#   define PROF_TOTALS_ಶೀರ್ಷಿಕೆ "count  total (s)   self (s)"
#  endif
# endif
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ proftime_T;	    // dummy for function prototypes
#ಆದರೆ_ಅಂತ್ಯ

// Type of compilation passed to compile_def_function()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CT_NONE,	    // use df_instr
    CT_PROFILE,	    // use df_instr_prof
    CT_DEBUG	    // use df_instr_debug, overrules CT_PROFILE
} compiletype_T;

/*
 * When compiling with 32 bit Perl time_t is 32 bits in the Perl code but 64
 * bits elsewhere.  That causes memory corruption.  Define time_T and use it
 * for global variables to avoid that.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ __time64_t  ಸಮಯ_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ time_t	    ಸಮಯ_T;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN64
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ __int64 sock_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ sock_T;
#ಆದರೆ_ಅಂತ್ಯ

// The clipboard provider feature uses clipmethod as well but should be separate
// from the clipboard code.
#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(FEAT_EVAL)
# define HAVE_CLIPMETHOD
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_CLIPMETHOD) && defined(FEAT_EVAL)
# define FEAT_CLIPBOARD_PROVIDER
#ಆದರೆ_ಅಂತ್ಯ

// Include option.h before structs.h, because the number of window-local and
// buffer-local options is used there.
#ಅನ್ತರ್ಭಾವ "option.h"	// options and default values

#ಅನ್ತರ್ಭಾವ "beval.h"	// BalloonEval

// Note that gui.h is included by structs.h

#ಅನ್ತರ್ಭಾವ "structs.h"	// defines many structures
#ಅನ್ತರ್ಭಾವ "xdiff/xdiff.h"	// TODO: maybe remove this, but this brings in mmfile_t so it can compile

#ಅನ್ತರ್ಭಾವ "alloc.h"

// Values for "do_profiling".
#ವ್ಯಾಖ್ಯಾನಿಸು PROF_NONE	0	// profiling not started
#ವ್ಯಾಖ್ಯಾನಿಸು PROF_YES	1	// profiling busy
#ವ್ಯಾಖ್ಯಾನಿಸು PROF_PAUSED	2	// profiling paused


// Codes for mouse button events in lower three bits:
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಎಡ	0x00
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಮಧ್ಯ	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಬಲ	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_RELEASE	0x03

// bit masks for modifiers:
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಸರಿಸು	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ALT	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_CTRL	0x10

// mouse buttons that are handled like a key press (GUI only)
// Note that the scroll wheel keys are inverted: MOUSE_5 scrolls lines up but
// the result of this is that the window moves down, similarly MOUSE_6 scrolls
// columns left but the window moves right.
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_4	0x100	// scroll wheel down
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_5	0x200	// scroll wheel up

#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_X1	0x300 // Mouse-button X1 (6th)
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_X2	0x400 // Mouse-button X2

#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_6	0x500	// scroll wheel left
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_7	0x600	// scroll wheel right

#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_MOVE 0x700    // report mouse moved

// 0x20 is reserved by xterm
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_DRAG_XTERM   0x40

#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_DRAG	(0x40 | MOUSE_RELEASE)

// Lowest button code for using the mouse wheel (xterm only)
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSEWHEEL_ಕೆಳಗಿನ		0x60

// Lowest button code for extra mouse buttons 8-11
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSESIDEBUTTONS_ಕೆಳಗಿನ		0xa0

#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_CLICK_ಮುಖವಾಡ	0x03

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_MOUSE_CLICKS(ಸಂಕೇತ_2) \
    (((ಚಿಹ್ನೆರಹಿತ)((ಸಂಕೇತ_2) & 0xC0) >> 6) + 1)

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ಅಂಕೆ_MOUSE_CLICKS(ಸಂಕೇತ_2, ಅಂಕೆ) \
    ((ಸಂಕೇತ_2) = ((ಸಂಕೇತ_2) & 0x3f) | ((((ಅಂಕೆ) - 1) & 3) << 6))

// Added to mouse column for GUI when 'mousefocus' wants to give focus to a
// window by simulating a click on its status line.  We could use up to 128 *
// 128 = 16384 columns, now it's reduced to 10000.
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_COLOFF 10000

/*
 * jump_to_mouse() returns one of first five these values, possibly with
 * some of the other four added.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_UNKNOWN		0
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ಬಫರ್		1
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು		2	// on status or command line
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_SEP_ಸಾಲು		4	// on vertical separator line
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_OTHER_WIN		8	// in other window but can't go there
#ವ್ಯಾಖ್ಯಾನಿಸು CURSOR_MOVED		0x100
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_FOLD_CLOSE	0x200	// clicked on '-' in fold column
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_FOLD_OPEN		0x400	// clicked on '+' in fold column
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_WINBAR		0x800	// in window toolbar

// flags for jump_to_mouse()
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_FOCUS		0x01	// need to stay in this window
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_MAY_VIS		0x02	// may start Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_DID_MOVE		0x04	// only act when mouse has moved
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_SETPOS		0x08	// only set current mouse position
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_MAY_STOP_VIS	0x10	// may stop Visual mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_RELEASED		0x20	// button was released

#ಆದರೆ defined(UNIX) && defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ಸಮಯ_H)
# define CHECK_DOUBLE_CLICK 1	// Checking for double clicks ourselves.
#ಆದರೆ_ಅಂತ್ಯ


// defines for eval_vars()
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_ಮಾರ್ಗ		1
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_ತಲೆ		2

// Defines for Vim variables.  These must match vimvars[] in evalvars.c!
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ಎಣಿಕೆ	0
#ವ್ಯಾಖ್ಯಾನಿಸು VV_COUNT1	1
#ವ್ಯಾಖ್ಯಾನಿಸು VV_PREVCOUNT	2
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ERRMSG	3
#ವ್ಯಾಖ್ಯಾನಿಸು VV_WARNINGMSG	4
#ವ್ಯಾಖ್ಯಾನಿಸು VV_STATUSMSG	5
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SHELL_ERROR	6
#ವ್ಯಾಖ್ಯಾನಿಸು VV_THIS_SESSION	7
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ಆವೃತ್ತಿ	8
#ವ್ಯಾಖ್ಯಾನಿಸು VV_LNUM		9
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMRESPONSE	10
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FNAME	11
#ವ್ಯಾಖ್ಯಾನಿಸು VV_LANG		12
#ವ್ಯಾಖ್ಯಾನಿಸು VV_LC_ಸಮಯ	13
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CTYPE	14
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CC_FROM	15
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CC_ಗಮ್ಯ	16
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FNAME_ಒಳ	17
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FNAME_ಹೊರಹೋಗು	18
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FNAME_NEW	19
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FNAME_DIFF	20
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CMDARG	21
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FOLDSTART	22
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FOLDEND	23
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FOLDDASHES	24
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FOLDLEVEL	25
#ವ್ಯಾಖ್ಯಾನಿಸು VV_PROGNAME	26
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SEND_SERVER	27
#ವ್ಯಾಖ್ಯಾನಿಸು VV_DYING	28
#ವ್ಯಾಖ್ಯಾನಿಸು VV_EXCEPTION	29
#ವ್ಯಾಖ್ಯಾನಿಸು VV_THROWPOINT	30
#ವ್ಯಾಖ್ಯಾನಿಸು VV_REG		31
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CMDBANG	32
#ವ್ಯಾಖ್ಯಾನಿಸು VV_INSERTMODE	33
#ವ್ಯಾಖ್ಯಾನಿಸು VV_VAL		34
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ಕೀಲಿ		35
#ವ್ಯಾಖ್ಯಾನಿಸು VV_PROFILING	36
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FCS_REASON	37
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FCS_CHOICE	38
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_BUFNR	39
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_WINNR	40
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_WINID	41
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_LNUM	42
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_ಲಂಬಸಾಲು	43
#ವ್ಯಾಖ್ಯಾನಿಸು VV_BEVAL_ಪಠ್ಯ	44
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SCROLLSTART	45
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SWAPNAME	46
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SWAPCHOICE	47
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SWAPCOMMAND	48
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CHAR		49
#ವ್ಯಾಖ್ಯಾನಿಸು VV_MOUSE_WIN	50
#ವ್ಯಾಖ್ಯಾನಿಸು VV_MOUSE_WINID	51
#ವ್ಯಾಖ್ಯಾನಿಸು VV_MOUSE_LNUM   52
#ವ್ಯಾಖ್ಯಾನಿಸು VV_MOUSE_ಲಂಬಸಾಲು	53
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OP		54
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SEARCHFORWARD 55
#ವ್ಯಾಖ್ಯಾನಿಸು VV_HLSEARCH	56
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OLDFILES	57
#ವ್ಯಾಖ್ಯಾನಿಸು VV_WINDOWID	58
#ವ್ಯಾಖ್ಯಾನಿಸು VV_PROGPATH	59
#ವ್ಯಾಖ್ಯಾನಿಸು VV_COMPLETED_ಅಂಶ 60
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_NEW   61
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_OLD   62
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_OLDLOCAL 63
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_OLDGLOBAL 64
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_COMMAND 65
#ವ್ಯಾಖ್ಯಾನಿಸು VV_OPTION_TYPE  66
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ERRORS	67
#ವ್ಯಾಖ್ಯಾನಿಸು VV_FALSE	68
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TRUE		69
#ವ್ಯಾಖ್ಯಾನಿಸು VV_NONE		70
#ವ್ಯಾಖ್ಯಾನಿಸು VV_NULL		71
#ವ್ಯಾಖ್ಯಾನಿಸು VV_NUMBERMAX	72
#ವ್ಯಾಖ್ಯಾನಿಸು VV_NUMBERMIN	73
#ವ್ಯಾಖ್ಯಾನಿಸು VV_NUMBERSIZE	74
#ವ್ಯಾಖ್ಯಾನಿಸು VV_VIM_DID_ENTER 75
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TESTING	76
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_ಸಂಖ್ಯೆ	77
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_STRING	78
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_FUNC	79
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_LIST	80
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_DICT	81
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_FLOAT	82
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_BOOL	83
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_NONE	84
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_ಕೆಲಸ	85
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_CHANNEL	86
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_BLOB	87
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_CLASS	88
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_OBJECT	89
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMRFGRESP	90
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMRBGRESP	91
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMU7RESP	92
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMSTYLERESP 93
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMBLINKRESP 94
#ವ್ಯಾಖ್ಯಾನಿಸು VV_EVENT	95
#ವ್ಯಾಖ್ಯಾನಿಸು VV_VERSIONLONG	96
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ECHOSPACE	97
#ವ್ಯಾಖ್ಯಾನಿಸು VV_ARGV		98
#ವ್ಯಾಖ್ಯಾನಿಸು VV_COLLATE      99
#ವ್ಯಾಖ್ಯಾನಿಸು VV_EXITING	100
#ವ್ಯಾಖ್ಯಾನಿಸು VV_COLORNAMES   101
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SIZEOFINT	102
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SIZEOFLONG	103
#ವ್ಯಾಖ್ಯಾನಿಸು VV_SIZEOFPOINTER 104
#ವ್ಯಾಖ್ಯಾನಿಸು VV_MAXCOL	105
#ವ್ಯಾಖ್ಯಾನಿಸು VV_PYTHON3_ಆವೃತ್ತಿ 106
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_TYPEALIAS 107
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_ENUM	  108
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_ENUMVALUE 109
#ವ್ಯಾಖ್ಯಾನಿಸು VV_STACKTRACE	110
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TYPE_TUPLE	111
#ವ್ಯಾಖ್ಯಾನಿಸು VV_WAYLAND_DISPLAY 112
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CLIPMETHOD 113
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMDA1 114
#ವ್ಯಾಖ್ಯಾನಿಸು VV_TERMOSC 115
#ವ್ಯಾಖ್ಯಾನಿಸು VV_VIM_DID_INIT		116
#ವ್ಯಾಖ್ಯಾನಿಸು VV_CLIPPROVIDERS 117
#ವ್ಯಾಖ್ಯಾನಿಸು VV_IMAGEBACKEND 118
#ವ್ಯಾಖ್ಯಾನಿಸು VV_LEN		119	// number of v: vars

// used for v_number in VAR_BOOL and VAR_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿಸು VVAL_FALSE	0L	// VAR_BOOL
#ವ್ಯಾಖ್ಯಾನಿಸು VVAL_TRUE	1L	// VAR_BOOL
#ವ್ಯಾಖ್ಯಾನಿಸು VVAL_NONE	2L	// VAR_SPECIAL
#ವ್ಯಾಖ್ಯಾನಿಸು VVAL_NULL	3L	// VAR_SPECIAL

// Type values for type().
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_ಸಂಖ್ಯೆ	    0
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_STRING	    1
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_FUNC	    2
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_LIST	    3
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_DICT	    4
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_FLOAT	    5
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_BOOL	    6
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_NONE	    7
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_ಕೆಲಸ	    8
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_CHANNEL    9
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_BLOB	    10
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_INSTR	    11
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_CLASS	    12
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_OBJECT	    13
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_TYPEALIAS  14
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_ENUM	    15
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_ENUMVALUE  16
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_TYPE_TUPLE	    17

#ವ್ಯಾಖ್ಯಾನಿಸು DICT_MAXNEST 100	// maximum nesting of lists and dicts

#ವ್ಯಾಖ್ಯಾನಿಸು TABSTOP_MAX 9999

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CLIPMETHOD_FAIL,
    CLIPMETHOD_NONE,
    CLIPMETHOD_WAYLAND,
    CLIPMETHOD_X11,
    CLIPMETHOD_PROVIDER
} clipmethod_T;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD

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
# define SELECT_ಒಳ_ಪ್ರಗತಿ	1
# define SELECT_ಮುಗಿದ		2

# define SELECT_ಬಹುಲಕ_CHAR	0
# define SELECT_ಬಹುಲಕ_ಶಬ್ದ	1
# define SELECT_ಬಹುಲಕ_ಸಾಲು	2

# ifdef FEAT_GUI_MSWIN
#  ifdef FEAT_OLE
#   define WM_OLE (WM_APP+0)
#  endif
# endif

// Info about selected text
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		ಲಭ್ಯ;	// Is clipboard available?
    ಪೂರ್ಣಾಂಕ		owned;		// Flag: do we own the selection?
    pos_T	ಆರಂಭ;		// Start of selected area
    pos_T	ಕೊನೆ;		// End of selected area
    ಪೂರ್ಣಾಂಕ		vmode;		// Visual mode character

    // Fields for selection that doesn't use Visual mode
    short_u	origin_ಅಡ್ಡಸಾಲು;
    short_u	origin_ಆರಂಭ_ಲಂಬಸಾಲು;
    short_u	origin_ಕೊನೆ_ಲಂಬಸಾಲು;
    short_u	ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು;
    short_u	ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು;
# ifdef FEAT_PROP_POPUP
    // limits for selection inside a popup window
    short_u	min_ಲಂಬಸಾಲು;
    short_u	max_ಲಂಬಸಾಲು;
    short_u	min_ಅಡ್ಡಸಾಲು;
    short_u	max_ಅಡ್ಡಸಾಲು;
# endif

    pos_T	prev;		// Previous position
    short_u	state;		// Current selection state
    short_u	ಬಹುಲಕ;		// Select by char, word, or line.

# ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_XCLIPBOARD)
    Atom	sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ಆದರೆ defined(FEAT_GUI_GTK) && !defined(USE_GTK4)
    GdkAtom     gtk_sel_atom;	// PRIMARY/CLIPBOARD selection ID
# endif

# ಆದರೆ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    int_u	format;		// Vim's own special clipboard format
    int_u	format_raw;	// Vim's raw text clipboard format
# endif
# ifdef FEAT_GUI_HAIKU
    // No clipboard at the moment. TODO?
# endif
} Clipboard_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ Clipboard_T;	// This is required for the prototypes.
#ಆದರೆ_ಅಂತ್ಯ

// Use 64-bit stat structure on MS-Windows.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _stat64 stat_T;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ stat stat_T;
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ soundcb_S soundcb_T;

#ಆದರೆ (defined(__GNUC__) || defined(__clang__)) && !defined(__MINGW32__)
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx) \
    __attribute__((format(ಬರೆ, fmt_idx, arg_idx)))
#ಇಲ್ಲದಿದ್ದರೆ
# define ATTRIBUTE_FORMAT_PRINTF(fmt_idx, arg_idx)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
# define likely(x)	__builtin_expect((x), 1)
# define unlikely(x)	__builtin_expect((x), 0)
# define ATTRIBUTE_COLD	__attribute__((cold))
#ಇಲ್ಲದಿದ್ದರೆ
# define unlikely(x)	(x)
# define likely(x)	(x)
# define ATTRIBUTE_COLD
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ASSERT_EQUAL,
    ASSERT_NOTEQUAL,
    ASSERT_ಹೊಂದಿಕೆ,
    ASSERT_NOTMATCH,
    ASSERT_FAILS,
    ASSERT_OTHER
} assert_type_T;

// Mode for bracketed_paste().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    PASTE_ಸೇರಿಸು,	// insert mode
    PASTE_CMDLINE,	// command line
    PASTE_EX,		// ex mode line
    PASTE_ONE_CHAR	// return first character
} paste_ಬಹುಲಕ_T;

// Argument for flush_buffers().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    FLUSH_ಕನಿಷ್ಠತಮ,
    FLUSH_TYPEAHEAD,	// flush current typebuf contents
    FLUSH_ಒಳಹರಿವು		// flush typebuf and inchar() input
} flush_buffers_T;

// Argument for prepare_tagpreview()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    USEPOPUP_NONE,
    USEPOPUP_ಸಾಮಾನ್ಯ,	// use info popup
    USEPOPUP_HIDDEN	// use info popup initially hidden
} use_popup_T;

// Argument for estack_sfile().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ESTACK_NONE,
    ESTACK_SFILE,
    ESTACK_ರಾಶಿ,
    ESTACK_SCRIPT,
} estack_arg_T;

// For temporarily backward compatibility, to be removed soon.
#ವ್ಯಾಖ್ಯಾನಿಸು ENABLE_STL_ಬಹುಲಕ_MULTI_NL

// Argument for build_stl_str_hl_local().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    STL_ಬಹುಲಕ_SINGLE,	    // Does not accept line breaks "%@"
    STL_ಬಹುಲಕ_MULTI,	    // Accept line breaks "%@"
    STL_ಬಹುಲಕ_GET_RENDERED_ಎತ್ತರ,   // Just get stl rendered height
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_STL_ಬಹುಲಕ_MULTI_NL
    STL_ಬಹುಲಕ_MULTI_NL,	    // Accept line breaks "%@" and "\n"
#ಆದರೆ_ಅಂತ್ಯ
} stl_ಬಹುಲಕ_T;

// Return value of match_keyprotocol()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    KEYPROTOCOL_NONE,
    KEYPROTOCOL_MOK2,
    KEYPROTOCOL_KITTY,
    KEYPROTOCOL_FAIL
} keyprot_T;

// errors for when calling a function
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CPT_ABBR,		// "abbr"
    CPT_ಬಗೆ,		// "kind"
    CPT_MENU,		// "menu"
    CPT_INFO,		// "info"
    CPT_ಎಣಿಕೆ,		// Number of entries
} cpitem_T;

/*
 * Type for the callback function that is invoked after an option value is
 * changed to validate and apply the new value.
 *
 * Returns NULL if the option value is valid is successfully applied.
 * Otherwise returns an error message.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ *(*opt_did_ಗಣ_cb_T)(optset_T *ವಾದಗಳು);

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*opt_expand_cb_T)(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches);

// Flags for assignment functions.
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_VAR	0     // ":var" (nothing special)
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_FINAL	0x01  // ":final"
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_CONST	0x02  // ":const"
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_NO_DECL	0x04  // "name = expr" without ":let"/":const"/":final"
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_DECL	0x08  // may declare variable if it does not exist
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_UNPACK	0x10  // using [a, b] = list
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_NO_MEMBER_TYPE 0x20 // use "any" for list and dict member type
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_FOR_LOOP 0x40 // assigning to loop variable
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_INIT	0x80 // not assigning a value, just a declaration
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_UPDATE_ಖಂಡ_ID 0x100  // update sav_block_id
#ವ್ಯಾಖ್ಯಾನಿಸು ನಿಯೋಜಿಸು_COMPOUND_OP 0x200  // compound operator e.g. "+="

#ಅನ್ತರ್ಭಾವ "ex_cmds.h"	    // Ex command defines
#ಅನ್ತರ್ಭಾವ "spell.h"	    // spell checking stuff

#ಅನ್ತರ್ಭಾವ "proto.h"	    // function prototypes

// This has to go after the include of proto.h, as proto/gui.pro declares
// functions of these names. The declarations would break if the defines had
// been seen at that stage.  But it must be before globals.h, where error_ga
// is declared.
#ಆದರೆ !defined(MSWIN) && !defined(FEAT_GUI_X11) && !defined(FEAT_GUI_HAIKU) \
	&& !defined(FEAT_GUI_GTK) && !defined(PROTO)
# define mch_errmsg(str)	fprintf(stderr, "%s", (str))
# define display_errors()	fflush(stderr)
# define mch_ಸಂದೇಶ(str)		ಬರೆ("%s", (str))
#ಇಲ್ಲದಿದ್ದರೆ
# define USE_MCH_ERRMSG
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) \
	&& (!defined(FEAT_GUI_MSWIN) || !defined(FEAT_MBYTE_IME))
// Whether IME is supported by im_get_status() defined in mbyte.c.
// For Win32 GUI it's in gui_w32.c when FEAT_MBYTE_IME is defined.
# define IME_WITHOUT_XIM
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) \
	|| defined(IME_WITHOUT_XIM) \
	|| (defined(FEAT_GUI_MSWIN) && defined(FEAT_MBYTE_IME))
// im_set_active() is available
# define HAVE_ಒಳಹರಿವು_ವಿಧಾನ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_LINEBREAK
// Without the 'numberwidth' option line numbers are always 7 chars.
# define ಸಂಖ್ಯೆ_ಅಗಲ(x) 7
#ಆದರೆ_ಅಂತ್ಯ

// This must come after including proto.h.
// For VMS this is defined in macros.h.
#ಆದರೆ !defined(MSWIN) && !defined(VMS) && !defined(PROTO)
# define mch_open(n, m, p)	open((n), (m), (p))
# define mch_fopen(n, p)	fopen((n), (p))
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "globals.h"	    // global variables and messages
#ಅನ್ತರ್ಭಾವ "errors.h"	    // error messages

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
/*
 * Return TRUE when currently using Vim9 script syntax.
 * Does not go up the stack, a ":function" inside vim9script uses legacy
 * syntax.
 * Defined here as "static inline" because it is called very frequently from
 * the eval hot path; inlining avoids the cross-file call overhead.
 */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
ಒಳ_vim9script(ಶೂನ್ಯ)
{
    // "sc_version" is also set when compiling a ":def" function in legacy
    // script.
    ಹಿಂತಿರುಗಿ (current_sctx.sc_ಆವೃತ್ತಿ == SCRIPT_ಆವೃತ್ತಿ_VIM9
					 || (cmdmod.cmod_flags & CMOD_VIM9CMD))
		&& !(cmdmod.cmod_flags & CMOD_LEGACY);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If console dialog not supported, but GUI dialog is, use the GUI one.
 */
#ಆದರೆ defined(FEAT_GUI_DIALOG) && !defined(FEAT_CON_DIALOG)
# define do_dialog gui_mch_dialog
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Default filters for gui_mch_browse().
 * The filters are almost system independent.  Except for the difference
 * between "*" and "*.*" for MSDOS-like systems.
 * NOTE: Motif only uses the very first pattern.  Therefore
 * BROWSE_FILTER_DEFAULT should start with a "*" pattern.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
# ifdef BACKSLASH_ಒಳ_FILENAME
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*.*)\t*.*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*.*)\t*.*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVB code (*.bas, *.frm)\t*.bas;*.frm\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# ಇಲ್ಲದಿದ್ದರೆ
#  define BROWSE_FILTER_MACROS \
	(char_u *)N_("Vim macro files (*.vim)\t*.vim\nAll Files (*)\t*\n")
#  define BROWSE_FILTER_ALL_FILES (char_u *)N_("All Files (*)\t*\n")
#  define BROWSE_FILTER_DEFAULT \
	(char_u *)N_("All Files (*)\t*\nC source (*.c, *.h)\t*.c;*.h\nC++ source (*.cpp, *.hpp)\t*.cpp;*.hpp\nVim files (*.vim, _vimrc, _gvimrc)\t*.vim;_vimrc;_gvimrc\n")
# endif
# define BROWSE_SAVE 1	    // flag for do_browse()
# define BROWSE_DIR 2	    // flag for do_browse()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
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
#ಆದರೆ_ಅಂತ್ಯ

// Note: a NULL argument for vim_realloc() is not portable, don't use it.
#ಆದರೆ defined(MEM_PROFILE)
# define vim_realloc(ptr, ಗಾತ್ರ_2)  mem_realloc((ptr), (ಗಾತ್ರ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define vim_realloc(ptr, ಗಾತ್ರ_2)  realloc((ptr), (ಗಾತ್ರ_2))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return byte length of character that starts with byte "b".
 * Returns 1 for a single-byte character.
 * MB_BYTE2LEN_CHECK() can be used to count a special key as one byte.
 * Don't call MB_BYTE2LEN(b) with b < 0 or b > 255!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MB_BYTE2LEN(b)		mb_bytelen_tab[b]
#ವ್ಯಾಖ್ಯಾನಿಸು MB_BYTE2LEN_CHECK(b)	(((b) < 0 || (b) > 255) ? 1 : mb_bytelen_tab[b])

// properties used in enc_canon_table[] (first three mutually exclusive)
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_8_ಬಿಟ್	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_DBCS	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_UNICODE	0x04

#ವ್ಯಾಖ್ಯಾನಿಸು ENC_ENDIAN_B	0x10	    // Unicode: Big endian
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_ENDIAN_L	0x20	    // Unicode: Little endian

#ವ್ಯಾಖ್ಯಾನಿಸು ENC_2BYTE	0x40	    // Unicode: UCS-2
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_4BYTE	0x80	    // Unicode: UCS-4
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_2_ಶಬ್ದ	0x100	    // Unicode: UTF-16

#ವ್ಯಾಖ್ಯಾನಿಸು ENC_LATIN1	0x200	    // Latin1
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_LATIN9	0x400	    // Latin9
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_MACROMAN	0x800	    // Mac Roman (not Macro Man! :-)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
# ifndef EILSEQ
#  define EILSEQ 123
# endif
# ifdef DYNAMIC_ICONV
// On Win32 iconv.dll is dynamically loaded.
#  define ICONV_ERRNO (*iconv_errno())
#  define ICONV_E2BIG  7
#  define ICONV_EINVAL 22
#  define ICONV_EILSEQ 42
# ಇಲ್ಲದಿದ್ದರೆ
#  define ICONV_ERRNO errno
#  define ICONV_E2BIG  E2BIG
#  define ICONV_EINVAL EINVAL
#  define ICONV_EILSEQ EILSEQ
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು SIGN_BYTE 1	    // byte value used where sign is displayed;
			    // attribute value is sign type

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
# define MULTISIGN_BYTE 2   // byte value used where sign is displayed if
			    // multiple signs exist on the line
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
// Logical pixels to physical pixels
# define LOG2PHY(l) (gui.ಒಳ_use ? (ದ್ವಿಗುಣ)(l) * gui.scale : (l))
 // Physical pixels to logical pixels
# define PHY2LOG(p) (gui.ಒಳ_use ? (ದ್ವಿಗುಣ)(p) / gui.scale : (p))
#ಇಲ್ಲದಿದ್ದರೆ
# define LOG2PHY(l) (l)
# define PHY2LOG(p) (p)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_XCLIPBOARD)
# ifdef FEAT_GUI_GTK
   // Avoid using a global variable for the X display.  It's ugly
   // and is likely to cause trouble in multihead environments.
#  define X_DISPLAY	((gui.ಒಳ_use) ? gui_mch_get_display() : xterm_dpy)
# ಇಲ್ಲದಿದ್ದರೆ
#  define X_DISPLAY	(gui.ಒಳ_use ? gui.dpy : xterm_dpy)
# endif
#ಅಥವಾಆದರೆ defined(FEAT_GUI)
# ifdef FEAT_GUI_GTK
#  define X_DISPLAY	((gui.ಒಳ_use) ? gui_mch_get_display() : (Display *)ಖಾಲಿ)
# ಇಲ್ಲದಿದ್ದರೆ
#  define X_DISPLAY	gui.dpy
# endif
#ಅಥವಾಆದರೆ defined(FEAT_XCLIPBOARD)
# define X_DISPLAY	xterm_dpy
#ಇಲ್ಲದಿದ್ದರೆ
# define X_DISPLAY	(Display *)ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(2,14,0)
#  define gtk_widget_get_ಕಿಟಕಿ(wid)	((wid)->ಕಿಟಕಿ_2)
#  define gtk_plug_get_socket_ಕಿಟಕಿ(wid)	((wid)->socket_ಕಿಟಕಿ)
#  define gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ(sel)	((sel)->ದತ್ತಾಂಶ)
#  define gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(sel)	((sel)->type)
#  define gtk_selection_ದತ್ತಾಂಶ_get_format(sel)	((sel)->format)
#  define gtk_selection_ದತ್ತಾಂಶ_get_ಉದ್ದಳತೆ(sel)	((sel)->ಉದ್ದಳತೆ)
#  define gtk_adjustment_ಗಣ_ಅಧೋ(adj, ಕೆಳಗಿನ) \
    ಮಾಡು { (adj)->ಅಧೋ = ಕೆಳಗಿನ; } ಆಗ (0)
#  define gtk_adjustment_ಗಣ_ಊರ್ಧ್ವ(adj, ಮೇಲೆ) \
    ಮಾಡು { (adj)->ಊರ್ಧ್ವ_2 = ಮೇಲೆ; } ಆಗ (0)
#  define gtk_adjustment_ಗಣ_ಪುಟ_ಗಾತ್ರ(adj, ಗಾತ್ರ_2) \
    ಮಾಡು { (adj)->ಪುಟ_ಗಾತ್ರ = ಗಾತ್ರ_2; } ಆಗ (0)
#  define gtk_adjustment_ಗಣ_ಪುಟ_increment(adj, inc) \
    ಮಾಡು { (adj)->ಪುಟ_increment = inc; } ಆಗ (0)
#  define gtk_adjustment_ಗಣ_ಹೆಜ್ಜೆ_increment(adj, inc) \
    ಮಾಡು { (adj)->ಹೆಜ್ಜೆ_increment = inc; } ಆಗ (0)
# endif
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(2,16,0)
#  define gtk_selection_ದತ್ತಾಂಶ_get_selection(sel)	((sel)->selection)
# endif
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(2,18,0)
#  define gtk_widget_get_ಹಂಚಿಕೆ(wid, alloc) \
    ಮಾಡು { *(alloc) = (wid)->ಹಂಚಿಕೆ; } ಆಗ (0)
#  define gtk_widget_ಗಣ_ಹಂಚಿಕೆ(wid, alloc) \
    ಮಾಡು { (wid)->ಹಂಚಿಕೆ = *(alloc); } ಆಗ (0)
#  define gtk_widget_get_has_ಕಿಟಕಿ(wid)	!GTK_WIDGET_NO_ಕಿಟಕಿ(wid)
#  define gtk_widget_get_sensitive(wid)	GTK_WIDGET_SENSITIVE(wid)
#  define gtk_widget_get_visible(wid)	GTK_WIDGET_VISIBLE(wid)
#  define gtk_widget_has_focus(wid)	GTK_WIDGET_HAS_FOCUS(wid)
#  define gtk_widget_ಗಣ_ಕಿಟಕಿ(wid, win) \
    ಮಾಡು { (wid)->ಕಿಟಕಿ_2 = (win); } ಆಗ (0)
#  define gtk_widget_ಗಣ_can_default(wid, can) \
    ಮಾಡು { ಆದರೆ (can) \
	    { GTK_WIDGET_ಗಣ_FLAGS(wid, GTK_CAN_DEFAULT); } \
	ಇಲ್ಲದಿದ್ದರೆ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_DEFAULT); } } ಆಗ (0)
#  define gtk_widget_ಗಣ_can_focus(wid, can) \
    ಮಾಡು { ಆದರೆ (can) \
	    { GTK_WIDGET_ಗಣ_FLAGS(wid, GTK_CAN_FOCUS); } \
	ಇಲ್ಲದಿದ್ದರೆ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS); } } ಆಗ (0)
#  define gtk_widget_ಗಣ_visible(wid, vis) \
    ಮಾಡು { ಆದರೆ (vis) \
	    { gtk_widget_show(wid); } \
	ಇಲ್ಲದಿದ್ದರೆ \
	    { gtk_widget_hide(wid); } } ಆಗ (0)
# endif
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(2,20,0)
#  define gtk_widget_get_mapped(wid)	GTK_WIDGET_MAPPED(wid)
#  define gtk_widget_get_realized(wid)	GTK_WIDGET_REALIZED(wid)
#  define gtk_widget_ಗಣ_mapped(wid, map) \
    ಮಾಡು { ಆದರೆ (map) \
	    { GTK_WIDGET_ಗಣ_FLAGS(wid, GTK_MAPPED); } \
	ಇಲ್ಲದಿದ್ದರೆ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_MAPPED); } } ಆಗ (0)
#  define gtk_widget_ಗಣ_realized(wid, rea) \
    ಮಾಡು { ಆದರೆ (rea) \
	    { GTK_WIDGET_ಗಣ_FLAGS(wid, GTK_REALIZED); } \
	ಇಲ್ಲದಿದ್ದರೆ \
	    { GTK_WIDGET_UNSET_FLAGS(wid, GTK_REALIZED); } } ಆಗ (0)
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PRINT_PANGO) && defined(FEAT_GUI_GTK) && defined(USE_GTK4)
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4, 14, 0)
#  define USE_GTK4_PRINT_DIALOG
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_NETBEANS_INTG
# undef NBDEBUG
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG // Netbeans debugging.
# include "nbdebug.h"
#ಇಲ್ಲದಿದ್ದರೆ
# define nbdebug(a)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಒಳ_PERL_FILE
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
# ಆದರೆ (defined(MACOS_X) && !defined(MAC_OS_X_ಆವೃತ್ತಿ_10_6)) \
				       && defined(bool) && !defined(FEAT_PERL)
#  undef bool
# endif

#ಆದರೆ_ಅಂತ್ಯ

// values for vim_handle_signal() that are not a signal
#ವ್ಯಾಖ್ಯಾನಿಸು SIGNAL_ಖಂಡ	(-1)
#ವ್ಯಾಖ್ಯಾನಿಸು SIGNAL_UNBLOCK  (-2)
#ಆದರೆ !defined(UNIX) && !defined(VMS) && !defined(PROTO)
# define vim_handle_signal(x) 0
#ಆದರೆ_ಅಂತ್ಯ

// flags for skip_vimgrep_pat()
#ವ್ಯಾಖ್ಯಾನಿಸು VGR_GLOBAL	1
#ವ್ಯಾಖ್ಯಾನಿಸು VGR_NOJUMP	2
#ವ್ಯಾಖ್ಯಾನಿಸು VGR_FUZZY	4

// behavior for bad character, "++bad=" argument
#ವ್ಯಾಖ್ಯಾನಿಸು BAD_REPLACE	'?'	// replace it with '?' (default)
#ವ್ಯಾಖ್ಯಾನಿಸು BAD_KEEP	(-1)	// leave it
#ವ್ಯಾಖ್ಯಾನಿಸು BAD_DROP	(-2)	// erase it

// last argument for do_source()
#ವ್ಯಾಖ್ಯಾನಿಸು DOSO_NONE	0
#ವ್ಯಾಖ್ಯಾನಿಸು DOSO_VIMRC	1	// loading vimrc file
#ವ್ಯಾಖ್ಯಾನಿಸು DOSO_GVIMRC	2	// loading gvimrc file

// flags for read_viminfo() and children
#ವ್ಯಾಖ್ಯಾನಿಸು VIF_WANT_INFO	    1	// load non-mark info
#ವ್ಯಾಖ್ಯಾನಿಸು VIF_WANT_MARKS	    2	// load file marks
#ವ್ಯಾಖ್ಯಾನಿಸು VIF_ONLY_CURBUF	    4	// bail out after loading marks for curbuf
#ವ್ಯಾಖ್ಯಾನಿಸು VIF_FORCEIT	    8	// overwrite info already read
#ವ್ಯಾಖ್ಯಾನಿಸು VIF_GET_OLDFILES    16	// load v:oldfiles

// flags for buf_freeall()
#ವ್ಯಾಖ್ಯಾನಿಸು BFA_DEL		 1	// buffer is going to be deleted
#ವ್ಯಾಖ್ಯಾನಿಸು BFA_WIPE	 2	// buffer is going to be wiped out
#ವ್ಯಾಖ್ಯಾನಿಸು BFA_KEEP_UNDO	 4	// do not free undo information
#ವ್ಯಾಖ್ಯಾನಿಸು BFA_IGNORE_ABORT 8	// do not abort for aborting()

// direction for nv_mousescroll() and ins_mousescroll()
#ವ್ಯಾಖ್ಯಾನಿಸು MSCR_ಕೆಳಕ್ಕೆ	0	// DOWN must be FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು MSCR_ಮೇಲೆ		1
#ವ್ಯಾಖ್ಯಾನಿಸು MSCR_ಎಡ	(-1)
#ವ್ಯಾಖ್ಯಾನಿಸು MSCR_ಬಲ	(-2)

#ವ್ಯಾಖ್ಯಾನಿಸು KEYLEN_PART_ಕೀಲಿ (-1)	// keylen value for incomplete key-code
#ವ್ಯಾಖ್ಯಾನಿಸು KEYLEN_PART_MAP (-2)	// keylen value for incomplete mapping
#ವ್ಯಾಖ್ಯಾನಿಸು KEYLEN_REMOVED  9999	// keylen value for removed sequence

// Return values from win32_fileinfo().
#ವ್ಯಾಖ್ಯಾನಿಸು FILEINFO_OK	     0
#ವ್ಯಾಖ್ಯಾನಿಸು FILEINFO_ENC_FAIL    1	// enc_to_utf16() failed
#ವ್ಯಾಖ್ಯಾನಿಸು FILEINFO_READ_FAIL   2	// CreateFile() failed
#ವ್ಯಾಖ್ಯಾನಿಸು FILEINFO_INFO_FAIL   3	// GetFileInformationByHandle() failed

// Return value from get_option_value_strict
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_BOOL	0x01	// Boolean option
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_ಅಂಕೆ	0x02	// Number option
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_STRING	0x04	// String option
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_GLOBAL	0x08	// Option has global value
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_WIN	0x10	// Option has window-local value
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_BUF	0x20	// Option has buffer-local value
#ವ್ಯಾಖ್ಯಾನಿಸು SOPT_UNSET	0x40	// Option does not have local value set

// Option types for various functions in option.c
#ವ್ಯಾಖ್ಯಾನಿಸು SREQ_GLOBAL	0	// Request global option
#ವ್ಯಾಖ್ಯಾನಿಸು SREQ_WIN	1	// Request window-local option
#ವ್ಯಾಖ್ಯಾನಿಸು SREQ_BUF	2	// Request buffer-local option

// Flags for get_reg_contents
#ವ್ಯಾಖ್ಯಾನಿಸು GREG_NO_ಅಭಿವ್ಯಕ್ತಿ	1	// Do not allow expression register
#ವ್ಯಾಖ್ಯಾನಿಸು GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC	2	// Return expression itself for "=" register
#ವ್ಯಾಖ್ಯಾನಿಸು GREG_LIST	4	// Return list

// Character used as separated in autoload function/variable names.
#ವ್ಯಾಖ್ಯಾನಿಸು AUTOLOAD_CHAR '#'

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
# define MAX_OPEN_CHANNELS 10
#ಇಲ್ಲದಿದ್ದರೆ
# define MAX_OPEN_CHANNELS 0
#ಆದರೆ_ಅಂತ್ಯ

// Maximum number of simultaneously accepted socketserver client channels.
// Bounds the channels (and file descriptors) added to the select()/poll()
// sets so a connection flood cannot overflow the fd_set / pollfd arrays.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
# define MAX_CLIENT_CHANNELS 100
#ಇಲ್ಲದಿದ್ದರೆ
# define MAX_CLIENT_CHANNELS 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
# define MAX_NAMED_PIPE_ಗಾತ್ರ 65535
#ಆದರೆ_ಅಂತ್ಯ

// Options for json_encode() and json_decode.
#ವ್ಯಾಖ್ಯಾನಿಸು JSON_JS		1   // use JS instead of JSON
#ವ್ಯಾಖ್ಯಾನಿಸು JSON_NO_NONE	2   // v:none item not allowed
#ವ್ಯಾಖ್ಯಾನಿಸು JSON_NL		4   // append a NL

// Used for flags of do_in_path()
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_ALL	    0x01	// all matches, not just the first one
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_DIR	    0x02	// find directories instead of files.
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_ERR	    0x04	// give an error message when none found.
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_ಆರಂಭ   0x08	// also use "start" directory in 'packpath'
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_OPT	    0x10	// also use "opt" directory in 'packpath'
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_NORTP   0x20	// do not use 'runtimepath'
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_NOAFTER 0x40	// skip "after" directories
#ವ್ಯಾಖ್ಯಾನಿಸು DIP_AFTER   0x80	// only use "after" directories

// Lowest number used for window ID. Cannot have this many windows.
#ವ್ಯಾಖ್ಯಾನಿಸು ಅತಿಕಡಿಮೆ_WIN_ID 1000

// Used by the garbage collector.
#ವ್ಯಾಖ್ಯಾನಿಸು COPYID_INC 2
#ವ್ಯಾಖ್ಯಾನಿಸು COPYID_ಮುಖವಾಡ (~0x1)

// Values for trans_function_name() argument:
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_INT		0x01	// internal function name OK
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_QUIET	0x02	// no error messages
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_NO_AUTOLOAD	0x04	// do not use script autoloading
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_NO_DEREF	0x08	// do not dereference a Funcref
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_READ_ONLY	0x10	// will not change the var
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_NO_DECL	0x20	// only used for GLV_NO_DECL
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_COMPILING	0x40	// only used for GLV_COMPILING
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_NEW_FUNC	0x80	// defining a new function
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_ನಿಯೋಜಿಸು_WITH_OP 0x100  // only for GLV_ASSIGN_WITH_OP
#ವ್ಯಾಖ್ಯಾನಿಸು TFN_ಒಳ_CLASS	0x200	// function in a class

// Values for get_lval() flags argument:
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_QUIET	TFN_QUIET	// no error messages
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_NO_AUTOLOAD	TFN_NO_AUTOLOAD	// do not use script autoloading
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_READ_ONLY	TFN_READ_ONLY	// will not change the var
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_NO_DECL	TFN_NO_DECL	// assignment without :var or :let
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_COMPILING	TFN_COMPILING	// variable may be defined later
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_ನಿಯೋಜಿಸು_WITH_OP TFN_ನಿಯೋಜಿಸು_WITH_OP // assignment with operator
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_PREFER_FUNC	0x10000		// prefer function above variable
#ವ್ಯಾಖ್ಯಾನಿಸು GLV_FOR_LOOP	0x20000		// assigning to a loop variable

#ವ್ಯಾಖ್ಯಾನಿಸು DO_NOT_FREE_CNT 99999	// refcount for dict or list that should not
				// be freed.

// fixed buffer length for fname_trans_sid()
#ವ್ಯಾಖ್ಯಾನಿಸು FLEN_FIXED 40

// flags for find_name_end()
#ವ್ಯಾಖ್ಯಾನಿಸು FNE_INCL_BR	1	// include [] in name
#ವ್ಯಾಖ್ಯಾನಿಸು FNE_CHECK_ಆರಂಭ	2	// check name starts with valid character

// BSD is supposed to cover FreeBSD and similar systems.
#ಆದರೆ (defined(SUN_SYSTEM) || defined(BSD) || defined(__FreeBSD_kernel__)) \
	&& (defined(S_ISCHR) || defined(S_IFCHR))
# define OPEN_CHR_FILES
#ಆದರೆ_ಅಂತ್ಯ

// stat macros
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISDIR(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISREG(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISBLK(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISSOCK(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISFIFO(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISCHR(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# ಇಲ್ಲದಿದ್ದರೆ
#  define S_ISLNK(m)	0
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ಸಮಯ_H)
# define ELAPSED_TIMEVAL
# define ELAPSED_INIT(v) gettimeofday(&(v), ಖಾಲಿ)
# define ELAPSED_FUNC(v) elapsed(&(v))
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ timeval elapsed_T;
ದೀರ್ಘ elapsed(ರಚನೆ timeval *ಆರಂಭ_tv);
#ಅಥವಾಆದರೆ defined(MSWIN)
# define ELAPSED_TICKCOUNT
# define ELAPSED_INIT(v) v = Get_Tick_ಎಣಿಕೆ()
# define ELAPSED_FUNC(v) elapsed(v)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ DWORD elapsed_T;
# ifndef PROTO
ದೀರ್ಘ elapsed(DWORD ಆರಂಭ_tick);
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Replacement for nchar used by nv_replace().
#ವ್ಯಾಖ್ಯಾನಿಸು REPLACE_CR_NCHAR    (-1)
#ವ್ಯಾಖ್ಯಾನಿಸು REPLACE_NL_NCHAR    (-2)

// flags for term_start()
#ವ್ಯಾಖ್ಯಾನಿಸು ಪದ_ಆರಂಭ_NOJOB	1
#ವ್ಯಾಖ್ಯಾನಿಸು ಪದ_ಆರಂಭ_FORCEIT	2
#ವ್ಯಾಖ್ಯಾನಿಸು ಪದ_ಆರಂಭ_SYSTEM	4

// Used for icon/title save and restore.
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_RESTORE_TITLE	1
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_RESTORE_ICON	2
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_RESTORE_BOTH	(SAVE_RESTORE_TITLE | SAVE_RESTORE_ICON)

// Flags for adjust_prop_columns()
#ವ್ಯಾಖ್ಯಾನಿಸು APC_SAVE_FOR_UNDO	1   // call u_savesub() before making changes
#ವ್ಯಾಖ್ಯಾನಿಸು APC_SUBSTITUTE		2   // text is replaced, not inserted
#ವ್ಯಾಖ್ಯಾನಿಸು APC_INDENT		4   // changing indent

#ವ್ಯಾಖ್ಯಾನಿಸು CLIP_ZINDEX 32000

// Flags for replace_termcodes()
#ವ್ಯಾಖ್ಯಾನಿಸು REPTERM_FROM_PART	1
#ವ್ಯಾಖ್ಯಾನಿಸು REPTERM_DO_LT		2
#ವ್ಯಾಖ್ಯಾನಿಸು REPTERM_SPECIAL		4
#ವ್ಯಾಖ್ಯಾನಿಸು REPTERM_NO_SIMPLIFY	8

// Flags for find_special_key()
#ವ್ಯಾಖ್ಯಾನಿಸು FSK_KEYCODE	0x01	// prefer key code, e.g. K_DEL instead of DEL
#ವ್ಯಾಖ್ಯಾನಿಸು FSK_KEEP_X_ಕೀಲಿ	0x02	// don't translate xHome to Home key
#ವ್ಯಾಖ್ಯಾನಿಸು FSK_ಒಳ_STRING	0x04	// TRUE in string, double quote is escaped
#ವ್ಯಾಖ್ಯಾನಿಸು FSK_SIMPLIFY	0x08	// simplify <C-H> and <A-x>
#ವ್ಯಾಖ್ಯಾನಿಸು FSK_FROM_PART	0x10	// left-hand-side of mapping

// Flags for the readdirex function, how to sort the result
#ವ್ಯಾಖ್ಯಾನಿಸು READDIR_ವಿಂಗಡಿಸು_NONE	0  // do not sort
#ವ್ಯಾಖ್ಯಾನಿಸು READDIR_ವಿಂಗಡಿಸು_BYTE	1  // sort by byte order (strcmp), default
#ವ್ಯಾಖ್ಯಾನಿಸು READDIR_ವಿಂಗಡಿಸು_IC		2  // sort ignoring case (strcasecmp)
#ವ್ಯಾಖ್ಯಾನಿಸು READDIR_ವಿಂಗಡಿಸು_COLLATE	3  // sort according to collation (strcoll)

// Flags for mch_delay.
#ವ್ಯಾಖ್ಯಾನಿಸು MCH_DELAY_IGNOREINPUT	1
#ವ್ಯಾಖ್ಯಾನಿಸು MCH_DELAY_SETTMODE	2

// Flags for eval_variable().
#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_VAR_VERBOSE	1   // may give error message
#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_VAR_NOAUTOLOAD	2   // do not use script autoloading
#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_VAR_IMPORT		4   // may return special variable for import
#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_VAR_NO_FUNC	8   // do not look for a function

// Fuzzy matching
#ವ್ಯಾಖ್ಯಾನಿಸು FUZZY_ಹೊಂದಿಕೆ_MAX_LEN	1024    // max characters that can be matched
#ವ್ಯಾಖ್ಯಾನಿಸು FUZZY_SCORE_NONE	INT_MIN // invalid fuzzy score

// flags for equal_type()
#ವ್ಯಾಖ್ಯಾನಿಸು ETYPE_ARG_UNKNOWN 1

// flags used by user commands and :autocmd
#ವ್ಯಾಖ್ಯಾನಿಸು UC_ಬಫರ್	1	// -buffer: local to current buffer
#ವ್ಯಾಖ್ಯಾನಿಸು UC_VIM9		2	// {} argument: Vim9 syntax.

// flags for the -completeopt= attribute of :command
#ವ್ಯಾಖ್ಯಾನಿಸು UCC_ESCAPE	0x1	// escape spaces, tabs and backslashes in
				// matches

// flags used by vim_strsave_fnameescape()
#ವ್ಯಾಖ್ಯಾನಿಸು VSE_NONE	0
#ವ್ಯಾಖ್ಯಾನಿಸು VSE_SHELL	1	// escape for a shell command
#ವ್ಯಾಖ್ಯಾನಿಸು VSE_ಬಫರ್	2	// escape for a ":buffer" command

// Flags used by find_func_even_dead()
#ವ್ಯಾಖ್ಯಾನಿಸು FFED_IS_GLOBAL	1	// "g:" was used
#ವ್ಯಾಖ್ಯಾನಿಸು FFED_NO_GLOBAL	2	// only check for script-local functions

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_LSHIFT_ಬಿಟ್ಟುಗಳು (varnumber_T)((ಗಾತ್ರ(uvarnumber_T) * 8) - 1)

// Flags used by "class_flags" of define_function()
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CLASS	1	// inside a class
#ವ್ಯಾಖ್ಯಾನಿಸು CF_INTERFACE	2	// inside an interface
#ವ್ಯಾಖ್ಯಾನಿಸು CF_ABSTRACT_ವಿಧಾನ	4	// inside an abstract class

// Flags used by getvcol()
#ವ್ಯಾಖ್ಯಾನಿಸು GETVCOL_ಕೊನೆ_EXCL_LBR	1
#ವ್ಯಾಖ್ಯಾನಿಸು GETVCOL_NO_ABOVE	2	// exclude virtual text above the line
#ವ್ಯಾಖ್ಯಾನಿಸು GETVCOL_FOR_VIRTCOL	4	// value is used for "w_virtcol"

// Used by expand_env_esc() callers that feed the result to
// wildcard expansion, so that such characters embedded in
// environment variable values are treated as literal.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# define ಮಾರ್ಗ_ESC_WILDCARDS	"*?%"
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef MSWIN
#  define ಮಾರ್ಗ_ESC_WILDCARDS	"*?["
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಮಾರ್ಗ_ESC_WILDCARDS	"*?[{"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // VIM__H
