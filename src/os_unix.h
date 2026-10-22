/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VAXC
# include <types.h>
# include <stat.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include <sys/types.h>
# include <sys/stat.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STDLIB_H
# include <stdlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define ದ್ವಿಮಾನ_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#ಆದರೆ_ಅಂತ್ಯ

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#ಆದರೆ defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
# include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#ಆದರೆ defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#ಆದರೆ_ಅಂತ್ಯ

// always use unlink() to remove files
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((ಅಕ್ಷರ *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((ಅಕ್ಷರ *)vms_fixfilename(x))
#  define mch_remove(x) delete((ಅಕ್ಷರ *)vms_fixfilename(x))
# ಇಲ್ಲದಿದ್ದರೆ
#  define vim_mkdir(x, y) mkdir((ಅಕ್ಷರ *)(x), y)
#  define mch_rmdir(x) rmdir((ಅಕ್ಷರ *)(x))
#  define mch_remove(x) unlink((ಅಕ್ಷರ *)(x))
# endif
#ಆದರೆ_ಅಂತ್ಯ

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#ವ್ಯಾಖ್ಯಾನಿಸು SIGHASARG

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHASARG
# define SIGPROTOARG	(ಪೂರ್ಣಾಂಕ)
# define SIGDEFARG(s)	(ಪೂರ್ಣಾಂಕ s ಬಳಸದ)
# define SIGDUMMYARG	0
#ಇಲ್ಲದಿದ್ದರೆ
# define SIGPROTOARG   (ಶೂನ್ಯ)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*sighandler_T) SIGPROTOARG;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((dirent)->d_name)
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define dirent ನೇರ
# define NAMLEN(dirent) (dirent)->d_namlen
# ಆದರೆ HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ಆದರೆ HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ಆದರೆ HAVE_NDIR_H
#  include <ndir.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <time.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_ಸಮಯ_H
# include <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಆದರೆ defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#ಆದರೆ !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು BASENAMELEN	(MAXNAMLEN - 5)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PWD_H
# include <pwd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_ಒಟ್ಟು_MEM
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <unixio.h>
# include <unixlib.h>
# include <signal.h>
# include <file.h>
# include <ssdef.h>
# include <descrip.h>
# include <libclidef.h>
# include <lnmdef.h>
# include <psldef.h>
# include <prvdef.h>
# include <dvidef.h>
# include <dcdef.h>
# include <stsdef.h>
# include <iodef.h>
# include <ttdef.h>
# include <tt2def.h>
# include <jpidef.h>
# include <rms.h>
# include <trmdef.h>
# include <string.h>
# include <starlet.h>
# include <socket.h>
# include <lib$routines.h>
# include <libdef.h>
# include <libdtdef.h>

# ಆದರೆ defined(FEAT_GUI_MOTIF)
#  define XFree XFREE
#  define XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
# endif
#ಆದರೆ_ಅಂತ್ಯ // VMS

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FLOCK
# include <sys/file.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dsc$descriptor   DESC;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Unix system-dependent file names
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# ಇಲ್ಲದಿದ್ದರೆ
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# ಇಲ್ಲದಿದ್ದರೆ
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# ಇಲ್ಲದಿದ್ದರೆ
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# ಇಲ್ಲದಿದ್ದರೆ
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# ಇಲ್ಲದಿದ್ದರೆ
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  ifdef VMS
#   define VIMINFO_FILE  "sys$login:.viminfo"
#  ಇಲ್ಲದಿದ್ದರೆ
#   define VIMINFO_FILE "$HOME/.viminfo"
#  endif
# endif
# ಆದರೆ !defined(VIMINFO_FILE2) && defined(VMS)
#  define VIMINFO_FILE2 "sys$login:_viminfo"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EXRC_FILE
# define EXRC_FILE	".exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# ಇಲ್ಲದಿದ್ದರೆ
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# ಇಲ್ಲದಿದ್ದರೆ
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# ಇಲ್ಲದಿದ್ದರೆ
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_ERRORFILE		"errors.err"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_RUNTIMEPATH

# ifdef VMS
#  define DFLT_RUNTIMEPATH      "sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,sys$login:vimfiles/after"
#  define CLEAN_RUNTIMEPATH      "$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef RUNTIME_GLOBAL
#   ifdef RUNTIME_GLOBAL_AFTER
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
#   ಇಲ್ಲದಿದ್ದರೆ
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
#   define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#   define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
#   define XDG_RUNTIMEPATH_FB	"~/.config/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.config/vim/after"
#   define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#  endif
# endif

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#ಇಲ್ಲದಿದ್ದರೆ
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#ಆದರೆ_ಅಂತ್ಯ

// Special wildcards that need to be handled by the shell
#ವ್ಯಾಖ್ಯಾನಿಸು SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#ಆದರೆ defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#ಇಲ್ಲದಿದ್ದರೆ
# define MAXPATHL	1024
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS  // Use less memory because of older systems
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM (2*1024)
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT (5*1024)
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM	(5*1024)	 // use up to 5 Mbyte for a buffer
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT	(10*1024)    // use up to 10 Mbyte for Vim
# endif
#ಆದರೆ_ಅಂತ್ಯ

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#ಆದರೆ defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(ಗಮ್ಯ, from, len) memmove((ಅಕ್ಷರ *)(ಗಮ್ಯ), (ಅಕ್ಷರ *)(from), len)
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef USEBCOPY
#  define mch_memmove(ಗಮ್ಯ, from, len) bcopy((ಅಕ್ಷರ *)(from), (ಅಕ್ಷರ *)(ಗಮ್ಯ), len)
# ಇಲ್ಲದಿದ್ದರೆ
    // ifdef USEMEMCPY
#  define mch_memmove(ಗಮ್ಯ, from, len) ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)(ಗಮ್ಯ), (ಅಕ್ಷರ *)(from), len)
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO
# ifdef HAVE_RENAME
#  define mch_rename(src, dst) rename(src, dst)
# ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ mch_rename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dest);
# endif
# ifndef VMS
#  ifdef __MVS__
  // on OS390 Unix getenv() doesn't return a pointer to persistent
  // storage -> use __getenv()
#   define mch_getenv(x) (char_u *)__getenv((ಅಕ್ಷರ *)(x))
#  ಇಲ್ಲದಿದ್ದರೆ
#   define mch_getenv(x) (char_u *)getenv((ಅಕ್ಷರ *)(x))
#  endif
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRING_H
# include <string.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_ಅಕ್ಷರಸರಣಿಗಳು_H) && !defined(NO_ಅಕ್ಷರಸರಣಿಗಳು_WITH_STRING_H)
# include <strings.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SETJMP_H)
# include <setjmp.h>
# ifdef HAVE_SIGSETJMP
#  define JMP_BUF sigjmp_buf
#  define SETJMP(x) sigsetjmp((x), 1)
#  define LONGJMP siglongjmp
# ಇಲ್ಲದಿದ್ದರೆ
#  define JMP_BUF jmp_buf
#  define SETJMP(x) setjmp(x)
#  define LONGJMP longjmp
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_DUP
# define HAVE_DUP		// have dup()
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_ST_ಬಹುಲಕ		// have stat.st_mode

// We have three kinds of ACL support.
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

