/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#ఒకవేళ defined(__VMS) || defined(__vms)
# ఒకవేళ !defined(VMS)
#  define VMS
# endif
#అయితే_ముగింపు

#అన్తర్భావ <decc$types.h>             // Required early for large-file support

#నిర్వచించు CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#నిర్వచించు FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#నిర్వచించు ద్విమాన_FILE_IO              // Use binary fileio
#నిర్వచించు USE_GETCWD
#నిర్వచించు USE_SYSTEM
#నిర్వచించు XPMATTRIBUTES_TYPE Xpm_లక్షణాలు

// Define when terminfo support found
#నిర్వచనం_రద్దు TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#నిర్వచించు HAVE_DATE_సమయం

// #undef USEBCOPY
#నిర్వచించు USEMEMMOVE
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
#నిర్వచించు HAVE_SIGCONTEXT

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

#నిర్వచించు TGETENT_ZERO_ERR
#నిర్వచించు HAVE_GETCWD
#నిర్వచించు HAVE_STRTOL
#నిర్వచించు HAVE_TGETENT
#నిర్వచించు HAVE_MEMSET
#నిర్వచించు HAVE_STRERROR
#నిర్వచించు HAVE_FCHOWN
#నిర్వచించు HAVE_RENAME
#నిర్వచించు HAVE_QSORT
#నిర్వచించు HAVE_FSYNC
#నిర్వచించు HAVE_GETPWUID
#నిర్వచించు HAVE_GETPWNAM
#నిర్వచించు	HAVE_STDLIB_H
#నిర్వచించు	HAVE_STRING_H
#నిర్వచించు	HAVE_ERRNO_H
#నిర్వచించు HAVE_OPENDIR
#నిర్వచించు HAVE_PUTENV
#నిర్వచించు HAVE_SETENV
#నిర్వచించు HAVE_SETJMP_H
#నిర్వచించు HAVE_MATH_H
#నిర్వచించు HAVE_GETTIMEOFDAY
#నిర్వచించు HAVE_PWD_H
#నిర్వచించు HAVE_NETDB_H
#నిర్వచించు	HAVE_DIRENT_H

#నిర్వచనం_రద్దు	HAVE_SYS_NDIR_H
#నిర్వచనం_రద్దు	HAVE_SYS_DIR_H
#నిర్వచనం_రద్దు	HAVE_NDIR_H
#నిర్వచనం_రద్దు	HAVE_SYS_WAIT_H
#నిర్వచనం_రద్దు  HAVE_SYS_SELECT_H
#నిర్వచనం_రద్దు  HAVE_SYS_UTSNAME_H
#నిర్వచనం_రద్దు  HAVE_SYS_SYSTEMINFO_H
#నిర్వచనం_రద్దు  HAVE_TERMCAP_H
#నిర్వచనం_రద్దు	HAVE_SGTTY_H
#నిర్వచనం_రద్దు	HAVE_SYS_IOCTL_H
#నిర్వచనం_రద్దు	HAVE_TERMIO_H
#నిర్వచనం_రద్దు	HAVE_STROPTS_H
#నిర్వచనం_రద్దు	HAVE_SYS_ప్రవాహం_H
#నిర్వచనం_రద్దు	HAVE_SYS_PTEM_H
#నిర్వచనం_రద్దు	HAVE_TERMIOS_H
#నిర్వచనం_రద్దు	HAVE_LIBC_H
#నిర్వచనం_రద్దు	HAVE_SYS_STATFS_H
#నిర్వచనం_రద్దు	HAVE_SYS_POLL_H
#నిర్వచనం_రద్దు  HAVE_FCHDIR
#నిర్వచనం_రద్దు  HAVE_LSTAT
#నిర్వచనం_రద్దు  HAVE_STDINT_H

// Default features
#నిర్వచించు FEAT_IPV6
#నిర్వచించు FEAT_XTERM_SAVE

#నిర్వచించు VIM_SIZEOF_INT  4
#నిర్వచించు VIM_SIZEOF_LONG 4

#ఒకవేళ __USE_OFF64_T
# define SIZEOF_OFF_T 8
#లేకపోతే
# define SIZEOF_OFF_T 4
#అయితే_ముగింపు

// Hardware specific
#ఒకవేళ defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_అక్షరశ్రేణులు_H
# undef  HAVE_SIGSETJMP
# undef  HAVE_ISNAN
# undef  HAVE_XOS_R_H
# define HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-2147483647-1)
# define LONG_LONG_MAX  (2147483647)
# define ULONG_LONG_MAX (4294967295U)

#లేకపోతే // ALPHA, IA64, X86_64
# define HAVE_GETTIMEOFDAY
# define HAVE_USLEEP
# define HAVE_STRCASECMP
# define HAVE_అక్షరశ్రేణులు_H
# define HAVE_SIGSETJMP
# undef  HAVE_XOS_R_H
# undef  HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-9223372036854775807-1)
# define LONG_LONG_MAX  (9223372036854775807)
# define ULONG_LONG_MAX (18446744073709551615U)

# ఒకవేళ defined(__DECC) && (__CRTL_VER >= 80500000) && (__STDC_VERSION__ >= 199901L) /* C99 */
#  define HAVE_ISINF
#  define HAVE_ISNAN
# endif

# ఒకవేళ defined(X86_64)
#  define HAVE_FSEEKO
#  define HAVE_STDINT_H
#  define HAVE_XOS_R_H
# endif

#అయితే_ముగింపు /* VAX [else] */

// Compiler specific
#ఒకవేళ defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_సమయం_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#లేకపోతే   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_సమయం_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#అయితే_ముగింపు

#ఒకవేళ defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#లేకపోతే
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#అయితే_ముగింపు

// GUI support defines
#ఒకవేళ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
# define X_INCLUDE_GRP_H  // To use getgrgid
# define XUSE_MTSAFE_API
# define HAVE_X11
# define WANT_X11
# ifdef  HAVE_XPM
#  define HAVE_X11_XPM_H
# endif
# define USE_FONTSET
# undef  X_LOCALE
#అయితే_ముగింపు

// Define needed types from stdint - older VMS do not have stdint.h
#నిర్వచితం_కాకపోతే HAVE_STDINT_H
   రకనిర్వచనం గుర్తులేని అక్షరం  uint8_t;
   రకనిర్వచనం గుర్తులేని పొట్టి uint16_t;
   రకనిర్వచనం గుర్తులేని పూర్ణాంకం   uint32_t;
#అయితే_ముగింపు
