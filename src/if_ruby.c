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
# ܐܢ RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ܐܢ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ܐܢ RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_cSymbol		(*dll_rb_cSymbol)
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

# ܐܢ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ܐܢ RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ܐܢ RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ܐܢ RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ܐܢ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ܐܢ RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# ܐܢ RUBY_VERSION >= 31
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

#ܐܢ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ܐܢ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#ضمن <ruby.h>
#ضمن <ruby/encoding.h>

#ܐܢ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic pop
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ܐܢ defined(MACOS_X)
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
#ܐܢ defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef StringValuePtr
# define StringValuePtr(s) STR2CSTR(s)
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
#  define symbol_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ܐܚܪܢܐ
#  include <dlfcn.h>
#  define HINSTANCE فراغ*
#  define RUBY_PROC فراغ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define symbol_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

ساكن صحيح ruby_initialized = 0;
ساكن فراغ *ruby_stack_start;
ساكن VALUE objtbl;

ساكن VALUE mVIM;
ساكن VALUE cBuffer;
ساكن VALUE cVimWindow;
ساكن VALUE eDeletedBufferError;
ساكن VALUE eDeletedWindowError;

ساكن صحيح ensure_ruby_initialized(فراغ);
ساكن فراغ error_print(صحيح);
ساكن فراغ ruby_io_init(فراغ);
ساكن فراغ ruby_vim_init(فراغ);
ساكن صحيح ruby_convert_to_vim_value(VALUE val, typval_T *rettv);

#ܐܢ defined(__ia64) && !defined(ruby_init_stack)
# define ruby_init_stack(addr) ruby_init_stack((addr), rb_ia64_bsp())
#endif

#ܐܢ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ܐܢ RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ܐܢ RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  ܐܢ RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  ܐܚܪܢܐ
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# ܐܚܪܢܐ
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
# ܐܢ RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# ܐܚܪܢܐ
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

# ܐܢ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ܐܢ RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# ܐܢ RUBY_VERSION < 20
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
# ܐܚܪܢܐ
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_value		dll_rb_string_value
# define rb_string_value_ptr		dll_rb_string_value_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_push			dll_rb_ary_push
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_stack
#  define ruby_init_stack(addr)	dll_ruby_init_stack((addr), rb_ia64_bsp())
# ܐܚܪܢܐ
#  define ruby_init_stack		dll_ruby_init_stack
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
ساكن VALUE (*dll_rb_assoc_new) (VALUE, VALUE);
VALUE *dll_rb_cFalseClass;
VALUE *dll_rb_cFixnum;
# ܐܢ defined(USE_RUBY_INTEGER)
VALUE *dll_rb_cInteger;
# endif
# ܐܢ RUBY_VERSION >= 20
VALUE *dll_rb_cFloat;
# endif
VALUE *dll_rb_cNilClass;
ساكن VALUE *dll_rb_cObject;
VALUE *dll_rb_cString;
VALUE *dll_rb_cSymbol;
VALUE *dll_rb_cTrueClass;
ساكن VALUE (*dll_rb_class_new_instance) (صحيح,VALUE*,VALUE);
ساكن فراغ (*dll_rb_check_type) (VALUE,صحيح);
# ifdef USE_TYPEDDATA
ساكن فراغ *(*dll_rb_check_typeddata) (VALUE,ثابت rb_data_type_t *);
# endif
ساكن VALUE (*dll_rb_class_path) (VALUE);
# ifdef USE_TYPEDDATA
#  ܐܢ RUBY_VERSION >= 23
ساكن VALUE (*dll_rb_data_typed_object_wrap) (VALUE, فراغ*, ثابت rb_data_type_t *);
#  ܐܚܪܢܐ
ساكن VALUE (*dll_rb_data_typed_object_alloc) (VALUE, فراغ*, ثابت rb_data_type_t *);
#  endif
# ܐܚܪܢܐ
ساكن VALUE (*dll_rb_data_object_alloc) (VALUE, فراغ*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# ܐܢ RUBY_VERSION >= 31
ساكن فراغ (*dll_rb_debug_rstring_null_ptr) (ثابت حرف*);
# endif
ساكن VALUE (*dll_rb_define_class_under) (VALUE, ثابت حرف*, VALUE);
ساكن فراغ (*dll_rb_define_const) (VALUE,ثابت حرف*,VALUE);
ساكن فراغ (*dll_rb_define_global_function) (ثابت حرف*,VALUE(*)(صحيح,VALUE*,VALUE),صحيح);
ساكن فراغ (*dll_rb_define_method) (VALUE,ثابت حرف*,VALUE(*)(ANYARGS),صحيح);
ساكن VALUE (*dll_rb_define_module) (ثابت حرف*);
ساكن فراغ (*dll_rb_define_module_function) (VALUE,ثابت حرف*,VALUE(*)(VALUE,VALUE),صحيح);
ساكن فراغ (*dll_rb_define_singleton_method) (VALUE,ثابت حرف*,VALUE(*)(ANYARGS),صحيح);
ساكن فراغ (*dll_rb_define_virtual_variable) (ثابت حرف*,VALUE(*)(ID,VALUE*),فراغ(*)());
ساكن VALUE *dll_rb_stdout;
ساكن VALUE *dll_rb_stderr;
ساكن VALUE *dll_rb_eArgError;
ساكن VALUE *dll_rb_eIndexError;
ساكن VALUE *dll_rb_eRuntimeError;
ساكن VALUE *dll_rb_eStandardError;
ساكن VALUE (*dll_rb_eval_string_protect) (ثابت حرف*, صحيح*);
# ܐܢ RUBY_VERSION >= 21
ساكن VALUE (*dll_rb_funcallv) (VALUE, ID, صحيح, ثابت VALUE*);
# ܐܚܪܢܐ
ساكن VALUE (*dll_rb_funcall2) (VALUE, ID, صحيح, ثابت VALUE*);
# endif
ساكن فراغ (*dll_rb_global_variable) (VALUE*);
ساكن VALUE (*dll_rb_hash_aset) (VALUE, VALUE, VALUE);
ساكن VALUE (*dll_rb_hash_foreach) (VALUE, صحيح (*)(VALUE, VALUE, VALUE), VALUE);
ساكن VALUE (*dll_rb_hash_new) (فراغ);
ساكن VALUE (*dll_rb_inspect) (VALUE);
ساكن VALUE (*dll_rb_int2inum) (long);
ساكن ID (*dll_rb_intern) (ثابت حرف*);
# ܐܢ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ساكن long (*dll_rb_fix2int) (VALUE);
ساكن long (*dll_rb_num2int) (VALUE);
ساكن unsigned long (*dll_rb_num2uint) (VALUE);
# endif
ساكن مزدوج (*dll_rb_num2dbl) (VALUE);
ساكن VALUE (*dll_rb_lastline_get) (فراغ);
ساكن فراغ (*dll_rb_lastline_set) (VALUE);
ساكن VALUE (*dll_rb_protect) (VALUE (*)(VALUE), VALUE, صحيح*);
ساكن فراغ (*dll_rb_load) (VALUE, صحيح);
ساكن long (*dll_rb_num2long) (VALUE);
ساكن unsigned long (*dll_rb_num2ulong) (VALUE);
ساكن VALUE (*dll_rb_obj_alloc) (VALUE);
ساكن VALUE (*dll_rb_obj_as_string) (VALUE);
ساكن VALUE (*dll_rb_obj_id) (VALUE);
ساكن فراغ (*dll_rb_raise) (VALUE, ثابت حرف*, ...);
ساكن VALUE (*dll_rb_string_value) (متطاير VALUE*);
ساكن VALUE (*dll_rb_str_cat) (VALUE, ثابت حرف*, long);
ساكن VALUE (*dll_rb_str_concat) (VALUE, VALUE);
ساكن VALUE (*dll_rb_str_new) (ثابت حرف*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ساكن VALUE (*dll_rb_str_new_cstr) (ثابت حرف*);
# ܐܚܪܢܐ
ساكن VALUE (*dll_rb_str_new2) (ثابت حرف*);
# endif
ساكن VALUE (*dll_rb_errinfo) (فراغ);
ساكن فراغ (*dll_ruby_init) (فراغ);
ساكن فراغ (*dll_ruby_init_loadpath) (فراغ);
# ifdef MSWIN
ساكن فراغ (*dll_ruby_sysinit) (صحيح*, حرف***);
ساكن صحيح (*dll_rb_w32_snprintf)(حرف*, size_t, ثابت حرف*, ...);
# endif
# ܐܢ RUBY_VERSION >= 31
#  ifdef _MSC_VER
ساكن فراغ (*dll_rb_unexpected_type) (VALUE, صحيح);
#  ܐܚܪܢܐ
NORETURN(ساكن فراغ (*dll_rb_unexpected_type) (VALUE, صحيح));
#  endif
# endif
ساكن حرف * (*dll_rb_string_value_ptr) (متطاير VALUE*);
ساكن VALUE (*dll_rb_float_new) (مزدوج);
ساكن VALUE (*dll_rb_ary_new) (فراغ);
ساكن VALUE (*dll_rb_ary_new4) (long n, ثابت VALUE *elts);
ساكن VALUE (*dll_rb_ary_push) (VALUE, VALUE);
# ܐܢ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ساكن فراغ (*dll_rb_ary_detransient) (VALUE);
# endif
# ifdef __ia64
ساكن فراغ * (*dll_rb_ia64_bsp) (فراغ);
ساكن فراغ (*dll_ruby_init_stack)(VALUE*, فراغ*);
# ܐܚܪܢܐ
ساكن فراغ (*dll_ruby_init_stack)(VALUE*);
# endif
ساكن VALUE (*dll_rb_int2big)(SIGNED_VALUE);

ساكن فراغ (*dll_ruby_script) (ثابت حرف*);
ساكن صحيح (*dll_rb_enc_find_index) (ثابت حرف*);
ساكن rb_encoding* (*dll_rb_enc_find) (ثابت حرف*);
ساكن VALUE (*dll_rb_enc_str_new) (ثابت حرف*, long, rb_encoding*);
ساكن VALUE (*dll_rb_sprintf) (ثابت حرف*, ...);
ساكن VALUE (*dll_rb_require) (ثابت حرف*);
ساكن فراغ* (*dll_ruby_options)(صحيح, حرف**);

# ܐܢ defined(USE_RGENGC) && USE_RGENGC
#  ܐܢ RUBY_VERSION == 21
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect_promoted)(VALUE);
#  ܐܚܪܢܐ
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect)(VALUE obj);
#  endif
# endif

# ܐܢ RUBY_VERSION >= 30
#  ifdef _MSC_VER
ساكن فراغ (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  ܐܚܪܢܐ
NORETURN(ساكن فراغ (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# ܐܢ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
فراغ rb_ary_detransient_stub(VALUE x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ܐܢ RUBY_VERSION >= 22
    long
rb_num2long_stub(VALUE x)
#  ܐܚܪܢܐ
    SIGNED_VALUE
rb_num2long_stub(VALUE x)
#  endif
{
    ܦܢܐ dll_rb_num2long(x);
}
#  ܐܢ RUBY_VERSION >= 26
    VALUE
rb_int2big_stub(intptr_t x)
#  ܐܚܪܢܐ
    VALUE
rb_int2big_stub(SIGNED_VALUE x)
#  endif
{
    ܦܢܐ dll_rb_int2big(x);
}
#  ܐܢ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(VALUE x)
{
    ܦܢܐ dll_rb_fix2int(x);
}
    long
rb_num2int_stub(VALUE x)
{
    ܦܢܐ dll_rb_num2int(x);
}
#  endif
#  ܐܢ RUBY_VERSION >= 20
    VALUE
rb_float_new_in_heap(مزدوج d)
{
    ܦܢܐ dll_rb_float_new(d);
}
#   ܐܢ RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(VALUE x)
#   ܐܚܪܢܐ
    VALUE
rb_num2ulong(VALUE x)
#   endif
{
    ܦܢܐ (long)RSHIFT((SIGNED_VALUE)(x),1);
}
#  endif
#  ܐܢ defined(USE_RGENGC) && USE_RGENGC
#   ܐܢ RUBY_VERSION == 21
    فراغ
rb_gc_writebarrier_unprotect_promoted_stub(VALUE obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ܐܚܪܢܐ
    فراغ
rb_gc_writebarrier_unprotect_stub(VALUE obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ܐܢ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    فراغ
rb_ary_detransient_stub(VALUE x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ܐܢ RUBY_VERSION >= 30
    فراغ
rb_check_type_stub(VALUE obj, صحيح t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(VALUE x)
{
    ܦܢܐ dll_rb_num2uint(x);
}
    فراغ
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  ܐܢ RUBY_VERSION >= 31
    فراغ
rb_debug_rstring_null_ptr_stub(ثابت حرف *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    فراغ
rb_unexpected_type_stub(VALUE self, صحيح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
فراغ *rb_check_typeddata_stub(VALUE obj, ثابت rb_data_type_t *data_type)
{
    ܦܢܐ dll_rb_check_typeddata(obj, data_type);
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
# ܐܢ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ܐܚܪܢܐ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ܐܢ RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_cSymbol},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  ܐܢ RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  ܐܚܪܢܐ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# ܐܚܪܢܐ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# ܐܢ RUBY_VERSION >= 31
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
# ܐܢ RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ܐܚܪܢܐ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ܐܢ RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_value},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# ܐܚܪܢܐ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ܐܢ RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_value_ptr},
# ܐܢ RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ܐܚܪܢܐ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ܐܚܪܢܐ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_push},
# ܐܢ RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_stack},
# ܐܢ defined(USE_RGENGC) && USE_RGENGC
#  ܐܢ RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ܐܚܪܢܐ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ܐܢ RUBY_VERSION >= 30
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

    ܐܢ (hinstRuby)
	ܦܢܐ OK;
    hinstRuby = load_dll(libname);
    ܐܢ (!hinstRuby)
    {
	ܐܢ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ܦܢܐ FAIL;
    }

    لكل (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	ܐܢ (!(*ruby_funcname_table[i].ptr = symbol_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = عدم;
	    ܐܢ (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    ܦܢܐ FAIL;
	}
    }
    ܦܢܐ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    صحيح
ruby_enabled(صحيح verbose)
{
    ܦܢܐ ruby_runtime_link_init((حرف *)p_rubydll, verbose) == OK;
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
    ܐܢ (!eap->skip && ensure_ruby_initialized())
    {
	ܐܢ (script == عدم)
	    rb_eval_string_protect((حرف *)eap->arg, &state);
	ܐܚܪܢܐ
	    rb_eval_string_protect(script, &state);
	ܐܢ (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ساكن VALUE
vim_str2rb_enc_str(ثابت حرف *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    ܐܢ (get_option_value((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_find((حرف *)sval);
	vim_free(sval);
	ܐܢ (enc)
	    ܦܢܐ rb_enc_str_new(s, (long)strlen(s), enc);
    }
    ܦܢܐ rb_str_new2(s);
}

    ساكن VALUE
eval_enc_string_protect(ثابت حرف *str, صحيح *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    VALUE v;

    ܐܢ (get_option_value((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_find((حرف *)sval);
	vim_free(sval);
	ܐܢ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ܦܢܐ rb_eval_string_protect(StringValuePtr(v), state);
	}
    }
    ܦܢܐ rb_eval_string_protect(str, state);
}

    فراغ
ex_rubydo(exarg_T *eap)
{
    صحيح state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ܐܢ (!ensure_ruby_initialized())
	ܦܢܐ;

    ܐܢ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ܦܢܐ;
    لكل (i = eap->line1; i <= eap->line2; i++)
    {
	VALUE line;

	ܐܢ (i > curbuf->b_ml.ml_line_count)
	    اكسر;
	line = vim_str2rb_enc_str((حرف *)ml_get(i));
	rb_lastline_set(line);
	eval_enc_string_protect((حرف *) eap->arg, &state);
	ܐܢ (state)
	{
	    error_print(state);
	    اكسر;
	}
	ܐܢ (was_curbuf != curbuf || i > curbuf->b_ml.ml_line_count)
	    اكسر;
	line = rb_lastline_get();
	ܐܢ (!NIL_P(line))
	{
	    ܐܢ (TYPE(line) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		ܦܢܐ;
	    }
	    ml_replace(i, (char_u *) StringValuePtr(line), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن VALUE
rb_load_wrap(VALUE file_to_load)
{
    rb_load(file_to_load, 0);
    ܦܢܐ Qnil;
}

    فراغ
ex_rubyfile(exarg_T *eap)
{
    صحيح state;

    ܐܢ (!ensure_ruby_initialized())
	ܦܢܐ;

    VALUE file_to_load = rb_str_new2((ثابت حرف *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    ܐܢ (state)
	error_print(state);
}

    فراغ
ruby_buffer_free(buf_T *buf)
{
    ܐܢ (buf->b_ruby_ref == عدم)
	ܦܢܐ;

    rb_hash_aset(objtbl, rb_obj_id((VALUE) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = عدم;
}

    فراغ
ruby_window_free(win_T *win)
{
    ܐܢ (win->w_ruby_ref == عدم)
	ܦܢܐ;

    rb_hash_aset(objtbl, rb_obj_id((VALUE) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = عدم;
}

    ساكن صحيح
ensure_ruby_initialized(فراغ)
{
    ܐܢ (ruby_initialized)
	ܦܢܐ ruby_initialized;

#ifdef DYNAMIC_RUBY
    ܐܢ (ruby_enabled(TRUE))
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
	    ruby_init_stack(ruby_stack_start);
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
    ܐܚܪܢܐ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ܦܢܐ 0;
    }
#endif

    ܦܢܐ ruby_initialized;
}

    ساكن فراغ
error_print(صحيح state)
{
    VALUE error;
    VALUE eclass;
    VALUE einfo;
    VALUE bt;
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
	    ܐܢ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ܐܚܪܢܐ
	    {
		VALUE epath;
		حرف *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ܐܢ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ܐܢ RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		msg_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ܐܚܪܢܐ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		msg_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    اكسر;
	افتراضي:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    اكسر;
    }
}

    ساكن VALUE
vim_message(VALUE self UNUSED, VALUE str)
{
    حرف *buff, *p;

    str = rb_obj_as_string(str);
    ܐܢ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(حرف, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ܐܢ (p) *p = '\0';
	msg(buff);
    }
    ܐܚܪܢܐ
    {
	msg("");
    }
    ܦܢܐ Qnil;
}

    ساكن VALUE
vim_set_option(VALUE self UNUSED, VALUE str)
{
    do_set((char_u *)StringValuePtr(str), 0);
    update_screen(UPD_NOT_VALID);
    ܦܢܐ Qnil;
}

    ساكن VALUE
vim_command(VALUE self UNUSED, VALUE str)
{
    do_cmdline_cmd((char_u *)StringValuePtr(str));
    ܦܢܐ Qnil;
}

#ifdef FEAT_EVAL
    ساكن VALUE
vim_to_ruby(typval_T *tv)
{
    VALUE result = Qnil;

    ܐܢ (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == عدم
					  ? "" : (حرف *)(tv->vval.v_string));
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_NUMBER)
    {
	result = INT2NUM(tv->vval.v_number);
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	ܐܢ (list != عدم)
	{
	    FOR_ALL_LIST_ITEMS(list, curr)
		rb_ary_push(result, vim_to_ruby(&curr->li_tv));
	}
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	ܐܢ (tv->vval.v_dict != عدم)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	    {
		ܐܢ (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((حرف *)hi->hi_key),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ܐܢ (tv->vval.v_number == VVAL_TRUE)
	    result = Qtrue;
	ܐܚܪܢܐ ܐܢ (tv->vval.v_number == VVAL_FALSE)
	    result = Qfalse;
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ܦܢܐ result;
}
#endif

    ساكن VALUE
vim_evaluate(VALUE self UNUSED, VALUE str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    VALUE       result;

    tv = eval_expr((char_u *)StringValuePtr(str), عدم);
    ܐܢ (tv == عدم)
	ܦܢܐ Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    ܦܢܐ result;
#ܐܚܪܢܐ
    ܦܢܐ Qnil;
#endif
}

#ifdef USE_TYPEDDATA
ساكن size_t buffer_dsize(ثابت فراغ *buf);

ساكن ثابت rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# ܐܢ RUBY_VERSION >= 27
	0, {0}
# ܐܚܪܢܐ
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
    ܦܢܐ حجم(buf_T);
}
#endif

    ساكن VALUE
buffer_new(buf_T *buf)
{
    ܐܢ (buf->b_ruby_ref)
    {
	ܦܢܐ (VALUE) buf->b_ruby_ref;
    }
    ܐܚܪܢܐ
    {
#ifdef USE_TYPEDDATA
	VALUE obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#ܐܚܪܢܐ
	VALUE obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ܦܢܐ obj;
    }
}

    ساكن buf_T *
get_buf(VALUE obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#ܐܚܪܢܐ
    Data_Get_Struct(obj, buf_T, buf);
#endif
    ܐܢ (buf == عدم)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    ܦܢܐ buf;
}

    ساكن VALUE
vim_blob(VALUE self UNUSED, VALUE str)
{
    VALUE result = rb_str_new("0z", 2);
    حرف    buf[4];
    صحيح	i;
    لكل (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned حرف)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    ܦܢܐ result;
}

    ساكن VALUE
buffer_s_current(VALUE self UNUSED)
{
    ܦܢܐ buffer_new(curbuf);
}

    ساكن VALUE
buffer_s_current_getter(ID id UNUSED, VALUE *x UNUSED)
{
    ܦܢܐ buffer_new(curbuf);
}

    ساكن VALUE
buffer_s_count(VALUE self UNUSED)
{
    buf_T *b;
    صحيح n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ܐܢ (b->b_p_bl)
	    n++;
    }

    ܦܢܐ INT2NUM(n);
}

    ساكن VALUE
buffer_s_aref(VALUE self UNUSED, VALUE num)
{
    buf_T *b;
    صحيح n = NUM2INT(num);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ܐܢ (!b->b_p_bl)
	    استمر;

	ܐܢ (n == 0)
	    ܦܢܐ buffer_new(b);

	n--;
    }
    ܦܢܐ Qnil;
}

    ساكن VALUE
buffer_name(VALUE self)
{
    buf_T *buf = get_buf(self);

    ܦܢܐ buf->b_ffname ? rb_str_new2((حرف *)buf->b_ffname) : Qnil;
}

    ساكن VALUE
buffer_number(VALUE self)
{
    buf_T *buf = get_buf(self);

    ܦܢܐ INT2NUM(buf->b_fnum);
}

    ساكن VALUE
buffer_count(VALUE self)
{
    buf_T *buf = get_buf(self);

    ܦܢܐ INT2NUM(buf->b_ml.ml_line_count);
}

    ساكن VALUE
get_buffer_line(buf_T *buf, linenr_T n)
{
    ܐܢ (n <= 0 || n > buf->b_ml.ml_line_count)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    ܦܢܐ vim_str2rb_enc_str((حرف *)ml_get_buf(buf, n, FALSE));
}

    ساكن VALUE
buffer_aref(VALUE self, VALUE num)
{
    buf_T *buf = get_buf(self);

    ܐܢ (buf != عدم)
	ܦܢܐ get_buffer_line(buf, (linenr_T)NUM2LONG(num));
    ܦܢܐ Qnil; // For stop warning
}

    ساكن VALUE
set_buffer_line(buf_T *buf, linenr_T n, VALUE str)
{
    حرف	*line = StringValuePtr(str);
    aco_save_T	aco;

    ܐܢ (n > 0 && n <= buf->b_ml.ml_line_count && line != عدم)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ܐܢ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ܐܢ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)line, TRUE);
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
    ܐܚܪܢܐ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    ܦܢܐ str;
}

    ساكن VALUE
buffer_aset(VALUE self, VALUE num, VALUE str)
{
    buf_T *buf = get_buf(self);

    ܐܢ (buf != عدم)
	ܦܢܐ set_buffer_line(buf, (linenr_T)NUM2LONG(num), str);
    ܦܢܐ str;
}

    ساكن VALUE
buffer_delete(VALUE self, VALUE num)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(num);
    aco_save_T	aco;

    ܐܢ (n > 0 && n <= buf->b_ml.ml_line_count)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ܐܢ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ܐܢ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_lines_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ܐܚܪܢܐ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ܦܢܐ Qnil;
}

    ساكن VALUE
buffer_append(VALUE self, VALUE num, VALUE str)
{
    buf_T	*buf = get_buf(self);
    حرف	*line = StringValuePtr(str);
    long	n = NUM2LONG(num);
    aco_save_T	aco;

    ܐܢ (line == عدم)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    ܐܚܪܢܐ ܐܢ (n >= 0 && n <= buf->b_ml.ml_line_count)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ܐܢ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ܐܢ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) line, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_lines_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ܐܚܪܢܐ
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ܦܢܐ str;
}

#ifdef USE_TYPEDDATA
ساكن size_t window_dsize(ثابت فراغ *buf);

ساكن ثابت rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# ܐܢ RUBY_VERSION >= 27
	0, {0}
# ܐܚܪܢܐ
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
    ܦܢܐ حجم(win_T);
}
#endif

    ساكن VALUE
window_new(win_T *win)
{
    ܐܢ (win->w_ruby_ref)
    {
	ܦܢܐ (VALUE) win->w_ruby_ref;
    }
    ܐܚܪܢܐ
    {
#ifdef USE_TYPEDDATA
	VALUE obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#ܐܚܪܢܐ
	VALUE obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ܦܢܐ obj;
    }
}

    ساكن win_T *
get_win(VALUE obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#ܐܚܪܢܐ
    Data_Get_Struct(obj, win_T, win);
#endif
    ܐܢ (win == عدم)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    ܦܢܐ win;
}

    ساكن VALUE
window_s_current(VALUE self UNUSED)
{
    ܦܢܐ window_new(curwin);
}

    ساكن VALUE
window_s_current_getter(ID id UNUSED, VALUE *x UNUSED)
{
    ܦܢܐ window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ساكن VALUE
line_s_current(VALUE self UNUSED)
{
    ܦܢܐ get_buffer_line(curbuf, curwin->w_cursor.lnum);
}

    ساكن VALUE
set_current_line(VALUE self UNUSED, VALUE str)
{
    ܦܢܐ set_buffer_line(curbuf, curwin->w_cursor.lnum, str);
}

    ساكن VALUE
current_line_number(VALUE self UNUSED)
{
    ܦܢܐ INT2FIX((صحيح)curwin->w_cursor.lnum);
}

    ساكن VALUE
window_s_count(VALUE self UNUSED)
{
    win_T	*w;
    صحيح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ܦܢܐ INT2NUM(n);
}

    ساكن VALUE
window_s_aref(VALUE self UNUSED, VALUE num)
{
    win_T *w;
    صحيح n = NUM2INT(num);

    لكل (w = firstwin; w != عدم; w = w->w_next, --n)
	ܐܢ (n == 0)
	    ܦܢܐ window_new(w);
    ܦܢܐ Qnil;
}

    ساكن VALUE
window_buffer(VALUE self)
{
    win_T *win = get_win(self);

    ܦܢܐ buffer_new(win->w_buffer);
}

    ساكن VALUE
window_height(VALUE self)
{
    win_T *win = get_win(self);

    ܦܢܐ INT2NUM(win->w_height);
}

    ساكن VALUE
window_set_height(VALUE self, VALUE height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    ܦܢܐ height;
}

    ساكن VALUE
window_width(VALUE self UNUSED)
{
    ܦܢܐ INT2NUM(get_win(self)->w_width);
}

    ساكن VALUE
window_set_width(VALUE self UNUSED, VALUE width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    ܦܢܐ width;
}

    ساكن VALUE
window_cursor(VALUE self)
{
    win_T *win = get_win(self);

    ܦܢܐ rb_assoc_new(INT2NUM(win->w_cursor.lnum), INT2NUM(win->w_cursor.col));
}

    ساكن VALUE
window_set_cursor(VALUE self, VALUE pos)
{
    VALUE lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ܐܢ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ܦܢܐ Qnil;
}

    ساكن VALUE
f_nop(VALUE self UNUSED)
{
    ܦܢܐ Qnil;
}

    ساكن VALUE
f_p(صحيح argc, VALUE *argv, VALUE self UNUSED)
{
    صحيح i;
    VALUE str = rb_str_new("", 0);
    VALUE ret = Qnil;

    لكل (i = 0; i < argc; i++)
    {
	ܐܢ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    msg(RSTRING_PTR(str));

    ܐܢ (argc == 1)
	ret = argv[0];
    ܐܚܪܢܐ ܐܢ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ܦܢܐ ret;
}

    ساكن فراغ
ruby_io_init(فراغ)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN VALUE rb_stdout;
    RUBYEXTERN VALUE rb_stderr;
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
    rb_define_const(mVIM, "VERSION_MAJOR", INT2NUM(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2NUM(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2NUM(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2NUM(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "count", (فراغ*)buffer_s_count, 0);
    rb_define_singleton_method(cBuffer, "[]", (فراغ*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (فراغ*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (فراغ*)buffer_number, 0);
    rb_define_method(cBuffer, "count", (فراغ*)buffer_count, 0);
    rb_define_method(cBuffer, "length", (فراغ*)buffer_count, 0);
    rb_define_method(cBuffer, "[]", (فراغ*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (فراغ*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (فراغ*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (فراغ*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (فراغ*)current_line_number, 0);
    rb_define_method(cBuffer, "line", (فراغ*)line_s_current, 0);
    rb_define_method(cBuffer, "line=", (فراغ*)set_current_line, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (فراغ*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (فراغ*)window_s_count, 0);
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
vim_ruby_init(فراغ *stack_start)
{
    // should get machine stack start address early in main function
    ruby_stack_start = stack_start;
}

    ساكن صحيح
convert_hash2dict(VALUE key, VALUE val, VALUE arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(key)));
    ܐܢ (di == عدم || ruby_convert_to_vim_value(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ܦܢܐ ST_STOP;
    }
    ܦܢܐ ST_CONTINUE;
}

    ساكن صحيح
ruby_convert_to_vim_value(VALUE val, typval_T *rettv)
{
    بدل (TYPE(val))
    {
	حالة T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NULL;
	    اكسر;
	حالة T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = VVAL_TRUE;
	    اكسر;
	حالة T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = VVAL_FALSE;
	    اكسر;
	حالة T_BIGNUM:
	حالة T_FIXNUM:
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = (varnumber_T)NUM2LONG(val);
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
		VALUE str = (VALUE)RSTRING(val);

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
		ܐܢ (l == عدم)
		    ܦܢܐ FAIL;

		لكل (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ܐܢ (ruby_convert_to_vim_value((VALUE)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ܦܢܐ FAIL;
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
		ܐܢ (d == عدم)
		    ܦܢܐ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (VALUE)d);
		ܐܢ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ܦܢܐ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    اكسر;
    }
    ܦܢܐ OK;
}

    فراغ
do_rubyeval(char_u *str, typval_T *rettv)
{
    صحيح retval = FAIL;

    ܐܢ (ensure_ruby_initialized())
    {
	صحيح state;
	VALUE obj;

	obj = rb_eval_string_protect((ثابت حرف *)str, &state);
	ܐܢ (state)
	    error_print(state);
	ܐܚܪܢܐ
	    retval = ruby_convert_to_vim_value(obj, rettv);
    }
    ܐܢ (retval == FAIL)
    {
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = 0;
    }
}
