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

#ವ್ಯಾಖ್ಯಾನಿಸು USE_ಪದ_CONSOLE

#ವ್ಯಾಖ್ಯಾನಿಸು USR_VIM_DIR "$BE_USER_SETTINGS/vim"

#ವ್ಯಾಖ್ಯಾನಿಸು USR_EXRC_FILE	USR_VIM_DIR "/exrc"
#ವ್ಯಾಖ್ಯಾನಿಸು USR_EXRC_FILE2	USR_VIM_DIR "/vim/exrc"
#ವ್ಯಾಖ್ಯಾನಿಸು USR_VIMRC_FILE	USR_VIM_DIR "/vimrc"
#ವ್ಯಾಖ್ಯಾನಿಸು USR_VIMRC_FILE2	USR_VIM_DIR "/vim/vimrc"
#ವ್ಯಾಖ್ಯಾನಿಸು USR_GVIMRC_FILE	USR_VIM_DIR "/gvimrc"
#ವ್ಯಾಖ್ಯಾನಿಸು USR_GVIMRC_FILE2	USR_VIM_DIR "/vim/gvimrc"
#ವ್ಯಾಖ್ಯಾನಿಸು VIMINFO_FILE	USR_VIM_DIR "/viminfo"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RUNTIME_GLOBAL
# ifdef RUNTIME_GLOBAL_AFTER
#  define DFLT_RUNTIMEPATH	USR_VIM_DIR "," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER "," USR_VIM_DIR "/after"
#  define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",$XDG_CONFIG_HOME/vim/after"
#  define XDG_RUNTIMEPATH_FB	"~/config/settings/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER ",~/config/settings/vim/after"
#  define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL_AFTER
# ಇಲ್ಲದಿದ್ದರೆ
#  define DFLT_RUNTIMEPATH	USR_VIM_DIR "," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after," USR_VIM_DIR "/after"
#  define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,$XDG_CONFIG_HOME/vim/after"
#  define XDG_RUNTIMEPATH_FB	"~/config/settings/vim," RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after,~/config/settings/vim/after"
#  define CLEAN_RUNTIMEPATH	RUNTIME_GLOBAL ",$VIMRUNTIME," RUNTIME_GLOBAL "/after"
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define DFLT_RUNTIMEPATH	USR_VIM_DIR ",$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after," USR_VIM_DIR "/after"
# define XDG_RUNTIMEPATH	"$XDG_CONFIG_HOME/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,$XDG_CONFIG_HOME/vim/after"
# define XDG_RUNTIMEPATH_FB	"~/config/settings/vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/config/settings/vim/after"
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#ಆದರೆ_ಅಂತ್ಯ
