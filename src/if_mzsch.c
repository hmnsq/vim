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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ಆದರೆ defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#ಆದರೆ defined(MZ_PRECISE_GC) && MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 400 \
    || MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#ಆದರೆ_ಅಂತ್ಯ

// Base data structures
#ವ್ಯಾಖ್ಯಾನಿಸು SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_ಬಫರ್_type)
#ವ್ಯಾಖ್ಯಾನಿಸು SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_ಕಿಟಕಿ_type)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_ಬಫರ್;

#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_ಬಫರ್_ಮೌಲ್ಯ ((buf_T *)(-1))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_ಕಿಟಕಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Scheme_Closed_Prim	*prim;
    ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		mina;	// arity information
    ಪೂರ್ಣಾಂಕ		maxa;
} Vim_Prim;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ	    *name;
    Scheme_Object   *ಪೋರ್ಟ್;
} ಪೋರ್ಟ್_Info;

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
ಸ್ಥಿರ Scheme_Object *sandbox_file_guard(ಪೂರ್ಣಾಂಕ, Scheme_Object **);
ಸ್ಥಿರ Scheme_Object *sandbox_ಜಾಲ_guard(ಪೂರ್ಣಾಂಕ, Scheme_Object **);
ಸ್ಥಿರ ಶೂನ್ಯ sandbox_check(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
//  Buffer-related commands
ಸ್ಥಿರ Scheme_Object *ಬಫರ್_new(buf_T *buf);
ಸ್ಥಿರ Scheme_Object *get_ಬಫರ್_by_ಅಂಕೆ(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, Scheme_Object **);
ಸ್ಥಿರ vim_mz_ಬಫರ್ *get_vim_curr_ಬಫರ್(ಶೂನ್ಯ);

//  Window-related commands
ಸ್ಥಿರ Scheme_Object *ಕಿಟಕಿ_new(win_T *win);
ಸ್ಥಿರ vim_mz_ಕಿಟಕಿ *get_vim_curr_ಕಿಟಕಿ(ಶೂನ್ಯ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vim_error_check(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_mzscheme_command(exarg_T *, ಶೂನ್ಯ *, Scheme_Closed_Prim *what);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ startup_mzscheme(ಶೂನ್ಯ);
ಸ್ಥಿರ ಅಕ್ಷರ *string_ಗಮ್ಯ_ಸಾಲು(Scheme_Object *obj);
#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#ಇಲ್ಲದಿದ್ದರೆ
# define OUTPUT_LEN_TYPE ದೀರ್ಘ
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ do_output(ಅಕ್ಷರ *mesg, OUTPUT_LEN_TYPE len);
ಸ್ಥಿರ ಶೂನ್ಯ do_printf(ಅಕ್ಷರ *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ಸ್ಥಿರ ಶೂನ್ಯ do_flush(ಶೂನ್ಯ);
ಸ್ಥಿರ Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ಸ್ಥಿರ Scheme_Object *ಹೊರತೆಗೆ_exn_ಸಂದೇಶ(Scheme_Object *v);
ಸ್ಥಿರ Scheme_Object *do_eval(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ noargc, Scheme_Object **noargv);
ಸ್ಥಿರ Scheme_Object *do_ತುಂಬು(ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ noargc, Scheme_Object **noargv);
ಸ್ಥಿರ ಶೂನ್ಯ ರಿಜಿಸ್ಟರ್_vim_exn(ಶೂನ್ಯ);
ಸ್ಥಿರ vim_mz_ಬಫರ್ *get_ಬಫರ್_arg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ argnum,
	ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ vim_mz_ಕಿಟಕಿ *get_ಕಿಟಕಿ_arg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ argnum,
	ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಾಲು_ಒಳ_range(linenr_T, buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ check_ಸಾಲು_range(linenr_T, buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mz_fix_cursor(ಪೂರ್ಣಾಂಕ lo, ಪೂರ್ಣಾಂಕ hi, ಪೂರ್ಣಾಂಕ extra);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval_with_exn_handling(ಶೂನ್ಯ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ಸ್ಥಿರ ಶೂನ್ಯ make_modules(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ init_exn_catching_apply(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mzscheme_env_main(Scheme_Env *env, ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mzscheme_init(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ Scheme_Object *vim_ಗಮ್ಯ_mzscheme(typval_T *vim_ಮೌಲ್ಯ);
ಸ್ಥಿರ Scheme_Object *vim_ಗಮ್ಯ_mzscheme_impl(typval_T *vim_ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ ಆಳ,
	Scheme_Hash_ಕೋಷ್ಟಕ *ಭೇಟಿಯಾದ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mzscheme_ಗಮ್ಯ_vim(Scheme_Object *obj, typval_T *tv);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mzscheme_ಗಮ್ಯ_vim_impl(Scheme_Object *obj, typval_T *tv, ಪೂರ್ಣಾಂಕ ಆಳ,
	Scheme_Hash_ಕೋಷ್ಟಕ *ಭೇಟಿಯಾದ);
ಸ್ಥಿರ Scheme_Object *vim_funcref(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_ಗಾತ್ರ_proc(ಶೂನ್ಯ *obj ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ gc_ಬೈಟುಗಳು_ಗಮ್ಯ_ಪದಗಳು(ಗಾತ್ರ(vim_mz_ಬಫರ್));
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_mark_proc(ಶೂನ್ಯ *obj)
{
    ಹಿಂತಿರುಗಿ ಬಫರ್_ಗಾತ್ರ_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಫರ್_fixup_proc(ಶೂನ್ಯ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    ಹಿಂತಿರುಗಿ ಬಫರ್_ಗಾತ್ರ_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಿಟಕಿ_ಗಾತ್ರ_proc(ಶೂನ್ಯ *obj ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ gc_ಬೈಟುಗಳು_ಗಮ್ಯ_ಪದಗಳು(ಗಾತ್ರ(vim_mz_ಕಿಟಕಿ));
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಿಟಕಿ_mark_proc(ಶೂನ್ಯ *obj)
{
    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_ಗಾತ್ರ_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಿಟಕಿ_fixup_proc(ಶೂನ್ಯ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_ಗಾತ್ರ_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define ಕಿಟಕಿ_REF(win) *(vim_mz_ಕಿಟಕಿ **)((win)->w_mzscheme_ref)
# define ಬಫರ್_REF(buf) *(vim_mz_ಬಫರ್ **)((buf)->b_mzscheme_ref)
#ಇಲ್ಲದಿದ್ದರೆ
# define ಕಿಟಕಿ_REF(win) (vim_mz_ಕಿಟಕಿ *)((win)->w_mzscheme_ref)
# define ಬಫರ್_REF(buf) (vim_mz_ಬಫರ್ *)((buf)->b_mzscheme_ref)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_MZSCHEME)
ಸ್ಥಿರ Scheme_Object *dll_scheme_eof;
ಸ್ಥಿರ Scheme_Object *dll_scheme_false;
ಸ್ಥಿರ Scheme_Object *dll_scheme_void;
ಸ್ಥಿರ Scheme_Object *dll_scheme_null;
ಸ್ಥಿರ Scheme_Object *dll_scheme_true;

ಸ್ಥಿರ Scheme_Thread **dll_scheme_current_thread_ptr;

ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_console_printf_ptr)(ಅಕ್ಷರ *str, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_console_output_ptr)(ಅಕ್ಷರ *str, OUTPUT_LEN_TYPE len);
ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_notify_multithread_ptr)(ಪೂರ್ಣಾಂಕ on);

ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc)(size_t ಗಾತ್ರ_ಒಳ_ಬೈಟುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc_atomic)(size_t ಗಾತ್ರ_ಒಳ_ಬೈಟುಗಳು);
ಸ್ಥಿರ Scheme_Env *(*dll_scheme_basic_env)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_check_ಎಳೆಗಳು)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ರಿಜಿಸ್ಟರ್_static)(ಶೂನ್ಯ *ptr, ದೀರ್ಘ ಗಾತ್ರ_2);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಗಣ_ರಾಶಿ_ತಳ)(ಶೂನ್ಯ *ತಳ, ಪೂರ್ಣಾಂಕ no_auto_statics);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಕೂಡಿಸು_global)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, Scheme_Object *val,
	Scheme_Env *env);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಕೂಡಿಸು_global_ಚಿಹ್ನೆ)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_rands,
	Scheme_Object **rands);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_builtin_ಮೌಲ್ಯ)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_byte_string_ಗಮ್ಯ_char_string)(Scheme_Object *s);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_ಮಾರ್ಗ)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chars);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್)(Scheme_Object *ಪೋರ್ಟ್);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು)(Scheme_Object *ಪೋರ್ಟ್);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 360
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_current_continuation_marks)(ಶೂನ್ಯ);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_ಟ್ಯಾಗ್);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *ಪೋರ್ಟ್);
ಸ್ಥಿರ ಅಕ್ಷರ *(*dll_scheme_display_ಗಮ್ಯ_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_rands_2, Scheme_Object **rands, ಪೂರ್ಣಾಂಕ val);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_dont_gc_ptr)(ಶೂನ್ಯ *p);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval_string)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str,
	Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval_string_all)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str,
	Scheme_Env *env, ಪೂರ್ಣಾಂಕ all);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಮುಕ್ತಾಯ_primitive_module)(Scheme_Env *env);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
ಸ್ಥಿರ ಅಕ್ಷರ *(*dll_scheme_format)(ಅಕ್ಷರ *format, ಪೂರ್ಣಾಂಕ flen, ಪೂರ್ಣಾಂಕ argc,
	Scheme_Object **argv, ದೀರ್ಘ *rlen);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಅಕ್ಷರ *(*dll_scheme_format_utf8)(ಅಕ್ಷರ *format, ಪೂರ್ಣಾಂಕ flen, ಪೂರ್ಣಾಂಕ argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, ಪೂರ್ಣಾಂಕ pos);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_gc_ptr_ok)(ಶೂನ್ಯ *p);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
ಸ್ಥಿರ ಅಕ್ಷರ *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	ದೀರ್ಘ *len);
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಅಕ್ಷರ *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_intern_ಚಿಹ್ನೆ)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *ಚಿಹ್ನೆ,
	Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, mzshort mina,
     mzshort maxa);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_integer_ಮೌಲ್ಯ)(ದೀರ್ಘ i);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, mzshort mina, mzshort maxa);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_string)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chars);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_string_output_ಪೋರ್ಟ್)();
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_byte_string)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chars);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_byte_string_output_ಪೋರ್ಟ್)();
# endif
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ Scheme_Object **(*dll_scheme_make_struct_ಹೆಸರುಗಳು)(Scheme_Object *ತಳ,
	Scheme_Object *ಕ್ಷೇತ್ರ_ಹೆಸರುಗಳು, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_ಹೊರಹೋಗು);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *ತಳ,
	Scheme_Object *ಪೋಷಕ, Scheme_Object *inspector, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_fields,
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ಸ್ಥಿರ Scheme_Object **(*dll_scheme_make_struct_ಮೌಲ್ಯಗಳು)(
	Scheme_Object *struct_type, Scheme_Object **ಹೆಸರುಗಳು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2,
	ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ Scheme_Type (*dll_scheme_make_type)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_ಸದಿಶ)(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2,
	Scheme_Object *fill);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_scheme_malloc_fail_ok)(ಶೂನ್ಯ *(*f)(size_t), size_t);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_open_ಒಳಹರಿವು_file)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *who);
ಸ್ಥಿರ Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_scheme_proper_list_ಉದ್ದಳತೆ)(Scheme_Object *list);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_raise)(Scheme_Object *exn);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_read)(Scheme_Object *ಪೋರ್ಟ್);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_signal_error)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_wrong_type)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *expected,
	ಪೂರ್ಣಾಂಕ which, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv);
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಗಣ_param)(Scheme_Config *c, ಪೂರ್ಣಾಂಕ pos,
	Scheme_Object *o);
ಸ್ಥಿರ Scheme_Config *(*dll_scheme_current_config)(ಶೂನ್ಯ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_char_string_ಗಮ್ಯ_byte_string)
    (Scheme_Object *s);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_char_string_ಗಮ್ಯ_ಮಾರ್ಗ)
    (Scheme_Object *s);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_scheme_ಗಣ_collects_ಮಾರ್ಗ)(Scheme_Object *p);
# endif
ಸ್ಥಿರ Scheme_Hash_ಕೋಷ್ಟಕ *(*dll_scheme_make_hash_ಕೋಷ್ಟಕ)(ಪೂರ್ಣಾಂಕ type);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_hash_ಗಣ)(Scheme_Hash_ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ,
	Scheme_Object *ಕೀಲಿ, Scheme_Object *ಮೌಲ್ಯ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ,
	Scheme_Object *ಕೀಲಿ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_double)(ದ್ವಿಗುಣ d);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_sized_byte_string)(ಅಕ್ಷರ *chars,
	ದೀರ್ಘ len, ಪೂರ್ಣಾಂಕ copy);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_dynamic_wind)(ಶೂನ್ಯ (*pre)(ಶೂನ್ಯ *), Scheme_Object *(* ಅಸ್ಥಿರ act)(ಶೂನ್ಯ *), ಶೂನ್ಯ (* ಅಸ್ಥಿರ post)(ಶೂನ್ಯ *), Scheme_Object *(*jmp_handler)(ಶೂನ್ಯ *), ಶೂನ್ಯ * ಅಸ್ಥಿರ ದತ್ತಾಂಶ);
# ifdef MZ_PRECISE_GC
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc_one_tagged)(size_t ಗಾತ್ರ_ಒಳ_ಬೈಟುಗಳು);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_GC_ರಿಜಿಸ್ಟರ್_traversers)(ಗಿಡ್ಡ ಟ್ಯಾಗ್, ಗಾತ್ರ_Proc ಗಾತ್ರ_2, Mark_Proc mark, Fixup_Proc fixup, ಪೂರ್ಣಾಂಕ is_constant_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ is_atomic);
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 400
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ಸ್ಥಿರ ಶೂನ್ಯ **(*dll_scheme_malloc_immobile_box)(ಶೂನ್ಯ *p);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_free_immobile_box)(ಶೂನ್ಯ **b);
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_scheme_main_ಸಿದ್ಧತೆ)(ಪೂರ್ಣಾಂಕ no_auto_statics, Scheme_Env_Main _main, ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv);
#   ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 603
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ರಿಜಿಸ್ಟರ್_tls_space)(ಶೂನ್ಯ *tls_space, ಪೂರ್ಣಾಂಕ tls_ಸೂಚ್ಯಂಕ);
#   endif
#  endif
#  ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ಮೂಲಕ_PROC)
ಸ್ಥಿರ Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(ಶೂನ್ಯ);
#  endif
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 600
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_embedded_ತುಂಬು)(intptr_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ predefined);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ರಿಜಿಸ್ಟರ್_embedded_ತುಂಬು)(intptr_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಗಣ_config_ಮಾರ್ಗ)(Scheme_Object *p);
# endif

# ಆದರೆ defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  ಆದರೆ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_ಕೋಷ್ಟಕ)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_ಕೂಡಿಸು_global dll_scheme_ಕೂಡಿಸು_global
#  define scheme_ಕೂಡಿಸು_global_ಚಿಹ್ನೆ dll_scheme_ಕೂಡಿಸು_global_ಚಿಹ್ನೆ
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_ಮೌಲ್ಯ dll_scheme_builtin_ಮೌಲ್ಯ
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
#   define scheme_byte_string_ಗಮ್ಯ_char_string dll_scheme_byte_string_ಗಮ್ಯ_char_string
#   define scheme_make_ಮಾರ್ಗ dll_scheme_make_ಮಾರ್ಗ
#  endif
#  define scheme_check_ಎಳೆಗಳು dll_scheme_check_ಎಳೆಗಳು
#  define scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್ dll_scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್
#  define scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_ಗಮ್ಯ_string dll_scheme_display_ಗಮ್ಯ_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_ಮುಕ್ತಾಯ_primitive_module dll_scheme_ಮುಕ್ತಾಯ_primitive_module
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
#   define scheme_format dll_scheme_format
#  ಇಲ್ಲದಿದ್ದರೆ
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  ಇಲ್ಲದಿದ್ದರೆ
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_ಚಿಹ್ನೆ dll_scheme_intern_ಚಿಹ್ನೆ
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_ಮೌಲ್ಯ dll_scheme_make_integer_ಮೌಲ್ಯ
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_ಪೋರ್ಟ್ dll_scheme_make_string_output_ಪೋರ್ಟ್
#  ಇಲ್ಲದಿದ್ದರೆ
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_ಪೋರ್ಟ್ \
    dll_scheme_make_byte_string_output_ಪೋರ್ಟ್
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_ಹೆಸರುಗಳು dll_scheme_make_struct_ಹೆಸರುಗಳು
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_ಮೌಲ್ಯಗಳು dll_scheme_make_struct_ಮೌಲ್ಯಗಳು
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_ಸದಿಶ dll_scheme_make_ಸದಿಶ
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_ಒಳಹರಿವು_file dll_scheme_open_ಒಳಹರಿವು_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_ಉದ್ದಳತೆ dll_scheme_proper_list_ಉದ್ದಳತೆ
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_ರಿಜಿಸ್ಟರ್_static dll_scheme_ರಿಜಿಸ್ಟರ್_static
#  define scheme_ಗಣ_ರಾಶಿ_ತಳ dll_scheme_ಗಣ_ರಾಶಿ_ತಳ
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
#   define scheme_ಗಣ_param dll_scheme_ಗಣ_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_ಗಮ್ಯ_byte_string \
    dll_scheme_char_string_ಗಮ್ಯ_byte_string
#   define scheme_char_string_ಗಮ್ಯ_ಮಾರ್ಗ \
    dll_scheme_char_string_ಗಮ್ಯ_ಮಾರ್ಗ
#   define scheme_ಗಣ_collects_ಮಾರ್ಗ dll_scheme_ಗಣ_collects_ಮಾರ್ಗ
#  endif
#  define scheme_make_hash_ಕೋಷ್ಟಕ dll_scheme_make_hash_ಕೋಷ್ಟಕ
#  define scheme_hash_ಗಣ dll_scheme_hash_ಗಣ
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_ರಿಜಿಸ್ಟರ್_traversers dll_GC_ರಿಜಿಸ್ಟರ್_traversers
#  endif
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_ಸಿದ್ಧತೆ dll_scheme_main_ಸಿದ್ಧತೆ
#    ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 603
#     define scheme_ರಿಜಿಸ್ಟರ್_tls_space dll_scheme_ರಿಜಿಸ್ಟರ್_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 600
#   define scheme_embedded_ತುಂಬು dll_scheme_embedded_ತುಂಬು
#   define scheme_ರಿಜಿಸ್ಟರ್_embedded_ತುಂಬು dll_scheme_ರಿಜಿಸ್ಟರ್_embedded_ತುಂಬು
#   define scheme_ಗಣ_config_ಮಾರ್ಗ dll_scheme_ಗಣ_config_ಮಾರ್ಗ
#  endif

#  ifndef PROTO
#   ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 500
#    ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ಮೂಲಕ_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ    *name;
    ಶೂನ್ಯ    **ptr;
} Thunk_Info;

ಸ್ಥಿರ Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (ಶೂನ್ಯ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (ಶೂನ್ಯ **)&dll_GC_malloc_atomic},
    {ಖಾಲಿ, ಖಾಲಿ}};

ಸ್ಥಿರ Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (ಶೂನ್ಯ **)&dll_scheme_eof},
    {"scheme_false", (ಶೂನ್ಯ **)&dll_scheme_false},
    {"scheme_void", (ಶೂನ್ಯ **)&dll_scheme_void},
    {"scheme_null", (ಶೂನ್ಯ **)&dll_scheme_null},
    {"scheme_true", (ಶೂನ್ಯ **)&dll_scheme_true},
# ಆದರೆ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_ಕೋಷ್ಟಕ)
    {"scheme_current_thread", (ಶೂನ್ಯ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (ಶೂನ್ಯ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (ಶೂನ್ಯ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(ಶೂನ್ಯ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (ಶೂನ್ಯ **)&dll_scheme_ಕೂಡಿಸು_global},
    {"scheme_add_global_symbol", (ಶೂನ್ಯ **)&dll_scheme_ಕೂಡಿಸು_global_ಚಿಹ್ನೆ},
    {"scheme_apply", (ಶೂನ್ಯ **)&dll_scheme_apply},
    {"scheme_basic_env", (ಶೂನ್ಯ **)&dll_scheme_basic_env},
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (ಶೂನ್ಯ **)&dll_scheme_byte_string_ಗಮ್ಯ_char_string},
    {"scheme_make_path", (ಶೂನ್ಯ **)&dll_scheme_make_ಮಾರ್ಗ},
# endif
    {"scheme_builtin_value", (ಶೂನ್ಯ **)&dll_scheme_builtin_ಮೌಲ್ಯ},
    {"scheme_check_threads", (ಶೂನ್ಯ **)&dll_scheme_check_ಎಳೆಗಳು},
    {"scheme_close_input_port", (ಶೂನ್ಯ **)&dll_scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್},
    {"scheme_count_lines", (ಶೂನ್ಯ **)&dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು},
    {"scheme_current_continuation_marks",
	(ಶೂನ್ಯ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (ಶೂನ್ಯ **)&dll_scheme_display},
    {"scheme_display_to_string", (ಶೂನ್ಯ **)&dll_scheme_display_ಗಮ್ಯ_string},
    {"scheme_do_eval", (ಶೂನ್ಯ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (ಶೂನ್ಯ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (ಶೂನ್ಯ **)&dll_scheme_eq},
    {"scheme_eval", (ಶೂನ್ಯ **)&dll_scheme_eval},
    {"scheme_eval_string", (ಶೂನ್ಯ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (ಶೂನ್ಯ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(ಶೂನ್ಯ **)&dll_scheme_ಮುಕ್ತಾಯ_primitive_module},
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
    {"scheme_format", (ಶೂನ್ಯ **)&dll_scheme_format},
# ಇಲ್ಲದಿದ್ದರೆ
    {"scheme_format_utf8", (ಶೂನ್ಯ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (ಶೂನ್ಯ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (ಶೂನ್ಯ **)&dll_scheme_gc_ptr_ok},
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
    {"scheme_get_sized_string_output",
	(ಶೂನ್ಯ **)&dll_scheme_get_sized_string_output},
# ಇಲ್ಲದಿದ್ದರೆ
    {"scheme_get_sized_byte_string_output",
	(ಶೂನ್ಯ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (ಶೂನ್ಯ **)&dll_scheme_intern_ಚಿಹ್ನೆ},
    {"scheme_lookup_global", (ಶೂನ್ಯ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(ಶೂನ್ಯ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (ಶೂನ್ಯ **)&dll_scheme_make_integer_ಮೌಲ್ಯ},
    {"scheme_make_pair", (ಶೂನ್ಯ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (ಶೂನ್ಯ **)&dll_scheme_make_prim_w_arity},
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
    {"scheme_make_string", (ಶೂನ್ಯ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(ಶೂನ್ಯ **)&dll_scheme_make_string_output_ಪೋರ್ಟ್},
# ಇಲ್ಲದಿದ್ದರೆ
    {"scheme_make_byte_string", (ಶೂನ್ಯ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(ಶೂನ್ಯ **)&dll_scheme_make_byte_string_output_ಪೋರ್ಟ್},
# endif
    {"scheme_make_struct_instance",
	(ಶೂನ್ಯ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (ಶೂನ್ಯ **)&dll_scheme_make_struct_ಹೆಸರುಗಳು},
    {"scheme_make_struct_type", (ಶೂನ್ಯ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (ಶೂನ್ಯ **)&dll_scheme_make_struct_ಮೌಲ್ಯಗಳು},
    {"scheme_make_type", (ಶೂನ್ಯ **)&dll_scheme_make_type},
    {"scheme_make_vector", (ಶೂನ್ಯ **)&dll_scheme_make_ಸದಿಶ},
    {"scheme_malloc_fail_ok", (ಶೂನ್ಯ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (ಶೂನ್ಯ **)&dll_scheme_open_ಒಳಹರಿವು_file},
    {"scheme_primitive_module", (ಶೂನ್ಯ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (ಶೂನ್ಯ **)&dll_scheme_proper_list_ಉದ್ದಳತೆ},
    {"scheme_raise", (ಶೂನ್ಯ **)&dll_scheme_raise},
    {"scheme_read", (ಶೂನ್ಯ **)&dll_scheme_read},
    {"scheme_register_static", (ಶೂನ್ಯ **)&dll_scheme_ರಿಜಿಸ್ಟರ್_static},
    {"scheme_set_stack_base", (ಶೂನ್ಯ **)&dll_scheme_ಗಣ_ರಾಶಿ_ತಳ},
    {"scheme_signal_error", (ಶೂನ್ಯ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (ಶೂನ್ಯ **)&dll_scheme_wrong_type},
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
    {"scheme_set_param", (ಶೂನ್ಯ **)&dll_scheme_ಗಣ_param},
    {"scheme_current_config", (ಶೂನ್ಯ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(ಶೂನ್ಯ **)&dll_scheme_char_string_ಗಮ್ಯ_byte_string},
    {"scheme_char_string_to_path", (ಶೂನ್ಯ **)&dll_scheme_char_string_ಗಮ್ಯ_ಮಾರ್ಗ},
    {"scheme_set_collects_path", (ಶೂನ್ಯ **)&dll_scheme_ಗಣ_collects_ಮಾರ್ಗ},
# endif
    {"scheme_make_hash_table", (ಶೂನ್ಯ **)&dll_scheme_make_hash_ಕೋಷ್ಟಕ},
    {"scheme_hash_set", (ಶೂನ್ಯ **)&dll_scheme_hash_ಗಣ},
    {"scheme_hash_get", (ಶೂನ್ಯ **)&dll_scheme_hash_get},
    {"scheme_make_double", (ಶೂನ್ಯ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (ಶೂನ್ಯ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (ಶೂನ್ಯ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (ಶೂನ್ಯ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (ಶೂನ್ಯ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (ಶೂನ್ಯ **)&dll_GC_ರಿಜಿಸ್ಟರ್_traversers},
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (ಶೂನ್ಯ **)&dll_scheme_main_ಸಿದ್ಧತೆ},
#   ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 603
    {"scheme_register_tls_space", (ಶೂನ್ಯ **)&dll_scheme_ರಿಜಿಸ್ಟರ್_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (ಶೂನ್ಯ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (ಶೂನ್ಯ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (ಶೂನ್ಯ **)&dll_scheme_free_immobile_box},
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 500
#  ಆದರೆ defined(IMPLEMENT_THREAD_LOCAL_ಮೂಲಕ_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_ಮೂಲಕ_PROC)
    {"scheme_external_get_thread_local_variables", (ಶೂನ್ಯ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 600
    {"scheme_embedded_load", (ಶೂನ್ಯ **)&dll_scheme_embedded_ತುಂಬು},
    {"scheme_register_embedded_load", (ಶೂನ್ಯ **)&dll_scheme_ರಿಜಿಸ್ಟರ್_embedded_ತುಂಬು},
    {"scheme_set_config_path", (ಶೂನ್ಯ **)&dll_scheme_ಗಣ_config_ಮಾರ್ಗ},
# endif
    {ಖಾಲಿ, ಖಾಲಿ}};

ಸ್ಥಿರ HINSTANCE hMzGC = 0;
ಸ್ಥಿರ HINSTANCE hMzSch = 0;

ಸ್ಥಿರ ಶೂನ್ಯ dynamic_mzscheme_ಕೊನೆ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mzscheme_runtime_link_init(ಅಕ್ಷರ *sch_dll, ಅಕ್ಷರ *gc_dll,
	ಪೂರ್ಣಾಂಕ verbose);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mzscheme_runtime_link_init(ಅಕ್ಷರ *sch_dll, ಅಕ್ಷರ *gc_dll, ಪೂರ್ಣಾಂಕ verbose)
{
    Thunk_Info *thunk = ಖಾಲಿ;

    ಆದರೆ (hMzGC && hMzSch)
	ಹಿಂತಿರುಗಿ OK;
    hMzSch = vim_ತುಂಬು_Lib(sch_dll);
    hMzGC = vim_ತುಂಬು_Lib(gc_dll);

    ಆದರೆ (!hMzGC)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), gc_dll, GetWin32Error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!hMzSch)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), sch_dll, GetWin32Error());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (thunk = mzsch_imports; thunk->name; thunk++)
    {
	ಆದರೆ ((*thunk->ptr =
		    (ಶೂನ್ಯ *)Get_Proc_ವಿಳಾಸ(hMzSch, thunk->name)) == ಖಾಲಿ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), thunk->name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಗಾಗಿ (thunk = mzgc_imports; thunk->name; thunk++)
    {
	ಆದರೆ ((*thunk->ptr =
		    (ಶೂನ್ಯ *)Get_Proc_ವಿಳಾಸ(hMzGC, thunk->name)) == ಖಾಲಿ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), thunk->name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
mzscheme_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ mzscheme_runtime_link_init(
	    (ಅಕ್ಷರ *)p_mzschemedll, (ಅಕ್ಷರ *)p_mzschemegcdll, verbose) == OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dynamic_mzscheme_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    ಆದರೆ (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#ಆದರೆ_ಅಂತ್ಯ // DYNAMIC_MZSCHEME

#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, ಅಂಕೆ) GUARANTEE_STRING(proc, ಅಂಕೆ)
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ Scheme_Object *
guaranteed_byte_string_arg(ಅಕ್ಷರ *proc, ಪೂರ್ಣಾಂಕ ಅಂಕೆ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    ಆದರೆ (SCHEME_BYTE_STRINGP(argv[ಅಂಕೆ]))
    {
	ಹಿಂತಿರುಗಿ argv[ಅಂಕೆ];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_CHAR_STRINGP(argv[ಅಂಕೆ]))
    {
	Scheme_Object *tmp = ಖಾಲಿ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ಒಳ_REG(0, argv[ಅಂಕೆ]);
	MZ_GC_VAR_ಒಳ_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_ಗಮ್ಯ_byte_string(argv[ಅಂಕೆ]);
	MZ_GC_UNREG();
	ಹಿಂತಿರುಗಿ tmp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	scheme_wrong_type(proc, "string", ಅಂಕೆ, argc, argv);
    // unreachable
    ಹಿಂತಿರುಗಿ scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, ಅಂಕೆ) guaranteed_byte_string_arg(proc, ಅಂಕೆ, argc, argv)
#ಆದರೆ_ಅಂತ್ಯ

// need to put it here for dynamic stuff to work
#ಆದರೆ defined(INCLUDE_MZSCHEME_ತಳ)
# include "mzscheme_base.c"
#ಆದರೆ_ಅಂತ್ಯ

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ಸ್ಥಿರ Scheme_Type mz_ಬಫರ್_type;
ಸ್ಥಿರ Scheme_Type mz_ಕಿಟಕಿ_type;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initialized = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_MZSCHEME
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ disabled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತುಂಬು_ತಳ_module_failed = FALSE;

// global environment
ಸ್ಥಿರ Scheme_Env    *environment = ಖಾಲಿ;
// output/error handlers
ಸ್ಥಿರ Scheme_Object *curout = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *curerr = ಖಾಲಿ;
// exn:vim exception
ಸ್ಥಿರ Scheme_Object *exn_catching_apply = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *exn_p = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *exn_ಸಂದೇಶ = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *vim_exn = ಖಾಲಿ; // Vim Error exception

#ಆದರೆ !defined(MZ_PRECISE_GC) || MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 400
ಸ್ಥಿರ ಶೂನ್ಯ *ರಾಶಿ_ತಳ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ದೀರ್ಘ range_ಆರಂಭ;
ಸ್ಥಿರ ದೀರ್ಘ range_ಕೊನೆ;

// MzScheme threads scheduling stuff
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mz_ಎಳೆಗಳು_allow = 0;

#ಆದರೆ defined(FEAT_GUI_MSWIN)
ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ಸ್ಥಿರ UINT timer_id = 0;
#ಅಥವಾಆದರೆ defined(FEAT_GUI_GTK)
ಸ್ಥಿರ gboolean timer_proc(gpointer);
ಸ್ಥಿರ guint timer_id = 0;
#ಅಥವಾಆದರೆ defined(FEAT_GUI_MOTIF)
ಸ್ಥಿರ ಶೂನ್ಯ timer_proc(Xt_ಸೂಚಕ, XtIntervalId *);
ಸ್ಥಿರ XtIntervalId timer_id = (XtIntervalId)0;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    ಶೂನ್ಯ
mzvim_check_ಎಳೆಗಳು(ಶೂನ್ಯ)
{
    // Last time MzScheme threads were scheduled
    ಸ್ಥಿರ time_t mz_ಕೊನೆಯ_ಸಮಯ = 0;

    ಆದರೆ (mz_ಎಳೆಗಳು_allow && p_mzq > 0)
    {
	time_t now = ಸಮಯ(ಖಾಲಿ);

	ಆದರೆ ((now - mz_ಕೊನೆಯ_ಸಮಯ) * 1000 > p_mzq)
	{
	    mz_ಕೊನೆಯ_ಸಮಯ = now;
	    scheme_check_ಎಳೆಗಳು();
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MZSCHEME_GUI_ಎಳೆಗಳು)
ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_timer(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ remove_timer(ಶೂನ್ಯ);

// timers are presented in GUI only
# ಆದರೆ defined(FEAT_GUI_MSWIN)
    ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK
timer_proc(
    HWND	hwnd ಬಳಸದ,
    UINT	u_ಸಂದೇಶ ಬಳಸದ,
    UINT_PTR	idEvent ಬಳಸದ,
    DWORD	dw_ಸಮಯ ಬಳಸದ)
# elif defined(FEAT_GUI_GTK)
    ಸ್ಥಿರ gboolean
timer_proc(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
# elif defined(FEAT_GUI_MOTIF)
    ಸ್ಥಿರ ಶೂನ್ಯ
timer_proc(Xt_ಸೂಚಕ timed_ಹೊರಹೋಗು ಬಳಸದ, XtIntervalId *interval_id ಬಳಸದ)
# endif
{
    scheme_check_ಎಳೆಗಳು();
# ಆದರೆ defined(FEAT_GUI_GTK)
    ಹಿಂತಿರುಗಿ TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    ಆದರೆ (mz_ಎಳೆಗಳು_allow && p_mzq > 0)
	timer_id = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, p_mzq,
		timer_proc, ಖಾಲಿ);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_timer(ಶೂನ್ಯ)
{
# ಆದರೆ defined(FEAT_GUI_MSWIN)
    timer_id = ಗಣ_Timer(ಖಾಲಿ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_ಕೂಡಿಸು((guint)p_mzq, (GSourceFunc)timer_proc, ಖಾಲಿ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, p_mzq, timer_proc, ಖಾಲಿ);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_timer(ಶೂನ್ಯ)
{
# ಆದರೆ defined(FEAT_GUI_MSWIN)
    KillTimer(ಖಾಲಿ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_ಮೂಲ_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(timer_id);
# endif
    timer_id = 0;
}

#ಆದರೆ_ಅಂತ್ಯ // MZSCHEME_GUI_THREADS

    ಅಕ್ಷರ *
did_ಗಣ_mzquantum(optset_T *ವಾದಗಳು ಬಳಸದ)
{
#ಆದರೆ defined(MZSCHEME_GUI_ಎಳೆಗಳು)
    // reset timer
    ಆದರೆ (timer_id != 0)
	remove_timer();
    ಆದರೆ (mz_ಎಳೆಗಳು_allow && p_mzq > 0 && gui.ಒಳ_use)
	ಸಿದ್ಧತೆ_timer();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
notify_multithread(ಪೂರ್ಣಾಂಕ on)
{
    mz_ಎಳೆಗಳು_allow = on;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZSCHEME_GUI_ಎಳೆಗಳು
    ಆದರೆ (on && timer_id == 0 && p_mzq > 0 && gui.ಒಳ_use)
	ಸಿದ್ಧತೆ_timer();
    ಆದರೆ (!on && timer_id != 0)
	remove_timer();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
mzscheme_ಕೊನೆ(ಶೂನ್ಯ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#ಆದರೆ 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_ಕೊನೆ();
# endif
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ HAVE_TLS_SPACE && !defined(VIMDLL)
# ಆದರೆ defined(_MSC_VER)
ಸ್ಥಿರ __declspec(thread) ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t tls_ಸೂಚ್ಯಂಕ;
# elif defined(__MINGW32__)
ಸ್ಥಿರ __thread ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t tls_ಸೂಚ್ಯಂಕ;
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ THREAD_LOCAL ಶೂನ್ಯ *tls_space;
ಸ್ಥಿರ intptr_t tls_ಸೂಚ್ಯಂಕ = 0;
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    ಪೂರ್ಣಾಂಕ
mzscheme_main(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    argc = 0;
    ಅಕ್ಷರ    *argv = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    ಆದರೆ (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	ಹಿಂತಿರುಗಿ vim_main2();
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TLS_SPACE
# ifdef VIMDLL
    ಶೂನ್ಯ **ptls_space;
    intptr_t tls_ಸೂಚ್ಯಂಕ_2;
    ಶೂನ್ಯ (*pget_tls_info)(ಶೂನ್ಯ ***ptls_space, intptr_t *ptls_ಸೂಚ್ಯಂಕ);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (ಶೂನ್ಯ *)Get_Proc_ವಿಳಾಸ(
				GetModuleHandle(ಖಾಲಿ), "get_tls_info");
    ಆದರೆ (pget_tls_info == ಖಾಲಿ)
    {
	disabled = TRUE;
	ಹಿಂತಿರುಗಿ vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_ಸೂಚ್ಯಂಕ_2);
    scheme_ರಿಜಿಸ್ಟರ್_tls_space(ptls_space, tls_ಸೂಚ್ಯಂಕ_2);
# ಇಲ್ಲದಿದ್ದರೆ
    scheme_ರಿಜಿಸ್ಟರ್_tls_space(&tls_space, tls_ಸೂಚ್ಯಂಕ);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRAMPOLINED_MZVIM_STARTUP
    ಹಿಂತಿರುಗಿ scheme_main_ಸಿದ್ಧತೆ(TRUE, mzscheme_env_main, argc, &argv);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ mzscheme_env_main(ಖಾಲಿ, argc, &argv);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mzscheme_env_main(Scheme_Env *env, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, ಅಕ್ಷರ **argv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ಖಾಲಿ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, dummy);

    ರಾಶಿ_ತಳ = &__gc_var_stack__;
# ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ dummy = 0;
    ರಾಶಿ_ತಳ = (ಶೂನ್ಯ *)&dummy;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ Scheme_Object*
ತುಂಬು_ತಳ_module(ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    scheme_namespace_require(scheme_intern_ಚಿಹ್ನೆ((ಅಕ್ಷರ *)ದತ್ತಾಂಶ));
    ಹಿಂತಿರುಗಿ scheme_null;
}

    ಸ್ಥಿರ Scheme_Object *
ತುಂಬು_ತಳ_module_on_error(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ)
{
    ತುಂಬು_ತಳ_module_failed = TRUE;
    ಹಿಂತಿರುಗಿ scheme_null;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
startup_mzscheme(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TRAMPOLINED_MZVIM_STARTUP
    scheme_ಗಣ_ರಾಶಿ_ತಳ(ರಾಶಿ_ತಳ, 1);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#ಆದರೆ_ಅಂತ್ಯ

    MZ_ರಿಜಿಸ್ಟರ್_STATIC(environment);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(curout);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(curerr);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(exn_catching_apply);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(exn_p);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(exn_ಸಂದೇಶ);
    MZ_ರಿಜಿಸ್ಟರ್_STATIC(vim_exn);

    MZ_GC_CHECK();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ INCLUDE_MZSCHEME_ತಳ
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#ಆದರೆ_ಅಂತ್ಯ

    // setup 'current-library-collection-paths' parameter
#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
    {
	Scheme_Object *coll_ಮಾರ್ಗ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, coll_ಮಾರ್ಗ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    coll_ಮಾರ್ಗ = scheme_make_ಮಾರ್ಗ((ಅಕ್ಷರ *)s);
	    MZ_GC_CHECK();
	    ಆದರೆ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	ಆದರೆ (coll_ಮಾರ್ಗ == ಖಾಲಿ)
	{
	    coll_ಮಾರ್ಗ = scheme_make_ಮಾರ್ಗ(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	ಆದರೆ (coll_ಮಾರ್ಗ != ಖಾಲಿ)
	{
	    scheme_ಗಣ_collects_ಮಾರ್ಗ(coll_ಮಾರ್ಗ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ಖಾಲಿ;
	Scheme_Object	*coll_pair = ಖಾಲಿ;
	Scheme_Config	*config = ಖಾಲಿ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_ಒಳ_REG(0, coll_string);
	MZ_GC_VAR_ಒಳ_REG(1, coll_pair);
	MZ_GC_VAR_ಒಳ_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_ಗಣ_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 600
    {
	Scheme_Object *config_ಮಾರ್ಗ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, config_ಮಾರ್ಗ);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    config_ಮಾರ್ಗ = scheme_make_ಮಾರ್ಗ((ಅಕ್ಷರ *)s);
	    MZ_GC_CHECK();
	    ಆದರೆ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	ಆದರೆ (config_ಮಾರ್ಗ == ಖಾಲಿ)
	{
	    config_ಮಾರ್ಗ = scheme_make_ಮಾರ್ಗ(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	ಆದರೆ (config_ಮಾರ್ಗ != ಖಾಲಿ)
	{
	    scheme_ಗಣ_config_ಮಾರ್ಗ(config_ಮಾರ್ಗ);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ಖಾಲಿ, ತುಂಬು_ತಳ_module, ಖಾಲಿ,
				    ತುಂಬು_ತಳ_module_on_error, "racket/base");
	ಆದರೆ (ತುಂಬು_ತಳ_module_failed)
	{
	    ತುಂಬು_ತಳ_module_failed = FALSE;
	    scheme_dynamic_wind(ಖಾಲಿ, ತುಂಬು_ತಳ_module, ಖಾಲಿ,
				    ತುಂಬು_ತಳ_module_on_error, "scheme/base");
	    ಆದರೆ (ತುಂಬು_ತಳ_module_failed)
		ಹಿಂತಿರುಗಿ -1;
	}
    }

    ರಿಜಿಸ್ಟರ್_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = ಖಾಲಿ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, make_security_guard);
	MZ_GC_REG();

# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_ಚಿಹ್ನೆ = ಖಾಲಿ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ಒಳ_REG(0, make_security_guard_ಚಿಹ್ನೆ);
	    MZ_GC_REG();
	    make_security_guard_ಚಿಹ್ನೆ = scheme_intern_ಚಿಹ್ನೆ("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_ಚಿಹ್ನೆ, environment);
	    MZ_GC_UNREG();
	}
# ಇಲ್ಲದಿದ್ದರೆ
	make_security_guard = scheme_builtin_ಮೌಲ್ಯ("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	ಆದರೆ (make_security_guard != ಖಾಲಿ)
	{
	    Scheme_Object   *ವಾದಗಳು[3] = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
	    Scheme_Object   *guard = ಖಾಲಿ;
	    Scheme_Config   *config = ಖಾಲಿ;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_ಒಳ_REG(0, ವಾದಗಳು, 3);
	    MZ_GC_VAR_ಒಳ_REG(3, guard);
	    MZ_GC_VAR_ಒಳ_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    ವಾದಗಳು[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    ವಾದಗಳು[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    ವಾದಗಳು[2] = scheme_make_prim_w_arity(sandbox_ಜಾಲ_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, ವಾದಗಳು);
	    MZ_GC_CHECK();
	    scheme_ಗಣ_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#ಆದರೆ_ಅಂತ್ಯ
    // Create buffer and window types for use in Scheme code
    mz_ಬಫರ್_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_ಕಿಟಕಿ_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
    GC_ರಿಜಿಸ್ಟರ್_traversers(mz_ಬಫರ್_type,
	    ಬಫರ್_ಗಾತ್ರ_proc, ಬಫರ್_mark_proc, ಬಫರ್_fixup_proc,
	    TRUE, TRUE);
    GC_ರಿಜಿಸ್ಟರ್_traversers(mz_ಕಿಟಕಿ_type,
	    ಕಿಟಕಿ_ಗಾತ್ರ_proc, ಕಿಟಕಿ_mark_proc, ಕಿಟಕಿ_fixup_proc,
	    TRUE, TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mzscheme_init(ಶೂನ್ಯ)
{
    ಆದರೆ (!initialized)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_MZSCHEME
	ಆದರೆ (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    ಹಿಂತಿರುಗಿ -1;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (ತುಂಬು_ತಳ_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_ತಳ_module_could_not_be_loaded));
	    ಹಿಂತಿರುಗಿ -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ಖಾಲಿ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_ಪೋರ್ಟ್();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_ಪೋರ್ಟ್();
	MZ_GC_CHECK();
	scheme_ಗಣ_param(config, MZCONFIG_OUTPUT_ಪೋರ್ಟ್, curout);
	MZ_GC_CHECK();
	scheme_ಗಣ_param(config, MZCONFIG_ERROR_ಪೋರ್ಟ್, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    ಹಿಂತಿರುಗಿ 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_with_exn_handling(
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *ಮೌಲ್ಯ = ಖಾಲಿ;
    Scheme_Object   *exn = ಖಾಲಿ;
    Scheme_Object   *prim = ಖಾಲಿ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ಒಳ_REG(0, ಮೌಲ್ಯ);
    MZ_GC_VAR_ಒಳ_REG(1, exn);
    MZ_GC_VAR_ಒಳ_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, ದತ್ತಾಂಶ, "mzvim", 0, 0);
    MZ_GC_CHECK();
    ಮೌಲ್ಯ = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    ಆದರೆ (!ಮೌಲ್ಯ)
    {
	ಮೌಲ್ಯ = ಹೊರತೆಗೆ_exn_ಸಂದೇಶ(exn);
	// Got an exn?
	ಆದರೆ (ಮೌಲ್ಯ)
	{
	    scheme_display(ಮೌಲ್ಯ, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (ret != ಖಾಲಿ)	// if pointer to retval supported give it up
	*ret = ಮೌಲ್ಯ;
    // Print any result, as long as it's not a void
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!SCHEME_VOIDP(ಮೌಲ್ಯ))
    {
	scheme_display(ಮೌಲ್ಯ, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * :mzscheme
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_mzscheme_command(exarg_T *eap, ಶೂನ್ಯ *ದತ್ತಾಂಶ, Scheme_Closed_Prim *what)
{
    ಆದರೆ (mzscheme_init())
	ಹಿಂತಿರುಗಿ FAIL;

    range_ಆರಂಭ = eap->line1;
    range_ಕೊನೆ = eap->line2;

    ಹಿಂತಿರುಗಿ eval_with_exn_handling(ದತ್ತಾಂಶ, what, ಖಾಲಿ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    ಶೂನ್ಯ
mzscheme_ಬಫರ್_free(buf_T *buf)
{
    ಆದರೆ (buf->b_mzscheme_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_mz_ಬಫರ್ *bp = ಖಾಲಿ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, bp);
    MZ_GC_REG();

    bp = ಬಫರ್_REF(buf);
    bp->buf = INVALID_ಬಫರ್_ಮೌಲ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#ಇಲ್ಲದಿದ್ದರೆ
    scheme_gc_ptr_ok(bp);
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_mzscheme_ref = ಖಾಲಿ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    ಶೂನ್ಯ
mzscheme_ಕಿಟಕಿ_free(win_T *win)
{
    ಆದರೆ (win->w_mzscheme_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_mz_ಕಿಟಕಿ *wp = ಖಾಲಿ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, wp);
    MZ_GC_REG();
    wp = ಕಿಟಕಿ_REF(win);
    wp->win = INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#ಇಲ್ಲದಿದ್ದರೆ
    scheme_gc_ptr_ok(wp);
#ಆದರೆ_ಅಂತ್ಯ
    win->w_mzscheme_ref = ಖಾಲಿ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    ಶೂನ್ಯ
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (script == ಖಾಲಿ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ಸ್ಥಿರ Scheme_Object *
do_ತುಂಬು(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ noargc ಬಳಸದ, Scheme_Object **noargv ಬಳಸದ)
{
    Scheme_Object   *ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    Scheme_Object   *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    ಅಕ್ಷರ	    *file = ಖಾಲಿ;
    ಪೋರ್ಟ್_Info	    *pinfo = (ಪೋರ್ಟ್_Info *)ದತ್ತಾಂಶ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ಒಳ_REG(0, ಅಭಿವ್ಯಕ್ತಿ);
    MZ_GC_VAR_ಒಳ_REG(1, ಫಲಿತಾಂಶ);
    MZ_GC_VAR_ಒಳ_REG(2, file);
    MZ_GC_REG();

    file = (ಅಕ್ಷರ *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->ಪೋರ್ಟ್ = scheme_open_ಒಳಹರಿವು_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು(pinfo->ಪೋರ್ಟ್);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    ಆಗ (!SCHEME_EOFP(ಅಭಿವ್ಯಕ್ತಿ = scheme_read(pinfo->ಪೋರ್ಟ್)))
    {
	ಫಲಿತಾಂಶ = scheme_eval(ಅಭಿವ್ಯಕ್ತಿ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್(pinfo->ಪೋರ್ಟ್);
    MZ_GC_CHECK();
    pinfo->ಪೋರ್ಟ್ = ಖಾಲಿ;
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * :mzfile
 */
    ಶೂನ್ಯ
ex_mzfile(exarg_T *eap)
{
    ಪೋರ್ಟ್_Info	pinfo = {ಖಾಲಿ, ಖಾಲಿ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, pinfo.ಪೋರ್ಟ್);
    MZ_GC_REG();

    pinfo.name = (ಅಕ್ಷರ *)eap->arg;
    ಆದರೆ (do_mzscheme_command(eap, &pinfo, do_ತುಂಬು) != OK
	    && pinfo.ಪೋರ್ಟ್ != ಖಾಲಿ)	// looks like port was not closed
    {
	scheme_close_ಒಳಹರಿವು_ಪೋರ್ಟ್(pinfo.ಪೋರ್ಟ್);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
init_exn_catching_apply(ಶೂನ್ಯ)
{
    ಆದರೆ (exn_catching_apply)
	ಹಿಂತಿರುಗಿ;

    ಅಕ್ಷರ *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_ಮೌಲ್ಯ("exn?");
    MZ_GC_CHECK();
    exn_ಸಂದೇಶ = scheme_builtin_ಮೌಲ್ಯ("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ಸ್ಥಿರ Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    ಆದರೆ (SCHEME_TRUEP(SCHEME_CAR(v)))
	ಹಿಂತಿರುಗಿ SCHEME_CDR(v);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*exn = SCHEME_CDR(v);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ Scheme_Object *
ಹೊರತೆಗೆ_exn_ಸಂದೇಶ(Scheme_Object *v)
{
    ಆದರೆ (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	ಹಿಂತಿರುಗಿ _scheme_apply(exn_ಸಂದೇಶ, 1, &v);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ; // Not an exn structure
}

    ಸ್ಥಿರ Scheme_Object *
do_eval(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಂಕ noargc ಬಳಸದ, Scheme_Object **noargv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ scheme_eval_string_all((ಅಕ್ಷರ *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_intrnl_output(ಅಕ್ಷರ *mesg, ಪೂರ್ಣಾಂಕ error)
{
    ಅಕ್ಷರ *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    ಆಗ (p)
    {
	*p = '\0';
	ಆದರೆ (error)
	    emsg(prev);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    ಆದರೆ (error)
	emsg(prev);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ(prev);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_output(ಅಕ್ಷರ *mesg, OUTPUT_LEN_TYPE len ಬಳಸದ)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_err_output(ಅಕ್ಷರ *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_printf(ಅಕ್ಷರ *format, ...)
{
    do_intrnl_output(format, 1);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_flush(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ *buff;
    OUTPUT_LEN_TYPE ಉದ್ದಳತೆ;

    buff = scheme_get_sized_byte_string_output(curerr, &ಉದ್ದಳತೆ);
    MZ_GC_CHECK();
    ಆದರೆ (ಉದ್ದಳತೆ)
    {
	do_err_output(buff);
	ಹಿಂತಿರುಗಿ;
    }

    buff = scheme_get_sized_byte_string_output(curout, &ಉದ್ದಳತೆ);
    MZ_GC_CHECK();
    ಆದರೆ (ಉದ್ದಳತೆ)
	do_output(buff, ಉದ್ದಳತೆ);
}

/*
 *========================================================================
 *  4. Implementation of the Vim Features for MzScheme
 *========================================================================
 */

/*
 * (command {command-string})
 */
    ಸ್ಥಿರ Scheme_Object *
vim_command(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    Scheme_Object *cmd = ಖಾಲಿ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_ಮೌಲ್ಯ(cmd), ಖಾಲಿ, ಖಾಲಿ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 * (eval {expr-string})
 */
    ಸ್ಥಿರ Scheme_Object *
vim_eval(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    Scheme_Object	*ಫಲಿತಾಂಶ = ಖಾಲಿ;
    typval_T		*vim_ಫಲಿತಾಂಶ;
    Scheme_Object *ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, ಫಲಿತಾಂಶ);
    MZ_GC_VAR_ಒಳ_REG(1, ಅಭಿವ್ಯಕ್ತಿ);
    MZ_GC_REG();
    ಅಭಿವ್ಯಕ್ತಿ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_ಫಲಿತಾಂಶ = eval_ಅಭಿವ್ಯಕ್ತಿ(BYTE_STRING_ಮೌಲ್ಯ(ಅಭಿವ್ಯಕ್ತಿ), ಖಾಲಿ);

    ಆದರೆ (vim_ಫಲಿತಾಂಶ == ಖಾಲಿ)
	raise_vim_exn(_("invalid expression"));

    ಫಲಿತಾಂಶ = vim_ಗಮ್ಯ_mzscheme(vim_ಫಲಿತಾಂಶ);
    MZ_GC_CHECK();
    free_tv(vim_ಫಲಿತಾಂಶ);

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    ಹಿಂತಿರುಗಿ scheme_false;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * (range-start)
 */
    ಸ್ಥಿರ Scheme_Object *
get_range_ಆರಂಭ(
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ			argc ಬಳಸದ,
    Scheme_Object	**argv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ scheme_make_integer(range_ಆರಂಭ);
}

/*
 * (range-end)
 */
    ಸ್ಥಿರ Scheme_Object *
get_range_ಕೊನೆ(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ scheme_make_integer(range_ಕೊನೆ);
}

/*
 * (beep)
 */
    ಸ್ಥಿರ Scheme_Object *
mzscheme_beep(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
    vim_beep(BO_LANG);
    ಹಿಂತಿರುಗಿ scheme_void;
}

ಸ್ಥಿರ Scheme_Object *M_global = ಖಾಲಿ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ಸ್ಥಿರ Scheme_Object *
get_option(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    ದೀರ್ಘ	    ಮೌಲ್ಯ;
    ಅಕ್ಷರ	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ಖಾಲಿ;
    Scheme_Object   *name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, rval);
    MZ_GC_VAR_ಒಳ_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    ಆದರೆ (argc > 1)
    {
	ಆದರೆ (M_global == ಖಾಲಿ)
	{
	    MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_global);
	    M_global = scheme_intern_ಚಿಹ್ನೆ("global");
	    MZ_GC_CHECK();
	}

	ಆದರೆ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_ಬಫರ್(argv[1]);
	    scope = OPT_LOCAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_ಕಿಟಕಿ(argv[1]);

	    curwin = win;
	    curbuf = win->w_ಬಫರ್;
	    scope = OPT_LOCAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_ಮೌಲ್ಯ(BYTE_STRING_ಮೌಲ್ಯ(name), &ಮೌಲ್ಯ, (char_u **)&strval,
								  ಖಾಲಿ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    ಆಯ್ಕೆ (rc)
    {
    ಸ್ಥಿತಿ gov_bool:
    ಸ್ಥಿತಿ gov_ಸಂಖ್ಯೆ:
	MZ_GC_UNREG();
	ಹಿಂತಿರುಗಿ scheme_make_integer_ಮೌಲ್ಯ(ಮೌಲ್ಯ);
    ಸ್ಥಿತಿ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	ಹಿಂತಿರುಗಿ rval;
    ಸ್ಥಿತಿ gov_hidden_bool:
    ಸ್ಥಿತಿ gov_hidden_ಸಂಖ್ಯೆ:
    ಸ್ಥಿತಿ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    ಸ್ಥಿತಿ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_option(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    char_u	*command = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    Scheme_Object *cmd = ಖಾಲಿ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    ಆದರೆ (argc > 1)
    {
	ಆದರೆ (M_global == ಖಾಲಿ)
	{
	    MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_global);
	    M_global = scheme_intern_ಚಿಹ್ನೆ("global");
	    MZ_GC_CHECK();
	}

	ಆದರೆ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_ಬಫರ್(argv[1]);
	    scope = OPT_LOCAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_ಕಿಟಕಿ(argv[1]);
	    curwin = win;
	    curbuf = win->w_ಬಫರ್;
	    scope = OPT_LOCAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(cmd));
    MZ_GC_UNREG();
    do_ಗಣ(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ಸ್ಥಿರ Scheme_Object *
get_curr_win(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ (Scheme_Object *)get_vim_curr_ಕಿಟಕಿ();
}

/*
 * (win-count)
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_ಎಣಿಕೆ(
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ			argc ಬಳಸದ,
    Scheme_Object	**argv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    ಹಿಂತಿರುಗಿ scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_list(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    ಗಾಗಿ ( ; w != ಖಾಲಿ; w = w->w_ಮುಂದಿನ)
	ಆದರೆ (w->w_ಬಫರ್ == buf->buf)
	{
	    list = scheme_make_pair(ಕಿಟಕಿ_new(w), list);
	    MZ_GC_CHECK();
	}

    ಹಿಂತಿರುಗಿ list;
}

    ಸ್ಥಿರ Scheme_Object *
ಕಿಟಕಿ_new(win_T *win)
{
    vim_mz_ಕಿಟಕಿ *self = ಖಾಲಿ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    ಆದರೆ (win->w_mzscheme_ref != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (Scheme_Object *)ಕಿಟಕಿ_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ಗಾತ್ರ(vim_mz_ಕಿಟಕಿ));
    CLEAR_ಸೂಚಕ(self);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#ಆದರೆ_ಅಂತ್ಯ
    MZ_GC_CHECK();
    ಕಿಟಕಿ_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_ಕಿಟಕಿ_type;

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_ಅಂಕೆ(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    win_T	*win = get_ಕಿಟಕಿ_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    ಗಾಗಿ (wp = firstwin; wp != win; wp = wp->w_ಮುಂದಿನ)
	++nr;

    ಹಿಂತಿರುಗಿ scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_by_ಅಂಕೆ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    win_T	*win = firstwin;
    ಪೂರ್ಣಾಂಕ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಆದರೆ (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    ಗಾಗಿ ( ; win != ಖಾಲಿ; win = win->w_ಮುಂದಿನ, --fnum)
	ಆದರೆ (fnum == 1)	    // to be 1-based
	    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_new(win);

    ಹಿಂತಿರುಗಿ scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_ಬಫರ್(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win = get_ಕಿಟಕಿ_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ ಬಫರ್_new(win->win->w_ಬಫರ್);
}

/*
 * (get-win-height [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_ಎತ್ತರ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win = get_ಕಿಟಕಿ_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ scheme_make_integer(win->win->w_ಎತ್ತರ);
}

/*
 * (set-win-height {height} [window])
 */
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_ಕಿಟಕಿ_ಎತ್ತರ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win;
    win_T	    *savewin;
    ಪೂರ್ಣಾಂಕ		    ಎತ್ತರ;

    win = get_ಕಿಟಕಿ_arg(prim->name, 1, argc, argv);
    ಎತ್ತರ = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    savewin = curwin;
    curwin = win->win;
    win_setheight(ಎತ್ತರ);
    curwin = savewin;

    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 * (get-win-width [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಕಿಟಕಿ_ಅಗಲ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win = get_ಕಿಟಕಿ_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ scheme_make_integer(win->win->w_ಅಗಲ);
}

/*
 * (set-win-width {width} [window])
 */
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_ಕಿಟಕಿ_ಅಗಲ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win;
    win_T	    *savewin;
    ಪೂರ್ಣಾಂಕ		    ಅಗಲ = 0;

    win = get_ಕಿಟಕಿ_arg(prim->name, 1, argc, argv);
    ಅಗಲ = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    savewin = curwin;
    curwin = win->win;
    win_setwidth(ಅಗಲ);
    curwin = savewin;

    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ಸ್ಥಿರ Scheme_Object *
get_cursor(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win;
    pos_T	    pos;

    win = get_ಕಿಟಕಿ_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    ಹಿಂತಿರುಗಿ scheme_make_pair(scheme_make_integer_ಮೌಲ್ಯ((ದೀರ್ಘ)pos.lnum),
		    scheme_make_integer_ಮೌಲ್ಯ((ದೀರ್ಘ)pos.ಲಂಬಸಾಲು + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_cursor(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಕಿಟಕಿ   *win;
    ದೀರ್ಘ	    lnum = 0;
    ದೀರ್ಘ	    ಲಂಬಸಾಲು = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    sandbox_check();
#ಆದರೆ_ಅಂತ್ಯ
    win = get_ಕಿಟಕಿ_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    ಆದರೆ (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    ಲಂಬಸಾಲು = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_ಸಾಲು_range(lnum, win->win->w_ಬಫರ್);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    win->win->w_ಗಣ_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ಸ್ಥಿರ Scheme_Object *
mzscheme_open_ಬಫರ್(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ = 0;
    Scheme_Object   *onum = ಖಾಲಿ;
    Scheme_Object   *buf = ಖಾಲಿ;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ಒಳ_REG(0, onum);
    MZ_GC_VAR_ಒಳ_REG(1, buf);
    MZ_GC_VAR_ಒಳ_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    sandbox_check();
#ಆದರೆ_ಅಂತ್ಯ
    // TODO make open existing file
    ಅಂಕೆ = buflist_ಕೂಡಿಸು(BYTE_STRING_ಮೌಲ್ಯ(fname), BLN_LISTED | BLN_CURBUF);

    ಆದರೆ (ಅಂಕೆ == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(ಅಂಕೆ);
    buf = get_ಬಫರ್_by_ಅಂಕೆ(ದತ್ತಾಂಶ, 1, &onum);
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_by_ಅಂಕೆ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	ಆದರೆ (buf->b_fnum == fnum)
	    ಹಿಂತಿರುಗಿ ಬಫರ್_new(buf);

    ಹಿಂತಿರುಗಿ scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_by_name(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    buf_T	*buf;
    Scheme_Object *ಬಫರ್ = ಖಾಲಿ;
    Scheme_Object *fname = ಖಾಲಿ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, ಬಫರ್);
    MZ_GC_VAR_ಒಳ_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    ಬಫರ್ = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	ಆದರೆ (buf->b_ffname == ಖಾಲಿ || buf->b_sfname == ಖಾಲಿ)
	    // empty string
	{
	    ಆದರೆ (BYTE_STRING_ಮೌಲ್ಯ(fname)[0] == NUL)
		ಬಫರ್ = ಬಫರ್_new(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!fnamecmp(buf->b_ffname, BYTE_STRING_ಮೌಲ್ಯ(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_ಮೌಲ್ಯ(fname)))
	{
	    // either short or long filename matches
	    ಬಫರ್ = ಬಫರ್_new(buf);
	}
    }

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}

/*
 * (get-next-buff [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಮುಂದಿನ_ಬಫರ್(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    buf_T	*buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv)->buf;

    ಆದರೆ (buf->b_ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ scheme_false;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಬಫರ್_new(buf->b_ಮುಂದಿನ);
}

/*
 * (get-prev-buff [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_prev_ಬಫರ್(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    buf_T	*buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv)->buf;

    ಆದರೆ (buf->b_prev == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ scheme_false;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಬಫರ್_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_ಅಂಕೆ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_ಎಣಿಕೆ(
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ			argc ಬಳಸದ,
    Scheme_Object	**argv ಬಳಸದ)
{
    buf_T   *b;
    ಪೂರ್ಣಾಂಕ	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    ಹಿಂತಿರುಗಿ scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_name(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ scheme_make_byte_string((ಅಕ್ಷರ *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ಸ್ಥಿರ Scheme_Object *
get_curr_ಬಫರ್(
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ			argc ಬಳಸದ,
    Scheme_Object	**argv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ (Scheme_Object *)get_vim_curr_ಬಫರ್();
}

    ಸ್ಥಿರ Scheme_Object *
ಬಫರ್_new(buf_T *buf)
{
    vim_mz_ಬಫರ್ *self = ಖಾಲಿ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    ಆದರೆ (buf->b_mzscheme_ref)
	ಹಿಂತಿರುಗಿ (Scheme_Object *)ಬಫರ್_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ಗಾತ್ರ(vim_mz_ಬಫರ್));
    CLEAR_ಸೂಚಕ(self);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#ಆದರೆ_ಅಂತ್ಯ
    MZ_GC_CHECK();
    ಬಫರ್_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_ಬಫರ್_type;

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_ಗಾತ್ರ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf = get_ಬಫರ್_arg(prim->name, 0, argc, argv);

    ಹಿಂತಿರುಗಿ scheme_make_integer(buf->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_ಸಾಲು(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf;
    ಪೂರ್ಣಾಂಕ		    linenr;
    char_u	    *ಸಾಲು;

    buf = get_ಬಫರ್_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಸಾಲು = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_make_byte_string((ಅಕ್ಷರ *)ಸಾಲು);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ಸ್ಥಿರ Scheme_Object *
get_ಬಫರ್_ಸಾಲು_list(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf;
    ಪೂರ್ಣಾಂಕ		    i, hi, lo, n;
    Scheme_Object   *list = ಖಾಲಿ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, list);
    MZ_GC_REG();

    buf = get_ಬಫರ್_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    ಆದರೆ (lo < 0)
	lo = 0;

    ಆದರೆ (hi < 0)
	hi = 0;
    ಆದರೆ (hi < lo)
	hi = lo;

    n = hi - lo;

    ಗಾಗಿ (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (ಅಕ್ಷರ *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ list;
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
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_ಬಫರ್_ಸಾಲು(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf;
    Scheme_Object   *ಸಾಲು = ಖಾಲಿ;
    ಅಕ್ಷರ	    *save;
    ಪೂರ್ಣಾಂಕ		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, ಸಾಲು);
    MZ_GC_REG();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    sandbox_check();
#ಆದರೆ_ಅಂತ್ಯ
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಆದರೆ (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    ಸಾಲು = argv[1];
    buf = get_ಬಫರ್_arg(prim->name, 2, argc, argv);

    check_ಸಾಲು_range(n, buf->buf);

    ಆದರೆ (SCHEME_FALSEP(ಸಾಲು))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	ಆದರೆ (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	ಆದರೆ (buf->buf == curwin->w_ಬಫರ್)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಹಿಂತಿರುಗಿ scheme_void;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು);

	curbuf = buf->buf;

	ಆದರೆ (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_free(save);
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	ಆದರೆ (buf->buf == curwin->w_ಬಫರ್)
	    check_cursor_ಲಂಬಸಾಲು();

	MZ_GC_UNREG();
	raise_if_error();
	ಹಿಂತಿರುಗಿ scheme_void;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_array(ಅಕ್ಷರ **array)
{
    ಅಕ್ಷರ **curr = array;
    ಆಗ (*curr != ಖಾಲಿ)
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
    ಸ್ಥಿರ Scheme_Object *
ಗಣ_ಬಫರ್_ಸಾಲು_list(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf = ಖಾಲಿ;
    Scheme_Object   *ಸಾಲು_list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    i, old_len, new_len, hi, lo;
    ದೀರ್ಘ	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, ಸಾಲು_list);
    MZ_GC_REG();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    sandbox_check();
#ಆದರೆ_ಅಂತ್ಯ
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    ಆದರೆ (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    ಸಾಲು_list = argv[2];
    buf = get_ಬಫರ್_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    ಆದರೆ (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_ಸಾಲು_range(lo, buf->buf);	    // inclusive
    check_ಸಾಲು_range(hi - 1, buf->buf);	    // exclusive

    ಆದರೆ (SCHEME_FALSEP(ಸಾಲು_list) || SCHEME_NULLP(ಸಾಲು_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	ಆದರೆ (u_savedel((linenr_T)lo, (ದೀರ್ಘ)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < old_len; i++)
		ಆದರೆ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    ಆದರೆ (buf->buf == curwin->w_ಬಫರ್)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_ಸಾಲುಗಳು_mark((linenr_T)lo, (ದೀರ್ಘ)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಹಿಂತಿರುಗಿ scheme_void;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_ಉದ್ದಳತೆ(ಸಾಲು_list);
	MZ_GC_CHECK();
	ಆದರೆ (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ		**array = ಖಾಲಿ;
	    Scheme_Object   *ಸಾಲು = ಖಾಲಿ;
	    Scheme_Object   *rest = ಖಾಲಿ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_ಒಳ_REG(0, ಸಾಲು);
	    MZ_GC_VAR_ಒಳ_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(ಅಕ್ಷರ *, new_len + 1);

	    rest = ಸಾಲು_list;
	    ಗಾಗಿ (i = 0; i < new_len; ++i)
	    {
		ಸಾಲು = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		ಆದರೆ (!SCHEME_STRINGP(ಸಾಲು))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು);
	    }

	    curbuf = buf->buf;

	    ಆದರೆ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    ಗಾಗಿ (i = 0; i < old_len - new_len; ++i)
	    {
		ಆದರೆ (ml_delete((linenr_T)lo) == FAIL)
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
	    ಗಾಗಿ (i = 0; i < old_len && i < new_len; i++)
		ಆದರೆ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    ಆಗ (i < new_len)
	    {
		ಆದರೆ (ml_append((linenr_T)(lo + i - 1),
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
						  (ದೀರ್ಘ)MAXLNUM, (ದೀರ್ಘ)extra);
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು((linenr_T)lo, 0, (linenr_T)hi, (ದೀರ್ಘ)extra);

	ಆದರೆ (buf->buf == curwin->w_ಬಫರ್)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಹಿಂತಿರುಗಿ scheme_void;
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
    ಸ್ಥಿರ Scheme_Object *
ಸೇರಿಸು_ಬಫರ್_ಸಾಲು_list(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)ದತ್ತಾಂಶ;
    vim_mz_ಬಫರ್   *buf = ಖಾಲಿ;
    Scheme_Object   *list = ಖಾಲಿ;
    ಅಕ್ಷರ	    *str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    i, n, ಗಾತ್ರ_2;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, list);
    MZ_GC_REG();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    sandbox_check();
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    ಆದರೆ (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_ಬಫರ್_arg(prim->name, 2, argc, argv);

    ಆದರೆ (n != 0)	    // 0 can be used in insert
	check_ಸಾಲು_range(n, buf->buf);
    ಆದರೆ (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_ಗಮ್ಯ_ಸಾಲು(list);
	curbuf = buf->buf;

	ಆದರೆ (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_free(str);
	    appended_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	ಹಿಂತಿರುಗಿ scheme_void;
    }

    // List
    ಗಾತ್ರ_2 = scheme_proper_list_ಉದ್ದಳತೆ(list);
    MZ_GC_CHECK();
    ಆದರೆ (ಗಾತ್ರ_2 < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Scheme_Object   *ಸಾಲು = ಖಾಲಿ;
	Scheme_Object   *rest = ಖಾಲಿ;
	ಅಕ್ಷರ		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ಒಳ_REG(0, ಸಾಲು);
	MZ_GC_VAR_ಒಳ_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(ಅಕ್ಷರ *, ಗಾತ್ರ_2 + 1);

	rest = list;
	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; ++i)
	{
	    ಸಾಲು = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_ಗಮ್ಯ_ಸಾಲು(ಸಾಲು);
	}

	curbuf = buf->buf;

	ಆದರೆ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; ++i)
		ಆದರೆ (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    ಆದರೆ (i > 0)
		appended_ಸಾಲುಗಳು_mark((linenr_T)n, (ದೀರ್ಘ)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    ಹಿಂತಿರುಗಿ scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_bufferp(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv)
{
    ಆದರೆ (SCHEME_VIMBUFFERP(argv[0]))
	ಹಿಂತಿರುಗಿ scheme_true;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ scheme_false;
}

/*
 * (win? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_windowp(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv)
{
    ಆದರೆ (SCHEME_VIMWINDOWP(argv[0]))
	ಹಿಂತಿರುಗಿ scheme_true;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_ಬಫರ್_validp(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv)
{
    ಆದರೆ (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_ಬಫರ್ *)argv[0])->buf != INVALID_ಬಫರ್_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ scheme_true;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ scheme_false;
}

/*
 * (win-valid? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_ಕಿಟಕಿ_validp(ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ, ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv)
{
    ಆದರೆ (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_ಕಿಟಕಿ *)argv[0])->win != INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ scheme_true;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ scheme_false;
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
    ಸ್ಥಿರ ಅಕ್ಷರ *
string_ಗಮ್ಯ_ಸಾಲು(Scheme_Object *obj)
{
    ಅಕ್ಷರ	*scheme_str = ಖಾಲಿ;
    ಅಕ್ಷರ	*vim_str = ಖಾಲಿ;
    OUTPUT_LEN_TYPE	len;
    ಪೂರ್ಣಾಂಕ		i;

    scheme_str = scheme_display_ಗಮ್ಯ_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    ಆದರೆ (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ಆದರೆ (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    ಹಿಂತಿರುಗಿ vim_str;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ಸ್ಥಿರ Scheme_Object *
vim_ಗಮ್ಯ_mzscheme(typval_T *vim_ಮೌಲ್ಯ)
{
    Scheme_Object	*ಫಲಿತಾಂಶ = ಖಾಲಿ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_ಕೋಷ್ಟಕ	*ಭೇಟಿಯಾದ = ಖಾಲಿ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, ಫಲಿತಾಂಶ);
    MZ_GC_VAR_ಒಳ_REG(1, ಭೇಟಿಯಾದ);
    MZ_GC_REG();

    ಭೇಟಿಯಾದ = scheme_make_hash_ಕೋಷ್ಟಕ(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    ಫಲಿತಾಂಶ = vim_ಗಮ್ಯ_mzscheme_impl(vim_ಮೌಲ್ಯ, 1, ಭೇಟಿಯಾದ);

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ Scheme_Object *
vim_ಗಮ್ಯ_mzscheme_impl(
    typval_T		*vim_ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಂಕ			ಆಳ,
    Scheme_Hash_ಕೋಷ್ಟಕ	*ಭೇಟಿಯಾದ)
{
    Scheme_Object   *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    new_ಮೌಲ್ಯ_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, ಫಲಿತಾಂಶ);
    MZ_GC_VAR_ಒಳ_REG(1, ಭೇಟಿಯಾದ);
    MZ_GC_REG();

    // Avoid infinite recursion
    ಆದರೆ (ಆಳ > 100)
    {
	MZ_GC_UNREG();
	ಹಿಂತಿರುಗಿ scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    ಫಲಿತಾಂಶ = scheme_hash_get(ಭೇಟಿಯಾದ, (Scheme_Object *)vim_ಮೌಲ್ಯ);
    MZ_GC_CHECK();
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ) // found, do nothing
	new_ಮೌಲ್ಯ_2 = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_STRING)
    {
	ಫಲಿತಾಂಶ = scheme_make_byte_string((ಅಕ್ಷರ *)vim_ಮೌಲ್ಯ->vval.v_string);
	MZ_GC_CHECK();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	ಫಲಿತಾಂಶ = scheme_make_integer((ದೀರ್ಘ)vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ);
	MZ_GC_CHECK();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_FLOAT)
    {
	ಫಲಿತಾಂಶ = scheme_make_double((ದ್ವಿಗುಣ)vim_ಮೌಲ್ಯ->vval.v_float);
	MZ_GC_CHECK();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_LIST)
    {
	list_T		*list = vim_ಮೌಲ್ಯ->vval.v_list;
	listitem_T	*curr;

	ಆದರೆ (list == ಖಾಲಿ || list->lv_ಮೊದಲ == ಖಾಲಿ)
	    ಫಲಿತಾಂಶ = scheme_null;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Scheme_Object   *obj = ಖಾಲಿ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ಒಳ_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_ಕೊನೆಯ;
	    obj = vim_ಗಮ್ಯ_mzscheme_impl(&curr->li_tv, ಆಳ + 1, ಭೇಟಿಯಾದ);
	    ಫಲಿತಾಂಶ = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    ಆಗ (curr != list->lv_ಮೊದಲ)
	    {
		curr = curr->li_prev;
		obj = vim_ಗಮ್ಯ_mzscheme_impl(&curr->li_tv, ಆಳ + 1, ಭೇಟಿಯಾದ);
		ಫಲಿತಾಂಶ = scheme_make_pair(obj, ಫಲಿತಾಂಶ);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_DICT)
    {
	Scheme_Object	  *ಕೀಲಿ = ಖಾಲಿ;
	Scheme_Object	  *obj = ಖಾಲಿ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ಒಳ_REG(0, ಕೀಲಿ);
	MZ_GC_VAR_ಒಳ_REG(1, obj);
	MZ_GC_REG();

	ಫಲಿತಾಂಶ = (Scheme_Object *)scheme_make_hash_ಕೋಷ್ಟಕ(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	ಆದರೆ (vim_ಮೌಲ್ಯ->vval.v_dict != ಖಾಲಿ)
	{
	    hashtab_T	*ht = &vim_ಮೌಲ್ಯ->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_ಬಳಸಿದ;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	    {
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_ಗಮ್ಯ_mzscheme_impl(&di->di_tv, ಆಳ + 1, ಭೇಟಿಯಾದ);
		    ಕೀಲಿ = scheme_make_byte_string((ಅಕ್ಷರ *)hi->hi_ಕೀಲಿ);
		    MZ_GC_CHECK();
		    scheme_hash_ಗಣ((Scheme_Hash_ಕೋಷ್ಟಕ *)ಫಲಿತಾಂಶ, ಕೀಲಿ, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ಖಾಲಿ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((ಅಕ್ಷರ *)vim_ಮೌಲ್ಯ->vval.v_string);
	MZ_GC_CHECK();
	ಫಲಿತಾಂಶ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)BYTE_STRING_ಮೌಲ್ಯ(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_PARTIAL)
    {
	ಆದರೆ (vim_ಮೌಲ್ಯ->vval.v_partial == ಖಾಲಿ)
	    ಫಲಿತಾಂಶ = scheme_null;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Scheme_Object *funcname = ಖಾಲಿ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_ಒಳ_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (ಅಕ್ಷರ *)partial_name(vim_ಮೌಲ್ಯ->vval.v_partial));
	    MZ_GC_CHECK();
	    ಫಲಿತಾಂಶ = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)BYTE_STRING_ಮೌಲ್ಯ(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ಮೌಲ್ಯ->v_type == VAR_BOOL || vim_ಮೌಲ್ಯ->v_type == VAR_SPECIAL)
    {
	ಆದರೆ (vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ <= VVAL_TRUE)
	    ಫಲಿತಾಂಶ = scheme_make_integer((ದೀರ್ಘ)vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = scheme_null;
	MZ_GC_CHECK();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಫಲಿತಾಂಶ = scheme_void;
	new_ಮೌಲ್ಯ_2 = FALSE;
    }
    ಆದರೆ (new_ಮೌಲ್ಯ_2)
    {
	scheme_hash_ಗಣ(ಭೇಟಿಯಾದ, (Scheme_Object *)vim_ಮೌಲ್ಯ, ಫಲಿತಾಂಶ);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mzscheme_ಗಮ್ಯ_vim(Scheme_Object *obj, typval_T *tv)
{
    ಪೂರ್ಣಾಂಕ i, ಸ್ಥಿತಿ_2;
    Scheme_Hash_ಕೋಷ್ಟಕ	*ಭೇಟಿಯಾದ = ಖಾಲಿ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, obj);
    MZ_GC_VAR_ಒಳ_REG(1, ಭೇಟಿಯಾದ);
    MZ_GC_REG();

    ಭೇಟಿಯಾದ = scheme_make_hash_ಕೋಷ್ಟಕ(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    ಸ್ಥಿತಿ_2 = mzscheme_ಗಮ್ಯ_vim_impl(obj, tv, 1, ಭೇಟಿಯಾದ);
    ಗಾಗಿ (i = 0; i < ಭೇಟಿಯಾದ->ಗಾತ್ರ_2; ++i)
    {
	// free up remembered objects
	ಆದರೆ (ಭೇಟಿಯಾದ->vals[i] != ಖಾಲಿ)
	    free_tv((typval_T *)ಭೇಟಿಯಾದ->vals[i]);
    }

    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mzscheme_ಗಮ್ಯ_vim_impl(Scheme_Object *obj, typval_T *tv, ಪೂರ್ಣಾಂಕ ಆಳ,
	Scheme_Hash_ಕೋಷ್ಟಕ *ಭೇಟಿಯಾದ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    typval_T	*ಸಿಕ್ಕಿತು;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, obj);
    MZ_GC_VAR_ಒಳ_REG(1, ಭೇಟಿಯಾದ);
    MZ_GC_REG();

    MZ_GC_CHECK();
    ಆದರೆ (ಆಳ > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಸಿಕ್ಕಿತು = (typval_T *)scheme_hash_get(ಭೇಟಿಯಾದ, obj);
    ಆದರೆ (ಸಿಕ್ಕಿತು != ಖಾಲಿ)
	copy_tv(ಸಿಕ್ಕಿತು, tv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = SCHEME_INT_VAL(obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_ಸಂಖ್ಯೆ = SCHEME_TRUEP(obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(obj));
    }
# ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ಖಾಲಿ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_ಒಳ_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_ಗಮ್ಯ_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(tmp));
	MZ_GC_UNREG();
    }
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	ಆದರೆ (list == ಖಾಲಿ)
	    ಸ್ಥಿತಿ_2 = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		    i;
	    Scheme_Object   *curr = ಖಾಲಿ;
	    Scheme_Object   *cval = ಖಾಲಿ;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_ಒಳ_REG(0, curr);
	    MZ_GC_VAR_ಒಳ_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    ಆದರೆ (v == ಖಾಲಿ)
		ಸ್ಥಿತಿ_2 = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *ಭೇಟಿಯಾದ_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, ಭೇಟಿಯಾದ_tv);
		scheme_hash_ಗಣ(ಭೇಟಿಯಾದ, obj, (Scheme_Object *)ಭೇಟಿಯಾದ_tv);

		ಆದರೆ (SCHEME_VECTORP(obj))
		{
		    ಗಾಗಿ (i = 0; i < SCHEME_VEC_ಗಾತ್ರ(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			ಸ್ಥಿತಿ_2 = mzscheme_ಗಮ್ಯ_vim_impl(cval, v, ಆಳ + 1, ಭೇಟಿಯಾದ);
			ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ_2 = list_append_tv(list, v);
			clear_tv(v);
			ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			    ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    ಗಾಗಿ (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			ಸ್ಥಿತಿ_2 = mzscheme_ಗಮ್ಯ_vim_impl(cval, v, ಆಳ + 1, ಭೇಟಿಯಾದ);
			ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ_2 = list_append_tv(list, v);
			clear_tv(v);
			ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			    ನಿಲ್ಲಿಸು;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && !SCHEME_NULLP(curr))
		    {
			ಸ್ಥಿತಿ_2 = mzscheme_ಗಮ್ಯ_vim_impl(cval, v, ಆಳ + 1, ಭೇಟಿಯಾದ);
			ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
			{
			    ಸ್ಥಿತಿ_2 = list_append_tv(list, v);
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SCHEME_HASHTP(obj))
    {
	ಪೂರ್ಣಾಂಕ		i;
	dict_T		*dict;
	Scheme_Object   *ಕೀಲಿ = ಖಾಲಿ;
	Scheme_Object   *val = ಖಾಲಿ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_ಒಳ_REG(0, ಕೀಲಿ);
	MZ_GC_VAR_ಒಳ_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	ಆದರೆ (dict == ಖಾಲಿ)
	    ಸ್ಥಿತಿ_2 = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    typval_T    *ಭೇಟಿಯಾದ_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, ಭೇಟಿಯಾದ_tv);
	    scheme_hash_ಗಣ(ಭೇಟಿಯಾದ, obj, (Scheme_Object *)ಭೇಟಿಯಾದ_tv);

	    ಗಾಗಿ (i = 0; i < ((Scheme_Hash_ಕೋಷ್ಟಕ *)obj)->ಗಾತ್ರ_2; ++i)
	    {
		ಆದರೆ (((Scheme_Hash_ಕೋಷ್ಟಕ *) obj)->vals[i] != ಖಾಲಿ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *ಅಂಶ = dictitem_alloc((char_u *)string_ಗಮ್ಯ_ಸಾಲು(
				((Scheme_Hash_ಕೋಷ್ಟಕ *) obj)->ಕೀಲಿಗಳು[i]));
		    // convert Scheme val to Vim and add it to the dict
		    ಆದರೆ (mzscheme_ಗಮ್ಯ_vim_impl(((Scheme_Hash_ಕೋಷ್ಟಕ *) obj)->vals[i],
				    &ಅಂಶ->di_tv, ಆಳ + 1, ಭೇಟಿಯಾದ) == FAIL
			    || dict_ಕೂಡಿಸು(dict, ಅಂಶ) == FAIL)
		    {
			dictitem_free(ಅಂಶ);
			ಸ್ಥಿತಿ_2 = FAIL;
			ನಿಲ್ಲಿಸು;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_ಗಮ್ಯ_ಸಾಲು(obj);
    }
    MZ_GC_UNREG();
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ಸ್ಥಿರ Scheme_Object *
vim_funcref(ಶೂನ್ಯ *name, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    ಪೂರ್ಣಾಂಕ i;
    typval_T ವಾದಗಳು;
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = OK;
    Scheme_Object *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, ಫಲಿತಾಂಶ);
    MZ_GC_REG();

    ಫಲಿತಾಂಶ = scheme_void;
    ಆದರೆ (list == ಖಾಲಿ)
	ಸ್ಥಿತಿ_2 = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ವಾದಗಳು.v_type = VAR_LIST;
	ವಾದಗಳು.vval.v_list = list;
	++list->lv_refcount;
	ಗಾಗಿ (i = 0; ಸ್ಥಿತಿ_2 == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    ಆದರೆ (v == ಖಾಲಿ)
		ಸ್ಥಿತಿ_2 = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸ್ಥಿತಿ_2 = mzscheme_ಗಮ್ಯ_vim(argv[i], v);
		ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
		{
		    ಸ್ಥಿತಿ_2 = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_ಮೌಲ್ಯ((Scheme_Object *)name), &ವಾದಗಳು, &ret);
	    MZ_GC_CHECK();
	    ಫಲಿತಾಂಶ = vim_ಗಮ್ಯ_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&ವಾದಗಳು);
    MZ_GC_UNREG();
    ಆದರೆ (ಸ್ಥಿತಿ_2 != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    ಇಲ್ಲದಿದ್ದರೆ
	raise_if_error();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಶೂನ್ಯ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ಖಾಲಿ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_ಒಳ_REG(0, ret);
    MZ_GC_REG();

    ಆದರೆ (mzscheme_init())
    {
	MZ_GC_UNREG();
	ಹಿಂತಿರುಗಿ;
    }

    MZ_GC_CHECK();
    ಆದರೆ (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_ಗಮ್ಯ_vim(ret, rettv);

    MZ_GC_UNREG();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_error_check(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ರಿಜಿಸ್ಟರ್_vim_exn(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	nc = 0;
    ಪೂರ್ಣಾಂಕ i;
    Scheme_Object   *struct_exn = ಖಾಲಿ;
    Scheme_Object   *exn_name = ಖಾಲಿ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_ಒಳ_REG(0, struct_exn);
    MZ_GC_VAR_ಒಳ_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_ಚಿಹ್ನೆ("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_ಮೌಲ್ಯ("struct:exn");
    MZ_GC_CHECK();

    ಆದರೆ (vim_exn == ಖಾಲಿ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ಖಾಲಿ, 0, 0, ಖಾಲಿ, ಖಾಲಿ
#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR >= 299
		, ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
		);


    {
	Scheme_Object   **tmp = ಖಾಲಿ;
	Scheme_Object   *exn_ಹೆಸರುಗಳು[5] = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
	Scheme_Object   *exn_ಮೌಲ್ಯಗಳು[5] = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_ಒಳ_REG(0, exn_ಹೆಸರುಗಳು, 5);
	MZ_GC_ARRAY_VAR_ಒಳ_REG(3, exn_ಮೌಲ್ಯಗಳು, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_ಹೆಸರುಗಳು(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_ಹೆಸರುಗಳು, tmp, nc * ಗಾತ್ರ(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_ಮೌಲ್ಯಗಳು(vim_exn, exn_ಹೆಸರುಗಳು, nc, 0);
	mch_memmove(exn_ಮೌಲ್ಯಗಳು, tmp, nc * ಗಾತ್ರ(Scheme_Object *));
	MZ_GC_CHECK();

	ಗಾಗಿ (i = 0; i < nc; i++)
	{
	    scheme_ಕೂಡಿಸು_global_ಚಿಹ್ನೆ(exn_ಹೆಸರುಗಳು[i],
		    exn_ಮೌಲ್ಯಗಳು[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    ಶೂನ್ಯ
raise_vim_exn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೂಡಿಸು_info)
{
    ಅಕ್ಷರ	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ಖಾಲಿ, ಖಾಲಿ};
    Scheme_Object   *exn = ಖಾಲಿ;
    Scheme_Object   *byte_string = ಖಾಲಿ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_ಒಳ_REG(0, argv, 2);
    MZ_GC_VAR_ಒಳ_REG(3, exn);
    MZ_GC_VAR_ಒಳ_REG(4, byte_string);
    MZ_GC_REG();

    ಆದರೆ (ಕೂಡಿಸು_info != ಖಾಲಿ)
    {
	ಅಕ್ಷರ		*c_string = ಖಾಲಿ;
	Scheme_Object   *info = ಖಾಲಿ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_ಒಳ_REG(0, c_string);
	MZ_GC_VAR_ಒಳ_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(ಕೂಡಿಸು_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (ಪೂರ್ಣಾಂಕ)STRLEN(fmt), 1, &info, ಖಾಲಿ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_ಗಮ್ಯ_char_string(byte_string);
	SCHEME_ಗಣ_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_ಗಮ್ಯ_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#ಆದರೆ MZSCHEME_ಆವೃತ್ತಿ_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#ಇಲ್ಲದಿದ್ದರೆ
    argv[1] = scheme_current_continuation_marks(ಖಾಲಿ);
    MZ_GC_CHECK();
#ಆದರೆ_ಅಂತ್ಯ

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    ಶೂನ್ಯ
raise_if_error(ಶೂನ್ಯ)
{
    ಆದರೆ (vim_error_check())
	raise_vim_exn(ಖಾಲಿ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ಸ್ಥಿರ vim_mz_ಬಫರ್ *
get_ಬಫರ್_arg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ argnum, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    vim_mz_ಬಫರ್ *b;

    ಆದರೆ (argc < argnum + 1)
	ಹಿಂತಿರುಗಿ get_vim_curr_ಬಫರ್();
    ಆದರೆ (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_ಬಫರ್ *)argv[argnum];
    (ಶೂನ್ಯ)get_valid_ಬಫರ್(argv[argnum]);
    ಹಿಂತಿರುಗಿ b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ಸ್ಥಿರ vim_mz_ಕಿಟಕಿ *
get_ಕಿಟಕಿ_arg(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಪೂರ್ಣಾಂಕ argnum, ಪೂರ್ಣಾಂಕ argc, Scheme_Object **argv)
{
    vim_mz_ಕಿಟಕಿ *w;

    ಆದರೆ (argc < argnum + 1)
	ಹಿಂತಿರುಗಿ get_vim_curr_ಕಿಟಕಿ();
    w = (vim_mz_ಕಿಟಕಿ *)argv[argnum];
    ಆದರೆ (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (ಶೂನ್ಯ)get_valid_ಕಿಟಕಿ(argv[argnum]);
    ಹಿಂತಿರುಗಿ w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_ಬಫರ್(ಶೂನ್ಯ *obj)
{
    buf_T *buf = ((vim_mz_ಬಫರ್ *)obj)->buf;

    ಆದರೆ (buf == INVALID_ಬಫರ್_ಮೌಲ್ಯ)
	scheme_signal_error(_("buffer is invalid"));
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_ಕಿಟಕಿ(ಶೂನ್ಯ *obj)
{
    win_T *win = ((vim_mz_ಕಿಟಕಿ *)obj)->win;
    ಆದರೆ (win == INVALID_ಕಿಟಕಿ_ಮೌಲ್ಯ)
	scheme_signal_error(_("window is invalid"));
    ಹಿಂತಿರುಗಿ win;
}

    ಪೂರ್ಣಾಂಕ
mzthreads_allowed(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ mz_ಎಳೆಗಳು_allow;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸಾಲು_ಒಳ_range(linenr_T lnum, buf_T *buf)
{
    ಹಿಂತಿರುಗಿ (lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಸಾಲು_range(linenr_T lnum, buf_T *buf)
{
    ಆದರೆ (!ಸಾಲು_ಒಳ_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mz_fix_cursor(ಪೂರ್ಣಾಂಕ lo, ಪೂರ್ಣಾಂಕ hi, ಪೂರ್ಣಾಂಕ extra)
{
    ಆದರೆ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ಆದರೆ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_ಲಂಬಸಾಲು();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    check_cursor_ಲಂಬಸಾಲು();
	ಬದಲಾಯಿತು_cline_bef_curs();
    }
    invalidate_botline();
}

ಸ್ಥಿರ Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_ಬಫರ್_ಸಾಲು, "get-buff-line", 1, 2},
    {ಗಣ_ಬಫರ್_ಸಾಲು, "set-buff-line", 2, 3},
    {get_ಬಫರ್_ಸಾಲು_list, "get-buff-line-list", 2, 3},
    {get_ಬಫರ್_name, "get-buff-name", 0, 1},
    {get_ಬಫರ್_ಅಂಕೆ, "get-buff-num", 0, 1},
    {get_ಬಫರ್_ಗಾತ್ರ, "get-buff-size", 0, 1},
    {ಗಣ_ಬಫರ್_ಸಾಲು_list, "set-buff-line-list", 3, 4},
    {ಸೇರಿಸು_ಬಫರ್_ಸಾಲು_list, "insert-buff-line-list", 2, 3},
    {get_curr_ಬಫರ್, "curr-buff", 0, 0},
    {get_ಬಫರ್_ಎಣಿಕೆ, "buff-count", 0, 0},
    {get_ಮುಂದಿನ_ಬಫರ್, "get-next-buff", 0, 1},
    {get_prev_ಬಫರ್, "get-prev-buff", 0, 1},
    {mzscheme_open_ಬಫರ್, "open-buff", 1, 1},
    {get_ಬಫರ್_by_name, "get-buff-by-name", 1, 1},
    {get_ಬಫರ್_by_ಅಂಕೆ, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_ಕಿಟಕಿ_ಎಣಿಕೆ, "win-count", 0, 0},
    {get_ಕಿಟಕಿ_by_ಅಂಕೆ, "get-win-by-num", 1, 1},
    {get_ಕಿಟಕಿ_ಅಂಕೆ, "get-win-num", 0, 1},
    {get_ಕಿಟಕಿ_ಬಫರ್, "get-win-buffer", 0, 1},
    {get_ಕಿಟಕಿ_ಎತ್ತರ, "get-win-height", 0, 1},
    {ಗಣ_ಕಿಟಕಿ_ಎತ್ತರ, "set-win-height", 1, 2},
    {get_ಕಿಟಕಿ_ಅಗಲ, "get-win-width", 0, 1},
    {ಗಣ_ಕಿಟಕಿ_ಅಗಲ, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {ಗಣ_cursor, "set-cursor", 1, 2},
    {get_ಕಿಟಕಿ_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_ಆರಂಭ, "range-start", 0, 0},
    {get_range_ಕೊನೆ, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {ಗಣ_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_ಬಫರ್_validp, "buff-valid?", 1, 1},
    {vim_ಕಿಟಕಿ_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    ಸ್ಥಿರ vim_mz_ಬಫರ್ *
get_vim_curr_ಬಫರ್(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf->b_mzscheme_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (vim_mz_ಬಫರ್ *)ಬಫರ್_new(curbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಬಫರ್_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ಸ್ಥಿರ vim_mz_ಕಿಟಕಿ *
get_vim_curr_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_mzscheme_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (vim_mz_ಕಿಟಕಿ *)ಕಿಟಕಿ_new(curwin);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಕಿಟಕಿ_REF(curwin);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
make_modules(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		    i;
    Scheme_Env	    *mod = ಖಾಲಿ;
    Scheme_Object   *vimext_ಚಿಹ್ನೆ = ಖಾಲಿ;
    Scheme_Object   *closed_prim = ಖಾಲಿ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_ಒಳ_REG(0, mod);
    MZ_GC_VAR_ಒಳ_REG(1, vimext_ಚಿಹ್ನೆ);
    MZ_GC_VAR_ಒಳ_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_ಚಿಹ್ನೆ = scheme_intern_ಚಿಹ್ನೆ("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_ಚಿಹ್ನೆ, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_ಕೂಡಿಸು_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_ಮುಕ್ತಾಯ_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
ಸ್ಥಿರ Scheme_Object *M_write = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *M_read = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *M_execute = ಖಾಲಿ;
ಸ್ಥಿರ Scheme_Object *M_delete = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
sandbox_check(ಶೂನ್ಯ)
{
    ಆದರೆ (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ಸ್ಥಿರ Scheme_Object *
sandbox_file_guard(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv)
{
    ಆದರೆ (!sandbox)
	ಹಿಂತಿರುಗಿ scheme_void;

    Scheme_Object *requested_access = argv[2];

    ಆದರೆ (M_write == ಖಾಲಿ)
    {
	MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_write);
	M_write = scheme_intern_ಚಿಹ್ನೆ("write");
	MZ_GC_CHECK();
    }
    ಆದರೆ (M_read == ಖಾಲಿ)
    {
	MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_read);
	M_read = scheme_intern_ಚಿಹ್ನೆ("read");
	MZ_GC_CHECK();
    }
    ಆದರೆ (M_execute == ಖಾಲಿ)
    {
	MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_execute);
	M_execute = scheme_intern_ಚಿಹ್ನೆ("execute");
	MZ_GC_CHECK();
    }
    ಆದರೆ (M_delete == ಖಾಲಿ)
    {
	MZ_ರಿಜಿಸ್ಟರ್_STATIC(M_delete);
	M_delete = scheme_intern_ಚಿಹ್ನೆ("delete");
	MZ_GC_CHECK();
    }

    ಆಗ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *ಅಂಶ = SCHEME_CAR(requested_access);
	ಆದರೆ (scheme_eq(ಅಂಶ, M_write) || scheme_eq(ಅಂಶ, M_read)
		|| scheme_eq(ಅಂಶ, M_execute) || scheme_eq(ಅಂಶ, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    ಹಿಂತಿರುಗಿ scheme_void;
}

    ಸ್ಥಿರ Scheme_Object *
sandbox_ಜಾಲ_guard(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, Scheme_Object **argv ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ scheme_void;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
