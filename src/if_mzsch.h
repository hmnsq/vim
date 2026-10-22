/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _IF_MZSCH_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _IF_MZSCH_H_
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#ಆದರೆ_ಅಂತ್ಯ

// #ifdef needed for "make depend"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# undef __CYGWIN32__
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_ಮೌಲ್ಯ(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#ಇಲ್ಲದಿದ್ದರೆ
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_ಪೋರ್ಟ್ scheme_make_string_output_ಪೋರ್ಟ್
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_ಮೌಲ್ಯ(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_ಗಮ್ಯ_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#ಆದರೆ_ಅಂತ್ಯ

// Precise GC macros
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(ಗಾತ್ರ_2)		 //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MZ_GC_VAR_ಒಳ_REG
# define MZ_GC_VAR_ಒಳ_REG(x, v)		 //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MZ_GC_ARRAY_VAR_ಒಳ_REG
# define MZ_GC_ARRAY_VAR_ಒಳ_REG(x, v, l) //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MZ_GC_REG
# define MZ_GC_REG()			 //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MZ_GC_UNREG
# define MZ_GC_UNREG()			 //ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#ಇಲ್ಲದಿದ್ದರೆ
# define MZ_GC_CHECK()			//ಖಾಲಿಗಳು
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // _IF_MZSCH_H_
