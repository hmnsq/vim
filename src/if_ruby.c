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
#که_تعریف_شوی HAVE_CONFIG_H
# include "auto/config.h"
#د_که_پای

#ضمن <stdio.h>
#ضمن <string.h>

#که_تعریف_شوی _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#د_که_پای
#که_نه_تعریف_شوی RUBYEXTERN
# define RUBYEXTERN خارجي
#د_که_پای

#که_تعریف_شوی DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# إذا RUBY_نسخه >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# إذا defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# إذا RUBY_نسخه >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_نښه		(*dll_rb_c_نښه)
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

# إذا RUBY_نسخه >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# إذا RUBY_نسخه >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# إذا RUBY_نسخه == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# إذا RUBY_نسخه >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# إذا RUBY_نسخه >= 26 && RUBY_نسخه <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# إذا RUBY_نسخه >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_اندازه_ډکېدنه	ruby_malloc_اندازه_ډکېدنه_stub
# endif

# إذا RUBY_نسخه >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#د_که_پای  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#که_تعریف_شوی SIZEOF_وخت_T
# undef SIZEOF_وخت_T
#د_که_پای

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ټېل
# pragma GCC diagnostic ignored "-Wunused-parameter"
#د_که_پای

#إذا defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#د_که_پای

#ضمن <ruby.h>
#ضمن <ruby/encoding.h>

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic راوباسه
#د_که_پای

// See above.
#که_تعریف_شوی SIZEOF_وخت_T
# undef SIZEOF_وخت_T
#د_که_پای

#تعریف_لغوه off_t	// ruby defines off_t as _int64, Mingw uses long
#تعریف_لغوه EXTERN
#تعریف_لغوه _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#إذا defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#د_که_پای

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#إذا defined(Typed_معلومات_Wrap_Struct) && (RUBY_نسخه >= 20)
# define USE_TYPEDDATA	1
#د_که_پای

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#که_نه_تعریف_شوی String_ارزښت_Ptr
# define String_ارزښت_Ptr(s) STR2CSTR(s)
#د_که_پای
#که_نه_تعریف_شوی RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#د_که_پای
#که_نه_تعریف_شوی RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#د_که_پای
#که_نه_تعریف_شوی RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#د_که_پای
#که_نه_تعریف_شوی RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#د_که_پای

#که_تعریف_شوی HAVE_DUP
# undef HAVE_DUP
#د_که_پای
#که_تعریف_شوی HAVE_FSYNC
# undef HAVE_FSYNC
#د_که_پای

// Avoid redefining TRUE/FALSE in vterm.h.
#که_تعریف_شوی TRUE
# undef TRUE
#د_که_پای
#که_تعریف_شوی FALSE
# undef FALSE
#د_که_پای

#ضمن "vim.h"
#ضمن "version.h"

#که_تعریف_شوی DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define بارول_dll vim_بارول_Lib
#  define نښه_from_dll Get_Proc_پته
#  define close_dll FreeLibrary
#  define بارول_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define HINSTANCE فراغ*
#  define RUBY_PROC فراغ*
#  define بارول_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define نښه_from_dll dlsym
#  define close_dll dlclose
#  define بارول_dll_error dlerror
# endif
#د_که_پای

ساكن صحيح ruby_initialized = 0;
ساكن فراغ *ruby_ډېری_پیل;
ساكن ارزښت_3 objtbl;

ساكن ارزښت_3 mVIM;
ساكن ارزښت_3 c_بفر;
ساكن ارزښت_3 c_Vim_کړکۍ;
ساكن ارزښت_3 e_Deleted_بفر_Error;
ساكن ارزښت_3 e_Deleted_کړکۍ_Error;

ساكن صحيح ensure_ruby_initialized(فراغ);
ساكن فراغ error_print(صحيح);
ساكن فراغ ruby_io_init(فراغ);
ساكن فراغ ruby_vim_init(فراغ);
ساكن صحيح ruby_convert_منزل_vim_ارزښت(ارزښت_3 val, typval_T *rettv);

#إذا defined(__ia64) && !defined(ruby_init_ډېری)
# define ruby_init_ډېری(addr) ruby_init_ډېری((addr), rb_ia64_bsp())
#د_که_پای

#إذا defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# إذا RUBY_نسخه >= 27
#  undef rb_define_global_function
#  undef rb_define_لاره
#  undef rb_define_module_function
#  undef rb_define_singleton_لاره
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# إذا RUBY_نسخه < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_لار			dll_rb_class_لار
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخه >= 23
#   define rb_معلومات_typed_object_wrap	dll_rb_معلومات_typed_object_wrap
#  وإلا
#   define rb_معلومات_typed_object_alloc	dll_rb_معلومات_typed_object_alloc
#  endif
# وإلا
#  define rb_معلومات_object_alloc		dll_rb_معلومات_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_لاره		dll_rb_define_لاره
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_لاره	dll_rb_define_singleton_لاره
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_شاخص_Error			(*dll_rb_e_شاخص_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# إذا RUBY_نسخه >= 21
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
#  إذا RUBY_نسخه < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_سېټ			dll_rb_lastline_سېټ
# define rb_protect			dll_rb_protect
# define rb_بارول			dll_rb_بارول
# إذا RUBY_نسخه < 20
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
# define rb_string_ارزښت		dll_rb_string_ارزښت
# define rb_string_ارزښت_ptr		dll_rb_string_ارزښت_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ټېل			dll_rb_ary_ټېل
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ډېری
#  define ruby_init_ډېری(addr)	dll_ruby_init_ډېری((addr), rb_ia64_bsp())
# وإلا
#  define ruby_init_ډېری		dll_ruby_init_ډېری
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_پیدا_کړه_شاخص		dll_rb_enc_پیدا_کړه_شاخص
# define rb_enc_پیدا_کړه			dll_rb_enc_پیدا_کړه
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
ساكن ارزښت_3 (*dll_rb_assoc_new) (ارزښت_3, ارزښت_3);
ارزښت_3 *dll_rb_cFalseClass;
ارزښت_3 *dll_rb_cFixnum;
# إذا defined(USE_RUBY_INTEGER)
ارزښت_3 *dll_rb_cInteger;
# endif
# إذا RUBY_نسخه >= 20
ارزښت_3 *dll_rb_cFloat;
# endif
ارزښت_3 *dll_rb_cNilClass;
ساكن ارزښت_3 *dll_rb_cObject;
ارزښت_3 *dll_rb_cString;
ارزښت_3 *dll_rb_c_نښه;
ارزښت_3 *dll_rb_cTrueClass;
ساكن ارزښت_3 (*dll_rb_class_new_instance) (صحيح,ارزښت_3*,ارزښت_3);
ساكن فراغ (*dll_rb_check_type) (ارزښت_3,صحيح);
# ifdef USE_TYPEDDATA
ساكن فراغ *(*dll_rb_check_typeddata) (ارزښت_3,ثابت rb_معلومات_type_t *);
# endif
ساكن ارزښت_3 (*dll_rb_class_لار) (ارزښت_3);
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخه >= 23
ساكن ارزښت_3 (*dll_rb_معلومات_typed_object_wrap) (ارزښت_3, فراغ*, ثابت rb_معلومات_type_t *);
#  وإلا
ساكن ارزښت_3 (*dll_rb_معلومات_typed_object_alloc) (ارزښت_3, فراغ*, ثابت rb_معلومات_type_t *);
#  endif
# وإلا
ساكن ارزښت_3 (*dll_rb_معلومات_object_alloc) (ارزښت_3, فراغ*, RUBY_معلومات_FUNC, RUBY_معلومات_FUNC);
# endif
# إذا RUBY_نسخه >= 31
ساكن فراغ (*dll_rb_debug_rstring_null_ptr) (ثابت حرف*);
# endif
ساكن ارزښت_3 (*dll_rb_define_class_under) (ارزښت_3, ثابت حرف*, ارزښت_3);
ساكن فراغ (*dll_rb_define_const) (ارزښت_3,ثابت حرف*,ارزښت_3);
ساكن فراغ (*dll_rb_define_global_function) (ثابت حرف*,ارزښت_3(*)(صحيح,ارزښت_3*,ارزښت_3),صحيح);
ساكن فراغ (*dll_rb_define_لاره) (ارزښت_3,ثابت حرف*,ارزښت_3(*)(ANYARGS),صحيح);
ساكن ارزښت_3 (*dll_rb_define_module) (ثابت حرف*);
ساكن فراغ (*dll_rb_define_module_function) (ارزښت_3,ثابت حرف*,ارزښت_3(*)(ارزښت_3,ارزښت_3),صحيح);
ساكن فراغ (*dll_rb_define_singleton_لاره) (ارزښت_3,ثابت حرف*,ارزښت_3(*)(ANYARGS),صحيح);
ساكن فراغ (*dll_rb_define_virtual_variable) (ثابت حرف*,ارزښت_3(*)(ID,ارزښت_3*),فراغ(*)());
ساكن ارزښت_3 *dll_rb_stdout;
ساكن ارزښت_3 *dll_rb_stderr;
ساكن ارزښت_3 *dll_rb_eArgError;
ساكن ارزښت_3 *dll_rb_e_شاخص_Error;
ساكن ارزښت_3 *dll_rb_eRuntimeError;
ساكن ارزښت_3 *dll_rb_eStandardError;
ساكن ارزښت_3 (*dll_rb_eval_string_protect) (ثابت حرف*, صحيح*);
# إذا RUBY_نسخه >= 21
ساكن ارزښت_3 (*dll_rb_funcallv) (ارزښت_3, ID, صحيح, ثابت ارزښت_3*);
# وإلا
ساكن ارزښت_3 (*dll_rb_funcall2) (ارزښت_3, ID, صحيح, ثابت ارزښت_3*);
# endif
ساكن فراغ (*dll_rb_global_variable) (ارزښت_3*);
ساكن ارزښت_3 (*dll_rb_hash_aset) (ارزښت_3, ارزښت_3, ارزښت_3);
ساكن ارزښت_3 (*dll_rb_hash_foreach) (ارزښت_3, صحيح (*)(ارزښت_3, ارزښت_3, ارزښت_3), ارزښت_3);
ساكن ارزښت_3 (*dll_rb_hash_new) (فراغ);
ساكن ارزښت_3 (*dll_rb_inspect) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_int2inum) (اوږد);
ساكن ID (*dll_rb_intern) (ثابت حرف*);
# إذا RUBY_نسخه >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ساكن اوږد (*dll_rb_fix2int) (ارزښت_3);
ساكن اوږد (*dll_rb_num2int) (ارزښت_3);
ساكن بینښه اوږد (*dll_rb_num2uint) (ارزښت_3);
# endif
ساكن مزدوج (*dll_rb_num2dbl) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_lastline_get) (فراغ);
ساكن فراغ (*dll_rb_lastline_سېټ) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_protect) (ارزښت_3 (*)(ارزښت_3), ارزښت_3, صحيح*);
ساكن فراغ (*dll_rb_بارول) (ارزښت_3, صحيح);
ساكن اوږد (*dll_rb_num2long) (ارزښت_3);
ساكن بینښه اوږد (*dll_rb_num2ulong) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_obj_alloc) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_obj_as_string) (ارزښت_3);
ساكن ارزښت_3 (*dll_rb_obj_id) (ارزښت_3);
ساكن فراغ (*dll_rb_raise) (ارزښت_3, ثابت حرف*, ...);
ساكن ارزښت_3 (*dll_rb_string_ارزښت) (متطاير ارزښت_3*);
ساكن ارزښت_3 (*dll_rb_str_cat) (ارزښت_3, ثابت حرف*, اوږد);
ساكن ارزښت_3 (*dll_rb_str_concat) (ارزښت_3, ارزښت_3);
ساكن ارزښت_3 (*dll_rb_str_new) (ثابت حرف*, اوږد);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ساكن ارزښت_3 (*dll_rb_str_new_cstr) (ثابت حرف*);
# وإلا
ساكن ارزښت_3 (*dll_rb_str_new2) (ثابت حرف*);
# endif
ساكن ارزښت_3 (*dll_rb_errinfo) (فراغ);
ساكن فراغ (*dll_ruby_init) (فراغ);
ساكن فراغ (*dll_ruby_init_loadpath) (فراغ);
# ifdef MSWIN
ساكن فراغ (*dll_ruby_sysinit) (صحيح*, حرف***);
ساكن صحيح (*dll_rb_w32_snprintf)(حرف*, size_t, ثابت حرف*, ...);
# endif
# إذا RUBY_نسخه >= 31
#  ifdef _MSC_VER
ساكن فراغ (*dll_rb_unexpected_type) (ارزښت_3, صحيح);
#  وإلا
NORETURN(ساكن فراغ (*dll_rb_unexpected_type) (ارزښت_3, صحيح));
#  endif
# endif
ساكن حرف * (*dll_rb_string_ارزښت_ptr) (متطاير ارزښت_3*);
ساكن ارزښت_3 (*dll_rb_float_new) (مزدوج);
ساكن ارزښت_3 (*dll_rb_ary_new) (فراغ);
ساكن ارزښت_3 (*dll_rb_ary_new4) (اوږد n, ثابت ارزښت_3 *elts);
ساكن ارزښت_3 (*dll_rb_ary_ټېل) (ارزښت_3, ارزښت_3);
# إذا RUBY_نسخه >= 26 && RUBY_نسخه <= 32
ساكن فراغ (*dll_rb_ary_detransient) (ارزښت_3);
# endif
# ifdef __ia64
ساكن فراغ * (*dll_rb_ia64_bsp) (فراغ);
ساكن فراغ (*dll_ruby_init_ډېری)(ارزښت_3*, فراغ*);
# وإلا
ساكن فراغ (*dll_ruby_init_ډېری)(ارزښت_3*);
# endif
ساكن ارزښت_3 (*dll_rb_int2big)(SIGNED_ارزښت);

ساكن فراغ (*dll_ruby_script) (ثابت حرف*);
ساكن صحيح (*dll_rb_enc_پیدا_کړه_شاخص) (ثابت حرف*);
ساكن rb_encoding* (*dll_rb_enc_پیدا_کړه) (ثابت حرف*);
ساكن ارزښت_3 (*dll_rb_enc_str_new) (ثابت حرف*, اوږد, rb_encoding*);
ساكن ارزښت_3 (*dll_rb_sprintf) (ثابت حرف*, ...);
ساكن ارزښت_3 (*dll_rb_require) (ثابت حرف*);
ساكن فراغ* (*dll_ruby_options)(صحيح, حرف**);

# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_نسخه == 21
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect_promoted)(ارزښت_3);
#  وإلا
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect)(ارزښت_3 obj);
#  endif
# endif

# إذا RUBY_نسخه >= 30
#  ifdef _MSC_VER
ساكن فراغ (*dll_ruby_malloc_اندازه_ډکېدنه)(size_t, size_t);
#  وإلا
NORETURN(ساكن فراغ (*dll_ruby_malloc_اندازه_ډکېدنه)(size_t, size_t));
#  endif
# endif

# إذا RUBY_نسخه >= 26 && RUBY_نسخه <= 32
فراغ rb_ary_detransient_stub(ارزښت_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  إذا RUBY_نسخه >= 22
    اوږد
rb_num2long_stub(ارزښت_3 x)
#  وإلا
    SIGNED_ارزښت
rb_num2long_stub(ارزښت_3 x)
#  endif
{
    أرجع dll_rb_num2long(x);
}
#  إذا RUBY_نسخه >= 26
    ارزښت_3
rb_int2big_stub(intptr_t x)
#  وإلا
    ارزښت_3
rb_int2big_stub(SIGNED_ارزښت x)
#  endif
{
    أرجع dll_rb_int2big(x);
}
#  إذا RUBY_نسخه >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    اوږد
rb_fix2int_stub(ارزښت_3 x)
{
    أرجع dll_rb_fix2int(x);
}
    اوږد
rb_num2int_stub(ارزښت_3 x)
{
    أرجع dll_rb_num2int(x);
}
#  endif
#  إذا RUBY_نسخه >= 20
    ارزښت_3
rb_float_new_دننه_ډېرۍ(مزدوج d)
{
    أرجع dll_rb_float_new(d);
}
#   إذا RUBY_نسخه >= 22
    بینښه اوږد
rb_num2ulong(ارزښت_3 x)
#   وإلا
    ارزښت_3
rb_num2ulong(ارزښت_3 x)
#   endif
{
    أرجع (اوږد)RSHIFT((SIGNED_ارزښت)(x),1);
}
#  endif
#  إذا defined(USE_RGENGC) && USE_RGENGC
#   إذا RUBY_نسخه == 21
    فراغ
rb_gc_writebarrier_unprotect_promoted_stub(ارزښت_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   وإلا
    فراغ
rb_gc_writebarrier_unprotect_stub(ارزښت_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  إذا RUBY_نسخه >= 26 && RUBY_نسخه <= 32
    فراغ
rb_ary_detransient_stub(ارزښت_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  إذا RUBY_نسخه >= 30
    فراغ
rb_check_type_stub(ارزښت_3 obj, صحيح t)
{
    dll_rb_check_type(obj, t);
}
    بینښه اوږد
rb_num2uint_stub(ارزښت_3 x)
{
    أرجع dll_rb_num2uint(x);
}
    فراغ
ruby_malloc_اندازه_ډکېدنه_stub(size_t x, size_t y)
{
    dll_ruby_malloc_اندازه_ډکېدنه(x, y);
}
#  endif
#  إذا RUBY_نسخه >= 31
    فراغ
rb_debug_rstring_null_ptr_stub(ثابت حرف *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    فراغ
rb_unexpected_type_stub(ارزښت_3 self, صحيح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
فراغ *rb_check_typeddata_stub(ارزښت_3 obj, ثابت rb_معلومات_type_t *معلومات_type)
{
    أرجع dll_rb_check_typeddata(obj, معلومات_type);
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
} ruby_funcname_جدول[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# إذا defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# وإلا
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# إذا RUBY_نسخه >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_نښه},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_لار},
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخه >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_معلومات_typed_object_wrap},
#  وإلا
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_معلومات_typed_object_alloc},
#  endif
# وإلا
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_معلومات_object_alloc},
# endif
# إذا RUBY_نسخه >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_لاره},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_لاره},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_شاخص_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# إذا RUBY_نسخه >= 21
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
# إذا RUBY_نسخه >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_سېټ},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_بارول},
    {"rb_num2long", (RUBY_PROC*)&dll_rb_num2long},
    {"rb_num2ulong", (RUBY_PROC*)&dll_rb_num2ulong},
    {"rb_obj_alloc", (RUBY_PROC*)&dll_rb_obj_alloc},
    {"rb_obj_as_string", (RUBY_PROC*)&dll_rb_obj_as_string},
    {"rb_obj_id", (RUBY_PROC*)&dll_rb_obj_id},
    {"rb_raise", (RUBY_PROC*)&dll_rb_raise},
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_ارزښت},
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
# إذا RUBY_نسخه >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_ارزښت_ptr},
# إذا RUBY_نسخه >= 20
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
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ټېل},
# إذا RUBY_نسخه >= 26 && RUBY_نسخه <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_پیدا_کړه_شاخص},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_پیدا_کړه},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ډېری},
# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_نسخه == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  وإلا
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# إذا RUBY_نسخه >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_اندازه_ډکېدنه},
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
    hinstRuby = بارول_dll(libname);
    إذا (!hinstRuby)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارول_library_str_str), libname, بارول_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; ruby_funcname_جدول[i].ptr; ++i)
    {
	إذا (!(*ruby_funcname_جدول[i].ptr = نښه_from_dll(hinstRuby,
			ruby_funcname_جدول[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str), ruby_funcname_جدول[i].name);
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
#د_که_پای // defined(DYNAMIC_RUBY)

    فراغ
ruby_پای_ټکی(فراغ)
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
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ساكن ارزښت_3
vim_str2rb_enc_str(ثابت حرف *s)
{
    اوږد lval;
    char_u *sval;
    rb_encoding *enc;

    إذا (get_option_ارزښت((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_پیدا_کړه((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	    أرجع rb_enc_str_new(s, (اوږد)مزي_اوږدوالی(s), enc);
    }
    أرجع rb_str_new2(s);
}

    ساكن ارزښت_3
eval_enc_string_protect(ثابت حرف *str, صحيح *state)
{
    اوږد lval;
    char_u *sval;
    rb_encoding *enc;
    ارزښت_3 v;

    إذا (get_option_ارزښت((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_پیدا_کړه((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    أرجع rb_eval_string_protect(String_ارزښت_Ptr(v), state);
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
	ارزښت_3 کرښه;

	إذا (i > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;
	کرښه = vim_str2rb_enc_str((حرف *)ml_get(i));
	rb_lastline_سېټ(کرښه);
	eval_enc_string_protect((حرف *) eap->arg, &state);
	إذا (state)
	{
	    error_print(state);
	    اكسر;
	}
	إذا (was_curbuf != curbuf || i > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;
	کرښه = rb_lastline_get();
	إذا (!NIL_P(کرښه))
	{
	    إذا (TYPE(کرښه) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_د_string));
		أرجع;
	    }
	    ml_replace(i, (char_u *) String_ارزښت_Ptr(کرښه), 1);
	    بدل_شوی();
#که_تعریف_شوی SYNTAX_HL
	    syn_بدل_شوی(i); // recompute syntax hl. for this line
#د_که_پای
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن ارزښت_3
rb_بارول_wrap(ارزښت_3 file_منزل_بارول)
{
    rb_بارول(file_منزل_بارول, 0);
    أرجع Qnil;
}

    فراغ
ex_rubyfile(exarg_T *eap)
{
    صحيح state;

    إذا (!ensure_ruby_initialized())
	أرجع;

    ارزښت_3 file_منزل_بارول = rb_str_new2((ثابت حرف *)eap->arg);
    rb_protect(rb_بارول_wrap, file_منزل_بارول, &state);
    إذا (state)
	error_print(state);
}

    فراغ
ruby_بفر_free(buf_T *buf)
{
    إذا (buf->b_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((ارزښت_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->معلومات = عدم;
}

    فراغ
ruby_کړکۍ_free(win_T *win)
{
    إذا (win->w_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((ارزښت_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->معلومات = عدم;
}

    ساكن صحيح
ensure_ruby_initialized(فراغ)
{
    إذا (ruby_initialized)
	أرجع ruby_initialized;

#که_تعریف_شوی DYNAMIC_RUBY
    إذا (ruby_enabled(TRUE))
#د_که_پای
    {
#که_تعریف_شوی MSWIN
	// suggested by Ariya Mizutani
	صحيح argc = 1;
	حرف *argv[] = {"gvim.exe"};
	حرف **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#د_که_پای
	{
	    ruby_init_ډېری(ruby_ډېری_پیل);
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
#که_تعریف_شوی DYNAMIC_RUBY
    وإلا
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	أرجع 0;
    }
#د_که_پای

    أرجع ruby_initialized;
}

    ساكن فراغ
error_print(صحيح state)
{
    ارزښت_3 error;
    ارزښت_3 eclass;
    ارزښت_3 einfo;
    ارزښت_3 bt;
    صحيح attr;
    حرف buff[BUFSIZ];
    اوږد i;

#تعریف_کړه نښان_RETURN	0x1
#تعریف_کړه نښان_BREAK	0x2
#تعریف_کړه نښان_بل	0x3
#تعریف_کړه نښان_RETRY	0x4
#تعریف_کړه نښان_REDO	0x5
#تعریف_کړه نښان_RAISE	0x6
#تعریف_کړه نښان_THROW	0x7
#تعریف_کړه نښان_FATAL	0x8
#تعریف_کړه نښان_ماسک	0xf

    بدل (state)
    {
	حالة نښان_RETURN:
	    emsg(_(e_unexpected_return));
	    اكسر;
	حالة نښان_بل:
	    emsg(_(e_unexpected_بل));
	    اكسر;
	حالة نښان_BREAK:
	    emsg(_(e_unexpected_break));
	    اكسر;
	حالة نښان_REDO:
	    emsg(_(e_unexpected_redo));
	    اكسر;
	حالة نښان_RETRY:
	    emsg(_(e_retry_outside_د_rescue_clause));
	    اكسر;
	حالة نښان_RAISE:
	حالة نښان_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_د(error);
	    einfo = rb_obj_as_string(error);
	    إذا (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    وإلا
	    {
		ارزښت_3 epath;
		حرف *p;

		epath = rb_class_لار(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		إذا (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#إذا RUBY_نسخه >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		پیغام_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#وإلا
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		پیغام_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#د_که_پای
	    اكسر;
	افتراضي:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_حالت_nr), state);
	    emsg(buff);
	    اكسر;
    }
}

    ساكن ارزښت_3
vim_پیغام(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
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
	پیغام(buff);
    }
    وإلا
    {
	پیغام("");
    }
    أرجع Qnil;
}

    ساكن ارزښت_3
vim_سېټ_option(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
{
    do_سېټ((char_u *)String_ارزښت_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن ارزښت_3
vim_command(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
{
    do_cmdline_cmd((char_u *)String_ارزښت_Ptr(str));
    أرجع Qnil;
}

#که_تعریف_شوی FEAT_EVAL
    ساكن ارزښت_3
vim_منزل_ruby(typval_T *tv)
{
    ارزښت_3 پایله = Qnil;

    إذا (tv->v_type == VAR_STRING)
    {
	پایله = rb_str_new2(tv->vval.v_string == عدم
					  ? "" : (حرف *)(tv->vval.v_string));
    }
    وإلا إذا (tv->v_type == VAR_شمېره)
    {
	پایله = INT2_عدد(tv->vval.v_شمېره);
    }
    وإلا إذا (tv->v_type == VAR_FLOAT)
    {
	پایله = rb_float_new(tv->vval.v_float);
    }
    وإلا إذا (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	پایله = rb_ary_new();

	إذا (list != عدم)
	{
	    FOR_ALL_LIST_توکي(list, curr)
		rb_ary_ټېل(پایله, vim_منزل_ruby(&curr->li_tv));
	}
    }
    وإلا إذا (tv->v_type == VAR_DICT)
    {
	پایله = rb_hash_new();

	إذا (tv->vval.v_dict != عدم)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_کارول_شوی;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
	    {
		إذا (!HASHITEM_تشه(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(پایله, rb_str_new2((حرف *)hi->hi_کلی),
						     vim_منزل_ruby(&di->di_tv));
		}
	    }
	}
    }
    وإلا إذا (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	إذا (tv->vval.v_شمېره == VVAL_TRUE)
	    پایله = Qtrue;
	وإلا إذا (tv->vval.v_شمېره == VVAL_FALSE)
	    پایله = Qfalse;
    }
    وإلا إذا (tv->v_type == VAR_BLOB)
    {
	پایله = rb_str_new(tv->vval.v_blob->bv_ga.ga_معلومات,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    أرجع پایله;
}
#د_که_پای

    ساكن ارزښت_3
vim_evaluate(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
{
#که_تعریف_شوی FEAT_EVAL
    typval_T    *tv;
    ارزښت_3       پایله;

    tv = eval_بیان((char_u *)String_ارزښت_Ptr(str), عدم);
    إذا (tv == عدم)
	أرجع Qnil;
    پایله = vim_منزل_ruby(tv);

    free_tv(tv);

    أرجع پایله;
#وإلا
    أرجع Qnil;
#د_که_پای
}

#که_تعریف_شوی USE_TYPEDDATA
ساكن size_t بفر_dsize(ثابت فراغ *buf);

ساكن ثابت rb_معلومات_type_t بفر_type = {
    "vim_buffer",
    {0, 0, بفر_dsize,
# إذا RUBY_نسخه >= 27
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
بفر_dsize(ثابت فراغ *buf ناکارول_شوی)
{
    أرجع حجم(buf_T);
}
#د_که_پای

    ساكن ارزښت_3
بفر_new(buf_T *buf)
{
    إذا (buf->b_ruby_ref)
    {
	أرجع (ارزښت_3) buf->b_ruby_ref;
    }
    وإلا
    {
#که_تعریف_شوی USE_TYPEDDATA
	ارزښت_3 obj = Typed_معلومات_Wrap_Struct(c_بفر, &بفر_type, buf);
#وإلا
	ارزښت_3 obj = معلومات_Wrap_Struct(c_بفر, 0, 0, buf);
#د_که_پای
	buf->b_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن buf_T *
get_buf(ارزښت_3 obj)
{
    buf_T *buf;

#که_تعریف_شوی USE_TYPEDDATA
    Typed_معلومات_Get_Struct(obj, buf_T, &بفر_type, buf);
#وإلا
    معلومات_Get_Struct(obj, buf_T, buf);
#د_که_پای
    إذا (buf == عدم)
	rb_raise(e_Deleted_بفر_Error, "attempt to refer to deleted buffer");
    أرجع buf;
}

    ساكن ارزښت_3
vim_blob(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
{
    ارزښت_3 پایله = rb_str_new("0z", 2);
    حرف    buf[4];
    صحيح	i;
    لكل (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (بینښه حرف)(RSTRING_PTR(str)[i]));
	rb_str_concat(پایله, rb_str_new2(buf));
    }
    أرجع پایله;
}

    ساكن ارزښت_3
بفر_s_current(ارزښت_3 self ناکارول_شوی)
{
    أرجع بفر_new(curbuf);
}

    ساكن ارزښت_3
بفر_s_current_getter(ID id ناکارول_شوی, ارزښت_3 *x ناکارول_شوی)
{
    أرجع بفر_new(curbuf);
}

    ساكن ارزښت_3
بفر_s_شمېر(ارزښت_3 self ناکارول_شوی)
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

    أرجع INT2_عدد(n);
}

    ساكن ارزښت_3
بفر_s_aref(ارزښت_3 self ناکارول_شوی, ارزښت_3 عدد)
{
    buf_T *b;
    صحيح n = NUM2INT(عدد);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	إذا (!b->b_p_bl)
	    استمر;

	إذا (n == 0)
	    أرجع بفر_new(b);

	n--;
    }
    أرجع Qnil;
}

    ساكن ارزښت_3
بفر_name(ارزښت_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع buf->b_ffname ? rb_str_new2((حرف *)buf->b_ffname) : Qnil;
}

    ساكن ارزښت_3
بفر_شمېره(ارزښت_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_عدد(buf->b_fnum);
}

    ساكن ارزښت_3
بفر_شمېر(ارزښت_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_عدد(buf->b_ml.ml_کرښه_شمېر);
}

    ساكن ارزښت_3
get_بفر_کرښه(buf_T *buf, linenr_T n)
{
    إذا (n <= 0 || n > buf->b_ml.ml_کرښه_شمېر)
	rb_raise(rb_e_شاخص_Error, "line number %ld out of range", (اوږد)n);
    أرجع vim_str2rb_enc_str((حرف *)ml_get_buf(buf, n, FALSE));
}

    ساكن ارزښت_3
بفر_aref(ارزښت_3 self, ارزښت_3 عدد)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع get_بفر_کرښه(buf, (linenr_T)NUM2LONG(عدد));
    أرجع Qnil; // For stop warning
}

    ساكن ارزښت_3
سېټ_بفر_کرښه(buf_T *buf, linenr_T n, ارزښت_3 str)
{
    حرف	*کرښه = String_ارزښت_Ptr(str);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_کرښه_شمېر && کرښه != عدم)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)کرښه, TRUE);
		بدل_شوی();
#که_تعریف_شوی SYNTAX_HL
		syn_بدل_شوی(n); // recompute syntax hl. for this line
#د_که_پای
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_شاخص_Error, "line number %ld out of range", (اوږد)n);
    }
    أرجع str;
}

    ساكن ارزښت_3
بفر_aset(ارزښت_3 self, ارزښت_3 عدد, ارزښت_3 str)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع سېټ_بفر_کرښه(buf, (linenr_T)NUM2LONG(عدد), str);
    أرجع str;
}

    ساكن ارزښت_3
بفر_delete(ارزښت_3 self, ارزښت_3 عدد)
{
    buf_T	*buf = get_buf(self);
    اوږد	n = NUM2LONG(عدد);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_کرښه_شمېر)
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
		deleted_کرښې_mark(n, 1L);

		بدل_شوی();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_شاخص_Error, "line number %ld out of range", n);
    }
    أرجع Qnil;
}

    ساكن ارزښت_3
بفر_append(ارزښت_3 self, ارزښت_3 عدد, ارزښت_3 str)
{
    buf_T	*buf = get_buf(self);
    حرف	*کرښه = String_ارزښت_Ptr(str);
    اوږد	n = NUM2LONG(عدد);
    aco_save_T	aco;

    إذا (کرښه == عدم)
    {
	rb_raise(rb_e_شاخص_Error, "NULL line");
    }
    وإلا إذا (n >= 0 && n <= buf->b_ml.ml_کرښه_شمېر)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) کرښه, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_کرښې_mark(n, 1L);

		بدل_شوی();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_شاخص_Error, "line number %ld out of range", n);
    }
    أرجع str;
}

#که_تعریف_شوی USE_TYPEDDATA
ساكن size_t کړکۍ_dsize(ثابت فراغ *buf);

ساكن ثابت rb_معلومات_type_t کړکۍ_type = {
    "vim_window",
    {0, 0, کړکۍ_dsize,
# إذا RUBY_نسخه >= 27
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
کړکۍ_dsize(ثابت فراغ *win ناکارول_شوی)
{
    أرجع حجم(win_T);
}
#د_که_پای

    ساكن ارزښت_3
کړکۍ_new(win_T *win)
{
    إذا (win->w_ruby_ref)
    {
	أرجع (ارزښت_3) win->w_ruby_ref;
    }
    وإلا
    {
#که_تعریف_شوی USE_TYPEDDATA
	ارزښت_3 obj = Typed_معلومات_Wrap_Struct(c_Vim_کړکۍ, &کړکۍ_type, win);
#وإلا
	ارزښت_3 obj = معلومات_Wrap_Struct(c_Vim_کړکۍ, 0, 0, win);
#د_که_پای
	win->w_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن win_T *
get_win(ارزښت_3 obj)
{
    win_T *win;

#که_تعریف_شوی USE_TYPEDDATA
    Typed_معلومات_Get_Struct(obj, win_T, &کړکۍ_type, win);
#وإلا
    معلومات_Get_Struct(obj, win_T, win);
#د_که_پای
    إذا (win == عدم)
	rb_raise(e_Deleted_کړکۍ_Error, "attempt to refer to deleted window");
    أرجع win;
}

    ساكن ارزښت_3
کړکۍ_s_current(ارزښت_3 self ناکارول_شوی)
{
    أرجع کړکۍ_new(curwin);
}

    ساكن ارزښت_3
کړکۍ_s_current_getter(ID id ناکارول_شوی, ارزښت_3 *x ناکارول_شوی)
{
    أرجع کړکۍ_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ساكن ارزښت_3
کرښه_s_current(ارزښت_3 self ناکارول_شوی)
{
    أرجع get_بفر_کرښه(curbuf, curwin->w_cursor.lnum);
}

    ساكن ارزښت_3
سېټ_current_کرښه(ارزښت_3 self ناکارول_شوی, ارزښت_3 str)
{
    أرجع سېټ_بفر_کرښه(curbuf, curwin->w_cursor.lnum, str);
}

    ساكن ارزښت_3
current_کرښه_شمېره(ارزښت_3 self ناکارول_شوی)
{
    أرجع INT2FIX((صحيح)curwin->w_cursor.lnum);
}

    ساكن ارزښت_3
کړکۍ_s_شمېر(ارزښت_3 self ناکارول_شوی)
{
    win_T	*w;
    صحيح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    أرجع INT2_عدد(n);
}

    ساكن ارزښت_3
کړکۍ_s_aref(ارزښت_3 self ناکارول_شوی, ارزښت_3 عدد)
{
    win_T *w;
    صحيح n = NUM2INT(عدد);

    لكل (w = firstwin; w != عدم; w = w->w_بل, --n)
	إذا (n == 0)
	    أرجع کړکۍ_new(w);
    أرجع Qnil;
}

    ساكن ارزښت_3
کړکۍ_بفر(ارزښت_3 self)
{
    win_T *win = get_win(self);

    أرجع بفر_new(win->w_بفر);
}

    ساكن ارزښت_3
کړکۍ_لوړوالی(ارزښت_3 self)
{
    win_T *win = get_win(self);

    أرجع INT2_عدد(win->w_لوړوالی);
}

    ساكن ارزښت_3
کړکۍ_سېټ_لوړوالی(ارزښت_3 self, ارزښت_3 لوړوالی)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(لوړوالی));
    curwin = savewin;
    أرجع لوړوالی;
}

    ساكن ارزښت_3
کړکۍ_پلنوالی(ارزښت_3 self ناکارول_شوی)
{
    أرجع INT2_عدد(get_win(self)->w_پلنوالی);
}

    ساكن ارزښت_3
کړکۍ_سېټ_پلنوالی(ارزښت_3 self ناکارول_شوی, ارزښت_3 پلنوالی)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(پلنوالی));
    curwin = savewin;
    أرجع پلنوالی;
}

    ساكن ارزښت_3
کړکۍ_cursor(ارزښت_3 self)
{
    win_T *win = get_win(self);

    أرجع rb_assoc_new(INT2_عدد(win->w_cursor.lnum), INT2_عدد(win->w_cursor.ستون));
}

    ساكن ارزښت_3
کړکۍ_سېټ_cursor(ارزښت_3 self, ارزښت_3 pos)
{
    ارزښت_3 lnum, ستون;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    إذا (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    ستون = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.ستون = NUM2UINT(ستون);
    win->w_سېټ_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن ارزښت_3
f_nop(ارزښت_3 self ناکارول_شوی)
{
    أرجع Qnil;
}

    ساكن ارزښت_3
f_p(صحيح argc, ارزښت_3 *argv, ارزښت_3 self ناکارول_شوی)
{
    صحيح i;
    ارزښت_3 str = rb_str_new("", 0);
    ارزښت_3 ret = Qnil;

    لكل (i = 0; i < argc; i++)
    {
	إذا (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    پیغام(RSTRING_PTR(str));

    إذا (argc == 1)
	ret = argv[0];
    وإلا إذا (argc > 1)
	ret = rb_ary_new4(argc, argv);
    أرجع ret;
}

    ساكن فراغ
ruby_io_init(فراغ)
{
#که_نه_تعریف_شوی DYNAMIC_RUBY
    RUBYEXTERN ارزښت_3 rb_stdout;
    RUBYEXTERN ارزښت_3 rb_stderr;
#د_که_پای

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_لاره(rb_stdout, "write", (فراغ*)vim_پیغام, 1);
    rb_define_singleton_لاره(rb_stdout, "flush", (فراغ*)f_nop, 0);
    rb_define_singleton_لاره(rb_stderr, "write", (فراغ*)vim_پیغام, 1);
    rb_define_singleton_لاره(rb_stderr, "flush", (فراغ*)f_nop, 0);
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
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_عدد(VIM_نسخه_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_عدد(VIM_نسخه_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_عدد(VIM_نسخه_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_عدد(VIM_نسخه_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_نسخه_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_نسخه_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_نسخه_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_نسخه_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_پیغام, 1);
    rb_define_module_function(mVIM, "set_option", vim_سېټ_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_بفر_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_کړکۍ_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_بفر = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_لاره(c_بفر, "current", (فراغ*)بفر_s_current, 0);
    rb_define_singleton_لاره(c_بفر, "count", (فراغ*)بفر_s_شمېر, 0);
    rb_define_singleton_لاره(c_بفر, "[]", (فراغ*)بفر_s_aref, 1);
    rb_define_لاره(c_بفر, "name", (فراغ*)بفر_name, 0);
    rb_define_لاره(c_بفر, "number", (فراغ*)بفر_شمېره, 0);
    rb_define_لاره(c_بفر, "count", (فراغ*)بفر_شمېر, 0);
    rb_define_لاره(c_بفر, "length", (فراغ*)بفر_شمېر, 0);
    rb_define_لاره(c_بفر, "[]", (فراغ*)بفر_aref, 1);
    rb_define_لاره(c_بفر, "[]=", (فراغ*)بفر_aset, 2);
    rb_define_لاره(c_بفر, "delete", (فراغ*)بفر_delete, 1);
    rb_define_لاره(c_بفر, "append", (فراغ*)بفر_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_لاره(c_بفر, "line_number", (فراغ*)current_کرښه_شمېره, 0);
    rb_define_لاره(c_بفر, "line", (فراغ*)کرښه_s_current, 0);
    rb_define_لاره(c_بفر, "line=", (فراغ*)سېټ_current_کرښه, 1);


    c_Vim_کړکۍ = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_لاره(c_Vim_کړکۍ, "current", (فراغ*)کړکۍ_s_current, 0);
    rb_define_singleton_لاره(c_Vim_کړکۍ, "count", (فراغ*)کړکۍ_s_شمېر, 0);
    rb_define_singleton_لاره(c_Vim_کړکۍ, "[]", (فراغ*)کړکۍ_s_aref, 1);
    rb_define_لاره(c_Vim_کړکۍ, "buffer", (فراغ*)کړکۍ_بفر, 0);
    rb_define_لاره(c_Vim_کړکۍ, "height", (فراغ*)کړکۍ_لوړوالی, 0);
    rb_define_لاره(c_Vim_کړکۍ, "height=", (فراغ*)کړکۍ_سېټ_لوړوالی, 1);
    rb_define_لاره(c_Vim_کړکۍ, "width", (فراغ*)کړکۍ_پلنوالی, 0);
    rb_define_لاره(c_Vim_کړکۍ, "width=", (فراغ*)کړکۍ_سېټ_پلنوالی, 1);
    rb_define_لاره(c_Vim_کړکۍ, "cursor", (فراغ*)کړکۍ_cursor, 0);
    rb_define_لاره(c_Vim_کړکۍ, "cursor=", (فراغ*)کړکۍ_سېټ_cursor, 1);

    rb_define_virtual_variable("$curbuf", بفر_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", کړکۍ_s_current_getter, 0);
}

    فراغ
vim_ruby_init(فراغ *ډېری_پیل)
{
    // should get machine stack start address early in main function
    ruby_ډېری_پیل = ډېری_پیل;
}

    ساكن صحيح
convert_hash2dict(ارزښت_3 کلی, ارزښت_3 val, ارزښت_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(کلی)));
    إذا (di == عدم || ruby_convert_منزل_vim_ارزښت(val, &di->di_tv) != OK
						     || dict_جمع_کړه(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	أرجع ST_STOP;
    }
    أرجع ST_CONTINUE;
}

    ساكن صحيح
ruby_convert_منزل_vim_ارزښت(ارزښت_3 val, typval_T *rettv)
{
    بدل (TYPE(val))
    {
	حالة T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_شمېره = VVAL_NULL;
	    اكسر;
	حالة T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_شمېره = VVAL_TRUE;
	    اكسر;
	حالة T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_شمېره = VVAL_FALSE;
	    اكسر;
	حالة T_BIGNUM:
	حالة T_FIXNUM:
	    rettv->v_type = VAR_شمېره;
	    rettv->vval.v_شمېره = (varnumber_T)NUM2LONG(val);
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
		ارزښت_3 str = (ارزښت_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    اكسر;
	حالة T_ARRAY:
	    {
		list_T *l;
		اوږد i;
		typval_T v;

		l = list_alloc();
		إذا (l == عدم)
		    أرجع FAIL;

		لكل (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    إذا (ruby_convert_منزل_vim_ارزښت((ارزښت_3)RARRAY_PTR(val)[i],
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

		rb_hash_foreach(val, convert_hash2dict, (ارزښت_3)d);
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
	ارزښت_3 obj;

	obj = rb_eval_string_protect((ثابت حرف *)str, &state);
	إذا (state)
	    error_print(state);
	وإلا
	    retval = ruby_convert_منزل_vim_ارزښت(obj, rettv);
    }
    إذا (retval == FAIL)
    {
	rettv->v_type = VAR_شمېره;
	rettv->vval.v_شمېره = 0;
    }
}
