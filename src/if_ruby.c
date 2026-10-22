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

#include "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN extern
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# якщо RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# якщо defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# якщо RUBY_VERSION >= 20
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

# якщо RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# якщо RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# якщо RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# якщо RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# якщо RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# якщо RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# якщо RUBY_VERSION >= 31
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

#якщо defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#якщо defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#include <ruby.h>
#include <ruby/encoding.h>

#якщо defined(__GNUC__) || defined(__clang__)
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
#якщо defined(MACOS_X)
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
#якщо defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
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

#include "vim.h"
#include "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define symbol_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# інакше
#  include <dlfcn.h>
#  define HINSTANCE void*
#  define RUBY_PROC void*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define symbol_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

static ціле ruby_initialized = 0;
static void *ruby_stack_start;
static VALUE objtbl;

static VALUE mVIM;
static VALUE cBuffer;
static VALUE cVimWindow;
static VALUE eDeletedBufferError;
static VALUE eDeletedWindowError;

static ціле ensure_ruby_initialized(void);
static void error_print(ціле);
static void ruby_io_init(void);
static void ruby_vim_init(void);
static ціле ruby_convert_to_vim_value(VALUE val, typval_T *rettv);

#якщо defined(__ia64) && !defined(ruby_init_stack)
# define ruby_init_stack(addr) ruby_init_stack((addr), rb_ia64_bsp())
#endif

#якщо defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# якщо RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# якщо RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  якщо RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  інакше
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# інакше
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
# якщо RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# інакше
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

# якщо VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  якщо RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# якщо RUBY_VERSION < 20
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
# інакше
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
# інакше
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
static VALUE (*dll_rb_assoc_new) (VALUE, VALUE);
VALUE *dll_rb_cFalseClass;
VALUE *dll_rb_cFixnum;
# якщо defined(USE_RUBY_INTEGER)
VALUE *dll_rb_cInteger;
# endif
# якщо RUBY_VERSION >= 20
VALUE *dll_rb_cFloat;
# endif
VALUE *dll_rb_cNilClass;
static VALUE *dll_rb_cObject;
VALUE *dll_rb_cString;
VALUE *dll_rb_cSymbol;
VALUE *dll_rb_cTrueClass;
static VALUE (*dll_rb_class_new_instance) (ціле,VALUE*,VALUE);
static void (*dll_rb_check_type) (VALUE,ціле);
# ifdef USE_TYPEDDATA
static void *(*dll_rb_check_typeddata) (VALUE,const rb_data_type_t *);
# endif
static VALUE (*dll_rb_class_path) (VALUE);
# ifdef USE_TYPEDDATA
#  якщо RUBY_VERSION >= 23
static VALUE (*dll_rb_data_typed_object_wrap) (VALUE, void*, const rb_data_type_t *);
#  інакше
static VALUE (*dll_rb_data_typed_object_alloc) (VALUE, void*, const rb_data_type_t *);
#  endif
# інакше
static VALUE (*dll_rb_data_object_alloc) (VALUE, void*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# якщо RUBY_VERSION >= 31
static void (*dll_rb_debug_rstring_null_ptr) (const char*);
# endif
static VALUE (*dll_rb_define_class_under) (VALUE, const char*, VALUE);
static void (*dll_rb_define_const) (VALUE,const char*,VALUE);
static void (*dll_rb_define_global_function) (const char*,VALUE(*)(ціле,VALUE*,VALUE),ціле);
static void (*dll_rb_define_method) (VALUE,const char*,VALUE(*)(ANYARGS),ціле);
static VALUE (*dll_rb_define_module) (const char*);
static void (*dll_rb_define_module_function) (VALUE,const char*,VALUE(*)(VALUE,VALUE),ціле);
static void (*dll_rb_define_singleton_method) (VALUE,const char*,VALUE(*)(ANYARGS),ціле);
static void (*dll_rb_define_virtual_variable) (const char*,VALUE(*)(ID,VALUE*),void(*)());
static VALUE *dll_rb_stdout;
static VALUE *dll_rb_stderr;
static VALUE *dll_rb_eArgError;
static VALUE *dll_rb_eIndexError;
static VALUE *dll_rb_eRuntimeError;
static VALUE *dll_rb_eStandardError;
static VALUE (*dll_rb_eval_string_protect) (const char*, ціле*);
# якщо RUBY_VERSION >= 21
static VALUE (*dll_rb_funcallv) (VALUE, ID, ціле, const VALUE*);
# інакше
static VALUE (*dll_rb_funcall2) (VALUE, ID, ціле, const VALUE*);
# endif
static void (*dll_rb_global_variable) (VALUE*);
static VALUE (*dll_rb_hash_aset) (VALUE, VALUE, VALUE);
static VALUE (*dll_rb_hash_foreach) (VALUE, ціле (*)(VALUE, VALUE, VALUE), VALUE);
static VALUE (*dll_rb_hash_new) (void);
static VALUE (*dll_rb_inspect) (VALUE);
static VALUE (*dll_rb_int2inum) (long);
static ID (*dll_rb_intern) (const char*);
# якщо RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
static long (*dll_rb_fix2int) (VALUE);
static long (*dll_rb_num2int) (VALUE);
static unsigned long (*dll_rb_num2uint) (VALUE);
# endif
static double (*dll_rb_num2dbl) (VALUE);
static VALUE (*dll_rb_lastline_get) (void);
static void (*dll_rb_lastline_set) (VALUE);
static VALUE (*dll_rb_protect) (VALUE (*)(VALUE), VALUE, ціле*);
static void (*dll_rb_load) (VALUE, ціле);
static long (*dll_rb_num2long) (VALUE);
static unsigned long (*dll_rb_num2ulong) (VALUE);
static VALUE (*dll_rb_obj_alloc) (VALUE);
static VALUE (*dll_rb_obj_as_string) (VALUE);
static VALUE (*dll_rb_obj_id) (VALUE);
static void (*dll_rb_raise) (VALUE, const char*, ...);
static VALUE (*dll_rb_string_value) (volatile VALUE*);
static VALUE (*dll_rb_str_cat) (VALUE, const char*, long);
static VALUE (*dll_rb_str_concat) (VALUE, VALUE);
static VALUE (*dll_rb_str_new) (const char*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
static VALUE (*dll_rb_str_new_cstr) (const char*);
# інакше
static VALUE (*dll_rb_str_new2) (const char*);
# endif
static VALUE (*dll_rb_errinfo) (void);
static void (*dll_ruby_init) (void);
static void (*dll_ruby_init_loadpath) (void);
# ifdef MSWIN
static void (*dll_ruby_sysinit) (ціле*, char***);
static ціле (*dll_rb_w32_snprintf)(char*, size_t, const char*, ...);
# endif
# якщо RUBY_VERSION >= 31
#  ifdef _MSC_VER
static void (*dll_rb_unexpected_type) (VALUE, ціле);
#  інакше
NORETURN(static void (*dll_rb_unexpected_type) (VALUE, ціле));
#  endif
# endif
static char * (*dll_rb_string_value_ptr) (volatile VALUE*);
static VALUE (*dll_rb_float_new) (double);
static VALUE (*dll_rb_ary_new) (void);
static VALUE (*dll_rb_ary_new4) (long n, const VALUE *elts);
static VALUE (*dll_rb_ary_push) (VALUE, VALUE);
# якщо RUBY_VERSION >= 26 && RUBY_VERSION <= 32
static void (*dll_rb_ary_detransient) (VALUE);
# endif
# ifdef __ia64
static void * (*dll_rb_ia64_bsp) (void);
static void (*dll_ruby_init_stack)(VALUE*, void*);
# інакше
static void (*dll_ruby_init_stack)(VALUE*);
# endif
static VALUE (*dll_rb_int2big)(SIGNED_VALUE);

static void (*dll_ruby_script) (const char*);
static ціле (*dll_rb_enc_find_index) (const char*);
static rb_encoding* (*dll_rb_enc_find) (const char*);
static VALUE (*dll_rb_enc_str_new) (const char*, long, rb_encoding*);
static VALUE (*dll_rb_sprintf) (const char*, ...);
static VALUE (*dll_rb_require) (const char*);
static void* (*dll_ruby_options)(ціле, char**);

# якщо defined(USE_RGENGC) && USE_RGENGC
#  якщо RUBY_VERSION == 21
static void (*dll_rb_gc_writebarrier_unprotect_promoted)(VALUE);
#  інакше
static void (*dll_rb_gc_writebarrier_unprotect)(VALUE obj);
#  endif
# endif

# якщо RUBY_VERSION >= 30
#  ifdef _MSC_VER
static void (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  інакше
NORETURN(static void (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# якщо RUBY_VERSION >= 26 && RUBY_VERSION <= 32
void rb_ary_detransient_stub(VALUE x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  якщо RUBY_VERSION >= 22
    long
rb_num2long_stub(VALUE x)
#  інакше
    SIGNED_VALUE
rb_num2long_stub(VALUE x)
#  endif
{
    повернути dll_rb_num2long(x);
}
#  якщо RUBY_VERSION >= 26
    VALUE
rb_int2big_stub(intptr_t x)
#  інакше
    VALUE
rb_int2big_stub(SIGNED_VALUE x)
#  endif
{
    повернути dll_rb_int2big(x);
}
#  якщо RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(VALUE x)
{
    повернути dll_rb_fix2int(x);
}
    long
rb_num2int_stub(VALUE x)
{
    повернути dll_rb_num2int(x);
}
#  endif
#  якщо RUBY_VERSION >= 20
    VALUE
rb_float_new_in_heap(double d)
{
    повернути dll_rb_float_new(d);
}
#   якщо RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(VALUE x)
#   інакше
    VALUE
rb_num2ulong(VALUE x)
#   endif
{
    повернути (long)RSHIFT((SIGNED_VALUE)(x),1);
}
#  endif
#  якщо defined(USE_RGENGC) && USE_RGENGC
#   якщо RUBY_VERSION == 21
    void
rb_gc_writebarrier_unprotect_promoted_stub(VALUE obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   інакше
    void
rb_gc_writebarrier_unprotect_stub(VALUE obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  якщо RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    void
rb_ary_detransient_stub(VALUE x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  якщо RUBY_VERSION >= 30
    void
rb_check_type_stub(VALUE obj, ціле t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(VALUE x)
{
    повернути dll_rb_num2uint(x);
}
    void
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  якщо RUBY_VERSION >= 31
    void
rb_debug_rstring_null_ptr_stub(const char *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    void
rb_unexpected_type_stub(VALUE self, ціле t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
void *rb_check_typeddata_stub(VALUE obj, const rb_data_type_t *data_type)
{
    повернути dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

static HINSTANCE hinstRuby = NULL; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
static struct
{
    char *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# якщо defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# інакше
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# якщо RUBY_VERSION >= 20
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
#  якщо RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  інакше
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# інакше
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# якщо RUBY_VERSION >= 31
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
# якщо RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# інакше
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# якщо RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
# інакше
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# якщо RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_value_ptr},
# якщо RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# інакше
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# інакше
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_push},
# якщо RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
# якщо defined(USE_RGENGC) && USE_RGENGC
#  якщо RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  інакше
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# якщо RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", NULL},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    static ціле
ruby_runtime_link_init(char *libname, ціле verbose)
{
    ціле i;

    якщо (hinstRuby)
	повернути OK;
    hinstRuby = load_dll(libname);
    якщо (!hinstRuby)
    {
	якщо (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	повернути FAIL;
    }

    для (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	якщо (!(*ruby_funcname_table[i].ptr = symbol_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = NULL;
	    якщо (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    повернути FAIL;
	}
    }
    повернути OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    ціле
ruby_enabled(ціле verbose)
{
    повернути ruby_runtime_link_init((char *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    void
ruby_end(void)
{
}

    void
ex_ruby(exarg_T *eap)
{
    ціле state;
    char *script = NULL;

    script = (char *)script_get(eap, eap->arg);
    якщо (!eap->skip && ensure_ruby_initialized())
    {
	якщо (script == NULL)
	    rb_eval_string_protect((char *)eap->arg, &state);
	інакше
	    rb_eval_string_protect(script, &state);
	якщо (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    static VALUE
vim_str2rb_enc_str(const char *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    якщо (get_option_value((char_u *)"enc", &lval, &sval, NULL, 0) == gov_string)
    {
	enc = rb_enc_find((char *)sval);
	vim_free(sval);
	якщо (enc)
	    повернути rb_enc_str_new(s, (long)strlen(s), enc);
    }
    повернути rb_str_new2(s);
}

    static VALUE
eval_enc_string_protect(const char *str, ціле *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    VALUE v;

    якщо (get_option_value((char_u *)"enc", &lval, &sval, NULL, 0) == gov_string)
    {
	enc = rb_enc_find((char *)sval);
	vim_free(sval);
	якщо (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    повернути rb_eval_string_protect(StringValuePtr(v), state);
	}
    }
    повернути rb_eval_string_protect(str, state);
}

    void
ex_rubydo(exarg_T *eap)
{
    ціле state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    якщо (!ensure_ruby_initialized())
	повернути;

    якщо (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	повернути;
    для (i = eap->line1; i <= eap->line2; i++)
    {
	VALUE line;

	якщо (i > curbuf->b_ml.ml_line_count)
	    break;
	line = vim_str2rb_enc_str((char *)ml_get(i));
	rb_lastline_set(line);
	eval_enc_string_protect((char *) eap->arg, &state);
	якщо (state)
	{
	    error_print(state);
	    break;
	}
	якщо (was_curbuf != curbuf || i > curbuf->b_ml.ml_line_count)
	    break;
	line = rb_lastline_get();
	якщо (!NIL_P(line))
	{
	    якщо (TYPE(line) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		повернути;
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

    static VALUE
rb_load_wrap(VALUE file_to_load)
{
    rb_load(file_to_load, 0);
    повернути Qnil;
}

    void
ex_rubyfile(exarg_T *eap)
{
    ціле state;

    якщо (!ensure_ruby_initialized())
	повернути;

    VALUE file_to_load = rb_str_new2((const char *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    якщо (state)
	error_print(state);
}

    void
ruby_buffer_free(buf_T *buf)
{
    якщо (buf->b_ruby_ref == NULL)
	повернути;

    rb_hash_aset(objtbl, rb_obj_id((VALUE) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = NULL;
}

    void
ruby_window_free(win_T *win)
{
    якщо (win->w_ruby_ref == NULL)
	повернути;

    rb_hash_aset(objtbl, rb_obj_id((VALUE) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = NULL;
}

    static ціле
ensure_ruby_initialized(void)
{
    якщо (ruby_initialized)
	повернути ruby_initialized;

#ifdef DYNAMIC_RUBY
    якщо (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	ціле argc = 1;
	char *argv[] = {"gvim.exe"};
	char **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_stack(ruby_stack_start);
	    ruby_init();
	}
	{
	    ціле dummy_argc = 2;
	    char *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    інакше
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	повернути 0;
    }
#endif

    повернути ruby_initialized;
}

    static void
error_print(ціле state)
{
    VALUE error;
    VALUE eclass;
    VALUE einfo;
    VALUE bt;
    ціле attr;
    char buff[BUFSIZ];
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

    switch (state)
    {
	case TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    break;
	case TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    break;
	case TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    break;
	case TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    break;
	case TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    break;
	case TAG_RAISE:
	case TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    якщо (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    інакше
	    {
		VALUE epath;
		char *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		якщо (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#якщо RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    для (i = 0; i < RARRAY_LEN(bt); i++)
		msg_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#інакше
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    для (i = 0; i < RARRAY_LEN(bt); i++)
		msg_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    break;
	default:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    break;
    }
}

    static VALUE
vim_message(VALUE self UNUSED, VALUE str)
{
    char *buff, *p;

    str = rb_obj_as_string(str);
    якщо (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(char, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	якщо (p) *p = '\0';
	msg(buff);
    }
    інакше
    {
	msg("");
    }
    повернути Qnil;
}

    static VALUE
vim_set_option(VALUE self UNUSED, VALUE str)
{
    do_set((char_u *)StringValuePtr(str), 0);
    update_screen(UPD_NOT_VALID);
    повернути Qnil;
}

    static VALUE
vim_command(VALUE self UNUSED, VALUE str)
{
    do_cmdline_cmd((char_u *)StringValuePtr(str));
    повернути Qnil;
}

#ifdef FEAT_EVAL
    static VALUE
vim_to_ruby(typval_T *tv)
{
    VALUE result = Qnil;

    якщо (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == NULL
					  ? "" : (char *)(tv->vval.v_string));
    }
    інакше якщо (tv->v_type == VAR_NUMBER)
    {
	result = INT2NUM(tv->vval.v_number);
    }
    інакше якщо (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    інакше якщо (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	якщо (list != NULL)
	{
	    FOR_ALL_LIST_ITEMS(list, curr)
		rb_ary_push(result, vim_to_ruby(&curr->li_tv));
	}
    }
    інакше якщо (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	якщо (tv->vval.v_dict != NULL)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	    {
		якщо (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((char *)hi->hi_key),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    інакше якщо (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	якщо (tv->vval.v_number == VVAL_TRUE)
	    result = Qtrue;
	інакше якщо (tv->vval.v_number == VVAL_FALSE)
	    result = Qfalse;
    }
    інакше якщо (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    повернути result;
}
#endif

    static VALUE
vim_evaluate(VALUE self UNUSED, VALUE str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    VALUE       result;

    tv = eval_expr((char_u *)StringValuePtr(str), NULL);
    якщо (tv == NULL)
	повернути Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    повернути result;
#інакше
    повернути Qnil;
#endif
}

#ifdef USE_TYPEDDATA
static size_t buffer_dsize(const void *buf);

static const rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# якщо RUBY_VERSION >= 27
	0, {0}
# інакше
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    static size_t
buffer_dsize(const void *buf UNUSED)
{
    повернути sizeof(buf_T);
}
#endif

    static VALUE
buffer_new(buf_T *buf)
{
    якщо (buf->b_ruby_ref)
    {
	повернути (VALUE) buf->b_ruby_ref;
    }
    інакше
    {
#ifdef USE_TYPEDDATA
	VALUE obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#інакше
	VALUE obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (void *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	повернути obj;
    }
}

    static buf_T *
get_buf(VALUE obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#інакше
    Data_Get_Struct(obj, buf_T, buf);
#endif
    якщо (buf == NULL)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    повернути buf;
}

    static VALUE
vim_blob(VALUE self UNUSED, VALUE str)
{
    VALUE result = rb_str_new("0z", 2);
    char    buf[4];
    ціле	i;
    для (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned char)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    повернути result;
}

    static VALUE
buffer_s_current(VALUE self UNUSED)
{
    повернути buffer_new(curbuf);
}

    static VALUE
buffer_s_current_getter(ID id UNUSED, VALUE *x UNUSED)
{
    повернути buffer_new(curbuf);
}

    static VALUE
buffer_s_count(VALUE self UNUSED)
{
    buf_T *b;
    ціле n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	якщо (b->b_p_bl)
	    n++;
    }

    повернути INT2NUM(n);
}

    static VALUE
buffer_s_aref(VALUE self UNUSED, VALUE num)
{
    buf_T *b;
    ціле n = NUM2INT(num);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	якщо (!b->b_p_bl)
	    continue;

	якщо (n == 0)
	    повернути buffer_new(b);

	n--;
    }
    повернути Qnil;
}

    static VALUE
buffer_name(VALUE self)
{
    buf_T *buf = get_buf(self);

    повернути buf->b_ffname ? rb_str_new2((char *)buf->b_ffname) : Qnil;
}

    static VALUE
buffer_number(VALUE self)
{
    buf_T *buf = get_buf(self);

    повернути INT2NUM(buf->b_fnum);
}

    static VALUE
buffer_count(VALUE self)
{
    buf_T *buf = get_buf(self);

    повернути INT2NUM(buf->b_ml.ml_line_count);
}

    static VALUE
get_buffer_line(buf_T *buf, linenr_T n)
{
    якщо (n <= 0 || n > buf->b_ml.ml_line_count)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    повернути vim_str2rb_enc_str((char *)ml_get_buf(buf, n, FALSE));
}

    static VALUE
buffer_aref(VALUE self, VALUE num)
{
    buf_T *buf = get_buf(self);

    якщо (buf != NULL)
	повернути get_buffer_line(buf, (linenr_T)NUM2LONG(num));
    повернути Qnil; // For stop warning
}

    static VALUE
set_buffer_line(buf_T *buf, linenr_T n, VALUE str)
{
    char	*line = StringValuePtr(str);
    aco_save_T	aco;

    якщо (n > 0 && n <= buf->b_ml.ml_line_count && line != NULL)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	якщо (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    якщо (u_savesub(n) == OK)
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
    інакше
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    повернути str;
}

    static VALUE
buffer_aset(VALUE self, VALUE num, VALUE str)
{
    buf_T *buf = get_buf(self);

    якщо (buf != NULL)
	повернути set_buffer_line(buf, (linenr_T)NUM2LONG(num), str);
    повернути str;
}

    static VALUE
buffer_delete(VALUE self, VALUE num)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(num);
    aco_save_T	aco;

    якщо (n > 0 && n <= buf->b_ml.ml_line_count)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	якщо (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    якщо (u_savedel(n, 1) == OK)
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
    інакше
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    повернути Qnil;
}

    static VALUE
buffer_append(VALUE self, VALUE num, VALUE str)
{
    buf_T	*buf = get_buf(self);
    char	*line = StringValuePtr(str);
    long	n = NUM2LONG(num);
    aco_save_T	aco;

    якщо (line == NULL)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    інакше якщо (n >= 0 && n <= buf->b_ml.ml_line_count)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	якщо (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    якщо (u_inssub(n + 1) == OK)
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
    інакше
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    повернути str;
}

#ifdef USE_TYPEDDATA
static size_t window_dsize(const void *buf);

static const rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# якщо RUBY_VERSION >= 27
	0, {0}
# інакше
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    static size_t
window_dsize(const void *win UNUSED)
{
    повернути sizeof(win_T);
}
#endif

    static VALUE
window_new(win_T *win)
{
    якщо (win->w_ruby_ref)
    {
	повернути (VALUE) win->w_ruby_ref;
    }
    інакше
    {
#ifdef USE_TYPEDDATA
	VALUE obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#інакше
	VALUE obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (void *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	повернути obj;
    }
}

    static win_T *
get_win(VALUE obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#інакше
    Data_Get_Struct(obj, win_T, win);
#endif
    якщо (win == NULL)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    повернути win;
}

    static VALUE
window_s_current(VALUE self UNUSED)
{
    повернути window_new(curwin);
}

    static VALUE
window_s_current_getter(ID id UNUSED, VALUE *x UNUSED)
{
    повернути window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    static VALUE
line_s_current(VALUE self UNUSED)
{
    повернути get_buffer_line(curbuf, curwin->w_cursor.lnum);
}

    static VALUE
set_current_line(VALUE self UNUSED, VALUE str)
{
    повернути set_buffer_line(curbuf, curwin->w_cursor.lnum, str);
}

    static VALUE
current_line_number(VALUE self UNUSED)
{
    повернути INT2FIX((ціле)curwin->w_cursor.lnum);
}

    static VALUE
window_s_count(VALUE self UNUSED)
{
    win_T	*w;
    ціле n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    повернути INT2NUM(n);
}

    static VALUE
window_s_aref(VALUE self UNUSED, VALUE num)
{
    win_T *w;
    ціле n = NUM2INT(num);

    для (w = firstwin; w != NULL; w = w->w_next, --n)
	якщо (n == 0)
	    повернути window_new(w);
    повернути Qnil;
}

    static VALUE
window_buffer(VALUE self)
{
    win_T *win = get_win(self);

    повернути buffer_new(win->w_buffer);
}

    static VALUE
window_height(VALUE self)
{
    win_T *win = get_win(self);

    повернути INT2NUM(win->w_height);
}

    static VALUE
window_set_height(VALUE self, VALUE height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    повернути height;
}

    static VALUE
window_width(VALUE self UNUSED)
{
    повернути INT2NUM(get_win(self)->w_width);
}

    static VALUE
window_set_width(VALUE self UNUSED, VALUE width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    повернути width;
}

    static VALUE
window_cursor(VALUE self)
{
    win_T *win = get_win(self);

    повернути rb_assoc_new(INT2NUM(win->w_cursor.lnum), INT2NUM(win->w_cursor.col));
}

    static VALUE
window_set_cursor(VALUE self, VALUE pos)
{
    VALUE lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    якщо (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    повернути Qnil;
}

    static VALUE
f_nop(VALUE self UNUSED)
{
    повернути Qnil;
}

    static VALUE
f_p(ціле argc, VALUE *argv, VALUE self UNUSED)
{
    ціле i;
    VALUE str = rb_str_new("", 0);
    VALUE ret = Qnil;

    для (i = 0; i < argc; i++)
    {
	якщо (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    msg(RSTRING_PTR(str));

    якщо (argc == 1)
	ret = argv[0];
    інакше якщо (argc > 1)
	ret = rb_ary_new4(argc, argv);
    повернути ret;
}

    static void
ruby_io_init(void)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN VALUE rb_stdout;
    RUBYEXTERN VALUE rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (void*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (void*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (void*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (void*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    static void
ruby_vim_init(void)
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
    rb_define_singleton_method(cBuffer, "current", (void*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (void*)buffer_s_count, 0);
    rb_define_singleton_method(cBuffer, "[]", (void*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (void*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (void*)buffer_number, 0);
    rb_define_method(cBuffer, "count", (void*)buffer_count, 0);
    rb_define_method(cBuffer, "length", (void*)buffer_count, 0);
    rb_define_method(cBuffer, "[]", (void*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (void*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (void*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (void*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (void*)current_line_number, 0);
    rb_define_method(cBuffer, "line", (void*)line_s_current, 0);
    rb_define_method(cBuffer, "line=", (void*)set_current_line, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (void*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (void*)window_s_count, 0);
    rb_define_singleton_method(cVimWindow, "[]", (void*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (void*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (void*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (void*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (void*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (void*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (void*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (void*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    void
vim_ruby_init(void *stack_start)
{
    // should get machine stack start address early in main function
    ruby_stack_start = stack_start;
}

    static ціле
convert_hash2dict(VALUE key, VALUE val, VALUE arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(key)));
    якщо (di == NULL || ruby_convert_to_vim_value(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	повернути ST_STOP;
    }
    повернути ST_CONTINUE;
}

    static ціле
ruby_convert_to_vim_value(VALUE val, typval_T *rettv)
{
    switch (TYPE(val))
    {
	case T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NULL;
	    break;
	case T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = VVAL_TRUE;
	    break;
	case T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = VVAL_FALSE;
	    break;
	case T_BIGNUM:
	case T_FIXNUM:
	    rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = (varnumber_T)NUM2LONG(val);
	    break;
	case T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    break;
	default:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	case T_STRING:
	    {
		VALUE str = (VALUE)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    break;
	case T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		якщо (l == NULL)
		    повернути FAIL;

		для (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    якщо (ruby_convert_to_vim_value((VALUE)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			повернути FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    break;
	case T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		якщо (d == NULL)
		    повернути FAIL;

		rb_hash_foreach(val, convert_hash2dict, (VALUE)d);
		якщо (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    повернути FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    break;
    }
    повернути OK;
}

    void
do_rubyeval(char_u *str, typval_T *rettv)
{
    ціле retval = FAIL;

    якщо (ensure_ruby_initialized())
    {
	ціле state;
	VALUE obj;

	obj = rb_eval_string_protect((const char *)str, &state);
	якщо (state)
	    error_print(state);
	інакше
	    retval = ruby_convert_to_vim_value(obj, rettv);
    }
    якщо (retval == FAIL)
    {
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = 0;
    }
}
