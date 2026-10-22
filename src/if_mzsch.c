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

#شامل "vim.h"

#شامل "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#اگر defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#اگر MZSCHEME_نسخہ_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_نسخہ_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#اگر_ختم

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#اگر defined(MZ_PRECISE_GC) && MZSCHEME_نسخہ_MAJOR >= 400 \
    || MZSCHEME_نسخہ_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#اگر_ختم

// Base data structures
#تعریف_کرو SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_بفر_type)
#تعریف_کرو SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_کھڑکی_type)

قسمتعریف ساخت
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_بفر;

#تعریف_کرو INVALID_بفر_قدر ((buf_T *)(-1))

قسمتعریف ساخت
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_کھڑکی;

#تعریف_کرو INVALID_کھڑکی_قدر ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
قسمتعریف ساخت
{
    Scheme_Closed_Prim	*prim;
    حرف	*name;
    صحیح		mina;	// arity information
    صحیح		maxa;
} Vim_Prim;

قسمتعریف ساخت
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
#اگر_تعریف_شدہ HAVE_SANDBOX
ساکن Scheme_Object *sandbox_file_guard(صحیح, Scheme_Object **);
ساکن Scheme_Object *sandbox_جال_guard(صحیح, Scheme_Object **);
ساکن خالی sandbox_check(خالی);
#اگر_ختم
//  Buffer-related commands
ساکن Scheme_Object *بفر_new(buf_T *buf);
ساکن Scheme_Object *get_بفر_by_عدد(خالی *, صحیح, Scheme_Object **);
ساکن vim_mz_بفر *get_vim_curr_بفر(خالی);

//  Window-related commands
ساکن Scheme_Object *کھڑکی_new(win_T *win);
ساکن vim_mz_کھڑکی *get_vim_curr_کھڑکی(خالی);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ساکن صحیح vim_error_check(خالی);
ساکن صحیح do_mzscheme_command(exarg_T *, خالی *, Scheme_Closed_Prim *what);
ساکن صحیح startup_mzscheme(خالی);
ساکن حرف *string_مقصد_سطر(Scheme_Object *obj);
#اگر MZSCHEME_نسخہ_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#ورنہ
# define OUTPUT_LEN_TYPE طویل
#اگر_ختم
ساکن خالی do_output(حرف *mesg, OUTPUT_LEN_TYPE len);
ساکن خالی do_printf(حرف *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساکن خالی do_flush(خالی);
ساکن Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ساکن Scheme_Object *نکالو_exn_پیغام(Scheme_Object *v);
ساکن Scheme_Object *do_eval(خالی *, صحیح noargc, Scheme_Object **noargv);
ساکن Scheme_Object *do_بھرو(خالی *, صحیح noargc, Scheme_Object **noargv);
ساکن خالی رجسٹر_vim_exn(خالی);
ساکن vim_mz_بفر *get_بفر_arg(ثابت حرف *fname, صحیح argnum,
	صحیح argc, Scheme_Object **argv);
ساکن vim_mz_کھڑکی *get_کھڑکی_arg(ثابت حرف *fname, صحیح argnum,
	صحیح argc, Scheme_Object **argv);
ساکن صحیح سطر_اندر_range(linenr_T, buf_T *);
ساکن خالی check_سطر_range(linenr_T, buf_T *);
ساکن خالی mz_fix_cursor(صحیح lo, صحیح hi, صحیح extra);

ساکن صحیح eval_with_exn_handling(خالی *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ساکن خالی make_modules(خالی);
ساکن خالی init_exn_catching_apply(خالی);
ساکن صحیح mzscheme_env_main(Scheme_Env *env, صحیح argc, حرف **argv);
ساکن صحیح mzscheme_init(خالی);
#اگر_تعریف_شدہ FEAT_EVAL
ساکن Scheme_Object *vim_مقصد_mzscheme(typval_T *vim_قدر);
ساکن Scheme_Object *vim_مقصد_mzscheme_impl(typval_T *vim_قدر, صحیح گہرائی,
	Scheme_Hash_جدول *دیکھا);
ساکن صحیح mzscheme_مقصد_vim(Scheme_Object *obj, typval_T *tv);
ساکن صحیح mzscheme_مقصد_vim_impl(Scheme_Object *obj, typval_T *tv, صحیح گہرائی,
	Scheme_Hash_جدول *دیکھا);
ساکن Scheme_Object *vim_funcref(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv);
#اگر_ختم

#اگر_تعریف_شدہ MZ_PRECISE_GC
ساکن صحیح بفر_سائز_proc(خالی *obj غیر_مستعمل)
{
    واپس gc_بائٹیں_مقصد_الفاظ(حجم(vim_mz_بفر));
}
ساکن صحیح بفر_mark_proc(خالی *obj)
{
    واپس بفر_سائز_proc(obj);
}
ساکن صحیح بفر_fixup_proc(خالی *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    واپس بفر_سائز_proc(obj);
}
ساکن صحیح کھڑکی_سائز_proc(خالی *obj غیر_مستعمل)
{
    واپس gc_بائٹیں_مقصد_الفاظ(حجم(vim_mz_کھڑکی));
}
ساکن صحیح کھڑکی_mark_proc(خالی *obj)
{
    واپس کھڑکی_سائز_proc(obj);
}
ساکن صحیح کھڑکی_fixup_proc(خالی *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    واپس کھڑکی_سائز_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define کھڑکی_REF(win) *(vim_mz_کھڑکی **)((win)->w_mzscheme_ref)
# define بفر_REF(buf) *(vim_mz_بفر **)((buf)->b_mzscheme_ref)
#ورنہ
# define کھڑکی_REF(win) (vim_mz_کھڑکی *)((win)->w_mzscheme_ref)
# define بفر_REF(buf) (vim_mz_بفر *)((buf)->b_mzscheme_ref)
#اگر_ختم

#اگر defined(DYNAMIC_MZSCHEME)
ساکن Scheme_Object *dll_scheme_eof;
ساکن Scheme_Object *dll_scheme_false;
ساکن Scheme_Object *dll_scheme_void;
ساکن Scheme_Object *dll_scheme_null;
ساکن Scheme_Object *dll_scheme_true;

ساکن Scheme_Thread **dll_scheme_current_thread_ptr;

ساکن خالی (**dll_scheme_console_printf_ptr)(حرف *str, ...);
ساکن خالی (**dll_scheme_console_output_ptr)(حرف *str, OUTPUT_LEN_TYPE len);
ساکن خالی (**dll_scheme_notify_multithread_ptr)(صحیح on);

ساکن خالی *(*dll_GC_malloc)(size_t سائز_اندر_بائٹیں);
ساکن خالی *(*dll_GC_malloc_atomic)(size_t سائز_اندر_بائٹیں);
ساکن Scheme_Env *(*dll_scheme_basic_env)(خالی);
ساکن خالی (*dll_scheme_check_دھاگے)(خالی);
ساکن خالی (*dll_scheme_رجسٹر_static)(خالی *ptr, طویل سائز);
ساکن خالی (*dll_scheme_سیٹ_ڈھیر_بنیاد)(خالی *بنیاد, صحیح no_auto_statics);
ساکن خالی (*dll_scheme_جوڑو_global)(ثابت حرف *name, Scheme_Object *val,
	Scheme_Env *env);
ساکن خالی (*dll_scheme_جوڑو_global_علامت)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ساکن Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, صحیح عدد_rands,
	Scheme_Object **rands);
ساکن Scheme_Object *(*dll_scheme_builtin_قدر)(ثابت حرف *name);
# اگر MZSCHEME_نسخہ_MAJOR >= 299
ساکن Scheme_Object *(*dll_scheme_byte_string_مقصد_char_string)(Scheme_Object *s);
ساکن Scheme_Object *(*dll_scheme_make_راستہ)(ثابت حرف *chars);
# endif
ساکن خالی (*dll_scheme_close_داخلہ_پورٹ)(Scheme_Object *پورٹ);
ساکن خالی (*dll_scheme_گنتی_سطریں)(Scheme_Object *پورٹ);
# اگر MZSCHEME_نسخہ_MAJOR < 360
ساکن Scheme_Object *(*dll_scheme_current_continuation_marks)(خالی);
# ورنہ
ساکن Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_لیبل);
# endif
ساکن خالی (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *پورٹ);
ساکن حرف *(*dll_scheme_display_مقصد_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ساکن صحیح (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ساکن Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	صحیح عدد_rands_2, Scheme_Object **rands, صحیح val);
ساکن خالی (*dll_scheme_dont_gc_ptr)(خالی *p);
ساکن Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ساکن Scheme_Object *(*dll_scheme_eval_string)(ثابت حرف *str,
	Scheme_Env *env);
ساکن Scheme_Object *(*dll_scheme_eval_string_all)(ثابت حرف *str,
	Scheme_Env *env, صحیح all);
ساکن خالی (*dll_scheme_اختتام_primitive_module)(Scheme_Env *env);
# اگر MZSCHEME_نسخہ_MAJOR < 299
ساکن حرف *(*dll_scheme_format)(حرف *format, صحیح flen, صحیح argc,
	Scheme_Object **argv, طویل *rlen);
# ورنہ
ساکن حرف *(*dll_scheme_format_utf8)(حرف *format, صحیح flen, صحیح argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ساکن Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, صحیح pos);
# endif
ساکن خالی (*dll_scheme_gc_ptr_ok)(خالی *p);
# اگر MZSCHEME_نسخہ_MAJOR < 299
ساکن حرف *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	طویل *len);
# ورنہ
ساکن حرف *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ساکن Scheme_Object *(*dll_scheme_intern_علامت)(ثابت حرف *name);
ساکن Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *علامت,
	Scheme_Env *env);
ساکن Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, خالی *ڈیٹا, ثابت حرف *name, mzshort mina,
     mzshort maxa);
ساکن Scheme_Object *(*dll_scheme_make_integer_قدر)(طویل i);
ساکن Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ساکن Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ثابت حرف *name, mzshort mina, mzshort maxa);
# اگر MZSCHEME_نسخہ_MAJOR < 299
ساکن Scheme_Object *(*dll_scheme_make_string)(ثابت حرف *chars);
ساکن Scheme_Object *(*dll_scheme_make_string_output_پورٹ)();
# ورنہ
ساکن Scheme_Object *(*dll_scheme_make_byte_string)(ثابت حرف *chars);
ساکن Scheme_Object *(*dll_scheme_make_byte_string_output_پورٹ)();
# endif
ساکن Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	صحیح argc, Scheme_Object **argv);
ساکن Scheme_Object **(*dll_scheme_make_struct_نام)(Scheme_Object *بنیاد,
	Scheme_Object *میدان_نام, صحیح flags, صحیح *گنتی_برآمد);
ساکن Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *بنیاد,
	Scheme_Object *والد, Scheme_Object *inspector, صحیح عدد_fields,
	صحیح عدد_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# اگر MZSCHEME_نسخہ_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ساکن Scheme_Object **(*dll_scheme_make_struct_اقدار)(
	Scheme_Object *struct_type, Scheme_Object **نام, صحیح گنتی,
	صحیح flags);
ساکن Scheme_Type (*dll_scheme_make_type)(ثابت حرف *name);
ساکن Scheme_Object *(*dll_scheme_make_سمتیہ)(صحیح سائز,
	Scheme_Object *fill);
ساکن خالی *(*dll_scheme_malloc_fail_ok)(خالی *(*f)(size_t), size_t);
ساکن Scheme_Object *(*dll_scheme_open_داخلہ_file)(ثابت حرف *name,
	ثابت حرف *who);
ساکن Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ساکن صحیح (*dll_scheme_proper_list_لمبائی)(Scheme_Object *list);
ساکن خالی (*dll_scheme_raise)(Scheme_Object *exn);
ساکن Scheme_Object *(*dll_scheme_read)(Scheme_Object *پورٹ);
ساکن خالی (*dll_scheme_signal_error)(ثابت حرف *پیغام, ...);
ساکن خالی (*dll_scheme_wrong_type)(ثابت حرف *name, ثابت حرف *expected,
	صحیح which, صحیح argc, Scheme_Object **argv);
# اگر MZSCHEME_نسخہ_MAJOR >= 299
ساکن خالی (*dll_scheme_سیٹ_param)(Scheme_Config *c, صحیح pos,
	Scheme_Object *o);
ساکن Scheme_Config *(*dll_scheme_current_config)(خالی);
ساکن Scheme_Object *(*dll_scheme_char_string_مقصد_byte_string)
    (Scheme_Object *s);
ساکن Scheme_Object *(*dll_scheme_char_string_مقصد_راستہ)
    (Scheme_Object *s);
ساکن خالی *(*dll_scheme_سیٹ_collects_راستہ)(Scheme_Object *p);
# endif
ساکن Scheme_Hash_جدول *(*dll_scheme_make_hash_جدول)(صحیح type);
ساکن خالی (*dll_scheme_hash_سیٹ)(Scheme_Hash_جدول *جدول,
	Scheme_Object *کلید, Scheme_Object *قدر);
ساکن Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_جدول *جدول,
	Scheme_Object *کلید);
ساکن Scheme_Object *(*dll_scheme_make_double)(دوہرا d);
ساکن Scheme_Object *(*dll_scheme_make_sized_byte_string)(حرف *chars,
	طویل len, صحیح copy);
ساکن Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ساکن Scheme_Object *(*dll_scheme_dynamic_wind)(خالی (*pre)(خالی *), Scheme_Object *(* متلون act)(خالی *), خالی (* متلون post)(خالی *), Scheme_Object *(*jmp_handler)(خالی *), خالی * متلون ڈیٹا);
# ifdef MZ_PRECISE_GC
ساکن خالی *(*dll_GC_malloc_one_tagged)(size_t سائز_اندر_بائٹیں);
ساکن خالی (*dll_GC_رجسٹر_traversers)(مختصر لیبل, سائز_Proc سائز, Mark_Proc mark, Fixup_Proc fixup, صحیح is_constant_سائز, صحیح is_atomic);
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 400
ساکن خالی (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ساکن خالی **(*dll_scheme_malloc_immobile_box)(خالی *p);
ساکن خالی (*dll_scheme_free_immobile_box)(خالی **b);
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ساکن صحیح (*dll_scheme_main_تیاری)(صحیح no_auto_statics, Scheme_Env_Main _main, صحیح argc, حرف **argv);
#   اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
ساکن خالی (*dll_scheme_رجسٹر_tls_space)(خالی *tls_space, صحیح tls_اشاریہ);
#   endif
#  endif
#  اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_بذریعہ_PROC)
ساکن Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(خالی);
#  endif
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 600
ساکن خالی (*dll_scheme_embedded_بھرو)(intptr_t len, ثابت حرف *s, صحیح predefined);
ساکن خالی (*dll_scheme_رجسٹر_embedded_بھرو)(intptr_t len, ثابت حرف *s);
ساکن خالی (*dll_scheme_سیٹ_config_راستہ)(Scheme_Object *p);
# endif

# اگر defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  اگر !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_جدول)
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
#  اگر MZSCHEME_نسخہ_MAJOR >= 299
#   define scheme_byte_string_مقصد_char_string dll_scheme_byte_string_مقصد_char_string
#   define scheme_make_راستہ dll_scheme_make_راستہ
#  endif
#  define scheme_check_دھاگے dll_scheme_check_دھاگے
#  define scheme_close_داخلہ_پورٹ dll_scheme_close_داخلہ_پورٹ
#  define scheme_گنتی_سطریں dll_scheme_گنتی_سطریں
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_مقصد_string dll_scheme_display_مقصد_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_اختتام_primitive_module dll_scheme_اختتام_primitive_module
#  اگر MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_format dll_scheme_format
#  ورنہ
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  اگر MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  ورنہ
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
#  اگر MZSCHEME_نسخہ_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_پورٹ dll_scheme_make_string_output_پورٹ
#  ورنہ
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_پورٹ \
    dll_scheme_make_byte_string_output_پورٹ
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_نام dll_scheme_make_struct_نام
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_اقدار dll_scheme_make_struct_اقدار
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_سمتیہ dll_scheme_make_سمتیہ
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_داخلہ_file dll_scheme_open_داخلہ_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_لمبائی dll_scheme_proper_list_لمبائی
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_رجسٹر_static dll_scheme_رجسٹر_static
#  define scheme_سیٹ_ڈھیر_بنیاد dll_scheme_سیٹ_ڈھیر_بنیاد
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  اگر MZSCHEME_نسخہ_MAJOR >= 299
#   define scheme_سیٹ_param dll_scheme_سیٹ_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_مقصد_byte_string \
    dll_scheme_char_string_مقصد_byte_string
#   define scheme_char_string_مقصد_راستہ \
    dll_scheme_char_string_مقصد_راستہ
#   define scheme_سیٹ_collects_راستہ dll_scheme_سیٹ_collects_راستہ
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
#  اگر MZSCHEME_نسخہ_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_تیاری dll_scheme_main_تیاری
#    اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
#     define scheme_رجسٹر_tls_space dll_scheme_رجسٹر_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  اگر MZSCHEME_نسخہ_MAJOR >= 600
#   define scheme_embedded_بھرو dll_scheme_embedded_بھرو
#   define scheme_رجسٹر_embedded_بھرو dll_scheme_رجسٹر_embedded_بھرو
#   define scheme_سیٹ_config_راستہ dll_scheme_سیٹ_config_راستہ
#  endif

#  ifndef PROTO
#   اگر MZSCHEME_نسخہ_MAJOR >= 500
#    اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_بذریعہ_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(خالی)
{
    واپس dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

قسمتعریف ساخت
{
    حرف    *name;
    خالی    **ptr;
} Thunk_Info;

ساکن Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (خالی **)&dll_GC_malloc},
    {"GC_malloc_atomic", (خالی **)&dll_GC_malloc_atomic},
    {تہی, تہی}};

ساکن Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (خالی **)&dll_scheme_eof},
    {"scheme_false", (خالی **)&dll_scheme_false},
    {"scheme_void", (خالی **)&dll_scheme_void},
    {"scheme_null", (خالی **)&dll_scheme_null},
    {"scheme_true", (خالی **)&dll_scheme_true},
# اگر !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_جدول)
    {"scheme_current_thread", (خالی **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (خالی **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (خالی **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(خالی **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (خالی **)&dll_scheme_جوڑو_global},
    {"scheme_add_global_symbol", (خالی **)&dll_scheme_جوڑو_global_علامت},
    {"scheme_apply", (خالی **)&dll_scheme_apply},
    {"scheme_basic_env", (خالی **)&dll_scheme_basic_env},
# اگر MZSCHEME_نسخہ_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (خالی **)&dll_scheme_byte_string_مقصد_char_string},
    {"scheme_make_path", (خالی **)&dll_scheme_make_راستہ},
# endif
    {"scheme_builtin_value", (خالی **)&dll_scheme_builtin_قدر},
    {"scheme_check_threads", (خالی **)&dll_scheme_check_دھاگے},
    {"scheme_close_input_port", (خالی **)&dll_scheme_close_داخلہ_پورٹ},
    {"scheme_count_lines", (خالی **)&dll_scheme_گنتی_سطریں},
    {"scheme_current_continuation_marks",
	(خالی **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (خالی **)&dll_scheme_display},
    {"scheme_display_to_string", (خالی **)&dll_scheme_display_مقصد_string},
    {"scheme_do_eval", (خالی **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (خالی **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (خالی **)&dll_scheme_eq},
    {"scheme_eval", (خالی **)&dll_scheme_eval},
    {"scheme_eval_string", (خالی **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (خالی **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(خالی **)&dll_scheme_اختتام_primitive_module},
# اگر MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_format", (خالی **)&dll_scheme_format},
# ورنہ
    {"scheme_format_utf8", (خالی **)&dll_scheme_format_utf8},
    {"scheme_get_param", (خالی **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (خالی **)&dll_scheme_gc_ptr_ok},
# اگر MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_get_sized_string_output",
	(خالی **)&dll_scheme_get_sized_string_output},
# ورنہ
    {"scheme_get_sized_byte_string_output",
	(خالی **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (خالی **)&dll_scheme_intern_علامت},
    {"scheme_lookup_global", (خالی **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(خالی **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (خالی **)&dll_scheme_make_integer_قدر},
    {"scheme_make_pair", (خالی **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (خالی **)&dll_scheme_make_prim_w_arity},
# اگر MZSCHEME_نسخہ_MAJOR < 299
    {"scheme_make_string", (خالی **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(خالی **)&dll_scheme_make_string_output_پورٹ},
# ورنہ
    {"scheme_make_byte_string", (خالی **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(خالی **)&dll_scheme_make_byte_string_output_پورٹ},
# endif
    {"scheme_make_struct_instance",
	(خالی **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (خالی **)&dll_scheme_make_struct_نام},
    {"scheme_make_struct_type", (خالی **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (خالی **)&dll_scheme_make_struct_اقدار},
    {"scheme_make_type", (خالی **)&dll_scheme_make_type},
    {"scheme_make_vector", (خالی **)&dll_scheme_make_سمتیہ},
    {"scheme_malloc_fail_ok", (خالی **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (خالی **)&dll_scheme_open_داخلہ_file},
    {"scheme_primitive_module", (خالی **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (خالی **)&dll_scheme_proper_list_لمبائی},
    {"scheme_raise", (خالی **)&dll_scheme_raise},
    {"scheme_read", (خالی **)&dll_scheme_read},
    {"scheme_register_static", (خالی **)&dll_scheme_رجسٹر_static},
    {"scheme_set_stack_base", (خالی **)&dll_scheme_سیٹ_ڈھیر_بنیاد},
    {"scheme_signal_error", (خالی **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (خالی **)&dll_scheme_wrong_type},
# اگر MZSCHEME_نسخہ_MAJOR >= 299
    {"scheme_set_param", (خالی **)&dll_scheme_سیٹ_param},
    {"scheme_current_config", (خالی **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(خالی **)&dll_scheme_char_string_مقصد_byte_string},
    {"scheme_char_string_to_path", (خالی **)&dll_scheme_char_string_مقصد_راستہ},
    {"scheme_set_collects_path", (خالی **)&dll_scheme_سیٹ_collects_راستہ},
# endif
    {"scheme_make_hash_table", (خالی **)&dll_scheme_make_hash_جدول},
    {"scheme_hash_set", (خالی **)&dll_scheme_hash_سیٹ},
    {"scheme_hash_get", (خالی **)&dll_scheme_hash_get},
    {"scheme_make_double", (خالی **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (خالی **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (خالی **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (خالی **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (خالی **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (خالی **)&dll_GC_رجسٹر_traversers},
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (خالی **)&dll_scheme_main_تیاری},
#   اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || MZSCHEME_نسخہ_MAJOR >= 603
    {"scheme_register_tls_space", (خالی **)&dll_scheme_رجسٹر_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (خالی **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (خالی **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (خالی **)&dll_scheme_free_immobile_box},
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 500
#  اگر defined(IMPLEMENT_THREAD_LOCAL_بذریعہ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_بذریعہ_PROC)
    {"scheme_external_get_thread_local_variables", (خالی **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# اگر MZSCHEME_نسخہ_MAJOR >= 600
    {"scheme_embedded_load", (خالی **)&dll_scheme_embedded_بھرو},
    {"scheme_register_embedded_load", (خالی **)&dll_scheme_رجسٹر_embedded_بھرو},
    {"scheme_set_config_path", (خالی **)&dll_scheme_سیٹ_config_راستہ},
# endif
    {تہی, تہی}};

ساکن HINSTANCE hMzGC = 0;
ساکن HINSTANCE hMzSch = 0;

ساکن خالی dynamic_mzscheme_انتہا(خالی);
ساکن صحیح mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll,
	صحیح verbose);

    ساکن صحیح
mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll, صحیح verbose)
{
    Thunk_Info *thunk = تہی;

    اگر (hMzGC && hMzSch)
	واپس OK;
    hMzSch = vim_بھرو_Lib(sch_dll);
    hMzGC = vim_بھرو_Lib(gc_dll);

    اگر (!hMzGC)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), gc_dll, GetWin32Error());
	واپس FAIL;
    }

    اگر (!hMzSch)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), sch_dll, GetWin32Error());
	واپس FAIL;
    }

    برائے (thunk = mzsch_imports; thunk->name; thunk++)
    {
	اگر ((*thunk->ptr =
		    (خالی *)Get_Proc_پتہ(hMzSch, thunk->name)) == تہی)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), thunk->name);
	    واپس FAIL;
	}
    }
    برائے (thunk = mzgc_imports; thunk->name; thunk++)
    {
	اگر ((*thunk->ptr =
		    (خالی *)Get_Proc_پتہ(hMzGC, thunk->name)) == تہی)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), thunk->name);
	    واپس FAIL;
	}
    }
    واپس OK;
}

    صحیح
mzscheme_enabled(صحیح verbose)
{
    واپس mzscheme_runtime_link_init(
	    (حرف *)p_mzschemedll, (حرف *)p_mzschemegcdll, verbose) == OK;
}

    ساکن خالی
dynamic_mzscheme_انتہا(خالی)
{
    اگر (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    اگر (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#اگر_ختم // DYNAMIC_MZSCHEME

#اگر MZSCHEME_نسخہ_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, عدد) GUARANTEE_STRING(proc, عدد)
#ورنہ
    ساکن Scheme_Object *
guaranteed_byte_string_arg(حرف *proc, صحیح عدد, صحیح argc, Scheme_Object **argv)
{
    اگر (SCHEME_BYTE_STRINGP(argv[عدد]))
    {
	واپس argv[عدد];
    }
    ورنہ اگر (SCHEME_CHAR_STRINGP(argv[عدد]))
    {
	Scheme_Object *tmp = تہی;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندر_REG(0, argv[عدد]);
	MZ_GC_VAR_اندر_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_مقصد_byte_string(argv[عدد]);
	MZ_GC_UNREG();
	واپس tmp;
    }
    ورنہ
	scheme_wrong_type(proc, "string", عدد, argc, argv);
    // unreachable
    واپس scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, عدد) guaranteed_byte_string_arg(proc, عدد, argc, argv)
#اگر_ختم

// need to put it here for dynamic stuff to work
#اگر defined(INCLUDE_MZSCHEME_بنیاد)
# include "mzscheme_base.c"
#اگر_ختم

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ساکن Scheme_Type mz_بفر_type;
ساکن Scheme_Type mz_کھڑکی_type;

ساکن صحیح initialized = FALSE;
#اگر_تعریف_شدہ DYNAMIC_MZSCHEME
ساکن صحیح disabled = FALSE;
#اگر_ختم
ساکن صحیح بھرو_بنیاد_module_failed = FALSE;

// global environment
ساکن Scheme_Env    *environment = تہی;
// output/error handlers
ساکن Scheme_Object *curout = تہی;
ساکن Scheme_Object *curerr = تہی;
// exn:vim exception
ساکن Scheme_Object *exn_catching_apply = تہی;
ساکن Scheme_Object *exn_p = تہی;
ساکن Scheme_Object *exn_پیغام = تہی;
ساکن Scheme_Object *vim_exn = تہی; // Vim Error exception

#اگر !defined(MZ_PRECISE_GC) || MZSCHEME_نسخہ_MAJOR < 400
ساکن خالی *ڈھیر_بنیاد = تہی;
#اگر_ختم

ساکن طویل range_آغاز;
ساکن طویل range_انتہا;

// MzScheme threads scheduling stuff
ساکن صحیح mz_دھاگے_allow = 0;

#اگر defined(FEAT_GUI_MSWIN)
ساکن خالی CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ساکن UINT timer_id = 0;
#وگرنہ_اگر defined(FEAT_GUI_GTK)
ساکن gboolean timer_proc(gpointer);
ساکن guint timer_id = 0;
#وگرنہ_اگر defined(FEAT_GUI_MOTIF)
ساکن خالی timer_proc(Xt_اشارہ_گر, XtIntervalId *);
ساکن XtIntervalId timer_id = (XtIntervalId)0;
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    خالی
mzvim_check_دھاگے(خالی)
{
    // Last time MzScheme threads were scheduled
    ساکن time_t mz_آخری_وقت = 0;

    اگر (mz_دھاگے_allow && p_mzq > 0)
    {
	time_t now = وقت(تہی);

	اگر ((now - mz_آخری_وقت) * 1000 > p_mzq)
	{
	    mz_آخری_وقت = now;
	    scheme_check_دھاگے();
	}
    }
}
#اگر_ختم

#اگر defined(MZSCHEME_GUI_دھاگے)
ساکن خالی تیاری_timer(خالی);
ساکن خالی remove_timer(خالی);

// timers are presented in GUI only
# اگر defined(FEAT_GUI_MSWIN)
    ساکن خالی CALLBACK
timer_proc(
    HWND	hwnd غیر_مستعمل,
    UINT	u_پیغام غیر_مستعمل,
    UINT_PTR	idEvent غیر_مستعمل,
    DWORD	dw_وقت غیر_مستعمل)
# elif defined(FEAT_GUI_GTK)
    ساکن gboolean
timer_proc(gpointer ڈیٹا غیر_مستعمل)
# elif defined(FEAT_GUI_MOTIF)
    ساکن خالی
timer_proc(Xt_اشارہ_گر timed_برآمد غیر_مستعمل, XtIntervalId *interval_id غیر_مستعمل)
# endif
{
    scheme_check_دھاگے();
# اگر defined(FEAT_GUI_GTK)
    واپس TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    اگر (mz_دھاگے_allow && p_mzq > 0)
	timer_id = Xt_App_جوڑو_وقت_برآمد(app_context, p_mzq,
		timer_proc, تہی);
# endif
}

    ساکن خالی
تیاری_timer(خالی)
{
# اگر defined(FEAT_GUI_MSWIN)
    timer_id = سیٹ_Timer(تہی, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_جوڑو((guint)p_mzq, (GSourceFunc)timer_proc, تہی);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_جوڑو_وقت_برآمد(app_context, p_mzq, timer_proc, تہی);
# endif
}

    ساکن خالی
remove_timer(خالی)
{
# اگر defined(FEAT_GUI_MSWIN)
    KillTimer(تہی, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_ماخذ_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_وقت_برآمد(timer_id);
# endif
    timer_id = 0;
}

#اگر_ختم // MZSCHEME_GUI_THREADS

    حرف *
did_سیٹ_mzquantum(optset_T *دلائل غیر_مستعمل)
{
#اگر defined(MZSCHEME_GUI_دھاگے)
    // reset timer
    اگر (timer_id != 0)
	remove_timer();
    اگر (mz_دھاگے_allow && p_mzq > 0 && gui.اندر_use)
	تیاری_timer();
#اگر_ختم
    واپس تہی;
}

    ساکن خالی
notify_multithread(صحیح on)
{
    mz_دھاگے_allow = on;
#اگر_تعریف_شدہ MZSCHEME_GUI_دھاگے
    اگر (on && timer_id == 0 && p_mzq > 0 && gui.اندر_use)
	تیاری_timer();
    اگر (!on && timer_id != 0)
	remove_timer();
#اگر_ختم
}

    خالی
mzscheme_انتہا(خالی)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#اگر 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_انتہا();
# endif
#اگر_ختم
}

#اگر HAVE_TLS_SPACE && !defined(VIMDLL)
# اگر defined(_MSC_VER)
ساکن __declspec(thread) خالی *tls_space;
خارجی intptr_t tls_اشاریہ;
# elif defined(__MINGW32__)
ساکن __thread خالی *tls_space;
خارجی intptr_t tls_اشاریہ;
# ورنہ
ساکن THREAD_LOCAL خالی *tls_space;
ساکن intptr_t tls_اشاریہ = 0;
# endif
#اگر_ختم

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    صحیح
mzscheme_main(خالی)
{
    صحیح	    argc = 0;
    حرف    *argv = تہی;

#اگر_تعریف_شدہ DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    اگر (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	واپس vim_main2();
    }
#اگر_ختم
#اگر_تعریف_شدہ HAVE_TLS_SPACE
# ifdef VIMDLL
    خالی **ptls_space;
    intptr_t tls_اشاریہ_2;
    خالی (*pget_tls_info)(خالی ***ptls_space, intptr_t *ptls_اشاریہ);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (خالی *)Get_Proc_پتہ(
				GetModuleHandle(تہی), "get_tls_info");
    اگر (pget_tls_info == تہی)
    {
	disabled = TRUE;
	واپس vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_اشاریہ_2);
    scheme_رجسٹر_tls_space(ptls_space, tls_اشاریہ_2);
# ورنہ
    scheme_رجسٹر_tls_space(&tls_space, tls_اشاریہ);
# endif
#اگر_ختم
#اگر_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    واپس scheme_main_تیاری(TRUE, mzscheme_env_main, argc, &argv);
#ورنہ
    واپس mzscheme_env_main(تہی, argc, &argv);
#اگر_ختم
}

    ساکن صحیح
mzscheme_env_main(Scheme_Env *env, صحیح argc غیر_مستعمل, حرف **argv غیر_مستعمل)
{
#اگر_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#ورنہ
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = تہی;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, dummy);

    ڈھیر_بنیاد = &__gc_var_stack__;
# ورنہ
    صحیح dummy = 0;
    ڈھیر_بنیاد = (خالی *)&dummy;
# endif
#اگر_ختم

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    واپس 0;
}

    ساکن Scheme_Object*
بھرو_بنیاد_module(خالی *ڈیٹا)
{
    scheme_namespace_require(scheme_intern_علامت((حرف *)ڈیٹا));
    واپس scheme_null;
}

    ساکن Scheme_Object *
بھرو_بنیاد_module_on_error(خالی *ڈیٹا غیر_مستعمل)
{
    بھرو_بنیاد_module_failed = TRUE;
    واپس scheme_null;
}

    ساکن صحیح
startup_mzscheme(خالی)
{
#اگر_غیر_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    scheme_سیٹ_ڈھیر_بنیاد(ڈھیر_بنیاد, 1);
#اگر_ختم

#اگر_غیر_تعریف_شدہ TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#اگر_ختم

    MZ_رجسٹر_STATIC(environment);
    MZ_رجسٹر_STATIC(curout);
    MZ_رجسٹر_STATIC(curerr);
    MZ_رجسٹر_STATIC(exn_catching_apply);
    MZ_رجسٹر_STATIC(exn_p);
    MZ_رجسٹر_STATIC(exn_پیغام);
    MZ_رجسٹر_STATIC(vim_exn);

    MZ_GC_CHECK();

#اگر_تعریف_شدہ INCLUDE_MZSCHEME_بنیاد
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#اگر_ختم

    // setup 'current-library-collection-paths' parameter
#اگر MZSCHEME_نسخہ_MAJOR >= 299
    {
	Scheme_Object *coll_راستہ = تہی;
	صحیح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, coll_راستہ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	اگر (s != تہی)
	{
	    coll_راستہ = scheme_make_راستہ((حرف *)s);
	    MZ_GC_CHECK();
	    اگر (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	اگر (coll_راستہ == تہی)
	{
	    coll_راستہ = scheme_make_راستہ(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	اگر (coll_راستہ != تہی)
	{
	    scheme_سیٹ_collects_راستہ(coll_راستہ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ورنہ
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = تہی;
	Scheme_Object	*coll_pair = تہی;
	Scheme_Config	*config = تہی;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_اندر_REG(0, coll_string);
	MZ_GC_VAR_اندر_REG(1, coll_pair);
	MZ_GC_VAR_اندر_REG(2, config);
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
#اگر_ختم

#اگر MZSCHEME_نسخہ_MAJOR >= 600
    {
	Scheme_Object *config_راستہ = تہی;
	صحیح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, config_راستہ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	اگر (s != تہی)
	{
	    config_راستہ = scheme_make_راستہ((حرف *)s);
	    MZ_GC_CHECK();
	    اگر (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	اگر (config_راستہ == تہی)
	{
	    config_راستہ = scheme_make_راستہ(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	اگر (config_راستہ != تہی)
	{
	    scheme_سیٹ_config_راستہ(config_راستہ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#اگر_ختم

#اگر MZSCHEME_نسخہ_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#اگر_ختم

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(تہی, بھرو_بنیاد_module, تہی,
				    بھرو_بنیاد_module_on_error, "racket/base");
	اگر (بھرو_بنیاد_module_failed)
	{
	    بھرو_بنیاد_module_failed = FALSE;
	    scheme_dynamic_wind(تہی, بھرو_بنیاد_module, تہی,
				    بھرو_بنیاد_module_on_error, "scheme/base");
	    اگر (بھرو_بنیاد_module_failed)
		واپس -1;
	}
    }

    رجسٹر_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#اگر_تعریف_شدہ HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = تہی;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, make_security_guard);
	MZ_GC_REG();

# اگر MZSCHEME_نسخہ_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_علامت = تہی;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندر_REG(0, make_security_guard_علامت);
	    MZ_GC_REG();
	    make_security_guard_علامت = scheme_intern_علامت("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_علامت, environment);
	    MZ_GC_UNREG();
	}
# ورنہ
	make_security_guard = scheme_builtin_قدر("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	اگر (make_security_guard != تہی)
	{
	    Scheme_Object   *دلائل[3] = {تہی, تہی, تہی};
	    Scheme_Object   *guard = تہی;
	    Scheme_Config   *config = تہی;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_اندر_REG(0, دلائل, 3);
	    MZ_GC_VAR_اندر_REG(3, guard);
	    MZ_GC_VAR_اندر_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    دلائل[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    دلائل[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    دلائل[2] = scheme_make_prim_w_arity(sandbox_جال_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, دلائل);
	    MZ_GC_CHECK();
	    scheme_سیٹ_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#اگر_ختم
    // Create buffer and window types for use in Scheme code
    mz_بفر_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_کھڑکی_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#اگر_تعریف_شدہ MZ_PRECISE_GC
    GC_رجسٹر_traversers(mz_بفر_type,
	    بفر_سائز_proc, بفر_mark_proc, بفر_fixup_proc,
	    TRUE, TRUE);
    GC_رجسٹر_traversers(mz_کھڑکی_type,
	    کھڑکی_سائز_proc, کھڑکی_mark_proc, کھڑکی_fixup_proc,
	    TRUE, TRUE);
#اگر_ختم

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    واپس 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ساکن صحیح
mzscheme_init(خالی)
{
    اگر (!initialized)
    {
#اگر_تعریف_شدہ DYNAMIC_MZSCHEME
	اگر (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    واپس -1;
	}
#اگر_ختم
	اگر (بھرو_بنیاد_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_بنیاد_module_could_not_be_loaded));
	    واپس -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = تہی;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, config);
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

    واپس 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ساکن صحیح
eval_with_exn_handling(
    خالی		*ڈیٹا,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *قدر = تہی;
    Scheme_Object   *exn = تہی;
    Scheme_Object   *prim = تہی;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندر_REG(0, قدر);
    MZ_GC_VAR_اندر_REG(1, exn);
    MZ_GC_VAR_اندر_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, ڈیٹا, "mzvim", 0, 0);
    MZ_GC_CHECK();
    قدر = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    اگر (!قدر)
    {
	قدر = نکالو_exn_پیغام(exn);
	// Got an exn?
	اگر (قدر)
	{
	    scheme_display(قدر, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	واپس FAIL;
    }

    اگر (ret != تہی)	// if pointer to retval supported give it up
	*ret = قدر;
    // Print any result, as long as it's not a void
    ورنہ اگر (!SCHEME_VOIDP(قدر))
    {
	scheme_display(قدر, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    واپس OK;
}

/*
 * :mzscheme
 */
    ساکن صحیح
do_mzscheme_command(exarg_T *eap, خالی *ڈیٹا, Scheme_Closed_Prim *what)
{
    اگر (mzscheme_init())
	واپس FAIL;

    range_آغاز = eap->line1;
    range_انتہا = eap->line2;

    واپس eval_with_exn_handling(ڈیٹا, what, تہی);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    خالی
mzscheme_بفر_free(buf_T *buf)
{
    اگر (buf->b_mzscheme_ref == تہی)
	واپس;

    vim_mz_بفر *bp = تہی;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, bp);
    MZ_GC_REG();

    bp = بفر_REF(buf);
    bp->buf = INVALID_بفر_قدر;
#اگر_تعریف_شدہ MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#ورنہ
    scheme_gc_ptr_ok(bp);
#اگر_ختم
    buf->b_mzscheme_ref = تہی;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    خالی
mzscheme_کھڑکی_free(win_T *win)
{
    اگر (win->w_mzscheme_ref == تہی)
	واپس;

    vim_mz_کھڑکی *wp = تہی;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, wp);
    MZ_GC_REG();
    wp = کھڑکی_REF(win);
    wp->win = INVALID_کھڑکی_قدر;
#اگر_تعریف_شدہ MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#ورنہ
    scheme_gc_ptr_ok(wp);
#اگر_ختم
    win->w_mzscheme_ref = تہی;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    خالی
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    اگر (eap->skip)
	واپس;

    اگر (script == تہی)
	do_mzscheme_command(eap, eap->arg, do_eval);
    ورنہ
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ساکن Scheme_Object *
do_بھرو(خالی *ڈیٹا, صحیح noargc غیر_مستعمل, Scheme_Object **noargv غیر_مستعمل)
{
    Scheme_Object   *اظہاریہ = تہی;
    Scheme_Object   *نتیجہ = تہی;
    حرف	    *file = تہی;
    پورٹ_Info	    *pinfo = (پورٹ_Info *)ڈیٹا;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندر_REG(0, اظہاریہ);
    MZ_GC_VAR_اندر_REG(1, نتیجہ);
    MZ_GC_VAR_اندر_REG(2, file);
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
    جب (!SCHEME_EOFP(اظہاریہ = scheme_read(pinfo->پورٹ)))
    {
	نتیجہ = scheme_eval(اظہاریہ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_داخلہ_پورٹ(pinfo->پورٹ);
    MZ_GC_CHECK();
    pinfo->پورٹ = تہی;
    MZ_GC_UNREG();
    واپس نتیجہ;
}

/*
 * :mzfile
 */
    خالی
ex_mzfile(exarg_T *eap)
{
    پورٹ_Info	pinfo = {تہی, تہی};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, pinfo.پورٹ);
    MZ_GC_REG();

    pinfo.name = (حرف *)eap->arg;
    اگر (do_mzscheme_command(eap, &pinfo, do_بھرو) != OK
	    && pinfo.پورٹ != تہی)	// looks like port was not closed
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
    ساکن خالی
init_exn_catching_apply(خالی)
{
    اگر (exn_catching_apply)
	واپس;

    حرف *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_قدر("exn?");
    MZ_GC_CHECK();
    exn_پیغام = scheme_builtin_قدر("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ساکن Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    اگر (SCHEME_TRUEP(SCHEME_CAR(v)))
	واپس SCHEME_CDR(v);
    ورنہ
    {
	*exn = SCHEME_CDR(v);
	واپس تہی;
    }
}

    ساکن Scheme_Object *
نکالو_exn_پیغام(Scheme_Object *v)
{
    اگر (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	واپس _scheme_apply(exn_پیغام, 1, &v);
    ورنہ
	واپس تہی; // Not an exn structure
}

    ساکن Scheme_Object *
do_eval(خالی *s, صحیح noargc غیر_مستعمل, Scheme_Object **noargv غیر_مستعمل)
{
    واپس scheme_eval_string_all((حرف *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ساکن خالی
do_intrnl_output(حرف *mesg, صحیح error)
{
    حرف *p, *پچھلا;

    پچھلا = mesg;
    p = strchr(پچھلا, '\n');
    جب (p)
    {
	*p = '\0';
	اگر (error)
	    emsg(پچھلا);
	ورنہ
	    پیغام(پچھلا);
	پچھلا = p + 1;
	p = strchr(پچھلا, '\n');
    }

    اگر (error)
	emsg(پچھلا);
    ورنہ
	پیغام(پچھلا);
}

    ساکن خالی
do_output(حرف *mesg, OUTPUT_LEN_TYPE len غیر_مستعمل)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ساکن خالی
do_err_output(حرف *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ساکن خالی
do_printf(حرف *format, ...)
{
    do_intrnl_output(format, 1);
}

    ساکن خالی
do_flush(خالی)
{
    حرف *buff;
    OUTPUT_LEN_TYPE لمبائی;

    buff = scheme_get_sized_byte_string_output(curerr, &لمبائی);
    MZ_GC_CHECK();
    اگر (لمبائی)
    {
	do_err_output(buff);
	واپس;
    }

    buff = scheme_get_sized_byte_string_output(curout, &لمبائی);
    MZ_GC_CHECK();
    اگر (لمبائی)
	do_output(buff, لمبائی);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    ساکن Scheme_Object *
vim_command(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    Scheme_Object *cmd = تہی;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_قدر(cmd), تہی, تہی, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    واپس scheme_void;
}

/*
 * (eval {expr-string})
 */
    ساکن Scheme_Object *
vim_eval(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)ڈیٹا;
    Scheme_Object	*نتیجہ = تہی;
    typval_T		*vim_نتیجہ;
    Scheme_Object *اظہاریہ = تہی;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, نتیجہ);
    MZ_GC_VAR_اندر_REG(1, اظہاریہ);
    MZ_GC_REG();
    اظہاریہ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_نتیجہ = eval_اظہاریہ(BYTE_STRING_قدر(اظہاریہ), تہی);

    اگر (vim_نتیجہ == تہی)
	raise_vim_exn(_("invalid expression"));

    نتیجہ = vim_مقصد_mzscheme(vim_نتیجہ);
    MZ_GC_CHECK();
    free_tv(vim_نتیجہ);

    MZ_GC_UNREG();
    واپس نتیجہ;
#ورنہ
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    واپس scheme_false;
#اگر_ختم
}

/*
 * (range-start)
 */
    ساکن Scheme_Object *
get_range_آغاز(
    خالی		*ڈیٹا غیر_مستعمل,
    صحیح			argc غیر_مستعمل,
    Scheme_Object	**argv غیر_مستعمل)
{
    واپس scheme_make_integer(range_آغاز);
}

/*
 * (range-end)
 */
    ساکن Scheme_Object *
get_range_انتہا(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
    واپس scheme_make_integer(range_انتہا);
}

/*
 * (beep)
 */
    ساکن Scheme_Object *
mzscheme_beep(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
    vim_beep(BO_LANG);
    واپس scheme_void;
}

ساکن Scheme_Object *M_global = تہی;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ساکن Scheme_Object *
get_option(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    طویل	    قدر;
    حرف	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = تہی;
    Scheme_Object   *name = تہی;
    صحیح		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, rval);
    MZ_GC_VAR_اندر_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    اگر (argc > 1)
    {
	اگر (M_global == تہی)
	{
	    MZ_رجسٹر_STATIC(M_global);
	    M_global = scheme_intern_علامت("global");
	    MZ_GC_CHECK();
	}

	اگر (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ورنہ اگر (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_بفر(argv[1]);
	    scope = OPT_LOCAL;
	}
	ورنہ اگر (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_کھڑکی(argv[1]);

	    curwin = win;
	    curbuf = win->w_بفر;
	    scope = OPT_LOCAL;
	}
	ورنہ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_قدر(BYTE_STRING_قدر(name), &قدر, (char_u **)&strval,
								  تہی, scope);
    curbuf = save_curb;
    curwin = save_curw;

    انتخاب (rc)
    {
    حالت gov_bool:
    حالت gov_نمبر:
	MZ_GC_UNREG();
	واپس scheme_make_integer_قدر(قدر);
    حالت gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	واپس rval;
    حالت gov_hidden_bool:
    حالت gov_hidden_نمبر:
    حالت gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    حالت gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    واپس scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ساکن Scheme_Object *
سیٹ_option(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    char_u	*command = تہی;
    صحیح		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    Scheme_Object *cmd = تہی;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    اگر (argc > 1)
    {
	اگر (M_global == تہی)
	{
	    MZ_رجسٹر_STATIC(M_global);
	    M_global = scheme_intern_علامت("global");
	    MZ_GC_CHECK();
	}

	اگر (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ورنہ اگر (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_بفر(argv[1]);
	    scope = OPT_LOCAL;
	}
	ورنہ اگر (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_کھڑکی(argv[1]);
	    curwin = win;
	    curbuf = win->w_بفر;
	    scope = OPT_LOCAL;
	}
	ورنہ
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
    واپس scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ساکن Scheme_Object *
get_curr_win(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
    واپس (Scheme_Object *)get_vim_curr_کھڑکی();
}

/*
 * (win-count)
 */
    ساکن Scheme_Object *
get_کھڑکی_گنتی(
    خالی		*ڈیٹا غیر_مستعمل,
    صحیح			argc غیر_مستعمل,
    Scheme_Object	**argv غیر_مستعمل)
{
    صحیح	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    واپس scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ساکن Scheme_Object *
get_کھڑکی_list(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_بفر_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    برائے ( ; w != تہی; w = w->w_اگلا)
	اگر (w->w_بفر == buf->buf)
	{
	    list = scheme_make_pair(کھڑکی_new(w), list);
	    MZ_GC_CHECK();
	}

    واپس list;
}

    ساکن Scheme_Object *
کھڑکی_new(win_T *win)
{
    vim_mz_کھڑکی *self = تہی;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    اگر (win->w_mzscheme_ref != تہی)
	واپس (Scheme_Object *)کھڑکی_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_کھڑکی));
    CLEAR_اشارہ_گر(self);
#اگر_تعریف_شدہ MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(تہی);
#ورنہ
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#اگر_ختم
    MZ_GC_CHECK();
    کھڑکی_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_کھڑکی_type;

    MZ_GC_UNREG();
    واپس (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ساکن Scheme_Object *
get_کھڑکی_عدد(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
    صحیح		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    win_T	*win = get_کھڑکی_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    برائے (wp = firstwin; wp != win; wp = wp->w_اگلا)
	++nr;

    واپس scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ساکن Scheme_Object *
get_کھڑکی_by_عدد(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    win_T	*win = firstwin;
    صحیح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    اگر (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    برائے ( ; win != تہی; win = win->w_اگلا, --fnum)
	اگر (fnum == 1)	    // to be 1-based
	    واپس کھڑکی_new(win);

    واپس scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ساکن Scheme_Object *
get_کھڑکی_بفر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    واپس بفر_new(win->win->w_بفر);
}

/*
 * (get-win-height [window])
 */
    ساکن Scheme_Object *
get_کھڑکی_اونچائی(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    واپس scheme_make_integer(win->win->w_اونچائی);
}

/*
 * (set-win-height {height} [window])
 */
    ساکن Scheme_Object *
سیٹ_کھڑکی_اونچائی(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win;
    win_T	    *savewin;
    صحیح		    اونچائی;

    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    اونچائی = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    savewin = curwin;
    curwin = win->win;
    win_setheight(اونچائی);
    curwin = savewin;

    raise_if_error();
    واپس scheme_void;
}

/*
 * (get-win-width [window])
 */
    ساکن Scheme_Object *
get_کھڑکی_چوڑائی(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win = get_کھڑکی_arg(prim->name, 0, argc, argv);

    واپس scheme_make_integer(win->win->w_چوڑائی);
}

/*
 * (set-win-width {width} [window])
 */
    ساکن Scheme_Object *
سیٹ_کھڑکی_چوڑائی(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win;
    win_T	    *savewin;
    صحیح		    چوڑائی = 0;

    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    چوڑائی = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    savewin = curwin;
    curwin = win->win;
    win_setwidth(چوڑائی);
    curwin = savewin;

    raise_if_error();
    واپس scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ساکن Scheme_Object *
get_cursor(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win;
    pos_T	    pos;

    win = get_کھڑکی_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    واپس scheme_make_pair(scheme_make_integer_قدر((طویل)pos.lnum),
		    scheme_make_integer_قدر((طویل)pos.ستون + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ساکن Scheme_Object *
سیٹ_cursor(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_کھڑکی   *win;
    طویل	    lnum = 0;
    طویل	    ستون = 0;

#اگر_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#اگر_ختم
    win = get_کھڑکی_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    اگر (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    ستون = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_سطر_range(lnum, win->win->w_بفر);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.ستون = ستون;
    win->win->w_سیٹ_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    واپس scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ساکن Scheme_Object *
mzscheme_open_بفر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    صحیح		    عدد = 0;
    Scheme_Object   *onum = تہی;
    Scheme_Object   *buf = تہی;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندر_REG(0, onum);
    MZ_GC_VAR_اندر_REG(1, buf);
    MZ_GC_VAR_اندر_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#اگر_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#اگر_ختم
    // TODO make open existing file
    عدد = buflist_جوڑو(BYTE_STRING_قدر(fname), BLN_LISTED | BLN_CURBUF);

    اگر (عدد == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(عدد);
    buf = get_بفر_by_عدد(ڈیٹا, 1, &onum);
    MZ_GC_UNREG();
    واپس buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ساکن Scheme_Object *
get_بفر_by_عدد(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    buf_T	*buf;
    صحیح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	اگر (buf->b_fnum == fnum)
	    واپس بفر_new(buf);

    واپس scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ساکن Scheme_Object *
get_بفر_by_name(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    buf_T	*buf;
    Scheme_Object *بفر = تہی;
    Scheme_Object *fname = تہی;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, بفر);
    MZ_GC_VAR_اندر_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    بفر = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	اگر (buf->b_ffname == تہی || buf->b_sfname == تہی)
	    // empty string
	{
	    اگر (BYTE_STRING_قدر(fname)[0] == NUL)
		بفر = بفر_new(buf);
	}
	ورنہ اگر (!fnamecmp(buf->b_ffname, BYTE_STRING_قدر(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_قدر(fname)))
	{
	    // either short or long filename matches
	    بفر = بفر_new(buf);
	}
    }

    MZ_GC_UNREG();
    واپس بفر;
}

/*
 * (get-next-buff [buffer])
 */
    ساکن Scheme_Object *
get_اگلا_بفر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    buf_T	*buf = get_بفر_arg(prim->name, 0, argc, argv)->buf;

    اگر (buf->b_اگلا == تہی)
	واپس scheme_false;
    ورنہ
	واپس بفر_new(buf->b_اگلا);
}

/*
 * (get-prev-buff [buffer])
 */
    ساکن Scheme_Object *
get_پچھلا_بفر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ڈیٹا;
    buf_T	*buf = get_بفر_arg(prim->name, 0, argc, argv)->buf;

    اگر (buf->b_پچھلا == تہی)
	واپس scheme_false;
    ورنہ
	واپس بفر_new(buf->b_پچھلا);
}

/*
 * (get-buff-num [buffer])
 */
    ساکن Scheme_Object *
get_بفر_عدد(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    واپس scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ساکن Scheme_Object *
get_بفر_گنتی(
    خالی		*ڈیٹا غیر_مستعمل,
    صحیح			argc غیر_مستعمل,
    Scheme_Object	**argv غیر_مستعمل)
{
    buf_T   *b;
    صحیح	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    واپس scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ساکن Scheme_Object *
get_بفر_name(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    واپس scheme_make_byte_string((حرف *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ساکن Scheme_Object *
get_curr_بفر(
    خالی		*ڈیٹا غیر_مستعمل,
    صحیح			argc غیر_مستعمل,
    Scheme_Object	**argv غیر_مستعمل)
{
    واپس (Scheme_Object *)get_vim_curr_بفر();
}

    ساکن Scheme_Object *
بفر_new(buf_T *buf)
{
    vim_mz_بفر *self = تہی;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    اگر (buf->b_mzscheme_ref)
	واپس (Scheme_Object *)بفر_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_بفر));
    CLEAR_اشارہ_گر(self);
#اگر_تعریف_شدہ MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(تہی);
#ورنہ
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#اگر_ختم
    MZ_GC_CHECK();
    بفر_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_بفر_type;

    MZ_GC_UNREG();
    واپس (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ساکن Scheme_Object *
get_بفر_سائز(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf = get_بفر_arg(prim->name, 0, argc, argv);

    واپس scheme_make_integer(buf->buf->b_ml.ml_سطر_گنتی);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ساکن Scheme_Object *
get_بفر_سطر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf;
    صحیح		    linenr;
    char_u	    *سطر;

    buf = get_بفر_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    سطر = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    واپس scheme_make_byte_string((حرف *)سطر);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ساکن Scheme_Object *
get_بفر_سطر_list(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf;
    صحیح		    i, hi, lo, n;
    Scheme_Object   *list = تہی;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, list);
    MZ_GC_REG();

    buf = get_بفر_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    اگر (lo < 0)
	lo = 0;

    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;

    n = hi - lo;

    برائے (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (حرف *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    واپس list;
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
    ساکن Scheme_Object *
سیٹ_بفر_سطر(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf;
    Scheme_Object   *سطر = تہی;
    حرف	    *save;
    صحیح		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, سطر);
    MZ_GC_REG();

#اگر_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#اگر_ختم
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    اگر (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    سطر = argv[1];
    buf = get_بفر_arg(prim->name, 2, argc, argv);

    check_سطر_range(n, buf->buf);

    اگر (SCHEME_FALSEP(سطر))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	اگر (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ورنہ اگر (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	اگر (buf->buf == curwin->w_بفر)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_سطریں_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	واپس scheme_void;
    }
    ورنہ
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_مقصد_سطر(سطر);

	curbuf = buf->buf;

	اگر (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ورنہ اگر (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	ورنہ
	{
	    vim_free(save);
	    بدلا_بائٹیں((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	اگر (buf->buf == curwin->w_بفر)
	    check_cursor_ستون();

	MZ_GC_UNREG();
	raise_if_error();
	واپس scheme_void;
    }
}

    ساکن خالی
free_array(حرف **array)
{
    حرف **curr = array;
    جب (*curr != تہی)
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
    ساکن Scheme_Object *
سیٹ_بفر_سطر_list(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf = تہی;
    Scheme_Object   *سطر_list = تہی;
    صحیح		    i, old_len, new_len, hi, lo;
    طویل	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, سطر_list);
    MZ_GC_REG();

#اگر_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#اگر_ختم
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    اگر (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    سطر_list = argv[2];
    buf = get_بفر_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    اگر (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_سطر_range(lo, buf->buf);	    // inclusive
    check_سطر_range(hi - 1, buf->buf);	    // exclusive

    اگر (SCHEME_FALSEP(سطر_list) || SCHEME_NULLP(سطر_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	اگر (u_savedel((linenr_T)lo, (طویل)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ورنہ
	{
	    برائے (i = 0; i < old_len; i++)
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    اگر (buf->buf == curwin->w_بفر)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_سطریں_mark((linenr_T)lo, (طویل)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	واپس scheme_void;
    }
    ورنہ
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_لمبائی(سطر_list);
	MZ_GC_CHECK();
	اگر (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	ورنہ
	{
	    حرف		**array = تہی;
	    Scheme_Object   *سطر = تہی;
	    Scheme_Object   *rest = تہی;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_اندر_REG(0, سطر);
	    MZ_GC_VAR_اندر_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(حرف *, new_len + 1);

	    rest = سطر_list;
	    برائے (i = 0; i < new_len; ++i)
	    {
		سطر = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		اگر (!SCHEME_STRINGP(سطر))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_مقصد_سطر(سطر);
	    }

	    curbuf = buf->buf;

	    اگر (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    برائے (i = 0; i < old_len - new_len; ++i)
	    {
		اگر (ml_delete((linenr_T)lo) == FAIL)
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
	    برائے (i = 0; i < old_len && i < new_len; i++)
		اگر (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    جب (i < new_len)
	    {
		اگر (ml_append((linenr_T)(lo + i - 1),
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
						  (طویل)MAXLNUM, (طویل)extra);
	بدلا_سطریں((linenr_T)lo, 0, (linenr_T)hi, (طویل)extra);

	اگر (buf->buf == curwin->w_بفر)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	واپس scheme_void;
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
    ساکن Scheme_Object *
داخل_کرو_بفر_سطر_list(خالی *ڈیٹا, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ڈیٹا;
    vim_mz_بفر   *buf = تہی;
    Scheme_Object   *list = تہی;
    حرف	    *str = تہی;
    صحیح		    i, n, سائز;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, list);
    MZ_GC_REG();

#اگر_تعریف_شدہ HAVE_SANDBOX
    sandbox_check();
#اگر_ختم
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    اگر (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_بفر_arg(prim->name, 2, argc, argv);

    اگر (n != 0)	    // 0 can be used in insert
	check_سطر_range(n, buf->buf);
    اگر (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_مقصد_سطر(list);
	curbuf = buf->buf;

	اگر (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ورنہ اگر (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	ورنہ
	{
	    vim_free(str);
	    appended_سطریں_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	واپس scheme_void;
    }

    // List
    سائز = scheme_proper_list_لمبائی(list);
    MZ_GC_CHECK();
    اگر (سائز < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    ورنہ
    {
	Scheme_Object   *سطر = تہی;
	Scheme_Object   *rest = تہی;
	حرف		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندر_REG(0, سطر);
	MZ_GC_VAR_اندر_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(حرف *, سائز + 1);

	rest = list;
	برائے (i = 0; i < سائز; ++i)
	{
	    سطر = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_مقصد_سطر(سطر);
	}

	curbuf = buf->buf;

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ورنہ
	{
	    برائے (i = 0; i < سائز; ++i)
		اگر (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    اگر (i > 0)
		appended_سطریں_mark((linenr_T)n, (طویل)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    واپس scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ساکن Scheme_Object *
vim_bufferp(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv)
{
    اگر (SCHEME_VIMBUFFERP(argv[0]))
	واپس scheme_true;
    ورنہ
	واپس scheme_false;
}

/*
 * (win? obj)
 */
    ساکن Scheme_Object *
vim_windowp(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv)
{
    اگر (SCHEME_VIMWINDOWP(argv[0]))
	واپس scheme_true;
    ورنہ
	واپس scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ساکن Scheme_Object *
vim_بفر_validp(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv)
{
    اگر (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_بفر *)argv[0])->buf != INVALID_بفر_قدر)
	واپس scheme_true;
    ورنہ
	واپس scheme_false;
}

/*
 * (win-valid? obj)
 */
    ساکن Scheme_Object *
vim_کھڑکی_validp(خالی *ڈیٹا غیر_مستعمل, صحیح argc غیر_مستعمل, Scheme_Object **argv)
{
    اگر (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_کھڑکی *)argv[0])->win != INVALID_کھڑکی_قدر)
	واپس scheme_true;
    ورنہ
	واپس scheme_false;
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
    ساکن حرف *
string_مقصد_سطر(Scheme_Object *obj)
{
    حرف	*scheme_str = تہی;
    حرف	*vim_str = تہی;
    OUTPUT_LEN_TYPE	len;
    صحیح		i;

    scheme_str = scheme_display_مقصد_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    اگر (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    برائے (i = 0; i < len; ++i)
    {
	اگر (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	ورنہ
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    واپس vim_str;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ساکن Scheme_Object *
vim_مقصد_mzscheme(typval_T *vim_قدر)
{
    Scheme_Object	*نتیجہ = تہی;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_جدول	*دیکھا = تہی;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, نتیجہ);
    MZ_GC_VAR_اندر_REG(1, دیکھا);
    MZ_GC_REG();

    دیکھا = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    نتیجہ = vim_مقصد_mzscheme_impl(vim_قدر, 1, دیکھا);

    MZ_GC_UNREG();
    واپس نتیجہ;
}

    ساکن Scheme_Object *
vim_مقصد_mzscheme_impl(
    typval_T		*vim_قدر,
    صحیح			گہرائی,
    Scheme_Hash_جدول	*دیکھا)
{
    Scheme_Object   *نتیجہ = تہی;
    صحیح		    new_قدر_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, نتیجہ);
    MZ_GC_VAR_اندر_REG(1, دیکھا);
    MZ_GC_REG();

    // Avoid infinite recursion
    اگر (گہرائی > 100)
    {
	MZ_GC_UNREG();
	واپس scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    نتیجہ = scheme_hash_get(دیکھا, (Scheme_Object *)vim_قدر);
    MZ_GC_CHECK();
    اگر (نتیجہ != تہی) // found, do nothing
	new_قدر_2 = FALSE;
    ورنہ اگر (vim_قدر->v_type == VAR_STRING)
    {
	نتیجہ = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_نمبر)
    {
	نتیجہ = scheme_make_integer((طویل)vim_قدر->vval.v_نمبر);
	MZ_GC_CHECK();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_FLOAT)
    {
	نتیجہ = scheme_make_double((دوہرا)vim_قدر->vval.v_float);
	MZ_GC_CHECK();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_LIST)
    {
	list_T		*list = vim_قدر->vval.v_list;
	listitem_T	*curr;

	اگر (list == تہی || list->lv_پہلا == تہی)
	    نتیجہ = scheme_null;
	ورنہ
	{
	    Scheme_Object   *obj = تہی;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندر_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_آخری;
	    obj = vim_مقصد_mzscheme_impl(&curr->li_tv, گہرائی + 1, دیکھا);
	    نتیجہ = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    جب (curr != list->lv_پہلا)
	    {
		curr = curr->li_پچھلا;
		obj = vim_مقصد_mzscheme_impl(&curr->li_tv, گہرائی + 1, دیکھا);
		نتیجہ = scheme_make_pair(obj, نتیجہ);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_DICT)
    {
	Scheme_Object	  *کلید = تہی;
	Scheme_Object	  *obj = تہی;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندر_REG(0, کلید);
	MZ_GC_VAR_اندر_REG(1, obj);
	MZ_GC_REG();

	نتیجہ = (Scheme_Object *)scheme_make_hash_جدول(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	اگر (vim_قدر->vval.v_dict != تہی)
	{
	    hashtab_T	*ht = &vim_قدر->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_مستعمل;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
	    {
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_مقصد_mzscheme_impl(&di->di_tv, گہرائی + 1, دیکھا);
		    کلید = scheme_make_byte_string((حرف *)hi->hi_کلید);
		    MZ_GC_CHECK();
		    scheme_hash_سیٹ((Scheme_Hash_جدول *)نتیجہ, کلید, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = تہی;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((حرف *)vim_قدر->vval.v_string);
	MZ_GC_CHECK();
	نتیجہ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ثابت حرف *)BYTE_STRING_قدر(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    ورنہ اگر (vim_قدر->v_type == VAR_PARTIAL)
    {
	اگر (vim_قدر->vval.v_partial == تہی)
	    نتیجہ = scheme_null;
	ورنہ
	{
	    Scheme_Object *funcname = تہی;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_اندر_REG(0, funcname);
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
    ورنہ اگر (vim_قدر->v_type == VAR_BOOL || vim_قدر->v_type == VAR_SPECIAL)
    {
	اگر (vim_قدر->vval.v_نمبر <= VVAL_TRUE)
	    نتیجہ = scheme_make_integer((طویل)vim_قدر->vval.v_نمبر);
	ورنہ
	    نتیجہ = scheme_null;
	MZ_GC_CHECK();
    }
    ورنہ
    {
	نتیجہ = scheme_void;
	new_قدر_2 = FALSE;
    }
    اگر (new_قدر_2)
    {
	scheme_hash_سیٹ(دیکھا, (Scheme_Object *)vim_قدر, نتیجہ);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    واپس نتیجہ;
}

    ساکن صحیح
mzscheme_مقصد_vim(Scheme_Object *obj, typval_T *tv)
{
    صحیح i, حالت_2;
    Scheme_Hash_جدول	*دیکھا = تہی;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, obj);
    MZ_GC_VAR_اندر_REG(1, دیکھا);
    MZ_GC_REG();

    دیکھا = scheme_make_hash_جدول(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    حالت_2 = mzscheme_مقصد_vim_impl(obj, tv, 1, دیکھا);
    برائے (i = 0; i < دیکھا->سائز; ++i)
    {
	// free up remembered objects
	اگر (دیکھا->vals[i] != تہی)
	    free_tv((typval_T *)دیکھا->vals[i]);
    }

    MZ_GC_UNREG();
    واپس حالت_2;
}
    ساکن صحیح
mzscheme_مقصد_vim_impl(Scheme_Object *obj, typval_T *tv, صحیح گہرائی,
	Scheme_Hash_جدول *دیکھا)
{
    صحیح		حالت_2 = OK;
    typval_T	*ملا;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, obj);
    MZ_GC_VAR_اندر_REG(1, دیکھا);
    MZ_GC_REG();

    MZ_GC_CHECK();
    اگر (گہرائی > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = 0;
	واپس FAIL;
    }

    ملا = (typval_T *)scheme_hash_get(دیکھا, obj);
    اگر (ملا != تہی)
	copy_tv(ملا, tv);
    ورنہ اگر (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_نمبر = VVAL_NULL;
    }
    ورنہ اگر (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = SCHEME_INT_VAL(obj);
    }
    ورنہ اگر (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_نمبر = SCHEME_TRUEP(obj);
    }
    ورنہ اگر (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    ورنہ اگر (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_قدر(obj));
    }
# اگر MZSCHEME_نسخہ_MAJOR >= 299
    ورنہ اگر (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = تہی;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_اندر_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_مقصد_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_قدر(tmp));
	MZ_GC_UNREG();
    }
# endif
    ورنہ اگر (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	اگر (list == تہی)
	    حالت_2 = FAIL;
	ورنہ
	{
	    صحیح		    i;
	    Scheme_Object   *curr = تہی;
	    Scheme_Object   *cval = تہی;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_اندر_REG(0, curr);
	    MZ_GC_VAR_اندر_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    اگر (v == تہی)
		حالت_2 = FAIL;
	    ورنہ
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *دیکھا_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, دیکھا_tv);
		scheme_hash_سیٹ(دیکھا, obj, (Scheme_Object *)دیکھا_tv);

		اگر (SCHEME_VECTORP(obj))
		{
		    برائے (i = 0; i < SCHEME_VEC_سائز(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			حالت_2 = mzscheme_مقصد_vim_impl(cval, v, گہرائی + 1, دیکھا);
			اگر (حالت_2 == FAIL)
			    توڑو;
			حالت_2 = list_append_tv(list, v);
			clear_tv(v);
			اگر (حالت_2 == FAIL)
			    توڑو;
		    }
		}
		ورنہ اگر (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    برائے (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			حالت_2 = mzscheme_مقصد_vim_impl(cval, v, گہرائی + 1, دیکھا);
			اگر (حالت_2 == FAIL)
			    توڑو;
			حالت_2 = list_append_tv(list, v);
			clear_tv(v);
			اگر (حالت_2 == FAIL)
			    توڑو;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    اگر (حالت_2 == OK && !SCHEME_NULLP(curr))
		    {
			حالت_2 = mzscheme_مقصد_vim_impl(cval, v, گہرائی + 1, دیکھا);
			اگر (حالت_2 == OK)
			{
			    حالت_2 = list_append_tv(list, v);
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
    ورنہ اگر (SCHEME_HASHTP(obj))
    {
	صحیح		i;
	dict_T		*dict;
	Scheme_Object   *کلید = تہی;
	Scheme_Object   *val = تہی;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_اندر_REG(0, کلید);
	MZ_GC_VAR_اندر_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	اگر (dict == تہی)
	    حالت_2 = FAIL;
	ورنہ
	{
	    typval_T    *دیکھا_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, دیکھا_tv);
	    scheme_hash_سیٹ(دیکھا, obj, (Scheme_Object *)دیکھا_tv);

	    برائے (i = 0; i < ((Scheme_Hash_جدول *)obj)->سائز; ++i)
	    {
		اگر (((Scheme_Hash_جدول *) obj)->vals[i] != تہی)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *شے = dictitem_alloc((char_u *)string_مقصد_سطر(
				((Scheme_Hash_جدول *) obj)->کلیدیں[i]));
		    // convert Scheme val to Vim and add it to the dict
		    اگر (mzscheme_مقصد_vim_impl(((Scheme_Hash_جدول *) obj)->vals[i],
				    &شے->di_tv, گہرائی + 1, دیکھا) == FAIL
			    || dict_جوڑو(dict, شے) == FAIL)
		    {
			dictitem_free(شے);
			حالت_2 = FAIL;
			توڑو;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    ورنہ
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_مقصد_سطر(obj);
    }
    MZ_GC_UNREG();
    واپس حالت_2;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ساکن Scheme_Object *
vim_funcref(خالی *name, صحیح argc, Scheme_Object **argv)
{
    صحیح i;
    typval_T دلائل;
    صحیح حالت_2 = OK;
    Scheme_Object *نتیجہ = تہی;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, نتیجہ);
    MZ_GC_REG();

    نتیجہ = scheme_void;
    اگر (list == تہی)
	حالت_2 = FAIL;
    ورنہ
    {
	دلائل.v_type = VAR_LIST;
	دلائل.vval.v_list = list;
	++list->lv_refcount;
	برائے (i = 0; حالت_2 == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    اگر (v == تہی)
		حالت_2 = FAIL;
	    ورنہ
	    {
		حالت_2 = mzscheme_مقصد_vim(argv[i], v);
		اگر (حالت_2 == OK)
		{
		    حالت_2 = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	اگر (حالت_2 == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_قدر((Scheme_Object *)name), &دلائل, &ret);
	    MZ_GC_CHECK();
	    نتیجہ = vim_مقصد_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&دلائل);
    MZ_GC_UNREG();
    اگر (حالت_2 != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    ورنہ
	raise_if_error();
    واپس نتیجہ;
}

    خالی
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = تہی;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_اندر_REG(0, ret);
    MZ_GC_REG();

    اگر (mzscheme_init())
    {
	MZ_GC_UNREG();
	واپس;
    }

    MZ_GC_CHECK();
    اگر (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_مقصد_vim(ret, rettv);

    MZ_GC_UNREG();
}
#اگر_ختم

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ساکن صحیح
vim_error_check(خالی)
{
    واپس (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ساکن خالی
رجسٹر_vim_exn(خالی)
{
    صحیح	nc = 0;
    صحیح i;
    Scheme_Object   *struct_exn = تہی;
    Scheme_Object   *exn_name = تہی;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_اندر_REG(0, struct_exn);
    MZ_GC_VAR_اندر_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_علامت("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_قدر("struct:exn");
    MZ_GC_CHECK();

    اگر (vim_exn == تہی)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, تہی, 0, 0, تہی, تہی
#اگر MZSCHEME_نسخہ_MAJOR >= 299
		, تہی
#اگر_ختم
		);


    {
	Scheme_Object   **tmp = تہی;
	Scheme_Object   *exn_نام[5] = {تہی, تہی, تہی, تہی, تہی};
	Scheme_Object   *exn_اقدار[5] = {تہی, تہی, تہی, تہی, تہی};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_اندر_REG(0, exn_نام, 5);
	MZ_GC_ARRAY_VAR_اندر_REG(3, exn_اقدار, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_نام(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_نام, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_اقدار(vim_exn, exn_نام, nc, 0);
	mch_memmove(exn_اقدار, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	برائے (i = 0; i < nc; i++)
	{
	    scheme_جوڑو_global_علامت(exn_نام[i],
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
    خالی
raise_vim_exn(ثابت حرف *جوڑو_info)
{
    حرف	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {تہی, تہی};
    Scheme_Object   *exn = تہی;
    Scheme_Object   *byte_string = تہی;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_اندر_REG(0, argv, 2);
    MZ_GC_VAR_اندر_REG(3, exn);
    MZ_GC_VAR_اندر_REG(4, byte_string);
    MZ_GC_REG();

    اگر (جوڑو_info != تہی)
    {
	حرف		*c_string = تہی;
	Scheme_Object   *info = تہی;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_اندر_REG(0, c_string);
	MZ_GC_VAR_اندر_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(جوڑو_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (صحیح)STRLEN(fmt), 1, &info, تہی);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_مقصد_char_string(byte_string);
	SCHEME_سیٹ_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    ورنہ
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_مقصد_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#اگر MZSCHEME_نسخہ_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#ورنہ
    argv[1] = scheme_current_continuation_marks(تہی);
    MZ_GC_CHECK();
#اگر_ختم

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    خالی
raise_if_error(خالی)
{
    اگر (vim_error_check())
	raise_vim_exn(تہی);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساکن vim_mz_بفر *
get_بفر_arg(ثابت حرف *fname, صحیح argnum, صحیح argc, Scheme_Object **argv)
{
    vim_mz_بفر *b;

    اگر (argc < argnum + 1)
	واپس get_vim_curr_بفر();
    اگر (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_بفر *)argv[argnum];
    (خالی)get_valid_بفر(argv[argnum]);
    واپس b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساکن vim_mz_کھڑکی *
get_کھڑکی_arg(ثابت حرف *fname, صحیح argnum, صحیح argc, Scheme_Object **argv)
{
    vim_mz_کھڑکی *w;

    اگر (argc < argnum + 1)
	واپس get_vim_curr_کھڑکی();
    w = (vim_mz_کھڑکی *)argv[argnum];
    اگر (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (خالی)get_valid_کھڑکی(argv[argnum]);
    واپس w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_بفر(خالی *obj)
{
    buf_T *buf = ((vim_mz_بفر *)obj)->buf;

    اگر (buf == INVALID_بفر_قدر)
	scheme_signal_error(_("buffer is invalid"));
    واپس buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_کھڑکی(خالی *obj)
{
    win_T *win = ((vim_mz_کھڑکی *)obj)->win;
    اگر (win == INVALID_کھڑکی_قدر)
	scheme_signal_error(_("window is invalid"));
    واپس win;
}

    صحیح
mzthreads_allowed(خالی)
{
    واپس mz_دھاگے_allow;
}

    ساکن صحیح
سطر_اندر_range(linenr_T lnum, buf_T *buf)
{
    واپس (lnum > 0 && lnum <= buf->b_ml.ml_سطر_گنتی);
}

    ساکن خالی
check_سطر_range(linenr_T lnum, buf_T *buf)
{
    اگر (!سطر_اندر_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ساکن خالی
mz_fix_cursor(صحیح lo, صحیح hi, صحیح extra)
{
    اگر (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	اگر (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ستون();
	}
	ورنہ اگر (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ورنہ
	    check_cursor_ستون();
	بدلا_cline_bef_curs();
    }
    invalidate_botline();
}

ساکن Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_بفر_سطر, "get-buff-line", 1, 2},
    {سیٹ_بفر_سطر, "set-buff-line", 2, 3},
    {get_بفر_سطر_list, "get-buff-line-list", 2, 3},
    {get_بفر_name, "get-buff-name", 0, 1},
    {get_بفر_عدد, "get-buff-num", 0, 1},
    {get_بفر_سائز, "get-buff-size", 0, 1},
    {سیٹ_بفر_سطر_list, "set-buff-line-list", 3, 4},
    {داخل_کرو_بفر_سطر_list, "insert-buff-line-list", 2, 3},
    {get_curr_بفر, "curr-buff", 0, 0},
    {get_بفر_گنتی, "buff-count", 0, 0},
    {get_اگلا_بفر, "get-next-buff", 0, 1},
    {get_پچھلا_بفر, "get-prev-buff", 0, 1},
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
    {get_کھڑکی_اونچائی, "get-win-height", 0, 1},
    {سیٹ_کھڑکی_اونچائی, "set-win-height", 1, 2},
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
    {get_range_آغاز, "range-start", 0, 0},
    {get_range_انتہا, "range-end", 0, 0},
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
    ساکن vim_mz_بفر *
get_vim_curr_بفر(خالی)
{
    اگر (curbuf->b_mzscheme_ref == تہی)
	واپس (vim_mz_بفر *)بفر_new(curbuf);
    ورنہ
	واپس بفر_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ساکن vim_mz_کھڑکی *
get_vim_curr_کھڑکی(خالی)
{
    اگر (curwin->w_mzscheme_ref == تہی)
	واپس (vim_mz_کھڑکی *)کھڑکی_new(curwin);
    ورنہ
	واپس کھڑکی_REF(curwin);
}

    ساکن خالی
make_modules(خالی)
{
    صحیح		    i;
    Scheme_Env	    *mod = تہی;
    Scheme_Object   *vimext_علامت = تہی;
    Scheme_Object   *closed_prim = تہی;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_اندر_REG(0, mod);
    MZ_GC_VAR_اندر_REG(1, vimext_علامت);
    MZ_GC_VAR_اندر_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_علامت = scheme_intern_علامت("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_علامت, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_جوڑو_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_اختتام_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#اگر_تعریف_شدہ HAVE_SANDBOX
ساکن Scheme_Object *M_write = تہی;
ساکن Scheme_Object *M_read = تہی;
ساکن Scheme_Object *M_execute = تہی;
ساکن Scheme_Object *M_delete = تہی;

    ساکن خالی
sandbox_check(خالی)
{
    اگر (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ساکن Scheme_Object *
sandbox_file_guard(صحیح argc غیر_مستعمل, Scheme_Object **argv)
{
    اگر (!sandbox)
	واپس scheme_void;

    Scheme_Object *requested_access = argv[2];

    اگر (M_write == تہی)
    {
	MZ_رجسٹر_STATIC(M_write);
	M_write = scheme_intern_علامت("write");
	MZ_GC_CHECK();
    }
    اگر (M_read == تہی)
    {
	MZ_رجسٹر_STATIC(M_read);
	M_read = scheme_intern_علامت("read");
	MZ_GC_CHECK();
    }
    اگر (M_execute == تہی)
    {
	MZ_رجسٹر_STATIC(M_execute);
	M_execute = scheme_intern_علامت("execute");
	MZ_GC_CHECK();
    }
    اگر (M_delete == تہی)
    {
	MZ_رجسٹر_STATIC(M_delete);
	M_delete = scheme_intern_علامت("delete");
	MZ_GC_CHECK();
    }

    جب (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *شے = SCHEME_CAR(requested_access);
	اگر (scheme_eq(شے, M_write) || scheme_eq(شے, M_read)
		|| scheme_eq(شے, M_execute) || scheme_eq(شے, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    واپس scheme_void;
}

    ساکن Scheme_Object *
sandbox_جال_guard(صحیح argc غیر_مستعمل, Scheme_Object **argv غیر_مستعمل)
{
    واپس scheme_void;
}
#اگر_ختم

#اگر_ختم
