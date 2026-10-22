/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#اگر defined(__VMS) || defined(__vms)
# اگر !defined(VMS)
#  define VMS
# endif
#اگر_ختم

#شامل <decc$types.h>             // Required early for large-file support

#تعریف_کرو CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#تعریف_کرو FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#تعریف_کرو ثنائی_FILE_IO              // Use binary fileio
#تعریف_کرو USE_GETCWD
#تعریف_کرو USE_SYSTEM
#تعریف_کرو XPMATTRIBUTES_TYPE Xpm_اوصاف

// Define when terminfo support found
#تعریف_منسوخ TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#تعریف_کرو HAVE_DATE_وقت

// #undef USEBCOPY
#تعریف_کرو USEMEMMOVE
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
#تعریف_کرو HAVE_SIGCONTEXT

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

#تعریف_کرو TGETENT_ZERO_ERR
#تعریف_کرو HAVE_GETCWD
#تعریف_کرو HAVE_STRTOL
#تعریف_کرو HAVE_TGETENT
#تعریف_کرو HAVE_MEMSET
#تعریف_کرو HAVE_STRERROR
#تعریف_کرو HAVE_FCHOWN
#تعریف_کرو HAVE_RENAME
#تعریف_کرو HAVE_QSORT
#تعریف_کرو HAVE_FSYNC
#تعریف_کرو HAVE_GETPWUID
#تعریف_کرو HAVE_GETPWNAM
#تعریف_کرو	HAVE_STDLIB_H
#تعریف_کرو	HAVE_STRING_H
#تعریف_کرو	HAVE_ERRNO_H
#تعریف_کرو HAVE_OPENDIR
#تعریف_کرو HAVE_PUTENV
#تعریف_کرو HAVE_SETENV
#تعریف_کرو HAVE_SETJMP_H
#تعریف_کرو HAVE_MATH_H
#تعریف_کرو HAVE_GETTIMEOFDAY
#تعریف_کرو HAVE_PWD_H
#تعریف_کرو HAVE_NETDB_H
#تعریف_کرو	HAVE_DIRENT_H

#تعریف_منسوخ	HAVE_SYS_NDIR_H
#تعریف_منسوخ	HAVE_SYS_DIR_H
#تعریف_منسوخ	HAVE_NDIR_H
#تعریف_منسوخ	HAVE_SYS_WAIT_H
#تعریف_منسوخ  HAVE_SYS_SELECT_H
#تعریف_منسوخ  HAVE_SYS_UTSNAME_H
#تعریف_منسوخ  HAVE_SYS_SYSTEMINFO_H
#تعریف_منسوخ  HAVE_TERMCAP_H
#تعریف_منسوخ	HAVE_SGTTY_H
#تعریف_منسوخ	HAVE_SYS_IOCTL_H
#تعریف_منسوخ	HAVE_TERMIO_H
#تعریف_منسوخ	HAVE_STROPTS_H
#تعریف_منسوخ	HAVE_SYS_دھارا_H
#تعریف_منسوخ	HAVE_SYS_PTEM_H
#تعریف_منسوخ	HAVE_TERMIOS_H
#تعریف_منسوخ	HAVE_LIBC_H
#تعریف_منسوخ	HAVE_SYS_STATFS_H
#تعریف_منسوخ	HAVE_SYS_POLL_H
#تعریف_منسوخ  HAVE_FCHDIR
#تعریف_منسوخ  HAVE_LSTAT
#تعریف_منسوخ  HAVE_STDINT_H

// Default features
#تعریف_کرو FEAT_IPV6
#تعریف_کرو FEAT_XTERM_SAVE

#تعریف_کرو VIM_SIZEOF_INT  4
#تعریف_کرو VIM_SIZEOF_LONG 4

#اگر __USE_OFF64_T
# define SIZEOF_OFF_T 8
#ورنہ
# define SIZEOF_OFF_T 4
#اگر_ختم

// Hardware specific
#اگر defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_لڑیاں_H
# undef  HAVE_SIGSETJMP
# undef  HAVE_ISNAN
# undef  HAVE_XOS_R_H
# define HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-2147483647-1)
# define LONG_LONG_MAX  (2147483647)
# define ULONG_LONG_MAX (4294967295U)

#ورنہ // ALPHA, IA64, X86_64
# define HAVE_GETTIMEOFDAY
# define HAVE_USLEEP
# define HAVE_STRCASECMP
# define HAVE_لڑیاں_H
# define HAVE_SIGSETJMP
# undef  HAVE_XOS_R_H
# undef  HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-9223372036854775807-1)
# define LONG_LONG_MAX  (9223372036854775807)
# define ULONG_LONG_MAX (18446744073709551615U)

# اگر defined(__DECC) && (__CRTL_VER >= 80500000) && (__STDC_VERSION__ >= 199901L) /* C99 */
#  define HAVE_ISINF
#  define HAVE_ISNAN
# endif

# اگر defined(X86_64)
#  define HAVE_FSEEKO
#  define HAVE_STDINT_H
#  define HAVE_XOS_R_H
# endif

#اگر_ختم /* VAX [else] */

// Compiler specific
#اگر defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_وقت_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#ورنہ   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_وقت_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#اگر_ختم

#اگر defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#ورنہ
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#اگر_ختم

// GUI support defines
#اگر defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
# define X_INCLUDE_GRP_H  // To use getgrgid
# define XUSE_MTSAFE_API
# define HAVE_X11
# define WANT_X11
# ifdef  HAVE_XPM
#  define HAVE_X11_XPM_H
# endif
# define USE_FONTSET
# undef  X_LOCALE
#اگر_ختم

// Define needed types from stdint - older VMS do not have stdint.h
#اگر_غیر_تعریف_شدہ HAVE_STDINT_H
   قسمتعریف غیرعلامتی حرف  uint8_t;
   قسمتعریف غیرعلامتی مختصر uint16_t;
   قسمتعریف غیرعلامتی صحیح   uint32_t;
#اگر_ختم
