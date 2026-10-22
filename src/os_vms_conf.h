/*
 * os_vms_conf.h.  Replaces auto/config.h for VMS
 *
 */

#ಆದರೆ defined(__VMS) || defined(__vms)
# ಆದರೆ !defined(VMS)
#  define VMS
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <decc$types.h>             // Required early for large-file support

#ವ್ಯಾಖ್ಯಾನಿಸು CASE_INSENSITIVE_FILENAME   // Open VMS is case insensitive
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_ILLEGAL "|*#?%"       // Illegal characters in a file name
#ವ್ಯಾಖ್ಯಾನಿಸು ದ್ವಿಮಾನ_FILE_IO              // Use binary fileio
#ವ್ಯಾಖ್ಯಾನಿಸು USE_GETCWD
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SYSTEM
#ವ್ಯಾಖ್ಯಾನಿಸು XPMATTRIBUTES_TYPE Xpm_ಗುಣಲಕ್ಷಣಗಳು

// Define when terminfo support found
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು TERMINFO

// Define when termcap.h contains ospeed
// #define HAVE_OSPEED

// Define when termcap.h contains UP, BC and PC
// #define HAVE_UP_BC_PC

// Define when termcap.h defines outfuntype
//#define HAVE_OUTFUNTYPE

// Define when __DATE__ " " __TIME__ can be used
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_DATE_ಸಮಯ

// #undef USEBCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು USEMEMMOVE
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
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SIGCONTEXT

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

#ವ್ಯಾಖ್ಯಾನಿಸು TGETENT_ZERO_ERR
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETCWD
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_STRTOL
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_TGETENT
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_MEMSET
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_STRERROR
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_FCHOWN
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_RENAME
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_QSORT
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_FSYNC
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETPWUID
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETPWNAM
#ವ್ಯಾಖ್ಯಾನಿಸು	HAVE_STDLIB_H
#ವ್ಯಾಖ್ಯಾನಿಸು	HAVE_STRING_H
#ವ್ಯಾಖ್ಯಾನಿಸು	HAVE_ERRNO_H
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_OPENDIR
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_PUTENV
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SETENV
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SETJMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_MATH_H
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETTIMEOFDAY
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_PWD_H
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_NETDB_H
#ವ್ಯಾಖ್ಯಾನಿಸು	HAVE_DIRENT_H

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_NDIR_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_DIR_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_NDIR_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_WAIT_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_SYS_SELECT_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_SYS_UTSNAME_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_SYS_SYSTEMINFO_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_TERMCAP_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SGTTY_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_IOCTL_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_TERMIO_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_STROPTS_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_ಹರಿವು_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_PTEM_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_TERMIOS_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_LIBC_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_STATFS_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	HAVE_SYS_POLL_H
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_FCHDIR
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_LSTAT
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_STDINT_H

// Default features
#ವ್ಯಾಖ್ಯಾನಿಸು FEAT_IPV6
#ವ್ಯಾಖ್ಯಾನಿಸು FEAT_XTERM_SAVE

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_SIZEOF_INT  4
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_SIZEOF_LONG 4

#ಆದರೆ __USE_OFF64_T
# define SIZEOF_OFF_T 8
#ಇಲ್ಲದಿದ್ದರೆ
# define SIZEOF_OFF_T 4
#ಆದರೆ_ಅಂತ್ಯ

// Hardware specific
#ಆದರೆ defined(__VAX) || defined(VAX)
# undef  HAVE_GETTIMEOFDAY
# undef  HAVE_USLEEP
# undef  HAVE_STRCASECMP
# undef  HAVE_ಅಕ್ಷರಸರಣಿಗಳು_H
# undef  HAVE_SIGSETJMP
# undef  HAVE_ISNAN
# undef  HAVE_XOS_R_H
# define HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-2147483647-1)
# define LONG_LONG_MAX  (2147483647)
# define ULONG_LONG_MAX (4294967295U)

#ಇಲ್ಲದಿದ್ದರೆ // ALPHA, IA64, X86_64
# define HAVE_GETTIMEOFDAY
# define HAVE_USLEEP
# define HAVE_STRCASECMP
# define HAVE_ಅಕ್ಷರಸರಣಿಗಳು_H
# define HAVE_SIGSETJMP
# undef  HAVE_XOS_R_H
# undef  HAVE_NO_LONG_LONG
# define LONG_LONG_MIN  (-9223372036854775807-1)
# define LONG_LONG_MAX  (9223372036854775807)
# define ULONG_LONG_MAX (18446744073709551615U)

# ಆದರೆ defined(__DECC) && (__CRTL_VER >= 80500000) && (__STDC_VERSION__ >= 199901L) /* C99 */
#  define HAVE_ISINF
#  define HAVE_ISNAN
# endif

# ಆದರೆ defined(X86_64)
#  define HAVE_FSEEKO
#  define HAVE_STDINT_H
#  define HAVE_XOS_R_H
# endif

#ಆದರೆ_ಅಂತ್ಯ /* VAX [else] */

// Compiler specific
#ಆದರೆ defined(VAXC) || defined(__VAXC)
# undef  HAVE_SELECT
# undef  HAVE_FCNTL_H
# undef  HAVE_UNISTD_H
# undef  HAVE_SYS_ಸಮಯ_H
# undef  HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# undef	HAVE_STRFTIME
#ಇಲ್ಲದಿದ್ದರೆ   // DECC
# define HAVE_SELECT
# define HAVE_FCNTL_H
# define HAVE_UNISTD_H 1
# define HAVE_SYS_ಸಮಯ_H
# define HAVE_LOCALE_H
# define BROKEN_LOCALE
# undef  DYNAMIC_ICONV
# define HAVE_STRFTIME
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_ICONV)
# define HAVE_ICONV_H
# define HAVE_ICONV
#ಇಲ್ಲದಿದ್ದರೆ
# undef HAVE_ICONV_H
# undef HAVE_ICONV
#ಆದರೆ_ಅಂತ್ಯ

// GUI support defines
#ಆದರೆ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
# define X_INCLUDE_GRP_H  // To use getgrgid
# define XUSE_MTSAFE_API
# define HAVE_X11
# define WANT_X11
# ifdef  HAVE_XPM
#  define HAVE_X11_XPM_H
# endif
# define USE_FONTSET
# undef  X_LOCALE
#ಆದರೆ_ಅಂತ್ಯ

// Define needed types from stdint - older VMS do not have stdint.h
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_STDINT_H
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  uint8_t;
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ uint16_t;
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ   uint32_t;
#ಆದರೆ_ಅಂತ್ಯ
