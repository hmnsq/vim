/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *	  Haiku port	by Siarzhuk Zharski
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_haiku.h
 */

#وصف_ڪريو USE_رڪن_CONSOLE

#وصف_ڪريو USR_VIM_DIR "$BE_USER_SETTINGS/vim"

#وصف_ڪريو USR_EXRC_FILE	USR_VIM_DIR "/exrc"
#وصف_ڪريو USR_EXRC_FILE2	USR_VIM_DIR "/vim/exrc"
#وصف_ڪريو USR_VIMRC_FILE	USR_VIM_DIR "/vimrc"
#وصف_ڪريو USR_VIMRC_FILE2	USR_VIM_DIR "/vim/vimrc"
#وصف_ڪريو USR_GVIMRC_FILE	USR_VIM_DIR "/gvimrc"
#وصف_ڪريو USR_GVIMRC_FILE2	USR_VIM_DIR "/vim/gvimrc"
#وصف_ڪريو VIMINFO_FILE	USR_VIM_DIR "/viminfo"

#جيڪڏهن_وصف_ٿيل RUNTIME_GLOBAL
# ifdef RUNTIME_GLOBAL_AFTER
#  define DFLT_RUNTIMEPATH	USR_VIM_DIR "," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER "," USR_VIM_DIR "/after"
#  define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#  define XDG_RUNTIMEPATH_FB	"~/config/settings/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/config/settings/vim/after"
#  define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
# وإلا
#  define DFLT_RUNTIMEPATH	USR_VIM_DIR "," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after," USR_VIM_DIR "/after"
#  define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#  define XDG_RUNTIMEPATH_FB	"~/config/settings/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/config/settings/vim/after"
#  define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
# endif
#وإلا
# define DFLT_RUNTIMEPATH	USR_VIM_DIR ",$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after," USR_VIM_DIR "/after"
# define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
# define XDG_RUNTIMEPATH_FB	"~/config/settings/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/config/settings/vim/after"
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#جيڪڏهن_پورو
