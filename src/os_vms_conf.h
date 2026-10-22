/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#ਜੇ defined(__VMS) || defined(__vms)
# ਜੇ !defined(VMS)
#  define VMS
# endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <decc$types.h>             // Required early for large-file support

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੁਆਧਾਰੀ_FILE_IO              // Use binary fileio
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_GETCWD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SYSTEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPMATTRIBUTES_TYPE Xpm_ਗੁਣ

// Define when terminfo support found
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_DATE_ਸਮਾਂ

// #undef USEBCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEMEMMOVE
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SIGCONTEXT

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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TGETENT_ZERO_ERR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETCWD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STRTOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_TGETENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_MEMSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STRERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_FCHOWN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_RENAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_QSORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_FSYNC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETPWUID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETPWNAM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	HAVE_STDLIB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	HAVE_STRING_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	HAVE_ERRNO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_OPENDIR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_PUTENV
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SETENV
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SETJMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_MATH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETTIMEOFDAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_PWD_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_NETDB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	HAVE_DIRENT_H

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_NDIR_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_DIR_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_NDIR_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_WAIT_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_SYS_SELECT_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_SYS_UTSNAME_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_SYS_SYSTEMINFO_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_TERMCAP_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SGTTY_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_IOCTL_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_TERMIO_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_STROPTS_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_ਧਾਰਾ_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_PTEM_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_TERMIOS_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_LIBC_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_STATFS_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	HAVE_SYS_POLL_H
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_FCHDIR
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_LSTAT
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_STDINT_H

// Default features
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEAT_IPV6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEAT_XTERM_SAVE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_SIZEOF_INT  4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_SIZEOF_LONG 4

#ਜੇ __USE_OFF64_T
# define SIZEOF_OFF_T 8
#ਨਹੀਂਤਾਂ
# define SIZEOF_OFF_T 4
#ਜੇ_ਅੰਤ

// Hardware specific
#ਜੇ defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_ਲੜੀਆਂ_H
# undef  HAVE_SIGSETJMP
# undef  HAVE_ISNAN
# undef  HAVE_XOS_R_H
# define HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-2147483647-1)
# define LONG_LONG_MAX  (2147483647)
# define ULONG_LONG_MAX (4294967295U)

#ਨਹੀਂਤਾਂ // ALPHA, IA64, X86_64
# define HAVE_GETTIMEOFDAY
# define HAVE_USLEEP
# define HAVE_STRCASECMP
# define HAVE_ਲੜੀਆਂ_H
# define HAVE_SIGSETJMP
# undef  HAVE_XOS_R_H
# undef  HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-9223372036854775807-1)
# define LONG_LONG_MAX  (9223372036854775807)
# define ULONG_LONG_MAX (18446744073709551615U)

# ਜੇ defined(__DECC) && (__CRTL_VER >= 80500000) && (__STDC_VERSION__ >= 199901L) /* C99 */
#  define HAVE_ISINF
#  define HAVE_ISNAN
# endif

# ਜੇ defined(X86_64)
#  define HAVE_FSEEKO
#  define HAVE_STDINT_H
#  define HAVE_XOS_R_H
# endif

#ਜੇ_ਅੰਤ /* VAX [else] */

// Compiler specific
#ਜੇ defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_ਸਮਾਂ_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#ਨਹੀਂਤਾਂ   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_ਸਮਾਂ_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#ਨਹੀਂਤਾਂ
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#ਜੇ_ਅੰਤ

// GUI support defines
#ਜੇ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
# define X_INCLUDE_GRP_H  // To use getgrgid
# define XUSE_MTSAFE_API
# define HAVE_X11
# define WANT_X11
# ifdef  HAVE_XPM
#  define HAVE_X11_XPM_H
# endif
# define USE_FONTSET
# undef  X_LOCALE
#ਜੇ_ਅੰਤ

// Define needed types from stdint - older VMS do not have stdint.h
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_STDINT_H
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  uint8_t;
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ uint16_t;
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ   uint32_t;
#ਜੇ_ਅੰਤ
