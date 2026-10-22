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

#അന്തര്ഭാവ "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN ബാഹ്യ
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# എങ്കിൽ RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# എങ്കിൽ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# എങ്കിൽ RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_ചിഹ്നം		(*dll_rb_c_ചിഹ്നം)
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

# എങ്കിൽ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# എങ്കിൽ RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# എങ്കിൽ RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# എങ്കിൽ RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# എങ്കിൽ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# എങ്കിൽ RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# എങ്കിൽ RUBY_VERSION >= 31
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

#എങ്കിൽ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic തള്ളുക
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#എങ്കിൽ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#അന്തര്ഭാവ <ruby.h>
#അന്തര്ഭാവ <ruby/encoding.h>

#എങ്കിൽ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic എടുക്കുക
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#എങ്കിൽ defined(MACOS_X)
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
#എങ്കിൽ defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_മൂല്യം_Ptr
# define String_മൂല്യം_Ptr(s) STR2CSTR(s)
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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define ചിഹ്നം_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# അല്ലെങ്കിൽ
#  include <dlfcn.h>
#  define HINSTANCE ശൂന്യം*
#  define RUBY_PROC ശൂന്യം*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ചിഹ്നം_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ ruby_initialized = 0;
സ്ഥിതം ശൂന്യം *ruby_അട്ടി_start;
സ്ഥിതം മൂല്യം_3 objtbl;

സ്ഥിതം മൂല്യം_3 mVIM;
സ്ഥിതം മൂല്യം_3 cBuffer;
സ്ഥിതം മൂല്യം_3 cVimWindow;
സ്ഥിതം മൂല്യം_3 eDeletedBufferError;
സ്ഥിതം മൂല്യം_3 eDeletedWindowError;

സ്ഥിതം പൂർണ്ണസംഖ്യ ensure_ruby_initialized(ശൂന്യം);
സ്ഥിതം ശൂന്യം error_print(പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം ruby_io_init(ശൂന്യം);
സ്ഥിതം ശൂന്യം ruby_vim_init(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ ruby_convert_to_vim_മൂല്യം(മൂല്യം_3 val, typval_T *rettv);

#എങ്കിൽ defined(__ia64) && !defined(ruby_init_അട്ടി)
# define ruby_init_അട്ടി(addr) ruby_init_അട്ടി((addr), rb_ia64_bsp())
#endif

#എങ്കിൽ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# എങ്കിൽ RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# എങ്കിൽ RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  എങ്കിൽ RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  അല്ലെങ്കിൽ
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# അല്ലെങ്കിൽ
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
# എങ്കിൽ RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# അല്ലെങ്കിൽ
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

# എങ്കിൽ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  എങ്കിൽ RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# എങ്കിൽ RUBY_VERSION < 20
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
# അല്ലെങ്കിൽ
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_മൂല്യം		dll_rb_string_മൂല്യം
# define rb_string_മൂല്യം_ptr		dll_rb_string_മൂല്യം_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_തള്ളുക			dll_rb_ary_തള്ളുക
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_അട്ടി
#  define ruby_init_അട്ടി(addr)	dll_ruby_init_അട്ടി((addr), rb_ia64_bsp())
# അല്ലെങ്കിൽ
#  define ruby_init_അട്ടി		dll_ruby_init_അട്ടി
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
സ്ഥിതം മൂല്യം_3 (*dll_rb_assoc_new) (മൂല്യം_3, മൂല്യം_3);
മൂല്യം_3 *dll_rb_cFalseClass;
മൂല്യം_3 *dll_rb_cFixnum;
# എങ്കിൽ defined(USE_RUBY_INTEGER)
മൂല്യം_3 *dll_rb_cInteger;
# endif
# എങ്കിൽ RUBY_VERSION >= 20
മൂല്യം_3 *dll_rb_cFloat;
# endif
മൂല്യം_3 *dll_rb_cNilClass;
സ്ഥിതം മൂല്യം_3 *dll_rb_cObject;
മൂല്യം_3 *dll_rb_cString;
മൂല്യം_3 *dll_rb_c_ചിഹ്നം;
മൂല്യം_3 *dll_rb_cTrueClass;
സ്ഥിതം മൂല്യം_3 (*dll_rb_class_new_instance) (പൂർണ്ണസംഖ്യ,മൂല്യം_3*,മൂല്യം_3);
സ്ഥിതം ശൂന്യം (*dll_rb_check_type) (മൂല്യം_3,പൂർണ്ണസംഖ്യ);
# ifdef USE_TYPEDDATA
സ്ഥിതം ശൂന്യം *(*dll_rb_check_typeddata) (മൂല്യം_3,സ്ഥിരം rb_data_type_t *);
# endif
സ്ഥിതം മൂല്യം_3 (*dll_rb_class_path) (മൂല്യം_3);
# ifdef USE_TYPEDDATA
#  എങ്കിൽ RUBY_VERSION >= 23
സ്ഥിതം മൂല്യം_3 (*dll_rb_data_typed_object_wrap) (മൂല്യം_3, ശൂന്യം*, സ്ഥിരം rb_data_type_t *);
#  അല്ലെങ്കിൽ
സ്ഥിതം മൂല്യം_3 (*dll_rb_data_typed_object_alloc) (മൂല്യം_3, ശൂന്യം*, സ്ഥിരം rb_data_type_t *);
#  endif
# അല്ലെങ്കിൽ
സ്ഥിതം മൂല്യം_3 (*dll_rb_data_object_alloc) (മൂല്യം_3, ശൂന്യം*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# എങ്കിൽ RUBY_VERSION >= 31
സ്ഥിതം ശൂന്യം (*dll_rb_debug_rstring_null_ptr) (സ്ഥിരം അക്ഷരം*);
# endif
സ്ഥിതം മൂല്യം_3 (*dll_rb_define_class_under) (മൂല്യം_3, സ്ഥിരം അക്ഷരം*, മൂല്യം_3);
സ്ഥിതം ശൂന്യം (*dll_rb_define_const) (മൂല്യം_3,സ്ഥിരം അക്ഷരം*,മൂല്യം_3);
സ്ഥിതം ശൂന്യം (*dll_rb_define_global_function) (സ്ഥിരം അക്ഷരം*,മൂല്യം_3(*)(പൂർണ്ണസംഖ്യ,മൂല്യം_3*,മൂല്യം_3),പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം (*dll_rb_define_method) (മൂല്യം_3,സ്ഥിരം അക്ഷരം*,മൂല്യം_3(*)(ANYARGS),പൂർണ്ണസംഖ്യ);
സ്ഥിതം മൂല്യം_3 (*dll_rb_define_module) (സ്ഥിരം അക്ഷരം*);
സ്ഥിതം ശൂന്യം (*dll_rb_define_module_function) (മൂല്യം_3,സ്ഥിരം അക്ഷരം*,മൂല്യം_3(*)(മൂല്യം_3,മൂല്യം_3),പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം (*dll_rb_define_singleton_method) (മൂല്യം_3,സ്ഥിരം അക്ഷരം*,മൂല്യം_3(*)(ANYARGS),പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം (*dll_rb_define_virtual_variable) (സ്ഥിരം അക്ഷരം*,മൂല്യം_3(*)(ID,മൂല്യം_3*),ശൂന്യം(*)());
സ്ഥിതം മൂല്യം_3 *dll_rb_stdout;
സ്ഥിതം മൂല്യം_3 *dll_rb_stderr;
സ്ഥിതം മൂല്യം_3 *dll_rb_eArgError;
സ്ഥിതം മൂല്യം_3 *dll_rb_eIndexError;
സ്ഥിതം മൂല്യം_3 *dll_rb_eRuntimeError;
സ്ഥിതം മൂല്യം_3 *dll_rb_eStandardError;
സ്ഥിതം മൂല്യം_3 (*dll_rb_eval_string_protect) (സ്ഥിരം അക്ഷരം*, പൂർണ്ണസംഖ്യ*);
# എങ്കിൽ RUBY_VERSION >= 21
സ്ഥിതം മൂല്യം_3 (*dll_rb_funcallv) (മൂല്യം_3, ID, പൂർണ്ണസംഖ്യ, സ്ഥിരം മൂല്യം_3*);
# അല്ലെങ്കിൽ
സ്ഥിതം മൂല്യം_3 (*dll_rb_funcall2) (മൂല്യം_3, ID, പൂർണ്ണസംഖ്യ, സ്ഥിരം മൂല്യം_3*);
# endif
സ്ഥിതം ശൂന്യം (*dll_rb_global_variable) (മൂല്യം_3*);
സ്ഥിതം മൂല്യം_3 (*dll_rb_hash_aset) (മൂല്യം_3, മൂല്യം_3, മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_hash_foreach) (മൂല്യം_3, പൂർണ്ണസംഖ്യ (*)(മൂല്യം_3, മൂല്യം_3, മൂല്യം_3), മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_hash_new) (ശൂന്യം);
സ്ഥിതം മൂല്യം_3 (*dll_rb_inspect) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_int2inum) (long);
സ്ഥിതം ID (*dll_rb_intern) (സ്ഥിരം അക്ഷരം*);
# എങ്കിൽ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
സ്ഥിതം long (*dll_rb_fix2int) (മൂല്യം_3);
സ്ഥിതം long (*dll_rb_num2int) (മൂല്യം_3);
സ്ഥിതം unsigned long (*dll_rb_num2uint) (മൂല്യം_3);
# endif
സ്ഥിതം ഇരട്ട (*dll_rb_num2dbl) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_lastline_get) (ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_rb_lastline_set) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_protect) (മൂല്യം_3 (*)(മൂല്യം_3), മൂല്യം_3, പൂർണ്ണസംഖ്യ*);
സ്ഥിതം ശൂന്യം (*dll_rb_load) (മൂല്യം_3, പൂർണ്ണസംഖ്യ);
സ്ഥിതം long (*dll_rb_num2long) (മൂല്യം_3);
സ്ഥിതം unsigned long (*dll_rb_num2ulong) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_obj_alloc) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_obj_as_string) (മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_obj_id) (മൂല്യം_3);
സ്ഥിതം ശൂന്യം (*dll_rb_raise) (മൂല്യം_3, സ്ഥിരം അക്ഷരം*, ...);
സ്ഥിതം മൂല്യം_3 (*dll_rb_string_മൂല്യം) (അസ്ഥിര മൂല്യം_3*);
സ്ഥിതം മൂല്യം_3 (*dll_rb_str_cat) (മൂല്യം_3, സ്ഥിരം അക്ഷരം*, long);
സ്ഥിതം മൂല്യം_3 (*dll_rb_str_concat) (മൂല്യം_3, മൂല്യം_3);
സ്ഥിതം മൂല്യം_3 (*dll_rb_str_new) (സ്ഥിരം അക്ഷരം*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
സ്ഥിതം മൂല്യം_3 (*dll_rb_str_new_cstr) (സ്ഥിരം അക്ഷരം*);
# അല്ലെങ്കിൽ
സ്ഥിതം മൂല്യം_3 (*dll_rb_str_new2) (സ്ഥിരം അക്ഷരം*);
# endif
സ്ഥിതം മൂല്യം_3 (*dll_rb_errinfo) (ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_ruby_init) (ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_ruby_init_loadpath) (ശൂന്യം);
# ifdef MSWIN
സ്ഥിതം ശൂന്യം (*dll_ruby_sysinit) (പൂർണ്ണസംഖ്യ*, അക്ഷരം***);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_rb_w32_snprintf)(അക്ഷരം*, size_t, സ്ഥിരം അക്ഷരം*, ...);
# endif
# എങ്കിൽ RUBY_VERSION >= 31
#  ifdef _MSC_VER
സ്ഥിതം ശൂന്യം (*dll_rb_unexpected_type) (മൂല്യം_3, പൂർണ്ണസംഖ്യ);
#  അല്ലെങ്കിൽ
NORETURN(സ്ഥിതം ശൂന്യം (*dll_rb_unexpected_type) (മൂല്യം_3, പൂർണ്ണസംഖ്യ));
#  endif
# endif
സ്ഥിതം അക്ഷരം * (*dll_rb_string_മൂല്യം_ptr) (അസ്ഥിര മൂല്യം_3*);
സ്ഥിതം മൂല്യം_3 (*dll_rb_float_new) (ഇരട്ട);
സ്ഥിതം മൂല്യം_3 (*dll_rb_ary_new) (ശൂന്യം);
സ്ഥിതം മൂല്യം_3 (*dll_rb_ary_new4) (long n, സ്ഥിരം മൂല്യം_3 *elts);
സ്ഥിതം മൂല്യം_3 (*dll_rb_ary_തള്ളുക) (മൂല്യം_3, മൂല്യം_3);
# എങ്കിൽ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
സ്ഥിതം ശൂന്യം (*dll_rb_ary_detransient) (മൂല്യം_3);
# endif
# ifdef __ia64
സ്ഥിതം ശൂന്യം * (*dll_rb_ia64_bsp) (ശൂന്യം);
സ്ഥിതം ശൂന്യം (*dll_ruby_init_അട്ടി)(മൂല്യം_3*, ശൂന്യം*);
# അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം (*dll_ruby_init_അട്ടി)(മൂല്യം_3*);
# endif
സ്ഥിതം മൂല്യം_3 (*dll_rb_int2big)(SIGNED_മൂല്യം);

സ്ഥിതം ശൂന്യം (*dll_ruby_script) (സ്ഥിരം അക്ഷരം*);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_rb_enc_find_index) (സ്ഥിരം അക്ഷരം*);
സ്ഥിതം rb_encoding* (*dll_rb_enc_find) (സ്ഥിരം അക്ഷരം*);
സ്ഥിതം മൂല്യം_3 (*dll_rb_enc_str_new) (സ്ഥിരം അക്ഷരം*, long, rb_encoding*);
സ്ഥിതം മൂല്യം_3 (*dll_rb_sprintf) (സ്ഥിരം അക്ഷരം*, ...);
സ്ഥിതം മൂല്യം_3 (*dll_rb_require) (സ്ഥിരം അക്ഷരം*);
സ്ഥിതം ശൂന്യം* (*dll_ruby_options)(പൂർണ്ണസംഖ്യ, അക്ഷരം**);

# എങ്കിൽ defined(USE_RGENGC) && USE_RGENGC
#  എങ്കിൽ RUBY_VERSION == 21
സ്ഥിതം ശൂന്യം (*dll_rb_gc_writebarrier_unprotect_promoted)(മൂല്യം_3);
#  അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം (*dll_rb_gc_writebarrier_unprotect)(മൂല്യം_3 obj);
#  endif
# endif

# എങ്കിൽ RUBY_VERSION >= 30
#  ifdef _MSC_VER
സ്ഥിതം ശൂന്യം (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  അല്ലെങ്കിൽ
NORETURN(സ്ഥിതം ശൂന്യം (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# എങ്കിൽ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ശൂന്യം rb_ary_detransient_stub(മൂല്യം_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  എങ്കിൽ RUBY_VERSION >= 22
    long
rb_num2long_stub(മൂല്യം_3 x)
#  അല്ലെങ്കിൽ
    SIGNED_മൂല്യം
rb_num2long_stub(മൂല്യം_3 x)
#  endif
{
    തിരികെ dll_rb_num2long(x);
}
#  എങ്കിൽ RUBY_VERSION >= 26
    മൂല്യം_3
rb_int2big_stub(intptr_t x)
#  അല്ലെങ്കിൽ
    മൂല്യം_3
rb_int2big_stub(SIGNED_മൂല്യം x)
#  endif
{
    തിരികെ dll_rb_int2big(x);
}
#  എങ്കിൽ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(മൂല്യം_3 x)
{
    തിരികെ dll_rb_fix2int(x);
}
    long
rb_num2int_stub(മൂല്യം_3 x)
{
    തിരികെ dll_rb_num2int(x);
}
#  endif
#  എങ്കിൽ RUBY_VERSION >= 20
    മൂല്യം_3
rb_float_new_in_heap(ഇരട്ട d)
{
    തിരികെ dll_rb_float_new(d);
}
#   എങ്കിൽ RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(മൂല്യം_3 x)
#   അല്ലെങ്കിൽ
    മൂല്യം_3
rb_num2ulong(മൂല്യം_3 x)
#   endif
{
    തിരികെ (long)RSHIFT((SIGNED_മൂല്യം)(x),1);
}
#  endif
#  എങ്കിൽ defined(USE_RGENGC) && USE_RGENGC
#   എങ്കിൽ RUBY_VERSION == 21
    ശൂന്യം
rb_gc_writebarrier_unprotect_promoted_stub(മൂല്യം_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   അല്ലെങ്കിൽ
    ശൂന്യം
rb_gc_writebarrier_unprotect_stub(മൂല്യം_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  എങ്കിൽ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    ശൂന്യം
rb_ary_detransient_stub(മൂല്യം_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  എങ്കിൽ RUBY_VERSION >= 30
    ശൂന്യം
rb_check_type_stub(മൂല്യം_3 obj, പൂർണ്ണസംഖ്യ t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(മൂല്യം_3 x)
{
    തിരികെ dll_rb_num2uint(x);
}
    ശൂന്യം
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  എങ്കിൽ RUBY_VERSION >= 31
    ശൂന്യം
rb_debug_rstring_null_ptr_stub(സ്ഥിരം അക്ഷരം *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    ശൂന്യം
rb_unexpected_type_stub(മൂല്യം_3 self, പൂർണ്ണസംഖ്യ t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
ശൂന്യം *rb_check_typeddata_stub(മൂല്യം_3 obj, സ്ഥിരം rb_data_type_t *data_type)
{
    തിരികെ dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

സ്ഥിതം HINSTANCE hinstRuby = ശൂന്യ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
സ്ഥിതം ഘടന
{
    അക്ഷരം *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# എങ്കിൽ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# അല്ലെങ്കിൽ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# എങ്കിൽ RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_ചിഹ്നം},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  എങ്കിൽ RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  അല്ലെങ്കിൽ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# അല്ലെങ്കിൽ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# എങ്കിൽ RUBY_VERSION >= 31
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
# എങ്കിൽ RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# അല്ലെങ്കിൽ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# എങ്കിൽ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_മൂല്യം},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# അല്ലെങ്കിൽ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# എങ്കിൽ RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_മൂല്യം_ptr},
# എങ്കിൽ RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# അല്ലെങ്കിൽ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# അല്ലെങ്കിൽ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_തള്ളുക},
# എങ്കിൽ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_അട്ടി},
# എങ്കിൽ defined(USE_RGENGC) && USE_RGENGC
#  എങ്കിൽ RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  അല്ലെങ്കിൽ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# എങ്കിൽ RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", ശൂന്യ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ruby_runtime_link_init(അക്ഷരം *libname, പൂർണ്ണസംഖ്യ verbose)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (hinstRuby)
	തിരികെ OK;
    hinstRuby = load_dll(libname);
    എങ്കിൽ (!hinstRuby)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }

    വേണ്ടി (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	എങ്കിൽ (!(*ruby_funcname_table[i].ptr = ചിഹ്നം_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ശൂന്യ;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    പൂർണ്ണസംഖ്യ
ruby_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ ruby_runtime_link_init((അക്ഷരം *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    ശൂന്യം
ruby_end(ശൂന്യം)
{
}

    ശൂന്യം
ex_ruby(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ state;
    അക്ഷരം *script = ശൂന്യ;

    script = (അക്ഷരം *)script_get(eap, eap->arg);
    എങ്കിൽ (!eap->skip && ensure_ruby_initialized())
    {
	എങ്കിൽ (script == ശൂന്യ)
	    rb_eval_string_protect((അക്ഷരം *)eap->arg, &state);
	അല്ലെങ്കിൽ
	    rb_eval_string_protect(script, &state);
	എങ്കിൽ (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    സ്ഥിതം മൂല്യം_3
vim_str2rb_enc_str(സ്ഥിരം അക്ഷരം *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    എങ്കിൽ (get_option_മൂല്യം((char_u *)"enc", &lval, &sval, ശൂന്യ, 0) == gov_string)
    {
	enc = rb_enc_find((അക്ഷരം *)sval);
	vim_free(sval);
	എങ്കിൽ (enc)
	    തിരികെ rb_enc_str_new(s, (long)strlen(s), enc);
    }
    തിരികെ rb_str_new2(s);
}

    സ്ഥിതം മൂല്യം_3
eval_enc_string_protect(സ്ഥിരം അക്ഷരം *str, പൂർണ്ണസംഖ്യ *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    മൂല്യം_3 v;

    എങ്കിൽ (get_option_മൂല്യം((char_u *)"enc", &lval, &sval, ശൂന്യ, 0) == gov_string)
    {
	enc = rb_enc_find((അക്ഷരം *)sval);
	vim_free(sval);
	എങ്കിൽ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    തിരികെ rb_eval_string_protect(String_മൂല്യം_Ptr(v), state);
	}
    }
    തിരികെ rb_eval_string_protect(str, state);
}

    ശൂന്യം
ex_rubydo(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    എങ്കിൽ (!ensure_ruby_initialized())
	തിരികെ;

    എങ്കിൽ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	തിരികെ;
    വേണ്ടി (i = eap->line1; i <= eap->line2; i++)
    {
	മൂല്യം_3 വരി;

	എങ്കിൽ (i > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;
	വരി = vim_str2rb_enc_str((അക്ഷരം *)ml_get(i));
	rb_lastline_set(വരി);
	eval_enc_string_protect((അക്ഷരം *) eap->arg, &state);
	എങ്കിൽ (state)
	{
	    error_print(state);
	    നിർത്തുക;
	}
	എങ്കിൽ (was_curbuf != curbuf || i > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;
	വരി = rb_lastline_get();
	എങ്കിൽ (!NIL_P(വരി))
	{
	    എങ്കിൽ (TYPE(വരി) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		തിരികെ;
	    }
	    ml_replace(i, (char_u *) String_മൂല്യം_Ptr(വരി), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    സ്ഥിതം മൂല്യം_3
rb_load_wrap(മൂല്യം_3 file_to_load)
{
    rb_load(file_to_load, 0);
    തിരികെ Qnil;
}

    ശൂന്യം
ex_rubyfile(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ state;

    എങ്കിൽ (!ensure_ruby_initialized())
	തിരികെ;

    മൂല്യം_3 file_to_load = rb_str_new2((സ്ഥിരം അക്ഷരം *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    എങ്കിൽ (state)
	error_print(state);
}

    ശൂന്യം
ruby_buffer_free(buf_T *buf)
{
    എങ്കിൽ (buf->b_ruby_ref == ശൂന്യ)
	തിരികെ;

    rb_hash_aset(objtbl, rb_obj_id((മൂല്യം_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = ശൂന്യ;
}

    ശൂന്യം
ruby_window_free(win_T *win)
{
    എങ്കിൽ (win->w_ruby_ref == ശൂന്യ)
	തിരികെ;

    rb_hash_aset(objtbl, rb_obj_id((മൂല്യം_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ensure_ruby_initialized(ശൂന്യം)
{
    എങ്കിൽ (ruby_initialized)
	തിരികെ ruby_initialized;

#ifdef DYNAMIC_RUBY
    എങ്കിൽ (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	പൂർണ്ണസംഖ്യ argc = 1;
	അക്ഷരം *argv[] = {"gvim.exe"};
	അക്ഷരം **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_അട്ടി(ruby_അട്ടി_start);
	    ruby_init();
	}
	{
	    പൂർണ്ണസംഖ്യ dummy_argc = 2;
	    അക്ഷരം *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    അല്ലെങ്കിൽ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	തിരികെ 0;
    }
#endif

    തിരികെ ruby_initialized;
}

    സ്ഥിതം ശൂന്യം
error_print(പൂർണ്ണസംഖ്യ state)
{
    മൂല്യം_3 error;
    മൂല്യം_3 eclass;
    മൂല്യം_3 einfo;
    മൂല്യം_3 bt;
    പൂർണ്ണസംഖ്യ attr;
    അക്ഷരം buff[BUFSIZ];
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

    തിരഞ്ഞെടുപ്പ് (state)
    {
	അവസ്ഥ TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    നിർത്തുക;
	അവസ്ഥ TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    നിർത്തുക;
	അവസ്ഥ TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    നിർത്തുക;
	അവസ്ഥ TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    നിർത്തുക;
	അവസ്ഥ TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    നിർത്തുക;
	അവസ്ഥ TAG_RAISE:
	അവസ്ഥ TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    എങ്കിൽ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    അല്ലെങ്കിൽ
	    {
		മൂല്യം_3 epath;
		അക്ഷരം *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		എങ്കിൽ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#എങ്കിൽ RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    വേണ്ടി (i = 0; i < RARRAY_LEN(bt); i++)
		സന്ദേശം_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#അല്ലെങ്കിൽ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    വേണ്ടി (i = 0; i < RARRAY_LEN(bt); i++)
		സന്ദേശം_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    നിർത്തുക;
    }
}

    സ്ഥിതം മൂല്യം_3
vim_message(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
    അക്ഷരം *buff, *p;

    str = rb_obj_as_string(str);
    എങ്കിൽ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(അക്ഷരം, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	എങ്കിൽ (p) *p = '\0';
	സന്ദേശം(buff);
    }
    അല്ലെങ്കിൽ
    {
	സന്ദേശം("");
    }
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
vim_set_option(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
    do_set((char_u *)String_മൂല്യം_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
vim_command(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
    do_cmdline_cmd((char_u *)String_മൂല്യം_Ptr(str));
    തിരികെ Qnil;
}

#ifdef FEAT_EVAL
    സ്ഥിതം മൂല്യം_3
vim_to_ruby(typval_T *tv)
{
    മൂല്യം_3 result = Qnil;

    എങ്കിൽ (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == ശൂന്യ
					  ? "" : (അക്ഷരം *)(tv->vval.v_string));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_സംഖ്യ)
    {
	result = INT2_അക്കം(tv->vval.v_സംഖ്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	എങ്കിൽ (list != ശൂന്യ)
	{
	    FOR_ALL_LIST_ഇനങ്ങൾ(list, curr)
		rb_ary_തള്ളുക(result, vim_to_ruby(&curr->li_tv));
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	എങ്കിൽ (tv->vval.v_dict != ശൂന്യ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
	    {
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((അക്ഷരം *)hi->hi_താക്കോൽ),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	എങ്കിൽ (tv->vval.v_സംഖ്യ == VVAL_TRUE)
	    result = Qtrue;
	അല്ലെങ്കിൽ എങ്കിൽ (tv->vval.v_സംഖ്യ == VVAL_FALSE)
	    result = Qfalse;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    തിരികെ result;
}
#endif

    സ്ഥിതം മൂല്യം_3
vim_evaluate(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    മൂല്യം_3       result;

    tv = eval_വ്യഞ്ജകം((char_u *)String_മൂല്യം_Ptr(str), ശൂന്യ);
    എങ്കിൽ (tv == ശൂന്യ)
	തിരികെ Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    തിരികെ result;
#അല്ലെങ്കിൽ
    തിരികെ Qnil;
#endif
}

#ifdef USE_TYPEDDATA
സ്ഥിതം size_t buffer_dsize(സ്ഥിരം ശൂന്യം *buf);

സ്ഥിതം സ്ഥിരം rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# എങ്കിൽ RUBY_VERSION >= 27
	0, {0}
# അല്ലെങ്കിൽ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    സ്ഥിതം size_t
buffer_dsize(സ്ഥിരം ശൂന്യം *buf UNUSED)
{
    തിരികെ വലുപ്പം(buf_T);
}
#endif

    സ്ഥിതം മൂല്യം_3
buffer_new(buf_T *buf)
{
    എങ്കിൽ (buf->b_ruby_ref)
    {
	തിരികെ (മൂല്യം_3) buf->b_ruby_ref;
    }
    അല്ലെങ്കിൽ
    {
#ifdef USE_TYPEDDATA
	മൂല്യം_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#അല്ലെങ്കിൽ
	മൂല്യം_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (ശൂന്യം *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	തിരികെ obj;
    }
}

    സ്ഥിതം buf_T *
get_buf(മൂല്യം_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#അല്ലെങ്കിൽ
    Data_Get_Struct(obj, buf_T, buf);
#endif
    എങ്കിൽ (buf == ശൂന്യ)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    തിരികെ buf;
}

    സ്ഥിതം മൂല്യം_3
vim_blob(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
    മൂല്യം_3 result = rb_str_new("0z", 2);
    അക്ഷരം    buf[4];
    പൂർണ്ണസംഖ്യ	i;
    വേണ്ടി (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned അക്ഷരം)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    തിരികെ result;
}

    സ്ഥിതം മൂല്യം_3
buffer_s_current(മൂല്യം_3 self UNUSED)
{
    തിരികെ buffer_new(curbuf);
}

    സ്ഥിതം മൂല്യം_3
buffer_s_current_getter(ID id UNUSED, മൂല്യം_3 *x UNUSED)
{
    തിരികെ buffer_new(curbuf);
}

    സ്ഥിതം മൂല്യം_3
buffer_s_എണ്ണം(മൂല്യം_3 self UNUSED)
{
    buf_T *b;
    പൂർണ്ണസംഖ്യ n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	എങ്കിൽ (b->b_p_bl)
	    n++;
    }

    തിരികെ INT2_അക്കം(n);
}

    സ്ഥിതം മൂല്യം_3
buffer_s_aref(മൂല്യം_3 self UNUSED, മൂല്യം_3 അക്കം)
{
    buf_T *b;
    പൂർണ്ണസംഖ്യ n = NUM2INT(അക്കം);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	എങ്കിൽ (!b->b_p_bl)
	    തുടരുക;

	എങ്കിൽ (n == 0)
	    തിരികെ buffer_new(b);

	n--;
    }
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
buffer_name(മൂല്യം_3 self)
{
    buf_T *buf = get_buf(self);

    തിരികെ buf->b_ffname ? rb_str_new2((അക്ഷരം *)buf->b_ffname) : Qnil;
}

    സ്ഥിതം മൂല്യം_3
buffer_സംഖ്യ(മൂല്യം_3 self)
{
    buf_T *buf = get_buf(self);

    തിരികെ INT2_അക്കം(buf->b_fnum);
}

    സ്ഥിതം മൂല്യം_3
buffer_എണ്ണം(മൂല്യം_3 self)
{
    buf_T *buf = get_buf(self);

    തിരികെ INT2_അക്കം(buf->b_ml.ml_വരി_എണ്ണം);
}

    സ്ഥിതം മൂല്യം_3
get_buffer_വരി(buf_T *buf, linenr_T n)
{
    എങ്കിൽ (n <= 0 || n > buf->b_ml.ml_വരി_എണ്ണം)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    തിരികെ vim_str2rb_enc_str((അക്ഷരം *)ml_get_buf(buf, n, FALSE));
}

    സ്ഥിതം മൂല്യം_3
buffer_aref(മൂല്യം_3 self, മൂല്യം_3 അക്കം)
{
    buf_T *buf = get_buf(self);

    എങ്കിൽ (buf != ശൂന്യ)
	തിരികെ get_buffer_വരി(buf, (linenr_T)NUM2LONG(അക്കം));
    തിരികെ Qnil; // For stop warning
}

    സ്ഥിതം മൂല്യം_3
set_buffer_വരി(buf_T *buf, linenr_T n, മൂല്യം_3 str)
{
    അക്ഷരം	*വരി = String_മൂല്യം_Ptr(str);
    aco_save_T	aco;

    എങ്കിൽ (n > 0 && n <= buf->b_ml.ml_വരി_എണ്ണം && വരി != ശൂന്യ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	എങ്കിൽ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    എങ്കിൽ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)വരി, TRUE);
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
    അല്ലെങ്കിൽ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    തിരികെ str;
}

    സ്ഥിതം മൂല്യം_3
buffer_aset(മൂല്യം_3 self, മൂല്യം_3 അക്കം, മൂല്യം_3 str)
{
    buf_T *buf = get_buf(self);

    എങ്കിൽ (buf != ശൂന്യ)
	തിരികെ set_buffer_വരി(buf, (linenr_T)NUM2LONG(അക്കം), str);
    തിരികെ str;
}

    സ്ഥിതം മൂല്യം_3
buffer_delete(മൂല്യം_3 self, മൂല്യം_3 അക്കം)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(അക്കം);
    aco_save_T	aco;

    എങ്കിൽ (n > 0 && n <= buf->b_ml.ml_വരി_എണ്ണം)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	എങ്കിൽ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    എങ്കിൽ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_വരികൾ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    അല്ലെങ്കിൽ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
buffer_append(മൂല്യം_3 self, മൂല്യം_3 അക്കം, മൂല്യം_3 str)
{
    buf_T	*buf = get_buf(self);
    അക്ഷരം	*വരി = String_മൂല്യം_Ptr(str);
    long	n = NUM2LONG(അക്കം);
    aco_save_T	aco;

    എങ്കിൽ (വരി == ശൂന്യ)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n >= 0 && n <= buf->b_ml.ml_വരി_എണ്ണം)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	എങ്കിൽ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    എങ്കിൽ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) വരി, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_വരികൾ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    അല്ലെങ്കിൽ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    തിരികെ str;
}

#ifdef USE_TYPEDDATA
സ്ഥിതം size_t window_dsize(സ്ഥിരം ശൂന്യം *buf);

സ്ഥിതം സ്ഥിരം rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# എങ്കിൽ RUBY_VERSION >= 27
	0, {0}
# അല്ലെങ്കിൽ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    സ്ഥിതം size_t
window_dsize(സ്ഥിരം ശൂന്യം *win UNUSED)
{
    തിരികെ വലുപ്പം(win_T);
}
#endif

    സ്ഥിതം മൂല്യം_3
window_new(win_T *win)
{
    എങ്കിൽ (win->w_ruby_ref)
    {
	തിരികെ (മൂല്യം_3) win->w_ruby_ref;
    }
    അല്ലെങ്കിൽ
    {
#ifdef USE_TYPEDDATA
	മൂല്യം_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#അല്ലെങ്കിൽ
	മൂല്യം_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (ശൂന്യം *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	തിരികെ obj;
    }
}

    സ്ഥിതം win_T *
get_win(മൂല്യം_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#അല്ലെങ്കിൽ
    Data_Get_Struct(obj, win_T, win);
#endif
    എങ്കിൽ (win == ശൂന്യ)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    തിരികെ win;
}

    സ്ഥിതം മൂല്യം_3
window_s_current(മൂല്യം_3 self UNUSED)
{
    തിരികെ window_new(curwin);
}

    സ്ഥിതം മൂല്യം_3
window_s_current_getter(ID id UNUSED, മൂല്യം_3 *x UNUSED)
{
    തിരികെ window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    സ്ഥിതം മൂല്യം_3
വരി_s_current(മൂല്യം_3 self UNUSED)
{
    തിരികെ get_buffer_വരി(curbuf, curwin->w_cursor.lnum);
}

    സ്ഥിതം മൂല്യം_3
set_current_വരി(മൂല്യം_3 self UNUSED, മൂല്യം_3 str)
{
    തിരികെ set_buffer_വരി(curbuf, curwin->w_cursor.lnum, str);
}

    സ്ഥിതം മൂല്യം_3
current_വരി_സംഖ്യ(മൂല്യം_3 self UNUSED)
{
    തിരികെ INT2FIX((പൂർണ്ണസംഖ്യ)curwin->w_cursor.lnum);
}

    സ്ഥിതം മൂല്യം_3
window_s_എണ്ണം(മൂല്യം_3 self UNUSED)
{
    win_T	*w;
    പൂർണ്ണസംഖ്യ n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    തിരികെ INT2_അക്കം(n);
}

    സ്ഥിതം മൂല്യം_3
window_s_aref(മൂല്യം_3 self UNUSED, മൂല്യം_3 അക്കം)
{
    win_T *w;
    പൂർണ്ണസംഖ്യ n = NUM2INT(അക്കം);

    വേണ്ടി (w = firstwin; w != ശൂന്യ; w = w->w_next, --n)
	എങ്കിൽ (n == 0)
	    തിരികെ window_new(w);
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
window_buffer(മൂല്യം_3 self)
{
    win_T *win = get_win(self);

    തിരികെ buffer_new(win->w_buffer);
}

    സ്ഥിതം മൂല്യം_3
window_height(മൂല്യം_3 self)
{
    win_T *win = get_win(self);

    തിരികെ INT2_അക്കം(win->w_height);
}

    സ്ഥിതം മൂല്യം_3
window_set_height(മൂല്യം_3 self, മൂല്യം_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    തിരികെ height;
}

    സ്ഥിതം മൂല്യം_3
window_width(മൂല്യം_3 self UNUSED)
{
    തിരികെ INT2_അക്കം(get_win(self)->w_width);
}

    സ്ഥിതം മൂല്യം_3
window_set_width(മൂല്യം_3 self UNUSED, മൂല്യം_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    തിരികെ width;
}

    സ്ഥിതം മൂല്യം_3
window_cursor(മൂല്യം_3 self)
{
    win_T *win = get_win(self);

    തിരികെ rb_assoc_new(INT2_അക്കം(win->w_cursor.lnum), INT2_അക്കം(win->w_cursor.col));
}

    സ്ഥിതം മൂല്യം_3
window_set_cursor(മൂല്യം_3 self, മൂല്യം_3 pos)
{
    മൂല്യം_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    എങ്കിൽ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
f_nop(മൂല്യം_3 self UNUSED)
{
    തിരികെ Qnil;
}

    സ്ഥിതം മൂല്യം_3
f_p(പൂർണ്ണസംഖ്യ argc, മൂല്യം_3 *argv, മൂല്യം_3 self UNUSED)
{
    പൂർണ്ണസംഖ്യ i;
    മൂല്യം_3 str = rb_str_new("", 0);
    മൂല്യം_3 ret = Qnil;

    വേണ്ടി (i = 0; i < argc; i++)
    {
	എങ്കിൽ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    സന്ദേശം(RSTRING_PTR(str));

    എങ്കിൽ (argc == 1)
	ret = argv[0];
    അല്ലെങ്കിൽ എങ്കിൽ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    തിരികെ ret;
}

    സ്ഥിതം ശൂന്യം
ruby_io_init(ശൂന്യം)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN മൂല്യം_3 rb_stdout;
    RUBYEXTERN മൂല്യം_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (ശൂന്യം*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (ശൂന്യം*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (ശൂന്യം*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (ശൂന്യം*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    സ്ഥിതം ശൂന്യം
ruby_vim_init(ശൂന്യം)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_അക്കം(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_അക്കം(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_അക്കം(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_അക്കം(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (ശൂന്യം*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (ശൂന്യം*)buffer_s_എണ്ണം, 0);
    rb_define_singleton_method(cBuffer, "[]", (ശൂന്യം*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (ശൂന്യം*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (ശൂന്യം*)buffer_സംഖ്യ, 0);
    rb_define_method(cBuffer, "count", (ശൂന്യം*)buffer_എണ്ണം, 0);
    rb_define_method(cBuffer, "length", (ശൂന്യം*)buffer_എണ്ണം, 0);
    rb_define_method(cBuffer, "[]", (ശൂന്യം*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (ശൂന്യം*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (ശൂന്യം*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (ശൂന്യം*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (ശൂന്യം*)current_വരി_സംഖ്യ, 0);
    rb_define_method(cBuffer, "line", (ശൂന്യം*)വരി_s_current, 0);
    rb_define_method(cBuffer, "line=", (ശൂന്യം*)set_current_വരി, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (ശൂന്യം*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (ശൂന്യം*)window_s_എണ്ണം, 0);
    rb_define_singleton_method(cVimWindow, "[]", (ശൂന്യം*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (ശൂന്യം*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (ശൂന്യം*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (ശൂന്യം*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (ശൂന്യം*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (ശൂന്യം*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (ശൂന്യം*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (ശൂന്യം*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    ശൂന്യം
vim_ruby_init(ശൂന്യം *അട്ടി_start)
{
    // should get machine stack start address early in main function
    ruby_അട്ടി_start = അട്ടി_start;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
convert_hash2dict(മൂല്യം_3 താക്കോൽ, മൂല്യം_3 val, മൂല്യം_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(താക്കോൽ)));
    എങ്കിൽ (di == ശൂന്യ || ruby_convert_to_vim_മൂല്യം(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	തിരികെ ST_STOP;
    }
    തിരികെ ST_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ruby_convert_to_vim_മൂല്യം(മൂല്യം_3 val, typval_T *rettv)
{
    തിരഞ്ഞെടുപ്പ് (TYPE(val))
    {
	അവസ്ഥ T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_സംഖ്യ = VVAL_NULL;
	    നിർത്തുക;
	അവസ്ഥ T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
	    നിർത്തുക;
	അവസ്ഥ T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_സംഖ്യ = VVAL_FALSE;
	    നിർത്തുക;
	അവസ്ഥ T_BIGNUM:
	അവസ്ഥ T_FIXNUM:
	    rettv->v_type = VAR_സംഖ്യ;
	    rettv->vval.v_സംഖ്യ = (varnumber_T)NUM2LONG(val);
	    നിർത്തുക;
	അവസ്ഥ T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	അവസ്ഥ T_STRING:
	    {
		മൂല്യം_3 str = (മൂല്യം_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    നിർത്തുക;
	അവസ്ഥ T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		എങ്കിൽ (l == ശൂന്യ)
		    തിരികെ FAIL;

		വേണ്ടി (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    എങ്കിൽ (ruby_convert_to_vim_മൂല്യം((മൂല്യം_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			തിരികെ FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    നിർത്തുക;
	അവസ്ഥ T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		എങ്കിൽ (d == ശൂന്യ)
		    തിരികെ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (മൂല്യം_3)d);
		എങ്കിൽ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    തിരികെ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    നിർത്തുക;
    }
    തിരികെ OK;
}

    ശൂന്യം
do_rubyeval(char_u *str, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ retval = FAIL;

    എങ്കിൽ (ensure_ruby_initialized())
    {
	പൂർണ്ണസംഖ്യ state;
	മൂല്യം_3 obj;

	obj = rb_eval_string_protect((സ്ഥിരം അക്ഷരം *)str, &state);
	എങ്കിൽ (state)
	    error_print(state);
	അല്ലെങ്കിൽ
	    retval = ruby_convert_to_vim_മൂല്യം(obj, rettv);
    }
    എങ്കിൽ (retval == FAIL)
    {
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = 0;
    }
}
