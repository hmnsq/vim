/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface for Vim, wrapper around scheme.h
 */
#నిర్వచితం_కాకపోతే _IF_MZSCH_H_
#నిర్వచించు _IF_MZSCH_H_
#నిర్వచితమైతే __MINGW32__
// Hack to engage Cygwin-specific settings
# define __CYGWIN32__
# include <stdint.h>
#అయితే_ముగింపు

// #ifdef needed for "make depend"
#నిర్వచితమైతే FEAT_MZSCHEME
# include <schvers.h>
# include <scheme.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __MINGW32__
# undef __CYGWIN32__
#అయితే_ముగింపు

#ఒకవేళ MZSCHEME_సంస్కరణ_MAJOR >= 299
# define SCHEME_STRINGP(obj) (SCHEME_BYTE_STRINGP(obj) || SCHEME_CHAR_STRINGP(obj))
# define BYTE_STRING_విలువ(obj) ((char_u *)SCHEME_BYTE_STR_VAL(obj))
#లేకపోతే
// macros for compatibility with older versions
# define scheme_current_config() scheme_config
# define scheme_make_sized_byte_string scheme_make_sized_string
# define scheme_format_utf8 scheme_format
# ifndef DYNAMIC_MZSCHEME
// for dynamic MzScheme there will be separate definitions in if_mzsch.c
#  define scheme_get_sized_byte_string_output scheme_get_sized_string_output
#  define scheme_make_byte_string scheme_make_string
#  define scheme_make_byte_string_output_పోర్టు scheme_make_string_output_పోర్టు
# endif

# define SCHEME_BYTE_STRLEN_VAL SCHEME_STRLEN_VAL
# define BYTE_STRING_విలువ(obj) ((char_u *)SCHEME_STR_VAL(obj))
# define scheme_byte_string_గమ్యం_char_string(obj) (obj)
# define SCHEME_BYTE_STRINGP SCHEME_STRINGP
#అయితే_ముగింపు

// Precise GC macros
#నిర్వచితం_కాకపోతే MZ_GC_DECL_REG
# define MZ_GC_DECL_REG(కొలత)		 //ఖాళీలు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MZ_GC_VAR_లోపల_REG
# define MZ_GC_VAR_లోపల_REG(x, v)		 //ఖాళీలు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MZ_GC_ARRAY_VAR_లోపల_REG
# define MZ_GC_ARRAY_VAR_లోపల_REG(x, v, l) //ఖాళీలు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MZ_GC_REG
# define MZ_GC_REG()			 //ఖాళీలు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MZ_GC_UNREG
# define MZ_GC_UNREG()			 //ఖాళీలు
#అయితే_ముగింపు

#నిర్వచితమైతే MZSCHEME_FORCE_GC
/*
 * force garbage collection to check all references are registered
 * seg faults will indicate not registered refs
 */
# define MZ_GC_CHECK() scheme_collect_garbage();
#లేకపోతే
# define MZ_GC_CHECK()			//ఖాళీలు
#అయితే_ముగింపు

#అయితే_ముగింపు // _IF_MZSCH_H_
