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

#அந்தர்பாவ "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN வெளி
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# எனில் RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# எனில் defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# எனில் RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_குறியீடு		(*dll_rb_c_குறியீடு)
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

# எனில் RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# எனில் RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# எனில் RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# எனில் RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# எனில் RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# எனில் RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# எனில் RUBY_VERSION >= 31
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

#எனில் defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic தள்ளு
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#எனில் defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#அந்தர்பாவ <ruby.h>
#அந்தர்பாவ <ruby/encoding.h>

#எனில் defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic எடு
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#எனில் defined(MACOS_X)
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
#எனில் defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_மதிப்பு_Ptr
# define String_மதிப்பு_Ptr(s) STR2CSTR(s)
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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define குறியீடு_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# இல்லையெனில்
#  include <dlfcn.h>
#  define HINSTANCE வெறுமை*
#  define RUBY_PROC வெறுமை*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define குறியீடு_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

நிலையான முழு ruby_initialized = 0;
நிலையான வெறுமை *ruby_அடுக்கு_start;
நிலையான மதிப்பு_3 objtbl;

நிலையான மதிப்பு_3 mVIM;
நிலையான மதிப்பு_3 cBuffer;
நிலையான மதிப்பு_3 cVimWindow;
நிலையான மதிப்பு_3 eDeletedBufferError;
நிலையான மதிப்பு_3 eDeletedWindowError;

நிலையான முழு ensure_ruby_initialized(வெறுமை);
நிலையான வெறுமை error_print(முழு);
நிலையான வெறுமை ruby_io_init(வெறுமை);
நிலையான வெறுமை ruby_vim_init(வெறுமை);
நிலையான முழு ruby_convert_to_vim_மதிப்பு(மதிப்பு_3 val, typval_T *rettv);

#எனில் defined(__ia64) && !defined(ruby_init_அடுக்கு)
# define ruby_init_அடுக்கு(addr) ruby_init_அடுக்கு((addr), rb_ia64_bsp())
#endif

#எனில் defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# எனில் RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# எனில் RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  எனில் RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  இல்லையெனில்
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# இல்லையெனில்
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
# எனில் RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# இல்லையெனில்
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

# எனில் VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  எனில் RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# எனில் RUBY_VERSION < 20
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
# இல்லையெனில்
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_மதிப்பு		dll_rb_string_மதிப்பு
# define rb_string_மதிப்பு_ptr		dll_rb_string_மதிப்பு_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_தள்ளு			dll_rb_ary_தள்ளு
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_அடுக்கு
#  define ruby_init_அடுக்கு(addr)	dll_ruby_init_அடுக்கு((addr), rb_ia64_bsp())
# இல்லையெனில்
#  define ruby_init_அடுக்கு		dll_ruby_init_அடுக்கு
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
நிலையான மதிப்பு_3 (*dll_rb_assoc_new) (மதிப்பு_3, மதிப்பு_3);
மதிப்பு_3 *dll_rb_cFalseClass;
மதிப்பு_3 *dll_rb_cFixnum;
# எனில் defined(USE_RUBY_INTEGER)
மதிப்பு_3 *dll_rb_cInteger;
# endif
# எனில் RUBY_VERSION >= 20
மதிப்பு_3 *dll_rb_cFloat;
# endif
மதிப்பு_3 *dll_rb_cNilClass;
நிலையான மதிப்பு_3 *dll_rb_cObject;
மதிப்பு_3 *dll_rb_cString;
மதிப்பு_3 *dll_rb_c_குறியீடு;
மதிப்பு_3 *dll_rb_cTrueClass;
நிலையான மதிப்பு_3 (*dll_rb_class_new_instance) (முழு,மதிப்பு_3*,மதிப்பு_3);
நிலையான வெறுமை (*dll_rb_check_type) (மதிப்பு_3,முழு);
# ifdef USE_TYPEDDATA
நிலையான வெறுமை *(*dll_rb_check_typeddata) (மதிப்பு_3,மாறிலி rb_data_type_t *);
# endif
நிலையான மதிப்பு_3 (*dll_rb_class_path) (மதிப்பு_3);
# ifdef USE_TYPEDDATA
#  எனில் RUBY_VERSION >= 23
நிலையான மதிப்பு_3 (*dll_rb_data_typed_object_wrap) (மதிப்பு_3, வெறுமை*, மாறிலி rb_data_type_t *);
#  இல்லையெனில்
நிலையான மதிப்பு_3 (*dll_rb_data_typed_object_alloc) (மதிப்பு_3, வெறுமை*, மாறிலி rb_data_type_t *);
#  endif
# இல்லையெனில்
நிலையான மதிப்பு_3 (*dll_rb_data_object_alloc) (மதிப்பு_3, வெறுமை*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# எனில் RUBY_VERSION >= 31
நிலையான வெறுமை (*dll_rb_debug_rstring_null_ptr) (மாறிலி எழுத்து*);
# endif
நிலையான மதிப்பு_3 (*dll_rb_define_class_under) (மதிப்பு_3, மாறிலி எழுத்து*, மதிப்பு_3);
நிலையான வெறுமை (*dll_rb_define_const) (மதிப்பு_3,மாறிலி எழுத்து*,மதிப்பு_3);
நிலையான வெறுமை (*dll_rb_define_global_function) (மாறிலி எழுத்து*,மதிப்பு_3(*)(முழு,மதிப்பு_3*,மதிப்பு_3),முழு);
நிலையான வெறுமை (*dll_rb_define_method) (மதிப்பு_3,மாறிலி எழுத்து*,மதிப்பு_3(*)(ANYARGS),முழு);
நிலையான மதிப்பு_3 (*dll_rb_define_module) (மாறிலி எழுத்து*);
நிலையான வெறுமை (*dll_rb_define_module_function) (மதிப்பு_3,மாறிலி எழுத்து*,மதிப்பு_3(*)(மதிப்பு_3,மதிப்பு_3),முழு);
நிலையான வெறுமை (*dll_rb_define_singleton_method) (மதிப்பு_3,மாறிலி எழுத்து*,மதிப்பு_3(*)(ANYARGS),முழு);
நிலையான வெறுமை (*dll_rb_define_virtual_variable) (மாறிலி எழுத்து*,மதிப்பு_3(*)(ID,மதிப்பு_3*),வெறுமை(*)());
நிலையான மதிப்பு_3 *dll_rb_stdout;
நிலையான மதிப்பு_3 *dll_rb_stderr;
நிலையான மதிப்பு_3 *dll_rb_eArgError;
நிலையான மதிப்பு_3 *dll_rb_eIndexError;
நிலையான மதிப்பு_3 *dll_rb_eRuntimeError;
நிலையான மதிப்பு_3 *dll_rb_eStandardError;
நிலையான மதிப்பு_3 (*dll_rb_eval_string_protect) (மாறிலி எழுத்து*, முழு*);
# எனில் RUBY_VERSION >= 21
நிலையான மதிப்பு_3 (*dll_rb_funcallv) (மதிப்பு_3, ID, முழு, மாறிலி மதிப்பு_3*);
# இல்லையெனில்
நிலையான மதிப்பு_3 (*dll_rb_funcall2) (மதிப்பு_3, ID, முழு, மாறிலி மதிப்பு_3*);
# endif
நிலையான வெறுமை (*dll_rb_global_variable) (மதிப்பு_3*);
நிலையான மதிப்பு_3 (*dll_rb_hash_aset) (மதிப்பு_3, மதிப்பு_3, மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_hash_foreach) (மதிப்பு_3, முழு (*)(மதிப்பு_3, மதிப்பு_3, மதிப்பு_3), மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_hash_new) (வெறுமை);
நிலையான மதிப்பு_3 (*dll_rb_inspect) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_int2inum) (long);
நிலையான ID (*dll_rb_intern) (மாறிலி எழுத்து*);
# எனில் RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
நிலையான long (*dll_rb_fix2int) (மதிப்பு_3);
நிலையான long (*dll_rb_num2int) (மதிப்பு_3);
நிலையான unsigned long (*dll_rb_num2uint) (மதிப்பு_3);
# endif
நிலையான இரட்டை (*dll_rb_num2dbl) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_lastline_get) (வெறுமை);
நிலையான வெறுமை (*dll_rb_lastline_set) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_protect) (மதிப்பு_3 (*)(மதிப்பு_3), மதிப்பு_3, முழு*);
நிலையான வெறுமை (*dll_rb_load) (மதிப்பு_3, முழு);
நிலையான long (*dll_rb_num2long) (மதிப்பு_3);
நிலையான unsigned long (*dll_rb_num2ulong) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_obj_alloc) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_obj_as_string) (மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_obj_id) (மதிப்பு_3);
நிலையான வெறுமை (*dll_rb_raise) (மதிப்பு_3, மாறிலி எழுத்து*, ...);
நிலையான மதிப்பு_3 (*dll_rb_string_மதிப்பு) (நிலையற்ற மதிப்பு_3*);
நிலையான மதிப்பு_3 (*dll_rb_str_cat) (மதிப்பு_3, மாறிலி எழுத்து*, long);
நிலையான மதிப்பு_3 (*dll_rb_str_concat) (மதிப்பு_3, மதிப்பு_3);
நிலையான மதிப்பு_3 (*dll_rb_str_new) (மாறிலி எழுத்து*, long);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
நிலையான மதிப்பு_3 (*dll_rb_str_new_cstr) (மாறிலி எழுத்து*);
# இல்லையெனில்
நிலையான மதிப்பு_3 (*dll_rb_str_new2) (மாறிலி எழுத்து*);
# endif
நிலையான மதிப்பு_3 (*dll_rb_errinfo) (வெறுமை);
நிலையான வெறுமை (*dll_ruby_init) (வெறுமை);
நிலையான வெறுமை (*dll_ruby_init_loadpath) (வெறுமை);
# ifdef MSWIN
நிலையான வெறுமை (*dll_ruby_sysinit) (முழு*, எழுத்து***);
நிலையான முழு (*dll_rb_w32_snprintf)(எழுத்து*, size_t, மாறிலி எழுத்து*, ...);
# endif
# எனில் RUBY_VERSION >= 31
#  ifdef _MSC_VER
நிலையான வெறுமை (*dll_rb_unexpected_type) (மதிப்பு_3, முழு);
#  இல்லையெனில்
NORETURN(நிலையான வெறுமை (*dll_rb_unexpected_type) (மதிப்பு_3, முழு));
#  endif
# endif
நிலையான எழுத்து * (*dll_rb_string_மதிப்பு_ptr) (நிலையற்ற மதிப்பு_3*);
நிலையான மதிப்பு_3 (*dll_rb_float_new) (இரட்டை);
நிலையான மதிப்பு_3 (*dll_rb_ary_new) (வெறுமை);
நிலையான மதிப்பு_3 (*dll_rb_ary_new4) (long n, மாறிலி மதிப்பு_3 *elts);
நிலையான மதிப்பு_3 (*dll_rb_ary_தள்ளு) (மதிப்பு_3, மதிப்பு_3);
# எனில் RUBY_VERSION >= 26 && RUBY_VERSION <= 32
நிலையான வெறுமை (*dll_rb_ary_detransient) (மதிப்பு_3);
# endif
# ifdef __ia64
நிலையான வெறுமை * (*dll_rb_ia64_bsp) (வெறுமை);
நிலையான வெறுமை (*dll_ruby_init_அடுக்கு)(மதிப்பு_3*, வெறுமை*);
# இல்லையெனில்
நிலையான வெறுமை (*dll_ruby_init_அடுக்கு)(மதிப்பு_3*);
# endif
நிலையான மதிப்பு_3 (*dll_rb_int2big)(SIGNED_மதிப்பு);

நிலையான வெறுமை (*dll_ruby_script) (மாறிலி எழுத்து*);
நிலையான முழு (*dll_rb_enc_find_index) (மாறிலி எழுத்து*);
நிலையான rb_encoding* (*dll_rb_enc_find) (மாறிலி எழுத்து*);
நிலையான மதிப்பு_3 (*dll_rb_enc_str_new) (மாறிலி எழுத்து*, long, rb_encoding*);
நிலையான மதிப்பு_3 (*dll_rb_sprintf) (மாறிலி எழுத்து*, ...);
நிலையான மதிப்பு_3 (*dll_rb_require) (மாறிலி எழுத்து*);
நிலையான வெறுமை* (*dll_ruby_options)(முழு, எழுத்து**);

# எனில் defined(USE_RGENGC) && USE_RGENGC
#  எனில் RUBY_VERSION == 21
நிலையான வெறுமை (*dll_rb_gc_writebarrier_unprotect_promoted)(மதிப்பு_3);
#  இல்லையெனில்
நிலையான வெறுமை (*dll_rb_gc_writebarrier_unprotect)(மதிப்பு_3 obj);
#  endif
# endif

# எனில் RUBY_VERSION >= 30
#  ifdef _MSC_VER
நிலையான வெறுமை (*dll_ruby_malloc_size_overflow)(size_t, size_t);
#  இல்லையெனில்
NORETURN(நிலையான வெறுமை (*dll_ruby_malloc_size_overflow)(size_t, size_t));
#  endif
# endif

# எனில் RUBY_VERSION >= 26 && RUBY_VERSION <= 32
வெறுமை rb_ary_detransient_stub(மதிப்பு_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  எனில் RUBY_VERSION >= 22
    long
rb_num2long_stub(மதிப்பு_3 x)
#  இல்லையெனில்
    SIGNED_மதிப்பு
rb_num2long_stub(மதிப்பு_3 x)
#  endif
{
    திரும்பு dll_rb_num2long(x);
}
#  எனில் RUBY_VERSION >= 26
    மதிப்பு_3
rb_int2big_stub(intptr_t x)
#  இல்லையெனில்
    மதிப்பு_3
rb_int2big_stub(SIGNED_மதிப்பு x)
#  endif
{
    திரும்பு dll_rb_int2big(x);
}
#  எனில் RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    long
rb_fix2int_stub(மதிப்பு_3 x)
{
    திரும்பு dll_rb_fix2int(x);
}
    long
rb_num2int_stub(மதிப்பு_3 x)
{
    திரும்பு dll_rb_num2int(x);
}
#  endif
#  எனில் RUBY_VERSION >= 20
    மதிப்பு_3
rb_float_new_in_heap(இரட்டை d)
{
    திரும்பு dll_rb_float_new(d);
}
#   எனில் RUBY_VERSION >= 22
    unsigned long
rb_num2ulong(மதிப்பு_3 x)
#   இல்லையெனில்
    மதிப்பு_3
rb_num2ulong(மதிப்பு_3 x)
#   endif
{
    திரும்பு (long)RSHIFT((SIGNED_மதிப்பு)(x),1);
}
#  endif
#  எனில் defined(USE_RGENGC) && USE_RGENGC
#   எனில் RUBY_VERSION == 21
    வெறுமை
rb_gc_writebarrier_unprotect_promoted_stub(மதிப்பு_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   இல்லையெனில்
    வெறுமை
rb_gc_writebarrier_unprotect_stub(மதிப்பு_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  எனில் RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    வெறுமை
rb_ary_detransient_stub(மதிப்பு_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  எனில் RUBY_VERSION >= 30
    வெறுமை
rb_check_type_stub(மதிப்பு_3 obj, முழு t)
{
    dll_rb_check_type(obj, t);
}
    unsigned long
rb_num2uint_stub(மதிப்பு_3 x)
{
    திரும்பு dll_rb_num2uint(x);
}
    வெறுமை
ruby_malloc_size_overflow_stub(size_t x, size_t y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  எனில் RUBY_VERSION >= 31
    வெறுமை
rb_debug_rstring_null_ptr_stub(மாறிலி எழுத்து *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    வெறுமை
rb_unexpected_type_stub(மதிப்பு_3 self, முழு t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
வெறுமை *rb_check_typeddata_stub(மதிப்பு_3 obj, மாறிலி rb_data_type_t *data_type)
{
    திரும்பு dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

நிலையான HINSTANCE hinstRuby = வெற்று; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
நிலையான கட்டமைப்பு
{
    எழுத்து *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# எனில் defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# இல்லையெனில்
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# எனில் RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_குறியீடு},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  எனில் RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  இல்லையெனில்
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# இல்லையெனில்
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# எனில் RUBY_VERSION >= 31
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
# எனில் RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# இல்லையெனில்
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# எனில் RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_மதிப்பு},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# இல்லையெனில்
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# எனில் RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_மதிப்பு_ptr},
# எனில் RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# இல்லையெனில்
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# இல்லையெனில்
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_தள்ளு},
# எனில் RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_அடுக்கு},
# எனில் defined(USE_RGENGC) && USE_RGENGC
#  எனில் RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  இல்லையெனில்
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# எனில் RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", வெற்று},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    நிலையான முழு
ruby_runtime_link_init(எழுத்து *libname, முழு verbose)
{
    முழு i;

    எனில் (hinstRuby)
	திரும்பு OK;
    hinstRuby = load_dll(libname);
    எனில் (!hinstRuby)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }

    க்கு (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	எனில் (!(*ruby_funcname_table[i].ptr = குறியீடு_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = வெற்று;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    முழு
ruby_enabled(முழு verbose)
{
    திரும்பு ruby_runtime_link_init((எழுத்து *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    வெறுமை
ruby_end(வெறுமை)
{
}

    வெறுமை
ex_ruby(exarg_T *eap)
{
    முழு state;
    எழுத்து *script = வெற்று;

    script = (எழுத்து *)script_get(eap, eap->arg);
    எனில் (!eap->skip && ensure_ruby_initialized())
    {
	எனில் (script == வெற்று)
	    rb_eval_string_protect((எழுத்து *)eap->arg, &state);
	இல்லையெனில்
	    rb_eval_string_protect(script, &state);
	எனில் (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    நிலையான மதிப்பு_3
vim_str2rb_enc_str(மாறிலி எழுத்து *s)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;

    எனில் (get_option_மதிப்பு((char_u *)"enc", &lval, &sval, வெற்று, 0) == gov_string)
    {
	enc = rb_enc_find((எழுத்து *)sval);
	vim_free(sval);
	எனில் (enc)
	    திரும்பு rb_enc_str_new(s, (long)strlen(s), enc);
    }
    திரும்பு rb_str_new2(s);
}

    நிலையான மதிப்பு_3
eval_enc_string_protect(மாறிலி எழுத்து *str, முழு *state)
{
    long lval;
    char_u *sval;
    rb_encoding *enc;
    மதிப்பு_3 v;

    எனில் (get_option_மதிப்பு((char_u *)"enc", &lval, &sval, வெற்று, 0) == gov_string)
    {
	enc = rb_enc_find((எழுத்து *)sval);
	vim_free(sval);
	எனில் (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    திரும்பு rb_eval_string_protect(String_மதிப்பு_Ptr(v), state);
	}
    }
    திரும்பு rb_eval_string_protect(str, state);
}

    வெறுமை
ex_rubydo(exarg_T *eap)
{
    முழு state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    எனில் (!ensure_ruby_initialized())
	திரும்பு;

    எனில் (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	திரும்பு;
    க்கு (i = eap->line1; i <= eap->line2; i++)
    {
	மதிப்பு_3 வரி;

	எனில் (i > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;
	வரி = vim_str2rb_enc_str((எழுத்து *)ml_get(i));
	rb_lastline_set(வரி);
	eval_enc_string_protect((எழுத்து *) eap->arg, &state);
	எனில் (state)
	{
	    error_print(state);
	    நிறுத்து;
	}
	எனில் (was_curbuf != curbuf || i > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;
	வரி = rb_lastline_get();
	எனில் (!NIL_P(வரி))
	{
	    எனில் (TYPE(வரி) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		திரும்பு;
	    }
	    ml_replace(i, (char_u *) String_மதிப்பு_Ptr(வரி), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    நிலையான மதிப்பு_3
rb_load_wrap(மதிப்பு_3 file_to_load)
{
    rb_load(file_to_load, 0);
    திரும்பு Qnil;
}

    வெறுமை
ex_rubyfile(exarg_T *eap)
{
    முழு state;

    எனில் (!ensure_ruby_initialized())
	திரும்பு;

    மதிப்பு_3 file_to_load = rb_str_new2((மாறிலி எழுத்து *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    எனில் (state)
	error_print(state);
}

    வெறுமை
ruby_buffer_free(buf_T *buf)
{
    எனில் (buf->b_ruby_ref == வெற்று)
	திரும்பு;

    rb_hash_aset(objtbl, rb_obj_id((மதிப்பு_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = வெற்று;
}

    வெறுமை
ruby_window_free(win_T *win)
{
    எனில் (win->w_ruby_ref == வெற்று)
	திரும்பு;

    rb_hash_aset(objtbl, rb_obj_id((மதிப்பு_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = வெற்று;
}

    நிலையான முழு
ensure_ruby_initialized(வெறுமை)
{
    எனில் (ruby_initialized)
	திரும்பு ruby_initialized;

#ifdef DYNAMIC_RUBY
    எனில் (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	முழு argc = 1;
	எழுத்து *argv[] = {"gvim.exe"};
	எழுத்து **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_அடுக்கு(ruby_அடுக்கு_start);
	    ruby_init();
	}
	{
	    முழு dummy_argc = 2;
	    எழுத்து *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    இல்லையெனில்
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	திரும்பு 0;
    }
#endif

    திரும்பு ruby_initialized;
}

    நிலையான வெறுமை
error_print(முழு state)
{
    மதிப்பு_3 error;
    மதிப்பு_3 eclass;
    மதிப்பு_3 einfo;
    மதிப்பு_3 bt;
    முழு attr;
    எழுத்து buff[BUFSIZ];
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

    தேர்வு (state)
    {
	நிலை TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    நிறுத்து;
	நிலை TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    நிறுத்து;
	நிலை TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    நிறுத்து;
	நிலை TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    நிறுத்து;
	நிலை TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    நிறுத்து;
	நிலை TAG_RAISE:
	நிலை TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    எனில் (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    இல்லையெனில்
	    {
		மதிப்பு_3 epath;
		எழுத்து *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		எனில் (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#எனில் RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    க்கு (i = 0; i < RARRAY_LEN(bt); i++)
		செய்தி_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#இல்லையெனில்
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    க்கு (i = 0; i < RARRAY_LEN(bt); i++)
		செய்தி_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    நிறுத்து;
	இயல்பு:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    நிறுத்து;
    }
}

    நிலையான மதிப்பு_3
vim_message(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
    எழுத்து *buff, *p;

    str = rb_obj_as_string(str);
    எனில் (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(எழுத்து, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	எனில் (p) *p = '\0';
	செய்தி(buff);
    }
    இல்லையெனில்
    {
	செய்தி("");
    }
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
vim_set_option(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
    do_set((char_u *)String_மதிப்பு_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
vim_command(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
    do_cmdline_cmd((char_u *)String_மதிப்பு_Ptr(str));
    திரும்பு Qnil;
}

#ifdef FEAT_EVAL
    நிலையான மதிப்பு_3
vim_to_ruby(typval_T *tv)
{
    மதிப்பு_3 result = Qnil;

    எனில் (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == வெற்று
					  ? "" : (எழுத்து *)(tv->vval.v_string));
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_எண்)
    {
	result = INT2_இலக்கம்(tv->vval.v_எண்);
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	எனில் (list != வெற்று)
	{
	    FOR_ALL_LIST_பொருட்கள்(list, curr)
		rb_ary_தள்ளு(result, vim_to_ruby(&curr->li_tv));
	}
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	எனில் (tv->vval.v_dict != வெற்று)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
	    {
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((எழுத்து *)hi->hi_சாவி),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	எனில் (tv->vval.v_எண் == VVAL_TRUE)
	    result = Qtrue;
	இல்லையெனில் எனில் (tv->vval.v_எண் == VVAL_FALSE)
	    result = Qfalse;
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    திரும்பு result;
}
#endif

    நிலையான மதிப்பு_3
vim_evaluate(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    மதிப்பு_3       result;

    tv = eval_கோவை((char_u *)String_மதிப்பு_Ptr(str), வெற்று);
    எனில் (tv == வெற்று)
	திரும்பு Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    திரும்பு result;
#இல்லையெனில்
    திரும்பு Qnil;
#endif
}

#ifdef USE_TYPEDDATA
நிலையான size_t buffer_dsize(மாறிலி வெறுமை *buf);

நிலையான மாறிலி rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# எனில் RUBY_VERSION >= 27
	0, {0}
# இல்லையெனில்
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    நிலையான size_t
buffer_dsize(மாறிலி வெறுமை *buf UNUSED)
{
    திரும்பு அளவு(buf_T);
}
#endif

    நிலையான மதிப்பு_3
buffer_new(buf_T *buf)
{
    எனில் (buf->b_ruby_ref)
    {
	திரும்பு (மதிப்பு_3) buf->b_ruby_ref;
    }
    இல்லையெனில்
    {
#ifdef USE_TYPEDDATA
	மதிப்பு_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#இல்லையெனில்
	மதிப்பு_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (வெறுமை *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	திரும்பு obj;
    }
}

    நிலையான buf_T *
get_buf(மதிப்பு_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#இல்லையெனில்
    Data_Get_Struct(obj, buf_T, buf);
#endif
    எனில் (buf == வெற்று)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    திரும்பு buf;
}

    நிலையான மதிப்பு_3
vim_blob(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
    மதிப்பு_3 result = rb_str_new("0z", 2);
    எழுத்து    buf[4];
    முழு	i;
    க்கு (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (unsigned எழுத்து)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    திரும்பு result;
}

    நிலையான மதிப்பு_3
buffer_s_current(மதிப்பு_3 self UNUSED)
{
    திரும்பு buffer_new(curbuf);
}

    நிலையான மதிப்பு_3
buffer_s_current_getter(ID id UNUSED, மதிப்பு_3 *x UNUSED)
{
    திரும்பு buffer_new(curbuf);
}

    நிலையான மதிப்பு_3
buffer_s_மொத்தம்(மதிப்பு_3 self UNUSED)
{
    buf_T *b;
    முழு n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	எனில் (b->b_p_bl)
	    n++;
    }

    திரும்பு INT2_இலக்கம்(n);
}

    நிலையான மதிப்பு_3
buffer_s_aref(மதிப்பு_3 self UNUSED, மதிப்பு_3 இலக்கம்)
{
    buf_T *b;
    முழு n = NUM2INT(இலக்கம்);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	எனில் (!b->b_p_bl)
	    தொடர்;

	எனில் (n == 0)
	    திரும்பு buffer_new(b);

	n--;
    }
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
buffer_name(மதிப்பு_3 self)
{
    buf_T *buf = get_buf(self);

    திரும்பு buf->b_ffname ? rb_str_new2((எழுத்து *)buf->b_ffname) : Qnil;
}

    நிலையான மதிப்பு_3
buffer_எண்(மதிப்பு_3 self)
{
    buf_T *buf = get_buf(self);

    திரும்பு INT2_இலக்கம்(buf->b_fnum);
}

    நிலையான மதிப்பு_3
buffer_மொத்தம்(மதிப்பு_3 self)
{
    buf_T *buf = get_buf(self);

    திரும்பு INT2_இலக்கம்(buf->b_ml.ml_வரி_மொத்தம்);
}

    நிலையான மதிப்பு_3
get_buffer_வரி(buf_T *buf, linenr_T n)
{
    எனில் (n <= 0 || n > buf->b_ml.ml_வரி_மொத்தம்)
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    திரும்பு vim_str2rb_enc_str((எழுத்து *)ml_get_buf(buf, n, FALSE));
}

    நிலையான மதிப்பு_3
buffer_aref(மதிப்பு_3 self, மதிப்பு_3 இலக்கம்)
{
    buf_T *buf = get_buf(self);

    எனில் (buf != வெற்று)
	திரும்பு get_buffer_வரி(buf, (linenr_T)NUM2LONG(இலக்கம்));
    திரும்பு Qnil; // For stop warning
}

    நிலையான மதிப்பு_3
set_buffer_வரி(buf_T *buf, linenr_T n, மதிப்பு_3 str)
{
    எழுத்து	*வரி = String_மதிப்பு_Ptr(str);
    aco_save_T	aco;

    எனில் (n > 0 && n <= buf->b_ml.ml_வரி_மொத்தம் && வரி != வெற்று)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    எனில் (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)வரி, TRUE);
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
    இல்லையெனில்
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (long)n);
    }
    திரும்பு str;
}

    நிலையான மதிப்பு_3
buffer_aset(மதிப்பு_3 self, மதிப்பு_3 இலக்கம், மதிப்பு_3 str)
{
    buf_T *buf = get_buf(self);

    எனில் (buf != வெற்று)
	திரும்பு set_buffer_வரி(buf, (linenr_T)NUM2LONG(இலக்கம்), str);
    திரும்பு str;
}

    நிலையான மதிப்பு_3
buffer_delete(மதிப்பு_3 self, மதிப்பு_3 இலக்கம்)
{
    buf_T	*buf = get_buf(self);
    long	n = NUM2LONG(இலக்கம்);
    aco_save_T	aco;

    எனில் (n > 0 && n <= buf->b_ml.ml_வரி_மொத்தம்)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    எனில் (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_வரிகள்_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    இல்லையெனில்
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
buffer_append(மதிப்பு_3 self, மதிப்பு_3 இலக்கம், மதிப்பு_3 str)
{
    buf_T	*buf = get_buf(self);
    எழுத்து	*வரி = String_மதிப்பு_Ptr(str);
    long	n = NUM2LONG(இலக்கம்);
    aco_save_T	aco;

    எனில் (வரி == வெற்று)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    இல்லையெனில் எனில் (n >= 0 && n <= buf->b_ml.ml_வரி_மொத்தம்)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    எனில் (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) வரி, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_வரிகள்_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    இல்லையெனில்
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    திரும்பு str;
}

#ifdef USE_TYPEDDATA
நிலையான size_t window_dsize(மாறிலி வெறுமை *buf);

நிலையான மாறிலி rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# எனில் RUBY_VERSION >= 27
	0, {0}
# இல்லையெனில்
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    நிலையான size_t
window_dsize(மாறிலி வெறுமை *win UNUSED)
{
    திரும்பு அளவு(win_T);
}
#endif

    நிலையான மதிப்பு_3
window_new(win_T *win)
{
    எனில் (win->w_ruby_ref)
    {
	திரும்பு (மதிப்பு_3) win->w_ruby_ref;
    }
    இல்லையெனில்
    {
#ifdef USE_TYPEDDATA
	மதிப்பு_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#இல்லையெனில்
	மதிப்பு_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (வெறுமை *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	திரும்பு obj;
    }
}

    நிலையான win_T *
get_win(மதிப்பு_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#இல்லையெனில்
    Data_Get_Struct(obj, win_T, win);
#endif
    எனில் (win == வெற்று)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    திரும்பு win;
}

    நிலையான மதிப்பு_3
window_s_current(மதிப்பு_3 self UNUSED)
{
    திரும்பு window_new(curwin);
}

    நிலையான மதிப்பு_3
window_s_current_getter(ID id UNUSED, மதிப்பு_3 *x UNUSED)
{
    திரும்பு window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    நிலையான மதிப்பு_3
வரி_s_current(மதிப்பு_3 self UNUSED)
{
    திரும்பு get_buffer_வரி(curbuf, curwin->w_cursor.lnum);
}

    நிலையான மதிப்பு_3
set_current_வரி(மதிப்பு_3 self UNUSED, மதிப்பு_3 str)
{
    திரும்பு set_buffer_வரி(curbuf, curwin->w_cursor.lnum, str);
}

    நிலையான மதிப்பு_3
current_வரி_எண்(மதிப்பு_3 self UNUSED)
{
    திரும்பு INT2FIX((முழு)curwin->w_cursor.lnum);
}

    நிலையான மதிப்பு_3
window_s_மொத்தம்(மதிப்பு_3 self UNUSED)
{
    win_T	*w;
    முழு n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    திரும்பு INT2_இலக்கம்(n);
}

    நிலையான மதிப்பு_3
window_s_aref(மதிப்பு_3 self UNUSED, மதிப்பு_3 இலக்கம்)
{
    win_T *w;
    முழு n = NUM2INT(இலக்கம்);

    க்கு (w = firstwin; w != வெற்று; w = w->w_next, --n)
	எனில் (n == 0)
	    திரும்பு window_new(w);
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
window_buffer(மதிப்பு_3 self)
{
    win_T *win = get_win(self);

    திரும்பு buffer_new(win->w_buffer);
}

    நிலையான மதிப்பு_3
window_height(மதிப்பு_3 self)
{
    win_T *win = get_win(self);

    திரும்பு INT2_இலக்கம்(win->w_height);
}

    நிலையான மதிப்பு_3
window_set_height(மதிப்பு_3 self, மதிப்பு_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    திரும்பு height;
}

    நிலையான மதிப்பு_3
window_width(மதிப்பு_3 self UNUSED)
{
    திரும்பு INT2_இலக்கம்(get_win(self)->w_width);
}

    நிலையான மதிப்பு_3
window_set_width(மதிப்பு_3 self UNUSED, மதிப்பு_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    திரும்பு width;
}

    நிலையான மதிப்பு_3
window_cursor(மதிப்பு_3 self)
{
    win_T *win = get_win(self);

    திரும்பு rb_assoc_new(INT2_இலக்கம்(win->w_cursor.lnum), INT2_இலக்கம்(win->w_cursor.col));
}

    நிலையான மதிப்பு_3
window_set_cursor(மதிப்பு_3 self, மதிப்பு_3 pos)
{
    மதிப்பு_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    எனில் (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
f_nop(மதிப்பு_3 self UNUSED)
{
    திரும்பு Qnil;
}

    நிலையான மதிப்பு_3
f_p(முழு argc, மதிப்பு_3 *argv, மதிப்பு_3 self UNUSED)
{
    முழு i;
    மதிப்பு_3 str = rb_str_new("", 0);
    மதிப்பு_3 ret = Qnil;

    க்கு (i = 0; i < argc; i++)
    {
	எனில் (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    செய்தி(RSTRING_PTR(str));

    எனில் (argc == 1)
	ret = argv[0];
    இல்லையெனில் எனில் (argc > 1)
	ret = rb_ary_new4(argc, argv);
    திரும்பு ret;
}

    நிலையான வெறுமை
ruby_io_init(வெறுமை)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN மதிப்பு_3 rb_stdout;
    RUBYEXTERN மதிப்பு_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (வெறுமை*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (வெறுமை*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (வெறுமை*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (வெறுமை*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    நிலையான வெறுமை
ruby_vim_init(வெறுமை)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_இலக்கம்(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_இலக்கம்(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_இலக்கம்(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_இலக்கம்(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (வெறுமை*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (வெறுமை*)buffer_s_மொத்தம், 0);
    rb_define_singleton_method(cBuffer, "[]", (வெறுமை*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (வெறுமை*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (வெறுமை*)buffer_எண், 0);
    rb_define_method(cBuffer, "count", (வெறுமை*)buffer_மொத்தம், 0);
    rb_define_method(cBuffer, "length", (வெறுமை*)buffer_மொத்தம், 0);
    rb_define_method(cBuffer, "[]", (வெறுமை*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (வெறுமை*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (வெறுமை*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (வெறுமை*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (வெறுமை*)current_வரி_எண், 0);
    rb_define_method(cBuffer, "line", (வெறுமை*)வரி_s_current, 0);
    rb_define_method(cBuffer, "line=", (வெறுமை*)set_current_வரி, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (வெறுமை*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (வெறுமை*)window_s_மொத்தம், 0);
    rb_define_singleton_method(cVimWindow, "[]", (வெறுமை*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (வெறுமை*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (வெறுமை*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (வெறுமை*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (வெறுமை*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (வெறுமை*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (வெறுமை*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (வெறுமை*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    வெறுமை
vim_ruby_init(வெறுமை *அடுக்கு_start)
{
    // should get machine stack start address early in main function
    ruby_அடுக்கு_start = அடுக்கு_start;
}

    நிலையான முழு
convert_hash2dict(மதிப்பு_3 சாவி, மதிப்பு_3 val, மதிப்பு_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(சாவி)));
    எனில் (di == வெற்று || ruby_convert_to_vim_மதிப்பு(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	திரும்பு ST_STOP;
    }
    திரும்பு ST_CONTINUE;
}

    நிலையான முழு
ruby_convert_to_vim_மதிப்பு(மதிப்பு_3 val, typval_T *rettv)
{
    தேர்வு (TYPE(val))
    {
	நிலை T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_எண் = VVAL_NULL;
	    நிறுத்து;
	நிலை T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_எண் = VVAL_TRUE;
	    நிறுத்து;
	நிலை T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_எண் = VVAL_FALSE;
	    நிறுத்து;
	நிலை T_BIGNUM:
	நிலை T_FIXNUM:
	    rettv->v_type = VAR_எண்;
	    rettv->vval.v_எண் = (varnumber_T)NUM2LONG(val);
	    நிறுத்து;
	நிலை T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    நிறுத்து;
	இயல்பு:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	நிலை T_STRING:
	    {
		மதிப்பு_3 str = (மதிப்பு_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    நிறுத்து;
	நிலை T_ARRAY:
	    {
		list_T *l;
		long i;
		typval_T v;

		l = list_alloc();
		எனில் (l == வெற்று)
		    திரும்பு FAIL;

		க்கு (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    எனில் (ruby_convert_to_vim_மதிப்பு((மதிப்பு_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			திரும்பு FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    நிறுத்து;
	நிலை T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		எனில் (d == வெற்று)
		    திரும்பு FAIL;

		rb_hash_foreach(val, convert_hash2dict, (மதிப்பு_3)d);
		எனில் (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    திரும்பு FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    நிறுத்து;
    }
    திரும்பு OK;
}

    வெறுமை
do_rubyeval(char_u *str, typval_T *rettv)
{
    முழு retval = FAIL;

    எனில் (ensure_ruby_initialized())
    {
	முழு state;
	மதிப்பு_3 obj;

	obj = rb_eval_string_protect((மாறிலி எழுத்து *)str, &state);
	எனில் (state)
	    error_print(state);
	இல்லையெனில்
	    retval = ruby_convert_to_vim_மதிப்பு(obj, rettv);
    }
    எனில் (retval == FAIL)
    {
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = 0;
    }
}
