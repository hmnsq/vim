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

#ଅନ୍ତର୍ଭାଵ "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN ବାହ୍ୟ
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# ଯଦି RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ଯଦି defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ଯଦି RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_ପ୍ରତୀକ		(*dll_rb_c_ପ୍ରତୀକ)
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

# ଯଦି RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ଯଦି RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ଯଦି RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ଯଦି RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ଯଦି RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ଯଦି RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# ଯଦି RUBY_VERSION >= 31
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

#ଯଦି defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ଠେଲ
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ଯଦି defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#ଅନ୍ତର୍ଭାଵ <ruby.h>
#ଅନ୍ତର୍ଭାଵ <ruby/encoding.h>

#ଯଦି defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic କାଢ଼
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ଯଦି defined(MACOS_X)
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
#ଯଦି defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_ମୂଲ୍ୟ_Ptr
# define String_ମୂଲ୍ୟ_Ptr(s) STR2CSTR(s)
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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define ପ୍ରତୀକ_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# ଅନ୍ୟଥା
#  include <dlfcn.h>
#  define HINSTANCE ଶୂନ୍ୟ*
#  define RUBY_PROC ଶୂନ୍ୟ*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define ପ୍ରତୀକ_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ruby_initialized = 0;
ସ୍ଥିର ଶୂନ୍ୟ *ruby_ଗଦା_start;
ସ୍ଥିର ମୂଲ୍ୟ_3 objtbl;

ସ୍ଥିର ମୂଲ୍ୟ_3 mVIM;
ସ୍ଥିର ମୂଲ୍ୟ_3 cBuffer;
ସ୍ଥିର ମୂଲ୍ୟ_3 cVimWindow;
ସ୍ଥିର ମୂଲ୍ୟ_3 eDeletedBufferError;
ସ୍ଥିର ମୂଲ୍ୟ_3 eDeletedWindowError;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ensure_ruby_initialized(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ error_print(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ ruby_io_init(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ruby_vim_init(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ruby_convert_to_vim_ମୂଲ୍ୟ(ମୂଲ୍ୟ_3 val, typval_T *rettv);

#ଯଦି defined(__ia64) && !defined(ruby_init_ଗଦା)
# define ruby_init_ଗଦା(addr) ruby_init_ଗଦା((addr), rb_ia64_bsp())
#endif

#ଯଦି defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ଯଦି RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ଯଦି RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  ଯଦି RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  ଅନ୍ୟଥା
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# ଅନ୍ୟଥା
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
# ଯଦି RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# ଅନ୍ୟଥା
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

# ଯଦି VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ଯଦି RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# ଯଦି RUBY_VERSION < 20
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
# ଅନ୍ୟଥା
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_ମୂଲ୍ୟ		dll_rb_string_ମୂଲ୍ୟ
# define rb_string_ମୂଲ୍ୟ_ptr		dll_rb_string_ମୂଲ୍ୟ_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ଠେଲ			dll_rb_ary_ଠେଲ
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ଗଦା
#  define ruby_init_ଗଦା(addr)	dll_ruby_init_ଗଦା((addr), rb_ia64_bsp())
# ଅନ୍ୟଥା
#  define ruby_init_ଗଦା		dll_ruby_init_ଗଦା
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
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_assoc_new) (ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3);
ମୂଲ୍ୟ_3 *dll_rb_cFalseClass;
ମୂଲ୍ୟ_3 *dll_rb_cFixnum;
# ଯଦି defined(USE_RUBY_INTEGER)
ମୂଲ୍ୟ_3 *dll_rb_cInteger;
# endif
# ଯଦି RUBY_VERSION >= 20
ମୂଲ୍ୟ_3 *dll_rb_cFloat;
# endif
ମୂଲ୍ୟ_3 *dll_rb_cNilClass;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_cObject;
ମୂଲ୍ୟ_3 *dll_rb_cString;
ମୂଲ୍ୟ_3 *dll_rb_c_ପ୍ରତୀକ;
ମୂଲ୍ୟ_3 *dll_rb_cTrueClass;
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_class_new_instance) (ପୂର୍ଣ୍ଣସଂଖ୍ୟା,ମୂଲ୍ୟ_3*,ମୂଲ୍ୟ_3);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_check_type) (ମୂଲ୍ୟ_3,ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
# ifdef USE_TYPEDDATA
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_rb_check_typeddata) (ମୂଲ୍ୟ_3,ଅଚଳ rb_data_type_t *);
# endif
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_class_path) (ମୂଲ୍ୟ_3);
# ifdef USE_TYPEDDATA
#  ଯଦି RUBY_VERSION >= 23
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_data_typed_object_wrap) (ମୂଲ୍ୟ_3, ଶୂନ୍ୟ*, ଅଚଳ rb_data_type_t *);
#  ଅନ୍ୟଥା
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_data_typed_object_alloc) (ମୂଲ୍ୟ_3, ଶୂନ୍ୟ*, ଅଚଳ rb_data_type_t *);
#  endif
# ଅନ୍ୟଥା
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_data_object_alloc) (ମୂଲ୍ୟ_3, ଶୂନ୍ୟ*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# ଯଦି RUBY_VERSION >= 31
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_debug_rstring_null_ptr) (ଅଚଳ ଅକ୍ଷର*);
# endif
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_define_class_under) (ମୂଲ୍ୟ_3, ଅଚଳ ଅକ୍ଷର*, ମୂଲ୍ୟ_3);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_const) (ମୂଲ୍ୟ_3,ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_global_function) (ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3(*)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା,ମୂଲ୍ୟ_3*,ମୂଲ୍ୟ_3),ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_method) (ମୂଲ୍ୟ_3,ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3(*)(ANYARGS),ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_define_module) (ଅଚଳ ଅକ୍ଷର*);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_module_function) (ମୂଲ୍ୟ_3,ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3(*)(ମୂଲ୍ୟ_3,ମୂଲ୍ୟ_3),ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_singleton_method) (ମୂଲ୍ୟ_3,ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3(*)(ANYARGS),ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_define_virtual_variable) (ଅଚଳ ଅକ୍ଷର*,ମୂଲ୍ୟ_3(*)(ID,ମୂଲ୍ୟ_3*),ଶୂନ୍ୟ(*)());
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_stdout;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_stderr;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_eArgError;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_eIndexError;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_eRuntimeError;
ସ୍ଥିର ମୂଲ୍ୟ_3 *dll_rb_eStandardError;
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_eval_string_protect) (ଅଚଳ ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା*);
# ଯଦି RUBY_VERSION >= 21
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_funcallv) (ମୂଲ୍ୟ_3, ID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ମୂଲ୍ୟ_3*);
# ଅନ୍ୟଥା
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_funcall2) (ମୂଲ୍ୟ_3, ID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ମୂଲ୍ୟ_3*);
# endif
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_global_variable) (ମୂଲ୍ୟ_3*);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_hash_aset) (ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_hash_foreach) (ମୂଲ୍ୟ_3, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3), ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_hash_new) (ଶୂନ୍ୟ);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_inspect) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_int2inum) (long);
ସ୍ଥିର ID (*dll_rb_intern) (ଅଚଳ ଅକ୍ଷର*);
# ଯଦି RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
ସ୍ଥିର long (*dll_rb_fix2int) (ମୂଲ୍ୟ_3);
ସ୍ଥିର long (*dll_rb_num2int) (ମୂଲ୍ୟ_3);
ସ୍ଥିର unsigned long (*dll_rb_num2uint) (ମୂଲ୍ୟ_3);
# endif
ସ୍ଥିର ଦ୍ୱିଗୁଣ (*dll_rb_num2dbl) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_lastline_get) (ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_lastline_set) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_protect) (ମୂଲ୍ୟ_3 (*)(ମୂଲ୍ୟ_3), ମୂଲ୍ୟ_3, ପୂର୍ଣ୍ଣସଂଖ୍ୟା*);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_load) (ମୂଲ୍ୟ_3, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର long (*dll_rb_num2long) (ମୂଲ୍ୟ_3);
ସ୍ଥିର unsigned long (*dll_rb_num2ulong) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_obj_alloc) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_obj_as_string) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_obj_id) (ମୂଲ୍ୟ_3);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_raise) (ମୂଲ୍ୟ_3, ଅଚଳ ଅକ୍ଷର*, ...);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_string_ମୂଲ୍ୟ) (ଅସ୍ଥିର ମୂଲ୍ୟ_3*);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_str_cat) (ମୂଲ୍ୟ_3, ଅଚଳ ଅକ୍ଷର*, long);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_str_concat) (ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_str_new) (ଅଚଳ ଅକ୍ଷର*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_str_new_cstr) (ଅଚଳ ଅକ୍ଷର*);
# ଅନ୍ୟଥା
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_str_new2) (ଅଚଳ ଅକ୍ଷର*);
# endif
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_errinfo) (ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_init) (ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_init_loadpath) (ଶୂନ୍ୟ);
# ifdef MSWIN
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_sysinit) (ପୂର୍ଣ୍ଣସଂଖ୍ୟା*, ଅକ୍ଷର***);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_rb_w32_snprintf)(ଅକ୍ଷର*, size_t, ଅଚଳ ଅକ୍ଷର*, ...);
# endif
# ଯଦି RUBY_VERSION >= 31
#  ifdef _MSC_VER
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_unexpected_type) (ମୂଲ୍ୟ_3, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
#  ଅନ୍ୟଥା
NORETURN(ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_unexpected_type) (ମୂଲ୍ୟ_3, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
#  endif
# endif
ସ୍ଥିର ଅକ୍ଷର * (*dll_rb_string_ମୂଲ୍ୟ_ptr) (ଅସ୍ଥିର ମୂଲ୍ୟ_3*);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_float_new) (ଦ୍ୱିଗୁଣ);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_ary_new) (ଶୂନ୍ୟ);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_ary_new4) (long n, ଅଚଳ ମୂଲ୍ୟ_3 *elts);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_ary_ଠେଲ) (ମୂଲ୍ୟ_3, ମୂଲ୍ୟ_3);
# ଯଦି RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_ary_detransient) (ମୂଲ୍ୟ_3);
# endif
# ifdef __ia64
ସ୍ଥିର ଶୂନ୍ୟ * (*dll_rb_ia64_bsp) (ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_init_ଗଦା)(ମୂଲ୍ୟ_3*, ଶୂନ୍ୟ*);
# ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_init_ଗଦା)(ମୂଲ୍ୟ_3*);
# endif
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_int2big)(SIGNED_ମୂଲ୍ୟ);

ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_script) (ଅଚଳ ଅକ୍ଷର*);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_rb_enc_find_index) (ଅଚଳ ଅକ୍ଷର*);
ସ୍ଥିର rb_encoding* (*dll_rb_enc_find) (ଅଚଳ ଅକ୍ଷର*);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_enc_str_new) (ଅଚଳ ଅକ୍ଷର*, long, rb_encoding*);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_sprintf) (ଅଚଳ ଅକ୍ଷର*, ...);
ସ୍ଥିର ମୂଲ୍ୟ_3 (*dll_rb_require) (ଅଚଳ ଅକ୍ଷର*);
ସ୍ଥିର ଶୂନ୍ୟ* (*dll_ruby_options)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅକ୍ଷର**);

# ଯଦି defined(USE_RGENGC) && USE_RGENGC
#  ଯଦି RUBY_VERSION == 21
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_gc_writebarrier_unprotect_promoted)(ମୂଲ୍ୟ_3);
#  ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ (*dll_rb_gc_writebarrier_unprotect)(ମୂଲ୍ୟ_3 obj);
#  endif
# endif

# ଯଦି RUBY_VERSION >= 30
#  ifdef _MSC_VER
ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  ଅନ୍ୟଥା
NORETURN(ସ୍ଥିର ଶୂନ୍ୟ (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# ଯଦି RUBY_VERSION >= 26 && RUBY_VERSION <= 32
ଶୂନ୍ୟ rb_ary_detransient_stub(ମୂଲ୍ୟ_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ଯଦି RUBY_VERSION >= 22
    long
rb_num2long_stub(ମୂଲ୍ୟ_3 x)
#  ଅନ୍ୟଥା
    SIGNED_ମୂଲ୍ୟ
rb_num2long_stub(ମୂଲ୍ୟ_3 x)
#  endif
{
    ଫେରନ୍ତୁ dll_rb_num2long(x);
}
#  ଯଦି RUBY_VERSION >= 26
    ମୂଲ୍ୟ_3
rb_int2big_stub(intptr_t x)
#  ଅନ୍ୟଥା
    ମୂଲ୍ୟ_3
rb_int2big_stub(SIGNED_ମୂଲ୍ୟ x)
#  endif
{
    ଫେରନ୍ତୁ dll_rb_int2big(x);
}
#  ଯଦି RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(ମୂଲ୍ୟ_3 x)
{
    ଫେରନ୍ତୁ dll_rb_fix2int(x);
}
    long
rb_num2int_stub(ମୂଲ୍ୟ_3 x)
{
    ଫେରନ୍ତୁ dll_rb_num2int(x);
}
#  endif
#  ଯଦି RUBY_VERSION >= 20
    ମୂଲ୍ୟ_3
rb_float_new_in_heap(ଦ୍ୱିଗୁଣ d)
{
    ଫେରନ୍ତୁ dll_rb_float_new(d);
}
#   ଯଦି RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(ମୂଲ୍ୟ_3 x)
#   ଅନ୍ୟଥା
    ମୂଲ୍ୟ_3
rb_num2ulong(ମୂଲ୍ୟ_3 x)
#   endif
{
    ଫେରନ୍ତୁ (long)RSHIFT((SIGNED_ମୂଲ୍ୟ)(x),1);
}
#  endif
#  ଯଦି defined(USE_RGENGC) && USE_RGENGC
#   ଯଦି RUBY_VERSION == 21
    ଶୂନ୍ୟ
rb_gc_writebarrier_unprotect_promoted_stub(ମୂଲ୍ୟ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   ଅନ୍ୟଥା
    ଶୂନ୍ୟ
rb_gc_writebarrier_unprotect_stub(ମୂଲ୍ୟ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ଯଦି RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    ଶୂନ୍ୟ
rb_ary_detransient_stub(ମୂଲ୍ୟ_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ଯଦି RUBY_VERSION >= 30
    ଶୂନ୍ୟ
rb_check_type_stub(ମୂଲ୍ୟ_3 obj, ପୂର୍ଣ୍ଣସଂଖ୍ୟା t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(ମୂଲ୍ୟ_3 x)
{
    ଫେରନ୍ତୁ dll_rb_num2uint(x);
}
    ଶୂନ୍ୟ
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  ଯଦି RUBY_VERSION >= 31
    ଶୂନ୍ୟ
rb_debug_rstring_null_ptr_stub(ଅଚଳ ଅକ୍ଷର *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    ଶୂନ୍ୟ
rb_unexpected_type_stub(ମୂଲ୍ୟ_3 self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
ଶୂନ୍ୟ *rb_check_typeddata_stub(ମୂଲ୍ୟ_3 obj, ଅଚଳ rb_data_type_t *data_type)
{
    ଫେରନ୍ତୁ dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

ସ୍ଥିର HINSTANCE hinstRuby = ଖାଲି; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
ସ୍ଥିର ସଂରଚନା
{
    ଅକ୍ଷର *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# ଯଦି defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# ଅନ୍ୟଥା
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ଯଦି RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_ପ୍ରତୀକ},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  ଯଦି RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  ଅନ୍ୟଥା
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# ଅନ୍ୟଥା
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# ଯଦି RUBY_VERSION >= 31
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
# ଯଦି RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# ଅନ୍ୟଥା
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ଯଦି RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_ମୂଲ୍ୟ},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# ଅନ୍ୟଥା
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ଯଦି RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_ମୂଲ୍ୟ_ptr},
# ଯଦି RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# ଅନ୍ୟଥା
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# ଅନ୍ୟଥା
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ଠେଲ},
# ଯଦି RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ଗଦା},
# ଯଦି defined(USE_RGENGC) && USE_RGENGC
#  ଯଦି RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  ଅନ୍ୟଥା
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ଯଦି RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", ଖାଲି},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ruby_runtime_link_init(ଅକ୍ଷର *libname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (hinstRuby)
	ଫେରନ୍ତୁ OK;
    hinstRuby = load_dll(libname);
    ଯଦି (!hinstRuby)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	ଯଦି (!(*ruby_funcname_table[i].ptr = ପ୍ରତୀକ_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ଖାଲି;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ruby_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ ruby_runtime_link_init((ଅକ୍ଷର *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    ଶୂନ୍ୟ
ruby_end(ଶୂନ୍ୟ)
{
}

    ଶୂନ୍ୟ
ex_ruby(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା state;
    ଅକ୍ଷର *script = ଖାଲି;

    script = (ଅକ୍ଷର *)script_get(eap, eap->arg);
    ଯଦି (!eap->skip && ensure_ruby_initialized())
    {
	ଯଦି (script == ଖାଲି)
	    rb_eval_string_protect((ଅକ୍ଷର *)eap->arg, &state);
	ଅନ୍ୟଥା
	    rb_eval_string_protect(script, &state);
	ଯଦି (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_str2rb_enc_str(ଅଚଳ ଅକ୍ଷର *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    ଯଦି (get_option_ମୂଲ୍ୟ((char_u *)"enc", &lval, &sval, ଖାଲି, 0) == gov_string)
    {
	enc = rb_enc_find((ଅକ୍ଷର *)sval);
	vim_free(sval);
	ଯଦି (enc)
	    ଫେରନ୍ତୁ rb_enc_str_new(s, (long)strlen(s), enc);
    }
    ଫେରନ୍ତୁ rb_str_new2(s);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
eval_enc_string_protect(ଅଚଳ ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    ମୂଲ୍ୟ_3 v;

    ଯଦି (get_option_ମୂଲ୍ୟ((char_u *)"enc", &lval, &sval, ଖାଲି, 0) == gov_string)
    {
	enc = rb_enc_find((ଅକ୍ଷର *)sval);
	vim_free(sval);
	ଯଦି (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    ଫେରନ୍ତୁ rb_eval_string_protect(String_ମୂଲ୍ୟ_Ptr(v), state);
	}
    }
    ଫେରନ୍ତୁ rb_eval_string_protect(str, state);
}

    ଶୂନ୍ୟ
ex_rubydo(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ଯଦି (!ensure_ruby_initialized())
	ଫେରନ୍ତୁ;

    ଯଦି (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	ଫେରନ୍ତୁ;
    ପାଇଁ (i = eap->line1; i <= eap->line2; i++)
    {
	ମୂଲ୍ୟ_3 ଧାଡ଼ି;

	ଯଦି (i > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି = vim_str2rb_enc_str((ଅକ୍ଷର *)ml_get(i));
	rb_lastline_set(ଧାଡ଼ି);
	eval_enc_string_protect((ଅକ୍ଷର *) eap->arg, &state);
	ଯଦି (state)
	{
	    error_print(state);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (was_curbuf != curbuf || i > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି = rb_lastline_get();
	ଯଦି (!NIL_P(ଧାଡ଼ି))
	{
	    ଯଦି (TYPE(ଧାଡ଼ି) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		ଫେରନ୍ତୁ;
	    }
	    ml_replace(i, (char_u *) String_ମୂଲ୍ୟ_Ptr(ଧାଡ଼ି), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
rb_load_wrap(ମୂଲ୍ୟ_3 file_to_load)
{
    rb_load(file_to_load, 0);
    ଫେରନ୍ତୁ Qnil;
}

    ଶୂନ୍ୟ
ex_rubyfile(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା state;

    ଯଦି (!ensure_ruby_initialized())
	ଫେରନ୍ତୁ;

    ମୂଲ୍ୟ_3 file_to_load = rb_str_new2((ଅଚଳ ଅକ୍ଷର *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    ଯଦି (state)
	error_print(state);
}

    ଶୂନ୍ୟ
ruby_buffer_free(buf_T *buf)
{
    ଯଦି (buf->b_ruby_ref == ଖାଲି)
	ଫେରନ୍ତୁ;

    rb_hash_aset(objtbl, rb_obj_id((ମୂଲ୍ୟ_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = ଖାଲି;
}

    ଶୂନ୍ୟ
ruby_window_free(win_T *win)
{
    ଯଦି (win->w_ruby_ref == ଖାଲି)
	ଫେରନ୍ତୁ;

    rb_hash_aset(objtbl, rb_obj_id((ମୂଲ୍ୟ_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ensure_ruby_initialized(ଶୂନ୍ୟ)
{
    ଯଦି (ruby_initialized)
	ଫେରନ୍ତୁ ruby_initialized;

#ifdef DYNAMIC_RUBY
    ଯଦି (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc = 1;
	ଅକ୍ଷର *argv[] = {"gvim.exe"};
	ଅକ୍ଷର **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_ଗଦା(ruby_ଗଦା_start);
	    ruby_init();
	}
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dummy_argc = 2;
	    ଅକ୍ଷର *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    ଅନ୍ୟଥା
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	ଫେରନ୍ତୁ 0;
    }
#endif

    ଫେରନ୍ତୁ ruby_initialized;
}

    ସ୍ଥିର ଶୂନ୍ୟ
error_print(ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
    ମୂଲ୍ୟ_3 error;
    ମୂଲ୍ୟ_3 eclass;
    ମୂଲ୍ୟ_3 einfo;
    ମୂଲ୍ୟ_3 bt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr;
    ଅକ୍ଷର buff[BUFSIZ];
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

    ବାଛ (state)
    {
	ସ୍ଥିତି TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TAG_RAISE:
	ସ୍ଥିତି TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    ଯଦି (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    ଅନ୍ୟଥା
	    {
		ମୂଲ୍ୟ_3 epath;
		ଅକ୍ଷର *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ଯଦି (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ଯଦି RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    ପାଇଁ (i = 0; i < RARRAY_LEN(bt); i++)
		ସନ୍ଦେଶ_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#ଅନ୍ୟଥା
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    ପାଇଁ (i = 0; i < RARRAY_LEN(bt); i++)
		ସନ୍ଦେଶ_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_message(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
    ଅକ୍ଷର *buff, *p;

    str = rb_obj_as_string(str);
    ଯଦି (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(ଅକ୍ଷର, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ଯଦି (p) *p = '\0';
	ସନ୍ଦେଶ(buff);
    }
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ("");
    }
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_set_option(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
    do_set((char_u *)String_ମୂଲ୍ୟ_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_command(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
    do_cmdline_cmd((char_u *)String_ମୂଲ୍ୟ_Ptr(str));
    ଫେରନ୍ତୁ Qnil;
}

#ifdef FEAT_EVAL
    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_to_ruby(typval_T *tv)
{
    ମୂଲ୍ୟ_3 result = Qnil;

    ଯଦି (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == ଖାଲି
					  ? "" : (ଅକ୍ଷର *)(tv->vval.v_string));
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_ସଂଖ୍ୟା)
    {
	result = INT2_ଅଙ୍କ(tv->vval.v_ସଂଖ୍ୟା);
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	ଯଦି (list != ଖାଲି)
	{
	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, curr)
		rb_ary_ଠେଲ(result, vim_to_ruby(&curr->li_tv));
	}
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	ଯଦି (tv->vval.v_dict != ଖାଲି)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
	    {
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((ଅକ୍ଷର *)hi->hi_ଚାବି),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ଯଦି (tv->vval.v_ସଂଖ୍ୟା == VVAL_TRUE)
	    result = Qtrue;
	ଅନ୍ୟଥା ଯଦି (tv->vval.v_ସଂଖ୍ୟା == VVAL_FALSE)
	    result = Qfalse;
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    ଫେରନ୍ତୁ result;
}
#endif

    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_evaluate(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    ମୂଲ୍ୟ_3       result;

    tv = eval_ପଦସମଷ୍ଟି((char_u *)String_ମୂଲ୍ୟ_Ptr(str), ଖାଲି);
    ଯଦି (tv == ଖାଲି)
	ଫେରନ୍ତୁ Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ Qnil;
#endif
}

#ifdef USE_TYPEDDATA
ସ୍ଥିର size_t buffer_dsize(ଅଚଳ ଶୂନ୍ୟ *buf);

ସ୍ଥିର ଅଚଳ rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# ଯଦି RUBY_VERSION >= 27
	0, {0}
# ଅନ୍ୟଥା
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ସ୍ଥିର size_t
buffer_dsize(ଅଚଳ ଶୂନ୍ୟ *buf UNUSED)
{
    ଫେରନ୍ତୁ ଆକାର(buf_T);
}
#endif

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_new(buf_T *buf)
{
    ଯଦି (buf->b_ruby_ref)
    {
	ଫେରନ୍ତୁ (ମୂଲ୍ୟ_3) buf->b_ruby_ref;
    }
    ଅନ୍ୟଥା
    {
#ifdef USE_TYPEDDATA
	ମୂଲ୍ୟ_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#ଅନ୍ୟଥା
	ମୂଲ୍ୟ_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (ଶୂନ୍ୟ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ଫେରନ୍ତୁ obj;
    }
}

    ସ୍ଥିର buf_T *
get_buf(ମୂଲ୍ୟ_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#ଅନ୍ୟଥା
    Data_Get_Struct(obj, buf_T, buf);
#endif
    ଯଦି (buf == ଖାଲି)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    ଫେରନ୍ତୁ buf;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
vim_blob(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
    ମୂଲ୍ୟ_3 result = rb_str_new("0z", 2);
    ଅକ୍ଷର    buf[4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପାଇଁ (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned ଅକ୍ଷର)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_s_current(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ buffer_new(curbuf);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_s_current_getter(ID id UNUSED, ମୂଲ୍ୟ_3 *x UNUSED)
{
    ଫେରନ୍ତୁ buffer_new(curbuf);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_s_ଗଣନା(ମୂଲ୍ୟ_3 self UNUSED)
{
    buf_T *b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ଯଦି (b->b_p_bl)
	    n++;
    }

    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(n);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_s_aref(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 ଅଙ୍କ)
{
    buf_T *b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = NUM2INT(ଅଙ୍କ);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ଯଦି (!b->b_p_bl)
	    ଜାରି;

	ଯଦି (n == 0)
	    ଫେରନ୍ତୁ buffer_new(b);

	n--;
    }
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_name(ମୂଲ୍ୟ_3 self)
{
    buf_T *buf = get_buf(self);

    ଫେରନ୍ତୁ buf->b_ffname ? rb_str_new2((ଅକ୍ଷର *)buf->b_ffname) : Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_ସଂଖ୍ୟା(ମୂଲ୍ୟ_3 self)
{
    buf_T *buf = get_buf(self);

    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(buf->b_fnum);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_ଗଣନା(ମୂଲ୍ୟ_3 self)
{
    buf_T *buf = get_buf(self);

    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
get_buffer_ଧାଡ଼ି(buf_T *buf, linenr_T n)
{
    ଯଦି (n <= 0 || n > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    ଫେରନ୍ତୁ vim_str2rb_enc_str((ଅକ୍ଷର *)ml_get_buf(buf, n, FALSE));
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_aref(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 ଅଙ୍କ)
{
    buf_T *buf = get_buf(self);

    ଯଦି (buf != ଖାଲି)
	ଫେରନ୍ତୁ get_buffer_ଧାଡ଼ି(buf, (linenr_T)NUM2LONG(ଅଙ୍କ));
    ଫେରନ୍ତୁ Qnil; // For stop warning
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
set_buffer_ଧାଡ଼ି(buf_T *buf, linenr_T n, ମୂଲ୍ୟ_3 str)
{
    ଅକ୍ଷର	*ଧାଡ଼ି = String_ମୂଲ୍ୟ_Ptr(str);
    aco_save_T	aco;

    ଯଦି (n > 0 && n <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା && ଧାଡ଼ି != ଖାଲି)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ଯଦି (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)ଧାଡ଼ି, TRUE);
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
    ଅନ୍ୟଥା
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    ଫେରନ୍ତୁ str;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_aset(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 ଅଙ୍କ, ମୂଲ୍ୟ_3 str)
{
    buf_T *buf = get_buf(self);

    ଯଦି (buf != ଖାଲି)
	ଫେରନ୍ତୁ set_buffer_ଧାଡ଼ି(buf, (linenr_T)NUM2LONG(ଅଙ୍କ), str);
    ଫେରନ୍ତୁ str;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_delete(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 ଅଙ୍କ)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(ଅଙ୍କ);
    aco_save_T	aco;

    ଯଦି (n > 0 && n <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ଯଦି (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ଅନ୍ୟଥା
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
buffer_append(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 ଅଙ୍କ, ମୂଲ୍ୟ_3 str)
{
    buf_T	*buf = get_buf(self);
    ଅକ୍ଷର	*ଧାଡ଼ି = String_ମୂଲ୍ୟ_Ptr(str);
    long	n = NUM2LONG(ଅଙ୍କ);
    aco_save_T	aco;

    ଯଦି (ଧାଡ଼ି == ଖାଲି)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    ଅନ୍ୟଥା ଯଦି (n >= 0 && n <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ଯଦି (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) ଧାଡ଼ି, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    ଅନ୍ୟଥା
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    ଫେରନ୍ତୁ str;
}

#ifdef USE_TYPEDDATA
ସ୍ଥିର size_t window_dsize(ଅଚଳ ଶୂନ୍ୟ *buf);

ସ୍ଥିର ଅଚଳ rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# ଯଦି RUBY_VERSION >= 27
	0, {0}
# ଅନ୍ୟଥା
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    ସ୍ଥିର size_t
window_dsize(ଅଚଳ ଶୂନ୍ୟ *win UNUSED)
{
    ଫେରନ୍ତୁ ଆକାର(win_T);
}
#endif

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_new(win_T *win)
{
    ଯଦି (win->w_ruby_ref)
    {
	ଫେରନ୍ତୁ (ମୂଲ୍ୟ_3) win->w_ruby_ref;
    }
    ଅନ୍ୟଥା
    {
#ifdef USE_TYPEDDATA
	ମୂଲ୍ୟ_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#ଅନ୍ୟଥା
	ମୂଲ୍ୟ_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (ଶୂନ୍ୟ *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	ଫେରନ୍ତୁ obj;
    }
}

    ସ୍ଥିର win_T *
get_win(ମୂଲ୍ୟ_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#ଅନ୍ୟଥା
    Data_Get_Struct(obj, win_T, win);
#endif
    ଯଦି (win == ଖାଲି)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    ଫେରନ୍ତୁ win;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_s_current(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ window_new(curwin);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_s_current_getter(ID id UNUSED, ମୂଲ୍ୟ_3 *x UNUSED)
{
    ଫେରନ୍ତୁ window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    ସ୍ଥିର ମୂଲ୍ୟ_3
ଧାଡ଼ି_s_current(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ get_buffer_ଧାଡ଼ି(curbuf, curwin->w_cursor.lnum);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
set_current_ଧାଡ଼ି(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 str)
{
    ଫେରନ୍ତୁ set_buffer_ଧାଡ଼ି(curbuf, curwin->w_cursor.lnum, str);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
current_ଧାଡ଼ି_ସଂଖ୍ୟା(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ INT2FIX((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.lnum);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_s_ଗଣନା(ମୂଲ୍ୟ_3 self UNUSED)
{
    win_T	*w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(n);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_s_aref(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 ଅଙ୍କ)
{
    win_T *w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = NUM2INT(ଅଙ୍କ);

    ପାଇଁ (w = firstwin; w != ଖାଲି; w = w->w_next, --n)
	ଯଦି (n == 0)
	    ଫେରନ୍ତୁ window_new(w);
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_buffer(ମୂଲ୍ୟ_3 self)
{
    win_T *win = get_win(self);

    ଫେରନ୍ତୁ buffer_new(win->w_buffer);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_height(ମୂଲ୍ୟ_3 self)
{
    win_T *win = get_win(self);

    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(win->w_height);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_set_height(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    ଫେରନ୍ତୁ height;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_width(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ INT2_ଅଙ୍କ(get_win(self)->w_width);
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_set_width(ମୂଲ୍ୟ_3 self UNUSED, ମୂଲ୍ୟ_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    ଫେରନ୍ତୁ width;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_cursor(ମୂଲ୍ୟ_3 self)
{
    win_T *win = get_win(self);

    ଫେରନ୍ତୁ rb_assoc_new(INT2_ଅଙ୍କ(win->w_cursor.lnum), INT2_ଅଙ୍କ(win->w_cursor.col));
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
window_set_cursor(ମୂଲ୍ୟ_3 self, ମୂଲ୍ୟ_3 pos)
{
    ମୂଲ୍ୟ_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ଯଦି (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
f_nop(ମୂଲ୍ୟ_3 self UNUSED)
{
    ଫେରନ୍ତୁ Qnil;
}

    ସ୍ଥିର ମୂଲ୍ୟ_3
f_p(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ମୂଲ୍ୟ_3 *argv, ମୂଲ୍ୟ_3 self UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ମୂଲ୍ୟ_3 str = rb_str_new("", 0);
    ମୂଲ୍ୟ_3 ret = Qnil;

    ପାଇଁ (i = 0; i < argc; i++)
    {
	ଯଦି (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    ସନ୍ଦେଶ(RSTRING_PTR(str));

    ଯଦି (argc == 1)
	ret = argv[0];
    ଅନ୍ୟଥା ଯଦି (argc > 1)
	ret = rb_ary_new4(argc, argv);
    ଫେରନ୍ତୁ ret;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ruby_io_init(ଶୂନ୍ୟ)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN ମୂଲ୍ୟ_3 rb_stdout;
    RUBYEXTERN ମୂଲ୍ୟ_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (ଶୂନ୍ୟ*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (ଶୂନ୍ୟ*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (ଶୂନ୍ୟ*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (ଶୂନ୍ୟ*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ruby_vim_init(ଶୂନ୍ୟ)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_ଅଙ୍କ(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_ଅଙ୍କ(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_ଅଙ୍କ(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_ଅଙ୍କ(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (ଶୂନ୍ୟ*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (ଶୂନ୍ୟ*)buffer_s_ଗଣନା, 0);
    rb_define_singleton_method(cBuffer, "[]", (ଶୂନ୍ୟ*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (ଶୂନ୍ୟ*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (ଶୂନ୍ୟ*)buffer_ସଂଖ୍ୟା, 0);
    rb_define_method(cBuffer, "count", (ଶୂନ୍ୟ*)buffer_ଗଣନା, 0);
    rb_define_method(cBuffer, "length", (ଶୂନ୍ୟ*)buffer_ଗଣନା, 0);
    rb_define_method(cBuffer, "[]", (ଶୂନ୍ୟ*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (ଶୂନ୍ୟ*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (ଶୂନ୍ୟ*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (ଶୂନ୍ୟ*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (ଶୂନ୍ୟ*)current_ଧାଡ଼ି_ସଂଖ୍ୟା, 0);
    rb_define_method(cBuffer, "line", (ଶୂନ୍ୟ*)ଧାଡ଼ି_s_current, 0);
    rb_define_method(cBuffer, "line=", (ଶୂନ୍ୟ*)set_current_ଧାଡ଼ି, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (ଶୂନ୍ୟ*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (ଶୂନ୍ୟ*)window_s_ଗଣନା, 0);
    rb_define_singleton_method(cVimWindow, "[]", (ଶୂନ୍ୟ*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (ଶୂନ୍ୟ*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (ଶୂନ୍ୟ*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (ଶୂନ୍ୟ*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (ଶୂନ୍ୟ*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (ଶୂନ୍ୟ*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (ଶୂନ୍ୟ*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (ଶୂନ୍ୟ*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    ଶୂନ୍ୟ
vim_ruby_init(ଶୂନ୍ୟ *ଗଦା_start)
{
    // should get machine stack start address early in main function
    ruby_ଗଦା_start = ଗଦା_start;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
convert_hash2dict(ମୂଲ୍ୟ_3 ଚାବି, ମୂଲ୍ୟ_3 val, ମୂଲ୍ୟ_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ଚାବି)));
    ଯଦି (di == ଖାଲି || ruby_convert_to_vim_ମୂଲ୍ୟ(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	ଫେରନ୍ତୁ ST_STOP;
    }
    ଫେରନ୍ତୁ ST_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ruby_convert_to_vim_ମୂଲ୍ୟ(ମୂଲ୍ୟ_3 val, typval_T *rettv)
{
    ବାଛ (TYPE(val))
    {
	ସ୍ଥିତି T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_BIGNUM:
	ସ୍ଥିତି T_FIXNUM:
	    rettv->v_type = VAR_ସଂଖ୍ୟା;
	    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)NUM2LONG(val);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	ସ୍ଥିତି T_STRING:
	    {
		ମୂଲ୍ୟ_3 str = (ମୂଲ୍ୟ_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		ଯଦି (l == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;

		ପାଇଁ (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ଯଦି (ruby_convert_to_vim_ମୂଲ୍ୟ((ମୂଲ୍ୟ_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			ଫେରନ୍ତୁ FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		ଯଦି (d == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;

		rb_hash_foreach(val, convert_hash2dict, (ମୂଲ୍ୟ_3)d);
		ଯଦି (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    ଫେରନ୍ତୁ FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
do_rubyeval(char_u *str, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = FAIL;

    ଯଦି (ensure_ruby_initialized())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା state;
	ମୂଲ୍ୟ_3 obj;

	obj = rb_eval_string_protect((ଅଚଳ ଅକ୍ଷର *)str, &state);
	ଯଦି (state)
	    error_print(state);
	ଅନ୍ୟଥା
	    retval = ruby_convert_to_vim_ମୂଲ୍ୟ(obj, rettv);
    }
    ଯଦି (retval == FAIL)
    {
	rettv->v_type = VAR_ସଂଖ୍ୟା;
	rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
}
