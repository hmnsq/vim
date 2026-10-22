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

#అన్తర్భావ "protodef.h"
#నిర్వచితమైతే HAVE_CONFIG_H
# include "auto/config.h"
#అయితే_ముగింపు

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచితమైతే _WIN32
# ifndef DYNAMIC_RUBY
#  define NT
#  define IMPORT // For static dll usage __declspec(dllimport)
#  define RUBYEXTERN __declspec(dllimport)
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే RUBYEXTERN
# define RUBYEXTERN బాహ్య
#అయితే_ముగింపు

#నిర్వచితమైతే DYNAMIC_RUBY
/*
 * This is tricky.  In ruby.h there is (inline) function rb_class_of()
 * definition.  This function use these variables.  But we want function to
 * use dll_* variables.
 */
# ఒకవేళ RUBY_సంస్కరణ >= 24
#  define USE_RUBY_INTEGER
# endif

# define rb_cFalseClass		(*dll_rb_cFalseClass)
# define rb_cFixnum		(*dll_rb_cFixnum)
# ఒకవేళ defined(USE_RUBY_INTEGER)
#  define rb_cInteger		(*dll_rb_cInteger)
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 20
#  define rb_cFloat		(*dll_rb_cFloat)
# endif
# define rb_cNilClass		(*dll_rb_cNilClass)
# define rb_cString		(*dll_rb_cString)
# define rb_c_చిహ్నం		(*dll_rb_c_చిహ్నం)
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

# ఒకవేళ RUBY_సంస్కరణ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
// Ruby 1.9 defines a number of static functions which use rb_fix2int and
// rb_num2int if VIM_SIZEOF_INT < VIM_SIZEOF_LONG (64bit)
#  define rb_fix2int	rb_fix2int_stub
#  define rb_num2int	rb_num2int_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 20
// USE_TYPEDDATA is not defined yet. We just check for 2.0.
#  define rb_check_typeddata		rb_check_typeddata_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ == 21
// Ruby 2.1 adds new GC called RGenGC and RARRAY_PTR uses
// rb_gc_writebarrier_unprotect_promoted if USE_RGENGC
#  define rb_gc_writebarrier_unprotect_promoted	rb_gc_writebarrier_unprotect_promoted_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 22
#  define rb_gc_writebarrier_unprotect	rb_gc_writebarrier_unprotect_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 26 && RUBY_సంస్కరణ <= 32
#  define rb_ary_detransient	rb_ary_detransient_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 30
#  define rb_check_type			rb_check_type_stub
#  define rb_num2uint			rb_num2uint_stub
#  define ruby_malloc_కొలత_అధికప్రవాహం	ruby_malloc_కొలత_అధికప్రవాహం_stub
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 31
#  define rb_debug_rstring_null_ptr	rb_debug_rstring_null_ptr_stub
#  define rb_unexpected_type		rb_unexpected_type_stub
# endif

#అయితే_ముగింపు  // ifdef DYNAMIC_RUBY

// On macOS pre-installed Ruby defines "SIZEOF_TIME_T" as "SIZEOF_LONG" so it
// conflicts with the definition in config.h then causes a macro-redefined
// warning.
#నిర్వచితమైతే SIZEOF_సమయం_T
# undef SIZEOF_సమయం_T
#అయితే_ముగింపు

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic నెట్టు
# pragma GCC diagnostic ignored "-Wunused-parameter"
#అయితే_ముగింపు

#ఒకవేళ defined(__clang__) && (__clang_major__ >= 21)
# pragma GCC diagnostic ignored "-Wdefault-const-init-field-unsafe"
#అయితే_ముగింపు

#అన్తర్భావ <ruby.h>
#అన్తర్భావ <ruby/encoding.h>

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic తీయి
#అయితే_ముగింపు

// See above.
#నిర్వచితమైతే SIZEOF_సమయం_T
# undef SIZEOF_సమయం_T
#అయితే_ముగింపు

#నిర్వచనం_రద్దు off_t	// ruby defines off_t as _int64, Mingw uses long
#నిర్వచనం_రద్దు EXTERN
#నిర్వచనం_రద్దు _

// T_DATA defined both by Ruby and Mac header files, hack around it...
#ఒకవేళ defined(MACOS_X)
# define __OPENTRANSPORT__
# define __OPENTRANSPORTPROTOCOL__
# define __OPENTRANSPORTPROVIDERS__
#అయితే_ముగింపు

/*
 * The TypedData_XXX macro family can be used since Ruby 1.9.2 but
 * rb_data_type_t changed in 1.9.3, therefore require at least 2.0.
 * The old Data_XXX macro family was deprecated on Ruby 2.2.
 * Use TypedData_XXX if available.
 */
#ఒకవేళ defined(Typed_దత్తాంశం_Wrap_Struct) && (RUBY_సంస్కరణ >= 20)
# define USE_TYPEDDATA	1
#అయితే_ముగింపు

/*
 * Backward compatibility for Ruby 1.8 and earlier.
 * Ruby 1.9 does not provide STR2CSTR, instead StringValuePtr is provided.
 * Ruby 1.9 does not provide RXXX(s)->len and RXXX(s)->ptr, instead
 * RXXX_LEN(s) and RXXX_PTR(s) are provided.
 */
#నిర్వచితం_కాకపోతే String_విలువ_Ptr
# define String_విలువ_Ptr(s) STR2CSTR(s)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే RARRAY_LEN
# define RARRAY_LEN(s) RARRAY(s)->len
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే RARRAY_PTR
# define RARRAY_PTR(s) RARRAY(s)->ptr
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే RSTRING_LEN
# define RSTRING_LEN(s) RSTRING(s)->len
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే RSTRING_PTR
# define RSTRING_PTR(s) RSTRING(s)->ptr
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_DUP
# undef HAVE_DUP
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FSYNC
# undef HAVE_FSYNC
#అయితే_ముగింపు

// Avoid redefining TRUE/FALSE in vterm.h.
#నిర్వచితమైతే TRUE
# undef TRUE
#అయితే_ముగింపు
#నిర్వచితమైతే FALSE
# undef FALSE
#అయితే_ముగింపు

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#నిర్వచితమైతే DYNAMIC_RUBY
# ifdef MSWIN	// must come after including vim.h, where it is defined
#  define RUBY_PROC FARPROC
#  define ఎక్కించు_dll vim_ఎక్కించు_Lib
#  define చిహ్నం_from_dll Get_Proc_చిరునామా
#  define close_dll FreeLibrary
#  define ఎక్కించు_dll_error GetWin32Error
# లేకపోతే
#  include <dlfcn.h>
#  define HINSTANCE శూన్య*
#  define RUBY_PROC శూన్య*
#  define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#  define చిహ్నం_from_dll dlsym
#  define close_dll dlclose
#  define ఎక్కించు_dll_error dlerror
# endif
#అయితే_ముగింపు

స్థిర పూర్ణాంకం ruby_initialized = 0;
స్థిర శూన్య *ruby_దొంతర_ప్రారంభం;
స్థిర విలువ_3 objtbl;

స్థిర విలువ_3 mVIM;
స్థిర విలువ_3 c_బఫరు;
స్థిర విలువ_3 c_Vim_కిటికీ;
స్థిర విలువ_3 e_Deleted_బఫరు_Error;
స్థిర విలువ_3 e_Deleted_కిటికీ_Error;

స్థిర పూర్ణాంకం ensure_ruby_initialized(శూన్య);
స్థిర శూన్య error_print(పూర్ణాంకం);
స్థిర శూన్య ruby_io_init(శూన్య);
స్థిర శూన్య ruby_vim_init(శూన్య);
స్థిర పూర్ణాంకం ruby_convert_గమ్యం_vim_విలువ(విలువ_3 val, typval_T *rettv);

#ఒకవేళ defined(__ia64) && !defined(ruby_init_దొంతర)
# define ruby_init_దొంతర(addr) ruby_init_దొంతర((addr), rb_ia64_bsp())
#అయితే_ముగింపు

#ఒకవేళ defined(DYNAMIC_RUBY)

/*
 * Wrapper defines
 */
// Ruby 2.7 actually expands the following symbols as macro.
# ఒకవేళ RUBY_సంస్కరణ >= 27
#  undef rb_define_global_function
#  undef rb_define_పద్ధతి
#  undef rb_define_module_function
#  undef rb_define_singleton_పద్ధతి
# endif

# define rb_assoc_new			dll_rb_assoc_new
# define rb_cObject			(*dll_rb_cObject)
# define rb_class_new_instance		dll_rb_class_new_instance
# ఒకవేళ RUBY_సంస్కరణ < 30
#  define rb_check_type			dll_rb_check_type
# endif
# define rb_class_మార్గం			dll_rb_class_మార్గం
# ifdef USE_TYPEDDATA
#  ఒకవేళ RUBY_సంస్కరణ >= 23
#   define rb_దత్తాంశం_typed_object_wrap	dll_rb_దత్తాంశం_typed_object_wrap
#  లేకపోతే
#   define rb_దత్తాంశం_typed_object_alloc	dll_rb_దత్తాంశం_typed_object_alloc
#  endif
# లేకపోతే
#  define rb_దత్తాంశం_object_alloc		dll_rb_దత్తాంశం_object_alloc
# endif
# define rb_define_class_under		dll_rb_define_class_under
# define rb_define_const			dll_rb_define_const
# define rb_define_global_function	dll_rb_define_global_function
# define rb_define_పద్ధతి		dll_rb_define_పద్ధతి
# define rb_define_module		dll_rb_define_module
# define rb_define_module_function	dll_rb_define_module_function
# define rb_define_singleton_పద్ధతి	dll_rb_define_singleton_పద్ధతి
# define rb_define_virtual_variable	dll_rb_define_virtual_variable
# define rb_stdout			(*dll_rb_stdout)
# define rb_stderr			(*dll_rb_stderr)
# define rb_eArgError			(*dll_rb_eArgError)
# define rb_e_సూచీ_Error			(*dll_rb_e_సూచీ_Error)
# define rb_eRuntimeError		(*dll_rb_eRuntimeError)
# define rb_eStandardError		(*dll_rb_eStandardError)
# define rb_eval_string_protect		dll_rb_eval_string_protect
# ఒకవేళ RUBY_సంస్కరణ >= 21
#  define rb_funcallv			dll_rb_funcallv
# లేకపోతే
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

# ఒకవేళ VIM_SIZEOF_INT < VIM_SIZEOF_LONG // 64 bits only
#  ఒకవేళ RUBY_సంస్కరణ < 30
#   define rb_num2uint			dll_rb_num2uint
#  endif
# endif
# define rb_num2dbl			dll_rb_num2dbl
# define rb_lastline_get			dll_rb_lastline_get
# define rb_lastline_సమితి			dll_rb_lastline_సమితి
# define rb_protect			dll_rb_protect
# define rb_ఎక్కించు			dll_rb_ఎక్కించు
# ఒకవేళ RUBY_సంస్కరణ < 20
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
# లేకపోతే
#  define rb_str_new2			dll_rb_str_new2
# endif
# define rb_string_విలువ		dll_rb_string_విలువ
# define rb_string_విలువ_ptr		dll_rb_string_విలువ_ptr
# define rb_float_new			dll_rb_float_new
# define rb_ary_new			dll_rb_ary_new
# ifdef rb_ary_new4
#  define RB_ARY_NEW4_MACRO 1
#  undef rb_ary_new4
# endif
# define rb_ary_new4			dll_rb_ary_new4
# define rb_ary_నెట్టు			dll_rb_ary_నెట్టు
# ifdef __ia64
#  define rb_ia64_bsp			dll_rb_ia64_bsp
#  undef ruby_init_దొంతర
#  define ruby_init_దొంతర(addr)	dll_ruby_init_దొంతర((addr), rb_ia64_bsp())
# లేకపోతే
#  define ruby_init_దొంతర		dll_ruby_init_దొంతర
# endif
# define rb_errinfo			dll_rb_errinfo
# define ruby_init			dll_ruby_init
# define ruby_init_loadpath		dll_ruby_init_loadpath
# ifdef MSWIN
#  define ruby_sysinit			dll_ruby_sysinit
#  define rb_w32_snprintf		dll_rb_w32_snprintf
# endif

# define ruby_script			dll_ruby_script
# define rb_enc_కనుగొను_సూచీ		dll_rb_enc_కనుగొను_సూచీ
# define rb_enc_కనుగొను			dll_rb_enc_కనుగొను
# undef rb_enc_str_new
# define rb_enc_str_new			dll_rb_enc_str_new
# define rb_sprintf			dll_rb_sprintf
# define rb_require			dll_rb_require
# define ruby_options			dll_ruby_options

/*
 * Pointers for dynamic link
 */
స్థిర విలువ_3 (*dll_rb_assoc_new) (విలువ_3, విలువ_3);
విలువ_3 *dll_rb_cFalseClass;
విలువ_3 *dll_rb_cFixnum;
# ఒకవేళ defined(USE_RUBY_INTEGER)
విలువ_3 *dll_rb_cInteger;
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 20
విలువ_3 *dll_rb_cFloat;
# endif
విలువ_3 *dll_rb_cNilClass;
స్థిర విలువ_3 *dll_rb_cObject;
విలువ_3 *dll_rb_cString;
విలువ_3 *dll_rb_c_చిహ్నం;
విలువ_3 *dll_rb_cTrueClass;
స్థిర విలువ_3 (*dll_rb_class_new_instance) (పూర్ణాంకం,విలువ_3*,విలువ_3);
స్థిర శూన్య (*dll_rb_check_type) (విలువ_3,పూర్ణాంకం);
# ifdef USE_TYPEDDATA
స్థిర శూన్య *(*dll_rb_check_typeddata) (విలువ_3,స్థిరరాశి rb_దత్తాంశం_type_t *);
# endif
స్థిర విలువ_3 (*dll_rb_class_మార్గం) (విలువ_3);
# ifdef USE_TYPEDDATA
#  ఒకవేళ RUBY_సంస్కరణ >= 23
స్థిర విలువ_3 (*dll_rb_దత్తాంశం_typed_object_wrap) (విలువ_3, శూన్య*, స్థిరరాశి rb_దత్తాంశం_type_t *);
#  లేకపోతే
స్థిర విలువ_3 (*dll_rb_దత్తాంశం_typed_object_alloc) (విలువ_3, శూన్య*, స్థిరరాశి rb_దత్తాంశం_type_t *);
#  endif
# లేకపోతే
స్థిర విలువ_3 (*dll_rb_దత్తాంశం_object_alloc) (విలువ_3, శూన్య*, RUBY_దత్తాంశం_FUNC, RUBY_దత్తాంశం_FUNC);
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 31
స్థిర శూన్య (*dll_rb_debug_rstring_null_ptr) (స్థిరరాశి అక్షరం*);
# endif
స్థిర విలువ_3 (*dll_rb_define_class_under) (విలువ_3, స్థిరరాశి అక్షరం*, విలువ_3);
స్థిర శూన్య (*dll_rb_define_const) (విలువ_3,స్థిరరాశి అక్షరం*,విలువ_3);
స్థిర శూన్య (*dll_rb_define_global_function) (స్థిరరాశి అక్షరం*,విలువ_3(*)(పూర్ణాంకం,విలువ_3*,విలువ_3),పూర్ణాంకం);
స్థిర శూన్య (*dll_rb_define_పద్ధతి) (విలువ_3,స్థిరరాశి అక్షరం*,విలువ_3(*)(ANYARGS),పూర్ణాంకం);
స్థిర విలువ_3 (*dll_rb_define_module) (స్థిరరాశి అక్షరం*);
స్థిర శూన్య (*dll_rb_define_module_function) (విలువ_3,స్థిరరాశి అక్షరం*,విలువ_3(*)(విలువ_3,విలువ_3),పూర్ణాంకం);
స్థిర శూన్య (*dll_rb_define_singleton_పద్ధతి) (విలువ_3,స్థిరరాశి అక్షరం*,విలువ_3(*)(ANYARGS),పూర్ణాంకం);
స్థిర శూన్య (*dll_rb_define_virtual_variable) (స్థిరరాశి అక్షరం*,విలువ_3(*)(ID,విలువ_3*),శూన్య(*)());
స్థిర విలువ_3 *dll_rb_stdout;
స్థిర విలువ_3 *dll_rb_stderr;
స్థిర విలువ_3 *dll_rb_eArgError;
స్థిర విలువ_3 *dll_rb_e_సూచీ_Error;
స్థిర విలువ_3 *dll_rb_eRuntimeError;
స్థిర విలువ_3 *dll_rb_eStandardError;
స్థిర విలువ_3 (*dll_rb_eval_string_protect) (స్థిరరాశి అక్షరం*, పూర్ణాంకం*);
# ఒకవేళ RUBY_సంస్కరణ >= 21
స్థిర విలువ_3 (*dll_rb_funcallv) (విలువ_3, ID, పూర్ణాంకం, స్థిరరాశి విలువ_3*);
# లేకపోతే
స్థిర విలువ_3 (*dll_rb_funcall2) (విలువ_3, ID, పూర్ణాంకం, స్థిరరాశి విలువ_3*);
# endif
స్థిర శూన్య (*dll_rb_global_variable) (విలువ_3*);
స్థిర విలువ_3 (*dll_rb_hash_aset) (విలువ_3, విలువ_3, విలువ_3);
స్థిర విలువ_3 (*dll_rb_hash_foreach) (విలువ_3, పూర్ణాంకం (*)(విలువ_3, విలువ_3, విలువ_3), విలువ_3);
స్థిర విలువ_3 (*dll_rb_hash_new) (శూన్య);
స్థిర విలువ_3 (*dll_rb_inspect) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_int2inum) (దీర్ఘ);
స్థిర ID (*dll_rb_intern) (స్థిరరాశి అక్షరం*);
# ఒకవేళ RUBY_సంస్కరణ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
స్థిర దీర్ఘ (*dll_rb_fix2int) (విలువ_3);
స్థిర దీర్ఘ (*dll_rb_num2int) (విలువ_3);
స్థిర గుర్తులేని దీర్ఘ (*dll_rb_num2uint) (విలువ_3);
# endif
స్థిర ద్విగుణ (*dll_rb_num2dbl) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_lastline_get) (శూన్య);
స్థిర శూన్య (*dll_rb_lastline_సమితి) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_protect) (విలువ_3 (*)(విలువ_3), విలువ_3, పూర్ణాంకం*);
స్థిర శూన్య (*dll_rb_ఎక్కించు) (విలువ_3, పూర్ణాంకం);
స్థిర దీర్ఘ (*dll_rb_num2long) (విలువ_3);
స్థిర గుర్తులేని దీర్ఘ (*dll_rb_num2ulong) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_obj_alloc) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_obj_as_string) (విలువ_3);
స్థిర విలువ_3 (*dll_rb_obj_id) (విలువ_3);
స్థిర శూన్య (*dll_rb_raise) (విలువ_3, స్థిరరాశి అక్షరం*, ...);
స్థిర విలువ_3 (*dll_rb_string_విలువ) (అస్థిర విలువ_3*);
స్థిర విలువ_3 (*dll_rb_str_cat) (విలువ_3, స్థిరరాశి అక్షరం*, దీర్ఘ);
స్థిర విలువ_3 (*dll_rb_str_concat) (విలువ_3, విలువ_3);
స్థిర విలువ_3 (*dll_rb_str_new) (స్థిరరాశి అక్షరం*, దీర్ఘ);
# ifdef need_rb_str_new_cstr
// Ruby may #define rb_str_new2 to use rb_str_new_cstr.
స్థిర విలువ_3 (*dll_rb_str_new_cstr) (స్థిరరాశి అక్షరం*);
# లేకపోతే
స్థిర విలువ_3 (*dll_rb_str_new2) (స్థిరరాశి అక్షరం*);
# endif
స్థిర విలువ_3 (*dll_rb_errinfo) (శూన్య);
స్థిర శూన్య (*dll_ruby_init) (శూన్య);
స్థిర శూన్య (*dll_ruby_init_loadpath) (శూన్య);
# ifdef MSWIN
స్థిర శూన్య (*dll_ruby_sysinit) (పూర్ణాంకం*, అక్షరం***);
స్థిర పూర్ణాంకం (*dll_rb_w32_snprintf)(అక్షరం*, size_t, స్థిరరాశి అక్షరం*, ...);
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 31
#  ifdef _MSC_VER
స్థిర శూన్య (*dll_rb_unexpected_type) (విలువ_3, పూర్ణాంకం);
#  లేకపోతే
NORETURN(స్థిర శూన్య (*dll_rb_unexpected_type) (విలువ_3, పూర్ణాంకం));
#  endif
# endif
స్థిర అక్షరం * (*dll_rb_string_విలువ_ptr) (అస్థిర విలువ_3*);
స్థిర విలువ_3 (*dll_rb_float_new) (ద్విగుణ);
స్థిర విలువ_3 (*dll_rb_ary_new) (శూన్య);
స్థిర విలువ_3 (*dll_rb_ary_new4) (దీర్ఘ n, స్థిరరాశి విలువ_3 *elts);
స్థిర విలువ_3 (*dll_rb_ary_నెట్టు) (విలువ_3, విలువ_3);
# ఒకవేళ RUBY_సంస్కరణ >= 26 && RUBY_సంస్కరణ <= 32
స్థిర శూన్య (*dll_rb_ary_detransient) (విలువ_3);
# endif
# ifdef __ia64
స్థిర శూన్య * (*dll_rb_ia64_bsp) (శూన్య);
స్థిర శూన్య (*dll_ruby_init_దొంతర)(విలువ_3*, శూన్య*);
# లేకపోతే
స్థిర శూన్య (*dll_ruby_init_దొంతర)(విలువ_3*);
# endif
స్థిర విలువ_3 (*dll_rb_int2big)(SIGNED_విలువ);

స్థిర శూన్య (*dll_ruby_script) (స్థిరరాశి అక్షరం*);
స్థిర పూర్ణాంకం (*dll_rb_enc_కనుగొను_సూచీ) (స్థిరరాశి అక్షరం*);
స్థిర rb_encoding* (*dll_rb_enc_కనుగొను) (స్థిరరాశి అక్షరం*);
స్థిర విలువ_3 (*dll_rb_enc_str_new) (స్థిరరాశి అక్షరం*, దీర్ఘ, rb_encoding*);
స్థిర విలువ_3 (*dll_rb_sprintf) (స్థిరరాశి అక్షరం*, ...);
స్థిర విలువ_3 (*dll_rb_require) (స్థిరరాశి అక్షరం*);
స్థిర శూన్య* (*dll_ruby_options)(పూర్ణాంకం, అక్షరం**);

# ఒకవేళ defined(USE_RGENGC) && USE_RGENGC
#  ఒకవేళ RUBY_సంస్కరణ == 21
స్థిర శూన్య (*dll_rb_gc_writebarrier_unprotect_promoted)(విలువ_3);
#  లేకపోతే
స్థిర శూన్య (*dll_rb_gc_writebarrier_unprotect)(విలువ_3 obj);
#  endif
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 30
#  ifdef _MSC_VER
స్థిర శూన్య (*dll_ruby_malloc_కొలత_అధికప్రవాహం)(size_t, size_t);
#  లేకపోతే
NORETURN(స్థిర శూన్య (*dll_ruby_malloc_కొలత_అధికప్రవాహం)(size_t, size_t));
#  endif
# endif

# ఒకవేళ RUBY_సంస్కరణ >= 26 && RUBY_సంస్కరణ <= 32
శూన్య rb_ary_detransient_stub(విలువ_3 x);
# endif

// Do not generate a prototype here, VALUE isn't always defined.
# ifndef PROTO
#  ఒకవేళ RUBY_సంస్కరణ >= 22
    దీర్ఘ
rb_num2long_stub(విలువ_3 x)
#  లేకపోతే
    SIGNED_విలువ
rb_num2long_stub(విలువ_3 x)
#  endif
{
    తిరిగి dll_rb_num2long(x);
}
#  ఒకవేళ RUBY_సంస్కరణ >= 26
    విలువ_3
rb_int2big_stub(intptr_t x)
#  లేకపోతే
    విలువ_3
rb_int2big_stub(SIGNED_విలువ x)
#  endif
{
    తిరిగి dll_rb_int2big(x);
}
#  ఒకవేళ RUBY_సంస్కరణ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    దీర్ఘ
rb_fix2int_stub(విలువ_3 x)
{
    తిరిగి dll_rb_fix2int(x);
}
    దీర్ఘ
rb_num2int_stub(విలువ_3 x)
{
    తిరిగి dll_rb_num2int(x);
}
#  endif
#  ఒకవేళ RUBY_సంస్కరణ >= 20
    విలువ_3
rb_float_new_లోపల_కుప్ప(ద్విగుణ d)
{
    తిరిగి dll_rb_float_new(d);
}
#   ఒకవేళ RUBY_సంస్కరణ >= 22
    గుర్తులేని దీర్ఘ
rb_num2ulong(విలువ_3 x)
#   లేకపోతే
    విలువ_3
rb_num2ulong(విలువ_3 x)
#   endif
{
    తిరిగి (దీర్ఘ)RSHIFT((SIGNED_విలువ)(x),1);
}
#  endif
#  ఒకవేళ defined(USE_RGENGC) && USE_RGENGC
#   ఒకవేళ RUBY_సంస్కరణ == 21
    శూన్య
rb_gc_writebarrier_unprotect_promoted_stub(విలువ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect_promoted(obj);
}
#   లేకపోతే
    శూన్య
rb_gc_writebarrier_unprotect_stub(విలువ_3 obj)
{
    dll_rb_gc_writebarrier_unprotect(obj);
}
#   endif
#  endif
#  ఒకవేళ RUBY_సంస్కరణ >= 26 && RUBY_సంస్కరణ <= 32
    శూన్య
rb_ary_detransient_stub(విలువ_3 x)
{
    dll_rb_ary_detransient(x);
}
#  endif
#  ఒకవేళ RUBY_సంస్కరణ >= 30
    శూన్య
rb_check_type_stub(విలువ_3 obj, పూర్ణాంకం t)
{
    dll_rb_check_type(obj, t);
}
    గుర్తులేని దీర్ఘ
rb_num2uint_stub(విలువ_3 x)
{
    తిరిగి dll_rb_num2uint(x);
}
    శూన్య
ruby_malloc_కొలత_అధికప్రవాహం_stub(size_t x, size_t y)
{
    dll_ruby_malloc_కొలత_అధికప్రవాహం(x, y);
}
#  endif
#  ఒకవేళ RUBY_సంస్కరణ >= 31
    శూన్య
rb_debug_rstring_null_ptr_stub(స్థిరరాశి అక్షరం *func)
{
    dll_rb_debug_rstring_null_ptr(func);
}
    శూన్య
rb_unexpected_type_stub(విలువ_3 self, పూర్ణాంకం t)
{
    dll_rb_unexpected_type(self, t);
}
#  endif
#  ifdef USE_TYPEDDATA
శూన్య *rb_check_typeddata_stub(విలువ_3 obj, స్థిరరాశి rb_దత్తాంశం_type_t *దత్తాంశం_type)
{
    తిరిగి dll_rb_check_typeddata(obj, దత్తాంశం_type);
}
#  endif
# endif // ifndef PROTO

స్థిర HINSTANCE hinstRuby = ఖాళీ; // Instance of ruby.dll

/*
 * Table of name to function pointer of ruby.
 */
స్థిర నిర్మాణం
{
    అక్షరం *name;
    RUBY_PROC *ptr;
} ruby_funcname_పట్టిక[] =
{
    {"rb_assoc_new", (RUBY_PROC*)&dll_rb_assoc_new},
    {"rb_cFalseClass", (RUBY_PROC*)&dll_rb_cFalseClass},
# ఒకవేళ defined(USE_RUBY_INTEGER)
    {"rb_cInteger", (RUBY_PROC*)&dll_rb_cInteger},
# లేకపోతే
    {"rb_cFixnum", (RUBY_PROC*)&dll_rb_cFixnum},
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 20
    {"rb_cFloat", (RUBY_PROC*)&dll_rb_cFloat},
# endif
    {"rb_cNilClass", (RUBY_PROC*)&dll_rb_cNilClass},
    {"rb_cObject", (RUBY_PROC*)&dll_rb_cObject},
    {"rb_cString", (RUBY_PROC*)&dll_rb_cString},
    {"rb_cSymbol", (RUBY_PROC*)&dll_rb_c_చిహ్నం},
    {"rb_cTrueClass", (RUBY_PROC*)&dll_rb_cTrueClass},
    {"rb_class_new_instance", (RUBY_PROC*)&dll_rb_class_new_instance},
    {"rb_check_type", (RUBY_PROC*)&dll_rb_check_type},
# ifdef USE_TYPEDDATA
    {"rb_check_typeddata", (RUBY_PROC*)&dll_rb_check_typeddata},
# endif
    {"rb_class_path", (RUBY_PROC*)&dll_rb_class_మార్గం},
# ifdef USE_TYPEDDATA
#  ఒకవేళ RUBY_సంస్కరణ >= 23
    {"rb_data_typed_object_wrap", (RUBY_PROC*)&dll_rb_దత్తాంశం_typed_object_wrap},
#  లేకపోతే
    {"rb_data_typed_object_alloc", (RUBY_PROC*)&dll_rb_దత్తాంశం_typed_object_alloc},
#  endif
# లేకపోతే
    {"rb_data_object_alloc", (RUBY_PROC*)&dll_rb_దత్తాంశం_object_alloc},
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 31
    {"rb_debug_rstring_null_ptr", (RUBY_PROC*)&dll_rb_debug_rstring_null_ptr},
# endif
    {"rb_define_class_under", (RUBY_PROC*)&dll_rb_define_class_under},
    {"rb_define_const", (RUBY_PROC*)&dll_rb_define_const},
    {"rb_define_global_function", (RUBY_PROC*)&dll_rb_define_global_function},
    {"rb_define_method", (RUBY_PROC*)&dll_rb_define_పద్ధతి},
    {"rb_define_module", (RUBY_PROC*)&dll_rb_define_module},
    {"rb_define_module_function", (RUBY_PROC*)&dll_rb_define_module_function},
    {"rb_define_singleton_method", (RUBY_PROC*)&dll_rb_define_singleton_పద్ధతి},
    {"rb_define_virtual_variable", (RUBY_PROC*)&dll_rb_define_virtual_variable},
    {"rb_stdout", (RUBY_PROC*)&dll_rb_stdout},
    {"rb_stderr", (RUBY_PROC*)&dll_rb_stderr},
    {"rb_eArgError", (RUBY_PROC*)&dll_rb_eArgError},
    {"rb_eIndexError", (RUBY_PROC*)&dll_rb_e_సూచీ_Error},
    {"rb_eRuntimeError", (RUBY_PROC*)&dll_rb_eRuntimeError},
    {"rb_eStandardError", (RUBY_PROC*)&dll_rb_eStandardError},
    {"rb_eval_string_protect", (RUBY_PROC*)&dll_rb_eval_string_protect},
# ఒకవేళ RUBY_సంస్కరణ >= 21
    {"rb_funcallv", (RUBY_PROC*)&dll_rb_funcallv},
# లేకపోతే
    {"rb_funcall2", (RUBY_PROC*)&dll_rb_funcall2},
# endif
    {"rb_global_variable", (RUBY_PROC*)&dll_rb_global_variable},
    {"rb_hash_aset", (RUBY_PROC*)&dll_rb_hash_aset},
    {"rb_hash_foreach", (RUBY_PROC*)&dll_rb_hash_foreach},
    {"rb_hash_new", (RUBY_PROC*)&dll_rb_hash_new},
    {"rb_inspect", (RUBY_PROC*)&dll_rb_inspect},
    {"rb_int2inum", (RUBY_PROC*)&dll_rb_int2inum},
    {"rb_intern", (RUBY_PROC*)&dll_rb_intern},
# ఒకవేళ RUBY_సంస్కరణ >= 30 || VIM_SIZEOF_INT < VIM_SIZEOF_LONG
    {"rb_fix2int", (RUBY_PROC*)&dll_rb_fix2int},
    {"rb_num2int", (RUBY_PROC*)&dll_rb_num2int},
    {"rb_num2uint", (RUBY_PROC*)&dll_rb_num2uint},
# endif
    {"rb_num2dbl", (RUBY_PROC*)&dll_rb_num2dbl},
    {"rb_lastline_get", (RUBY_PROC*)&dll_rb_lastline_get},
    {"rb_lastline_set", (RUBY_PROC*)&dll_rb_lastline_సమితి},
    {"rb_protect", (RUBY_PROC*)&dll_rb_protect},
    {"rb_load", (RUBY_PROC*)&dll_rb_ఎక్కించు},
    {"rb_num2long", (RUBY_PROC*)&dll_rb_num2long},
    {"rb_num2ulong", (RUBY_PROC*)&dll_rb_num2ulong},
    {"rb_obj_alloc", (RUBY_PROC*)&dll_rb_obj_alloc},
    {"rb_obj_as_string", (RUBY_PROC*)&dll_rb_obj_as_string},
    {"rb_obj_id", (RUBY_PROC*)&dll_rb_obj_id},
    {"rb_raise", (RUBY_PROC*)&dll_rb_raise},
    {"rb_string_value", (RUBY_PROC*)&dll_rb_string_విలువ},
    {"rb_str_cat", (RUBY_PROC*)&dll_rb_str_cat},
    {"rb_str_concat", (RUBY_PROC*)&dll_rb_str_concat},
    {"rb_str_new", (RUBY_PROC*)&dll_rb_str_new},
# ifdef need_rb_str_new_cstr
    {"rb_str_new_cstr", (RUBY_PROC*)&dll_rb_str_new_cstr},
# లేకపోతే
    {"rb_str_new2", (RUBY_PROC*)&dll_rb_str_new2},
# endif
    {"rb_errinfo", (RUBY_PROC*)&dll_rb_errinfo},
    {"ruby_init", (RUBY_PROC*)&dll_ruby_init},
    {"ruby_init_loadpath", (RUBY_PROC*)&dll_ruby_init_loadpath},
# ifdef MSWIN
    {"ruby_sysinit", (RUBY_PROC*)&dll_ruby_sysinit},
    {"rb_w32_snprintf", (RUBY_PROC*)&dll_rb_w32_snprintf},
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 31
    {"rb_unexpected_type", (RUBY_PROC*)&dll_rb_unexpected_type},
# endif
    {"rb_string_value_ptr", (RUBY_PROC*)&dll_rb_string_విలువ_ptr},
# ఒకవేళ RUBY_సంస్కరణ >= 20
    {"rb_float_new_in_heap", (RUBY_PROC*)&dll_rb_float_new},
# లేకపోతే
    {"rb_float_new", (RUBY_PROC*)&dll_rb_float_new},
# endif
    {"rb_ary_new", (RUBY_PROC*)&dll_rb_ary_new},
# ifdef RB_ARY_NEW4_MACRO
    {"rb_ary_new_from_values", (RUBY_PROC*)&dll_rb_ary_new4},
# లేకపోతే
    {"rb_ary_new4", (RUBY_PROC*)&dll_rb_ary_new4},
# endif
    {"rb_ary_push", (RUBY_PROC*)&dll_rb_ary_నెట్టు},
# ఒకవేళ RUBY_సంస్కరణ >= 26 && RUBY_సంస్కరణ <= 32
    {"rb_ary_detransient", (RUBY_PROC*)&dll_rb_ary_detransient},
# endif
    {"rb_int2big", (RUBY_PROC*)&dll_rb_int2big},
    {"ruby_script", (RUBY_PROC*)&dll_ruby_script},
    {"rb_enc_find_index", (RUBY_PROC*)&dll_rb_enc_కనుగొను_సూచీ},
    {"rb_enc_find", (RUBY_PROC*)&dll_rb_enc_కనుగొను},
    {"rb_enc_str_new", (RUBY_PROC*)&dll_rb_enc_str_new},
    {"rb_sprintf", (RUBY_PROC*)&dll_rb_sprintf},
    {"rb_require", (RUBY_PROC*)&dll_rb_require},
    {"ruby_options", (RUBY_PROC*)&dll_ruby_options},
# ifdef __ia64
    {"rb_ia64_bsp", (RUBY_PROC*)&dll_rb_ia64_bsp},
# endif
    {"ruby_init_stack", (RUBY_PROC*)&dll_ruby_init_దొంతర},
# ఒకవేళ defined(USE_RGENGC) && USE_RGENGC
#  ఒకవేళ RUBY_సంస్కరణ == 21
    {"rb_gc_writebarrier_unprotect_promoted", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect_promoted},
#  లేకపోతే
    {"rb_gc_writebarrier_unprotect", (RUBY_PROC*)&dll_rb_gc_writebarrier_unprotect},
#  endif
# endif
# ఒకవేళ RUBY_సంస్కరణ >= 30
    {"ruby_malloc_size_overflow", (RUBY_PROC*)&dll_ruby_malloc_కొలత_అధికప్రవాహం},
# endif
    {"", ఖాళీ},
};

/*
 * Load library and get all pointers.
 * Parameter 'libname' provides name of DLL.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
ruby_runtime_link_init(అక్షరం *libname, పూర్ణాంకం verbose)
{
    పూర్ణాంకం i;

    ఒకవేళ (hinstRuby)
	తిరిగి OK;
    hinstRuby = ఎక్కించు_dll(libname);
    ఒకవేళ (!hinstRuby)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }

    కొరకు (i = 0; ruby_funcname_పట్టిక[i].ptr; ++i)
    {
	ఒకవేళ (!(*ruby_funcname_పట్టిక[i].ptr = చిహ్నం_from_dll(hinstRuby,
			ruby_funcname_పట్టిక[i].name)))
	{
	    close_dll(hinstRuby);
	    hinstRuby = ఖాళీ;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), ruby_funcname_పట్టిక[i].name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

/*
 * If ruby is enabled (there is installed ruby on Windows system) return TRUE,
 * else FALSE.
 */
    పూర్ణాంకం
ruby_enabled(పూర్ణాంకం verbose)
{
    తిరిగి ruby_runtime_link_init((అక్షరం *)p_rubydll, verbose) == OK;
}
#అయితే_ముగింపు // defined(DYNAMIC_RUBY)

    శూన్య
ruby_చివర(శూన్య)
{
}

    శూన్య
ex_ruby(exarg_T *eap)
{
    పూర్ణాంకం state;
    అక్షరం *script = ఖాళీ;

    script = (అక్షరం *)script_get(eap, eap->arg);
    ఒకవేళ (!eap->skip && ensure_ruby_initialized())
    {
	ఒకవేళ (script == ఖాళీ)
	    rb_eval_string_protect((అక్షరం *)eap->arg, &state);
	లేకపోతే
	    rb_eval_string_protect(script, &state);
	ఒకవేళ (state)
	    error_print(state);
    }
    vim_free(script);
}

/*
 *  In Ruby 1.9 or later, ruby String object has encoding.
 *  conversion buffer string of vim to ruby String object using
 *  VIM encoding option.
 */
    స్థిర విలువ_3
vim_str2rb_enc_str(స్థిరరాశి అక్షరం *s)
{
    దీర్ఘ lval;
    char_u *sval;
    rb_encoding *enc;

    ఒకవేళ (get_option_విలువ((char_u *)"enc", &lval, &sval, ఖాళీ, 0) == gov_string)
    {
	enc = rb_enc_కనుగొను((అక్షరం *)sval);
	vim_free(sval);
	ఒకవేళ (enc)
	    తిరిగి rb_enc_str_new(s, (దీర్ఘ)అక్షరశ్రేణి_పొడవు(s), enc);
    }
    తిరిగి rb_str_new2(s);
}

    స్థిర విలువ_3
eval_enc_string_protect(స్థిరరాశి అక్షరం *str, పూర్ణాంకం *state)
{
    దీర్ఘ lval;
    char_u *sval;
    rb_encoding *enc;
    విలువ_3 v;

    ఒకవేళ (get_option_విలువ((char_u *)"enc", &lval, &sval, ఖాళీ, 0) == gov_string)
    {
	enc = rb_enc_కనుగొను((అక్షరం *)sval);
	vim_free(sval);
	ఒకవేళ (enc)
	{
	    v = rb_sprintf("#-*- coding:%s -*-\n%s", rb_enc_name(enc), str);
	    తిరిగి rb_eval_string_protect(String_విలువ_Ptr(v), state);
	}
    }
    తిరిగి rb_eval_string_protect(str, state);
}

    శూన్య
ex_rubydo(exarg_T *eap)
{
    పూర్ణాంకం state;
    linenr_T i;
    buf_T   *was_curbuf = curbuf;

    ఒకవేళ (!ensure_ruby_initialized())
	తిరిగి;

    ఒకవేళ (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	తిరిగి;
    కొరకు (i = eap->line1; i <= eap->line2; i++)
    {
	విలువ_3 పంక్తి;

	ఒకవేళ (i > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
	పంక్తి = vim_str2rb_enc_str((అక్షరం *)ml_get(i));
	rb_lastline_సమితి(పంక్తి);
	eval_enc_string_protect((అక్షరం *) eap->arg, &state);
	ఒకవేళ (state)
	{
	    error_print(state);
	    ఆపు;
	}
	ఒకవేళ (was_curbuf != curbuf || i > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;
	పంక్తి = rb_lastline_get();
	ఒకవేళ (!NIL_P(పంక్తి))
	{
	    ఒకవేళ (TYPE(పంక్తి) != T_STRING)
	    {
		emsg(_(e_dollar_must_be_an_instance_యొక్క_string));
		తిరిగి;
	    }
	    ml_replace(i, (char_u *) String_విలువ_Ptr(పంక్తి), 1);
	    మారింది();
#నిర్వచితమైతే SYNTAX_HL
	    syn_మారింది(i); // recompute syntax hl. for this line
#అయితే_ముగింపు
	}
    }
    check_cursor();
    update_curbuf(UPD_NOT_VALID);
}

    స్థిర విలువ_3
rb_ఎక్కించు_wrap(విలువ_3 file_గమ్యం_ఎక్కించు)
{
    rb_ఎక్కించు(file_గమ్యం_ఎక్కించు, 0);
    తిరిగి Qnil;
}

    శూన్య
ex_rubyfile(exarg_T *eap)
{
    పూర్ణాంకం state;

    ఒకవేళ (!ensure_ruby_initialized())
	తిరిగి;

    విలువ_3 file_గమ్యం_ఎక్కించు = rb_str_new2((స్థిరరాశి అక్షరం *)eap->arg);
    rb_protect(rb_ఎక్కించు_wrap, file_గమ్యం_ఎక్కించు, &state);
    ఒకవేళ (state)
	error_print(state);
}

    శూన్య
ruby_బఫరు_free(buf_T *buf)
{
    ఒకవేళ (buf->b_ruby_ref == ఖాళీ)
	తిరిగి;

    rb_hash_aset(objtbl, rb_obj_id((విలువ_3) buf->b_ruby_ref), Qnil);
    RDATA(buf->b_ruby_ref)->దత్తాంశం = ఖాళీ;
}

    శూన్య
ruby_కిటికీ_free(win_T *win)
{
    ఒకవేళ (win->w_ruby_ref == ఖాళీ)
	తిరిగి;

    rb_hash_aset(objtbl, rb_obj_id((విలువ_3) win->w_ruby_ref), Qnil);
    RDATA(win->w_ruby_ref)->దత్తాంశం = ఖాళీ;
}

    స్థిర పూర్ణాంకం
ensure_ruby_initialized(శూన్య)
{
    ఒకవేళ (ruby_initialized)
	తిరిగి ruby_initialized;

#నిర్వచితమైతే DYNAMIC_RUBY
    ఒకవేళ (ruby_enabled(TRUE))
#అయితే_ముగింపు
    {
#నిర్వచితమైతే MSWIN
	// suggested by Ariya Mizutani
	పూర్ణాంకం argc = 1;
	అక్షరం *argv[] = {"gvim.exe"};
	అక్షరం **argvp = argv;
	ruby_sysinit(&argc, &argvp);
#అయితే_ముగింపు
	{
	    ruby_init_దొంతర(ruby_దొంతర_ప్రారంభం);
	    ruby_init();
	}
	{
	    పూర్ణాంకం dummy_argc = 2;
	    అక్షరం *dummy_argv[] = {"vim-ruby", "-e_=0"};
	    ruby_options(dummy_argc, dummy_argv);
	}
	ruby_script("vim-ruby");
	ruby_io_init();
	ruby_vim_init();
	ruby_initialized = 1;
    }
#నిర్వచితమైతే DYNAMIC_RUBY
    లేకపోతే
    {
	emsg(_(e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded));
	తిరిగి 0;
    }
#అయితే_ముగింపు

    తిరిగి ruby_initialized;
}

    స్థిర శూన్య
error_print(పూర్ణాంకం state)
{
    విలువ_3 error;
    విలువ_3 eclass;
    విలువ_3 einfo;
    విలువ_3 bt;
    పూర్ణాంకం attr;
    అక్షరం buff[BUFSIZ];
    దీర్ఘ i;

#నిర్వచించు ట్యాగ్_RETURN	0x1
#నిర్వచించు ట్యాగ్_BREAK	0x2
#నిర్వచించు ట్యాగ్_తదుపరి	0x3
#నిర్వచించు ట్యాగ్_RETRY	0x4
#నిర్వచించు ట్యాగ్_REDO	0x5
#నిర్వచించు ట్యాగ్_RAISE	0x6
#నిర్వచించు ట్యాగ్_THROW	0x7
#నిర్వచించు ట్యాగ్_FATAL	0x8
#నిర్వచించు ట్యాగ్_ముసుగు	0xf

    ఎంపిక (state)
    {
	స్థితి ట్యాగ్_RETURN:
	    emsg(_(e_unexpected_return));
	    ఆపు;
	స్థితి ట్యాగ్_తదుపరి:
	    emsg(_(e_unexpected_తదుపరి));
	    ఆపు;
	స్థితి ట్యాగ్_BREAK:
	    emsg(_(e_unexpected_break));
	    ఆపు;
	స్థితి ట్యాగ్_REDO:
	    emsg(_(e_unexpected_redo));
	    ఆపు;
	స్థితి ట్యాగ్_RETRY:
	    emsg(_(e_retry_outside_యొక్క_rescue_clause));
	    ఆపు;
	స్థితి ట్యాగ్_RAISE:
	స్థితి ట్యాగ్_FATAL:
	    error = rb_errinfo();
	    eclass = CLASS_యొక్క(error);
	    einfo = rb_obj_as_string(error);
	    ఒకవేళ (eclass == rb_eRuntimeError && RSTRING_LEN(einfo) == 0)
	    {
		emsg(_(e_unhandled_exception));
	    }
	    లేకపోతే
	    {
		విలువ_3 epath;
		అక్షరం *p;

		epath = rb_class_మార్గం(eclass);
		vim_snprintf(buff, BUFSIZ, "%s: %s",
			 RSTRING_PTR(epath), RSTRING_PTR(einfo));
		p = strchr(buff, '\n');
		ఒకవేళ (p) *p = '\0';
		emsg(buff);
	    }

	    attr = syn_name2attr((char_u *)"Error");
#ఒకవేళ RUBY_సంస్కరణ >= 21
	    bt = rb_funcallv(error, rb_intern("backtrace"), 0, 0);
	    కొరకు (i = 0; i < RARRAY_LEN(bt); i++)
		సందేశం_attr(RSTRING_PTR(RARRAY_AREF(bt, i)), attr);
#లేకపోతే
	    bt = rb_funcall2(error, rb_intern("backtrace"), 0, 0);
	    కొరకు (i = 0; i < RARRAY_LEN(bt); i++)
		సందేశం_attr(RSTRING_PTR(RARRAY_PTR(bt)[i]), attr);
#అయితే_ముగింపు
	    ఆపు;
	అప్రమేయ:
	    vim_snprintf(buff, BUFSIZ, _(e_unknown_longjmp_స్థితి_nr), state);
	    emsg(buff);
	    ఆపు;
    }
}

    స్థిర విలువ_3
vim_సందేశం(విలువ_3 self వాడని, విలువ_3 str)
{
    అక్షరం *buff, *p;

    str = rb_obj_as_string(str);
    ఒకవేళ (RSTRING_LEN(str) > 0)
    {
	// Only do this when the string isn't empty, alloc(0) causes trouble.
	buff = ALLOCA_N(అక్షరం, RSTRING_LEN(str) + 1);
	strcpy(buff, RSTRING_PTR(str));
	p = strchr(buff, '\n');
	ఒకవేళ (p) *p = '\0';
	సందేశం(buff);
    }
    లేకపోతే
    {
	సందేశం("");
    }
    తిరిగి Qnil;
}

    స్థిర విలువ_3
vim_సమితి_option(విలువ_3 self వాడని, విలువ_3 str)
{
    do_సమితి((char_u *)String_విలువ_Ptr(str), 0);
    update_screen(UPD_NOT_VALID);
    తిరిగి Qnil;
}

    స్థిర విలువ_3
vim_command(విలువ_3 self వాడని, విలువ_3 str)
{
    do_cmdline_cmd((char_u *)String_విలువ_Ptr(str));
    తిరిగి Qnil;
}

#నిర్వచితమైతే FEAT_EVAL
    స్థిర విలువ_3
vim_గమ్యం_ruby(typval_T *tv)
{
    విలువ_3 ఫలితం = Qnil;

    ఒకవేళ (tv->v_type == VAR_STRING)
    {
	ఫలితం = rb_str_new2(tv->vval.v_string == ఖాళీ
					  ? "" : (అక్షరం *)(tv->vval.v_string));
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_సంఖ్య)
    {
	ఫలితం = INT2_అంకె(tv->vval.v_సంఖ్య);
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_FLOAT)
    {
	ఫలితం = rb_float_new(tv->vval.v_float);
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST)
    {
	list_T      *list = tv->vval.v_list;
	listitem_T  *curr;

	ఫలితం = rb_ary_new();

	ఒకవేళ (list != ఖాళీ)
	{
	    FOR_ALL_LIST_వస్తువులు(list, curr)
		rb_ary_నెట్టు(ఫలితం, vim_గమ్యం_ruby(&curr->li_tv));
	}
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_DICT)
    {
	ఫలితం = rb_hash_new();

	ఒకవేళ (tv->vval.v_dict != ఖాళీ)
	{
	    hashtab_T   *ht = &tv->vval.v_dict->dv_hashtab;
	    long_u      todo = ht->ht_వాడిన;
	    hashitem_T  *hi;
	    dictitem_T  *di;

	    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
	    {
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    rb_hash_aset(ఫలితం, rb_str_new2((అక్షరం *)hi->hi_కీలకం),
						     vim_గమ్యం_ruby(&di->di_tv));
		}
	    }
	}
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_BOOL || tv->v_type == VAR_SPECIAL)
    {
	ఒకవేళ (tv->vval.v_సంఖ్య == VVAL_TRUE)
	    ఫలితం = Qtrue;
	లేకపోతే ఒకవేళ (tv->vval.v_సంఖ్య == VVAL_FALSE)
	    ఫలితం = Qfalse;
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_BLOB)
    {
	ఫలితం = rb_str_new(tv->vval.v_blob->bv_ga.ga_దత్తాంశం,
		tv->vval.v_blob->bv_ga.ga_len);
    }
    // else return Qnil;

    తిరిగి ఫలితం;
}
#అయితే_ముగింపు

    స్థిర విలువ_3
vim_evaluate(విలువ_3 self వాడని, విలువ_3 str)
{
#నిర్వచితమైతే FEAT_EVAL
    typval_T    *tv;
    విలువ_3       ఫలితం;

    tv = eval_సమాసం((char_u *)String_విలువ_Ptr(str), ఖాళీ);
    ఒకవేళ (tv == ఖాళీ)
	తిరిగి Qnil;
    ఫలితం = vim_గమ్యం_ruby(tv);

    free_tv(tv);

    తిరిగి ఫలితం;
#లేకపోతే
    తిరిగి Qnil;
#అయితే_ముగింపు
}

#నిర్వచితమైతే USE_TYPEDDATA
స్థిర size_t బఫరు_dsize(స్థిరరాశి శూన్య *buf);

స్థిర స్థిరరాశి rb_దత్తాంశం_type_t బఫరు_type = {
    "vim_buffer",
    {0, 0, బఫరు_dsize,
# ఒకవేళ RUBY_సంస్కరణ >= 27
	0, {0}
# లేకపోతే
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    స్థిర size_t
బఫరు_dsize(స్థిరరాశి శూన్య *buf వాడని)
{
    తిరిగి పరిమాణం(buf_T);
}
#అయితే_ముగింపు

    స్థిర విలువ_3
బఫరు_new(buf_T *buf)
{
    ఒకవేళ (buf->b_ruby_ref)
    {
	తిరిగి (విలువ_3) buf->b_ruby_ref;
    }
    లేకపోతే
    {
#నిర్వచితమైతే USE_TYPEDDATA
	విలువ_3 obj = Typed_దత్తాంశం_Wrap_Struct(c_బఫరు, &బఫరు_type, buf);
#లేకపోతే
	విలువ_3 obj = దత్తాంశం_Wrap_Struct(c_బఫరు, 0, 0, buf);
#అయితే_ముగింపు
	buf->b_ruby_ref = (శూన్య *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	తిరిగి obj;
    }
}

    స్థిర buf_T *
get_buf(విలువ_3 obj)
{
    buf_T *buf;

#నిర్వచితమైతే USE_TYPEDDATA
    Typed_దత్తాంశం_Get_Struct(obj, buf_T, &బఫరు_type, buf);
#లేకపోతే
    దత్తాంశం_Get_Struct(obj, buf_T, buf);
#అయితే_ముగింపు
    ఒకవేళ (buf == ఖాళీ)
	rb_raise(e_Deleted_బఫరు_Error, "attempt to refer to deleted buffer");
    తిరిగి buf;
}

    స్థిర విలువ_3
vim_blob(విలువ_3 self వాడని, విలువ_3 str)
{
    విలువ_3 ఫలితం = rb_str_new("0z", 2);
    అక్షరం    buf[4];
    పూర్ణాంకం	i;
    కొరకు (i = 0; i < RSTRING_LEN(str); i++)
    {
	sprintf(buf, "%02X", (గుర్తులేని అక్షరం)(RSTRING_PTR(str)[i]));
	rb_str_concat(ఫలితం, rb_str_new2(buf));
    }
    తిరిగి ఫలితం;
}

    స్థిర విలువ_3
బఫరు_s_current(విలువ_3 self వాడని)
{
    తిరిగి బఫరు_new(curbuf);
}

    స్థిర విలువ_3
బఫరు_s_current_getter(ID id వాడని, విలువ_3 *x వాడని)
{
    తిరిగి బఫరు_new(curbuf);
}

    స్థిర విలువ_3
బఫరు_s_లెక్క(విలువ_3 self వాడని)
{
    buf_T *b;
    పూర్ణాంకం n = 0;

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ఒకవేళ (b->b_p_bl)
	    n++;
    }

    తిరిగి INT2_అంకె(n);
}

    స్థిర విలువ_3
బఫరు_s_aref(విలువ_3 self వాడని, విలువ_3 అంకె)
{
    buf_T *b;
    పూర్ణాంకం n = NUM2INT(అంకె);

    FOR_ALL_BUFFERS(b)
    {
	//  Deleted buffers should not be counted
	//    SegPhault - 01/07/05
	ఒకవేళ (!b->b_p_bl)
	    కొనసాగు;

	ఒకవేళ (n == 0)
	    తిరిగి బఫరు_new(b);

	n--;
    }
    తిరిగి Qnil;
}

    స్థిర విలువ_3
బఫరు_name(విలువ_3 self)
{
    buf_T *buf = get_buf(self);

    తిరిగి buf->b_ffname ? rb_str_new2((అక్షరం *)buf->b_ffname) : Qnil;
}

    స్థిర విలువ_3
బఫరు_సంఖ్య(విలువ_3 self)
{
    buf_T *buf = get_buf(self);

    తిరిగి INT2_అంకె(buf->b_fnum);
}

    స్థిర విలువ_3
బఫరు_లెక్క(విలువ_3 self)
{
    buf_T *buf = get_buf(self);

    తిరిగి INT2_అంకె(buf->b_ml.ml_పంక్తి_లెక్క);
}

    స్థిర విలువ_3
get_బఫరు_పంక్తి(buf_T *buf, linenr_T n)
{
    ఒకవేళ (n <= 0 || n > buf->b_ml.ml_పంక్తి_లెక్క)
	rb_raise(rb_e_సూచీ_Error, "line number %ld out of range", (దీర్ఘ)n);
    తిరిగి vim_str2rb_enc_str((అక్షరం *)ml_get_buf(buf, n, FALSE));
}

    స్థిర విలువ_3
బఫరు_aref(విలువ_3 self, విలువ_3 అంకె)
{
    buf_T *buf = get_buf(self);

    ఒకవేళ (buf != ఖాళీ)
	తిరిగి get_బఫరు_పంక్తి(buf, (linenr_T)NUM2LONG(అంకె));
    తిరిగి Qnil; // For stop warning
}

    స్థిర విలువ_3
సమితి_బఫరు_పంక్తి(buf_T *buf, linenr_T n, విలువ_3 str)
{
    అక్షరం	*పంక్తి = String_విలువ_Ptr(str);
    aco_save_T	aco;

    ఒకవేళ (n > 0 && n <= buf->b_ml.ml_పంక్తి_లెక్క && పంక్తి != ఖాళీ)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ఒకవేళ (u_savesub(n) == OK)
	    {
		ml_replace(n, (char_u *)పంక్తి, TRUE);
		మారింది();
#నిర్వచితమైతే SYNTAX_HL
		syn_మారింది(n); // recompute syntax hl. for this line
#అయితే_ముగింపు
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    లేకపోతే
    {
	rb_raise(rb_e_సూచీ_Error, "line number %ld out of range", (దీర్ఘ)n);
    }
    తిరిగి str;
}

    స్థిర విలువ_3
బఫరు_aset(విలువ_3 self, విలువ_3 అంకె, విలువ_3 str)
{
    buf_T *buf = get_buf(self);

    ఒకవేళ (buf != ఖాళీ)
	తిరిగి సమితి_బఫరు_పంక్తి(buf, (linenr_T)NUM2LONG(అంకె), str);
    తిరిగి str;
}

    స్థిర విలువ_3
బఫరు_delete(విలువ_3 self, విలువ_3 అంకె)
{
    buf_T	*buf = get_buf(self);
    దీర్ఘ	n = NUM2LONG(అంకె);
    aco_save_T	aco;

    ఒకవేళ (n > 0 && n <= buf->b_ml.ml_పంక్తి_లెక్క)
    {
	// Set curwin/curbuf for "buf" and save some things.
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ఒకవేళ (u_savedel(n, 1) == OK)
	    {
		ml_delete(n);

		// Changes to non-active buffers should properly refresh
		//   SegPhault - 01/09/05
		deleted_పంక్తులు_mark(n, 1L);

		మారింది();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    లేకపోతే
    {
	rb_raise(rb_e_సూచీ_Error, "line number %ld out of range", n);
    }
    తిరిగి Qnil;
}

    స్థిర విలువ_3
బఫరు_append(విలువ_3 self, విలువ_3 అంకె, విలువ_3 str)
{
    buf_T	*buf = get_buf(self);
    అక్షరం	*పంక్తి = String_విలువ_Ptr(str);
    దీర్ఘ	n = NUM2LONG(అంకె);
    aco_save_T	aco;

    ఒకవేళ (పంక్తి == ఖాళీ)
    {
	rb_raise(rb_e_సూచీ_Error, "NULL line");
    }
    లేకపోతే ఒకవేళ (n >= 0 && n <= buf->b_ml.ml_పంక్తి_లెక్క)
    {
	// set curwin/curbuf for "buf" and save some things
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ఒకవేళ (u_inssub(n + 1) == OK)
	    {
		ml_append(n, (char_u *) పంక్తి, (colnr_T) 0, FALSE);

		//  Changes to non-active buffers should properly refresh screen
		//    SegPhault - 12/20/04
		appended_పంక్తులు_mark(n, 1L);

		మారింది();
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	    // Careful: autocommands may have made "buf" invalid!
	}

	update_curbuf(UPD_NOT_VALID);
    }
    లేకపోతే
    {
	rb_raise(rb_e_సూచీ_Error, "line number %ld out of range", n);
    }
    తిరిగి str;
}

#నిర్వచితమైతే USE_TYPEDDATA
స్థిర size_t కిటికీ_dsize(స్థిరరాశి శూన్య *buf);

స్థిర స్థిరరాశి rb_దత్తాంశం_type_t కిటికీ_type = {
    "vim_window",
    {0, 0, కిటికీ_dsize,
# ఒకవేళ RUBY_సంస్కరణ >= 27
	0, {0}
# లేకపోతే
	{0, 0}
# endif
    },
    0, 0,
# ifdef RUBY_TYPED_FREE_IMMEDIATELY
    0,
# endif
};

    స్థిర size_t
కిటికీ_dsize(స్థిరరాశి శూన్య *win వాడని)
{
    తిరిగి పరిమాణం(win_T);
}
#అయితే_ముగింపు

    స్థిర విలువ_3
కిటికీ_new(win_T *win)
{
    ఒకవేళ (win->w_ruby_ref)
    {
	తిరిగి (విలువ_3) win->w_ruby_ref;
    }
    లేకపోతే
    {
#నిర్వచితమైతే USE_TYPEDDATA
	విలువ_3 obj = Typed_దత్తాంశం_Wrap_Struct(c_Vim_కిటికీ, &కిటికీ_type, win);
#లేకపోతే
	విలువ_3 obj = దత్తాంశం_Wrap_Struct(c_Vim_కిటికీ, 0, 0, win);
#అయితే_ముగింపు
	win->w_ruby_ref = (శూన్య *) obj;
	rb_hash_aset(objtbl, rb_obj_id(obj), obj);
	తిరిగి obj;
    }
}

    స్థిర win_T *
get_win(విలువ_3 obj)
{
    win_T *win;

#నిర్వచితమైతే USE_TYPEDDATA
    Typed_దత్తాంశం_Get_Struct(obj, win_T, &కిటికీ_type, win);
#లేకపోతే
    దత్తాంశం_Get_Struct(obj, win_T, win);
#అయితే_ముగింపు
    ఒకవేళ (win == ఖాళీ)
	rb_raise(e_Deleted_కిటికీ_Error, "attempt to refer to deleted window");
    తిరిగి win;
}

    స్థిర విలువ_3
కిటికీ_s_current(విలువ_3 self వాడని)
{
    తిరిగి కిటికీ_new(curwin);
}

    స్థిర విలువ_3
కిటికీ_s_current_getter(ID id వాడని, విలువ_3 *x వాడని)
{
    తిరిగి కిటికీ_new(curwin);
}

/*
 * Added line manipulation functions
 *    SegPhault - 03/07/05
 */
    స్థిర విలువ_3
పంక్తి_s_current(విలువ_3 self వాడని)
{
    తిరిగి get_బఫరు_పంక్తి(curbuf, curwin->w_cursor.lnum);
}

    స్థిర విలువ_3
సమితి_current_పంక్తి(విలువ_3 self వాడని, విలువ_3 str)
{
    తిరిగి సమితి_బఫరు_పంక్తి(curbuf, curwin->w_cursor.lnum, str);
}

    స్థిర విలువ_3
current_పంక్తి_సంఖ్య(విలువ_3 self వాడని)
{
    తిరిగి INT2FIX((పూర్ణాంకం)curwin->w_cursor.lnum);
}

    స్థిర విలువ_3
కిటికీ_s_లెక్క(విలువ_3 self వాడని)
{
    win_T	*w;
    పూర్ణాంకం n = 0;

    FOR_ALL_WINDOWS(w)
	n++;
    తిరిగి INT2_అంకె(n);
}

    స్థిర విలువ_3
కిటికీ_s_aref(విలువ_3 self వాడని, విలువ_3 అంకె)
{
    win_T *w;
    పూర్ణాంకం n = NUM2INT(అంకె);

    కొరకు (w = firstwin; w != ఖాళీ; w = w->w_తదుపరి, --n)
	ఒకవేళ (n == 0)
	    తిరిగి కిటికీ_new(w);
    తిరిగి Qnil;
}

    స్థిర విలువ_3
కిటికీ_బఫరు(విలువ_3 self)
{
    win_T *win = get_win(self);

    తిరిగి బఫరు_new(win->w_బఫరు);
}

    స్థిర విలువ_3
కిటికీ_ఎత్తు(విలువ_3 self)
{
    win_T *win = get_win(self);

    తిరిగి INT2_అంకె(win->w_ఎత్తు);
}

    స్థిర విలువ_3
కిటికీ_సమితి_ఎత్తు(విలువ_3 self, విలువ_3 ఎత్తు)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setheight(NUM2INT(ఎత్తు));
    curwin = savewin;
    తిరిగి ఎత్తు;
}

    స్థిర విలువ_3
కిటికీ_వెడల్పు(విలువ_3 self వాడని)
{
    తిరిగి INT2_అంకె(get_win(self)->w_వెడల్పు);
}

    స్థిర విలువ_3
కిటికీ_సమితి_వెడల్పు(విలువ_3 self వాడని, విలువ_3 వెడల్పు)
{
    win_T *win = get_win(self);
    win_T *savewin = curwin;

    curwin = win;
    win_setwidth(NUM2INT(వెడల్పు));
    curwin = savewin;
    తిరిగి వెడల్పు;
}

    స్థిర విలువ_3
కిటికీ_cursor(విలువ_3 self)
{
    win_T *win = get_win(self);

    తిరిగి rb_assoc_new(INT2_అంకె(win->w_cursor.lnum), INT2_అంకె(win->w_cursor.నిలువు));
}

    స్థిర విలువ_3
కిటికీ_సమితి_cursor(విలువ_3 self, విలువ_3 pos)
{
    విలువ_3 lnum, నిలువు;
    win_T *win = get_win(self);

    Check_Type(pos, T_ARRAY);
    ఒకవేళ (RARRAY_LEN(pos) != 2)
	rb_raise(rb_eArgError, "array length must be 2");
    lnum = RARRAY_PTR(pos)[0];
    నిలువు = RARRAY_PTR(pos)[1];
    win->w_cursor.lnum = NUM2LONG(lnum);
    win->w_cursor.నిలువు = NUM2UINT(నిలువు);
    win->w_సమితి_curswant = true;
    check_cursor();		    // put cursor on an existing line
    update_screen(UPD_NOT_VALID);
    తిరిగి Qnil;
}

    స్థిర విలువ_3
f_nop(విలువ_3 self వాడని)
{
    తిరిగి Qnil;
}

    స్థిర విలువ_3
f_p(పూర్ణాంకం argc, విలువ_3 *argv, విలువ_3 self వాడని)
{
    పూర్ణాంకం i;
    విలువ_3 str = rb_str_new("", 0);
    విలువ_3 ret = Qnil;

    కొరకు (i = 0; i < argc; i++)
    {
	ఒకవేళ (i > 0) rb_str_cat(str, ", ", 2);
	rb_str_concat(str, rb_inspect(argv[i]));
    }
    సందేశం(RSTRING_PTR(str));

    ఒకవేళ (argc == 1)
	ret = argv[0];
    లేకపోతే ఒకవేళ (argc > 1)
	ret = rb_ary_new4(argc, argv);
    తిరిగి ret;
}

    స్థిర శూన్య
ruby_io_init(శూన్య)
{
#నిర్వచితం_కాకపోతే DYNAMIC_RUBY
    RUBYEXTERN విలువ_3 rb_stdout;
    RUBYEXTERN విలువ_3 rb_stderr;
#అయితే_ముగింపు

    rb_stdout = rb_obj_alloc(rb_cObject);
    rb_stderr = rb_obj_alloc(rb_cObject);
    rb_define_singleton_పద్ధతి(rb_stdout, "write", (శూన్య*)vim_సందేశం, 1);
    rb_define_singleton_పద్ధతి(rb_stdout, "flush", (శూన్య*)f_nop, 0);
    rb_define_singleton_పద్ధతి(rb_stderr, "write", (శూన్య*)vim_సందేశం, 1);
    rb_define_singleton_పద్ధతి(rb_stderr, "flush", (శూన్య*)f_nop, 0);
    rb_define_global_function("p", f_p, -1);
}

    స్థిర శూన్య
ruby_vim_init(శూన్య)
{
    objtbl = rb_hash_new();
    rb_global_variable(&objtbl);

    // The Vim module used to be called "VIM", but "Vim" is better.  Make an
    // alias "VIM" for backwards compatibility.
    mVIM = rb_define_module("Vim");
    rb_define_const(rb_cObject, "VIM", mVIM);
    rb_define_const(mVIM, "VERSION_MAJOR", INT2_అంకె(VIM_సంస్కరణ_MAJOR));
    rb_define_const(mVIM, "VERSION_MINOR", INT2_అంకె(VIM_సంస్కరణ_MINOR));
    rb_define_const(mVIM, "VERSION_BUILD", INT2_అంకె(VIM_సంస్కరణ_BUILD));
    rb_define_const(mVIM, "VERSION_PATCHLEVEL", INT2_అంకె(VIM_సంస్కరణ_PATCHLEVEL));
    rb_define_const(mVIM, "VERSION_SHORT", rb_str_new2(VIM_సంస్కరణ_SHORT));
    rb_define_const(mVIM, "VERSION_MEDIUM", rb_str_new2(VIM_సంస్కరణ_MEDIUM));
    rb_define_const(mVIM, "VERSION_LONG", rb_str_new2(VIM_సంస్కరణ_LONG));
    rb_define_const(mVIM, "VERSION_LONG_DATE", rb_str_new2(VIM_సంస్కరణ_LONG_DATE));
    rb_define_module_function(mVIM, "message", vim_సందేశం, 1);
    rb_define_module_function(mVIM, "set_option", vim_సమితి_option, 1);
    rb_define_module_function(mVIM, "command", vim_command, 1);
    rb_define_module_function(mVIM, "evaluate", vim_evaluate, 1);
    rb_define_module_function(mVIM, "blob", vim_blob, 1);

    e_Deleted_బఫరు_Error = rb_define_class_under(mVIM, "DeletedBufferError",
						rb_eStandardError);
    e_Deleted_కిటికీ_Error = rb_define_class_under(mVIM, "DeletedWindowError",
						rb_eStandardError);

    c_బఫరు = rb_define_class_under(mVIM, "Buffer", rb_cObject);
    rb_define_singleton_పద్ధతి(c_బఫరు, "current", (శూన్య*)బఫరు_s_current, 0);
    rb_define_singleton_పద్ధతి(c_బఫరు, "count", (శూన్య*)బఫరు_s_లెక్క, 0);
    rb_define_singleton_పద్ధతి(c_బఫరు, "[]", (శూన్య*)బఫరు_s_aref, 1);
    rb_define_పద్ధతి(c_బఫరు, "name", (శూన్య*)బఫరు_name, 0);
    rb_define_పద్ధతి(c_బఫరు, "number", (శూన్య*)బఫరు_సంఖ్య, 0);
    rb_define_పద్ధతి(c_బఫరు, "count", (శూన్య*)బఫరు_లెక్క, 0);
    rb_define_పద్ధతి(c_బఫరు, "length", (శూన్య*)బఫరు_లెక్క, 0);
    rb_define_పద్ధతి(c_బఫరు, "[]", (శూన్య*)బఫరు_aref, 1);
    rb_define_పద్ధతి(c_బఫరు, "[]=", (శూన్య*)బఫరు_aset, 2);
    rb_define_పద్ధతి(c_బఫరు, "delete", (శూన్య*)బఫరు_delete, 1);
    rb_define_పద్ధతి(c_బఫరు, "append", (శూన్య*)బఫరు_append, 2);

    // Added line manipulation functions
    //   SegPhault - 03/07/05
    rb_define_పద్ధతి(c_బఫరు, "line_number", (శూన్య*)current_పంక్తి_సంఖ్య, 0);
    rb_define_పద్ధతి(c_బఫరు, "line", (శూన్య*)పంక్తి_s_current, 0);
    rb_define_పద్ధతి(c_బఫరు, "line=", (శూన్య*)సమితి_current_పంక్తి, 1);


    c_Vim_కిటికీ = rb_define_class_under(mVIM, "Window", rb_cObject);
    rb_define_singleton_పద్ధతి(c_Vim_కిటికీ, "current", (శూన్య*)కిటికీ_s_current, 0);
    rb_define_singleton_పద్ధతి(c_Vim_కిటికీ, "count", (శూన్య*)కిటికీ_s_లెక్క, 0);
    rb_define_singleton_పద్ధతి(c_Vim_కిటికీ, "[]", (శూన్య*)కిటికీ_s_aref, 1);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "buffer", (శూన్య*)కిటికీ_బఫరు, 0);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "height", (శూన్య*)కిటికీ_ఎత్తు, 0);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "height=", (శూన్య*)కిటికీ_సమితి_ఎత్తు, 1);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "width", (శూన్య*)కిటికీ_వెడల్పు, 0);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "width=", (శూన్య*)కిటికీ_సమితి_వెడల్పు, 1);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "cursor", (శూన్య*)కిటికీ_cursor, 0);
    rb_define_పద్ధతి(c_Vim_కిటికీ, "cursor=", (శూన్య*)కిటికీ_సమితి_cursor, 1);

    rb_define_virtual_variable("$curbuf", బఫరు_s_current_getter, 0);
    rb_define_virtual_variable("$curwin", కిటికీ_s_current_getter, 0);
}

    శూన్య
vim_ruby_init(శూన్య *దొంతర_ప్రారంభం)
{
    // should get machine stack start address early in main function
    ruby_దొంతర_ప్రారంభం = దొంతర_ప్రారంభం;
}

    స్థిర పూర్ణాంకం
convert_hash2dict(విలువ_3 కీలకం, విలువ_3 val, విలువ_3 arg)
{
    dict_T *d = (dict_T *)arg;
    dictitem_T *di;

    di = dictitem_alloc((char_u *)RSTRING_PTR(rb_obj_as_string(కీలకం)));
    ఒకవేళ (di == ఖాళీ || ruby_convert_గమ్యం_vim_విలువ(val, &di->di_tv) != OK
						     || dict_కూడు(d, di) != OK)
    {
	d->dv_hashtab.ht_flags |= HTFLAGS_ERROR;
	తిరిగి ST_STOP;
    }
    తిరిగి ST_CONTINUE;
}

    స్థిర పూర్ణాంకం
ruby_convert_గమ్యం_vim_విలువ(విలువ_3 val, typval_T *rettv)
{
    ఎంపిక (TYPE(val))
    {
	స్థితి T_NIL:
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_సంఖ్య = VVAL_NULL;
	    ఆపు;
	స్థితి T_TRUE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_సంఖ్య = VVAL_TRUE;
	    ఆపు;
	స్థితి T_FALSE:
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_సంఖ్య = VVAL_FALSE;
	    ఆపు;
	స్థితి T_BIGNUM:
	స్థితి T_FIXNUM:
	    rettv->v_type = VAR_సంఖ్య;
	    rettv->vval.v_సంఖ్య = (varnumber_T)NUM2LONG(val);
	    ఆపు;
	స్థితి T_FLOAT:
	    rettv->v_type = VAR_FLOAT;
	    rettv->vval.v_float = (float_T)NUM2DBL(val);
	    ఆపు;
	అప్రమేయ:
	    val = rb_obj_as_string(val);
	    // FALLTHROUGH
	స్థితి T_STRING:
	    {
		విలువ_3 str = (విలువ_3)RSTRING(val);

		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = vim_strnsave((char_u *)RSTRING_PTR(str),
							     RSTRING_LEN(str));
	    }
	    ఆపు;
	స్థితి T_ARRAY:
	    {
		list_T *l;
		దీర్ఘ i;
		typval_T v;

		l = list_alloc();
		ఒకవేళ (l == ఖాళీ)
		    తిరిగి FAIL;

		కొరకు (i = 0; i < RARRAY_LEN(val); ++i)
		{
		    ఒకవేళ (ruby_convert_గమ్యం_vim_విలువ((విలువ_3)RARRAY_PTR(val)[i],
									&v) != OK)
		    {
			list_unref(l);
			తిరిగి FAIL;
		    }
		    list_append_tv(l, &v);
		    clear_tv(&v);
		}

		rettv->v_type = VAR_LIST;
		rettv->vval.v_list = l;
		++l->lv_refcount;
	    }
	    ఆపు;
	స్థితి T_HASH:
	    {
		dict_T *d;

		d = dict_alloc();
		ఒకవేళ (d == ఖాళీ)
		    తిరిగి FAIL;

		rb_hash_foreach(val, convert_hash2dict, (విలువ_3)d);
		ఒకవేళ (d->dv_hashtab.ht_flags & HTFLAGS_ERROR)
		{
		    dict_unref(d);
		    తిరిగి FAIL;
		}

		rettv->v_type = VAR_DICT;
		rettv->vval.v_dict = d;
		++d->dv_refcount;
	    }
	    ఆపు;
    }
    తిరిగి OK;
}

    శూన్య
do_rubyeval(char_u *str, typval_T *rettv)
{
    పూర్ణాంకం retval = FAIL;

    ఒకవేళ (ensure_ruby_initialized())
    {
	పూర్ణాంకం state;
	విలువ_3 obj;

	obj = rb_eval_string_protect((స్థిరరాశి అక్షరం *)str, &state);
	ఒకవేళ (state)
	    error_print(state);
	లేకపోతే
	    retval = ruby_convert_గమ్యం_vim_విలువ(obj, rettv);
    }
    ఒకవేళ (retval == FAIL)
    {
	rettv->v_type = VAR_సంఖ్య;
	rettv->vval.v_సంఖ్య = 0;
    }
}
