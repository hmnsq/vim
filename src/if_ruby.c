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
#جيڪڏهن_وصف_ٿيل HAVE_CONFIG_H
# include "auto/config.h"
#جيڪڏهن_پورو

#ضمن <stdio.h>
#ضمن <string.h>

#جيڪڏهن_وصف_ٿيل _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل RUBYEXTERN
# define RUBYEXTERN خارجي
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# إذا RUBY_نسخو >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# إذا defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# إذا RUBY_نسخو >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_نشان		(*dll_rb_c_نشان)
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

# إذا RUBY_نسخو >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# إذا RUBY_نسخو >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# إذا RUBY_نسخو == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# إذا RUBY_نسخو >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# إذا RUBY_نسخو >= 26 && RUBY_نسخو <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# إذا RUBY_نسخو >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_سائيز_حد_کان_مٿي	ruby_malloc_سائيز_حد_کان_مٿي_stub
# endif

# إذا RUBY_نسخو >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#جيڪڏهن_پورو  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#جيڪڏهن_وصف_ٿيل SIZEOF_وقت_T
# undef SIZEOF_وقت_T
#جيڪڏهن_پورو

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ڌڪو
# pragma GCC diagnostic ignored "-Wunused-parameter"
#جيڪڏهن_پورو

#إذا defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#جيڪڏهن_پورو

#ضمن <ruby.h>
#ضمن <ruby/encoding.h>

#إذا defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ڪڍو
#جيڪڏهن_پورو

// See above.
#جيڪڏهن_وصف_ٿيل SIZEOF_وقت_T
# undef SIZEOF_وقت_T
#جيڪڏهن_پورو

#وصف_رد off_t	// ruby defines off_t as _int64, Mingw uses long
#وصف_رد EXTERN
#وصف_رد _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#إذا defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#جيڪڏهن_پورو

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#إذا defined(Typed_معلومات_Wrap_Struct) && (RUBY_نسخو >= 20)
# define USE_TYPEDDATA	1
#جيڪڏهن_پورو

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#جيڪڏهن_وصف_نه_ٿيل String_قدر_Ptr
# define String_قدر_Ptr(s) STR2CSTR(s)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_DUP
# undef HAVE_DUP
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_FSYNC
# undef HAVE_FSYNC
#جيڪڏهن_پورو

// Avoid redefining TRUE/FALSE in vterm.h.
#جيڪڏهن_وصف_ٿيل TRUE
# undef TRUE
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FALSE
# undef FALSE
#جيڪڏهن_پورو

#ضمن "vim.h"
#ضمن "version.h"

#جيڪڏهن_وصف_ٿيل DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define ڀريو_dll vim_ڀريو_Lib
#  define نشان_from_dll Get_Proc_پتو
#  define close_dll FreeLibrary
#  define ڀريو_dll_error GetWin32Error
# وإلا
#  include <dlfcn.h>
#  define HINSTANCE فراغ*
#  define RUBY_PROC فراغ*
#  define ڀريو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define نشان_from_dll dlsym
#  define close_dll dlclose
#  define ڀريو_dll_error dlerror
# endif
#جيڪڏهن_پورو

ساكن صحيح ruby_initialized = 0;
ساكن فراغ *ruby_ڍير_شروعات;
ساكن قدر_3 objtbl;

ساكن قدر_3 mVIM;
ساكن قدر_3 c_بفر;
ساكن قدر_3 c_Vim_دري;
ساكن قدر_3 e_Deleted_بفر_Error;
ساكن قدر_3 e_Deleted_دري_Error;

ساكن صحيح ensure_ruby_initialized(فراغ);
ساكن فراغ error_print(صحيح);
ساكن فراغ ruby_io_init(فراغ);
ساكن فراغ ruby_vim_init(فراغ);
ساكن صحيح ruby_convert_منزل_vim_قدر(قدر_3 val, typval_T *rettv);

#إذا defined(__ia64) && !defined(ruby_init_ڍير)
# define ruby_init_ڍير(addr) ruby_init_ڍير((addr), rb_ia64_bsp())
#جيڪڏهن_پورو

#إذا defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# إذا RUBY_نسخو >= 27
#  undef rb_define_global_function
#  undef rb_define_طريقو
#  undef rb_define_module_function
#  undef rb_define_singleton_طريقو
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# إذا RUBY_نسخو < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_رستو			dll_rb_class_رستو
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخو >= 23
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
# define rb_define_طريقو		dll_rb_define_طريقو
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_طريقو	dll_rb_define_singleton_طريقو
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_اشاريو_Error			(*dll_rb_e_اشاريو_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# إذا RUBY_نسخو >= 21
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
#  إذا RUBY_نسخو < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_سيٽ			dll_rb_lastline_سيٽ
# define rb_protect			dll_rb_protect
# define rb_ڀريو			dll_rb_ڀريو
# إذا RUBY_نسخو < 20
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
# define rb_string_قدر		dll_rb_string_قدر
# define rb_string_قدر_ptr		dll_rb_string_قدر_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ڌڪو			dll_rb_ary_ڌڪو
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ڍير
#  define ruby_init_ڍير(addr)	dll_ruby_init_ڍير((addr), rb_ia64_bsp())
# وإلا
#  define ruby_init_ڍير		dll_ruby_init_ڍير
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_ڳوليو_اشاريو		dll_rb_enc_ڳوليو_اشاريو
# define rb_enc_ڳوليو			dll_rb_enc_ڳوليو
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
ساكن قدر_3 (*dll_rb_assoc_new) (قدر_3, قدر_3);
قدر_3 *dll_rb_cFalseClass;
قدر_3 *dll_rb_cFixnum;
# إذا defined(USE_RUBY_INTEGER)
قدر_3 *dll_rb_cInteger;
# endif
# إذا RUBY_نسخو >= 20
قدر_3 *dll_rb_cFloat;
# endif
قدر_3 *dll_rb_cNilClass;
ساكن قدر_3 *dll_rb_cObject;
قدر_3 *dll_rb_cString;
قدر_3 *dll_rb_c_نشان;
قدر_3 *dll_rb_cTrueClass;
ساكن قدر_3 (*dll_rb_class_new_instance) (صحيح,قدر_3*,قدر_3);
ساكن فراغ (*dll_rb_check_type) (قدر_3,صحيح);
# ifdef USE_TYPEDDATA
ساكن فراغ *(*dll_rb_check_typeddata) (قدر_3,ثابت rb_معلومات_type_t *);
# endif
ساكن قدر_3 (*dll_rb_class_رستو) (قدر_3);
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخو >= 23
ساكن قدر_3 (*dll_rb_معلومات_typed_object_wrap) (قدر_3, فراغ*, ثابت rb_معلومات_type_t *);
#  وإلا
ساكن قدر_3 (*dll_rb_معلومات_typed_object_alloc) (قدر_3, فراغ*, ثابت rb_معلومات_type_t *);
#  endif
# وإلا
ساكن قدر_3 (*dll_rb_معلومات_object_alloc) (قدر_3, فراغ*, RUBY_معلومات_FUNC, RUBY_معلومات_FUNC);
# endif
# إذا RUBY_نسخو >= 31
ساكن فراغ (*dll_rb_debug_rstring_null_ptr) (ثابت حرف*);
# endif
ساكن قدر_3 (*dll_rb_define_class_under) (قدر_3, ثابت حرف*, قدر_3);
ساكن فراغ (*dll_rb_define_const) (قدر_3,ثابت حرف*,قدر_3);
ساكن فراغ (*dll_rb_define_global_function) (ثابت حرف*,قدر_3(*)(صحيح,قدر_3*,قدر_3),صحيح);
ساكن فراغ (*dll_rb_define_طريقو) (قدر_3,ثابت حرف*,قدر_3(*)(ANYARGS),صحيح);
ساكن قدر_3 (*dll_rb_define_module) (ثابت حرف*);
ساكن فراغ (*dll_rb_define_module_function) (قدر_3,ثابت حرف*,قدر_3(*)(قدر_3,قدر_3),صحيح);
ساكن فراغ (*dll_rb_define_singleton_طريقو) (قدر_3,ثابت حرف*,قدر_3(*)(ANYARGS),صحيح);
ساكن فراغ (*dll_rb_define_virtual_variable) (ثابت حرف*,قدر_3(*)(ID,قدر_3*),فراغ(*)());
ساكن قدر_3 *dll_rb_stdout;
ساكن قدر_3 *dll_rb_stderr;
ساكن قدر_3 *dll_rb_eArgError;
ساكن قدر_3 *dll_rb_e_اشاريو_Error;
ساكن قدر_3 *dll_rb_eRuntimeError;
ساكن قدر_3 *dll_rb_eStandardError;
ساكن قدر_3 (*dll_rb_eval_string_protect) (ثابت حرف*, صحيح*);
# إذا RUBY_نسخو >= 21
ساكن قدر_3 (*dll_rb_funcallv) (قدر_3, ID, صحيح, ثابت قدر_3*);
# وإلا
ساكن قدر_3 (*dll_rb_funcall2) (قدر_3, ID, صحيح, ثابت قدر_3*);
# endif
ساكن فراغ (*dll_rb_global_variable) (قدر_3*);
ساكن قدر_3 (*dll_rb_hash_aset) (قدر_3, قدر_3, قدر_3);
ساكن قدر_3 (*dll_rb_hash_foreach) (قدر_3, صحيح (*)(قدر_3, قدر_3, قدر_3), قدر_3);
ساكن قدر_3 (*dll_rb_hash_new) (فراغ);
ساكن قدر_3 (*dll_rb_inspect) (قدر_3);
ساكن قدر_3 (*dll_rb_int2inum) (ڊگهو);
ساكن ID (*dll_rb_intern) (ثابت حرف*);
# إذا RUBY_نسخو >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ساكن ڊگهو (*dll_rb_fix2int) (قدر_3);
ساكن ڊگهو (*dll_rb_num2int) (قدر_3);
ساكن بنانشان ڊگهو (*dll_rb_num2uint) (قدر_3);
# endif
ساكن مزدوج (*dll_rb_num2dbl) (قدر_3);
ساكن قدر_3 (*dll_rb_lastline_get) (فراغ);
ساكن فراغ (*dll_rb_lastline_سيٽ) (قدر_3);
ساكن قدر_3 (*dll_rb_protect) (قدر_3 (*)(قدر_3), قدر_3, صحيح*);
ساكن فراغ (*dll_rb_ڀريو) (قدر_3, صحيح);
ساكن ڊگهو (*dll_rb_num2long) (قدر_3);
ساكن بنانشان ڊگهو (*dll_rb_num2ulong) (قدر_3);
ساكن قدر_3 (*dll_rb_obj_alloc) (قدر_3);
ساكن قدر_3 (*dll_rb_obj_as_string) (قدر_3);
ساكن قدر_3 (*dll_rb_obj_id) (قدر_3);
ساكن فراغ (*dll_rb_raise) (قدر_3, ثابت حرف*, ...);
ساكن قدر_3 (*dll_rb_string_قدر) (متطاير قدر_3*);
ساكن قدر_3 (*dll_rb_str_cat) (قدر_3, ثابت حرف*, ڊگهو);
ساكن قدر_3 (*dll_rb_str_concat) (قدر_3, قدر_3);
ساكن قدر_3 (*dll_rb_str_new) (ثابت حرف*, ڊگهو);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ساكن قدر_3 (*dll_rb_str_new_cstr) (ثابت حرف*);
# وإلا
ساكن قدر_3 (*dll_rb_str_new2) (ثابت حرف*);
# endif
ساكن قدر_3 (*dll_rb_errinfo) (فراغ);
ساكن فراغ (*dll_ruby_init) (فراغ);
ساكن فراغ (*dll_ruby_init_loadpath) (فراغ);
# ifdef MSWIN
ساكن فراغ (*dll_ruby_sysinit) (صحيح*, حرف***);
ساكن صحيح (*dll_rb_w32_snprintf)(حرف*, size_t, ثابت حرف*, ...);
# endif
# إذا RUBY_نسخو >= 31
#  ifdef _MSC_VER
ساكن فراغ (*dll_rb_unexpected_type) (قدر_3, صحيح);
#  وإلا
NORETURN(ساكن فراغ (*dll_rb_unexpected_type) (قدر_3, صحيح));
#  endif
# endif
ساكن حرف * (*dll_rb_string_قدر_ptr) (متطاير قدر_3*);
ساكن قدر_3 (*dll_rb_float_new) (مزدوج);
ساكن قدر_3 (*dll_rb_ary_new) (فراغ);
ساكن قدر_3 (*dll_rb_ary_new4) (ڊگهو n, ثابت قدر_3 *elts);
ساكن قدر_3 (*dll_rb_ary_ڌڪو) (قدر_3, قدر_3);
# إذا RUBY_نسخو >= 26 && RUBY_نسخو <= 32
ساكن فراغ (*dll_rb_ary_detransient) (قدر_3);
# endif
# ifdef __ia64
ساكن فراغ * (*dll_rb_ia64_bsp) (فراغ);
ساكن فراغ (*dll_ruby_init_ڍير)(قدر_3*, فراغ*);
# وإلا
ساكن فراغ (*dll_ruby_init_ڍير)(قدر_3*);
# endif
ساكن قدر_3 (*dll_rb_int2big)(SIGNED_قدر);

ساكن فراغ (*dll_ruby_script) (ثابت حرف*);
ساكن صحيح (*dll_rb_enc_ڳوليو_اشاريو) (ثابت حرف*);
ساكن rb_encoding* (*dll_rb_enc_ڳوليو) (ثابت حرف*);
ساكن قدر_3 (*dll_rb_enc_str_new) (ثابت حرف*, ڊگهو, rb_encoding*);
ساكن قدر_3 (*dll_rb_sprintf) (ثابت حرف*, ...);
ساكن قدر_3 (*dll_rb_require) (ثابت حرف*);
ساكن فراغ* (*dll_ruby_options)(صحيح, حرف**);

# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_نسخو == 21
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect_promoted)(قدر_3);
#  وإلا
ساكن فراغ (*dll_rb_gc_writebarrier_unprotect)(قدر_3 obj);
#  endif
# endif

# إذا RUBY_نسخو >= 30
#  ifdef _MSC_VER
ساكن فراغ (*dll_ruby_malloc_سائيز_حد_کان_مٿي)(size_t, size_t);
#  وإلا
NORETURN(ساكن فراغ (*dll_ruby_malloc_سائيز_حد_کان_مٿي)(size_t, size_t));
#  endif
# endif

# إذا RUBY_نسخو >= 26 && RUBY_نسخو <= 32
فراغ rb_ary_detransient_stub(قدر_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  إذا RUBY_نسخو >= 22
    ڊگهو
rb_num2long_stub(قدر_3 x)
#  وإلا
    SIGNED_قدر
rb_num2long_stub(قدر_3 x)
#  endif
{
    أرجع dll_rb_num2long(x);
}
#  إذا RUBY_نسخو >= 26
    قدر_3
rb_int2big_stub(intptr_t x)
#  وإلا
    قدر_3
rb_int2big_stub(SIGNED_قدر x)
#  endif
{
    أرجع dll_rb_int2big(x);
}
#  إذا RUBY_نسخو >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    ڊگهو
rb_fix2int_stub(قدر_3 x)
{
    أرجع dll_rb_fix2int(x);
}
    ڊگهو
rb_num2int_stub(قدر_3 x)
{
    أرجع dll_rb_num2int(x);
}
#  endif
#  إذا RUBY_نسخو >= 20
    قدر_3
rb_float_new_اندريون_ڍير(مزدوج d)
{
    أرجع dll_rb_float_new(d);
}
#   إذا RUBY_نسخو >= 22
    بنانشان ڊگهو
rb_num2ulong(قدر_3 x)
#   وإلا
    قدر_3
rb_num2ulong(قدر_3 x)
#   endif
{
    أرجع (ڊگهو)RSHIFT((SIGNED_قدر)(x),1);
}
#  endif
#  إذا defined(USE_RGENGC) && USE_RGENGC
#   إذا RUBY_نسخو == 21
    فراغ
rb_gc_writebarrier_unprotect_promoted_stub(قدر_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   وإلا
    فراغ
rb_gc_writebarrier_unprotect_stub(قدر_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  إذا RUBY_نسخو >= 26 && RUBY_نسخو <= 32
    فراغ
rb_ary_detransient_stub(قدر_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  إذا RUBY_نسخو >= 30
    فراغ
rb_check_type_stub(قدر_3 obj, صحيح t)
{
    dll_rb_check_type(obj, t);
}
    بنانشان ڊگهو
rb_num2uint_stub(قدر_3 x)
{
    أرجع dll_rb_num2uint(x);
}
    فراغ
ruby_malloc_سائيز_حد_کان_مٿي_stub(size_t x, size_t y)
{
    dll_ruby_malloc_سائيز_حد_کان_مٿي(x, y);
}
#  endif
#  إذا RUBY_نسخو >= 31
    فراغ
rb_debug_rstring_null_ptr_stub(ثابت حرف *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    فراغ
rb_unexpected_type_stub(قدر_3 self, صحيح t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
فراغ *rb_check_typeddata_stub(قدر_3 obj, ثابت rb_معلومات_type_t *معلومات_type)
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
# إذا RUBY_نسخو >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_نشان},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_رستو},
# ifdef USE_TYPEDDATA
#  إذا RUBY_نسخو >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_معلومات_typed_object_wrap},
#  وإلا
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_معلومات_typed_object_alloc},
#  endif
# وإلا
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_معلومات_object_alloc},
# endif
# إذا RUBY_نسخو >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_طريقو},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_طريقو},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_اشاريو_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# إذا RUBY_نسخو >= 21
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
# إذا RUBY_نسخو >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_سيٽ},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_ڀريو},
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
# إذا RUBY_نسخو >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_قدر_ptr},
# إذا RUBY_نسخو >= 20
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
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ڌڪو},
# إذا RUBY_نسخو >= 26 && RUBY_نسخو <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_ڳوليو_اشاريو},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_ڳوليو},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ڍير},
# إذا defined(USE_RGENGC) && USE_RGENGC
#  إذا RUBY_نسخو == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  وإلا
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# إذا RUBY_نسخو >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_سائيز_حد_کان_مٿي},
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
    hinstRuby = ڀريو_dll(libname);
    إذا (!hinstRuby)
    {
	إذا (verbose)
	    semsg(_(e_could_not_ڀريو_library_str_str), libname, ڀريو_dll_error());
	أرجع FAIL;
    }

    لكل (i = 0; ruby_funcname_جدول[i].ptr; ++i)
    {
	إذا (!(*ruby_funcname_جدول[i].ptr = نشان_from_dll(hinstRuby,
			ruby_funcname_جدول[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = عدم;
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), ruby_funcname_جدول[i].name);
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
#جيڪڏهن_پورو // defined(DYNAMIC_RUBY)

    فراغ
ruby_پڇاڙي(فراغ)
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
    ساكن قدر_3
vim_str2rb_enc_str(ثابت حرف *s)
{
    ڊگهو lval;
    char_u *sval;
    rb_encoding *enc;

    إذا (get_option_قدر((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_ڳوليو((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	    أرجع rb_enc_str_new(s, (ڊگهو)لڙي_ڊيگهه(s), enc);
    }
    أرجع rb_str_new2(s);
}

    ساكن قدر_3
eval_enc_string_protect(ثابت حرف *str, صحيح *state)
{
    ڊگهو lval;
    char_u *sval;
    rb_encoding *enc;
    قدر_3 v;

    إذا (get_option_قدر((char_u *)"enc", &lval, &sval, عدم, 0) == gov_string)
    {
	enc = rb_enc_ڳوليو((حرف *)sval);
	vim_free(sval);
	إذا (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    أرجع rb_eval_string_protect(String_قدر_Ptr(v), state);
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
	قدر_3 سٽ;

	إذا (i > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    اكسر;
	سٽ = vim_str2rb_enc_str((حرف *)ml_get(i));
	rb_lastline_سيٽ(سٽ);
	eval_enc_string_protect((حرف *) eap->arg, &state);
	إذا (state)
	{
	    error_print(state);
	    اكسر;
	}
	إذا (was_curbuf != curbuf || i > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    اكسر;
	سٽ = rb_lastline_get();
	إذا (!NIL_P(سٽ))
	{
	    إذا (TYPE(سٽ) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_جو_string));
		أرجع;
	    }
	    ml_replace(i, (char_u *) String_قدر_Ptr(سٽ), 1);
	    بدليو();
#جيڪڏهن_وصف_ٿيل SYNTAX_HL
	    syn_بدليو(i); // recompute syntax hl. for this line
#جيڪڏهن_پورو
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ساكن قدر_3
rb_ڀريو_wrap(قدر_3 file_منزل_ڀريو)
{
    rb_ڀريو(file_منزل_ڀريو, 0);
    أرجع Qnil;
}

    فراغ
ex_rubyfile(exarg_T *eap)
{
    صحيح state;

    إذا (!ensure_ruby_initialized())
	أرجع;

    قدر_3 file_منزل_ڀريو = rb_str_new2((ثابت حرف *)eap->arg);
    rb_protect(rb_ڀريو_wrap, file_منزل_ڀريو, &state);
    إذا (state)
	error_print(state);
}

    فراغ
ruby_بفر_free(buf_T *buf)
{
    إذا (buf->b_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((قدر_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->معلومات = عدم;
}

    فراغ
ruby_دري_free(win_T *win)
{
    إذا (win->w_ruby_ref == عدم)
	أرجع;

    rb_hash_aset(objtbl, rb_obj_id((قدر_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->معلومات = عدم;
}

    ساكن صحيح
ensure_ruby_initialized(فراغ)
{
    إذا (ruby_initialized)
	أرجع ruby_initialized;

#جيڪڏهن_وصف_ٿيل DYNAMIC_RUBY
    إذا (ruby_enabled(TRUE))
#جيڪڏهن_پورو
    {
#جيڪڏهن_وصف_ٿيل MSWIN
	// suggested by Ariya Mizutani
	صحيح argc = 1;
	حرف *argv[] = {"gvim.exe"};
	حرف **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#جيڪڏهن_پورو
	{
	    ruby_init_ڍير(ruby_ڍير_شروعات);
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
#جيڪڏهن_وصف_ٿيل DYNAMIC_RUBY
    وإلا
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	أرجع 0;
    }
#جيڪڏهن_پورو

    أرجع ruby_initialized;
}

    ساكن فراغ
error_print(صحيح state)
{
    قدر_3 error;
    قدر_3 eclass;
    قدر_3 einfo;
    قدر_3 bt;
    صحيح attr;
    حرف buff[BUFSIZ];
    ڊگهو i;

#وصف_ڪريو ٽيگ_RETURN	0x1
#وصف_ڪريو ٽيگ_BREAK	0x2
#وصف_ڪريو ٽيگ_ايندڙ	0x3
#وصف_ڪريو ٽيگ_RETRY	0x4
#وصف_ڪريو ٽيگ_REDO	0x5
#وصف_ڪريو ٽيگ_RAISE	0x6
#وصف_ڪريو ٽيگ_THROW	0x7
#وصف_ڪريو ٽيگ_FATAL	0x8
#وصف_ڪريو ٽيگ_نقاب	0xf

    بدل (state)
    {
	حالة ٽيگ_RETURN:
	    emsg(_(e_unexpected_return));
	    اكسر;
	حالة ٽيگ_ايندڙ:
	    emsg(_(e_unexpected_ايندڙ));
	    اكسر;
	حالة ٽيگ_BREAK:
	    emsg(_(e_unexpected_break));
	    اكسر;
	حالة ٽيگ_REDO:
	    emsg(_(e_unexpected_redo));
	    اكسر;
	حالة ٽيگ_RETRY:
	    emsg(_(e_retry_outside_جو_rescue_clause));
	    اكسر;
	حالة ٽيگ_RAISE:
	حالة ٽيگ_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_جو(error);
	    einfo = rb_obj_as_string(error);
	    إذا (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    وإلا
	    {
		قدر_3 epath;
		حرف *p;

		epath = rb_class_رستو(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		إذا (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#إذا RUBY_نسخو >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		نياپو_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#وإلا
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    لكل (i = 0; i < RARRAY_LEN(bt); i++)
		نياپو_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#جيڪڏهن_پورو
	    اكسر;
	افتراضي:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_حالت_nr), state);
	    emsg(buff);
	    اكسر;
    }
}

    ساكن قدر_3
vim_پيغام(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
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
	نياپو(buff);
    }
    وإلا
    {
	نياپو("");
    }
    أرجع Qnil;
}

    ساكن قدر_3
vim_سيٽ_option(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
{
    do_سيٽ((char_u *)String_قدر_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن قدر_3
vim_command(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
{
    do_cmdline_cmd((char_u *)String_قدر_Ptr(str));
    أرجع Qnil;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    ساكن قدر_3
vim_منزل_ruby(typval_T *tv)
{
    قدر_3 نتيجو = Qnil;

    إذا (tv->v_type == VAR_STRING)
    {
	نتيجو = rb_str_new2(tv->vval.v_string == عدم
					  ? "" : (حرف *)(tv->vval.v_string));
    }
    وإلا إذا (tv->v_type == VAR_عدد)
    {
	نتيجو = INT2_انگ(tv->vval.v_عدد);
    }
    وإلا إذا (tv->v_type == VAR_FLOAT)
    {
	نتيجو = rb_float_new(tv->vval.v_float);
    }
    وإلا إذا (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	نتيجو = rb_ary_new();

	إذا (list != عدم)
	{
	    FOR_ALL_LIST_شيون(list, curr)
		rb_ary_ڌڪو(نتيجو, vim_منزل_ruby(&curr->li_tv));
	}
    }
    وإلا إذا (tv->v_type == VAR_DICT)
    {
	نتيجو = rb_hash_new();

	إذا (tv->vval.v_dict != عدم)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_استعمال_ٿيل;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_شيون(ht, hi, todo)
	    {
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(نتيجو, rb_str_new2((حرف *)hi->hi_ڪنجي),
						     vim_منزل_ruby(&di->di_tv));
		}
	    }
	}
    }
    وإلا إذا (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	إذا (tv->vval.v_عدد == VVAL_TRUE)
	    نتيجو = Qtrue;
	وإلا إذا (tv->vval.v_عدد == VVAL_FALSE)
	    نتيجو = Qfalse;
    }
    وإلا إذا (tv->v_type == VAR_BLOB)
    {
	نتيجو = rb_str_new(tv->vval.v_blob->bv_ga.ga_معلومات,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    أرجع نتيجو;
}
#جيڪڏهن_پورو

    ساكن قدر_3
vim_evaluate(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    typval_T    *tv;
    قدر_3       نتيجو;

    tv = eval_اظهار((char_u *)String_قدر_Ptr(str), عدم);
    إذا (tv == عدم)
	أرجع Qnil;
    نتيجو = vim_منزل_ruby(tv);

    free_tv(tv);

    أرجع نتيجو;
#وإلا
    أرجع Qnil;
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
ساكن size_t بفر_dsize(ثابت فراغ *buf);

ساكن ثابت rb_معلومات_type_t بفر_type = {
    "vim_buffer",
    {0, 0, بفر_dsize,
# إذا RUBY_نسخو >= 27
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
بفر_dsize(ثابت فراغ *buf اڻ_استعمال_ٿيل)
{
    أرجع حجم(buf_T);
}
#جيڪڏهن_پورو

    ساكن قدر_3
بفر_new(buf_T *buf)
{
    إذا (buf->b_ruby_ref)
    {
	أرجع (قدر_3) buf->b_ruby_ref;
    }
    وإلا
    {
#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
	قدر_3 obj = Typed_معلومات_Wrap_Struct(c_بفر, &بفر_type, buf);
#وإلا
	قدر_3 obj = معلومات_Wrap_Struct(c_بفر, 0, 0, buf);
#جيڪڏهن_پورو
	buf->b_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن buf_T *
get_buf(قدر_3 obj)
{
    buf_T *buf;

#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
    Typed_معلومات_Get_Struct(obj, buf_T, &بفر_type, buf);
#وإلا
    معلومات_Get_Struct(obj, buf_T, buf);
#جيڪڏهن_پورو
    إذا (buf == عدم)
	rb_raise(e_Deleted_بفر_Error, "attempt to refer to deleted buffer");
    أرجع buf;
}

    ساكن قدر_3
vim_blob(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
{
    قدر_3 نتيجو = rb_str_new("0z", 2);
    حرف    buf[4];
    صحيح	i;
    لكل (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (بنانشان حرف)(RSTRING_PTR(str)[i]));
	rb_str_concat(نتيجو, rb_str_new2(buf));
    }
    أرجع نتيجو;
}

    ساكن قدر_3
بفر_s_current(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع بفر_new(curbuf);
}

    ساكن قدر_3
بفر_s_current_getter(ID id اڻ_استعمال_ٿيل, قدر_3 *x اڻ_استعمال_ٿيل)
{
    أرجع بفر_new(curbuf);
}

    ساكن قدر_3
بفر_s_ڳڻپ(قدر_3 self اڻ_استعمال_ٿيل)
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

    أرجع INT2_انگ(n);
}

    ساكن قدر_3
بفر_s_aref(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 انگ)
{
    buf_T *b;
    صحيح n = NUM2INT(انگ);

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

    ساكن قدر_3
بفر_name(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع buf->b_ffname ? rb_str_new2((حرف *)buf->b_ffname) : Qnil;
}

    ساكن قدر_3
بفر_عدد(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_انگ(buf->b_fnum);
}

    ساكن قدر_3
بفر_ڳڻپ(قدر_3 self)
{
    buf_T *buf = get_buf(self);

    أرجع INT2_انگ(buf->b_ml.ml_سٽ_ڳڻپ);
}

    ساكن قدر_3
get_بفر_سٽ(buf_T *buf, linenr_T n)
{
    إذا (n <= 0 || n > buf->b_ml.ml_سٽ_ڳڻپ)
	rb_raise(rb_e_اشاريو_Error, "line number %ld out of range", (ڊگهو)n);
    أرجع vim_str2rb_enc_str((حرف *)ml_get_buf(buf, n, FALSE));
}

    ساكن قدر_3
بفر_aref(قدر_3 self, قدر_3 انگ)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع get_بفر_سٽ(buf, (linenr_T)NUM2LONG(انگ));
    أرجع Qnil; // For stop warning
}

    ساكن قدر_3
سيٽ_بفر_سٽ(buf_T *buf, linenr_T n, قدر_3 str)
{
    حرف	*سٽ = String_قدر_Ptr(str);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_سٽ_ڳڻپ && سٽ != عدم)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)سٽ, TRUE);
		بدليو();
#جيڪڏهن_وصف_ٿيل SYNTAX_HL
		syn_بدليو(n); // recompute syntax hl. for this line
#جيڪڏهن_پورو
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_اشاريو_Error, "line number %ld out of range", (ڊگهو)n);
    }
    أرجع str;
}

    ساكن قدر_3
بفر_aset(قدر_3 self, قدر_3 انگ, قدر_3 str)
{
    buf_T *buf = get_buf(self);

    إذا (buf != عدم)
	أرجع سيٽ_بفر_سٽ(buf, (linenr_T)NUM2LONG(انگ), str);
    أرجع str;
}

    ساكن قدر_3
بفر_delete(قدر_3 self, قدر_3 انگ)
{
    buf_T	*buf = get_buf(self);
    ڊگهو	n = NUM2LONG(انگ);
    aco_save_T	aco;

    إذا (n > 0 && n <= buf->b_ml.ml_سٽ_ڳڻپ)
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
		deleted_سٽون_mark(n, 1L);

		بدليو();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_اشاريو_Error, "line number %ld out of range", n);
    }
    أرجع Qnil;
}

    ساكن قدر_3
بفر_append(قدر_3 self, قدر_3 انگ, قدر_3 str)
{
    buf_T	*buf = get_buf(self);
    حرف	*سٽ = String_قدر_Ptr(str);
    ڊگهو	n = NUM2LONG(انگ);
    aco_save_T	aco;

    إذا (سٽ == عدم)
    {
	rb_raise(rb_e_اشاريو_Error, "NULL line");
    }
    وإلا إذا (n >= 0 && n <= buf->b_ml.ml_سٽ_ڳڻپ)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    إذا (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) سٽ, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_سٽون_mark(n, 1L);

		بدليو();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    وإلا
    {
	rb_raise(rb_e_اشاريو_Error, "line number %ld out of range", n);
    }
    أرجع str;
}

#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
ساكن size_t دري_dsize(ثابت فراغ *buf);

ساكن ثابت rb_معلومات_type_t دري_type = {
    "vim_window",
    {0, 0, دري_dsize,
# إذا RUBY_نسخو >= 27
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
دري_dsize(ثابت فراغ *win اڻ_استعمال_ٿيل)
{
    أرجع حجم(win_T);
}
#جيڪڏهن_پورو

    ساكن قدر_3
دري_new(win_T *win)
{
    إذا (win->w_ruby_ref)
    {
	أرجع (قدر_3) win->w_ruby_ref;
    }
    وإلا
    {
#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
	قدر_3 obj = Typed_معلومات_Wrap_Struct(c_Vim_دري, &دري_type, win);
#وإلا
	قدر_3 obj = معلومات_Wrap_Struct(c_Vim_دري, 0, 0, win);
#جيڪڏهن_پورو
	win->w_ruby_ref = (فراغ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	أرجع obj;
    }
}

    ساكن win_T *
get_win(قدر_3 obj)
{
    win_T *win;

#جيڪڏهن_وصف_ٿيل USE_TYPEDDATA
    Typed_معلومات_Get_Struct(obj, win_T, &دري_type, win);
#وإلا
    معلومات_Get_Struct(obj, win_T, win);
#جيڪڏهن_پورو
    إذا (win == عدم)
	rb_raise(e_Deleted_دري_Error, "attempt to refer to deleted window");
    أرجع win;
}

    ساكن قدر_3
دري_s_current(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع دري_new(curwin);
}

    ساكن قدر_3
دري_s_current_getter(ID id اڻ_استعمال_ٿيل, قدر_3 *x اڻ_استعمال_ٿيل)
{
    أرجع دري_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ساكن قدر_3
سٽ_s_current(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع get_بفر_سٽ(curbuf, curwin->w_cursor.lnum);
}

    ساكن قدر_3
سيٽ_current_سٽ(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 str)
{
    أرجع سيٽ_بفر_سٽ(curbuf, curwin->w_cursor.lnum, str);
}

    ساكن قدر_3
current_سٽ_عدد(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع INT2FIX((صحيح)curwin->w_cursor.lnum);
}

    ساكن قدر_3
دري_s_ڳڻپ(قدر_3 self اڻ_استعمال_ٿيل)
{
    win_T	*w;
    صحيح n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    أرجع INT2_انگ(n);
}

    ساكن قدر_3
دري_s_aref(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 انگ)
{
    win_T *w;
    صحيح n = NUM2INT(انگ);

    لكل (w = firstwin; w != عدم; w = w->w_ايندڙ, --n)
	إذا (n == 0)
	    أرجع دري_new(w);
    أرجع Qnil;
}

    ساكن قدر_3
دري_بفر(قدر_3 self)
{
    win_T *win = get_win(self);

    أرجع بفر_new(win->w_بفر);
}

    ساكن قدر_3
دري_اوچائي(قدر_3 self)
{
    win_T *win = get_win(self);

    أرجع INT2_انگ(win->w_اوچائي);
}

    ساكن قدر_3
دري_سيٽ_اوچائي(قدر_3 self, قدر_3 اوچائي)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(اوچائي));
    curwin = savewin;
    أرجع اوچائي;
}

    ساكن قدر_3
دري_ويڪر(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع INT2_انگ(get_win(self)->w_ويڪر);
}

    ساكن قدر_3
دري_سيٽ_ويڪر(قدر_3 self اڻ_استعمال_ٿيل, قدر_3 ويڪر)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(ويڪر));
    curwin = savewin;
    أرجع ويڪر;
}

    ساكن قدر_3
دري_cursor(قدر_3 self)
{
    win_T *win = get_win(self);

    أرجع rb_assoc_new(INT2_انگ(win->w_cursor.lnum), INT2_انگ(win->w_cursor.کانو));
}

    ساكن قدر_3
دري_سيٽ_cursor(قدر_3 self, قدر_3 pos)
{
    قدر_3 lnum, کانو;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    إذا (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    کانو = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.کانو = NUM2UINT(کانو);
    win->w_سيٽ_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    أرجع Qnil;
}

    ساكن قدر_3
f_nop(قدر_3 self اڻ_استعمال_ٿيل)
{
    أرجع Qnil;
}

    ساكن قدر_3
f_p(صحيح argc, قدر_3 *argv, قدر_3 self اڻ_استعمال_ٿيل)
{
    صحيح i;
    قدر_3 str = rb_str_new("", 0);
    قدر_3 ret = Qnil;

    لكل (i = 0; i < argc; i++)
    {
	إذا (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    نياپو(RSTRING_PTR(str));

    إذا (argc == 1)
	ret = argv[0];
    وإلا إذا (argc > 1)
	ret = rb_ary_new4(argc, argv);
    أرجع ret;
}

    ساكن فراغ
ruby_io_init(فراغ)
{
#جيڪڏهن_وصف_نه_ٿيل DYNAMIC_RUBY
    RUBYEXTERN قدر_3 rb_stdout;
    RUBYEXTERN قدر_3 rb_stderr;
#جيڪڏهن_پورو

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_طريقو(rb_stdout, "write", (فراغ*)vim_پيغام, 1);
    rb_define_singleton_طريقو(rb_stdout, "flush", (فراغ*)f_nop, 0);
    rb_define_singleton_طريقو(rb_stderr, "write", (فراغ*)vim_پيغام, 1);
    rb_define_singleton_طريقو(rb_stderr, "flush", (فراغ*)f_nop, 0);
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
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_انگ(VIM_نسخو_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_انگ(VIM_نسخو_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_انگ(VIM_نسخو_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_انگ(VIM_نسخو_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_نسخو_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_نسخو_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_نسخو_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_نسخو_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_پيغام, 1);
    rb_define_module_function(mVIM, "set_option", vim_سيٽ_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_بفر_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_دري_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_بفر = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_طريقو(c_بفر, "current", (فراغ*)بفر_s_current, 0);
    rb_define_singleton_طريقو(c_بفر, "count", (فراغ*)بفر_s_ڳڻپ, 0);
    rb_define_singleton_طريقو(c_بفر, "[]", (فراغ*)بفر_s_aref, 1);
    rb_define_طريقو(c_بفر, "name", (فراغ*)بفر_name, 0);
    rb_define_طريقو(c_بفر, "number", (فراغ*)بفر_عدد, 0);
    rb_define_طريقو(c_بفر, "count", (فراغ*)بفر_ڳڻپ, 0);
    rb_define_طريقو(c_بفر, "length", (فراغ*)بفر_ڳڻپ, 0);
    rb_define_طريقو(c_بفر, "[]", (فراغ*)بفر_aref, 1);
    rb_define_طريقو(c_بفر, "[]=", (فراغ*)بفر_aset, 2);
    rb_define_طريقو(c_بفر, "delete", (فراغ*)بفر_delete, 1);
    rb_define_طريقو(c_بفر, "append", (فراغ*)بفر_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_طريقو(c_بفر, "line_number", (فراغ*)current_سٽ_عدد, 0);
    rb_define_طريقو(c_بفر, "line", (فراغ*)سٽ_s_current, 0);
    rb_define_طريقو(c_بفر, "line=", (فراغ*)سيٽ_current_سٽ, 1);


    c_Vim_دري = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_طريقو(c_Vim_دري, "current", (فراغ*)دري_s_current, 0);
    rb_define_singleton_طريقو(c_Vim_دري, "count", (فراغ*)دري_s_ڳڻپ, 0);
    rb_define_singleton_طريقو(c_Vim_دري, "[]", (فراغ*)دري_s_aref, 1);
    rb_define_طريقو(c_Vim_دري, "buffer", (فراغ*)دري_بفر, 0);
    rb_define_طريقو(c_Vim_دري, "height", (فراغ*)دري_اوچائي, 0);
    rb_define_طريقو(c_Vim_دري, "height=", (فراغ*)دري_سيٽ_اوچائي, 1);
    rb_define_طريقو(c_Vim_دري, "width", (فراغ*)دري_ويڪر, 0);
    rb_define_طريقو(c_Vim_دري, "width=", (فراغ*)دري_سيٽ_ويڪر, 1);
    rb_define_طريقو(c_Vim_دري, "cursor", (فراغ*)دري_cursor, 0);
    rb_define_طريقو(c_Vim_دري, "cursor=", (فراغ*)دري_سيٽ_cursor, 1);

    rb_define_virtual_variable("$curbuf", بفر_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", دري_s_current_getter, 0);
}

    فراغ
vim_ruby_init(فراغ *ڍير_شروعات)
{
    // should get machine stack start address early in main function
    ruby_ڍير_شروعات = ڍير_شروعات;
}

    ساكن صحيح
convert_hash2dict(قدر_3 ڪنجي, قدر_3 val, قدر_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ڪنجي)));
    إذا (di == عدم || ruby_convert_منزل_vim_قدر(val, &di->di_tv) != OK
						     || dict_جوڙيو(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	أرجع ST_STOP;
    }
    أرجع ST_CONTINUE;
}

    ساكن صحيح
ruby_convert_منزل_vim_قدر(قدر_3 val, typval_T *rettv)
{
    بدل (TYPE(val))
    {
	حالة T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NULL;
	    اكسر;
	حالة T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = VVAL_TRUE;
	    اكسر;
	حالة T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = VVAL_FALSE;
	    اكسر;
	حالة T_BIGNUM:
	حالة T_FIXNUM:
	    rettv->v_type = VAR_عدد;
	    rettv->vval.v_عدد = (varnumber_T)NUM2LONG(val);
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
		قدر_3 str = (قدر_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    اكسر;
	حالة T_ARRAY:
	    {
		list_T *l;
		ڊگهو i;
		typval_T v;

		l = list_alloc();
		إذا (l == عدم)
		    أرجع FAIL;

		لكل (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    إذا (ruby_convert_منزل_vim_قدر((قدر_3)RARRAY_PTR(val)[i],
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

		rb_hash_foreach(val, convert_hash2dict, (قدر_3)d);
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
	قدر_3 obj;

	obj = rb_eval_string_protect((ثابت حرف *)str, &state);
	إذا (state)
	    error_print(state);
	وإلا
	    retval = ruby_convert_منزل_vim_قدر(obj, rettv);
    }
    إذا (retval == FAIL)
    {
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = 0;
    }
}
