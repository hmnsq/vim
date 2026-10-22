/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#جيڪڏهن_وصف_نه_ٿيل _IF_MZSCH_H_
#وصف_ڪريو _IF_MZSCH_H_
#جيڪڏهن_وصف_ٿيل __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#جيڪڏهن_پورو

// #ifdef needed for "make depend"
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __MINGW32__
# undef __CYGWIN32__
#جيڪڏهن_پورو

#إذا MZSCHEME_نسخو_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_قدر(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#وإلا
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_پورٽ scheme_make_string_output_پورٽ
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_قدر(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_منزل_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#جيڪڏهن_پورو

// Precise GC macros
#جيڪڏهن_وصف_نه_ٿيل MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(سائيز)		 //خالي_جايون
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل MZ_GC_VAR_اندريون_REG
# define MZ_GC_VAR_اندريون_REG(x, v)		 //خالي_جايون
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل MZ_GC_ARRAY_VAR_اندريون_REG
# define MZ_GC_ARRAY_VAR_اندريون_REG(x, v, l) //خالي_جايون
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل MZ_GC_REG
# define MZ_GC_REG()			 //خالي_جايون
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل MZ_GC_UNREG
# define MZ_GC_UNREG()			 //خالي_جايون
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#وإلا
# define MZ_GC_CHECK()			//خالي_جايون
#جيڪڏهن_پورو

#جيڪڏهن_پورو // _IF_MZSCH_H_
