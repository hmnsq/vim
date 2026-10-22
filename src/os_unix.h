/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <ctype.h>

#నిర్వచితమైతే VAXC
# include <types.h>
# include <stat.h>
#లేకపోతే
# include <sys/types.h>
# include <sys/stat.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_STDLIB_H
# include <stdlib.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __CYGWIN__
# define WIN32UNIX	// Compiling for Win32 using Unix files.
# define ద్విమాన_FILE_IO

# define CASE_INSENSITIVE_FILENAME
# define USE_FNAME_CASE	// Fix filename case differences.
#అయితే_ముగింపు

// On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
// unistd.h.  This hack should fix that (suggested by Jeff George).
// But on AIX 4.3 it's alright (suggested by Jake Hamby).
#ఒకవేళ defined(FEAT_GUI) && defined(_AIX) && !defined(_AIX43) && !defined(_NO_PROTO)
# define _NO_PROTO
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_UNISTD_H
# include <unistd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
# include <sys/param.h>	    // defines BSD, if it's a BSD system
#అయితే_ముగింపు

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#ఒకవేళ defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#అయితే_ముగింపు

// always use unlink() to remove files
#నిర్వచితం_కాకపోతే PROTO
# ifdef VMS
#  define vim_mkdir(x, y) mkdir((అక్షరం *)vms_fixfilename(x), y)
#  define mch_rmdir(x)  delete((అక్షరం *)vms_fixfilename(x))
#  define mch_remove(x) delete((అక్షరం *)vms_fixfilename(x))
# లేకపోతే
#  define vim_mkdir(x, y) mkdir((అక్షరం *)(x), y)
#  define mch_rmdir(x) rmdir((అక్షరం *)(x))
#  define mch_remove(x) unlink((అక్షరం *)(x))
# endif
#అయితే_ముగింపు

// The number of arguments to a signal handler is configured here.
// It used to be a long list of almost all systems. Any system that doesn't
// have an argument???
#నిర్వచించు SIGHASARG

#నిర్వచితమైతే SIGHASARG
# define SIGPROTOARG	(పూర్ణాంకం)
# define SIGDEFARG(s)	(పూర్ణాంకం s వాడని)
# define SIGDUMMYARG	0
#లేకపోతే
# define SIGPROTOARG   (శూన్య)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#అయితే_ముగింపు

రకనిర్వచనం శూన్య (*sighandler_T) SIGPROTOARG;

#నిర్వచితమైతే HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) అక్షరశ్రేణి_పొడవు((dirent)->d_name)
# endif
#లేకపోతే
# define dirent ప్రత్యక్ష
# define NAMLEN(dirent) (dirent)->d_namlen
# ఒకవేళ HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ఒకవేళ HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ఒకవేళ HAVE_NDIR_H
#  include <ndir.h>
# endif
#అయితే_ముగింపు

#అన్తర్భావ <time.h>
#నిర్వచితమైతే HAVE_SYS_సమయం_H
# include <sys/time.h>
#అయితే_ముగింపు

#అన్తర్భావ <signal.h>

#ఒకవేళ defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#అయితే_ముగింపు

#ఒకవేళ defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    // for dynix/ptx
#అయితే_ముగింపు

#ఒకవేళ defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    // for Linux before .99p3
#అయితే_ముగింపు

#ఒకవేళ defined(__sun)
# undef MAXNAMLEN
# define MAXNAMLEN 255
#అయితే_ముగింపు

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#ఒకవేళ !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    // for all other Unix
#అయితే_ముగింపు

#నిర్వచించు BASENAMELEN	(MAXNAMLEN - 5)

#నిర్వచితమైతే HAVE_PWD_H
# include <pwd.h>
#అయితే_ముగింపు

#ఒకవేళ (defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)) \
	|| (defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)) \
	|| defined(HAVE_SYSCTL) || defined(HAVE_SYSCONF)
# define HAVE_మొత్తం_MEM
#అయితే_ముగింపు


#నిర్వచితమైతే VMS
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

# ఒకవేళ defined(FEAT_GUI_MOTIF)
#  define XFree XFREE
#  define XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
# endif
#అయితే_ముగింపు // VMS

#నిర్వచితమైతే HAVE_FLOCK
# include <sys/file.h>
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
రకనిర్వచనం నిర్మాణం dsc$descriptor   DESC;
#అయితే_ముగింపు

/*
 * Unix system-dependent file names
 */
#నిర్వచితం_కాకపోతే SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_EXRC_FILE
# ifdef VMS
#  define USR_EXRC_FILE "sys$login:.exrc"
# లేకపోతే
#  define USR_EXRC_FILE "$HOME/.exrc"
# endif
#అయితే_ముగింపు

#ఒకవేళ !defined(USR_EXRC_FILE2) && defined(VMS)
# define USR_EXRC_FILE2 "sys$login:_exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_VIMRC_FILE
# ifdef VMS
#  define USR_VIMRC_FILE  "sys$login:.vimrc"
# లేకపోతే
#  define USR_VIMRC_FILE "$HOME/.vimrc"
# endif
#అయితే_ముగింపు


#ఒకవేళ !defined(USR_VIMRC_FILE2)
# ifdef VMS
#  define USR_VIMRC_FILE2	"sys$login:vimfiles/vimrc"
# లేకపోతే
#  define USR_VIMRC_FILE2	"~/.vim/vimrc"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే XDG_VIMRC_FILE
# define XDG_VIMRC_FILE (mch_getenv((char_u *)"XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/vimrc" \
	: "~/.config/vim/vimrc")
#అయితే_ముగింపు

#ఒకవేళ !defined(USR_VIMRC_FILE3) && defined(VMS)
# define USR_VIMRC_FILE3 "sys$login:_vimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE
# ifdef VMS
#  define USR_GVIMRC_FILE "sys$login:.gvimrc"
# లేకపోతే
#  define USR_GVIMRC_FILE "$HOME/.gvimrc"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE2
# ifdef VMS
#  define USR_GVIMRC_FILE2	"sys$login:vimfiles/gvimrc"
# లేకపోతే
#  define USR_GVIMRC_FILE2	"~/.vim/gvimrc"
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3  "sys$login:_gvimrc"
# endif
#లేకపోతే
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3 (mch_getenv("XDG_CONFIG_HOME") \
	? "$XDG_CONFIG_HOME/vim/gvimrc" \
	: "~/.config/vim/gvimrc")
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  ifdef VMS
#   define VIMINFO_FILE  "sys$login:.viminfo"
#  లేకపోతే
#   define VIMINFO_FILE "$HOME/.viminfo"
#  endif
# endif
# ఒకవేళ !defined(VIMINFO_FILE2) && defined(VMS)
#  define VIMINFO_FILE2 "sys$login:_viminfo"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే EXRC_FILE
# define EXRC_FILE	".exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_BDIR
# ifdef VMS
#  define DFLT_BDIR    "./,sys$login:,tmp:"
# లేకపోతే
#  define DFLT_BDIR    ".,~/tmp,~/"    // default for 'backupdir'
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_DIR
# ifdef VMS
#  define DFLT_DIR     "./,sys$login:,tmp:"
# లేకపోతే
#  define DFLT_DIR     ".,~/tmp,/var/tmp,/tmp" // default for 'directory'
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_VDIR
# ifdef VMS
#  define DFLT_VDIR    "sys$login:vimfiles/view"
# లేకపోతే
#  define DFLT_VDIR    "$HOME/.vim/view"       // default for 'viewdir'
#  define XDG_VDIR     (mch_getenv("XDG_CONFIG_HOME") ? \
	"$XDG_CONFIG_HOME/vim/view" : "~/.config/vim/view")
# endif
#అయితే_ముగింపు

#నిర్వచించు DFLT_ERRORFILE		"errors.err"

#నిర్వచితం_కాకపోతే DFLT_RUNTIMEPATH

# ifdef VMS
#  define DFLT_RUNTIMEPATH      "sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,sys$login:vimfiles/after"
#  define CLEAN_RUNTIMEPATH      "$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
# లేకపోతే
#  ifdef RUNTIME_GLOBAL
#   ifdef RUNTIME_GLOBAL_AFTER
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
#   లేకపోతే
#    define DFLT_RUNTIMEPATH	"~/.vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.vim/after"
#    define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#    define XDG_RUNTIMEPATH_FB	"~/.config/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/.config/vim/after"
#    define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
#   endif
#  లేకపోతే
#   define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#   define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
#   define XDG_RUNTIMEPATH_FB	"~/.config/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.config/vim/after"
#   define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#  endif
# endif

#అయితే_ముగింపు

#నిర్వచితమైతే VMS
# ifndef VAX
#  define VMS_TEMPNAM    // to fix default .LIS extension
# endif
# define TEMPNAME       "TMP:v?XXXXXX.txt"
# define TEMPNAMELEN    28
#లేకపోతే
// Try several directories to put the temp files.
# define TEMPDIRNAMES  "$TMPDIR", "/tmp", ".", "$HOME"
# define TEMPNAMELEN    256
#అయితే_ముగింపు

// Special wildcards that need to be handled by the shell
#నిర్వచించు SPECIAL_WILDCHAR    "`'{"

/*
 * Unix has plenty of memory, use large buffers
 */
#నిర్వచించు CMDBUFFSIZE 1024	// size of the command processing buffer

// Use the system path length if it makes sense.
#ఒకవేళ defined(PATH_MAX) && (PATH_MAX > 1000)
# define MAXPATHL	PATH_MAX
#లేకపోతే
# define MAXPATHL	1024
#అయితే_ముగింపు

#నిర్వచించు CHECK_INODE		// used when checking if a swap file already
				// exists for a file
#నిర్వచితమైతే VMS  // Use less memory because of older systems
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM (2*1024)
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT (5*1024)
# endif
#లేకపోతే
# ifndef DFLT_MAXMEM
#  define DFLT_MAXMEM	(5*1024)	 // use up to 5 Mbyte for a buffer
# endif
# ifndef DFLT_MAXMEMTOT
#  define DFLT_MAXMEMTOT	(10*1024)    // use up to 10 Mbyte for Vim
# endif
#అయితే_ముగింపు

// memmove() is not present on all systems, use memmove, bcopy or memcpy.
// Some systems have (void *) arguments, some (char *). If we use (char *) it
// works for all
#ఒకవేళ defined(USEMEMMOVE) || (!defined(USEBCOPY) && !defined(USEMEMCPY))
# define mch_memmove(గమ్యం, from, len) memmove((అక్షరం *)(గమ్యం), (అక్షరం *)(from), len)
#లేకపోతే
# ifdef USEBCOPY
#  define mch_memmove(గమ్యం, from, len) bcopy((అక్షరం *)(from), (అక్షరం *)(గమ్యం), len)
# లేకపోతే
    // ifdef USEMEMCPY
#  define mch_memmove(గమ్యం, from, len) జ్ఞాపకం_నకలు((అక్షరం *)(గమ్యం), (అక్షరం *)(from), len)
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PROTO
# ifdef HAVE_RENAME
#  define mch_rename(src, dst) rename(src, dst)
# లేకపోతే
పూర్ణాంకం mch_rename(స్థిరరాశి అక్షరం *src, స్థిరరాశి అక్షరం *dest);
# endif
# ifndef VMS
#  ifdef __MVS__
  // on OS390 Unix getenv() doesn't return a pointer to persistent
  // storage -> use __getenv()
#   define mch_getenv(x) (char_u *)__getenv((అక్షరం *)(x))
#  లేకపోతే
#   define mch_getenv(x) (char_u *)getenv((అక్షరం *)(x))
#  endif
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#అయితే_ముగింపు

// Note: Some systems need both string.h and strings.h (Savage).  However,
// some systems can't handle both, only use string.h in that case.
#నిర్వచితమైతే HAVE_STRING_H
# include <string.h>
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_అక్షరశ్రేణులు_H) && !defined(NO_అక్షరశ్రేణులు_WITH_STRING_H)
# include <strings.h>
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SETJMP_H)
# include <setjmp.h>
# ifdef HAVE_SIGSETJMP
#  define JMP_BUF sigjmp_buf
#  define SETJMP(x) sigsetjmp((x), 1)
#  define LONGJMP siglongjmp
# లేకపోతే
#  define JMP_BUF jmp_buf
#  define SETJMP(x) setjmp(x)
#  define LONGJMP longjmp
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_DUP
# define HAVE_DUP		// have dup()
#అయితే_ముగింపు
#నిర్వచించు HAVE_ST_బహుళకం		// have stat.st_mode

// We have three kinds of ACL support.
#నిర్వచించు HAVE_ACL (HAVE_POSIX_ACL || HAVE_SOLARIS_ACL || HAVE_AIX_ACL)

