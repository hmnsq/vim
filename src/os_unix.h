/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ضمن <stdio.h>
#ضمن <ctype.h>

#اگر_تعریف_شده VAXC
# include <types.h>
# include <stat.h>
#وإلا
# include <sys/types.h>
# include <sys/stat.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_STDLIB_H
# include <stdlib.h>
#پایان_اگر

#اگر_تعریف_شده __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define دودویی_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#پایان_اگر

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#إذا defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#پایان_اگر

#اگر_تعریف_شده HAVE_UNISTD_H
# include <unistd.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#پایان_اگر

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#إذا defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#پایان_اگر

// always use unlink() to remove files
#اگر_تعریف_نشده PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((حرف *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((حرف *)vms_fixfilename(x))
#  define mch_remove(x) delete((حرف *)vms_fixfilename(x))
# وإلا
#  define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#  define mch_rmdir(x) rmdir((حرف *)(x))
#  define mch_remove(x) unlink((حرف *)(x))
# endif
#پایان_اگر

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#تعریف_کن SIGHASARG

#اگر_تعریف_شده SIGHASARG
# define SIGPROTOARG	(صحيح)
# define SIGDEFARG(s)	(صحيح s استفادهنشده)
# define SIGDUMMYARG	0
#وإلا
# define SIGPROTOARG   (فراغ)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#پایان_اگر

تعريفنوع فراغ (*sighandler_T) SIGPROTOARG;

#اگر_تعریف_شده HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) طول_رشته((dirent)->d_name)
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
#پایان_اگر

#ضمن <time.h>
#اگر_تعریف_شده HAVE_SYS_زمان_H
# include <sys/time.h>
#پایان_اگر

#ضمن <signal.h>

#إذا defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#پایان_اگر

#إذا defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#پایان_اگر

#إذا defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#پایان_اگر

#إذا defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#پایان_اگر

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#إذا !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#پایان_اگر

#تعریف_کن BASENAMELEN	(MAXNAMLEN - 5)

#اگر_تعریف_شده HAVE_PWD_H
# include <pwd.h>
#پایان_اگر

#إذا (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_مجموع_MEM
#پایان_اگر


#اگر_تعریف_شده VMS
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
#پایان_اگر // VMS

#اگر_تعریف_شده HAVE_FLOCK
# include <sys/file.h>
#پایان_اگر

#اگر_تعریف_شده VMS
تعريفنوع بنية dsc$descriptor   DESC;
#پایان_اگر

/*
 * Unix system-dependent file names
 */
#اگر_تعریف_نشده SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#پایان_اگر
#اگر_تعریف_نشده SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#پایان_اگر
#اگر_تعریف_نشده DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#پایان_اگر
#اگر_تعریف_نشده SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#پایان_اگر

#اگر_تعریف_نشده USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# وإلا
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#پایان_اگر

#إذا !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#پایان_اگر

#اگر_تعریف_نشده USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# وإلا
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#پایان_اگر


#إذا !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# وإلا
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#پایان_اگر

#اگر_تعریف_نشده XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#پایان_اگر

#إذا !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#پایان_اگر

#اگر_تعریف_نشده USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# وإلا
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#پایان_اگر

#اگر_تعریف_نشده USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# وإلا
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#پایان_اگر

#اگر_تعریف_شده VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#وإلا
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#پایان_اگر

#اگر_تعریف_نشده VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#پایان_اگر

#اگر_تعریف_نشده EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#پایان_اگر

#اگر_تعریف_شده FEAT_VIMINFO
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
#پایان_اگر

#اگر_تعریف_نشده EXRC_FILE
# define EXRC_FILE	".exrc"
#پایان_اگر

#اگر_تعریف_نشده VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#پایان_اگر

#اگر_تعریف_نشده SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#پایان_اگر

#اگر_تعریف_نشده DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# وإلا
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#پایان_اگر

#اگر_تعریف_نشده DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# وإلا
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#پایان_اگر

#اگر_تعریف_نشده DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# وإلا
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#پایان_اگر

#تعریف_کن DFLT_ERRORFILE		"errors.err"

#اگر_تعریف_نشده DFLT_RUNTIMEPATH

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

#پایان_اگر

#اگر_تعریف_شده VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#وإلا
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#پایان_اگر

// Special wildcards that need to be handled by the shell
#تعریف_کن SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#تعریف_کن CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#إذا defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#وإلا
# define MAXPATHL	1024
#پایان_اگر

#تعریف_کن CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#اگر_تعریف_شده VMS  // Use less memory because of older systems
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
#پایان_اگر

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#إذا defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(مقصد, from, len) memmove((حرف *)(مقصد), (حرف *)(from), len)
#وإلا
# ifdef USEBCOPY
#  define mch_memmove(مقصد, from, len) bcopy((حرف *)(from), (حرف *)(مقصد), len)
# وإلا
    // ifdef USEMEMCPY
#  define mch_memmove(مقصد, from, len) کپی_حافظه((حرف *)(مقصد), (حرف *)(from), len)
# endif
#پایان_اگر

#اگر_تعریف_نشده PROTO
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
#پایان_اگر

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#اگر_تعریف_شده HAVE_STRING_H
# include <string.h>
#پایان_اگر
#إذا defined(HAVE_رشتهها_H) && !defined(NO_رشتهها_WITH_STRING_H)
# include <strings.h>
#پایان_اگر

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
#پایان_اگر

#اگر_تعریف_نشده HAVE_DUP
# define HAVE_DUP		// have dup()
#پایان_اگر
#تعریف_کن HAVE_ST_نما		// have stat.st_mode

// We have three kinds of ACL support.
#تعریف_کن HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

