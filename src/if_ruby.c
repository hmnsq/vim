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
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN ಬಾಹ್ಯ
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# ಯದಿ RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ಯದಿ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ಯದಿ RUBY_VERSION >= 20
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

# ಯದಿ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ಯದಿ RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ಯದಿ RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ಯದಿ RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ಯದಿ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ಯದಿ RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# ಯದಿ RUBY_VERSION >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#endif  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#ಯದಿ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ತಳ್ಳು
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ಯದಿ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#ಅನ್ತರ್ಭಾವ <ruby.h>
#ಅನ್ತರ್ಭಾವ <ruby/encoding.h>

#ಯದಿ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ತೆಗೆ
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ಯದಿ defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#endif

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#ಯದಿ defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_ಮೌಲ್ಯ_Ptr
# define String_ಮೌಲ್ಯ_Ptr(s) STR2CSTR(s)
#endif
#ifndef RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#endif
#ifndef RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#endif
#ifndef RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#endif
#ifndef RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#endif

#ifdef HAVE_DUP
# undef HAVE_DUP
#endif
#ifdef HAVE_FSYNC
# undef HAVE_FSYNC
#endif

// Avoid redefining TRUE/FALSE in vterm.h.
#ifdef TRUE
# undef TRUE
#endif
#ifdef FALSE
# undef FALSE
#endif

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define ಚಿಹ್ನೆ_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ಅನ್ಯಥಾ
#  include <dlfcn.h>
#  define HINSTANCE ಶೂನ್ಯ*
#  define RUBY_PROC ಶೂನ್ಯ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ಚಿಹ್ನೆ_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ruby_initialized = 0;
ಸ್ಥಿರ ಶೂನ್ಯ *ruby_ರಾಶಿ_start;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 objtbl;

ಸ್ಥಿರ ಮೌಲ್ಯ_3 mVIM;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 cBuffer;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 cVimWindow;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 eDeletedBufferError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 eDeletedWindowError;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ensure_ruby_initialized(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ error_print(ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ ruby_io_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ruby_vim_init(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ruby_convert_to_vim_ಮೌಲ್ಯ(ಮೌಲ್ಯ_3 val, typval_T *rettv);

#ಯದಿ defined(__ia64) && !defined(ruby_init_ರಾಶಿ)
# define ruby_init_ರಾಶಿ(addr) ruby_init_ರಾಶಿ((addr), rb_ia64_bsp())
#endif

#ಯದಿ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ಯದಿ RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ಯದಿ RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  ಯದಿ RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  ಅನ್ಯಥಾ
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# ಅನ್ಯಥಾ
#  define rb_data_object_alloc		dll_rb_data_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_method		dll_rb_define_method
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_method	dll_rb_define_singleton_method
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_eIndexError			(*dll_rb_eIndexError)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# ಯದಿ RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# ಅನ್ಯಥಾ
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

# ಯದಿ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ಯದಿ RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# ಯದಿ RUBY_VERSION < 20
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
# ಅನ್ಯಥಾ
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
# ಅನ್ಯಥಾ
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
# define rb_enc_find_index		dll_rb_enc_find_index
# define rb_enc_find			dll_rb_enc_find
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
# ಯದಿ defined(USE_RUBY_INTEGER)
ಮೌಲ್ಯ_3 *dll_rb_cInteger;
# endif
# ಯದಿ RUBY_VERSION >= 20
ಮೌಲ್ಯ_3 *dll_rb_cFloat;
# endif
ಮೌಲ್ಯ_3 *dll_rb_cNilClass;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_cObject;
ಮೌಲ್ಯ_3 *dll_rb_cString;
ಮೌಲ್ಯ_3 *dll_rb_c_ಚಿಹ್ನೆ;
ಮೌಲ್ಯ_3 *dll_rb_cTrueClass;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_class_new_instance) (ಪೂರ್ಣಾಙ್ಕ,ಮೌಲ್ಯ_3*,ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_check_type) (ಮೌಲ್ಯ_3,ಪೂರ್ಣಾಙ್ಕ);
# ifdef USE_TYPEDDATA
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_rb_check_typeddata) (ಮೌಲ್ಯ_3,ಧ್ರುವ rb_data_type_t *);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_class_path) (ಮೌಲ್ಯ_3);
# ifdef USE_TYPEDDATA
#  ಯದಿ RUBY_VERSION >= 23
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_data_typed_object_wrap) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, ಧ್ರುವ rb_data_type_t *);
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_data_typed_object_alloc) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, ಧ್ರುವ rb_data_type_t *);
#  endif
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_data_object_alloc) (ಮೌಲ್ಯ_3, ಶೂನ್ಯ*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# ಯದಿ RUBY_VERSION >= 31
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_debug_rstring_null_ptr) (ಧ್ರುವ ವರ್ಣ*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_define_class_under) (ಮೌಲ್ಯ_3, ಧ್ರುವ ವರ್ಣ*, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_const) (ಮೌಲ್ಯ_3,ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_global_function) (ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3(*)(ಪೂರ್ಣಾಙ್ಕ,ಮೌಲ್ಯ_3*,ಮೌಲ್ಯ_3),ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_method) (ಮೌಲ್ಯ_3,ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3(*)(ANYARGS),ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_define_module) (ಧ್ರುವ ವರ್ಣ*);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_module_function) (ಮೌಲ್ಯ_3,ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3(*)(ಮೌಲ್ಯ_3,ಮೌಲ್ಯ_3),ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_singleton_method) (ಮೌಲ್ಯ_3,ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3(*)(ANYARGS),ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_define_virtual_variable) (ಧ್ರುವ ವರ್ಣ*,ಮೌಲ್ಯ_3(*)(ID,ಮೌಲ್ಯ_3*),ಶೂನ್ಯ(*)());
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_stdout;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_stderr;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eArgError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eIndexError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eRuntimeError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 *dll_rb_eStandardError;
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_eval_string_protect) (ಧ್ರುವ ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ*);
# ಯದಿ RUBY_VERSION >= 21
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_funcallv) (ಮೌಲ್ಯ_3, ID, ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಮೌಲ್ಯ_3*);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_funcall2) (ಮೌಲ್ಯ_3, ID, ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಮೌಲ್ಯ_3*);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_global_variable) (ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_aset) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_foreach) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಙ್ಕ (*)(ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3), ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_hash_new) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_inspect) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_int2inum) (long);
ಸ್ಥಿರ ID (*dll_rb_intern) (ಧ್ರುವ ವರ್ಣ*);
# ಯದಿ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ಸ್ಥಿರ long (*dll_rb_fix2int) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ long (*dll_rb_num2int) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ unsigned long (*dll_rb_num2uint) (ಮೌಲ್ಯ_3);
# endif
ಸ್ಥಿರ ದ್ವಿಗುಣ (*dll_rb_num2dbl) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_lastline_get) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_lastline_set) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_protect) (ಮೌಲ್ಯ_3 (*)(ಮೌಲ್ಯ_3), ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಙ್ಕ*);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_load) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ long (*dll_rb_num2long) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ unsigned long (*dll_rb_num2ulong) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_alloc) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_as_string) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_obj_id) (ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_raise) (ಮೌಲ್ಯ_3, ಧ್ರುವ ವರ್ಣ*, ...);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_string_ಮೌಲ್ಯ) (ಚಞ್ಚಲ ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_cat) (ಮೌಲ್ಯ_3, ಧ್ರುವ ವರ್ಣ*, long);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_concat) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new) (ಧ್ರುವ ವರ್ಣ*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new_cstr) (ಧ್ರುವ ವರ್ಣ*);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_str_new2) (ಧ್ರುವ ವರ್ಣ*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_errinfo) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_loadpath) (ಶೂನ್ಯ);
# ifdef MSWIN
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_sysinit) (ಪೂರ್ಣಾಙ್ಕ*, ವರ್ಣ***);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_rb_w32_snprintf)(ವರ್ಣ*, size_t, ಧ್ರುವ ವರ್ಣ*, ...);
# endif
# ಯದಿ RUBY_VERSION >= 31
#  ifdef _MSC_VER
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_unexpected_type) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಙ್ಕ);
#  ಅನ್ಯಥಾ
NORETURN(ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_unexpected_type) (ಮೌಲ್ಯ_3, ಪೂರ್ಣಾಙ್ಕ));
#  endif
# endif
ಸ್ಥಿರ ವರ್ಣ * (*dll_rb_string_ಮೌಲ್ಯ_ptr) (ಚಞ್ಚಲ ಮೌಲ್ಯ_3*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_float_new) (ದ್ವಿಗುಣ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_new) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_new4) (long n, ಧ್ರುವ ಮೌಲ್ಯ_3 *elts);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_ary_ತಳ್ಳು) (ಮೌಲ್ಯ_3, ಮೌಲ್ಯ_3);
# ಯದಿ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_ary_detransient) (ಮೌಲ್ಯ_3);
# endif
# ifdef __ia64
ಸ್ಥಿರ ಶೂನ್ಯ * (*dll_rb_ia64_bsp) (ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_ರಾಶಿ)(ಮೌಲ್ಯ_3*, ಶೂನ್ಯ*);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_init_ರಾಶಿ)(ಮೌಲ್ಯ_3*);
# endif
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_int2big)(SIGNED_ಮೌಲ್ಯ);

ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_script) (ಧ್ರುವ ವರ್ಣ*);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_rb_enc_find_index) (ಧ್ರುವ ವರ್ಣ*);
ಸ್ಥಿರ rb_encoding* (*dll_rb_enc_find) (ಧ್ರುವ ವರ್ಣ*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_enc_str_new) (ಧ್ರುವ ವರ್ಣ*, long, rb_encoding*);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_sprintf) (ಧ್ರುವ ವರ್ಣ*, ...);
ಸ್ಥಿರ ಮೌಲ್ಯ_3 (*dll_rb_require) (ಧ್ರುವ ವರ್ಣ*);
ಸ್ಥಿರ ಶೂನ್ಯ* (*dll_ruby_options)(ಪೂರ್ಣಾಙ್ಕ, ವರ್ಣ**);

# ಯದಿ defined(USE_RGENGC) && USE_RGENGC
#  ಯದಿ RUBY_VERSION == 21
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_gc_writebarrier_unprotect_promoted)(ಮೌಲ್ಯ_3);
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_rb_gc_writebarrier_unprotect)(ಮೌಲ್ಯ_3 obj);
#  endif
# endif

# ಯದಿ RUBY_VERSION >= 30
#  ifdef _MSC_VER
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  ಅನ್ಯಥಾ
NORETURN(ಸ್ಥಿರ ಶೂನ್ಯ (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# ಯದಿ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ಶೂನ್ಯ rb_ary_detransient_stub(ಮೌಲ್ಯ_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ಯದಿ RUBY_VERSION >= 22
    long
rb_num2long_stub(ಮೌಲ್ಯ_3 x)
#  ಅನ್ಯಥಾ
    SIGNED_ಮೌಲ್ಯ
rb_num2long_stub(ಮೌಲ್ಯ_3 x)
#  endif
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_num2long(x);
}
#  ಯದಿ RUBY_VERSION >= 26
    ಮೌಲ್ಯ_3
rb_int2big_stub(intptr_t x)
#  ಅನ್ಯಥಾ
    ಮೌಲ್ಯ_3
rb_int2big_stub(SIGNED_ಮೌಲ್ಯ x)
#  endif
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_int2big(x);
}
#  ಯದಿ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(ಮೌಲ್ಯ_3 x)
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_fix2int(x);
}
    long
rb_num2int_stub(ಮೌಲ್ಯ_3 x)
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_num2int(x);
}
#  endif
#  ಯದಿ RUBY_VERSION >= 20
    ಮೌಲ್ಯ_3
rb_float_new_in_heap(ದ್ವಿಗುಣ d)
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_float_new(d);
}
#   ಯದಿ RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(ಮೌಲ್ಯ_3 x)
#   ಅನ್ಯಥಾ
    ಮೌಲ್ಯ_3
rb_num2ulong(ಮೌಲ್ಯ_3 x)
#   endif
{
    ಪ್ರತ್ಯಾಗಮ (long)RSHIFT((SIGNED_ಮೌಲ್ಯ)(x),1);
}
#  endif
#  ಯದಿ defined(USE_RGENGC) && USE_RGENGC
#   ಯದಿ RUBY_VERSION == 21
    ಶೂನ್ಯ
rb_gc_writebarrier_unprotect_promoted_stub(ಮೌಲ್ಯ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ಅನ್ಯಥಾ
    ಶೂನ್ಯ
rb_gc_writebarrier_unprotect_stub(ಮೌಲ್ಯ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ಯದಿ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    ಶೂನ್ಯ
rb_ary_detransient_stub(ಮೌಲ್ಯ_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ಯದಿ RUBY_VERSION >= 30
    ಶೂನ್ಯ
rb_check_type_stub(ಮೌಲ್ಯ_3 obj, ಪೂರ್ಣಾಙ್ಕ t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(ಮೌಲ್ಯ_3 x)
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_num2uint(x);
}
    ಶೂನ್ಯ
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  ಯದಿ RUBY_VERSION >= 31
    ಶೂನ್ಯ
rb_debug_rstring_null_ptr_stub(ಧ್ರುವ ವರ್ಣ *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    ಶೂನ್ಯ
rb_unexpected_type_stub(ಮೌಲ್ಯ_3 self, ಪೂರ್ಣಾಙ್ಕ t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
ಶೂನ್ಯ *rb_check_typeddata_stub(ಮೌಲ್ಯ_3 obj, ಧ್ರುವ rb_data_type_t *data_type)
{
    ಪ್ರತ್ಯಾಗಮ dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

ಸ್ಥಿರ HINSTANCE hinstRuby = ರಿಕ್ತ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ಸ್ಥಿರ ಸಂರಚನಾ
{
    ವರ್ಣ *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# ಯದಿ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ಅನ್ಯಥಾ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ಯದಿ RUBY_VERSION >= 20
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
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  ಯದಿ RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  ಅನ್ಯಥಾ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# ಅನ್ಯಥಾ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# ಯದಿ RUBY_VERSION >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_method},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_method},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_eIndexError},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# ಯದಿ RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ಅನ್ಯಥಾ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ಯದಿ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_set},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_load},
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
# ಅನ್ಯಥಾ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ಯದಿ RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_ಮೌಲ್ಯ_ptr},
# ಯದಿ RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ಅನ್ಯಥಾ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ಅನ್ಯಥಾ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ತಳ್ಳು},
# ಯದಿ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_find_index},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_find},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ರಾಶಿ},
# ಯದಿ defined(USE_RGENGC) && USE_RGENGC
#  ಯದಿ RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ಅನ್ಯಥಾ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ಯದಿ RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", ರಿಕ್ತ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ruby_runtime_link_init(ವರ್ಣ *libname, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (hinstRuby)
	ಪ್ರತ್ಯಾಗಮ OK;
    hinstRuby = load_dll(libname);
    ಯದಿ (!hinstRuby)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	ಯದಿ (!(*ruby_funcname_table[i].ptr = ಚಿಹ್ನೆ_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ರಿಕ್ತ;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
ruby_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪ್ರತ್ಯಾಗಮ ruby_runtime_link_init((ವರ್ಣ *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    ಶೂನ್ಯ
ruby_end(ಶೂನ್ಯ)
{
}

    ಶೂನ್ಯ
ex_ruby(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ state;
    ವರ್ಣ *script = ರಿಕ್ತ;

    script = (ವರ್ಣ *)script_get(eap, eap->arg);
    ಯದಿ (!eap->skip && ensure_ruby_initialized())
    {
	ಯದಿ (script == ರಿಕ್ತ)
	    rb_eval_string_protect((ವರ್ಣ *)eap->arg, &state);
	ಅನ್ಯಥಾ
	    rb_eval_string_protect(script, &state);
	ಯದಿ (state)
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
vim_str2rb_enc_str(ಧ್ರುವ ವರ್ಣ *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    ಯದಿ (get_option_ಮೌಲ್ಯ((char_u *)"enc", &lval, &sval, ರಿಕ್ತ, 0) == gov_string)
    {
	enc = rb_enc_find((ವರ್ಣ *)sval);
	vim_free(sval);
	ಯದಿ (enc)
	    ಪ್ರತ್ಯಾಗಮ rb_enc_str_new(s, (long)strlen(s), enc);
    }
    ಪ್ರತ್ಯಾಗಮ rb_str_new2(s);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
eval_enc_string_protect(ಧ್ರುವ ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    ಮೌಲ್ಯ_3 v;

    ಯದಿ (get_option_ಮೌಲ್ಯ((char_u *)"enc", &lval, &sval, ರಿಕ್ತ, 0) == gov_string)
    {
	enc = rb_enc_find((ವರ್ಣ *)sval);
	vim_free(sval);
	ಯದಿ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ಪ್ರತ್ಯಾಗಮ rb_eval_string_protect(String_ಮೌಲ್ಯ_Ptr(v), state);
	}
    }
    ಪ್ರತ್ಯಾಗಮ rb_eval_string_protect(str, state);
}

    ಶೂನ್ಯ
ex_rubydo(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ಯದಿ (!ensure_ruby_initialized())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (i = eap->line1; i <= eap->line2; i++)
    {
	ಮೌಲ್ಯ_3 ಸಾಲು;

	ಯದಿ (i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	ಸಾಲು = vim_str2rb_enc_str((ವರ್ಣ *)ml_get(i));
	rb_lastline_set(ಸಾಲು);
	eval_enc_string_protect((ವರ್ಣ *) eap->arg, &state);
	ಯದಿ (state)
	{
	    error_print(state);
	    ಭಙ್ಗ;
	}
	ಯದಿ (was_curbuf != curbuf || i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	ಸಾಲು = rb_lastline_get();
	ಯದಿ (!NIL_P(ಸಾಲು))
	{
	    ಯದಿ (TYPE(ಸಾಲು) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ml_replace(i, (char_u *) String_ಮೌಲ್ಯ_Ptr(ಸಾಲು), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
rb_load_wrap(ಮೌಲ್ಯ_3 file_to_load)
{
    rb_load(file_to_load, 0);
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಶೂನ್ಯ
ex_rubyfile(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ state;

    ಯದಿ (!ensure_ruby_initialized())
	ಪ್ರತ್ಯಾಗಮ;

    ಮೌಲ್ಯ_3 file_to_load = rb_str_new2((ಧ್ರುವ ವರ್ಣ *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    ಯದಿ (state)
	error_print(state);
}

    ಶೂನ್ಯ
ruby_buffer_free(buf_T *buf)
{
    ಯದಿ (buf->b_ruby_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    rb_hash_aset(objtbl, rb_obj_id((ಮೌಲ್ಯ_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
ruby_window_free(win_T *win)
{
    ಯದಿ (win->w_ruby_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    rb_hash_aset(objtbl, rb_obj_id((ಮೌಲ್ಯ_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ensure_ruby_initialized(ಶೂನ್ಯ)
{
    ಯದಿ (ruby_initialized)
	ಪ್ರತ್ಯಾಗಮ ruby_initialized;

#ifdef DYNAMIC_RUBY
    ಯದಿ (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	ಪೂರ್ಣಾಙ್ಕ argc = 1;
	ವರ್ಣ *argv[] = {"gvim.exe"};
	ವರ್ಣ **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_ರಾಶಿ(ruby_ರಾಶಿ_start);
	    ruby_init();
	}
	{
	    ಪೂರ್ಣಾಙ್ಕ dummy_argc = 2;
	    ವರ್ಣ *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    ಅನ್ಯಥಾ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ಪ್ರತ್ಯಾಗಮ 0;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ ruby_initialized;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
error_print(ಪೂರ್ಣಾಙ್ಕ state)
{
    ಮೌಲ್ಯ_3 error;
    ಮೌಲ್ಯ_3 eclass;
    ಮೌಲ್ಯ_3 einfo;
    ಮೌಲ್ಯ_3 bt;
    ಪೂರ್ಣಾಙ್ಕ attr;
    ವರ್ಣ buff[BUFSIZ];
    long i;

#define TAG_RETURN	0x1
#define TAG_BREAK	0x2
#define TAG_NEXT	0x3
#define TAG_RETRY	0x4
#define TAG_REDO	0x5
#define TAG_RAISE	0x6
#define TAG_THROW	0x7
#define TAG_FATAL	0x8
#define TAG_MASK	0xf

    ವಿಕಲ್ಪ (state)
    {
	ಪಕ್ಷ TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    ಭಙ್ಗ;
	ಪಕ್ಷ TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    ಭಙ್ಗ;
	ಪಕ್ಷ TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    ಭಙ್ಗ;
	ಪಕ್ಷ TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    ಭಙ್ಗ;
	ಪಕ್ಷ TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    ಭಙ್ಗ;
	ಪಕ್ಷ TAG_RAISE:
	ಪಕ್ಷ TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    ಯದಿ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ಅನ್ಯಥಾ
	    {
		ಮೌಲ್ಯ_3 epath;
		ವರ್ಣ *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ಯದಿ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ಯದಿ RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    ಪ್ರತಿ (i = 0; i < RARRAY_LEN(bt); i++)
		ಸಂದೇಶ_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ಅನ್ಯಥಾ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    ಪ್ರತಿ (i = 0; i < RARRAY_LEN(bt); i++)
		ಸಂದೇಶ_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_message(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
    ವರ್ಣ *buff, *p;

    str = rb_obj_as_string(str);
    ಯದಿ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(ವರ್ಣ, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ಯದಿ (p) *p = '\0';
	ಸಂದೇಶ(buff);
    }
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ("");
    }
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_set_option(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
    do_set((char_u *)String_ಮೌಲ್ಯ_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_command(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
    do_cmdline_cmd((char_u *)String_ಮೌಲ್ಯ_Ptr(str));
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

#ifdef FEAT_EVAL
    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_to_ruby(typval_T *tv)
{
    ಮೌಲ್ಯ_3 result = Qnil;

    ಯದಿ (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == ರಿಕ್ತ
					  ? "" : (ವರ್ಣ *)(tv->vval.v_string));
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	result = INT2_ಅಂಕೆ(tv->vval.v_ಸಂಖ್ಯೆ);
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	ಯದಿ (list != ರಿಕ್ತ)
	{
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, curr)
		rb_ary_ತಳ್ಳು(result, vim_to_ruby(&curr->li_tv));
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	ಯದಿ (tv->vval.v_dict != ರಿಕ್ತ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	    {
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((ವರ್ಣ *)hi->hi_ಕೀಲಿ),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ಯದಿ (tv->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE)
	    result = Qtrue;
	ಅನ್ಯಥಾ ಯದಿ (tv->vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE)
	    result = Qfalse;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ಪ್ರತ್ಯಾಗಮ result;
}
#endif

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_evaluate(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    ಮೌಲ್ಯ_3       result;

    tv = eval_ಅಭಿವ್ಯಕ್ತಿ((char_u *)String_ಮೌಲ್ಯ_Ptr(str), ರಿಕ್ತ);
    ಯದಿ (tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ Qnil;
#endif
}

#ifdef USE_TYPEDDATA
ಸ್ಥಿರ size_t buffer_dsize(ಧ್ರುವ ಶೂನ್ಯ *buf);

ಸ್ಥಿರ ಧ್ರುವ rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# ಯದಿ RUBY_VERSION >= 27
	0, {0}
# ಅನ್ಯಥಾ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ಸ್ಥಿರ size_t
buffer_dsize(ಧ್ರುವ ಶೂನ್ಯ *buf UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(buf_T);
}
#endif

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_new(buf_T *buf)
{
    ಯದಿ (buf->b_ruby_ref)
    {
	ಪ್ರತ್ಯಾಗಮ (ಮೌಲ್ಯ_3) buf->b_ruby_ref;
    }
    ಅನ್ಯಥಾ
    {
#ifdef USE_TYPEDDATA
	ಮೌಲ್ಯ_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#ಅನ್ಯಥಾ
	ಮೌಲ್ಯ_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (ಶೂನ್ಯ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ಪ್ರತ್ಯಾಗಮ obj;
    }
}

    ಸ್ಥಿರ buf_T *
get_buf(ಮೌಲ್ಯ_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#ಅನ್ಯಥಾ
    Data_Get_Struct(obj, buf_T, buf);
#endif
    ಯದಿ (buf == ರಿಕ್ತ)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
vim_blob(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
    ಮೌಲ್ಯ_3 result = rb_str_new("0z", 2);
    ವರ್ಣ    buf[4];
    ಪೂರ್ಣಾಙ್ಕ	i;
    ಪ್ರತಿ (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned ವರ್ಣ)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_s_current(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ buffer_new(curbuf);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_s_current_getter(ID id UNUSED, ಮೌಲ್ಯ_3 *x UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ buffer_new(curbuf);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_s_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self UNUSED)
{
    buf_T *b;
    ಪೂರ್ಣಾಙ್ಕ n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ಯದಿ (b->b_p_bl)
	    n++;
    }

    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(n);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_s_aref(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T *b;
    ಪೂರ್ಣಾಙ್ಕ n = NUM2INT(ಅಂಕೆ);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ಯದಿ (!b->b_p_bl)
	    ಅನುವರ್ತ;

	ಯದಿ (n == 0)
	    ಪ್ರತ್ಯಾಗಮ buffer_new(b);

	n--;
    }
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_name(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಪ್ರತ್ಯಾಗಮ buf->b_ffname ? rb_str_new2((ವರ್ಣ *)buf->b_ffname) : Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_ಸಂಖ್ಯೆ(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(buf->b_fnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self)
{
    buf_T *buf = get_buf(self);

    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
get_buffer_ಸಾಲು(buf_T *buf, linenr_T n)
{
    ಯದಿ (n <= 0 || n > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    ಪ್ರತ್ಯಾಗಮ vim_str2rb_enc_str((ವರ್ಣ *)ml_get_buf(buf, n, FALSE));
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_aref(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T *buf = get_buf(self);

    ಯದಿ (buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ get_buffer_ಸಾಲು(buf, (linenr_T)NUM2LONG(ಅಂಕೆ));
    ಪ್ರತ್ಯಾಗಮ Qnil; // For stop warning
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
set_buffer_ಸಾಲು(buf_T *buf, linenr_T n, ಮೌಲ್ಯ_3 str)
{
    ವರ್ಣ	*ಸಾಲು = String_ಮೌಲ್ಯ_Ptr(str);
    aco_save_T	aco;

    ಯದಿ (n > 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && ಸಾಲು != ರಿಕ್ತ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಯದಿ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)ಸಾಲು, TRUE);
		changed();
#ifdef SYNTAX_HL
		syn_changed(n); // recompute syntax hl. for this line
#endif
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಅನ್ಯಥಾ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    ಪ್ರತ್ಯಾಗಮ str;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_aset(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ, ಮೌಲ್ಯ_3 str)
{
    buf_T *buf = get_buf(self);

    ಯದಿ (buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ set_buffer_ಸಾಲು(buf, (linenr_T)NUM2LONG(ಅಂಕೆ), str);
    ಪ್ರತ್ಯಾಗಮ str;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_delete(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(ಅಂಕೆ);
    aco_save_T	aco;

    ಯದಿ (n > 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಯದಿ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_ಸಾಲುಗಳು_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಅನ್ಯಥಾ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
buffer_append(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 ಅಂಕೆ, ಮೌಲ್ಯ_3 str)
{
    buf_T	*buf = get_buf(self);
    ವರ್ಣ	*ಸಾಲು = String_ಮೌಲ್ಯ_Ptr(str);
    long	n = NUM2LONG(ಅಂಕೆ);
    aco_save_T	aco;

    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    ಅನ್ಯಥಾ ಯದಿ (n >= 0 && n <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಯದಿ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) ಸಾಲು, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_ಸಾಲುಗಳು_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ಅನ್ಯಥಾ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ಪ್ರತ್ಯಾಗಮ str;
}

#ifdef USE_TYPEDDATA
ಸ್ಥಿರ size_t window_dsize(ಧ್ರುವ ಶೂನ್ಯ *buf);

ಸ್ಥಿರ ಧ್ರುವ rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# ಯದಿ RUBY_VERSION >= 27
	0, {0}
# ಅನ್ಯಥಾ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ಸ್ಥಿರ size_t
window_dsize(ಧ್ರುವ ಶೂನ್ಯ *win UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(win_T);
}
#endif

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_new(win_T *win)
{
    ಯದಿ (win->w_ruby_ref)
    {
	ಪ್ರತ್ಯಾಗಮ (ಮೌಲ್ಯ_3) win->w_ruby_ref;
    }
    ಅನ್ಯಥಾ
    {
#ifdef USE_TYPEDDATA
	ಮೌಲ್ಯ_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#ಅನ್ಯಥಾ
	ಮೌಲ್ಯ_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (ಶೂನ್ಯ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ಪ್ರತ್ಯಾಗಮ obj;
    }
}

    ಸ್ಥಿರ win_T *
get_win(ಮೌಲ್ಯ_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#ಅನ್ಯಥಾ
    Data_Get_Struct(obj, win_T, win);
#endif
    ಯದಿ (win == ರಿಕ್ತ)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    ಪ್ರತ್ಯಾಗಮ win;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_s_current(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ window_new(curwin);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_s_current_getter(ID id UNUSED, ಮೌಲ್ಯ_3 *x UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ಸ್ಥಿರ ಮೌಲ್ಯ_3
ಸಾಲು_s_current(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ get_buffer_ಸಾಲು(curbuf, curwin->w_cursor.lnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
set_current_ಸಾಲು(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 str)
{
    ಪ್ರತ್ಯಾಗಮ set_buffer_ಸಾಲು(curbuf, curwin->w_cursor.lnum, str);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
current_ಸಾಲು_ಸಂಖ್ಯೆ(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ INT2FIX((ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.lnum);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_s_ಎಣಿಕೆ(ಮೌಲ್ಯ_3 self UNUSED)
{
    win_T	*w;
    ಪೂರ್ಣಾಙ್ಕ n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(n);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_s_aref(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 ಅಂಕೆ)
{
    win_T *w;
    ಪೂರ್ಣಾಙ್ಕ n = NUM2INT(ಅಂಕೆ);

    ಪ್ರತಿ (w = firstwin; w != ರಿಕ್ತ; w = w->w_next, --n)
	ಯದಿ (n == 0)
	    ಪ್ರತ್ಯಾಗಮ window_new(w);
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_buffer(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಪ್ರತ್ಯಾಗಮ buffer_new(win->w_buffer);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_height(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(win->w_height);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_set_height(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    ಪ್ರತ್ಯಾಗಮ height;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_width(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ INT2_ಅಂಕೆ(get_win(self)->w_width);
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_set_width(ಮೌಲ್ಯ_3 self UNUSED, ಮೌಲ್ಯ_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    ಪ್ರತ್ಯಾಗಮ width;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_cursor(ಮೌಲ್ಯ_3 self)
{
    win_T *win = get_win(self);

    ಪ್ರತ್ಯಾಗಮ rb_assoc_new(INT2_ಅಂಕೆ(win->w_cursor.lnum), INT2_ಅಂಕೆ(win->w_cursor.col));
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
window_set_cursor(ಮೌಲ್ಯ_3 self, ಮೌಲ್ಯ_3 pos)
{
    ಮೌಲ್ಯ_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ಯದಿ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
f_nop(ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ Qnil;
}

    ಸ್ಥಿರ ಮೌಲ್ಯ_3
f_p(ಪೂರ್ಣಾಙ್ಕ argc, ಮೌಲ್ಯ_3 *argv, ಮೌಲ್ಯ_3 self UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಮೌಲ್ಯ_3 str = rb_str_new("", 0);
    ಮೌಲ್ಯ_3 ret = Qnil;

    ಪ್ರತಿ (i = 0; i < argc; i++)
    {
	ಯದಿ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    ಸಂದೇಶ(RSTRING_PTR(str));

    ಯದಿ (argc == 1)
	ret = argv[0];
    ಅನ್ಯಥಾ ಯದಿ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ruby_io_init(ಶೂನ್ಯ)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN ಮೌಲ್ಯ_3 rb_stdout;
    RUBYEXTERN ಮೌಲ್ಯ_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (ಶೂನ್ಯ*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (ಶೂನ್ಯ*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (ಶೂನ್ಯ*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (ಶೂನ್ಯ*)f_nop, 0);
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
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_ಅಂಕೆ(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_ಅಂಕೆ(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_ಅಂಕೆ(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_ಅಂಕೆ(VIM_VERSION_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_VERSION_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_VERSION_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_VERSION_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_VERSION_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_message, 1);
    rb_define_module_function(mVIM, "set_option", vim_set_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    eDeletedBufferError = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    eDeletedWindowError = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    cBuffer = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_method(cBuffer, "current", (ಶೂನ್ಯ*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (ಶೂನ್ಯ*)buffer_s_ಎಣಿಕೆ, 0);
    rb_define_singleton_method(cBuffer, "[]", (ಶೂನ್ಯ*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (ಶೂನ್ಯ*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (ಶೂನ್ಯ*)buffer_ಸಂಖ್ಯೆ, 0);
    rb_define_method(cBuffer, "count", (ಶೂನ್ಯ*)buffer_ಎಣಿಕೆ, 0);
    rb_define_method(cBuffer, "length", (ಶೂನ್ಯ*)buffer_ಎಣಿಕೆ, 0);
    rb_define_method(cBuffer, "[]", (ಶೂನ್ಯ*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (ಶೂನ್ಯ*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (ಶೂನ್ಯ*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (ಶೂನ್ಯ*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (ಶೂನ್ಯ*)current_ಸಾಲು_ಸಂಖ್ಯೆ, 0);
    rb_define_method(cBuffer, "line", (ಶೂನ್ಯ*)ಸಾಲು_s_current, 0);
    rb_define_method(cBuffer, "line=", (ಶೂನ್ಯ*)set_current_ಸಾಲು, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (ಶೂನ್ಯ*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (ಶೂನ್ಯ*)window_s_ಎಣಿಕೆ, 0);
    rb_define_singleton_method(cVimWindow, "[]", (ಶೂನ್ಯ*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (ಶೂನ್ಯ*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (ಶೂನ್ಯ*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (ಶೂನ್ಯ*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (ಶೂನ್ಯ*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (ಶೂನ್ಯ*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (ಶೂನ್ಯ*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (ಶೂನ್ಯ*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    ಶೂನ್ಯ
vim_ruby_init(ಶೂನ್ಯ *ರಾಶಿ_start)
{
    // should get machine stack start address early in main function
    ruby_ರಾಶಿ_start = ರಾಶಿ_start;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
convert_hash2dict(ಮೌಲ್ಯ_3 ಕೀಲಿ, ಮೌಲ್ಯ_3 val, ಮೌಲ್ಯ_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ಕೀಲಿ)));
    ಯದಿ (di == ರಿಕ್ತ || ruby_convert_to_vim_ಮೌಲ್ಯ(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ಪ್ರತ್ಯಾಗಮ ST_STOP;
    }
    ಪ್ರತ್ಯಾಗಮ ST_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ruby_convert_to_vim_ಮೌಲ್ಯ(ಮೌಲ್ಯ_3 val, typval_T *rettv)
{
    ವಿಕಲ್ಪ (TYPE(val))
    {
	ಪಕ್ಷ T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_BIGNUM:
	ಪಕ್ಷ T_FIXNUM:
	    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)NUM2LONG(val);
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	ಪಕ್ಷ T_STRING:
	    {
		ಮೌಲ್ಯ_3 str = (ಮೌಲ್ಯ_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		ಯದಿ (l == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		ಪ್ರತಿ (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ಯದಿ (ruby_convert_to_vim_ಮೌಲ್ಯ((ಮೌಲ್ಯ_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		ಯದಿ (d == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (ಮೌಲ್ಯ_3)d);
		ಯದಿ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
do_rubyeval(char_u *str, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ retval = FAIL;

    ಯದಿ (ensure_ruby_initialized())
    {
	ಪೂರ್ಣಾಙ್ಕ state;
	ಮೌಲ್ಯ_3 obj;

	obj = rb_eval_string_protect((ಧ್ರುವ ವರ್ಣ *)str, &state);
	ಯದಿ (state)
	    error_print(state);
	ಅನ್ಯಥಾ
	    retval = ruby_convert_to_vim_ಮೌಲ್ಯ(obj, rettv);
    }
    ಯದಿ (retval == FAIL)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
}
