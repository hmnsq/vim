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
#إذا MZSCHEME_نسخہ_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_نسخہ_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#جے_انت

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#إذا defined(MZ_PRECISE_GC) && MZSCHEME_نسخہ_MAJOR >= 400 \
    || MZSCHEME_نسخہ_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#جے_انت

// Base data structures
#تعریف_کرو SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_بفر_type)
#تعریف_کرو SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_کھڑکی_type)

تعريفنوع بنية
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_بفر;

#تعریف_کرو INVALID_بفر_قدر ((buf_T *)(-1))

تعريفنوع بنية
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_کھڑکی;

#تعریف_کرو INVALID_کھڑکی_قدر ((win_T *)(-1))

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
    Scheme_Object   *پورٹ;
} پورٹ_Info;

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
#جے_تعریف_شدہ HAVE_SANDBOX
ساكن Scheme_Object *sandbox_file_guard(صحيح, Scheme_Object **);
ساكن Scheme_Object *sandbox_جال_guard(صحيح, Scheme_Object **);
ساكن فراغ sandbox_check(فراغ);
#جے_انت
//  Buffer-related commands
ساكن Scheme_Object *بفر_new(buf_T *buf);
ساكن Scheme_Object *get_بفر_by_عدد(فراغ *, صحيح, Scheme_Object **);
ساكن vim_mz_بفر *get_vim_curr_بفر(فراغ);

//  Window-related commands
ساكن Scheme_Object *کھڑکی_new(win_T *win);
ساكن vim_mz_کھڑکی *get_vim_curr_کھڑکی(فراغ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ساكن صحيح vim_error_check(فراغ);
ساكن صحيح do_mzscheme_command(exarg_T *, فراغ *, Scheme_Closed_Prim *what);
ساكن صحيح startup_mzscheme(فراغ);
ساكن حرف *string_منزل_سطر(Scheme_Object *obj);
#إذا MZSCHEME_نسخہ_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#وإلا
# define OUTPUT_LEN_TYPE لما
#جے_انت
ساكن فراغ do_output(حرف *mesg, OUTPUT_LEN_TYPE len);
ساكن فراغ do_printf(حرف *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساكن فراغ do_flush(فراغ);
ساكن Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ساكن Scheme_Object *کڈھو_exn_سنیہا(Scheme_Object *v);
ساكن Scheme_Object *do_eval(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن Scheme_Object *do_لدو(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن فراغ رجسٹر_vim_exn(فراغ);
ساكن vim_mz_بفر *get_بفر_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن vim_mz_کھڑکی *get_کھڑکی_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن صحيح سطر_وچ_range(linenr_T, buf_T *);
ساكن فراغ check_سطر_range(linenr_T, buf_T *);
ساكن فراغ mz_fix_cursor(صحيح lo, صحيح hi, صحيح extra);

ساكن صحيح eval_with_exn_handling(فراغ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ساكن فراغ make_modules(فراغ);
ساكن فراغ init_exn_catching_apply(فراغ);
ساكن صحيح mzscheme_env_main(Scheme_Env *env, صحيح argc, حرف **argv);
ساكن صحيح mzscheme_init(فراغ);
#جے_تعریف_شدہ FEAT_EVAL
ساكن Scheme_Object *vim_منزل_mzscheme(typval_T *vim_قدر);
ساكن Scheme_Object *vim_منزل_mzscheme_impl(typval_T *vim_قدر, صحيح ڈونگھائی,
	Scheme_Hash_جدول *ویکھیا);
ساكن صحيح mzscheme_منزل_vim(Scheme_Object *obj, typval_T *tv);
ساكن صحيح mzscheme_منزل_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح ڈونگھائی,
	Scheme_Hash_جدول *ویکھیا);
ساكن Scheme_Object *vim_funcref(فراغ *انکڑے, صحيح argc, Scheme_Object **argv);
#جے_انت

#جے_تعریف_شدہ MZ_PRECISE_GC
ساكن صحيح بفر_ناپ_proc(فراغ *obj بن_ورتیا)
{
    أرجع gc_بائٹاں_منزل_الفاظ(حجم(vim_mz_بفر));
}
ساكن صحيح بفر_mark_proc(فراغ *obj)
{
    أرجع بفر_ناپ_proc(obj);
}
ساكن صحيح بفر_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    أرجع بفر_ناپ_proc(obj);
}
ساكن صحيح کھڑکی_ناپ_proc(فراغ *obj بن_ورتیا)
{
    أرجع gc_بائٹاں_منزل_الفاظ(حجم(vim_mz_کھڑکی));
}
ساكن صحيح کھڑکی_mark_proc(فراغ *obj)
{
    أرجع کھڑکی_ناپ_proc(obj);
}
ساكن صحيح کھڑکی_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    أرجع کھڑکی_ناپ_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define کھڑکی_REF(win) *(vim_mz_کھڑکی **)((win)->w_mzscheme_ref)
# define بفر_REF(buf) *(vim_mz_بفر **)((buf)->b_mzscheme_ref)
#وإلا
# define کھڑکی_REF(win) (vim_mz_کھڑکی *)((win)->w_mzscheme_ref)
# define بفر_REF(buf) (vim_mz_بفر *)((buf)->b_mzscheme_ref)
#جے_انت

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

ساكن فراغ *(*dll_GC_malloc)(size_t ناپ_وچ_بائٹاں);
ساكن فراغ *(*dll_GC_malloc_atomic)(size_t ناپ_وچ_بائٹاں);
ساكن Scheme_Env *(*dll_scheme_basic_env)(فراغ);
ساكن فراغ (*dll_scheme_check_دھاگے)(فراغ);
ساكن فراغ (*dll_scheme_رجسٹر_static)(فراغ *ptr, لما ناپ);
ساكن فراغ (*dll_scheme_سیٹ_ڈھیر_ادھار)(فراغ *ادھار, صحيح no_auto_statics);
ساكن فراغ (*dll_scheme_جوڑو_global)(ثابت حرف *name, Scheme_Object *val,
	Scheme_Env *env);
ساكن فراغ (*dll_scheme_جوڑو_global_علامت)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, صحيح عدد_rands,
	Scheme_Object **rands);
ساكن Scheme_Object *(*dll_scheme_builtin_قدر)(ثابت حرف *name);
# إذا MZSCHEME_نسخہ_MAJOR >= 299
ساكن Scheme_Object *(*dll_scheme_byte_string_منزل_char_string)(Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_make_رستہ)(ثابت حرف *chars);
# endif
ساكن فراغ (*dll_scheme_close_داخلہ_پورٹ)(Scheme_Object *پورٹ);
ساكن فراغ (*dll_scheme_گنتی_سطریں)(Scheme_Object *پورٹ);
# إذا MZSCHEME_نسخہ_MAJOR < 360
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(فراغ);
# وإلا
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_ٹیگ);
# endif
ساكن فراغ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *پورٹ);
ساكن حرف *(*dll_scheme_display_منزل_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ساكن صحيح (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ساكن Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	صحيح عدد_rands_2, Scheme_Object **rands, صحيح val);
ساكن فراغ (*dll_scheme_dont_gc_ptr)(فراغ *p);
ساكن Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string)(ثابت حرف *str,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string_all)(ثابت حرف *str,
	Scheme_Env *env, صحيح all);
ساكن فراغ (*dll_scheme_انت_primitive_module)(Scheme_Env *env);
# إذا MZSCHEME_نسخہ_MAJOR < 299
ساكن حرف *(*dll_scheme_format)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, لما *rlen);
# وإلا
ساكن حرف *(*dll_scheme_format_utf8)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ساكن Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, صحيح pos);
# endif
ساكن فراغ (*dll_scheme_gc_ptr_ok)(فراغ *p);
# إذا MZSCHEME_نسخہ_MAJOR < 299
ساكن حرف *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	لما *len);
# وإلا
ساكن حرف *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ساكن Scheme_Object *(*dll_scheme_intern_علامت)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *علامت,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, فراغ *انکڑے, ثابت حرف *name, mzshort mina,
     mzshort maxa);
ساكن Scheme_Object *(*dll_scheme_make_integer_قدر)(لما i);
ساكن Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ساكن Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ثابت حرف *name, mzshort mina, mzshort maxa);
# إذا MZSCHEME_نسخہ_MAJOR < 299
ساكن Scheme_Object *(*dll_scheme_make_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_string_output_پورٹ)();
# وإلا
ساكن Scheme_Object *(*dll_scheme_make_byte_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_byte_string_output_پورٹ)();
# endif
ساكن Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	صحيح argc, Scheme_Object **argv);
ساكن Scheme_Object **(*dll_scheme_make_struct_ناں)(Scheme_Object *ادھار,
	Scheme_Object *میدان_ناں, صحيح flags, صحيح *گنتی_باہرلا);
ساكن Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *ادھار,
	Scheme_Object *ماپا, Scheme_Object *inspector, صحيح عدد_fields,
	صحيح عدد_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# إذا MZSCHEME_نسخہ_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ساكن Scheme_Object **(*dll_scheme_make_struct_اقدار)(
	Scheme_Object *struct_type, Scheme_Object **ناں, صحيح گنتی,
	صحيح flags);
ساكن Scheme_Type (*dll_scheme_make_type)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_make_سمتیہ)(صحيح ناپ,
	Scheme_Object *fill);
ساكن فراغ *(*dll_scheme_malloc_fail_ok)(فراغ *(*f)(size_t), size_t);
ساكن Scheme_Object *(*dll_scheme_open_داخلہ_file)(ثابت حرف *name,
	ثابت حرف *who);
ساكن Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ساكن صحيح (*dll_scheme_proper_list_لمائی)(Scheme_Object *list);
ساكن فراغ (*dll_scheme_raise)(Scheme_Object *exn);
ساكن Scheme_Object *(*dll_scheme_read)(Scheme_Object *پورٹ);
ساكن فراغ (*dll_scheme_signal_error)(ثابت حرف *پیغام, ...);
ساكن فراغ (*dll_scheme_wrong_type)(ثابت حرف *name, ثابت حرف *expected,
	صحيح which, صحيح argc, Scheme_Object **argv);
# إذا MZSCHEME_نسخہ_MAJOR >= 299
ساكن فراغ (*dll_scheme_سیٹ_param)(Scheme_Config *c, صحيح pos,
	Scheme_Object *o);
ساكن Scheme_Config *(*dll_scheme_current_config)(فراغ);
ساكن Scheme_Object *(*dll_scheme_char_string_منزل_byte_string)
    (Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_char_string_منزل_رستہ)
    (Scheme_Object *s);
ساكن فراغ *(*dll_scheme_سیٹ_collects_رستہ)(Scheme_Object *p);
# endif
ساكن Scheme_Hash_جدول *(*dll_scheme_make_hash_جدول)(صحيح type);
ساكن فراغ (*dll_scheme_hash_سیٹ)(Scheme_Hash_جدول *جدول,
	Scheme_Object *کلید, Scheme_Object *قدر);
ساكن Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_جدول *جدول,
	Scheme_Object *کلید);
ساكن Scheme_Object *(*dll_scheme_make_double)(مزدوج d);
ساكن Scheme_Object *(*dll_scheme_make_sized_byte_string)(حرف *chars,
	لما len, صحيح copy);
ساكن Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ساكن Scheme_Object *(*dll_scheme_dynamic_wind)(فراغ (*pre)(فراغ *), Scheme_Object *(* متطاير act)(فراغ *), فراغ (* متطاير post)(فراغ *), Scheme_Object *(*jmp_handler)(فراغ *), فراغ * متطاير انکڑے);
# ifdef MZ_PRECISE_GC
ساكن فراغ *(*dll_GC_malloc_one_tagged)(size_t ناپ_وچ_بائٹاں);
ساكن فراغ (*dll_GC_رجسٹر_traversers)(چھوٹا ٹیگ, ناپ_Proc ناپ, Mark_Proc mark, Fixup_Proc fixup, صحيح is_constant_ناپ, صحيح is_atomic);
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 400
ساكن فراغ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ساكن فراغ **(*dll_scheme_malloc_immobile_box)(فراغ *p);
ساكن فراغ (*dll_scheme_free_immobile_box)(فراغ **b);
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ساكن صحيح (*dll_scheme_main_تیاری)(صحيح no_auto_statics, Scheme_Env_Main _main, صحيح argc, حرف **argv);
#   إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
ساكن فراغ (*dll_scheme_رجسٹر_tls_space)(فراغ *tls_space, صحيح tls_اشاریہ);
#   endif
#  endif
#  إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_راہیں_PROC)
ساكن Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(فراغ);
#  endif
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 600
ساكن فراغ (*dll_scheme_embedded_لدو)(intptr_t len, ثابت حرف *s, صحيح predefined);
ساكن فراغ (*dll_scheme_رجسٹر_embedded_لدو)(intptr_t len, ثابت حرف *s);
ساكن فراغ (*dll_scheme_سیٹ_config_رستہ)(Scheme_Object *p);
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

#  define scheme_جوڑو_global dll_scheme_جوڑو_global
#  define scheme_جوڑو_global_علامت dll_scheme_جوڑو_global_علامت
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_قدر dll_scheme_builtin_قدر
#  إذا MZSCHEME_نسخہ_MAJOR >= 299
#   define scheme_byte_string_منزل_char_string dll_scheme_byte_string_منزل_char_string
#   define scheme_make_رستہ dll_scheme_make_رستہ
#  endif
#  define scheme_check_دھاگے dll_scheme_check_دھاگے
#  define scheme_close_داخلہ_پورٹ dll_scheme_close_داخلہ_پورٹ
#  define scheme_گنتی_سطریں dll_scheme_گنتی_سطریں
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
#  define scheme_انت_primitive_module dll_scheme_انت_primitive_module
#  إذا MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_format dll_scheme_format
#  وإلا
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  إذا MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  وإلا
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_علامت dll_scheme_intern_علامت
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_قدر dll_scheme_make_integer_قدر
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  إذا MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_پورٹ dll_scheme_make_string_output_پورٹ
#  وإلا
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_پورٹ \
    dll_scheme_make_byte_string_output_پورٹ
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_ناں dll_scheme_make_struct_ناں
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_اقدار dll_scheme_make_struct_اقدار
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_سمتیہ dll_scheme_make_سمتیہ
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_داخلہ_file dll_scheme_open_داخلہ_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_لمائی dll_scheme_proper_list_لمائی
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_رجسٹر_static dll_scheme_رجسٹر_static
#  define scheme_سیٹ_ڈھیر_ادھار dll_scheme_سیٹ_ڈھیر_ادھار
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  إذا MZSCHEME_نسخہ_MAJOR >= 299
#   define scheme_سیٹ_param dll_scheme_سیٹ_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_منزل_byte_string \
    dll_scheme_char_string_منزل_byte_string
#   define scheme_char_string_منزل_رستہ \
    dll_scheme_char_string_منزل_رستہ
#   define scheme_سیٹ_collects_رستہ dll_scheme_سیٹ_collects_رستہ
#  endif
#  define scheme_make_hash_جدول dll_scheme_make_hash_جدول
#  define scheme_hash_سیٹ dll_scheme_hash_سیٹ
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_رجسٹر_traversers dll_GC_رجسٹر_traversers
#  endif
#  إذا MZSCHEME_نسخہ_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_تیاری dll_scheme_main_تیاری
#    إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
#     define scheme_رجسٹر_tls_space dll_scheme_رجسٹر_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  إذا MZSCHEME_نسخہ_MAJOR >= 600
#   define scheme_embedded_لدو dll_scheme_embedded_لدو
#   define scheme_رجسٹر_embedded_لدو dll_scheme_رجسٹر_embedded_لدو
#   define scheme_سیٹ_config_رستہ dll_scheme_سیٹ_config_رستہ
#  endif

#  ifndef PROTO
#   إذا MZSCHEME_نسخہ_MAJOR >= 500
#    إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_راہیں_PROC)
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
    {"scheme_add_global", (فراغ **)&dll_scheme_جوڑو_global},
    {"scheme_add_global_symbol", (فراغ **)&dll_scheme_جوڑو_global_علامت},
    {"scheme_apply", (فراغ **)&dll_scheme_apply},
    {"scheme_basic_env", (فراغ **)&dll_scheme_basic_env},
# إذا MZSCHEME_نسخہ_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (فراغ **)&dll_scheme_byte_string_منزل_char_string},
    {"scheme_make_path", (فراغ **)&dll_scheme_make_رستہ},
# endif
    {"scheme_builtin_value", (فراغ **)&dll_scheme_builtin_قدر},
    {"scheme_check_threads", (فراغ **)&dll_scheme_check_دھاگے},
    {"scheme_close_input_port", (فراغ **)&dll_scheme_close_داخلہ_پورٹ},
    {"scheme_count_lines", (فراغ **)&dll_scheme_گنتی_سطریں},
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
	(فراغ **)&dll_scheme_انت_primitive_module},
# إذا MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_format", (فراغ **)&dll_scheme_format},
# وإلا
    {"scheme_format_utf8", (فراغ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (فراغ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (فراغ **)&dll_scheme_gc_ptr_ok},
# إذا MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_get_sized_string_output",
	(فراغ **)&dll_scheme_get_sized_string_output},
# وإلا
    {"scheme_get_sized_byte_string_output",
	(فراغ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (فراغ **)&dll_scheme_intern_علامت},
    {"scheme_lookup_global", (فراغ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(فراغ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (فراغ **)&dll_scheme_make_integer_قدر},
    {"scheme_make_pair", (فراغ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (فراغ **)&dll_scheme_make_prim_w_arity},
# إذا MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_make_string", (فراغ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(فراغ **)&dll_scheme_make_string_output_پورٹ},
# وإلا
    {"scheme_make_byte_string", (فراغ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(فراغ **)&dll_scheme_make_byte_string_output_پورٹ},
# endif
    {"scheme_make_struct_instance",
	(فراغ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (فراغ **)&dll_scheme_make_struct_ناں},
    {"scheme_make_struct_type", (فراغ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (فراغ **)&dll_scheme_make_struct_اقدار},
    {"scheme_make_type", (فراغ **)&dll_scheme_make_type},
    {"scheme_make_vector", (فراغ **)&dll_scheme_make_سمتیہ},
    {"scheme_malloc_fail_ok", (فراغ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (فراغ **)&dll_scheme_open_داخلہ_file},
    {"scheme_primitive_module", (فراغ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (فراغ **)&dll_scheme_proper_list_لمائی},
    {"scheme_raise", (فراغ **)&dll_scheme_raise},
    {"scheme_read", (فراغ **)&dll_scheme_read},
    {"scheme_register_static", (فراغ **)&dll_scheme_رجسٹر_static},
    {"scheme_set_stack_base", (فراغ **)&dll_scheme_سیٹ_ڈھیر_ادھار},
    {"scheme_signal_error", (فراغ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (فراغ **)&dll_scheme_wrong_type},
# إذا MZSCHEME_نسخہ_MAJOR >= 299
    {"scheme_set_param", (فراغ **)&dll_scheme_سیٹ_param},
    {"scheme_current_config", (فراغ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(فراغ **)&dll_scheme_char_string_منزل_byte_string},
    {"scheme_char_string_to_path", (فراغ **)&dll_scheme_char_string_منزل_رستہ},
    {"scheme_set_collects_path", (فراغ **)&dll_scheme_سیٹ_collects_رستہ},
# endif
    {"scheme_make_hash_table", (فراغ **)&dll_scheme_make_hash_جدول},
    {"scheme_hash_set", (فراغ **)&dll_scheme_hash_سیٹ},
    {"scheme_hash_get", (فراغ **)&dll_scheme_hash_get},
    {"scheme_make_double", (فراغ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (فراغ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (فراغ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (فراغ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (فراغ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (فراغ **)&dll_GC_رجسٹر_traversers},
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (فراغ **)&dll_scheme_main_تیاری},
#   إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
    {"scheme_register_tls_space", (فراغ **)&dll_scheme_رجسٹر_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (فراغ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (فراغ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (فراغ **)&dll_scheme_free_immobile_box},
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 500
#  إذا defined(IMPLEMENT_THREAD_LOCAL_راہیں_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_راہیں_PROC)
    {"scheme_external_get_thread_local_variables", (فراغ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# إذا MZSCHEME_نسخہ_MAJOR >= 600
    {"scheme_embedded_load", (فراغ **)&dll_scheme_embedded_لدو},
    {"scheme_register_embedded_load", (فراغ **)&dll_scheme_رجسٹر_embedded_لدو},
    {"scheme_set_config_path", (فراغ **)&dll_scheme_سیٹ_config_رستہ},
# endif
    {عدم, عدم}};

ساكن HINSTANCE hMzGC = 0;
ساكن HINSTANCE hMzSch = 0;

ساكن فراغ dynamic_mzscheme_سرا(فراغ);
ساكن صحيح mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll,
	صحيح verbose);

    ساكن صحيح
mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll, صحيح verbose)
{
    Thunk_Info *thunk = عدم;

    إذا (hMzGC && hMzSch)
	أرجع OK;
    hMzSch = vim_لدو_Lib(sch_dll);
    hMzGC = vim_لدو_Lib(gc_dll);

    إذا (!hMzGC)
    {
	إذا (verbose)
	    semsg(_(e_could_not_لدو_library_str_str), gc_dll, GetWin32Error());
	أرجع FAIL;
    }

    إذا (!hMzSch)
    {
	إذا (verbose)
	    semsg(_(e_could_not_لدو_library_str_str), sch_dll, GetWin32Error());
	أرجع FAIL;
    }

    لكل (thunk = mzsch_imports; thunk->name; thunk++)
    {
	إذا ((*thunk->ptr =
		    (فراغ *)Get_Proc_پتہ(hMzSch, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    إذا (verbose)
		semsg(_(e_could_not_لدو_library_function_str), thunk->name);
	    أرجع FAIL;
	}
    }
    لكل (thunk = mzgc_imports; thunk->name; thunk++)
    {
	إذا ((*thunk->ptr =
		    (فراغ *)Get_Proc_پتہ(hMzGC, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    إذا (verbose)
		semsg(_(e_could_not_لدو_library_function_str), thunk->name);
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
dynamic_mzscheme_سرا(فراغ)
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
#جے_انت // DYNAMIC_MZSCHEME

#إذا MZSCHEME_نسخہ_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, عدد) GUARANTEE_STRING(proc, عدد)
#وإلا
    ساكن Scheme_Object *
guaranteed_byte_string_arg(حرف *proc, صحيح عدد, صحيح argc, Scheme_Object **argv)
{
    إذا (SCHEME_BYTE_STRINGP(argv[عدد]))
    {
	أرجع argv[عدد];
    }
    وإلا إذا (SCHEME_CHAR_STRINGP(argv[عدد]))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_وچ_REG(0, argv[عدد]);
	MZ_GC_VAR_وچ_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_منزل_byte_string(argv[عدد]);
	MZ_GC_UNREG();
	أرجع tmp;
    }
    وإلا
	scheme_wrong_type(proc, "string", عدد, argc, argv);
    // unreachable
    أرجع scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, عدد) guaranteed_byte_string_arg(proc, عدد, argc, argv)
#جے_انت

// need to put it here for dynamic stuff to work
#إذا defined(INCLUDE_MZSCHEME_ادھار)
# include "mzscheme_base.c"
#جے_انت

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ساكن Scheme_Type mz_بفر_type;
ساكن Scheme_Type mz_کھڑکی_type;

ساكن صحيح initialized = FALSE;
#جے_تعریف_شدہ DYNAMIC_MZSCHEME
ساكن صحيح disabled = FALSE;
#جے_انت
ساكن صحيح لدو_ادھار_module_failed = FALSE;

// global environment
ساكن Scheme_Env    *environment = عدم;
// output/error handlers
ساكن Scheme_Object *curout = عدم;
ساكن Scheme_Object *curerr = عدم;
// exn:vim exception
ساكن Scheme_Object *exn_catching_apply = عدم;
ساكن Scheme_Object *exn_p = عدم;
ساكن Scheme_Object *exn_سنیہا = عدم;
ساكن Scheme_Object *vim_exn = عدم; // Vim Error exception

#إذا !defined(MZ_PRECISE_GC) || MZSCHEME_نسخہ_MAJOR < 400
ساكن فراغ *ڈھیر_ادھار = عدم;
#جے_انت

ساكن لما range_شروع;
ساكن لما range_سرا;

// MzScheme threads scheduling stuff
ساكن صحيح mz_دھاگے_allow = 0;

#إذا defined(FEAT_GUI_MSWIN)
ساكن فراغ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ساكن UINT timer_id = 0;
#وإلاإذا defined(FEAT_GUI_GTK)
ساكن gboolean timer_proc(gpointer);
ساكن guint timer_id = 0;
#وإلاإذا defined(FEAT_GUI_MOTIF)
ساكن فراغ timer_proc(Xt_اشارہ_گر, XtIntervalId *);
ساكن XtIntervalId timer_id = (XtIntervalId)0;
#جے_انت

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    فراغ
mzvim_check_دھاگے(فراغ)
{
    // Last time MzScheme threads were scheduled
    ساكن time_t mz_آخری_ویلا = 0;

    إذا (mz_دھاگے_allow && p_mzq > 0)
    {
	time_t now = ویلا(عدم);

	إذا ((now - mz_آخری_ویلا) * 1000 > p_mzq)
	{
	    mz_آخری_ویلا = now;
	    scheme_check_دھاگے();
	}
    }
}
#جے_انت

#إذا defined(MZSCHEME_GUI_دھاگے)
ساكن فراغ تیاری_timer(فراغ);
ساكن فراغ remove_timer(فراغ);

// timers are presented in GUI only
# إذا defined(FEAT_GUI_MSWIN)
    ساكن فراغ CALLBACK
timer_proc(
    HWND	hwnd بن_ورتیا,
    UINT	u_پیغام بن_ورتیا,
    UINT_PTR	idEvent بن_ورتیا,
    DWORD	dw_ویلا بن_ورتیا)
# elif defined(FEAT_GUI_GTK)
    ساكن gboolean
timer_proc(gpointer انکڑے بن_ورتیا)
# elif defined(FEAT_GUI_MOTIF)
    ساكن فراغ
timer_proc(Xt_اشارہ_گر timed_باہرلا بن_ورتیا, XtIntervalId *interval_id بن_ورتیا)
# endif
{
    scheme_check_دھاگے();
# إذا defined(FEAT_GUI_GTK)
    أرجع TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    إذا (mz_دھاگے_allow && p_mzq > 0)
	timer_id = Xt_App_جوڑو_ویلا_باہرلا(app_context, p_mzq,
		timer_proc, عدم);
# endif
}

    ساكن فراغ
تیاری_timer(فراغ)
{
# إذا defined(FEAT_GUI_MSWIN)
    timer_id = سیٹ_Timer(عدم, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_جوڑو((guint)p_mzq, (GSourceFunc)timer_proc, عدم);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_جوڑو_ویلا_باہرلا(app_context, p_mzq, timer_proc, عدم);
# endif
}

    ساكن فراغ
remove_timer(فراغ)
{
# إذا defined(FEAT_GUI_MSWIN)
    KillTimer(عدم, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_سروت_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_ویلا_باہرلا(timer_id);
# endif
    timer_id = 0;
}

#جے_انت // MZSCHEME_GUI_THREADS

    حرف *
did_سیٹ_mzquantum(optset_T *دلیلاں بن_ورتیا)
{
#إذا defined(MZSCHEME_GUI_دھاگے)
    // reset timer
    إذا (timer_id != 0)
	remove_timer();
    إذا (mz_دھاگے_allow && p_mzq > 0 && gui.وچ_use)
	تیاری_timer();
#جے_انت
    أرجع عدم;
}

    ساكن فراغ
notify_multithread(صحيح on)
{
    mz_دھاگے_allow = on;
#جے_تعریف_شدہ MZSCHEME_GUI_دھاگے
    إذا (on && timer_id == 0 && p_mzq > 0 && gui.وچ_use)
	تیاری_timer();
    إذا (!on && timer_id != 0)
	remove_timer();
#جے_انت
}

    فراغ
mzscheme_سرا(فراغ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#إذا 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_سرا();
# endif
#جے_انت
}

#إذا HAVE_TLS_SPACE && !defined(VIMDLL)
# إذا defined(_MSC_VER)
ساكن __declspec(thread) فراغ *tls_space;
خارجي intptr_t tls_اشاریہ;
# elif defined(__MINGW32__)
ساكن __thread فراغ *tls_space;
خارجي intptr_t tls_اشاریہ;
# وإلا
ساكن THREAD_LOCAL فراغ *tls_space;
ساكن intptr_t tls_اشاریہ = 0;
# endif
#جے_انت

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

#جے_تعریف_شدہ DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    إذا (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
#جے_انت
#جے_تعریف_شدہ HAVE_TLS_SPACE
# ifdef VIMDLL
    فراغ **ptls_space;
    intptr_t tls_اشاریہ_2;
    فراغ (*pget_tls_info)(فراغ ***ptls_space, intptr_t *ptls_اشاریہ);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (فراغ *)Get_Proc_پتہ(
				GetModuleHandle(عدم), "get_tls_info");
    إذا (pget_tls_info == عدم)
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_اشاریہ_2);
    scheme_رجسٹر_tls_space(ptls_space, tls_اشاریہ_2);
# وإلا
    scheme_رجسٹر_tls_space(&tls_space, tls_اشاریہ);
# endif
#جے_انت
#جے_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    أرجع scheme_main_تیاری(TRUE, mzscheme_env_main, argc, &argv);
#وإلا
    أرجع mzscheme_env_main(عدم, argc, &argv);
#جے_انت
}

    ساكن صحيح
mzscheme_env_main(Scheme_Env *env, صحيح argc بن_ورتیا, حرف **argv بن_ورتیا)
{
#جے_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#وإلا
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, dummy);

    ڈھیر_ادھار = &__gc_var_stack__;
# وإلا
    صحيح dummy = 0;
    ڈھیر_ادھار = (فراغ *)&dummy;
# endif
#جے_انت

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    أرجع 0;
}

    ساكن Scheme_Object*
لدو_ادھار_module(فراغ *انکڑے)
{
    scheme_namespace_require(scheme_intern_علامت((حرف *)انکڑے));
    أرجع scheme_null;
}

    ساكن Scheme_Object *
لدو_ادھار_module_on_error(فراغ *انکڑے بن_ورتیا)
{
    لدو_ادھار_module_failed = TRUE;
    أرجع scheme_null;
}

    ساكن صحيح
startup_mzscheme(فراغ)
{
#جے_بنا_تعریف TRAMPOLINED_MZVIM_STARTUP
    scheme_سیٹ_ڈھیر_ادھار(ڈھیر_ادھار, 1);
#جے_انت

#جے_بنا_تعریف TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#جے_انت

    MZ_رجسٹر_STATIC(environment);
    MZ_رجسٹر_STATIC(curout);
    MZ_رجسٹر_STATIC(curerr);
    MZ_رجسٹر_STATIC(exn_catching_apply);
    MZ_رجسٹر_STATIC(exn_p);
    MZ_رجسٹر_STATIC(exn_سنیہا);
    MZ_رجسٹر_STATIC(vim_exn);

    MZ_GC_CHECK();

#جے_تعریف_شدہ INCLUDE_MZSCHEME_ادھار
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#جے_انت

    // setup 'current-library-collection-paths' parameter
#إذا MZSCHEME_نسخہ_MAJOR >= 299
    {
	Scheme_Object *coll_رستہ = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_وچ_REG(0, coll_رستہ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	إذا (s != عدم)
	{
	    coll_رستہ = scheme_make_رستہ((حرف *)s);
	    MZ_GC_CHECK();
	    إذا (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	إذا (coll_رستہ == عدم)
	{
	    coll_رستہ = scheme_make_رستہ(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	إذا (coll_رستہ != عدم)
	{
	    scheme_سیٹ_collects_رستہ(coll_رستہ);
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
	MZ_GC_VAR_وچ_REG(0, coll_string);
	MZ_GC_VAR_وچ_REG(1, coll_pair);
	MZ_GC_VAR_وچ_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_سیٹ_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#جے_انت

#إذا MZSCHEME_نسخہ_MAJOR >= 600
    {
	Scheme_Object *config_رستہ = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_وچ_REG(0, config_رستہ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	إذا (s != عدم)
	{
	    config_رستہ = scheme_make_رستہ((حرف *)s);
	    MZ_GC_CHECK();
	    إذا (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	إذا (config_رستہ == عدم)
	{
	    config_رستہ = scheme_make_رستہ(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	إذا (config_رستہ != عدم)
	{
	    scheme_سیٹ_config_رستہ(config_رستہ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#جے_انت

#إذا MZSCHEME_نسخہ_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#جے_انت

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(عدم, لدو_ادھار_module, عدم,
				    لدو_ادھار_module_on_error, "racket/base");
	إذا (لدو_ادھار_module_failed)
	{
	    لدو_ادھار_module_failed = FALSE;
	    scheme_dynamic_wind(عدم, لدو_ادھار_module, عدم,
				    لدو_ادھار_module_on_error, "scheme/base");
	    إذا (لدو_ادھار_module_failed)
		أرجع -1;
	}
    }

    رجسٹر_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#جے_تعریف_شدہ HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_وچ_REG(0, make_security_guard);
	MZ_GC_REG();

# إذا MZSCHEME_نسخہ_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_علامت = عدم;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_وچ_REG(0, make_security_guard_علامت);
	    MZ_GC_REG();
	    make_security_guard_علامت = scheme_intern_علامت("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_علامت, environment);
	    MZ_GC_UNREG();
	}
# وإلا
	make_security_guard = scheme_builtin_قدر("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	إذا (make_security_guard != عدم)
	{
	    Scheme_Object   *دلیلاں[3] = {عدم, عدم, عدم};
	    Scheme_Object   *guard = عدم;
	    Scheme_Config   *config = عدم;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_وچ_REG(0, دلیلاں, 3);
	    MZ_GC_VAR_وچ_REG(3, guard);
	    MZ_GC_VAR_وچ_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    دلیلاں[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    دلیلاں[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    دلیلاں[2] = scheme_make_prim_w_arity(sandbox_جال_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, دلیلاں);
	    MZ_GC_CHECK();
	    scheme_سیٹ_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#جے_انت
    // Create buffer and window types for use in Scheme code
    mz_بفر_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_کھڑکی_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#جے_تعریف_شدہ MZ_PRECISE_GC
    GC_رجسٹر_traversers(mz_بفر_type,
	    بفر_ناپ_proc, بفر_mark_proc, بفر_fixup_proc,
	    TRUE, TRUE);
    GC_رجسٹر_traversers(mz_کھڑکی_type,
	    کھڑکی_ناپ_proc, کھڑکی_mark_proc, کھڑکی_fixup_proc,
	    TRUE, TRUE);
#جے_انت

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
#جے_تعریف_شدہ DYNAMIC_MZSCHEME
	إذا (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    أرجع -1;
	}
#جے_انت
	إذا (لدو_ادھار_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_ادھار_module_could_not_be_loaded));
	    أرجع -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_وچ_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_پورٹ();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_پورٹ();
	MZ_GC_CHECK();
	scheme_سیٹ_param(config, MZCONFIG_OUTPUT_پورٹ, curout);
	MZ_GC_CHECK();
	scheme_سیٹ_param(config, MZCONFIG_ERROR_پورٹ, curerr);
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
    فراغ		*انکڑے,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *قدر = عدم;
    Scheme_Object   *exn = عدم;
    Scheme_Object   *prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_وچ_REG(0, قدر);
    MZ_GC_VAR_وچ_REG(1, exn);
    MZ_GC_VAR_وچ_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, انکڑے, "mzvim", 0, 0);
    MZ_GC_CHECK();
    قدر = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    إذا (!قدر)
    {
	قدر = کڈھو_exn_سنیہا(exn);
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
do_mzscheme_command(exarg_T *eap, فراغ *انکڑے, Scheme_Closed_Prim *what)
{
    إذا (mzscheme_init())
	أرجع FAIL;

    range_شروع = eap->line1;
    range_سرا = eap->line2;

    أرجع eval_with_exn_handling(انکڑے, what, عدم);
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
    MZ_GC_VAR_وچ_REG(0, bp);
    MZ_GC_REG();

    bp = بفر_REF(buf);
    bp->buf = INVALID_بفر_قدر;
#جے_تعریف_شدہ MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(bp);
#جے_انت
    buf->b_mzscheme_ref = عدم;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    فراغ
mzscheme_کھڑکی_free(win_T *win)
{
    إذا (win->w_mzscheme_ref == عدم)
	أرجع;

    vim_mz_کھڑکی *wp = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, wp);
    MZ_GC_REG();
    wp = کھڑکی_REF(win);
    wp->win = INVALID_کھڑکی_قدر;
#جے_تعریف_شدہ MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(wp);
#جے_انت
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
do_لدو(فراغ *انکڑے, صحيح noargc بن_ورتیا, Scheme_Object **noargv بن_ورتیا)
{
    Scheme_Object   *اظہاریہ = عدم;
    Scheme_Object   *نتیجہ = عدم;
    حرف	    *file = عدم;
    پورٹ_Info	    *pinfo = (پورٹ_Info *)انکڑے;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_وچ_REG(0, اظہاریہ);
    MZ_GC_VAR_وچ_REG(1, نتیجہ);
    MZ_GC_VAR_وچ_REG(2, file);
    MZ_GC_REG();

    file = (حرف *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->پورٹ = scheme_open_داخلہ_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_گنتی_سطریں(pinfo->پورٹ);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    طالما (!SCHEME_EOFP(اظہاریہ = scheme_read(pinfo->پورٹ)))
    {
	نتیجہ = scheme_eval(اظہاریہ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_داخلہ_پورٹ(pinfo->پورٹ);
    MZ_GC_CHECK();
    pinfo->پورٹ = عدم;
    MZ_GC_UNREG();
    أرجع نتیجہ;
}

/*
 * :mzfile
 */
    فراغ
ex_mzfile(exarg_T *eap)
{
    پورٹ_Info	pinfo = {عدم, عدم};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, pinfo.پورٹ);
    MZ_GC_REG();

    pinfo.name = (حرف *)eap->arg;
    إذا (do_mzscheme_command(eap, &pinfo, do_لدو) != OK
	    && pinfo.پورٹ != عدم)	// looks like port was not closed
    {
	scheme_close_داخلہ_پورٹ(pinfo.پورٹ);
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
    exn_سنیہا = scheme_builtin_قدر("exn-message");
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
کڈھو_exn_سنیہا(Scheme_Object *v)
{
    إذا (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	أرجع _scheme_apply(exn_سنیہا, 1, &v);
    وإلا
	أرجع عدم; // Not an exn structure
}

    ساكن Scheme_Object *
do_eval(فراغ *s, صحيح noargc بن_ورتیا, Scheme_Object **noargv بن_ورتیا)
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
	    پیغام(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    إذا (error)
	emsg(prev);
    وإلا
	پیغام(prev);
}

    ساكن فراغ
do_output(حرف *mesg, OUTPUT_LEN_TYPE len بن_ورتیا)
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
    OUTPUT_LEN_TYPE لمائی;

    buff = scheme_get_sized_byte_string_output(curerr, &لمائی);
    MZ_GC_CHECK();
    إذا (لمائی)
    {
	do_err_output(buff);
	أرجع;
    }

    buff = scheme_get_sized_byte_string_output(curout, &لمائی);
    MZ_GC_CHECK();
    إذا (لمائی)
	do_output(buff, لمائی);
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
vim_command(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    Scheme_Object *cmd = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, cmd);
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
vim_eval(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)انکڑے;
    Scheme_Object	*نتیجہ = عدم;
    typval_T		*vim_نتیجہ;
    Scheme_Object *اظہاریہ = عدم;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, نتیجہ);
    MZ_GC_VAR_وچ_REG(1, اظہاریہ);
    MZ_GC_REG();
    اظہاریہ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_نتیجہ = eval_اظہاریہ(BYTE_STRING_قدر(اظہاریہ), عدم);

    إذا (vim_نتیجہ == عدم)
	raise_vim_exn(_("invalid expression"));

    نتیجہ = vim_منزل_mzscheme(vim_نتیجہ);
    MZ_GC_CHECK();
    free_tv(vim_نتیجہ);

    MZ_GC_UNREG();
    أرجع نتیجہ;
#وإلا
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    أرجع scheme_false;
#جے_انت
}

/*
 * (range-start)
 */
    ساكن Scheme_Object *
get_range_شروع(
    فراغ		*انکڑے بن_ورتیا,
    صحيح			argc بن_ورتیا,
    Scheme_Object	**argv بن_ورتیا)
{
    أرجع scheme_make_integer(range_شروع);
}

/*
 * (range-end)
 */
    ساكن Scheme_Object *
get_range_سرا(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
    أرجع scheme_make_integer(range_سرا);
}

/*
 * (beep)
 */
    ساكن Scheme_Object *
mzscheme_beep(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
    vim_beep(BO_LANG);
    أرجع scheme_void;
}

ساكن Scheme_Object *M_global = عدم;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ساكن Scheme_Object *
get_option(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    لما	    قدر;
    حرف	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = عدم;
    Scheme_Object   *name = عدم;
    صحيح		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, rval);
    MZ_GC_VAR_وچ_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    إذا (argc > 1)
    {
	إذا (M_global == عدم)
	{
	    MZ_رجسٹر_STATIC(M_global);
	    M_global = scheme_intern_علامت("global");
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
	    win_T *win = get_valid_کھڑکی(argv[1]);

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
    حالة gov_نمبر:
	MZ_GC_UNREG();
	أرجع scheme_make_integer_قدر(قدر);
    حالة gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	أرجع rval;
    حالة gov_hidden_bool:
    حالة gov_hidden_نمبر:
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
سیٹ_option(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    char_u	*command = عدم;
    صحيح		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    Scheme_Object *cmd = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    إذا (argc > 1)
    {
	إذا (M_global == عدم)
	{
	    MZ_رجسٹر_STATIC(M_global);
	    M_global = scheme_intern_علامت("global");
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
	    win_T *win = get_valid_کھڑکی(argv[1]);
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
    do_سیٹ(command, scope);
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
get_curr_win(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
    أرجع (Scheme_Object *)get_vim_curr_کھڑکی();
}

/*
 * (win-count)
 */
    ساكن Scheme_Object *
get_کھڑکی_گنتی(
    فراغ		*انکڑے بن_ورتیا,
    صحيح			argc بن_ورتیا,
    Scheme_Object	**argv بن_ورتیا)
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
get_کھڑکی_list(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_بفر_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    لكل ( ; w != عدم; w = w->w_اگلا)
	إذا (w->w_بفر == buf->buf)
	{
	    list = scheme_make_pair(کھڑکی_new(w), list);
	    MZ_GC_CHECK();
	}

    أرجع list;
}

    ساكن Scheme_Object *
کھڑکی_new(win_T *win)
{
    vim_mz_کھڑکی *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    إذا (win->w_mzscheme_ref != عدم)
	أرجع (Scheme_Object *)کھڑکی_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_کھڑکی));
    CLEAR_اشارہ_گر(self);
#جے_تعریف_شدہ MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#جے_انت
    MZ_GC_CHECK();
    کھڑکی_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_کھڑکی_type;

    MZ_GC_UNREG();
    أرجع (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ساكن Scheme_Object *
get_کھڑکی_عدد(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
    صحيح		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    win_T	*win = get_کھڑکی_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    لكل (wp = firstwin; wp != win; wp = wp->w_اگلا)
	++nr;

    أرجع scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ساكن Scheme_Object *
get_کھڑکی_by_عدد(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    win_T	*win = firstwin;
    صحيح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    إذا (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    لكل ( ; win != عدم; win = win->w_اگلا, --fnum)
	إذا (fnum == 1)	    // to be 1-based
	    أرجع کھڑکی_new(win);

    أرجع scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ساكن Scheme_Object *
get_کھڑکی_بفر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    أرجع بفر_new(win->win->w_بفر);
}

/*
 * (get-win-height [window])
 */
    ساكن Scheme_Object *
get_کھڑکی_اچائی(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_اچائی);
}

/*
 * (set-win-height {height} [window])
 */
    ساكن Scheme_Object *
سیٹ_کھڑکی_اچائی(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win;
    win_T	    *savewin;
    صحيح		    اچائی;

    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    اچائی = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#جے_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#جے_انت

    savewin = curwin;
    curwin = win->win;
    win_setheight(اچائی);
    curwin = savewin;

    raise_if_error();
    أرجع scheme_void;
}

/*
 * (get-win-width [window])
 */
    ساكن Scheme_Object *
get_کھڑکی_چوڑائی(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_چوڑائی);
}

/*
 * (set-win-width {width} [window])
 */
    ساكن Scheme_Object *
سیٹ_کھڑکی_چوڑائی(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win;
    win_T	    *savewin;
    صحيح		    چوڑائی = 0;

    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    چوڑائی = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#جے_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#جے_انت

    savewin = curwin;
    curwin = win->win;
    win_setwidth(چوڑائی);
    curwin = savewin;

    raise_if_error();
    أرجع scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ساكن Scheme_Object *
get_cursor(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win;
    pos_T	    pos;

    win = get_کھڑکی_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    أرجع scheme_make_pair(scheme_make_integer_قدر((لما)pos.lnum),
		    scheme_make_integer_قدر((لما)pos.خانہ + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ساكن Scheme_Object *
سیٹ_cursor(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_کھڑکی   *win;
    لما	    lnum = 0;
    لما	    خانہ = 0;

#جے_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#جے_انت
    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    إذا (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    خانہ = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_سطر_range(lnum, win->win->w_بفر);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.خانہ = خانہ;
    win->win->w_سیٹ_curswant = true;
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
mzscheme_open_بفر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    صحيح		    عدد = 0;
    Scheme_Object   *onum = عدم;
    Scheme_Object   *buf = عدم;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_وچ_REG(0, onum);
    MZ_GC_VAR_وچ_REG(1, buf);
    MZ_GC_VAR_وچ_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#جے_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#جے_انت
    // TODO make open existing file
    عدد = buflist_جوڑو(BYTE_STRING_قدر(fname), BLN_LISTED | BLN_CURBUF);

    إذا (عدد == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(عدد);
    buf = get_بفر_by_عدد(انکڑے, 1, &onum);
    MZ_GC_UNREG();
    أرجع buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ساكن Scheme_Object *
get_بفر_by_عدد(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
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
get_بفر_by_name(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    buf_T	*buf;
    Scheme_Object *بفر = عدم;
    Scheme_Object *fname = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, بفر);
    MZ_GC_VAR_وچ_REG(1, fname);
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
get_اگلا_بفر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
    buf_T	*buf = get_بفر_arg(prim->name, 0, argc, argv)->buf;

    إذا (buf->b_اگلا == عدم)
	أرجع scheme_false;
    وإلا
	أرجع بفر_new(buf->b_اگلا);
}

/*
 * (get-prev-buff [buffer])
 */
    ساكن Scheme_Object *
get_prev_بفر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)انکڑے;
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
get_بفر_عدد(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ساكن Scheme_Object *
get_بفر_گنتی(
    فراغ		*انکڑے بن_ورتیا,
    صحيح			argc بن_ورتیا,
    Scheme_Object	**argv بن_ورتیا)
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
get_بفر_name(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_byte_string((حرف *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ساكن Scheme_Object *
get_curr_بفر(
    فراغ		*انکڑے بن_ورتیا,
    صحيح			argc بن_ورتیا,
    Scheme_Object	**argv بن_ورتیا)
{
    أرجع (Scheme_Object *)get_vim_curr_بفر();
}

    ساكن Scheme_Object *
بفر_new(buf_T *buf)
{
    vim_mz_بفر *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    إذا (buf->b_mzscheme_ref)
	أرجع (Scheme_Object *)بفر_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_بفر));
    CLEAR_اشارہ_گر(self);
#جے_تعریف_شدہ MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#جے_انت
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
get_بفر_ناپ(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_ml.ml_سطر_گنتی);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ساكن Scheme_Object *
get_بفر_سطر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf;
    صحيح		    linenr;
    char_u	    *سطر;

    buf = get_بفر_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    سطر = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    أرجع scheme_make_byte_string((حرف *)سطر);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ساكن Scheme_Object *
get_بفر_سطر_list(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf;
    صحيح		    i, hi, lo, n;
    Scheme_Object   *list = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, list);
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
سیٹ_بفر_سطر(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf;
    Scheme_Object   *سطر = عدم;
    حرف	    *save;
    صحيح		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, سطر);
    MZ_GC_REG();

#جے_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#جے_انت
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    إذا (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    سطر = argv[1];
    buf = get_بفر_arg(prim->name, 2, argc, argv);

    check_سطر_range(n, buf->buf);

    إذا (SCHEME_FALSEP(سطر))
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
	deleted_سطریں_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
    وإلا
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_منزل_سطر(سطر);

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
	    بدلیا_بائٹاں((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	إذا (buf->buf == curwin->w_بفر)
	    check_cursor_خانہ();

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
سیٹ_بفر_سطر_list(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf = عدم;
    Scheme_Object   *سطر_list = عدم;
    صحيح		    i, old_len, new_len, hi, lo;
    لما	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, سطر_list);
    MZ_GC_REG();

#جے_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#جے_انت
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    إذا (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    سطر_list = argv[2];
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

    check_سطر_range(lo, buf->buf);	    // inclusive
    check_سطر_range(hi - 1, buf->buf);	    // exclusive

    إذا (SCHEME_FALSEP(سطر_list) || SCHEME_NULLP(سطر_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	إذا (u_savedel((linenr_T)lo, (لما)old_len) == FAIL)
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
	    deleted_سطریں_mark((linenr_T)lo, (لما)old_len);
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
	new_len = scheme_proper_list_لمائی(سطر_list);
	MZ_GC_CHECK();
	إذا (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	وإلا
	{
	    حرف		**array = عدم;
	    Scheme_Object   *سطر = عدم;
	    Scheme_Object   *rest = عدم;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_وچ_REG(0, سطر);
	    MZ_GC_VAR_وچ_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(حرف *, new_len + 1);

	    rest = سطر_list;
	    لكل (i = 0; i < new_len; ++i)
	    {
		سطر = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		إذا (!SCHEME_STRINGP(سطر))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_منزل_سطر(سطر);
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
						  (لما)MAXLNUM, (لما)extra);
	بدلیا_سطریں((linenr_T)lo, 0, (linenr_T)hi, (لما)extra);

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
داخل_کرو_بفر_سطر_list(فراغ *انکڑے, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)انکڑے;
    vim_mz_بفر   *buf = عدم;
    Scheme_Object   *list = عدم;
    حرف	    *str = عدم;
    صحيح		    i, n, ناپ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, list);
    MZ_GC_REG();

#جے_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#جے_انت
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
	check_سطر_range(n, buf->buf);
    إذا (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_منزل_سطر(list);
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
	    appended_سطریں_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }

    // List
    ناپ = scheme_proper_list_لمائی(list);
    MZ_GC_CHECK();
    إذا (ناپ < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    وإلا
    {
	Scheme_Object   *سطر = عدم;
	Scheme_Object   *rest = عدم;
	حرف		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_وچ_REG(0, سطر);
	MZ_GC_VAR_وچ_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(حرف *, ناپ + 1);

	rest = list;
	لكل (i = 0; i < ناپ; ++i)
	{
	    سطر = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_منزل_سطر(سطر);
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
	    لكل (i = 0; i < ناپ; ++i)
		إذا (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    إذا (i > 0)
		appended_سطریں_mark((linenr_T)n, (لما)i);
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
vim_bufferp(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv)
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
vim_windowp(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv)
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
vim_بفر_validp(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv)
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
vim_کھڑکی_validp(فراغ *انکڑے بن_ورتیا, صحيح argc بن_ورتیا, Scheme_Object **argv)
{
    إذا (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_کھڑکی *)argv[0])->win != INVALID_کھڑکی_قدر)
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
string_منزل_سطر(Scheme_Object *obj)
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

#جے_تعریف_شدہ FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ساكن Scheme_Object *
vim_منزل_mzscheme(typval_T *vim_قدر)
{
    Scheme_Object	*نتیجہ = عدم;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_جدول	*ویکھیا = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, نتیجہ);
    MZ_GC_VAR_وچ_REG(1, ویکھیا);
    MZ_GC_REG();

    ویکھیا = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    نتیجہ = vim_منزل_mzscheme_impl(vim_قدر, 1, ویکھیا);

    MZ_GC_UNREG();
    أرجع نتیجہ;
}

    ساكن Scheme_Object *
vim_منزل_mzscheme_impl(
    typval_T		*vim_قدر,
    صحيح			ڈونگھائی,
    Scheme_Hash_جدول	*ویکھیا)
{
    Scheme_Object   *نتیجہ = عدم;
    صحيح		    new_قدر_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, نتیجہ);
    MZ_GC_VAR_وچ_REG(1, ویکھیا);
    MZ_GC_REG();

    // Avoid infinite recursion
    إذا (ڈونگھائی > 100)
    {
	MZ_GC_UNREG();
	أرجع scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    نتیجہ = scheme_hash_get(ویکھیا, (Scheme_Object *)vim_قدر);
    MZ_GC_CHECK();
    إذا (نتیجہ != عدم) // found, do nothing
	new_قدر_2 = FALSE;
    وإلا إذا (vim_قدر->v_type == VAR_STRING)
    {
	نتیجہ = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_نمبر)
    {
	نتیجہ = scheme_make_integer((لما)vim_قدر->vval.v_نمبر);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_FLOAT)
    {
	نتیجہ = scheme_make_double((مزدوج)vim_قدر->vval.v_float);
	MZ_GC_CHECK();
    }
    وإلا إذا (vim_قدر->v_type == VAR_LIST)
    {
	list_T		*list = vim_قدر->vval.v_list;
	listitem_T	*curr;

	إذا (list == عدم || list->lv_پہلا == عدم)
	    نتیجہ = scheme_null;
	وإلا
	{
	    Scheme_Object   *obj = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_وچ_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_آخری;
	    obj = vim_منزل_mzscheme_impl(&curr->li_tv, ڈونگھائی + 1, ویکھیا);
	    نتیجہ = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    طالما (curr != list->lv_پہلا)
	    {
		curr = curr->li_prev;
		obj = vim_منزل_mzscheme_impl(&curr->li_tv, ڈونگھائی + 1, ویکھیا);
		نتیجہ = scheme_make_pair(obj, نتیجہ);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    وإلا إذا (vim_قدر->v_type == VAR_DICT)
    {
	Scheme_Object	  *کلید = عدم;
	Scheme_Object	  *obj = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_وچ_REG(0, کلید);
	MZ_GC_VAR_وچ_REG(1, obj);
	MZ_GC_REG();

	نتیجہ = (Scheme_Object *)scheme_make_hash_جدول(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	إذا (vim_قدر->vval.v_dict != عدم)
	{
	    hashtab_T	*ht = &vim_قدر->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_ورتیا;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
	    {
		إذا (!HASHITEM_خالی_تھاواں(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_منزل_mzscheme_impl(&di->di_tv, ڈونگھائی + 1, ویکھیا);
		    کلید = scheme_make_byte_string((حرف *)hi->hi_کلید);
		    MZ_GC_CHECK();
		    scheme_hash_سیٹ((Scheme_Hash_جدول *)نتیجہ, کلید, obj);
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
	MZ_GC_VAR_وچ_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
	نتیجہ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ثابت حرف *)BYTE_STRING_قدر(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    وإلا إذا (vim_قدر->v_type == VAR_PARTIAL)
    {
	إذا (vim_قدر->vval.v_partial == عدم)
	    نتیجہ = scheme_null;
	وإلا
	{
	    Scheme_Object *funcname = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_وچ_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (حرف *)partial_name(vim_قدر->vval.v_partial));
	    MZ_GC_CHECK();
	    نتیجہ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ثابت حرف *)BYTE_STRING_قدر(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    وإلا إذا (vim_قدر->v_type == VAR_BOOL || vim_قدر->v_type == VAR_SPECIAL)
    {
	إذا (vim_قدر->vval.v_نمبر <= VVAL_TRUE)
	    نتیجہ = scheme_make_integer((لما)vim_قدر->vval.v_نمبر);
	وإلا
	    نتیجہ = scheme_null;
	MZ_GC_CHECK();
    }
    وإلا
    {
	نتیجہ = scheme_void;
	new_قدر_2 = FALSE;
    }
    إذا (new_قدر_2)
    {
	scheme_hash_سیٹ(ویکھیا, (Scheme_Object *)vim_قدر, نتیجہ);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    أرجع نتیجہ;
}

    ساكن صحيح
mzscheme_منزل_vim(Scheme_Object *obj, typval_T *tv)
{
    صحيح i, حالت;
    Scheme_Hash_جدول	*ویکھیا = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, obj);
    MZ_GC_VAR_وچ_REG(1, ویکھیا);
    MZ_GC_REG();

    ویکھیا = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    حالت = mzscheme_منزل_vim_impl(obj, tv, 1, ویکھیا);
    لكل (i = 0; i < ویکھیا->ناپ; ++i)
    {
	// free up remembered objects
	إذا (ویکھیا->vals[i] != عدم)
	    free_tv((typval_T *)ویکھیا->vals[i]);
    }

    MZ_GC_UNREG();
    أرجع حالت;
}
    ساكن صحيح
mzscheme_منزل_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح ڈونگھائی,
	Scheme_Hash_جدول *ویکھیا)
{
    صحيح		حالت = OK;
    typval_T	*لبھیا;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, obj);
    MZ_GC_VAR_وچ_REG(1, ویکھیا);
    MZ_GC_REG();

    MZ_GC_CHECK();
    إذا (ڈونگھائی > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = 0;
	أرجع FAIL;
    }

    لبھیا = (typval_T *)scheme_hash_get(ویکھیا, obj);
    إذا (لبھیا != عدم)
	copy_tv(لبھیا, tv);
    وإلا إذا (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_نمبر = VVAL_NULL;
    }
    وإلا إذا (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = SCHEME_INT_VAL(obj);
    }
    وإلا إذا (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_نمبر = SCHEME_TRUEP(obj);
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
# إذا MZSCHEME_نسخہ_MAJOR >= 299
    وإلا إذا (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_وچ_REG(0, tmp);
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
	    MZ_GC_VAR_وچ_REG(0, curr);
	    MZ_GC_VAR_وچ_REG(1, cval);
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
		typval_T    *ویکھیا_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, ویکھیا_tv);
		scheme_hash_سیٹ(ویکھیا, obj, (Scheme_Object *)ویکھیا_tv);

		إذا (SCHEME_VECTORP(obj))
		{
		    لكل (i = 0; i < SCHEME_VEC_ناپ(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			حالت = mzscheme_منزل_vim_impl(cval, v, ڈونگھائی + 1, ویکھیا);
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
			حالت = mzscheme_منزل_vim_impl(cval, v, ڈونگھائی + 1, ویکھیا);
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
			حالت = mzscheme_منزل_vim_impl(cval, v, ڈونگھائی + 1, ویکھیا);
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
	Scheme_Object   *کلید = عدم;
	Scheme_Object   *val = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_وچ_REG(0, کلید);
	MZ_GC_VAR_وچ_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	إذا (dict == عدم)
	    حالت = FAIL;
	وإلا
	{
	    typval_T    *ویکھیا_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, ویکھیا_tv);
	    scheme_hash_سیٹ(ویکھیا, obj, (Scheme_Object *)ویکھیا_tv);

	    لكل (i = 0; i < ((Scheme_Hash_جدول *)obj)->ناپ; ++i)
	    {
		إذا (((Scheme_Hash_جدول *) obj)->vals[i] != عدم)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *شے = dictitem_alloc((char_u *)string_منزل_سطر(
				((Scheme_Hash_جدول *) obj)->کلیدیں[i]));
		    // convert Scheme val to Vim and add it to the dict
		    إذا (mzscheme_منزل_vim_impl(((Scheme_Hash_جدول *) obj)->vals[i],
				    &شے->di_tv, ڈونگھائی + 1, ویکھیا) == FAIL
			    || dict_جوڑو(dict, شے) == FAIL)
		    {
			dictitem_free(شے);
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
	tv->vval.v_string = (char_u *)string_منزل_سطر(obj);
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
    typval_T دلیلاں;
    صحيح حالت = OK;
    Scheme_Object *نتیجہ = عدم;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, نتیجہ);
    MZ_GC_REG();

    نتیجہ = scheme_void;
    إذا (list == عدم)
	حالت = FAIL;
    وإلا
    {
	دلیلاں.v_type = VAR_LIST;
	دلیلاں.vval.v_list = list;
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

	    mzscheme_call_vim(BYTE_STRING_قدر((Scheme_Object *)name), &دلیلاں, &ret);
	    MZ_GC_CHECK();
	    نتیجہ = vim_منزل_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&دلیلاں);
    MZ_GC_UNREG();
    إذا (حالت != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    وإلا
	raise_if_error();
    أرجع نتیجہ;
}

    فراغ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_وچ_REG(0, ret);
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
#جے_انت

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
رجسٹر_vim_exn(فراغ)
{
    صحيح	nc = 0;
    صحيح i;
    Scheme_Object   *struct_exn = عدم;
    Scheme_Object   *exn_name = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_وچ_REG(0, struct_exn);
    MZ_GC_VAR_وچ_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_علامت("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_قدر("struct:exn");
    MZ_GC_CHECK();

    إذا (vim_exn == عدم)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, عدم, 0, 0, عدم, عدم
#إذا MZSCHEME_نسخہ_MAJOR >= 299
		, عدم
#جے_انت
		);


    {
	Scheme_Object   **tmp = عدم;
	Scheme_Object   *exn_ناں[5] = {عدم, عدم, عدم, عدم, عدم};
	Scheme_Object   *exn_اقدار[5] = {عدم, عدم, عدم, عدم, عدم};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_وچ_REG(0, exn_ناں, 5);
	MZ_GC_ARRAY_VAR_وچ_REG(3, exn_اقدار, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_ناں(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_ناں, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_اقدار(vim_exn, exn_ناں, nc, 0);
	mch_memmove(exn_اقدار, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	لكل (i = 0; i < nc; i++)
	{
	    scheme_جوڑو_global_علامت(exn_ناں[i],
		    exn_اقدار[i], environment);
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
raise_vim_exn(ثابت حرف *جوڑو_info)
{
    حرف	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {عدم, عدم};
    Scheme_Object   *exn = عدم;
    Scheme_Object   *byte_string = عدم;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_وچ_REG(0, argv, 2);
    MZ_GC_VAR_وچ_REG(3, exn);
    MZ_GC_VAR_وچ_REG(4, byte_string);
    MZ_GC_REG();

    إذا (جوڑو_info != عدم)
    {
	حرف		*c_string = عدم;
	Scheme_Object   *info = عدم;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_وچ_REG(0, c_string);
	MZ_GC_VAR_وچ_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(جوڑو_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (صحيح)STRLEN(fmt), 1, &info, عدم);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_منزل_char_string(byte_string);
	SCHEME_سیٹ_IMMUTABLE(argv[0]);
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

#إذا MZSCHEME_نسخہ_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#وإلا
    argv[1] = scheme_current_continuation_marks(عدم);
    MZ_GC_CHECK();
#جے_انت

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
    ساكن vim_mz_کھڑکی *
get_کھڑکی_arg(ثابت حرف *fname, صحيح argnum, صحيح argc, Scheme_Object **argv)
{
    vim_mz_کھڑکی *w;

    إذا (argc < argnum + 1)
	أرجع get_vim_curr_کھڑکی();
    w = (vim_mz_کھڑکی *)argv[argnum];
    إذا (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (فراغ)get_valid_کھڑکی(argv[argnum]);
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
win_T *get_valid_کھڑکی(فراغ *obj)
{
    win_T *win = ((vim_mz_کھڑکی *)obj)->win;
    إذا (win == INVALID_کھڑکی_قدر)
	scheme_signal_error(_("window is invalid"));
    أرجع win;
}

    صحيح
mzthreads_allowed(فراغ)
{
    أرجع mz_دھاگے_allow;
}

    ساكن صحيح
سطر_وچ_range(linenr_T lnum, buf_T *buf)
{
    أرجع (lnum > 0 && lnum <= buf->b_ml.ml_سطر_گنتی);
}

    ساكن فراغ
check_سطر_range(linenr_T lnum, buf_T *buf)
{
    إذا (!سطر_وچ_range(lnum, buf))
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
	    check_cursor_خانہ();
	}
	وإلا إذا (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وإلا
	    check_cursor_خانہ();
	بدلیا_cline_bef_curs();
    }
    invalidate_botline();
}

ساكن Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_بفر_سطر, "get-buff-line", 1, 2},
    {سیٹ_بفر_سطر, "set-buff-line", 2, 3},
    {get_بفر_سطر_list, "get-buff-line-list", 2, 3},
    {get_بفر_name, "get-buff-name", 0, 1},
    {get_بفر_عدد, "get-buff-num", 0, 1},
    {get_بفر_ناپ, "get-buff-size", 0, 1},
    {سیٹ_بفر_سطر_list, "set-buff-line-list", 3, 4},
    {داخل_کرو_بفر_سطر_list, "insert-buff-line-list", 2, 3},
    {get_curr_بفر, "curr-buff", 0, 0},
    {get_بفر_گنتی, "buff-count", 0, 0},
    {get_اگلا_بفر, "get-next-buff", 0, 1},
    {get_prev_بفر, "get-prev-buff", 0, 1},
    {mzscheme_open_بفر, "open-buff", 1, 1},
    {get_بفر_by_name, "get-buff-by-name", 1, 1},
    {get_بفر_by_عدد, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_کھڑکی_گنتی, "win-count", 0, 0},
    {get_کھڑکی_by_عدد, "get-win-by-num", 1, 1},
    {get_کھڑکی_عدد, "get-win-num", 0, 1},
    {get_کھڑکی_بفر, "get-win-buffer", 0, 1},
    {get_کھڑکی_اچائی, "get-win-height", 0, 1},
    {سیٹ_کھڑکی_اچائی, "set-win-height", 1, 2},
    {get_کھڑکی_چوڑائی, "get-win-width", 0, 1},
    {سیٹ_کھڑکی_چوڑائی, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {سیٹ_cursor, "set-cursor", 1, 2},
    {get_کھڑکی_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_شروع, "range-start", 0, 0},
    {get_range_سرا, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {سیٹ_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_بفر_validp, "buff-valid?", 1, 1},
    {vim_کھڑکی_validp, "win-valid?", 1, 1}
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
    ساكن vim_mz_کھڑکی *
get_vim_curr_کھڑکی(فراغ)
{
    إذا (curwin->w_mzscheme_ref == عدم)
	أرجع (vim_mz_کھڑکی *)کھڑکی_new(curwin);
    وإلا
	أرجع کھڑکی_REF(curwin);
}

    ساكن فراغ
make_modules(فراغ)
{
    صحيح		    i;
    Scheme_Env	    *mod = عدم;
    Scheme_Object   *vimext_علامت = عدم;
    Scheme_Object   *closed_prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_وچ_REG(0, mod);
    MZ_GC_VAR_وچ_REG(1, vimext_علامت);
    MZ_GC_VAR_وچ_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_علامت = scheme_intern_علامت("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_علامت, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    لكل (i = 0; i < (صحيح)ARRAY_لمائی(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_جوڑو_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_انت_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#جے_تعریف_شدہ HAVE_SANDBOX
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
sandbox_file_guard(صحيح argc بن_ورتیا, Scheme_Object **argv)
{
    إذا (!sandbox)
	أرجع scheme_void;

    Scheme_Object *requested_access = argv[2];

    إذا (M_write == عدم)
    {
	MZ_رجسٹر_STATIC(M_write);
	M_write = scheme_intern_علامت("write");
	MZ_GC_CHECK();
    }
    إذا (M_read == عدم)
    {
	MZ_رجسٹر_STATIC(M_read);
	M_read = scheme_intern_علامت("read");
	MZ_GC_CHECK();
    }
    إذا (M_execute == عدم)
    {
	MZ_رجسٹر_STATIC(M_execute);
	M_execute = scheme_intern_علامت("execute");
	MZ_GC_CHECK();
    }
    إذا (M_delete == عدم)
    {
	MZ_رجسٹر_STATIC(M_delete);
	M_delete = scheme_intern_علامت("delete");
	MZ_GC_CHECK();
    }

    طالما (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *شے = SCHEME_CAR(requested_access);
	إذا (scheme_eq(شے, M_write) || scheme_eq(شے, M_read)
		|| scheme_eq(شے, M_execute) || scheme_eq(شے, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    أرجع scheme_void;
}

    ساكن Scheme_Object *
sandbox_جال_guard(صحيح argc بن_ورتیا, Scheme_Object **argv بن_ورتیا)
{
    أرجع scheme_void;
}
#جے_انت

#جے_انت
