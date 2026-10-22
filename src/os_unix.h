/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ضمن <stdio.h>
#ضمن <ctype.h>

#که_تعریف_شوی VAXC
# include <types.h>
# include <stat.h>
#وإلا
# include <sys/types.h>
# include <sys/stat.h>
#د_که_پای

#که_تعریف_شوی HAVE_STDLIB_H
# include <stdlib.h>
#د_که_پای

#که_تعریف_شوی __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define دوګونی_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#د_که_پای

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#إذا defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#د_که_پای

#که_تعریف_شوی HAVE_UNISTD_H
# include <unistd.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#د_که_پای

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#إذا defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#د_که_پای

// always use unlink() to remove files
#که_نه_تعریف_شوی PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((حرف *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((حرف *)vms_fixfilename(x))
#  define mch_remove(x) delete((حرف *)vms_fixfilename(x))
# وإلا
#  define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#  define mch_rmdir(x) rmdir((حرف *)(x))
#  define mch_remove(x) unlink((حرف *)(x))
# endif
#د_که_پای

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#تعریف_کړه SIGHASARG

#که_تعریف_شوی SIGHASARG
# define SIGPROTOARG	(صحيح)
# define SIGDEFARG(s)	(صحيح s ناکارول_شوی)
# define SIGDUMMYARG	0
#وإلا
# define SIGPROTOARG   (فراغ)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#د_که_پای

تعريفنوع فراغ (*sighandler_T) SIGPROTOARG;

#که_تعریف_شوی HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) مزي_اوږدوالی((dirent)->d_name)
# endif
#وإلا
# define dirent مستقیم
# define NAMLEN(dirent) (dirent)->d_namlen
# إذا HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# إذا HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# إذا HAVE_NDIR_H
#  include <ndir.h>
# endif
#د_که_پای

#ضمن <time.h>
#که_تعریف_شوی HAVE_SYS_وخت_H
# include <sys/time.h>
#د_که_پای

#ضمن <signal.h>

#إذا defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#د_که_پای

#إذا defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#د_که_پای

#إذا defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#د_که_پای

#إذا defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#د_که_پای

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#إذا !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#د_که_پای

#تعریف_کړه BASENAMELEN	(MAXNAMLEN - 5)

#که_تعریف_شوی HAVE_PWD_H
# include <pwd.h>
#د_که_پای

#إذا (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_ټول_MEM
#د_که_پای


#که_تعریف_شوی VMS
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

# إذا defined(FEAT_GUI_MOTIF)
#  define XFree XFREE
#  define XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
# endif
#د_که_پای // VMS

#که_تعریف_شوی HAVE_FLOCK
# include <sys/file.h>
#د_که_پای

#که_تعریف_شوی VMS
تعريفنوع بنية dsc$descriptor   DESC;
#د_که_پای

/*
 * Unix system-dependent file names
 */
#که_نه_تعریف_شوی SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#د_که_پای
#که_نه_تعریف_شوی SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#د_که_پای
#که_نه_تعریف_شوی DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#د_که_پای
#که_نه_تعریف_شوی SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#د_که_پای

#که_نه_تعریف_شوی USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# وإلا
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#د_که_پای

#إذا !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#د_که_پای

#که_نه_تعریف_شوی USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# وإلا
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#د_که_پای


#إذا !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# وإلا
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#د_که_پای

#که_نه_تعریف_شوی XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#د_که_پای

#إذا !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#د_که_پای

#که_نه_تعریف_شوی USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# وإلا
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#د_که_پای

#که_نه_تعریف_شوی USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# وإلا
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#د_که_پای

#که_تعریف_شوی VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#وإلا
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#د_که_پای

#که_نه_تعریف_شوی VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#د_که_پای

#که_نه_تعریف_شوی EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  ifdef VMS
#   define VIMINFO_FILE  "sys$login:.viminfo"
#  وإلا
#   define VIMINFO_FILE "$HOME/.viminfo"
#  endif
# endif
# إذا !defined(VIMINFO_FILE2) && defined(VMS)
#  define VIMINFO_FILE2 "sys$login:_viminfo"
# endif
#د_که_پای

#که_نه_تعریف_شوی EXRC_FILE
# define EXRC_FILE	".exrc"
#د_که_پای

#که_نه_تعریف_شوی VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#د_که_پای

#که_تعریف_شوی FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#د_که_پای

#که_نه_تعریف_شوی SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#د_که_پای

#که_نه_تعریف_شوی DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# وإلا
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#د_که_پای

#که_نه_تعریف_شوی DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# وإلا
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#د_که_پای

#که_نه_تعریف_شوی DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# وإلا
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#د_که_پای

#تعریف_کړه DFLT_ERRORFILE		"errors.err"

#که_نه_تعریف_شوی DFLT_RUNTIMEPATH

# ifdef VMS
#  define DFLT_RUNTIMEPATH      "sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,sys$login:vimfiles/after"
#  define CLEAN_RUNTIMEPATH      "$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
# وإلا
#  ifdef RUNTIME_GLOBAL
#   ifdef RUNTIME_GLOBAL_AFTER
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
#   وإلا
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
#   endif
#  وإلا
#   define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#   define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
#   define XDG_RUNTIMEPATH_FB	"~/.config/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.config/vim/after"
#   define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#  endif
# endif

#د_که_پای

#که_تعریف_شوی VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#وإلا
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#د_که_پای

// Special wildcards that need to be handled by the shell
#تعریف_کړه SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#تعریف_کړه CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#إذا defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#وإلا
# define MAXPATHL	1024
#د_که_پای

#تعریف_کړه CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#که_تعریف_شوی VMS  // Use less memory because of older systems
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM (2*1024)
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT (5*1024)
# endif
#وإلا
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM	(5*1024)	 // use up to 5 Mbyte for a buffer
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT	(10*1024)    // use up to 10 Mbyte for Vim
# endif
#د_که_پای

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#إذا defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(منزل, from, len) memmove((حرف *)(منزل), (حرف *)(from), len)
#وإلا
# ifdef USEBCOPY
#  define mch_memmove(منزل, from, len) bcopy((حرف *)(from), (حرف *)(منزل), len)
# وإلا
    // ifdef USEMEMCPY
#  define mch_memmove(منزل, from, len) حافظه_نقل((حرف *)(منزل), (حرف *)(from), len)
# endif
#د_که_پای

#که_نه_تعریف_شوی PROTO
# ifdef HAVE_RENAME
#  define mch_rename(src, dst) rename(src, dst)
# وإلا
صحيح mch_rename(ثابت حرف *src, ثابت حرف *dest);
# endif
# ifndef VMS
#  ifdef __MVS__
  // on OS390 Unix getenv() doesn't return a pointer to persistent
  // storage -> use __getenv()
#   define mch_getenv(x) (char_u *)__getenv((حرف *)(x))
#  وإلا
#   define mch_getenv(x) (char_u *)getenv((حرف *)(x))
#  endif
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#د_که_پای

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#که_تعریف_شوی HAVE_STRING_H
# include <string.h>
#د_که_پای
#إذا defined(HAVE_مزي_ګانې_H) && !defined(NO_مزي_ګانې_WITH_STRING_H)
# include <strings.h>
#د_که_پای

#إذا defined(HAVE_SETJMP_H)
# include <setjmp.h>
# ifdef HAVE_SIGSETJMP
#  define JMP_BUF sigjmp_buf
#  define SETJMP(x) sigsetjmp((x), 1)
#  define LONGJMP siglongjmp
# وإلا
#  define JMP_BUF jmp_buf
#  define SETJMP(x) setjmp(x)
#  define LONGJMP longjmp
# endif
#د_که_پای

#که_نه_تعریف_شوی HAVE_DUP
# define HAVE_DUP		// have dup()
#د_که_پای
#تعریف_کړه HAVE_ST_ډېرپېښ		// have stat.st_mode

// We have three kinds of ACL support.
#تعریف_کړه HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

