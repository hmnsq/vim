/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VAXC
# include <types.h>
# include <stat.h>
#ਨਹੀਂਤਾਂ
# include <sys/types.h>
# include <sys/stat.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STDLIB_H
# include <stdlib.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define ਦੁਆਧਾਰੀ_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#ਜੇ_ਅੰਤ

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#ਜੇ defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
# include <unistd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#ਜੇ_ਅੰਤ

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#ਜੇ defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#ਜੇ_ਅੰਤ

// always use unlink() to remove files
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((ਅੱਖਰ *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((ਅੱਖਰ *)vms_fixfilename(x))
#  define mch_remove(x) delete((ਅੱਖਰ *)vms_fixfilename(x))
# ਨਹੀਂਤਾਂ
#  define vim_mkdir(x, y) mkdir((ਅੱਖਰ *)(x), y)
#  define mch_rmdir(x) rmdir((ਅੱਖਰ *)(x))
#  define mch_remove(x) unlink((ਅੱਖਰ *)(x))
# endif
#ਜੇ_ਅੰਤ

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGHASARG

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHASARG
# define SIGPROTOARG	(ਪੂਰਨ)
# define SIGDEFARG(s)	(ਪੂਰਨ s ਅਣਵਰਤਿਆ)
# define SIGDUMMYARG	0
#ਨਹੀਂਤਾਂ
# define SIGPROTOARG   (ਖਾਲੀ)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*sighandler_T) SIGPROTOARG;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) ਲੜੀ_ਲੰਬਾਈ((dirent)->d_name)
# endif
#ਨਹੀਂਤਾਂ
# define dirent ਸਿੱਧਾ
# define NAMLEN(dirent) (dirent)->d_namlen
# ਜੇ HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ਜੇ HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ਜੇ HAVE_NDIR_H
#  include <ndir.h>
# endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_ਸਮਾਂ_H
# include <sys/time.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਜੇ defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#ਜੇ_ਅੰਤ

#ਜੇ defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#ਜੇ_ਅੰਤ

#ਜੇ defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#ਜੇ_ਅੰਤ

#ਜੇ defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#ਜੇ_ਅੰਤ

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#ਜੇ !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BASENAMELEN	(MAXNAMLEN - 5)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PWD_H
# include <pwd.h>
#ਜੇ_ਅੰਤ

#ਜੇ (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_ਕੁੱਲ_MEM
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
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

# ਜੇ defined(FEAT_GUI_MOTIF)
#  define XFree XFREE
#  define XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
# endif
#ਜੇ_ਅੰਤ // VMS

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FLOCK
# include <sys/file.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dsc$descriptor   DESC;
#ਜੇ_ਅੰਤ

/*
 * Unix system-dependent file names
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# ਨਹੀਂਤਾਂ
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# ਨਹੀਂਤਾਂ
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#ਜੇ_ਅੰਤ


#ਜੇ !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# ਨਹੀਂਤਾਂ
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#ਜੇ_ਅੰਤ

#ਜੇ !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# ਨਹੀਂਤਾਂ
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# ਨਹੀਂਤਾਂ
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#ਨਹੀਂਤਾਂ
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  ifdef VMS
#   define VIMINFO_FILE  "sys$login:.viminfo"
#  ਨਹੀਂਤਾਂ
#   define VIMINFO_FILE "$HOME/.viminfo"
#  endif
# endif
# ਜੇ !defined(VIMINFO_FILE2) && defined(VMS)
#  define VIMINFO_FILE2 "sys$login:_viminfo"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EXRC_FILE
# define EXRC_FILE	".exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# ਨਹੀਂਤਾਂ
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# ਨਹੀਂਤਾਂ
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# ਨਹੀਂਤਾਂ
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_ERRORFILE		"errors.err"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_RUNTIMEPATH

# ifdef VMS
#  define DFLT_RUNTIMEPATH      "sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,sys$login:vimfiles/after"
#  define CLEAN_RUNTIMEPATH      "$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
# ਨਹੀਂਤਾਂ
#  ifdef RUNTIME_GLOBAL
#   ifdef RUNTIME_GLOBAL_AFTER
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
#   ਨਹੀਂਤਾਂ
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
#   endif
#  ਨਹੀਂਤਾਂ
#   define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#   define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
#   define XDG_RUNTIMEPATH_FB	"~/.config/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.config/vim/after"
#   define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#  endif
# endif

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#ਨਹੀਂਤਾਂ
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#ਜੇ_ਅੰਤ

// Special wildcards that need to be handled by the shell
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#ਜੇ defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#ਨਹੀਂਤਾਂ
# define MAXPATHL	1024
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS  // Use less memory because of older systems
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM (2*1024)
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT (5*1024)
# endif
#ਨਹੀਂਤਾਂ
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM	(5*1024)	 // use up to 5 Mbyte for a buffer
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT	(10*1024)    // use up to 10 Mbyte for Vim
# endif
#ਜੇ_ਅੰਤ

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#ਜੇ defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(ਮੰਜ਼ਿਲ, from, len) memmove((ਅੱਖਰ *)(ਮੰਜ਼ਿਲ), (ਅੱਖਰ *)(from), len)
#ਨਹੀਂਤਾਂ
# ifdef USEBCOPY
#  define mch_memmove(ਮੰਜ਼ਿਲ, from, len) bcopy((ਅੱਖਰ *)(from), (ਅੱਖਰ *)(ਮੰਜ਼ਿਲ), len)
# ਨਹੀਂਤਾਂ
    // ifdef USEMEMCPY
#  define mch_memmove(ਮੰਜ਼ਿਲ, from, len) ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)(ਮੰਜ਼ਿਲ), (ਅੱਖਰ *)(from), len)
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO
# ifdef HAVE_RENAME
#  define mch_rename(src, dst) rename(src, dst)
# ਨਹੀਂਤਾਂ
ਪੂਰਨ mch_rename(ਅਚਲ ਅੱਖਰ *src, ਅਚਲ ਅੱਖਰ *dest);
# endif
# ifndef VMS
#  ifdef __MVS__
  // on OS390 Unix getenv() doesn't return a pointer to persistent
  // storage -> use __getenv()
#   define mch_getenv(x) (char_u *)__getenv((ਅੱਖਰ *)(x))
#  ਨਹੀਂਤਾਂ
#   define mch_getenv(x) (char_u *)getenv((ਅੱਖਰ *)(x))
#  endif
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#ਜੇ_ਅੰਤ

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRING_H
# include <string.h>
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_ਲੜੀਆਂ_H) && !defined(NO_ਲੜੀਆਂ_WITH_STRING_H)
# include <strings.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SETJMP_H)
# include <setjmp.h>
# ifdef HAVE_SIGSETJMP
#  define JMP_BUF sigjmp_buf
#  define SETJMP(x) sigsetjmp((x), 1)
#  define LONGJMP siglongjmp
# ਨਹੀਂਤਾਂ
#  define JMP_BUF jmp_buf
#  define SETJMP(x) setjmp(x)
#  define LONGJMP longjmp
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_DUP
# define HAVE_DUP		// have dup()
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ST_ਬਹੁਲਕ		// have stat.st_mode

// We have three kinds of ACL support.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

