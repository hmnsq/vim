/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Ruby interface by Shugo Maeda
 *   with improvements by SegPhault (Ryan Paul)
 *   with improvements by Jon Maken
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "protodef.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CONFIG_H
# include "auto/config.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RUBYEXTERN
# define RUBYEXTERN ಬಾಹ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ಆದರೆ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_ಚಿಹ್ನೆ		(*dll_rb_c_ಚಿಹ್ನೆ)
# define rb_cTrueClass		(*dll_rb_cTrueClass)

/*
 * All Ruby functions are exported with "__declspec(dllimport)" in ruby.h.
 * But it causes trouble for these variables, because it is defined in this
 * file.  When defined this RUBY_EXPORT it modified to "extern" and be able
 * to avoid this problem.
 */
# define RUBY_EXPORT

// Ruby 1.9 defines a number of static functions which use rb_num2long and
// rb_int2big
# define rb_num2long	rb_num2long_stub
# define rb_int2big	rb_int2big_stub

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26 && RUBY_ಆವೃತ್ತಿ <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ	ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ_stub
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#ಆದರೆ_ಅಂತ್ಯ  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIZEOF_ಸಮಯ_T
# undef SIZEOF_ಸಮಯ_T
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ತಳ್ಳು
# pragma GCC diagnostic ignored "-Wunused-parameter"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <ruby.h>
#ಅನ್ತರ್ಭಾವ <ruby/encoding.h>

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

// See above.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIZEOF_ಸಮಯ_T
# undef SIZEOF_ಸಮಯ_T
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು off_t	// ruby defines off_t as _int64, Mingw uses long
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು EXTERN
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ಆದರೆ defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#ಆದರೆ defined(Typed_ದತ್ತಾಂಶ_Wrap_Struct) && (RUBY_ಆವೃತ್ತಿ >= 20)
# define USE_TYPEDDATA	1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ String_ಮೌಲ್ಯ_Ptr
# define String_ಮೌಲ್ಯ_Ptr(s) STR2CSTR(s)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DUP
# undef HAVE_DUP
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
# undef HAVE_FSYNC
#ಆದರೆ_ಅಂತ್ಯ

// Avoid redefining TRUE/FALSE in vterm.h.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRUE
# undef TRUE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FALSE
# undef FALSE
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define ತುಂಬು_dll vim_ತುಂಬು_Lib
#  define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#  define close_dll FreeLibrary
#  define ತುಂಬು_dll_error GetWin32Error
# ಇಲ್ಲದಿದ್ದರೆ
#  include <dlfcn.h>
#  define HINSTANCE ಶೂನ್ಯ*
#  define RUBY_PROC ಶೂನ್ಯ*
#  define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define close_dll dlclose
#  define ತುಂಬು_dll_error dlerror
# endif
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ruby_initialized = 0;
ಸ್ಥಿರ ಶೂನ್ಯ *ruby_ರಾಶಿ_ಆರಂಭ;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 objtbl;

ಸ್ಥಿರ ಮೌಲ್ಯ_3 mVIM;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 c_ಬಫರ್;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 c_Vim_ಕಿಟಕಿ;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 e_Deleted_ಬಫರ್_Error;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 e_Deleted_ಕಿಟಕಿ_Error;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ensure_ruby_initialized(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ error_print(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ ruby_io_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ruby_vim_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ruby_convert_ಗಮ್ಯ_vim_ಮೌಲ್ಯ(ಮೌಲ್ಯ_3 val, typval_T *rettv);

#ಆದರೆ defined(__ia64) && !defined(ruby_init_ರಾಶಿ)
# define ruby_init_ರಾಶಿ(addr) ruby_init_ರಾಶಿ((addr), rb_ia64_bsp())
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 27
#  undef rb_define_global_function
#  undef rb_define_ವಿಧಾನ
#  undef rb_define_module_function
#  undef rb_define_singleton_ವಿಧಾನ
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ಆದರೆ RUBY_ಆವೃತ್ತಿ < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_ಮಾರ್ಗ			dll_rb_class_ಮಾರ್ಗ
# ifdef USE_TYPEDDATA
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 23
#   define rb_ದತ್ತಾಂಶ_typed_object_wrap	dll_rb_ದತ್ತಾಂಶ_typed_object_wrap
#  ಇಲ್ಲದಿದ್ದರೆ
#   define rb_ದತ್ತಾಂಶ_typed_object_alloc	dll_rb_ದತ್ತಾಂಶ_typed_object_alloc
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  define rb_ದತ್ತಾಂಶ_object_alloc		dll_rb_ದತ್ತಾಂಶ_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_ವಿಧಾನ		dll_rb_define_ವಿಧಾನ
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_ವಿಧಾನ	dll_rb_define_singleton_ವಿಧಾನ
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_ಸೂಚ್ಯಂಕ_Error			(*dll_rb_e_ಸೂಚ್ಯಂಕ_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 21
#  define rb_funcallv			dll_rb_funcallv
# ಇಲ್ಲದಿದ್ದರೆ
#  define rb_funcall2			dll_rb_funcall2
# endif
# define rb_global_variable		dll_rb_global_variable
# define rb_hash_aset			dll_rb_hash_aset
# define rb_hash_foreach		dll_rb_hash_foreach
# define rb_hash_new			dll_rb_hash_new
# define rb_inspect			dll_rb_inspect
# define rb_int2inum			dll_rb_int2inum

// ruby.h may redefine rb_intern to use RUBY_CONST_ID_CACHE(), but that won't
// work.  Not using the cache appears to be the best solution.
# undef rb_intern
# define rb_intern			dll_rb_intern

# ಆದರೆ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_ಗಣ			dll_rb_lastline_ಗಣ
# define rb_protect			dll_rb_protect
# define rb_ತುಂಬು			dll_rb_ತುಂಬು
# ಆದರೆ RUBY_ಆವೃತ್ತಿ < 20
#  define rb_num2ulong			dll_rb_num2ulong
# endif
# define rb_obj_alloc			dll_rb_obj_alloc
# define rb_obj_as_string		dll_rb_obj_as_string
# define rb_obj_id			dll_rb_obj_id
# define rb_raise			dll_rb_raise
# define rb_str_cat			dll_rb_str_cat
# define rb_str_concat			dll_rb_str_concat
# undef rb_str_new
# define rb_str_new			dll_rb_str_new
# ifdef rb_str_new2
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
#  define need_rb_str_new_cstr 1
// Ruby's headers #define rb_str_new_cstr to make use of GCC's
// __builtin_constant_p extension.
#  undef rb_str_new_cstr
#  define rb_str_new_cstr		dll_rb_str_new_cstr
# ಇಲ್ಲದಿದ್ದರೆ
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_ಮೌಲ್ಯ		dll_rb_string_ಮೌಲ್ಯ
# define rb_string_ಮೌಲ್ಯ_ptr		dll_rb_string_ಮೌಲ್ಯ_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ತಳ್ಳು			dll_rb_ary_ತಳ್ಳು
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ರಾಶಿ
#  define ruby_init_ರಾಶಿ(addr)	dll_ruby_init_ರಾಶಿ((addr), rb_ia64_bsp())
# ಇಲ್ಲದಿದ್ದರೆ
#  define ruby_init_ರಾಶಿ		dll_ruby_init_ರಾಶಿ
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ		dll_rb_enc_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ
# define rb_enc_ಕಂಡುಹಿಡಿ			dll_rb_enc_ಕಂಡುಹಿಡಿ
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_assoc_new) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
ಮೌಲ್ಯ_3 *dll_rb_cFalseClass;
ಮೌಲ್ಯ_3 *dll_rb_cFixnum;
# ಆದರೆ defined(USE_RUBY_INTEGER)
ಮೌಲ್ಯ_3 *dll_rb_cInteger;
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
ಮೌಲ್ಯ_3 *dll_rb_cFloat;
# endif
ಮೌಲ್ಯ_3 *dll_rb_cNilClass;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_cObject;
ಮೌಲ್ಯ_3 *dll_rb_cString;
ಮೌಲ್ಯ_3 *dll_rb_c_ಚಿಹ್ನೆ;
ಮೌಲ್ಯ_3 *dll_rb_cTrueClass;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_class_new_instance) (ಪೂರ್ಣಾಂಕ,ಮೌಲ್ಯ_3*,ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_check_type) (ಮೌಲ್ಯ_3,ಪೂರ್ಣಾಂಕ);
# ifdef USE_TYPEDDATA
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_rb_check_typeddata) (ಮೌಲ್ಯ_3,ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t *);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_class_ಮಾರ್ಗ) (ಮೌಲ್ಯ_3);
# ifdef USE_TYPEDDATA
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 23
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ದತ್ತಾಂಶ_typed_object_wrap) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t *);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ದತ್ತಾಂಶ_typed_object_alloc) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t *);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ದತ್ತಾಂಶ_object_alloc) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, RUBY_ದತ್ತಾಂಶ_FUNC, RUBY_ದತ್ತಾಂಶ_FUNC);
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_debug_rstring_null_ptr) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_define_class_under) (ಮೌಲ್ಯ_3, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_const) (ಮೌಲ್ಯ_3,ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_global_function) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3(*)(ಪೂರ್ಣಾಂಕ,ಮೌಲ್ಯ_3*,ಮೌಲ್ಯ_3),ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_ವಿಧಾನ) (ಮೌಲ್ಯ_3,ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3(*)(ANYARGS),ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_define_module) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_module_function) (ಮೌಲ್ಯ_3,ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3(*)(ಮೌಲ್ಯ_3,ಮೌಲ್ಯ_3),ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_singleton_ವಿಧಾನ) (ಮೌಲ್ಯ_3,ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3(*)(ANYARGS),ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_virtual_variable) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*,ಮೌಲ್ಯ_3(*)(ID,ಮೌಲ್ಯ_3*),ಶೂನ್ಯ(*)());
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_stdout;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_stderr;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eArgError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_e_ಸೂಚ್ಯಂಕ_Error;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eRuntimeError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eStandardError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_eval_string_protect) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ*);
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 21
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_funcallv) (ಮೌಲ್ಯ_3, ID, ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_3*);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_funcall2) (ಮೌಲ್ಯ_3, ID, ಪೂರ್ಣಾಂಕ, ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_3*);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_global_variable) (ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_aset) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_foreach) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಂಕ (*)(ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3), ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_new) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_inspect) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_int2inum) (ದೀರ್ಘ);
ಸ್ಥಿರ ID (*dll_rb_intern) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ಸ್ಥಿರ ದೀರ್ಘ (*dll_rb_fix2int) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ದೀರ್ಘ (*dll_rb_num2int) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ (*dll_rb_num2uint) (ಮೌಲ್ಯ_3);
# endif
ಸ್ಥಿರ ದ್ವಿಗುಣ (*dll_rb_num2dbl) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_lastline_get) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_lastline_ಗಣ) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_protect) (ಮೌಲ್ಯ_3 (*)(ಮೌಲ್ಯ_3), ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಂಕ*);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_ತುಂಬು) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ದೀರ್ಘ (*dll_rb_num2long) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ (*dll_rb_num2ulong) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_alloc) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_as_string) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_id) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_raise) (ಮೌಲ್ಯ_3, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ...);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_string_ಮೌಲ್ಯ) (ಅಸ್ಥಿರ ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_cat) (ಮೌಲ್ಯ_3, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ದೀರ್ಘ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_concat) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ದೀರ್ಘ);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new_cstr) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new2) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_errinfo) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_loadpath) (ಶೂನ್ಯ);
# ifdef MSWIN
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_sysinit) (ಪೂರ್ಣಾಂಕ*, ಅಕ್ಷರ***);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_rb_w32_snprintf)(ಅಕ್ಷರ*, size_t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ...);
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
#  ifdef _MSC_VER
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_unexpected_type) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಂಕ);
#  ಇಲ್ಲದಿದ್ದರೆ
NORETURN(ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_unexpected_type) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಂಕ));
#  endif
# endif
ಸ್ಥಿರ ಅಕ್ಷರ * (*dll_rb_string_ಮೌಲ್ಯ_ptr) (ಅಸ್ಥಿರ ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_float_new) (ದ್ವಿಗುಣ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_new) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_new4) (ದೀರ್ಘ n, ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_3 *elts);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_ತಳ್ಳು) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26 && RUBY_ಆವೃತ್ತಿ <= 32
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_ary_detransient) (ಮೌಲ್ಯ_3);
# endif
# ifdef __ia64
ಸ್ಥಿರ ಶೂನ್ಯ * (*dll_rb_ia64_bsp) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_ರಾಶಿ)(ಮೌಲ್ಯ_3*, ಶೂನ್ಯ*);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_ರಾಶಿ)(ಮೌಲ್ಯ_3*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_int2big)(SIGNED_ಮೌಲ್ಯ);

ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_script) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_rb_enc_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಸ್ಥಿರ rb_encoding* (*dll_rb_enc_ಕಂಡುಹಿಡಿ) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_enc_str_new) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ದೀರ್ಘ, rb_encoding*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_sprintf) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ...);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_require) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_ruby_options)(ಪೂರ್ಣಾಂಕ, ಅಕ್ಷರ**);

# ಆದರೆ defined(USE_RGENGC) && USE_RGENGC
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ == 21
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_gc_writebarrier_unprotect_promoted)(ಮೌಲ್ಯ_3);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_gc_writebarrier_unprotect)(ಮೌಲ್ಯ_3 obj);
#  endif
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30
#  ifdef _MSC_VER
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ)(size_t, size_t);
#  ಇಲ್ಲದಿದ್ದರೆ
NORETURN(ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ)(size_t, size_t));
#  endif
# endif

# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26 && RUBY_ಆವೃತ್ತಿ <= 32
ಶೂನ್ಯ rb_ary_detransient_stub(ಮೌಲ್ಯ_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 22
    ದೀರ್ಘ
rb_num2long_stub(ಮೌಲ್ಯ_3 x)
#  ಇಲ್ಲದಿದ್ದರೆ
    SIGNED_ಮೌಲ್ಯ
rb_num2long_stub(ಮೌಲ್ಯ_3 x)
#  endif
{
    ಹಿಂತಿರುಗಿ dll_rb_num2long(x);
}
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26
    ಮೌಲ್ಯ_3
rb_int2big_stub(intptr_t x)
#  ಇಲ್ಲದಿದ್ದರೆ
    ಮೌಲ್ಯ_3
rb_int2big_stub(SIGNED_ಮೌಲ್ಯ x)
#  endif
{
    ಹಿಂತಿರುಗಿ dll_rb_int2big(x);
}
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    ದೀರ್ಘ
rb_fix2int_stub(ಮೌಲ್ಯ_3 x)
{
    ಹಿಂತಿರುಗಿ dll_rb_fix2int(x);
}
    ದೀರ್ಘ
rb_num2int_stub(ಮೌಲ್ಯ_3 x)
{
    ಹಿಂತಿರುಗಿ dll_rb_num2int(x);
}
#  endif
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
    ಮೌಲ್ಯ_3
rb_float_new_ಒಳ_ರಾಶಿ(ದ್ವಿಗುಣ d)
{
    ಹಿಂತಿರುಗಿ dll_rb_float_new(d);
}
#   ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 22
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ
rb_num2ulong(ಮೌಲ್ಯ_3 x)
#   ಇಲ್ಲದಿದ್ದರೆ
    ಮೌಲ್ಯ_3
rb_num2ulong(ಮೌಲ್ಯ_3 x)
#   endif
{
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)RSHIFT((SIGNED_ಮೌಲ್ಯ)(x),1);
}
#  endif
#  ಆದರೆ defined(USE_RGENGC) && USE_RGENGC
#   ಆದರೆ RUBY_ಆವೃತ್ತಿ == 21
    ಶೂನ್ಯ
rb_gc_writebarrier_unprotect_promoted_stub(ಮೌಲ್ಯ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ಇಲ್ಲದಿದ್ದರೆ
    ಶೂನ್ಯ
rb_gc_writebarrier_unprotect_stub(ಮೌಲ್ಯ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26 && RUBY_ಆವೃತ್ತಿ <= 32
    ಶೂನ್ಯ
rb_ary_detransient_stub(ಮೌಲ್ಯ_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30
    ಶೂನ್ಯ
rb_check_type_stub(ಮೌಲ್ಯ_3 obj, ಪೂರ್ಣಾಂಕ t)
{
    dll_rb_check_type(obj, t);
}
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ
rb_num2uint_stub(ಮೌಲ್ಯ_3 x)
{
    ಹಿಂತಿರುಗಿ dll_rb_num2uint(x);
}
    ಶೂನ್ಯ
ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ_stub(size_t x, size_t y)
{
    dll_ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ(x, y);
}
#  endif
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
    ಶೂನ್ಯ
rb_debug_rstring_null_ptr_stub(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    ಶೂನ್ಯ
rb_unexpected_type_stub(ಮೌಲ್ಯ_3 self, ಪೂರ್ಣಾಂಕ t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
ಶೂನ್ಯ *rb_check_typeddata_stub(ಮೌಲ್ಯ_3 obj, ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t *ದತ್ತಾಂಶ_type)
{
    ಹಿಂತಿರುಗಿ dll_rb_check_typeddata(obj, ದತ್ತಾಂಶ_type);
}
#  endif
# endif // ifndef PROTO

ಸ್ಥಿರ HINSTANCE hinstRuby = ಖಾಲಿ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ಸ್ಥಿರ ರಚನೆ
{
    ಅಕ್ಷರ *name;
    RUBY_PROC *ptr;
} ruby_funcname_ಕೋಷ್ಟಕ[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# ಆದರೆ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_ಚಿಹ್ನೆ},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_ಮಾರ್ಗ},
# ifdef USE_TYPEDDATA
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_ದತ್ತಾಂಶ_typed_object_wrap},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_ದತ್ತಾಂಶ_typed_object_alloc},
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_ದತ್ತಾಂಶ_object_alloc},
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_ವಿಧಾನ},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_ವಿಧಾನ},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_ಸೂಚ್ಯಂಕ_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_ಗಣ},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_ತುಂಬು},
    {"rb_num2long", (RUBY_PROC*)&dll_rb_num2long},
    {"rb_num2ulong", (RUBY_PROC*)&dll_rb_num2ulong},
    {"rb_obj_alloc", (RUBY_PROC*)&dll_rb_obj_alloc},
    {"rb_obj_as_string", (RUBY_PROC*)&dll_rb_obj_as_string},
    {"rb_obj_id", (RUBY_PROC*)&dll_rb_obj_id},
    {"rb_raise", (RUBY_PROC*)&dll_rb_raise},
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_ಮೌಲ್ಯ},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_ಮೌಲ್ಯ_ptr},
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ಇಲ್ಲದಿದ್ದರೆ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ತಳ್ಳು},
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 26 && RUBY_ಆವೃತ್ತಿ <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_ಕಂಡುಹಿಡಿ},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ರಾಶಿ},
# ಆದರೆ defined(USE_RGENGC) && USE_RGENGC
#  ಆದರೆ RUBY_ಆವೃತ್ತಿ == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_ಗಾತ್ರ_ಅತಿಪ್ರವಾಹ},
# endif
    {"", ಖಾಲಿ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ruby_runtime_link_init(ಅಕ್ಷರ *libname, ಪೂರ್ಣಾಂಕ verbose)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (hinstRuby)
	ಹಿಂತಿರುಗಿ OK;
    hinstRuby = ತುಂಬು_dll(libname);
    ಆದರೆ (!hinstRuby)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (i = 0; ruby_funcname_ಕೋಷ್ಟಕ[i].ptr; ++i)
    {
	ಆದರೆ (!(*ruby_funcname_ಕೋಷ್ಟಕ[i].ptr = ಚಿಹ್ನೆ_from_dll(hinstRuby,
			ruby_funcname_ಕೋಷ್ಟಕ[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ಖಾಲಿ;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), ruby_funcname_ಕೋಷ್ಟಕ[i].name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    ಪೂರ್ಣಾಂಕ
ruby_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ ruby_runtime_link_init((ಅಕ್ಷರ *)p_rubydll, verbose) == OK;
}
#ಆದರೆ_ಅಂತ್ಯ // defined(DYNAMIC_RUBY)

    ಶೂನ್ಯ
ruby_ಕೊನೆ(ಶೂನ್ಯ)
{
}

    ಶೂನ್ಯ
ex_ruby(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ state;
    ಅಕ್ಷರ *script = ಖಾಲಿ;

    script = (ಅಕ್ಷರ *)script_get(eap, eap->arg);
    ಆದರೆ (!eap->skip && ensure_ruby_initialized())
    {
	ಆದರೆ (script == ಖಾಲಿ)
	    rb_eval_string_protect((ಅಕ್ಷರ *)eap->arg, &state);
	ಇಲ್ಲದಿದ್ದರೆ
	    rb_eval_string_protect(script, &state);
	ಆದರೆ (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_str2rb_enc_str(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    ದೀರ್ಘ lval;
    char_u *sval;
    rb_encoding *enc;

    ಆದರೆ (get_option_ಮೌಲ್ಯ((char_u *)"enc", &lval, &sval, ಖಾಲಿ, 0) == gov_string)
    {
	enc = rb_enc_ಕಂಡುಹಿಡಿ((ಅಕ್ಷರ *)sval);
	vim_free(sval);
	ಆದರೆ (enc)
	    ಹಿಂತಿರುಗಿ rb_enc_str_new(s, (ದೀರ್ಘ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s), enc);
    }
    ಹಿಂತಿರುಗಿ rb_str_new2(s);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
eval_enc_string_protect(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ *state)
{
    ದೀರ್ಘ lval;
    char_u *sval;
    rb_encoding *enc;
    ಮೌಲ್ಯ_3 v;

    ಆದರೆ (get_option_ಮೌಲ್ಯ((char_u *)"enc", &lval, &sval, ಖಾಲಿ, 0) == gov_string)
    {
	enc = rb_enc_ಕಂಡುಹಿಡಿ((ಅಕ್ಷರ *)sval);
	vim_free(sval);
	ಆದರೆ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ಹಿಂತಿರುಗಿ rb_eval_string_protect(String_ಮೌಲ್ಯ_Ptr(v), state);
	}
    }
    ಹಿಂತಿರುಗಿ rb_eval_string_protect(str, state);
}

    ಶೂನ್ಯ
ex_rubydo(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ಆದರೆ (!ensure_ruby_initialized())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (i = eap->line1; i <= eap->line2; i++)
    {
	ಮೌಲ್ಯ_3 ಸಾಲು;

	ಆದರೆ (i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು = vim_str2rb_enc_str((ಅಕ್ಷರ *)ml_get(i));
	rb_lastline_ಗಣ(ಸಾಲು);
	eval_enc_string_protect((ಅಕ್ಷರ *) eap->arg, &state);
	ಆದರೆ (state)
	{
	    error_print(state);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (was_curbuf != curbuf || i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು = rb_lastline_get();
	ಆದರೆ (!NIL_P(ಸಾಲು))
	{
	    ಆದರೆ (TYPE(ಸಾಲು) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_ದ_string));
		ಹಿಂತಿರುಗಿ;
	    }
	    ml_replace(i, (char_u *) String_ಮೌಲ್ಯ_Ptr(ಸಾಲು), 1);
	    ಬದಲಾಯಿತು();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYNTAX_HL
	    syn_ಬದಲಾಯಿತು(i); // recompute syntax hl. for this line
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
rb_ತುಂಬು_wrap(ಮೌಲ್ಯ_3 file_ಗಮ್ಯ_ತುಂಬು)
{
    rb_ತುಂಬು(file_ಗಮ್ಯ_ತುಂಬು, 0);
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಶೂನ್ಯ
ex_rubyfile(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ state;

    ಆದರೆ (!ensure_ruby_initialized())
	ಹಿಂತಿರುಗಿ;

    ಮೌಲ್ಯ_3 file_ಗಮ್ಯ_ತುಂಬು = rb_str_new2((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)eap->arg);
    rb_protect(rb_ತುಂಬು_wrap, file_ಗಮ್ಯ_ತುಂಬು, &state);
    ಆದರೆ (state)
	error_print(state);
}

    ಶೂನ್ಯ
ruby_ಬಫರ್_free(buf_T *buf)
{
    ಆದರೆ (buf->b_ruby_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rb_hash_aset(objtbl, rb_obj_id((ಮೌಲ್ಯ_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->ದತ್ತಾಂಶ = ಖಾಲಿ;
}

    ಶೂನ್ಯ
ruby_ಕಿಟಕಿ_free(win_T *win)
{
    ಆದರೆ (win->w_ruby_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rb_hash_aset(objtbl, rb_obj_id((ಮೌಲ್ಯ_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->ದತ್ತಾಂಶ = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ensure_ruby_initialized(ಶೂನ್ಯ)
{
    ಆದರೆ (ruby_initialized)
	ಹಿಂತಿರುಗಿ ruby_initialized;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
    ಆದರೆ (ruby_enabled(TRUE))
#ಆದರೆ_ಅಂತ್ಯ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// suggested by Ariya Mizutani
	ಪೂರ್ಣಾಂಕ argc = 1;
	ಅಕ್ಷರ *argv[] = {"gvim.exe"};
	ಅಕ್ಷರ **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ruby_init_ರಾಶಿ(ruby_ರಾಶಿ_ಆರಂಭ);
	    ruby_init();
	}
	{
	    ಪೂರ್ಣಾಂಕ dummy_argc = 2;
	    ಅಕ್ಷರ *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
    ಇಲ್ಲದಿದ್ದರೆ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ಹಿಂತಿರುಗಿ 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ruby_initialized;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
error_print(ಪೂರ್ಣಾಂಕ state)
{
    ಮೌಲ್ಯ_3 error;
    ಮೌಲ್ಯ_3 eclass;
    ಮೌಲ್ಯ_3 einfo;
    ಮೌಲ್ಯ_3 bt;
    ಪೂರ್ಣಾಂಕ attr;
    ಅಕ್ಷರ buff[BUFSIZ];
    ದೀರ್ಘ i;

#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_RETURN	0x1
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_BREAK	0x2
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_ಮುಂದಿನ	0x3
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_RETRY	0x4
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_REDO	0x5
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_RAISE	0x6
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_THROW	0x7
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_FATAL	0x8
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_ಮುಖವಾಡ	0xf

    ಆಯ್ಕೆ (state)
    {
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_RETURN:
	    emsg(_(e_unexpected_return));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_ಮುಂದಿನ:
	    emsg(_(e_unexpected_ಮುಂದಿನ));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_BREAK:
	    emsg(_(e_unexpected_break));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_REDO:
	    emsg(_(e_unexpected_redo));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_RETRY:
	    emsg(_(e_retry_outside_ದ_rescue_clause));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_RAISE:
	ಸ್ಥಿತಿ ಟ್ಯಾಗ್_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_ದ(error);
	    einfo = rb_obj_as_string(error);
	    ಆದರೆ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಮೌಲ್ಯ_3 epath;
		ಅಕ್ಷರ *p;

		epath = rb_class_ಮಾರ್ಗ(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ಆದರೆ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    ಗಾಗಿ (i = 0; i < RARRAY_LEN(bt); i++)
		ಸಂದೇಶ_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ಇಲ್ಲದಿದ್ದರೆ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    ಗಾಗಿ (i = 0; i < RARRAY_LEN(bt); i++)
		ಸಂದೇಶ_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_ಸ್ಥಿತಿ_nr), state);
	    emsg(buff);
	    ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_ಸಂದೇಶ(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
    ಅಕ್ಷರ *buff, *p;

    str = rb_obj_as_string(str);
    ಆದರೆ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(ಅಕ್ಷರ, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ಆದರೆ (p) *p = '\0';
	ಸಂದೇಶ(buff);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ("");
    }
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_ಗಣ_option(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
    do_ಗಣ((char_u *)String_ಮೌಲ್ಯ_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_command(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
    do_cmdline_cmd((char_u *)String_ಮೌಲ್ಯ_Ptr(str));
    ಹಿಂತಿರುಗಿ Qnil;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_ಗಮ್ಯ_ruby(typval_T *tv)
{
    ಮೌಲ್ಯ_3 ಫಲಿತಾಂಶ = Qnil;

    ಆದರೆ (tv->v_type == VAR_STRING)
    {
	ಫಲಿತಾಂಶ = rb_str_new2(tv->vval.v_string == ಖಾಲಿ
					  ? "" : (ಅಕ್ಷರ *)(tv->vval.v_string));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	ಫಲಿತಾಂಶ = INT2_ಅಂಕೆ(tv->vval.v_ಸಂಖ್ಯೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_FLOAT)
    {
	ಫಲಿತಾಂಶ = rb_float_new(tv->vval.v_float);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	ಫಲಿತಾಂಶ = rb_ary_new();

	ಆದರೆ (list != ಖಾಲಿ)
	{
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, curr)
		rb_ary_ತಳ್ಳು(ಫಲಿತಾಂಶ, vim_ಗಮ್ಯ_ruby(&curr->li_tv));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_DICT)
    {
	ಫಲಿತಾಂಶ = rb_hash_new();

	ಆದರೆ (tv->vval.v_dict != ಖಾಲಿ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_ಬಳಸಿದ;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	    {
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(ಫಲಿತಾಂಶ, rb_str_new2((ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ),
						     vim_ಗಮ್ಯ_ruby(&di->di_tv));
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ಆದರೆ (tv->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE)
	    ಫಲಿತಾಂಶ = Qtrue;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE)
	    ಫಲಿತಾಂಶ = Qfalse;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_BLOB)
    {
	ಫಲಿತಾಂಶ = rb_str_new(tv->vval.v_blob->bv_ga.ga_ದತ್ತಾಂಶ,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_evaluate(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    typval_T    *tv;
    ಮೌಲ್ಯ_3       ಫಲಿತಾಂಶ;

    tv = eval_ಅಭಿವ್ಯಕ್ತಿ((char_u *)String_ಮೌಲ್ಯ_Ptr(str), ಖಾಲಿ);
    ಆದರೆ (tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ Qnil;
    ಫಲಿತಾಂಶ = vim_ಗಮ್ಯ_ruby(tv);

    free_tv(tv);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ Qnil;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
ಸ್ಥಿರ size_t ಬಫರ್_dsize(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t ಬಫರ್_type = {
    "vim_buffer",
    {0, 0, ಬಫರ್_dsize,
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 27
	0, {0}
# ಇಲ್ಲದಿದ್ದರೆ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ಸ್ಥಿರ size_t
ಬಫರ್_dsize(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ(buf_T);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_new(buf_T *buf)
{
    ಆದರೆ (buf->b_ruby_ref)
    {
	ಹಿಂತಿರುಗಿ (ಮೌಲ್ಯ_3) buf->b_ruby_ref;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
	ಮೌಲ್ಯ_3 obj = Typed_ದತ್ತಾಂಶ_Wrap_Struct(c_ಬಫರ್, &ಬಫರ್_type, buf);
#ಇಲ್ಲದಿದ್ದರೆ
	ಮೌಲ್ಯ_3 obj = ದತ್ತಾಂಶ_Wrap_Struct(c_ಬಫರ್, 0, 0, buf);
#ಆದರೆ_ಅಂತ್ಯ
	buf->b_ruby_ref = (ಶೂನ್ಯ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ಹಿಂತಿರುಗಿ obj;
    }
}

    ಸ್ಥಿರ buf_T *
get_buf(ಮೌಲ್ಯ_3 obj)
{
    buf_T *buf;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
    Typed_ದತ್ತಾಂಶ_Get_Struct(obj, buf_T, &ಬಫರ್_type, buf);
#ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ_Get_Struct(obj, buf_T, buf);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (buf == ಖಾಲಿ)
	rb_raise(e_Deleted_ಬಫರ್_Error, "attempt to refer to deleted buffer");
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_blob(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
    ಮೌಲ್ಯ_3 ಫಲಿತಾಂಶ = rb_str_new("0z", 2);
    ಅಕ್ಷರ    buf[4];
    ಪೂರ್ಣಾಂಕ	i;
    ಗಾಗಿ (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(RSTRING_PTR(str)[i]));
	rb_str_concat(ಫಲಿತಾಂಶ, rb_str_new2(buf));
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_s_current(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಬಫರ್_new(curbuf);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_s_current_getter(ID id ಬಳಸದ, ಮೌಲ್ಯ_3 *x ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಬಫರ್_new(curbuf);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_s_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    buf_T *b;
    ಪೂರ್ಣಾಂಕ n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ಆದರೆ (b->b_p_bl)
	    n++;
    }

    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(n);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_s_aref(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T *b;
    ಪೂರ್ಣಾಂಕ n = NUM2INT(ಅಂಕೆ);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ಆದರೆ (!b->b_p_bl)
	    ಮುಂದುವರಿ;

	ಆದರೆ (n == 0)
	    ಹಿಂತಿರುಗಿ ಬಫರ್_new(b);

	n--;
    }
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_name(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಹಿಂತಿರುಗಿ buf->b_ffname ? rb_str_new2((ಅಕ್ಷರ *)buf->b_ffname) : Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_ಸಂಖ್ಯೆ(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(buf->b_fnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
get_ಬಫರ್_ಸಾಲು(buf_T *buf, linenr_T n)
{
    ಆದರೆ (n <= 0 || n > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	rb_raise(rb_e_ಸೂಚ್ಯಂಕ_Error, "line number %ld out of range", (ದೀರ್ಘ)n);
    ಹಿಂತಿರುಗಿ vim_str2rb_enc_str((ಅಕ್ಷರ *)ml_get_buf(buf, n, FALSE));
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_aref(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T *buf = get_buf(self);

    ಆದರೆ (buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ get_ಬಫರ್_ಸಾಲು(buf, (linenr_T)NUM2LONG(ಅಂಕೆ));
    ಹಿಂತಿರುಗಿ Qnil; // For stop warning
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಗಣ_ಬಫರ್_ಸಾಲು(buf_T *buf, linenr_T n, ಮೌಲ್ಯ_3 str)
{
    ಅಕ್ಷರ	*ಸಾಲು = String_ಮೌಲ್ಯ_Ptr(str);
    aco_save_T	aco;

    ಆದರೆ (n > 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && ಸಾಲು != ಖಾಲಿ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಆದರೆ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)ಸಾಲು, TRUE);
		ಬದಲಾಯಿತು();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYNTAX_HL
		syn_ಬದಲಾಯಿತು(n); // recompute syntax hl. for this line
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rb_raise(rb_e_ಸೂಚ್ಯಂಕ_Error, "line number %ld out of range", (ದೀರ್ಘ)n);
    }
    ಹಿಂತಿರುಗಿ str;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_aset(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ, ಮೌಲ್ಯ_3 str)
{
    buf_T *buf = get_buf(self);

    ಆದರೆ (buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಗಣ_ಬಫರ್_ಸಾಲು(buf, (linenr_T)NUM2LONG(ಅಂಕೆ), str);
    ಹಿಂತಿರುಗಿ str;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_delete(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T	*buf = get_buf(self);
    ದೀರ್ಘ	n = NUM2LONG(ಅಂಕೆ);
    aco_save_T	aco;

    ಆದರೆ (n > 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಆದರೆ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_ಸಾಲುಗಳು_mark(n, 1L);

		ಬದಲಾಯಿತು();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rb_raise(rb_e_ಸೂಚ್ಯಂಕ_Error, "line number %ld out of range", n);
    }
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಬಫರ್_append(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ, ಮೌಲ್ಯ_3 str)
{
    buf_T	*buf = get_buf(self);
    ಅಕ್ಷರ	*ಸಾಲು = String_ಮೌಲ್ಯ_Ptr(str);
    ದೀರ್ಘ	n = NUM2LONG(ಅಂಕೆ);
    aco_save_T	aco;

    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
    {
	rb_raise(rb_e_ಸೂಚ್ಯಂಕ_Error, "NULL line");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n >= 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಆದರೆ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) ಸಾಲು, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_ಸಾಲುಗಳು_mark(n, 1L);

		ಬದಲಾಯಿತು();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rb_raise(rb_e_ಸೂಚ್ಯಂಕ_Error, "line number %ld out of range", n);
    }
    ಹಿಂತಿರುಗಿ str;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
ಸ್ಥಿರ size_t ಕಿಟಕಿ_dsize(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ rb_ದತ್ತಾಂಶ_type_t ಕಿಟಕಿ_type = {
    "vim_window",
    {0, 0, ಕಿಟಕಿ_dsize,
# ಆದರೆ RUBY_ಆವೃತ್ತಿ >= 27
	0, {0}
# ಇಲ್ಲದಿದ್ದರೆ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ಸ್ಥಿರ size_t
ಕಿಟಕಿ_dsize(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *win ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ(win_T);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_new(win_T *win)
{
    ಆದರೆ (win->w_ruby_ref)
    {
	ಹಿಂತಿರುಗಿ (ಮೌಲ್ಯ_3) win->w_ruby_ref;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
	ಮೌಲ್ಯ_3 obj = Typed_ದತ್ತಾಂಶ_Wrap_Struct(c_Vim_ಕಿಟಕಿ, &ಕಿಟಕಿ_type, win);
#ಇಲ್ಲದಿದ್ದರೆ
	ಮೌಲ್ಯ_3 obj = ದತ್ತಾಂಶ_Wrap_Struct(c_Vim_ಕಿಟಕಿ, 0, 0, win);
#ಆದರೆ_ಅಂತ್ಯ
	win->w_ruby_ref = (ಶೂನ್ಯ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ಹಿಂತಿರುಗಿ obj;
    }
}

    ಸ್ಥಿರ win_T *
get_win(ಮೌಲ್ಯ_3 obj)
{
    win_T *win;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_TYPEDDATA
    Typed_ದತ್ತಾಂಶ_Get_Struct(obj, win_T, &ಕಿಟಕಿ_type, win);
#ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ_Get_Struct(obj, win_T, win);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (win == ಖಾಲಿ)
	rb_raise(e_Deleted_ಕಿಟಕಿ_Error, "attempt to refer to deleted window");
    ಹಿಂತಿರುಗಿ win;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_s_current(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_new(curwin);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_s_current_getter(ID id ಬಳಸದ, ಮೌಲ್ಯ_3 *x ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಸಾಲು_s_current(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ get_ಬಫರ್_ಸಾಲು(curbuf, curwin->w_cursor.lnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಗಣ_current_ಸಾಲು(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 str)
{
    ಹಿಂತಿರುಗಿ ಗಣ_ಬಫರ್_ಸಾಲು(curbuf, curwin->w_cursor.lnum, str);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
current_ಸಾಲು_ಸಂಖ್ಯೆ(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ INT2FIX((ಪೂರ್ಣಾಂಕ)curwin->w_cursor.lnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_s_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    win_T	*w;
    ಪೂರ್ಣಾಂಕ n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(n);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_s_aref(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    win_T *w;
    ಪೂರ್ಣಾಂಕ n = NUM2INT(ಅಂಕೆ);

    ಗಾಗಿ (w = firstwin; w != ಖಾಲಿ; w = w->w_ಮುಂದಿನ, --n)
	ಆದರೆ (n == 0)
	    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_new(w);
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಬಫರ್(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಹಿಂತಿರುಗಿ ಬಫರ್_new(win->w_ಬಫರ್);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಎತ್ತರ(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(win->w_ಎತ್ತರ);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಗಣ_ಎತ್ತರ(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಎತ್ತರ)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(ಎತ್ತರ));
    curwin = savewin;
    ಹಿಂತಿರುಗಿ ಎತ್ತರ;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಅಗಲ(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ INT2_ಅಂಕೆ(get_win(self)->w_ಅಗಲ);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಗಣ_ಅಗಲ(ಮೌಲ್ಯ_3 self ಬಳಸದ, ಮೌಲ್ಯ_3 ಅಗಲ)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(ಅಗಲ));
    curwin = savewin;
    ಹಿಂತಿರುಗಿ ಅಗಲ;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_cursor(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಹಿಂತಿರುಗಿ rb_assoc_new(INT2_ಅಂಕೆ(win->w_cursor.lnum), INT2_ಅಂಕೆ(win->w_cursor.ಲಂಬಸಾಲು));
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಕಿಟಕಿ_ಗಣ_cursor(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 pos)
{
    ಮೌಲ್ಯ_3 lnum, ಲಂಬಸಾಲು;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ಆದರೆ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    ಲಂಬಸಾಲು = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.ಲಂಬಸಾಲು = NUM2UINT(ಲಂಬಸಾಲು);
    win->w_ಗಣ_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
f_nop(ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
f_p(ಪೂರ್ಣಾಂಕ argc, ಮೌಲ್ಯ_3 *argv, ಮೌಲ್ಯ_3 self ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಮೌಲ್ಯ_3 str = rb_str_new("", 0);
    ಮೌಲ್ಯ_3 ret = Qnil;

    ಗಾಗಿ (i = 0; i < argc; i++)
    {
	ಆದರೆ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    ಸಂದೇಶ(RSTRING_PTR(str));

    ಆದರೆ (argc == 1)
	ret = argv[0];
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ruby_io_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DYNAMIC_RUBY
    RUBYEXTERN ಮೌಲ್ಯ_3 rb_stdout;
    RUBYEXTERN ಮೌಲ್ಯ_3 rb_stderr;
#ಆದರೆ_ಅಂತ್ಯ

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_ವಿಧಾನ(rb_stdout, "write", (ಶೂನ್ಯ*)vim_ಸಂದೇಶ, 1);
    rb_define_singleton_ವಿಧಾನ(rb_stdout, "flush", (ಶೂನ್ಯ*)f_nop, 0);
    rb_define_singleton_ವಿಧಾನ(rb_stderr, "write", (ಶೂನ್ಯ*)vim_ಸಂದೇಶ, 1);
    rb_define_singleton_ವಿಧಾನ(rb_stderr, "flush", (ಶೂನ್ಯ*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ruby_vim_init(ಶೂನ್ಯ)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_ಅಂಕೆ(VIM_ಆವೃತ್ತಿ_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_ಅಂಕೆ(VIM_ಆವೃತ್ತಿ_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_ಅಂಕೆ(VIM_ಆವೃತ್ತಿ_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_ಅಂಕೆ(VIM_ಆವೃತ್ತಿ_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_ಆವೃತ್ತಿ_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_ಆವೃತ್ತಿ_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_ಆವೃತ್ತಿ_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_ಆವೃತ್ತಿ_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_ಸಂದೇಶ, 1);
    rb_define_module_function(mVIM, "set_option", vim_ಗಣ_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_ಬಫರ್_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_ಕಿಟಕಿ_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_ಬಫರ್ = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_ವಿಧಾನ(c_ಬಫರ್, "current", (ಶೂನ್ಯ*)ಬಫರ್_s_current, 0);
    rb_define_singleton_ವಿಧಾನ(c_ಬಫರ್, "count", (ಶೂನ್ಯ*)ಬಫರ್_s_ಎಣಿಕೆ, 0);
    rb_define_singleton_ವಿಧಾನ(c_ಬಫರ್, "[]", (ಶೂನ್ಯ*)ಬಫರ್_s_aref, 1);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "name", (ಶೂನ್ಯ*)ಬಫರ್_name, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "number", (ಶೂನ್ಯ*)ಬಫರ್_ಸಂಖ್ಯೆ, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "count", (ಶೂನ್ಯ*)ಬಫರ್_ಎಣಿಕೆ, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "length", (ಶೂನ್ಯ*)ಬಫರ್_ಎಣಿಕೆ, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "[]", (ಶೂನ್ಯ*)ಬಫರ್_aref, 1);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "[]=", (ಶೂನ್ಯ*)ಬಫರ್_aset, 2);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "delete", (ಶೂನ್ಯ*)ಬಫರ್_delete, 1);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "append", (ಶೂನ್ಯ*)ಬಫರ್_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "line_number", (ಶೂನ್ಯ*)current_ಸಾಲು_ಸಂಖ್ಯೆ, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "line", (ಶೂನ್ಯ*)ಸಾಲು_s_current, 0);
    rb_define_ವಿಧಾನ(c_ಬಫರ್, "line=", (ಶೂನ್ಯ*)ಗಣ_current_ಸಾಲು, 1);


    c_Vim_ಕಿಟಕಿ = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "current", (ಶೂನ್ಯ*)ಕಿಟಕಿ_s_current, 0);
    rb_define_singleton_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "count", (ಶೂನ್ಯ*)ಕಿಟಕಿ_s_ಎಣಿಕೆ, 0);
    rb_define_singleton_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "[]", (ಶೂನ್ಯ*)ಕಿಟಕಿ_s_aref, 1);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "buffer", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಬಫರ್, 0);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "height", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಎತ್ತರ, 0);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "height=", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಗಣ_ಎತ್ತರ, 1);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "width", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಅಗಲ, 0);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "width=", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಗಣ_ಅಗಲ, 1);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "cursor", (ಶೂನ್ಯ*)ಕಿಟಕಿ_cursor, 0);
    rb_define_ವಿಧಾನ(c_Vim_ಕಿಟಕಿ, "cursor=", (ಶೂನ್ಯ*)ಕಿಟಕಿ_ಗಣ_cursor, 1);

    rb_define_virtual_variable("$curbuf", ಬಫರ್_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", ಕಿಟಕಿ_s_current_getter, 0);
}

    ಶೂನ್ಯ
vim_ruby_init(ಶೂನ್ಯ *ರಾಶಿ_ಆರಂಭ)
{
    // should get machine stack start address early in main function
    ruby_ರಾಶಿ_ಆರಂಭ = ರಾಶಿ_ಆರಂಭ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
convert_hash2dict(ಮೌಲ್ಯ_3 ಕೀಲಿ, ಮೌಲ್ಯ_3 val, ಮೌಲ್ಯ_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ಕೀಲಿ)));
    ಆದರೆ (di == ಖಾಲಿ || ruby_convert_ಗಮ್ಯ_vim_ಮೌಲ್ಯ(val, &di->di_tv) != OK
						     || dict_ಕೂಡಿಸು(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ಹಿಂತಿರುಗಿ ST_STOP;
    }
    ಹಿಂತಿರುಗಿ ST_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ruby_convert_ಗಮ್ಯ_vim_ಮೌಲ್ಯ(ಮೌಲ್ಯ_3 val, typval_T *rettv)
{
    ಆಯ್ಕೆ (TYPE(val))
    {
	ಸ್ಥಿತಿ T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_BIGNUM:
	ಸ್ಥಿತಿ T_FIXNUM:
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)NUM2LONG(val);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	ಸ್ಥಿತಿ T_STRING:
	    {
		ಮೌಲ್ಯ_3 str = (ಮೌಲ್ಯ_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_ARRAY:
	    {
		list_T *l;
		ದೀರ್ಘ i;
		typval_T v;

		l = list_alloc();
		ಆದರೆ (l == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		ಗಾಗಿ (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ಆದರೆ (ruby_convert_ಗಮ್ಯ_vim_ಮೌಲ್ಯ((ಮೌಲ್ಯ_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		ಆದರೆ (d == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (ಮೌಲ್ಯ_3)d);
		ಆದರೆ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
do_rubyeval(char_u *str, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ retval = FAIL;

    ಆದರೆ (ensure_ruby_initialized())
    {
	ಪೂರ್ಣಾಂಕ state;
	ಮೌಲ್ಯ_3 obj;

	obj = rb_eval_string_protect((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str, &state);
	ಆದರೆ (state)
	    error_print(state);
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = ruby_convert_ಗಮ್ಯ_vim_ಮೌಲ್ಯ(obj, rettv);
    }
    ಆದರೆ (retval == FAIL)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}
