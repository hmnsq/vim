/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ضمن <stdio.h>
#ضمن <ctype.h>

#جے_تعریف_شدہ VAXC
# include <types.h>
# include <stat.h>
#وإلا
# include <sys/types.h>
# include <sys/stat.h>
#جے_انت

#جے_تعریف_شدہ HAVE_STDLIB_H
# include <stdlib.h>
#جے_انت

#جے_تعریف_شدہ __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define ثنائی_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#جے_انت

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#إذا defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#جے_انت

#جے_تعریف_شدہ HAVE_UNISTD_H
# include <unistd.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#جے_انت

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#إذا defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#جے_انت

// always use unlink() to remove files
#جے_بنا_تعریف PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((حرف *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((حرف *)vms_fixfilename(x))
#  define mch_remove(x) delete((حرف *)vms_fixfilename(x))
# وإلا
#  define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#  define mch_rmdir(x) rmdir((حرف *)(x))
#  define mch_remove(x) unlink((حرف *)(x))
# endif
#جے_انت

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#تعریف_کرو SIGHASARG

#جے_تعریف_شدہ SIGHASARG
# define SIGPROTOARG	(صحيح)
# define SIGDEFARG(s)	(صحيح s بن_ورتیا)
# define SIGDUMMYARG	0
#وإلا
# define SIGPROTOARG   (فراغ)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#جے_انت

تعريفنوع فراغ (*sighandler_T) SIGPROTOARG;

#جے_تعریف_شدہ HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) لڑی_لمبائی((dirent)->d_name)
# endif
#وإلا
# define dirent سدھا
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
#جے_انت

#ضمن <time.h>
#جے_تعریف_شدہ HAVE_SYS_ویلا_H
# include <sys/time.h>
#جے_انت

#ضمن <signal.h>

#إذا defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#جے_انت

#إذا defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#جے_انت

#إذا defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#جے_انت

#إذا defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#جے_انت

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#إذا !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#جے_انت

#تعریف_کرو BASENAMELEN	(MAXNAMLEN - 5)

#جے_تعریف_شدہ HAVE_PWD_H
# include <pwd.h>
#جے_انت

#إذا (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_کل_MEM
#جے_انت


#جے_تعریف_شدہ VMS
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
#جے_انت // VMS

#جے_تعریف_شدہ HAVE_FLOCK
# include <sys/file.h>
#جے_انت

#جے_تعریف_شدہ VMS
تعريفنوع بنية dsc$descriptor   DESC;
#جے_انت

/*
 * Unix system-dependent file names
 */
#جے_بنا_تعریف SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#جے_انت
#جے_بنا_تعریف SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#جے_انت
#جے_بنا_تعریف DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#جے_انت
#جے_بنا_تعریف SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#جے_انت

#جے_بنا_تعریف USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# وإلا
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#جے_انت

#إذا !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#جے_انت

#جے_بنا_تعریف USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# وإلا
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#جے_انت


#إذا !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# وإلا
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#جے_انت

#جے_بنا_تعریف XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#جے_انت

#إذا !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#جے_انت

#جے_بنا_تعریف USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# وإلا
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#جے_انت

#جے_بنا_تعریف USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# وإلا
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#جے_انت

#جے_تعریف_شدہ VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#وإلا
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#جے_انت

#جے_بنا_تعریف VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#جے_انت

#جے_بنا_تعریف EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#جے_انت

#جے_تعریف_شدہ FEAT_VIMINFO
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
#جے_انت

#جے_بنا_تعریف EXRC_FILE
# define EXRC_FILE	".exrc"
#جے_انت

#جے_بنا_تعریف VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#جے_انت

#جے_بنا_تعریف SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#جے_انت

#جے_بنا_تعریف DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# وإلا
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#جے_انت

#جے_بنا_تعریف DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# وإلا
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#جے_انت

#جے_بنا_تعریف DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# وإلا
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#جے_انت

#تعریف_کرو DFLT_ERRORFILE		"errors.err"

#جے_بنا_تعریف DFLT_RUNTIMEPATH

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

#جے_انت

#جے_تعریف_شدہ VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#وإلا
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#جے_انت

// Special wildcards that need to be handled by the shell
#تعریف_کرو SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#تعریف_کرو CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#إذا defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#وإلا
# define MAXPATHL	1024
#جے_انت

#تعریف_کرو CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#جے_تعریف_شدہ VMS  // Use less memory because of older systems
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
#جے_انت

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
#  define mch_memmove(منزل, from, len) حافظہ_نقل((حرف *)(منزل), (حرف *)(from), len)
# endif
#جے_انت

#جے_بنا_تعریف PROTO
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
#جے_انت

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#جے_تعریف_شدہ HAVE_STRING_H
# include <string.h>
#جے_انت
#إذا defined(HAVE_لڑیاں_H) && !defined(NO_لڑیاں_WITH_STRING_H)
# include <strings.h>
#جے_انت

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
#جے_انت

#جے_بنا_تعریف HAVE_DUP
# define HAVE_DUP		// have dup()
#جے_انت
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode

// We have three kinds of ACL support.
#تعریف_کرو HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

