/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#اگر_غیر_تعریف_شدہ _IF_MZSCH_H_
#تعریف_کرو _IF_MZSCH_H_
#اگر_تعریف_شدہ __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#اگر_ختم

// #ifdef needed for "make depend"
#اگر_تعریف_شدہ FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#اگر_ختم

#اگر_تعریف_شدہ __MINGW32__
# undef __CYGWIN32__
#اگر_ختم

#اگر MZSCHEME_نسخہ_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_قدر(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#ورنہ
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_پورٹ scheme_make_string_output_پورٹ
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_قدر(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_مقصد_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#اگر_ختم

// Precise GC macros
#اگر_غیر_تعریف_شدہ MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(سائز)		 //خالی_خانے
#اگر_ختم
#اگر_غیر_تعریف_شدہ MZ_GC_VAR_اندر_REG
# define MZ_GC_VAR_اندر_REG(x, v)		 //خالی_خانے
#اگر_ختم
#اگر_غیر_تعریف_شدہ MZ_GC_ARRAY_VAR_اندر_REG
# define MZ_GC_ARRAY_VAR_اندر_REG(x, v, l) //خالی_خانے
#اگر_ختم
#اگر_غیر_تعریف_شدہ MZ_GC_REG
# define MZ_GC_REG()			 //خالی_خانے
#اگر_ختم
#اگر_غیر_تعریف_شدہ MZ_GC_UNREG
# define MZ_GC_UNREG()			 //خالی_خانے
#اگر_ختم

#اگر_تعریف_شدہ MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#ورنہ
# define MZ_GC_CHECK()			//خالی_خانے
#اگر_ختم

#اگر_ختم // _IF_MZSCH_H_
