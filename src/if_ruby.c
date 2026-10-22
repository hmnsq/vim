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

#अन्तर्भाव "protodef.h"
#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#ifdef _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#endif
#ifndef RUBYEXTERN
# define RUBYEXTERN बाह्य
#endif

#ifdef DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# जर RUBY_VERSION >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# जर defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# जर RUBY_VERSION >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_चिन्ह		(*dll_rb_c_चिन्ह)
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

# जर RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# जर RUBY_VERSION >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# जर RUBY_VERSION == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# जर RUBY_VERSION >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# जर RUBY_VERSION >= 26 && RUBY_VERSION <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# जर RUBY_VERSION >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_size_overflow	ruby_malloc_size_overflow_stub
# endif

# जर RUBY_VERSION >= 31
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

#जर defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ढकला
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#जर defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#endif

#अन्तर्भाव <ruby.h>
#अन्तर्भाव <ruby/encoding.h>

#जर defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic काढा
#endif

// See above.
#ifdef SIZEOF_TIME_T
# undef SIZEOF_TIME_T
#endif

#undef off_t	// ruby defines off_t as _int64, Mingw uses long
#undef EXTERN
#undef _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#जर defined(MACOS_X)
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
#जर defined(TypedData_Wrap_Struct) && (RUBY_VERSION >= 20)
# define USE_TYPEDDATA	1
#endif

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#ifndef String_मूल्य_Ptr
# define String_मूल्य_Ptr(s) STR2CSTR(s)
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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#ifdef DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define load_dll vimLoadLib
#  define चिन्ह_from_dll GetProcAddress
#  define close_dll FreeLibrary
#  define load_dll_error GetWin32Error
# अन्यथा
#  include <dlfcn.h>
#  define HINSTANCE शून्य*
#  define RUBY_PROC शून्य*
#  define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define चिन्ह_from_dll dlsym
#  define close_dll dlclose
#  define load_dll_error dlerror
# endif
#endif

स्थिर पूर्णांक ruby_initialized = 0;
स्थिर शून्य *ruby_चळत_start;
स्थिर मूल्य_3 objtbl;

स्थिर मूल्य_3 mVIM;
स्थिर मूल्य_3 cBuffer;
स्थिर मूल्य_3 cVimWindow;
स्थिर मूल्य_3 eDeletedBufferError;
स्थिर मूल्य_3 eDeletedWindowError;

स्थिर पूर्णांक ensure_ruby_initialized(शून्य);
स्थिर शून्य error_print(पूर्णांक);
स्थिर शून्य ruby_io_init(शून्य);
स्थिर शून्य ruby_vim_init(शून्य);
स्थिर पूर्णांक ruby_convert_to_vim_मूल्य(मूल्य_3 val, typval_T *rettv);

#जर defined(__ia64) && !defined(ruby_init_चळत)
# define ruby_init_चळत(addr) ruby_init_चळत((addr), rb_ia64_bsp())
#endif

#जर defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# जर RUBY_VERSION >= 27
#  undef rb_define_global_function
#  undef rb_define_method
#  undef rb_define_module_function
#  undef rb_define_singleton_method
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# जर RUBY_VERSION < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_path			dll_rb_class_path
# ifdef USE_TYPEDDATA
#  जर RUBY_VERSION >= 23
#   define rb_data_typed_object_wrap	dll_rb_data_typed_object_wrap
#  अन्यथा
#   define rb_data_typed_object_alloc	dll_rb_data_typed_object_alloc
#  endif
# अन्यथा
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
# जर RUBY_VERSION >= 21
#  define rb_funcallv			dll_rb_funcallv
# अन्यथा
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

# जर VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  जर RUBY_VERSION < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_set			dll_rb_lastline_set
# define rb_protect			dll_rb_protect
# define rb_load			dll_rb_load
# जर RUBY_VERSION < 20
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
# अन्यथा
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_मूल्य		dll_rb_string_मूल्य
# define rb_string_मूल्य_ptr		dll_rb_string_मूल्य_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_ढकला			dll_rb_ary_ढकला
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_चळत
#  define ruby_init_चळत(addr)	dll_ruby_init_चळत((addr), rb_ia64_bsp())
# अन्यथा
#  define ruby_init_चळत		dll_ruby_init_चळत
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
स्थिर मूल्य_3 (*dll_rb_assoc_new) (मूल्य_3, मूल्य_3);
मूल्य_3 *dll_rb_cFalseClass;
मूल्य_3 *dll_rb_cFixnum;
# जर defined(USE_RUBY_INTEGER)
मूल्य_3 *dll_rb_cInteger;
# endif
# जर RUBY_VERSION >= 20
मूल्य_3 *dll_rb_cFloat;
# endif
मूल्य_3 *dll_rb_cNilClass;
स्थिर मूल्य_3 *dll_rb_cObject;
मूल्य_3 *dll_rb_cString;
मूल्य_3 *dll_rb_c_चिन्ह;
मूल्य_3 *dll_rb_cTrueClass;
स्थिर मूल्य_3 (*dll_rb_class_new_instance) (पूर्णांक,मूल्य_3*,मूल्य_3);
स्थिर शून्य (*dll_rb_check_type) (मूल्य_3,पूर्णांक);
# ifdef USE_TYPEDDATA
स्थिर शून्य *(*dll_rb_check_typeddata) (मूल्य_3,स्थिरांक rb_data_type_t *);
# endif
स्थिर मूल्य_3 (*dll_rb_class_path) (मूल्य_3);
# ifdef USE_TYPEDDATA
#  जर RUBY_VERSION >= 23
स्थिर मूल्य_3 (*dll_rb_data_typed_object_wrap) (मूल्य_3, शून्य*, स्थिरांक rb_data_type_t *);
#  अन्यथा
स्थिर मूल्य_3 (*dll_rb_data_typed_object_alloc) (मूल्य_3, शून्य*, स्थिरांक rb_data_type_t *);
#  endif
# अन्यथा
स्थिर मूल्य_3 (*dll_rb_data_object_alloc) (मूल्य_3, शून्य*, RUBY_DATA_FUNC, RUBY_DATA_FUNC);
# endif
# जर RUBY_VERSION >= 31
स्थिर शून्य (*dll_rb_debug_rstring_null_ptr) (स्थिरांक अक्षर*);
# endif
स्थिर मूल्य_3 (*dll_rb_define_class_under) (मूल्य_3, स्थिरांक अक्षर*, मूल्य_3);
स्थिर शून्य (*dll_rb_define_const) (मूल्य_3,स्थिरांक अक्षर*,मूल्य_3);
स्थिर शून्य (*dll_rb_define_global_function) (स्थिरांक अक्षर*,मूल्य_3(*)(पूर्णांक,मूल्य_3*,मूल्य_3),पूर्णांक);
स्थिर शून्य (*dll_rb_define_method) (मूल्य_3,स्थिरांक अक्षर*,मूल्य_3(*)(ANYARGS),पूर्णांक);
स्थिर मूल्य_3 (*dll_rb_define_module) (स्थिरांक अक्षर*);
स्थिर शून्य (*dll_rb_define_module_function) (मूल्य_3,स्थिरांक अक्षर*,मूल्य_3(*)(मूल्य_3,मूल्य_3),पूर्णांक);
स्थिर शून्य (*dll_rb_define_singleton_method) (मूल्य_3,स्थिरांक अक्षर*,मूल्य_3(*)(ANYARGS),पूर्णांक);
स्थिर शून्य (*dll_rb_define_virtual_variable) (स्थिरांक अक्षर*,मूल्य_3(*)(ID,मूल्य_3*),शून्य(*)());
स्थिर मूल्य_3 *dll_rb_stdout;
स्थिर मूल्य_3 *dll_rb_stderr;
स्थिर मूल्य_3 *dll_rb_eArgError;
स्थिर मूल्य_3 *dll_rb_eIndexError;
स्थिर मूल्य_3 *dll_rb_eRuntimeError;
स्थिर मूल्य_3 *dll_rb_eStandardError;
स्थिर मूल्य_3 (*dll_rb_eval_string_protect) (स्थिरांक अक्षर*, पूर्णांक*);
# जर RUBY_VERSION >= 21
स्थिर मूल्य_3 (*dll_rb_funcallv) (मूल्य_3, ID, पूर्णांक, स्थिरांक मूल्य_3*);
# अन्यथा
स्थिर मूल्य_3 (*dll_rb_funcall2) (मूल्य_3, ID, पूर्णांक, स्थिरांक मूल्य_3*);
# endif
स्थिर शून्य (*dll_rb_global_variable) (मूल्य_3*);
स्थिर मूल्य_3 (*dll_rb_hash_aset) (मूल्य_3, मूल्य_3, मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_hash_foreach) (मूल्य_3, पूर्णांक (*)(मूल्य_3, मूल्य_3, मूल्य_3), मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_hash_new) (शून्य);
स्थिर मूल्य_3 (*dll_rb_inspect) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_int2inum) (दीर्घ);
स्थिर ID (*dll_rb_intern) (स्थिरांक अक्षर*);
# जर RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
स्थिर दीर्घ (*dll_rb_fix2int) (मूल्य_3);
स्थिर दीर्घ (*dll_rb_num2int) (मूल्य_3);
स्थिर अचिन्हित दीर्घ (*dll_rb_num2uint) (मूल्य_3);
# endif
स्थिर द्विगुण (*dll_rb_num2dbl) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_lastline_get) (शून्य);
स्थिर शून्य (*dll_rb_lastline_set) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_protect) (मूल्य_3 (*)(मूल्य_3), मूल्य_3, पूर्णांक*);
स्थिर शून्य (*dll_rb_load) (मूल्य_3, पूर्णांक);
स्थिर दीर्घ (*dll_rb_num2long) (मूल्य_3);
स्थिर अचिन्हित दीर्घ (*dll_rb_num2ulong) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_obj_alloc) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_obj_as_string) (मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_obj_id) (मूल्य_3);
स्थिर शून्य (*dll_rb_raise) (मूल्य_3, स्थिरांक अक्षर*, ...);
स्थिर मूल्य_3 (*dll_rb_string_मूल्य) (अस्थिर मूल्य_3*);
स्थिर मूल्य_3 (*dll_rb_str_cat) (मूल्य_3, स्थिरांक अक्षर*, दीर्घ);
स्थिर मूल्य_3 (*dll_rb_str_concat) (मूल्य_3, मूल्य_3);
स्थिर मूल्य_3 (*dll_rb_str_new) (स्थिरांक अक्षर*, दीर्घ);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
स्थिर मूल्य_3 (*dll_rb_str_new_cstr) (स्थिरांक अक्षर*);
# अन्यथा
स्थिर मूल्य_3 (*dll_rb_str_new2) (स्थिरांक अक्षर*);
# endif
स्थिर मूल्य_3 (*dll_rb_errinfo) (शून्य);
स्थिर शून्य (*dll_ruby_init) (शून्य);
स्थिर शून्य (*dll_ruby_init_loadpath) (शून्य);
# ifdef MSWIN
स्थिर शून्य (*dll_ruby_sysinit) (पूर्णांक*, अक्षर***);
स्थिर पूर्णांक (*dll_rb_w32_snprintf)(अक्षर*, माप_प्रकार, स्थिरांक अक्षर*, ...);
# endif
# जर RUBY_VERSION >= 31
#  ifdef _MSC_VER
स्थिर शून्य (*dll_rb_unexpected_type) (मूल्य_3, पूर्णांक);
#  अन्यथा
NORETURN(स्थिर शून्य (*dll_rb_unexpected_type) (मूल्य_3, पूर्णांक));
#  endif
# endif
स्थिर अक्षर * (*dll_rb_string_मूल्य_ptr) (अस्थिर मूल्य_3*);
स्थिर मूल्य_3 (*dll_rb_float_new) (द्विगुण);
स्थिर मूल्य_3 (*dll_rb_ary_new) (शून्य);
स्थिर मूल्य_3 (*dll_rb_ary_new4) (दीर्घ n, स्थिरांक मूल्य_3 *elts);
स्थिर मूल्य_3 (*dll_rb_ary_ढकला) (मूल्य_3, मूल्य_3);
# जर RUBY_VERSION >= 26 && RUBY_VERSION <= 32
स्थिर शून्य (*dll_rb_ary_detransient) (मूल्य_3);
# endif
# ifdef __ia64
स्थिर शून्य * (*dll_rb_ia64_bsp) (शून्य);
स्थिर शून्य (*dll_ruby_init_चळत)(मूल्य_3*, शून्य*);
# अन्यथा
स्थिर शून्य (*dll_ruby_init_चळत)(मूल्य_3*);
# endif
स्थिर मूल्य_3 (*dll_rb_int2big)(SIGNED_मूल्य);

स्थिर शून्य (*dll_ruby_script) (स्थिरांक अक्षर*);
स्थिर पूर्णांक (*dll_rb_enc_find_index) (स्थिरांक अक्षर*);
स्थिर rb_encoding* (*dll_rb_enc_find) (स्थिरांक अक्षर*);
स्थिर मूल्य_3 (*dll_rb_enc_str_new) (स्थिरांक अक्षर*, दीर्घ, rb_encoding*);
स्थिर मूल्य_3 (*dll_rb_sprintf) (स्थिरांक अक्षर*, ...);
स्थिर मूल्य_3 (*dll_rb_require) (स्थिरांक अक्षर*);
स्थिर शून्य* (*dll_ruby_options)(पूर्णांक, अक्षर**);

# जर defined(USE_RGENGC) && USE_RGENGC
#  जर RUBY_VERSION == 21
स्थिर शून्य (*dll_rb_gc_writebarrier_unprotect_promoted)(मूल्य_3);
#  अन्यथा
स्थिर शून्य (*dll_rb_gc_writebarrier_unprotect)(मूल्य_3 obj);
#  endif
# endif

# जर RUBY_VERSION >= 30
#  ifdef _MSC_VER
स्थिर शून्य (*dll_ruby_malloc_size_overflow)(माप_प्रकार, माप_प्रकार);
#  अन्यथा
NORETURN(स्थिर शून्य (*dll_ruby_malloc_size_overflow)(माप_प्रकार, माप_प्रकार));
#  endif
# endif

# जर RUBY_VERSION >= 26 && RUBY_VERSION <= 32
शून्य rb_ary_detransient_stub(मूल्य_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  जर RUBY_VERSION >= 22
    दीर्घ
rb_num2long_stub(मूल्य_3 x)
#  अन्यथा
    SIGNED_मूल्य
rb_num2long_stub(मूल्य_3 x)
#  endif
{
    परत dll_rb_num2long(x);
}
#  जर RUBY_VERSION >= 26
    मूल्य_3
rb_int2big_stub(intptr_t x)
#  अन्यथा
    मूल्य_3
rb_int2big_stub(SIGNED_मूल्य x)
#  endif
{
    परत dll_rb_int2big(x);
}
#  जर RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    दीर्घ
rb_fix2int_stub(मूल्य_3 x)
{
    परत dll_rb_fix2int(x);
}
    दीर्घ
rb_num2int_stub(मूल्य_3 x)
{
    परत dll_rb_num2int(x);
}
#  endif
#  जर RUBY_VERSION >= 20
    मूल्य_3
rb_float_new_in_heap(द्विगुण d)
{
    परत dll_rb_float_new(d);
}
#   जर RUBY_VERSION >= 22
    अचिन्हित दीर्घ
rb_num2ulong(मूल्य_3 x)
#   अन्यथा
    मूल्य_3
rb_num2ulong(मूल्य_3 x)
#   endif
{
    परत (दीर्घ)RSHIFT((SIGNED_मूल्य)(x),1);
}
#  endif
#  जर defined(USE_RGENGC) && USE_RGENGC
#   जर RUBY_VERSION == 21
    शून्य
rb_gc_writebarrier_unprotect_promoted_stub(मूल्य_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   अन्यथा
    शून्य
rb_gc_writebarrier_unprotect_stub(मूल्य_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  जर RUBY_VERSION >= 26 && RUBY_VERSION <= 32
    शून्य
rb_ary_detransient_stub(मूल्य_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  जर RUBY_VERSION >= 30
    शून्य
rb_check_type_stub(मूल्य_3 obj, पूर्णांक t)
{
    dll_rb_check_type(obj, t);
}
    अचिन्हित दीर्घ
rb_num2uint_stub(मूल्य_3 x)
{
    परत dll_rb_num2uint(x);
}
    शून्य
ruby_malloc_size_overflow_stub(माप_प्रकार x, माप_प्रकार y)
{
    dll_ruby_malloc_size_overflow(x, y);
}
#  endif
#  जर RUBY_VERSION >= 31
    शून्य
rb_debug_rstring_null_ptr_stub(स्थिरांक अक्षर *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    शून्य
rb_unexpected_type_stub(मूल्य_3 self, पूर्णांक t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
शून्य *rb_check_typeddata_stub(मूल्य_3 obj, स्थिरांक rb_data_type_t *data_type)
{
    परत dll_rb_check_typeddata(obj, data_type);
}
#  endif
# endif // ifndef PROTO

स्थिर HINSTANCE hinstRuby = रिक्त; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
स्थिर संरचना
{
    अक्षर *name;
    RUBY_PROC *ptr;
} ruby_funcname_table[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# जर defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# अन्यथा
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# जर RUBY_VERSION >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_चिन्ह},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_path},
# ifdef USE_TYPEDDATA
#  जर RUBY_VERSION >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_data_typed_object_wrap},
#  अन्यथा
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_data_typed_object_alloc},
#  endif
# अन्यथा
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_data_object_alloc},
# endif
# जर RUBY_VERSION >= 31
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
# जर RUBY_VERSION >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# अन्यथा
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# जर RUBY_VERSION >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
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
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_मूल्य},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# अन्यथा
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# जर RUBY_VERSION >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_मूल्य_ptr},
# जर RUBY_VERSION >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# अन्यथा
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# अन्यथा
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_ढकला},
# जर RUBY_VERSION >= 26 && RUBY_VERSION <= 32
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
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_चळत},
# जर defined(USE_RGENGC) && USE_RGENGC
#  जर RUBY_VERSION == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  अन्यथा
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# जर RUBY_VERSION >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_size_overflow},
# endif
    {"", रिक्त},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
ruby_runtime_link_init(अक्षर *libname, पूर्णांक verbose)
{
    पूर्णांक i;

    जर (hinstRuby)
	परत OK;
    hinstRuby = load_dll(libname);
    जर (!hinstRuby)
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	परत FAIL;
    }

    साठी (i = 0; ruby_funcname_table[i].ptr; ++i)
    {
	जर (!(*ruby_funcname_table[i].ptr = चिन्ह_from_dll(hinstRuby,
			ruby_funcname_table[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = रिक्त;
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), ruby_funcname_table[i].name);
	    परत FAIL;
	}
    }
    परत OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    पूर्णांक
ruby_enabled(पूर्णांक verbose)
{
    परत ruby_runtime_link_init((अक्षर *)p_rubydll, verbose) == OK;
}
#endif // defined(DYNAMIC_RUBY)

    शून्य
ruby_end(शून्य)
{
}

    शून्य
ex_ruby(exarg_T *eap)
{
    पूर्णांक state;
    अक्षर *script = रिक्त;

    script = (अक्षर *)script_get(eap, eap->arg);
    जर (!eap->skip && ensure_ruby_initialized())
    {
	जर (script == रिक्त)
	    rb_eval_string_protect((अक्षर *)eap->arg, &state);
	अन्यथा
	    rb_eval_string_protect(script, &state);
	जर (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    स्थिर मूल्य_3
vim_str2rb_enc_str(स्थिरांक अक्षर *s)
{
    दीर्घ lval;
    char_u *sval;
    rb_encoding *enc;

    जर (get_option_मूल्य((char_u *)"enc", &lval, &sval, रिक्त, 0) == gov_string)
    {
	enc = rb_enc_find((अक्षर *)sval);
	vim_free(sval);
	जर (enc)
	    परत rb_enc_str_new(s, (दीर्घ)म_माप(s), enc);
    }
    परत rb_str_new2(s);
}

    स्थिर मूल्य_3
eval_enc_string_protect(स्थिरांक अक्षर *str, पूर्णांक *state)
{
    दीर्घ lval;
    char_u *sval;
    rb_encoding *enc;
    मूल्य_3 v;

    जर (get_option_मूल्य((char_u *)"enc", &lval, &sval, रिक्त, 0) == gov_string)
    {
	enc = rb_enc_find((अक्षर *)sval);
	vim_free(sval);
	जर (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    परत rb_eval_string_protect(String_मूल्य_Ptr(v), state);
	}
    }
    परत rb_eval_string_protect(str, state);
}

    शून्य
ex_rubydo(exarg_T *eap)
{
    पूर्णांक state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    जर (!ensure_ruby_initialized())
	परत;

    जर (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	परत;
    साठी (i = eap->line1; i <= eap->line2; i++)
    {
	मूल्य_3 ओळ;

	जर (i > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;
	ओळ = vim_str2rb_enc_str((अक्षर *)ml_get(i));
	rb_lastline_set(ओळ);
	eval_enc_string_protect((अक्षर *) eap->arg, &state);
	जर (state)
	{
	    error_print(state);
	    थांबा;
	}
	जर (was_curbuf != curbuf || i > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;
	ओळ = rb_lastline_get();
	जर (!NIL_P(ओळ))
	{
	    जर (TYPE(ओळ) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_of_string));
		परत;
	    }
	    ml_replace(i, (char_u *) String_मूल्य_Ptr(ओळ), 1);
	    changed();
#ifdef SYNTAX_HL
	    syn_changed(i); // recompute syntax hl. for this line
#endif
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    स्थिर मूल्य_3
rb_load_wrap(मूल्य_3 file_to_load)
{
    rb_load(file_to_load, 0);
    परत Qnil;
}

    शून्य
ex_rubyfile(exarg_T *eap)
{
    पूर्णांक state;

    जर (!ensure_ruby_initialized())
	परत;

    मूल्य_3 file_to_load = rb_str_new2((स्थिरांक अक्षर *)eap->arg);
    rb_protect(rb_load_wrap, file_to_load, &state);
    जर (state)
	error_print(state);
}

    शून्य
ruby_buffer_free(buf_T *buf)
{
    जर (buf->b_ruby_ref == रिक्त)
	परत;

    rb_hash_aset(objtbl, rb_obj_id((मूल्य_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->data = रिक्त;
}

    शून्य
ruby_window_free(win_T *win)
{
    जर (win->w_ruby_ref == रिक्त)
	परत;

    rb_hash_aset(objtbl, rb_obj_id((मूल्य_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->data = रिक्त;
}

    स्थिर पूर्णांक
ensure_ruby_initialized(शून्य)
{
    जर (ruby_initialized)
	परत ruby_initialized;

#ifdef DYNAMIC_RUBY
    जर (ruby_enabled(TRUE))
#endif
    {
#ifdef MSWIN
	// suggested by Ariya Mizutani
	पूर्णांक argc = 1;
	अक्षर *argv[] = {"gvim.exe"};
	अक्षर **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#endif
	{
	    ruby_init_चळत(ruby_चळत_start);
	    ruby_init();
	}
	{
	    पूर्णांक dummy_argc = 2;
	    अक्षर *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#ifdef DYNAMIC_RUBY
    अन्यथा
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	परत 0;
    }
#endif

    परत ruby_initialized;
}

    स्थिर शून्य
error_print(पूर्णांक state)
{
    मूल्य_3 error;
    मूल्य_3 eclass;
    मूल्य_3 einfo;
    मूल्य_3 bt;
    पूर्णांक attr;
    अक्षर buff[बफ_मान];
    दीर्घ i;

#define TAG_RETURN	0x1
#define TAG_BREAK	0x2
#define TAG_NEXT	0x3
#define TAG_RETRY	0x4
#define TAG_REDO	0x5
#define TAG_RAISE	0x6
#define TAG_THROW	0x7
#define TAG_FATAL	0x8
#define TAG_MASK	0xf

    निवड (state)
    {
	स्थिती TAG_RETURN:
	    emsg(_(e_unexpected_return));
	    थांबा;
	स्थिती TAG_NEXT:
	    emsg(_(e_unexpected_next));
	    थांबा;
	स्थिती TAG_BREAK:
	    emsg(_(e_unexpected_break));
	    थांबा;
	स्थिती TAG_REDO:
	    emsg(_(e_unexpected_redo));
	    थांबा;
	स्थिती TAG_RETRY:
	    emsg(_(e_retry_outside_of_rescue_clause));
	    थांबा;
	स्थिती TAG_RAISE:
	स्थिती TAG_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_OF(error);
	    einfo = rb_obj_as_string(error);
	    जर (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    अन्यथा
	    {
		मूल्य_3 epath;
		अक्षर *p;

		epath = rb_class_path(eclass);
		vim_snprintf(buff, बफ_मान, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = म_अक्षर(buff, '\n');
		जर (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#जर RUBY_VERSION >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    साठी (i = 0; i < RARRAY_LEN(bt); i++)
		संदेश_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#अन्यथा
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    साठी (i = 0; i < RARRAY_LEN(bt); i++)
		संदेश_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#endif
	    थांबा;
	पूर्वनिर्धारित:
	    vim_snprintf(buff, बफ_मान, _(e_unknown_longjmp_status_nr), state);
	    emsg(buff);
	    थांबा;
    }
}

    स्थिर मूल्य_3
vim_message(मूल्य_3 self UNUSED, मूल्य_3 str)
{
    अक्षर *buff, *p;

    str = rb_obj_as_string(str);
    जर (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(अक्षर, RSTRING_LEN(str) + 1);
	म_नकल(buff, RSTRING_PTR(str));
	p = म_अक्षर(buff, '\n');
	जर (p) *p = '\0';
	संदेश(buff);
    }
    अन्यथा
    {
	संदेश("");
    }
    परत Qnil;
}

    स्थिर मूल्य_3
vim_set_option(मूल्य_3 self UNUSED, मूल्य_3 str)
{
    do_set((char_u *)String_मूल्य_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    परत Qnil;
}

    स्थिर मूल्य_3
vim_command(मूल्य_3 self UNUSED, मूल्य_3 str)
{
    do_cmdline_cmd((char_u *)String_मूल्य_Ptr(str));
    परत Qnil;
}

#ifdef FEAT_EVAL
    स्थिर मूल्य_3
vim_to_ruby(typval_T *tv)
{
    मूल्य_3 result = Qnil;

    जर (tv->v_type == VAR_STRING)
    {
	result = rb_str_new2(tv->vval.v_string == रिक्त
					  ? "" : (अक्षर *)(tv->vval.v_string));
    }
    अन्यथा जर (tv->v_type == VAR_संख्या)
    {
	result = INT2_अंक(tv->vval.v_संख्या);
    }
    अन्यथा जर (tv->v_type == VAR_FLOAT)
    {
	result = rb_float_new(tv->vval.v_float);
    }
    अन्यथा जर (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	result = rb_ary_new();

	जर (list != रिक्त)
	{
	    FOR_ALL_LIST_वस्तुसंच(list, curr)
		rb_ary_ढकला(result, vim_to_ruby(&curr->li_tv));
	}
    }
    अन्यथा जर (tv->v_type == VAR_DICT)
    {
	result = rb_hash_new();

	जर (tv->vval.v_dict != रिक्त)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_used;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
	    {
		जर (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(result, rb_str_new2((अक्षर *)hi->hi_किल्ली),
						     vim_to_ruby(&di->di_tv));
		}
	    }
	}
    }
    अन्यथा जर (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	जर (tv->vval.v_संख्या == VVAL_TRUE)
	    result = Qtrue;
	अन्यथा जर (tv->vval.v_संख्या == VVAL_FALSE)
	    result = Qfalse;
    }
    अन्यथा जर (tv->v_type == VAR_BLOB)
    {
	result = rb_str_new(tv->vval.v_blob->bv_ga.ga_data,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    परत result;
}
#endif

    स्थिर मूल्य_3
vim_evaluate(मूल्य_3 self UNUSED, मूल्य_3 str)
{
#ifdef FEAT_EVAL
    typval_T    *tv;
    मूल्य_3       result;

    tv = eval_अभिव्यक्ती((char_u *)String_मूल्य_Ptr(str), रिक्त);
    जर (tv == रिक्त)
	परत Qnil;
    result = vim_to_ruby(tv);

    free_tv(tv);

    परत result;
#अन्यथा
    परत Qnil;
#endif
}

#ifdef USE_TYPEDDATA
स्थिर माप_प्रकार buffer_dsize(स्थिरांक शून्य *buf);

स्थिर स्थिरांक rb_data_type_t buffer_type = {
    "vim_buffer",
    {0, 0, buffer_dsize,
# जर RUBY_VERSION >= 27
	0, {0}
# अन्यथा
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    स्थिर माप_प्रकार
buffer_dsize(स्थिरांक शून्य *buf UNUSED)
{
    परत आकार(buf_T);
}
#endif

    स्थिर मूल्य_3
buffer_new(buf_T *buf)
{
    जर (buf->b_ruby_ref)
    {
	परत (मूल्य_3) buf->b_ruby_ref;
    }
    अन्यथा
    {
#ifdef USE_TYPEDDATA
	मूल्य_3 obj = TypedData_Wrap_Struct(cBuffer, &buffer_type, buf);
#अन्यथा
	मूल्य_3 obj = Data_Wrap_Struct(cBuffer, 0, 0, buf);
#endif
	buf->b_ruby_ref = (शून्य *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	परत obj;
    }
}

    स्थिर buf_T *
get_buf(मूल्य_3 obj)
{
    buf_T *buf;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, buf_T, &buffer_type, buf);
#अन्यथा
    Data_Get_Struct(obj, buf_T, buf);
#endif
    जर (buf == रिक्त)
	rb_raise(eDeletedBufferError, "attempt to refer to deleted buffer");
    परत buf;
}

    स्थिर मूल्य_3
vim_blob(मूल्य_3 self UNUSED, मूल्य_3 str)
{
    मूल्य_3 result = rb_str_new("0z", 2);
    अक्षर    buf[4];
    पूर्णांक	i;
    साठी (i = 0; i < RSTRING_LEN(str); i++)
    {
	प्र_लिखो(buf, "%02X", (अचिन्हित अक्षर)(RSTRING_PTR(str)[i]));
	rb_str_concat(result, rb_str_new2(buf));
    }
    परत result;
}

    स्थिर मूल्य_3
buffer_s_current(मूल्य_3 self UNUSED)
{
    परत buffer_new(curbuf);
}

    स्थिर मूल्य_3
buffer_s_current_getter(ID id UNUSED, मूल्य_3 *x UNUSED)
{
    परत buffer_new(curbuf);
}

    स्थिर मूल्य_3
buffer_s_मोजणी(मूल्य_3 self UNUSED)
{
    buf_T *b;
    पूर्णांक n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	जर (b->b_p_bl)
	    n++;
    }

    परत INT2_अंक(n);
}

    स्थिर मूल्य_3
buffer_s_aref(मूल्य_3 self UNUSED, मूल्य_3 अंक)
{
    buf_T *b;
    पूर्णांक n = NUM2INT(अंक);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	जर (!b->b_p_bl)
	    सुरूठेवा;

	जर (n == 0)
	    परत buffer_new(b);

	n--;
    }
    परत Qnil;
}

    स्थिर मूल्य_3
buffer_name(मूल्य_3 self)
{
    buf_T *buf = get_buf(self);

    परत buf->b_ffname ? rb_str_new2((अक्षर *)buf->b_ffname) : Qnil;
}

    स्थिर मूल्य_3
buffer_संख्या(मूल्य_3 self)
{
    buf_T *buf = get_buf(self);

    परत INT2_अंक(buf->b_fnum);
}

    स्थिर मूल्य_3
buffer_मोजणी(मूल्य_3 self)
{
    buf_T *buf = get_buf(self);

    परत INT2_अंक(buf->b_ml.ml_ओळ_मोजणी);
}

    स्थिर मूल्य_3
get_buffer_ओळ(buf_T *buf, linenr_T n)
{
    जर (n <= 0 || n > buf->b_ml.ml_ओळ_मोजणी)
	rb_raise(rb_eIndexError, "line number %ld out of range", (दीर्घ)n);
    परत vim_str2rb_enc_str((अक्षर *)ml_get_buf(buf, n, FALSE));
}

    स्थिर मूल्य_3
buffer_aref(मूल्य_3 self, मूल्य_3 अंक)
{
    buf_T *buf = get_buf(self);

    जर (buf != रिक्त)
	परत get_buffer_ओळ(buf, (linenr_T)NUM2LONG(अंक));
    परत Qnil; // For stop warning
}

    स्थिर मूल्य_3
set_buffer_ओळ(buf_T *buf, linenr_T n, मूल्य_3 str)
{
    अक्षर	*ओळ = String_मूल्य_Ptr(str);
    aco_save_T	aco;

    जर (n > 0 && n <= buf->b_ml.ml_ओळ_मोजणी && ओळ != रिक्त)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	जर (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    जर (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)ओळ, TRUE);
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
    अन्यथा
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", (दीर्घ)n);
    }
    परत str;
}

    स्थिर मूल्य_3
buffer_aset(मूल्य_3 self, मूल्य_3 अंक, मूल्य_3 str)
{
    buf_T *buf = get_buf(self);

    जर (buf != रिक्त)
	परत set_buffer_ओळ(buf, (linenr_T)NUM2LONG(अंक), str);
    परत str;
}

    स्थिर मूल्य_3
buffer_delete(मूल्य_3 self, मूल्य_3 अंक)
{
    buf_T	*buf = get_buf(self);
    दीर्घ	n = NUM2LONG(अंक);
    aco_save_T	aco;

    जर (n > 0 && n <= buf->b_ml.ml_ओळ_मोजणी)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	जर (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    जर (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_ओळी_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    अन्यथा
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    परत Qnil;
}

    स्थिर मूल्य_3
buffer_append(मूल्य_3 self, मूल्य_3 अंक, मूल्य_3 str)
{
    buf_T	*buf = get_buf(self);
    अक्षर	*ओळ = String_मूल्य_Ptr(str);
    दीर्घ	n = NUM2LONG(अंक);
    aco_save_T	aco;

    जर (ओळ == रिक्त)
    {
	rb_raise(rb_eIndexError, "NULL line");
    }
    अन्यथा जर (n >= 0 && n <= buf->b_ml.ml_ओळ_मोजणी)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	जर (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    जर (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) ओळ, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_ओळी_mark(n, 1L);

		changed();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    अन्यथा
    {
	rb_raise(rb_eIndexError, "line number %ld out of range", n);
    }
    परत str;
}

#ifdef USE_TYPEDDATA
स्थिर माप_प्रकार window_dsize(स्थिरांक शून्य *buf);

स्थिर स्थिरांक rb_data_type_t window_type = {
    "vim_window",
    {0, 0, window_dsize,
# जर RUBY_VERSION >= 27
	0, {0}
# अन्यथा
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    स्थिर माप_प्रकार
window_dsize(स्थिरांक शून्य *win UNUSED)
{
    परत आकार(win_T);
}
#endif

    स्थिर मूल्य_3
window_new(win_T *win)
{
    जर (win->w_ruby_ref)
    {
	परत (मूल्य_3) win->w_ruby_ref;
    }
    अन्यथा
    {
#ifdef USE_TYPEDDATA
	मूल्य_3 obj = TypedData_Wrap_Struct(cVimWindow, &window_type, win);
#अन्यथा
	मूल्य_3 obj = Data_Wrap_Struct(cVimWindow, 0, 0, win);
#endif
	win->w_ruby_ref = (शून्य *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	परत obj;
    }
}

    स्थिर win_T *
get_win(मूल्य_3 obj)
{
    win_T *win;

#ifdef USE_TYPEDDATA
    TypedData_Get_Struct(obj, win_T, &window_type, win);
#अन्यथा
    Data_Get_Struct(obj, win_T, win);
#endif
    जर (win == रिक्त)
	rb_raise(eDeletedWindowError, "attempt to refer to deleted window");
    परत win;
}

    स्थिर मूल्य_3
window_s_current(मूल्य_3 self UNUSED)
{
    परत window_new(curwin);
}

    स्थिर मूल्य_3
window_s_current_getter(ID id UNUSED, मूल्य_3 *x UNUSED)
{
    परत window_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    स्थिर मूल्य_3
ओळ_s_current(मूल्य_3 self UNUSED)
{
    परत get_buffer_ओळ(curbuf, curwin->w_cursor.lnum);
}

    स्थिर मूल्य_3
set_current_ओळ(मूल्य_3 self UNUSED, मूल्य_3 str)
{
    परत set_buffer_ओळ(curbuf, curwin->w_cursor.lnum, str);
}

    स्थिर मूल्य_3
current_ओळ_संख्या(मूल्य_3 self UNUSED)
{
    परत INT2FIX((पूर्णांक)curwin->w_cursor.lnum);
}

    स्थिर मूल्य_3
window_s_मोजणी(मूल्य_3 self UNUSED)
{
    win_T	*w;
    पूर्णांक n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    परत INT2_अंक(n);
}

    स्थिर मूल्य_3
window_s_aref(मूल्य_3 self UNUSED, मूल्य_3 अंक)
{
    win_T *w;
    पूर्णांक n = NUM2INT(अंक);

    साठी (w = firstwin; w != रिक्त; w = w->w_next, --n)
	जर (n == 0)
	    परत window_new(w);
    परत Qnil;
}

    स्थिर मूल्य_3
window_buffer(मूल्य_3 self)
{
    win_T *win = get_win(self);

    परत buffer_new(win->w_buffer);
}

    स्थिर मूल्य_3
window_height(मूल्य_3 self)
{
    win_T *win = get_win(self);

    परत INT2_अंक(win->w_height);
}

    स्थिर मूल्य_3
window_set_height(मूल्य_3 self, मूल्य_3 height)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(height));
    curwin = savewin;
    परत height;
}

    स्थिर मूल्य_3
window_width(मूल्य_3 self UNUSED)
{
    परत INT2_अंक(get_win(self)->w_width);
}

    स्थिर मूल्य_3
window_set_width(मूल्य_3 self UNUSED, मूल्य_3 width)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(width));
    curwin = savewin;
    परत width;
}

    स्थिर मूल्य_3
window_cursor(मूल्य_3 self)
{
    win_T *win = get_win(self);

    परत rb_assoc_new(INT2_अंक(win->w_cursor.lnum), INT2_अंक(win->w_cursor.col));
}

    स्थिर मूल्य_3
window_set_cursor(मूल्य_3 self, मूल्य_3 pos)
{
    मूल्य_3 lnum, col;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    जर (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    col = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.col = NUM2UINT(col);
    win->w_set_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    परत Qnil;
}

    स्थिर मूल्य_3
f_nop(मूल्य_3 self UNUSED)
{
    परत Qnil;
}

    स्थिर मूल्य_3
f_p(पूर्णांक argc, मूल्य_3 *argv, मूल्य_3 self UNUSED)
{
    पूर्णांक i;
    मूल्य_3 str = rb_str_new("", 0);
    मूल्य_3 ret = Qnil;

    साठी (i = 0; i < argc; i++)
    {
	जर (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    संदेश(RSTRING_PTR(str));

    जर (argc == 1)
	ret = argv[0];
    अन्यथा जर (argc > 1)
	ret = rb_ary_new4(argc, argv);
    परत ret;
}

    स्थिर शून्य
ruby_io_init(शून्य)
{
#ifndef DYNAMIC_RUBY
    RUBYEXTERN मूल्य_3 rb_stdout;
    RUBYEXTERN मूल्य_3 rb_stderr;
#endif

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(rb_stdout, "write", (शून्य*)vim_message, 1);
    rb_define_singleton_method(rb_stdout, "flush", (शून्य*)f_nop, 0);
    rb_define_singleton_method(rb_stderr, "write", (शून्य*)vim_message, 1);
    rb_define_singleton_method(rb_stderr, "flush", (शून्य*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    स्थिर शून्य
ruby_vim_init(शून्य)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_अंक(VIM_VERSION_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_अंक(VIM_VERSION_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_अंक(VIM_VERSION_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_अंक(VIM_VERSION_PATCHLEVEL));
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
    rb_define_singleton_method(cBuffer, "current", (शून्य*)buffer_s_current, 0);
    rb_define_singleton_method(cBuffer, "count", (शून्य*)buffer_s_मोजणी, 0);
    rb_define_singleton_method(cBuffer, "[]", (शून्य*)buffer_s_aref, 1);
    rb_define_method(cBuffer, "name", (शून्य*)buffer_name, 0);
    rb_define_method(cBuffer, "number", (शून्य*)buffer_संख्या, 0);
    rb_define_method(cBuffer, "count", (शून्य*)buffer_मोजणी, 0);
    rb_define_method(cBuffer, "length", (शून्य*)buffer_मोजणी, 0);
    rb_define_method(cBuffer, "[]", (शून्य*)buffer_aref, 1);
    rb_define_method(cBuffer, "[]=", (शून्य*)buffer_aset, 2);
    rb_define_method(cBuffer, "delete", (शून्य*)buffer_delete, 1);
    rb_define_method(cBuffer, "append", (शून्य*)buffer_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_method(cBuffer, "line_number", (शून्य*)current_ओळ_संख्या, 0);
    rb_define_method(cBuffer, "line", (शून्य*)ओळ_s_current, 0);
    rb_define_method(cBuffer, "line=", (शून्य*)set_current_ओळ, 1);


    cVimWindow = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_method(cVimWindow, "current", (शून्य*)window_s_current, 0);
    rb_define_singleton_method(cVimWindow, "count", (शून्य*)window_s_मोजणी, 0);
    rb_define_singleton_method(cVimWindow, "[]", (शून्य*)window_s_aref, 1);
    rb_define_method(cVimWindow, "buffer", (शून्य*)window_buffer, 0);
    rb_define_method(cVimWindow, "height", (शून्य*)window_height, 0);
    rb_define_method(cVimWindow, "height=", (शून्य*)window_set_height, 1);
    rb_define_method(cVimWindow, "width", (शून्य*)window_width, 0);
    rb_define_method(cVimWindow, "width=", (शून्य*)window_set_width, 1);
    rb_define_method(cVimWindow, "cursor", (शून्य*)window_cursor, 0);
    rb_define_method(cVimWindow, "cursor=", (शून्य*)window_set_cursor, 1);

    rb_define_virtual_variable("$curbuf", buffer_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", window_s_current_getter, 0);
}

    शून्य
vim_ruby_init(शून्य *चळत_start)
{
    // should get machine stack start address early in main function
    ruby_चळत_start = चळत_start;
}

    स्थिर पूर्णांक
convert_hash2dict(मूल्य_3 किल्ली, मूल्य_3 val, मूल्य_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(किल्ली)));
    जर (di == रिक्त || ruby_convert_to_vim_मूल्य(val, &di->di_tv) != OK
						     || dict_add(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	परत ST_STOP;
    }
    परत ST_CONTINUE;
}

    स्थिर पूर्णांक
ruby_convert_to_vim_मूल्य(मूल्य_3 val, typval_T *rettv)
{
    निवड (TYPE(val))
    {
	स्थिती T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_संख्या = VVAL_NULL;
	    थांबा;
	स्थिती T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_संख्या = VVAL_TRUE;
	    थांबा;
	स्थिती T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_संख्या = VVAL_FALSE;
	    थांबा;
	स्थिती T_BIGNUM:
	स्थिती T_FIXNUM:
	    rettv->v_type = VAR_संख्या;
	    rettv->vval.v_संख्या = (varnumber_T)NUM2LONG(val);
	    थांबा;
	स्थिती T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    थांबा;
	पूर्वनिर्धारित:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	स्थिती T_STRING:
	    {
		मूल्य_3 str = (मूल्य_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    थांबा;
	स्थिती T_ARRAY:
	    {
		list_T *l;
		दीर्घ i;
		typval_T v;

		l = list_alloc();
		जर (l == रिक्त)
		    परत FAIL;

		साठी (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    जर (ruby_convert_to_vim_मूल्य((मूल्य_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			परत FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    थांबा;
	स्थिती T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		जर (d == रिक्त)
		    परत FAIL;

		rb_hash_foreach(val, convert_hash2dict, (मूल्य_3)d);
		जर (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    परत FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    थांबा;
    }
    परत OK;
}

    शून्य
do_rubyeval(char_u *str, typval_T *rettv)
{
    पूर्णांक retval = FAIL;

    जर (ensure_ruby_initialized())
    {
	पूर्णांक state;
	मूल्य_3 obj;

	obj = rb_eval_string_protect((स्थिरांक अक्षर *)str, &state);
	जर (state)
	    error_print(state);
	अन्यथा
	    retval = ruby_convert_to_vim_मूल्य(obj, rettv);
    }
    जर (retval == FAIL)
    {
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = 0;
    }
}
