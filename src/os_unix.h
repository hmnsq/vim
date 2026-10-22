/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#شامل <stdio.h>
#شامل <ctype.h>

#اگر_تعریف_شدہ VAXC
# include <types.h>
# include <stat.h>
#ورنہ
# include <sys/types.h>
# include <sys/stat.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_STDLIB_H
# include <stdlib.h>
#اگر_ختم

#اگر_تعریف_شدہ __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define ثنائی_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#اگر_ختم

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#اگر defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#اگر_ختم

#اگر_تعریف_شدہ HAVE_UNISTD_H
# include <unistd.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#اگر_ختم

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#اگر defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#اگر_ختم

// always use unlink() to remove files
#اگر_غیر_تعریف_شدہ PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((حرف *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((حرف *)vms_fixfilename(x))
#  define mch_remove(x) delete((حرف *)vms_fixfilename(x))
# ورنہ
#  define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#  define mch_rmdir(x) rmdir((حرف *)(x))
#  define mch_remove(x) unlink((حرف *)(x))
# endif
#اگر_ختم

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#تعریف_کرو SIGHASARG

#اگر_تعریف_شدہ SIGHASARG
# define SIGPROTOARG	(صحیح)
# define SIGDEFARG(s)	(صحیح s غیر_مستعمل)
# define SIGDUMMYARG	0
#ورنہ
# define SIGPROTOARG   (خالی)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#اگر_ختم

قسمتعریف خالی (*sighandler_T) SIGPROTOARG;

#اگر_تعریف_شدہ HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) لڑی_لمبائی((dirent)->d_name)
# endif
#ورنہ
# define dirent براہ_راست
# define NAMLEN(dirent) (dirent)->d_namlen
# اگر HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# اگر HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# اگر HAVE_NDIR_H
#  include <ndir.h>
# endif
#اگر_ختم

#شامل <time.h>
#اگر_تعریف_شدہ HAVE_SYS_وقت_H
# include <sys/time.h>
#اگر_ختم

#شامل <signal.h>

#اگر defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#اگر_ختم

#اگر defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#اگر_ختم

#اگر defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#اگر_ختم

#اگر defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#اگر_ختم

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#اگر !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#اگر_ختم

#تعریف_کرو BASENAMELEN	(MAXNAMLEN - 5)

#اگر_تعریف_شدہ HAVE_PWD_H
# include <pwd.h>
#اگر_ختم

#اگر (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_کل_MEM
#اگر_ختم


#اگر_تعریف_شدہ VMS
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

# اگر defined(FEAT_GUI_MOTIF)
#  define XFree XFREE
#  define XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
# endif
#اگر_ختم // VMS

#اگر_تعریف_شدہ HAVE_FLOCK
# include <sys/file.h>
#اگر_ختم

#اگر_تعریف_شدہ VMS
قسمتعریف ساخت dsc$descriptor   DESC;
#اگر_ختم

/*
 * Unix system-dependent file names
 */
#اگر_غیر_تعریف_شدہ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#اگر_ختم
#اگر_غیر_تعریف_شدہ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# ورنہ
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#اگر_ختم

#اگر !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# ورنہ
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#اگر_ختم


#اگر !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# ورنہ
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#اگر_ختم

#اگر !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# ورنہ
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# ورنہ
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#اگر_ختم

#اگر_تعریف_شدہ VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#ورنہ
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#اگر_ختم

#اگر_غیر_تعریف_شدہ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  ifdef VMS
#   define VIMINFO_FILE  "sys$login:.viminfo"
#  ورنہ
#   define VIMINFO_FILE "$HOME/.viminfo"
#  endif
# endif
# اگر !defined(VIMINFO_FILE2) && defined(VMS)
#  define VIMINFO_FILE2 "sys$login:_viminfo"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ EXRC_FILE
# define EXRC_FILE	".exrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# ورنہ
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# ورنہ
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# ورنہ
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#اگر_ختم

#تعریف_کرو DFLT_ERRORFILE		"errors.err"

#اگر_غیر_تعریف_شدہ DFLT_RUNTIMEPATH

# ifdef VMS
#  define DFLT_RUNTIMEPATH      "sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,sys$login:vimfiles/after"
#  define CLEAN_RUNTIMEPATH      "$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
# ورنہ
#  ifdef RUNTIME_GLOBAL
#   ifdef RUNTIME_GLOBAL_AFTER
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
#   ورنہ
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
#   endif
#  ورنہ
#   define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#   define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
#   define XDG_RUNTIMEPATH_FB	"~/.config/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.config/vim/after"
#   define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#  endif
# endif

#اگر_ختم

#اگر_تعریف_شدہ VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#ورنہ
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#اگر_ختم

// Special wildcards that need to be handled by the shell
#تعریف_کرو SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#تعریف_کرو CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#اگر defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#ورنہ
# define MAXPATHL	1024
#اگر_ختم

#تعریف_کرو CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#اگر_تعریف_شدہ VMS  // Use less memory because of older systems
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM (2*1024)
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT (5*1024)
# endif
#ورنہ
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM	(5*1024)	 // use up to 5 Mbyte for a buffer
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT	(10*1024)    // use up to 10 Mbyte for Vim
# endif
#اگر_ختم

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#اگر defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(مقصد, from, len) memmove((حرف *)(مقصد), (حرف *)(from), len)
#ورنہ
# ifdef USEBCOPY
#  define mch_memmove(مقصد, from, len) bcopy((حرف *)(from), (حرف *)(مقصد), len)
# ورنہ
    // ifdef USEMEMCPY
#  define mch_memmove(مقصد, from, len) حافظہ_نقل((حرف *)(مقصد), (حرف *)(from), len)
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ PROTO
# ifdef HAVE_RENAME
#  define mch_rename(src, dst) rename(src, dst)
# ورنہ
صحیح mch_rename(ثابت حرف *src, ثابت حرف *dest);
# endif
# ifndef VMS
#  ifdef __MVS__
  // on OS390 Unix getenv() doesn't return a pointer to persistent
  // storage -> use __getenv()
#   define mch_getenv(x) (char_u *)__getenv((حرف *)(x))
#  ورنہ
#   define mch_getenv(x) (char_u *)getenv((حرف *)(x))
#  endif
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#اگر_ختم

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#اگر_تعریف_شدہ HAVE_STRING_H
# include <string.h>
#اگر_ختم
#اگر defined(HAVE_لڑیاں_H) && !defined(NO_لڑیاں_WITH_STRING_H)
# include <strings.h>
#اگر_ختم

#اگر defined(HAVE_SETJMP_H)
# include <setjmp.h>
# ifdef HAVE_SIGSETJMP
#  define JMP_BUF sigjmp_buf
#  define SETJMP(x) sigsetjmp((x), 1)
#  define LONGJMP siglongjmp
# ورنہ
#  define JMP_BUF jmp_buf
#  define SETJMP(x) setjmp(x)
#  define LONGJMP longjmp
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_DUP
# define HAVE_DUP		// have dup()
#اگر_ختم
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode

// We have three kinds of ACL support.
#تعریف_کرو HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

