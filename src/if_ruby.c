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

#ضمن "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#ضمن <stdio.h>
#ضمن <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN خارجي
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# إذا RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# إذا defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# إذا RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_رمز		(*dll_rb_c_رمز)
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

# إذا RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# إذا RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# إذا RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# إذا RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# إذا RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# إذا RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# إذا RUBY_VERSION >= 31
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

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ادفع
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#إذا defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#ضمن <ruby.h>
#ضمن <ruby/encoding.h>

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic اسحب
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#إذا defined(MACOS_X)
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
#إذا defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_قيمة_Ptr
# define String_قيمة_Ptr(s) STR2CSTR(s)
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

#ضمن "vim.h"
#ضمن "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define رمز_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define HINSTANCE فراغ*
#  define RUBY_PROC فراغ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define رمز_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

ساكن صحيح ruby_initialized = 0;
ساكن فراغ *ruby_مكدس_start;
ساكن قيمة_3 objtbl;

ساكن قيمة_3 mVIM;
ساكن قيمة_3 cBuffer;
ساكن قيمة_3 cVimWindow;
ساكن قيمة_3 eDeletedBufferError;
ساكن قيمة_3 eDeletedWindowError;

ساكن صحيح ensure_ruby_initialized(فراغ);
ساكن فراغ error_اطبع(صحيح);
ساكن فراغ ruby_io_init(فراغ);
ساكن فراغ ruby_vim_init(فراغ);
ساكن صحيح ruby_convert_to_vim_قيمة(قيمة_3 val, typval_T *rettv);

#إذا defined(__ia64) && !defined(ruby_init_مكدس)
# define ruby_init_مكدس(addr) ruby_init_مكدس((addr), rb_ia64_bsp())
#endif

#إذا defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# إذا RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# إذا RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  إذا RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  وإلا
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# وإلا
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
# إذا RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# وإلا
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

# إذا VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  إذا RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# إذا RUBY_VERSION < 20
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
# وإلا
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_قيمة		dll_rb_string_قيمة
# define rb_string_قيمة_ptr		dll_rb_string_قيمة_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ادفع			dll_rb_ary_ادفع
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_مكدس
#  define ruby_init_مكدس(addr)	dll_ruby_init_مكدس((addr), rb_ia64_bsp())
# وإلا
#  define ruby_init_مكدس		dll_ruby_init_مكدس
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
ساكن قيمة_3 (*dll_rb_assoc_new) (قيمة_3, قيمة_3);
قيمة_3 *dll_rb_cFalseClass;
قيمة_3 *dll_rb_cFixnum;
# إذا defined(USE_RUBY_INTEGER)
قيمة_3 *dll_rb_cInteger;
# endif
# إذا RUBY_VERSION >= 20
قيمة_3 *dll_rb_cFloat;
# endif
قيمة_3 *dll_rb_cNilClass;
ساكن قيمة_3 *dll_rb_cObject;
قيمة_3 *dll_rb_cString;
قيمة_3 *dll_rb_c_رمز;
قيمة_3 *dll_rb_cTrueClass;
ساكن قيمة_3 (*dll_rb_class_new_instance) (صحيح,قيمة_3*,قيمة_3);
ساكن فراغ (*dll_rb_check_type) (قيمة_3,صحيح);
# ifdef USE_TYPEDDATA
ساكن فراغ *(*dll_rb_check_typeddata) (قيمة_3,ثابت rb_data_type_t *);
# endif
ساكن قيمة_3 (*dll_rb_class_path) (قيمة_3);
# ifdef USE_TYPEDDATA
#  إذا RUBY_VERSION >= 23
ساكن قيمة_3 (*dll_rb_data_typed_object_wrap) (قيمة_3, فراغ*, ثابت rb_data_type_t *);
#  وإلا
ساكن قيمة_3 (*dll_rb_data_typed_object_alloc) (قيمة_3, فراغ*, ثابت rb_data_type_t *);
#  endif
# وإلا
ساكن قيمة_3 (*dll_rb_data_object_alloc) (قيمة_3, فراغ*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# إذا RUBY_VERSION >= 31
ساكن فراغ (*dll_rb_debug_rstring_null_ptr) (ثابت حرف*);
# endif
ساكن قيمة_3 (*dll_rb_define_class_under) (قيمة_3, ثابت حرف*, قيمة_3);
ساكن فراغ (*dll_rb_define_const) (قيمة_3,ثابت حرف*,قيمة_3);
ساكن فراغ (*dll_rb_define_global_function) (ثابت حرف*,قيمة_3(*)(صحيح,قيمة_3*,قيمة_3),صحيح);
ساكن فراغ (*dll_rb_define_method) (قيمة_3,ثابت حرف*,قيمة_3(*)(ANYARGS),صحيح);
ساكن قيمة_3 (*dll_rb_define_module) (ثابت حرف*);
ساكن فراغ (*dll_rb_define_module_function) (قيمة_3,ثابت حرف*,قيمة_3(*)(قيمة_3,قيمة_3),صحيح);
ساكن فراغ (*dll_rb_define_singleton_method) (قيمة_3,ثابت حرف*,قيمة_3(*)(ANYARGS),صحيح);
ساكن فراغ (*dll_rb_define_virtual_variable) (ثابت حرف*,قيمة_3(*)(ID,قيمة_3*),فراغ(*)());
ساكن قيمة_3 *dll_rb_stdout;
ساكن قيمة_3 *dll_rb_stderr;
ساكن قيمة_3 *dll_rb_eArgError;
ساكن قيمة_3 *dll_rb_eIndexError;
ساكن قيمة_3 *dll_rb_eRuntimeError;
ساكن قيمة_3 *dll_rb_eStandardError;
ساكن قيمة_3 (*dll_rb_eval_string_protect) (ثابت حرف*, صحيح*);
# إذا RUBY_VERSION >= 21
ساكن قيمة_3 (*dll_rb_funcallv) (قيمة_3, ID, صحيح, ثابت قيمة_3*);
# وإلا
ساكن قيمة_3 (*dll_rb_funcall2) (قيمة_3, ID, صحيح, ثابت قيمة_3*);
# endif
ساكن فراغ (*dll_rb_global_variable) (قيمة_3*);
ساكن قيمة_3 (*dll_rb_hash_aset) (قيمة_3, قيمة_3, قيمة_3);
ساكن قيمة_3 (*dll_rb_hash_foreach) (قيمة_3, صحيح (*)(قيمة_3, قيمة_3, قيمة_3), قيمة_3);
ساكن قيمة_3 (*dll_rb_hash_new) (فراغ);
ساكن قيمة_3 (*dll_rb_inspect) (قيمة_3);
ساكن قيمة_3 (*dll_rb_int2inum) (long);
ساكن ID (*dll_rb_intern) (ثابت حرف*);
# إذا RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ساكن long (*dll_rb_fix2int) (قيمة_3);
ساكن long (*dll_rb_num2int) (قيمة_3);
ساكن unsigned long (*dll_rb_num2uint) (قيمة_3);
# endif
ساكن مزدوج (*dll_rb_num2dbl) (قيمة_3);
ساكن قيمة_3 (*dll_rb_lastline_get) (فراغ);
ساكن فراغ (*dll_rb_lastline_set) (قيمة_3);
ساكن قيمة_3 (*dll_rb_protect) (قيمة_3 (*)(قيمة_3), قيمة_3, صحيح*);
ساكن فراغ (*dll_rb_load) (قيمة_3, صحيح);
ساكن long (*dll_rb_num2long) (قيمة_3);
ساكن unsigned long (*dll_rb_num2ulong) (قيمة_3);
ساكن قيمة_3 (*dll_rb_obj_alloc) (قيمة_3);
ساكن قيمة_3 (*dll_rb_obj_as_string) (قيمة_3);
ساكن قيمة_3 (*dll_rb_obj_id) (قيمة_3);
ساكن فراغ (*dll_rb_raise) (قيمة_3, ثابت حرف*, ...);
ساكن قيمة_3 (*dll_rb_string_قيمة) (متطاير قيمة_3*);
ساكن قيمة_3 (*dll_rb_str_cat) (قيمة_3, ثابت حرف*, long);
ساكن قيمة_3 (*dll_rb_str_concat) (قيمة_3, قيمة_3);
ساكن قيمة_3 (*dll_rb_str_new) (ثابت حرف*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ساكن قيمة_3 (*dll_rb_str_new_cstr) (ثابت حرف*);
# وإلا
ساكن قيمة_3 (*dll_rb_str_new2) (ثابت حرف*);
# endif
ساكن قيمة_3 (*dll_rb_errinfo) (فراغ);
ساكن فراغ (*dll_ruby_init) (فراغ);
ساكن فراغ (*dll_ruby_init_loadpath) (فراغ);
# ifdef MSWIN
ساكن فراغ (*dll_ruby_sysinit) (صحيح*, حرف***);
ساكن صحيح (*dll_rb_w32_snprintf)(حرف*, size_t, ثابت حرف*, ...);
# endif
# إذا RUBY_VERSION >= 31
#  ifdef _MSC_VER
ساكن فراغ (*dll_rb_unexpected_type) (قيمة_3, صحيح);
#  وإلا
NORETURN(ساكن فراغ (*dll_rb_unexpected_type) (قيمة_3, صحيح));
#  endif
# endif
ساكن حرف * (*dll_rb_string_قيمة_ptr) (متطاير قيمة_3*);
ساكن قيمة_3 (*dll_rb_float_new) (مزدوج);
ساكن قيمة_3 (*dll_rb_ary_new) (فراغ);
ساكن قيمة_3 (*dll_rb_ary_new4) (long n, ثابت قيمة_3 *elts);
ساكن قيمة_3 (*dll_rb_ary_ادفع) (قيمة_3, قيمة_3);
# إذا RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ساكن فراغ (*dll_rb_ary_detransient) (قيمة_3);
# endif
# ifdef __ia64
ساكن فراغ * (*dll_rb_ia64_bsp) (فراغ);
ساكن فراغ (*dll_ruby_init_مكدس)(قيمة_3*, فراغ*);
# وإلا
ساكن فراغ (*dll_ruby_init_مكدس)(قيمة_3*);
# endif
ساكن قيمة_3 (*dll_rb_int2big)(SIGNED_قيمة);

ساكن فراغ (*dll_ruby_script) (ثابت حرف*);
ساكن صحيح (*dll_rb_enc_find_index) (ثابت حرف*);
ساكن rb_encoding* (*dll_rb_enc_find) (ثابت حرف*);
ساكن قيمة_3 (*dll_rb_enc_str_new) (ثابت حرف*, long, rb_encoding*);
ساكن قيمة_3 (*dll_rb_sprintf) (ثابت حرف*, ...);
ساكن قيمة_3 (*dll_rb_require) (ثابت حرف*);
ساكن فراغ* (*dll_ruby_options)(صحيح, حرف**);

# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_VERSION == 21
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect_promoted)(قيمة_3);
#  وإلا
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect)(قيمة_3 obj);
#  endif
# endif

# إذا RUBY_VERSION >= 30
#  ifdef _MSC_VER
ساكن فراغ (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  وإلا
NORETURN(ساكن فراغ (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# إذا RUBY_VERSION >= 26 && RUBY_VERSION <= 32
فراغ rb_ary_detransient_stub(قيمة_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  إذا RUBY_VERSION >= 22
    long
rb_num2long_stub(قيمة_3 x)
#  وإلا
    SIGNED_قيمة
rb_num2long_stub(قيمة_3 x)
#  endif
{
    أرجع dll_rb_num2long(x);
}
#  إذا RUBY_VERSION >= 26
    قيمة_3
rb_int2big_stub(intptr_t x)
#  وإلا
    قيمة_3
rb_int2big_stub(SIGNED_قيمة x)
#  endif
{
    أرجع dll_rb_int2big(x);
}
#  إذا RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(قيمة_3 x)
{
    أرجع dll_rb_fix2int(x);
}
    long
rb_num2int_stub(قيمة_3 x)
{
    أرجع dll_rb_num2int(x);
}
#  endif
#  إذا RUBY_VERSION >= 20
    قيمة_3
rb_float_new_in_heap(مزدوج d)
{
    أرجع dll_rb_float_new(d);
}
#   إذا RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(قيمة_3 x)
#   وإلا
    قيمة_3
rb_num2ulong(قيمة_3 x)
#   endif
{
    أرجع (long)RSHIFT((SIGNED_قيمة)(x),1);
}
#  endif
#  إذا defined(USE_RGENGC) && USE_RGENGC
#   إذا RUBY_VERSION == 21
    فراغ
rb_gc_writebarrier_unprotect_promoted_stub(قيمة_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   وإلا
    فراغ
rb_gc_writebarrier_unprotect_stub(قيمة_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  إذا RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    فراغ
rb_ary_detransient_stub(قيمة_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  إذا RUBY_VERSION >= 30
    فراغ
rb_check_type_stub(قيمة_3 obj, صحيح t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(قيمة_3 x)
{
    أرجع dll_rb_num2uint(x);
}
    فراغ
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  إذا RUBY_VERSION >= 31
    فراغ
rb_debug_rstring_null_ptr_stub(ثابت حرف *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    فراغ
rb_unexpected_type_stub(قيمة_3 self, صحيح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
فراغ *rb_check_typeddata_stub(قيمة_3 obj, ثابت rb_data_type_t *data_type)
{
    أرجع dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

ساكن HINSTANCE hinstRuby = عدم; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ساكن بنية
{
    حرف *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# إذا defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# وإلا
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# إذا RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_رمز},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  إذا RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  وإلا
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# وإلا
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# إذا RUBY_VERSION >= 31
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
# إذا RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# وإلا
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# إذا RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_قيمة},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# وإلا
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# إذا RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_قيمة_ptr},
# إذا RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# وإلا
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# وإلا
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ادفع},
# إذا RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_مكدس},
# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  وإلا
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# إذا RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", عدم},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساكن صحيح
ruby_runtime_link_init(حرف *libname, صحيح verbose)
{
    صحيح i;

    إذا (hinstRuby)
	أرجع OK;
    hinstRuby = load_dll(libname);
    إذا (!hinstRuby)
    {
	إذا (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	إذا (!(*ruby_funcname_table[i].ptr = رمز_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    صحيح
ruby_enabled(صحيح verbose)
{
    أرجع ruby_runtime_link_init((حرف *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    فراغ
ruby_end(فراغ)
{
}

    فراغ
ex_ruby(exarg_T *eap)
{
    صحيح state;
    حرف *script = عدم;

    script = (حرف *)script_get(eap, eap->arg);
    إذا (!eap->skip && ensure_ruby_initialized())
    {
	إذا (script == عدم)
	    rb_eval_string_protect((حرف *)eap->arg, &state);
	وإلا
	    rb_eval_string_protect(script, &state);
	إذا (state)
	    error_اطبع(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ساكن قيمة_3
vim_str2rb_enc_str(ثابت حرف *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    إذا (get_option_قيمة((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_find((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	    أرجع rb_enc_str_new(s, (long)strlen(s), enc);
    }
    أرجع rb_str_new2(s);
}

    ساكن قيمة_3
eval_enc_string_protect(ثابت حرف *str, صحيح *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    قيمة_3 v;

    إذا (get_option_قيمة((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_find((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    أرجع rb_eval_string_protect(String_قيمة_Ptr(v), state);
	}
    }
    أرجع rb_eval_string_protect(str, state);
}

    فراغ
ex_rubydo(exarg_T *eap)
{
    صحيح state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    إذا (!ensure_ruby_initialized())
	أرجع;

    إذا (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	أرجع;
    لكل (i = eap->line1; i <= eap->line2; i++)
    {
	قيمة_3 سطر;

	إذا (i > curbuf->b_ml.ml_سطر_عدد)
	    اكسر;
	سطر = vim_str2rb_enc_str((حرف *)ml_get(i));
	rb_lastline_set(سطر);
	eval_enc_string_protect((حرف *) eap->arg, &state);
	إذا (state)
	{
	    error_اطبع(state);
	    اكسر;
	}
	إذا (was_curbuf != curbuf || i > curbuf->b_ml.ml_سطر_عدد)
	    اكسر;
	سطر = rb_lastline_get();
	إذا (!NIL_P(سطر))
	{
	    إذا (TYPE(سطر) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		أرجع;
	    }
	    ml_replace(i, (char_u *) String_قيمة_Ptr(سطر), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن قيمة_3
rb_load_wrap(قيمة_3 file_to_load)
{
    rb_load(file_to_load, 0);
    أرجع Qnil;
}

    فراغ
ex_rubyfile(exarg_T *eap)
{
    صحيح state;

    إذا (!ensure_ruby_initialized())
	أرجع;

    قيمة_3 file_to_load = rb_str_new2((ثابت حرف *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    إذا (state)
	error_اطبع(state);
}

    فراغ
ruby_buffer_free(buf_T *buf)
{
    إذا (buf->b_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((قيمة_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = عدم;
}

    فراغ
ruby_window_free(win_T *win)
{
    إذا (win->w_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((قيمة_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = عدم;
}

    ساكن صحيح
ensure_ruby_initialized(فراغ)
{
    إذا (ruby_initialized)
	أرجع ruby_initialized;

#ifdef DYNAMIC_RUBY
    إذا (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	صحيح argc = 1;
	حرف *argv[] = {"gvim.exe"};
	حرف **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_مكدس(ruby_مكدس_start);
	    ruby_init();
	}
	{
	    صحيح dummy_argc = 2;
	    حرف *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    وإلا
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	أرجع 0;
    }
#endif

    أرجع ruby_initialized;
}

    ساكن فراغ
error_اطبع(صحيح state)
{
    قيمة_3 error;
    قيمة_3 eclass;
    قيمة_3 einfo;
    قيمة_3 bt;
    صحيح attr;
    حرف buff[BUFSIZ];
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

    بدل (state)
    {
	حالة TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    اكسر;
	حالة TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    اكسر;
	حالة TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    اكسر;
	حالة TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    اكسر;
	حالة TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    اكسر;
	حالة TAG_RAISE:
	حالة TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    إذا (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    وإلا
	    {
		قيمة_3 epath;
		حرف *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		إذا (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#إذا RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		رسالة_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#وإلا
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		رسالة_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    اكسر;
	افتراضي:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    اكسر;
    }
}

    ساكن قيمة_3
vim_message(قيمة_3 self UNUSED, قيمة_3 str)
{
    حرف *buff, *p;

    str = rb_obj_as_string(str);
    إذا (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(حرف, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	إذا (p) *p = '\0';
	رسالة(buff);
    }
    وإلا
    {
	رسالة("");
    }
    أرجع Qnil;
}

    ساكن قيمة_3
vim_set_option(قيمة_3 self UNUSED, قيمة_3 str)
{
    do_set((char_u *)String_قيمة_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن قيمة_3
vim_command(قيمة_3 self UNUSED, قيمة_3 str)
{
    do_cmdline_cmd((char_u *)String_قيمة_Ptr(str));
    أرجع Qnil;
}

#ifdef FEAT_EVAL
    ساكن قيمة_3
vim_to_ruby(typval_T *tv)
{
    قيمة_3 result = Qnil;

    إذا (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == عدم
					  ? "" : (حرف *)(tv->vval.v_string));
    }
    وإلا إذا (tv->v_type == VAR_رقم)
    {
	result = INT2_رقم(tv->vval.v_رقم);
    }
    وإلا إذا (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    وإلا إذا (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	إذا (list != عدم)
	{
	    FOR_ALL_LIST_عناصر(list, curr)
		rb_ary_ادفع(result, vim_to_ruby(&curr->li_tv));
	}
    }
    وإلا إذا (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	إذا (tv->vval.v_dict != عدم)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_عناصر(ht, hi, todo)
	    {
		إذا (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((حرف *)hi->hi_مفتاح),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    وإلا إذا (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	إذا (tv->vval.v_رقم == VVAL_TRUE)
	    result = Qtrue;
	وإلا إذا (tv->vval.v_رقم == VVAL_FALSE)
	    result = Qfalse;
    }
    وإلا إذا (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    أرجع result;
}
#endif

    ساكن قيمة_3
vim_evaluate(قيمة_3 self UNUSED, قيمة_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    قيمة_3       result;

    tv = eval_تعبير((char_u *)String_قيمة_Ptr(str), عدم);
    إذا (tv == عدم)
	أرجع Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    أرجع result;
#وإلا
    أرجع Qnil;
#endif
}

#ifdef USE_TYPEDDATA
ساكن size_t buffer_dsize(ثابت فراغ *buf);

ساكن ثابت rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# إذا RUBY_VERSION >= 27
	0, {0}
# وإلا
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ساكن size_t
buffer_dsize(ثابت فراغ *buf UNUSED)
{
    أرجع حجم(buf_T);
}
#endif

    ساكن قيمة_3
buffer_new(buf_T *buf)
{
    إذا (buf->b_ruby_ref)
    {
	أرجع (قيمة_3) buf->b_ruby_ref;
    }
    وإلا
    {
#ifdef USE_TYPEDDATA
	قيمة_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#وإلا
	قيمة_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن buf_T *
get_buf(قيمة_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#وإلا
    Data_Get_Struct(obj, buf_T, buf);
#endif
    إذا (buf == عدم)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    أرجع buf;
}

    ساكن قيمة_3
vim_blob(قيمة_3 self UNUSED, قيمة_3 str)
{
    قيمة_3 result = rb_str_new("0z", 2);
    حرف    buf[4];
    صحيح	i;
    لكل (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned حرف)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    أرجع result;
}

    ساكن قيمة_3
buffer_s_current(قيمة_3 self UNUSED)
{
    أرجع buffer_new(curbuf);
}

    ساكن قيمة_3
buffer_s_current_getter(ID id UNUSED, قيمة_3 *x UNUSED)
{
    أرجع buffer_new(curbuf);
}

    ساكن قيمة_3
buffer_s_عدد(قيمة_3 self UNUSED)
{
    buf_T *b;
    صحيح n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	إذا (b->b_p_bl)
	    n++;
    }

    أرجع INT2_رقم(n);
}

    ساكن قيمة_3
buffer_s_aref(قيمة_3 self UNUSED, قيمة_3 رقم)
{
    buf_T *b;
    صحيح n = NUM2INT(رقم);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	إذا (!b->b_p_bl)
	    استمر;

	إذا (n == 0)
	    أرجع buffer_new(b);

	n--;
    }
    أرجع Qnil;
}

    ساكن قيمة_3
buffer_name(قيمة_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع buf->b_ffname ? rb_str_new2((حرف *)buf->b_ffname) : Qnil;
}

    ساكن قيمة_3
buffer_رقم(قيمة_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_رقم(buf->b_fnum);
}

    ساكن قيمة_3
buffer_عدد(قيمة_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_رقم(buf->b_ml.ml_سطر_عدد);
}

    ساكن قيمة_3
get_buffer_سطر(buf_T *buf, linenr_T n)
{
    إذا (n <= 0 || n > buf->b_ml.ml_سطر_عدد)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    أرجع vim_str2rb_enc_str((حرف *)ml_get_buf(buf, n, FALSE));
}

    ساكن قيمة_3
buffer_aref(قيمة_3 self, قيمة_3 رقم)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع get_buffer_سطر(buf, (linenr_T)NUM2LONG(رقم));
    أرجع Qnil; // For stop warning
}

    ساكن قيمة_3
set_buffer_سطر(buf_T *buf, linenr_T n, قيمة_3 str)
{
    حرف	*سطر = String_قيمة_Ptr(str);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_سطر_عدد && سطر != عدم)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_savesub(n) == OK)
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
    وإلا
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    أرجع str;
}

    ساكن قيمة_3
buffer_aset(قيمة_3 self, قيمة_3 رقم, قيمة_3 str)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع set_buffer_سطر(buf, (linenr_T)NUM2LONG(رقم), str);
    أرجع str;
}

    ساكن قيمة_3
buffer_delete(قيمة_3 self, قيمة_3 رقم)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(رقم);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_سطر_عدد)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_أسطر_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    أرجع Qnil;
}

    ساكن قيمة_3
buffer_append(قيمة_3 self, قيمة_3 رقم, قيمة_3 str)
{
    buf_T	*buf = get_buf(self);
    حرف	*سطر = String_قيمة_Ptr(str);
    long	n = NUM2LONG(رقم);
    aco_save_T	aco;

    إذا (سطر == عدم)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    وإلا إذا (n >= 0 && n <= buf->b_ml.ml_سطر_عدد)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) سطر, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_أسطر_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    أرجع str;
}

#ifdef USE_TYPEDDATA
ساكن size_t window_dsize(ثابت فراغ *buf);

ساكن ثابت rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# إذا RUBY_VERSION >= 27
	0, {0}
# وإلا
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ساكن size_t
window_dsize(ثابت فراغ *win UNUSED)
{
    أرجع حجم(win_T);
}
#endif

    ساكن قيمة_3
window_new(win_T *win)
{
    إذا (win->w_ruby_ref)
    {
	أرجع (قيمة_3) win->w_ruby_ref;
    }
    وإلا
    {
#ifdef USE_TYPEDDATA
	قيمة_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#وإلا
	قيمة_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن win_T *
get_win(قيمة_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#وإلا
    Data_Get_Struct(obj, win_T, win);
#endif
    إذا (win == عدم)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    أرجع win;
}

    ساكن قيمة_3
window_s_current(قيمة_3 self UNUSED)
{
    أرجع window_new(curwin);
}

    ساكن قيمة_3
window_s_current_getter(ID id UNUSED, قيمة_3 *x UNUSED)
{
    أرجع window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ساكن قيمة_3
سطر_s_current(قيمة_3 self UNUSED)
{
    أرجع get_buffer_سطر(curbuf, curwin->w_cursor.lnum);
}

    ساكن قيمة_3
set_current_سطر(قيمة_3 self UNUSED, قيمة_3 str)
{
    أرجع set_buffer_سطر(curbuf, curwin->w_cursor.lnum, str);
}

    ساكن قيمة_3
current_سطر_رقم(قيمة_3 self UNUSED)
{
    أرجع INT2FIX((صحيح)curwin->w_cursor.lnum);
}

    ساكن قيمة_3
window_s_عدد(قيمة_3 self UNUSED)
{
    win_T	*w;
    صحيح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    أرجع INT2_رقم(n);
}

    ساكن قيمة_3
window_s_aref(قيمة_3 self UNUSED, قيمة_3 رقم)
{
    win_T *w;
    صحيح n = NUM2INT(رقم);

    لكل (w = firstwin; w != عدم; w = w->w_next, --n)
	إذا (n == 0)
	    أرجع window_new(w);
    أرجع Qnil;
}

    ساكن قيمة_3
window_buffer(قيمة_3 self)
{
    win_T *win = get_win(self);

    أرجع buffer_new(win->w_buffer);
}

    ساكن قيمة_3
window_height(قيمة_3 self)
{
    win_T *win = get_win(self);

    أرجع INT2_رقم(win->w_height);
}

    ساكن قيمة_3
window_set_height(قيمة_3 self, قيمة_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    أرجع height;
}

    ساكن قيمة_3
window_width(قيمة_3 self UNUSED)
{
    أرجع INT2_رقم(get_win(self)->w_width);
}

    ساكن قيمة_3
window_set_width(قيمة_3 self UNUSED, قيمة_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    أرجع width;
}

    ساكن قيمة_3
window_cursor(قيمة_3 self)
{
    win_T *win = get_win(self);

    أرجع rb_assoc_new(INT2_رقم(win->w_cursor.lnum), INT2_رقم(win->w_cursor.col));
}

    ساكن قيمة_3
window_set_cursor(قيمة_3 self, قيمة_3 pos)
{
    قيمة_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    إذا (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن قيمة_3
f_nop(قيمة_3 self UNUSED)
{
    أرجع Qnil;
}

    ساكن قيمة_3
f_p(صحيح argc, قيمة_3 *argv, قيمة_3 self UNUSED)
{
    صحيح i;
    قيمة_3 str = rb_str_new("", 0);
    قيمة_3 ret = Qnil;

    لكل (i = 0; i < argc; i++)
    {
	إذا (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    رسالة(RSTRING_PTR(str));

    إذا (argc == 1)
	ret = argv[0];
    وإلا إذا (argc > 1)
	ret = rb_ary_new4(argc, argv);
    أرجع ret;
}

    ساكن فراغ
ruby_io_init(فراغ)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN قيمة_3 rb_stdout;
    RUBYEXTERN قيمة_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (فراغ*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (فراغ*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (فراغ*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (فراغ*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ساكن فراغ
ruby_vim_init(فراغ)
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
    rb_define_singleton_method(cBuffer, "current", (فراغ*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (فراغ*)buffer_s_عدد, 0);
    rb_define_singleton_method(cBuffer, "[]", (فراغ*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (فراغ*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (فراغ*)buffer_رقم, 0);
    rb_define_method(cBuffer, "count", (فراغ*)buffer_عدد, 0);
    rb_define_method(cBuffer, "length", (فراغ*)buffer_عدد, 0);
    rb_define_method(cBuffer, "[]", (فراغ*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (فراغ*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (فراغ*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (فراغ*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (فراغ*)current_سطر_رقم, 0);
    rb_define_method(cBuffer, "line", (فراغ*)سطر_s_current, 0);
    rb_define_method(cBuffer, "line=", (فراغ*)set_current_سطر, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (فراغ*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (فراغ*)window_s_عدد, 0);
    rb_define_singleton_method(cVimWindow, "[]", (فراغ*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (فراغ*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (فراغ*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (فراغ*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (فراغ*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (فراغ*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (فراغ*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (فراغ*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    فراغ
vim_ruby_init(فراغ *مكدس_start)
{
    // should get machine stack start address early in main function
    ruby_مكدس_start = مكدس_start;
}

    ساكن صحيح
convert_hash2dict(قيمة_3 مفتاح, قيمة_3 val, قيمة_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(مفتاح)));
    إذا (di == عدم || ruby_convert_to_vim_قيمة(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	أرجع ST_STOP;
    }
    أرجع ST_CONTINUE;
}

    ساكن صحيح
ruby_convert_to_vim_قيمة(قيمة_3 val, typval_T *rettv)
{
    بدل (TYPE(val))
    {
	حالة T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_رقم = VVAL_NULL;
	    اكسر;
	حالة T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_رقم = VVAL_TRUE;
	    اكسر;
	حالة T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_رقم = VVAL_FALSE;
	    اكسر;
	حالة T_BIGNUM:
	حالة T_FIXNUM:
	    rettv->v_type = VAR_رقم;
	    rettv->vval.v_رقم = (varnumber_T)NUM2LONG(val);
	    اكسر;
	حالة T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    اكسر;
	افتراضي:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	حالة T_STRING:
	    {
		قيمة_3 str = (قيمة_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    اكسر;
	حالة T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		إذا (l == عدم)
		    أرجع FAIL;

		لكل (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    إذا (ruby_convert_to_vim_قيمة((قيمة_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			أرجع FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    اكسر;
	حالة T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		إذا (d == عدم)
		    أرجع FAIL;

		rb_hash_foreach(val, convert_hash2dict, (قيمة_3)d);
		إذا (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    أرجع FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    اكسر;
    }
    أرجع OK;
}

    فراغ
do_rubyeval(char_u *str, typval_T *rettv)
{
    صحيح retval = FAIL;

    إذا (ensure_ruby_initialized())
    {
	صحيح state;
	قيمة_3 obj;

	obj = rb_eval_string_protect((ثابت حرف *)str, &state);
	إذا (state)
	    error_اطبع(state);
	وإلا
	    retval = ruby_convert_to_vim_قيمة(obj, rettv);
    }
    إذا (retval == FAIL)
    {
	rettv->v_type = VAR_رقم;
	rettv->vval.v_رقم = 0;
    }
}
