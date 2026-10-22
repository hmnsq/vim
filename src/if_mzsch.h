/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _IF_MZSCH_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _IF_MZSCH_H_
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#ਜੇ_ਅੰਤ

// #ifdef needed for "make depend"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# undef __CYGWIN32__
#ਜੇ_ਅੰਤ

#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_ਮੁੱਲ(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#ਨਹੀਂਤਾਂ
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_ਪੋਰਟ scheme_make_string_output_ਪੋਰਟ
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_ਮੁੱਲ(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_ਮੰਜ਼ਿਲ_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#ਜੇ_ਅੰਤ

// Precise GC macros
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(ਮਾਪ)		 //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MZ_GC_VAR_ਵਿੱਚ_REG
# define MZ_GC_VAR_ਵਿੱਚ_REG(x, v)		 //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG
# define MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG(x, v, l) //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MZ_GC_REG
# define MZ_GC_REG()			 //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MZ_GC_UNREG
# define MZ_GC_UNREG()			 //ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#ਨਹੀਂਤਾਂ
# define MZ_GC_CHECK()			//ਖਾਲੀ_ਥਾਵਾਂ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // _IF_MZSCH_H_
