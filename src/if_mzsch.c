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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ਜੇ defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#ਜੇ_ਅੰਤ

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#ਜੇ defined(MZ_PRECISE_GC) && MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 400 \
    || MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#ਜੇ_ਅੰਤ

// Base data structures
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_ਬਫਰ_type)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_ਖਿੜਕੀ_type)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_ਬਫਰ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_ਬਫਰ_ਮੁੱਲ ((buf_T *)(-1))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_ਖਿੜਕੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_ਖਿੜਕੀ_ਮੁੱਲ ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Scheme_Closed_Prim	*prim;
    ਅੱਖਰ	*name;
    ਪੂਰਨ		mina;	// arity information
    ਪੂਰਨ		maxa;
} Vim_Prim;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ	    *name;
    Scheme_Object   *ਪੋਰਟ;
} ਪੋਰਟ_Info;

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
ਸਥਿਰ Scheme_Object *sandbox_file_guard(ਪੂਰਨ, Scheme_Object **);
ਸਥਿਰ Scheme_Object *sandbox_ਜਾਲ_guard(ਪੂਰਨ, Scheme_Object **);
ਸਥਿਰ ਖਾਲੀ sandbox_check(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
//  Buffer-related commands
ਸਥਿਰ Scheme_Object *ਬਫਰ_new(buf_T *buf);
ਸਥਿਰ Scheme_Object *get_ਬਫਰ_by_ਅੰਕ(ਖਾਲੀ *, ਪੂਰਨ, Scheme_Object **);
ਸਥਿਰ vim_mz_ਬਫਰ *get_vim_curr_ਬਫਰ(ਖਾਲੀ);

//  Window-related commands
ਸਥਿਰ Scheme_Object *ਖਿੜਕੀ_new(win_T *win);
ਸਥਿਰ vim_mz_ਖਿੜਕੀ *get_vim_curr_ਖਿੜਕੀ(ਖਾਲੀ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ਸਥਿਰ ਪੂਰਨ vim_error_check(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ do_mzscheme_command(exarg_T *, ਖਾਲੀ *, Scheme_Closed_Prim *what);
ਸਥਿਰ ਪੂਰਨ startup_mzscheme(ਖਾਲੀ);
ਸਥਿਰ ਅੱਖਰ *string_ਮੰਜ਼ਿਲ_ਸਤਰ(Scheme_Object *obj);
#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#ਨਹੀਂਤਾਂ
# define OUTPUT_LEN_TYPE ਲੰਮਾ
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ do_output(ਅੱਖਰ *mesg, OUTPUT_LEN_TYPE len);
ਸਥਿਰ ਖਾਲੀ do_printf(ਅੱਖਰ *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ਸਥਿਰ ਖਾਲੀ do_flush(ਖਾਲੀ);
ਸਥਿਰ Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ਸਥਿਰ Scheme_Object *ਕੱਢੋ_exn_ਸੁਨੇਹਾ(Scheme_Object *v);
ਸਥਿਰ Scheme_Object *do_eval(ਖਾਲੀ *, ਪੂਰਨ noargc, Scheme_Object **noargv);
ਸਥਿਰ Scheme_Object *do_ਲੱਦੋ(ਖਾਲੀ *, ਪੂਰਨ noargc, Scheme_Object **noargv);
ਸਥਿਰ ਖਾਲੀ ਰਜਿਸਟਰ_vim_exn(ਖਾਲੀ);
ਸਥਿਰ vim_mz_ਬਫਰ *get_ਬਫਰ_arg(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ argnum,
	ਪੂਰਨ argc, Scheme_Object **argv);
ਸਥਿਰ vim_mz_ਖਿੜਕੀ *get_ਖਿੜਕੀ_arg(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ argnum,
	ਪੂਰਨ argc, Scheme_Object **argv);
ਸਥਿਰ ਪੂਰਨ ਸਤਰ_ਵਿੱਚ_range(linenr_T, buf_T *);
ਸਥਿਰ ਖਾਲੀ check_ਸਤਰ_range(linenr_T, buf_T *);
ਸਥਿਰ ਖਾਲੀ mz_fix_cursor(ਪੂਰਨ lo, ਪੂਰਨ hi, ਪੂਰਨ extra);

ਸਥਿਰ ਪੂਰਨ eval_with_exn_handling(ਖਾਲੀ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ਸਥਿਰ ਖਾਲੀ make_modules(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ init_exn_catching_apply(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ mzscheme_env_main(Scheme_Env *env, ਪੂਰਨ argc, ਅੱਖਰ **argv);
ਸਥਿਰ ਪੂਰਨ mzscheme_init(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ Scheme_Object *vim_ਮੰਜ਼ਿਲ_mzscheme(typval_T *vim_ਮੁੱਲ);
ਸਥਿਰ Scheme_Object *vim_ਮੰਜ਼ਿਲ_mzscheme_impl(typval_T *vim_ਮੁੱਲ, ਪੂਰਨ ਡੂੰਘਾਈ,
	Scheme_Hash_ਸਾਰਣੀ *ਦੇਖਿਆ);
ਸਥਿਰ ਪੂਰਨ mzscheme_ਮੰਜ਼ਿਲ_vim(Scheme_Object *obj, typval_T *tv);
ਸਥਿਰ ਪੂਰਨ mzscheme_ਮੰਜ਼ਿਲ_vim_impl(Scheme_Object *obj, typval_T *tv, ਪੂਰਨ ਡੂੰਘਾਈ,
	Scheme_Hash_ਸਾਰਣੀ *ਦੇਖਿਆ);
ਸਥਿਰ Scheme_Object *vim_funcref(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_ਮਾਪ_proc(ਖਾਲੀ *obj ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ gc_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_ਸ਼ਬਦ(ਆਕਾਰ(vim_mz_ਬਫਰ));
}
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_mark_proc(ਖਾਲੀ *obj)
{
    ਵਾਪਸ ਬਫਰ_ਮਾਪ_proc(obj);
}
ਸਥਿਰ ਪੂਰਨ ਬਫਰ_fixup_proc(ਖਾਲੀ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    ਵਾਪਸ ਬਫਰ_ਮਾਪ_proc(obj);
}
ਸਥਿਰ ਪੂਰਨ ਖਿੜਕੀ_ਮਾਪ_proc(ਖਾਲੀ *obj ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ gc_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_ਸ਼ਬਦ(ਆਕਾਰ(vim_mz_ਖਿੜਕੀ));
}
ਸਥਿਰ ਪੂਰਨ ਖਿੜਕੀ_mark_proc(ਖਾਲੀ *obj)
{
    ਵਾਪਸ ਖਿੜਕੀ_ਮਾਪ_proc(obj);
}
ਸਥਿਰ ਪੂਰਨ ਖਿੜਕੀ_fixup_proc(ਖਾਲੀ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    ਵਾਪਸ ਖਿੜਕੀ_ਮਾਪ_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define ਖਿੜਕੀ_REF(win) *(vim_mz_ਖਿੜਕੀ **)((win)->w_mzscheme_ref)
# define ਬਫਰ_REF(buf) *(vim_mz_ਬਫਰ **)((buf)->b_mzscheme_ref)
#ਨਹੀਂਤਾਂ
# define ਖਿੜਕੀ_REF(win) (vim_mz_ਖਿੜਕੀ *)((win)->w_mzscheme_ref)
# define ਬਫਰ_REF(buf) (vim_mz_ਬਫਰ *)((buf)->b_mzscheme_ref)
#ਜੇ_ਅੰਤ

#ਜੇ defined(DYNAMIC_MZSCHEME)
ਸਥਿਰ Scheme_Object *dll_scheme_eof;
ਸਥਿਰ Scheme_Object *dll_scheme_false;
ਸਥਿਰ Scheme_Object *dll_scheme_void;
ਸਥਿਰ Scheme_Object *dll_scheme_null;
ਸਥਿਰ Scheme_Object *dll_scheme_true;

ਸਥਿਰ Scheme_Thread **dll_scheme_current_thread_ptr;

ਸਥਿਰ ਖਾਲੀ (**dll_scheme_console_printf_ptr)(ਅੱਖਰ *str, ...);
ਸਥਿਰ ਖਾਲੀ (**dll_scheme_console_output_ptr)(ਅੱਖਰ *str, OUTPUT_LEN_TYPE len);
ਸਥਿਰ ਖਾਲੀ (**dll_scheme_notify_multithread_ptr)(ਪੂਰਨ on);

ਸਥਿਰ ਖਾਲੀ *(*dll_GC_malloc)(size_t ਮਾਪ_ਵਿੱਚ_ਬਾਈਟਾਂ);
ਸਥਿਰ ਖਾਲੀ *(*dll_GC_malloc_atomic)(size_t ਮਾਪ_ਵਿੱਚ_ਬਾਈਟਾਂ);
ਸਥਿਰ Scheme_Env *(*dll_scheme_basic_env)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_check_ਧਾਗੇ)(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਰਜਿਸਟਰ_static)(ਖਾਲੀ *ptr, ਲੰਮਾ ਮਾਪ);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਸੈੱਟ_ਢੇਰ_ਅਧਾਰ)(ਖਾਲੀ *ਅਧਾਰ, ਪੂਰਨ no_auto_statics);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਜੋੜੋ_global)(ਅਚਲ ਅੱਖਰ *name, Scheme_Object *val,
	Scheme_Env *env);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਜੋੜੋ_global_ਚਿੰਨ੍ਹ)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ਸਥਿਰ Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, ਪੂਰਨ ਅੰਕ_rands,
	Scheme_Object **rands);
ਸਥਿਰ Scheme_Object *(*dll_scheme_builtin_ਮੁੱਲ)(ਅਚਲ ਅੱਖਰ *name);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
ਸਥਿਰ Scheme_Object *(*dll_scheme_byte_string_ਮੰਜ਼ਿਲ_char_string)(Scheme_Object *s);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_ਰਸਤਾ)(ਅਚਲ ਅੱਖਰ *chars);
# endif
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ)(Scheme_Object *ਪੋਰਟ);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਗਿਣਤੀ_ਸਤਰਾਂ)(Scheme_Object *ਪੋਰਟ);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 360
ਸਥਿਰ Scheme_Object *(*dll_scheme_current_continuation_marks)(ਖਾਲੀ);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_ਟੈਗ);
# endif
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *ਪੋਰਟ);
ਸਥਿਰ ਅੱਖਰ *(*dll_scheme_display_ਮੰਜ਼ਿਲ_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ਸਥਿਰ ਪੂਰਨ (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ਸਥਿਰ Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	ਪੂਰਨ ਅੰਕ_rands_2, Scheme_Object **rands, ਪੂਰਨ val);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_dont_gc_ptr)(ਖਾਲੀ *p);
ਸਥਿਰ Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ਸਥਿਰ Scheme_Object *(*dll_scheme_eval_string)(ਅਚਲ ਅੱਖਰ *str,
	Scheme_Env *env);
ਸਥਿਰ Scheme_Object *(*dll_scheme_eval_string_all)(ਅਚਲ ਅੱਖਰ *str,
	Scheme_Env *env, ਪੂਰਨ all);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਅੰਤ_primitive_module)(Scheme_Env *env);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
ਸਥਿਰ ਅੱਖਰ *(*dll_scheme_format)(ਅੱਖਰ *format, ਪੂਰਨ flen, ਪੂਰਨ argc,
	Scheme_Object **argv, ਲੰਮਾ *rlen);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਅੱਖਰ *(*dll_scheme_format_utf8)(ਅੱਖਰ *format, ਪੂਰਨ flen, ਪੂਰਨ argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ਸਥਿਰ Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, ਪੂਰਨ pos);
# endif
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_gc_ptr_ok)(ਖਾਲੀ *p);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
ਸਥਿਰ ਅੱਖਰ *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	ਲੰਮਾ *len);
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਅੱਖਰ *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ਸਥਿਰ Scheme_Object *(*dll_scheme_intern_ਚਿੰਨ੍ਹ)(ਅਚਲ ਅੱਖਰ *name);
ਸਥਿਰ Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *ਚਿੰਨ੍ਹ,
	Scheme_Env *env);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, ਖਾਲੀ *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *name, mzshort mina,
     mzshort maxa);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_integer_ਮੁੱਲ)(ਲੰਮਾ i);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ਅਚਲ ਅੱਖਰ *name, mzshort mina, mzshort maxa);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_string)(ਅਚਲ ਅੱਖਰ *chars);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_string_output_ਪੋਰਟ)();
# ਨਹੀਂਤਾਂ
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_byte_string)(ਅਚਲ ਅੱਖਰ *chars);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_byte_string_output_ਪੋਰਟ)();
# endif
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	ਪੂਰਨ argc, Scheme_Object **argv);
ਸਥਿਰ Scheme_Object **(*dll_scheme_make_struct_ਨਾਂ)(Scheme_Object *ਅਧਾਰ,
	Scheme_Object *ਖੇਤਰ_ਨਾਂ, ਪੂਰਨ flags, ਪੂਰਨ *ਗਿਣਤੀ_ਬਾਹਰਲਾ);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *ਅਧਾਰ,
	Scheme_Object *ਮਾਪਾ, Scheme_Object *inspector, ਪੂਰਨ ਅੰਕ_fields,
	ਪੂਰਨ ਅੰਕ_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ਸਥਿਰ Scheme_Object **(*dll_scheme_make_struct_ਮੁੱਲਾਂ)(
	Scheme_Object *struct_type, Scheme_Object **ਨਾਂ, ਪੂਰਨ ਗਿਣਤੀ_2,
	ਪੂਰਨ flags);
ਸਥਿਰ Scheme_Type (*dll_scheme_make_type)(ਅਚਲ ਅੱਖਰ *name);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_ਸਦਿਸ਼)(ਪੂਰਨ ਮਾਪ,
	Scheme_Object *fill);
ਸਥਿਰ ਖਾਲੀ *(*dll_scheme_malloc_fail_ok)(ਖਾਲੀ *(*f)(size_t), size_t);
ਸਥਿਰ Scheme_Object *(*dll_scheme_open_ਨਿਵੇਸ਼_file)(ਅਚਲ ਅੱਖਰ *name,
	ਅਚਲ ਅੱਖਰ *who);
ਸਥਿਰ Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ਸਥਿਰ ਪੂਰਨ (*dll_scheme_proper_list_ਲੰਮਾਈ)(Scheme_Object *list);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_raise)(Scheme_Object *exn);
ਸਥਿਰ Scheme_Object *(*dll_scheme_read)(Scheme_Object *ਪੋਰਟ);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_signal_error)(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ, ...);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_wrong_type)(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *expected,
	ਪੂਰਨ which, ਪੂਰਨ argc, Scheme_Object **argv);
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਸੈੱਟ_param)(Scheme_Config *c, ਪੂਰਨ pos,
	Scheme_Object *o);
ਸਥਿਰ Scheme_Config *(*dll_scheme_current_config)(ਖਾਲੀ);
ਸਥਿਰ Scheme_Object *(*dll_scheme_char_string_ਮੰਜ਼ਿਲ_byte_string)
    (Scheme_Object *s);
ਸਥਿਰ Scheme_Object *(*dll_scheme_char_string_ਮੰਜ਼ਿਲ_ਰਸਤਾ)
    (Scheme_Object *s);
ਸਥਿਰ ਖਾਲੀ *(*dll_scheme_ਸੈੱਟ_collects_ਰਸਤਾ)(Scheme_Object *p);
# endif
ਸਥਿਰ Scheme_Hash_ਸਾਰਣੀ *(*dll_scheme_make_hash_ਸਾਰਣੀ)(ਪੂਰਨ type);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_hash_ਸੈੱਟ)(Scheme_Hash_ਸਾਰਣੀ *ਸਾਰਣੀ,
	Scheme_Object *ਕੁੰਜੀ, Scheme_Object *ਮੁੱਲ);
ਸਥਿਰ Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_ਸਾਰਣੀ *ਸਾਰਣੀ,
	Scheme_Object *ਕੁੰਜੀ);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_double)(ਦੁੱਗਣਾ d);
ਸਥਿਰ Scheme_Object *(*dll_scheme_make_sized_byte_string)(ਅੱਖਰ *chars,
	ਲੰਮਾ len, ਪੂਰਨ copy);
ਸਥਿਰ Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ਸਥਿਰ Scheme_Object *(*dll_scheme_dynamic_wind)(ਖਾਲੀ (*pre)(ਖਾਲੀ *), Scheme_Object *(* ਅਸਥਿਰ act)(ਖਾਲੀ *), ਖਾਲੀ (* ਅਸਥਿਰ post)(ਖਾਲੀ *), Scheme_Object *(*jmp_handler)(ਖਾਲੀ *), ਖਾਲੀ * ਅਸਥਿਰ ਅੰਕੜੇ);
# ifdef MZ_PRECISE_GC
ਸਥਿਰ ਖਾਲੀ *(*dll_GC_malloc_one_tagged)(size_t ਮਾਪ_ਵਿੱਚ_ਬਾਈਟਾਂ);
ਸਥਿਰ ਖਾਲੀ (*dll_GC_ਰਜਿਸਟਰ_traversers)(ਛੋਟਾ ਟੈਗ, ਮਾਪ_Proc ਮਾਪ, Mark_Proc mark, Fixup_Proc fixup, ਪੂਰਨ is_constant_ਮਾਪ, ਪੂਰਨ is_atomic);
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 400
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ਸਥਿਰ ਖਾਲੀ **(*dll_scheme_malloc_immobile_box)(ਖਾਲੀ *p);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_free_immobile_box)(ਖਾਲੀ **b);
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ਸਥਿਰ ਪੂਰਨ (*dll_scheme_main_ਤਿਆਰੀ)(ਪੂਰਨ no_auto_statics, Scheme_Env_Main _main, ਪੂਰਨ argc, ਅੱਖਰ **argv);
#   ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 603
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਰਜਿਸਟਰ_tls_space)(ਖਾਲੀ *tls_space, ਪੂਰਨ tls_ਸੂਚਕਾਂਕ);
#   endif
#  endif
#  ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ਰਾਹੀਂ_PROC)
ਸਥਿਰ Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(ਖਾਲੀ);
#  endif
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 600
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_embedded_ਲੱਦੋ)(intptr_t len, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ predefined);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਰਜਿਸਟਰ_embedded_ਲੱਦੋ)(intptr_t len, ਅਚਲ ਅੱਖਰ *s);
ਸਥਿਰ ਖਾਲੀ (*dll_scheme_ਸੈੱਟ_config_ਰਸਤਾ)(Scheme_Object *p);
# endif

# ਜੇ defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  ਜੇ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_ਸਾਰਣੀ)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_ਜੋੜੋ_global dll_scheme_ਜੋੜੋ_global
#  define scheme_ਜੋੜੋ_global_ਚਿੰਨ੍ਹ dll_scheme_ਜੋੜੋ_global_ਚਿੰਨ੍ਹ
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_ਮੁੱਲ dll_scheme_builtin_ਮੁੱਲ
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
#   define scheme_byte_string_ਮੰਜ਼ਿਲ_char_string dll_scheme_byte_string_ਮੰਜ਼ਿਲ_char_string
#   define scheme_make_ਰਸਤਾ dll_scheme_make_ਰਸਤਾ
#  endif
#  define scheme_check_ਧਾਗੇ dll_scheme_check_ਧਾਗੇ
#  define scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ dll_scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ
#  define scheme_ਗਿਣਤੀ_ਸਤਰਾਂ dll_scheme_ਗਿਣਤੀ_ਸਤਰਾਂ
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_ਮੰਜ਼ਿਲ_string dll_scheme_display_ਮੰਜ਼ਿਲ_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_ਅੰਤ_primitive_module dll_scheme_ਅੰਤ_primitive_module
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
#   define scheme_format dll_scheme_format
#  ਨਹੀਂਤਾਂ
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  ਨਹੀਂਤਾਂ
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_ਚਿੰਨ੍ਹ dll_scheme_intern_ਚਿੰਨ੍ਹ
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_ਮੁੱਲ dll_scheme_make_integer_ਮੁੱਲ
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_ਪੋਰਟ dll_scheme_make_string_output_ਪੋਰਟ
#  ਨਹੀਂਤਾਂ
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_ਪੋਰਟ \
    dll_scheme_make_byte_string_output_ਪੋਰਟ
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_ਨਾਂ dll_scheme_make_struct_ਨਾਂ
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_ਮੁੱਲਾਂ dll_scheme_make_struct_ਮੁੱਲਾਂ
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_ਸਦਿਸ਼ dll_scheme_make_ਸਦਿਸ਼
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_ਨਿਵੇਸ਼_file dll_scheme_open_ਨਿਵੇਸ਼_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_ਲੰਮਾਈ dll_scheme_proper_list_ਲੰਮਾਈ
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_ਰਜਿਸਟਰ_static dll_scheme_ਰਜਿਸਟਰ_static
#  define scheme_ਸੈੱਟ_ਢੇਰ_ਅਧਾਰ dll_scheme_ਸੈੱਟ_ਢੇਰ_ਅਧਾਰ
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
#   define scheme_ਸੈੱਟ_param dll_scheme_ਸੈੱਟ_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_ਮੰਜ਼ਿਲ_byte_string \
    dll_scheme_char_string_ਮੰਜ਼ਿਲ_byte_string
#   define scheme_char_string_ਮੰਜ਼ਿਲ_ਰਸਤਾ \
    dll_scheme_char_string_ਮੰਜ਼ਿਲ_ਰਸਤਾ
#   define scheme_ਸੈੱਟ_collects_ਰਸਤਾ dll_scheme_ਸੈੱਟ_collects_ਰਸਤਾ
#  endif
#  define scheme_make_hash_ਸਾਰਣੀ dll_scheme_make_hash_ਸਾਰਣੀ
#  define scheme_hash_ਸੈੱਟ dll_scheme_hash_ਸੈੱਟ
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_ਰਜਿਸਟਰ_traversers dll_GC_ਰਜਿਸਟਰ_traversers
#  endif
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_ਤਿਆਰੀ dll_scheme_main_ਤਿਆਰੀ
#    ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 603
#     define scheme_ਰਜਿਸਟਰ_tls_space dll_scheme_ਰਜਿਸਟਰ_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 600
#   define scheme_embedded_ਲੱਦੋ dll_scheme_embedded_ਲੱਦੋ
#   define scheme_ਰਜਿਸਟਰ_embedded_ਲੱਦੋ dll_scheme_ਰਜਿਸਟਰ_embedded_ਲੱਦੋ
#   define scheme_ਸੈੱਟ_config_ਰਸਤਾ dll_scheme_ਸੈੱਟ_config_ਰਸਤਾ
#  endif

#  ifndef PROTO
#   ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 500
#    ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ਰਾਹੀਂ_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(ਖਾਲੀ)
{
    ਵਾਪਸ dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ    *name;
    ਖਾਲੀ    **ptr;
} Thunk_Info;

ਸਥਿਰ Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (ਖਾਲੀ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (ਖਾਲੀ **)&dll_GC_malloc_atomic},
    {ਨਿਰਾ, ਨਿਰਾ}};

ਸਥਿਰ Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (ਖਾਲੀ **)&dll_scheme_eof},
    {"scheme_false", (ਖਾਲੀ **)&dll_scheme_false},
    {"scheme_void", (ਖਾਲੀ **)&dll_scheme_void},
    {"scheme_null", (ਖਾਲੀ **)&dll_scheme_null},
    {"scheme_true", (ਖਾਲੀ **)&dll_scheme_true},
# ਜੇ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_ਸਾਰਣੀ)
    {"scheme_current_thread", (ਖਾਲੀ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (ਖਾਲੀ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (ਖਾਲੀ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(ਖਾਲੀ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (ਖਾਲੀ **)&dll_scheme_ਜੋੜੋ_global},
    {"scheme_add_global_symbol", (ਖਾਲੀ **)&dll_scheme_ਜੋੜੋ_global_ਚਿੰਨ੍ਹ},
    {"scheme_apply", (ਖਾਲੀ **)&dll_scheme_apply},
    {"scheme_basic_env", (ਖਾਲੀ **)&dll_scheme_basic_env},
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (ਖਾਲੀ **)&dll_scheme_byte_string_ਮੰਜ਼ਿਲ_char_string},
    {"scheme_make_path", (ਖਾਲੀ **)&dll_scheme_make_ਰਸਤਾ},
# endif
    {"scheme_builtin_value", (ਖਾਲੀ **)&dll_scheme_builtin_ਮੁੱਲ},
    {"scheme_check_threads", (ਖਾਲੀ **)&dll_scheme_check_ਧਾਗੇ},
    {"scheme_close_input_port", (ਖਾਲੀ **)&dll_scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ},
    {"scheme_count_lines", (ਖਾਲੀ **)&dll_scheme_ਗਿਣਤੀ_ਸਤਰਾਂ},
    {"scheme_current_continuation_marks",
	(ਖਾਲੀ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (ਖਾਲੀ **)&dll_scheme_display},
    {"scheme_display_to_string", (ਖਾਲੀ **)&dll_scheme_display_ਮੰਜ਼ਿਲ_string},
    {"scheme_do_eval", (ਖਾਲੀ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (ਖਾਲੀ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (ਖਾਲੀ **)&dll_scheme_eq},
    {"scheme_eval", (ਖਾਲੀ **)&dll_scheme_eval},
    {"scheme_eval_string", (ਖਾਲੀ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (ਖਾਲੀ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(ਖਾਲੀ **)&dll_scheme_ਅੰਤ_primitive_module},
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
    {"scheme_format", (ਖਾਲੀ **)&dll_scheme_format},
# ਨਹੀਂਤਾਂ
    {"scheme_format_utf8", (ਖਾਲੀ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (ਖਾਲੀ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (ਖਾਲੀ **)&dll_scheme_gc_ptr_ok},
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
    {"scheme_get_sized_string_output",
	(ਖਾਲੀ **)&dll_scheme_get_sized_string_output},
# ਨਹੀਂਤਾਂ
    {"scheme_get_sized_byte_string_output",
	(ਖਾਲੀ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (ਖਾਲੀ **)&dll_scheme_intern_ਚਿੰਨ੍ਹ},
    {"scheme_lookup_global", (ਖਾਲੀ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(ਖਾਲੀ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (ਖਾਲੀ **)&dll_scheme_make_integer_ਮੁੱਲ},
    {"scheme_make_pair", (ਖਾਲੀ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (ਖਾਲੀ **)&dll_scheme_make_prim_w_arity},
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
    {"scheme_make_string", (ਖਾਲੀ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(ਖਾਲੀ **)&dll_scheme_make_string_output_ਪੋਰਟ},
# ਨਹੀਂਤਾਂ
    {"scheme_make_byte_string", (ਖਾਲੀ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(ਖਾਲੀ **)&dll_scheme_make_byte_string_output_ਪੋਰਟ},
# endif
    {"scheme_make_struct_instance",
	(ਖਾਲੀ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (ਖਾਲੀ **)&dll_scheme_make_struct_ਨਾਂ},
    {"scheme_make_struct_type", (ਖਾਲੀ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (ਖਾਲੀ **)&dll_scheme_make_struct_ਮੁੱਲਾਂ},
    {"scheme_make_type", (ਖਾਲੀ **)&dll_scheme_make_type},
    {"scheme_make_vector", (ਖਾਲੀ **)&dll_scheme_make_ਸਦਿਸ਼},
    {"scheme_malloc_fail_ok", (ਖਾਲੀ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (ਖਾਲੀ **)&dll_scheme_open_ਨਿਵੇਸ਼_file},
    {"scheme_primitive_module", (ਖਾਲੀ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (ਖਾਲੀ **)&dll_scheme_proper_list_ਲੰਮਾਈ},
    {"scheme_raise", (ਖਾਲੀ **)&dll_scheme_raise},
    {"scheme_read", (ਖਾਲੀ **)&dll_scheme_read},
    {"scheme_register_static", (ਖਾਲੀ **)&dll_scheme_ਰਜਿਸਟਰ_static},
    {"scheme_set_stack_base", (ਖਾਲੀ **)&dll_scheme_ਸੈੱਟ_ਢੇਰ_ਅਧਾਰ},
    {"scheme_signal_error", (ਖਾਲੀ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (ਖਾਲੀ **)&dll_scheme_wrong_type},
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
    {"scheme_set_param", (ਖਾਲੀ **)&dll_scheme_ਸੈੱਟ_param},
    {"scheme_current_config", (ਖਾਲੀ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(ਖਾਲੀ **)&dll_scheme_char_string_ਮੰਜ਼ਿਲ_byte_string},
    {"scheme_char_string_to_path", (ਖਾਲੀ **)&dll_scheme_char_string_ਮੰਜ਼ਿਲ_ਰਸਤਾ},
    {"scheme_set_collects_path", (ਖਾਲੀ **)&dll_scheme_ਸੈੱਟ_collects_ਰਸਤਾ},
# endif
    {"scheme_make_hash_table", (ਖਾਲੀ **)&dll_scheme_make_hash_ਸਾਰਣੀ},
    {"scheme_hash_set", (ਖਾਲੀ **)&dll_scheme_hash_ਸੈੱਟ},
    {"scheme_hash_get", (ਖਾਲੀ **)&dll_scheme_hash_get},
    {"scheme_make_double", (ਖਾਲੀ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (ਖਾਲੀ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (ਖਾਲੀ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (ਖਾਲੀ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (ਖਾਲੀ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (ਖਾਲੀ **)&dll_GC_ਰਜਿਸਟਰ_traversers},
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (ਖਾਲੀ **)&dll_scheme_main_ਤਿਆਰੀ},
#   ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 603
    {"scheme_register_tls_space", (ਖਾਲੀ **)&dll_scheme_ਰਜਿਸਟਰ_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (ਖਾਲੀ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (ਖਾਲੀ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (ਖਾਲੀ **)&dll_scheme_free_immobile_box},
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 500
#  ਜੇ defined(IMPLEMENT_THREAD_LOCAL_ਰਾਹੀਂ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ਰਾਹੀਂ_PROC)
    {"scheme_external_get_thread_local_variables", (ਖਾਲੀ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 600
    {"scheme_embedded_load", (ਖਾਲੀ **)&dll_scheme_embedded_ਲੱਦੋ},
    {"scheme_register_embedded_load", (ਖਾਲੀ **)&dll_scheme_ਰਜਿਸਟਰ_embedded_ਲੱਦੋ},
    {"scheme_set_config_path", (ਖਾਲੀ **)&dll_scheme_ਸੈੱਟ_config_ਰਸਤਾ},
# endif
    {ਨਿਰਾ, ਨਿਰਾ}};

ਸਥਿਰ HINSTANCE hMzGC = 0;
ਸਥਿਰ HINSTANCE hMzSch = 0;

ਸਥਿਰ ਖਾਲੀ dynamic_mzscheme_ਸਿਰਾ(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ mzscheme_runtime_link_init(ਅੱਖਰ *sch_dll, ਅੱਖਰ *gc_dll,
	ਪੂਰਨ verbose);

    ਸਥਿਰ ਪੂਰਨ
mzscheme_runtime_link_init(ਅੱਖਰ *sch_dll, ਅੱਖਰ *gc_dll, ਪੂਰਨ verbose)
{
    Thunk_Info *thunk = ਨਿਰਾ;

    ਜੇ (hMzGC && hMzSch)
	ਵਾਪਸ OK;
    hMzSch = vim_ਲੱਦੋ_Lib(sch_dll);
    hMzGC = vim_ਲੱਦੋ_Lib(gc_dll);

    ਜੇ (!hMzGC)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), gc_dll, GetWin32Error());
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!hMzSch)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), sch_dll, GetWin32Error());
	ਵਾਪਸ FAIL;
    }

    ਲਈ (thunk = mzsch_imports; thunk->name; thunk++)
    {
	ਜੇ ((*thunk->ptr =
		    (ਖਾਲੀ *)Get_Proc_ਪਤਾ(hMzSch, thunk->name)) == ਨਿਰਾ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), thunk->name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਲਈ (thunk = mzgc_imports; thunk->name; thunk++)
    {
	ਜੇ ((*thunk->ptr =
		    (ਖਾਲੀ *)Get_Proc_ਪਤਾ(hMzGC, thunk->name)) == ਨਿਰਾ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), thunk->name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
mzscheme_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ mzscheme_runtime_link_init(
	    (ਅੱਖਰ *)p_mzschemedll, (ਅੱਖਰ *)p_mzschemegcdll, verbose) == OK;
}

    ਸਥਿਰ ਖਾਲੀ
dynamic_mzscheme_ਸਿਰਾ(ਖਾਲੀ)
{
    ਜੇ (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    ਜੇ (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#ਜੇ_ਅੰਤ // DYNAMIC_MZSCHEME

#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, ਅੰਕ) GUARANTEE_STRING(proc, ਅੰਕ)
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ Scheme_Object *
guaranteed_byte_string_arg(ਅੱਖਰ *proc, ਪੂਰਨ ਅੰਕ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    ਜੇ (SCHEME_BYTE_STRINGP(argv[ਅੰਕ]))
    {
	ਵਾਪਸ argv[ਅੰਕ];
    }
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_CHAR_STRINGP(argv[ਅੰਕ]))
    {
	Scheme_Object *tmp = ਨਿਰਾ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, argv[ਅੰਕ]);
	MZ_GC_VAR_ਵਿੱਚ_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_ਮੰਜ਼ਿਲ_byte_string(argv[ਅੰਕ]);
	MZ_GC_UNREG();
	ਵਾਪਸ tmp;
    }
    ਨਹੀਂਤਾਂ
	scheme_wrong_type(proc, "string", ਅੰਕ, argc, argv);
    // unreachable
    ਵਾਪਸ scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, ਅੰਕ) guaranteed_byte_string_arg(proc, ਅੰਕ, argc, argv)
#ਜੇ_ਅੰਤ

// need to put it here for dynamic stuff to work
#ਜੇ defined(INCLUDE_MZSCHEME_ਅਧਾਰ)
# include "mzscheme_base.c"
#ਜੇ_ਅੰਤ

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ਸਥਿਰ Scheme_Type mz_ਬਫਰ_type;
ਸਥਿਰ Scheme_Type mz_ਖਿੜਕੀ_type;

ਸਥਿਰ ਪੂਰਨ initialized = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_MZSCHEME
ਸਥਿਰ ਪੂਰਨ disabled = FALSE;
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ਲੱਦੋ_ਅਧਾਰ_module_failed = FALSE;

// global environment
ਸਥਿਰ Scheme_Env    *environment = ਨਿਰਾ;
// output/error handlers
ਸਥਿਰ Scheme_Object *curout = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *curerr = ਨਿਰਾ;
// exn:vim exception
ਸਥਿਰ Scheme_Object *exn_catching_apply = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *exn_p = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *exn_ਸੁਨੇਹਾ = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *vim_exn = ਨਿਰਾ; // Vim Error exception

#ਜੇ !defined(MZ_PRECISE_GC) || MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 400
ਸਥਿਰ ਖਾਲੀ *ਢੇਰ_ਅਧਾਰ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਲੰਮਾ range_ਸ਼ੁਰੂ;
ਸਥਿਰ ਲੰਮਾ range_ਸਿਰਾ;

// MzScheme threads scheduling stuff
ਸਥਿਰ ਪੂਰਨ mz_ਧਾਗੇ_allow = 0;

#ਜੇ defined(FEAT_GUI_MSWIN)
ਸਥਿਰ ਖਾਲੀ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ਸਥਿਰ UINT timer_id = 0;
#ਜਾਂਜੇ defined(FEAT_GUI_GTK)
ਸਥਿਰ gboolean timer_proc(gpointer);
ਸਥਿਰ guint timer_id = 0;
#ਜਾਂਜੇ defined(FEAT_GUI_MOTIF)
ਸਥਿਰ ਖਾਲੀ timer_proc(Xt_ਸੂਚਕ, XtIntervalId *);
ਸਥਿਰ XtIntervalId timer_id = (XtIntervalId)0;
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    ਖਾਲੀ
mzvim_check_ਧਾਗੇ(ਖਾਲੀ)
{
    // Last time MzScheme threads were scheduled
    ਸਥਿਰ time_t mz_ਆਖਰੀ_ਸਮਾਂ = 0;

    ਜੇ (mz_ਧਾਗੇ_allow && p_mzq > 0)
    {
	time_t now = ਸਮਾਂ(ਨਿਰਾ);

	ਜੇ ((now - mz_ਆਖਰੀ_ਸਮਾਂ) * 1000 > p_mzq)
	{
	    mz_ਆਖਰੀ_ਸਮਾਂ = now;
	    scheme_check_ਧਾਗੇ();
	}
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MZSCHEME_GUI_ਧਾਗੇ)
ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_timer(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ remove_timer(ਖਾਲੀ);

// timers are presented in GUI only
# ਜੇ defined(FEAT_GUI_MSWIN)
    ਸਥਿਰ ਖਾਲੀ CALLBACK
timer_proc(
    HWND	hwnd ਅਣਵਰਤਿਆ,
    UINT	u_ਸੁਨੇਹਾ ਅਣਵਰਤਿਆ,
    UINT_PTR	idEvent ਅਣਵਰਤਿਆ,
    DWORD	dw_ਸਮਾਂ ਅਣਵਰਤਿਆ)
# elif defined(FEAT_GUI_GTK)
    ਸਥਿਰ gboolean
timer_proc(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
# elif defined(FEAT_GUI_MOTIF)
    ਸਥਿਰ ਖਾਲੀ
timer_proc(Xt_ਸੂਚਕ timed_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ, XtIntervalId *interval_id ਅਣਵਰਤਿਆ)
# endif
{
    scheme_check_ਧਾਗੇ();
# ਜੇ defined(FEAT_GUI_GTK)
    ਵਾਪਸ TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    ਜੇ (mz_ਧਾਗੇ_allow && p_mzq > 0)
	timer_id = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, p_mzq,
		timer_proc, ਨਿਰਾ);
# endif
}

    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_timer(ਖਾਲੀ)
{
# ਜੇ defined(FEAT_GUI_MSWIN)
    timer_id = ਸੈੱਟ_Timer(ਨਿਰਾ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_ਜੋੜੋ((guint)p_mzq, (GSourceFunc)timer_proc, ਨਿਰਾ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, p_mzq, timer_proc, ਨਿਰਾ);
# endif
}

    ਸਥਿਰ ਖਾਲੀ
remove_timer(ਖਾਲੀ)
{
# ਜੇ defined(FEAT_GUI_MSWIN)
    KillTimer(ਨਿਰਾ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_ਸਰੋਤ_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(timer_id);
# endif
    timer_id = 0;
}

#ਜੇ_ਅੰਤ // MZSCHEME_GUI_THREADS

    ਅੱਖਰ *
did_ਸੈੱਟ_mzquantum(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(MZSCHEME_GUI_ਧਾਗੇ)
    // reset timer
    ਜੇ (timer_id != 0)
	remove_timer();
    ਜੇ (mz_ਧਾਗੇ_allow && p_mzq > 0 && gui.ਵਿੱਚ_use)
	ਤਿਆਰੀ_timer();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
notify_multithread(ਪੂਰਨ on)
{
    mz_ਧਾਗੇ_allow = on;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZSCHEME_GUI_ਧਾਗੇ
    ਜੇ (on && timer_id == 0 && p_mzq > 0 && gui.ਵਿੱਚ_use)
	ਤਿਆਰੀ_timer();
    ਜੇ (!on && timer_id != 0)
	remove_timer();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
mzscheme_ਸਿਰਾ(ਖਾਲੀ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#ਜੇ 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_ਸਿਰਾ();
# endif
#ਜੇ_ਅੰਤ
}

#ਜੇ HAVE_TLS_SPACE && !defined(VIMDLL)
# ਜੇ defined(_MSC_VER)
ਸਥਿਰ __declspec(thread) ਖਾਲੀ *tls_space;
ਬਾਹਰੀ intptr_t tls_ਸੂਚਕਾਂਕ;
# elif defined(__MINGW32__)
ਸਥਿਰ __thread ਖਾਲੀ *tls_space;
ਬਾਹਰੀ intptr_t tls_ਸੂਚਕਾਂਕ;
# ਨਹੀਂਤਾਂ
ਸਥਿਰ THREAD_LOCAL ਖਾਲੀ *tls_space;
ਸਥਿਰ intptr_t tls_ਸੂਚਕਾਂਕ = 0;
# endif
#ਜੇ_ਅੰਤ

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    ਪੂਰਨ
mzscheme_main(ਖਾਲੀ)
{
    ਪੂਰਨ	    argc = 0;
    ਅੱਖਰ    *argv = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    ਜੇ (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	ਵਾਪਸ vim_main2();
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TLS_SPACE
# ifdef VIMDLL
    ਖਾਲੀ **ptls_space;
    intptr_t tls_ਸੂਚਕਾਂਕ_2;
    ਖਾਲੀ (*pget_tls_info)(ਖਾਲੀ ***ptls_space, intptr_t *ptls_ਸੂਚਕਾਂਕ);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (ਖਾਲੀ *)Get_Proc_ਪਤਾ(
				GetModuleHandle(ਨਿਰਾ), "get_tls_info");
    ਜੇ (pget_tls_info == ਨਿਰਾ)
    {
	disabled = TRUE;
	ਵਾਪਸ vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_ਸੂਚਕਾਂਕ_2);
    scheme_ਰਜਿਸਟਰ_tls_space(ptls_space, tls_ਸੂਚਕਾਂਕ_2);
# ਨਹੀਂਤਾਂ
    scheme_ਰਜਿਸਟਰ_tls_space(&tls_space, tls_ਸੂਚਕਾਂਕ);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRAMPOLINED_MZVIM_STARTUP
    ਵਾਪਸ scheme_main_ਤਿਆਰੀ(TRUE, mzscheme_env_main, argc, &argv);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ mzscheme_env_main(ਨਿਰਾ, argc, &argv);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਪੂਰਨ
mzscheme_env_main(Scheme_Env *env, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, ਅੱਖਰ **argv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#ਨਹੀਂਤਾਂ
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ਨਿਰਾ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, dummy);

    ਢੇਰ_ਅਧਾਰ = &__gc_var_stack__;
# ਨਹੀਂਤਾਂ
    ਪੂਰਨ dummy = 0;
    ਢੇਰ_ਅਧਾਰ = (ਖਾਲੀ *)&dummy;
# endif
#ਜੇ_ਅੰਤ

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    ਵਾਪਸ 0;
}

    ਸਥਿਰ Scheme_Object*
ਲੱਦੋ_ਅਧਾਰ_module(ਖਾਲੀ *ਅੰਕੜੇ)
{
    scheme_namespace_require(scheme_intern_ਚਿੰਨ੍ਹ((ਅੱਖਰ *)ਅੰਕੜੇ));
    ਵਾਪਸ scheme_null;
}

    ਸਥਿਰ Scheme_Object *
ਲੱਦੋ_ਅਧਾਰ_module_on_error(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਲੱਦੋ_ਅਧਾਰ_module_failed = TRUE;
    ਵਾਪਸ scheme_null;
}

    ਸਥਿਰ ਪੂਰਨ
startup_mzscheme(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TRAMPOLINED_MZVIM_STARTUP
    scheme_ਸੈੱਟ_ਢੇਰ_ਅਧਾਰ(ਢੇਰ_ਅਧਾਰ, 1);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#ਜੇ_ਅੰਤ

    MZ_ਰਜਿਸਟਰ_STATIC(environment);
    MZ_ਰਜਿਸਟਰ_STATIC(curout);
    MZ_ਰਜਿਸਟਰ_STATIC(curerr);
    MZ_ਰਜਿਸਟਰ_STATIC(exn_catching_apply);
    MZ_ਰਜਿਸਟਰ_STATIC(exn_p);
    MZ_ਰਜਿਸਟਰ_STATIC(exn_ਸੁਨੇਹਾ);
    MZ_ਰਜਿਸਟਰ_STATIC(vim_exn);

    MZ_GC_CHECK();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ INCLUDE_MZSCHEME_ਅਧਾਰ
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#ਜੇ_ਅੰਤ

    // setup 'current-library-collection-paths' parameter
#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
    {
	Scheme_Object *coll_ਰਸਤਾ = ਨਿਰਾ;
	ਪੂਰਨ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, coll_ਰਸਤਾ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	ਜੇ (s != ਨਿਰਾ)
	{
	    coll_ਰਸਤਾ = scheme_make_ਰਸਤਾ((ਅੱਖਰ *)s);
	    MZ_GC_CHECK();
	    ਜੇ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	ਜੇ (coll_ਰਸਤਾ == ਨਿਰਾ)
	{
	    coll_ਰਸਤਾ = scheme_make_ਰਸਤਾ(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	ਜੇ (coll_ਰਸਤਾ != ਨਿਰਾ)
	{
	    scheme_ਸੈੱਟ_collects_ਰਸਤਾ(coll_ਰਸਤਾ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ਨਹੀਂਤਾਂ
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ਨਿਰਾ;
	Scheme_Object	*coll_pair = ਨਿਰਾ;
	Scheme_Config	*config = ਨਿਰਾ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, coll_string);
	MZ_GC_VAR_ਵਿੱਚ_REG(1, coll_pair);
	MZ_GC_VAR_ਵਿੱਚ_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_ਸੈੱਟ_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#ਜੇ_ਅੰਤ

#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 600
    {
	Scheme_Object *config_ਰਸਤਾ = ਨਿਰਾ;
	ਪੂਰਨ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, config_ਰਸਤਾ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	ਜੇ (s != ਨਿਰਾ)
	{
	    config_ਰਸਤਾ = scheme_make_ਰਸਤਾ((ਅੱਖਰ *)s);
	    MZ_GC_CHECK();
	    ਜੇ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	ਜੇ (config_ਰਸਤਾ == ਨਿਰਾ)
	{
	    config_ਰਸਤਾ = scheme_make_ਰਸਤਾ(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	ਜੇ (config_ਰਸਤਾ != ਨਿਰਾ)
	{
	    scheme_ਸੈੱਟ_config_ਰਸਤਾ(config_ਰਸਤਾ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ਜੇ_ਅੰਤ

#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#ਜੇ_ਅੰਤ

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ਨਿਰਾ, ਲੱਦੋ_ਅਧਾਰ_module, ਨਿਰਾ,
				    ਲੱਦੋ_ਅਧਾਰ_module_on_error, "racket/base");
	ਜੇ (ਲੱਦੋ_ਅਧਾਰ_module_failed)
	{
	    ਲੱਦੋ_ਅਧਾਰ_module_failed = FALSE;
	    scheme_dynamic_wind(ਨਿਰਾ, ਲੱਦੋ_ਅਧਾਰ_module, ਨਿਰਾ,
				    ਲੱਦੋ_ਅਧਾਰ_module_on_error, "scheme/base");
	    ਜੇ (ਲੱਦੋ_ਅਧਾਰ_module_failed)
		ਵਾਪਸ -1;
	}
    }

    ਰਜਿਸਟਰ_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = ਨਿਰਾ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, make_security_guard);
	MZ_GC_REG();

# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_ਚਿੰਨ੍ਹ = ਨਿਰਾ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ਵਿੱਚ_REG(0, make_security_guard_ਚਿੰਨ੍ਹ);
	    MZ_GC_REG();
	    make_security_guard_ਚਿੰਨ੍ਹ = scheme_intern_ਚਿੰਨ੍ਹ("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_ਚਿੰਨ੍ਹ, environment);
	    MZ_GC_UNREG();
	}
# ਨਹੀਂਤਾਂ
	make_security_guard = scheme_builtin_ਮੁੱਲ("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	ਜੇ (make_security_guard != ਨਿਰਾ)
	{
	    Scheme_Object   *ਦਲੀਲਾਂ[3] = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
	    Scheme_Object   *guard = ਨਿਰਾ;
	    Scheme_Config   *config = ਨਿਰਾ;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG(0, ਦਲੀਲਾਂ, 3);
	    MZ_GC_VAR_ਵਿੱਚ_REG(3, guard);
	    MZ_GC_VAR_ਵਿੱਚ_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    ਦਲੀਲਾਂ[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    ਦਲੀਲਾਂ[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    ਦਲੀਲਾਂ[2] = scheme_make_prim_w_arity(sandbox_ਜਾਲ_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, ਦਲੀਲਾਂ);
	    MZ_GC_CHECK();
	    scheme_ਸੈੱਟ_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#ਜੇ_ਅੰਤ
    // Create buffer and window types for use in Scheme code
    mz_ਬਫਰ_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_ਖਿੜਕੀ_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
    GC_ਰਜਿਸਟਰ_traversers(mz_ਬਫਰ_type,
	    ਬਫਰ_ਮਾਪ_proc, ਬਫਰ_mark_proc, ਬਫਰ_fixup_proc,
	    TRUE, TRUE);
    GC_ਰਜਿਸਟਰ_traversers(mz_ਖਿੜਕੀ_type,
	    ਖਿੜਕੀ_ਮਾਪ_proc, ਖਿੜਕੀ_mark_proc, ਖਿੜਕੀ_fixup_proc,
	    TRUE, TRUE);
#ਜੇ_ਅੰਤ

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    ਵਾਪਸ 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ਸਥਿਰ ਪੂਰਨ
mzscheme_init(ਖਾਲੀ)
{
    ਜੇ (!initialized)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_MZSCHEME
	ਜੇ (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    ਵਾਪਸ -1;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (ਲੱਦੋ_ਅਧਾਰ_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_ਅਧਾਰ_module_could_not_be_loaded));
	    ਵਾਪਸ -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ਨਿਰਾ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_ਪੋਰਟ();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_ਪੋਰਟ();
	MZ_GC_CHECK();
	scheme_ਸੈੱਟ_param(config, MZCONFIG_OUTPUT_ਪੋਰਟ, curout);
	MZ_GC_CHECK();
	scheme_ਸੈੱਟ_param(config, MZCONFIG_ERROR_ਪੋਰਟ, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    ਵਾਪਸ 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ਸਥਿਰ ਪੂਰਨ
eval_with_exn_handling(
    ਖਾਲੀ		*ਅੰਕੜੇ,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *ਮੁੱਲ = ਨਿਰਾ;
    Scheme_Object   *exn = ਨਿਰਾ;
    Scheme_Object   *prim = ਨਿਰਾ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਮੁੱਲ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, exn);
    MZ_GC_VAR_ਵਿੱਚ_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, ਅੰਕੜੇ, "mzvim", 0, 0);
    MZ_GC_CHECK();
    ਮੁੱਲ = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    ਜੇ (!ਮੁੱਲ)
    {
	ਮੁੱਲ = ਕੱਢੋ_exn_ਸੁਨੇਹਾ(exn);
	// Got an exn?
	ਜੇ (ਮੁੱਲ)
	{
	    scheme_display(ਮੁੱਲ, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	ਵਾਪਸ FAIL;
    }

    ਜੇ (ret != ਨਿਰਾ)	// if pointer to retval supported give it up
	*ret = ਮੁੱਲ;
    // Print any result, as long as it's not a void
    ਨਹੀਂਤਾਂ ਜੇ (!SCHEME_VOIDP(ਮੁੱਲ))
    {
	scheme_display(ਮੁੱਲ, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    ਵਾਪਸ OK;
}

/*
 * :mzscheme
 */
    ਸਥਿਰ ਪੂਰਨ
do_mzscheme_command(exarg_T *eap, ਖਾਲੀ *ਅੰਕੜੇ, Scheme_Closed_Prim *what)
{
    ਜੇ (mzscheme_init())
	ਵਾਪਸ FAIL;

    range_ਸ਼ੁਰੂ = eap->line1;
    range_ਸਿਰਾ = eap->line2;

    ਵਾਪਸ eval_with_exn_handling(ਅੰਕੜੇ, what, ਨਿਰਾ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    ਖਾਲੀ
mzscheme_ਬਫਰ_free(buf_T *buf)
{
    ਜੇ (buf->b_mzscheme_ref == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_mz_ਬਫਰ *bp = ਨਿਰਾ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, bp);
    MZ_GC_REG();

    bp = ਬਫਰ_REF(buf);
    bp->buf = INVALID_ਬਫਰ_ਮੁੱਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#ਨਹੀਂਤਾਂ
    scheme_gc_ptr_ok(bp);
#ਜੇ_ਅੰਤ
    buf->b_mzscheme_ref = ਨਿਰਾ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    ਖਾਲੀ
mzscheme_ਖਿੜਕੀ_free(win_T *win)
{
    ਜੇ (win->w_mzscheme_ref == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_mz_ਖਿੜਕੀ *wp = ਨਿਰਾ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, wp);
    MZ_GC_REG();
    wp = ਖਿੜਕੀ_REF(win);
    wp->win = INVALID_ਖਿੜਕੀ_ਮੁੱਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#ਨਹੀਂਤਾਂ
    scheme_gc_ptr_ok(wp);
#ਜੇ_ਅੰਤ
    win->w_mzscheme_ref = ਨਿਰਾ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    ਖਾਲੀ
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਜੇ (script == ਨਿਰਾ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    ਨਹੀਂਤਾਂ
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ਸਥਿਰ Scheme_Object *
do_ਲੱਦੋ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ noargc ਅਣਵਰਤਿਆ, Scheme_Object **noargv ਅਣਵਰਤਿਆ)
{
    Scheme_Object   *ਵਿਅੰਜਕ = ਨਿਰਾ;
    Scheme_Object   *ਨਤੀਜਾ = ਨਿਰਾ;
    ਅੱਖਰ	    *file = ਨਿਰਾ;
    ਪੋਰਟ_Info	    *pinfo = (ਪੋਰਟ_Info *)ਅੰਕੜੇ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਵਿਅੰਜਕ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਨਤੀਜਾ);
    MZ_GC_VAR_ਵਿੱਚ_REG(2, file);
    MZ_GC_REG();

    file = (ਅੱਖਰ *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->ਪੋਰਟ = scheme_open_ਨਿਵੇਸ਼_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ਗਿਣਤੀ_ਸਤਰਾਂ(pinfo->ਪੋਰਟ);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    ਜਦੋਂ (!SCHEME_EOFP(ਵਿਅੰਜਕ = scheme_read(pinfo->ਪੋਰਟ)))
    {
	ਨਤੀਜਾ = scheme_eval(ਵਿਅੰਜਕ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ(pinfo->ਪੋਰਟ);
    MZ_GC_CHECK();
    pinfo->ਪੋਰਟ = ਨਿਰਾ;
    MZ_GC_UNREG();
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * :mzfile
 */
    ਖਾਲੀ
ex_mzfile(exarg_T *eap)
{
    ਪੋਰਟ_Info	pinfo = {ਨਿਰਾ, ਨਿਰਾ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, pinfo.ਪੋਰਟ);
    MZ_GC_REG();

    pinfo.name = (ਅੱਖਰ *)eap->arg;
    ਜੇ (do_mzscheme_command(eap, &pinfo, do_ਲੱਦੋ) != OK
	    && pinfo.ਪੋਰਟ != ਨਿਰਾ)	// looks like port was not closed
    {
	scheme_close_ਨਿਵੇਸ਼_ਪੋਰਟ(pinfo.ਪੋਰਟ);
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
    ਸਥਿਰ ਖਾਲੀ
init_exn_catching_apply(ਖਾਲੀ)
{
    ਜੇ (exn_catching_apply)
	ਵਾਪਸ;

    ਅੱਖਰ *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_ਮੁੱਲ("exn?");
    MZ_GC_CHECK();
    exn_ਸੁਨੇਹਾ = scheme_builtin_ਮੁੱਲ("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ਸਥਿਰ Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    ਜੇ (SCHEME_TRUEP(SCHEME_CAR(v)))
	ਵਾਪਸ SCHEME_CDR(v);
    ਨਹੀਂਤਾਂ
    {
	*exn = SCHEME_CDR(v);
	ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ Scheme_Object *
ਕੱਢੋ_exn_ਸੁਨੇਹਾ(Scheme_Object *v)
{
    ਜੇ (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	ਵਾਪਸ _scheme_apply(exn_ਸੁਨੇਹਾ, 1, &v);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ; // Not an exn structure
}

    ਸਥਿਰ Scheme_Object *
do_eval(ਖਾਲੀ *s, ਪੂਰਨ noargc ਅਣਵਰਤਿਆ, Scheme_Object **noargv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ scheme_eval_string_all((ਅੱਖਰ *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ਸਥਿਰ ਖਾਲੀ
do_intrnl_output(ਅੱਖਰ *mesg, ਪੂਰਨ error)
{
    ਅੱਖਰ *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    ਜਦੋਂ (p)
    {
	*p = '\0';
	ਜੇ (error)
	    emsg(prev);
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    ਜੇ (error)
	emsg(prev);
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ(prev);
}

    ਸਥਿਰ ਖਾਲੀ
do_output(ਅੱਖਰ *mesg, OUTPUT_LEN_TYPE len ਅਣਵਰਤਿਆ)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ਸਥਿਰ ਖਾਲੀ
do_err_output(ਅੱਖਰ *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ਸਥਿਰ ਖਾਲੀ
do_printf(ਅੱਖਰ *format, ...)
{
    do_intrnl_output(format, 1);
}

    ਸਥਿਰ ਖਾਲੀ
do_flush(ਖਾਲੀ)
{
    ਅੱਖਰ *buff;
    OUTPUT_LEN_TYPE ਲੰਮਾਈ;

    buff = scheme_get_sized_byte_string_output(curerr, &ਲੰਮਾਈ);
    MZ_GC_CHECK();
    ਜੇ (ਲੰਮਾਈ)
    {
	do_err_output(buff);
	ਵਾਪਸ;
    }

    buff = scheme_get_sized_byte_string_output(curout, &ਲੰਮਾਈ);
    MZ_GC_CHECK();
    ਜੇ (ਲੰਮਾਈ)
	do_output(buff, ਲੰਮਾਈ);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    ਸਥਿਰ Scheme_Object *
vim_command(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    Scheme_Object *cmd = ਨਿਰਾ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_ਮੁੱਲ(cmd), ਨਿਰਾ, ਨਿਰਾ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    ਵਾਪਸ scheme_void;
}

/*
 * (eval {expr-string})
 */
    ਸਥਿਰ Scheme_Object *
vim_eval(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)ਅੰਕੜੇ;
    Scheme_Object	*ਨਤੀਜਾ = ਨਿਰਾ;
    typval_T		*vim_ਨਤੀਜਾ;
    Scheme_Object *ਵਿਅੰਜਕ = ਨਿਰਾ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਨਤੀਜਾ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਵਿਅੰਜਕ);
    MZ_GC_REG();
    ਵਿਅੰਜਕ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_ਨਤੀਜਾ = eval_ਵਿਅੰਜਕ(BYTE_STRING_ਮੁੱਲ(ਵਿਅੰਜਕ), ਨਿਰਾ);

    ਜੇ (vim_ਨਤੀਜਾ == ਨਿਰਾ)
	raise_vim_exn(_("invalid expression"));

    ਨਤੀਜਾ = vim_ਮੰਜ਼ਿਲ_mzscheme(vim_ਨਤੀਜਾ);
    MZ_GC_CHECK();
    free_tv(vim_ਨਤੀਜਾ);

    MZ_GC_UNREG();
    ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    ਵਾਪਸ scheme_false;
#ਜੇ_ਅੰਤ
}

/*
 * (range-start)
 */
    ਸਥਿਰ Scheme_Object *
get_range_ਸ਼ੁਰੂ(
    ਖਾਲੀ		*ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ			argc ਅਣਵਰਤਿਆ,
    Scheme_Object	**argv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ scheme_make_integer(range_ਸ਼ੁਰੂ);
}

/*
 * (range-end)
 */
    ਸਥਿਰ Scheme_Object *
get_range_ਸਿਰਾ(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ scheme_make_integer(range_ਸਿਰਾ);
}

/*
 * (beep)
 */
    ਸਥਿਰ Scheme_Object *
mzscheme_beep(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
    vim_beep(BO_LANG);
    ਵਾਪਸ scheme_void;
}

ਸਥਿਰ Scheme_Object *M_global = ਨਿਰਾ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ਸਥਿਰ Scheme_Object *
get_option(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    ਲੰਮਾ	    ਮੁੱਲ;
    ਅੱਖਰ	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ਨਿਰਾ;
    Scheme_Object   *name = ਨਿਰਾ;
    ਪੂਰਨ		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, rval);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    ਜੇ (argc > 1)
    {
	ਜੇ (M_global == ਨਿਰਾ)
	{
	    MZ_ਰਜਿਸਟਰ_STATIC(M_global);
	    M_global = scheme_intern_ਚਿੰਨ੍ਹ("global");
	    MZ_GC_CHECK();
	}

	ਜੇ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_ਬਫਰ(argv[1]);
	    scope = OPT_LOCAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_ਖਿੜਕੀ(argv[1]);

	    curwin = win;
	    curbuf = win->w_ਬਫਰ;
	    scope = OPT_LOCAL;
	}
	ਨਹੀਂਤਾਂ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_ਮੁੱਲ(BYTE_STRING_ਮੁੱਲ(name), &ਮੁੱਲ, (char_u **)&strval,
								  ਨਿਰਾ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    ਚੋਣ (rc)
    {
    ਹਾਲਤ gov_bool:
    ਹਾਲਤ gov_ਸੰਖਿਆ:
	MZ_GC_UNREG();
	ਵਾਪਸ scheme_make_integer_ਮੁੱਲ(ਮੁੱਲ);
    ਹਾਲਤ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	ਵਾਪਸ rval;
    ਹਾਲਤ gov_hidden_bool:
    ਹਾਲਤ gov_hidden_ਸੰਖਿਆ:
    ਹਾਲਤ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    ਹਾਲਤ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    ਵਾਪਸ scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_option(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    char_u	*command = ਨਿਰਾ;
    ਪੂਰਨ		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    Scheme_Object *cmd = ਨਿਰਾ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    ਜੇ (argc > 1)
    {
	ਜੇ (M_global == ਨਿਰਾ)
	{
	    MZ_ਰਜਿਸਟਰ_STATIC(M_global);
	    M_global = scheme_intern_ਚਿੰਨ੍ਹ("global");
	    MZ_GC_CHECK();
	}

	ਜੇ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_ਬਫਰ(argv[1]);
	    scope = OPT_LOCAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_ਖਿੜਕੀ(argv[1]);
	    curwin = win;
	    curbuf = win->w_ਬਫਰ;
	    scope = OPT_LOCAL;
	}
	ਨਹੀਂਤਾਂ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_ਮੁੱਲ(cmd));
    MZ_GC_UNREG();
    do_ਸੈੱਟ(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    ਵਾਪਸ scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ਸਥਿਰ Scheme_Object *
get_curr_win(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ (Scheme_Object *)get_vim_curr_ਖਿੜਕੀ();
}

/*
 * (win-count)
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_ਗਿਣਤੀ(
    ਖਾਲੀ		*ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ			argc ਅਣਵਰਤਿਆ,
    Scheme_Object	**argv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    ਵਾਪਸ scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_list(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    ਲਈ ( ; w != ਨਿਰਾ; w = w->w_ਅਗਲਾ)
	ਜੇ (w->w_ਬਫਰ == buf->buf)
	{
	    list = scheme_make_pair(ਖਿੜਕੀ_new(w), list);
	    MZ_GC_CHECK();
	}

    ਵਾਪਸ list;
}

    ਸਥਿਰ Scheme_Object *
ਖਿੜਕੀ_new(win_T *win)
{
    vim_mz_ਖਿੜਕੀ *self = ਨਿਰਾ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    ਜੇ (win->w_mzscheme_ref != ਨਿਰਾ)
	ਵਾਪਸ (Scheme_Object *)ਖਿੜਕੀ_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ਆਕਾਰ(vim_mz_ਖਿੜਕੀ));
    CLEAR_ਸੂਚਕ(self);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#ਜੇ_ਅੰਤ
    MZ_GC_CHECK();
    ਖਿੜਕੀ_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_ਖਿੜਕੀ_type;

    MZ_GC_UNREG();
    ਵਾਪਸ (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_ਅੰਕ(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    win_T	*win = get_ਖਿੜਕੀ_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    ਲਈ (wp = firstwin; wp != win; wp = wp->w_ਅਗਲਾ)
	++nr;

    ਵਾਪਸ scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_by_ਅੰਕ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    win_T	*win = firstwin;
    ਪੂਰਨ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ਜੇ (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    ਲਈ ( ; win != ਨਿਰਾ; win = win->w_ਅਗਲਾ, --fnum)
	ਜੇ (fnum == 1)	    // to be 1-based
	    ਵਾਪਸ ਖਿੜਕੀ_new(win);

    ਵਾਪਸ scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_ਬਫਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win = get_ਖਿੜਕੀ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ ਬਫਰ_new(win->win->w_ਬਫਰ);
}

/*
 * (get-win-height [window])
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_ਉਚਾਈ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win = get_ਖਿੜਕੀ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ scheme_make_integer(win->win->w_ਉਚਾਈ);
}

/*
 * (set-win-height {height} [window])
 */
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_ਖਿੜਕੀ_ਉਚਾਈ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win;
    win_T	    *savewin;
    ਪੂਰਨ		    ਉਚਾਈ;

    win = get_ਖਿੜਕੀ_arg(prim->name, 1, argc, argv);
    ਉਚਾਈ = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    savewin = curwin;
    curwin = win->win;
    win_setheight(ਉਚਾਈ);
    curwin = savewin;

    raise_if_error();
    ਵਾਪਸ scheme_void;
}

/*
 * (get-win-width [window])
 */
    ਸਥਿਰ Scheme_Object *
get_ਖਿੜਕੀ_ਚੌੜਾਈ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win = get_ਖਿੜਕੀ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ scheme_make_integer(win->win->w_ਚੌੜਾਈ);
}

/*
 * (set-win-width {width} [window])
 */
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_ਖਿੜਕੀ_ਚੌੜਾਈ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win;
    win_T	    *savewin;
    ਪੂਰਨ		    ਚੌੜਾਈ = 0;

    win = get_ਖਿੜਕੀ_arg(prim->name, 1, argc, argv);
    ਚੌੜਾਈ = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    savewin = curwin;
    curwin = win->win;
    win_setwidth(ਚੌੜਾਈ);
    curwin = savewin;

    raise_if_error();
    ਵਾਪਸ scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ਸਥਿਰ Scheme_Object *
get_cursor(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win;
    pos_T	    pos;

    win = get_ਖਿੜਕੀ_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    ਵਾਪਸ scheme_make_pair(scheme_make_integer_ਮੁੱਲ((ਲੰਮਾ)pos.lnum),
		    scheme_make_integer_ਮੁੱਲ((ਲੰਮਾ)pos.ਖਾਨਾ + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_cursor(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਖਿੜਕੀ   *win;
    ਲੰਮਾ	    lnum = 0;
    ਲੰਮਾ	    ਖਾਨਾ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    sandbox_check();
#ਜੇ_ਅੰਤ
    win = get_ਖਿੜਕੀ_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    ਜੇ (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    ਖਾਨਾ = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_ਸਤਰ_range(lnum, win->win->w_ਬਫਰ);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
    win->win->w_ਸੈੱਟ_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    ਵਾਪਸ scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ਸਥਿਰ Scheme_Object *
mzscheme_open_ਬਫਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    ਪੂਰਨ		    ਅੰਕ = 0;
    Scheme_Object   *onum = ਨਿਰਾ;
    Scheme_Object   *buf = ਨਿਰਾ;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, onum);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, buf);
    MZ_GC_VAR_ਵਿੱਚ_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    sandbox_check();
#ਜੇ_ਅੰਤ
    // TODO make open existing file
    ਅੰਕ = buflist_ਜੋੜੋ(BYTE_STRING_ਮੁੱਲ(fname), BLN_LISTED | BLN_CURBUF);

    ਜੇ (ਅੰਕ == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(ਅੰਕ);
    buf = get_ਬਫਰ_by_ਅੰਕ(ਅੰਕੜੇ, 1, &onum);
    MZ_GC_UNREG();
    ਵਾਪਸ buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_by_ਅੰਕ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    buf_T	*buf;
    ਪੂਰਨ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	ਜੇ (buf->b_fnum == fnum)
	    ਵਾਪਸ ਬਫਰ_new(buf);

    ਵਾਪਸ scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_by_name(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    buf_T	*buf;
    Scheme_Object *ਬਫਰ = ਨਿਰਾ;
    Scheme_Object *fname = ਨਿਰਾ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਬਫਰ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    ਬਫਰ = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	ਜੇ (buf->b_ffname == ਨਿਰਾ || buf->b_sfname == ਨਿਰਾ)
	    // empty string
	{
	    ਜੇ (BYTE_STRING_ਮੁੱਲ(fname)[0] == NUL)
		ਬਫਰ = ਬਫਰ_new(buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (!fnamecmp(buf->b_ffname, BYTE_STRING_ਮੁੱਲ(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_ਮੁੱਲ(fname)))
	{
	    // either short or long filename matches
	    ਬਫਰ = ਬਫਰ_new(buf);
	}
    }

    MZ_GC_UNREG();
    ਵਾਪਸ ਬਫਰ;
}

/*
 * (get-next-buff [buffer])
 */
    ਸਥਿਰ Scheme_Object *
get_ਅਗਲਾ_ਬਫਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    buf_T	*buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv)->buf;

    ਜੇ (buf->b_ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ scheme_false;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਬਫਰ_new(buf->b_ਅਗਲਾ);
}

/*
 * (get-prev-buff [buffer])
 */
    ਸਥਿਰ Scheme_Object *
get_prev_ਬਫਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ਅੰਕੜੇ;
    buf_T	*buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv)->buf;

    ਜੇ (buf->b_prev == ਨਿਰਾ)
	ਵਾਪਸ scheme_false;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਬਫਰ_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_ਅੰਕ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_ਗਿਣਤੀ(
    ਖਾਲੀ		*ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ			argc ਅਣਵਰਤਿਆ,
    Scheme_Object	**argv ਅਣਵਰਤਿਆ)
{
    buf_T   *b;
    ਪੂਰਨ	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    ਵਾਪਸ scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_name(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ scheme_make_byte_string((ਅੱਖਰ *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ਸਥਿਰ Scheme_Object *
get_curr_ਬਫਰ(
    ਖਾਲੀ		*ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ			argc ਅਣਵਰਤਿਆ,
    Scheme_Object	**argv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ (Scheme_Object *)get_vim_curr_ਬਫਰ();
}

    ਸਥਿਰ Scheme_Object *
ਬਫਰ_new(buf_T *buf)
{
    vim_mz_ਬਫਰ *self = ਨਿਰਾ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    ਜੇ (buf->b_mzscheme_ref)
	ਵਾਪਸ (Scheme_Object *)ਬਫਰ_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ਆਕਾਰ(vim_mz_ਬਫਰ));
    CLEAR_ਸੂਚਕ(self);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#ਜੇ_ਅੰਤ
    MZ_GC_CHECK();
    ਬਫਰ_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_ਬਫਰ_type;

    MZ_GC_UNREG();
    ਵਾਪਸ (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_ਮਾਪ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf = get_ਬਫਰ_arg(prim->name, 0, argc, argv);

    ਵਾਪਸ scheme_make_integer(buf->buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_ਸਤਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf;
    ਪੂਰਨ		    linenr;
    char_u	    *ਸਤਰ;

    buf = get_ਬਫਰ_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ਸਤਰ = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    ਵਾਪਸ scheme_make_byte_string((ਅੱਖਰ *)ਸਤਰ);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ਸਥਿਰ Scheme_Object *
get_ਬਫਰ_ਸਤਰ_list(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf;
    ਪੂਰਨ		    i, hi, lo, n;
    Scheme_Object   *list = ਨਿਰਾ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, list);
    MZ_GC_REG();

    buf = get_ਬਫਰ_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    ਜੇ (lo < 0)
	lo = 0;

    ਜੇ (hi < 0)
	hi = 0;
    ਜੇ (hi < lo)
	hi = lo;

    n = hi - lo;

    ਲਈ (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (ਅੱਖਰ *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ਵਾਪਸ list;
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
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_ਬਫਰ_ਸਤਰ(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf;
    Scheme_Object   *ਸਤਰ = ਨਿਰਾ;
    ਅੱਖਰ	    *save;
    ਪੂਰਨ		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਸਤਰ);
    MZ_GC_REG();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    sandbox_check();
#ਜੇ_ਅੰਤ
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ਜੇ (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    ਸਤਰ = argv[1];
    buf = get_ਬਫਰ_arg(prim->name, 2, argc, argv);

    check_ਸਤਰ_range(n, buf->buf);

    ਜੇ (SCHEME_FALSEP(ਸਤਰ))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	ਜੇ (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	ਜੇ (buf->buf == curwin->w_ਬਫਰ)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_ਸਤਰਾਂ_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ਵਾਪਸ scheme_void;
    }
    ਨਹੀਂਤਾਂ
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ);

	curbuf = buf->buf;

	ਜੇ (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_free(save);
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	ਜੇ (buf->buf == curwin->w_ਬਫਰ)
	    check_cursor_ਖਾਨਾ();

	MZ_GC_UNREG();
	raise_if_error();
	ਵਾਪਸ scheme_void;
    }
}

    ਸਥਿਰ ਖਾਲੀ
free_array(ਅੱਖਰ **array)
{
    ਅੱਖਰ **curr = array;
    ਜਦੋਂ (*curr != ਨਿਰਾ)
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
    ਸਥਿਰ Scheme_Object *
ਸੈੱਟ_ਬਫਰ_ਸਤਰ_list(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf = ਨਿਰਾ;
    Scheme_Object   *ਸਤਰ_list = ਨਿਰਾ;
    ਪੂਰਨ		    i, old_len, new_len, hi, lo;
    ਲੰਮਾ	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਸਤਰ_list);
    MZ_GC_REG();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    sandbox_check();
#ਜੇ_ਅੰਤ
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    ਜੇ (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    ਸਤਰ_list = argv[2];
    buf = get_ਬਫਰ_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    ਜੇ (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_ਸਤਰ_range(lo, buf->buf);	    // inclusive
    check_ਸਤਰ_range(hi - 1, buf->buf);	    // exclusive

    ਜੇ (SCHEME_FALSEP(ਸਤਰ_list) || SCHEME_NULLP(ਸਤਰ_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	ਜੇ (u_savedel((linenr_T)lo, (ਲੰਮਾ)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < old_len; i++)
		ਜੇ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    ਜੇ (buf->buf == curwin->w_ਬਫਰ)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_ਸਤਰਾਂ_mark((linenr_T)lo, (ਲੰਮਾ)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ਵਾਪਸ scheme_void;
    }
    ਨਹੀਂਤਾਂ
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_ਲੰਮਾਈ(ਸਤਰ_list);
	MZ_GC_CHECK();
	ਜੇ (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ		**array = ਨਿਰਾ;
	    Scheme_Object   *ਸਤਰ = ਨਿਰਾ;
	    Scheme_Object   *rest = ਨਿਰਾ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਸਤਰ);
	    MZ_GC_VAR_ਵਿੱਚ_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(ਅੱਖਰ *, new_len + 1);

	    rest = ਸਤਰ_list;
	    ਲਈ (i = 0; i < new_len; ++i)
	    {
		ਸਤਰ = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		ਜੇ (!SCHEME_STRINGP(ਸਤਰ))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ);
	    }

	    curbuf = buf->buf;

	    ਜੇ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    ਲਈ (i = 0; i < old_len - new_len; ++i)
	    {
		ਜੇ (ml_delete((linenr_T)lo) == FAIL)
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
	    ਲਈ (i = 0; i < old_len && i < new_len; i++)
		ਜੇ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    ਜਦੋਂ (i < new_len)
	    {
		ਜੇ (ml_append((linenr_T)(lo + i - 1),
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
						  (ਲੰਮਾ)MAXLNUM, (ਲੰਮਾ)extra);
	ਬਦਲਿਆ_ਸਤਰਾਂ((linenr_T)lo, 0, (linenr_T)hi, (ਲੰਮਾ)extra);

	ਜੇ (buf->buf == curwin->w_ਬਫਰ)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ਵਾਪਸ scheme_void;
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
    ਸਥਿਰ Scheme_Object *
ਪਾਓ_ਬਫਰ_ਸਤਰ_list(ਖਾਲੀ *ਅੰਕੜੇ, ਪੂਰਨ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ਅੰਕੜੇ;
    vim_mz_ਬਫਰ   *buf = ਨਿਰਾ;
    Scheme_Object   *list = ਨਿਰਾ;
    ਅੱਖਰ	    *str = ਨਿਰਾ;
    ਪੂਰਨ		    i, n, ਮਾਪ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, list);
    MZ_GC_REG();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    sandbox_check();
#ਜੇ_ਅੰਤ
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    ਜੇ (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_ਬਫਰ_arg(prim->name, 2, argc, argv);

    ਜੇ (n != 0)	    // 0 can be used in insert
	check_ਸਤਰ_range(n, buf->buf);
    ਜੇ (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_ਮੰਜ਼ਿਲ_ਸਤਰ(list);
	curbuf = buf->buf;

	ਜੇ (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_free(str);
	    appended_ਸਤਰਾਂ_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	ਵਾਪਸ scheme_void;
    }

    // List
    ਮਾਪ = scheme_proper_list_ਲੰਮਾਈ(list);
    MZ_GC_CHECK();
    ਜੇ (ਮਾਪ < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    ਨਹੀਂਤਾਂ
    {
	Scheme_Object   *ਸਤਰ = ਨਿਰਾ;
	Scheme_Object   *rest = ਨਿਰਾ;
	ਅੱਖਰ		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, ਸਤਰ);
	MZ_GC_VAR_ਵਿੱਚ_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(ਅੱਖਰ *, ਮਾਪ + 1);

	rest = list;
	ਲਈ (i = 0; i < ਮਾਪ; ++i)
	{
	    ਸਤਰ = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_ਮੰਜ਼ਿਲ_ਸਤਰ(ਸਤਰ);
	}

	curbuf = buf->buf;

	ਜੇ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; i < ਮਾਪ; ++i)
		ਜੇ (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    ਜੇ (i > 0)
		appended_ਸਤਰਾਂ_mark((linenr_T)n, (ਲੰਮਾ)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    ਵਾਪਸ scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ਸਥਿਰ Scheme_Object *
vim_bufferp(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv)
{
    ਜੇ (SCHEME_VIMBUFFERP(argv[0]))
	ਵਾਪਸ scheme_true;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ scheme_false;
}

/*
 * (win? obj)
 */
    ਸਥਿਰ Scheme_Object *
vim_windowp(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv)
{
    ਜੇ (SCHEME_VIMWINDOWP(argv[0]))
	ਵਾਪਸ scheme_true;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ਸਥਿਰ Scheme_Object *
vim_ਬਫਰ_validp(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv)
{
    ਜੇ (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_ਬਫਰ *)argv[0])->buf != INVALID_ਬਫਰ_ਮੁੱਲ)
	ਵਾਪਸ scheme_true;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ scheme_false;
}

/*
 * (win-valid? obj)
 */
    ਸਥਿਰ Scheme_Object *
vim_ਖਿੜਕੀ_validp(ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ, ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv)
{
    ਜੇ (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_ਖਿੜਕੀ *)argv[0])->win != INVALID_ਖਿੜਕੀ_ਮੁੱਲ)
	ਵਾਪਸ scheme_true;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ scheme_false;
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
    ਸਥਿਰ ਅੱਖਰ *
string_ਮੰਜ਼ਿਲ_ਸਤਰ(Scheme_Object *obj)
{
    ਅੱਖਰ	*scheme_str = ਨਿਰਾ;
    ਅੱਖਰ	*vim_str = ਨਿਰਾ;
    OUTPUT_LEN_TYPE	len;
    ਪੂਰਨ		i;

    scheme_str = scheme_display_ਮੰਜ਼ਿਲ_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    ਜੇ (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    ਲਈ (i = 0; i < len; ++i)
    {
	ਜੇ (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	ਨਹੀਂਤਾਂ
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    ਵਾਪਸ vim_str;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ਸਥਿਰ Scheme_Object *
vim_ਮੰਜ਼ਿਲ_mzscheme(typval_T *vim_ਮੁੱਲ)
{
    Scheme_Object	*ਨਤੀਜਾ = ਨਿਰਾ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_ਸਾਰਣੀ	*ਦੇਖਿਆ = ਨਿਰਾ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਨਤੀਜਾ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਦੇਖਿਆ);
    MZ_GC_REG();

    ਦੇਖਿਆ = scheme_make_hash_ਸਾਰਣੀ(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    ਨਤੀਜਾ = vim_ਮੰਜ਼ਿਲ_mzscheme_impl(vim_ਮੁੱਲ, 1, ਦੇਖਿਆ);

    MZ_GC_UNREG();
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ Scheme_Object *
vim_ਮੰਜ਼ਿਲ_mzscheme_impl(
    typval_T		*vim_ਮੁੱਲ,
    ਪੂਰਨ			ਡੂੰਘਾਈ,
    Scheme_Hash_ਸਾਰਣੀ	*ਦੇਖਿਆ)
{
    Scheme_Object   *ਨਤੀਜਾ = ਨਿਰਾ;
    ਪੂਰਨ		    new_ਮੁੱਲ_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਨਤੀਜਾ);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਦੇਖਿਆ);
    MZ_GC_REG();

    // Avoid infinite recursion
    ਜੇ (ਡੂੰਘਾਈ > 100)
    {
	MZ_GC_UNREG();
	ਵਾਪਸ scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    ਨਤੀਜਾ = scheme_hash_get(ਦੇਖਿਆ, (Scheme_Object *)vim_ਮੁੱਲ);
    MZ_GC_CHECK();
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ) // found, do nothing
	new_ਮੁੱਲ_2 = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_STRING)
    {
	ਨਤੀਜਾ = scheme_make_byte_string((ਅੱਖਰ *)vim_ਮੁੱਲ->vval.v_string);
	MZ_GC_CHECK();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_ਸੰਖਿਆ)
    {
	ਨਤੀਜਾ = scheme_make_integer((ਲੰਮਾ)vim_ਮੁੱਲ->vval.v_ਸੰਖਿਆ);
	MZ_GC_CHECK();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_FLOAT)
    {
	ਨਤੀਜਾ = scheme_make_double((ਦੁੱਗਣਾ)vim_ਮੁੱਲ->vval.v_float);
	MZ_GC_CHECK();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_LIST)
    {
	list_T		*list = vim_ਮੁੱਲ->vval.v_list;
	listitem_T	*curr;

	ਜੇ (list == ਨਿਰਾ || list->lv_ਪਹਿਲਾ == ਨਿਰਾ)
	    ਨਤੀਜਾ = scheme_null;
	ਨਹੀਂਤਾਂ
	{
	    Scheme_Object   *obj = ਨਿਰਾ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ਵਿੱਚ_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_ਆਖਰੀ;
	    obj = vim_ਮੰਜ਼ਿਲ_mzscheme_impl(&curr->li_tv, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
	    ਨਤੀਜਾ = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    ਜਦੋਂ (curr != list->lv_ਪਹਿਲਾ)
	    {
		curr = curr->li_prev;
		obj = vim_ਮੰਜ਼ਿਲ_mzscheme_impl(&curr->li_tv, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
		ਨਤੀਜਾ = scheme_make_pair(obj, ਨਤੀਜਾ);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_DICT)
    {
	Scheme_Object	  *ਕੁੰਜੀ = ਨਿਰਾ;
	Scheme_Object	  *obj = ਨਿਰਾ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, ਕੁੰਜੀ);
	MZ_GC_VAR_ਵਿੱਚ_REG(1, obj);
	MZ_GC_REG();

	ਨਤੀਜਾ = (Scheme_Object *)scheme_make_hash_ਸਾਰਣੀ(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	ਜੇ (vim_ਮੁੱਲ->vval.v_dict != ਨਿਰਾ)
	{
	    hashtab_T	*ht = &vim_ਮੁੱਲ->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_ਵਰਤਿਆ;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
	    {
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_ਮੰਜ਼ਿਲ_mzscheme_impl(&di->di_tv, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
		    ਕੁੰਜੀ = scheme_make_byte_string((ਅੱਖਰ *)hi->hi_ਕੁੰਜੀ);
		    MZ_GC_CHECK();
		    scheme_hash_ਸੈੱਟ((Scheme_Hash_ਸਾਰਣੀ *)ਨਤੀਜਾ, ਕੁੰਜੀ, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ਨਿਰਾ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((ਅੱਖਰ *)vim_ਮੁੱਲ->vval.v_string);
	MZ_GC_CHECK();
	ਨਤੀਜਾ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ਅਚਲ ਅੱਖਰ *)BYTE_STRING_ਮੁੱਲ(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_PARTIAL)
    {
	ਜੇ (vim_ਮੁੱਲ->vval.v_partial == ਨਿਰਾ)
	    ਨਤੀਜਾ = scheme_null;
	ਨਹੀਂਤਾਂ
	{
	    Scheme_Object *funcname = ਨਿਰਾ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ਵਿੱਚ_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (ਅੱਖਰ *)partial_name(vim_ਮੁੱਲ->vval.v_partial));
	    MZ_GC_CHECK();
	    ਨਤੀਜਾ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ਅਚਲ ਅੱਖਰ *)BYTE_STRING_ਮੁੱਲ(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਮੁੱਲ->v_type == VAR_BOOL || vim_ਮੁੱਲ->v_type == VAR_SPECIAL)
    {
	ਜੇ (vim_ਮੁੱਲ->vval.v_ਸੰਖਿਆ <= VVAL_TRUE)
	    ਨਤੀਜਾ = scheme_make_integer((ਲੰਮਾ)vim_ਮੁੱਲ->vval.v_ਸੰਖਿਆ);
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = scheme_null;
	MZ_GC_CHECK();
    }
    ਨਹੀਂਤਾਂ
    {
	ਨਤੀਜਾ = scheme_void;
	new_ਮੁੱਲ_2 = FALSE;
    }
    ਜੇ (new_ਮੁੱਲ_2)
    {
	scheme_hash_ਸੈੱਟ(ਦੇਖਿਆ, (Scheme_Object *)vim_ਮੁੱਲ, ਨਤੀਜਾ);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ ਪੂਰਨ
mzscheme_ਮੰਜ਼ਿਲ_vim(Scheme_Object *obj, typval_T *tv)
{
    ਪੂਰਨ i, ਸਥਿਤੀ;
    Scheme_Hash_ਸਾਰਣੀ	*ਦੇਖਿਆ = ਨਿਰਾ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, obj);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਦੇਖਿਆ);
    MZ_GC_REG();

    ਦੇਖਿਆ = scheme_make_hash_ਸਾਰਣੀ(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    ਸਥਿਤੀ = mzscheme_ਮੰਜ਼ਿਲ_vim_impl(obj, tv, 1, ਦੇਖਿਆ);
    ਲਈ (i = 0; i < ਦੇਖਿਆ->ਮਾਪ; ++i)
    {
	// free up remembered objects
	ਜੇ (ਦੇਖਿਆ->vals[i] != ਨਿਰਾ)
	    free_tv((typval_T *)ਦੇਖਿਆ->vals[i]);
    }

    MZ_GC_UNREG();
    ਵਾਪਸ ਸਥਿਤੀ;
}
    ਸਥਿਰ ਪੂਰਨ
mzscheme_ਮੰਜ਼ਿਲ_vim_impl(Scheme_Object *obj, typval_T *tv, ਪੂਰਨ ਡੂੰਘਾਈ,
	Scheme_Hash_ਸਾਰਣੀ *ਦੇਖਿਆ)
{
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    typval_T	*ਮਿਲਿਆ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, obj);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, ਦੇਖਿਆ);
    MZ_GC_REG();

    MZ_GC_CHECK();
    ਜੇ (ਡੂੰਘਾਈ > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = 0;
	ਵਾਪਸ FAIL;
    }

    ਮਿਲਿਆ = (typval_T *)scheme_hash_get(ਦੇਖਿਆ, obj);
    ਜੇ (ਮਿਲਿਆ != ਨਿਰਾ)
	copy_tv(ਮਿਲਿਆ, tv);
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_ਸੰਖਿਆ;
	tv->vval.v_ਸੰਖਿਆ = SCHEME_INT_VAL(obj);
    }
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_ਸੰਖਿਆ = SCHEME_TRUEP(obj);
    }
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ਮੁੱਲ(obj));
    }
# ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ਨਿਰਾ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_ਮੰਜ਼ਿਲ_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ਮੁੱਲ(tmp));
	MZ_GC_UNREG();
    }
# endif
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	ਜੇ (list == ਨਿਰਾ)
	    ਸਥਿਤੀ = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		    i;
	    Scheme_Object   *curr = ਨਿਰਾ;
	    Scheme_Object   *cval = ਨਿਰਾ;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_ਵਿੱਚ_REG(0, curr);
	    MZ_GC_VAR_ਵਿੱਚ_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    ਜੇ (v == ਨਿਰਾ)
		ਸਥਿਤੀ = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *ਦੇਖਿਆ_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, ਦੇਖਿਆ_tv);
		scheme_hash_ਸੈੱਟ(ਦੇਖਿਆ, obj, (Scheme_Object *)ਦੇਖਿਆ_tv);

		ਜੇ (SCHEME_VECTORP(obj))
		{
		    ਲਈ (i = 0; i < SCHEME_VEC_ਮਾਪ(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			ਸਥਿਤੀ = mzscheme_ਮੰਜ਼ਿਲ_vim_impl(cval, v, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
			ਜੇ (ਸਥਿਤੀ == FAIL)
			    ਰੋਕੋ;
			ਸਥਿਤੀ = list_append_tv(list, v);
			clear_tv(v);
			ਜੇ (ਸਥਿਤੀ == FAIL)
			    ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    ਲਈ (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			ਸਥਿਤੀ = mzscheme_ਮੰਜ਼ਿਲ_vim_impl(cval, v, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
			ਜੇ (ਸਥਿਤੀ == FAIL)
			    ਰੋਕੋ;
			ਸਥਿਤੀ = list_append_tv(list, v);
			clear_tv(v);
			ਜੇ (ਸਥਿਤੀ == FAIL)
			    ਰੋਕੋ;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    ਜੇ (ਸਥਿਤੀ == OK && !SCHEME_NULLP(curr))
		    {
			ਸਥਿਤੀ = mzscheme_ਮੰਜ਼ਿਲ_vim_impl(cval, v, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ);
			ਜੇ (ਸਥਿਤੀ == OK)
			{
			    ਸਥਿਤੀ = list_append_tv(list, v);
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
    ਨਹੀਂਤਾਂ ਜੇ (SCHEME_HASHTP(obj))
    {
	ਪੂਰਨ		i;
	dict_T		*dict;
	Scheme_Object   *ਕੁੰਜੀ = ਨਿਰਾ;
	Scheme_Object   *val = ਨਿਰਾ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, ਕੁੰਜੀ);
	MZ_GC_VAR_ਵਿੱਚ_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	ਜੇ (dict == ਨਿਰਾ)
	    ਸਥਿਤੀ = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    typval_T    *ਦੇਖਿਆ_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, ਦੇਖਿਆ_tv);
	    scheme_hash_ਸੈੱਟ(ਦੇਖਿਆ, obj, (Scheme_Object *)ਦੇਖਿਆ_tv);

	    ਲਈ (i = 0; i < ((Scheme_Hash_ਸਾਰਣੀ *)obj)->ਮਾਪ; ++i)
	    {
		ਜੇ (((Scheme_Hash_ਸਾਰਣੀ *) obj)->vals[i] != ਨਿਰਾ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *ਵਸਤੂ = dictitem_alloc((char_u *)string_ਮੰਜ਼ਿਲ_ਸਤਰ(
				((Scheme_Hash_ਸਾਰਣੀ *) obj)->ਕੁੰਜੀਆਂ[i]));
		    // convert Scheme val to Vim and add it to the dict
		    ਜੇ (mzscheme_ਮੰਜ਼ਿਲ_vim_impl(((Scheme_Hash_ਸਾਰਣੀ *) obj)->vals[i],
				    &ਵਸਤੂ->di_tv, ਡੂੰਘਾਈ + 1, ਦੇਖਿਆ) == FAIL
			    || dict_ਜੋੜੋ(dict, ਵਸਤੂ) == FAIL)
		    {
			dictitem_free(ਵਸਤੂ);
			ਸਥਿਤੀ = FAIL;
			ਰੋਕੋ;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    ਨਹੀਂਤਾਂ
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_ਮੰਜ਼ਿਲ_ਸਤਰ(obj);
    }
    MZ_GC_UNREG();
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ਸਥਿਰ Scheme_Object *
vim_funcref(ਖਾਲੀ *name, ਪੂਰਨ argc, Scheme_Object **argv)
{
    ਪੂਰਨ i;
    typval_T ਦਲੀਲਾਂ;
    ਪੂਰਨ ਸਥਿਤੀ = OK;
    Scheme_Object *ਨਤੀਜਾ = ਨਿਰਾ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ਨਤੀਜਾ);
    MZ_GC_REG();

    ਨਤੀਜਾ = scheme_void;
    ਜੇ (list == ਨਿਰਾ)
	ਸਥਿਤੀ = FAIL;
    ਨਹੀਂਤਾਂ
    {
	ਦਲੀਲਾਂ.v_type = VAR_LIST;
	ਦਲੀਲਾਂ.vval.v_list = list;
	++list->lv_refcount;
	ਲਈ (i = 0; ਸਥਿਤੀ == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    ਜੇ (v == ਨਿਰਾ)
		ਸਥਿਤੀ = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		ਸਥਿਤੀ = mzscheme_ਮੰਜ਼ਿਲ_vim(argv[i], v);
		ਜੇ (ਸਥਿਤੀ == OK)
		{
		    ਸਥਿਤੀ = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	ਜੇ (ਸਥਿਤੀ == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_ਮੁੱਲ((Scheme_Object *)name), &ਦਲੀਲਾਂ, &ret);
	    MZ_GC_CHECK();
	    ਨਤੀਜਾ = vim_ਮੰਜ਼ਿਲ_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&ਦਲੀਲਾਂ);
    MZ_GC_UNREG();
    ਜੇ (ਸਥਿਤੀ != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    ਨਹੀਂਤਾਂ
	raise_if_error();
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਖਾਲੀ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ਨਿਰਾ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, ret);
    MZ_GC_REG();

    ਜੇ (mzscheme_init())
    {
	MZ_GC_UNREG();
	ਵਾਪਸ;
    }

    MZ_GC_CHECK();
    ਜੇ (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_ਮੰਜ਼ਿਲ_vim(ret, rettv);

    MZ_GC_UNREG();
}
#ਜੇ_ਅੰਤ

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ਸਥਿਰ ਪੂਰਨ
vim_error_check(ਖਾਲੀ)
{
    ਵਾਪਸ (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ਸਥਿਰ ਖਾਲੀ
ਰਜਿਸਟਰ_vim_exn(ਖਾਲੀ)
{
    ਪੂਰਨ	nc = 0;
    ਪੂਰਨ i;
    Scheme_Object   *struct_exn = ਨਿਰਾ;
    Scheme_Object   *exn_name = ਨਿਰਾ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, struct_exn);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_ਚਿੰਨ੍ਹ("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_ਮੁੱਲ("struct:exn");
    MZ_GC_CHECK();

    ਜੇ (vim_exn == ਨਿਰਾ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ਨਿਰਾ, 0, 0, ਨਿਰਾ, ਨਿਰਾ
#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR >= 299
		, ਨਿਰਾ
#ਜੇ_ਅੰਤ
		);


    {
	Scheme_Object   **tmp = ਨਿਰਾ;
	Scheme_Object   *exn_ਨਾਂ[5] = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
	Scheme_Object   *exn_ਮੁੱਲਾਂ[5] = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG(0, exn_ਨਾਂ, 5);
	MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG(3, exn_ਮੁੱਲਾਂ, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_ਨਾਂ(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_ਨਾਂ, tmp, nc * ਆਕਾਰ(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_ਮੁੱਲਾਂ(vim_exn, exn_ਨਾਂ, nc, 0);
	mch_memmove(exn_ਮੁੱਲਾਂ, tmp, nc * ਆਕਾਰ(Scheme_Object *));
	MZ_GC_CHECK();

	ਲਈ (i = 0; i < nc; i++)
	{
	    scheme_ਜੋੜੋ_global_ਚਿੰਨ੍ਹ(exn_ਨਾਂ[i],
		    exn_ਮੁੱਲਾਂ[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    ਖਾਲੀ
raise_vim_exn(ਅਚਲ ਅੱਖਰ *ਜੋੜੋ_info)
{
    ਅੱਖਰ	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ਨਿਰਾ, ਨਿਰਾ};
    Scheme_Object   *exn = ਨਿਰਾ;
    Scheme_Object   *byte_string = ਨਿਰਾ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_ਵਿੱਚ_REG(0, argv, 2);
    MZ_GC_VAR_ਵਿੱਚ_REG(3, exn);
    MZ_GC_VAR_ਵਿੱਚ_REG(4, byte_string);
    MZ_GC_REG();

    ਜੇ (ਜੋੜੋ_info != ਨਿਰਾ)
    {
	ਅੱਖਰ		*c_string = ਨਿਰਾ;
	Scheme_Object   *info = ਨਿਰਾ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_ਵਿੱਚ_REG(0, c_string);
	MZ_GC_VAR_ਵਿੱਚ_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(ਜੋੜੋ_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (ਪੂਰਨ)STRLEN(fmt), 1, &info, ਨਿਰਾ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_ਮੰਜ਼ਿਲ_char_string(byte_string);
	SCHEME_ਸੈੱਟ_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    ਨਹੀਂਤਾਂ
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_ਮੰਜ਼ਿਲ_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#ਜੇ MZSCHEME_ਸੰਸਕਰਣ_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#ਨਹੀਂਤਾਂ
    argv[1] = scheme_current_continuation_marks(ਨਿਰਾ);
    MZ_GC_CHECK();
#ਜੇ_ਅੰਤ

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    ਖਾਲੀ
raise_if_error(ਖਾਲੀ)
{
    ਜੇ (vim_error_check())
	raise_vim_exn(ਨਿਰਾ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ਸਥਿਰ vim_mz_ਬਫਰ *
get_ਬਫਰ_arg(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ argnum, ਪੂਰਨ argc, Scheme_Object **argv)
{
    vim_mz_ਬਫਰ *b;

    ਜੇ (argc < argnum + 1)
	ਵਾਪਸ get_vim_curr_ਬਫਰ();
    ਜੇ (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_ਬਫਰ *)argv[argnum];
    (ਖਾਲੀ)get_valid_ਬਫਰ(argv[argnum]);
    ਵਾਪਸ b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ਸਥਿਰ vim_mz_ਖਿੜਕੀ *
get_ਖਿੜਕੀ_arg(ਅਚਲ ਅੱਖਰ *fname, ਪੂਰਨ argnum, ਪੂਰਨ argc, Scheme_Object **argv)
{
    vim_mz_ਖਿੜਕੀ *w;

    ਜੇ (argc < argnum + 1)
	ਵਾਪਸ get_vim_curr_ਖਿੜਕੀ();
    w = (vim_mz_ਖਿੜਕੀ *)argv[argnum];
    ਜੇ (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (ਖਾਲੀ)get_valid_ਖਿੜਕੀ(argv[argnum]);
    ਵਾਪਸ w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_ਬਫਰ(ਖਾਲੀ *obj)
{
    buf_T *buf = ((vim_mz_ਬਫਰ *)obj)->buf;

    ਜੇ (buf == INVALID_ਬਫਰ_ਮੁੱਲ)
	scheme_signal_error(_("buffer is invalid"));
    ਵਾਪਸ buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_ਖਿੜਕੀ(ਖਾਲੀ *obj)
{
    win_T *win = ((vim_mz_ਖਿੜਕੀ *)obj)->win;
    ਜੇ (win == INVALID_ਖਿੜਕੀ_ਮੁੱਲ)
	scheme_signal_error(_("window is invalid"));
    ਵਾਪਸ win;
}

    ਪੂਰਨ
mzthreads_allowed(ਖਾਲੀ)
{
    ਵਾਪਸ mz_ਧਾਗੇ_allow;
}

    ਸਥਿਰ ਪੂਰਨ
ਸਤਰ_ਵਿੱਚ_range(linenr_T lnum, buf_T *buf)
{
    ਵਾਪਸ (lnum > 0 && lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

    ਸਥਿਰ ਖਾਲੀ
check_ਸਤਰ_range(linenr_T lnum, buf_T *buf)
{
    ਜੇ (!ਸਤਰ_ਵਿੱਚ_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ਸਥਿਰ ਖਾਲੀ
mz_fix_cursor(ਪੂਰਨ lo, ਪੂਰਨ hi, ਪੂਰਨ extra)
{
    ਜੇ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ਜੇ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ਖਾਨਾ();
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ਨਹੀਂਤਾਂ
	    check_cursor_ਖਾਨਾ();
	ਬਦਲਿਆ_cline_bef_curs();
    }
    invalidate_botline();
}

ਸਥਿਰ Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_ਬਫਰ_ਸਤਰ, "get-buff-line", 1, 2},
    {ਸੈੱਟ_ਬਫਰ_ਸਤਰ, "set-buff-line", 2, 3},
    {get_ਬਫਰ_ਸਤਰ_list, "get-buff-line-list", 2, 3},
    {get_ਬਫਰ_name, "get-buff-name", 0, 1},
    {get_ਬਫਰ_ਅੰਕ, "get-buff-num", 0, 1},
    {get_ਬਫਰ_ਮਾਪ, "get-buff-size", 0, 1},
    {ਸੈੱਟ_ਬਫਰ_ਸਤਰ_list, "set-buff-line-list", 3, 4},
    {ਪਾਓ_ਬਫਰ_ਸਤਰ_list, "insert-buff-line-list", 2, 3},
    {get_curr_ਬਫਰ, "curr-buff", 0, 0},
    {get_ਬਫਰ_ਗਿਣਤੀ, "buff-count", 0, 0},
    {get_ਅਗਲਾ_ਬਫਰ, "get-next-buff", 0, 1},
    {get_prev_ਬਫਰ, "get-prev-buff", 0, 1},
    {mzscheme_open_ਬਫਰ, "open-buff", 1, 1},
    {get_ਬਫਰ_by_name, "get-buff-by-name", 1, 1},
    {get_ਬਫਰ_by_ਅੰਕ, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_ਖਿੜਕੀ_ਗਿਣਤੀ, "win-count", 0, 0},
    {get_ਖਿੜਕੀ_by_ਅੰਕ, "get-win-by-num", 1, 1},
    {get_ਖਿੜਕੀ_ਅੰਕ, "get-win-num", 0, 1},
    {get_ਖਿੜਕੀ_ਬਫਰ, "get-win-buffer", 0, 1},
    {get_ਖਿੜਕੀ_ਉਚਾਈ, "get-win-height", 0, 1},
    {ਸੈੱਟ_ਖਿੜਕੀ_ਉਚਾਈ, "set-win-height", 1, 2},
    {get_ਖਿੜਕੀ_ਚੌੜਾਈ, "get-win-width", 0, 1},
    {ਸੈੱਟ_ਖਿੜਕੀ_ਚੌੜਾਈ, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {ਸੈੱਟ_cursor, "set-cursor", 1, 2},
    {get_ਖਿੜਕੀ_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_ਸ਼ੁਰੂ, "range-start", 0, 0},
    {get_range_ਸਿਰਾ, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {ਸੈੱਟ_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_ਬਫਰ_validp, "buff-valid?", 1, 1},
    {vim_ਖਿੜਕੀ_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    ਸਥਿਰ vim_mz_ਬਫਰ *
get_vim_curr_ਬਫਰ(ਖਾਲੀ)
{
    ਜੇ (curbuf->b_mzscheme_ref == ਨਿਰਾ)
	ਵਾਪਸ (vim_mz_ਬਫਰ *)ਬਫਰ_new(curbuf);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਬਫਰ_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ਸਥਿਰ vim_mz_ਖਿੜਕੀ *
get_vim_curr_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਜੇ (curwin->w_mzscheme_ref == ਨਿਰਾ)
	ਵਾਪਸ (vim_mz_ਖਿੜਕੀ *)ਖਿੜਕੀ_new(curwin);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਖਿੜਕੀ_REF(curwin);
}

    ਸਥਿਰ ਖਾਲੀ
make_modules(ਖਾਲੀ)
{
    ਪੂਰਨ		    i;
    Scheme_Env	    *mod = ਨਿਰਾ;
    Scheme_Object   *vimext_ਚਿੰਨ੍ਹ = ਨਿਰਾ;
    Scheme_Object   *closed_prim = ਨਿਰਾ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ਵਿੱਚ_REG(0, mod);
    MZ_GC_VAR_ਵਿੱਚ_REG(1, vimext_ਚਿੰਨ੍ਹ);
    MZ_GC_VAR_ਵਿੱਚ_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_ਚਿੰਨ੍ਹ = scheme_intern_ਚਿੰਨ੍ਹ("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_ਚਿੰਨ੍ਹ, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_ਜੋੜੋ_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_ਅੰਤ_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
ਸਥਿਰ Scheme_Object *M_write = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *M_read = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *M_execute = ਨਿਰਾ;
ਸਥਿਰ Scheme_Object *M_delete = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
sandbox_check(ਖਾਲੀ)
{
    ਜੇ (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ਸਥਿਰ Scheme_Object *
sandbox_file_guard(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv)
{
    ਜੇ (!sandbox)
	ਵਾਪਸ scheme_void;

    Scheme_Object *requested_access = argv[2];

    ਜੇ (M_write == ਨਿਰਾ)
    {
	MZ_ਰਜਿਸਟਰ_STATIC(M_write);
	M_write = scheme_intern_ਚਿੰਨ੍ਹ("write");
	MZ_GC_CHECK();
    }
    ਜੇ (M_read == ਨਿਰਾ)
    {
	MZ_ਰਜਿਸਟਰ_STATIC(M_read);
	M_read = scheme_intern_ਚਿੰਨ੍ਹ("read");
	MZ_GC_CHECK();
    }
    ਜੇ (M_execute == ਨਿਰਾ)
    {
	MZ_ਰਜਿਸਟਰ_STATIC(M_execute);
	M_execute = scheme_intern_ਚਿੰਨ੍ਹ("execute");
	MZ_GC_CHECK();
    }
    ਜੇ (M_delete == ਨਿਰਾ)
    {
	MZ_ਰਜਿਸਟਰ_STATIC(M_delete);
	M_delete = scheme_intern_ਚਿੰਨ੍ਹ("delete");
	MZ_GC_CHECK();
    }

    ਜਦੋਂ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *ਵਸਤੂ = SCHEME_CAR(requested_access);
	ਜੇ (scheme_eq(ਵਸਤੂ, M_write) || scheme_eq(ਵਸਤੂ, M_read)
		|| scheme_eq(ਵਸਤੂ, M_execute) || scheme_eq(ਵਸਤੂ, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    ਵਾਪਸ scheme_void;
}

    ਸਥਿਰ Scheme_Object *
sandbox_ਜਾਲ_guard(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, Scheme_Object **argv ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ scheme_void;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
