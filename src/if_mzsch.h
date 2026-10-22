/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#که_نه_تعریف_شوی _IF_MZSCH_H_
#تعریف_کړه _IF_MZSCH_H_
#که_تعریف_شوی __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#د_که_پای

// #ifdef needed for "make depend"
#که_تعریف_شوی FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#د_که_پای

#که_تعریف_شوی __MINGW32__
# undef __CYGWIN32__
#د_که_پای

#إذا MZSCHEME_نسخه_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_ارزښت(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#وإلا
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_پورټ scheme_make_string_output_پورټ
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_ارزښت(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_منزل_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#د_که_پای

// Precise GC macros
#که_نه_تعریف_شوی MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(اندازه)		 //تشه
#د_که_پای
#که_نه_تعریف_شوی MZ_GC_VAR_دننه_REG
# define MZ_GC_VAR_دننه_REG(x, v)		 //تشه
#د_که_پای
#که_نه_تعریف_شوی MZ_GC_ARRAY_VAR_دننه_REG
# define MZ_GC_ARRAY_VAR_دننه_REG(x, v, l) //تشه
#د_که_پای
#که_نه_تعریف_شوی MZ_GC_REG
# define MZ_GC_REG()			 //تشه
#د_که_پای
#که_نه_تعریف_شوی MZ_GC_UNREG
# define MZ_GC_UNREG()			 //تشه
#د_که_پای

#که_تعریف_شوی MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#وإلا
# define MZ_GC_CHECK()			//تشه
#د_که_پای

#د_که_پای // _IF_MZSCH_H_
