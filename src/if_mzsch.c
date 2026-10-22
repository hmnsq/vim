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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#જો defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#જો MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#જો defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

પ્રકારવ્યાખ્યા સંરચના
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_કિંમત ((buf_T *)(-1))

પ્રકારવ્યાખ્યા સંરચના
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_કિંમત ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
પ્રકારવ્યાખ્યા સંરચના
{
    Scheme_Closed_Prim	*prim;
    અક્ષર	*name;
    પૂર્ણાંક		mina;	// arity information
    પૂર્ણાંક		maxa;
} Vim_Prim;

પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર	    *name;
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
સ્થિર Scheme_Object *sandbox_file_guard(પૂર્ણાંક, Scheme_Object **);
સ્થિર Scheme_Object *sandbox_network_guard(પૂર્ણાંક, Scheme_Object **);
સ્થિર શૂન્ય sandbox_check(શૂન્ય);
#endif
//  Buffer-related commands
સ્થિર Scheme_Object *buffer_new(buf_T *buf);
સ્થિર Scheme_Object *get_buffer_by_અંક(શૂન્ય *, પૂર્ણાંક, Scheme_Object **);
સ્થિર vim_mz_buffer *get_vim_curr_buffer(શૂન્ય);

//  Window-related commands
સ્થિર Scheme_Object *window_new(win_T *win);
સ્થિર vim_mz_window *get_vim_curr_window(શૂન્ય);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
સ્થિર પૂર્ણાંક vim_error_check(શૂન્ય);
સ્થિર પૂર્ણાંક do_mzscheme_command(exarg_T *, શૂન્ય *, Scheme_Closed_Prim *what);
સ્થિર પૂર્ણાંક startup_mzscheme(શૂન્ય);
સ્થિર અક્ષર *string_to_લીટી(Scheme_Object *obj);
#જો MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#નહીંતર
# define OUTPUT_LEN_TYPE long
#endif
સ્થિર શૂન્ય do_output(અક્ષર *mesg, OUTPUT_LEN_TYPE len);
સ્થિર શૂન્ય do_printf(અક્ષર *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
સ્થિર શૂન્ય do_flush(શૂન્ય);
સ્થિર Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
સ્થિર Scheme_Object *extract_exn_message(Scheme_Object *v);
સ્થિર Scheme_Object *do_eval(શૂન્ય *, પૂર્ણાંક noargc, Scheme_Object **noargv);
સ્થિર Scheme_Object *do_load(શૂન્ય *, પૂર્ણાંક noargc, Scheme_Object **noargv);
સ્થિર શૂન્ય register_vim_exn(શૂન્ય);
સ્થિર vim_mz_buffer *get_buffer_arg(અચલ અક્ષર *fname, પૂર્ણાંક argnum,
	પૂર્ણાંક argc, Scheme_Object **argv);
સ્થિર vim_mz_window *get_window_arg(અચલ અક્ષર *fname, પૂર્ણાંક argnum,
	પૂર્ણાંક argc, Scheme_Object **argv);
સ્થિર પૂર્ણાંક લીટી_in_range(linenr_T, buf_T *);
સ્થિર શૂન્ય check_લીટી_range(linenr_T, buf_T *);
સ્થિર શૂન્ય mz_fix_cursor(પૂર્ણાંક lo, પૂર્ણાંક hi, પૂર્ણાંક extra);

સ્થિર પૂર્ણાંક eval_with_exn_handling(શૂન્ય *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
સ્થિર શૂન્ય make_modules(શૂન્ય);
સ્થિર શૂન્ય init_exn_catching_apply(શૂન્ય);
સ્થિર પૂર્ણાંક mzscheme_env_main(Scheme_Env *env, પૂર્ણાંક argc, અક્ષર **argv);
સ્થિર પૂર્ણાંક mzscheme_init(શૂન્ય);
#ifdef FEAT_EVAL
સ્થિર Scheme_Object *vim_to_mzscheme(typval_T *vim_કિંમત);
સ્થિર Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_કિંમત, પૂર્ણાંક depth,
	Scheme_Hash_Table *visited);
સ્થિર પૂર્ણાંક mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
સ્થિર પૂર્ણાંક mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, પૂર્ણાંક depth,
	Scheme_Hash_Table *visited);
સ્થિર Scheme_Object *vim_funcref(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
સ્થિર પૂર્ણાંક buffer_size_proc(શૂન્ય *obj UNUSED)
{
    પાછા gc_BYTES_TO_શબ્દો(કદ(vim_mz_buffer));
}
સ્થિર પૂર્ણાંક buffer_mark_proc(શૂન્ય *obj)
{
    પાછા buffer_size_proc(obj);
}
સ્થિર પૂર્ણાંક buffer_fixup_proc(શૂન્ય *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    પાછા buffer_size_proc(obj);
}
સ્થિર પૂર્ણાંક window_size_proc(શૂન્ય *obj UNUSED)
{
    પાછા gc_BYTES_TO_શબ્દો(કદ(vim_mz_window));
}
સ્થિર પૂર્ણાંક window_mark_proc(શૂન્ય *obj)
{
    પાછા window_size_proc(obj);
}
સ્થિર પૂર્ણાંક window_fixup_proc(શૂન્ય *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    પાછા window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#નહીંતર
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#જો defined(DYNAMIC_MZSCHEME)
સ્થિર Scheme_Object *dll_scheme_eof;
સ્થિર Scheme_Object *dll_scheme_false;
સ્થિર Scheme_Object *dll_scheme_void;
સ્થિર Scheme_Object *dll_scheme_null;
સ્થિર Scheme_Object *dll_scheme_true;

સ્થિર Scheme_Thread **dll_scheme_current_thread_ptr;

સ્થિર શૂન્ય (**dll_scheme_console_printf_ptr)(અક્ષર *str, ...);
સ્થિર શૂન્ય (**dll_scheme_console_output_ptr)(અક્ષર *str, OUTPUT_LEN_TYPE len);
સ્થિર શૂન્ય (**dll_scheme_notify_multithread_ptr)(પૂર્ણાંક on);

સ્થિર શૂન્ય *(*dll_GC_malloc)(size_t size_in_bytes);
સ્થિર શૂન્ય *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
સ્થિર Scheme_Env *(*dll_scheme_basic_env)(શૂન્ય);
સ્થિર શૂન્ય (*dll_scheme_check_threads)(શૂન્ય);
સ્થિર શૂન્ય (*dll_scheme_register_static)(શૂન્ય *ptr, long size);
સ્થિર શૂન્ય (*dll_scheme_set_ઢગલો_base)(શૂન્ય *base, પૂર્ણાંક no_auto_statics);
સ્થિર શૂન્ય (*dll_scheme_add_global)(અચલ અક્ષર *name, Scheme_Object *val,
	Scheme_Env *env);
સ્થિર શૂન્ય (*dll_scheme_add_global_પ્રતીક)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
સ્થિર Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, પૂર્ણાંક અંક_rands,
	Scheme_Object **rands);
સ્થિર Scheme_Object *(*dll_scheme_builtin_કિંમત)(અચલ અક્ષર *name);
# જો MZSCHEME_VERSION_MAJOR >= 299
સ્થિર Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
સ્થિર Scheme_Object *(*dll_scheme_make_path)(અચલ અક્ષર *chars);
# endif
સ્થિર શૂન્ય (*dll_scheme_close_આગત_port)(Scheme_Object *port);
સ્થિર શૂન્ય (*dll_scheme_ગણતરી_લીટીઓ)(Scheme_Object *port);
# જો MZSCHEME_VERSION_MAJOR < 360
સ્થિર Scheme_Object *(*dll_scheme_current_continuation_marks)(શૂન્ય);
# નહીંતર
સ્થિર Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
સ્થિર શૂન્ય (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
સ્થિર અક્ષર *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
સ્થિર પૂર્ણાંક (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
સ્થિર Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	પૂર્ણાંક અંક_rands_2, Scheme_Object **rands, પૂર્ણાંક val);
સ્થિર શૂન્ય (*dll_scheme_dont_gc_ptr)(શૂન્ય *p);
સ્થિર Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
સ્થિર Scheme_Object *(*dll_scheme_eval_string)(અચલ અક્ષર *str,
	Scheme_Env *env);
સ્થિર Scheme_Object *(*dll_scheme_eval_string_all)(અચલ અક્ષર *str,
	Scheme_Env *env, પૂર્ણાંક all);
સ્થિર શૂન્ય (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# જો MZSCHEME_VERSION_MAJOR < 299
સ્થિર અક્ષર *(*dll_scheme_format)(અક્ષર *format, પૂર્ણાંક flen, પૂર્ણાંક argc,
	Scheme_Object **argv, long *rlen);
# નહીંતર
સ્થિર અક્ષર *(*dll_scheme_format_utf8)(અક્ષર *format, પૂર્ણાંક flen, પૂર્ણાંક argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
સ્થિર Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, પૂર્ણાંક pos);
# endif
સ્થિર શૂન્ય (*dll_scheme_gc_ptr_ok)(શૂન્ય *p);
# જો MZSCHEME_VERSION_MAJOR < 299
સ્થિર અક્ષર *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# નહીંતર
સ્થિર અક્ષર *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
સ્થિર Scheme_Object *(*dll_scheme_intern_પ્રતીક)(અચલ અક્ષર *name);
સ્થિર Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *પ્રતીક,
	Scheme_Env *env);
સ્થિર Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, શૂન્ય *data, અચલ અક્ષર *name, mzshort mina,
     mzshort maxa);
સ્થિર Scheme_Object *(*dll_scheme_make_integer_કિંમત)(long i);
સ્થિર Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
સ્થિર Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	અચલ અક્ષર *name, mzshort mina, mzshort maxa);
# જો MZSCHEME_VERSION_MAJOR < 299
સ્થિર Scheme_Object *(*dll_scheme_make_string)(અચલ અક્ષર *chars);
સ્થિર Scheme_Object *(*dll_scheme_make_string_output_port)();
# નહીંતર
સ્થિર Scheme_Object *(*dll_scheme_make_byte_string)(અચલ અક્ષર *chars);
સ્થિર Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
સ્થિર Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	પૂર્ણાંક argc, Scheme_Object **argv);
સ્થિર Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, પૂર્ણાંક flags, પૂર્ણાંક *ગણતરી_out);
સ્થિર Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, પૂર્ણાંક અંક_fields,
	પૂર્ણાંક અંક_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# જો MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
સ્થિર Scheme_Object **(*dll_scheme_make_struct_કિંમતો)(
	Scheme_Object *struct_type, Scheme_Object **names, પૂર્ણાંક ગણતરી,
	પૂર્ણાંક flags);
સ્થિર Scheme_Type (*dll_scheme_make_type)(અચલ અક્ષર *name);
સ્થિર Scheme_Object *(*dll_scheme_make_vector)(પૂર્ણાંક size,
	Scheme_Object *fill);
સ્થિર શૂન્ય *(*dll_scheme_malloc_fail_ok)(શૂન્ય *(*f)(size_t), size_t);
સ્થિર Scheme_Object *(*dll_scheme_open_આગત_file)(અચલ અક્ષર *name,
	અચલ અક્ષર *who);
સ્થિર Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
સ્થિર પૂર્ણાંક (*dll_scheme_proper_list_length)(Scheme_Object *list);
સ્થિર શૂન્ય (*dll_scheme_raise)(Scheme_Object *exn);
સ્થિર Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
સ્થિર શૂન્ય (*dll_scheme_signal_error)(અચલ અક્ષર *સંદેશ, ...);
સ્થિર શૂન્ય (*dll_scheme_wrong_type)(અચલ અક્ષર *name, અચલ અક્ષર *expected,
	પૂર્ણાંક which, પૂર્ણાંક argc, Scheme_Object **argv);
# જો MZSCHEME_VERSION_MAJOR >= 299
સ્થિર શૂન્ય (*dll_scheme_set_param)(Scheme_Config *c, પૂર્ણાંક pos,
	Scheme_Object *o);
સ્થિર Scheme_Config *(*dll_scheme_current_config)(શૂન્ય);
સ્થિર Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
સ્થિર Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
સ્થિર શૂન્ય *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
સ્થિર Scheme_Hash_Table *(*dll_scheme_make_hash_table)(પૂર્ણાંક type);
સ્થિર શૂન્ય (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *ચાવી, Scheme_Object *કિંમત);
સ્થિર Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *ચાવી);
સ્થિર Scheme_Object *(*dll_scheme_make_double)(દ્વિગુણ d);
સ્થિર Scheme_Object *(*dll_scheme_make_sized_byte_string)(અક્ષર *chars,
	long len, પૂર્ણાંક copy);
સ્થિર Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
સ્થિર Scheme_Object *(*dll_scheme_dynamic_wind)(શૂન્ય (*pre)(શૂન્ય *), Scheme_Object *(* અસ્થિર act)(શૂન્ય *), શૂન્ય (* અસ્થિર post)(શૂન્ય *), Scheme_Object *(*jmp_handler)(શૂન્ય *), શૂન્ય * અસ્થિર data);
# ifdef MZ_PRECISE_GC
સ્થિર શૂન્ય *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
સ્થિર શૂન્ય (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, પૂર્ણાંક is_constant_size, પૂર્ણાંક is_atomic);
# endif
# જો MZSCHEME_VERSION_MAJOR >= 400
સ્થિર શૂન્ય (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
સ્થિર શૂન્ય **(*dll_scheme_malloc_immobile_box)(શૂન્ય *p);
સ્થિર શૂન્ય (*dll_scheme_free_immobile_box)(શૂન્ય **b);
# endif
# જો MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
સ્થિર પૂર્ણાંક (*dll_scheme_main_setup)(પૂર્ણાંક no_auto_statics, Scheme_Env_Main _main, પૂર્ણાંક argc, અક્ષર **argv);
#   જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
સ્થિર શૂન્ય (*dll_scheme_register_tls_space)(શૂન્ય *tls_space, પૂર્ણાંક _tls_index);
#   endif
#  endif
#  જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
સ્થિર Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(શૂન્ય);
#  endif
# endif
# જો MZSCHEME_VERSION_MAJOR >= 600
સ્થિર શૂન્ય (*dll_scheme_embedded_load)(intptr_t len, અચલ અક્ષર *s, પૂર્ણાંક predefined);
સ્થિર શૂન્ય (*dll_scheme_register_embedded_load)(intptr_t len, અચલ અક્ષર *s);
સ્થિર શૂન્ય (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# જો defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  જો !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_પ્રતીક dll_scheme_add_global_પ્રતીક
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_કિંમત dll_scheme_builtin_કિંમત
#  જો MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_આગત_port dll_scheme_close_આગત_port
#  define scheme_ગણતરી_લીટીઓ dll_scheme_ગણતરી_લીટીઓ
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
#  જો MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  નહીંતર
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  જો MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  નહીંતર
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_પ્રતીક dll_scheme_intern_પ્રતીક
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_કિંમત dll_scheme_make_integer_કિંમત
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  જો MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  નહીંતર
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_કિંમતો dll_scheme_make_struct_કિંમતો
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_આગત_file dll_scheme_open_આગત_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_ઢગલો_base dll_scheme_set_ઢગલો_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  જો MZSCHEME_VERSION_MAJOR >= 299
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
#  જો MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  જો MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   જો MZSCHEME_VERSION_MAJOR >= 500
#    જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(શૂન્ય)
{
    પાછા dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર    *name;
    શૂન્ય    **ptr;
} Thunk_Info;

સ્થિર Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (શૂન્ય **)&dll_GC_malloc},
    {"GC_malloc_atomic", (શૂન્ય **)&dll_GC_malloc_atomic},
    {ખાલી, ખાલી}};

સ્થિર Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (શૂન્ય **)&dll_scheme_eof},
    {"scheme_false", (શૂન્ય **)&dll_scheme_false},
    {"scheme_void", (શૂન્ય **)&dll_scheme_void},
    {"scheme_null", (શૂન્ય **)&dll_scheme_null},
    {"scheme_true", (શૂન્ય **)&dll_scheme_true},
# જો !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (શૂન્ય **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (શૂન્ય **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (શૂન્ય **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(શૂન્ય **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (શૂન્ય **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (શૂન્ય **)&dll_scheme_add_global_પ્રતીક},
    {"scheme_apply", (શૂન્ય **)&dll_scheme_apply},
    {"scheme_basic_env", (શૂન્ય **)&dll_scheme_basic_env},
# જો MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (શૂન્ય **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (શૂન્ય **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (શૂન્ય **)&dll_scheme_builtin_કિંમત},
    {"scheme_check_threads", (શૂન્ય **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (શૂન્ય **)&dll_scheme_close_આગત_port},
    {"scheme_count_lines", (શૂન્ય **)&dll_scheme_ગણતરી_લીટીઓ},
    {"scheme_current_continuation_marks",
	(શૂન્ય **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (શૂન્ય **)&dll_scheme_display},
    {"scheme_display_to_string", (શૂન્ય **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (શૂન્ય **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (શૂન્ય **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (શૂન્ય **)&dll_scheme_eq},
    {"scheme_eval", (શૂન્ય **)&dll_scheme_eval},
    {"scheme_eval_string", (શૂન્ય **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (શૂન્ય **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(શૂન્ય **)&dll_scheme_finish_primitive_module},
# જો MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (શૂન્ય **)&dll_scheme_format},
# નહીંતર
    {"scheme_format_utf8", (શૂન્ય **)&dll_scheme_format_utf8},
    {"scheme_get_param", (શૂન્ય **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (શૂન્ય **)&dll_scheme_gc_ptr_ok},
# જો MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(શૂન્ય **)&dll_scheme_get_sized_string_output},
# નહીંતર
    {"scheme_get_sized_byte_string_output",
	(શૂન્ય **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (શૂન્ય **)&dll_scheme_intern_પ્રતીક},
    {"scheme_lookup_global", (શૂન્ય **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(શૂન્ય **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (શૂન્ય **)&dll_scheme_make_integer_કિંમત},
    {"scheme_make_pair", (શૂન્ય **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (શૂન્ય **)&dll_scheme_make_prim_w_arity},
# જો MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (શૂન્ય **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(શૂન્ય **)&dll_scheme_make_string_output_port},
# નહીંતર
    {"scheme_make_byte_string", (શૂન્ય **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(શૂન્ય **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(શૂન્ય **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (શૂન્ય **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (શૂન્ય **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (શૂન્ય **)&dll_scheme_make_struct_કિંમતો},
    {"scheme_make_type", (શૂન્ય **)&dll_scheme_make_type},
    {"scheme_make_vector", (શૂન્ય **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (શૂન્ય **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (શૂન્ય **)&dll_scheme_open_આગત_file},
    {"scheme_primitive_module", (શૂન્ય **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (શૂન્ય **)&dll_scheme_proper_list_length},
    {"scheme_raise", (શૂન્ય **)&dll_scheme_raise},
    {"scheme_read", (શૂન્ય **)&dll_scheme_read},
    {"scheme_register_static", (શૂન્ય **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (શૂન્ય **)&dll_scheme_set_ઢગલો_base},
    {"scheme_signal_error", (શૂન્ય **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (શૂન્ય **)&dll_scheme_wrong_type},
# જો MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (શૂન્ય **)&dll_scheme_set_param},
    {"scheme_current_config", (શૂન્ય **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(શૂન્ય **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (શૂન્ય **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (શૂન્ય **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (શૂન્ય **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (શૂન્ય **)&dll_scheme_hash_set},
    {"scheme_hash_get", (શૂન્ય **)&dll_scheme_hash_get},
    {"scheme_make_double", (શૂન્ય **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (શૂન્ય **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (શૂન્ય **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (શૂન્ય **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (શૂન્ય **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (શૂન્ય **)&dll_GC_register_traversers},
# endif
# જો MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (શૂન્ય **)&dll_scheme_main_setup},
#   જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (શૂન્ય **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (શૂન્ય **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (શૂન્ય **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (શૂન્ય **)&dll_scheme_free_immobile_box},
# endif
# જો MZSCHEME_VERSION_MAJOR >= 500
#  જો defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (શૂન્ય **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# જો MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (શૂન્ય **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (શૂન્ય **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (શૂન્ય **)&dll_scheme_set_config_path},
# endif
    {ખાલી, ખાલી}};

સ્થિર HINSTANCE hMzGC = 0;
સ્થિર HINSTANCE hMzSch = 0;

સ્થિર શૂન્ય dynamic_mzscheme_end(શૂન્ય);
સ્થિર પૂર્ણાંક mzscheme_runtime_link_init(અક્ષર *sch_dll, અક્ષર *gc_dll,
	પૂર્ણાંક verbose);

    સ્થિર પૂર્ણાંક
mzscheme_runtime_link_init(અક્ષર *sch_dll, અક્ષર *gc_dll, પૂર્ણાંક verbose)
{
    Thunk_Info *thunk = ખાલી;

    જો (hMzGC && hMzSch)
	પાછા OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    જો (!hMzGC)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	પાછા FAIL;
    }

    જો (!hMzSch)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	પાછા FAIL;
    }

    માટે (thunk = mzsch_imports; thunk->name; thunk++)
    {
	જો ((*thunk->ptr =
		    (શૂન્ય *)GetProcAddress(hMzSch, thunk->name)) == ખાલી)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    પાછા FAIL;
	}
    }
    માટે (thunk = mzgc_imports; thunk->name; thunk++)
    {
	જો ((*thunk->ptr =
		    (શૂન્ય *)GetProcAddress(hMzGC, thunk->name)) == ખાલી)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

    પૂર્ણાંક
mzscheme_enabled(પૂર્ણાંક verbose)
{
    પાછા mzscheme_runtime_link_init(
	    (અક્ષર *)p_mzschemedll, (અક્ષર *)p_mzschemegcdll, verbose) == OK;
}

    સ્થિર શૂન્ય
dynamic_mzscheme_end(શૂન્ય)
{
    જો (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    જો (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#જો MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, અંક) GUARANTEE_STRING(proc, અંક)
#નહીંતર
    સ્થિર Scheme_Object *
guaranteed_byte_string_arg(અક્ષર *proc, પૂર્ણાંક અંક, પૂર્ણાંક argc, Scheme_Object **argv)
{
    જો (SCHEME_BYTE_STRINGP(argv[અંક]))
    {
	પાછા argv[અંક];
    }
    નહીંતર જો (SCHEME_CHAR_STRINGP(argv[અંક]))
    {
	Scheme_Object *tmp = ખાલી;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[અંક]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[અંક]);
	MZ_GC_UNREG();
	પાછા tmp;
    }
    નહીંતર
	scheme_wrong_type(proc, "string", અંક, argc, argv);
    // unreachable
    પાછા scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, અંક) guaranteed_byte_string_arg(proc, અંક, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#જો defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

સ્થિર Scheme_Type mz_buffer_type;
સ્થિર Scheme_Type mz_window_type;

સ્થિર પૂર્ણાંક initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
સ્થિર પૂર્ણાંક disabled = FALSE;
#endif
સ્થિર પૂર્ણાંક load_base_module_failed = FALSE;

// global environment
સ્થિર Scheme_Env    *environment = ખાલી;
// output/error handlers
સ્થિર Scheme_Object *curout = ખાલી;
સ્થિર Scheme_Object *curerr = ખાલી;
// exn:vim exception
સ્થિર Scheme_Object *exn_catching_apply = ખાલી;
સ્થિર Scheme_Object *exn_p = ખાલી;
સ્થિર Scheme_Object *exn_message = ખાલી;
સ્થિર Scheme_Object *vim_exn = ખાલી; // Vim Error exception

#જો !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
સ્થિર શૂન્ય *ઢગલો_base = ખાલી;
#endif

સ્થિર long range_start;
સ્થિર long range_end;

// MzScheme threads scheduling stuff
સ્થિર પૂર્ણાંક mz_threads_allow = 0;

#જો defined(FEAT_GUI_MSWIN)
સ્થિર શૂન્ય CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
સ્થિર UINT timer_id = 0;
#અથવાજો defined(FEAT_GUI_GTK)
સ્થિર gboolean timer_proc(gpointer);
સ્થિર guint timer_id = 0;
#અથવાજો defined(FEAT_GUI_MOTIF)
સ્થિર શૂન્ય timer_proc(XtPointer, XtIntervalId *);
સ્થિર XtIntervalId timer_id = (XtIntervalId)0;
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    શૂન્ય
mzvim_check_threads(શૂન્ય)
{
    // Last time MzScheme threads were scheduled
    સ્થિર time_t mz_last_time = 0;

    જો (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(ખાલી);

	જો ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#જો defined(MZSCHEME_GUI_THREADS)
સ્થિર શૂન્ય setup_timer(શૂન્ય);
સ્થિર શૂન્ય remove_timer(શૂન્ય);

// timers are presented in GUI only
# જો defined(FEAT_GUI_MSWIN)
    સ્થિર શૂન્ય CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_સંદેશ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    સ્થિર gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    સ્થિર શૂન્ય
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# જો defined(FEAT_GUI_GTK)
    પાછા TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    જો (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, ખાલી);
# endif
}

    સ્થિર શૂન્ય
setup_timer(શૂન્ય)
{
# જો defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(ખાલી, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, ખાલી);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, ખાલી);
# endif
}

    સ્થિર શૂન્ય
remove_timer(શૂન્ય)
{
# જો defined(FEAT_GUI_MSWIN)
    KillTimer(ખાલી, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    અક્ષર *
did_set_mzquantum(optset_T *args UNUSED)
{
#જો defined(MZSCHEME_GUI_THREADS)
    // reset timer
    જો (timer_id != 0)
	remove_timer();
    જો (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    પાછા ખાલી;
}

    સ્થિર શૂન્ય
notify_multithread(પૂર્ણાંક on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    જો (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    જો (!on && timer_id != 0)
	remove_timer();
#endif
}

    શૂન્ય
mzscheme_end(શૂન્ય)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#જો 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#જો HAVE_TLS_SPACE && !defined(VIMDLL)
# જો defined(_MSC_VER)
સ્થિર __declspec(thread) શૂન્ય *tls_space;
બાહ્ય intptr_t _tls_index;
# elif defined(__MINGW32__)
સ્થિર __thread શૂન્ય *tls_space;
બાહ્ય intptr_t _tls_index;
# નહીંતર
સ્થિર THREAD_LOCAL શૂન્ય *tls_space;
સ્થિર intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    પૂર્ણાંક
mzscheme_main(શૂન્ય)
{
    પૂર્ણાંક	    argc = 0;
    અક્ષર    *argv = ખાલી;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    જો (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	પાછા vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    શૂન્ય **ptls_space;
    intptr_t tls_index;
    શૂન્ય (*pget_tls_info)(શૂન્ય ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (શૂન્ય *)GetProcAddress(
				GetModuleHandle(ખાલી), "get_tls_info");
    જો (pget_tls_info == ખાલી)
    {
	disabled = TRUE;
	પાછા vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# નહીંતર
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    પાછા scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#નહીંતર
    પાછા mzscheme_env_main(ખાલી, argc, &argv);
#endif
}

    સ્થિર પૂર્ણાંક
mzscheme_env_main(Scheme_Env *env, પૂર્ણાંક argc UNUSED, અક્ષર **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#નહીંતર
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = ખાલી;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    ઢગલો_base = &__gc_var_stack__;
# નહીંતર
    પૂર્ણાંક dummy = 0;
    ઢગલો_base = (શૂન્ય *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    પાછા 0;
}

    સ્થિર Scheme_Object*
load_base_module(શૂન્ય *data)
{
    scheme_namespace_require(scheme_intern_પ્રતીક((અક્ષર *)data));
    પાછા scheme_null;
}

    સ્થિર Scheme_Object *
load_base_module_on_error(શૂન્ય *data UNUSED)
{
    load_base_module_failed = TRUE;
    પાછા scheme_null;
}

    સ્થિર પૂર્ણાંક
startup_mzscheme(શૂન્ય)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_ઢગલો_base(ઢગલો_base, 1);
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
#જો MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = ખાલી;
	પૂર્ણાંક mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	જો (s != ખાલી)
	{
	    coll_path = scheme_make_path((અક્ષર *)s);
	    MZ_GC_CHECK();
	    જો (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	જો (coll_path == ખાલી)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	જો (coll_path != ખાલી)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#નહીંતર
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = ખાલી;
	Scheme_Object	*coll_pair = ખાલી;
	Scheme_Config	*config = ખાલી;

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

#જો MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = ખાલી;
	પૂર્ણાંક mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	જો (s != ખાલી)
	{
	    config_path = scheme_make_path((અક્ષર *)s);
	    MZ_GC_CHECK();
	    જો (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	જો (config_path == ખાલી)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	જો (config_path != ખાલી)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#જો MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(ખાલી, load_base_module, ખાલી,
				    load_base_module_on_error, "racket/base");
	જો (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(ખાલી, load_base_module, ખાલી,
				    load_base_module_on_error, "scheme/base");
	    જો (load_base_module_failed)
		પાછા -1;
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
	Scheme_Object	*make_security_guard = ખાલી;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# જો MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_પ્રતીક = ખાલી;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_પ્રતીક);
	    MZ_GC_REG();
	    make_security_guard_પ્રતીક = scheme_intern_પ્રતીક("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_પ્રતીક, environment);
	    MZ_GC_UNREG();
	}
# નહીંતર
	make_security_guard = scheme_builtin_કિંમત("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	જો (make_security_guard != ખાલી)
	{
	    Scheme_Object   *args[3] = {ખાલી, ખાલી, ખાલી};
	    Scheme_Object   *guard = ખાલી;
	    Scheme_Config   *config = ખાલી;
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

    પાછા 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    સ્થિર પૂર્ણાંક
mzscheme_init(શૂન્ય)
{
    જો (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	જો (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    પાછા -1;
	}
#endif
	જો (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    પાછા -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = ખાલી;
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

    પાછા 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    સ્થિર પૂર્ણાંક
eval_with_exn_handling(
    શૂન્ય		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *કિંમત = ખાલી;
    Scheme_Object   *exn = ખાલી;
    Scheme_Object   *prim = ખાલી;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, કિંમત);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    કિંમત = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    જો (!કિંમત)
    {
	કિંમત = extract_exn_message(exn);
	// Got an exn?
	જો (કિંમત)
	{
	    scheme_display(કિંમત, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	પાછા FAIL;
    }

    જો (ret != ખાલી)	// if pointer to retval supported give it up
	*ret = કિંમત;
    // Print any result, as long as it's not a void
    નહીંતર જો (!SCHEME_VOIDP(કિંમત))
    {
	scheme_display(કિંમત, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    પાછા OK;
}

/*
 * :mzscheme
 */
    સ્થિર પૂર્ણાંક
do_mzscheme_command(exarg_T *eap, શૂન્ય *data, Scheme_Closed_Prim *what)
{
    જો (mzscheme_init())
	પાછા FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    પાછા eval_with_exn_handling(data, what, ખાલી);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    શૂન્ય
mzscheme_buffer_free(buf_T *buf)
{
    જો (buf->b_mzscheme_ref == ખાલી)
	પાછા;

    vim_mz_buffer *bp = ખાલી;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_કિંમત;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#નહીંતર
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = ખાલી;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    શૂન્ય
mzscheme_window_free(win_T *win)
{
    જો (win->w_mzscheme_ref == ખાલી)
	પાછા;

    vim_mz_window *wp = ખાલી;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_કિંમત;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#નહીંતર
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = ખાલી;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    શૂન્ય
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    જો (eap->skip)
	પાછા;

    જો (script == ખાલી)
	do_mzscheme_command(eap, eap->arg, do_eval);
    નહીંતર
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    સ્થિર Scheme_Object *
do_load(શૂન્ય *data, પૂર્ણાંક noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *પદાવલિ = ખાલી;
    Scheme_Object   *result = ખાલી;
    અક્ષર	    *file = ખાલી;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, પદાવલિ);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (અક્ષર *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_આગત_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_ગણતરી_લીટીઓ(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    જ્યારે (!SCHEME_EOFP(પદાવલિ = scheme_read(pinfo->port)))
    {
	result = scheme_eval(પદાવલિ, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_આગત_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = ખાલી;
    MZ_GC_UNREG();
    પાછા result;
}

/*
 * :mzfile
 */
    શૂન્ય
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {ખાલી, ખાલી};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (અક્ષર *)eap->arg;
    જો (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != ખાલી)	// looks like port was not closed
    {
	scheme_close_આગત_port(pinfo.port);
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
    સ્થિર શૂન્ય
init_exn_catching_apply(શૂન્ય)
{
    જો (exn_catching_apply)
	પાછા;

    અક્ષર *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_કિંમત("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_કિંમત("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    સ્થિર Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    જો (SCHEME_TRUEP(SCHEME_CAR(v)))
	પાછા SCHEME_CDR(v);
    નહીંતર
    {
	*exn = SCHEME_CDR(v);
	પાછા ખાલી;
    }
}

    સ્થિર Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    જો (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	પાછા _scheme_apply(exn_message, 1, &v);
    નહીંતર
	પાછા ખાલી; // Not an exn structure
}

    સ્થિર Scheme_Object *
do_eval(શૂન્ય *s, પૂર્ણાંક noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    પાછા scheme_eval_string_all((અક્ષર *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    સ્થિર શૂન્ય
do_intrnl_output(અક્ષર *mesg, પૂર્ણાંક error)
{
    અક્ષર *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    જ્યારે (p)
    {
	*p = '\0';
	જો (error)
	    emsg(prev);
	નહીંતર
	    સંદેશ(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    જો (error)
	emsg(prev);
    નહીંતર
	સંદેશ(prev);
}

    સ્થિર શૂન્ય
do_output(અક્ષર *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    સ્થિર શૂન્ય
do_err_output(અક્ષર *mesg)
{
    do_intrnl_output(mesg, 1);
}

    સ્થિર શૂન્ય
do_printf(અક્ષર *format, ...)
{
    do_intrnl_output(format, 1);
}

    સ્થિર શૂન્ય
do_flush(શૂન્ય)
{
    અક્ષર *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    જો (length)
    {
	do_err_output(buff);
	પાછા;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    જો (length)
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
    સ્થિર Scheme_Object *
vim_command(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ખાલી;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_કિંમત(cmd), ખાલી, ખાલી, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    પાછા scheme_void;
}

/*
 * (eval {expr-string})
 */
    સ્થિર Scheme_Object *
vim_eval(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = ખાલી;
    typval_T		*vim_result;
    Scheme_Object *પદાવલિ = ખાલી;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, પદાવલિ);
    MZ_GC_REG();
    પદાવલિ = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_પદાવલિ(BYTE_STRING_કિંમત(પદાવલિ), ખાલી);

    જો (vim_result == ખાલી)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    પાછા result;
#નહીંતર
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    પાછા scheme_false;
#endif
}

/*
 * (range-start)
 */
    સ્થિર Scheme_Object *
get_range_start(
    શૂન્ય		*data UNUSED,
    પૂર્ણાંક			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    પાછા scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    સ્થિર Scheme_Object *
get_range_end(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
    પાછા scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    સ્થિર Scheme_Object *
mzscheme_beep(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    પાછા scheme_void;
}

સ્થિર Scheme_Object *M_global = ખાલી;

/*
 * (get-option {option-name}) [buffer/window]
 */
    સ્થિર Scheme_Object *
get_option(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    કિંમત;
    અક્ષર	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = ખાલી;
    Scheme_Object   *name = ખાલી;
    પૂર્ણાંક		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    જો (argc > 1)
    {
	જો (M_global == ખાલી)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_પ્રતીક("global");
	    MZ_GC_CHECK();
	}

	જો (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	નહીંતર જો (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	નહીંતર જો (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	નહીંતર
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_કિંમત(BYTE_STRING_કિંમત(name), &કિંમત, (char_u **)&strval,
								  ખાલી, scope);
    curbuf = save_curb;
    curwin = save_curw;

    પસંદગી (rc)
    {
    સ્થિતિ gov_bool:
    સ્થિતિ gov_સંખ્યા:
	MZ_GC_UNREG();
	પાછા scheme_make_integer_કિંમત(કિંમત);
    સ્થિતિ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	પાછા rval;
    સ્થિતિ gov_hidden_bool:
    સ્થિતિ gov_hidden_સંખ્યા:
    સ્થિતિ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    સ્થિતિ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    પાછા scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    સ્થિર Scheme_Object *
set_option(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    char_u	*command = ખાલી;
    પૂર્ણાંક		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = ખાલી;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    જો (argc > 1)
    {
	જો (M_global == ખાલી)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_પ્રતીક("global");
	    MZ_GC_CHECK();
	}

	જો (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	નહીંતર જો (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	નહીંતર જો (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	નહીંતર
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_કિંમત(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    પાછા scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    સ્થિર Scheme_Object *
get_curr_win(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
    પાછા (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    સ્થિર Scheme_Object *
get_window_ગણતરી(
    શૂન્ય		*data UNUSED,
    પૂર્ણાંક			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    પૂર્ણાંક	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    પાછા scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    સ્થિર Scheme_Object *
get_window_list(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    માટે ( ; w != ખાલી; w = w->w_next)
	જો (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    પાછા list;
}

    સ્થિર Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = ખાલી;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    જો (win->w_mzscheme_ref != ખાલી)
	પાછા (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, કદ(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(ખાલી);
#નહીંતર
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    પાછા (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    સ્થિર Scheme_Object *
get_window_અંક(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
    પૂર્ણાંક		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    માટે (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    પાછા scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    સ્થિર Scheme_Object *
get_window_by_અંક(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    પૂર્ણાંક		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    જો (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    માટે ( ; win != ખાલી; win = win->w_next, --fnum)
	જો (fnum == 1)	    // to be 1-based
	    પાછા window_new(win);

    પાછા scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    સ્થિર Scheme_Object *
get_window_buffer(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    પાછા buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    સ્થિર Scheme_Object *
get_window_height(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    પાછા scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    સ્થિર Scheme_Object *
set_window_height(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    પૂર્ણાંક		    height;

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
    પાછા scheme_void;
}

/*
 * (get-win-width [window])
 */
    સ્થિર Scheme_Object *
get_window_width(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    પાછા scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    સ્થિર Scheme_Object *
set_window_width(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    પૂર્ણાંક		    width = 0;

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
    પાછા scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    સ્થિર Scheme_Object *
get_cursor(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    પાછા scheme_make_pair(scheme_make_integer_કિંમત((long)pos.lnum),
		    scheme_make_integer_કિંમત((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    સ્થિર Scheme_Object *
set_cursor(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
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

    જો (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_લીટી_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    પાછા scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    સ્થિર Scheme_Object *
mzscheme_open_buffer(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    પૂર્ણાંક		    અંક = 0;
    Scheme_Object   *onum = ખાલી;
    Scheme_Object   *buf = ખાલી;
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
    અંક = buflist_add(BYTE_STRING_કિંમત(fname), BLN_LISTED | BLN_CURBUF);

    જો (અંક == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(અંક);
    buf = get_buffer_by_અંક(data, 1, &onum);
    MZ_GC_UNREG();
    પાછા buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    સ્થિર Scheme_Object *
get_buffer_by_અંક(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    પૂર્ણાંક		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	જો (buf->b_fnum == fnum)
	    પાછા buffer_new(buf);

    પાછા scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    સ્થિર Scheme_Object *
get_buffer_by_name(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = ખાલી;
    Scheme_Object *fname = ખાલી;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	જો (buf->b_ffname == ખાલી || buf->b_sfname == ખાલી)
	    // empty string
	{
	    જો (BYTE_STRING_કિંમત(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	નહીંતર જો (!fnamecmp(buf->b_ffname, BYTE_STRING_કિંમત(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_કિંમત(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    પાછા buffer;
}

/*
 * (get-next-buff [buffer])
 */
    સ્થિર Scheme_Object *
get_next_buffer(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    જો (buf->b_next == ખાલી)
	પાછા scheme_false;
    નહીંતર
	પાછા buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    સ્થિર Scheme_Object *
get_prev_buffer(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    જો (buf->b_prev == ખાલી)
	પાછા scheme_false;
    નહીંતર
	પાછા buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    સ્થિર Scheme_Object *
get_buffer_અંક(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    પાછા scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    સ્થિર Scheme_Object *
get_buffer_ગણતરી(
    શૂન્ય		*data UNUSED,
    પૂર્ણાંક			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    પૂર્ણાંક	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    પાછા scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    સ્થિર Scheme_Object *
get_buffer_name(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    પાછા scheme_make_byte_string((અક્ષર *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    સ્થિર Scheme_Object *
get_curr_buffer(
    શૂન્ય		*data UNUSED,
    પૂર્ણાંક			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    પાછા (Scheme_Object *)get_vim_curr_buffer();
}

    સ્થિર Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = ખાલી;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    જો (buf->b_mzscheme_ref)
	પાછા (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, કદ(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(ખાલી);
#નહીંતર
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    પાછા (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    સ્થિર Scheme_Object *
get_buffer_size(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    પાછા scheme_make_integer(buf->buf->b_ml.ml_લીટી_ગણતરી);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    સ્થિર Scheme_Object *
get_buffer_લીટી(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    પૂર્ણાંક		    linenr;
    char_u	    *લીટી;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    લીટી = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    પાછા scheme_make_byte_string((અક્ષર *)લીટી);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    સ્થિર Scheme_Object *
get_buffer_લીટી_list(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    પૂર્ણાંક		    i, hi, lo, n;
    Scheme_Object   *list = ખાલી;

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
    જો (lo < 0)
	lo = 0;

    જો (hi < 0)
	hi = 0;
    જો (hi < lo)
	hi = lo;

    n = hi - lo;

    માટે (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (અક્ષર *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    પાછા list;
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
    સ્થિર Scheme_Object *
set_buffer_લીટી(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *લીટી = ખાલી;
    અક્ષર	    *save;
    પૂર્ણાંક		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, લીટી);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    જો (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    લીટી = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_લીટી_range(n, buf->buf);

    જો (SCHEME_FALSEP(લીટી))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	જો (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	નહીંતર જો (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	જો (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_લીટીઓ_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	પાછા scheme_void;
    }
    નહીંતર
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_લીટી(લીટી);

	curbuf = buf->buf;

	જો (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	નહીંતર જો (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	નહીંતર
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	જો (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	પાછા scheme_void;
    }
}

    સ્થિર શૂન્ય
free_array(અક્ષર **array)
{
    અક્ષર **curr = array;
    જ્યારે (*curr != ખાલી)
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
    સ્થિર Scheme_Object *
set_buffer_લીટી_list(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ખાલી;
    Scheme_Object   *લીટી_list = ખાલી;
    પૂર્ણાંક		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, લીટી_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    જો (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    લીટી_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    જો (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_લીટી_range(lo, buf->buf);	    // inclusive
    check_લીટી_range(hi - 1, buf->buf);	    // exclusive

    જો (SCHEME_FALSEP(લીટી_list) || SCHEME_NULLP(લીટી_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	જો (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	નહીંતર
	{
	    માટે (i = 0; i < old_len; i++)
		જો (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    જો (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_લીટીઓ_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	પાછા scheme_void;
    }
    નહીંતર
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(લીટી_list);
	MZ_GC_CHECK();
	જો (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	નહીંતર
	{
	    અક્ષર		**array = ખાલી;
	    Scheme_Object   *લીટી = ખાલી;
	    Scheme_Object   *rest = ખાલી;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, લીટી);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(અક્ષર *, new_len + 1);

	    rest = લીટી_list;
	    માટે (i = 0; i < new_len; ++i)
	    {
		લીટી = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		જો (!SCHEME_STRINGP(લીટી))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_લીટી(લીટી);
	    }

	    curbuf = buf->buf;

	    જો (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    માટે (i = 0; i < old_len - new_len; ++i)
	    {
		જો (ml_delete((linenr_T)lo) == FAIL)
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
	    માટે (i = 0; i < old_len && i < new_len; i++)
		જો (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    જ્યારે (i < new_len)
	    {
		જો (ml_append((linenr_T)(lo + i - 1),
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
	changed_લીટીઓ((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	જો (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	પાછા scheme_void;
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
    સ્થિર Scheme_Object *
ઉમેરો_buffer_લીટી_list(શૂન્ય *data, પૂર્ણાંક argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = ખાલી;
    Scheme_Object   *list = ખાલી;
    અક્ષર	    *str = ખાલી;
    પૂર્ણાંક		    i, n, size;

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

    જો (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    જો (n != 0)	    // 0 can be used in insert
	check_લીટી_range(n, buf->buf);
    જો (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_લીટી(list);
	curbuf = buf->buf;

	જો (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	નહીંતર જો (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	નહીંતર
	{
	    vim_free(str);
	    appended_લીટીઓ_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	પાછા scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    જો (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    નહીંતર
    {
	Scheme_Object   *લીટી = ખાલી;
	Scheme_Object   *rest = ખાલી;
	અક્ષર		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, લીટી);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(અક્ષર *, size + 1);

	rest = list;
	માટે (i = 0; i < size; ++i)
	{
	    લીટી = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_લીટી(લીટી);
	}

	curbuf = buf->buf;

	જો (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	નહીંતર
	{
	    માટે (i = 0; i < size; ++i)
		જો (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    જો (i > 0)
		appended_લીટીઓ_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    પાછા scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    સ્થિર Scheme_Object *
vim_bufferp(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv)
{
    જો (SCHEME_VIMBUFFERP(argv[0]))
	પાછા scheme_true;
    નહીંતર
	પાછા scheme_false;
}

/*
 * (win? obj)
 */
    સ્થિર Scheme_Object *
vim_windowp(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv)
{
    જો (SCHEME_VIMWINDOWP(argv[0]))
	પાછા scheme_true;
    નહીંતર
	પાછા scheme_false;
}

/*
 * (buff-valid? obj)
 */
    સ્થિર Scheme_Object *
vim_buffer_validp(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv)
{
    જો (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_કિંમત)
	પાછા scheme_true;
    નહીંતર
	પાછા scheme_false;
}

/*
 * (win-valid? obj)
 */
    સ્થિર Scheme_Object *
vim_window_validp(શૂન્ય *data UNUSED, પૂર્ણાંક argc UNUSED, Scheme_Object **argv)
{
    જો (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_કિંમત)
	પાછા scheme_true;
    નહીંતર
	પાછા scheme_false;
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
    સ્થિર અક્ષર *
string_to_લીટી(Scheme_Object *obj)
{
    અક્ષર	*scheme_str = ખાલી;
    અક્ષર	*vim_str = ખાલી;
    OUTPUT_LEN_TYPE	len;
    પૂર્ણાંક		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    જો (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    માટે (i = 0; i < len; ++i)
    {
	જો (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	નહીંતર
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    પાછા vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    સ્થિર Scheme_Object *
vim_to_mzscheme(typval_T *vim_કિંમત)
{
    Scheme_Object	*result = ખાલી;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = ખાલી;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_કિંમત, 1, visited);

    MZ_GC_UNREG();
    પાછા result;
}

    સ્થિર Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_કિંમત,
    પૂર્ણાંક			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = ખાલી;
    પૂર્ણાંક		    new_કિંમત_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    જો (depth > 100)
    {
	MZ_GC_UNREG();
	પાછા scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_કિંમત);
    MZ_GC_CHECK();
    જો (result != ખાલી) // found, do nothing
	new_કિંમત_2 = FALSE;
    નહીંતર જો (vim_કિંમત->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((અક્ષર *)vim_કિંમત->vval.v_string);
	MZ_GC_CHECK();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_સંખ્યા)
    {
	result = scheme_make_integer((long)vim_કિંમત->vval.v_સંખ્યા);
	MZ_GC_CHECK();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((દ્વિગુણ)vim_કિંમત->vval.v_float);
	MZ_GC_CHECK();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_LIST)
    {
	list_T		*list = vim_કિંમત->vval.v_list;
	listitem_T	*curr;

	જો (list == ખાલી || list->lv_first == ખાલી)
	    result = scheme_null;
	નહીંતર
	{
	    Scheme_Object   *obj = ખાલી;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    જ્યારે (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_DICT)
    {
	Scheme_Object	  *ચાવી = ખાલી;
	Scheme_Object	  *obj = ખાલી;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ચાવી);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	જો (vim_કિંમત->vval.v_dict != ખાલી)
	{
	    hashtab_T	*ht = &vim_કિંમત->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
	    {
		જો (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    ચાવી = scheme_make_byte_string((અક્ષર *)hi->hi_ચાવી);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, ચાવી, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = ખાલી;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((અક્ષર *)vim_કિંમત->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(અચલ અક્ષર *)BYTE_STRING_કિંમત(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_PARTIAL)
    {
	જો (vim_કિંમત->vval.v_partial == ખાલી)
	    result = scheme_null;
	નહીંતર
	{
	    Scheme_Object *funcname = ખાલી;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (અક્ષર *)partial_name(vim_કિંમત->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (અચલ અક્ષર *)BYTE_STRING_કિંમત(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    નહીંતર જો (vim_કિંમત->v_type == VAR_BOOL || vim_કિંમત->v_type == VAR_SPECIAL)
    {
	જો (vim_કિંમત->vval.v_સંખ્યા <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_કિંમત->vval.v_સંખ્યા);
	નહીંતર
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    નહીંતર
    {
	result = scheme_void;
	new_કિંમત_2 = FALSE;
    }
    જો (new_કિંમત_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_કિંમત, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    પાછા result;
}

    સ્થિર પૂર્ણાંક
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    પૂર્ણાંક i, status;
    Scheme_Hash_Table	*visited = ખાલી;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    માટે (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	જો (visited->vals[i] != ખાલી)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    પાછા status;
}
    સ્થિર પૂર્ણાંક
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, પૂર્ણાંક depth,
	Scheme_Hash_Table *visited)
{
    પૂર્ણાંક		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    જો (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_સંખ્યા;
	tv->vval.v_સંખ્યા = 0;
	પાછા FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    જો (found != ખાલી)
	copy_tv(found, tv);
    નહીંતર જો (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_સંખ્યા = VVAL_NULL;
    }
    નહીંતર જો (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_સંખ્યા;
	tv->vval.v_સંખ્યા = SCHEME_INT_VAL(obj);
    }
    નહીંતર જો (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_સંખ્યા = SCHEME_TRUEP(obj);
    }
    નહીંતર જો (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    નહીંતર જો (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_કિંમત(obj));
    }
# જો MZSCHEME_VERSION_MAJOR >= 299
    નહીંતર જો (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = ખાલી;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_કિંમત(tmp));
	MZ_GC_UNREG();
    }
# endif
    નહીંતર જો (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	જો (list == ખાલી)
	    status = FAIL;
	નહીંતર
	{
	    પૂર્ણાંક		    i;
	    Scheme_Object   *curr = ખાલી;
	    Scheme_Object   *cval = ખાલી;
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
	    જો (v == ખાલી)
		status = FAIL;
	    નહીંતર
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		જો (SCHEME_VECTORP(obj))
		{
		    માટે (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			જો (status == FAIL)
			    તોડો;
			status = list_append_tv(list, v);
			clear_tv(v);
			જો (status == FAIL)
			    તોડો;
		    }
		}
		નહીંતર જો (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    માટે (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			જો (status == FAIL)
			    તોડો;
			status = list_append_tv(list, v);
			clear_tv(v);
			જો (status == FAIL)
			    તોડો;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    જો (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			જો (status == OK)
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
    નહીંતર જો (SCHEME_HASHTP(obj))
    {
	પૂર્ણાંક		i;
	dict_T		*dict;
	Scheme_Object   *ચાવી = ખાલી;
	Scheme_Object   *val = ખાલી;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, ચાવી);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	જો (dict == ખાલી)
	    status = FAIL;
	નહીંતર
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    માટે (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		જો (((Scheme_Hash_Table *) obj)->vals[i] != ખાલી)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_લીટી(
				((Scheme_Hash_Table *) obj)->ચાવીઓ[i]));
		    // convert Scheme val to Vim and add it to the dict
		    જો (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			તોડો;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    નહીંતર
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_લીટી(obj);
    }
    MZ_GC_UNREG();
    પાછા status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    સ્થિર Scheme_Object *
vim_funcref(શૂન્ય *name, પૂર્ણાંક argc, Scheme_Object **argv)
{
    પૂર્ણાંક i;
    typval_T args;
    પૂર્ણાંક status = OK;
    Scheme_Object *result = ખાલી;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    જો (list == ખાલી)
	status = FAIL;
    નહીંતર
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	માટે (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    જો (v == ખાલી)
		status = FAIL;
	    નહીંતર
	    {
		status = mzscheme_to_vim(argv[i], v);
		જો (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	જો (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_કિંમત((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    જો (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    નહીંતર
	raise_if_error();
    પાછા result;
}

    શૂન્ય
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = ખાલી;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    જો (mzscheme_init())
    {
	MZ_GC_UNREG();
	પાછા;
    }

    MZ_GC_CHECK();
    જો (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    સ્થિર પૂર્ણાંક
vim_error_check(શૂન્ય)
{
    પાછા (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    સ્થિર શૂન્ય
register_vim_exn(શૂન્ય)
{
    પૂર્ણાંક	nc = 0;
    પૂર્ણાંક i;
    Scheme_Object   *struct_exn = ખાલી;
    Scheme_Object   *exn_name = ખાલી;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_પ્રતીક("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_કિંમત("struct:exn");
    MZ_GC_CHECK();

    જો (vim_exn == ખાલી)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, ખાલી, 0, 0, ખાલી, ખાલી
#જો MZSCHEME_VERSION_MAJOR >= 299
		, ખાલી
#endif
		);


    {
	Scheme_Object   **tmp = ખાલી;
	Scheme_Object   *exn_names[5] = {ખાલી, ખાલી, ખાલી, ખાલી, ખાલી};
	Scheme_Object   *exn_કિંમતો[5] = {ખાલી, ખાલી, ખાલી, ખાલી, ખાલી};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_કિંમતો, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * કદ(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_કિંમતો(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_કિંમતો, tmp, nc * કદ(Scheme_Object *));
	MZ_GC_CHECK();

	માટે (i = 0; i < nc; i++)
	{
	    scheme_add_global_પ્રતીક(exn_names[i],
		    exn_કિંમતો[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    શૂન્ય
raise_vim_exn(અચલ અક્ષર *add_info)
{
    અક્ષર	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {ખાલી, ખાલી};
    Scheme_Object   *exn = ખાલી;
    Scheme_Object   *byte_string = ખાલી;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    જો (add_info != ખાલી)
    {
	અક્ષર		*c_string = ખાલી;
	Scheme_Object   *info = ખાલી;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (પૂર્ણાંક)STRLEN(fmt), 1, &info, ખાલી);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    નહીંતર
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#જો MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#નહીંતર
    argv[1] = scheme_current_continuation_marks(ખાલી);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    શૂન્ય
raise_if_error(શૂન્ય)
{
    જો (vim_error_check())
	raise_vim_exn(ખાલી);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    સ્થિર vim_mz_buffer *
get_buffer_arg(અચલ અક્ષર *fname, પૂર્ણાંક argnum, પૂર્ણાંક argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    જો (argc < argnum + 1)
	પાછા get_vim_curr_buffer();
    જો (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (શૂન્ય)get_valid_buffer(argv[argnum]);
    પાછા b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    સ્થિર vim_mz_window *
get_window_arg(અચલ અક્ષર *fname, પૂર્ણાંક argnum, પૂર્ણાંક argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    જો (argc < argnum + 1)
	પાછા get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    જો (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (શૂન્ય)get_valid_window(argv[argnum]);
    પાછા w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(શૂન્ય *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    જો (buf == INVALID_BUFFER_કિંમત)
	scheme_signal_error(_("buffer is invalid"));
    પાછા buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(શૂન્ય *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    જો (win == INVALID_WINDOW_કિંમત)
	scheme_signal_error(_("window is invalid"));
    પાછા win;
}

    પૂર્ણાંક
mzthreads_allowed(શૂન્ય)
{
    પાછા mz_threads_allow;
}

    સ્થિર પૂર્ણાંક
લીટી_in_range(linenr_T lnum, buf_T *buf)
{
    પાછા (lnum > 0 && lnum <= buf->b_ml.ml_લીટી_ગણતરી);
}

    સ્થિર શૂન્ય
check_લીટી_range(linenr_T lnum, buf_T *buf)
{
    જો (!લીટી_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    સ્થિર શૂન્ય
mz_fix_cursor(પૂર્ણાંક lo, પૂર્ણાંક hi, પૂર્ણાંક extra)
{
    જો (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	જો (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	નહીંતર જો (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	નહીંતર
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

સ્થિર Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_લીટી, "get-buff-line", 1, 2},
    {set_buffer_લીટી, "set-buff-line", 2, 3},
    {get_buffer_લીટી_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_અંક, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_લીટી_list, "set-buff-line-list", 3, 4},
    {ઉમેરો_buffer_લીટી_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_ગણતરી, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_અંક, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_ગણતરી, "win-count", 0, 0},
    {get_window_by_અંક, "get-win-by-num", 1, 1},
    {get_window_અંક, "get-win-num", 0, 1},
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
    સ્થિર vim_mz_buffer *
get_vim_curr_buffer(શૂન્ય)
{
    જો (curbuf->b_mzscheme_ref == ખાલી)
	પાછા (vim_mz_buffer *)buffer_new(curbuf);
    નહીંતર
	પાછા BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    સ્થિર vim_mz_window *
get_vim_curr_window(શૂન્ય)
{
    જો (curwin->w_mzscheme_ref == ખાલી)
	પાછા (vim_mz_window *)window_new(curwin);
    નહીંતર
	પાછા WINDOW_REF(curwin);
}

    સ્થિર શૂન્ય
make_modules(શૂન્ય)
{
    પૂર્ણાંક		    i;
    Scheme_Env	    *mod = ખાલી;
    Scheme_Object   *vimext_પ્રતીક = ખાલી;
    Scheme_Object   *closed_prim = ખાલી;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_પ્રતીક);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_પ્રતીક = scheme_intern_પ્રતીક("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_પ્રતીક, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(prims); i++)
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
સ્થિર Scheme_Object *M_write = ખાલી;
સ્થિર Scheme_Object *M_read = ખાલી;
સ્થિર Scheme_Object *M_execute = ખાલી;
સ્થિર Scheme_Object *M_delete = ખાલી;

    સ્થિર શૂન્ય
sandbox_check(શૂન્ય)
{
    જો (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    સ્થિર Scheme_Object *
sandbox_file_guard(પૂર્ણાંક argc UNUSED, Scheme_Object **argv)
{
    જો (!sandbox)
	પાછા scheme_void;

    Scheme_Object *requested_access = argv[2];

    જો (M_write == ખાલી)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_પ્રતીક("write");
	MZ_GC_CHECK();
    }
    જો (M_read == ખાલી)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_પ્રતીક("read");
	MZ_GC_CHECK();
    }
    જો (M_execute == ખાલી)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_પ્રતીક("execute");
	MZ_GC_CHECK();
    }
    જો (M_delete == ખાલી)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_પ્રતીક("delete");
	MZ_GC_CHECK();
    }

    જ્યારે (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	જો (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    પાછા scheme_void;
}

    સ્થિર Scheme_Object *
sandbox_network_guard(પૂર્ણાંક argc UNUSED, Scheme_Object **argv UNUSED)
{
    પાછા scheme_void;
}
#endif

#endif
