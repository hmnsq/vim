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

#অন্তর্ভাব "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN বাহ্যিক
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# যদি RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# যদি defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# যদি RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_প্রতীক		(*dll_rb_c_প্রতীক)
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

# যদি RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# যদি RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# যদি RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# যদি RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# যদি RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# যদি RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_আকার_overflow	ruby_malloc_আকার_overflow_stub
# endif

# যদি RUBY_VERSION >= 31
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

#যদি defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ঠেলো
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#যদি defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#অন্তর্ভাব <ruby.h>
#অন্তর্ভাব <ruby/encoding.h>

#যদি defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic বের_করো
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#যদি defined(MACOS_X)
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
#যদি defined(Typed_তথ্য_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_মান_Ptr
# define String_মান_Ptr(s) STR2CSTR(s)
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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define প্রতীক_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# অন্যথা
#  include <dlfcn.h>
#  define HINSTANCE শূন্য*
#  define RUBY_PROC শূন্য*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define প্রতীক_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

স্থির পূর্ণসংখ্যা ruby_initialized = 0;
স্থির শূন্য *ruby_স্তূপ_start;
স্থির মান_3 objtbl;

স্থির মান_3 mVIM;
স্থির মান_3 c_বাফার;
স্থির মান_3 cVimWindow;
স্থির মান_3 e_Deleted_বাফার_Error;
স্থির মান_3 eDeletedWindowError;

স্থির পূর্ণসংখ্যা ensure_ruby_initialized(শূন্য);
স্থির শূন্য error_print(পূর্ণসংখ্যা);
স্থির শূন্য ruby_io_init(শূন্য);
স্থির শূন্য ruby_vim_init(শূন্য);
স্থির পূর্ণসংখ্যা ruby_convert_to_vim_মান(মান_3 val, typval_T *rettv);

#যদি defined(__ia64) && !defined(ruby_init_স্তূপ)
# define ruby_init_স্তূপ(addr) ruby_init_স্তূপ((addr), rb_ia64_bsp())
#endif

#যদি defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# যদি RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# যদি RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  যদি RUBY_VERSION >= 23
#   define rb_তথ্য_typed_object_wrap	dll_rb_তথ্য_typed_object_wrap
#  অন্যথা
#   define rb_তথ্য_typed_object_alloc	dll_rb_তথ্য_typed_object_alloc
#  endif
# অন্যথা
#  define rb_তথ্য_object_alloc		dll_rb_তথ্য_object_alloc
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
# define rb_e_সূচক_Error			(*dll_rb_e_সূচক_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# যদি RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# অন্যথা
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

# যদি VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  যদি RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# যদি RUBY_VERSION < 20
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
# অন্যথা
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_মান		dll_rb_string_মান
# define rb_string_মান_ptr		dll_rb_string_মান_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ঠেলো			dll_rb_ary_ঠেলো
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_স্তূপ
#  define ruby_init_স্তূপ(addr)	dll_ruby_init_স্তূপ((addr), rb_ia64_bsp())
# অন্যথা
#  define ruby_init_স্তূপ		dll_ruby_init_স্তূপ
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_find_সূচক		dll_rb_enc_find_সূচক
# define rb_enc_find			dll_rb_enc_find
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
স্থির মান_3 (*dll_rb_assoc_new) (মান_3, মান_3);
মান_3 *dll_rb_cFalseClass;
মান_3 *dll_rb_cFixnum;
# যদি defined(USE_RUBY_INTEGER)
মান_3 *dll_rb_cInteger;
# endif
# যদি RUBY_VERSION >= 20
মান_3 *dll_rb_cFloat;
# endif
মান_3 *dll_rb_cNilClass;
স্থির মান_3 *dll_rb_cObject;
মান_3 *dll_rb_cString;
মান_3 *dll_rb_c_প্রতীক;
মান_3 *dll_rb_cTrueClass;
স্থির মান_3 (*dll_rb_class_new_instance) (পূর্ণসংখ্যা,মান_3*,মান_3);
স্থির শূন্য (*dll_rb_check_type) (মান_3,পূর্ণসংখ্যা);
# ifdef USE_TYPEDDATA
স্থির শূন্য *(*dll_rb_check_typeddata) (মান_3,ধ্রুবক rb_তথ্য_type_t *);
# endif
স্থির মান_3 (*dll_rb_class_path) (মান_3);
# ifdef USE_TYPEDDATA
#  যদি RUBY_VERSION >= 23
স্থির মান_3 (*dll_rb_তথ্য_typed_object_wrap) (মান_3, শূন্য*, ধ্রুবক rb_তথ্য_type_t *);
#  অন্যথা
স্থির মান_3 (*dll_rb_তথ্য_typed_object_alloc) (মান_3, শূন্য*, ধ্রুবক rb_তথ্য_type_t *);
#  endif
# অন্যথা
স্থির মান_3 (*dll_rb_তথ্য_object_alloc) (মান_3, শূন্য*, RUBY_তথ্য_FUNC, RUBY_তথ্য_FUNC);
# endif
# যদি RUBY_VERSION >= 31
স্থির শূন্য (*dll_rb_debug_rstring_null_ptr) (ধ্রুবক অক্ষর*);
# endif
স্থির মান_3 (*dll_rb_define_class_under) (মান_3, ধ্রুবক অক্ষর*, মান_3);
স্থির শূন্য (*dll_rb_define_const) (মান_3,ধ্রুবক অক্ষর*,মান_3);
স্থির শূন্য (*dll_rb_define_global_function) (ধ্রুবক অক্ষর*,মান_3(*)(পূর্ণসংখ্যা,মান_3*,মান_3),পূর্ণসংখ্যা);
স্থির শূন্য (*dll_rb_define_method) (মান_3,ধ্রুবক অক্ষর*,মান_3(*)(ANYARGS),পূর্ণসংখ্যা);
স্থির মান_3 (*dll_rb_define_module) (ধ্রুবক অক্ষর*);
স্থির শূন্য (*dll_rb_define_module_function) (মান_3,ধ্রুবক অক্ষর*,মান_3(*)(মান_3,মান_3),পূর্ণসংখ্যা);
স্থির শূন্য (*dll_rb_define_singleton_method) (মান_3,ধ্রুবক অক্ষর*,মান_3(*)(ANYARGS),পূর্ণসংখ্যা);
স্থির শূন্য (*dll_rb_define_virtual_variable) (ধ্রুবক অক্ষর*,মান_3(*)(ID,মান_3*),শূন্য(*)());
স্থির মান_3 *dll_rb_stdout;
স্থির মান_3 *dll_rb_stderr;
স্থির মান_3 *dll_rb_eArgError;
স্থির মান_3 *dll_rb_e_সূচক_Error;
স্থির মান_3 *dll_rb_eRuntimeError;
স্থির মান_3 *dll_rb_eStandardError;
স্থির মান_3 (*dll_rb_eval_string_protect) (ধ্রুবক অক্ষর*, পূর্ণসংখ্যা*);
# যদি RUBY_VERSION >= 21
স্থির মান_3 (*dll_rb_funcallv) (মান_3, ID, পূর্ণসংখ্যা, ধ্রুবক মান_3*);
# অন্যথা
স্থির মান_3 (*dll_rb_funcall2) (মান_3, ID, পূর্ণসংখ্যা, ধ্রুবক মান_3*);
# endif
স্থির শূন্য (*dll_rb_global_variable) (মান_3*);
স্থির মান_3 (*dll_rb_hash_aset) (মান_3, মান_3, মান_3);
স্থির মান_3 (*dll_rb_hash_foreach) (মান_3, পূর্ণসংখ্যা (*)(মান_3, মান_3, মান_3), মান_3);
স্থির মান_3 (*dll_rb_hash_new) (শূন্য);
স্থির মান_3 (*dll_rb_inspect) (মান_3);
স্থির মান_3 (*dll_rb_int2inum) (long);
স্থির ID (*dll_rb_intern) (ধ্রুবক অক্ষর*);
# যদি RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
স্থির long (*dll_rb_fix2int) (মান_3);
স্থির long (*dll_rb_num2int) (মান_3);
স্থির unsigned long (*dll_rb_num2uint) (মান_3);
# endif
স্থির দ্বিগুণ (*dll_rb_num2dbl) (মান_3);
স্থির মান_3 (*dll_rb_lastline_get) (শূন্য);
স্থির শূন্য (*dll_rb_lastline_set) (মান_3);
স্থির মান_3 (*dll_rb_protect) (মান_3 (*)(মান_3), মান_3, পূর্ণসংখ্যা*);
স্থির শূন্য (*dll_rb_load) (মান_3, পূর্ণসংখ্যা);
স্থির long (*dll_rb_num2long) (মান_3);
স্থির unsigned long (*dll_rb_num2ulong) (মান_3);
স্থির মান_3 (*dll_rb_obj_alloc) (মান_3);
স্থির মান_3 (*dll_rb_obj_as_string) (মান_3);
স্থির মান_3 (*dll_rb_obj_id) (মান_3);
স্থির শূন্য (*dll_rb_raise) (মান_3, ধ্রুবক অক্ষর*, ...);
স্থির মান_3 (*dll_rb_string_মান) (অস্থির মান_3*);
স্থির মান_3 (*dll_rb_str_cat) (মান_3, ধ্রুবক অক্ষর*, long);
স্থির মান_3 (*dll_rb_str_concat) (মান_3, মান_3);
স্থির মান_3 (*dll_rb_str_new) (ধ্রুবক অক্ষর*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
স্থির মান_3 (*dll_rb_str_new_cstr) (ধ্রুবক অক্ষর*);
# অন্যথা
স্থির মান_3 (*dll_rb_str_new2) (ধ্রুবক অক্ষর*);
# endif
স্থির মান_3 (*dll_rb_errinfo) (শূন্য);
স্থির শূন্য (*dll_ruby_init) (শূন্য);
স্থির শূন্য (*dll_ruby_init_loadpath) (শূন্য);
# ifdef MSWIN
স্থির শূন্য (*dll_ruby_sysinit) (পূর্ণসংখ্যা*, অক্ষর***);
স্থির পূর্ণসংখ্যা (*dll_rb_w32_snprintf)(অক্ষর*, size_t, ধ্রুবক অক্ষর*, ...);
# endif
# যদি RUBY_VERSION >= 31
#  ifdef _MSC_VER
স্থির শূন্য (*dll_rb_unexpected_type) (মান_3, পূর্ণসংখ্যা);
#  অন্যথা
NORETURN(স্থির শূন্য (*dll_rb_unexpected_type) (মান_3, পূর্ণসংখ্যা));
#  endif
# endif
স্থির অক্ষর * (*dll_rb_string_মান_ptr) (অস্থির মান_3*);
স্থির মান_3 (*dll_rb_float_new) (দ্বিগুণ);
স্থির মান_3 (*dll_rb_ary_new) (শূন্য);
স্থির মান_3 (*dll_rb_ary_new4) (long n, ধ্রুবক মান_3 *elts);
স্থির মান_3 (*dll_rb_ary_ঠেলো) (মান_3, মান_3);
# যদি RUBY_VERSION >= 26 && RUBY_VERSION <= 32
স্থির শূন্য (*dll_rb_ary_detransient) (মান_3);
# endif
# ifdef __ia64
স্থির শূন্য * (*dll_rb_ia64_bsp) (শূন্য);
স্থির শূন্য (*dll_ruby_init_স্তূপ)(মান_3*, শূন্য*);
# অন্যথা
স্থির শূন্য (*dll_ruby_init_স্তূপ)(মান_3*);
# endif
স্থির মান_3 (*dll_rb_int2big)(SIGNED_মান);

স্থির শূন্য (*dll_ruby_script) (ধ্রুবক অক্ষর*);
স্থির পূর্ণসংখ্যা (*dll_rb_enc_find_সূচক) (ধ্রুবক অক্ষর*);
স্থির rb_encoding* (*dll_rb_enc_find) (ধ্রুবক অক্ষর*);
স্থির মান_3 (*dll_rb_enc_str_new) (ধ্রুবক অক্ষর*, long, rb_encoding*);
স্থির মান_3 (*dll_rb_sprintf) (ধ্রুবক অক্ষর*, ...);
স্থির মান_3 (*dll_rb_require) (ধ্রুবক অক্ষর*);
স্থির শূন্য* (*dll_ruby_options)(পূর্ণসংখ্যা, অক্ষর**);

# যদি defined(USE_RGENGC) && USE_RGENGC
#  যদি RUBY_VERSION == 21
স্থির শূন্য (*dll_rb_gc_writebarrier_unprotect_promoted)(মান_3);
#  অন্যথা
স্থির শূন্য (*dll_rb_gc_writebarrier_unprotect)(মান_3 obj);
#  endif
# endif

# যদি RUBY_VERSION >= 30
#  ifdef _MSC_VER
স্থির শূন্য (*dll_ruby_malloc_আকার_overflow)(size_t, size_t);
#  অন্যথা
NORETURN(স্থির শূন্য (*dll_ruby_malloc_আকার_overflow)(size_t, size_t));
#  endif
# endif

# যদি RUBY_VERSION >= 26 && RUBY_VERSION <= 32
শূন্য rb_ary_detransient_stub(মান_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  যদি RUBY_VERSION >= 22
    long
rb_num2long_stub(মান_3 x)
#  অন্যথা
    SIGNED_মান
rb_num2long_stub(মান_3 x)
#  endif
{
    ফেরত dll_rb_num2long(x);
}
#  যদি RUBY_VERSION >= 26
    মান_3
rb_int2big_stub(intptr_t x)
#  অন্যথা
    মান_3
rb_int2big_stub(SIGNED_মান x)
#  endif
{
    ফেরত dll_rb_int2big(x);
}
#  যদি RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(মান_3 x)
{
    ফেরত dll_rb_fix2int(x);
}
    long
rb_num2int_stub(মান_3 x)
{
    ফেরত dll_rb_num2int(x);
}
#  endif
#  যদি RUBY_VERSION >= 20
    মান_3
rb_float_new_in_heap(দ্বিগুণ d)
{
    ফেরত dll_rb_float_new(d);
}
#   যদি RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(মান_3 x)
#   অন্যথা
    মান_3
rb_num2ulong(মান_3 x)
#   endif
{
    ফেরত (long)RSHIFT((SIGNED_মান)(x),1);
}
#  endif
#  যদি defined(USE_RGENGC) && USE_RGENGC
#   যদি RUBY_VERSION == 21
    শূন্য
rb_gc_writebarrier_unprotect_promoted_stub(মান_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   অন্যথা
    শূন্য
rb_gc_writebarrier_unprotect_stub(মান_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  যদি RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    শূন্য
rb_ary_detransient_stub(মান_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  যদি RUBY_VERSION >= 30
    শূন্য
rb_check_type_stub(মান_3 obj, পূর্ণসংখ্যা t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(মান_3 x)
{
    ফেরত dll_rb_num2uint(x);
}
    শূন্য
ruby_malloc_আকার_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_আকার_overflow(x, y);
}
#  endif
#  যদি RUBY_VERSION >= 31
    শূন্য
rb_debug_rstring_null_ptr_stub(ধ্রুবক অক্ষর *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    শূন্য
rb_unexpected_type_stub(মান_3 self, পূর্ণসংখ্যা t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
শূন্য *rb_check_typeddata_stub(মান_3 obj, ধ্রুবক rb_তথ্য_type_t *তথ্য_type)
{
    ফেরত dll_rb_check_typeddata(obj, তথ্য_type);
}
#  endif
# endif // ifndef PROTO

স্থির HINSTANCE hinstRuby = ফাঁকা; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
স্থির কাঠামো
{
    অক্ষর *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# যদি defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# অন্যথা
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# যদি RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_প্রতীক},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  যদি RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_তথ্য_typed_object_wrap},
#  অন্যথা
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_তথ্য_typed_object_alloc},
#  endif
# অন্যথা
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_তথ্য_object_alloc},
# endif
# যদি RUBY_VERSION >= 31
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
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_সূচক_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# যদি RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# অন্যথা
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# যদি RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_মান},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# অন্যথা
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# যদি RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_মান_ptr},
# যদি RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# অন্যথা
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# অন্যথা
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ঠেলো},
# যদি RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_find_সূচক},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_find},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_স্তূপ},
# যদি defined(USE_RGENGC) && USE_RGENGC
#  যদি RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  অন্যথা
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# যদি RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_আকার_overflow},
# endif
    {"", ফাঁকা},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
ruby_runtime_link_init(অক্ষর *libname, পূর্ণসংখ্যা verbose)
{
    পূর্ণসংখ্যা i;

    যদি (hinstRuby)
	ফেরত OK;
    hinstRuby = load_dll(libname);
    যদি (!hinstRuby)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ফেরত FAIL;
    }

    জন্য (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	যদি (!(*ruby_funcname_table[i].ptr = প্রতীক_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ফাঁকা;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    ফেরত FAIL;
	}
    }
    ফেরত OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    পূর্ণসংখ্যা
ruby_enabled(পূর্ণসংখ্যা verbose)
{
    ফেরত ruby_runtime_link_init((অক্ষর *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    শূন্য
ruby_end(শূন্য)
{
}

    শূন্য
ex_ruby(exarg_T *eap)
{
    পূর্ণসংখ্যা state;
    অক্ষর *script = ফাঁকা;

    script = (অক্ষর *)script_get(eap, eap->arg);
    যদি (!eap->skip && ensure_ruby_initialized())
    {
	যদি (script == ফাঁকা)
	    rb_eval_string_protect((অক্ষর *)eap->arg, &state);
	অন্যথা
	    rb_eval_string_protect(script, &state);
	যদি (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    স্থির মান_3
vim_str2rb_enc_str(ধ্রুবক অক্ষর *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    যদি (get_option_মান((char_u *)"enc", &lval, &sval, ফাঁকা, 0) == gov_string)
    {
	enc = rb_enc_find((অক্ষর *)sval);
	vim_free(sval);
	যদি (enc)
	    ফেরত rb_enc_str_new(s, (long)strlen(s), enc);
    }
    ফেরত rb_str_new2(s);
}

    স্থির মান_3
eval_enc_string_protect(ধ্রুবক অক্ষর *str, পূর্ণসংখ্যা *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    মান_3 v;

    যদি (get_option_মান((char_u *)"enc", &lval, &sval, ফাঁকা, 0) == gov_string)
    {
	enc = rb_enc_find((অক্ষর *)sval);
	vim_free(sval);
	যদি (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ফেরত rb_eval_string_protect(String_মান_Ptr(v), state);
	}
    }
    ফেরত rb_eval_string_protect(str, state);
}

    শূন্য
ex_rubydo(exarg_T *eap)
{
    পূর্ণসংখ্যা state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    যদি (!ensure_ruby_initialized())
	ফেরত;

    যদি (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ফেরত;
    জন্য (i = eap->line1; i <= eap->line2; i++)
    {
	মান_3 সারি;

	যদি (i > curbuf->b_ml.ml_সারি_গুনতি)
	    বিরতি;
	সারি = vim_str2rb_enc_str((অক্ষর *)ml_get(i));
	rb_lastline_set(সারি);
	eval_enc_string_protect((অক্ষর *) eap->arg, &state);
	যদি (state)
	{
	    error_print(state);
	    বিরতি;
	}
	যদি (was_curbuf != curbuf || i > curbuf->b_ml.ml_সারি_গুনতি)
	    বিরতি;
	সারি = rb_lastline_get();
	যদি (!NIL_P(সারি))
	{
	    যদি (TYPE(সারি) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		ফেরত;
	    }
	    ml_replace(i, (char_u *) String_মান_Ptr(সারি), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    স্থির মান_3
rb_load_wrap(মান_3 file_to_load)
{
    rb_load(file_to_load, 0);
    ফেরত Qnil;
}

    শূন্য
ex_rubyfile(exarg_T *eap)
{
    পূর্ণসংখ্যা state;

    যদি (!ensure_ruby_initialized())
	ফেরত;

    মান_3 file_to_load = rb_str_new2((ধ্রুবক অক্ষর *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    যদি (state)
	error_print(state);
}

    শূন্য
ruby_বাফার_free(buf_T *buf)
{
    যদি (buf->b_ruby_ref == ফাঁকা)
	ফেরত;

    rb_hash_aset(objtbl, rb_obj_id((মান_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->তথ্য = ফাঁকা;
}

    শূন্য
ruby_window_free(win_T *win)
{
    যদি (win->w_ruby_ref == ফাঁকা)
	ফেরত;

    rb_hash_aset(objtbl, rb_obj_id((মান_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->তথ্য = ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
ensure_ruby_initialized(শূন্য)
{
    যদি (ruby_initialized)
	ফেরত ruby_initialized;

#ifdef DYNAMIC_RUBY
    যদি (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	পূর্ণসংখ্যা argc = 1;
	অক্ষর *argv[] = {"gvim.exe"};
	অক্ষর **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_স্তূপ(ruby_স্তূপ_start);
	    ruby_init();
	}
	{
	    পূর্ণসংখ্যা dummy_argc = 2;
	    অক্ষর *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    অন্যথা
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ফেরত 0;
    }
#endif

    ফেরত ruby_initialized;
}

    স্থির শূন্য
error_print(পূর্ণসংখ্যা state)
{
    মান_3 error;
    মান_3 eclass;
    মান_3 einfo;
    মান_3 bt;
    পূর্ণসংখ্যা attr;
    অক্ষর buff[BUFSIZ];
    long i;

#define TAG_RETURN	0x1
#define TAG_BREAK	0x2
#define TAG_পরবর্তী	0x3
#define TAG_RETRY	0x4
#define TAG_REDO	0x5
#define TAG_RAISE	0x6
#define TAG_THROW	0x7
#define TAG_FATAL	0x8
#define TAG_MASK	0xf

    নির্বাচন (state)
    {
	ক্ষেত্র TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    বিরতি;
	ক্ষেত্র TAG_পরবর্তী:
	    emsg(_(e_unexpected_পরবর্তী));
	    বিরতি;
	ক্ষেত্র TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    বিরতি;
	ক্ষেত্র TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    বিরতি;
	ক্ষেত্র TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    বিরতি;
	ক্ষেত্র TAG_RAISE:
	ক্ষেত্র TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    যদি (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    অন্যথা
	    {
		মান_3 epath;
		অক্ষর *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		যদি (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#যদি RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    জন্য (i = 0; i < RARRAY_LEN(bt); i++)
		বার্তা_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#অন্যথা
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    জন্য (i = 0; i < RARRAY_LEN(bt); i++)
		বার্তা_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    বিরতি;
	পূর্বনির্ধারিত:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    বিরতি;
    }
}

    স্থির মান_3
vim_message(মান_3 self UNUSED, মান_3 str)
{
    অক্ষর *buff, *p;

    str = rb_obj_as_string(str);
    যদি (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(অক্ষর, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	যদি (p) *p = '\0';
	বার্তা(buff);
    }
    অন্যথা
    {
	বার্তা("");
    }
    ফেরত Qnil;
}

    স্থির মান_3
vim_set_option(মান_3 self UNUSED, মান_3 str)
{
    do_set((char_u *)String_মান_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    ফেরত Qnil;
}

    স্থির মান_3
vim_command(মান_3 self UNUSED, মান_3 str)
{
    do_cmdline_cmd((char_u *)String_মান_Ptr(str));
    ফেরত Qnil;
}

#ifdef FEAT_EVAL
    স্থির মান_3
vim_to_ruby(typval_T *tv)
{
    মান_3 ফলাফল = Qnil;

    যদি (tv->v_type == VAR_STRING)
    {
	ফলাফল = rb_str_new2(tv->vval.v_string == ফাঁকা
					  ? "" : (অক্ষর *)(tv->vval.v_string));
    }
    অন্যথা যদি (tv->v_type == VAR_সংখ্যা)
    {
	ফলাফল = INT2_অঙ্ক(tv->vval.v_সংখ্যা);
    }
    অন্যথা যদি (tv->v_type == VAR_FLOAT)
    {
	ফলাফল = rb_float_new(tv->vval.v_float);
    }
    অন্যথা যদি (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	ফলাফল = rb_ary_new();

	যদি (list != ফাঁকা)
	{
	    FOR_ALL_LIST_বস্তুসমূহ(list, curr)
		rb_ary_ঠেলো(ফলাফল, vim_to_ruby(&curr->li_tv));
	}
    }
    অন্যথা যদি (tv->v_type == VAR_DICT)
    {
	ফলাফল = rb_hash_new();

	যদি (tv->vval.v_dict != ফাঁকা)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
	    {
		যদি (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(ফলাফল, rb_str_new2((অক্ষর *)hi->hi_চাবি),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    অন্যথা যদি (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	যদি (tv->vval.v_সংখ্যা == VVAL_TRUE)
	    ফলাফল = Qtrue;
	অন্যথা যদি (tv->vval.v_সংখ্যা == VVAL_FALSE)
	    ফলাফল = Qfalse;
    }
    অন্যথা যদি (tv->v_type == VAR_BLOB)
    {
	ফলাফল = rb_str_new(tv->vval.v_blob->bv_ga.ga_তথ্য,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ফেরত ফলাফল;
}
#endif

    স্থির মান_3
vim_evaluate(মান_3 self UNUSED, মান_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    মান_3       ফলাফল;

    tv = eval_রাশিমালা((char_u *)String_মান_Ptr(str), ফাঁকা);
    যদি (tv == ফাঁকা)
	ফেরত Qnil;
    ফলাফল = vim_to_ruby(tv);

    free_tv(tv);

    ফেরত ফলাফল;
#অন্যথা
    ফেরত Qnil;
#endif
}

#ifdef USE_TYPEDDATA
স্থির size_t বাফার_dsize(ধ্রুবক শূন্য *buf);

স্থির ধ্রুবক rb_তথ্য_type_t বাফার_type = {
    "vim_buffer",
    {0, 0, বাফার_dsize,
# যদি RUBY_VERSION >= 27
	0, {0}
# অন্যথা
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    স্থির size_t
বাফার_dsize(ধ্রুবক শূন্য *buf UNUSED)
{
    ফেরত আকার(buf_T);
}
#endif

    স্থির মান_3
বাফার_new(buf_T *buf)
{
    যদি (buf->b_ruby_ref)
    {
	ফেরত (মান_3) buf->b_ruby_ref;
    }
    অন্যথা
    {
#ifdef USE_TYPEDDATA
	মান_3 obj = Typed_তথ্য_Wrap_Struct(c_বাফার, &বাফার_type, buf);
#অন্যথা
	মান_3 obj = তথ্য_Wrap_Struct(c_বাফার, 0, 0, buf);
#endif
	buf->b_ruby_ref = (শূন্য *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ফেরত obj;
    }
}

    স্থির buf_T *
get_buf(মান_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    Typed_তথ্য_Get_Struct(obj, buf_T, &বাফার_type, buf);
#অন্যথা
    তথ্য_Get_Struct(obj, buf_T, buf);
#endif
    যদি (buf == ফাঁকা)
	rb_raise(e_Deleted_বাফার_Error, "attempt to refer to deleted buffer");
    ফেরত buf;
}

    স্থির মান_3
vim_blob(মান_3 self UNUSED, মান_3 str)
{
    মান_3 ফলাফল = rb_str_new("0z", 2);
    অক্ষর    buf[4];
    পূর্ণসংখ্যা	i;
    জন্য (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned অক্ষর)(RSTRING_PTR(str)[i]));
	rb_str_concat(ফলাফল, rb_str_new2(buf));
    }
    ফেরত ফলাফল;
}

    স্থির মান_3
বাফার_s_current(মান_3 self UNUSED)
{
    ফেরত বাফার_new(curbuf);
}

    স্থির মান_3
বাফার_s_current_getter(ID id UNUSED, মান_3 *x UNUSED)
{
    ফেরত বাফার_new(curbuf);
}

    স্থির মান_3
বাফার_s_গুনতি(মান_3 self UNUSED)
{
    buf_T *b;
    পূর্ণসংখ্যা n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	যদি (b->b_p_bl)
	    n++;
    }

    ফেরত INT2_অঙ্ক(n);
}

    স্থির মান_3
বাফার_s_aref(মান_3 self UNUSED, মান_3 অঙ্ক)
{
    buf_T *b;
    পূর্ণসংখ্যা n = NUM2INT(অঙ্ক);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	যদি (!b->b_p_bl)
	    চালু;

	যদি (n == 0)
	    ফেরত বাফার_new(b);

	n--;
    }
    ফেরত Qnil;
}

    স্থির মান_3
বাফার_name(মান_3 self)
{
    buf_T *buf = get_buf(self);

    ফেরত buf->b_ffname ? rb_str_new2((অক্ষর *)buf->b_ffname) : Qnil;
}

    স্থির মান_3
বাফার_সংখ্যা(মান_3 self)
{
    buf_T *buf = get_buf(self);

    ফেরত INT2_অঙ্ক(buf->b_fnum);
}

    স্থির মান_3
বাফার_গুনতি(মান_3 self)
{
    buf_T *buf = get_buf(self);

    ফেরত INT2_অঙ্ক(buf->b_ml.ml_সারি_গুনতি);
}

    স্থির মান_3
get_বাফার_সারি(buf_T *buf, linenr_T n)
{
    যদি (n <= 0 || n > buf->b_ml.ml_সারি_গুনতি)
	rb_raise(rb_e_সূচক_Error, "line number %ld out of range", (long)n);
    ফেরত vim_str2rb_enc_str((অক্ষর *)ml_get_buf(buf, n, FALSE));
}

    স্থির মান_3
বাফার_aref(মান_3 self, মান_3 অঙ্ক)
{
    buf_T *buf = get_buf(self);

    যদি (buf != ফাঁকা)
	ফেরত get_বাফার_সারি(buf, (linenr_T)NUM2LONG(অঙ্ক));
    ফেরত Qnil; // For stop warning
}

    স্থির মান_3
set_বাফার_সারি(buf_T *buf, linenr_T n, মান_3 str)
{
    অক্ষর	*সারি = String_মান_Ptr(str);
    aco_save_T	aco;

    যদি (n > 0 && n <= buf->b_ml.ml_সারি_গুনতি && সারি != ফাঁকা)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    যদি (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)সারি, TRUE);
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
    অন্যথা
    {
	rb_raise(rb_e_সূচক_Error, "line number %ld out of range", (long)n);
    }
    ফেরত str;
}

    স্থির মান_3
বাফার_aset(মান_3 self, মান_3 অঙ্ক, মান_3 str)
{
    buf_T *buf = get_buf(self);

    যদি (buf != ফাঁকা)
	ফেরত set_বাফার_সারি(buf, (linenr_T)NUM2LONG(অঙ্ক), str);
    ফেরত str;
}

    স্থির মান_3
বাফার_delete(মান_3 self, মান_3 অঙ্ক)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(অঙ্ক);
    aco_save_T	aco;

    যদি (n > 0 && n <= buf->b_ml.ml_সারি_গুনতি)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    যদি (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_সারিগুলি_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    অন্যথা
    {
	rb_raise(rb_e_সূচক_Error, "line number %ld out of range", n);
    }
    ফেরত Qnil;
}

    স্থির মান_3
বাফার_append(মান_3 self, মান_3 অঙ্ক, মান_3 str)
{
    buf_T	*buf = get_buf(self);
    অক্ষর	*সারি = String_মান_Ptr(str);
    long	n = NUM2LONG(অঙ্ক);
    aco_save_T	aco;

    যদি (সারি == ফাঁকা)
    {
	rb_raise(rb_e_সূচক_Error, "NULL line");
    }
    অন্যথা যদি (n >= 0 && n <= buf->b_ml.ml_সারি_গুনতি)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    যদি (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) সারি, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_সারিগুলি_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    অন্যথা
    {
	rb_raise(rb_e_সূচক_Error, "line number %ld out of range", n);
    }
    ফেরত str;
}

#ifdef USE_TYPEDDATA
স্থির size_t window_dsize(ধ্রুবক শূন্য *buf);

স্থির ধ্রুবক rb_তথ্য_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# যদি RUBY_VERSION >= 27
	0, {0}
# অন্যথা
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    স্থির size_t
window_dsize(ধ্রুবক শূন্য *win UNUSED)
{
    ফেরত আকার(win_T);
}
#endif

    স্থির মান_3
window_new(win_T *win)
{
    যদি (win->w_ruby_ref)
    {
	ফেরত (মান_3) win->w_ruby_ref;
    }
    অন্যথা
    {
#ifdef USE_TYPEDDATA
	মান_3 obj = Typed_তথ্য_Wrap_Struct(cVimWindow, &window_type, win);
#অন্যথা
	মান_3 obj = তথ্য_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (শূন্য *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ফেরত obj;
    }
}

    স্থির win_T *
get_win(মান_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    Typed_তথ্য_Get_Struct(obj, win_T, &window_type, win);
#অন্যথা
    তথ্য_Get_Struct(obj, win_T, win);
#endif
    যদি (win == ফাঁকা)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    ফেরত win;
}

    স্থির মান_3
window_s_current(মান_3 self UNUSED)
{
    ফেরত window_new(curwin);
}

    স্থির মান_3
window_s_current_getter(ID id UNUSED, মান_3 *x UNUSED)
{
    ফেরত window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    স্থির মান_3
সারি_s_current(মান_3 self UNUSED)
{
    ফেরত get_বাফার_সারি(curbuf, curwin->w_cursor.lnum);
}

    স্থির মান_3
set_current_সারি(মান_3 self UNUSED, মান_3 str)
{
    ফেরত set_বাফার_সারি(curbuf, curwin->w_cursor.lnum, str);
}

    স্থির মান_3
current_সারি_সংখ্যা(মান_3 self UNUSED)
{
    ফেরত INT2FIX((পূর্ণসংখ্যা)curwin->w_cursor.lnum);
}

    স্থির মান_3
window_s_গুনতি(মান_3 self UNUSED)
{
    win_T	*w;
    পূর্ণসংখ্যা n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ফেরত INT2_অঙ্ক(n);
}

    স্থির মান_3
window_s_aref(মান_3 self UNUSED, মান_3 অঙ্ক)
{
    win_T *w;
    পূর্ণসংখ্যা n = NUM2INT(অঙ্ক);

    জন্য (w = firstwin; w != ফাঁকা; w = w->w_পরবর্তী, --n)
	যদি (n == 0)
	    ফেরত window_new(w);
    ফেরত Qnil;
}

    স্থির মান_3
window_বাফার(মান_3 self)
{
    win_T *win = get_win(self);

    ফেরত বাফার_new(win->w_বাফার);
}

    স্থির মান_3
window_height(মান_3 self)
{
    win_T *win = get_win(self);

    ফেরত INT2_অঙ্ক(win->w_height);
}

    স্থির মান_3
window_set_height(মান_3 self, মান_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    ফেরত height;
}

    স্থির মান_3
window_width(মান_3 self UNUSED)
{
    ফেরত INT2_অঙ্ক(get_win(self)->w_width);
}

    স্থির মান_3
window_set_width(মান_3 self UNUSED, মান_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    ফেরত width;
}

    স্থির মান_3
window_cursor(মান_3 self)
{
    win_T *win = get_win(self);

    ফেরত rb_assoc_new(INT2_অঙ্ক(win->w_cursor.lnum), INT2_অঙ্ক(win->w_cursor.col));
}

    স্থির মান_3
window_set_cursor(মান_3 self, মান_3 pos)
{
    মান_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    যদি (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ফেরত Qnil;
}

    স্থির মান_3
f_nop(মান_3 self UNUSED)
{
    ফেরত Qnil;
}

    স্থির মান_3
f_p(পূর্ণসংখ্যা argc, মান_3 *argv, মান_3 self UNUSED)
{
    পূর্ণসংখ্যা i;
    মান_3 str = rb_str_new("", 0);
    মান_3 ret = Qnil;

    জন্য (i = 0; i < argc; i++)
    {
	যদি (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    বার্তা(RSTRING_PTR(str));

    যদি (argc == 1)
	ret = argv[0];
    অন্যথা যদি (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ফেরত ret;
}

    স্থির শূন্য
ruby_io_init(শূন্য)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN মান_3 rb_stdout;
    RUBYEXTERN মান_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (শূন্য*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (শূন্য*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (শূন্য*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (শূন্য*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    স্থির শূন্য
ruby_vim_init(শূন্য)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_অঙ্ক(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_অঙ্ক(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_অঙ্ক(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_অঙ্ক(VIM_VERSION_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_VERSION_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_VERSION_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_VERSION_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_VERSION_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_message, 1);
    rb_define_module_function(mVIM, "set_option", vim_set_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_বাফার_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    eDeletedWindowError = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_বাফার = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_method(c_বাফার, "current", (শূন্য*)বাফার_s_current, 0);
    rb_define_singleton_method(c_বাফার, "count", (শূন্য*)বাফার_s_গুনতি, 0);
    rb_define_singleton_method(c_বাফার, "[]", (শূন্য*)বাফার_s_aref, 1);
    rb_define_method(c_বাফার, "name", (শূন্য*)বাফার_name, 0);
    rb_define_method(c_বাফার, "number", (শূন্য*)বাফার_সংখ্যা, 0);
    rb_define_method(c_বাফার, "count", (শূন্য*)বাফার_গুনতি, 0);
    rb_define_method(c_বাফার, "length", (শূন্য*)বাফার_গুনতি, 0);
    rb_define_method(c_বাফার, "[]", (শূন্য*)বাফার_aref, 1);
    rb_define_method(c_বাফার, "[]=", (শূন্য*)বাফার_aset, 2);
    rb_define_method(c_বাফার, "delete", (শূন্য*)বাফার_delete, 1);
    rb_define_method(c_বাফার, "append", (শূন্য*)বাফার_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(c_বাফার, "line_number", (শূন্য*)current_সারি_সংখ্যা, 0);
    rb_define_method(c_বাফার, "line", (শূন্য*)সারি_s_current, 0);
    rb_define_method(c_বাফার, "line=", (শূন্য*)set_current_সারি, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (শূন্য*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (শূন্য*)window_s_গুনতি, 0);
    rb_define_singleton_method(cVimWindow, "[]", (শূন্য*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (শূন্য*)window_বাফার, 0);
    rb_define_method(cVimWindow, "height", (শূন্য*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (শূন্য*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (শূন্য*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (শূন্য*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (শূন্য*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (শূন্য*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", বাফার_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    শূন্য
vim_ruby_init(শূন্য *স্তূপ_start)
{
    // should get machine stack start address early in main function
    ruby_স্তূপ_start = স্তূপ_start;
}

    স্থির পূর্ণসংখ্যা
convert_hash2dict(মান_3 চাবি, মান_3 val, মান_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(চাবি)));
    যদি (di == ফাঁকা || ruby_convert_to_vim_মান(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ফেরত ST_STOP;
    }
    ফেরত ST_CONTINUE;
}

    স্থির পূর্ণসংখ্যা
ruby_convert_to_vim_মান(মান_3 val, typval_T *rettv)
{
    নির্বাচন (TYPE(val))
    {
	ক্ষেত্র T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_সংখ্যা = VVAL_NULL;
	    বিরতি;
	ক্ষেত্র T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_সংখ্যা = VVAL_TRUE;
	    বিরতি;
	ক্ষেত্র T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_সংখ্যা = VVAL_FALSE;
	    বিরতি;
	ক্ষেত্র T_BIGNUM:
	ক্ষেত্র T_FIXNUM:
	    rettv->v_type = VAR_সংখ্যা;
	    rettv->vval.v_সংখ্যা = (varnumber_T)NUM2LONG(val);
	    বিরতি;
	ক্ষেত্র T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    বিরতি;
	পূর্বনির্ধারিত:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	ক্ষেত্র T_STRING:
	    {
		মান_3 str = (মান_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    বিরতি;
	ক্ষেত্র T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		যদি (l == ফাঁকা)
		    ফেরত FAIL;

		জন্য (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    যদি (ruby_convert_to_vim_মান((মান_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ফেরত FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    বিরতি;
	ক্ষেত্র T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		যদি (d == ফাঁকা)
		    ফেরত FAIL;

		rb_hash_foreach(val, convert_hash2dict, (মান_3)d);
		যদি (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ফেরত FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    বিরতি;
    }
    ফেরত OK;
}

    শূন্য
do_rubyeval(char_u *str, typval_T *rettv)
{
    পূর্ণসংখ্যা retval = FAIL;

    যদি (ensure_ruby_initialized())
    {
	পূর্ণসংখ্যা state;
	মান_3 obj;

	obj = rb_eval_string_protect((ধ্রুবক অক্ষর *)str, &state);
	যদি (state)
	    error_print(state);
	অন্যথা
	    retval = ruby_convert_to_vim_মান(obj, rettv);
    }
    যদি (retval == FAIL)
    {
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = 0;
    }
}
