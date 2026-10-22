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

#ਅਨ੍ਤਰ੍ਭਾਵ "protodef.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
# include "auto/config.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RUBYEXTERN
# define RUBYEXTERN ਬਾਹਰੀ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ਜੇ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_ਚਿੰਨ੍ਹ		(*dll_rb_c_ਚਿੰਨ੍ਹ)
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

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 26 && RUBY_ਸੰਸਕਰਣ <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_ਮਾਪ_ਉਛਾਲ	ruby_malloc_ਮਾਪ_ਉਛਾਲ_stub
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#ਜੇ_ਅੰਤ  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIZEOF_ਸਮਾਂ_T
# undef SIZEOF_ਸਮਾਂ_T
#ਜੇ_ਅੰਤ

#ਜੇ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ਧੱਕੋ
# pragma GCC diagnostic ignored "-Wunused-parameter"
#ਜੇ_ਅੰਤ

#ਜੇ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <ruby.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ruby/encoding.h>

#ਜੇ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

// See above.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIZEOF_ਸਮਾਂ_T
# undef SIZEOF_ਸਮਾਂ_T
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ off_t	// ruby defines off_t as _int64, Mingw uses long
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ EXTERN
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ਜੇ defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#ਜੇ_ਅੰਤ

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#ਜੇ defined(Typed_ਅੰਕੜੇ_Wrap_Struct) && (RUBY_ਸੰਸਕਰਣ >= 20)
# define USE_TYPEDDATA	1
#ਜੇ_ਅੰਤ

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ String_ਮੁੱਲ_Ptr
# define String_ਮੁੱਲ_Ptr(s) STR2CSTR(s)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DUP
# undef HAVE_DUP
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
# undef HAVE_FSYNC
#ਜੇ_ਅੰਤ

// Avoid redefining TRUE/FALSE in vterm.h.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRUE
# undef TRUE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FALSE
# undef FALSE
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#  define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#  define close_dll FreeLibrary
#  define ਲੱਦੋ_dll_error GetWin32Error
# ਨਹੀਂਤਾਂ
#  include <dlfcn.h>
#  define HINSTANCE ਖਾਲੀ*
#  define RUBY_PROC ਖਾਲੀ*
#  define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ਚਿੰਨ੍ਹ_from_dll dlsym
#  define close_dll dlclose
#  define ਲੱਦੋ_dll_error dlerror
# endif
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ ruby_initialized = 0;
ਸਥਿਰ ਖਾਲੀ *ruby_ਢੇਰ_ਸ਼ੁਰੂ;
ਸਥਿਰ ਮੁੱਲ_3 objtbl;

ਸਥਿਰ ਮੁੱਲ_3 mVIM;
ਸਥਿਰ ਮੁੱਲ_3 c_ਬਫਰ;
ਸਥਿਰ ਮੁੱਲ_3 c_Vim_ਖਿੜਕੀ;
ਸਥਿਰ ਮੁੱਲ_3 e_Deleted_ਬਫਰ_Error;
ਸਥਿਰ ਮੁੱਲ_3 e_Deleted_ਖਿੜਕੀ_Error;

ਸਥਿਰ ਪੂਰਨ ensure_ruby_initialized(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ error_print(ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ ruby_io_init(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ruby_vim_init(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ ruby_convert_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ(ਮੁੱਲ_3 val, typval_T *rettv);

#ਜੇ defined(__ia64) && !defined(ruby_init_ਢੇਰ)
# define ruby_init_ਢੇਰ(addr) ruby_init_ਢੇਰ((addr), rb_ia64_bsp())
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 27
#  undef rb_define_global_function
#  undef rb_define_ਵਿਧੀ
#  undef rb_define_module_function
#  undef rb_define_singleton_ਵਿਧੀ
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ਜੇ RUBY_ਸੰਸਕਰਣ < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_ਰਸਤਾ			dll_rb_class_ਰਸਤਾ
# ifdef USE_TYPEDDATA
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 23
#   define rb_ਅੰਕੜੇ_typed_object_wrap	dll_rb_ਅੰਕੜੇ_typed_object_wrap
#  ਨਹੀਂਤਾਂ
#   define rb_ਅੰਕੜੇ_typed_object_alloc	dll_rb_ਅੰਕੜੇ_typed_object_alloc
#  endif
# ਨਹੀਂਤਾਂ
#  define rb_ਅੰਕੜੇ_object_alloc		dll_rb_ਅੰਕੜੇ_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_ਵਿਧੀ		dll_rb_define_ਵਿਧੀ
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_ਵਿਧੀ	dll_rb_define_singleton_ਵਿਧੀ
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_ਸੂਚਕਾਂਕ_Error			(*dll_rb_e_ਸੂਚਕਾਂਕ_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 21
#  define rb_funcallv			dll_rb_funcallv
# ਨਹੀਂਤਾਂ
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

# ਜੇ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ਜੇ RUBY_ਸੰਸਕਰਣ < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_ਸੈੱਟ			dll_rb_lastline_ਸੈੱਟ
# define rb_protect			dll_rb_protect
# define rb_ਲੱਦੋ			dll_rb_ਲੱਦੋ
# ਜੇ RUBY_ਸੰਸਕਰਣ < 20
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
# ਨਹੀਂਤਾਂ
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_ਮੁੱਲ		dll_rb_string_ਮੁੱਲ
# define rb_string_ਮੁੱਲ_ptr		dll_rb_string_ਮੁੱਲ_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ਧੱਕੋ			dll_rb_ary_ਧੱਕੋ
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ਢੇਰ
#  define ruby_init_ਢੇਰ(addr)	dll_ruby_init_ਢੇਰ((addr), rb_ia64_bsp())
# ਨਹੀਂਤਾਂ
#  define ruby_init_ਢੇਰ		dll_ruby_init_ਢੇਰ
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_ਲੱਭੋ_ਸੂਚਕਾਂਕ		dll_rb_enc_ਲੱਭੋ_ਸੂਚਕਾਂਕ
# define rb_enc_ਲੱਭੋ			dll_rb_enc_ਲੱਭੋ
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_assoc_new) (ਮੁੱਲ_3, ਮੁੱਲ_3);
ਮੁੱਲ_3 *dll_rb_cFalseClass;
ਮੁੱਲ_3 *dll_rb_cFixnum;
# ਜੇ defined(USE_RUBY_INTEGER)
ਮੁੱਲ_3 *dll_rb_cInteger;
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
ਮੁੱਲ_3 *dll_rb_cFloat;
# endif
ਮੁੱਲ_3 *dll_rb_cNilClass;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_cObject;
ਮੁੱਲ_3 *dll_rb_cString;
ਮੁੱਲ_3 *dll_rb_c_ਚਿੰਨ੍ਹ;
ਮੁੱਲ_3 *dll_rb_cTrueClass;
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_class_new_instance) (ਪੂਰਨ,ਮੁੱਲ_3*,ਮੁੱਲ_3);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_check_type) (ਮੁੱਲ_3,ਪੂਰਨ);
# ifdef USE_TYPEDDATA
ਸਥਿਰ ਖਾਲੀ *(*dll_rb_check_typeddata) (ਮੁੱਲ_3,ਅਚਲ rb_ਅੰਕੜੇ_type_t *);
# endif
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_class_ਰਸਤਾ) (ਮੁੱਲ_3);
# ifdef USE_TYPEDDATA
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 23
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ਅੰਕੜੇ_typed_object_wrap) (ਮੁੱਲ_3, ਖਾਲੀ*, ਅਚਲ rb_ਅੰਕੜੇ_type_t *);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ਅੰਕੜੇ_typed_object_alloc) (ਮੁੱਲ_3, ਖਾਲੀ*, ਅਚਲ rb_ਅੰਕੜੇ_type_t *);
#  endif
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ਅੰਕੜੇ_object_alloc) (ਮੁੱਲ_3, ਖਾਲੀ*, RUBY_ਅੰਕੜੇ_FUNC, RUBY_ਅੰਕੜੇ_FUNC);
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
ਸਥਿਰ ਖਾਲੀ (*dll_rb_debug_rstring_null_ptr) (ਅਚਲ ਅੱਖਰ*);
# endif
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_define_class_under) (ਮੁੱਲ_3, ਅਚਲ ਅੱਖਰ*, ਮੁੱਲ_3);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_const) (ਮੁੱਲ_3,ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_global_function) (ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3(*)(ਪੂਰਨ,ਮੁੱਲ_3*,ਮੁੱਲ_3),ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_ਵਿਧੀ) (ਮੁੱਲ_3,ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3(*)(ANYARGS),ਪੂਰਨ);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_define_module) (ਅਚਲ ਅੱਖਰ*);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_module_function) (ਮੁੱਲ_3,ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3(*)(ਮੁੱਲ_3,ਮੁੱਲ_3),ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_singleton_ਵਿਧੀ) (ਮੁੱਲ_3,ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3(*)(ANYARGS),ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_define_virtual_variable) (ਅਚਲ ਅੱਖਰ*,ਮੁੱਲ_3(*)(ID,ਮੁੱਲ_3*),ਖਾਲੀ(*)());
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_stdout;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_stderr;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_eArgError;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_e_ਸੂਚਕਾਂਕ_Error;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_eRuntimeError;
ਸਥਿਰ ਮੁੱਲ_3 *dll_rb_eStandardError;
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_eval_string_protect) (ਅਚਲ ਅੱਖਰ*, ਪੂਰਨ*);
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 21
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_funcallv) (ਮੁੱਲ_3, ID, ਪੂਰਨ, ਅਚਲ ਮੁੱਲ_3*);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_funcall2) (ਮੁੱਲ_3, ID, ਪੂਰਨ, ਅਚਲ ਮੁੱਲ_3*);
# endif
ਸਥਿਰ ਖਾਲੀ (*dll_rb_global_variable) (ਮੁੱਲ_3*);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_hash_aset) (ਮੁੱਲ_3, ਮੁੱਲ_3, ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_hash_foreach) (ਮੁੱਲ_3, ਪੂਰਨ (*)(ਮੁੱਲ_3, ਮੁੱਲ_3, ਮੁੱਲ_3), ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_hash_new) (ਖਾਲੀ);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_inspect) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_int2inum) (ਲੰਮਾ);
ਸਥਿਰ ID (*dll_rb_intern) (ਅਚਲ ਅੱਖਰ*);
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ਸਥਿਰ ਲੰਮਾ (*dll_rb_fix2int) (ਮੁੱਲ_3);
ਸਥਿਰ ਲੰਮਾ (*dll_rb_num2int) (ਮੁੱਲ_3);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ (*dll_rb_num2uint) (ਮੁੱਲ_3);
# endif
ਸਥਿਰ ਦੁੱਗਣਾ (*dll_rb_num2dbl) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_lastline_get) (ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_lastline_ਸੈੱਟ) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_protect) (ਮੁੱਲ_3 (*)(ਮੁੱਲ_3), ਮੁੱਲ_3, ਪੂਰਨ*);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_ਲੱਦੋ) (ਮੁੱਲ_3, ਪੂਰਨ);
ਸਥਿਰ ਲੰਮਾ (*dll_rb_num2long) (ਮੁੱਲ_3);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ (*dll_rb_num2ulong) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_obj_alloc) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_obj_as_string) (ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_obj_id) (ਮੁੱਲ_3);
ਸਥਿਰ ਖਾਲੀ (*dll_rb_raise) (ਮੁੱਲ_3, ਅਚਲ ਅੱਖਰ*, ...);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_string_ਮੁੱਲ) (ਅਸਥਿਰ ਮੁੱਲ_3*);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_str_cat) (ਮੁੱਲ_3, ਅਚਲ ਅੱਖਰ*, ਲੰਮਾ);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_str_concat) (ਮੁੱਲ_3, ਮੁੱਲ_3);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_str_new) (ਅਚਲ ਅੱਖਰ*, ਲੰਮਾ);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_str_new_cstr) (ਅਚਲ ਅੱਖਰ*);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_str_new2) (ਅਚਲ ਅੱਖਰ*);
# endif
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_errinfo) (ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_init) (ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_init_loadpath) (ਖਾਲੀ);
# ifdef MSWIN
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_sysinit) (ਪੂਰਨ*, ਅੱਖਰ***);
ਸਥਿਰ ਪੂਰਨ (*dll_rb_w32_snprintf)(ਅੱਖਰ*, size_t, ਅਚਲ ਅੱਖਰ*, ...);
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
#  ifdef _MSC_VER
ਸਥਿਰ ਖਾਲੀ (*dll_rb_unexpected_type) (ਮੁੱਲ_3, ਪੂਰਨ);
#  ਨਹੀਂਤਾਂ
NORETURN(ਸਥਿਰ ਖਾਲੀ (*dll_rb_unexpected_type) (ਮੁੱਲ_3, ਪੂਰਨ));
#  endif
# endif
ਸਥਿਰ ਅੱਖਰ * (*dll_rb_string_ਮੁੱਲ_ptr) (ਅਸਥਿਰ ਮੁੱਲ_3*);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_float_new) (ਦੁੱਗਣਾ);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ary_new) (ਖਾਲੀ);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ary_new4) (ਲੰਮਾ n, ਅਚਲ ਮੁੱਲ_3 *elts);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_ary_ਧੱਕੋ) (ਮੁੱਲ_3, ਮੁੱਲ_3);
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 26 && RUBY_ਸੰਸਕਰਣ <= 32
ਸਥਿਰ ਖਾਲੀ (*dll_rb_ary_detransient) (ਮੁੱਲ_3);
# endif
# ifdef __ia64
ਸਥਿਰ ਖਾਲੀ * (*dll_rb_ia64_bsp) (ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_init_ਢੇਰ)(ਮੁੱਲ_3*, ਖਾਲੀ*);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_init_ਢੇਰ)(ਮੁੱਲ_3*);
# endif
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_int2big)(SIGNED_ਮੁੱਲ);

ਸਥਿਰ ਖਾਲੀ (*dll_ruby_script) (ਅਚਲ ਅੱਖਰ*);
ਸਥਿਰ ਪੂਰਨ (*dll_rb_enc_ਲੱਭੋ_ਸੂਚਕਾਂਕ) (ਅਚਲ ਅੱਖਰ*);
ਸਥਿਰ rb_encoding* (*dll_rb_enc_ਲੱਭੋ) (ਅਚਲ ਅੱਖਰ*);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_enc_str_new) (ਅਚਲ ਅੱਖਰ*, ਲੰਮਾ, rb_encoding*);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_sprintf) (ਅਚਲ ਅੱਖਰ*, ...);
ਸਥਿਰ ਮੁੱਲ_3 (*dll_rb_require) (ਅਚਲ ਅੱਖਰ*);
ਸਥਿਰ ਖਾਲੀ* (*dll_ruby_options)(ਪੂਰਨ, ਅੱਖਰ**);

# ਜੇ defined(USE_RGENGC) && USE_RGENGC
#  ਜੇ RUBY_ਸੰਸਕਰਣ == 21
ਸਥਿਰ ਖਾਲੀ (*dll_rb_gc_writebarrier_unprotect_promoted)(ਮੁੱਲ_3);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ (*dll_rb_gc_writebarrier_unprotect)(ਮੁੱਲ_3 obj);
#  endif
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30
#  ifdef _MSC_VER
ਸਥਿਰ ਖਾਲੀ (*dll_ruby_malloc_ਮਾਪ_ਉਛਾਲ)(size_t, size_t);
#  ਨਹੀਂਤਾਂ
NORETURN(ਸਥਿਰ ਖਾਲੀ (*dll_ruby_malloc_ਮਾਪ_ਉਛਾਲ)(size_t, size_t));
#  endif
# endif

# ਜੇ RUBY_ਸੰਸਕਰਣ >= 26 && RUBY_ਸੰਸਕਰਣ <= 32
ਖਾਲੀ rb_ary_detransient_stub(ਮੁੱਲ_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 22
    ਲੰਮਾ
rb_num2long_stub(ਮੁੱਲ_3 x)
#  ਨਹੀਂਤਾਂ
    SIGNED_ਮੁੱਲ
rb_num2long_stub(ਮੁੱਲ_3 x)
#  endif
{
    ਵਾਪਸ dll_rb_num2long(x);
}
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 26
    ਮੁੱਲ_3
rb_int2big_stub(intptr_t x)
#  ਨਹੀਂਤਾਂ
    ਮੁੱਲ_3
rb_int2big_stub(SIGNED_ਮੁੱਲ x)
#  endif
{
    ਵਾਪਸ dll_rb_int2big(x);
}
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    ਲੰਮਾ
rb_fix2int_stub(ਮੁੱਲ_3 x)
{
    ਵਾਪਸ dll_rb_fix2int(x);
}
    ਲੰਮਾ
rb_num2int_stub(ਮੁੱਲ_3 x)
{
    ਵਾਪਸ dll_rb_num2int(x);
}
#  endif
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
    ਮੁੱਲ_3
rb_float_new_ਵਿੱਚ_ਢੇਰ(ਦੁੱਗਣਾ d)
{
    ਵਾਪਸ dll_rb_float_new(d);
}
#   ਜੇ RUBY_ਸੰਸਕਰਣ >= 22
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ
rb_num2ulong(ਮੁੱਲ_3 x)
#   ਨਹੀਂਤਾਂ
    ਮੁੱਲ_3
rb_num2ulong(ਮੁੱਲ_3 x)
#   endif
{
    ਵਾਪਸ (ਲੰਮਾ)RSHIFT((SIGNED_ਮੁੱਲ)(x),1);
}
#  endif
#  ਜੇ defined(USE_RGENGC) && USE_RGENGC
#   ਜੇ RUBY_ਸੰਸਕਰਣ == 21
    ਖਾਲੀ
rb_gc_writebarrier_unprotect_promoted_stub(ਮੁੱਲ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ਨਹੀਂਤਾਂ
    ਖਾਲੀ
rb_gc_writebarrier_unprotect_stub(ਮੁੱਲ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 26 && RUBY_ਸੰਸਕਰਣ <= 32
    ਖਾਲੀ
rb_ary_detransient_stub(ਮੁੱਲ_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 30
    ਖਾਲੀ
rb_check_type_stub(ਮੁੱਲ_3 obj, ਪੂਰਨ t)
{
    dll_rb_check_type(obj, t);
}
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ
rb_num2uint_stub(ਮੁੱਲ_3 x)
{
    ਵਾਪਸ dll_rb_num2uint(x);
}
    ਖਾਲੀ
ruby_malloc_ਮਾਪ_ਉਛਾਲ_stub(size_t x, size_t y)
{
    dll_ruby_malloc_ਮਾਪ_ਉਛਾਲ(x, y);
}
#  endif
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
    ਖਾਲੀ
rb_debug_rstring_null_ptr_stub(ਅਚਲ ਅੱਖਰ *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    ਖਾਲੀ
rb_unexpected_type_stub(ਮੁੱਲ_3 self, ਪੂਰਨ t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
ਖਾਲੀ *rb_check_typeddata_stub(ਮੁੱਲ_3 obj, ਅਚਲ rb_ਅੰਕੜੇ_type_t *ਅੰਕੜੇ_type)
{
    ਵਾਪਸ dll_rb_check_typeddata(obj, ਅੰਕੜੇ_type);
}
#  endif
# endif // ifndef PROTO

ਸਥਿਰ HINSTANCE hinstRuby = ਨਿਰਾ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ਸਥਿਰ ਬਣਤਰ
{
    ਅੱਖਰ *name;
    RUBY_PROC *ptr;
} ruby_funcname_ਸਾਰਣੀ[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# ਜੇ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ਨਹੀਂਤਾਂ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_ਚਿੰਨ੍ਹ},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_ਰਸਤਾ},
# ifdef USE_TYPEDDATA
#  ਜੇ RUBY_ਸੰਸਕਰਣ >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_ਅੰਕੜੇ_typed_object_wrap},
#  ਨਹੀਂਤਾਂ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_ਅੰਕੜੇ_typed_object_alloc},
#  endif
# ਨਹੀਂਤਾਂ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_ਅੰਕੜੇ_object_alloc},
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_ਵਿਧੀ},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_ਵਿਧੀ},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_ਸੂਚਕਾਂਕ_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ਨਹੀਂਤਾਂ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_ਸੈੱਟ},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_ਲੱਦੋ},
    {"rb_num2long", (RUBY_PROC*)&dll_rb_num2long},
    {"rb_num2ulong", (RUBY_PROC*)&dll_rb_num2ulong},
    {"rb_obj_alloc", (RUBY_PROC*)&dll_rb_obj_alloc},
    {"rb_obj_as_string", (RUBY_PROC*)&dll_rb_obj_as_string},
    {"rb_obj_id", (RUBY_PROC*)&dll_rb_obj_id},
    {"rb_raise", (RUBY_PROC*)&dll_rb_raise},
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_ਮੁੱਲ},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# ਨਹੀਂਤਾਂ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_ਮੁੱਲ_ptr},
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ਨਹੀਂਤਾਂ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ਨਹੀਂਤਾਂ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ਧੱਕੋ},
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 26 && RUBY_ਸੰਸਕਰਣ <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_ਲੱਭੋ_ਸੂਚਕਾਂਕ},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_ਲੱਭੋ},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ਢੇਰ},
# ਜੇ defined(USE_RGENGC) && USE_RGENGC
#  ਜੇ RUBY_ਸੰਸਕਰਣ == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ਨਹੀਂਤਾਂ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_ਮਾਪ_ਉਛਾਲ},
# endif
    {"", ਨਿਰਾ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
ruby_runtime_link_init(ਅੱਖਰ *libname, ਪੂਰਨ verbose)
{
    ਪੂਰਨ i;

    ਜੇ (hinstRuby)
	ਵਾਪਸ OK;
    hinstRuby = ਲੱਦੋ_dll(libname);
    ਜੇ (!hinstRuby)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }

    ਲਈ (i = 0; ruby_funcname_ਸਾਰਣੀ[i].ptr; ++i)
    {
	ਜੇ (!(*ruby_funcname_ਸਾਰਣੀ[i].ptr = ਚਿੰਨ੍ਹ_from_dll(hinstRuby,
			ruby_funcname_ਸਾਰਣੀ[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ਨਿਰਾ;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), ruby_funcname_ਸਾਰਣੀ[i].name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    ਪੂਰਨ
ruby_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ ruby_runtime_link_init((ਅੱਖਰ *)p_rubydll, verbose) == OK;
}
#ਜੇ_ਅੰਤ // defined(DYNAMIC_RUBY)

    ਖਾਲੀ
ruby_ਸਿਰਾ(ਖਾਲੀ)
{
}

    ਖਾਲੀ
ex_ruby(exarg_T *eap)
{
    ਪੂਰਨ state;
    ਅੱਖਰ *script = ਨਿਰਾ;

    script = (ਅੱਖਰ *)script_get(eap, eap->arg);
    ਜੇ (!eap->skip && ensure_ruby_initialized())
    {
	ਜੇ (script == ਨਿਰਾ)
	    rb_eval_string_protect((ਅੱਖਰ *)eap->arg, &state);
	ਨਹੀਂਤਾਂ
	    rb_eval_string_protect(script, &state);
	ਜੇ (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ਸਥਿਰ ਮੁੱਲ_3
vim_str2rb_enc_str(ਅਚਲ ਅੱਖਰ *s)
{
    ਲੰਮਾ lval;
    char_u *sval;
    rb_encoding *enc;

    ਜੇ (get_option_ਮੁੱਲ((char_u *)"enc", &lval, &sval, ਨਿਰਾ, 0) == gov_string)
    {
	enc = rb_enc_ਲੱਭੋ((ਅੱਖਰ *)sval);
	vim_free(sval);
	ਜੇ (enc)
	    ਵਾਪਸ rb_enc_str_new(s, (ਲੰਮਾ)ਲੜੀ_ਲੰਬਾਈ(s), enc);
    }
    ਵਾਪਸ rb_str_new2(s);
}

    ਸਥਿਰ ਮੁੱਲ_3
eval_enc_string_protect(ਅਚਲ ਅੱਖਰ *str, ਪੂਰਨ *state)
{
    ਲੰਮਾ lval;
    char_u *sval;
    rb_encoding *enc;
    ਮੁੱਲ_3 v;

    ਜੇ (get_option_ਮੁੱਲ((char_u *)"enc", &lval, &sval, ਨਿਰਾ, 0) == gov_string)
    {
	enc = rb_enc_ਲੱਭੋ((ਅੱਖਰ *)sval);
	vim_free(sval);
	ਜੇ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ਵਾਪਸ rb_eval_string_protect(String_ਮੁੱਲ_Ptr(v), state);
	}
    }
    ਵਾਪਸ rb_eval_string_protect(str, state);
}

    ਖਾਲੀ
ex_rubydo(exarg_T *eap)
{
    ਪੂਰਨ state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ਜੇ (!ensure_ruby_initialized())
	ਵਾਪਸ;

    ਜੇ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ਵਾਪਸ;
    ਲਈ (i = eap->line1; i <= eap->line2; i++)
    {
	ਮੁੱਲ_3 ਸਤਰ;

	ਜੇ (i > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
	ਸਤਰ = vim_str2rb_enc_str((ਅੱਖਰ *)ml_get(i));
	rb_lastline_ਸੈੱਟ(ਸਤਰ);
	eval_enc_string_protect((ਅੱਖਰ *) eap->arg, &state);
	ਜੇ (state)
	{
	    error_print(state);
	    ਰੋਕੋ;
	}
	ਜੇ (was_curbuf != curbuf || i > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;
	ਸਤਰ = rb_lastline_get();
	ਜੇ (!NIL_P(ਸਤਰ))
	{
	    ਜੇ (TYPE(ਸਤਰ) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_ਦਾ_string));
		ਵਾਪਸ;
	    }
	    ml_replace(i, (char_u *) String_ਮੁੱਲ_Ptr(ਸਤਰ), 1);
	    ਬਦਲਿਆ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYNTAX_HL
	    syn_ਬਦਲਿਆ(i); // recompute syntax hl. for this line
#ਜੇ_ਅੰਤ
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ਸਥਿਰ ਮੁੱਲ_3
rb_ਲੱਦੋ_wrap(ਮੁੱਲ_3 file_ਮੰਜ਼ਿਲ_ਲੱਦੋ)
{
    rb_ਲੱਦੋ(file_ਮੰਜ਼ਿਲ_ਲੱਦੋ, 0);
    ਵਾਪਸ Qnil;
}

    ਖਾਲੀ
ex_rubyfile(exarg_T *eap)
{
    ਪੂਰਨ state;

    ਜੇ (!ensure_ruby_initialized())
	ਵਾਪਸ;

    ਮੁੱਲ_3 file_ਮੰਜ਼ਿਲ_ਲੱਦੋ = rb_str_new2((ਅਚਲ ਅੱਖਰ *)eap->arg);
    rb_protect(rb_ਲੱਦੋ_wrap, file_ਮੰਜ਼ਿਲ_ਲੱਦੋ, &state);
    ਜੇ (state)
	error_print(state);
}

    ਖਾਲੀ
ruby_ਬਫਰ_free(buf_T *buf)
{
    ਜੇ (buf->b_ruby_ref == ਨਿਰਾ)
	ਵਾਪਸ;

    rb_hash_aset(objtbl, rb_obj_id((ਮੁੱਲ_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->ਅੰਕੜੇ = ਨਿਰਾ;
}

    ਖਾਲੀ
ruby_ਖਿੜਕੀ_free(win_T *win)
{
    ਜੇ (win->w_ruby_ref == ਨਿਰਾ)
	ਵਾਪਸ;

    rb_hash_aset(objtbl, rb_obj_id((ਮੁੱਲ_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->ਅੰਕੜੇ = ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
ensure_ruby_initialized(ਖਾਲੀ)
{
    ਜੇ (ruby_initialized)
	ਵਾਪਸ ruby_initialized;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
    ਜੇ (ruby_enabled(TRUE))
#ਜੇ_ਅੰਤ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// suggested by Ariya Mizutani
	ਪੂਰਨ argc = 1;
	ਅੱਖਰ *argv[] = {"gvim.exe"};
	ਅੱਖਰ **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#ਜੇ_ਅੰਤ
	{
	    ruby_init_ਢੇਰ(ruby_ਢੇਰ_ਸ਼ੁਰੂ);
	    ruby_init();
	}
	{
	    ਪੂਰਨ dummy_argc = 2;
	    ਅੱਖਰ *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
    ਨਹੀਂਤਾਂ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ਵਾਪਸ 0;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ruby_initialized;
}

    ਸਥਿਰ ਖਾਲੀ
error_print(ਪੂਰਨ state)
{
    ਮੁੱਲ_3 error;
    ਮੁੱਲ_3 eclass;
    ਮੁੱਲ_3 einfo;
    ਮੁੱਲ_3 bt;
    ਪੂਰਨ attr;
    ਅੱਖਰ buff[BUFSIZ];
    ਲੰਮਾ i;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_RETURN	0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_BREAK	0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_ਅਗਲਾ	0x3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_RETRY	0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_REDO	0x5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_RAISE	0x6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_THROW	0x7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_FATAL	0x8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_ਨਕਾਬ	0xf

    ਚੋਣ (state)
    {
	ਹਾਲਤ ਟੈਗ_RETURN:
	    emsg(_(e_unexpected_return));
	    ਰੋਕੋ;
	ਹਾਲਤ ਟੈਗ_ਅਗਲਾ:
	    emsg(_(e_unexpected_ਅਗਲਾ));
	    ਰੋਕੋ;
	ਹਾਲਤ ਟੈਗ_BREAK:
	    emsg(_(e_unexpected_break));
	    ਰੋਕੋ;
	ਹਾਲਤ ਟੈਗ_REDO:
	    emsg(_(e_unexpected_redo));
	    ਰੋਕੋ;
	ਹਾਲਤ ਟੈਗ_RETRY:
	    emsg(_(e_retry_outside_ਦਾ_rescue_clause));
	    ਰੋਕੋ;
	ਹਾਲਤ ਟੈਗ_RAISE:
	ਹਾਲਤ ਟੈਗ_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_ਦਾ(error);
	    einfo = rb_obj_as_string(error);
	    ਜੇ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਮੁੱਲ_3 epath;
		ਅੱਖਰ *p;

		epath = rb_class_ਰਸਤਾ(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ਜੇ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ਜੇ RUBY_ਸੰਸਕਰਣ >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    ਲਈ (i = 0; i < RARRAY_LEN(bt); i++)
		ਸੁਨੇਹਾ_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ਨਹੀਂਤਾਂ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    ਲਈ (i = 0; i < RARRAY_LEN(bt); i++)
		ਸੁਨੇਹਾ_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਮੂਲ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_ਸਥਿਤੀ_nr), state);
	    emsg(buff);
	    ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਮੁੱਲ_3
vim_ਸੁਨੇਹਾ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
    ਅੱਖਰ *buff, *p;

    str = rb_obj_as_string(str);
    ਜੇ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(ਅੱਖਰ, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ਜੇ (p) *p = '\0';
	ਸੁਨੇਹਾ(buff);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ("");
    }
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
vim_ਸੈੱਟ_option(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
    do_ਸੈੱਟ((char_u *)String_ਮੁੱਲ_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
vim_command(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
    do_cmdline_cmd((char_u *)String_ਮੁੱਲ_Ptr(str));
    ਵਾਪਸ Qnil;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸਥਿਰ ਮੁੱਲ_3
vim_ਮੰਜ਼ਿਲ_ruby(typval_T *tv)
{
    ਮੁੱਲ_3 ਨਤੀਜਾ = Qnil;

    ਜੇ (tv->v_type == VAR_STRING)
    {
	ਨਤੀਜਾ = rb_str_new2(tv->vval.v_string == ਨਿਰਾ
					  ? "" : (ਅੱਖਰ *)(tv->vval.v_string));
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_ਸੰਖਿਆ)
    {
	ਨਤੀਜਾ = INT2_ਅੰਕ(tv->vval.v_ਸੰਖਿਆ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_FLOAT)
    {
	ਨਤੀਜਾ = rb_float_new(tv->vval.v_float);
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	ਨਤੀਜਾ = rb_ary_new();

	ਜੇ (list != ਨਿਰਾ)
	{
	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, curr)
		rb_ary_ਧੱਕੋ(ਨਤੀਜਾ, vim_ਮੰਜ਼ਿਲ_ruby(&curr->li_tv));
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_DICT)
    {
	ਨਤੀਜਾ = rb_hash_new();

	ਜੇ (tv->vval.v_dict != ਨਿਰਾ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_ਵਰਤਿਆ;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
	    {
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(ਨਤੀਜਾ, rb_str_new2((ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ),
						     vim_ਮੰਜ਼ਿਲ_ruby(&di->di_tv));
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ਜੇ (tv->vval.v_ਸੰਖਿਆ == VVAL_TRUE)
	    ਨਤੀਜਾ = Qtrue;
	ਨਹੀਂਤਾਂ ਜੇ (tv->vval.v_ਸੰਖਿਆ == VVAL_FALSE)
	    ਨਤੀਜਾ = Qfalse;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_BLOB)
    {
	ਨਤੀਜਾ = rb_str_new(tv->vval.v_blob->bv_ga.ga_ਅੰਕੜੇ,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਮੁੱਲ_3
vim_evaluate(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    typval_T    *tv;
    ਮੁੱਲ_3       ਨਤੀਜਾ;

    tv = eval_ਵਿਅੰਜਕ((char_u *)String_ਮੁੱਲ_Ptr(str), ਨਿਰਾ);
    ਜੇ (tv == ਨਿਰਾ)
	ਵਾਪਸ Qnil;
    ਨਤੀਜਾ = vim_ਮੰਜ਼ਿਲ_ruby(tv);

    free_tv(tv);

    ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ Qnil;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
ਸਥਿਰ size_t ਬਫਰ_dsize(ਅਚਲ ਖਾਲੀ *buf);

ਸਥਿਰ ਅਚਲ rb_ਅੰਕੜੇ_type_t ਬਫਰ_type = {
    "vim_buffer",
    {0, 0, ਬਫਰ_dsize,
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 27
	0, {0}
# ਨਹੀਂਤਾਂ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ਸਥਿਰ size_t
ਬਫਰ_dsize(ਅਚਲ ਖਾਲੀ *buf ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਆਕਾਰ(buf_T);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_new(buf_T *buf)
{
    ਜੇ (buf->b_ruby_ref)
    {
	ਵਾਪਸ (ਮੁੱਲ_3) buf->b_ruby_ref;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
	ਮੁੱਲ_3 obj = Typed_ਅੰਕੜੇ_Wrap_Struct(c_ਬਫਰ, &ਬਫਰ_type, buf);
#ਨਹੀਂਤਾਂ
	ਮੁੱਲ_3 obj = ਅੰਕੜੇ_Wrap_Struct(c_ਬਫਰ, 0, 0, buf);
#ਜੇ_ਅੰਤ
	buf->b_ruby_ref = (ਖਾਲੀ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ਵਾਪਸ obj;
    }
}

    ਸਥਿਰ buf_T *
get_buf(ਮੁੱਲ_3 obj)
{
    buf_T *buf;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
    Typed_ਅੰਕੜੇ_Get_Struct(obj, buf_T, &ਬਫਰ_type, buf);
#ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ_Get_Struct(obj, buf_T, buf);
#ਜੇ_ਅੰਤ
    ਜੇ (buf == ਨਿਰਾ)
	rb_raise(e_Deleted_ਬਫਰ_Error, "attempt to refer to deleted buffer");
    ਵਾਪਸ buf;
}

    ਸਥਿਰ ਮੁੱਲ_3
vim_blob(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
    ਮੁੱਲ_3 ਨਤੀਜਾ = rb_str_new("0z", 2);
    ਅੱਖਰ    buf[4];
    ਪੂਰਨ	i;
    ਲਈ (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(RSTRING_PTR(str)[i]));
	rb_str_concat(ਨਤੀਜਾ, rb_str_new2(buf));
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_s_current(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਬਫਰ_new(curbuf);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_s_current_getter(ID id ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 *x ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਬਫਰ_new(curbuf);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_s_ਗਿਣਤੀ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    buf_T *b;
    ਪੂਰਨ n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ਜੇ (b->b_p_bl)
	    n++;
    }

    ਵਾਪਸ INT2_ਅੰਕ(n);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_s_aref(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 ਅੰਕ)
{
    buf_T *b;
    ਪੂਰਨ n = NUM2INT(ਅੰਕ);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ਜੇ (!b->b_p_bl)
	    ਜਾਰੀ;

	ਜੇ (n == 0)
	    ਵਾਪਸ ਬਫਰ_new(b);

	n--;
    }
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_name(ਮੁੱਲ_3 self)
{
    buf_T *buf = get_buf(self);

    ਵਾਪਸ buf->b_ffname ? rb_str_new2((ਅੱਖਰ *)buf->b_ffname) : Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_ਸੰਖਿਆ(ਮੁੱਲ_3 self)
{
    buf_T *buf = get_buf(self);

    ਵਾਪਸ INT2_ਅੰਕ(buf->b_fnum);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_ਗਿਣਤੀ(ਮੁੱਲ_3 self)
{
    buf_T *buf = get_buf(self);

    ਵਾਪਸ INT2_ਅੰਕ(buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

    ਸਥਿਰ ਮੁੱਲ_3
get_ਬਫਰ_ਸਤਰ(buf_T *buf, linenr_T n)
{
    ਜੇ (n <= 0 || n > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	rb_raise(rb_e_ਸੂਚਕਾਂਕ_Error, "line number %ld out of range", (ਲੰਮਾ)n);
    ਵਾਪਸ vim_str2rb_enc_str((ਅੱਖਰ *)ml_get_buf(buf, n, FALSE));
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_aref(ਮੁੱਲ_3 self, ਮੁੱਲ_3 ਅੰਕ)
{
    buf_T *buf = get_buf(self);

    ਜੇ (buf != ਨਿਰਾ)
	ਵਾਪਸ get_ਬਫਰ_ਸਤਰ(buf, (linenr_T)NUM2LONG(ਅੰਕ));
    ਵਾਪਸ Qnil; // For stop warning
}

    ਸਥਿਰ ਮੁੱਲ_3
ਸੈੱਟ_ਬਫਰ_ਸਤਰ(buf_T *buf, linenr_T n, ਮੁੱਲ_3 str)
{
    ਅੱਖਰ	*ਸਤਰ = String_ਮੁੱਲ_Ptr(str);
    aco_save_T	aco;

    ਜੇ (n > 0 && n <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ && ਸਤਰ != ਨਿਰਾ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ਜੇ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)ਸਤਰ, TRUE);
		ਬਦਲਿਆ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYNTAX_HL
		syn_ਬਦਲਿਆ(n); // recompute syntax hl. for this line
#ਜੇ_ਅੰਤ
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	rb_raise(rb_e_ਸੂਚਕਾਂਕ_Error, "line number %ld out of range", (ਲੰਮਾ)n);
    }
    ਵਾਪਸ str;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_aset(ਮੁੱਲ_3 self, ਮੁੱਲ_3 ਅੰਕ, ਮੁੱਲ_3 str)
{
    buf_T *buf = get_buf(self);

    ਜੇ (buf != ਨਿਰਾ)
	ਵਾਪਸ ਸੈੱਟ_ਬਫਰ_ਸਤਰ(buf, (linenr_T)NUM2LONG(ਅੰਕ), str);
    ਵਾਪਸ str;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_delete(ਮੁੱਲ_3 self, ਮੁੱਲ_3 ਅੰਕ)
{
    buf_T	*buf = get_buf(self);
    ਲੰਮਾ	n = NUM2LONG(ਅੰਕ);
    aco_save_T	aco;

    ਜੇ (n > 0 && n <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ਜੇ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_ਸਤਰਾਂ_mark(n, 1L);

		ਬਦਲਿਆ();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	rb_raise(rb_e_ਸੂਚਕਾਂਕ_Error, "line number %ld out of range", n);
    }
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਬਫਰ_append(ਮੁੱਲ_3 self, ਮੁੱਲ_3 ਅੰਕ, ਮੁੱਲ_3 str)
{
    buf_T	*buf = get_buf(self);
    ਅੱਖਰ	*ਸਤਰ = String_ਮੁੱਲ_Ptr(str);
    ਲੰਮਾ	n = NUM2LONG(ਅੰਕ);
    aco_save_T	aco;

    ਜੇ (ਸਤਰ == ਨਿਰਾ)
    {
	rb_raise(rb_e_ਸੂਚਕਾਂਕ_Error, "NULL line");
    }
    ਨਹੀਂਤਾਂ ਜੇ (n >= 0 && n <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ਜੇ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) ਸਤਰ, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_ਸਤਰਾਂ_mark(n, 1L);

		ਬਦਲਿਆ();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	rb_raise(rb_e_ਸੂਚਕਾਂਕ_Error, "line number %ld out of range", n);
    }
    ਵਾਪਸ str;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
ਸਥਿਰ size_t ਖਿੜਕੀ_dsize(ਅਚਲ ਖਾਲੀ *buf);

ਸਥਿਰ ਅਚਲ rb_ਅੰਕੜੇ_type_t ਖਿੜਕੀ_type = {
    "vim_window",
    {0, 0, ਖਿੜਕੀ_dsize,
# ਜੇ RUBY_ਸੰਸਕਰਣ >= 27
	0, {0}
# ਨਹੀਂਤਾਂ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ਸਥਿਰ size_t
ਖਿੜਕੀ_dsize(ਅਚਲ ਖਾਲੀ *win ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਆਕਾਰ(win_T);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_new(win_T *win)
{
    ਜੇ (win->w_ruby_ref)
    {
	ਵਾਪਸ (ਮੁੱਲ_3) win->w_ruby_ref;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
	ਮੁੱਲ_3 obj = Typed_ਅੰਕੜੇ_Wrap_Struct(c_Vim_ਖਿੜਕੀ, &ਖਿੜਕੀ_type, win);
#ਨਹੀਂਤਾਂ
	ਮੁੱਲ_3 obj = ਅੰਕੜੇ_Wrap_Struct(c_Vim_ਖਿੜਕੀ, 0, 0, win);
#ਜੇ_ਅੰਤ
	win->w_ruby_ref = (ਖਾਲੀ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ਵਾਪਸ obj;
    }
}

    ਸਥਿਰ win_T *
get_win(ਮੁੱਲ_3 obj)
{
    win_T *win;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_TYPEDDATA
    Typed_ਅੰਕੜੇ_Get_Struct(obj, win_T, &ਖਿੜਕੀ_type, win);
#ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ_Get_Struct(obj, win_T, win);
#ਜੇ_ਅੰਤ
    ਜੇ (win == ਨਿਰਾ)
	rb_raise(e_Deleted_ਖਿੜਕੀ_Error, "attempt to refer to deleted window");
    ਵਾਪਸ win;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_s_current(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਖਿੜਕੀ_new(curwin);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_s_current_getter(ID id ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 *x ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਖਿੜਕੀ_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ਸਥਿਰ ਮੁੱਲ_3
ਸਤਰ_s_current(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ get_ਬਫਰ_ਸਤਰ(curbuf, curwin->w_cursor.lnum);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਸੈੱਟ_current_ਸਤਰ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 str)
{
    ਵਾਪਸ ਸੈੱਟ_ਬਫਰ_ਸਤਰ(curbuf, curwin->w_cursor.lnum, str);
}

    ਸਥਿਰ ਮੁੱਲ_3
current_ਸਤਰ_ਸੰਖਿਆ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ INT2FIX((ਪੂਰਨ)curwin->w_cursor.lnum);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_s_ਗਿਣਤੀ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    win_T	*w;
    ਪੂਰਨ n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ਵਾਪਸ INT2_ਅੰਕ(n);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_s_aref(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 ਅੰਕ)
{
    win_T *w;
    ਪੂਰਨ n = NUM2INT(ਅੰਕ);

    ਲਈ (w = firstwin; w != ਨਿਰਾ; w = w->w_ਅਗਲਾ, --n)
	ਜੇ (n == 0)
	    ਵਾਪਸ ਖਿੜਕੀ_new(w);
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਬਫਰ(ਮੁੱਲ_3 self)
{
    win_T *win = get_win(self);

    ਵਾਪਸ ਬਫਰ_new(win->w_ਬਫਰ);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਉਚਾਈ(ਮੁੱਲ_3 self)
{
    win_T *win = get_win(self);

    ਵਾਪਸ INT2_ਅੰਕ(win->w_ਉਚਾਈ);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਸੈੱਟ_ਉਚਾਈ(ਮੁੱਲ_3 self, ਮੁੱਲ_3 ਉਚਾਈ)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(ਉਚਾਈ));
    curwin = savewin;
    ਵਾਪਸ ਉਚਾਈ;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਚੌੜਾਈ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ INT2_ਅੰਕ(get_win(self)->w_ਚੌੜਾਈ);
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਸੈੱਟ_ਚੌੜਾਈ(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ, ਮੁੱਲ_3 ਚੌੜਾਈ)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(ਚੌੜਾਈ));
    curwin = savewin;
    ਵਾਪਸ ਚੌੜਾਈ;
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_cursor(ਮੁੱਲ_3 self)
{
    win_T *win = get_win(self);

    ਵਾਪਸ rb_assoc_new(INT2_ਅੰਕ(win->w_cursor.lnum), INT2_ਅੰਕ(win->w_cursor.ਖਾਨਾ));
}

    ਸਥਿਰ ਮੁੱਲ_3
ਖਿੜਕੀ_ਸੈੱਟ_cursor(ਮੁੱਲ_3 self, ਮੁੱਲ_3 pos)
{
    ਮੁੱਲ_3 lnum, ਖਾਨਾ;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ਜੇ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    ਖਾਨਾ = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.ਖਾਨਾ = NUM2UINT(ਖਾਨਾ);
    win->w_ਸੈੱਟ_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
f_nop(ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ Qnil;
}

    ਸਥਿਰ ਮੁੱਲ_3
f_p(ਪੂਰਨ argc, ਮੁੱਲ_3 *argv, ਮੁੱਲ_3 self ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ i;
    ਮੁੱਲ_3 str = rb_str_new("", 0);
    ਮੁੱਲ_3 ret = Qnil;

    ਲਈ (i = 0; i < argc; i++)
    {
	ਜੇ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    ਸੁਨੇਹਾ(RSTRING_PTR(str));

    ਜੇ (argc == 1)
	ret = argv[0];
    ਨਹੀਂਤਾਂ ਜੇ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਖਾਲੀ
ruby_io_init(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DYNAMIC_RUBY
    RUBYEXTERN ਮੁੱਲ_3 rb_stdout;
    RUBYEXTERN ਮੁੱਲ_3 rb_stderr;
#ਜੇ_ਅੰਤ

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_ਵਿਧੀ(rb_stdout, "write", (ਖਾਲੀ*)vim_ਸੁਨੇਹਾ, 1);
    rb_define_singleton_ਵਿਧੀ(rb_stdout, "flush", (ਖਾਲੀ*)f_nop, 0);
    rb_define_singleton_ਵਿਧੀ(rb_stderr, "write", (ਖਾਲੀ*)vim_ਸੁਨੇਹਾ, 1);
    rb_define_singleton_ਵਿਧੀ(rb_stderr, "flush", (ਖਾਲੀ*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ਸਥਿਰ ਖਾਲੀ
ruby_vim_init(ਖਾਲੀ)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_ਅੰਕ(VIM_ਸੰਸਕਰਣ_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_ਅੰਕ(VIM_ਸੰਸਕਰਣ_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_ਅੰਕ(VIM_ਸੰਸਕਰਣ_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_ਅੰਕ(VIM_ਸੰਸਕਰਣ_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_ਸੰਸਕਰਣ_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_ਸੰਸਕਰਣ_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_ਸੰਸਕਰਣ_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_ਸੰਸਕਰਣ_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_ਸੁਨੇਹਾ, 1);
    rb_define_module_function(mVIM, "set_option", vim_ਸੈੱਟ_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_ਬਫਰ_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_ਖਿੜਕੀ_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_ਬਫਰ = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_ਵਿਧੀ(c_ਬਫਰ, "current", (ਖਾਲੀ*)ਬਫਰ_s_current, 0);
    rb_define_singleton_ਵਿਧੀ(c_ਬਫਰ, "count", (ਖਾਲੀ*)ਬਫਰ_s_ਗਿਣਤੀ, 0);
    rb_define_singleton_ਵਿਧੀ(c_ਬਫਰ, "[]", (ਖਾਲੀ*)ਬਫਰ_s_aref, 1);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "name", (ਖਾਲੀ*)ਬਫਰ_name, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "number", (ਖਾਲੀ*)ਬਫਰ_ਸੰਖਿਆ, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "count", (ਖਾਲੀ*)ਬਫਰ_ਗਿਣਤੀ, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "length", (ਖਾਲੀ*)ਬਫਰ_ਗਿਣਤੀ, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "[]", (ਖਾਲੀ*)ਬਫਰ_aref, 1);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "[]=", (ਖਾਲੀ*)ਬਫਰ_aset, 2);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "delete", (ਖਾਲੀ*)ਬਫਰ_delete, 1);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "append", (ਖਾਲੀ*)ਬਫਰ_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "line_number", (ਖਾਲੀ*)current_ਸਤਰ_ਸੰਖਿਆ, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "line", (ਖਾਲੀ*)ਸਤਰ_s_current, 0);
    rb_define_ਵਿਧੀ(c_ਬਫਰ, "line=", (ਖਾਲੀ*)ਸੈੱਟ_current_ਸਤਰ, 1);


    c_Vim_ਖਿੜਕੀ = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "current", (ਖਾਲੀ*)ਖਿੜਕੀ_s_current, 0);
    rb_define_singleton_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "count", (ਖਾਲੀ*)ਖਿੜਕੀ_s_ਗਿਣਤੀ, 0);
    rb_define_singleton_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "[]", (ਖਾਲੀ*)ਖਿੜਕੀ_s_aref, 1);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "buffer", (ਖਾਲੀ*)ਖਿੜਕੀ_ਬਫਰ, 0);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "height", (ਖਾਲੀ*)ਖਿੜਕੀ_ਉਚਾਈ, 0);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "height=", (ਖਾਲੀ*)ਖਿੜਕੀ_ਸੈੱਟ_ਉਚਾਈ, 1);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "width", (ਖਾਲੀ*)ਖਿੜਕੀ_ਚੌੜਾਈ, 0);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "width=", (ਖਾਲੀ*)ਖਿੜਕੀ_ਸੈੱਟ_ਚੌੜਾਈ, 1);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "cursor", (ਖਾਲੀ*)ਖਿੜਕੀ_cursor, 0);
    rb_define_ਵਿਧੀ(c_Vim_ਖਿੜਕੀ, "cursor=", (ਖਾਲੀ*)ਖਿੜਕੀ_ਸੈੱਟ_cursor, 1);

    rb_define_virtual_variable("$curbuf", ਬਫਰ_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", ਖਿੜਕੀ_s_current_getter, 0);
}

    ਖਾਲੀ
vim_ruby_init(ਖਾਲੀ *ਢੇਰ_ਸ਼ੁਰੂ)
{
    // should get machine stack start address early in main function
    ruby_ਢੇਰ_ਸ਼ੁਰੂ = ਢੇਰ_ਸ਼ੁਰੂ;
}

    ਸਥਿਰ ਪੂਰਨ
convert_hash2dict(ਮੁੱਲ_3 ਕੁੰਜੀ, ਮੁੱਲ_3 val, ਮੁੱਲ_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ਕੁੰਜੀ)));
    ਜੇ (di == ਨਿਰਾ || ruby_convert_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ(val, &di->di_tv) != OK
						     || dict_ਜੋੜੋ(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ਵਾਪਸ ST_STOP;
    }
    ਵਾਪਸ ST_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
ruby_convert_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ(ਮੁੱਲ_3 val, typval_T *rettv)
{
    ਚੋਣ (TYPE(val))
    {
	ਹਾਲਤ T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
	    ਰੋਕੋ;
	ਹਾਲਤ T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
	    ਰੋਕੋ;
	ਹਾਲਤ T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
	    ਰੋਕੋ;
	ਹਾਲਤ T_BIGNUM:
	ਹਾਲਤ T_FIXNUM:
	    rettv->v_type = VAR_ਸੰਖਿਆ;
	    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)NUM2LONG(val);
	    ਰੋਕੋ;
	ਹਾਲਤ T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    ਰੋਕੋ;
	ਮੂਲ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	ਹਾਲਤ T_STRING:
	    {
		ਮੁੱਲ_3 str = (ਮੁੱਲ_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ T_ARRAY:
	    {
		list_T *l;
		ਲੰਮਾ i;
		typval_T v;

		l = list_alloc();
		ਜੇ (l == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		ਲਈ (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ਜੇ (ruby_convert_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ((ਮੁੱਲ_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ਵਾਪਸ FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		ਜੇ (d == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (ਮੁੱਲ_3)d);
		ਜੇ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ਵਾਪਸ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
do_rubyeval(char_u *str, typval_T *rettv)
{
    ਪੂਰਨ retval = FAIL;

    ਜੇ (ensure_ruby_initialized())
    {
	ਪੂਰਨ state;
	ਮੁੱਲ_3 obj;

	obj = rb_eval_string_protect((ਅਚਲ ਅੱਖਰ *)str, &state);
	ਜੇ (state)
	    error_print(state);
	ਨਹੀਂਤਾਂ
	    retval = ruby_convert_ਮੰਜ਼ਿਲ_vim_ਮੁੱਲ(obj, rettv);
    }
    ਜੇ (retval == FAIL)
    {
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = 0;
    }
}
