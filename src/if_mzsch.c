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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ଯଦି defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#ଯଦି MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#ଯଦି defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_ମୂଲ୍ୟ ((buf_T *)(-1))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_ମୂଲ୍ୟ ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    Scheme_Closed_Prim	*prim;
    ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mina;	// arity information
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxa;
} Vim_Prim;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର	    *name;
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
ସ୍ଥିର Scheme_Object *sandbox_file_guard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, Scheme_Object **);
ସ୍ଥିର Scheme_Object *sandbox_network_guard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, Scheme_Object **);
ସ୍ଥିର ଶୂନ୍ୟ sandbox_check(ଶୂନ୍ୟ);
#endif
//  Buffer-related commands
ସ୍ଥିର Scheme_Object *buffer_new(buf_T *buf);
ସ୍ଥିର Scheme_Object *get_buffer_by_ଅଙ୍କ(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, Scheme_Object **);
ସ୍ଥିର vim_mz_buffer *get_vim_curr_buffer(ଶୂନ୍ୟ);

//  Window-related commands
ସ୍ଥିର Scheme_Object *window_new(win_T *win);
ସ୍ଥିର vim_mz_window *get_vim_curr_window(ଶୂନ୍ୟ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_error_check(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_mzscheme_command(exarg_T *, ଶୂନ୍ୟ *, Scheme_Closed_Prim *what);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା startup_mzscheme(ଶୂନ୍ୟ);
ସ୍ଥିର ଅକ୍ଷର *string_to_ଧାଡ଼ି(Scheme_Object *obj);
#ଯଦି MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#ଅନ୍ୟଥା
# define OUTPUT_LEN_TYPE long
#endif
ସ୍ଥିର ଶୂନ୍ୟ do_output(ଅକ୍ଷର *mesg, OUTPUT_LEN_TYPE len);
ସ୍ଥିର ଶୂନ୍ୟ do_printf(ଅକ୍ଷର *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ସ୍ଥିର ଶୂନ୍ୟ do_flush(ଶୂନ୍ୟ);
ସ୍ଥିର Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ସ୍ଥିର Scheme_Object *extract_exn_message(Scheme_Object *v);
ସ୍ଥିର Scheme_Object *do_eval(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noargc, Scheme_Object **noargv);
ସ୍ଥିର Scheme_Object *do_load(ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noargc, Scheme_Object **noargv);
ସ୍ଥିର ଶୂନ୍ୟ register_vim_exn(ଶୂନ୍ୟ);
ସ୍ଥିର vim_mz_buffer *get_buffer_arg(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv);
ସ୍ଥିର vim_mz_window *get_window_arg(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_in_range(linenr_T, buf_T *);
ସ୍ଥିର ଶୂନ୍ୟ check_ଧାଡ଼ି_range(linenr_T, buf_T *);
ସ୍ଥିର ଶୂନ୍ୟ mz_fix_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval_with_exn_handling(ଶୂନ୍ୟ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ସ୍ଥିର ଶୂନ୍ୟ make_modules(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ init_exn_catching_apply(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mzscheme_env_main(Scheme_Env *env, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mzscheme_init(ଶୂନ୍ୟ);
#ifdef FEAT_EVAL
ସ୍ଥିର Scheme_Object *vim_to_mzscheme(typval_T *vim_ମୂଲ୍ୟ);
ସ୍ଥିର Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth,
	Scheme_Hash_Table *visited);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth,
	Scheme_Hash_Table *visited);
ସ୍ଥିର Scheme_Object *vim_funcref(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffer_size_proc(ଶୂନ୍ୟ *obj UNUSED)
{
    ଫେରନ୍ତୁ gc_BYTES_TO_ଶବ୍ଦଗୁଡ଼ିକ(ଆକାର(vim_mz_buffer));
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffer_mark_proc(ଶୂନ୍ୟ *obj)
{
    ଫେରନ୍ତୁ buffer_size_proc(obj);
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffer_fixup_proc(ଶୂନ୍ୟ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    ଫେରନ୍ତୁ buffer_size_proc(obj);
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା window_size_proc(ଶୂନ୍ୟ *obj UNUSED)
{
    ଫେରନ୍ତୁ gc_BYTES_TO_ଶବ୍ଦଗୁଡ଼ିକ(ଆକାର(vim_mz_window));
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା window_mark_proc(ଶୂନ୍ୟ *obj)
{
    ଫେରନ୍ତୁ window_size_proc(obj);
}
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା window_fixup_proc(ଶୂନ୍ୟ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    ଫେରନ୍ତୁ window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#ଅନ୍ୟଥା
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#ଯଦି defined(DYNAMIC_MZSCHEME)
ସ୍ଥିର Scheme_Object *dll_scheme_eof;
ସ୍ଥିର Scheme_Object *dll_scheme_false;
ସ୍ଥିର Scheme_Object *dll_scheme_void;
ସ୍ଥିର Scheme_Object *dll_scheme_null;
ସ୍ଥିର Scheme_Object *dll_scheme_true;

ସ୍ଥିର Scheme_Thread **dll_scheme_current_thread_ptr;

ସ୍ଥିର ଶୂନ୍ୟ (**dll_scheme_console_printf_ptr)(ଅକ୍ଷର *str, ...);
ସ୍ଥିର ଶୂନ୍ୟ (**dll_scheme_console_output_ptr)(ଅକ୍ଷର *str, OUTPUT_LEN_TYPE len);
ସ୍ଥିର ଶୂନ୍ୟ (**dll_scheme_notify_multithread_ptr)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on);

ସ୍ଥିର ଶୂନ୍ୟ *(*dll_GC_malloc)(size_t size_in_bytes);
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
ସ୍ଥିର Scheme_Env *(*dll_scheme_basic_env)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_check_threads)(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_register_static)(ଶୂନ୍ୟ *ptr, long size);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_set_ଗଦା_base)(ଶୂନ୍ୟ *base, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_auto_statics);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_add_global)(ଅଚଳ ଅକ୍ଷର *name, Scheme_Object *val,
	Scheme_Env *env);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_add_global_ପ୍ରତୀକ)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ସ୍ଥିର Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_rands,
	Scheme_Object **rands);
ସ୍ଥିର Scheme_Object *(*dll_scheme_builtin_ମୂଲ୍ୟ)(ଅଚଳ ଅକ୍ଷର *name);
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
ସ୍ଥିର Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_path)(ଅଚଳ ଅକ୍ଷର *chars);
# endif
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_close_ନିବେଶ_port)(Scheme_Object *port);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ)(Scheme_Object *port);
# ଯଦି MZSCHEME_VERSION_MAJOR < 360
ସ୍ଥିର Scheme_Object *(*dll_scheme_current_continuation_marks)(ଶୂନ୍ୟ);
# ଅନ୍ୟଥା
ସ୍ଥିର Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
ସ୍ଥିର ଅକ୍ଷର *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ସ୍ଥିର Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_rands_2, Scheme_Object **rands, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_dont_gc_ptr)(ଶୂନ୍ୟ *p);
ସ୍ଥିର Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ସ୍ଥିର Scheme_Object *(*dll_scheme_eval_string)(ଅଚଳ ଅକ୍ଷର *str,
	Scheme_Env *env);
ସ୍ଥିର Scheme_Object *(*dll_scheme_eval_string_all)(ଅଚଳ ଅକ୍ଷର *str,
	Scheme_Env *env, ପୂର୍ଣ୍ଣସଂଖ୍ୟା all);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
ସ୍ଥିର ଅକ୍ଷର *(*dll_scheme_format)(ଅକ୍ଷର *format, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc,
	Scheme_Object **argv, long *rlen);
# ଅନ୍ୟଥା
ସ୍ଥିର ଅକ୍ଷର *(*dll_scheme_format_utf8)(ଅକ୍ଷର *format, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ସ୍ଥିର Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos);
# endif
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_gc_ptr_ok)(ଶୂନ୍ୟ *p);
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
ସ୍ଥିର ଅକ୍ଷର *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# ଅନ୍ୟଥା
ସ୍ଥିର ଅକ୍ଷର *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ସ୍ଥିର Scheme_Object *(*dll_scheme_intern_ପ୍ରତୀକ)(ଅଚଳ ଅକ୍ଷର *name);
ସ୍ଥିର Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *ପ୍ରତୀକ,
	Scheme_Env *env);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, ଶୂନ୍ୟ *data, ଅଚଳ ଅକ୍ଷର *name, mzshort mina,
     mzshort maxa);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_integer_ମୂଲ୍ୟ)(long i);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ଅଚଳ ଅକ୍ଷର *name, mzshort mina, mzshort maxa);
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_string)(ଅଚଳ ଅକ୍ଷର *chars);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_string_output_port)();
# ଅନ୍ୟଥା
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_byte_string)(ଅଚଳ ଅକ୍ଷର *chars);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv);
ସ୍ଥିର Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଗଣନା_out);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_fields,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ସ୍ଥିର Scheme_Object **(*dll_scheme_make_struct_ମୂଲ୍ୟଗୁଡ଼ିକ)(
	Scheme_Object *struct_type, Scheme_Object **names, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର Scheme_Type (*dll_scheme_make_type)(ଅଚଳ ଅକ୍ଷର *name);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_vector)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size,
	Scheme_Object *fill);
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_scheme_malloc_fail_ok)(ଶୂନ୍ୟ *(*f)(size_t), size_t);
ସ୍ଥିର Scheme_Object *(*dll_scheme_open_ନିବେଶ_file)(ଅଚଳ ଅକ୍ଷର *name,
	ଅଚଳ ଅକ୍ଷର *who);
ସ୍ଥିର Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_scheme_proper_list_length)(Scheme_Object *list);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_raise)(Scheme_Object *exn);
ସ୍ଥିର Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_signal_error)(ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ, ...);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_wrong_type)(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *expected,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା which, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv);
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_set_param)(Scheme_Config *c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos,
	Scheme_Object *o);
ସ୍ଥିର Scheme_Config *(*dll_scheme_current_config)(ଶୂନ୍ୟ);
ସ୍ଥିର Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
ସ୍ଥିର Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
ସ୍ଥିର Scheme_Hash_Table *(*dll_scheme_make_hash_table)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *ଚାବି, Scheme_Object *ମୂଲ୍ୟ);
ସ୍ଥିର Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *ଚାବି);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_double)(ଦ୍ୱିଗୁଣ d);
ସ୍ଥିର Scheme_Object *(*dll_scheme_make_sized_byte_string)(ଅକ୍ଷର *chars,
	long len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy);
ସ୍ଥିର Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ସ୍ଥିର Scheme_Object *(*dll_scheme_dynamic_wind)(ଶୂନ୍ୟ (*pre)(ଶୂନ୍ୟ *), Scheme_Object *(* ଅସ୍ଥିର act)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ (* ଅସ୍ଥିର post)(ଶୂନ୍ୟ *), Scheme_Object *(*jmp_handler)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ * ଅସ୍ଥିର data);
# ifdef MZ_PRECISE_GC
ସ୍ଥିର ଶୂନ୍ୟ *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_constant_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_atomic);
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 400
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ସ୍ଥିର ଶୂନ୍ୟ **(*dll_scheme_malloc_immobile_box)(ଶୂନ୍ୟ *p);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_free_immobile_box)(ଶୂନ୍ୟ **b);
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_scheme_main_setup)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_auto_statics, Scheme_Env_Main _main, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv);
#   ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_register_tls_space)(ଶୂନ୍ୟ *tls_space, ପୂର୍ଣ୍ଣସଂଖ୍ୟା _tls_index);
#   endif
#  endif
#  ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
ସ୍ଥିର Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(ଶୂନ୍ୟ);
#  endif
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 600
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_embedded_load)(intptr_t len, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା predefined);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_register_embedded_load)(intptr_t len, ଅଚଳ ଅକ୍ଷର *s);
ସ୍ଥିର ଶୂନ୍ୟ (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# ଯଦି defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  ଯଦି !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_ପ୍ରତୀକ dll_scheme_add_global_ପ୍ରତୀକ
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_ମୂଲ୍ୟ dll_scheme_builtin_ମୂଲ୍ୟ
#  ଯଦି MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_ନିବେଶ_port dll_scheme_close_ନିବେଶ_port
#  define scheme_ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ dll_scheme_ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ
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
#  ଯଦି MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  ଅନ୍ୟଥା
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  ଯଦି MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  ଅନ୍ୟଥା
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_ପ୍ରତୀକ dll_scheme_intern_ପ୍ରତୀକ
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_ମୂଲ୍ୟ dll_scheme_make_integer_ମୂଲ୍ୟ
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  ଯଦି MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  ଅନ୍ୟଥା
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_ମୂଲ୍ୟଗୁଡ଼ିକ dll_scheme_make_struct_ମୂଲ୍ୟଗୁଡ଼ିକ
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_ନିବେଶ_file dll_scheme_open_ନିବେଶ_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_ଗଦା_base dll_scheme_set_ଗଦା_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  ଯଦି MZSCHEME_VERSION_MAJOR >= 299
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
#  ଯଦି MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  ଯଦି MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   ଯଦି MZSCHEME_VERSION_MAJOR >= 500
#    ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର    *name;
    ଶୂନ୍ୟ    **ptr;
} Thunk_Info;

ସ୍ଥିର Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (ଶୂନ୍ୟ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (ଶୂନ୍ୟ **)&dll_GC_malloc_atomic},
    {ଖାଲି, ଖାଲି}};

ସ୍ଥିର Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (ଶୂନ୍ୟ **)&dll_scheme_eof},
    {"scheme_false", (ଶୂନ୍ୟ **)&dll_scheme_false},
    {"scheme_void", (ଶୂନ୍ୟ **)&dll_scheme_void},
    {"scheme_null", (ଶୂନ୍ୟ **)&dll_scheme_null},
    {"scheme_true", (ଶୂନ୍ୟ **)&dll_scheme_true},
# ଯଦି !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (ଶୂନ୍ୟ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (ଶୂନ୍ୟ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (ଶୂନ୍ୟ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(ଶୂନ୍ୟ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (ଶୂନ୍ୟ **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (ଶୂନ୍ୟ **)&dll_scheme_add_global_ପ୍ରତୀକ},
    {"scheme_apply", (ଶୂନ୍ୟ **)&dll_scheme_apply},
    {"scheme_basic_env", (ଶୂନ୍ୟ **)&dll_scheme_basic_env},
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (ଶୂନ୍ୟ **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (ଶୂନ୍ୟ **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (ଶୂନ୍ୟ **)&dll_scheme_builtin_ମୂଲ୍ୟ},
    {"scheme_check_threads", (ଶୂନ୍ୟ **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (ଶୂନ୍ୟ **)&dll_scheme_close_ନିବେଶ_port},
    {"scheme_count_lines", (ଶୂନ୍ୟ **)&dll_scheme_ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ},
    {"scheme_current_continuation_marks",
	(ଶୂନ୍ୟ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (ଶୂନ୍ୟ **)&dll_scheme_display},
    {"scheme_display_to_string", (ଶୂନ୍ୟ **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (ଶୂନ୍ୟ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (ଶୂନ୍ୟ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (ଶୂନ୍ୟ **)&dll_scheme_eq},
    {"scheme_eval", (ଶୂନ୍ୟ **)&dll_scheme_eval},
    {"scheme_eval_string", (ଶୂନ୍ୟ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (ଶୂନ୍ୟ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(ଶୂନ୍ୟ **)&dll_scheme_finish_primitive_module},
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (ଶୂନ୍ୟ **)&dll_scheme_format},
# ଅନ୍ୟଥା
    {"scheme_format_utf8", (ଶୂନ୍ୟ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (ଶୂନ୍ୟ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (ଶୂନ୍ୟ **)&dll_scheme_gc_ptr_ok},
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(ଶୂନ୍ୟ **)&dll_scheme_get_sized_string_output},
# ଅନ୍ୟଥା
    {"scheme_get_sized_byte_string_output",
	(ଶୂନ୍ୟ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (ଶୂନ୍ୟ **)&dll_scheme_intern_ପ୍ରତୀକ},
    {"scheme_lookup_global", (ଶୂନ୍ୟ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(ଶୂନ୍ୟ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (ଶୂନ୍ୟ **)&dll_scheme_make_integer_ମୂଲ୍ୟ},
    {"scheme_make_pair", (ଶୂନ୍ୟ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (ଶୂନ୍ୟ **)&dll_scheme_make_prim_w_arity},
# ଯଦି MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (ଶୂନ୍ୟ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(ଶୂନ୍ୟ **)&dll_scheme_make_string_output_port},
# ଅନ୍ୟଥା
    {"scheme_make_byte_string", (ଶୂନ୍ୟ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(ଶୂନ୍ୟ **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(ଶୂନ୍ୟ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (ଶୂନ୍ୟ **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (ଶୂନ୍ୟ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (ଶୂନ୍ୟ **)&dll_scheme_make_struct_ମୂଲ୍ୟଗୁଡ଼ିକ},
    {"scheme_make_type", (ଶୂନ୍ୟ **)&dll_scheme_make_type},
    {"scheme_make_vector", (ଶୂନ୍ୟ **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (ଶୂନ୍ୟ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (ଶୂନ୍ୟ **)&dll_scheme_open_ନିବେଶ_file},
    {"scheme_primitive_module", (ଶୂନ୍ୟ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (ଶୂନ୍ୟ **)&dll_scheme_proper_list_length},
    {"scheme_raise", (ଶୂନ୍ୟ **)&dll_scheme_raise},
    {"scheme_read", (ଶୂନ୍ୟ **)&dll_scheme_read},
    {"scheme_register_static", (ଶୂନ୍ୟ **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (ଶୂନ୍ୟ **)&dll_scheme_set_ଗଦା_base},
    {"scheme_signal_error", (ଶୂନ୍ୟ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (ଶୂନ୍ୟ **)&dll_scheme_wrong_type},
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (ଶୂନ୍ୟ **)&dll_scheme_set_param},
    {"scheme_current_config", (ଶୂନ୍ୟ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(ଶୂନ୍ୟ **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (ଶୂନ୍ୟ **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (ଶୂନ୍ୟ **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (ଶୂନ୍ୟ **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (ଶୂନ୍ୟ **)&dll_scheme_hash_set},
    {"scheme_hash_get", (ଶୂନ୍ୟ **)&dll_scheme_hash_get},
    {"scheme_make_double", (ଶୂନ୍ୟ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (ଶୂନ୍ୟ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (ଶୂନ୍ୟ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (ଶୂନ୍ୟ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (ଶୂନ୍ୟ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (ଶୂନ୍ୟ **)&dll_GC_register_traversers},
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (ଶୂନ୍ୟ **)&dll_scheme_main_setup},
#   ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (ଶୂନ୍ୟ **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (ଶୂନ୍ୟ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (ଶୂନ୍ୟ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (ଶୂନ୍ୟ **)&dll_scheme_free_immobile_box},
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 500
#  ଯଦି defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (ଶୂନ୍ୟ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# ଯଦି MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (ଶୂନ୍ୟ **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (ଶୂନ୍ୟ **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (ଶୂନ୍ୟ **)&dll_scheme_set_config_path},
# endif
    {ଖାଲି, ଖାଲି}};

ସ୍ଥିର HINSTANCE hMzGC = 0;
ସ୍ଥିର HINSTANCE hMzSch = 0;

ସ୍ଥିର ଶୂନ୍ୟ dynamic_mzscheme_end(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mzscheme_runtime_link_init(ଅକ୍ଷର *sch_dll, ଅକ୍ଷର *gc_dll,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose);

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_runtime_link_init(ଅକ୍ଷର *sch_dll, ଅକ୍ଷର *gc_dll, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    Thunk_Info *thunk = ଖାଲି;

    ଯଦି (hMzGC && hMzSch)
	ଫେରନ୍ତୁ OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    ଯଦି (!hMzGC)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!hMzSch)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (thunk = mzsch_imports; thunk->name; thunk++)
    {
	ଯଦି ((*thunk->ptr =
		    (ଶୂନ୍ୟ *)GetProcAddress(hMzSch, thunk->name)) == ଖାଲି)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ପାଇଁ (thunk = mzgc_imports; thunk->name; thunk++)
    {
	ଯଦି ((*thunk->ptr =
		    (ଶୂନ୍ୟ *)GetProcAddress(hMzGC, thunk->name)) == ଖାଲି)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ mzscheme_runtime_link_init(
	    (ଅକ୍ଷର *)p_mzschemedll, (ଅକ୍ଷର *)p_mzschemegcdll, verbose) == OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
dynamic_mzscheme_end(ଶୂନ୍ୟ)
{
    ଯଦି (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    ଯଦି (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#ଯଦି MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, ଅଙ୍କ) GUARANTEE_STRING(proc, ଅଙ୍କ)
#ଅନ୍ୟଥା
    ସ୍ଥିର Scheme_Object *
guaranteed_byte_string_arg(ଅକ୍ଷର *proc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    ଯଦି (SCHEME_BYTE_STRINGP(argv[ଅଙ୍କ]))
    {
	ଫେରନ୍ତୁ argv[ଅଙ୍କ];
    }
    ଅନ୍ୟଥା ଯଦି (SCHEME_CHAR_STRINGP(argv[ଅଙ୍କ]))
    {
	Scheme_Object *tmp = ଖାଲି;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[ଅଙ୍କ]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[ଅଙ୍କ]);
	MZ_GC_UNREG();
	ଫେରନ୍ତୁ tmp;
    }
    ଅନ୍ୟଥା
	scheme_wrong_type(proc, "string", ଅଙ୍କ, argc, argv);
    // unreachable
    ଫେରନ୍ତୁ scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, ଅଙ୍କ) guaranteed_byte_string_arg(proc, ଅଙ୍କ, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#ଯଦି defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ସ୍ଥିର Scheme_Type mz_buffer_type;
ସ୍ଥିର Scheme_Type mz_window_type;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା disabled = FALSE;
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_base_module_failed = FALSE;

// global environment
ସ୍ଥିର Scheme_Env    *environment = ଖାଲି;
// output/error handlers
ସ୍ଥିର Scheme_Object *curout = ଖାଲି;
ସ୍ଥିର Scheme_Object *curerr = ଖାଲି;
// exn:vim exception
ସ୍ଥିର Scheme_Object *exn_catching_apply = ଖାଲି;
ସ୍ଥିର Scheme_Object *exn_p = ଖାଲି;
ସ୍ଥିର Scheme_Object *exn_message = ଖାଲି;
ସ୍ଥିର Scheme_Object *vim_exn = ଖାଲି; // Vim Error exception

#ଯଦି !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
ସ୍ଥିର ଶୂନ୍ୟ *ଗଦା_base = ଖାଲି;
#endif

ସ୍ଥିର long range_start;
ସ୍ଥିର long range_end;

// MzScheme threads scheduling stuff
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mz_threads_allow = 0;

#ଯଦି defined(FEAT_GUI_MSWIN)
ସ୍ଥିର ଶୂନ୍ୟ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ସ୍ଥିର UINT timer_id = 0;
#ନଚେତଯଦି defined(FEAT_GUI_GTK)
ସ୍ଥିର gboolean timer_proc(gpointer);
ସ୍ଥିର guint timer_id = 0;
#ନଚେତଯଦି defined(FEAT_GUI_MOTIF)
ସ୍ଥିର ଶୂନ୍ୟ timer_proc(XtPointer, XtIntervalId *);
ସ୍ଥିର XtIntervalId timer_id = (XtIntervalId)0;
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    ଶୂନ୍ୟ
mzvim_check_threads(ଶୂନ୍ୟ)
{
    // Last time MzScheme threads were scheduled
    ସ୍ଥିର time_t mz_last_time = 0;

    ଯଦି (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(ଖାଲି);

	ଯଦି ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#ଯଦି defined(MZSCHEME_GUI_THREADS)
ସ୍ଥିର ଶୂନ୍ୟ setup_timer(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ remove_timer(ଶୂନ୍ୟ);

// timers are presented in GUI only
# ଯଦି defined(FEAT_GUI_MSWIN)
    ସ୍ଥିର ଶୂନ୍ୟ CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_ସନ୍ଦେଶ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    ସ୍ଥିର gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    ସ୍ଥିର ଶୂନ୍ୟ
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# ଯଦି defined(FEAT_GUI_GTK)
    ଫେରନ୍ତୁ TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    ଯଦି (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, ଖାଲି);
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
setup_timer(ଶୂନ୍ୟ)
{
# ଯଦି defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(ଖାଲି, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, ଖାଲି);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, ଖାଲି);
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_timer(ଶୂନ୍ୟ)
{
# ଯଦି defined(FEAT_GUI_MSWIN)
    KillTimer(ଖାଲି, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    ଅକ୍ଷର *
did_set_mzquantum(optset_T *args UNUSED)
{
#ଯଦି defined(MZSCHEME_GUI_THREADS)
    // reset timer
    ଯଦି (timer_id != 0)
	remove_timer();
    ଯଦି (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
notify_multithread(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    ଯଦି (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    ଯଦି (!on && timer_id != 0)
	remove_timer();
#endif
}

    ଶୂନ୍ୟ
mzscheme_end(ଶୂନ୍ୟ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#ଯଦି 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#ଯଦି HAVE_TLS_SPACE && !defined(VIMDLL)
# ଯଦି defined(_MSC_VER)
ସ୍ଥିର __declspec(thread) ଶୂନ୍ୟ *tls_space;
ବାହ୍ୟ intptr_t _tls_index;
# elif defined(__MINGW32__)
ସ୍ଥିର __thread ଶୂନ୍ୟ *tls_space;
ବାହ୍ୟ intptr_t _tls_index;
# ଅନ୍ୟଥା
ସ୍ଥିର THREAD_LOCAL ଶୂନ୍ୟ *tls_space;
ସ୍ଥିର intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_main(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argc = 0;
    ଅକ୍ଷର    *argv = ଖାଲି;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    ଯଦି (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	ଫେରନ୍ତୁ vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    ଶୂନ୍ୟ **ptls_space;
    intptr_t tls_index;
    ଶୂନ୍ୟ (*pget_tls_info)(ଶୂନ୍ୟ ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (ଶୂନ୍ୟ *)GetProcAddress(
				GetModuleHandle(ଖାଲି), "get_tls_info");
    ଯଦି (pget_tls_info == ଖାଲି)
    {
	disabled = TRUE;
	ଫେରନ୍ତୁ vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# ଅନ୍ୟଥା
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    ଫେରନ୍ତୁ scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ mzscheme_env_main(ଖାଲି, argc, &argv);
#endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_env_main(Scheme_Env *env, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, ଅକ୍ଷର **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#ଅନ୍ୟଥା
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ଖାଲି;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    ଗଦା_base = &__gc_var_stack__;
# ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dummy = 0;
    ଗଦା_base = (ଶୂନ୍ୟ *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର Scheme_Object*
load_base_module(ଶୂନ୍ୟ *data)
{
    scheme_namespace_require(scheme_intern_ପ୍ରତୀକ((ଅକ୍ଷର *)data));
    ଫେରନ୍ତୁ scheme_null;
}

    ସ୍ଥିର Scheme_Object *
load_base_module_on_error(ଶୂନ୍ୟ *data UNUSED)
{
    load_base_module_failed = TRUE;
    ଫେରନ୍ତୁ scheme_null;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
startup_mzscheme(ଶୂନ୍ୟ)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_ଗଦା_base(ଗଦା_base, 1);
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
#ଯଦି MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	ଯଦି (s != ଖାଲି)
	{
	    coll_path = scheme_make_path((ଅକ୍ଷର *)s);
	    MZ_GC_CHECK();
	    ଯଦି (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	ଯଦି (coll_path == ଖାଲି)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	ଯଦି (coll_path != ଖାଲି)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#ଅନ୍ୟଥା
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ଖାଲି;
	Scheme_Object	*coll_pair = ଖାଲି;
	Scheme_Config	*config = ଖାଲି;

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

#ଯଦି MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	ଯଦି (s != ଖାଲି)
	{
	    config_path = scheme_make_path((ଅକ୍ଷର *)s);
	    MZ_GC_CHECK();
	    ଯଦି (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	ଯଦି (config_path == ଖାଲି)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	ଯଦି (config_path != ଖାଲି)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#ଯଦି MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ଖାଲି, load_base_module, ଖାଲି,
				    load_base_module_on_error, "racket/base");
	ଯଦି (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(ଖାଲି, load_base_module, ଖାଲି,
				    load_base_module_on_error, "scheme/base");
	    ଯଦି (load_base_module_failed)
		ଫେରନ୍ତୁ -1;
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
	Scheme_Object	*make_security_guard = ଖାଲି;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# ଯଦି MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_ପ୍ରତୀକ = ଖାଲି;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_ପ୍ରତୀକ);
	    MZ_GC_REG();
	    make_security_guard_ପ୍ରତୀକ = scheme_intern_ପ୍ରତୀକ("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_ପ୍ରତୀକ, environment);
	    MZ_GC_UNREG();
	}
# ଅନ୍ୟଥା
	make_security_guard = scheme_builtin_ମୂଲ୍ୟ("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	ଯଦି (make_security_guard != ଖାଲି)
	{
	    Scheme_Object   *args[3] = {ଖାଲି, ଖାଲି, ଖାଲି};
	    Scheme_Object   *guard = ଖାଲି;
	    Scheme_Config   *config = ଖାଲି;
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

    ଫେରନ୍ତୁ 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_init(ଶୂନ୍ୟ)
{
    ଯଦି (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	ଯଦି (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    ଫେରନ୍ତୁ -1;
	}
#endif
	ଯଦି (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    ଫେରନ୍ତୁ -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ଖାଲି;
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

    ଫେରନ୍ତୁ 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_with_exn_handling(
    ଶୂନ୍ୟ		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *ମୂଲ୍ୟ = ଖାଲି;
    Scheme_Object   *exn = ଖାଲି;
    Scheme_Object   *prim = ଖାଲି;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, ମୂଲ୍ୟ);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    ମୂଲ୍ୟ = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    ଯଦି (!ମୂଲ୍ୟ)
    {
	ମୂଲ୍ୟ = extract_exn_message(exn);
	// Got an exn?
	ଯଦି (ମୂଲ୍ୟ)
	{
	    scheme_display(ମୂଲ୍ୟ, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (ret != ଖାଲି)	// if pointer to retval supported give it up
	*ret = ମୂଲ୍ୟ;
    // Print any result, as long as it's not a void
    ଅନ୍ୟଥା ଯଦି (!SCHEME_VOIDP(ମୂଲ୍ୟ))
    {
	scheme_display(ମୂଲ୍ୟ, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ OK;
}

/*
 * :mzscheme
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_mzscheme_command(exarg_T *eap, ଶୂନ୍ୟ *data, Scheme_Closed_Prim *what)
{
    ଯଦି (mzscheme_init())
	ଫେରନ୍ତୁ FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    ଫେରନ୍ତୁ eval_with_exn_handling(data, what, ଖାଲି);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    ଶୂନ୍ୟ
mzscheme_buffer_free(buf_T *buf)
{
    ଯଦି (buf->b_mzscheme_ref == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_mz_buffer *bp = ଖାଲି;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_ମୂଲ୍ୟ;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#ଅନ୍ୟଥା
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = ଖାଲି;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    ଶୂନ୍ୟ
mzscheme_window_free(win_T *win)
{
    ଯଦି (win->w_mzscheme_ref == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_mz_window *wp = ଖାଲି;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_ମୂଲ୍ୟ;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#ଅନ୍ୟଥା
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = ଖାଲି;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    ଶୂନ୍ୟ
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    ଯଦି (script == ଖାଲି)
	do_mzscheme_command(eap, eap->arg, do_eval);
    ଅନ୍ୟଥା
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ସ୍ଥିର Scheme_Object *
do_load(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *ପଦସମଷ୍ଟି = ଖାଲି;
    Scheme_Object   *result = ଖାଲି;
    ଅକ୍ଷର	    *file = ଖାଲି;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, ପଦସମଷ୍ଟି);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (ଅକ୍ଷର *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_ନିବେଶ_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    ଯେତେବେଳେ (!SCHEME_EOFP(ପଦସମଷ୍ଟି = scheme_read(pinfo->port)))
    {
	result = scheme_eval(ପଦସମଷ୍ଟି, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_ନିବେଶ_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = ଖାଲି;
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ result;
}

/*
 * :mzfile
 */
    ଶୂନ୍ୟ
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {ଖାଲି, ଖାଲି};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (ଅକ୍ଷର *)eap->arg;
    ଯଦି (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != ଖାଲି)	// looks like port was not closed
    {
	scheme_close_ନିବେଶ_port(pinfo.port);
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
    ସ୍ଥିର ଶୂନ୍ୟ
init_exn_catching_apply(ଶୂନ୍ୟ)
{
    ଯଦି (exn_catching_apply)
	ଫେରନ୍ତୁ;

    ଅକ୍ଷର *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_ମୂଲ୍ୟ("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_ମୂଲ୍ୟ("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ସ୍ଥିର Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    ଯଦି (SCHEME_TRUEP(SCHEME_CAR(v)))
	ଫେରନ୍ତୁ SCHEME_CDR(v);
    ଅନ୍ୟଥା
    {
	*exn = SCHEME_CDR(v);
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    ଯଦି (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	ଫେରନ୍ତୁ _scheme_apply(exn_message, 1, &v);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି; // Not an exn structure
}

    ସ୍ଥିର Scheme_Object *
do_eval(ଶୂନ୍ୟ *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    ଫେରନ୍ତୁ scheme_eval_string_all((ଅକ୍ଷର *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_intrnl_output(ଅକ୍ଷର *mesg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା error)
{
    ଅକ୍ଷର *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    ଯେତେବେଳେ (p)
    {
	*p = '\0';
	ଯଦି (error)
	    emsg(prev);
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    ଯଦି (error)
	emsg(prev);
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ(prev);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_output(ଅକ୍ଷର *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_err_output(ଅକ୍ଷର *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_printf(ଅକ୍ଷର *format, ...)
{
    do_intrnl_output(format, 1);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_flush(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    ଯଦି (length)
    {
	do_err_output(buff);
	ଫେରନ୍ତୁ;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    ଯଦି (length)
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
    ସ୍ଥିର Scheme_Object *
vim_command(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ଖାଲି;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_ମୂଲ୍ୟ(cmd), ଖାଲି, ଖାଲି, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    ଫେରନ୍ତୁ scheme_void;
}

/*
 * (eval {expr-string})
 */
    ସ୍ଥିର Scheme_Object *
vim_eval(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = ଖାଲି;
    typval_T		*vim_result;
    Scheme_Object *ପଦସମଷ୍ଟି = ଖାଲି;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, ପଦସମଷ୍ଟି);
    MZ_GC_REG();
    ପଦସମଷ୍ଟି = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_ପଦସମଷ୍ଟି(BYTE_STRING_ମୂଲ୍ୟ(ପଦସମଷ୍ଟି), ଖାଲି);

    ଯଦି (vim_result == ଖାଲି)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    ଫେରନ୍ତୁ scheme_false;
#endif
}

/*
 * (range-start)
 */
    ସ୍ଥିର Scheme_Object *
get_range_start(
    ଶୂନ୍ୟ		*data UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ଫେରନ୍ତୁ scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    ସ୍ଥିର Scheme_Object *
get_range_end(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
    ଫେରନ୍ତୁ scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    ସ୍ଥିର Scheme_Object *
mzscheme_beep(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    ଫେରନ୍ତୁ scheme_void;
}

ସ୍ଥିର Scheme_Object *M_global = ଖାଲି;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ସ୍ଥିର Scheme_Object *
get_option(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    ମୂଲ୍ୟ;
    ଅକ୍ଷର	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ଖାଲି;
    Scheme_Object   *name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    ଯଦି (argc > 1)
    {
	ଯଦି (M_global == ଖାଲି)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ପ୍ରତୀକ("global");
	    MZ_GC_CHECK();
	}

	ଯଦି (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ଅନ୍ୟଥା ଯଦି (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	ଅନ୍ୟଥା ଯଦି (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	ଅନ୍ୟଥା
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_ମୂଲ୍ୟ(BYTE_STRING_ମୂଲ୍ୟ(name), &ମୂଲ୍ୟ, (char_u **)&strval,
								  ଖାଲି, scope);
    curbuf = save_curb;
    curwin = save_curw;

    ବାଛ (rc)
    {
    ସ୍ଥିତି gov_bool:
    ସ୍ଥିତି gov_ସଂଖ୍ୟା:
	MZ_GC_UNREG();
	ଫେରନ୍ତୁ scheme_make_integer_ମୂଲ୍ୟ(ମୂଲ୍ୟ);
    ସ୍ଥିତି gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	ଫେରନ୍ତୁ rval;
    ସ୍ଥିତି gov_hidden_bool:
    ସ୍ଥିତି gov_hidden_ସଂଖ୍ୟା:
    ସ୍ଥିତି gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    ସ୍ଥିତି gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    ଫେରନ୍ତୁ scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ସ୍ଥିର Scheme_Object *
set_option(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    char_u	*command = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ଖାଲି;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    ଯଦି (argc > 1)
    {
	ଯଦି (M_global == ଖାଲି)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_ପ୍ରତୀକ("global");
	    MZ_GC_CHECK();
	}

	ଯଦି (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	ଅନ୍ୟଥା ଯଦି (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	ଅନ୍ୟଥା ଯଦି (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	ଅନ୍ୟଥା
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_ମୂଲ୍ୟ(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    ଫେରନ୍ତୁ scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ସ୍ଥିର Scheme_Object *
get_curr_win(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
    ଫେରନ୍ତୁ (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    ସ୍ଥିର Scheme_Object *
get_window_ଗଣନା(
    ଶୂନ୍ୟ		*data UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    ଫେରନ୍ତୁ scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ସ୍ଥିର Scheme_Object *
get_window_list(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    ପାଇଁ ( ; w != ଖାଲି; w = w->w_next)
	ଯଦି (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    ଫେରନ୍ତୁ list;
}

    ସ୍ଥିର Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = ଖାଲି;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    ଯଦି (win->w_mzscheme_ref != ଖାଲି)
	ଫେରନ୍ତୁ (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ଆକାର(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ଖାଲି);
#ଅନ୍ୟଥା
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ସ୍ଥିର Scheme_Object *
get_window_ଅଙ୍କ(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    ପାଇଁ (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    ଫେରନ୍ତୁ scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ସ୍ଥିର Scheme_Object *
get_window_by_ଅଙ୍କ(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ଯଦି (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    ପାଇଁ ( ; win != ଖାଲି; win = win->w_next, --fnum)
	ଯଦି (fnum == 1)	    // to be 1-based
	    ଫେରନ୍ତୁ window_new(win);

    ଫେରନ୍ତୁ scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ସ୍ଥିର Scheme_Object *
get_window_buffer(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    ସ୍ଥିର Scheme_Object *
get_window_height(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    ସ୍ଥିର Scheme_Object *
set_window_height(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    height;

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
    ଫେରନ୍ତୁ scheme_void;
}

/*
 * (get-win-width [window])
 */
    ସ୍ଥିର Scheme_Object *
get_window_width(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    ସ୍ଥିର Scheme_Object *
set_window_width(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    width = 0;

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
    ଫେରନ୍ତୁ scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ସ୍ଥିର Scheme_Object *
get_cursor(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    ଫେରନ୍ତୁ scheme_make_pair(scheme_make_integer_ମୂଲ୍ୟ((long)pos.lnum),
		    scheme_make_integer_ମୂଲ୍ୟ((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ସ୍ଥିର Scheme_Object *
set_cursor(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
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

    ଯଦି (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_ଧାଡ଼ି_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    ଫେରନ୍ତୁ scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ସ୍ଥିର Scheme_Object *
mzscheme_open_buffer(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ = 0;
    Scheme_Object   *onum = ଖାଲି;
    Scheme_Object   *buf = ଖାଲି;
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
    ଅଙ୍କ = buflist_add(BYTE_STRING_ମୂଲ୍ୟ(fname), BLN_LISTED | BLN_CURBUF);

    ଯଦି (ଅଙ୍କ == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(ଅଙ୍କ);
    buf = get_buffer_by_ଅଙ୍କ(data, 1, &onum);
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_by_ଅଙ୍କ(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	ଯଦି (buf->b_fnum == fnum)
	    ଫେରନ୍ତୁ buffer_new(buf);

    ଫେରନ୍ତୁ scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_by_name(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = ଖାଲି;
    Scheme_Object *fname = ଖାଲି;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	ଯଦି (buf->b_ffname == ଖାଲି || buf->b_sfname == ଖାଲି)
	    // empty string
	{
	    ଯଦି (BYTE_STRING_ମୂଲ୍ୟ(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	ଅନ୍ୟଥା ଯଦି (!fnamecmp(buf->b_ffname, BYTE_STRING_ମୂଲ୍ୟ(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_ମୂଲ୍ୟ(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ buffer;
}

/*
 * (get-next-buff [buffer])
 */
    ସ୍ଥିର Scheme_Object *
get_next_buffer(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    ଯଦି (buf->b_next == ଖାଲି)
	ଫେରନ୍ତୁ scheme_false;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    ସ୍ଥିର Scheme_Object *
get_prev_buffer(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    ଯଦି (buf->b_prev == ଖାଲି)
	ଫେରନ୍ତୁ scheme_false;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_ଅଙ୍କ(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_ଗଣନା(
    ଶୂନ୍ୟ		*data UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    ଫେରନ୍ତୁ scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_name(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ scheme_make_byte_string((ଅକ୍ଷର *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ସ୍ଥିର Scheme_Object *
get_curr_buffer(
    ଶୂନ୍ୟ		*data UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    ଫେରନ୍ତୁ (Scheme_Object *)get_vim_curr_buffer();
}

    ସ୍ଥିର Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = ଖାଲି;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    ଯଦି (buf->b_mzscheme_ref)
	ଫେରନ୍ତୁ (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, ଆକାର(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ଖାଲି);
#ଅନ୍ୟଥା
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_size(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    ଫେରନ୍ତୁ scheme_make_integer(buf->buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_ଧାଡ଼ି(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    linenr;
    char_u	    *ଧାଡ଼ି;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ଧାଡ଼ି = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    ଫେରନ୍ତୁ scheme_make_byte_string((ଅକ୍ଷର *)ଧାଡ଼ି);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ସ୍ଥିର Scheme_Object *
get_buffer_ଧାଡ଼ି_list(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i, hi, lo, n;
    Scheme_Object   *list = ଖାଲି;

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
    ଯଦି (lo < 0)
	lo = 0;

    ଯଦି (hi < 0)
	hi = 0;
    ଯଦି (hi < lo)
	hi = lo;

    n = hi - lo;

    ପାଇଁ (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (ଅକ୍ଷର *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ list;
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
    ସ୍ଥିର Scheme_Object *
set_buffer_ଧାଡ଼ି(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *ଧାଡ଼ି = ଖାଲି;
    ଅକ୍ଷର	    *save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ଧାଡ଼ି);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    ଯଦି (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    ଧାଡ଼ି = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_ଧାଡ଼ି_range(n, buf->buf);

    ଯଦି (SCHEME_FALSEP(ଧାଡ଼ି))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	ଯଦି (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ଅନ୍ୟଥା ଯଦି (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	ଯଦି (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ଫେରନ୍ତୁ scheme_void;
    }
    ଅନ୍ୟଥା
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_ଧାଡ଼ି(ଧାଡ଼ି);

	curbuf = buf->buf;

	ଯଦି (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ଅନ୍ୟଥା ଯଦି (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	ଅନ୍ୟଥା
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	ଯଦି (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	ଫେରନ୍ତୁ scheme_void;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
free_array(ଅକ୍ଷର **array)
{
    ଅକ୍ଷର **curr = array;
    ଯେତେବେଳେ (*curr != ଖାଲି)
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
    ସ୍ଥିର Scheme_Object *
set_buffer_ଧାଡ଼ି_list(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ଖାଲି;
    Scheme_Object   *ଧାଡ଼ି_list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ଧାଡ଼ି_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    ଯଦି (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    ଧାଡ଼ି_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    ଯଦି (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_ଧାଡ଼ି_range(lo, buf->buf);	    // inclusive
    check_ଧାଡ଼ି_range(hi - 1, buf->buf);	    // exclusive

    ଯଦି (SCHEME_FALSEP(ଧାଡ଼ି_list) || SCHEME_NULLP(ଧାଡ଼ି_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	ଯଦି (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < old_len; i++)
		ଯଦି (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    ଯଦି (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ଫେରନ୍ତୁ scheme_void;
    }
    ଅନ୍ୟଥା
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(ଧାଡ଼ି_list);
	MZ_GC_CHECK();
	ଯଦି (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	ଅନ୍ୟଥା
	{
	    ଅକ୍ଷର		**array = ଖାଲି;
	    Scheme_Object   *ଧାଡ଼ି = ଖାଲି;
	    Scheme_Object   *rest = ଖାଲି;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, ଧାଡ଼ି);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(ଅକ୍ଷର *, new_len + 1);

	    rest = ଧାଡ଼ି_list;
	    ପାଇଁ (i = 0; i < new_len; ++i)
	    {
		ଧାଡ଼ି = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		ଯଦି (!SCHEME_STRINGP(ଧାଡ଼ି))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_ଧାଡ଼ି(ଧାଡ଼ି);
	    }

	    curbuf = buf->buf;

	    ଯଦି (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    ପାଇଁ (i = 0; i < old_len - new_len; ++i)
	    {
		ଯଦି (ml_delete((linenr_T)lo) == FAIL)
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
	    ପାଇଁ (i = 0; i < old_len && i < new_len; i++)
		ଯଦି (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    ଯେତେବେଳେ (i < new_len)
	    {
		ଯଦି (ml_append((linenr_T)(lo + i - 1),
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
	changed_ଧାଡ଼ିଗୁଡ଼ିକ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	ଯଦି (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	ଫେରନ୍ତୁ scheme_void;
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
    ସ୍ଥିର Scheme_Object *
ଭର୍ତ୍ତି_buffer_ଧାଡ଼ି_list(ଶୂନ୍ୟ *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ଖାଲି;
    Scheme_Object   *list = ଖାଲି;
    ଅକ୍ଷର	    *str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i, n, size;

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

    ଯଦି (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    ଯଦି (n != 0)	    // 0 can be used in insert
	check_ଧାଡ଼ି_range(n, buf->buf);
    ଯଦି (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_ଧାଡ଼ି(list);
	curbuf = buf->buf;

	ଯଦି (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ଅନ୍ୟଥା ଯଦି (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	ଅନ୍ୟଥା
	{
	    vim_free(str);
	    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	ଫେରନ୍ତୁ scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    ଯଦି (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    ଅନ୍ୟଥା
    {
	Scheme_Object   *ଧାଡ଼ି = ଖାଲି;
	Scheme_Object   *rest = ଖାଲି;
	ଅକ୍ଷର		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ଧାଡ଼ି);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(ଅକ୍ଷର *, size + 1);

	rest = list;
	ପାଇଁ (i = 0; i < size; ++i)
	{
	    ଧାଡ଼ି = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_ଧାଡ଼ି(ଧାଡ଼ି);
	}

	curbuf = buf->buf;

	ଯଦି (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; i < size; ++i)
		ଯଦି (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    ଯଦି (i > 0)
		appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    ଫେରନ୍ତୁ scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ସ୍ଥିର Scheme_Object *
vim_bufferp(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv)
{
    ଯଦି (SCHEME_VIMBUFFERP(argv[0]))
	ଫେରନ୍ତୁ scheme_true;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ scheme_false;
}

/*
 * (win? obj)
 */
    ସ୍ଥିର Scheme_Object *
vim_windowp(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv)
{
    ଯଦି (SCHEME_VIMWINDOWP(argv[0]))
	ଫେରନ୍ତୁ scheme_true;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ସ୍ଥିର Scheme_Object *
vim_buffer_validp(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv)
{
    ଯଦି (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ scheme_true;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ scheme_false;
}

/*
 * (win-valid? obj)
 */
    ସ୍ଥିର Scheme_Object *
vim_window_validp(ଶୂନ୍ୟ *data UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv)
{
    ଯଦି (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ scheme_true;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ scheme_false;
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
    ସ୍ଥିର ଅକ୍ଷର *
string_to_ଧାଡ଼ି(Scheme_Object *obj)
{
    ଅକ୍ଷର	*scheme_str = ଖାଲି;
    ଅକ୍ଷର	*vim_str = ଖାଲି;
    OUTPUT_LEN_TYPE	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    ଯଦି (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    ପାଇଁ (i = 0; i < len; ++i)
    {
	ଯଦି (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	ଅନ୍ୟଥା
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    ଫେରନ୍ତୁ vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ସ୍ଥିର Scheme_Object *
vim_to_mzscheme(typval_T *vim_ମୂଲ୍ୟ)
{
    Scheme_Object	*result = ଖାଲି;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = ଖାଲି;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_ମୂଲ୍ୟ, 1, visited);

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_ମୂଲ୍ୟ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    new_ମୂଲ୍ୟ_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    ଯଦି (depth > 100)
    {
	MZ_GC_UNREG();
	ଫେରନ୍ତୁ scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_ମୂଲ୍ୟ);
    MZ_GC_CHECK();
    ଯଦି (result != ଖାଲି) // found, do nothing
	new_ମୂଲ୍ୟ_2 = FALSE;
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((ଅକ୍ଷର *)vim_ମୂଲ୍ୟ->vval.v_string);
	MZ_GC_CHECK();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_ସଂଖ୍ୟା)
    {
	result = scheme_make_integer((long)vim_ମୂଲ୍ୟ->vval.v_ସଂଖ୍ୟା);
	MZ_GC_CHECK();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((ଦ୍ୱିଗୁଣ)vim_ମୂଲ୍ୟ->vval.v_float);
	MZ_GC_CHECK();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_LIST)
    {
	list_T		*list = vim_ମୂଲ୍ୟ->vval.v_list;
	listitem_T	*curr;

	ଯଦି (list == ଖାଲି || list->lv_first == ଖାଲି)
	    result = scheme_null;
	ଅନ୍ୟଥା
	{
	    Scheme_Object   *obj = ଖାଲି;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    ଯେତେବେଳେ (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_DICT)
    {
	Scheme_Object	  *ଚାବି = ଖାଲି;
	Scheme_Object	  *obj = ଖାଲି;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ଚାବି);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	ଯଦି (vim_ମୂଲ୍ୟ->vval.v_dict != ଖାଲି)
	{
	    hashtab_T	*ht = &vim_ମୂଲ୍ୟ->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
	    {
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    ଚାବି = scheme_make_byte_string((ଅକ୍ଷର *)hi->hi_ଚାବି);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, ଚାବି, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ଖାଲି;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((ଅକ୍ଷର *)vim_ମୂଲ୍ୟ->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ଅଚଳ ଅକ୍ଷର *)BYTE_STRING_ମୂଲ୍ୟ(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_PARTIAL)
    {
	ଯଦି (vim_ମୂଲ୍ୟ->vval.v_partial == ଖାଲି)
	    result = scheme_null;
	ଅନ୍ୟଥା
	{
	    Scheme_Object *funcname = ଖାଲି;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (ଅକ୍ଷର *)partial_name(vim_ମୂଲ୍ୟ->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ଅଚଳ ଅକ୍ଷର *)BYTE_STRING_ମୂଲ୍ୟ(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    ଅନ୍ୟଥା ଯଦି (vim_ମୂଲ୍ୟ->v_type == VAR_BOOL || vim_ମୂଲ୍ୟ->v_type == VAR_SPECIAL)
    {
	ଯଦି (vim_ମୂଲ୍ୟ->vval.v_ସଂଖ୍ୟା <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_ମୂଲ୍ୟ->vval.v_ସଂଖ୍ୟା);
	ଅନ୍ୟଥା
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    ଅନ୍ୟଥା
    {
	result = scheme_void;
	new_ମୂଲ୍ୟ_2 = FALSE;
    }
    ଯଦି (new_ମୂଲ୍ୟ_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_ମୂଲ୍ୟ, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, status;
    Scheme_Hash_Table	*visited = ଖାଲି;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    ପାଇଁ (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	ଯଦି (visited->vals[i] != ଖାଲି)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    ଫେରନ୍ତୁ status;
}
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth,
	Scheme_Hash_Table *visited)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    ଯଦି (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = 0;
	ଫେରନ୍ତୁ FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    ଯଦି (found != ଖାଲି)
	copy_tv(found, tv);
    ଅନ୍ୟଥା ଯଦି (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
    }
    ଅନ୍ୟଥା ଯଦି (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = SCHEME_INT_VAL(obj);
    }
    ଅନ୍ୟଥା ଯଦି (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_ସଂଖ୍ୟା = SCHEME_TRUEP(obj);
    }
    ଅନ୍ୟଥା ଯଦି (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    ଅନ୍ୟଥା ଯଦି (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ମୂଲ୍ୟ(obj));
    }
# ଯଦି MZSCHEME_VERSION_MAJOR >= 299
    ଅନ୍ୟଥା ଯଦି (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ଖାଲି;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_ମୂଲ୍ୟ(tmp));
	MZ_GC_UNREG();
    }
# endif
    ଅନ୍ୟଥା ଯଦି (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	ଯଦି (list == ଖାଲି)
	    status = FAIL;
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
	    Scheme_Object   *curr = ଖାଲି;
	    Scheme_Object   *cval = ଖାଲି;
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
	    ଯଦି (v == ଖାଲି)
		status = FAIL;
	    ଅନ୍ୟଥା
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		ଯଦି (SCHEME_VECTORP(obj))
		{
		    ପାଇଁ (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ଯଦି (status == FAIL)
			    ଭାଙ୍ଗ;
			status = list_append_tv(list, v);
			clear_tv(v);
			ଯଦି (status == FAIL)
			    ଭାଙ୍ଗ;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    ପାଇଁ (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ଯଦି (status == FAIL)
			    ଭାଙ୍ଗ;
			status = list_append_tv(list, v);
			clear_tv(v);
			ଯଦି (status == FAIL)
			    ଭାଙ୍ଗ;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    ଯଦି (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			ଯଦି (status == OK)
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
    ଅନ୍ୟଥା ଯଦି (SCHEME_HASHTP(obj))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	dict_T		*dict;
	Scheme_Object   *ଚାବି = ଖାଲି;
	Scheme_Object   *val = ଖାଲି;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ଚାବି);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	ଯଦି (dict == ଖାଲି)
	    status = FAIL;
	ଅନ୍ୟଥା
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    ପାଇଁ (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		ଯଦି (((Scheme_Hash_Table *) obj)->vals[i] != ଖାଲି)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_ଧାଡ଼ି(
				((Scheme_Hash_Table *) obj)->ଚାବିଗୁଡ଼ିକ[i]));
		    // convert Scheme val to Vim and add it to the dict
		    ଯଦି (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			ଭାଙ୍ଗ;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    ଅନ୍ୟଥା
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_ଧାଡ଼ି(obj);
    }
    MZ_GC_UNREG();
    ଫେରନ୍ତୁ status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ସ୍ଥିର Scheme_Object *
vim_funcref(ଶୂନ୍ୟ *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    typval_T args;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = OK;
    Scheme_Object *result = ଖାଲି;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    ଯଦି (list == ଖାଲି)
	status = FAIL;
    ଅନ୍ୟଥା
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	ପାଇଁ (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    ଯଦି (v == ଖାଲି)
		status = FAIL;
	    ଅନ୍ୟଥା
	    {
		status = mzscheme_to_vim(argv[i], v);
		ଯଦି (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	ଯଦି (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_ମୂଲ୍ୟ((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    ଯଦି (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    ଅନ୍ୟଥା
	raise_if_error();
    ଫେରନ୍ତୁ result;
}

    ଶୂନ୍ୟ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ଖାଲି;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    ଯଦି (mzscheme_init())
    {
	MZ_GC_UNREG();
	ଫେରନ୍ତୁ;
    }

    MZ_GC_CHECK();
    ଯଦି (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_error_check(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ସ୍ଥିର ଶୂନ୍ୟ
register_vim_exn(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    Scheme_Object   *struct_exn = ଖାଲି;
    Scheme_Object   *exn_name = ଖାଲି;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_ପ୍ରତୀକ("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_ମୂଲ୍ୟ("struct:exn");
    MZ_GC_CHECK();

    ଯଦି (vim_exn == ଖାଲି)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ଖାଲି, 0, 0, ଖାଲି, ଖାଲି
#ଯଦି MZSCHEME_VERSION_MAJOR >= 299
		, ଖାଲି
#endif
		);


    {
	Scheme_Object   **tmp = ଖାଲି;
	Scheme_Object   *exn_names[5] = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};
	Scheme_Object   *exn_ମୂଲ୍ୟଗୁଡ଼ିକ[5] = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_ମୂଲ୍ୟଗୁଡ଼ିକ, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * ଆକାର(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_ମୂଲ୍ୟଗୁଡ଼ିକ(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_ମୂଲ୍ୟଗୁଡ଼ିକ, tmp, nc * ଆକାର(Scheme_Object *));
	MZ_GC_CHECK();

	ପାଇଁ (i = 0; i < nc; i++)
	{
	    scheme_add_global_ପ୍ରତୀକ(exn_names[i],
		    exn_ମୂଲ୍ୟଗୁଡ଼ିକ[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    ଶୂନ୍ୟ
raise_vim_exn(ଅଚଳ ଅକ୍ଷର *add_info)
{
    ଅକ୍ଷର	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ଖାଲି, ଖାଲି};
    Scheme_Object   *exn = ଖାଲି;
    Scheme_Object   *byte_string = ଖାଲି;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    ଯଦି (add_info != ଖାଲି)
    {
	ଅକ୍ଷର		*c_string = ଖାଲି;
	Scheme_Object   *info = ଖାଲି;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fmt), 1, &info, ଖାଲି);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    ଅନ୍ୟଥା
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#ଯଦି MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#ଅନ୍ୟଥା
    argv[1] = scheme_current_continuation_marks(ଖାଲି);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    ଶୂନ୍ୟ
raise_if_error(ଶୂନ୍ୟ)
{
    ଯଦି (vim_error_check())
	raise_vim_exn(ଖାଲି);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ସ୍ଥିର vim_mz_buffer *
get_buffer_arg(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    ଯଦି (argc < argnum + 1)
	ଫେରନ୍ତୁ get_vim_curr_buffer();
    ଯଦି (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (ଶୂନ୍ୟ)get_valid_buffer(argv[argnum]);
    ଫେରନ୍ତୁ b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ସ୍ଥିର vim_mz_window *
get_window_arg(ଅଚଳ ଅକ୍ଷର *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    ଯଦି (argc < argnum + 1)
	ଫେରନ୍ତୁ get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    ଯଦି (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (ଶୂନ୍ୟ)get_valid_window(argv[argnum]);
    ଫେରନ୍ତୁ w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(ଶୂନ୍ୟ *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    ଯଦି (buf == INVALID_BUFFER_ମୂଲ୍ୟ)
	scheme_signal_error(_("buffer is invalid"));
    ଫେରନ୍ତୁ buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(ଶୂନ୍ୟ *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    ଯଦି (win == INVALID_WINDOW_ମୂଲ୍ୟ)
	scheme_signal_error(_("window is invalid"));
    ଫେରନ୍ତୁ win;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mzthreads_allowed(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ mz_threads_allow;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଧାଡ଼ି_in_range(linenr_T lnum, buf_T *buf)
{
    ଫେରନ୍ତୁ (lnum > 0 && lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

    ସ୍ଥିର ଶୂନ୍ୟ
check_ଧାଡ଼ି_range(linenr_T lnum, buf_T *buf)
{
    ଯଦି (!ଧାଡ଼ି_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mz_fix_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra)
{
    ଯଦି (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	ଯଦି (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	ଅନ୍ୟଥା ଯଦି (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	ଅନ୍ୟଥା
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

ସ୍ଥିର Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_ଧାଡ଼ି, "get-buff-line", 1, 2},
    {set_buffer_ଧାଡ଼ି, "set-buff-line", 2, 3},
    {get_buffer_ଧାଡ଼ି_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_ଅଙ୍କ, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_ଧାଡ଼ି_list, "set-buff-line-list", 3, 4},
    {ଭର୍ତ୍ତି_buffer_ଧାଡ଼ି_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_ଗଣନା, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_ଅଙ୍କ, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_ଗଣନା, "win-count", 0, 0},
    {get_window_by_ଅଙ୍କ, "get-win-by-num", 1, 1},
    {get_window_ଅଙ୍କ, "get-win-num", 0, 1},
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
    ସ୍ଥିର vim_mz_buffer *
get_vim_curr_buffer(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf->b_mzscheme_ref == ଖାଲି)
	ଫେରନ୍ତୁ (vim_mz_buffer *)buffer_new(curbuf);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ସ୍ଥିର vim_mz_window *
get_vim_curr_window(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_mzscheme_ref == ଖାଲି)
	ଫେରନ୍ତୁ (vim_mz_window *)window_new(curwin);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ WINDOW_REF(curwin);
}

    ସ୍ଥିର ଶୂନ୍ୟ
make_modules(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    Scheme_Env	    *mod = ଖାଲି;
    Scheme_Object   *vimext_ପ୍ରତୀକ = ଖାଲି;
    Scheme_Object   *closed_prim = ଖାଲି;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_ପ୍ରତୀକ);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_ପ୍ରତୀକ = scheme_intern_ପ୍ରତୀକ("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_ପ୍ରତୀକ, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(prims); i++)
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
ସ୍ଥିର Scheme_Object *M_write = ଖାଲି;
ସ୍ଥିର Scheme_Object *M_read = ଖାଲି;
ସ୍ଥିର Scheme_Object *M_execute = ଖାଲି;
ସ୍ଥିର Scheme_Object *M_delete = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
sandbox_check(ଶୂନ୍ୟ)
{
    ଯଦି (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ସ୍ଥିର Scheme_Object *
sandbox_file_guard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv)
{
    ଯଦି (!sandbox)
	ଫେରନ୍ତୁ scheme_void;

    Scheme_Object *requested_access = argv[2];

    ଯଦି (M_write == ଖାଲି)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_ପ୍ରତୀକ("write");
	MZ_GC_CHECK();
    }
    ଯଦି (M_read == ଖାଲି)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_ପ୍ରତୀକ("read");
	MZ_GC_CHECK();
    }
    ଯଦି (M_execute == ଖାଲି)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_ପ୍ରତୀକ("execute");
	MZ_GC_CHECK();
    }
    ଯଦି (M_delete == ଖାଲି)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_ପ୍ରତୀକ("delete");
	MZ_GC_CHECK();
    }

    ଯେତେବେଳେ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	ଯଦି (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    ଫେରନ୍ତୁ scheme_void;
}

    ସ୍ଥିର Scheme_Object *
sandbox_network_guard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, Scheme_Object **argv UNUSED)
{
    ଫେରନ୍ତୁ scheme_void;
}
#endif

#endif
