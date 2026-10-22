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
#ಯದಿ defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#ಯದಿ MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#ಯದಿ defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_ಮೌಲ್ಯ ((buf_T *)(-1))

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_ಮೌಲ್ಯ ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    Scheme_Closed_Prim	*prim;
    ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		mina;	// arity information
    ಪೂರ್ಣಾಙ್ಕ		maxa;
} Vim_Prim;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ವರ್ಣ	    *name;
    Scheme_Object   *port;
} Port_Info;

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
#ifdef HAVE_SANDBOX
ಸ್ಥಿರ Scheme_Object *sandbox_file_guard(ಪೂರ್ಣಾಙ್ಕ, Scheme_Object **);
ಸ್ಥಿರ Scheme_Object *sandbox_network_guard(ಪೂರ್ಣಾಙ್ಕ, Scheme_Object **);
ಸ್ಥಿರ ಶೂನ್ಯ sandbox_check(ಶೂನ್ಯ);
#endif
//  Buffer-related commands
ಸ್ಥಿರ Scheme_Object *buffer_new(buf_T *buf);
ಸ್ಥಿರ Scheme_Object *get_buffer_by_ಅಂಕೆ(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, Scheme_Object **);
ಸ್ಥಿರ vim_mz_buffer *get_vim_curr_buffer(ಶೂನ್ಯ);

//  Window-related commands
ಸ್ಥಿರ Scheme_Object *window_new(win_T *win);
ಸ್ಥಿರ vim_mz_window *get_vim_curr_window(ಶೂನ್ಯ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vim_error_check(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_mzscheme_command(exarg_T *, ಶೂನ್ಯ *, Scheme_Closed_Prim *what);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ startup_mzscheme(ಶೂನ್ಯ);
ಸ್ಥಿರ ವರ್ಣ *string_to_ಸಾಲು(Scheme_Object *obj);
#ಯದಿ MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#ಅನ್ಯಥಾ
# define OUTPUT_LEN_TYPE long
#endif
ಸ್ಥಿರ ಶೂನ್ಯ do_output(ವರ್ಣ *mesg, OUTPUT_LEN_TYPE len);
ಸ್ಥಿರ ಶೂನ್ಯ do_printf(ವರ್ಣ *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ಸ್ಥಿರ ಶೂನ್ಯ do_flush(ಶೂನ್ಯ);
ಸ್ಥಿರ Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ಸ್ಥಿರ Scheme_Object *extract_exn_message(Scheme_Object *v);
ಸ್ಥಿರ Scheme_Object *do_eval(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ noargc, Scheme_Object **noargv);
ಸ್ಥಿರ Scheme_Object *do_load(ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ noargc, Scheme_Object **noargv);
ಸ್ಥಿರ ಶೂನ್ಯ register_vim_exn(ಶೂನ್ಯ);
ಸ್ಥಿರ vim_mz_buffer *get_buffer_arg(ಧ್ರುವ ವರ್ಣ *fname, ಪೂರ್ಣಾಙ್ಕ argnum,
	ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ vim_mz_window *get_window_arg(ಧ್ರುವ ವರ್ಣ *fname, ಪೂರ್ಣಾಙ್ಕ argnum,
	ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_in_range(linenr_T, buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ check_ಸಾಲು_range(linenr_T, buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mz_fix_cursor(ಪೂರ್ಣಾಙ್ಕ lo, ಪೂರ್ಣಾಙ್ಕ hi, ಪೂರ್ಣಾಙ್ಕ extra);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval_with_exn_handling(ಶೂನ್ಯ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ಸ್ಥಿರ ಶೂನ್ಯ make_modules(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ init_exn_catching_apply(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mzscheme_env_main(Scheme_Env *env, ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mzscheme_init(ಶೂನ್ಯ);
#ifdef FEAT_EVAL
ಸ್ಥಿರ Scheme_Object *vim_to_mzscheme(typval_T *vim_ಮೌಲ್ಯ);
ಸ್ಥಿರ Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ depth,
	Scheme_Hash_Table *visited);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, ಪೂರ್ಣಾಙ್ಕ depth,
	Scheme_Hash_Table *visited);
ಸ್ಥಿರ Scheme_Object *vim_funcref(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ buffer_size_proc(ಶೂನ್ಯ *obj UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ gc_BYTES_TO_ಪದಗಳು(ಪರಿಮಾಣ(vim_mz_buffer));
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ buffer_mark_proc(ಶೂನ್ಯ *obj)
{
    ಪ್ರತ್ಯಾಗಮ buffer_size_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ buffer_fixup_proc(ಶೂನ್ಯ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    ಪ್ರತ್ಯಾಗಮ buffer_size_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ window_size_proc(ಶೂನ್ಯ *obj UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ gc_BYTES_TO_ಪದಗಳು(ಪರಿಮಾಣ(vim_mz_window));
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ window_mark_proc(ಶೂನ್ಯ *obj)
{
    ಪ್ರತ್ಯಾಗಮ window_size_proc(obj);
}
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ window_fixup_proc(ಶೂನ್ಯ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    ಪ್ರತ್ಯಾಗಮ window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#ಅನ್ಯಥಾ
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#ಯದಿ defined(DYNAMIC_MZSCHEME)
ಸ್ಥಿರ Scheme_Object *dll_scheme_eof;
ಸ್ಥಿರ Scheme_Object *dll_scheme_false;
ಸ್ಥಿರ Scheme_Object *dll_scheme_void;
ಸ್ಥಿರ Scheme_Object *dll_scheme_null;
ಸ್ಥಿರ Scheme_Object *dll_scheme_true;

ಸ್ಥಿರ Scheme_Thread **dll_scheme_current_thread_ptr;

ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_console_printf_ptr)(ವರ್ಣ *str, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_console_output_ptr)(ವರ್ಣ *str, OUTPUT_LEN_TYPE len);
ಸ್ಥಿರ ಶೂನ್ಯ (**dll_scheme_notify_multithread_ptr)(ಪೂರ್ಣಾಙ್ಕ on);

ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc)(size_t size_in_bytes);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
ಸ್ಥಿರ Scheme_Env *(*dll_scheme_basic_env)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_check_threads)(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_register_static)(ಶೂನ್ಯ *ptr, long size);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_set_ರಾಶಿ_base)(ಶೂನ್ಯ *base, ಪೂರ್ಣಾಙ್ಕ no_auto_statics);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_add_global)(ಧ್ರುವ ವರ್ಣ *name, Scheme_Object *val,
	Scheme_Env *env);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_add_global_ಚಿಹ್ನೆ)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_rands,
	Scheme_Object **rands);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_builtin_ಮೌಲ್ಯ)(ಧ್ರುವ ವರ್ಣ *name);
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_path)(ಧ್ರುವ ವರ್ಣ *chars);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_close_ಒಳಹರಿವು_port)(Scheme_Object *port);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು)(Scheme_Object *port);
# ಯದಿ MZSCHEME_VERSION_MAJOR < 360
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_current_continuation_marks)(ಶೂನ್ಯ);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
ಸ್ಥಿರ ವರ್ಣ *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_rands_2, Scheme_Object **rands, ಪೂರ್ಣಾಙ್ಕ val);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_dont_gc_ptr)(ಶೂನ್ಯ *p);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval_string)(ಧ್ರುವ ವರ್ಣ *str,
	Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_eval_string_all)(ಧ್ರುವ ವರ್ಣ *str,
	Scheme_Env *env, ಪೂರ್ಣಾಙ್ಕ all);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
ಸ್ಥಿರ ವರ್ಣ *(*dll_scheme_format)(ವರ್ಣ *format, ಪೂರ್ಣಾಙ್ಕ flen, ಪೂರ್ಣಾಙ್ಕ argc,
	Scheme_Object **argv, long *rlen);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ವರ್ಣ *(*dll_scheme_format_utf8)(ವರ್ಣ *format, ಪೂರ್ಣಾಙ್ಕ flen, ಪೂರ್ಣಾಙ್ಕ argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, ಪೂರ್ಣಾಙ್ಕ pos);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_gc_ptr_ok)(ಶೂನ್ಯ *p);
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
ಸ್ಥಿರ ವರ್ಣ *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ವರ್ಣ *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_intern_ಚಿಹ್ನೆ)(ಧ್ರುವ ವರ್ಣ *name);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *ಚಿಹ್ನೆ,
	Scheme_Env *env);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, ಶೂನ್ಯ *data, ಧ್ರುವ ವರ್ಣ *name, mzshort mina,
     mzshort maxa);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_integer_ಮೌಲ್ಯ)(long i);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ಧ್ರುವ ವರ್ಣ *name, mzshort mina, mzshort maxa);
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_string)(ಧ್ರುವ ವರ್ಣ *chars);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_string_output_port)();
# ಅನ್ಯಥಾ
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_byte_string)(ಧ್ರುವ ವರ್ಣ *chars);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv);
ಸ್ಥಿರ Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ *ಎಣಿಕೆ_out);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_fields,
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ಸ್ಥಿರ Scheme_Object **(*dll_scheme_make_struct_ಮೌಲ್ಯಗಳು)(
	Scheme_Object *struct_type, Scheme_Object **names, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ,
	ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ Scheme_Type (*dll_scheme_make_type)(ಧ್ರುವ ವರ್ಣ *name);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_vector)(ಪೂರ್ಣಾಙ್ಕ size,
	Scheme_Object *fill);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_scheme_malloc_fail_ok)(ಶೂನ್ಯ *(*f)(size_t), size_t);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_open_ಒಳಹರಿವು_file)(ಧ್ರುವ ವರ್ಣ *name,
	ಧ್ರುವ ವರ್ಣ *who);
ಸ್ಥಿರ Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_scheme_proper_list_length)(Scheme_Object *list);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_raise)(Scheme_Object *exn);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_signal_error)(ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ, ...);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_wrong_type)(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *expected,
	ಪೂರ್ಣಾಙ್ಕ which, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv);
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_set_param)(Scheme_Config *c, ಪೂರ್ಣಾಙ್ಕ pos,
	Scheme_Object *o);
ಸ್ಥಿರ Scheme_Config *(*dll_scheme_current_config)(ಶೂನ್ಯ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
ಸ್ಥಿರ Scheme_Hash_Table *(*dll_scheme_make_hash_table)(ಪೂರ್ಣಾಙ್ಕ type);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *ಕೀಲಿ, Scheme_Object *ಮೌಲ್ಯ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *ಕೀಲಿ);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_double)(ದ್ವಿಗುಣ d);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_make_sized_byte_string)(ವರ್ಣ *chars,
	long len, ಪೂರ್ಣಾಙ್ಕ copy);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ಸ್ಥಿರ Scheme_Object *(*dll_scheme_dynamic_wind)(ಶೂನ್ಯ (*pre)(ಶೂನ್ಯ *), Scheme_Object *(* ಚಞ್ಚಲ act)(ಶೂನ್ಯ *), ಶೂನ್ಯ (* ಚಞ್ಚಲ post)(ಶೂನ್ಯ *), Scheme_Object *(*jmp_handler)(ಶೂನ್ಯ *), ಶೂನ್ಯ * ಚಞ್ಚಲ data);
# ifdef MZ_PRECISE_GC
ಸ್ಥಿರ ಶೂನ್ಯ *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, ಪೂರ್ಣಾಙ್ಕ is_constant_size, ಪೂರ್ಣಾಙ್ಕ is_atomic);
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 400
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ಸ್ಥಿರ ಶೂನ್ಯ **(*dll_scheme_malloc_immobile_box)(ಶೂನ್ಯ *p);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_free_immobile_box)(ಶೂನ್ಯ **b);
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_scheme_main_setup)(ಪೂರ್ಣಾಙ್ಕ no_auto_statics, Scheme_Env_Main _main, ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);
#   ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_register_tls_space)(ಶೂನ್ಯ *tls_space, ಪೂರ್ಣಾಙ್ಕ _tls_index);
#   endif
#  endif
#  ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
ಸ್ಥಿರ Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(ಶೂನ್ಯ);
#  endif
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 600
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_embedded_load)(intptr_t len, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ predefined);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_register_embedded_load)(intptr_t len, ಧ್ರುವ ವರ್ಣ *s);
ಸ್ಥಿರ ಶೂನ್ಯ (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# ಯದಿ defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  ಯದಿ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_ಚಿಹ್ನೆ dll_scheme_add_global_ಚಿಹ್ನೆ
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_ಮೌಲ್ಯ dll_scheme_builtin_ಮೌಲ್ಯ
#  ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_ಒಳಹರಿವು_port dll_scheme_close_ಒಳಹರಿವು_port
#  define scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು
#  define scheme_current_continuation_marks \
    dll_scheme_current_continuation_marks
#  define scheme_display dll_scheme_display
#  define scheme_display_to_string dll_scheme_display_to_string
#  define scheme_do_eval dll_scheme_do_eval
#  define scheme_dont_gc_ptr dll_scheme_dont_gc_ptr
#  define scheme_eq dll_scheme_eq
#  define scheme_eval dll_scheme_eval
#  define scheme_eval_string dll_scheme_eval_string
#  define scheme_eval_string_all dll_scheme_eval_string_all
#  define scheme_finish_primitive_module dll_scheme_finish_primitive_module
#  ಯದಿ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  ಅನ್ಯಥಾ
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  ಯದಿ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  ಅನ್ಯಥಾ
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
#  ಯದಿ MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  ಅನ್ಯಥಾ
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_ಮೌಲ್ಯಗಳು dll_scheme_make_struct_ಮೌಲ್ಯಗಳು
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_ಒಳಹರಿವು_file dll_scheme_open_ಒಳಹರಿವು_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_ರಾಶಿ_base dll_scheme_set_ರಾಶಿ_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_set_param dll_scheme_set_param
#   define scheme_current_config dll_scheme_current_config
#   define scheme_char_string_to_byte_string \
    dll_scheme_char_string_to_byte_string
#   define scheme_char_string_to_path \
    dll_scheme_char_string_to_path
#   define scheme_set_collects_path dll_scheme_set_collects_path
#  endif
#  define scheme_make_hash_table dll_scheme_make_hash_table
#  define scheme_hash_set dll_scheme_hash_set
#  define scheme_hash_get dll_scheme_hash_get
#  define scheme_make_double dll_scheme_make_double
#  define scheme_make_sized_byte_string dll_scheme_make_sized_byte_string
#  define scheme_namespace_require dll_scheme_namespace_require
#  define scheme_dynamic_wind dll_scheme_dynamic_wind
#  ifdef MZ_PRECISE_GC
#   define GC_malloc_one_tagged dll_GC_malloc_one_tagged
#   define GC_register_traversers dll_GC_register_traversers
#  endif
#  ಯದಿ MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  ಯದಿ MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   ಯದಿ MZSCHEME_VERSION_MAJOR >= 500
#    ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ವರ್ಣ    *name;
    ಶೂನ್ಯ    **ptr;
} Thunk_Info;

ಸ್ಥಿರ Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (ಶೂನ್ಯ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (ಶೂನ್ಯ **)&dll_GC_malloc_atomic},
    {ರಿಕ್ತ, ರಿಕ್ತ}};

ಸ್ಥಿರ Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (ಶೂನ್ಯ **)&dll_scheme_eof},
    {"scheme_false", (ಶೂನ್ಯ **)&dll_scheme_false},
    {"scheme_void", (ಶೂನ್ಯ **)&dll_scheme_void},
    {"scheme_null", (ಶೂನ್ಯ **)&dll_scheme_null},
    {"scheme_true", (ಶೂನ್ಯ **)&dll_scheme_true},
# ಯದಿ !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (ಶೂನ್ಯ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (ಶೂನ್ಯ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (ಶೂನ್ಯ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(ಶೂನ್ಯ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (ಶೂನ್ಯ **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (ಶೂನ್ಯ **)&dll_scheme_add_global_ಚಿಹ್ನೆ},
    {"scheme_apply", (ಶೂನ್ಯ **)&dll_scheme_apply},
    {"scheme_basic_env", (ಶೂನ್ಯ **)&dll_scheme_basic_env},
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (ಶೂನ್ಯ **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (ಶೂನ್ಯ **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (ಶೂನ್ಯ **)&dll_scheme_builtin_ಮೌಲ್ಯ},
    {"scheme_check_threads", (ಶೂನ್ಯ **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (ಶೂನ್ಯ **)&dll_scheme_close_ಒಳಹರಿವು_port},
    {"scheme_count_lines", (ಶೂನ್ಯ **)&dll_scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು},
    {"scheme_current_continuation_marks",
	(ಶೂನ್ಯ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (ಶೂನ್ಯ **)&dll_scheme_display},
    {"scheme_display_to_string", (ಶೂನ್ಯ **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (ಶೂನ್ಯ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (ಶೂನ್ಯ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (ಶೂನ್ಯ **)&dll_scheme_eq},
    {"scheme_eval", (ಶೂನ್ಯ **)&dll_scheme_eval},
    {"scheme_eval_string", (ಶೂನ್ಯ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (ಶೂನ್ಯ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(ಶೂನ್ಯ **)&dll_scheme_finish_primitive_module},
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (ಶೂನ್ಯ **)&dll_scheme_format},
# ಅನ್ಯಥಾ
    {"scheme_format_utf8", (ಶೂನ್ಯ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (ಶೂನ್ಯ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (ಶೂನ್ಯ **)&dll_scheme_gc_ptr_ok},
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(ಶೂನ್ಯ **)&dll_scheme_get_sized_string_output},
# ಅನ್ಯಥಾ
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
# ಯದಿ MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (ಶೂನ್ಯ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(ಶೂನ್ಯ **)&dll_scheme_make_string_output_port},
# ಅನ್ಯಥಾ
    {"scheme_make_byte_string", (ಶೂನ್ಯ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(ಶೂನ್ಯ **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(ಶೂನ್ಯ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (ಶೂನ್ಯ **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (ಶೂನ್ಯ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (ಶೂನ್ಯ **)&dll_scheme_make_struct_ಮೌಲ್ಯಗಳು},
    {"scheme_make_type", (ಶೂನ್ಯ **)&dll_scheme_make_type},
    {"scheme_make_vector", (ಶೂನ್ಯ **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (ಶೂನ್ಯ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (ಶೂನ್ಯ **)&dll_scheme_open_ಒಳಹರಿವು_file},
    {"scheme_primitive_module", (ಶೂನ್ಯ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (ಶೂನ್ಯ **)&dll_scheme_proper_list_length},
    {"scheme_raise", (ಶೂನ್ಯ **)&dll_scheme_raise},
    {"scheme_read", (ಶೂನ್ಯ **)&dll_scheme_read},
    {"scheme_register_static", (ಶೂನ್ಯ **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (ಶೂನ್ಯ **)&dll_scheme_set_ರಾಶಿ_base},
    {"scheme_signal_error", (ಶೂನ್ಯ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (ಶೂನ್ಯ **)&dll_scheme_wrong_type},
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (ಶೂನ್ಯ **)&dll_scheme_set_param},
    {"scheme_current_config", (ಶೂನ್ಯ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(ಶೂನ್ಯ **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (ಶೂನ್ಯ **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (ಶೂನ್ಯ **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (ಶೂನ್ಯ **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (ಶೂನ್ಯ **)&dll_scheme_hash_set},
    {"scheme_hash_get", (ಶೂನ್ಯ **)&dll_scheme_hash_get},
    {"scheme_make_double", (ಶೂನ್ಯ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (ಶೂನ್ಯ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (ಶೂನ್ಯ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (ಶೂನ್ಯ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (ಶೂನ್ಯ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (ಶೂನ್ಯ **)&dll_GC_register_traversers},
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (ಶೂನ್ಯ **)&dll_scheme_main_setup},
#   ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (ಶೂನ್ಯ **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (ಶೂನ್ಯ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (ಶೂನ್ಯ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (ಶೂನ್ಯ **)&dll_scheme_free_immobile_box},
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 500
#  ಯದಿ defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (ಶೂನ್ಯ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (ಶೂನ್ಯ **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (ಶೂನ್ಯ **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (ಶೂನ್ಯ **)&dll_scheme_set_config_path},
# endif
    {ರಿಕ್ತ, ರಿಕ್ತ}};

ಸ್ಥಿರ HINSTANCE hMzGC = 0;
ಸ್ಥಿರ HINSTANCE hMzSch = 0;

ಸ್ಥಿರ ಶೂನ್ಯ dynamic_mzscheme_end(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mzscheme_runtime_link_init(ವರ್ಣ *sch_dll, ವರ್ಣ *gc_dll,
	ಪೂರ್ಣಾಙ್ಕ verbose);

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mzscheme_runtime_link_init(ವರ್ಣ *sch_dll, ವರ್ಣ *gc_dll, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    Thunk_Info *thunk = ರಿಕ್ತ;

    ಯದಿ (hMzGC && hMzSch)
	ಪ್ರತ್ಯಾಗಮ OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    ಯದಿ (!hMzGC)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (!hMzSch)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (thunk = mzsch_imports; thunk->name; thunk++)
    {
	ಯದಿ ((*thunk->ptr =
		    (ಶೂನ್ಯ *)GetProcAddress(hMzSch, thunk->name)) == ರಿಕ್ತ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತಿ (thunk = mzgc_imports; thunk->name; thunk++)
    {
	ಯದಿ ((*thunk->ptr =
		    (ಶೂನ್ಯ *)GetProcAddress(hMzGC, thunk->name)) == ರಿಕ್ತ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
mzscheme_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪ್ರತ್ಯಾಗಮ mzscheme_runtime_link_init(
	    (ವರ್ಣ *)p_mzschemedll, (ವರ್ಣ *)p_mzschemegcdll, verbose) == OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dynamic_mzscheme_end(ಶೂನ್ಯ)
{
    ಯದಿ (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    ಯದಿ (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#ಯದಿ MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, ಅಂಕೆ) GUARANTEE_STRING(proc, ಅಂಕೆ)
#ಅನ್ಯಥಾ
    ಸ್ಥಿರ Scheme_Object *
guaranteed_byte_string_arg(ವರ್ಣ *proc, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    ಯದಿ (SCHEME_BYTE_STRINGP(argv[ಅಂಕೆ]))
    {
	ಪ್ರತ್ಯಾಗಮ argv[ಅಂಕೆ];
    }
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_CHAR_STRINGP(argv[ಅಂಕೆ]))
    {
	Scheme_Object *tmp = ರಿಕ್ತ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[ಅಂಕೆ]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[ಅಂಕೆ]);
	MZ_GC_UNREG();
	ಪ್ರತ್ಯಾಗಮ tmp;
    }
    ಅನ್ಯಥಾ
	scheme_wrong_type(proc, "string", ಅಂಕೆ, argc, argv);
    // unreachable
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, ಅಂಕೆ) guaranteed_byte_string_arg(proc, ಅಂಕೆ, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#ಯದಿ defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ಸ್ಥಿರ Scheme_Type mz_buffer_type;
ಸ್ಥಿರ Scheme_Type mz_window_type;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ disabled = FALSE;
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ load_base_module_failed = FALSE;

// global environment
ಸ್ಥಿರ Scheme_Env    *environment = ರಿಕ್ತ;
// output/error handlers
ಸ್ಥಿರ Scheme_Object *curout = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *curerr = ರಿಕ್ತ;
// exn:vim exception
ಸ್ಥಿರ Scheme_Object *exn_catching_apply = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *exn_p = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *exn_message = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *vim_exn = ರಿಕ್ತ; // Vim Error exception

#ಯದಿ !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
ಸ್ಥಿರ ಶೂನ್ಯ *ರಾಶಿ_base = ರಿಕ್ತ;
#endif

ಸ್ಥಿರ long range_start;
ಸ್ಥಿರ long range_end;

// MzScheme threads scheduling stuff
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mz_threads_allow = 0;

#ಯದಿ defined(FEAT_GUI_MSWIN)
ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ಸ್ಥಿರ UINT timer_id = 0;
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_GTK)
ಸ್ಥಿರ gboolean timer_proc(gpointer);
ಸ್ಥಿರ guint timer_id = 0;
#ನೋಚೇದ್ಯದಿ defined(FEAT_GUI_MOTIF)
ಸ್ಥಿರ ಶೂನ್ಯ timer_proc(XtPointer, XtIntervalId *);
ಸ್ಥಿರ XtIntervalId timer_id = (XtIntervalId)0;
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    ಶೂನ್ಯ
mzvim_check_threads(ಶೂನ್ಯ)
{
    // Last time MzScheme threads were scheduled
    ಸ್ಥಿರ time_t mz_last_time = 0;

    ಯದಿ (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(ರಿಕ್ತ);

	ಯದಿ ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#ಯದಿ defined(MZSCHEME_GUI_THREADS)
ಸ್ಥಿರ ಶೂನ್ಯ setup_timer(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ remove_timer(ಶೂನ್ಯ);

// timers are presented in GUI only
# ಯದಿ defined(FEAT_GUI_MSWIN)
    ಸ್ಥಿರ ಶೂನ್ಯ CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_ಸಂದೇಶ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    ಸ್ಥಿರ gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    ಸ್ಥಿರ ಶೂನ್ಯ
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# ಯದಿ defined(FEAT_GUI_GTK)
    ಪ್ರತ್ಯಾಗಮ TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    ಯದಿ (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, ರಿಕ್ತ);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
setup_timer(ಶೂನ್ಯ)
{
# ಯದಿ defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(ರಿಕ್ತ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, ರಿಕ್ತ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, ರಿಕ್ತ);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_timer(ಶೂನ್ಯ)
{
# ಯದಿ defined(FEAT_GUI_MSWIN)
    KillTimer(ರಿಕ್ತ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    ವರ್ಣ *
did_set_mzquantum(optset_T *args UNUSED)
{
#ಯದಿ defined(MZSCHEME_GUI_THREADS)
    // reset timer
    ಯದಿ (timer_id != 0)
	remove_timer();
    ಯದಿ (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
notify_multithread(ಪೂರ್ಣಾಙ್ಕ on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    ಯದಿ (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    ಯದಿ (!on && timer_id != 0)
	remove_timer();
#endif
}

    ಶೂನ್ಯ
mzscheme_end(ಶೂನ್ಯ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#ಯದಿ 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#ಯದಿ HAVE_TLS_SPACE && !defined(VIMDLL)
# ಯದಿ defined(_MSC_VER)
ಸ್ಥಿರ __declspec(thread) ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t _tls_index;
# elif defined(__MINGW32__)
ಸ್ಥಿರ __thread ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t _tls_index;
# ಅನ್ಯಥಾ
ಸ್ಥಿರ THREAD_LOCAL ಶೂನ್ಯ *tls_space;
ಸ್ಥಿರ intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    ಪೂರ್ಣಾಙ್ಕ
mzscheme_main(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    argc = 0;
    ವರ್ಣ    *argv = ರಿಕ್ತ;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    ಯದಿ (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	ಪ್ರತ್ಯಾಗಮ vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    ಶೂನ್ಯ **ptls_space;
    intptr_t tls_index;
    ಶೂನ್ಯ (*pget_tls_info)(ಶೂನ್ಯ ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (ಶೂನ್ಯ *)GetProcAddress(
				GetModuleHandle(ರಿಕ್ತ), "get_tls_info");
    ಯದಿ (pget_tls_info == ರಿಕ್ತ)
    {
	disabled = TRUE;
	ಪ್ರತ್ಯಾಗಮ vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# ಅನ್ಯಥಾ
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    ಪ್ರತ್ಯಾಗಮ scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ mzscheme_env_main(ರಿಕ್ತ, argc, &argv);
#endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mzscheme_env_main(Scheme_Env *env, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, ವರ್ಣ **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#ಅನ್ಯಥಾ
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ರಿಕ್ತ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    ರಾಶಿ_base = &__gc_var_stack__;
# ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ dummy = 0;
    ರಾಶಿ_base = (ಶೂನ್ಯ *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ Scheme_Object*
load_base_module(ಶೂನ್ಯ *data)
{
    scheme_namespace_require(scheme_intern_ಚಿಹ್ನೆ((ವರ್ಣ *)data));
    ಪ್ರತ್ಯಾಗಮ scheme_null;
}

    ಸ್ಥಿರ Scheme_Object *
load_base_module_on_error(ಶೂನ್ಯ *data UNUSED)
{
    load_base_module_failed = TRUE;
    ಪ್ರತ್ಯಾಗಮ scheme_null;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
startup_mzscheme(ಶೂನ್ಯ)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_ರಾಶಿ_base(ರಾಶಿ_base, 1);
#endif

#ifndef TRAMPOLINED_MZVIM_STARTUP
    // in newer versions of precise GC the initial env has been created
    environment = scheme_basic_env();
#endif

    MZ_REGISTER_STATIC(environment);
    MZ_REGISTER_STATIC(curout);
    MZ_REGISTER_STATIC(curerr);
    MZ_REGISTER_STATIC(exn_catching_apply);
    MZ_REGISTER_STATIC(exn_p);
    MZ_REGISTER_STATIC(exn_message);
    MZ_REGISTER_STATIC(vim_exn);

    MZ_GC_CHECK();

#ifdef INCLUDE_MZSCHEME_BASE
    // invoke function from generated and included mzscheme_base.c
    declare_modules(environment);
#endif

    // setup 'current-library-collection-paths' parameter
#ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	ಯದಿ (s != ರಿಕ್ತ)
	{
	    coll_path = scheme_make_path((ವರ್ಣ *)s);
	    MZ_GC_CHECK();
	    ಯದಿ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	ಯದಿ (coll_path == ರಿಕ್ತ)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	ಯದಿ (coll_path != ರಿಕ್ತ)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ಅನ್ಯಥಾ
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ರಿಕ್ತ;
	Scheme_Object	*coll_pair = ರಿಕ್ತ;
	Scheme_Config	*config = ರಿಕ್ತ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, coll_string);
	MZ_GC_VAR_IN_REG(1, coll_pair);
	MZ_GC_VAR_IN_REG(2, config);
	MZ_GC_REG();
	coll_string = scheme_make_byte_string(MZSCHEME_COLLECTS);
	MZ_GC_CHECK();
	coll_pair = scheme_make_pair(coll_string, scheme_null);
	MZ_GC_CHECK();
	config = scheme_current_config();
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_COLLECTION_PATHS, coll_pair);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }
# endif
#endif

#ಯದಿ MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	ಯದಿ (s != ರಿಕ್ತ)
	{
	    config_path = scheme_make_path((ವರ್ಣ *)s);
	    MZ_GC_CHECK();
	    ಯದಿ (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	ಯದಿ (config_path == ರಿಕ್ತ)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	ಯದಿ (config_path != ರಿಕ್ತ)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#ಯದಿ MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ರಿಕ್ತ, load_base_module, ರಿಕ್ತ,
				    load_base_module_on_error, "racket/base");
	ಯದಿ (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(ರಿಕ್ತ, load_base_module, ರಿಕ್ತ,
				    load_base_module_on_error, "scheme/base");
	    ಯದಿ (load_base_module_failed)
		ಪ್ರತ್ಯಾಗಮ -1;
	}
    }

    register_vim_exn();
    // use new environment to initialise exception handling
    init_exn_catching_apply();

    // redirect output
    scheme_console_output = do_output;
    scheme_console_printf = do_printf;

#ifdef HAVE_SANDBOX
    {
	Scheme_Object	*make_security_guard = ರಿಕ್ತ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# ಯದಿ MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_ಚಿಹ್ನೆ = ರಿಕ್ತ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_ಚಿಹ್ನೆ);
	    MZ_GC_REG();
	    make_security_guard_ಚಿಹ್ನೆ = scheme_intern_ಚಿಹ್ನೆ("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_ಚಿಹ್ನೆ, environment);
	    MZ_GC_UNREG();
	}
# ಅನ್ಯಥಾ
	make_security_guard = scheme_builtin_ಮೌಲ್ಯ("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	ಯದಿ (make_security_guard != ರಿಕ್ತ)
	{
	    Scheme_Object   *args[3] = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
	    Scheme_Object   *guard = ರಿಕ್ತ;
	    Scheme_Config   *config = ರಿಕ್ತ;
	    MZ_GC_DECL_REG(5);
	    MZ_GC_ARRAY_VAR_IN_REG(0, args, 3);
	    MZ_GC_VAR_IN_REG(3, guard);
	    MZ_GC_VAR_IN_REG(4, config);
	    MZ_GC_REG();
	    config = scheme_current_config();
	    MZ_GC_CHECK();
	    args[0] = scheme_get_param(config, MZCONFIG_SECURITY_GUARD);
	    MZ_GC_CHECK();
	    args[1] = scheme_make_prim_w_arity(sandbox_file_guard,
		    "sandbox-file-guard", 3, 3);
	    args[2] = scheme_make_prim_w_arity(sandbox_network_guard,
		    "sandbox-network-guard", 4, 4);
	    guard = scheme_apply(make_security_guard, 3, args);
	    MZ_GC_CHECK();
	    scheme_set_param(config, MZCONFIG_SECURITY_GUARD, guard);
	    MZ_GC_CHECK();
	    MZ_GC_UNREG();
	}
	MZ_GC_UNREG();
    }
#endif
    // Create buffer and window types for use in Scheme code
    mz_buffer_type = scheme_make_type("<vim-buffer>");
    MZ_GC_CHECK();
    mz_window_type = scheme_make_type("<vim-window>");
    MZ_GC_CHECK();
#ifdef MZ_PRECISE_GC
    GC_register_traversers(mz_buffer_type,
	    buffer_size_proc, buffer_mark_proc, buffer_fixup_proc,
	    TRUE, TRUE);
    GC_register_traversers(mz_window_type,
	    window_size_proc, window_mark_proc, window_fixup_proc,
	    TRUE, TRUE);
#endif

    make_modules();

    /*
     * setup callback to receive notifications
     * whether thread scheduling is (or not) required
     */
    scheme_notify_multithread = notify_multithread;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mzscheme_init(ಶೂನ್ಯ)
{
    ಯದಿ (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	ಯದಿ (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
#endif
	ಯದಿ (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ರಿಕ್ತ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config);
	MZ_GC_REG();
	config = scheme_current_config();
	MZ_GC_CHECK();
	// recreate ports each call effectively clearing these ones
	curout = scheme_make_byte_string_output_port();
	MZ_GC_CHECK();
	curerr = scheme_make_byte_string_output_port();
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_OUTPUT_PORT, curout);
	MZ_GC_CHECK();
	scheme_set_param(config, MZCONFIG_ERROR_PORT, curerr);
	MZ_GC_CHECK();
	MZ_GC_UNREG();
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_with_exn_handling(
    ಶೂನ್ಯ		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *ಮೌಲ್ಯ = ರಿಕ್ತ;
    Scheme_Object   *exn = ರಿಕ್ತ;
    Scheme_Object   *prim = ರಿಕ್ತ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, ಮೌಲ್ಯ);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    ಮೌಲ್ಯ = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    ಯದಿ (!ಮೌಲ್ಯ)
    {
	ಮೌಲ್ಯ = extract_exn_message(exn);
	// Got an exn?
	ಯದಿ (ಮೌಲ್ಯ)
	{
	    scheme_display(ಮೌಲ್ಯ, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (ret != ರಿಕ್ತ)	// if pointer to retval supported give it up
	*ret = ಮೌಲ್ಯ;
    // Print any result, as long as it's not a void
    ಅನ್ಯಥಾ ಯದಿ (!SCHEME_VOIDP(ಮೌಲ್ಯ))
    {
	scheme_display(ಮೌಲ್ಯ, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * :mzscheme
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_mzscheme_command(exarg_T *eap, ಶೂನ್ಯ *data, Scheme_Closed_Prim *what)
{
    ಯದಿ (mzscheme_init())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    ಪ್ರತ್ಯಾಗಮ eval_with_exn_handling(data, what, ರಿಕ್ತ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    ಶೂನ್ಯ
mzscheme_buffer_free(buf_T *buf)
{
    ಯದಿ (buf->b_mzscheme_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_mz_buffer *bp = ರಿಕ್ತ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_ಮೌಲ್ಯ;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#ಅನ್ಯಥಾ
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = ರಿಕ್ತ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    ಶೂನ್ಯ
mzscheme_window_free(win_T *win)
{
    ಯದಿ (win->w_mzscheme_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_mz_window *wp = ರಿಕ್ತ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_ಮೌಲ್ಯ;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#ಅನ್ಯಥಾ
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = ರಿಕ್ತ;
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
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (script == ರಿಕ್ತ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    ಅನ್ಯಥಾ
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ಸ್ಥಿರ Scheme_Object *
do_load(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    Scheme_Object   *result = ರಿಕ್ತ;
    ವರ್ಣ	    *file = ರಿಕ್ತ;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, ಅಭಿವ್ಯಕ್ತಿ);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (ವರ್ಣ *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_ಒಳಹರಿವು_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ಎಣಿಕೆ_ಸಾಲುಗಳು(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    ಯಾವತ್ (!SCHEME_EOFP(ಅಭಿವ್ಯಕ್ತಿ = scheme_read(pinfo->port)))
    {
	result = scheme_eval(ಅಭಿವ್ಯಕ್ತಿ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_ಒಳಹರಿವು_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = ರಿಕ್ತ;
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * :mzfile
 */
    ಶೂನ್ಯ
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {ರಿಕ್ತ, ರಿಕ್ತ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (ವರ್ಣ *)eap->arg;
    ಯದಿ (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != ರಿಕ್ತ)	// looks like port was not closed
    {
	scheme_close_ಒಳಹರಿವು_port(pinfo.port);
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
    ಯದಿ (exn_catching_apply)
	ಪ್ರತ್ಯಾಗಮ;

    ವರ್ಣ *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_ಮೌಲ್ಯ("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_ಮೌಲ್ಯ("exn-message");
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

    ಯದಿ (SCHEME_TRUEP(SCHEME_CAR(v)))
	ಪ್ರತ್ಯಾಗಮ SCHEME_CDR(v);
    ಅನ್ಯಥಾ
    {
	*exn = SCHEME_CDR(v);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    ಯದಿ (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	ಪ್ರತ್ಯಾಗಮ _scheme_apply(exn_message, 1, &v);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; // Not an exn structure
}

    ಸ್ಥಿರ Scheme_Object *
do_eval(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ scheme_eval_string_all((ವರ್ಣ *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_intrnl_output(ವರ್ಣ *mesg, ಪೂರ್ಣಾಙ್ಕ error)
{
    ವರ್ಣ *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    ಯಾವತ್ (p)
    {
	*p = '\0';
	ಯದಿ (error)
	    emsg(prev);
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    ಯದಿ (error)
	emsg(prev);
    ಅನ್ಯಥಾ
	ಸಂದೇಶ(prev);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_output(ವರ್ಣ *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_err_output(ವರ್ಣ *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_printf(ವರ್ಣ *format, ...)
{
    do_intrnl_output(format, 1);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_flush(ಶೂನ್ಯ)
{
    ವರ್ಣ *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    ಯದಿ (length)
    {
	do_err_output(buff);
	ಪ್ರತ್ಯಾಗಮ;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    ಯದಿ (length)
	do_output(buff, length);
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
vim_command(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ರಿಕ್ತ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_ಮೌಲ್ಯ(cmd), ರಿಕ್ತ, ರಿಕ್ತ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

/*
 * (eval {expr-string})
 */
    ಸ್ಥಿರ Scheme_Object *
vim_eval(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = ರಿಕ್ತ;
    typval_T		*vim_result;
    Scheme_Object *ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, ಅಭಿವ್ಯಕ್ತಿ);
    MZ_GC_REG();
    ಅಭಿವ್ಯಕ್ತಿ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_ಅಭಿವ್ಯಕ್ತಿ(BYTE_STRING_ಮೌಲ್ಯ(ಅಭಿವ್ಯಕ್ತಿ), ರಿಕ್ತ);

    ಯದಿ (vim_result == ರಿಕ್ತ)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    ಪ್ರತ್ಯಾಗಮ scheme_false;
#endif
}

/*
 * (range-start)
 */
    ಸ್ಥಿರ Scheme_Object *
get_range_start(
    ಶೂನ್ಯ		*data UNUSED,
    ಪೂರ್ಣಾಙ್ಕ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    ಸ್ಥಿರ Scheme_Object *
get_range_end(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    ಸ್ಥಿರ Scheme_Object *
mzscheme_beep(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

ಸ್ಥಿರ Scheme_Object *M_global = ರಿಕ್ತ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ಸ್ಥಿರ Scheme_Object *
get_option(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    ಮೌಲ್ಯ;
    ವರ್ಣ	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ರಿಕ್ತ;
    Scheme_Object   *name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    ಯದಿ (argc > 1)
    {
	ಯದಿ (M_global == ರಿಕ್ತ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ಚಿಹ್ನೆ("global");
	    MZ_GC_CHECK();
	}

	ಯದಿ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ಅನ್ಯಥಾ ಯದಿ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	ಅನ್ಯಥಾ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_ಮೌಲ್ಯ(BYTE_STRING_ಮೌಲ್ಯ(name), &ಮೌಲ್ಯ, (char_u **)&strval,
								  ರಿಕ್ತ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    ವಿಕಲ್ಪ (rc)
    {
    ಪಕ್ಷ gov_bool:
    ಪಕ್ಷ gov_ಸಂಖ್ಯೆ:
	MZ_GC_UNREG();
	ಪ್ರತ್ಯಾಗಮ scheme_make_integer_ಮೌಲ್ಯ(ಮೌಲ್ಯ);
    ಪಕ್ಷ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	ಪ್ರತ್ಯಾಗಮ rval;
    ಪಕ್ಷ gov_hidden_bool:
    ಪಕ್ಷ gov_hidden_ಸಂಖ್ಯೆ:
    ಪಕ್ಷ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    ಪಕ್ಷ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ಸ್ಥಿರ Scheme_Object *
set_option(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    char_u	*command = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ರಿಕ್ತ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    ಯದಿ (argc > 1)
    {
	ಯದಿ (M_global == ರಿಕ್ತ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ಚಿಹ್ನೆ("global");
	    MZ_GC_CHECK();
	}

	ಯದಿ (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ಅನ್ಯಥಾ ಯದಿ (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	ಅನ್ಯಥಾ
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
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
get_curr_win(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_ಎಣಿಕೆ(
    ಶೂನ್ಯ		*data UNUSED,
    ಪೂರ್ಣಾಙ್ಕ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_list(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    ಪ್ರತಿ ( ; w != ರಿಕ್ತ; w = w->w_next)
	ಯದಿ (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    ಪ್ರತ್ಯಾಗಮ list;
}

    ಸ್ಥಿರ Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = ರಿಕ್ತ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    ಯದಿ (win->w_mzscheme_ref != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ಪರಿಮಾಣ(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ರಿಕ್ತ);
#ಅನ್ಯಥಾ
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_ಅಂಕೆ(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    ಪ್ರತಿ (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_by_ಅಂಕೆ(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    ಪೂರ್ಣಾಙ್ಕ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಯದಿ (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    ಪ್ರತಿ ( ; win != ರಿಕ್ತ; win = win->w_next, --fnum)
	ಯದಿ (fnum == 1)	    // to be 1-based
	    ಪ್ರತ್ಯಾಗಮ window_new(win);

    ಪ್ರತ್ಯಾಗಮ scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_buffer(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_height(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    ಸ್ಥಿರ Scheme_Object *
set_window_height(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    ಪೂರ್ಣಾಙ್ಕ		    height;

    win = get_window_arg(prim->name, 1, argc, argv);
    height = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    savewin = curwin;
    curwin = win->win;
    win_setheight(height);
    curwin = savewin;

    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

/*
 * (get-win-width [window])
 */
    ಸ್ಥಿರ Scheme_Object *
get_window_width(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    ಸ್ಥಿರ Scheme_Object *
set_window_width(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    ಪೂರ್ಣಾಙ್ಕ		    width = 0;

    win = get_window_arg(prim->name, 1, argc, argv);
    width = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    savewin = curwin;
    curwin = win->win;
    win_setwidth(width);
    curwin = savewin;

    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ಸ್ಥಿರ Scheme_Object *
get_cursor(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    ಪ್ರತ್ಯಾಗಮ scheme_make_pair(scheme_make_integer_ಮೌಲ್ಯ((long)pos.lnum),
		    scheme_make_integer_ಮೌಲ್ಯ((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ಸ್ಥಿರ Scheme_Object *
set_cursor(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    long	    lnum = 0;
    long	    col = 0;

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    win = get_window_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    ಯದಿ (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_ಸಾಲು_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
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
mzscheme_open_buffer(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ = 0;
    Scheme_Object   *onum = ರಿಕ್ತ;
    Scheme_Object   *buf = ರಿಕ್ತ;
    Scheme_Object   *fname;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, onum);
    MZ_GC_VAR_IN_REG(1, buf);
    MZ_GC_VAR_IN_REG(2, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    // TODO make open existing file
    ಅಂಕೆ = buflist_add(BYTE_STRING_ಮೌಲ್ಯ(fname), BLN_LISTED | BLN_CURBUF);

    ಯದಿ (ಅಂಕೆ == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(ಅಂಕೆ);
    buf = get_buffer_by_ಅಂಕೆ(data, 1, &onum);
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_by_ಅಂಕೆ(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	ಯದಿ (buf->b_fnum == fnum)
	    ಪ್ರತ್ಯಾಗಮ buffer_new(buf);

    ಪ್ರತ್ಯಾಗಮ scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_by_name(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = ರಿಕ್ತ;
    Scheme_Object *fname = ರಿಕ್ತ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	ಯದಿ (buf->b_ffname == ರಿಕ್ತ || buf->b_sfname == ರಿಕ್ತ)
	    // empty string
	{
	    ಯದಿ (BYTE_STRING_ಮೌಲ್ಯ(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	ಅನ್ಯಥಾ ಯದಿ (!fnamecmp(buf->b_ffname, BYTE_STRING_ಮೌಲ್ಯ(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_ಮೌಲ್ಯ(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ buffer;
}

/*
 * (get-next-buff [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_next_buffer(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    ಯದಿ (buf->b_next == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ scheme_false;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_prev_buffer(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    ಯದಿ (buf->b_prev == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ scheme_false;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_ಅಂಕೆ(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_ಎಣಿಕೆ(
    ಶೂನ್ಯ		*data UNUSED,
    ಪೂರ್ಣಾಙ್ಕ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    ಪೂರ್ಣಾಙ್ಕ	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_name(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ scheme_make_byte_string((ವರ್ಣ *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ಸ್ಥಿರ Scheme_Object *
get_curr_buffer(
    ಶೂನ್ಯ		*data UNUSED,
    ಪೂರ್ಣಾಙ್ಕ			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)get_vim_curr_buffer();
}

    ಸ್ಥಿರ Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = ರಿಕ್ತ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    ಯದಿ (buf->b_mzscheme_ref)
	ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ಪರಿಮಾಣ(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ರಿಕ್ತ);
#ಅನ್ಯಥಾ
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_size(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ಪ್ರತ್ಯಾಗಮ scheme_make_integer(buf->buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_ಸಾಲು(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    ಪೂರ್ಣಾಙ್ಕ		    linenr;
    char_u	    *ಸಾಲು;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಸಾಲು = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_make_byte_string((ವರ್ಣ *)ಸಾಲು);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ಸ್ಥಿರ Scheme_Object *
get_buffer_ಸಾಲು_list(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    ಪೂರ್ಣಾಙ್ಕ		    i, hi, lo, n;
    Scheme_Object   *list = ರಿಕ್ತ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, list);
    MZ_GC_REG();

    buf = get_buffer_arg(prim->name, 2, argc, argv);
    list = scheme_null;
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    /*
     * Handle some error conditions
     */
    ಯದಿ (lo < 0)
	lo = 0;

    ಯದಿ (hi < 0)
	hi = 0;
    ಯದಿ (hi < lo)
	hi = lo;

    n = hi - lo;

    ಪ್ರತಿ (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (ವರ್ಣ *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ list;
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
set_buffer_ಸಾಲು(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *ಸಾಲು = ರಿಕ್ತ;
    ವರ್ಣ	    *save;
    ಪೂರ್ಣಾಙ್ಕ		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ಸಾಲು);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ಯದಿ (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    ಸಾಲು = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_ಸಾಲು_range(n, buf->buf);

    ಯದಿ (SCHEME_FALSEP(ಸಾಲು))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	ಯದಿ (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	ಯದಿ (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
    }
    ಅನ್ಯಥಾ
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_ಸಾಲು(ಸಾಲು);

	curbuf = buf->buf;

	ಯದಿ (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	ಅನ್ಯಥಾ
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	ಯದಿ (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_array(ವರ್ಣ **array)
{
    ವರ್ಣ **curr = array;
    ಯಾವತ್ (*curr != ರಿಕ್ತ)
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
set_buffer_ಸಾಲು_list(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ರಿಕ್ತ;
    Scheme_Object   *ಸಾಲು_list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ಸಾಲು_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    ಯದಿ (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    ಸಾಲು_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    ಯದಿ (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_ಸಾಲು_range(lo, buf->buf);	    // inclusive
    check_ಸಾಲು_range(hi - 1, buf->buf);	    // exclusive

    ಯದಿ (SCHEME_FALSEP(ಸಾಲು_list) || SCHEME_NULLP(ಸಾಲು_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	ಯದಿ (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < old_len; i++)
		ಯದಿ (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    ಯದಿ (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_ಸಾಲುಗಳು_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
    }
    ಅನ್ಯಥಾ
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(ಸಾಲು_list);
	MZ_GC_CHECK();
	ಯದಿ (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	ಅನ್ಯಥಾ
	{
	    ವರ್ಣ		**array = ರಿಕ್ತ;
	    Scheme_Object   *ಸಾಲು = ರಿಕ್ತ;
	    Scheme_Object   *rest = ರಿಕ್ತ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, ಸಾಲು);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(ವರ್ಣ *, new_len + 1);

	    rest = ಸಾಲು_list;
	    ಪ್ರತಿ (i = 0; i < new_len; ++i)
	    {
		ಸಾಲು = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		ಯದಿ (!SCHEME_STRINGP(ಸಾಲು))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_ಸಾಲು(ಸಾಲು);
	    }

	    curbuf = buf->buf;

	    ಯದಿ (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    ಪ್ರತಿ (i = 0; i < old_len - new_len; ++i)
	    {
		ಯದಿ (ml_delete((linenr_T)lo) == FAIL)
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
	    ಪ್ರತಿ (i = 0; i < old_len && i < new_len; i++)
		ಯದಿ (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    ಯಾವತ್ (i < new_len)
	    {
		ಯದಿ (ml_append((linenr_T)(lo + i - 1),
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
						  (long)MAXLNUM, (long)extra);
	changed_ಸಾಲುಗಳು((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	ಯದಿ (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
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
ಸೇರಿಸು_buffer_ಸಾಲು_list(ಶೂನ್ಯ *data, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ರಿಕ್ತ;
    Scheme_Object   *list = ರಿಕ್ತ;
    ವರ್ಣ	    *str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    i, n, size;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    /*
     * First of all, we check the type of the supplied MzScheme object.
     * It must be a string or a list, or the call is in error.
     */
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    list = argv[1];

    ಯದಿ (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    ಯದಿ (n != 0)	    // 0 can be used in insert
	check_ಸಾಲು_range(n, buf->buf);
    ಯದಿ (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_ಸಾಲು(list);
	curbuf = buf->buf;

	ಯದಿ (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	ಅನ್ಯಥಾ
	{
	    vim_free(str);
	    appended_ಸಾಲುಗಳು_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    ಯದಿ (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    ಅನ್ಯಥಾ
    {
	Scheme_Object   *ಸಾಲು = ರಿಕ್ತ;
	Scheme_Object   *rest = ರಿಕ್ತ;
	ವರ್ಣ		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ಸಾಲು);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(ವರ್ಣ *, size + 1);

	rest = list;
	ಪ್ರತಿ (i = 0; i < size; ++i)
	{
	    ಸಾಲು = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_ಸಾಲು(ಸಾಲು);
	}

	curbuf = buf->buf;

	ಯದಿ (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; i < size; ++i)
		ಯದಿ (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    ಯದಿ (i > 0)
		appended_ಸಾಲುಗಳು_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_bufferp(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv)
{
    ಯದಿ (SCHEME_VIMBUFFERP(argv[0]))
	ಪ್ರತ್ಯಾಗಮ scheme_true;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ scheme_false;
}

/*
 * (win? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_windowp(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv)
{
    ಯದಿ (SCHEME_VIMWINDOWP(argv[0]))
	ಪ್ರತ್ಯಾಗಮ scheme_true;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_buffer_validp(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv)
{
    ಯದಿ (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ scheme_true;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ scheme_false;
}

/*
 * (win-valid? obj)
 */
    ಸ್ಥಿರ Scheme_Object *
vim_window_validp(ಶೂನ್ಯ *data UNUSED, ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv)
{
    ಯದಿ (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ scheme_true;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ scheme_false;
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
    ಸ್ಥಿರ ವರ್ಣ *
string_to_ಸಾಲು(Scheme_Object *obj)
{
    ವರ್ಣ	*scheme_str = ರಿಕ್ತ;
    ವರ್ಣ	*vim_str = ರಿಕ್ತ;
    OUTPUT_LEN_TYPE	len;
    ಪೂರ್ಣಾಙ್ಕ		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    ಯದಿ (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	ಯದಿ (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	ಅನ್ಯಥಾ
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    ಪ್ರತ್ಯಾಗಮ vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ಸ್ಥಿರ Scheme_Object *
vim_to_mzscheme(typval_T *vim_ಮೌಲ್ಯ)
{
    Scheme_Object	*result = ರಿಕ್ತ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = ರಿಕ್ತ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_ಮೌಲ್ಯ, 1, visited);

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಸ್ಥಿರ Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಙ್ಕ			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    new_ಮೌಲ್ಯ_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    ಯದಿ (depth > 100)
    {
	MZ_GC_UNREG();
	ಪ್ರತ್ಯಾಗಮ scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_ಮೌಲ್ಯ);
    MZ_GC_CHECK();
    ಯದಿ (result != ರಿಕ್ತ) // found, do nothing
	new_ಮೌಲ್ಯ_2 = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((ವರ್ಣ *)vim_ಮೌಲ್ಯ->vval.v_string);
	MZ_GC_CHECK();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_ಸಂಖ್ಯೆ)
    {
	result = scheme_make_integer((long)vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ);
	MZ_GC_CHECK();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((ದ್ವಿಗುಣ)vim_ಮೌಲ್ಯ->vval.v_float);
	MZ_GC_CHECK();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_LIST)
    {
	list_T		*list = vim_ಮೌಲ್ಯ->vval.v_list;
	listitem_T	*curr;

	ಯದಿ (list == ರಿಕ್ತ || list->lv_first == ರಿಕ್ತ)
	    result = scheme_null;
	ಅನ್ಯಥಾ
	{
	    Scheme_Object   *obj = ರಿಕ್ತ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    ಯಾವತ್ (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_DICT)
    {
	Scheme_Object	  *ಕೀಲಿ = ರಿಕ್ತ;
	Scheme_Object	  *obj = ರಿಕ್ತ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ಕೀಲಿ);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	ಯದಿ (vim_ಮೌಲ್ಯ->vval.v_dict != ರಿಕ್ತ)
	{
	    hashtab_T	*ht = &vim_ಮೌಲ್ಯ->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	    {
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    ಕೀಲಿ = scheme_make_byte_string((ವರ್ಣ *)hi->hi_ಕೀಲಿ);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, ಕೀಲಿ, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ರಿಕ್ತ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((ವರ್ಣ *)vim_ಮೌಲ್ಯ->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ಧ್ರುವ ವರ್ಣ *)BYTE_STRING_ಮೌಲ್ಯ(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_PARTIAL)
    {
	ಯದಿ (vim_ಮೌಲ್ಯ->vval.v_partial == ರಿಕ್ತ)
	    result = scheme_null;
	ಅನ್ಯಥಾ
	{
	    Scheme_Object *funcname = ರಿಕ್ತ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (ವರ್ಣ *)partial_name(vim_ಮೌಲ್ಯ->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ಧ್ರುವ ವರ್ಣ *)BYTE_STRING_ಮೌಲ್ಯ(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_ಮೌಲ್ಯ->v_type == VAR_BOOL || vim_ಮೌಲ್ಯ->v_type == VAR_SPECIAL)
    {
	ಯದಿ (vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ);
	ಅನ್ಯಥಾ
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    ಅನ್ಯಥಾ
    {
	result = scheme_void;
	new_ಮೌಲ್ಯ_2 = FALSE;
    }
    ಯದಿ (new_ಮೌಲ್ಯ_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_ಮೌಲ್ಯ, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    ಪೂರ್ಣಾಙ್ಕ i, status;
    Scheme_Hash_Table	*visited = ರಿಕ್ತ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    ಪ್ರತಿ (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	ಯದಿ (visited->vals[i] != ರಿಕ್ತ)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ status;
}
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, ಪೂರ್ಣಾಙ್ಕ depth,
	Scheme_Hash_Table *visited)
{
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    ಯದಿ (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    ಯದಿ (found != ರಿಕ್ತ)
	copy_tv(found, tv);
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
    }
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = SCHEME_INT_VAL(obj);
    }
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_ಸಂಖ್ಯೆ = SCHEME_TRUEP(obj);
    }
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(obj));
    }
# ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ರಿಕ್ತ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ಮೌಲ್ಯ(tmp));
	MZ_GC_UNREG();
    }
# endif
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	ಯದಿ (list == ರಿಕ್ತ)
	    status = FAIL;
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		    i;
	    Scheme_Object   *curr = ರಿಕ್ತ;
	    Scheme_Object   *cval = ರಿಕ್ತ;
	    // temporary var to hold current element of vectors and pairs
	    typval_T	    *v;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, curr);
	    MZ_GC_VAR_IN_REG(1, cval);
	    MZ_GC_REG();

	    tv->v_type = VAR_LIST;
	    tv->vval.v_list = list;
	    ++list->lv_refcount;

	    v = ALLOC_ONE(typval_T);
	    ಯದಿ (v == ರಿಕ್ತ)
		status = FAIL;
	    ಅನ್ಯಥಾ
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		ಯದಿ (SCHEME_VECTORP(obj))
		{
		    ಪ್ರತಿ (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ಯದಿ (status == FAIL)
			    ಭಙ್ಗ;
			status = list_append_tv(list, v);
			clear_tv(v);
			ಯದಿ (status == FAIL)
			    ಭಙ್ಗ;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    ಪ್ರತಿ (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ಯದಿ (status == FAIL)
			    ಭಙ್ಗ;
			status = list_append_tv(list, v);
			clear_tv(v);
			ಯದಿ (status == FAIL)
			    ಭಙ್ಗ;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    ಯದಿ (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ಯದಿ (status == OK)
			{
			    status = list_append_tv(list, v);
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
    ಅನ್ಯಥಾ ಯದಿ (SCHEME_HASHTP(obj))
    {
	ಪೂರ್ಣಾಙ್ಕ		i;
	dict_T		*dict;
	Scheme_Object   *ಕೀಲಿ = ರಿಕ್ತ;
	Scheme_Object   *val = ರಿಕ್ತ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ಕೀಲಿ);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	ಯದಿ (dict == ರಿಕ್ತ)
	    status = FAIL;
	ಅನ್ಯಥಾ
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    ಪ್ರತಿ (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		ಯದಿ (((Scheme_Hash_Table *) obj)->vals[i] != ರಿಕ್ತ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_ಸಾಲು(
				((Scheme_Hash_Table *) obj)->ಕೀಲಿಗಳು[i]));
		    // convert Scheme val to Vim and add it to the dict
		    ಯದಿ (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			ಭಙ್ಗ;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    ಅನ್ಯಥಾ
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_ಸಾಲು(obj);
    }
    MZ_GC_UNREG();
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ಸ್ಥಿರ Scheme_Object *
vim_funcref(ಶೂನ್ಯ *name, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    typval_T args;
    ಪೂರ್ಣಾಙ್ಕ status = OK;
    Scheme_Object *result = ರಿಕ್ತ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    ಯದಿ (list == ರಿಕ್ತ)
	status = FAIL;
    ಅನ್ಯಥಾ
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	ಪ್ರತಿ (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    ಯದಿ (v == ರಿಕ್ತ)
		status = FAIL;
	    ಅನ್ಯಥಾ
	    {
		status = mzscheme_to_vim(argv[i], v);
		ಯದಿ (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	ಯದಿ (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_ಮೌಲ್ಯ((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    ಯದಿ (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    ಅನ್ಯಥಾ
	raise_if_error();
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಶೂನ್ಯ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ರಿಕ್ತ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    ಯದಿ (mzscheme_init())
    {
	MZ_GC_UNREG();
	ಪ್ರತ್ಯಾಗಮ;
    }

    MZ_GC_CHECK();
    ಯದಿ (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim_error_check(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
register_vim_exn(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	nc = 0;
    ಪೂರ್ಣಾಙ್ಕ i;
    Scheme_Object   *struct_exn = ರಿಕ್ತ;
    Scheme_Object   *exn_name = ರಿಕ್ತ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_ಚಿಹ್ನೆ("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_ಮೌಲ್ಯ("struct:exn");
    MZ_GC_CHECK();

    ಯದಿ (vim_exn == ರಿಕ್ತ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ರಿಕ್ತ, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ
#ಯದಿ MZSCHEME_VERSION_MAJOR >= 299
		, ರಿಕ್ತ
#endif
		);


    {
	Scheme_Object   **tmp = ರಿಕ್ತ;
	Scheme_Object   *exn_names[5] = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
	Scheme_Object   *exn_ಮೌಲ್ಯಗಳು[5] = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_ಮೌಲ್ಯಗಳು, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * ಪರಿಮಾಣ(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_ಮೌಲ್ಯಗಳು(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_ಮೌಲ್ಯಗಳು, tmp, nc * ಪರಿಮಾಣ(Scheme_Object *));
	MZ_GC_CHECK();

	ಪ್ರತಿ (i = 0; i < nc; i++)
	{
	    scheme_add_global_ಚಿಹ್ನೆ(exn_names[i],
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
raise_vim_exn(ಧ್ರುವ ವರ್ಣ *add_info)
{
    ವರ್ಣ	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ರಿಕ್ತ, ರಿಕ್ತ};
    Scheme_Object   *exn = ರಿಕ್ತ;
    Scheme_Object   *byte_string = ರಿಕ್ತ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    ಯದಿ (add_info != ರಿಕ್ತ)
    {
	ವರ್ಣ		*c_string = ರಿಕ್ತ;
	Scheme_Object   *info = ರಿಕ್ತ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fmt), 1, &info, ರಿಕ್ತ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    ಅನ್ಯಥಾ
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#ಯದಿ MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#ಅನ್ಯಥಾ
    argv[1] = scheme_current_continuation_marks(ರಿಕ್ತ);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    ಶೂನ್ಯ
raise_if_error(ಶೂನ್ಯ)
{
    ಯದಿ (vim_error_check())
	raise_vim_exn(ರಿಕ್ತ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ಸ್ಥಿರ vim_mz_buffer *
get_buffer_arg(ಧ್ರುವ ವರ್ಣ *fname, ಪೂರ್ಣಾಙ್ಕ argnum, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    ಯದಿ (argc < argnum + 1)
	ಪ್ರತ್ಯಾಗಮ get_vim_curr_buffer();
    ಯದಿ (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (ಶೂನ್ಯ)get_valid_buffer(argv[argnum]);
    ಪ್ರತ್ಯಾಗಮ b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ಸ್ಥಿರ vim_mz_window *
get_window_arg(ಧ್ರುವ ವರ್ಣ *fname, ಪೂರ್ಣಾಙ್ಕ argnum, ಪೂರ್ಣಾಙ್ಕ argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    ಯದಿ (argc < argnum + 1)
	ಪ್ರತ್ಯಾಗಮ get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    ಯದಿ (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (ಶೂನ್ಯ)get_valid_window(argv[argnum]);
    ಪ್ರತ್ಯಾಗಮ w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(ಶೂನ್ಯ *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    ಯದಿ (buf == INVALID_BUFFER_ಮೌಲ್ಯ)
	scheme_signal_error(_("buffer is invalid"));
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(ಶೂನ್ಯ *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    ಯದಿ (win == INVALID_WINDOW_ಮೌಲ್ಯ)
	scheme_signal_error(_("window is invalid"));
    ಪ್ರತ್ಯಾಗಮ win;
}

    ಪೂರ್ಣಾಙ್ಕ
mzthreads_allowed(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ mz_threads_allow;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಸಾಲು_in_range(linenr_T lnum, buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ (lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_ಸಾಲು_range(linenr_T lnum, buf_T *buf)
{
    ಯದಿ (!ಸಾಲು_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mz_fix_cursor(ಪೂರ್ಣಾಙ್ಕ lo, ಪೂರ್ಣಾಙ್ಕ hi, ಪೂರ್ಣಾಙ್ಕ extra)
{
    ಯದಿ (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ಯದಿ (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ಅನ್ಯಥಾ ಯದಿ (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ಅನ್ಯಥಾ
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

ಸ್ಥಿರ Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_ಸಾಲು, "get-buff-line", 1, 2},
    {set_buffer_ಸಾಲು, "set-buff-line", 2, 3},
    {get_buffer_ಸಾಲು_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_ಅಂಕೆ, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_ಸಾಲು_list, "set-buff-line-list", 3, 4},
    {ಸೇರಿಸು_buffer_ಸಾಲು_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_ಎಣಿಕೆ, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_ಅಂಕೆ, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_ಎಣಿಕೆ, "win-count", 0, 0},
    {get_window_by_ಅಂಕೆ, "get-win-by-num", 1, 1},
    {get_window_ಅಂಕೆ, "get-win-num", 0, 1},
    {get_window_buffer, "get-win-buffer", 0, 1},
    {get_window_height, "get-win-height", 0, 1},
    {set_window_height, "set-win-height", 1, 2},
    {get_window_width, "get-win-width", 0, 1},
    {set_window_width, "set-win-width", 1, 2},
    {get_cursor, "get-cursor", 0, 1},
    {set_cursor, "set-cursor", 1, 2},
    {get_window_list, "get-win-list", 0, 1},
    /*
     * Vim-related commands
     */
    {vim_command, "command", 1, 1},
    {vim_eval, "eval", 1, 1},
    {get_range_start, "range-start", 0, 0},
    {get_range_end, "range-end", 0, 0},
    {mzscheme_beep, "beep", 0, 0},
    {get_option, "get-option", 1, 2},
    {set_option, "set-option", 1, 2},
    /*
     * small utilities
     */
    {vim_bufferp, "buff?", 1, 1},
    {vim_windowp, "win?", 1, 1},
    {vim_buffer_validp, "buff-valid?", 1, 1},
    {vim_window_validp, "win-valid?", 1, 1}
};

/*
 * return MzScheme wrapper for curbuf
 */
    ಸ್ಥಿರ vim_mz_buffer *
get_vim_curr_buffer(ಶೂನ್ಯ)
{
    ಯದಿ (curbuf->b_mzscheme_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (vim_mz_buffer *)buffer_new(curbuf);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ಸ್ಥಿರ vim_mz_window *
get_vim_curr_window(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_mzscheme_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (vim_mz_window *)window_new(curwin);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ WINDOW_REF(curwin);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
make_modules(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    Scheme_Env	    *mod = ರಿಕ್ತ;
    Scheme_Object   *vimext_ಚಿಹ್ನೆ = ರಿಕ್ತ;
    Scheme_Object   *closed_prim = ರಿಕ್ತ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_ಚಿಹ್ನೆ);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_ಚಿಹ್ನೆ = scheme_intern_ಚಿಹ್ನೆ("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_ಚಿಹ್ನೆ, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(prims); i++)
    {
	Vim_Prim *prim = prims + i;
	closed_prim = scheme_make_closed_prim_w_arity(prim->prim, prim, prim->name,
			    prim->mina, prim->maxa);
	scheme_add_global(prim->name, closed_prim, mod);
	MZ_GC_CHECK();
    }
    scheme_finish_primitive_module(mod);
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

#ifdef HAVE_SANDBOX
ಸ್ಥಿರ Scheme_Object *M_write = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *M_read = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *M_execute = ರಿಕ್ತ;
ಸ್ಥಿರ Scheme_Object *M_delete = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
sandbox_check(ಶೂನ್ಯ)
{
    ಯದಿ (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ಸ್ಥಿರ Scheme_Object *
sandbox_file_guard(ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv)
{
    ಯದಿ (!sandbox)
	ಪ್ರತ್ಯಾಗಮ scheme_void;

    Scheme_Object *requested_access = argv[2];

    ಯದಿ (M_write == ರಿಕ್ತ)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_ಚಿಹ್ನೆ("write");
	MZ_GC_CHECK();
    }
    ಯದಿ (M_read == ರಿಕ್ತ)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_ಚಿಹ್ನೆ("read");
	MZ_GC_CHECK();
    }
    ಯದಿ (M_execute == ರಿಕ್ತ)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_ಚಿಹ್ನೆ("execute");
	MZ_GC_CHECK();
    }
    ಯದಿ (M_delete == ರಿಕ್ತ)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_ಚಿಹ್ನೆ("delete");
	MZ_GC_CHECK();
    }

    ಯಾವತ್ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	ಯದಿ (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    ಪ್ರತ್ಯಾಗಮ scheme_void;
}

    ಸ್ಥಿರ Scheme_Object *
sandbox_network_guard(ಪೂರ್ಣಾಙ್ಕ argc UNUSED, Scheme_Object **argv UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ scheme_void;
}
#endif

#endif
