/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#إذا defined(__VMS) || defined(__vms)
# إذا !defined(VMS)
#  define VMS
# endif
#د_که_پای

#ضمن <decc$types.h>             // Required early for large-file support

#تعریف_کړه CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#تعریف_کړه FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#تعریف_کړه دوګونی_FILE_IO              // Use binary fileio
#تعریف_کړه USE_GETCWD
#تعریف_کړه USE_SYSTEM
#تعریف_کړه XPMATTRIBUTES_TYPE Xpm_ځانګړتیاوې

// Define when terminfo support found
#تعریف_لغوه TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#تعریف_کړه HAVE_DATE_وخت

// #undef USEBCOPY
#تعریف_کړه USEMEMMOVE
// #undef USEMEMCPY

// Define when "man -s 2" is to be used
// #undef USEMAN_S

// Define to empty if the keyword does not work.
// #undef const

// Define to `int' if <sys/types.h> doesn't define.
// #undef mode_t

// Define to `long' if <sys/types.h> doesn't define.
// #undef off_t

// Define to `long' if <sys/types.h> doesn't define.
// #undef pid_t

// Define to `unsigned' if <sys/types.h> doesn't define.
// #undef size_t

// Define to `int' if <sys/types.h> doesn't define.
// #undef uid_t

// Define to `int' if <sys/types.h> doesn't define.
// #undef gid_t

// Define to `long' if <sys/types.h> doesn't define.
// #undef ino_t

// Define if you have the nanosleep() function.
// #undef HAVE_NANOSLEEP

// Define if you can safely include both <sys/time.h> and <sys/select.h>.
// #undef SYS_SELECT_WITH_SYS_TIME

// Define if struct sigcontext is present
#تعریف_کړه HAVE_SIGCONTEXT

// Define if toupper/tolower only work on lower/uppercase characters
// #define BROKEN_TOUPPER

// Define if tgetstr() has a second argument that is (char *)
// #undef TGETSTR_CHAR_P

// Define if you have the sigset() function.
// #undef HAVE_SIGSET

// Define if you have the setpgid() function.
// #undef HAVE_SETPGID

// Define if you have the setsid() function.
// #undef HAVE_SETSID

// Define if you have the sigset() function.
// #undef HAVE_SIGSET

#تعریف_کړه TGETENT_ZERO_ERR
#تعریف_کړه HAVE_GETCWD
#تعریف_کړه HAVE_STRTOL
#تعریف_کړه HAVE_TGETENT
#تعریف_کړه HAVE_MEMSET
#تعریف_کړه HAVE_STRERROR
#تعریف_کړه HAVE_FCHOWN
#تعریف_کړه HAVE_RENAME
#تعریف_کړه HAVE_QSORT
#تعریف_کړه HAVE_FSYNC
#تعریف_کړه HAVE_GETPWUID
#تعریف_کړه HAVE_GETPWNAM
#تعریف_کړه	HAVE_STDLIB_H
#تعریف_کړه	HAVE_STRING_H
#تعریف_کړه	HAVE_ERRNO_H
#تعریف_کړه HAVE_OPENDIR
#تعریف_کړه HAVE_PUTENV
#تعریف_کړه HAVE_SETENV
#تعریف_کړه HAVE_SETJMP_H
#تعریف_کړه HAVE_MATH_H
#تعریف_کړه HAVE_GETTIMEOFDAY
#تعریف_کړه HAVE_PWD_H
#تعریف_کړه HAVE_NETDB_H
#تعریف_کړه	HAVE_DIRENT_H

#تعریف_لغوه	HAVE_SYS_NDIR_H
#تعریف_لغوه	HAVE_SYS_DIR_H
#تعریف_لغوه	HAVE_NDIR_H
#تعریف_لغوه	HAVE_SYS_WAIT_H
#تعریف_لغوه  HAVE_SYS_SELECT_H
#تعریف_لغوه  HAVE_SYS_UTSNAME_H
#تعریف_لغوه  HAVE_SYS_SYSTEMINFO_H
#تعریف_لغوه  HAVE_TERMCAP_H
#تعریف_لغوه	HAVE_SGTTY_H
#تعریف_لغوه	HAVE_SYS_IOCTL_H
#تعریف_لغوه	HAVE_TERMIO_H
#تعریف_لغوه	HAVE_STROPTS_H
#تعریف_لغوه	HAVE_SYS_بهیر_H
#تعریف_لغوه	HAVE_SYS_PTEM_H
#تعریف_لغوه	HAVE_TERMIOS_H
#تعریف_لغوه	HAVE_LIBC_H
#تعریف_لغوه	HAVE_SYS_STATFS_H
#تعریف_لغوه	HAVE_SYS_POLL_H
#تعریف_لغوه  HAVE_FCHDIR
#تعریف_لغوه  HAVE_LSTAT
#تعریف_لغوه  HAVE_STDINT_H

// Default features
#تعریف_کړه FEAT_IPV6
#تعریف_کړه FEAT_XTERM_SAVE

#تعریف_کړه VIM_SIZEOF_INT  4
#تعریف_کړه VIM_SIZEOF_LONG 4

#إذا __USE_OFF64_T
# define SIZEOF_OFF_T 8
#وإلا
# define SIZEOF_OFF_T 4
#د_که_پای

// Hardware specific
#إذا defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_مزي_ګانې_H
# undef  HAVE_SIGSETJMP
# undef  HAVE_ISNAN
# undef  HAVE_XOS_R_H
# define HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-2147483647-1)
# define LONG_LONG_MAX  (2147483647)
# define ULONG_LONG_MAX (4294967295U)

#وإلا // ALPHA, IA64, X86_64
# define HAVE_GETTIMEOFDAY
# define HAVE_USLEEP
# define HAVE_STRCASECMP
# define HAVE_مزي_ګانې_H
# define HAVE_SIGSETJMP
# undef  HAVE_XOS_R_H
# undef  HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-9223372036854775807-1)
# define LONG_LONG_MAX  (9223372036854775807)
# define ULONG_LONG_MAX (18446744073709551615U)

# إذا defined(__DECC) && (__CRTL_VER >= 80500000) && (__STDC_VERSION__ >= 199901L) /* C99 */
#  define HAVE_ISINF
#  define HAVE_ISNAN
# endif

# إذا defined(X86_64)
#  define HAVE_FSEEKO
#  define HAVE_STDINT_H
#  define HAVE_XOS_R_H
# endif

#د_که_پای /* VAX [else] */

// Compiler specific
#إذا defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_وخت_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#وإلا   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_وخت_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#د_که_پای

#إذا defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#وإلا
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#د_که_پای

// GUI support defines
#إذا defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
# define X_INCLUDE_GRP_H  // To use getgrgid
# define XUSE_MTSAFE_API
# define HAVE_X11
# define WANT_X11
# ifdef  HAVE_XPM
#  define HAVE_X11_XPM_H
# endif
# define USE_FONTSET
# undef  X_LOCALE
#د_که_پای

// Define needed types from stdint - older VMS do not have stdint.h
#که_نه_تعریف_شوی HAVE_STDINT_H
   تعريفنوع بینښه حرف  uint8_t;
   تعريفنوع بینښه لنډ uint16_t;
   تعريفنوع بینښه صحيح   uint32_t;
#د_که_پای
