/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#إذا defined(__VMS) || defined(__vms)
# إذا !defined(VMS)
#  define VMS
# endif
#جيڪڏهن_پورو

#ضمن <decc$types.h>             // Required early for large-file support

#وصف_ڪريو CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#وصف_ڪريو FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#وصف_ڪريو ٻٽو_FILE_IO              // Use binary fileio
#وصف_ڪريو USE_GETCWD
#وصف_ڪريو USE_SYSTEM
#وصف_ڪريو XPMATTRIBUTES_TYPE Xpm_خاصيتون

// Define when terminfo support found
#وصف_رد TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#وصف_ڪريو HAVE_DATE_وقت

// #undef USEBCOPY
#وصف_ڪريو USEMEMMOVE
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
#وصف_ڪريو HAVE_SIGCONTEXT

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

#وصف_ڪريو TGETENT_ZERO_ERR
#وصف_ڪريو HAVE_GETCWD
#وصف_ڪريو HAVE_STRTOL
#وصف_ڪريو HAVE_TGETENT
#وصف_ڪريو HAVE_MEMSET
#وصف_ڪريو HAVE_STRERROR
#وصف_ڪريو HAVE_FCHOWN
#وصف_ڪريو HAVE_RENAME
#وصف_ڪريو HAVE_QSORT
#وصف_ڪريو HAVE_FSYNC
#وصف_ڪريو HAVE_GETPWUID
#وصف_ڪريو HAVE_GETPWNAM
#وصف_ڪريو	HAVE_STDLIB_H
#وصف_ڪريو	HAVE_STRING_H
#وصف_ڪريو	HAVE_ERRNO_H
#وصف_ڪريو HAVE_OPENDIR
#وصف_ڪريو HAVE_PUTENV
#وصف_ڪريو HAVE_SETENV
#وصف_ڪريو HAVE_SETJMP_H
#وصف_ڪريو HAVE_MATH_H
#وصف_ڪريو HAVE_GETTIMEOFDAY
#وصف_ڪريو HAVE_PWD_H
#وصف_ڪريو HAVE_NETDB_H
#وصف_ڪريو	HAVE_DIRENT_H

#وصف_رد	HAVE_SYS_NDIR_H
#وصف_رد	HAVE_SYS_DIR_H
#وصف_رد	HAVE_NDIR_H
#وصف_رد	HAVE_SYS_WAIT_H
#وصف_رد  HAVE_SYS_SELECT_H
#وصف_رد  HAVE_SYS_UTSNAME_H
#وصف_رد  HAVE_SYS_SYSTEMINFO_H
#وصف_رد  HAVE_TERMCAP_H
#وصف_رد	HAVE_SGTTY_H
#وصف_رد	HAVE_SYS_IOCTL_H
#وصف_رد	HAVE_TERMIO_H
#وصف_رد	HAVE_STROPTS_H
#وصف_رد	HAVE_SYS_وهڪرو_H
#وصف_رد	HAVE_SYS_PTEM_H
#وصف_رد	HAVE_TERMIOS_H
#وصف_رد	HAVE_LIBC_H
#وصف_رد	HAVE_SYS_STATFS_H
#وصف_رد	HAVE_SYS_POLL_H
#وصف_رد  HAVE_FCHDIR
#وصف_رد  HAVE_LSTAT
#وصف_رد  HAVE_STDINT_H

// Default features
#وصف_ڪريو FEAT_IPV6
#وصف_ڪريو FEAT_XTERM_SAVE

#وصف_ڪريو VIM_SIZEOF_INT  4
#وصف_ڪريو VIM_SIZEOF_LONG 4

#إذا __USE_OFF64_T
# define SIZEOF_OFF_T 8
#وإلا
# define SIZEOF_OFF_T 4
#جيڪڏهن_پورو

// Hardware specific
#إذا defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_لڙيون_H
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
# define HAVE_لڙيون_H
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

#جيڪڏهن_پورو /* VAX [else] */

// Compiler specific
#إذا defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_وقت_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#وإلا   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_وقت_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#جيڪڏهن_پورو

#إذا defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#وإلا
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو

// Define needed types from stdint - older VMS do not have stdint.h
#جيڪڏهن_وصف_نه_ٿيل HAVE_STDINT_H
   تعريفنوع بنانشان حرف  uint8_t;
   تعريفنوع بنانشان ننڍو uint16_t;
   تعريفنوع بنانشان صحيح   uint32_t;
#جيڪڏهن_پورو
