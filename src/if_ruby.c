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

#شامل "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#شامل <stdio.h>
#شامل <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN بیرونی
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# اگر RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# اگر defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# اگر RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_نماد		(*dll_rb_c_نماد)
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

# اگر RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# اگر RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# اگر RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# اگر RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# اگر RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# اگر RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# اگر RUBY_VERSION >= 31
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

#اگر defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic افزودن
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#اگر defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#شامل <ruby.h>
#شامل <ruby/encoding.h>

#اگر defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic برداشت
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#اگر defined(MACOS_X)
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
#اگر defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_مقدار_Ptr
# define String_مقدار_Ptr(s) STR2CSTR(s)
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

#شامل "vim.h"
#شامل "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define نماد_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# وگرنه
#  include <dlfcn.h>
#  define HINSTANCE تهی*
#  define RUBY_PROC تهی*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define نماد_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

ایستا صحیح ruby_initialized = 0;
ایستا تهی *ruby_پشته_start;
ایستا مقدار_3 objtbl;

ایستا مقدار_3 mVIM;
ایستا مقدار_3 cBuffer;
ایستا مقدار_3 cVimWindow;
ایستا مقدار_3 eDeletedBufferError;
ایستا مقدار_3 eDeletedWindowError;

ایستا صحیح ensure_ruby_initialized(تهی);
ایستا تهی error_print(صحیح);
ایستا تهی ruby_io_init(تهی);
ایستا تهی ruby_vim_init(تهی);
ایستا صحیح ruby_convert_to_vim_مقدار(مقدار_3 val, typval_T *rettv);

#اگر defined(__ia64) && !defined(ruby_init_پشته)
# define ruby_init_پشته(addr) ruby_init_پشته((addr), rb_ia64_bsp())
#endif

#اگر defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# اگر RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# اگر RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  اگر RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  وگرنه
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# وگرنه
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
# اگر RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# وگرنه
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

# اگر VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  اگر RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# اگر RUBY_VERSION < 20
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
# وگرنه
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_مقدار		dll_rb_string_مقدار
# define rb_string_مقدار_ptr		dll_rb_string_مقدار_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_افزودن			dll_rb_ary_افزودن
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_پشته
#  define ruby_init_پشته(addr)	dll_ruby_init_پشته((addr), rb_ia64_bsp())
# وگرنه
#  define ruby_init_پشته		dll_ruby_init_پشته
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
ایستا مقدار_3 (*dll_rb_assoc_new) (مقدار_3, مقدار_3);
مقدار_3 *dll_rb_cFalseClass;
مقدار_3 *dll_rb_cFixnum;
# اگر defined(USE_RUBY_INTEGER)
مقدار_3 *dll_rb_cInteger;
# endif
# اگر RUBY_VERSION >= 20
مقدار_3 *dll_rb_cFloat;
# endif
مقدار_3 *dll_rb_cNilClass;
ایستا مقدار_3 *dll_rb_cObject;
مقدار_3 *dll_rb_cString;
مقدار_3 *dll_rb_c_نماد;
مقدار_3 *dll_rb_cTrueClass;
ایستا مقدار_3 (*dll_rb_class_new_instance) (صحیح,مقدار_3*,مقدار_3);
ایستا تهی (*dll_rb_check_type) (مقدار_3,صحیح);
# ifdef USE_TYPEDDATA
ایستا تهی *(*dll_rb_check_typeddata) (مقدار_3,ثابت rb_data_type_t *);
# endif
ایستا مقدار_3 (*dll_rb_class_path) (مقدار_3);
# ifdef USE_TYPEDDATA
#  اگر RUBY_VERSION >= 23
ایستا مقدار_3 (*dll_rb_data_typed_object_wrap) (مقدار_3, تهی*, ثابت rb_data_type_t *);
#  وگرنه
ایستا مقدار_3 (*dll_rb_data_typed_object_alloc) (مقدار_3, تهی*, ثابت rb_data_type_t *);
#  endif
# وگرنه
ایستا مقدار_3 (*dll_rb_data_object_alloc) (مقدار_3, تهی*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# اگر RUBY_VERSION >= 31
ایستا تهی (*dll_rb_debug_rstring_null_ptr) (ثابت نویسه*);
# endif
ایستا مقدار_3 (*dll_rb_define_class_under) (مقدار_3, ثابت نویسه*, مقدار_3);
ایستا تهی (*dll_rb_define_const) (مقدار_3,ثابت نویسه*,مقدار_3);
ایستا تهی (*dll_rb_define_global_function) (ثابت نویسه*,مقدار_3(*)(صحیح,مقدار_3*,مقدار_3),صحیح);
ایستا تهی (*dll_rb_define_method) (مقدار_3,ثابت نویسه*,مقدار_3(*)(ANYARGS),صحیح);
ایستا مقدار_3 (*dll_rb_define_module) (ثابت نویسه*);
ایستا تهی (*dll_rb_define_module_function) (مقدار_3,ثابت نویسه*,مقدار_3(*)(مقدار_3,مقدار_3),صحیح);
ایستا تهی (*dll_rb_define_singleton_method) (مقدار_3,ثابت نویسه*,مقدار_3(*)(ANYARGS),صحیح);
ایستا تهی (*dll_rb_define_virtual_variable) (ثابت نویسه*,مقدار_3(*)(ID,مقدار_3*),تهی(*)());
ایستا مقدار_3 *dll_rb_stdout;
ایستا مقدار_3 *dll_rb_stderr;
ایستا مقدار_3 *dll_rb_eArgError;
ایستا مقدار_3 *dll_rb_eIndexError;
ایستا مقدار_3 *dll_rb_eRuntimeError;
ایستا مقدار_3 *dll_rb_eStandardError;
ایستا مقدار_3 (*dll_rb_eval_string_protect) (ثابت نویسه*, صحیح*);
# اگر RUBY_VERSION >= 21
ایستا مقدار_3 (*dll_rb_funcallv) (مقدار_3, ID, صحیح, ثابت مقدار_3*);
# وگرنه
ایستا مقدار_3 (*dll_rb_funcall2) (مقدار_3, ID, صحیح, ثابت مقدار_3*);
# endif
ایستا تهی (*dll_rb_global_variable) (مقدار_3*);
ایستا مقدار_3 (*dll_rb_hash_aset) (مقدار_3, مقدار_3, مقدار_3);
ایستا مقدار_3 (*dll_rb_hash_foreach) (مقدار_3, صحیح (*)(مقدار_3, مقدار_3, مقدار_3), مقدار_3);
ایستا مقدار_3 (*dll_rb_hash_new) (تهی);
ایستا مقدار_3 (*dll_rb_inspect) (مقدار_3);
ایستا مقدار_3 (*dll_rb_int2inum) (long);
ایستا ID (*dll_rb_intern) (ثابت نویسه*);
# اگر RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ایستا long (*dll_rb_fix2int) (مقدار_3);
ایستا long (*dll_rb_num2int) (مقدار_3);
ایستا unsigned long (*dll_rb_num2uint) (مقدار_3);
# endif
ایستا دوگانه (*dll_rb_num2dbl) (مقدار_3);
ایستا مقدار_3 (*dll_rb_lastline_get) (تهی);
ایستا تهی (*dll_rb_lastline_set) (مقدار_3);
ایستا مقدار_3 (*dll_rb_protect) (مقدار_3 (*)(مقدار_3), مقدار_3, صحیح*);
ایستا تهی (*dll_rb_load) (مقدار_3, صحیح);
ایستا long (*dll_rb_num2long) (مقدار_3);
ایستا unsigned long (*dll_rb_num2ulong) (مقدار_3);
ایستا مقدار_3 (*dll_rb_obj_alloc) (مقدار_3);
ایستا مقدار_3 (*dll_rb_obj_as_string) (مقدار_3);
ایستا مقدار_3 (*dll_rb_obj_id) (مقدار_3);
ایستا تهی (*dll_rb_raise) (مقدار_3, ثابت نویسه*, ...);
ایستا مقدار_3 (*dll_rb_string_مقدار) (فرار مقدار_3*);
ایستا مقدار_3 (*dll_rb_str_cat) (مقدار_3, ثابت نویسه*, long);
ایستا مقدار_3 (*dll_rb_str_concat) (مقدار_3, مقدار_3);
ایستا مقدار_3 (*dll_rb_str_new) (ثابت نویسه*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ایستا مقدار_3 (*dll_rb_str_new_cstr) (ثابت نویسه*);
# وگرنه
ایستا مقدار_3 (*dll_rb_str_new2) (ثابت نویسه*);
# endif
ایستا مقدار_3 (*dll_rb_errinfo) (تهی);
ایستا تهی (*dll_ruby_init) (تهی);
ایستا تهی (*dll_ruby_init_loadpath) (تهی);
# ifdef MSWIN
ایستا تهی (*dll_ruby_sysinit) (صحیح*, نویسه***);
ایستا صحیح (*dll_rb_w32_snprintf)(نویسه*, size_t, ثابت نویسه*, ...);
# endif
# اگر RUBY_VERSION >= 31
#  ifdef _MSC_VER
ایستا تهی (*dll_rb_unexpected_type) (مقدار_3, صحیح);
#  وگرنه
NORETURN(ایستا تهی (*dll_rb_unexpected_type) (مقدار_3, صحیح));
#  endif
# endif
ایستا نویسه * (*dll_rb_string_مقدار_ptr) (فرار مقدار_3*);
ایستا مقدار_3 (*dll_rb_float_new) (دوگانه);
ایستا مقدار_3 (*dll_rb_ary_new) (تهی);
ایستا مقدار_3 (*dll_rb_ary_new4) (long n, ثابت مقدار_3 *elts);
ایستا مقدار_3 (*dll_rb_ary_افزودن) (مقدار_3, مقدار_3);
# اگر RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ایستا تهی (*dll_rb_ary_detransient) (مقدار_3);
# endif
# ifdef __ia64
ایستا تهی * (*dll_rb_ia64_bsp) (تهی);
ایستا تهی (*dll_ruby_init_پشته)(مقدار_3*, تهی*);
# وگرنه
ایستا تهی (*dll_ruby_init_پشته)(مقدار_3*);
# endif
ایستا مقدار_3 (*dll_rb_int2big)(SIGNED_مقدار);

ایستا تهی (*dll_ruby_script) (ثابت نویسه*);
ایستا صحیح (*dll_rb_enc_find_index) (ثابت نویسه*);
ایستا rb_encoding* (*dll_rb_enc_find) (ثابت نویسه*);
ایستا مقدار_3 (*dll_rb_enc_str_new) (ثابت نویسه*, long, rb_encoding*);
ایستا مقدار_3 (*dll_rb_sprintf) (ثابت نویسه*, ...);
ایستا مقدار_3 (*dll_rb_require) (ثابت نویسه*);
ایستا تهی* (*dll_ruby_options)(صحیح, نویسه**);

# اگر defined(USE_RGENGC) && USE_RGENGC
#  اگر RUBY_VERSION == 21
ایستا تهی (*dll_rb_gc_writebarrier_unprotect_promoted)(مقدار_3);
#  وگرنه
ایستا تهی (*dll_rb_gc_writebarrier_unprotect)(مقدار_3 obj);
#  endif
# endif

# اگر RUBY_VERSION >= 30
#  ifdef _MSC_VER
ایستا تهی (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  وگرنه
NORETURN(ایستا تهی (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# اگر RUBY_VERSION >= 26 && RUBY_VERSION <= 32
تهی rb_ary_detransient_stub(مقدار_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  اگر RUBY_VERSION >= 22
    long
rb_num2long_stub(مقدار_3 x)
#  وگرنه
    SIGNED_مقدار
rb_num2long_stub(مقدار_3 x)
#  endif
{
    بازگشت dll_rb_num2long(x);
}
#  اگر RUBY_VERSION >= 26
    مقدار_3
rb_int2big_stub(intptr_t x)
#  وگرنه
    مقدار_3
rb_int2big_stub(SIGNED_مقدار x)
#  endif
{
    بازگشت dll_rb_int2big(x);
}
#  اگر RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(مقدار_3 x)
{
    بازگشت dll_rb_fix2int(x);
}
    long
rb_num2int_stub(مقدار_3 x)
{
    بازگشت dll_rb_num2int(x);
}
#  endif
#  اگر RUBY_VERSION >= 20
    مقدار_3
rb_float_new_in_heap(دوگانه d)
{
    بازگشت dll_rb_float_new(d);
}
#   اگر RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(مقدار_3 x)
#   وگرنه
    مقدار_3
rb_num2ulong(مقدار_3 x)
#   endif
{
    بازگشت (long)RSHIFT((SIGNED_مقدار)(x),1);
}
#  endif
#  اگر defined(USE_RGENGC) && USE_RGENGC
#   اگر RUBY_VERSION == 21
    تهی
rb_gc_writebarrier_unprotect_promoted_stub(مقدار_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   وگرنه
    تهی
rb_gc_writebarrier_unprotect_stub(مقدار_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  اگر RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    تهی
rb_ary_detransient_stub(مقدار_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  اگر RUBY_VERSION >= 30
    تهی
rb_check_type_stub(مقدار_3 obj, صحیح t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(مقدار_3 x)
{
    بازگشت dll_rb_num2uint(x);
}
    تهی
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  اگر RUBY_VERSION >= 31
    تهی
rb_debug_rstring_null_ptr_stub(ثابت نویسه *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    تهی
rb_unexpected_type_stub(مقدار_3 self, صحیح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
تهی *rb_check_typeddata_stub(مقدار_3 obj, ثابت rb_data_type_t *data_type)
{
    بازگشت dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

ایستا HINSTANCE hinstRuby = پوچ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ایستا ساختار
{
    نویسه *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# اگر defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# وگرنه
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# اگر RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_نماد},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  اگر RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  وگرنه
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# وگرنه
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# اگر RUBY_VERSION >= 31
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
# اگر RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# وگرنه
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# اگر RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_مقدار},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# وگرنه
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# اگر RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_مقدار_ptr},
# اگر RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# وگرنه
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# وگرنه
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_افزودن},
# اگر RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_پشته},
# اگر defined(USE_RGENGC) && USE_RGENGC
#  اگر RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  وگرنه
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# اگر RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", پوچ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ایستا صحیح
ruby_runtime_link_init(نویسه *libname, صحیح verbose)
{
    صحیح i;

    اگر (hinstRuby)
	بازگشت OK;
    hinstRuby = load_dll(libname);
    اگر (!hinstRuby)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	بازگشت FAIL;
    }

    برای (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	اگر (!(*ruby_funcname_table[i].ptr = نماد_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = پوچ;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    صحیح
ruby_enabled(صحیح verbose)
{
    بازگشت ruby_runtime_link_init((نویسه *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    تهی
ruby_end(تهی)
{
}

    تهی
ex_ruby(exarg_T *eap)
{
    صحیح state;
    نویسه *script = پوچ;

    script = (نویسه *)script_get(eap, eap->arg);
    اگر (!eap->skip && ensure_ruby_initialized())
    {
	اگر (script == پوچ)
	    rb_eval_string_protect((نویسه *)eap->arg, &state);
	وگرنه
	    rb_eval_string_protect(script, &state);
	اگر (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ایستا مقدار_3
vim_str2rb_enc_str(ثابت نویسه *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    اگر (get_option_مقدار((char_u *)"enc", &lval, &sval, پوچ, 0) == gov_string)
    {
	enc = rb_enc_find((نویسه *)sval);
	vim_free(sval);
	اگر (enc)
	    بازگشت rb_enc_str_new(s, (long)strlen(s), enc);
    }
    بازگشت rb_str_new2(s);
}

    ایستا مقدار_3
eval_enc_string_protect(ثابت نویسه *str, صحیح *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    مقدار_3 v;

    اگر (get_option_مقدار((char_u *)"enc", &lval, &sval, پوچ, 0) == gov_string)
    {
	enc = rb_enc_find((نویسه *)sval);
	vim_free(sval);
	اگر (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    بازگشت rb_eval_string_protect(String_مقدار_Ptr(v), state);
	}
    }
    بازگشت rb_eval_string_protect(str, state);
}

    تهی
ex_rubydo(exarg_T *eap)
{
    صحیح state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    اگر (!ensure_ruby_initialized())
	بازگشت;

    اگر (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	بازگشت;
    برای (i = eap->line1; i <= eap->line2; i++)
    {
	مقدار_3 سطر;

	اگر (i > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;
	سطر = vim_str2rb_enc_str((نویسه *)ml_get(i));
	rb_lastline_set(سطر);
	eval_enc_string_protect((نویسه *) eap->arg, &state);
	اگر (state)
	{
	    error_print(state);
	    بشکن;
	}
	اگر (was_curbuf != curbuf || i > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;
	سطر = rb_lastline_get();
	اگر (!NIL_P(سطر))
	{
	    اگر (TYPE(سطر) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		بازگشت;
	    }
	    ml_replace(i, (char_u *) String_مقدار_Ptr(سطر), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ایستا مقدار_3
rb_load_wrap(مقدار_3 file_to_load)
{
    rb_load(file_to_load, 0);
    بازگشت Qnil;
}

    تهی
ex_rubyfile(exarg_T *eap)
{
    صحیح state;

    اگر (!ensure_ruby_initialized())
	بازگشت;

    مقدار_3 file_to_load = rb_str_new2((ثابت نویسه *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    اگر (state)
	error_print(state);
}

    تهی
ruby_buffer_free(buf_T *buf)
{
    اگر (buf->b_ruby_ref == پوچ)
	بازگشت;

    rb_hash_aset(objtbl, rb_obj_id((مقدار_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = پوچ;
}

    تهی
ruby_window_free(win_T *win)
{
    اگر (win->w_ruby_ref == پوچ)
	بازگشت;

    rb_hash_aset(objtbl, rb_obj_id((مقدار_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = پوچ;
}

    ایستا صحیح
ensure_ruby_initialized(تهی)
{
    اگر (ruby_initialized)
	بازگشت ruby_initialized;

#ifdef DYNAMIC_RUBY
    اگر (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	صحیح argc = 1;
	نویسه *argv[] = {"gvim.exe"};
	نویسه **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_پشته(ruby_پشته_start);
	    ruby_init();
	}
	{
	    صحیح dummy_argc = 2;
	    نویسه *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    وگرنه
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	بازگشت 0;
    }
#endif

    بازگشت ruby_initialized;
}

    ایستا تهی
error_print(صحیح state)
{
    مقدار_3 error;
    مقدار_3 eclass;
    مقدار_3 einfo;
    مقدار_3 bt;
    صحیح attr;
    نویسه buff[BUFSIZ];
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

    انتخاب (state)
    {
	حالت TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    بشکن;
	حالت TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    بشکن;
	حالت TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    بشکن;
	حالت TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    بشکن;
	حالت TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    بشکن;
	حالت TAG_RAISE:
	حالت TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    اگر (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    وگرنه
	    {
		مقدار_3 epath;
		نویسه *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		اگر (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#اگر RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    برای (i = 0; i < RARRAY_LEN(bt); i++)
		پیام_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#وگرنه
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    برای (i = 0; i < RARRAY_LEN(bt); i++)
		پیام_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    بشکن;
	پیشفرض:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    بشکن;
    }
}

    ایستا مقدار_3
vim_message(مقدار_3 self UNUSED, مقدار_3 str)
{
    نویسه *buff, *p;

    str = rb_obj_as_string(str);
    اگر (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(نویسه, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	اگر (p) *p = '\0';
	پیام(buff);
    }
    وگرنه
    {
	پیام("");
    }
    بازگشت Qnil;
}

    ایستا مقدار_3
vim_set_option(مقدار_3 self UNUSED, مقدار_3 str)
{
    do_set((char_u *)String_مقدار_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    بازگشت Qnil;
}

    ایستا مقدار_3
vim_command(مقدار_3 self UNUSED, مقدار_3 str)
{
    do_cmdline_cmd((char_u *)String_مقدار_Ptr(str));
    بازگشت Qnil;
}

#ifdef FEAT_EVAL
    ایستا مقدار_3
vim_to_ruby(typval_T *tv)
{
    مقدار_3 result = Qnil;

    اگر (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == پوچ
					  ? "" : (نویسه *)(tv->vval.v_string));
    }
    وگرنه اگر (tv->v_type == VAR_عدد)
    {
	result = INT2_رقم(tv->vval.v_عدد);
    }
    وگرنه اگر (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    وگرنه اگر (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	اگر (list != پوچ)
	{
	    FOR_ALL_LIST_اقلام(list, curr)
		rb_ary_افزودن(result, vim_to_ruby(&curr->li_tv));
	}
    }
    وگرنه اگر (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	اگر (tv->vval.v_dict != پوچ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
	    {
		اگر (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((نویسه *)hi->hi_کلید),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    وگرنه اگر (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	اگر (tv->vval.v_عدد == VVAL_TRUE)
	    result = Qtrue;
	وگرنه اگر (tv->vval.v_عدد == VVAL_FALSE)
	    result = Qfalse;
    }
    وگرنه اگر (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    بازگشت result;
}
#endif

    ایستا مقدار_3
vim_evaluate(مقدار_3 self UNUSED, مقدار_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    مقدار_3       result;

    tv = eval_عبارت((char_u *)String_مقدار_Ptr(str), پوچ);
    اگر (tv == پوچ)
	بازگشت Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    بازگشت result;
#وگرنه
    بازگشت Qnil;
#endif
}

#ifdef USE_TYPEDDATA
ایستا size_t buffer_dsize(ثابت تهی *buf);

ایستا ثابت rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# اگر RUBY_VERSION >= 27
	0, {0}
# وگرنه
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ایستا size_t
buffer_dsize(ثابت تهی *buf UNUSED)
{
    بازگشت اندازه(buf_T);
}
#endif

    ایستا مقدار_3
buffer_new(buf_T *buf)
{
    اگر (buf->b_ruby_ref)
    {
	بازگشت (مقدار_3) buf->b_ruby_ref;
    }
    وگرنه
    {
#ifdef USE_TYPEDDATA
	مقدار_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#وگرنه
	مقدار_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (تهی *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	بازگشت obj;
    }
}

    ایستا buf_T *
get_buf(مقدار_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#وگرنه
    Data_Get_Struct(obj, buf_T, buf);
#endif
    اگر (buf == پوچ)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    بازگشت buf;
}

    ایستا مقدار_3
vim_blob(مقدار_3 self UNUSED, مقدار_3 str)
{
    مقدار_3 result = rb_str_new("0z", 2);
    نویسه    buf[4];
    صحیح	i;
    برای (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned نویسه)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    بازگشت result;
}

    ایستا مقدار_3
buffer_s_current(مقدار_3 self UNUSED)
{
    بازگشت buffer_new(curbuf);
}

    ایستا مقدار_3
buffer_s_current_getter(ID id UNUSED, مقدار_3 *x UNUSED)
{
    بازگشت buffer_new(curbuf);
}

    ایستا مقدار_3
buffer_s_شمار(مقدار_3 self UNUSED)
{
    buf_T *b;
    صحیح n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	اگر (b->b_p_bl)
	    n++;
    }

    بازگشت INT2_رقم(n);
}

    ایستا مقدار_3
buffer_s_aref(مقدار_3 self UNUSED, مقدار_3 رقم)
{
    buf_T *b;
    صحیح n = NUM2INT(رقم);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	اگر (!b->b_p_bl)
	    ادامه;

	اگر (n == 0)
	    بازگشت buffer_new(b);

	n--;
    }
    بازگشت Qnil;
}

    ایستا مقدار_3
buffer_name(مقدار_3 self)
{
    buf_T *buf = get_buf(self);

    بازگشت buf->b_ffname ? rb_str_new2((نویسه *)buf->b_ffname) : Qnil;
}

    ایستا مقدار_3
buffer_عدد(مقدار_3 self)
{
    buf_T *buf = get_buf(self);

    بازگشت INT2_رقم(buf->b_fnum);
}

    ایستا مقدار_3
buffer_شمار(مقدار_3 self)
{
    buf_T *buf = get_buf(self);

    بازگشت INT2_رقم(buf->b_ml.ml_سطر_شمار);
}

    ایستا مقدار_3
get_buffer_سطر(buf_T *buf, linenr_T n)
{
    اگر (n <= 0 || n > buf->b_ml.ml_سطر_شمار)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    بازگشت vim_str2rb_enc_str((نویسه *)ml_get_buf(buf, n, FALSE));
}

    ایستا مقدار_3
buffer_aref(مقدار_3 self, مقدار_3 رقم)
{
    buf_T *buf = get_buf(self);

    اگر (buf != پوچ)
	بازگشت get_buffer_سطر(buf, (linenr_T)NUM2LONG(رقم));
    بازگشت Qnil; // For stop warning
}

    ایستا مقدار_3
set_buffer_سطر(buf_T *buf, linenr_T n, مقدار_3 str)
{
    نویسه	*سطر = String_مقدار_Ptr(str);
    aco_save_T	aco;

    اگر (n > 0 && n <= buf->b_ml.ml_سطر_شمار && سطر != پوچ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    اگر (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)سطر, TRUE);
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
    وگرنه
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    بازگشت str;
}

    ایستا مقدار_3
buffer_aset(مقدار_3 self, مقدار_3 رقم, مقدار_3 str)
{
    buf_T *buf = get_buf(self);

    اگر (buf != پوچ)
	بازگشت set_buffer_سطر(buf, (linenr_T)NUM2LONG(رقم), str);
    بازگشت str;
}

    ایستا مقدار_3
buffer_delete(مقدار_3 self, مقدار_3 رقم)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(رقم);
    aco_save_T	aco;

    اگر (n > 0 && n <= buf->b_ml.ml_سطر_شمار)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    اگر (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_سطرها_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وگرنه
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    بازگشت Qnil;
}

    ایستا مقدار_3
buffer_append(مقدار_3 self, مقدار_3 رقم, مقدار_3 str)
{
    buf_T	*buf = get_buf(self);
    نویسه	*سطر = String_مقدار_Ptr(str);
    long	n = NUM2LONG(رقم);
    aco_save_T	aco;

    اگر (سطر == پوچ)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    وگرنه اگر (n >= 0 && n <= buf->b_ml.ml_سطر_شمار)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    اگر (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) سطر, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_سطرها_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وگرنه
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    بازگشت str;
}

#ifdef USE_TYPEDDATA
ایستا size_t window_dsize(ثابت تهی *buf);

ایستا ثابت rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# اگر RUBY_VERSION >= 27
	0, {0}
# وگرنه
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ایستا size_t
window_dsize(ثابت تهی *win UNUSED)
{
    بازگشت اندازه(win_T);
}
#endif

    ایستا مقدار_3
window_new(win_T *win)
{
    اگر (win->w_ruby_ref)
    {
	بازگشت (مقدار_3) win->w_ruby_ref;
    }
    وگرنه
    {
#ifdef USE_TYPEDDATA
	مقدار_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#وگرنه
	مقدار_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (تهی *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	بازگشت obj;
    }
}

    ایستا win_T *
get_win(مقدار_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#وگرنه
    Data_Get_Struct(obj, win_T, win);
#endif
    اگر (win == پوچ)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    بازگشت win;
}

    ایستا مقدار_3
window_s_current(مقدار_3 self UNUSED)
{
    بازگشت window_new(curwin);
}

    ایستا مقدار_3
window_s_current_getter(ID id UNUSED, مقدار_3 *x UNUSED)
{
    بازگشت window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ایستا مقدار_3
سطر_s_current(مقدار_3 self UNUSED)
{
    بازگشت get_buffer_سطر(curbuf, curwin->w_cursor.lnum);
}

    ایستا مقدار_3
set_current_سطر(مقدار_3 self UNUSED, مقدار_3 str)
{
    بازگشت set_buffer_سطر(curbuf, curwin->w_cursor.lnum, str);
}

    ایستا مقدار_3
current_سطر_عدد(مقدار_3 self UNUSED)
{
    بازگشت INT2FIX((صحیح)curwin->w_cursor.lnum);
}

    ایستا مقدار_3
window_s_شمار(مقدار_3 self UNUSED)
{
    win_T	*w;
    صحیح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    بازگشت INT2_رقم(n);
}

    ایستا مقدار_3
window_s_aref(مقدار_3 self UNUSED, مقدار_3 رقم)
{
    win_T *w;
    صحیح n = NUM2INT(رقم);

    برای (w = firstwin; w != پوچ; w = w->w_next, --n)
	اگر (n == 0)
	    بازگشت window_new(w);
    بازگشت Qnil;
}

    ایستا مقدار_3
window_buffer(مقدار_3 self)
{
    win_T *win = get_win(self);

    بازگشت buffer_new(win->w_buffer);
}

    ایستا مقدار_3
window_height(مقدار_3 self)
{
    win_T *win = get_win(self);

    بازگشت INT2_رقم(win->w_height);
}

    ایستا مقدار_3
window_set_height(مقدار_3 self, مقدار_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    بازگشت height;
}

    ایستا مقدار_3
window_width(مقدار_3 self UNUSED)
{
    بازگشت INT2_رقم(get_win(self)->w_width);
}

    ایستا مقدار_3
window_set_width(مقدار_3 self UNUSED, مقدار_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    بازگشت width;
}

    ایستا مقدار_3
window_cursor(مقدار_3 self)
{
    win_T *win = get_win(self);

    بازگشت rb_assoc_new(INT2_رقم(win->w_cursor.lnum), INT2_رقم(win->w_cursor.col));
}

    ایستا مقدار_3
window_set_cursor(مقدار_3 self, مقدار_3 pos)
{
    مقدار_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    اگر (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    بازگشت Qnil;
}

    ایستا مقدار_3
f_nop(مقدار_3 self UNUSED)
{
    بازگشت Qnil;
}

    ایستا مقدار_3
f_p(صحیح argc, مقدار_3 *argv, مقدار_3 self UNUSED)
{
    صحیح i;
    مقدار_3 str = rb_str_new("", 0);
    مقدار_3 ret = Qnil;

    برای (i = 0; i < argc; i++)
    {
	اگر (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    پیام(RSTRING_PTR(str));

    اگر (argc == 1)
	ret = argv[0];
    وگرنه اگر (argc > 1)
	ret = rb_ary_new4(argc, argv);
    بازگشت ret;
}

    ایستا تهی
ruby_io_init(تهی)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN مقدار_3 rb_stdout;
    RUBYEXTERN مقدار_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (تهی*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (تهی*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (تهی*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (تهی*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ایستا تهی
ruby_vim_init(تهی)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_رقم(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_رقم(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_رقم(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_رقم(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (تهی*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (تهی*)buffer_s_شمار, 0);
    rb_define_singleton_method(cBuffer, "[]", (تهی*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (تهی*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (تهی*)buffer_عدد, 0);
    rb_define_method(cBuffer, "count", (تهی*)buffer_شمار, 0);
    rb_define_method(cBuffer, "length", (تهی*)buffer_شمار, 0);
    rb_define_method(cBuffer, "[]", (تهی*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (تهی*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (تهی*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (تهی*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (تهی*)current_سطر_عدد, 0);
    rb_define_method(cBuffer, "line", (تهی*)سطر_s_current, 0);
    rb_define_method(cBuffer, "line=", (تهی*)set_current_سطر, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (تهی*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (تهی*)window_s_شمار, 0);
    rb_define_singleton_method(cVimWindow, "[]", (تهی*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (تهی*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (تهی*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (تهی*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (تهی*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (تهی*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (تهی*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (تهی*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    تهی
vim_ruby_init(تهی *پشته_start)
{
    // should get machine stack start address early in main function
    ruby_پشته_start = پشته_start;
}

    ایستا صحیح
convert_hash2dict(مقدار_3 کلید, مقدار_3 val, مقدار_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(کلید)));
    اگر (di == پوچ || ruby_convert_to_vim_مقدار(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	بازگشت ST_STOP;
    }
    بازگشت ST_CONTINUE;
}

    ایستا صحیح
ruby_convert_to_vim_مقدار(مقدار_3 val, typval_T *rettv)
{
    انتخاب (TYPE(val))
    {
	حالت T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NULL;
	    بشکن;
	حالت T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = VVAL_TRUE;
	    بشکن;
	حالت T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = VVAL_FALSE;
	    بشکن;
	حالت T_BIGNUM:
	حالت T_FIXNUM:
	    rettv->v_type = VAR_عدد;
	    rettv->vval.v_عدد = (varnumber_T)NUM2LONG(val);
	    بشکن;
	حالت T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    بشکن;
	پیشفرض:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	حالت T_STRING:
	    {
		مقدار_3 str = (مقدار_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    بشکن;
	حالت T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		اگر (l == پوچ)
		    بازگشت FAIL;

		برای (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    اگر (ruby_convert_to_vim_مقدار((مقدار_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			بازگشت FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    بشکن;
	حالت T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		اگر (d == پوچ)
		    بازگشت FAIL;

		rb_hash_foreach(val, convert_hash2dict, (مقدار_3)d);
		اگر (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    بازگشت FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    بشکن;
    }
    بازگشت OK;
}

    تهی
do_rubyeval(char_u *str, typval_T *rettv)
{
    صحیح retval = FAIL;

    اگر (ensure_ruby_initialized())
    {
	صحیح state;
	مقدار_3 obj;

	obj = rb_eval_string_protect((ثابت نویسه *)str, &state);
	اگر (state)
	    error_print(state);
	وگرنه
	    retval = ruby_convert_to_vim_مقدار(obj, rettv);
    }
    اگر (retval == FAIL)
    {
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = 0;
    }
}
