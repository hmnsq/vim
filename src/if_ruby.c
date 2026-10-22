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
#اگر_تعریف_شدہ HAVE_CONFIG_H
# include "auto/config.h"
#اگر_ختم

#شامل <stdio.h>
#شامل <string.h>

#اگر_تعریف_شدہ _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ RUBYEXTERN
# define RUBYEXTERN خارجی
#اگر_ختم

#اگر_تعریف_شدہ DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# اگر RUBY_نسخہ >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# اگر defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# اگر RUBY_نسخہ >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_علامت		(*dll_rb_c_علامت)
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

# اگر RUBY_نسخہ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# اگر RUBY_نسخہ >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# اگر RUBY_نسخہ == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# اگر RUBY_نسخہ >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# اگر RUBY_نسخہ >= 26 && RUBY_نسخہ <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# اگر RUBY_نسخہ >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_سائز_حد_سے_تجاوز	ruby_malloc_سائز_حد_سے_تجاوز_stub
# endif

# اگر RUBY_نسخہ >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#اگر_ختم  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#اگر_تعریف_شدہ SIZEOF_وقت_T
# undef SIZEOF_وقت_T
#اگر_ختم

#اگر defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ڈالو
# pragma GCC diagnostic ignored "-Wunused-parameter"
#اگر_ختم

#اگر defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#اگر_ختم

#شامل <ruby.h>
#شامل <ruby/encoding.h>

#اگر defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic نکالو
#اگر_ختم

// See above.
#اگر_تعریف_شدہ SIZEOF_وقت_T
# undef SIZEOF_وقت_T
#اگر_ختم

#تعریف_منسوخ off_t	// ruby defines off_t as _int64, Mingw uses long
#تعریف_منسوخ EXTERN
#تعریف_منسوخ _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#اگر defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#اگر_ختم

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#اگر defined(Typed_ڈیٹا_Wrap_Struct) && (RUBY_نسخہ >= 20)
# define USE_TYPEDDATA	1
#اگر_ختم

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#اگر_غیر_تعریف_شدہ String_قدر_Ptr
# define String_قدر_Ptr(s) STR2CSTR(s)
#اگر_ختم
#اگر_غیر_تعریف_شدہ RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#اگر_ختم
#اگر_غیر_تعریف_شدہ RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#اگر_ختم
#اگر_غیر_تعریف_شدہ RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#اگر_ختم
#اگر_غیر_تعریف_شدہ RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#اگر_ختم

#اگر_تعریف_شدہ HAVE_DUP
# undef HAVE_DUP
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FSYNC
# undef HAVE_FSYNC
#اگر_ختم

// Avoid redefining TRUE/FALSE in vterm.h.
#اگر_تعریف_شدہ TRUE
# undef TRUE
#اگر_ختم
#اگر_تعریف_شدہ FALSE
# undef FALSE
#اگر_ختم

#شامل "vim.h"
#شامل "version.h"

#اگر_تعریف_شدہ DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define بھرو_dll vim_بھرو_Lib
#  define علامت_from_dll Get_Proc_پتہ
#  define close_dll FreeLibrary
#  define بھرو_dll_error GetWin32Error
# ورنہ
#  include <dlfcn.h>
#  define HINSTANCE خالی*
#  define RUBY_PROC خالی*
#  define بھرو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define علامت_from_dll dlsym
#  define close_dll dlclose
#  define بھرو_dll_error dlerror
# endif
#اگر_ختم

ساکن صحیح ruby_initialized = 0;
ساکن خالی *ruby_ڈھیر_آغاز;
ساکن قدر_3 objtbl;

ساکن قدر_3 mVIM;
ساکن قدر_3 c_بفر;
ساکن قدر_3 c_Vim_کھڑکی;
ساکن قدر_3 e_Deleted_بفر_Error;
ساکن قدر_3 e_Deleted_کھڑکی_Error;

ساکن صحیح ensure_ruby_initialized(خالی);
ساکن خالی error_print(صحیح);
ساکن خالی ruby_io_init(خالی);
ساکن خالی ruby_vim_init(خالی);
ساکن صحیح ruby_convert_مقصد_vim_قدر(قدر_3 val, typval_T *rettv);

#اگر defined(__ia64) && !defined(ruby_init_ڈھیر)
# define ruby_init_ڈھیر(addr) ruby_init_ڈھیر((addr), rb_ia64_bsp())
#اگر_ختم

#اگر defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# اگر RUBY_نسخہ >= 27
#  undef rb_define_global_function
#  undef rb_define_طریقہ
#  undef rb_define_module_function
#  undef rb_define_singleton_طریقہ
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# اگر RUBY_نسخہ < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_راستہ			dll_rb_class_راستہ
# ifdef USE_TYPEDDATA
#  اگر RUBY_نسخہ >= 23
#   define rb_ڈیٹا_typed_object_wrap	dll_rb_ڈیٹا_typed_object_wrap
#  ورنہ
#   define rb_ڈیٹا_typed_object_alloc	dll_rb_ڈیٹا_typed_object_alloc
#  endif
# ورنہ
#  define rb_ڈیٹا_object_alloc		dll_rb_ڈیٹا_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_طریقہ		dll_rb_define_طریقہ
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_طریقہ	dll_rb_define_singleton_طریقہ
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_اشاریہ_Error			(*dll_rb_e_اشاریہ_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# اگر RUBY_نسخہ >= 21
#  define rb_funcallv			dll_rb_funcallv
# ورنہ
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
#  اگر RUBY_نسخہ < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_سیٹ			dll_rb_lastline_سیٹ
# define rb_protect			dll_rb_protect
# define rb_بھرو			dll_rb_بھرو
# اگر RUBY_نسخہ < 20
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
# ورنہ
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_قدر		dll_rb_string_قدر
# define rb_string_قدر_ptr		dll_rb_string_قدر_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ڈالو			dll_rb_ary_ڈالو
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ڈھیر
#  define ruby_init_ڈھیر(addr)	dll_ruby_init_ڈھیر((addr), rb_ia64_bsp())
# ورنہ
#  define ruby_init_ڈھیر		dll_ruby_init_ڈھیر
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_ڈھونڈو_اشاریہ		dll_rb_enc_ڈھونڈو_اشاریہ
# define rb_enc_ڈھونڈو			dll_rb_enc_ڈھونڈو
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
ساکن قدر_3 (*dll_rb_assoc_new) (قدر_3, قدر_3);
قدر_3 *dll_rb_cFalseClass;
قدر_3 *dll_rb_cFixnum;
# اگر defined(USE_RUBY_INTEGER)
قدر_3 *dll_rb_cInteger;
# endif
# اگر RUBY_نسخہ >= 20
قدر_3 *dll_rb_cFloat;
# endif
قدر_3 *dll_rb_cNilClass;
ساکن قدر_3 *dll_rb_cObject;
قدر_3 *dll_rb_cString;
قدر_3 *dll_rb_c_علامت;
قدر_3 *dll_rb_cTrueClass;
ساکن قدر_3 (*dll_rb_class_new_instance) (صحیح,قدر_3*,قدر_3);
ساکن خالی (*dll_rb_check_type) (قدر_3,صحیح);
# ifdef USE_TYPEDDATA
ساکن خالی *(*dll_rb_check_typeddata) (قدر_3,ثابت rb_ڈیٹا_type_t *);
# endif
ساکن قدر_3 (*dll_rb_class_راستہ) (قدر_3);
# ifdef USE_TYPEDDATA
#  اگر RUBY_نسخہ >= 23
ساکن قدر_3 (*dll_rb_ڈیٹا_typed_object_wrap) (قدر_3, خالی*, ثابت rb_ڈیٹا_type_t *);
#  ورنہ
ساکن قدر_3 (*dll_rb_ڈیٹا_typed_object_alloc) (قدر_3, خالی*, ثابت rb_ڈیٹا_type_t *);
#  endif
# ورنہ
ساکن قدر_3 (*dll_rb_ڈیٹا_object_alloc) (قدر_3, خالی*, RUBY_ڈیٹا_FUNC, RUBY_ڈیٹا_FUNC);
# endif
# اگر RUBY_نسخہ >= 31
ساکن خالی (*dll_rb_debug_rstring_null_ptr) (ثابت حرف*);
# endif
ساکن قدر_3 (*dll_rb_define_class_under) (قدر_3, ثابت حرف*, قدر_3);
ساکن خالی (*dll_rb_define_const) (قدر_3,ثابت حرف*,قدر_3);
ساکن خالی (*dll_rb_define_global_function) (ثابت حرف*,قدر_3(*)(صحیح,قدر_3*,قدر_3),صحیح);
ساکن خالی (*dll_rb_define_طریقہ) (قدر_3,ثابت حرف*,قدر_3(*)(ANYARGS),صحیح);
ساکن قدر_3 (*dll_rb_define_module) (ثابت حرف*);
ساکن خالی (*dll_rb_define_module_function) (قدر_3,ثابت حرف*,قدر_3(*)(قدر_3,قدر_3),صحیح);
ساکن خالی (*dll_rb_define_singleton_طریقہ) (قدر_3,ثابت حرف*,قدر_3(*)(ANYARGS),صحیح);
ساکن خالی (*dll_rb_define_virtual_variable) (ثابت حرف*,قدر_3(*)(ID,قدر_3*),خالی(*)());
ساکن قدر_3 *dll_rb_stdout;
ساکن قدر_3 *dll_rb_stderr;
ساکن قدر_3 *dll_rb_eArgError;
ساکن قدر_3 *dll_rb_e_اشاریہ_Error;
ساکن قدر_3 *dll_rb_eRuntimeError;
ساکن قدر_3 *dll_rb_eStandardError;
ساکن قدر_3 (*dll_rb_eval_string_protect) (ثابت حرف*, صحیح*);
# اگر RUBY_نسخہ >= 21
ساکن قدر_3 (*dll_rb_funcallv) (قدر_3, ID, صحیح, ثابت قدر_3*);
# ورنہ
ساکن قدر_3 (*dll_rb_funcall2) (قدر_3, ID, صحیح, ثابت قدر_3*);
# endif
ساکن خالی (*dll_rb_global_variable) (قدر_3*);
ساکن قدر_3 (*dll_rb_hash_aset) (قدر_3, قدر_3, قدر_3);
ساکن قدر_3 (*dll_rb_hash_foreach) (قدر_3, صحیح (*)(قدر_3, قدر_3, قدر_3), قدر_3);
ساکن قدر_3 (*dll_rb_hash_new) (خالی);
ساکن قدر_3 (*dll_rb_inspect) (قدر_3);
ساکن قدر_3 (*dll_rb_int2inum) (طویل);
ساکن ID (*dll_rb_intern) (ثابت حرف*);
# اگر RUBY_نسخہ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ساکن طویل (*dll_rb_fix2int) (قدر_3);
ساکن طویل (*dll_rb_num2int) (قدر_3);
ساکن غیرعلامتی طویل (*dll_rb_num2uint) (قدر_3);
# endif
ساکن دوہرا (*dll_rb_num2dbl) (قدر_3);
ساکن قدر_3 (*dll_rb_lastline_get) (خالی);
ساکن خالی (*dll_rb_lastline_سیٹ) (قدر_3);
ساکن قدر_3 (*dll_rb_protect) (قدر_3 (*)(قدر_3), قدر_3, صحیح*);
ساکن خالی (*dll_rb_بھرو) (قدر_3, صحیح);
ساکن طویل (*dll_rb_num2long) (قدر_3);
ساکن غیرعلامتی طویل (*dll_rb_num2ulong) (قدر_3);
ساکن قدر_3 (*dll_rb_obj_alloc) (قدر_3);
ساکن قدر_3 (*dll_rb_obj_as_string) (قدر_3);
ساکن قدر_3 (*dll_rb_obj_id) (قدر_3);
ساکن خالی (*dll_rb_raise) (قدر_3, ثابت حرف*, ...);
ساکن قدر_3 (*dll_rb_string_قدر) (متلون قدر_3*);
ساکن قدر_3 (*dll_rb_str_cat) (قدر_3, ثابت حرف*, طویل);
ساکن قدر_3 (*dll_rb_str_concat) (قدر_3, قدر_3);
ساکن قدر_3 (*dll_rb_str_new) (ثابت حرف*, طویل);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ساکن قدر_3 (*dll_rb_str_new_cstr) (ثابت حرف*);
# ورنہ
ساکن قدر_3 (*dll_rb_str_new2) (ثابت حرف*);
# endif
ساکن قدر_3 (*dll_rb_errinfo) (خالی);
ساکن خالی (*dll_ruby_init) (خالی);
ساکن خالی (*dll_ruby_init_loadpath) (خالی);
# ifdef MSWIN
ساکن خالی (*dll_ruby_sysinit) (صحیح*, حرف***);
ساکن صحیح (*dll_rb_w32_snprintf)(حرف*, size_t, ثابت حرف*, ...);
# endif
# اگر RUBY_نسخہ >= 31
#  ifdef _MSC_VER
ساکن خالی (*dll_rb_unexpected_type) (قدر_3, صحیح);
#  ورنہ
NORETURN(ساکن خالی (*dll_rb_unexpected_type) (قدر_3, صحیح));
#  endif
# endif
ساکن حرف * (*dll_rb_string_قدر_ptr) (متلون قدر_3*);
ساکن قدر_3 (*dll_rb_float_new) (دوہرا);
ساکن قدر_3 (*dll_rb_ary_new) (خالی);
ساکن قدر_3 (*dll_rb_ary_new4) (طویل n, ثابت قدر_3 *elts);
ساکن قدر_3 (*dll_rb_ary_ڈالو) (قدر_3, قدر_3);
# اگر RUBY_نسخہ >= 26 && RUBY_نسخہ <= 32
ساکن خالی (*dll_rb_ary_detransient) (قدر_3);
# endif
# ifdef __ia64
ساکن خالی * (*dll_rb_ia64_bsp) (خالی);
ساکن خالی (*dll_ruby_init_ڈھیر)(قدر_3*, خالی*);
# ورنہ
ساکن خالی (*dll_ruby_init_ڈھیر)(قدر_3*);
# endif
ساکن قدر_3 (*dll_rb_int2big)(SIGNED_قدر);

ساکن خالی (*dll_ruby_script) (ثابت حرف*);
ساکن صحیح (*dll_rb_enc_ڈھونڈو_اشاریہ) (ثابت حرف*);
ساکن rb_encoding* (*dll_rb_enc_ڈھونڈو) (ثابت حرف*);
ساکن قدر_3 (*dll_rb_enc_str_new) (ثابت حرف*, طویل, rb_encoding*);
ساکن قدر_3 (*dll_rb_sprintf) (ثابت حرف*, ...);
ساکن قدر_3 (*dll_rb_require) (ثابت حرف*);
ساکن خالی* (*dll_ruby_options)(صحیح, حرف**);

# اگر defined(USE_RGENGC) && USE_RGENGC
#  اگر RUBY_نسخہ == 21
ساکن خالی (*dll_rb_gc_writebarrier_unprotect_promoted)(قدر_3);
#  ورنہ
ساکن خالی (*dll_rb_gc_writebarrier_unprotect)(قدر_3 obj);
#  endif
# endif

# اگر RUBY_نسخہ >= 30
#  ifdef _MSC_VER
ساکن خالی (*dll_ruby_malloc_سائز_حد_سے_تجاوز)(size_t, size_t);
#  ورنہ
NORETURN(ساکن خالی (*dll_ruby_malloc_سائز_حد_سے_تجاوز)(size_t, size_t));
#  endif
# endif

# اگر RUBY_نسخہ >= 26 && RUBY_نسخہ <= 32
خالی rb_ary_detransient_stub(قدر_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  اگر RUBY_نسخہ >= 22
    طویل
rb_num2long_stub(قدر_3 x)
#  ورنہ
    SIGNED_قدر
rb_num2long_stub(قدر_3 x)
#  endif
{
    واپس dll_rb_num2long(x);
}
#  اگر RUBY_نسخہ >= 26
    قدر_3
rb_int2big_stub(intptr_t x)
#  ورنہ
    قدر_3
rb_int2big_stub(SIGNED_قدر x)
#  endif
{
    واپس dll_rb_int2big(x);
}
#  اگر RUBY_نسخہ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    طویل
rb_fix2int_stub(قدر_3 x)
{
    واپس dll_rb_fix2int(x);
}
    طویل
rb_num2int_stub(قدر_3 x)
{
    واپس dll_rb_num2int(x);
}
#  endif
#  اگر RUBY_نسخہ >= 20
    قدر_3
rb_float_new_اندر_ڈھیر(دوہرا d)
{
    واپس dll_rb_float_new(d);
}
#   اگر RUBY_نسخہ >= 22
    غیرعلامتی طویل
rb_num2ulong(قدر_3 x)
#   ورنہ
    قدر_3
rb_num2ulong(قدر_3 x)
#   endif
{
    واپس (طویل)RSHIFT((SIGNED_قدر)(x),1);
}
#  endif
#  اگر defined(USE_RGENGC) && USE_RGENGC
#   اگر RUBY_نسخہ == 21
    خالی
rb_gc_writebarrier_unprotect_promoted_stub(قدر_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ورنہ
    خالی
rb_gc_writebarrier_unprotect_stub(قدر_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  اگر RUBY_نسخہ >= 26 && RUBY_نسخہ <= 32
    خالی
rb_ary_detransient_stub(قدر_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  اگر RUBY_نسخہ >= 30
    خالی
rb_check_type_stub(قدر_3 obj, صحیح t)
{
    dll_rb_check_type(obj, t);
}
    غیرعلامتی طویل
rb_num2uint_stub(قدر_3 x)
{
    واپس dll_rb_num2uint(x);
}
    خالی
ruby_malloc_سائز_حد_سے_تجاوز_stub(size_t x, size_t y)
{
    dll_ruby_malloc_سائز_حد_سے_تجاوز(x, y);
}
#  endif
#  اگر RUBY_نسخہ >= 31
    خالی
rb_debug_rstring_null_ptr_stub(ثابت حرف *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    خالی
rb_unexpected_type_stub(قدر_3 self, صحیح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
خالی *rb_check_typeddata_stub(قدر_3 obj, ثابت rb_ڈیٹا_type_t *ڈیٹا_type)
{
    واپس dll_rb_check_typeddata(obj, ڈیٹا_type);
}
#  endif
# endif // ifndef PROTO

ساکن HINSTANCE hinstRuby = تہی; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ساکن ساخت
{
    حرف *name;
    RUBY_PROC *ptr;
} ruby_funcname_جدول[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# اگر defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ورنہ
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# اگر RUBY_نسخہ >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_علامت},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_راستہ},
# ifdef USE_TYPEDDATA
#  اگر RUBY_نسخہ >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_ڈیٹا_typed_object_wrap},
#  ورنہ
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_ڈیٹا_typed_object_alloc},
#  endif
# ورنہ
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_ڈیٹا_object_alloc},
# endif
# اگر RUBY_نسخہ >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_طریقہ},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_طریقہ},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_اشاریہ_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# اگر RUBY_نسخہ >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ورنہ
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# اگر RUBY_نسخہ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_سیٹ},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_بھرو},
    {"rb_num2long", (RUBY_PROC*)&dll_rb_num2long},
    {"rb_num2ulong", (RUBY_PROC*)&dll_rb_num2ulong},
    {"rb_obj_alloc", (RUBY_PROC*)&dll_rb_obj_alloc},
    {"rb_obj_as_string", (RUBY_PROC*)&dll_rb_obj_as_string},
    {"rb_obj_id", (RUBY_PROC*)&dll_rb_obj_id},
    {"rb_raise", (RUBY_PROC*)&dll_rb_raise},
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_قدر},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# ورنہ
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# اگر RUBY_نسخہ >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_قدر_ptr},
# اگر RUBY_نسخہ >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ورنہ
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ورنہ
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ڈالو},
# اگر RUBY_نسخہ >= 26 && RUBY_نسخہ <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_ڈھونڈو_اشاریہ},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_ڈھونڈو},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ڈھیر},
# اگر defined(USE_RGENGC) && USE_RGENGC
#  اگر RUBY_نسخہ == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ورنہ
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# اگر RUBY_نسخہ >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_سائز_حد_سے_تجاوز},
# endif
    {"", تہی},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ساکن صحیح
ruby_runtime_link_init(حرف *libname, صحیح verbose)
{
    صحیح i;

    اگر (hinstRuby)
	واپس OK;
    hinstRuby = بھرو_dll(libname);
    اگر (!hinstRuby)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }

    برائے (i = 0; ruby_funcname_جدول[i].ptr; ++i)
    {
	اگر (!(*ruby_funcname_جدول[i].ptr = علامت_from_dll(hinstRuby,
			ruby_funcname_جدول[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = تہی;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), ruby_funcname_جدول[i].name);
	    واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    صحیح
ruby_enabled(صحیح verbose)
{
    واپس ruby_runtime_link_init((حرف *)p_rubydll, verbose) == OK;
}
#اگر_ختم // defined(DYNAMIC_RUBY)

    خالی
ruby_انتہا(خالی)
{
}

    خالی
ex_ruby(exarg_T *eap)
{
    صحیح state;
    حرف *script = تہی;

    script = (حرف *)script_get(eap, eap->arg);
    اگر (!eap->skip && ensure_ruby_initialized())
    {
	اگر (script == تہی)
	    rb_eval_string_protect((حرف *)eap->arg, &state);
	ورنہ
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
    ساکن قدر_3
vim_str2rb_enc_str(ثابت حرف *s)
{
    طویل lval;
    char_u *sval;
    rb_encoding *enc;

    اگر (get_option_قدر((char_u *)"enc", &lval, &sval, تہی, 0) == gov_string)
    {
	enc = rb_enc_ڈھونڈو((حرف *)sval);
	vim_free(sval);
	اگر (enc)
	    واپس rb_enc_str_new(s, (طویل)لڑی_لمبائی(s), enc);
    }
    واپس rb_str_new2(s);
}

    ساکن قدر_3
eval_enc_string_protect(ثابت حرف *str, صحیح *state)
{
    طویل lval;
    char_u *sval;
    rb_encoding *enc;
    قدر_3 v;

    اگر (get_option_قدر((char_u *)"enc", &lval, &sval, تہی, 0) == gov_string)
    {
	enc = rb_enc_ڈھونڈو((حرف *)sval);
	vim_free(sval);
	اگر (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    واپس rb_eval_string_protect(String_قدر_Ptr(v), state);
	}
    }
    واپس rb_eval_string_protect(str, state);
}

    خالی
ex_rubydo(exarg_T *eap)
{
    صحیح state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    اگر (!ensure_ruby_initialized())
	واپس;

    اگر (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	واپس;
    برائے (i = eap->line1; i <= eap->line2; i++)
    {
	قدر_3 سطر;

	اگر (i > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;
	سطر = vim_str2rb_enc_str((حرف *)ml_get(i));
	rb_lastline_سیٹ(سطر);
	eval_enc_string_protect((حرف *) eap->arg, &state);
	اگر (state)
	{
	    error_print(state);
	    توڑو;
	}
	اگر (was_curbuf != curbuf || i > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;
	سطر = rb_lastline_get();
	اگر (!NIL_P(سطر))
	{
	    اگر (TYPE(سطر) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_کا_string));
		واپس;
	    }
	    ml_replace(i, (char_u *) String_قدر_Ptr(سطر), 1);
	    بدلا();
#اگر_تعریف_شدہ SYNTAX_HL
	    syn_بدلا(i); // recompute syntax hl. for this line
#اگر_ختم
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساکن قدر_3
rb_بھرو_wrap(قدر_3 file_مقصد_بھرو)
{
    rb_بھرو(file_مقصد_بھرو, 0);
    واپس Qnil;
}

    خالی
ex_rubyfile(exarg_T *eap)
{
    صحیح state;

    اگر (!ensure_ruby_initialized())
	واپس;

    قدر_3 file_مقصد_بھرو = rb_str_new2((ثابت حرف *)eap->arg);
    rb_protect(rb_بھرو_wrap, file_مقصد_بھرو, &state);
    اگر (state)
	error_print(state);
}

    خالی
ruby_بفر_free(buf_T *buf)
{
    اگر (buf->b_ruby_ref == تہی)
	واپس;

    rb_hash_aset(objtbl, rb_obj_id((قدر_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->ڈیٹا = تہی;
}

    خالی
ruby_کھڑکی_free(win_T *win)
{
    اگر (win->w_ruby_ref == تہی)
	واپس;

    rb_hash_aset(objtbl, rb_obj_id((قدر_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->ڈیٹا = تہی;
}

    ساکن صحیح
ensure_ruby_initialized(خالی)
{
    اگر (ruby_initialized)
	واپس ruby_initialized;

#اگر_تعریف_شدہ DYNAMIC_RUBY
    اگر (ruby_enabled(TRUE))
#اگر_ختم
    {
#اگر_تعریف_شدہ MSWIN
	// suggested by Ariya Mizutani
	صحیح argc = 1;
	حرف *argv[] = {"gvim.exe"};
	حرف **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#اگر_ختم
	{
	    ruby_init_ڈھیر(ruby_ڈھیر_آغاز);
	    ruby_init();
	}
	{
	    صحیح dummy_argc = 2;
	    حرف *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#اگر_تعریف_شدہ DYNAMIC_RUBY
    ورنہ
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	واپس 0;
    }
#اگر_ختم

    واپس ruby_initialized;
}

    ساکن خالی
error_print(صحیح state)
{
    قدر_3 error;
    قدر_3 eclass;
    قدر_3 einfo;
    قدر_3 bt;
    صحیح attr;
    حرف buff[BUFSIZ];
    طویل i;

#تعریف_کرو لیبل_RETURN	0x1
#تعریف_کرو لیبل_BREAK	0x2
#تعریف_کرو لیبل_اگلا	0x3
#تعریف_کرو لیبل_RETRY	0x4
#تعریف_کرو لیبل_REDO	0x5
#تعریف_کرو لیبل_RAISE	0x6
#تعریف_کرو لیبل_THROW	0x7
#تعریف_کرو لیبل_FATAL	0x8
#تعریف_کرو لیبل_نقاب	0xf

    انتخاب (state)
    {
	حالت لیبل_RETURN:
	    emsg(_(e_unexpected_return));
	    توڑو;
	حالت لیبل_اگلا:
	    emsg(_(e_unexpected_اگلا));
	    توڑو;
	حالت لیبل_BREAK:
	    emsg(_(e_unexpected_break));
	    توڑو;
	حالت لیبل_REDO:
	    emsg(_(e_unexpected_redo));
	    توڑو;
	حالت لیبل_RETRY:
	    emsg(_(e_retry_outside_کا_rescue_clause));
	    توڑو;
	حالت لیبل_RAISE:
	حالت لیبل_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_کا(error);
	    einfo = rb_obj_as_string(error);
	    اگر (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ورنہ
	    {
		قدر_3 epath;
		حرف *p;

		epath = rb_class_راستہ(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		اگر (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#اگر RUBY_نسخہ >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    برائے (i = 0; i < RARRAY_LEN(bt); i++)
		پیغام_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ورنہ
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    برائے (i = 0; i < RARRAY_LEN(bt); i++)
		پیغام_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#اگر_ختم
	    توڑو;
	طےشدہ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_حالت_nr), state);
	    emsg(buff);
	    توڑو;
    }
}

    ساکن قدر_3
vim_پیغام(قدر_3 self غیر_مستعمل, قدر_3 str)
{
    حرف *buff, *p;

    str = rb_obj_as_string(str);
    اگر (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(حرف, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	اگر (p) *p = '\0';
	پیغام(buff);
    }
    ورنہ
    {
	پیغام("");
    }
    واپس Qnil;
}

    ساکن قدر_3
vim_سیٹ_option(قدر_3 self غیر_مستعمل, قدر_3 str)
{
    do_سیٹ((char_u *)String_قدر_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    واپس Qnil;
}

    ساکن قدر_3
vim_command(قدر_3 self غیر_مستعمل, قدر_3 str)
{
    do_cmdline_cmd((char_u *)String_قدر_Ptr(str));
    واپس Qnil;
}

#اگر_تعریف_شدہ FEAT_EVAL
    ساکن قدر_3
vim_مقصد_ruby(typval_T *tv)
{
    قدر_3 نتیجہ = Qnil;

    اگر (tv->v_type == VAR_STRING)
    {
	نتیجہ = rb_str_new2(tv->vval.v_string == تہی
					  ? "" : (حرف *)(tv->vval.v_string));
    }
    ورنہ اگر (tv->v_type == VAR_نمبر)
    {
	نتیجہ = INT2_عدد(tv->vval.v_نمبر);
    }
    ورنہ اگر (tv->v_type == VAR_FLOAT)
    {
	نتیجہ = rb_float_new(tv->vval.v_float);
    }
    ورنہ اگر (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	نتیجہ = rb_ary_new();

	اگر (list != تہی)
	{
	    FOR_ALL_LIST_اشیاء(list, curr)
		rb_ary_ڈالو(نتیجہ, vim_مقصد_ruby(&curr->li_tv));
	}
    }
    ورنہ اگر (tv->v_type == VAR_DICT)
    {
	نتیجہ = rb_hash_new();

	اگر (tv->vval.v_dict != تہی)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_مستعمل;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
	    {
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(نتیجہ, rb_str_new2((حرف *)hi->hi_کلید),
						     vim_مقصد_ruby(&di->di_tv));
		}
	    }
	}
    }
    ورنہ اگر (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	اگر (tv->vval.v_نمبر == VVAL_TRUE)
	    نتیجہ = Qtrue;
	ورنہ اگر (tv->vval.v_نمبر == VVAL_FALSE)
	    نتیجہ = Qfalse;
    }
    ورنہ اگر (tv->v_type == VAR_BLOB)
    {
	نتیجہ = rb_str_new(tv->vval.v_blob->bv_ga.ga_ڈیٹا,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    واپس نتیجہ;
}
#اگر_ختم

    ساکن قدر_3
vim_evaluate(قدر_3 self غیر_مستعمل, قدر_3 str)
{
#اگر_تعریف_شدہ FEAT_EVAL
    typval_T    *tv;
    قدر_3       نتیجہ;

    tv = eval_اظہاریہ((char_u *)String_قدر_Ptr(str), تہی);
    اگر (tv == تہی)
	واپس Qnil;
    نتیجہ = vim_مقصد_ruby(tv);

    free_tv(tv);

    واپس نتیجہ;
#ورنہ
    واپس Qnil;
#اگر_ختم
}

#اگر_تعریف_شدہ USE_TYPEDDATA
ساکن size_t بفر_dsize(ثابت خالی *buf);

ساکن ثابت rb_ڈیٹا_type_t بفر_type = {
    "vim_buffer",
    {0, 0, بفر_dsize,
# اگر RUBY_نسخہ >= 27
	0, {0}
# ورنہ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ساکن size_t
بفر_dsize(ثابت خالی *buf غیر_مستعمل)
{
    واپس حجم(buf_T);
}
#اگر_ختم

    ساکن قدر_3
بفر_new(buf_T *buf)
{
    اگر (buf->b_ruby_ref)
    {
	واپس (قدر_3) buf->b_ruby_ref;
    }
    ورنہ
    {
#اگر_تعریف_شدہ USE_TYPEDDATA
	قدر_3 obj = Typed_ڈیٹا_Wrap_Struct(c_بفر, &بفر_type, buf);
#ورنہ
	قدر_3 obj = ڈیٹا_Wrap_Struct(c_بفر, 0, 0, buf);
#اگر_ختم
	buf->b_ruby_ref = (خالی *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	واپس obj;
    }
}

    ساکن buf_T *
get_buf(قدر_3 obj)
{
    buf_T *buf;

#اگر_تعریف_شدہ USE_TYPEDDATA
    Typed_ڈیٹا_Get_Struct(obj, buf_T, &بفر_type, buf);
#ورنہ
    ڈیٹا_Get_Struct(obj, buf_T, buf);
#اگر_ختم
    اگر (buf == تہی)
	rb_raise(e_Deleted_بفر_Error, "attempt to refer to deleted buffer");
    واپس buf;
}

    ساکن قدر_3
vim_blob(قدر_3 self غیر_مستعمل, قدر_3 str)
{
    قدر_3 نتیجہ = rb_str_new("0z", 2);
    حرف    buf[4];
    صحیح	i;
    برائے (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (غیرعلامتی حرف)(RSTRING_PTR(str)[i]));
	rb_str_concat(نتیجہ, rb_str_new2(buf));
    }
    واپس نتیجہ;
}

    ساکن قدر_3
بفر_s_current(قدر_3 self غیر_مستعمل)
{
    واپس بفر_new(curbuf);
}

    ساکن قدر_3
بفر_s_current_getter(ID id غیر_مستعمل, قدر_3 *x غیر_مستعمل)
{
    واپس بفر_new(curbuf);
}

    ساکن قدر_3
بفر_s_گنتی(قدر_3 self غیر_مستعمل)
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

    واپس INT2_عدد(n);
}

    ساکن قدر_3
بفر_s_aref(قدر_3 self غیر_مستعمل, قدر_3 عدد)
{
    buf_T *b;
    صحیح n = NUM2INT(عدد);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	اگر (!b->b_p_bl)
	    جاری;

	اگر (n == 0)
	    واپس بفر_new(b);

	n--;
    }
    واپس Qnil;
}

    ساکن قدر_3
بفر_name(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    واپس buf->b_ffname ? rb_str_new2((حرف *)buf->b_ffname) : Qnil;
}

    ساکن قدر_3
بفر_نمبر(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    واپس INT2_عدد(buf->b_fnum);
}

    ساکن قدر_3
بفر_گنتی(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    واپس INT2_عدد(buf->b_ml.ml_سطر_گنتی);
}

    ساکن قدر_3
get_بفر_سطر(buf_T *buf, linenr_T n)
{
    اگر (n <= 0 || n > buf->b_ml.ml_سطر_گنتی)
	rb_raise(rb_e_اشاریہ_Error, "line number %ld out of range", (طویل)n);
    واپس vim_str2rb_enc_str((حرف *)ml_get_buf(buf, n, FALSE));
}

    ساکن قدر_3
بفر_aref(قدر_3 self, قدر_3 عدد)
{
    buf_T *buf = get_buf(self);

    اگر (buf != تہی)
	واپس get_بفر_سطر(buf, (linenr_T)NUM2LONG(عدد));
    واپس Qnil; // For stop warning
}

    ساکن قدر_3
سیٹ_بفر_سطر(buf_T *buf, linenr_T n, قدر_3 str)
{
    حرف	*سطر = String_قدر_Ptr(str);
    aco_save_T	aco;

    اگر (n > 0 && n <= buf->b_ml.ml_سطر_گنتی && سطر != تہی)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    اگر (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)سطر, TRUE);
		بدلا();
#اگر_تعریف_شدہ SYNTAX_HL
		syn_بدلا(n); // recompute syntax hl. for this line
#اگر_ختم
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ورنہ
    {
	rb_raise(rb_e_اشاریہ_Error, "line number %ld out of range", (طویل)n);
    }
    واپس str;
}

    ساکن قدر_3
بفر_aset(قدر_3 self, قدر_3 عدد, قدر_3 str)
{
    buf_T *buf = get_buf(self);

    اگر (buf != تہی)
	واپس سیٹ_بفر_سطر(buf, (linenr_T)NUM2LONG(عدد), str);
    واپس str;
}

    ساکن قدر_3
بفر_delete(قدر_3 self, قدر_3 عدد)
{
    buf_T	*buf = get_buf(self);
    طویل	n = NUM2LONG(عدد);
    aco_save_T	aco;

    اگر (n > 0 && n <= buf->b_ml.ml_سطر_گنتی)
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
		deleted_سطریں_mark(n, 1L);

		بدلا();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ورنہ
    {
	rb_raise(rb_e_اشاریہ_Error, "line number %ld out of range", n);
    }
    واپس Qnil;
}

    ساکن قدر_3
بفر_append(قدر_3 self, قدر_3 عدد, قدر_3 str)
{
    buf_T	*buf = get_buf(self);
    حرف	*سطر = String_قدر_Ptr(str);
    طویل	n = NUM2LONG(عدد);
    aco_save_T	aco;

    اگر (سطر == تہی)
    {
	rb_raise(rb_e_اشاریہ_Error, "NULL line");
    }
    ورنہ اگر (n >= 0 && n <= buf->b_ml.ml_سطر_گنتی)
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
		appended_سطریں_mark(n, 1L);

		بدلا();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ورنہ
    {
	rb_raise(rb_e_اشاریہ_Error, "line number %ld out of range", n);
    }
    واپس str;
}

#اگر_تعریف_شدہ USE_TYPEDDATA
ساکن size_t کھڑکی_dsize(ثابت خالی *buf);

ساکن ثابت rb_ڈیٹا_type_t کھڑکی_type = {
    "vim_window",
    {0, 0, کھڑکی_dsize,
# اگر RUBY_نسخہ >= 27
	0, {0}
# ورنہ
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ساکن size_t
کھڑکی_dsize(ثابت خالی *win غیر_مستعمل)
{
    واپس حجم(win_T);
}
#اگر_ختم

    ساکن قدر_3
کھڑکی_new(win_T *win)
{
    اگر (win->w_ruby_ref)
    {
	واپس (قدر_3) win->w_ruby_ref;
    }
    ورنہ
    {
#اگر_تعریف_شدہ USE_TYPEDDATA
	قدر_3 obj = Typed_ڈیٹا_Wrap_Struct(c_Vim_کھڑکی, &کھڑکی_type, win);
#ورنہ
	قدر_3 obj = ڈیٹا_Wrap_Struct(c_Vim_کھڑکی, 0, 0, win);
#اگر_ختم
	win->w_ruby_ref = (خالی *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	واپس obj;
    }
}

    ساکن win_T *
get_win(قدر_3 obj)
{
    win_T *win;

#اگر_تعریف_شدہ USE_TYPEDDATA
    Typed_ڈیٹا_Get_Struct(obj, win_T, &کھڑکی_type, win);
#ورنہ
    ڈیٹا_Get_Struct(obj, win_T, win);
#اگر_ختم
    اگر (win == تہی)
	rb_raise(e_Deleted_کھڑکی_Error, "attempt to refer to deleted window");
    واپس win;
}

    ساکن قدر_3
کھڑکی_s_current(قدر_3 self غیر_مستعمل)
{
    واپس کھڑکی_new(curwin);
}

    ساکن قدر_3
کھڑکی_s_current_getter(ID id غیر_مستعمل, قدر_3 *x غیر_مستعمل)
{
    واپس کھڑکی_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ساکن قدر_3
سطر_s_current(قدر_3 self غیر_مستعمل)
{
    واپس get_بفر_سطر(curbuf, curwin->w_cursor.lnum);
}

    ساکن قدر_3
سیٹ_current_سطر(قدر_3 self غیر_مستعمل, قدر_3 str)
{
    واپس سیٹ_بفر_سطر(curbuf, curwin->w_cursor.lnum, str);
}

    ساکن قدر_3
current_سطر_نمبر(قدر_3 self غیر_مستعمل)
{
    واپس INT2FIX((صحیح)curwin->w_cursor.lnum);
}

    ساکن قدر_3
کھڑکی_s_گنتی(قدر_3 self غیر_مستعمل)
{
    win_T	*w;
    صحیح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    واپس INT2_عدد(n);
}

    ساکن قدر_3
کھڑکی_s_aref(قدر_3 self غیر_مستعمل, قدر_3 عدد)
{
    win_T *w;
    صحیح n = NUM2INT(عدد);

    برائے (w = firstwin; w != تہی; w = w->w_اگلا, --n)
	اگر (n == 0)
	    واپس کھڑکی_new(w);
    واپس Qnil;
}

    ساکن قدر_3
کھڑکی_بفر(قدر_3 self)
{
    win_T *win = get_win(self);

    واپس بفر_new(win->w_بفر);
}

    ساکن قدر_3
کھڑکی_اونچائی(قدر_3 self)
{
    win_T *win = get_win(self);

    واپس INT2_عدد(win->w_اونچائی);
}

    ساکن قدر_3
کھڑکی_سیٹ_اونچائی(قدر_3 self, قدر_3 اونچائی)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(اونچائی));
    curwin = savewin;
    واپس اونچائی;
}

    ساکن قدر_3
کھڑکی_چوڑائی(قدر_3 self غیر_مستعمل)
{
    واپس INT2_عدد(get_win(self)->w_چوڑائی);
}

    ساکن قدر_3
کھڑکی_سیٹ_چوڑائی(قدر_3 self غیر_مستعمل, قدر_3 چوڑائی)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(چوڑائی));
    curwin = savewin;
    واپس چوڑائی;
}

    ساکن قدر_3
کھڑکی_cursor(قدر_3 self)
{
    win_T *win = get_win(self);

    واپس rb_assoc_new(INT2_عدد(win->w_cursor.lnum), INT2_عدد(win->w_cursor.ستون));
}

    ساکن قدر_3
کھڑکی_سیٹ_cursor(قدر_3 self, قدر_3 pos)
{
    قدر_3 lnum, ستون;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    اگر (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    ستون = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.ستون = NUM2UINT(ستون);
    win->w_سیٹ_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    واپس Qnil;
}

    ساکن قدر_3
f_nop(قدر_3 self غیر_مستعمل)
{
    واپس Qnil;
}

    ساکن قدر_3
f_p(صحیح argc, قدر_3 *argv, قدر_3 self غیر_مستعمل)
{
    صحیح i;
    قدر_3 str = rb_str_new("", 0);
    قدر_3 ret = Qnil;

    برائے (i = 0; i < argc; i++)
    {
	اگر (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    پیغام(RSTRING_PTR(str));

    اگر (argc == 1)
	ret = argv[0];
    ورنہ اگر (argc > 1)
	ret = rb_ary_new4(argc, argv);
    واپس ret;
}

    ساکن خالی
ruby_io_init(خالی)
{
#اگر_غیر_تعریف_شدہ DYNAMIC_RUBY
    RUBYEXTERN قدر_3 rb_stdout;
    RUBYEXTERN قدر_3 rb_stderr;
#اگر_ختم

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_طریقہ(rb_stdout, "write", (خالی*)vim_پیغام, 1);
    rb_define_singleton_طریقہ(rb_stdout, "flush", (خالی*)f_nop, 0);
    rb_define_singleton_طریقہ(rb_stderr, "write", (خالی*)vim_پیغام, 1);
    rb_define_singleton_طریقہ(rb_stderr, "flush", (خالی*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ساکن خالی
ruby_vim_init(خالی)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_عدد(VIM_نسخہ_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_عدد(VIM_نسخہ_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_عدد(VIM_نسخہ_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_عدد(VIM_نسخہ_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_نسخہ_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_نسخہ_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_نسخہ_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_نسخہ_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_پیغام, 1);
    rb_define_module_function(mVIM, "set_option", vim_سیٹ_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_بفر_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_کھڑکی_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_بفر = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_طریقہ(c_بفر, "current", (خالی*)بفر_s_current, 0);
    rb_define_singleton_طریقہ(c_بفر, "count", (خالی*)بفر_s_گنتی, 0);
    rb_define_singleton_طریقہ(c_بفر, "[]", (خالی*)بفر_s_aref, 1);
    rb_define_طریقہ(c_بفر, "name", (خالی*)بفر_name, 0);
    rb_define_طریقہ(c_بفر, "number", (خالی*)بفر_نمبر, 0);
    rb_define_طریقہ(c_بفر, "count", (خالی*)بفر_گنتی, 0);
    rb_define_طریقہ(c_بفر, "length", (خالی*)بفر_گنتی, 0);
    rb_define_طریقہ(c_بفر, "[]", (خالی*)بفر_aref, 1);
    rb_define_طریقہ(c_بفر, "[]=", (خالی*)بفر_aset, 2);
    rb_define_طریقہ(c_بفر, "delete", (خالی*)بفر_delete, 1);
    rb_define_طریقہ(c_بفر, "append", (خالی*)بفر_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_طریقہ(c_بفر, "line_number", (خالی*)current_سطر_نمبر, 0);
    rb_define_طریقہ(c_بفر, "line", (خالی*)سطر_s_current, 0);
    rb_define_طریقہ(c_بفر, "line=", (خالی*)سیٹ_current_سطر, 1);


    c_Vim_کھڑکی = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_طریقہ(c_Vim_کھڑکی, "current", (خالی*)کھڑکی_s_current, 0);
    rb_define_singleton_طریقہ(c_Vim_کھڑکی, "count", (خالی*)کھڑکی_s_گنتی, 0);
    rb_define_singleton_طریقہ(c_Vim_کھڑکی, "[]", (خالی*)کھڑکی_s_aref, 1);
    rb_define_طریقہ(c_Vim_کھڑکی, "buffer", (خالی*)کھڑکی_بفر, 0);
    rb_define_طریقہ(c_Vim_کھڑکی, "height", (خالی*)کھڑکی_اونچائی, 0);
    rb_define_طریقہ(c_Vim_کھڑکی, "height=", (خالی*)کھڑکی_سیٹ_اونچائی, 1);
    rb_define_طریقہ(c_Vim_کھڑکی, "width", (خالی*)کھڑکی_چوڑائی, 0);
    rb_define_طریقہ(c_Vim_کھڑکی, "width=", (خالی*)کھڑکی_سیٹ_چوڑائی, 1);
    rb_define_طریقہ(c_Vim_کھڑکی, "cursor", (خالی*)کھڑکی_cursor, 0);
    rb_define_طریقہ(c_Vim_کھڑکی, "cursor=", (خالی*)کھڑکی_سیٹ_cursor, 1);

    rb_define_virtual_variable("$curbuf", بفر_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", کھڑکی_s_current_getter, 0);
}

    خالی
vim_ruby_init(خالی *ڈھیر_آغاز)
{
    // should get machine stack start address early in main function
    ruby_ڈھیر_آغاز = ڈھیر_آغاز;
}

    ساکن صحیح
convert_hash2dict(قدر_3 کلید, قدر_3 val, قدر_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(کلید)));
    اگر (di == تہی || ruby_convert_مقصد_vim_قدر(val, &di->di_tv) != OK
						     || dict_جوڑو(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	واپس ST_STOP;
    }
    واپس ST_CONTINUE;
}

    ساکن صحیح
ruby_convert_مقصد_vim_قدر(قدر_3 val, typval_T *rettv)
{
    انتخاب (TYPE(val))
    {
	حالت T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_نمبر = VVAL_NULL;
	    توڑو;
	حالت T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_نمبر = VVAL_TRUE;
	    توڑو;
	حالت T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_نمبر = VVAL_FALSE;
	    توڑو;
	حالت T_BIGNUM:
	حالت T_FIXNUM:
	    rettv->v_type = VAR_نمبر;
	    rettv->vval.v_نمبر = (varnumber_T)NUM2LONG(val);
	    توڑو;
	حالت T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    توڑو;
	طےشدہ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	حالت T_STRING:
	    {
		قدر_3 str = (قدر_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    توڑو;
	حالت T_ARRAY:
	    {
		list_T *l;
		طویل i;
		typval_T v;

		l = list_alloc();
		اگر (l == تہی)
		    واپس FAIL;

		برائے (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    اگر (ruby_convert_مقصد_vim_قدر((قدر_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			واپس FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    توڑو;
	حالت T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		اگر (d == تہی)
		    واپس FAIL;

		rb_hash_foreach(val, convert_hash2dict, (قدر_3)d);
		اگر (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    واپس FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    توڑو;
    }
    واپس OK;
}

    خالی
do_rubyeval(char_u *str, typval_T *rettv)
{
    صحیح retval = FAIL;

    اگر (ensure_ruby_initialized())
    {
	صحیح state;
	قدر_3 obj;

	obj = rb_eval_string_protect((ثابت حرف *)str, &state);
	اگر (state)
	    error_print(state);
	ورنہ
	    retval = ruby_convert_مقصد_vim_قدر(obj, rettv);
    }
    اگر (retval == FAIL)
    {
	rettv->v_type = VAR_نمبر;
	rettv->vval.v_نمبر = 0;
    }
}
