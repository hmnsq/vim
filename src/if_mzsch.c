/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * MzScheme interface by Sergey Khorev <sergey.khorev@gmail.com>
 * Based on work by Brent Fulgham <bfulgham@debian.org>
 * (Based on lots of help from Matthew Flatt)
 *
 * This consists of six parts:
 * 1. MzScheme interpreter main program
 * 2. Routines that handle the external interface between MzScheme and
 *    Vim.
 * 3. MzScheme input/output handlers: writes output via [e]msg().
 * 4. Implementation of the Vim Features for MzScheme
 * 5. Vim Window-related Manipulation Functions.
 * 6. Vim Buffer-related Manipulation Functions
 *
 * NOTES
 * 1. Memory, allocated with scheme_malloc*, need not to be freed explicitly,
 *    garbage collector will do it self
 * 2. Requires at least NORMAL features. I can't imagine why one may want
 *    to build with TINY features but with MzScheme interface.
 * 3. I don't use K&R-style functions. Anyways, MzScheme headers are ANSI.
 */

#ضمن "vim.h"

#ضمن "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#إذا defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#إذا MZSCHEME_نسخو_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_نسخو_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#جيڪڏهن_پورو

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#إذا defined(MZ_PRECISE_GC) && MZSCHEME_نسخو_MAJOR >= 400 \
    || MZSCHEME_نسخو_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#جيڪڏهن_پورو

// Base data structures
#وصف_ڪريو SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_بفر_type)
#وصف_ڪريو SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_دري_type)

تعريفنوع بنية
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_بفر;

#وصف_ڪريو INVALID_بفر_قدر ((buf_T *)(-1))

تعريفنوع بنية
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_دري;

#وصف_ڪريو INVALID_دري_قدر ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
تعريفنوع بنية
{
    Scheme_Closed_Prim	*prim;
    حرف	*name;
    صحيح		mina;	// arity information
    صحيح		maxa;
} Vim_Prim;

تعريفنوع بنية
{
    حرف	    *name;
    Scheme_Object   *پورٽ;
} پورٽ_Info;

/*
 *========================================================================
 *  Vim-Control Commands
 *========================================================================
 */
/*
 *========================================================================
 *  Utility functions for the vim/mzscheme interface
 *========================================================================
 */
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
ساكن Scheme_Object *sandbox_file_guard(صحيح, Scheme_Object **);
ساكن Scheme_Object *sandbox_ڄار_guard(صحيح, Scheme_Object **);
ساكن فراغ sandbox_check(فراغ);
#جيڪڏهن_پورو
//  Buffer-related commands
ساكن Scheme_Object *بفر_new(buf_T *buf);
ساكن Scheme_Object *get_بفر_by_انگ(فراغ *, صحيح, Scheme_Object **);
ساكن vim_mz_بفر *get_vim_curr_بفر(فراغ);

//  Window-related commands
ساكن Scheme_Object *دري_new(win_T *win);
ساكن vim_mz_دري *get_vim_curr_دري(فراغ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ساكن صحيح vim_error_check(فراغ);
ساكن صحيح do_mzscheme_command(exarg_T *, فراغ *, Scheme_Closed_Prim *what);
ساكن صحيح startup_mzscheme(فراغ);
ساكن حرف *string_منزل_سٽ(Scheme_Object *obj);
#إذا MZSCHEME_نسخو_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#وإلا
# define OUTPUT_LEN_TYPE ڊگهو
#جيڪڏهن_پورو
ساكن فراغ do_output(حرف *mesg, OUTPUT_LEN_TYPE len);
ساكن فراغ do_printf(حرف *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساكن فراغ do_flush(فراغ);
ساكن Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ساكن Scheme_Object *ڪڍو_exn_پيغام(Scheme_Object *v);
ساكن Scheme_Object *do_eval(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن Scheme_Object *do_ڀريو(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن فراغ رجسٽر_vim_exn(فراغ);
ساكن vim_mz_بفر *get_بفر_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن vim_mz_دري *get_دري_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن صحيح سٽ_اندريون_range(linenr_T, buf_T *);
ساكن فراغ check_سٽ_range(linenr_T, buf_T *);
ساكن فراغ mz_fix_cursor(صحيح lo, صحيح hi, صحيح extra);

ساكن صحيح eval_with_exn_handling(فراغ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ساكن فراغ make_modules(فراغ);
ساكن فراغ init_exn_catching_apply(فراغ);
ساكن صحيح mzscheme_env_main(Scheme_Env *env, صحيح argc, حرف **argv);
ساكن صحيح mzscheme_init(فراغ);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن Scheme_Object *vim_منزل_mzscheme(typval_T *vim_قدر);
ساكن Scheme_Object *vim_منزل_mzscheme_impl(typval_T *vim_قدر, صحيح اونهائي,
	Scheme_Hash_جدول *گهميل);
ساكن صحيح mzscheme_منزل_vim(Scheme_Object *obj, typval_T *tv);
ساكن صحيح mzscheme_منزل_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح اونهائي,
	Scheme_Hash_جدول *گهميل);
ساكن Scheme_Object *vim_funcref(فراغ *معلومات, صحيح argc, Scheme_Object **argv);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
ساكن صحيح بفر_سائيز_proc(فراغ *obj اڻ_استعمال_ٿيل)
{
    أرجع gc_بائيٽون_منزل_لفظ(حجم(vim_mz_بفر));
}
ساكن صحيح بفر_mark_proc(فراغ *obj)
{
    أرجع بفر_سائيز_proc(obj);
}
ساكن صحيح بفر_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    أرجع بفر_سائيز_proc(obj);
}
ساكن صحيح دري_سائيز_proc(فراغ *obj اڻ_استعمال_ٿيل)
{
    أرجع gc_بائيٽون_منزل_لفظ(حجم(vim_mz_دري));
}
ساكن صحيح دري_mark_proc(فراغ *obj)
{
    أرجع دري_سائيز_proc(obj);
}
ساكن صحيح دري_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    أرجع دري_سائيز_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define دري_REF(win) *(vim_mz_دري **)((win)->w_mzscheme_ref)
# define بفر_REF(buf) *(vim_mz_بفر **)((buf)->b_mzscheme_ref)
#وإلا
# define دري_REF(win) (vim_mz_دري *)((win)->w_mzscheme_ref)
# define بفر_REF(buf) (vim_mz_بفر *)((buf)->b_mzscheme_ref)
#جيڪڏهن_پورو

#إذا defined(DYNAMIC_MZSCHEME)
ساكن Scheme_Object *dll_scheme_eof;
ساكن Scheme_Object *dll_scheme_false;
ساكن Scheme_Object *dll_scheme_void;
ساكن Scheme_Object *dll_scheme_null;
ساكن Scheme_Object *dll_scheme_true;

ساكن Scheme_Thread **dll_scheme_current_thread_ptr;

ساكن فراغ (**dll_scheme_console_printf_ptr)(حرف *str, ...);
ساكن فراغ (**dll_scheme_console_output_ptr)(حرف *str, OUTPUT_LEN_TYPE len);
ساكن فراغ (**dll_scheme_notify_multithread_ptr)(صحيح on);

ساكن فراغ *(*dll_GC_malloc)(size_t سائيز_اندريون_بائيٽون);
ساكن فراغ *(*dll_GC_malloc_atomic)(size_t سائيز_اندريون_بائيٽون);
ساكن Scheme_Env *(*dll_scheme_basic_env)(فراغ);
ساكن فراغ (*dll_scheme_check_ڌاڳا)(فراغ);
ساكن فراغ (*dll_scheme_رجسٽر_static)(فراغ *ptr, ڊگهو سائيز);
ساكن فراغ (*dll_scheme_سيٽ_ڍير_بنياد)(فراغ *بنياد, صحيح no_auto_statics);
ساكن فراغ (*dll_scheme_جوڙيو_global)(ثابت حرف *name, Scheme_Object *val,
	Scheme_Env *env);
ساكن فراغ (*dll_scheme_جوڙيو_global_نشان)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, صحيح انگ_rands,
	Scheme_Object **rands);
ساكن Scheme_Object *(*dll_scheme_builtin_قدر)(ثابت حرف *name);
# إذا MZSCHEME_نسخو_MAJOR >= 299
ساكن Scheme_Object *(*dll_scheme_byte_string_منزل_char_string)(Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_make_رستو)(ثابت حرف *chars);
# endif
ساكن فراغ (*dll_scheme_close_داخلا_پورٽ)(Scheme_Object *پورٽ);
ساكن فراغ (*dll_scheme_ڳڻپ_سٽون)(Scheme_Object *پورٽ);
# إذا MZSCHEME_نسخو_MAJOR < 360
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(فراغ);
# وإلا
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_ٽيگ);
# endif
ساكن فراغ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *پورٽ);
ساكن حرف *(*dll_scheme_display_منزل_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ساكن صحيح (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ساكن Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	صحيح انگ_rands_2, Scheme_Object **rands, صحيح val);
ساكن فراغ (*dll_scheme_dont_gc_ptr)(فراغ *p);
ساكن Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string)(ثابت حرف *str,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string_all)(ثابت حرف *str,
	Scheme_Env *env, صحيح all);
ساكن فراغ (*dll_scheme_پڄاڻي_primitive_module)(Scheme_Env *env);
# إذا MZSCHEME_نسخو_MAJOR < 299
ساكن حرف *(*dll_scheme_format)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, ڊگهو *rlen);
# وإلا
ساكن حرف *(*dll_scheme_format_utf8)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ساكن Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, صحيح pos);
# endif
ساكن فراغ (*dll_scheme_gc_ptr_ok)(فراغ *p);
# إذا MZSCHEME_نسخو_MAJOR < 299
ساكن حرف *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	ڊگهو *len);
# وإلا
ساكن حرف *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ساكن Scheme_Object *(*dll_scheme_intern_نشان)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *نشان,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, فراغ *معلومات, ثابت حرف *name, mzshort mina,
     mzshort maxa);
ساكن Scheme_Object *(*dll_scheme_make_integer_قدر)(ڊگهو i);
ساكن Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ساكن Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ثابت حرف *name, mzshort mina, mzshort maxa);
# إذا MZSCHEME_نسخو_MAJOR < 299
ساكن Scheme_Object *(*dll_scheme_make_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_string_output_پورٽ)();
# وإلا
ساكن Scheme_Object *(*dll_scheme_make_byte_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_byte_string_output_پورٽ)();
# endif
ساكن Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	صحيح argc, Scheme_Object **argv);
ساكن Scheme_Object **(*dll_scheme_make_struct_نالا)(Scheme_Object *بنياد,
	Scheme_Object *ميدان_نالا, صحيح flags, صحيح *ڳڻپ_ٻاهريون);
ساكن Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *بنياد,
	Scheme_Object *والد, Scheme_Object *inspector, صحيح انگ_fields,
	صحيح انگ_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# إذا MZSCHEME_نسخو_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ساكن Scheme_Object **(*dll_scheme_make_struct_قدرون)(
	Scheme_Object *struct_type, Scheme_Object **نالا, صحيح ڳڻپ,
	صحيح flags);
ساكن Scheme_Type (*dll_scheme_make_type)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_make_ويڪٽر)(صحيح سائيز,
	Scheme_Object *fill);
ساكن فراغ *(*dll_scheme_malloc_fail_ok)(فراغ *(*f)(size_t), size_t);
ساكن Scheme_Object *(*dll_scheme_open_داخلا_file)(ثابت حرف *name,
	ثابت حرف *who);
ساكن Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ساكن صحيح (*dll_scheme_proper_list_ڊيگهائي)(Scheme_Object *list);
ساكن فراغ (*dll_scheme_raise)(Scheme_Object *exn);
ساكن Scheme_Object *(*dll_scheme_read)(Scheme_Object *پورٽ);
ساكن فراغ (*dll_scheme_signal_error)(ثابت حرف *نياپو, ...);
ساكن فراغ (*dll_scheme_wrong_type)(ثابت حرف *name, ثابت حرف *expected,
	صحيح which, صحيح argc, Scheme_Object **argv);
# إذا MZSCHEME_نسخو_MAJOR >= 299
ساكن فراغ (*dll_scheme_سيٽ_param)(Scheme_Config *c, صحيح pos,
	Scheme_Object *o);
ساكن Scheme_Config *(*dll_scheme_current_config)(فراغ);
ساكن Scheme_Object *(*dll_scheme_char_string_منزل_byte_string)
    (Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_char_string_منزل_رستو)
    (Scheme_Object *s);
ساكن فراغ *(*dll_scheme_سيٽ_collects_رستو)(Scheme_Object *p);
# endif
ساكن Scheme_Hash_جدول *(*dll_scheme_make_hash_جدول)(صحيح type);
ساكن فراغ (*dll_scheme_hash_سيٽ)(Scheme_Hash_جدول *جدول,
	Scheme_Object *ڪنجي, Scheme_Object *قدر);
ساكن Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_جدول *جدول,
	Scheme_Object *ڪنجي);
ساكن Scheme_Object *(*dll_scheme_make_double)(مزدوج d);
ساكن Scheme_Object *(*dll_scheme_make_sized_byte_string)(حرف *chars,
	ڊگهو len, صحيح copy);
ساكن Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ساكن Scheme_Object *(*dll_scheme_dynamic_wind)(فراغ (*pre)(فراغ *), Scheme_Object *(* متطاير act)(فراغ *), فراغ (* متطاير post)(فراغ *), Scheme_Object *(*jmp_handler)(فراغ *), فراغ * متطاير معلومات);
# ifdef MZ_PRECISE_GC
ساكن فراغ *(*dll_GC_malloc_one_tagged)(size_t سائيز_اندريون_بائيٽون);
ساكن فراغ (*dll_GC_رجسٽر_traversers)(ننڍو ٽيگ, سائيز_Proc سائيز, Mark_Proc mark, Fixup_Proc fixup, صحيح is_constant_سائيز, صحيح is_atomic);
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 400
ساكن فراغ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ساكن فراغ **(*dll_scheme_malloc_immobile_box)(فراغ *p);
ساكن فراغ (*dll_scheme_free_immobile_box)(فراغ **b);
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ساكن صحيح (*dll_scheme_main_تياري)(صحيح no_auto_statics, Scheme_Env_Main _main, صحيح argc, حرف **argv);
#   إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || MZSCHEME_نسخو_MAJOR >= 603
ساكن فراغ (*dll_scheme_رجسٽر_tls_space)(فراغ *tls_space, صحيح tls_اشاريو);
#   endif
#  endif
#  إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ذريعي_PROC)
ساكن Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(فراغ);
#  endif
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 600
ساكن فراغ (*dll_scheme_embedded_ڀريو)(intptr_t len, ثابت حرف *s, صحيح predefined);
ساكن فراغ (*dll_scheme_رجسٽر_embedded_ڀريو)(intptr_t len, ثابت حرف *s);
ساكن فراغ (*dll_scheme_سيٽ_config_رستو)(Scheme_Object *p);
# endif

# إذا defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  إذا !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_جدول)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_جوڙيو_global dll_scheme_جوڙيو_global
#  define scheme_جوڙيو_global_نشان dll_scheme_جوڙيو_global_نشان
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_قدر dll_scheme_builtin_قدر
#  إذا MZSCHEME_نسخو_MAJOR >= 299
#   define scheme_byte_string_منزل_char_string dll_scheme_byte_string_منزل_char_string
#   define scheme_make_رستو dll_scheme_make_رستو
#  endif
#  define scheme_check_ڌاڳا dll_scheme_check_ڌاڳا
#  define scheme_close_داخلا_پورٽ dll_scheme_close_داخلا_پورٽ
#  define scheme_ڳڻپ_سٽون dll_scheme_ڳڻپ_سٽون
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_منزل_string dll_scheme_display_منزل_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_پڄاڻي_primitive_module dll_scheme_پڄاڻي_primitive_module
#  إذا MZSCHEME_نسخو_MAJOR < 299
#   define scheme_format dll_scheme_format
#  وإلا
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  إذا MZSCHEME_نسخو_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  وإلا
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_نشان dll_scheme_intern_نشان
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_قدر dll_scheme_make_integer_قدر
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  إذا MZSCHEME_نسخو_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_پورٽ dll_scheme_make_string_output_پورٽ
#  وإلا
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_پورٽ \
    dll_scheme_make_byte_string_output_پورٽ
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_نالا dll_scheme_make_struct_نالا
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_قدرون dll_scheme_make_struct_قدرون
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_ويڪٽر dll_scheme_make_ويڪٽر
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_داخلا_file dll_scheme_open_داخلا_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_ڊيگهائي dll_scheme_proper_list_ڊيگهائي
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_رجسٽر_static dll_scheme_رجسٽر_static
#  define scheme_سيٽ_ڍير_بنياد dll_scheme_سيٽ_ڍير_بنياد
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  إذا MZSCHEME_نسخو_MAJOR >= 299
#   define scheme_سيٽ_param dll_scheme_سيٽ_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_منزل_byte_string \
    dll_scheme_char_string_منزل_byte_string
#   define scheme_char_string_منزل_رستو \
    dll_scheme_char_string_منزل_رستو
#   define scheme_سيٽ_collects_رستو dll_scheme_سيٽ_collects_رستو
#  endif
#  define scheme_make_hash_جدول dll_scheme_make_hash_جدول
#  define scheme_hash_سيٽ dll_scheme_hash_سيٽ
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_رجسٽر_traversers dll_GC_رجسٽر_traversers
#  endif
#  إذا MZSCHEME_نسخو_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_تياري dll_scheme_main_تياري
#    إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || MZSCHEME_نسخو_MAJOR >= 603
#     define scheme_رجسٽر_tls_space dll_scheme_رجسٽر_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  إذا MZSCHEME_نسخو_MAJOR >= 600
#   define scheme_embedded_ڀريو dll_scheme_embedded_ڀريو
#   define scheme_رجسٽر_embedded_ڀريو dll_scheme_رجسٽر_embedded_ڀريو
#   define scheme_سيٽ_config_رستو dll_scheme_سيٽ_config_رستو
#  endif

#  ifndef PROTO
#   إذا MZSCHEME_نسخو_MAJOR >= 500
#    إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ذريعي_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(فراغ)
{
    أرجع dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

تعريفنوع بنية
{
    حرف    *name;
    فراغ    **ptr;
} Thunk_Info;

ساكن Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (فراغ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (فراغ **)&dll_GC_malloc_atomic},
    {عدم, عدم}};

ساكن Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (فراغ **)&dll_scheme_eof},
    {"scheme_false", (فراغ **)&dll_scheme_false},
    {"scheme_void", (فراغ **)&dll_scheme_void},
    {"scheme_null", (فراغ **)&dll_scheme_null},
    {"scheme_true", (فراغ **)&dll_scheme_true},
# إذا !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_جدول)
    {"scheme_current_thread", (فراغ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (فراغ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (فراغ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(فراغ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (فراغ **)&dll_scheme_جوڙيو_global},
    {"scheme_add_global_symbol", (فراغ **)&dll_scheme_جوڙيو_global_نشان},
    {"scheme_apply", (فراغ **)&dll_scheme_apply},
    {"scheme_basic_env", (فراغ **)&dll_scheme_basic_env},
# إذا MZSCHEME_نسخو_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (فراغ **)&dll_scheme_byte_string_منزل_char_string},
    {"scheme_make_path", (فراغ **)&dll_scheme_make_رستو},
# endif
    {"scheme_builtin_value", (فراغ **)&dll_scheme_builtin_قدر},
    {"scheme_check_threads", (فراغ **)&dll_scheme_check_ڌاڳا},
    {"scheme_close_input_port", (فراغ **)&dll_scheme_close_داخلا_پورٽ},
    {"scheme_count_lines", (فراغ **)&dll_scheme_ڳڻپ_سٽون},
    {"scheme_current_continuation_marks",
	(فراغ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (فراغ **)&dll_scheme_display},
    {"scheme_display_to_string", (فراغ **)&dll_scheme_display_منزل_string},
    {"scheme_do_eval", (فراغ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (فراغ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (فراغ **)&dll_scheme_eq},
    {"scheme_eval", (فراغ **)&dll_scheme_eval},
    {"scheme_eval_string", (فراغ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (فراغ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(فراغ **)&dll_scheme_پڄاڻي_primitive_module},
# إذا MZSCHEME_نسخو_MAJOR < 299
    {"scheme_format", (فراغ **)&dll_scheme_format},
# وإلا
    {"scheme_format_utf8", (فراغ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (فراغ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (فراغ **)&dll_scheme_gc_ptr_ok},
# إذا MZSCHEME_نسخو_MAJOR < 299
    {"scheme_get_sized_string_output",
	(فراغ **)&dll_scheme_get_sized_string_output},
# وإلا
    {"scheme_get_sized_byte_string_output",
	(فراغ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (فراغ **)&dll_scheme_intern_نشان},
    {"scheme_lookup_global", (فراغ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(فراغ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (فراغ **)&dll_scheme_make_integer_قدر},
    {"scheme_make_pair", (فراغ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (فراغ **)&dll_scheme_make_prim_w_arity},
# إذا MZSCHEME_نسخو_MAJOR < 299
    {"scheme_make_string", (فراغ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(فراغ **)&dll_scheme_make_string_output_پورٽ},
# وإلا
    {"scheme_make_byte_string", (فراغ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(فراغ **)&dll_scheme_make_byte_string_output_پورٽ},
# endif
    {"scheme_make_struct_instance",
	(فراغ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (فراغ **)&dll_scheme_make_struct_نالا},
    {"scheme_make_struct_type", (فراغ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (فراغ **)&dll_scheme_make_struct_قدرون},
    {"scheme_make_type", (فراغ **)&dll_scheme_make_type},
    {"scheme_make_vector", (فراغ **)&dll_scheme_make_ويڪٽر},
    {"scheme_malloc_fail_ok", (فراغ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (فراغ **)&dll_scheme_open_داخلا_file},
    {"scheme_primitive_module", (فراغ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (فراغ **)&dll_scheme_proper_list_ڊيگهائي},
    {"scheme_raise", (فراغ **)&dll_scheme_raise},
    {"scheme_read", (فراغ **)&dll_scheme_read},
    {"scheme_register_static", (فراغ **)&dll_scheme_رجسٽر_static},
    {"scheme_set_stack_base", (فراغ **)&dll_scheme_سيٽ_ڍير_بنياد},
    {"scheme_signal_error", (فراغ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (فراغ **)&dll_scheme_wrong_type},
# إذا MZSCHEME_نسخو_MAJOR >= 299
    {"scheme_set_param", (فراغ **)&dll_scheme_سيٽ_param},
    {"scheme_current_config", (فراغ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(فراغ **)&dll_scheme_char_string_منزل_byte_string},
    {"scheme_char_string_to_path", (فراغ **)&dll_scheme_char_string_منزل_رستو},
    {"scheme_set_collects_path", (فراغ **)&dll_scheme_سيٽ_collects_رستو},
# endif
    {"scheme_make_hash_table", (فراغ **)&dll_scheme_make_hash_جدول},
    {"scheme_hash_set", (فراغ **)&dll_scheme_hash_سيٽ},
    {"scheme_hash_get", (فراغ **)&dll_scheme_hash_get},
    {"scheme_make_double", (فراغ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (فراغ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (فراغ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (فراغ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (فراغ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (فراغ **)&dll_GC_رجسٽر_traversers},
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (فراغ **)&dll_scheme_main_تياري},
#   إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || MZSCHEME_نسخو_MAJOR >= 603
    {"scheme_register_tls_space", (فراغ **)&dll_scheme_رجسٽر_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (فراغ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (فراغ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (فراغ **)&dll_scheme_free_immobile_box},
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 500
#  إذا defined(IMPLEMENT_THREAD_LOCAL_ذريعي_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ذريعي_PROC)
    {"scheme_external_get_thread_local_variables", (فراغ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# إذا MZSCHEME_نسخو_MAJOR >= 600
    {"scheme_embedded_load", (فراغ **)&dll_scheme_embedded_ڀريو},
    {"scheme_register_embedded_load", (فراغ **)&dll_scheme_رجسٽر_embedded_ڀريو},
    {"scheme_set_config_path", (فراغ **)&dll_scheme_سيٽ_config_رستو},
# endif
    {عدم, عدم}};

ساكن HINSTANCE hMzGC = 0;
ساكن HINSTANCE hMzSch = 0;

ساكن فراغ dynamic_mzscheme_پڇاڙي(فراغ);
ساكن صحيح mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll,
	صحيح verbose);

    ساكن صحيح
mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll, صحيح verbose)
{
    Thunk_Info *thunk = عدم;

    إذا (hMzGC && hMzSch)
	أرجع OK;
    hMzSch = vim_ڀريو_Lib(sch_dll);
    hMzGC = vim_ڀريو_Lib(gc_dll);

    إذا (!hMzGC)
    {
	إذا (verbose)
	    semsg(_(e_could_not_ڀريو_library_str_str), gc_dll, GetWin32Error());
	أرجع FAIL;
    }

    إذا (!hMzSch)
    {
	إذا (verbose)
	    semsg(_(e_could_not_ڀريو_library_str_str), sch_dll, GetWin32Error());
	أرجع FAIL;
    }

    لكل (thunk = mzsch_imports; thunk->name; thunk++)
    {
	إذا ((*thunk->ptr =
		    (فراغ *)Get_Proc_پتو(hMzSch, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), thunk->name);
	    أرجع FAIL;
	}
    }
    لكل (thunk = mzgc_imports; thunk->name; thunk++)
    {
	إذا ((*thunk->ptr =
		    (فراغ *)Get_Proc_پتو(hMzGC, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), thunk->name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    صحيح
mzscheme_enabled(صحيح verbose)
{
    أرجع mzscheme_runtime_link_init(
	    (حرف *)p_mzschemedll, (حرف *)p_mzschemegcdll, verbose) == OK;
}

    ساكن فراغ
dynamic_mzscheme_پڇاڙي(فراغ)
{
    إذا (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    إذا (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#جيڪڏهن_پورو // DYNAMIC_MZSCHEME

#إذا MZSCHEME_نسخو_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, انگ) GUARANTEE_STRING(proc, انگ)
#وإلا
    ساكن Scheme_Object *
guaranteed_byte_string_arg(حرف *proc, صحيح انگ, صحيح argc, Scheme_Object **argv)
{
    إذا (SCHEME_BYTE_STRINGP(argv[انگ]))
    {
	أرجع argv[انگ];
    }
    وإلا إذا (SCHEME_CHAR_STRINGP(argv[انگ]))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندريون_REG(0, argv[انگ]);
	MZ_GC_VAR_اندريون_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_منزل_byte_string(argv[انگ]);
	MZ_GC_UNREG();
	أرجع tmp;
    }
    وإلا
	scheme_wrong_type(proc, "string", انگ, argc, argv);
    // unreachable
    أرجع scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, انگ) guaranteed_byte_string_arg(proc, انگ, argc, argv)
#جيڪڏهن_پورو

// need to put it here for dynamic stuff to work
#إذا defined(INCLUDE_MZSCHEME_بنياد)
# include "mzscheme_base.c"
#جيڪڏهن_پورو

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ساكن Scheme_Type mz_بفر_type;
ساكن Scheme_Type mz_دري_type;

ساكن صحيح initialized = FALSE;
#جيڪڏهن_وصف_ٿيل DYNAMIC_MZSCHEME
ساكن صحيح disabled = FALSE;
#جيڪڏهن_پورو
ساكن صحيح ڀريو_بنياد_module_failed = FALSE;

// global environment
ساكن Scheme_Env    *environment = عدم;
// output/error handlers
ساكن Scheme_Object *curout = عدم;
ساكن Scheme_Object *curerr = عدم;
// exn:vim exception
ساكن Scheme_Object *exn_catching_apply = عدم;
ساكن Scheme_Object *exn_p = عدم;
ساكن Scheme_Object *exn_پيغام = عدم;
ساكن Scheme_Object *vim_exn = عدم; // Vim Error exception

#إذا !defined(MZ_PRECISE_GC) || MZSCHEME_نسخو_MAJOR < 400
ساكن فراغ *ڍير_بنياد = عدم;
#جيڪڏهن_پورو

ساكن ڊگهو range_شروعات;
ساكن ڊگهو range_پڇاڙي;

// MzScheme threads scheduling stuff
ساكن صحيح mz_ڌاڳا_allow = 0;

#إذا defined(FEAT_GUI_MSWIN)
ساكن فراغ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ساكن UINT timer_id = 0;
#وإلاإذا defined(FEAT_GUI_GTK)
ساكن gboolean timer_proc(gpointer);
ساكن guint timer_id = 0;
#وإلاإذا defined(FEAT_GUI_MOTIF)
ساكن فراغ timer_proc(Xt_اشارو, XtIntervalId *);
ساكن XtIntervalId timer_id = (XtIntervalId)0;
#جيڪڏهن_پورو

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    فراغ
mzvim_check_ڌاڳا(فراغ)
{
    // Last time MzScheme threads were scheduled
    ساكن time_t mz_آخري_وقت = 0;

    إذا (mz_ڌاڳا_allow && p_mzq > 0)
    {
	time_t now = وقت(عدم);

	إذا ((now - mz_آخري_وقت) * 1000 > p_mzq)
	{
	    mz_آخري_وقت = now;
	    scheme_check_ڌاڳا();
	}
    }
}
#جيڪڏهن_پورو

#إذا defined(MZSCHEME_GUI_ڌاڳا)
ساكن فراغ تياري_timer(فراغ);
ساكن فراغ remove_timer(فراغ);

// timers are presented in GUI only
# إذا defined(FEAT_GUI_MSWIN)
    ساكن فراغ CALLBACK
timer_proc(
    HWND	hwnd اڻ_استعمال_ٿيل,
    UINT	u_نياپو اڻ_استعمال_ٿيل,
    UINT_PTR	idEvent اڻ_استعمال_ٿيل,
    DWORD	dw_وقت اڻ_استعمال_ٿيل)
# elif defined(FEAT_GUI_GTK)
    ساكن gboolean
timer_proc(gpointer معلومات اڻ_استعمال_ٿيل)
# elif defined(FEAT_GUI_MOTIF)
    ساكن فراغ
timer_proc(Xt_اشارو timed_ٻاهريون اڻ_استعمال_ٿيل, XtIntervalId *interval_id اڻ_استعمال_ٿيل)
# endif
{
    scheme_check_ڌاڳا();
# إذا defined(FEAT_GUI_GTK)
    أرجع TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    إذا (mz_ڌاڳا_allow && p_mzq > 0)
	timer_id = Xt_App_جوڙيو_وقت_ٻاهريون(app_context, p_mzq,
		timer_proc, عدم);
# endif
}

    ساكن فراغ
تياري_timer(فراغ)
{
# إذا defined(FEAT_GUI_MSWIN)
    timer_id = سيٽ_Timer(عدم, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_جوڙيو((guint)p_mzq, (GSourceFunc)timer_proc, عدم);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_جوڙيو_وقت_ٻاهريون(app_context, p_mzq, timer_proc, عدم);
# endif
}

    ساكن فراغ
remove_timer(فراغ)
{
# إذا defined(FEAT_GUI_MSWIN)
    KillTimer(عدم, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_ذريعو_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_وقت_ٻاهريون(timer_id);
# endif
    timer_id = 0;
}

#جيڪڏهن_پورو // MZSCHEME_GUI_THREADS

    حرف *
did_سيٽ_mzquantum(optset_T *دليلون اڻ_استعمال_ٿيل)
{
#إذا defined(MZSCHEME_GUI_ڌاڳا)
    // reset timer
    إذا (timer_id != 0)
	remove_timer();
    إذا (mz_ڌاڳا_allow && p_mzq > 0 && gui.اندريون_use)
	تياري_timer();
#جيڪڏهن_پورو
    أرجع عدم;
}

    ساكن فراغ
notify_multithread(صحيح on)
{
    mz_ڌاڳا_allow = on;
#جيڪڏهن_وصف_ٿيل MZSCHEME_GUI_ڌاڳا
    إذا (on && timer_id == 0 && p_mzq > 0 && gui.اندريون_use)
	تياري_timer();
    إذا (!on && timer_id != 0)
	remove_timer();
#جيڪڏهن_پورو
}

    فراغ
mzscheme_پڇاڙي(فراغ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#إذا 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_پڇاڙي();
# endif
#جيڪڏهن_پورو
}

#إذا HAVE_TLS_SPACE && !defined(VIMDLL)
# إذا defined(_MSC_VER)
ساكن __declspec(thread) فراغ *tls_space;
خارجي intptr_t tls_اشاريو;
# elif defined(__MINGW32__)
ساكن __thread فراغ *tls_space;
خارجي intptr_t tls_اشاريو;
# وإلا
ساكن THREAD_LOCAL فراغ *tls_space;
ساكن intptr_t tls_اشاريو = 0;
# endif
#جيڪڏهن_پورو

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    صحيح
mzscheme_main(فراغ)
{
    صحيح	    argc = 0;
    حرف    *argv = عدم;

#جيڪڏهن_وصف_ٿيل DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    إذا (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_TLS_SPACE
# ifdef VIMDLL
    فراغ **ptls_space;
    intptr_t tls_اشاريو_2;
    فراغ (*pget_tls_info)(فراغ ***ptls_space, intptr_t *ptls_اشاريو);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (فراغ *)Get_Proc_پتو(
				GetModuleHandle(عدم), "get_tls_info");
    إذا (pget_tls_info == عدم)
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_اشاريو_2);
    scheme_رجسٽر_tls_space(ptls_space, tls_اشاريو_2);
# وإلا
    scheme_رجسٽر_tls_space(&tls_space, tls_اشاريو);
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل TRAMPOLINED_MZVIM_STARTUP
    أرجع scheme_main_تياري(TRUE, mzscheme_env_main, argc, &argv);
#وإلا
    أرجع mzscheme_env_main(عدم, argc, &argv);
#جيڪڏهن_پورو
}

    ساكن صحيح
mzscheme_env_main(Scheme_Env *env, صحيح argc اڻ_استعمال_ٿيل, حرف **argv اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#وإلا
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, dummy);

    ڍير_بنياد = &__gc_var_stack__;
# وإلا
    صحيح dummy = 0;
    ڍير_بنياد = (فراغ *)&dummy;
# endif
#جيڪڏهن_پورو

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    أرجع 0;
}

    ساكن Scheme_Object*
ڀريو_بنياد_module(فراغ *معلومات)
{
    scheme_namespace_require(scheme_intern_نشان((حرف *)معلومات));
    أرجع scheme_null;
}

    ساكن Scheme_Object *
ڀريو_بنياد_module_on_error(فراغ *معلومات اڻ_استعمال_ٿيل)
{
    ڀريو_بنياد_module_failed = TRUE;
    أرجع scheme_null;
}

    ساكن صحيح
startup_mzscheme(فراغ)
{
#جيڪڏهن_وصف_نه_ٿيل TRAMPOLINED_MZVIM_STARTUP
    scheme_سيٽ_ڍير_بنياد(ڍير_بنياد, 1);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#جيڪڏهن_پورو

    MZ_رجسٽر_STATIC(environment);
    MZ_رجسٽر_STATIC(curout);
    MZ_رجسٽر_STATIC(curerr);
    MZ_رجسٽر_STATIC(exn_catching_apply);
    MZ_رجسٽر_STATIC(exn_p);
    MZ_رجسٽر_STATIC(exn_پيغام);
    MZ_رجسٽر_STATIC(vim_exn);

    MZ_GC_CHECK();

#جيڪڏهن_وصف_ٿيل INCLUDE_MZSCHEME_بنياد
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#جيڪڏهن_پورو

    // setup 'current-library-collection-paths' parameter
#إذا MZSCHEME_نسخو_MAJOR >= 299
    {
	Scheme_Object *coll_رستو = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, coll_رستو);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	إذا (s != عدم)
	{
	    coll_رستو = scheme_make_رستو((حرف *)s);
	    MZ_GC_CHECK();
	    إذا (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	إذا (coll_رستو == عدم)
	{
	    coll_رستو = scheme_make_رستو(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	إذا (coll_رستو != عدم)
	{
	    scheme_سيٽ_collects_رستو(coll_رستو);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#وإلا
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = عدم;
	Scheme_Object	*coll_pair = عدم;
	Scheme_Config	*config = عدم;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_اندريون_REG(0, coll_string);
	MZ_GC_VAR_اندريون_REG(1, coll_pair);
	MZ_GC_VAR_اندريون_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_سيٽ_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#جيڪڏهن_پورو

#إذا MZSCHEME_نسخو_MAJOR >= 600
    {
	Scheme_Object *config_رستو = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, config_رستو);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	إذا (s != عدم)
	{
	    config_رستو = scheme_make_رستو((حرف *)s);
	    MZ_GC_CHECK();
	    إذا (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	إذا (config_رستو == عدم)
	{
	    config_رستو = scheme_make_رستو(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	إذا (config_رستو != عدم)
	{
	    scheme_سيٽ_config_رستو(config_رستو);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#جيڪڏهن_پورو

#إذا MZSCHEME_نسخو_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#جيڪڏهن_پورو

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(عدم, ڀريو_بنياد_module, عدم,
				    ڀريو_بنياد_module_on_error, "racket/base");
	إذا (ڀريو_بنياد_module_failed)
	{
	    ڀريو_بنياد_module_failed = FALSE;
	    scheme_dynamic_wind(عدم, ڀريو_بنياد_module, عدم,
				    ڀريو_بنياد_module_on_error, "scheme/base");
	    إذا (ڀريو_بنياد_module_failed)
		أرجع -1;
	}
    }

    رجسٽر_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, make_security_guard);
	MZ_GC_REG();

# إذا MZSCHEME_نسخو_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_نشان = عدم;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندريون_REG(0, make_security_guard_نشان);
	    MZ_GC_REG();
	    make_security_guard_نشان = scheme_intern_نشان("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_نشان, environment);
	    MZ_GC_UNREG();
	}
# وإلا
	make_security_guard = scheme_builtin_قدر("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	إذا (make_security_guard != عدم)
	{
	    Scheme_Object   *دليلون[3] = {عدم, عدم, عدم};
	    Scheme_Object   *guard = عدم;
	    Scheme_Config   *config = عدم;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_اندريون_REG(0, دليلون, 3);
	    MZ_GC_VAR_اندريون_REG(3, guard);
	    MZ_GC_VAR_اندريون_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    دليلون[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    دليلون[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    دليلون[2] = scheme_make_prim_w_arity(sandbox_ڄار_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, دليلون);
	    MZ_GC_CHECK();
	    scheme_سيٽ_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#جيڪڏهن_پورو
    // Create buffer and window types for use in Scheme code
    mz_بفر_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_دري_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
    GC_رجسٽر_traversers(mz_بفر_type,
	    بفر_سائيز_proc, بفر_mark_proc, بفر_fixup_proc,
	    TRUE, TRUE);
    GC_رجسٽر_traversers(mz_دري_type,
	    دري_سائيز_proc, دري_mark_proc, دري_fixup_proc,
	    TRUE, TRUE);
#جيڪڏهن_پورو

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    أرجع 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ساكن صحيح
mzscheme_init(فراغ)
{
    إذا (!initialized)
    {
#جيڪڏهن_وصف_ٿيل DYNAMIC_MZSCHEME
	إذا (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    أرجع -1;
	}
#جيڪڏهن_پورو
	إذا (ڀريو_بنياد_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_بنياد_module_could_not_be_loaded));
	    أرجع -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_پورٽ();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_پورٽ();
	MZ_GC_CHECK();
	scheme_سيٽ_param(config, MZCONFIG_OUTPUT_پورٽ, curout);
	MZ_GC_CHECK();
	scheme_سيٽ_param(config, MZCONFIG_ERROR_پورٽ, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    أرجع 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ساكن صحيح
eval_with_exn_handling(
    فراغ		*معلومات,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *قدر = عدم;
    Scheme_Object   *exn = عدم;
    Scheme_Object   *prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندريون_REG(0, قدر);
    MZ_GC_VAR_اندريون_REG(1, exn);
    MZ_GC_VAR_اندريون_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, معلومات, "mzvim", 0, 0);
    MZ_GC_CHECK();
    قدر = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    إذا (!قدر)
    {
	قدر = ڪڍو_exn_پيغام(exn);
	// Got an exn?
	إذا (قدر)
	{
	    scheme_display(قدر, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	أرجع FAIL;
    }

    إذا (ret != عدم)	// if pointer to retval supported give it up
	*ret = قدر;
    // Print any result, as long as it's not a void
    وإلا إذا (!SCHEME_VOIDP(قدر))
    {
	scheme_display(قدر, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    أرجع OK;
}

/*
 * :mzscheme
 */
    ساكن صحيح
do_mzscheme_command(exarg_T *eap, فراغ *معلومات, Scheme_Closed_Prim *what)
{
    إذا (mzscheme_init())
	أرجع FAIL;

    range_شروعات = eap->line1;
    range_پڇاڙي = eap->line2;

    أرجع eval_with_exn_handling(معلومات, what, عدم);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    فراغ
mzscheme_بفر_free(buf_T *buf)
{
    إذا (buf->b_mzscheme_ref == عدم)
	أرجع;

    vim_mz_بفر *bp = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, bp);
    MZ_GC_REG();

    bp = بفر_REF(buf);
    bp->buf = INVALID_بفر_قدر;
#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(bp);
#جيڪڏهن_پورو
    buf->b_mzscheme_ref = عدم;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    فراغ
mzscheme_دري_free(win_T *win)
{
    إذا (win->w_mzscheme_ref == عدم)
	أرجع;

    vim_mz_دري *wp = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, wp);
    MZ_GC_REG();
    wp = دري_REF(win);
    wp->win = INVALID_دري_قدر;
#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(wp);
#جيڪڏهن_پورو
    win->w_mzscheme_ref = عدم;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    فراغ
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    إذا (eap->skip)
	أرجع;

    إذا (script == عدم)
	do_mzscheme_command(eap, eap->arg, do_eval);
    وإلا
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ساكن Scheme_Object *
do_ڀريو(فراغ *معلومات, صحيح noargc اڻ_استعمال_ٿيل, Scheme_Object **noargv اڻ_استعمال_ٿيل)
{
    Scheme_Object   *اظهار = عدم;
    Scheme_Object   *نتيجو = عدم;
    حرف	    *file = عدم;
    پورٽ_Info	    *pinfo = (پورٽ_Info *)معلومات;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندريون_REG(0, اظهار);
    MZ_GC_VAR_اندريون_REG(1, نتيجو);
    MZ_GC_VAR_اندريون_REG(2, file);
    MZ_GC_REG();

    file = (حرف *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->پورٽ = scheme_open_داخلا_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ڳڻپ_سٽون(pinfo->پورٽ);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    طالما (!SCHEME_EOFP(اظهار = scheme_read(pinfo->پورٽ)))
    {
	نتيجو = scheme_eval(اظهار, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_داخلا_پورٽ(pinfo->پورٽ);
    MZ_GC_CHECK();
    pinfo->پورٽ = عدم;
    MZ_GC_UNREG();
    أرجع نتيجو;
}

/*
 * :mzfile
 */
    فراغ
ex_mzfile(exarg_T *eap)
{
    پورٽ_Info	pinfo = {عدم, عدم};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, pinfo.پورٽ);
    MZ_GC_REG();

    pinfo.name = (حرف *)eap->arg;
    إذا (do_mzscheme_command(eap, &pinfo, do_ڀريو) != OK
	    && pinfo.پورٽ != عدم)	// looks like port was not closed
    {
	scheme_close_داخلا_پورٽ(pinfo.پورٽ);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
}


/*
 *========================================================================
 * Exception handling code -- cribbed form the MzScheme sources and
 * Matthew Flatt's "Inside PLT MzScheme" document.
 *========================================================================
 */
    ساكن فراغ
init_exn_catching_apply(فراغ)
{
    إذا (exn_catching_apply)
	أرجع;

    حرف *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_قدر("exn?");
    MZ_GC_CHECK();
    exn_پيغام = scheme_builtin_قدر("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ساكن Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    إذا (SCHEME_TRUEP(SCHEME_CAR(v)))
	أرجع SCHEME_CDR(v);
    وإلا
    {
	*exn = SCHEME_CDR(v);
	أرجع عدم;
    }
}

    ساكن Scheme_Object *
ڪڍو_exn_پيغام(Scheme_Object *v)
{
    إذا (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	أرجع _scheme_apply(exn_پيغام, 1, &v);
    وإلا
	أرجع عدم; // Not an exn structure
}

    ساكن Scheme_Object *
do_eval(فراغ *s, صحيح noargc اڻ_استعمال_ٿيل, Scheme_Object **noargv اڻ_استعمال_ٿيل)
{
    أرجع scheme_eval_string_all((حرف *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ساكن فراغ
do_intrnl_output(حرف *mesg, صحيح error)
{
    حرف *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    طالما (p)
    {
	*p = '\0';
	إذا (error)
	    emsg(prev);
	وإلا
	    نياپو(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    إذا (error)
	emsg(prev);
    وإلا
	نياپو(prev);
}

    ساكن فراغ
do_output(حرف *mesg, OUTPUT_LEN_TYPE len اڻ_استعمال_ٿيل)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ساكن فراغ
do_err_output(حرف *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ساكن فراغ
do_printf(حرف *format, ...)
{
    do_intrnl_output(format, 1);
}

    ساكن فراغ
do_flush(فراغ)
{
    حرف *buff;
    OUTPUT_LEN_TYPE ڊيگهائي;

    buff = scheme_get_sized_byte_string_output(curerr, &ڊيگهائي);
    MZ_GC_CHECK();
    إذا (ڊيگهائي)
    {
	do_err_output(buff);
	أرجع;
    }

    buff = scheme_get_sized_byte_string_output(curout, &ڊيگهائي);
    MZ_GC_CHECK();
    إذا (ڊيگهائي)
	do_output(buff, ڊيگهائي);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    ساكن Scheme_Object *
vim_command(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    Scheme_Object *cmd = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_قدر(cmd), عدم, عدم, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    أرجع scheme_void;
}

/*
 * (eval {expr-string})
 */
    ساكن Scheme_Object *
vim_eval(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)معلومات;
    Scheme_Object	*نتيجو = عدم;
    typval_T		*vim_نتيجو;
    Scheme_Object *اظهار = عدم;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, نتيجو);
    MZ_GC_VAR_اندريون_REG(1, اظهار);
    MZ_GC_REG();
    اظهار = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_نتيجو = eval_اظهار(BYTE_STRING_قدر(اظهار), عدم);

    إذا (vim_نتيجو == عدم)
	raise_vim_exn(_("invalid expression"));

    نتيجو = vim_منزل_mzscheme(vim_نتيجو);
    MZ_GC_CHECK();
    free_tv(vim_نتيجو);

    MZ_GC_UNREG();
    أرجع نتيجو;
#وإلا
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    أرجع scheme_false;
#جيڪڏهن_پورو
}

/*
 * (range-start)
 */
    ساكن Scheme_Object *
get_range_شروعات(
    فراغ		*معلومات اڻ_استعمال_ٿيل,
    صحيح			argc اڻ_استعمال_ٿيل,
    Scheme_Object	**argv اڻ_استعمال_ٿيل)
{
    أرجع scheme_make_integer(range_شروعات);
}

/*
 * (range-end)
 */
    ساكن Scheme_Object *
get_range_پڇاڙي(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
    أرجع scheme_make_integer(range_پڇاڙي);
}

/*
 * (beep)
 */
    ساكن Scheme_Object *
mzscheme_beep(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
    vim_beep(BO_LANG);
    أرجع scheme_void;
}

ساكن Scheme_Object *M_global = عدم;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ساكن Scheme_Object *
get_option(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    ڊگهو	    قدر;
    حرف	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = عدم;
    Scheme_Object   *name = عدم;
    صحيح		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, rval);
    MZ_GC_VAR_اندريون_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    إذا (argc > 1)
    {
	إذا (M_global == عدم)
	{
	    MZ_رجسٽر_STATIC(M_global);
	    M_global = scheme_intern_نشان("global");
	    MZ_GC_CHECK();
	}

	إذا (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وإلا إذا (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_بفر(argv[1]);
	    scope = OPT_LOCAL;
	}
	وإلا إذا (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_دري(argv[1]);

	    curwin = win;
	    curbuf = win->w_بفر;
	    scope = OPT_LOCAL;
	}
	وإلا
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_قدر(BYTE_STRING_قدر(name), &قدر, (char_u **)&strval,
								  عدم, scope);
    curbuf = save_curb;
    curwin = save_curw;

    بدل (rc)
    {
    حالة gov_bool:
    حالة gov_عدد:
	MZ_GC_UNREG();
	أرجع scheme_make_integer_قدر(قدر);
    حالة gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	أرجع rval;
    حالة gov_hidden_bool:
    حالة gov_hidden_عدد:
    حالة gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    حالة gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    أرجع scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ساكن Scheme_Object *
سيٽ_option(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    char_u	*command = عدم;
    صحيح		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    Scheme_Object *cmd = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    إذا (argc > 1)
    {
	إذا (M_global == عدم)
	{
	    MZ_رجسٽر_STATIC(M_global);
	    M_global = scheme_intern_نشان("global");
	    MZ_GC_CHECK();
	}

	إذا (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وإلا إذا (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_بفر(argv[1]);
	    scope = OPT_LOCAL;
	}
	وإلا إذا (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_دري(argv[1]);
	    curwin = win;
	    curbuf = win->w_بفر;
	    scope = OPT_LOCAL;
	}
	وإلا
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_قدر(cmd));
    MZ_GC_UNREG();
    do_سيٽ(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    أرجع scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ساكن Scheme_Object *
get_curr_win(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
    أرجع (Scheme_Object *)get_vim_curr_دري();
}

/*
 * (win-count)
 */
    ساكن Scheme_Object *
get_دري_ڳڻپ(
    فراغ		*معلومات اڻ_استعمال_ٿيل,
    صحيح			argc اڻ_استعمال_ٿيل,
    Scheme_Object	**argv اڻ_استعمال_ٿيل)
{
    صحيح	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    أرجع scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ساكن Scheme_Object *
get_دري_list(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_بفر_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    لكل ( ; w != عدم; w = w->w_ايندڙ)
	إذا (w->w_بفر == buf->buf)
	{
	    list = scheme_make_pair(دري_new(w), list);
	    MZ_GC_CHECK();
	}

    أرجع list;
}

    ساكن Scheme_Object *
دري_new(win_T *win)
{
    vim_mz_دري *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    إذا (win->w_mzscheme_ref != عدم)
	أرجع (Scheme_Object *)دري_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_دري));
    CLEAR_اشارو(self);
#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#جيڪڏهن_پورو
    MZ_GC_CHECK();
    دري_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_دري_type;

    MZ_GC_UNREG();
    أرجع (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ساكن Scheme_Object *
get_دري_انگ(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
    صحيح		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    win_T	*win = get_دري_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    لكل (wp = firstwin; wp != win; wp = wp->w_ايندڙ)
	++nr;

    أرجع scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ساكن Scheme_Object *
get_دري_by_انگ(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    win_T	*win = firstwin;
    صحيح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    إذا (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    لكل ( ; win != عدم; win = win->w_ايندڙ, --fnum)
	إذا (fnum == 1)	    // to be 1-based
	    أرجع دري_new(win);

    أرجع scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ساكن Scheme_Object *
get_دري_بفر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win = get_دري_arg(prim->name, 0, argc, argv);

    أرجع بفر_new(win->win->w_بفر);
}

/*
 * (get-win-height [window])
 */
    ساكن Scheme_Object *
get_دري_اوچائي(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win = get_دري_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_اوچائي);
}

/*
 * (set-win-height {height} [window])
 */
    ساكن Scheme_Object *
سيٽ_دري_اوچائي(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win;
    win_T	    *savewin;
    صحيح		    اوچائي;

    win = get_دري_arg(prim->name, 1, argc, argv);
    اوچائي = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    need_mouse_correct = TRUE;
#جيڪڏهن_پورو

    savewin = curwin;
    curwin = win->win;
    win_setheight(اوچائي);
    curwin = savewin;

    raise_if_error();
    أرجع scheme_void;
}

/*
 * (get-win-width [window])
 */
    ساكن Scheme_Object *
get_دري_ويڪر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win = get_دري_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_ويڪر);
}

/*
 * (set-win-width {width} [window])
 */
    ساكن Scheme_Object *
سيٽ_دري_ويڪر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win;
    win_T	    *savewin;
    صحيح		    ويڪر = 0;

    win = get_دري_arg(prim->name, 1, argc, argv);
    ويڪر = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    need_mouse_correct = TRUE;
#جيڪڏهن_پورو

    savewin = curwin;
    curwin = win->win;
    win_setwidth(ويڪر);
    curwin = savewin;

    raise_if_error();
    أرجع scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ساكن Scheme_Object *
get_cursor(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win;
    pos_T	    pos;

    win = get_دري_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    أرجع scheme_make_pair(scheme_make_integer_قدر((ڊگهو)pos.lnum),
		    scheme_make_integer_قدر((ڊگهو)pos.کانو + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ساكن Scheme_Object *
سيٽ_cursor(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_دري   *win;
    ڊگهو	    lnum = 0;
    ڊگهو	    کانو = 0;

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    sandbox_check();
#جيڪڏهن_پورو
    win = get_دري_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    إذا (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    کانو = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_سٽ_range(lnum, win->win->w_بفر);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.کانو = کانو;
    win->win->w_سيٽ_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    أرجع scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ساكن Scheme_Object *
mzscheme_open_بفر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    صحيح		    انگ = 0;
    Scheme_Object   *onum = عدم;
    Scheme_Object   *buf = عدم;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندريون_REG(0, onum);
    MZ_GC_VAR_اندريون_REG(1, buf);
    MZ_GC_VAR_اندريون_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    sandbox_check();
#جيڪڏهن_پورو
    // TODO make open existing file
    انگ = buflist_جوڙيو(BYTE_STRING_قدر(fname), BLN_LISTED | BLN_CURBUF);

    إذا (انگ == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(انگ);
    buf = get_بفر_by_انگ(معلومات, 1, &onum);
    MZ_GC_UNREG();
    أرجع buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ساكن Scheme_Object *
get_بفر_by_انگ(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    buf_T	*buf;
    صحيح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	إذا (buf->b_fnum == fnum)
	    أرجع بفر_new(buf);

    أرجع scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ساكن Scheme_Object *
get_بفر_by_name(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    buf_T	*buf;
    Scheme_Object *بفر = عدم;
    Scheme_Object *fname = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, بفر);
    MZ_GC_VAR_اندريون_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    بفر = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	إذا (buf->b_ffname == عدم || buf->b_sfname == عدم)
	    // empty string
	{
	    إذا (BYTE_STRING_قدر(fname)[0] == NUL)
		بفر = بفر_new(buf);
	}
	وإلا إذا (!fnamecmp(buf->b_ffname, BYTE_STRING_قدر(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_قدر(fname)))
	{
	    // either short or long filename matches
	    بفر = بفر_new(buf);
	}
    }

    MZ_GC_UNREG();
    أرجع بفر;
}

/*
 * (get-next-buff [buffer])
 */
    ساكن Scheme_Object *
get_ايندڙ_بفر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    buf_T	*buf = get_بفر_arg(prim->name, 0, argc, argv)->buf;

    إذا (buf->b_ايندڙ == عدم)
	أرجع scheme_false;
    وإلا
	أرجع بفر_new(buf->b_ايندڙ);
}

/*
 * (get-prev-buff [buffer])
 */
    ساكن Scheme_Object *
get_prev_بفر(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)معلومات;
    buf_T	*buf = get_بفر_arg(prim->name, 0, argc, argv)->buf;

    إذا (buf->b_prev == عدم)
	أرجع scheme_false;
    وإلا
	أرجع بفر_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ساكن Scheme_Object *
get_بفر_انگ(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ساكن Scheme_Object *
get_بفر_ڳڻپ(
    فراغ		*معلومات اڻ_استعمال_ٿيل,
    صحيح			argc اڻ_استعمال_ٿيل,
    Scheme_Object	**argv اڻ_استعمال_ٿيل)
{
    buf_T   *b;
    صحيح	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    أرجع scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ساكن Scheme_Object *
get_بفر_name(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_byte_string((حرف *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ساكن Scheme_Object *
get_curr_بفر(
    فراغ		*معلومات اڻ_استعمال_ٿيل,
    صحيح			argc اڻ_استعمال_ٿيل,
    Scheme_Object	**argv اڻ_استعمال_ٿيل)
{
    أرجع (Scheme_Object *)get_vim_curr_بفر();
}

    ساكن Scheme_Object *
بفر_new(buf_T *buf)
{
    vim_mz_بفر *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    إذا (buf->b_mzscheme_ref)
	أرجع (Scheme_Object *)بفر_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_بفر));
    CLEAR_اشارو(self);
#جيڪڏهن_وصف_ٿيل MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#جيڪڏهن_پورو
    MZ_GC_CHECK();
    بفر_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_بفر_type;

    MZ_GC_UNREG();
    أرجع (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ساكن Scheme_Object *
get_بفر_سائيز(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_ml.ml_سٽ_ڳڻپ);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ساكن Scheme_Object *
get_بفر_سٽ(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf;
    صحيح		    linenr;
    char_u	    *سٽ;

    buf = get_بفر_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    سٽ = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    أرجع scheme_make_byte_string((حرف *)سٽ);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ساكن Scheme_Object *
get_بفر_سٽ_list(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf;
    صحيح		    i, hi, lo, n;
    Scheme_Object   *list = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, list);
    MZ_GC_REG();

    buf = get_بفر_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    إذا (lo < 0)
	lo = 0;

    إذا (hi < 0)
	hi = 0;
    إذا (hi < lo)
	hi = lo;

    n = hi - lo;

    لكل (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (حرف *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    أرجع list;
}

/*
 * (set-buff-line {linenr} {string/#f} [buffer])
 *
 * Replace a line in the specified buffer. The line number is
 * in Vim format (1-based). The replacement line is given as
 * an MzScheme string object. The object is checked for validity
 * and correct format. An exception is thrown if the values are not
 * the correct format.
 *
 * It returns a Scheme Object that indicates the length of the
 * string changed.
 */
    ساكن Scheme_Object *
سيٽ_بفر_سٽ(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf;
    Scheme_Object   *سٽ = عدم;
    حرف	    *save;
    صحيح		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, سٽ);
    MZ_GC_REG();

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    sandbox_check();
#جيڪڏهن_پورو
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    إذا (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    سٽ = argv[1];
    buf = get_بفر_arg(prim->name, 2, argc, argv);

    check_سٽ_range(n, buf->buf);

    إذا (SCHEME_FALSEP(سٽ))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	إذا (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا إذا (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	إذا (buf->buf == curwin->w_بفر)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_سٽون_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
    وإلا
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_منزل_سٽ(سٽ);

	curbuf = buf->buf;

	إذا (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا إذا (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	وإلا
	{
	    vim_free(save);
	    بدليو_بائيٽون((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	إذا (buf->buf == curwin->w_بفر)
	    check_cursor_کانو();

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
}

    ساكن فراغ
free_array(حرف **array)
{
    حرف **curr = array;
    طالما (*curr != عدم)
	vim_free(*curr++);
    vim_free(array);
}

/*
 * (set-buff-line-list {start} {end} {string-list/#f/null} [buffer])
 *
 * Replace a range of lines in the specified buffer. The line numbers are in
 * Vim format (1-based). The range is from lo up to, but not including, hi.
 * The replacement lines are given as a Scheme list of string objects. The
 * list is checked for validity and correct format.
 *
 * Errors are returned as a value of FAIL. The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change is set to the
 * change in the buffer length.
 */
    ساكن Scheme_Object *
سيٽ_بفر_سٽ_list(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf = عدم;
    Scheme_Object   *سٽ_list = عدم;
    صحيح		    i, old_len, new_len, hi, lo;
    ڊگهو	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, سٽ_list);
    MZ_GC_REG();

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    sandbox_check();
#جيڪڏهن_پورو
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    إذا (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    سٽ_list = argv[2];
    buf = get_بفر_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    إذا (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_سٽ_range(lo, buf->buf);	    // inclusive
    check_سٽ_range(hi - 1, buf->buf);	    // exclusive

    إذا (SCHEME_FALSEP(سٽ_list) || SCHEME_NULLP(سٽ_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	إذا (u_savedel((linenr_T)lo, (ڊگهو)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا
	{
	    لكل (i = 0; i < old_len; i++)
		إذا (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    إذا (buf->buf == curwin->w_بفر)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_سٽون_mark((linenr_T)lo, (ڊگهو)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
    وإلا
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_ڊيگهائي(سٽ_list);
	MZ_GC_CHECK();
	إذا (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	وإلا
	{
	    حرف		**array = عدم;
	    Scheme_Object   *سٽ = عدم;
	    Scheme_Object   *rest = عدم;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_اندريون_REG(0, سٽ);
	    MZ_GC_VAR_اندريون_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(حرف *, new_len + 1);

	    rest = سٽ_list;
	    لكل (i = 0; i < new_len; ++i)
	    {
		سٽ = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		إذا (!SCHEME_STRINGP(سٽ))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_منزل_سٽ(سٽ);
	    }

	    curbuf = buf->buf;

	    إذا (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
	    {
		curbuf = savebuf;
		free_array(array);
		raise_vim_exn(_("cannot save undo information"));
	    }

	    /*
	     * If the size of the range is reducing (ie, new_len < old_len) we
	     * need to delete some old_len. We do this at the start, by
	     * repeatedly deleting line "lo".
	     */
	    لكل (i = 0; i < old_len - new_len; ++i)
	    {
		إذا (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot delete line"));
		}
		extra--;
	    }

	    /*
	     * For as long as possible, replace the existing old_len with the
	     * new old_len. This is a more efficient operation, as it requires
	     * less memory allocation and freeing.
	     */
	    لكل (i = 0; i < old_len && i < new_len; i++)
		إذا (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot replace line"));
		}

	    /*
	     * Now we may need to insert the remaining new_len.  We don't need to
	     * free the string passed back because MzScheme has control of that
	     * memory.
	     */
	    طالما (i < new_len)
	    {
		إذا (ml_append((linenr_T)(lo + i - 1),
			    (char_u *)array[i], 0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}
		++i;
		++extra;
	    }
	    MZ_GC_UNREG();
	    free_array(array);
	}

	/*
	 * Adjust marks. Invalidate any which lie in the
	 * changed range, and move any in the remainder of the buffer.
	 */
	mark_adjust((linenr_T)lo, (linenr_T)(hi - 1),
						  (ڊگهو)MAXLNUM, (ڊگهو)extra);
	بدليو_سٽون((linenr_T)lo, 0, (linenr_T)hi, (ڊگهو)extra);

	إذا (buf->buf == curwin->w_بفر)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
}

/*
 * (insert-buff-line-list {linenr} {string/string-list} [buffer])
 *
 * Insert a number of lines into the specified buffer after the specified line.
 * The line number is in Vim format (1-based). The lines to be inserted are
 * given as an MzScheme list of string objects or as a single string. The lines
 * to be added are checked for validity and correct format. Errors are
 * returned as a value of FAIL.  The return value is OK on success.
 * If OK is returned and len_change is not NULL, *len_change
 * is set to the change in the buffer length.
 */
    ساكن Scheme_Object *
وجهو_بفر_سٽ_list(فراغ *معلومات, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)معلومات;
    vim_mz_بفر   *buf = عدم;
    Scheme_Object   *list = عدم;
    حرف	    *str = عدم;
    صحيح		    i, n, سائيز;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, list);
    MZ_GC_REG();

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    sandbox_check();
#جيڪڏهن_پورو
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    إذا (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_بفر_arg(prim->name, 2, argc, argv);

    إذا (n != 0)	    // 0 can be used in insert
	check_سٽ_range(n, buf->buf);
    إذا (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_منزل_سٽ(list);
	curbuf = buf->buf;

	إذا (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا إذا (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	وإلا
	{
	    vim_free(str);
	    appended_سٽون_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }

    // List
    سائيز = scheme_proper_list_ڊيگهائي(list);
    MZ_GC_CHECK();
    إذا (سائيز < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    وإلا
    {
	Scheme_Object   *سٽ = عدم;
	Scheme_Object   *rest = عدم;
	حرف		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندريون_REG(0, سٽ);
	MZ_GC_VAR_اندريون_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(حرف *, سائيز + 1);

	rest = list;
	لكل (i = 0; i < سائيز; ++i)
	{
	    سٽ = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_منزل_سٽ(سٽ);
	}

	curbuf = buf->buf;

	إذا (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا
	{
	    لكل (i = 0; i < سائيز; ++i)
		إذا (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    إذا (i > 0)
		appended_سٽون_mark((linenr_T)n, (ڊگهو)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    أرجع scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ساكن Scheme_Object *
vim_bufferp(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv)
{
    إذا (SCHEME_VIMBUFFERP(argv[0]))
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (win? obj)
 */
    ساكن Scheme_Object *
vim_windowp(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv)
{
    إذا (SCHEME_VIMWINDOWP(argv[0]))
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ساكن Scheme_Object *
vim_بفر_validp(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv)
{
    إذا (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_بفر *)argv[0])->buf != INVALID_بفر_قدر)
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (win-valid? obj)
 */
    ساكن Scheme_Object *
vim_دري_validp(فراغ *معلومات اڻ_استعمال_ٿيل, صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv)
{
    إذا (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_دري *)argv[0])->win != INVALID_دري_قدر)
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 *===========================================================================
 * Utilities
 *===========================================================================
 */

/*
 * Convert an MzScheme string into a Vim line.
 *
 * All internal nulls are replaced by newline characters.
 * It is an error for the string to contain newline characters.
 *
 * Returns pointer to Vim allocated memory
 */
    ساكن حرف *
string_منزل_سٽ(Scheme_Object *obj)
{
    حرف	*scheme_str = عدم;
    حرف	*vim_str = عدم;
    OUTPUT_LEN_TYPE	len;
    صحيح		i;

    scheme_str = scheme_display_منزل_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    إذا (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    لكل (i = 0; i < len; ++i)
    {
	إذا (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	وإلا
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    أرجع vim_str;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ساكن Scheme_Object *
vim_منزل_mzscheme(typval_T *vim_قدر)
{
    Scheme_Object	*نتيجو = عدم;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_جدول	*گهميل = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, نتيجو);
    MZ_GC_VAR_اندريون_REG(1, گهميل);
    MZ_GC_REG();

    گهميل = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    نتيجو = vim_منزل_mzscheme_impl(vim_قدر, 1, گهميل);

    MZ_GC_UNREG();
    أرجع نتيجو;
}

    ساكن Scheme_Object *
vim_منزل_mzscheme_impl(
    typval_T		*vim_قدر,
    صحيح			اونهائي,
    Scheme_Hash_جدول	*گهميل)
{
    Scheme_Object   *نتيجو = عدم;
    صحيح		    new_قدر_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, نتيجو);
    MZ_GC_VAR_اندريون_REG(1, گهميل);
    MZ_GC_REG();

    // Avoid infinite recursion
    إذا (اونهائي > 100)
    {
	MZ_GC_UNREG();
	أرجع scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    نتيجو = scheme_hash_get(گهميل, (Scheme_Object *)vim_قدر);
    MZ_GC_CHECK();
    إذا (نتيجو != عدم) // found, do nothing
	new_قدر_2 = FALSE;
    وإلا إذا (vim_قدر->v_type == VAR_STRING)
    {
	نتيجو = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_عدد)
    {
	نتيجو = scheme_make_integer((ڊگهو)vim_قدر->vval.v_عدد);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_FLOAT)
    {
	نتيجو = scheme_make_double((مزدوج)vim_قدر->vval.v_float);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_LIST)
    {
	list_T		*list = vim_قدر->vval.v_list;
	listitem_T	*curr;

	إذا (list == عدم || list->lv_پهريون == عدم)
	    نتيجو = scheme_null;
	وإلا
	{
	    Scheme_Object   *obj = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندريون_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_آخري;
	    obj = vim_منزل_mzscheme_impl(&curr->li_tv, اونهائي + 1, گهميل);
	    نتيجو = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    طالما (curr != list->lv_پهريون)
	    {
		curr = curr->li_prev;
		obj = vim_منزل_mzscheme_impl(&curr->li_tv, اونهائي + 1, گهميل);
		نتيجو = scheme_make_pair(obj, نتيجو);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    وإلا إذا (vim_قدر->v_type == VAR_DICT)
    {
	Scheme_Object	  *ڪنجي = عدم;
	Scheme_Object	  *obj = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندريون_REG(0, ڪنجي);
	MZ_GC_VAR_اندريون_REG(1, obj);
	MZ_GC_REG();

	نتيجو = (Scheme_Object *)scheme_make_hash_جدول(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	إذا (vim_قدر->vval.v_dict != عدم)
	{
	    hashtab_T	*ht = &vim_قدر->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_استعمال_ٿيل;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_شيون(ht, hi, todo)
	    {
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_منزل_mzscheme_impl(&di->di_tv, اونهائي + 1, گهميل);
		    ڪنجي = scheme_make_byte_string((حرف *)hi->hi_ڪنجي);
		    MZ_GC_CHECK();
		    scheme_hash_سيٽ((Scheme_Hash_جدول *)نتيجو, ڪنجي, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    وإلا إذا (vim_قدر->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = عدم;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
	نتيجو = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ثابت حرف *)BYTE_STRING_قدر(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    وإلا إذا (vim_قدر->v_type == VAR_PARTIAL)
    {
	إذا (vim_قدر->vval.v_partial == عدم)
	    نتيجو = scheme_null;
	وإلا
	{
	    Scheme_Object *funcname = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندريون_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (حرف *)partial_name(vim_قدر->vval.v_partial));
	    MZ_GC_CHECK();
	    نتيجو = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ثابت حرف *)BYTE_STRING_قدر(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    وإلا إذا (vim_قدر->v_type == VAR_BOOL || vim_قدر->v_type == VAR_SPECIAL)
    {
	إذا (vim_قدر->vval.v_عدد <= VVAL_TRUE)
	    نتيجو = scheme_make_integer((ڊگهو)vim_قدر->vval.v_عدد);
	وإلا
	    نتيجو = scheme_null;
	MZ_GC_CHECK();
    }
    وإلا
    {
	نتيجو = scheme_void;
	new_قدر_2 = FALSE;
    }
    إذا (new_قدر_2)
    {
	scheme_hash_سيٽ(گهميل, (Scheme_Object *)vim_قدر, نتيجو);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    أرجع نتيجو;
}

    ساكن صحيح
mzscheme_منزل_vim(Scheme_Object *obj, typval_T *tv)
{
    صحيح i, حالت;
    Scheme_Hash_جدول	*گهميل = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, obj);
    MZ_GC_VAR_اندريون_REG(1, گهميل);
    MZ_GC_REG();

    گهميل = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    حالت = mzscheme_منزل_vim_impl(obj, tv, 1, گهميل);
    لكل (i = 0; i < گهميل->سائيز; ++i)
    {
	// free up remembered objects
	إذا (گهميل->vals[i] != عدم)
	    free_tv((typval_T *)گهميل->vals[i]);
    }

    MZ_GC_UNREG();
    أرجع حالت;
}
    ساكن صحيح
mzscheme_منزل_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح اونهائي,
	Scheme_Hash_جدول *گهميل)
{
    صحيح		حالت = OK;
    typval_T	*مليو;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, obj);
    MZ_GC_VAR_اندريون_REG(1, گهميل);
    MZ_GC_REG();

    MZ_GC_CHECK();
    إذا (اونهائي > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = 0;
	أرجع FAIL;
    }

    مليو = (typval_T *)scheme_hash_get(گهميل, obj);
    إذا (مليو != عدم)
	copy_tv(مليو, tv);
    وإلا إذا (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_عدد = VVAL_NULL;
    }
    وإلا إذا (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = SCHEME_INT_VAL(obj);
    }
    وإلا إذا (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_عدد = SCHEME_TRUEP(obj);
    }
    وإلا إذا (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    وإلا إذا (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_قدر(obj));
    }
# إذا MZSCHEME_نسخو_MAJOR >= 299
    وإلا إذا (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندريون_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_منزل_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_قدر(tmp));
	MZ_GC_UNREG();
    }
# endif
    وإلا إذا (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	إذا (list == عدم)
	    حالت = FAIL;
	وإلا
	{
	    صحيح		    i;
	    Scheme_Object   *curr = عدم;
	    Scheme_Object   *cval = عدم;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_اندريون_REG(0, curr);
	    MZ_GC_VAR_اندريون_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    إذا (v == عدم)
		حالت = FAIL;
	    وإلا
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *گهميل_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, گهميل_tv);
		scheme_hash_سيٽ(گهميل, obj, (Scheme_Object *)گهميل_tv);

		إذا (SCHEME_VECTORP(obj))
		{
		    لكل (i = 0; i < SCHEME_VEC_سائيز(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			حالت = mzscheme_منزل_vim_impl(cval, v, اونهائي + 1, گهميل);
			إذا (حالت == FAIL)
			    اكسر;
			حالت = list_append_tv(list, v);
			clear_tv(v);
			إذا (حالت == FAIL)
			    اكسر;
		    }
		}
		وإلا إذا (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    لكل (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			حالت = mzscheme_منزل_vim_impl(cval, v, اونهائي + 1, گهميل);
			إذا (حالت == FAIL)
			    اكسر;
			حالت = list_append_tv(list, v);
			clear_tv(v);
			إذا (حالت == FAIL)
			    اكسر;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    إذا (حالت == OK && !SCHEME_NULLP(curr))
		    {
			حالت = mzscheme_منزل_vim_impl(cval, v, اونهائي + 1, گهميل);
			إذا (حالت == OK)
			{
			    حالت = list_append_tv(list, v);
			    clear_tv(v);
			}
		    }
		}
		// nothing to do for scheme_null
		vim_free(v);
	    }
	    MZ_GC_UNREG();
	}
    }
    وإلا إذا (SCHEME_HASHTP(obj))
    {
	صحيح		i;
	dict_T		*dict;
	Scheme_Object   *ڪنجي = عدم;
	Scheme_Object   *val = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندريون_REG(0, ڪنجي);
	MZ_GC_VAR_اندريون_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	إذا (dict == عدم)
	    حالت = FAIL;
	وإلا
	{
	    typval_T    *گهميل_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, گهميل_tv);
	    scheme_hash_سيٽ(گهميل, obj, (Scheme_Object *)گهميل_tv);

	    لكل (i = 0; i < ((Scheme_Hash_جدول *)obj)->سائيز; ++i)
	    {
		إذا (((Scheme_Hash_جدول *) obj)->vals[i] != عدم)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *شي = dictitem_alloc((char_u *)string_منزل_سٽ(
				((Scheme_Hash_جدول *) obj)->ڪنجيون[i]));
		    // convert Scheme val to Vim and add it to the dict
		    إذا (mzscheme_منزل_vim_impl(((Scheme_Hash_جدول *) obj)->vals[i],
				    &شي->di_tv, اونهائي + 1, گهميل) == FAIL
			    || dict_جوڙيو(dict, شي) == FAIL)
		    {
			dictitem_free(شي);
			حالت = FAIL;
			اكسر;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    وإلا
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_منزل_سٽ(obj);
    }
    MZ_GC_UNREG();
    أرجع حالت;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ساكن Scheme_Object *
vim_funcref(فراغ *name, صحيح argc, Scheme_Object **argv)
{
    صحيح i;
    typval_T دليلون;
    صحيح حالت = OK;
    Scheme_Object *نتيجو = عدم;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, نتيجو);
    MZ_GC_REG();

    نتيجو = scheme_void;
    إذا (list == عدم)
	حالت = FAIL;
    وإلا
    {
	دليلون.v_type = VAR_LIST;
	دليلون.vval.v_list = list;
	++list->lv_refcount;
	لكل (i = 0; حالت == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    إذا (v == عدم)
		حالت = FAIL;
	    وإلا
	    {
		حالت = mzscheme_منزل_vim(argv[i], v);
		إذا (حالت == OK)
		{
		    حالت = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	إذا (حالت == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_قدر((Scheme_Object *)name), &دليلون, &ret);
	    MZ_GC_CHECK();
	    نتيجو = vim_منزل_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&دليلون);
    MZ_GC_UNREG();
    إذا (حالت != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    وإلا
	raise_if_error();
    أرجع نتيجو;
}

    فراغ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندريون_REG(0, ret);
    MZ_GC_REG();

    إذا (mzscheme_init())
    {
	MZ_GC_UNREG();
	أرجع;
    }

    MZ_GC_CHECK();
    إذا (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_منزل_vim(ret, rettv);

    MZ_GC_UNREG();
}
#جيڪڏهن_پورو

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ساكن صحيح
vim_error_check(فراغ)
{
    أرجع (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ساكن فراغ
رجسٽر_vim_exn(فراغ)
{
    صحيح	nc = 0;
    صحيح i;
    Scheme_Object   *struct_exn = عدم;
    Scheme_Object   *exn_name = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندريون_REG(0, struct_exn);
    MZ_GC_VAR_اندريون_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_نشان("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_قدر("struct:exn");
    MZ_GC_CHECK();

    إذا (vim_exn == عدم)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, عدم, 0, 0, عدم, عدم
#إذا MZSCHEME_نسخو_MAJOR >= 299
		, عدم
#جيڪڏهن_پورو
		);


    {
	Scheme_Object   **tmp = عدم;
	Scheme_Object   *exn_نالا[5] = {عدم, عدم, عدم, عدم, عدم};
	Scheme_Object   *exn_قدرون[5] = {عدم, عدم, عدم, عدم, عدم};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_اندريون_REG(0, exn_نالا, 5);
	MZ_GC_ARRAY_VAR_اندريون_REG(3, exn_قدرون, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_نالا(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_نالا, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_قدرون(vim_exn, exn_نالا, nc, 0);
	mch_memmove(exn_قدرون, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	لكل (i = 0; i < nc; i++)
	{
	    scheme_جوڙيو_global_نشان(exn_نالا[i],
		    exn_قدرون[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    فراغ
raise_vim_exn(ثابت حرف *جوڙيو_info)
{
    حرف	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {عدم, عدم};
    Scheme_Object   *exn = عدم;
    Scheme_Object   *byte_string = عدم;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_اندريون_REG(0, argv, 2);
    MZ_GC_VAR_اندريون_REG(3, exn);
    MZ_GC_VAR_اندريون_REG(4, byte_string);
    MZ_GC_REG();

    إذا (جوڙيو_info != عدم)
    {
	حرف		*c_string = عدم;
	Scheme_Object   *info = عدم;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_اندريون_REG(0, c_string);
	MZ_GC_VAR_اندريون_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(جوڙيو_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (صحيح)STRLEN(fmt), 1, &info, عدم);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_منزل_char_string(byte_string);
	SCHEME_سيٽ_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    وإلا
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_منزل_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#إذا MZSCHEME_نسخو_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#وإلا
    argv[1] = scheme_current_continuation_marks(عدم);
    MZ_GC_CHECK();
#جيڪڏهن_پورو

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    فراغ
raise_if_error(فراغ)
{
    إذا (vim_error_check())
	raise_vim_exn(عدم);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساكن vim_mz_بفر *
get_بفر_arg(ثابت حرف *fname, صحيح argnum, صحيح argc, Scheme_Object **argv)
{
    vim_mz_بفر *b;

    إذا (argc < argnum + 1)
	أرجع get_vim_curr_بفر();
    إذا (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_بفر *)argv[argnum];
    (فراغ)get_valid_بفر(argv[argnum]);
    أرجع b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساكن vim_mz_دري *
get_دري_arg(ثابت حرف *fname, صحيح argnum, صحيح argc, Scheme_Object **argv)
{
    vim_mz_دري *w;

    إذا (argc < argnum + 1)
	أرجع get_vim_curr_دري();
    w = (vim_mz_دري *)argv[argnum];
    إذا (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (فراغ)get_valid_دري(argv[argnum]);
    أرجع w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_بفر(فراغ *obj)
{
    buf_T *buf = ((vim_mz_بفر *)obj)->buf;

    إذا (buf == INVALID_بفر_قدر)
	scheme_signal_error(_("buffer is invalid"));
    أرجع buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_دري(فراغ *obj)
{
    win_T *win = ((vim_mz_دري *)obj)->win;
    إذا (win == INVALID_دري_قدر)
	scheme_signal_error(_("window is invalid"));
    أرجع win;
}

    صحيح
mzthreads_allowed(فراغ)
{
    أرجع mz_ڌاڳا_allow;
}

    ساكن صحيح
سٽ_اندريون_range(linenr_T lnum, buf_T *buf)
{
    أرجع (lnum > 0 && lnum <= buf->b_ml.ml_سٽ_ڳڻپ);
}

    ساكن فراغ
check_سٽ_range(linenr_T lnum, buf_T *buf)
{
    إذا (!سٽ_اندريون_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ساكن فراغ
mz_fix_cursor(صحيح lo, صحيح hi, صحيح extra)
{
    إذا (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	إذا (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_کانو();
	}
	وإلا إذا (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وإلا
	    check_cursor_کانو();
	بدليو_cline_bef_curs();
    }
    invalidate_botline();
}

ساكن Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_بفر_سٽ, "get-buff-line", 1, 2},
    {سيٽ_بفر_سٽ, "set-buff-line", 2, 3},
    {get_بفر_سٽ_list, "get-buff-line-list", 2, 3},
    {get_بفر_name, "get-buff-name", 0, 1},
    {get_بفر_انگ, "get-buff-num", 0, 1},
    {get_بفر_سائيز, "get-buff-size", 0, 1},
    {سيٽ_بفر_سٽ_list, "set-buff-line-list", 3, 4},
    {وجهو_بفر_سٽ_list, "insert-buff-line-list", 2, 3},
    {get_curr_بفر, "curr-buff", 0, 0},
    {get_بفر_ڳڻپ, "buff-count", 0, 0},
    {get_ايندڙ_بفر, "get-next-buff", 0, 1},
    {get_prev_بفر, "get-prev-buff", 0, 1},
    {mzscheme_open_بفر, "open-buff", 1, 1},
    {get_بفر_by_name, "get-buff-by-name", 1, 1},
    {get_بفر_by_انگ, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_دري_ڳڻپ, "win-count", 0, 0},
    {get_دري_by_انگ, "get-win-by-num", 1, 1},
    {get_دري_انگ, "get-win-num", 0, 1},
    {get_دري_بفر, "get-win-buffer", 0, 1},
    {get_دري_اوچائي, "get-win-height", 0, 1},
    {سيٽ_دري_اوچائي, "set-win-height", 1, 2},
    {get_دري_ويڪر, "get-win-width", 0, 1},
    {سيٽ_دري_ويڪر, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {سيٽ_cursor, "set-cursor", 1, 2},
    {get_دري_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_شروعات, "range-start", 0, 0},
    {get_range_پڇاڙي, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {سيٽ_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_بفر_validp, "buff-valid?", 1, 1},
    {vim_دري_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    ساكن vim_mz_بفر *
get_vim_curr_بفر(فراغ)
{
    إذا (curbuf->b_mzscheme_ref == عدم)
	أرجع (vim_mz_بفر *)بفر_new(curbuf);
    وإلا
	أرجع بفر_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ساكن vim_mz_دري *
get_vim_curr_دري(فراغ)
{
    إذا (curwin->w_mzscheme_ref == عدم)
	أرجع (vim_mz_دري *)دري_new(curwin);
    وإلا
	أرجع دري_REF(curwin);
}

    ساكن فراغ
make_modules(فراغ)
{
    صحيح		    i;
    Scheme_Env	    *mod = عدم;
    Scheme_Object   *vimext_نشان = عدم;
    Scheme_Object   *closed_prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندريون_REG(0, mod);
    MZ_GC_VAR_اندريون_REG(1, vimext_نشان);
    MZ_GC_VAR_اندريون_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_نشان = scheme_intern_نشان("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_نشان, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    لكل (i = 0; i < (صحيح)ARRAY_ڊيگهائي(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_جوڙيو_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_پڄاڻي_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
ساكن Scheme_Object *M_write = عدم;
ساكن Scheme_Object *M_read = عدم;
ساكن Scheme_Object *M_execute = عدم;
ساكن Scheme_Object *M_delete = عدم;

    ساكن فراغ
sandbox_check(فراغ)
{
    إذا (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ساكن Scheme_Object *
sandbox_file_guard(صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv)
{
    إذا (!sandbox)
	أرجع scheme_void;

    Scheme_Object *requested_access = argv[2];

    إذا (M_write == عدم)
    {
	MZ_رجسٽر_STATIC(M_write);
	M_write = scheme_intern_نشان("write");
	MZ_GC_CHECK();
    }
    إذا (M_read == عدم)
    {
	MZ_رجسٽر_STATIC(M_read);
	M_read = scheme_intern_نشان("read");
	MZ_GC_CHECK();
    }
    إذا (M_execute == عدم)
    {
	MZ_رجسٽر_STATIC(M_execute);
	M_execute = scheme_intern_نشان("execute");
	MZ_GC_CHECK();
    }
    إذا (M_delete == عدم)
    {
	MZ_رجسٽر_STATIC(M_delete);
	M_delete = scheme_intern_نشان("delete");
	MZ_GC_CHECK();
    }

    طالما (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *شي = SCHEME_CAR(requested_access);
	إذا (scheme_eq(شي, M_write) || scheme_eq(شي, M_read)
		|| scheme_eq(شي, M_execute) || scheme_eq(شي, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    أرجع scheme_void;
}

    ساكن Scheme_Object *
sandbox_ڄار_guard(صحيح argc اڻ_استعمال_ٿيل, Scheme_Object **argv اڻ_استعمال_ٿيل)
{
    أرجع scheme_void;
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو
