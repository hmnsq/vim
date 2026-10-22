/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#اگر_تعریف_نشده _IF_MZSCH_H_
#تعریف_کن _IF_MZSCH_H_
#اگر_تعریف_شده __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#پایان_اگر

// #ifdef needed for "make depend"
#اگر_تعریف_شده FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#پایان_اگر

#اگر_تعریف_شده __MINGW32__
# undef __CYGWIN32__
#پایان_اگر

#إذا MZSCHEME_نسخه_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_مقدار(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#وإلا
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_پورت scheme_make_string_output_پورت
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_مقدار(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_مقصد_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#پایان_اگر

// Precise GC macros
#اگر_تعریف_نشده MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(اندازه)		 //خالی
#پایان_اگر
#اگر_تعریف_نشده MZ_GC_VAR_درون_REG
# define MZ_GC_VAR_درون_REG(x, v)		 //خالی
#پایان_اگر
#اگر_تعریف_نشده MZ_GC_ARRAY_VAR_درون_REG
# define MZ_GC_ARRAY_VAR_درون_REG(x, v, l) //خالی
#پایان_اگر
#اگر_تعریف_نشده MZ_GC_REG
# define MZ_GC_REG()			 //خالی
#پایان_اگر
#اگر_تعریف_نشده MZ_GC_UNREG
# define MZ_GC_UNREG()			 //خالی
#پایان_اگر

#اگر_تعریف_شده MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#وإلا
# define MZ_GC_CHECK()			//خالی
#پایان_اگر

#پایان_اگر // _IF_MZSCH_H_
