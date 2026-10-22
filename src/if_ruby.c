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

#અન્તર્ભાવ "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN બાહ્ય
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# જો RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# જો defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# જો RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_પ્રતીક		(*dll_rb_c_પ્રતીક)
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

# જો RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# જો RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# જો RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# જો RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# જો RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# જો RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# જો RUBY_VERSION >= 31
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

#જો defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ધકેલો
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#જો defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#અન્તર્ભાવ <ruby.h>
#અન્તર્ભાવ <ruby/encoding.h>

#જો defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic કાઢો
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#જો defined(MACOS_X)
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
#જો defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_કિંમત_Ptr
# define String_કિંમત_Ptr(s) STR2CSTR(s)
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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define પ્રતીક_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# નહીંતર
#  include <dlfcn.h>
#  define HINSTANCE શૂન્ય*
#  define RUBY_PROC શૂન્ય*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define પ્રતીક_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

સ્થિર પૂર્ણાંક ruby_initialized = 0;
સ્થિર શૂન્ય *ruby_ઢગલો_start;
સ્થિર કિંમત_3 objtbl;

સ્થિર કિંમત_3 mVIM;
સ્થિર કિંમત_3 cBuffer;
સ્થિર કિંમત_3 cVimWindow;
સ્થિર કિંમત_3 eDeletedBufferError;
સ્થિર કિંમત_3 eDeletedWindowError;

સ્થિર પૂર્ણાંક ensure_ruby_initialized(શૂન્ય);
સ્થિર શૂન્ય error_print(પૂર્ણાંક);
સ્થિર શૂન્ય ruby_io_init(શૂન્ય);
સ્થિર શૂન્ય ruby_vim_init(શૂન્ય);
સ્થિર પૂર્ણાંક ruby_convert_to_vim_કિંમત(કિંમત_3 val, typval_T *rettv);

#જો defined(__ia64) && !defined(ruby_init_ઢગલો)
# define ruby_init_ઢગલો(addr) ruby_init_ઢગલો((addr), rb_ia64_bsp())
#endif

#જો defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# જો RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# જો RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  જો RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  નહીંતર
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# નહીંતર
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
# જો RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# નહીંતર
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

# જો VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  જો RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# જો RUBY_VERSION < 20
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
# નહીંતર
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_કિંમત		dll_rb_string_કિંમત
# define rb_string_કિંમત_ptr		dll_rb_string_કિંમત_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ધકેલો			dll_rb_ary_ધકેલો
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_ઢગલો
#  define ruby_init_ઢગલો(addr)	dll_ruby_init_ઢગલો((addr), rb_ia64_bsp())
# નહીંતર
#  define ruby_init_ઢગલો		dll_ruby_init_ઢગલો
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
સ્થિર કિંમત_3 (*dll_rb_assoc_new) (કિંમત_3, કિંમત_3);
કિંમત_3 *dll_rb_cFalseClass;
કિંમત_3 *dll_rb_cFixnum;
# જો defined(USE_RUBY_INTEGER)
કિંમત_3 *dll_rb_cInteger;
# endif
# જો RUBY_VERSION >= 20
કિંમત_3 *dll_rb_cFloat;
# endif
કિંમત_3 *dll_rb_cNilClass;
સ્થિર કિંમત_3 *dll_rb_cObject;
કિંમત_3 *dll_rb_cString;
કિંમત_3 *dll_rb_c_પ્રતીક;
કિંમત_3 *dll_rb_cTrueClass;
સ્થિર કિંમત_3 (*dll_rb_class_new_instance) (પૂર્ણાંક,કિંમત_3*,કિંમત_3);
સ્થિર શૂન્ય (*dll_rb_check_type) (કિંમત_3,પૂર્ણાંક);
# ifdef USE_TYPEDDATA
સ્થિર શૂન્ય *(*dll_rb_check_typeddata) (કિંમત_3,અચલ rb_data_type_t *);
# endif
સ્થિર કિંમત_3 (*dll_rb_class_path) (કિંમત_3);
# ifdef USE_TYPEDDATA
#  જો RUBY_VERSION >= 23
સ્થિર કિંમત_3 (*dll_rb_data_typed_object_wrap) (કિંમત_3, શૂન્ય*, અચલ rb_data_type_t *);
#  નહીંતર
સ્થિર કિંમત_3 (*dll_rb_data_typed_object_alloc) (કિંમત_3, શૂન્ય*, અચલ rb_data_type_t *);
#  endif
# નહીંતર
સ્થિર કિંમત_3 (*dll_rb_data_object_alloc) (કિંમત_3, શૂન્ય*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# જો RUBY_VERSION >= 31
સ્થિર શૂન્ય (*dll_rb_debug_rstring_null_ptr) (અચલ અક્ષર*);
# endif
સ્થિર કિંમત_3 (*dll_rb_define_class_under) (કિંમત_3, અચલ અક્ષર*, કિંમત_3);
સ્થિર શૂન્ય (*dll_rb_define_const) (કિંમત_3,અચલ અક્ષર*,કિંમત_3);
સ્થિર શૂન્ય (*dll_rb_define_global_function) (અચલ અક્ષર*,કિંમત_3(*)(પૂર્ણાંક,કિંમત_3*,કિંમત_3),પૂર્ણાંક);
સ્થિર શૂન્ય (*dll_rb_define_method) (કિંમત_3,અચલ અક્ષર*,કિંમત_3(*)(ANYARGS),પૂર્ણાંક);
સ્થિર કિંમત_3 (*dll_rb_define_module) (અચલ અક્ષર*);
સ્થિર શૂન્ય (*dll_rb_define_module_function) (કિંમત_3,અચલ અક્ષર*,કિંમત_3(*)(કિંમત_3,કિંમત_3),પૂર્ણાંક);
સ્થિર શૂન્ય (*dll_rb_define_singleton_method) (કિંમત_3,અચલ અક્ષર*,કિંમત_3(*)(ANYARGS),પૂર્ણાંક);
સ્થિર શૂન્ય (*dll_rb_define_virtual_variable) (અચલ અક્ષર*,કિંમત_3(*)(ID,કિંમત_3*),શૂન્ય(*)());
સ્થિર કિંમત_3 *dll_rb_stdout;
સ્થિર કિંમત_3 *dll_rb_stderr;
સ્થિર કિંમત_3 *dll_rb_eArgError;
સ્થિર કિંમત_3 *dll_rb_eIndexError;
સ્થિર કિંમત_3 *dll_rb_eRuntimeError;
સ્થિર કિંમત_3 *dll_rb_eStandardError;
સ્થિર કિંમત_3 (*dll_rb_eval_string_protect) (અચલ અક્ષર*, પૂર્ણાંક*);
# જો RUBY_VERSION >= 21
સ્થિર કિંમત_3 (*dll_rb_funcallv) (કિંમત_3, ID, પૂર્ણાંક, અચલ કિંમત_3*);
# નહીંતર
સ્થિર કિંમત_3 (*dll_rb_funcall2) (કિંમત_3, ID, પૂર્ણાંક, અચલ કિંમત_3*);
# endif
સ્થિર શૂન્ય (*dll_rb_global_variable) (કિંમત_3*);
સ્થિર કિંમત_3 (*dll_rb_hash_aset) (કિંમત_3, કિંમત_3, કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_hash_foreach) (કિંમત_3, પૂર્ણાંક (*)(કિંમત_3, કિંમત_3, કિંમત_3), કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_hash_new) (શૂન્ય);
સ્થિર કિંમત_3 (*dll_rb_inspect) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_int2inum) (long);
સ્થિર ID (*dll_rb_intern) (અચલ અક્ષર*);
# જો RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
સ્થિર long (*dll_rb_fix2int) (કિંમત_3);
સ્થિર long (*dll_rb_num2int) (કિંમત_3);
સ્થિર unsigned long (*dll_rb_num2uint) (કિંમત_3);
# endif
સ્થિર દ્વિગુણ (*dll_rb_num2dbl) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_lastline_get) (શૂન્ય);
સ્થિર શૂન્ય (*dll_rb_lastline_set) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_protect) (કિંમત_3 (*)(કિંમત_3), કિંમત_3, પૂર્ણાંક*);
સ્થિર શૂન્ય (*dll_rb_load) (કિંમત_3, પૂર્ણાંક);
સ્થિર long (*dll_rb_num2long) (કિંમત_3);
સ્થિર unsigned long (*dll_rb_num2ulong) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_obj_alloc) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_obj_as_string) (કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_obj_id) (કિંમત_3);
સ્થિર શૂન્ય (*dll_rb_raise) (કિંમત_3, અચલ અક્ષર*, ...);
સ્થિર કિંમત_3 (*dll_rb_string_કિંમત) (અસ્થિર કિંમત_3*);
સ્થિર કિંમત_3 (*dll_rb_str_cat) (કિંમત_3, અચલ અક્ષર*, long);
સ્થિર કિંમત_3 (*dll_rb_str_concat) (કિંમત_3, કિંમત_3);
સ્થિર કિંમત_3 (*dll_rb_str_new) (અચલ અક્ષર*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
સ્થિર કિંમત_3 (*dll_rb_str_new_cstr) (અચલ અક્ષર*);
# નહીંતર
સ્થિર કિંમત_3 (*dll_rb_str_new2) (અચલ અક્ષર*);
# endif
સ્થિર કિંમત_3 (*dll_rb_errinfo) (શૂન્ય);
સ્થિર શૂન્ય (*dll_ruby_init) (શૂન્ય);
સ્થિર શૂન્ય (*dll_ruby_init_loadpath) (શૂન્ય);
# ifdef MSWIN
સ્થિર શૂન્ય (*dll_ruby_sysinit) (પૂર્ણાંક*, અક્ષર***);
સ્થિર પૂર્ણાંક (*dll_rb_w32_snprintf)(અક્ષર*, size_t, અચલ અક્ષર*, ...);
# endif
# જો RUBY_VERSION >= 31
#  ifdef _MSC_VER
સ્થિર શૂન્ય (*dll_rb_unexpected_type) (કિંમત_3, પૂર્ણાંક);
#  નહીંતર
NORETURN(સ્થિર શૂન્ય (*dll_rb_unexpected_type) (કિંમત_3, પૂર્ણાંક));
#  endif
# endif
સ્થિર અક્ષર * (*dll_rb_string_કિંમત_ptr) (અસ્થિર કિંમત_3*);
સ્થિર કિંમત_3 (*dll_rb_float_new) (દ્વિગુણ);
સ્થિર કિંમત_3 (*dll_rb_ary_new) (શૂન્ય);
સ્થિર કિંમત_3 (*dll_rb_ary_new4) (long n, અચલ કિંમત_3 *elts);
સ્થિર કિંમત_3 (*dll_rb_ary_ધકેલો) (કિંમત_3, કિંમત_3);
# જો RUBY_VERSION >= 26 && RUBY_VERSION <= 32
સ્થિર શૂન્ય (*dll_rb_ary_detransient) (કિંમત_3);
# endif
# ifdef __ia64
સ્થિર શૂન્ય * (*dll_rb_ia64_bsp) (શૂન્ય);
સ્થિર શૂન્ય (*dll_ruby_init_ઢગલો)(કિંમત_3*, શૂન્ય*);
# નહીંતર
સ્થિર શૂન્ય (*dll_ruby_init_ઢગલો)(કિંમત_3*);
# endif
સ્થિર કિંમત_3 (*dll_rb_int2big)(SIGNED_કિંમત);

સ્થિર શૂન્ય (*dll_ruby_script) (અચલ અક્ષર*);
સ્થિર પૂર્ણાંક (*dll_rb_enc_find_index) (અચલ અક્ષર*);
સ્થિર rb_encoding* (*dll_rb_enc_find) (અચલ અક્ષર*);
સ્થિર કિંમત_3 (*dll_rb_enc_str_new) (અચલ અક્ષર*, long, rb_encoding*);
સ્થિર કિંમત_3 (*dll_rb_sprintf) (અચલ અક્ષર*, ...);
સ્થિર કિંમત_3 (*dll_rb_require) (અચલ અક્ષર*);
સ્થિર શૂન્ય* (*dll_ruby_options)(પૂર્ણાંક, અક્ષર**);

# જો defined(USE_RGENGC) && USE_RGENGC
#  જો RUBY_VERSION == 21
સ્થિર શૂન્ય (*dll_rb_gc_writebarrier_unprotect_promoted)(કિંમત_3);
#  નહીંતર
સ્થિર શૂન્ય (*dll_rb_gc_writebarrier_unprotect)(કિંમત_3 obj);
#  endif
# endif

# જો RUBY_VERSION >= 30
#  ifdef _MSC_VER
સ્થિર શૂન્ય (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  નહીંતર
NORETURN(સ્થિર શૂન્ય (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# જો RUBY_VERSION >= 26 && RUBY_VERSION <= 32
શૂન્ય rb_ary_detransient_stub(કિંમત_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  જો RUBY_VERSION >= 22
    long
rb_num2long_stub(કિંમત_3 x)
#  નહીંતર
    SIGNED_કિંમત
rb_num2long_stub(કિંમત_3 x)
#  endif
{
    પાછા dll_rb_num2long(x);
}
#  જો RUBY_VERSION >= 26
    કિંમત_3
rb_int2big_stub(intptr_t x)
#  નહીંતર
    કિંમત_3
rb_int2big_stub(SIGNED_કિંમત x)
#  endif
{
    પાછા dll_rb_int2big(x);
}
#  જો RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(કિંમત_3 x)
{
    પાછા dll_rb_fix2int(x);
}
    long
rb_num2int_stub(કિંમત_3 x)
{
    પાછા dll_rb_num2int(x);
}
#  endif
#  જો RUBY_VERSION >= 20
    કિંમત_3
rb_float_new_in_heap(દ્વિગુણ d)
{
    પાછા dll_rb_float_new(d);
}
#   જો RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(કિંમત_3 x)
#   નહીંતર
    કિંમત_3
rb_num2ulong(કિંમત_3 x)
#   endif
{
    પાછા (long)RSHIFT((SIGNED_કિંમત)(x),1);
}
#  endif
#  જો defined(USE_RGENGC) && USE_RGENGC
#   જો RUBY_VERSION == 21
    શૂન્ય
rb_gc_writebarrier_unprotect_promoted_stub(કિંમત_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   નહીંતર
    શૂન્ય
rb_gc_writebarrier_unprotect_stub(કિંમત_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  જો RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    શૂન્ય
rb_ary_detransient_stub(કિંમત_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  જો RUBY_VERSION >= 30
    શૂન્ય
rb_check_type_stub(કિંમત_3 obj, પૂર્ણાંક t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(કિંમત_3 x)
{
    પાછા dll_rb_num2uint(x);
}
    શૂન્ય
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  જો RUBY_VERSION >= 31
    શૂન્ય
rb_debug_rstring_null_ptr_stub(અચલ અક્ષર *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    શૂન્ય
rb_unexpected_type_stub(કિંમત_3 self, પૂર્ણાંક t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
શૂન્ય *rb_check_typeddata_stub(કિંમત_3 obj, અચલ rb_data_type_t *data_type)
{
    પાછા dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

સ્થિર HINSTANCE hinstRuby = ખાલી; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
સ્થિર સંરચના
{
    અક્ષર *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# જો defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# નહીંતર
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# જો RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_પ્રતીક},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  જો RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  નહીંતર
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# નહીંતર
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# જો RUBY_VERSION >= 31
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
# જો RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# નહીંતર
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# જો RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_કિંમત},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# નહીંતર
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# જો RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_કિંમત_ptr},
# જો RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# નહીંતર
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# નહીંતર
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ધકેલો},
# જો RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_ઢગલો},
# જો defined(USE_RGENGC) && USE_RGENGC
#  જો RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  નહીંતર
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# જો RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", ખાલી},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
ruby_runtime_link_init(અક્ષર *libname, પૂર્ણાંક verbose)
{
    પૂર્ણાંક i;

    જો (hinstRuby)
	પાછા OK;
    hinstRuby = load_dll(libname);
    જો (!hinstRuby)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	પાછા FAIL;
    }

    માટે (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	જો (!(*ruby_funcname_table[i].ptr = પ્રતીક_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ખાલી;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    પૂર્ણાંક
ruby_enabled(પૂર્ણાંક verbose)
{
    પાછા ruby_runtime_link_init((અક્ષર *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    શૂન્ય
ruby_end(શૂન્ય)
{
}

    શૂન્ય
ex_ruby(exarg_T *eap)
{
    પૂર્ણાંક state;
    અક્ષર *script = ખાલી;

    script = (અક્ષર *)script_get(eap, eap->arg);
    જો (!eap->skip && ensure_ruby_initialized())
    {
	જો (script == ખાલી)
	    rb_eval_string_protect((અક્ષર *)eap->arg, &state);
	નહીંતર
	    rb_eval_string_protect(script, &state);
	જો (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    સ્થિર કિંમત_3
vim_str2rb_enc_str(અચલ અક્ષર *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    જો (get_option_કિંમત((char_u *)"enc", &lval, &sval, ખાલી, 0) == gov_string)
    {
	enc = rb_enc_find((અક્ષર *)sval);
	vim_free(sval);
	જો (enc)
	    પાછા rb_enc_str_new(s, (long)strlen(s), enc);
    }
    પાછા rb_str_new2(s);
}

    સ્થિર કિંમત_3
eval_enc_string_protect(અચલ અક્ષર *str, પૂર્ણાંક *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    કિંમત_3 v;

    જો (get_option_કિંમત((char_u *)"enc", &lval, &sval, ખાલી, 0) == gov_string)
    {
	enc = rb_enc_find((અક્ષર *)sval);
	vim_free(sval);
	જો (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    પાછા rb_eval_string_protect(String_કિંમત_Ptr(v), state);
	}
    }
    પાછા rb_eval_string_protect(str, state);
}

    શૂન્ય
ex_rubydo(exarg_T *eap)
{
    પૂર્ણાંક state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    જો (!ensure_ruby_initialized())
	પાછા;

    જો (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	પાછા;
    માટે (i = eap->line1; i <= eap->line2; i++)
    {
	કિંમત_3 લીટી;

	જો (i > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;
	લીટી = vim_str2rb_enc_str((અક્ષર *)ml_get(i));
	rb_lastline_set(લીટી);
	eval_enc_string_protect((અક્ષર *) eap->arg, &state);
	જો (state)
	{
	    error_print(state);
	    તોડો;
	}
	જો (was_curbuf != curbuf || i > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;
	લીટી = rb_lastline_get();
	જો (!NIL_P(લીટી))
	{
	    જો (TYPE(લીટી) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		પાછા;
	    }
	    ml_replace(i, (char_u *) String_કિંમત_Ptr(લીટી), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    સ્થિર કિંમત_3
rb_load_wrap(કિંમત_3 file_to_load)
{
    rb_load(file_to_load, 0);
    પાછા Qnil;
}

    શૂન્ય
ex_rubyfile(exarg_T *eap)
{
    પૂર્ણાંક state;

    જો (!ensure_ruby_initialized())
	પાછા;

    કિંમત_3 file_to_load = rb_str_new2((અચલ અક્ષર *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    જો (state)
	error_print(state);
}

    શૂન્ય
ruby_buffer_free(buf_T *buf)
{
    જો (buf->b_ruby_ref == ખાલી)
	પાછા;

    rb_hash_aset(objtbl, rb_obj_id((કિંમત_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = ખાલી;
}

    શૂન્ય
ruby_window_free(win_T *win)
{
    જો (win->w_ruby_ref == ખાલી)
	પાછા;

    rb_hash_aset(objtbl, rb_obj_id((કિંમત_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = ખાલી;
}

    સ્થિર પૂર્ણાંક
ensure_ruby_initialized(શૂન્ય)
{
    જો (ruby_initialized)
	પાછા ruby_initialized;

#ifdef DYNAMIC_RUBY
    જો (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	પૂર્ણાંક argc = 1;
	અક્ષર *argv[] = {"gvim.exe"};
	અક્ષર **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_ઢગલો(ruby_ઢગલો_start);
	    ruby_init();
	}
	{
	    પૂર્ણાંક dummy_argc = 2;
	    અક્ષર *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    નહીંતર
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	પાછા 0;
    }
#endif

    પાછા ruby_initialized;
}

    સ્થિર શૂન્ય
error_print(પૂર્ણાંક state)
{
    કિંમત_3 error;
    કિંમત_3 eclass;
    કિંમત_3 einfo;
    કિંમત_3 bt;
    પૂર્ણાંક attr;
    અક્ષર buff[BUFSIZ];
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

    પસંદગી (state)
    {
	સ્થિતિ TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    તોડો;
	સ્થિતિ TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    તોડો;
	સ્થિતિ TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    તોડો;
	સ્થિતિ TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    તોડો;
	સ્થિતિ TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    તોડો;
	સ્થિતિ TAG_RAISE:
	સ્થિતિ TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    જો (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    નહીંતર
	    {
		કિંમત_3 epath;
		અક્ષર *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		જો (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#જો RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    માટે (i = 0; i < RARRAY_LEN(bt); i++)
		સંદેશ_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#નહીંતર
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    માટે (i = 0; i < RARRAY_LEN(bt); i++)
		સંદેશ_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    તોડો;
	મૂળભૂત:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    તોડો;
    }
}

    સ્થિર કિંમત_3
vim_message(કિંમત_3 self UNUSED, કિંમત_3 str)
{
    અક્ષર *buff, *p;

    str = rb_obj_as_string(str);
    જો (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(અક્ષર, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	જો (p) *p = '\0';
	સંદેશ(buff);
    }
    નહીંતર
    {
	સંદેશ("");
    }
    પાછા Qnil;
}

    સ્થિર કિંમત_3
vim_set_option(કિંમત_3 self UNUSED, કિંમત_3 str)
{
    do_set((char_u *)String_કિંમત_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    પાછા Qnil;
}

    સ્થિર કિંમત_3
vim_command(કિંમત_3 self UNUSED, કિંમત_3 str)
{
    do_cmdline_cmd((char_u *)String_કિંમત_Ptr(str));
    પાછા Qnil;
}

#ifdef FEAT_EVAL
    સ્થિર કિંમત_3
vim_to_ruby(typval_T *tv)
{
    કિંમત_3 result = Qnil;

    જો (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == ખાલી
					  ? "" : (અક્ષર *)(tv->vval.v_string));
    }
    નહીંતર જો (tv->v_type == VAR_સંખ્યા)
    {
	result = INT2_અંક(tv->vval.v_સંખ્યા);
    }
    નહીંતર જો (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    નહીંતર જો (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	જો (list != ખાલી)
	{
	    FOR_ALL_LIST_વસ્તુઓ(list, curr)
		rb_ary_ધકેલો(result, vim_to_ruby(&curr->li_tv));
	}
    }
    નહીંતર જો (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	જો (tv->vval.v_dict != ખાલી)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
	    {
		જો (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((અક્ષર *)hi->hi_ચાવી),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    નહીંતર જો (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	જો (tv->vval.v_સંખ્યા == VVAL_TRUE)
	    result = Qtrue;
	નહીંતર જો (tv->vval.v_સંખ્યા == VVAL_FALSE)
	    result = Qfalse;
    }
    નહીંતર જો (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    પાછા result;
}
#endif

    સ્થિર કિંમત_3
vim_evaluate(કિંમત_3 self UNUSED, કિંમત_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    કિંમત_3       result;

    tv = eval_પદાવલિ((char_u *)String_કિંમત_Ptr(str), ખાલી);
    જો (tv == ખાલી)
	પાછા Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    પાછા result;
#નહીંતર
    પાછા Qnil;
#endif
}

#ifdef USE_TYPEDDATA
સ્થિર size_t buffer_dsize(અચલ શૂન્ય *buf);

સ્થિર અચલ rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# જો RUBY_VERSION >= 27
	0, {0}
# નહીંતર
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    સ્થિર size_t
buffer_dsize(અચલ શૂન્ય *buf UNUSED)
{
    પાછા કદ(buf_T);
}
#endif

    સ્થિર કિંમત_3
buffer_new(buf_T *buf)
{
    જો (buf->b_ruby_ref)
    {
	પાછા (કિંમત_3) buf->b_ruby_ref;
    }
    નહીંતર
    {
#ifdef USE_TYPEDDATA
	કિંમત_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#નહીંતર
	કિંમત_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (શૂન્ય *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	પાછા obj;
    }
}

    સ્થિર buf_T *
get_buf(કિંમત_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#નહીંતર
    Data_Get_Struct(obj, buf_T, buf);
#endif
    જો (buf == ખાલી)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    પાછા buf;
}

    સ્થિર કિંમત_3
vim_blob(કિંમત_3 self UNUSED, કિંમત_3 str)
{
    કિંમત_3 result = rb_str_new("0z", 2);
    અક્ષર    buf[4];
    પૂર્ણાંક	i;
    માટે (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned અક્ષર)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    પાછા result;
}

    સ્થિર કિંમત_3
buffer_s_current(કિંમત_3 self UNUSED)
{
    પાછા buffer_new(curbuf);
}

    સ્થિર કિંમત_3
buffer_s_current_getter(ID id UNUSED, કિંમત_3 *x UNUSED)
{
    પાછા buffer_new(curbuf);
}

    સ્થિર કિંમત_3
buffer_s_ગણતરી(કિંમત_3 self UNUSED)
{
    buf_T *b;
    પૂર્ણાંક n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	જો (b->b_p_bl)
	    n++;
    }

    પાછા INT2_અંક(n);
}

    સ્થિર કિંમત_3
buffer_s_aref(કિંમત_3 self UNUSED, કિંમત_3 અંક)
{
    buf_T *b;
    પૂર્ણાંક n = NUM2INT(અંક);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	જો (!b->b_p_bl)
	    ચાલુ;

	જો (n == 0)
	    પાછા buffer_new(b);

	n--;
    }
    પાછા Qnil;
}

    સ્થિર કિંમત_3
buffer_name(કિંમત_3 self)
{
    buf_T *buf = get_buf(self);

    પાછા buf->b_ffname ? rb_str_new2((અક્ષર *)buf->b_ffname) : Qnil;
}

    સ્થિર કિંમત_3
buffer_સંખ્યા(કિંમત_3 self)
{
    buf_T *buf = get_buf(self);

    પાછા INT2_અંક(buf->b_fnum);
}

    સ્થિર કિંમત_3
buffer_ગણતરી(કિંમત_3 self)
{
    buf_T *buf = get_buf(self);

    પાછા INT2_અંક(buf->b_ml.ml_લીટી_ગણતરી);
}

    સ્થિર કિંમત_3
get_buffer_લીટી(buf_T *buf, linenr_T n)
{
    જો (n <= 0 || n > buf->b_ml.ml_લીટી_ગણતરી)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    પાછા vim_str2rb_enc_str((અક્ષર *)ml_get_buf(buf, n, FALSE));
}

    સ્થિર કિંમત_3
buffer_aref(કિંમત_3 self, કિંમત_3 અંક)
{
    buf_T *buf = get_buf(self);

    જો (buf != ખાલી)
	પાછા get_buffer_લીટી(buf, (linenr_T)NUM2LONG(અંક));
    પાછા Qnil; // For stop warning
}

    સ્થિર કિંમત_3
set_buffer_લીટી(buf_T *buf, linenr_T n, કિંમત_3 str)
{
    અક્ષર	*લીટી = String_કિંમત_Ptr(str);
    aco_save_T	aco;

    જો (n > 0 && n <= buf->b_ml.ml_લીટી_ગણતરી && લીટી != ખાલી)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	જો (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    જો (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)લીટી, TRUE);
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
    નહીંતર
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    પાછા str;
}

    સ્થિર કિંમત_3
buffer_aset(કિંમત_3 self, કિંમત_3 અંક, કિંમત_3 str)
{
    buf_T *buf = get_buf(self);

    જો (buf != ખાલી)
	પાછા set_buffer_લીટી(buf, (linenr_T)NUM2LONG(અંક), str);
    પાછા str;
}

    સ્થિર કિંમત_3
buffer_delete(કિંમત_3 self, કિંમત_3 અંક)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(અંક);
    aco_save_T	aco;

    જો (n > 0 && n <= buf->b_ml.ml_લીટી_ગણતરી)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	જો (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    જો (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_લીટીઓ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    નહીંતર
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    પાછા Qnil;
}

    સ્થિર કિંમત_3
buffer_append(કિંમત_3 self, કિંમત_3 અંક, કિંમત_3 str)
{
    buf_T	*buf = get_buf(self);
    અક્ષર	*લીટી = String_કિંમત_Ptr(str);
    long	n = NUM2LONG(અંક);
    aco_save_T	aco;

    જો (લીટી == ખાલી)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    નહીંતર જો (n >= 0 && n <= buf->b_ml.ml_લીટી_ગણતરી)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	જો (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    જો (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) લીટી, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_લીટીઓ_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    નહીંતર
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    પાછા str;
}

#ifdef USE_TYPEDDATA
સ્થિર size_t window_dsize(અચલ શૂન્ય *buf);

સ્થિર અચલ rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# જો RUBY_VERSION >= 27
	0, {0}
# નહીંતર
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    સ્થિર size_t
window_dsize(અચલ શૂન્ય *win UNUSED)
{
    પાછા કદ(win_T);
}
#endif

    સ્થિર કિંમત_3
window_new(win_T *win)
{
    જો (win->w_ruby_ref)
    {
	પાછા (કિંમત_3) win->w_ruby_ref;
    }
    નહીંતર
    {
#ifdef USE_TYPEDDATA
	કિંમત_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#નહીંતર
	કિંમત_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (શૂન્ય *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	પાછા obj;
    }
}

    સ્થિર win_T *
get_win(કિંમત_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#નહીંતર
    Data_Get_Struct(obj, win_T, win);
#endif
    જો (win == ખાલી)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    પાછા win;
}

    સ્થિર કિંમત_3
window_s_current(કિંમત_3 self UNUSED)
{
    પાછા window_new(curwin);
}

    સ્થિર કિંમત_3
window_s_current_getter(ID id UNUSED, કિંમત_3 *x UNUSED)
{
    પાછા window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    સ્થિર કિંમત_3
લીટી_s_current(કિંમત_3 self UNUSED)
{
    પાછા get_buffer_લીટી(curbuf, curwin->w_cursor.lnum);
}

    સ્થિર કિંમત_3
set_current_લીટી(કિંમત_3 self UNUSED, કિંમત_3 str)
{
    પાછા set_buffer_લીટી(curbuf, curwin->w_cursor.lnum, str);
}

    સ્થિર કિંમત_3
current_લીટી_સંખ્યા(કિંમત_3 self UNUSED)
{
    પાછા INT2FIX((પૂર્ણાંક)curwin->w_cursor.lnum);
}

    સ્થિર કિંમત_3
window_s_ગણતરી(કિંમત_3 self UNUSED)
{
    win_T	*w;
    પૂર્ણાંક n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    પાછા INT2_અંક(n);
}

    સ્થિર કિંમત_3
window_s_aref(કિંમત_3 self UNUSED, કિંમત_3 અંક)
{
    win_T *w;
    પૂર્ણાંક n = NUM2INT(અંક);

    માટે (w = firstwin; w != ખાલી; w = w->w_next, --n)
	જો (n == 0)
	    પાછા window_new(w);
    પાછા Qnil;
}

    સ્થિર કિંમત_3
window_buffer(કિંમત_3 self)
{
    win_T *win = get_win(self);

    પાછા buffer_new(win->w_buffer);
}

    સ્થિર કિંમત_3
window_height(કિંમત_3 self)
{
    win_T *win = get_win(self);

    પાછા INT2_અંક(win->w_height);
}

    સ્થિર કિંમત_3
window_set_height(કિંમત_3 self, કિંમત_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    પાછા height;
}

    સ્થિર કિંમત_3
window_width(કિંમત_3 self UNUSED)
{
    પાછા INT2_અંક(get_win(self)->w_width);
}

    સ્થિર કિંમત_3
window_set_width(કિંમત_3 self UNUSED, કિંમત_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    પાછા width;
}

    સ્થિર કિંમત_3
window_cursor(કિંમત_3 self)
{
    win_T *win = get_win(self);

    પાછા rb_assoc_new(INT2_અંક(win->w_cursor.lnum), INT2_અંક(win->w_cursor.col));
}

    સ્થિર કિંમત_3
window_set_cursor(કિંમત_3 self, કિંમત_3 pos)
{
    કિંમત_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    જો (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    પાછા Qnil;
}

    સ્થિર કિંમત_3
f_nop(કિંમત_3 self UNUSED)
{
    પાછા Qnil;
}

    સ્થિર કિંમત_3
f_p(પૂર્ણાંક argc, કિંમત_3 *argv, કિંમત_3 self UNUSED)
{
    પૂર્ણાંક i;
    કિંમત_3 str = rb_str_new("", 0);
    કિંમત_3 ret = Qnil;

    માટે (i = 0; i < argc; i++)
    {
	જો (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    સંદેશ(RSTRING_PTR(str));

    જો (argc == 1)
	ret = argv[0];
    નહીંતર જો (argc > 1)
	ret = rb_ary_new4(argc, argv);
    પાછા ret;
}

    સ્થિર શૂન્ય
ruby_io_init(શૂન્ય)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN કિંમત_3 rb_stdout;
    RUBYEXTERN કિંમત_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (શૂન્ય*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (શૂન્ય*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (શૂન્ય*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (શૂન્ય*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    સ્થિર શૂન્ય
ruby_vim_init(શૂન્ય)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_અંક(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_અંક(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_અંક(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_અંક(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (શૂન્ય*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (શૂન્ય*)buffer_s_ગણતરી, 0);
    rb_define_singleton_method(cBuffer, "[]", (શૂન્ય*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (શૂન્ય*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (શૂન્ય*)buffer_સંખ્યા, 0);
    rb_define_method(cBuffer, "count", (શૂન્ય*)buffer_ગણતરી, 0);
    rb_define_method(cBuffer, "length", (શૂન્ય*)buffer_ગણતરી, 0);
    rb_define_method(cBuffer, "[]", (શૂન્ય*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (શૂન્ય*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (શૂન્ય*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (શૂન્ય*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (શૂન્ય*)current_લીટી_સંખ્યા, 0);
    rb_define_method(cBuffer, "line", (શૂન્ય*)લીટી_s_current, 0);
    rb_define_method(cBuffer, "line=", (શૂન્ય*)set_current_લીટી, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (શૂન્ય*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (શૂન્ય*)window_s_ગણતરી, 0);
    rb_define_singleton_method(cVimWindow, "[]", (શૂન્ય*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (શૂન્ય*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (શૂન્ય*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (શૂન્ય*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (શૂન્ય*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (શૂન્ય*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (શૂન્ય*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (શૂન્ય*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    શૂન્ય
vim_ruby_init(શૂન્ય *ઢગલો_start)
{
    // should get machine stack start address early in main function
    ruby_ઢગલો_start = ઢગલો_start;
}

    સ્થિર પૂર્ણાંક
convert_hash2dict(કિંમત_3 ચાવી, કિંમત_3 val, કિંમત_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(ચાવી)));
    જો (di == ખાલી || ruby_convert_to_vim_કિંમત(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	પાછા ST_STOP;
    }
    પાછા ST_CONTINUE;
}

    સ્થિર પૂર્ણાંક
ruby_convert_to_vim_કિંમત(કિંમત_3 val, typval_T *rettv)
{
    પસંદગી (TYPE(val))
    {
	સ્થિતિ T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_સંખ્યા = VVAL_NULL;
	    તોડો;
	સ્થિતિ T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_સંખ્યા = VVAL_TRUE;
	    તોડો;
	સ્થિતિ T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_સંખ્યા = VVAL_FALSE;
	    તોડો;
	સ્થિતિ T_BIGNUM:
	સ્થિતિ T_FIXNUM:
	    rettv->v_type = VAR_સંખ્યા;
	    rettv->vval.v_સંખ્યા = (varnumber_T)NUM2LONG(val);
	    તોડો;
	સ્થિતિ T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    તોડો;
	મૂળભૂત:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	સ્થિતિ T_STRING:
	    {
		કિંમત_3 str = (કિંમત_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    તોડો;
	સ્થિતિ T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		જો (l == ખાલી)
		    પાછા FAIL;

		માટે (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    જો (ruby_convert_to_vim_કિંમત((કિંમત_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			પાછા FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    તોડો;
	સ્થિતિ T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		જો (d == ખાલી)
		    પાછા FAIL;

		rb_hash_foreach(val, convert_hash2dict, (કિંમત_3)d);
		જો (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    પાછા FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    તોડો;
    }
    પાછા OK;
}

    શૂન્ય
do_rubyeval(char_u *str, typval_T *rettv)
{
    પૂર્ણાંક retval = FAIL;

    જો (ensure_ruby_initialized())
    {
	પૂર્ણાંક state;
	કિંમત_3 obj;

	obj = rb_eval_string_protect((અચલ અક્ષર *)str, &state);
	જો (state)
	    error_print(state);
	નહીંતર
	    retval = ruby_convert_to_vim_કિંમત(obj, rettv);
    }
    જો (retval == FAIL)
    {
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = 0;
    }
}
