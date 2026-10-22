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

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#যদি defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#যদি MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#যদি defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

প্রকারনির্দেশ সংরচনা
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_মান ((buf_T *)(-1))

প্রকারনির্দেশ সংরচনা
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_মান ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
প্রকারনির্দেশ সংরচনা
{
    Scheme_Closed_Prim	*prim;
    বর্ণ	*name;
    পূর্ণাঙ্ক		mina;	// arity information
    পূর্ণাঙ্ক		maxa;
} Vim_Prim;

প্রকারনির্দেশ সংরচনা
{
    বর্ণ	    *name;
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
স্থির Scheme_Object *sandbox_file_guard(পূর্ণাঙ্ক, Scheme_Object **);
স্থির Scheme_Object *sandbox_network_guard(পূর্ণাঙ্ক, Scheme_Object **);
স্থির শূন্য sandbox_check(শূন্য);
#endif
//  Buffer-related commands
স্থির Scheme_Object *buffer_new(buf_T *buf);
স্থির Scheme_Object *get_buffer_by_অঙ্ক(শূন্য *, পূর্ণাঙ্ক, Scheme_Object **);
স্থির vim_mz_buffer *get_vim_curr_buffer(শূন্য);

//  Window-related commands
স্থির Scheme_Object *window_new(win_T *win);
স্থির vim_mz_window *get_vim_curr_window(শূন্য);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
স্থির পূর্ণাঙ্ক vim_error_check(শূন্য);
স্থির পূর্ণাঙ্ক do_mzscheme_command(exarg_T *, শূন্য *, Scheme_Closed_Prim *what);
স্থির পূর্ণাঙ্ক startup_mzscheme(শূন্য);
স্থির বর্ণ *string_to_সারি(Scheme_Object *obj);
#যদি MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#অন্যথা
# define OUTPUT_LEN_TYPE long
#endif
স্থির শূন্য do_output(বর্ণ *mesg, OUTPUT_LEN_TYPE len);
স্থির শূন্য do_printf(বর্ণ *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
স্থির শূন্য do_flush(শূন্য);
স্থির Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
স্থির Scheme_Object *extract_exn_message(Scheme_Object *v);
স্থির Scheme_Object *do_eval(শূন্য *, পূর্ণাঙ্ক noargc, Scheme_Object **noargv);
স্থির Scheme_Object *do_load(শূন্য *, পূর্ণাঙ্ক noargc, Scheme_Object **noargv);
স্থির শূন্য register_vim_exn(শূন্য);
স্থির vim_mz_buffer *get_buffer_arg(ধ্রুব বর্ণ *fname, পূর্ণাঙ্ক argnum,
	পূর্ণাঙ্ক argc, Scheme_Object **argv);
স্থির vim_mz_window *get_window_arg(ধ্রুব বর্ণ *fname, পূর্ণাঙ্ক argnum,
	পূর্ণাঙ্ক argc, Scheme_Object **argv);
স্থির পূর্ণাঙ্ক সারি_in_range(linenr_T, buf_T *);
স্থির শূন্য check_সারি_range(linenr_T, buf_T *);
স্থির শূন্য mz_fix_cursor(পূর্ণাঙ্ক lo, পূর্ণাঙ্ক hi, পূর্ণাঙ্ক extra);

স্থির পূর্ণাঙ্ক eval_with_exn_handling(শূন্য *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
স্থির শূন্য make_modules(শূন্য);
স্থির শূন্য init_exn_catching_apply(শূন্য);
স্থির পূর্ণাঙ্ক mzscheme_env_main(Scheme_Env *env, পূর্ণাঙ্ক argc, বর্ণ **argv);
স্থির পূর্ণাঙ্ক mzscheme_init(শূন্য);
#ifdef FEAT_EVAL
স্থির Scheme_Object *vim_to_mzscheme(typval_T *vim_মান);
স্থির Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_মান, পূর্ণাঙ্ক depth,
	Scheme_Hash_Table *visited);
স্থির পূর্ণাঙ্ক mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
স্থির পূর্ণাঙ্ক mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, পূর্ণাঙ্ক depth,
	Scheme_Hash_Table *visited);
স্থির Scheme_Object *vim_funcref(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
স্থির পূর্ণাঙ্ক buffer_size_proc(শূন্য *obj UNUSED)
{
    প্রত্যাগম gc_BYTES_TO_শব্দগুলি(পরিমাণ(vim_mz_buffer));
}
স্থির পূর্ণাঙ্ক buffer_mark_proc(শূন্য *obj)
{
    প্রত্যাগম buffer_size_proc(obj);
}
স্থির পূর্ণাঙ্ক buffer_fixup_proc(শূন্য *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    প্রত্যাগম buffer_size_proc(obj);
}
স্থির পূর্ণাঙ্ক window_size_proc(শূন্য *obj UNUSED)
{
    প্রত্যাগম gc_BYTES_TO_শব্দগুলি(পরিমাণ(vim_mz_window));
}
স্থির পূর্ণাঙ্ক window_mark_proc(শূন্য *obj)
{
    প্রত্যাগম window_size_proc(obj);
}
স্থির পূর্ণাঙ্ক window_fixup_proc(শূন্য *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    প্রত্যাগম window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#অন্যথা
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#যদি defined(DYNAMIC_MZSCHEME)
স্থির Scheme_Object *dll_scheme_eof;
স্থির Scheme_Object *dll_scheme_false;
স্থির Scheme_Object *dll_scheme_void;
স্থির Scheme_Object *dll_scheme_null;
স্থির Scheme_Object *dll_scheme_true;

স্থির Scheme_Thread **dll_scheme_current_thread_ptr;

স্থির শূন্য (**dll_scheme_console_printf_ptr)(বর্ণ *str, ...);
স্থির শূন্য (**dll_scheme_console_output_ptr)(বর্ণ *str, OUTPUT_LEN_TYPE len);
স্থির শূন্য (**dll_scheme_notify_multithread_ptr)(পূর্ণাঙ্ক on);

স্থির শূন্য *(*dll_GC_malloc)(size_t size_in_bytes);
স্থির শূন্য *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
স্থির Scheme_Env *(*dll_scheme_basic_env)(শূন্য);
স্থির শূন্য (*dll_scheme_check_threads)(শূন্য);
স্থির শূন্য (*dll_scheme_register_static)(শূন্য *ptr, long size);
স্থির শূন্য (*dll_scheme_set_স্তূপ_base)(শূন্য *base, পূর্ণাঙ্ক no_auto_statics);
স্থির শূন্য (*dll_scheme_add_global)(ধ্রুব বর্ণ *name, Scheme_Object *val,
	Scheme_Env *env);
স্থির শূন্য (*dll_scheme_add_global_প্রতীক)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
স্থির Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, পূর্ণাঙ্ক অঙ্ক_rands,
	Scheme_Object **rands);
স্থির Scheme_Object *(*dll_scheme_builtin_মান)(ধ্রুব বর্ণ *name);
# যদি MZSCHEME_VERSION_MAJOR >= 299
স্থির Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
স্থির Scheme_Object *(*dll_scheme_make_path)(ধ্রুব বর্ণ *chars);
# endif
স্থির শূন্য (*dll_scheme_close_প্রবেশ_port)(Scheme_Object *port);
স্থির শূন্য (*dll_scheme_গুনতি_সারিগুলি)(Scheme_Object *port);
# যদি MZSCHEME_VERSION_MAJOR < 360
স্থির Scheme_Object *(*dll_scheme_current_continuation_marks)(শূন্য);
# অন্যথা
স্থির Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
স্থির শূন্য (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
স্থির বর্ণ *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
স্থির পূর্ণাঙ্ক (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
স্থির Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	পূর্ণাঙ্ক অঙ্ক_rands_2, Scheme_Object **rands, পূর্ণাঙ্ক val);
স্থির শূন্য (*dll_scheme_dont_gc_ptr)(শূন্য *p);
স্থির Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
স্থির Scheme_Object *(*dll_scheme_eval_string)(ধ্রুব বর্ণ *str,
	Scheme_Env *env);
স্থির Scheme_Object *(*dll_scheme_eval_string_all)(ধ্রুব বর্ণ *str,
	Scheme_Env *env, পূর্ণাঙ্ক all);
স্থির শূন্য (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# যদি MZSCHEME_VERSION_MAJOR < 299
স্থির বর্ণ *(*dll_scheme_format)(বর্ণ *format, পূর্ণাঙ্ক flen, পূর্ণাঙ্ক argc,
	Scheme_Object **argv, long *rlen);
# অন্যথা
স্থির বর্ণ *(*dll_scheme_format_utf8)(বর্ণ *format, পূর্ণাঙ্ক flen, পূর্ণাঙ্ক argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
স্থির Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, পূর্ণাঙ্ক pos);
# endif
স্থির শূন্য (*dll_scheme_gc_ptr_ok)(শূন্য *p);
# যদি MZSCHEME_VERSION_MAJOR < 299
স্থির বর্ণ *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# অন্যথা
স্থির বর্ণ *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
স্থির Scheme_Object *(*dll_scheme_intern_প্রতীক)(ধ্রুব বর্ণ *name);
স্থির Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *প্রতীক,
	Scheme_Env *env);
স্থির Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, শূন্য *data, ধ্রুব বর্ণ *name, mzshort mina,
     mzshort maxa);
স্থির Scheme_Object *(*dll_scheme_make_integer_মান)(long i);
স্থির Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
স্থির Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ধ্রুব বর্ণ *name, mzshort mina, mzshort maxa);
# যদি MZSCHEME_VERSION_MAJOR < 299
স্থির Scheme_Object *(*dll_scheme_make_string)(ধ্রুব বর্ণ *chars);
স্থির Scheme_Object *(*dll_scheme_make_string_output_port)();
# অন্যথা
স্থির Scheme_Object *(*dll_scheme_make_byte_string)(ধ্রুব বর্ণ *chars);
স্থির Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
স্থির Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	পূর্ণাঙ্ক argc, Scheme_Object **argv);
স্থির Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক *গুনতি_out);
স্থির Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, পূর্ণাঙ্ক অঙ্ক_fields,
	পূর্ণাঙ্ক অঙ্ক_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# যদি MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
স্থির Scheme_Object **(*dll_scheme_make_struct_মানসমূহ)(
	Scheme_Object *struct_type, Scheme_Object **names, পূর্ণাঙ্ক গুনতি,
	পূর্ণাঙ্ক flags);
স্থির Scheme_Type (*dll_scheme_make_type)(ধ্রুব বর্ণ *name);
স্থির Scheme_Object *(*dll_scheme_make_vector)(পূর্ণাঙ্ক size,
	Scheme_Object *fill);
স্থির শূন্য *(*dll_scheme_malloc_fail_ok)(শূন্য *(*f)(size_t), size_t);
স্থির Scheme_Object *(*dll_scheme_open_প্রবেশ_file)(ধ্রুব বর্ণ *name,
	ধ্রুব বর্ণ *who);
স্থির Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
স্থির পূর্ণাঙ্ক (*dll_scheme_proper_list_length)(Scheme_Object *list);
স্থির শূন্য (*dll_scheme_raise)(Scheme_Object *exn);
স্থির Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
স্থির শূন্য (*dll_scheme_signal_error)(ধ্রুব বর্ণ *বার্তা, ...);
স্থির শূন্য (*dll_scheme_wrong_type)(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *expected,
	পূর্ণাঙ্ক which, পূর্ণাঙ্ক argc, Scheme_Object **argv);
# যদি MZSCHEME_VERSION_MAJOR >= 299
স্থির শূন্য (*dll_scheme_set_param)(Scheme_Config *c, পূর্ণাঙ্ক pos,
	Scheme_Object *o);
স্থির Scheme_Config *(*dll_scheme_current_config)(শূন্য);
স্থির Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
স্থির Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
স্থির শূন্য *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
স্থির Scheme_Hash_Table *(*dll_scheme_make_hash_table)(পূর্ণাঙ্ক type);
স্থির শূন্য (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *চাবি, Scheme_Object *মান);
স্থির Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *চাবি);
স্থির Scheme_Object *(*dll_scheme_make_double)(দ্বিগুণ d);
স্থির Scheme_Object *(*dll_scheme_make_sized_byte_string)(বর্ণ *chars,
	long len, পূর্ণাঙ্ক copy);
স্থির Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
স্থির Scheme_Object *(*dll_scheme_dynamic_wind)(শূন্য (*pre)(শূন্য *), Scheme_Object *(* চঞ্চল act)(শূন্য *), শূন্য (* চঞ্চল post)(শূন্য *), Scheme_Object *(*jmp_handler)(শূন্য *), শূন্য * চঞ্চল data);
# ifdef MZ_PRECISE_GC
স্থির শূন্য *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
স্থির শূন্য (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, পূর্ণাঙ্ক is_constant_size, পূর্ণাঙ্ক is_atomic);
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 400
স্থির শূন্য (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
স্থির শূন্য **(*dll_scheme_malloc_immobile_box)(শূন্য *p);
স্থির শূন্য (*dll_scheme_free_immobile_box)(শূন্য **b);
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
স্থির পূর্ণাঙ্ক (*dll_scheme_main_setup)(পূর্ণাঙ্ক no_auto_statics, Scheme_Env_Main _main, পূর্ণাঙ্ক argc, বর্ণ **argv);
#   যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
স্থির শূন্য (*dll_scheme_register_tls_space)(শূন্য *tls_space, পূর্ণাঙ্ক _tls_index);
#   endif
#  endif
#  যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
স্থির Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(শূন্য);
#  endif
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 600
স্থির শূন্য (*dll_scheme_embedded_load)(intptr_t len, ধ্রুব বর্ণ *s, পূর্ণাঙ্ক predefined);
স্থির শূন্য (*dll_scheme_register_embedded_load)(intptr_t len, ধ্রুব বর্ণ *s);
স্থির শূন্য (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# যদি defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  যদি !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_প্রতীক dll_scheme_add_global_প্রতীক
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_মান dll_scheme_builtin_মান
#  যদি MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_প্রবেশ_port dll_scheme_close_প্রবেশ_port
#  define scheme_গুনতি_সারিগুলি dll_scheme_গুনতি_সারিগুলি
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
#  যদি MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  অন্যথা
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  যদি MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  অন্যথা
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_প্রতীক dll_scheme_intern_প্রতীক
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_মান dll_scheme_make_integer_মান
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  যদি MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  অন্যথা
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_মানসমূহ dll_scheme_make_struct_মানসমূহ
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_প্রবেশ_file dll_scheme_open_প্রবেশ_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_স্তূপ_base dll_scheme_set_স্তূপ_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  যদি MZSCHEME_VERSION_MAJOR >= 299
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
#  যদি MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  যদি MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   যদি MZSCHEME_VERSION_MAJOR >= 500
#    যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(শূন্য)
{
    প্রত্যাগম dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

প্রকারনির্দেশ সংরচনা
{
    বর্ণ    *name;
    শূন্য    **ptr;
} Thunk_Info;

স্থির Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (শূন্য **)&dll_GC_malloc},
    {"GC_malloc_atomic", (শূন্য **)&dll_GC_malloc_atomic},
    {রিক্ত, রিক্ত}};

স্থির Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (শূন্য **)&dll_scheme_eof},
    {"scheme_false", (শূন্য **)&dll_scheme_false},
    {"scheme_void", (শূন্য **)&dll_scheme_void},
    {"scheme_null", (শূন্য **)&dll_scheme_null},
    {"scheme_true", (শূন্য **)&dll_scheme_true},
# যদি !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (শূন্য **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (শূন্য **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (শূন্য **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(শূন্য **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (শূন্য **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (শূন্য **)&dll_scheme_add_global_প্রতীক},
    {"scheme_apply", (শূন্য **)&dll_scheme_apply},
    {"scheme_basic_env", (শূন্য **)&dll_scheme_basic_env},
# যদি MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (শূন্য **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (শূন্য **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (শূন্য **)&dll_scheme_builtin_মান},
    {"scheme_check_threads", (শূন্য **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (শূন্য **)&dll_scheme_close_প্রবেশ_port},
    {"scheme_count_lines", (শূন্য **)&dll_scheme_গুনতি_সারিগুলি},
    {"scheme_current_continuation_marks",
	(শূন্য **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (শূন্য **)&dll_scheme_display},
    {"scheme_display_to_string", (শূন্য **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (শূন্য **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (শূন্য **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (শূন্য **)&dll_scheme_eq},
    {"scheme_eval", (শূন্য **)&dll_scheme_eval},
    {"scheme_eval_string", (শূন্য **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (শূন্য **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(শূন্য **)&dll_scheme_finish_primitive_module},
# যদি MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (শূন্য **)&dll_scheme_format},
# অন্যথা
    {"scheme_format_utf8", (শূন্য **)&dll_scheme_format_utf8},
    {"scheme_get_param", (শূন্য **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (শূন্য **)&dll_scheme_gc_ptr_ok},
# যদি MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(শূন্য **)&dll_scheme_get_sized_string_output},
# অন্যথা
    {"scheme_get_sized_byte_string_output",
	(শূন্য **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (শূন্য **)&dll_scheme_intern_প্রতীক},
    {"scheme_lookup_global", (শূন্য **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(শূন্য **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (শূন্য **)&dll_scheme_make_integer_মান},
    {"scheme_make_pair", (শূন্য **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (শূন্য **)&dll_scheme_make_prim_w_arity},
# যদি MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (শূন্য **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(শূন্য **)&dll_scheme_make_string_output_port},
# অন্যথা
    {"scheme_make_byte_string", (শূন্য **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(শূন্য **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(শূন্য **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (শূন্য **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (শূন্য **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (শূন্য **)&dll_scheme_make_struct_মানসমূহ},
    {"scheme_make_type", (শূন্য **)&dll_scheme_make_type},
    {"scheme_make_vector", (শূন্য **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (শূন্য **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (শূন্য **)&dll_scheme_open_প্রবেশ_file},
    {"scheme_primitive_module", (শূন্য **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (শূন্য **)&dll_scheme_proper_list_length},
    {"scheme_raise", (শূন্য **)&dll_scheme_raise},
    {"scheme_read", (শূন্য **)&dll_scheme_read},
    {"scheme_register_static", (শূন্য **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (শূন্য **)&dll_scheme_set_স্তূপ_base},
    {"scheme_signal_error", (শূন্য **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (শূন্য **)&dll_scheme_wrong_type},
# যদি MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (শূন্য **)&dll_scheme_set_param},
    {"scheme_current_config", (শূন্য **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(শূন্য **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (শূন্য **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (শূন্য **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (শূন্য **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (শূন্য **)&dll_scheme_hash_set},
    {"scheme_hash_get", (শূন্য **)&dll_scheme_hash_get},
    {"scheme_make_double", (শূন্য **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (শূন্য **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (শূন্য **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (শূন্য **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (শূন্য **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (শূন্য **)&dll_GC_register_traversers},
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (শূন্য **)&dll_scheme_main_setup},
#   যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (শূন্য **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (শূন্য **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (শূন্য **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (শূন্য **)&dll_scheme_free_immobile_box},
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 500
#  যদি defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (শূন্য **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# যদি MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (শূন্য **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (শূন্য **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (শূন্য **)&dll_scheme_set_config_path},
# endif
    {রিক্ত, রিক্ত}};

স্থির HINSTANCE hMzGC = 0;
স্থির HINSTANCE hMzSch = 0;

স্থির শূন্য dynamic_mzscheme_end(শূন্য);
স্থির পূর্ণাঙ্ক mzscheme_runtime_link_init(বর্ণ *sch_dll, বর্ণ *gc_dll,
	পূর্ণাঙ্ক verbose);

    স্থির পূর্ণাঙ্ক
mzscheme_runtime_link_init(বর্ণ *sch_dll, বর্ণ *gc_dll, পূর্ণাঙ্ক verbose)
{
    Thunk_Info *thunk = রিক্ত;

    যদি (hMzGC && hMzSch)
	প্রত্যাগম OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    যদি (!hMzGC)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	প্রত্যাগম FAIL;
    }

    যদি (!hMzSch)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	প্রত্যাগম FAIL;
    }

    প্রতি (thunk = mzsch_imports; thunk->name; thunk++)
    {
	যদি ((*thunk->ptr =
		    (শূন্য *)GetProcAddress(hMzSch, thunk->name)) == রিক্ত)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    প্রত্যাগম FAIL;
	}
    }
    প্রতি (thunk = mzgc_imports; thunk->name; thunk++)
    {
	যদি ((*thunk->ptr =
		    (শূন্য *)GetProcAddress(hMzGC, thunk->name)) == রিক্ত)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

    পূর্ণাঙ্ক
mzscheme_enabled(পূর্ণাঙ্ক verbose)
{
    প্রত্যাগম mzscheme_runtime_link_init(
	    (বর্ণ *)p_mzschemedll, (বর্ণ *)p_mzschemegcdll, verbose) == OK;
}

    স্থির শূন্য
dynamic_mzscheme_end(শূন্য)
{
    যদি (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    যদি (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#যদি MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, অঙ্ক) GUARANTEE_STRING(proc, অঙ্ক)
#অন্যথা
    স্থির Scheme_Object *
guaranteed_byte_string_arg(বর্ণ *proc, পূর্ণাঙ্ক অঙ্ক, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    যদি (SCHEME_BYTE_STRINGP(argv[অঙ্ক]))
    {
	প্রত্যাগম argv[অঙ্ক];
    }
    অন্যথা যদি (SCHEME_CHAR_STRINGP(argv[অঙ্ক]))
    {
	Scheme_Object *tmp = রিক্ত;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[অঙ্ক]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[অঙ্ক]);
	MZ_GC_UNREG();
	প্রত্যাগম tmp;
    }
    অন্যথা
	scheme_wrong_type(proc, "string", অঙ্ক, argc, argv);
    // unreachable
    প্রত্যাগম scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, অঙ্ক) guaranteed_byte_string_arg(proc, অঙ্ক, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#যদি defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

স্থির Scheme_Type mz_buffer_type;
স্থির Scheme_Type mz_window_type;

স্থির পূর্ণাঙ্ক initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
স্থির পূর্ণাঙ্ক disabled = FALSE;
#endif
স্থির পূর্ণাঙ্ক load_base_module_failed = FALSE;

// global environment
স্থির Scheme_Env    *environment = রিক্ত;
// output/error handlers
স্থির Scheme_Object *curout = রিক্ত;
স্থির Scheme_Object *curerr = রিক্ত;
// exn:vim exception
স্থির Scheme_Object *exn_catching_apply = রিক্ত;
স্থির Scheme_Object *exn_p = রিক্ত;
স্থির Scheme_Object *exn_message = রিক্ত;
স্থির Scheme_Object *vim_exn = রিক্ত; // Vim Error exception

#যদি !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
স্থির শূন্য *স্তূপ_base = রিক্ত;
#endif

স্থির long range_start;
স্থির long range_end;

// MzScheme threads scheduling stuff
স্থির পূর্ণাঙ্ক mz_threads_allow = 0;

#যদি defined(FEAT_GUI_MSWIN)
স্থির শূন্য CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
স্থির UINT timer_id = 0;
#নোচেদ্যদি defined(FEAT_GUI_GTK)
স্থির gboolean timer_proc(gpointer);
স্থির guint timer_id = 0;
#নোচেদ্যদি defined(FEAT_GUI_MOTIF)
স্থির শূন্য timer_proc(XtPointer, XtIntervalId *);
স্থির XtIntervalId timer_id = (XtIntervalId)0;
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    শূন্য
mzvim_check_threads(শূন্য)
{
    // Last time MzScheme threads were scheduled
    স্থির time_t mz_last_time = 0;

    যদি (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(রিক্ত);

	যদি ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#যদি defined(MZSCHEME_GUI_THREADS)
স্থির শূন্য setup_timer(শূন্য);
স্থির শূন্য remove_timer(শূন্য);

// timers are presented in GUI only
# যদি defined(FEAT_GUI_MSWIN)
    স্থির শূন্য CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_বার্তা UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    স্থির gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    স্থির শূন্য
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# যদি defined(FEAT_GUI_GTK)
    প্রত্যাগম TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    যদি (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, রিক্ত);
# endif
}

    স্থির শূন্য
setup_timer(শূন্য)
{
# যদি defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(রিক্ত, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, রিক্ত);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, রিক্ত);
# endif
}

    স্থির শূন্য
remove_timer(শূন্য)
{
# যদি defined(FEAT_GUI_MSWIN)
    KillTimer(রিক্ত, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    বর্ণ *
did_set_mzquantum(optset_T *args UNUSED)
{
#যদি defined(MZSCHEME_GUI_THREADS)
    // reset timer
    যদি (timer_id != 0)
	remove_timer();
    যদি (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    প্রত্যাগম রিক্ত;
}

    স্থির শূন্য
notify_multithread(পূর্ণাঙ্ক on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    যদি (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    যদি (!on && timer_id != 0)
	remove_timer();
#endif
}

    শূন্য
mzscheme_end(শূন্য)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#যদি 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#যদি HAVE_TLS_SPACE && !defined(VIMDLL)
# যদি defined(_MSC_VER)
স্থির __declspec(thread) শূন্য *tls_space;
বাহ্য intptr_t _tls_index;
# elif defined(__MINGW32__)
স্থির __thread শূন্য *tls_space;
বাহ্য intptr_t _tls_index;
# অন্যথা
স্থির THREAD_LOCAL শূন্য *tls_space;
স্থির intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    পূর্ণাঙ্ক
mzscheme_main(শূন্য)
{
    পূর্ণাঙ্ক	    argc = 0;
    বর্ণ    *argv = রিক্ত;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    যদি (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	প্রত্যাগম vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    শূন্য **ptls_space;
    intptr_t tls_index;
    শূন্য (*pget_tls_info)(শূন্য ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (শূন্য *)GetProcAddress(
				GetModuleHandle(রিক্ত), "get_tls_info");
    যদি (pget_tls_info == রিক্ত)
    {
	disabled = TRUE;
	প্রত্যাগম vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# অন্যথা
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    প্রত্যাগম scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#অন্যথা
    প্রত্যাগম mzscheme_env_main(রিক্ত, argc, &argv);
#endif
}

    স্থির পূর্ণাঙ্ক
mzscheme_env_main(Scheme_Env *env, পূর্ণাঙ্ক argc UNUSED, বর্ণ **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#অন্যথা
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = রিক্ত;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    স্তূপ_base = &__gc_var_stack__;
# অন্যথা
    পূর্ণাঙ্ক dummy = 0;
    স্তূপ_base = (শূন্য *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    প্রত্যাগম 0;
}

    স্থির Scheme_Object*
load_base_module(শূন্য *data)
{
    scheme_namespace_require(scheme_intern_প্রতীক((বর্ণ *)data));
    প্রত্যাগম scheme_null;
}

    স্থির Scheme_Object *
load_base_module_on_error(শূন্য *data UNUSED)
{
    load_base_module_failed = TRUE;
    প্রত্যাগম scheme_null;
}

    স্থির পূর্ণাঙ্ক
startup_mzscheme(শূন্য)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_স্তূপ_base(স্তূপ_base, 1);
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
#যদি MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = রিক্ত;
	পূর্ণাঙ্ক mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	যদি (s != রিক্ত)
	{
	    coll_path = scheme_make_path((বর্ণ *)s);
	    MZ_GC_CHECK();
	    যদি (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	যদি (coll_path == রিক্ত)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	যদি (coll_path != রিক্ত)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#অন্যথা
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = রিক্ত;
	Scheme_Object	*coll_pair = রিক্ত;
	Scheme_Config	*config = রিক্ত;

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

#যদি MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = রিক্ত;
	পূর্ণাঙ্ক mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	যদি (s != রিক্ত)
	{
	    config_path = scheme_make_path((বর্ণ *)s);
	    MZ_GC_CHECK();
	    যদি (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	যদি (config_path == রিক্ত)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	যদি (config_path != রিক্ত)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#যদি MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(রিক্ত, load_base_module, রিক্ত,
				    load_base_module_on_error, "racket/base");
	যদি (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(রিক্ত, load_base_module, রিক্ত,
				    load_base_module_on_error, "scheme/base");
	    যদি (load_base_module_failed)
		প্রত্যাগম -1;
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
	Scheme_Object	*make_security_guard = রিক্ত;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# যদি MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_প্রতীক = রিক্ত;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_প্রতীক);
	    MZ_GC_REG();
	    make_security_guard_প্রতীক = scheme_intern_প্রতীক("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_প্রতীক, environment);
	    MZ_GC_UNREG();
	}
# অন্যথা
	make_security_guard = scheme_builtin_মান("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	যদি (make_security_guard != রিক্ত)
	{
	    Scheme_Object   *args[3] = {রিক্ত, রিক্ত, রিক্ত};
	    Scheme_Object   *guard = রিক্ত;
	    Scheme_Config   *config = রিক্ত;
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

    প্রত্যাগম 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    স্থির পূর্ণাঙ্ক
mzscheme_init(শূন্য)
{
    যদি (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	যদি (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    প্রত্যাগম -1;
	}
#endif
	যদি (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    প্রত্যাগম -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = রিক্ত;
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

    প্রত্যাগম 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    স্থির পূর্ণাঙ্ক
eval_with_exn_handling(
    শূন্য		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *মান = রিক্ত;
    Scheme_Object   *exn = রিক্ত;
    Scheme_Object   *prim = রিক্ত;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, মান);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    মান = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    যদি (!মান)
    {
	মান = extract_exn_message(exn);
	// Got an exn?
	যদি (মান)
	{
	    scheme_display(মান, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	প্রত্যাগম FAIL;
    }

    যদি (ret != রিক্ত)	// if pointer to retval supported give it up
	*ret = মান;
    // Print any result, as long as it's not a void
    অন্যথা যদি (!SCHEME_VOIDP(মান))
    {
	scheme_display(মান, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    প্রত্যাগম OK;
}

/*
 * :mzscheme
 */
    স্থির পূর্ণাঙ্ক
do_mzscheme_command(exarg_T *eap, শূন্য *data, Scheme_Closed_Prim *what)
{
    যদি (mzscheme_init())
	প্রত্যাগম FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    প্রত্যাগম eval_with_exn_handling(data, what, রিক্ত);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    শূন্য
mzscheme_buffer_free(buf_T *buf)
{
    যদি (buf->b_mzscheme_ref == রিক্ত)
	প্রত্যাগম;

    vim_mz_buffer *bp = রিক্ত;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_মান;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#অন্যথা
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = রিক্ত;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    শূন্য
mzscheme_window_free(win_T *win)
{
    যদি (win->w_mzscheme_ref == রিক্ত)
	প্রত্যাগম;

    vim_mz_window *wp = রিক্ত;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_মান;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#অন্যথা
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = রিক্ত;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    শূন্য
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    যদি (eap->skip)
	প্রত্যাগম;

    যদি (script == রিক্ত)
	do_mzscheme_command(eap, eap->arg, do_eval);
    অন্যথা
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    স্থির Scheme_Object *
do_load(শূন্য *data, পূর্ণাঙ্ক noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *রাশিমালা = রিক্ত;
    Scheme_Object   *result = রিক্ত;
    বর্ণ	    *file = রিক্ত;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, রাশিমালা);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (বর্ণ *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_প্রবেশ_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_গুনতি_সারিগুলি(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    যাবত্ (!SCHEME_EOFP(রাশিমালা = scheme_read(pinfo->port)))
    {
	result = scheme_eval(রাশিমালা, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_প্রবেশ_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = রিক্ত;
    MZ_GC_UNREG();
    প্রত্যাগম result;
}

/*
 * :mzfile
 */
    শূন্য
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {রিক্ত, রিক্ত};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (বর্ণ *)eap->arg;
    যদি (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != রিক্ত)	// looks like port was not closed
    {
	scheme_close_প্রবেশ_port(pinfo.port);
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
    স্থির শূন্য
init_exn_catching_apply(শূন্য)
{
    যদি (exn_catching_apply)
	প্রত্যাগম;

    বর্ণ *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_মান("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_মান("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    স্থির Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    যদি (SCHEME_TRUEP(SCHEME_CAR(v)))
	প্রত্যাগম SCHEME_CDR(v);
    অন্যথা
    {
	*exn = SCHEME_CDR(v);
	প্রত্যাগম রিক্ত;
    }
}

    স্থির Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    যদি (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	প্রত্যাগম _scheme_apply(exn_message, 1, &v);
    অন্যথা
	প্রত্যাগম রিক্ত; // Not an exn structure
}

    স্থির Scheme_Object *
do_eval(শূন্য *s, পূর্ণাঙ্ক noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    প্রত্যাগম scheme_eval_string_all((বর্ণ *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    স্থির শূন্য
do_intrnl_output(বর্ণ *mesg, পূর্ণাঙ্ক error)
{
    বর্ণ *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    যাবত্ (p)
    {
	*p = '\0';
	যদি (error)
	    emsg(prev);
	অন্যথা
	    বার্তা(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    যদি (error)
	emsg(prev);
    অন্যথা
	বার্তা(prev);
}

    স্থির শূন্য
do_output(বর্ণ *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    স্থির শূন্য
do_err_output(বর্ণ *mesg)
{
    do_intrnl_output(mesg, 1);
}

    স্থির শূন্য
do_printf(বর্ণ *format, ...)
{
    do_intrnl_output(format, 1);
}

    স্থির শূন্য
do_flush(শূন্য)
{
    বর্ণ *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    যদি (length)
    {
	do_err_output(buff);
	প্রত্যাগম;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    যদি (length)
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
    স্থির Scheme_Object *
vim_command(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = রিক্ত;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_মান(cmd), রিক্ত, রিক্ত, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    প্রত্যাগম scheme_void;
}

/*
 * (eval {expr-string})
 */
    স্থির Scheme_Object *
vim_eval(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = রিক্ত;
    typval_T		*vim_result;
    Scheme_Object *রাশিমালা = রিক্ত;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, রাশিমালা);
    MZ_GC_REG();
    রাশিমালা = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_রাশিমালা(BYTE_STRING_মান(রাশিমালা), রিক্ত);

    যদি (vim_result == রিক্ত)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    প্রত্যাগম result;
#অন্যথা
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    প্রত্যাগম scheme_false;
#endif
}

/*
 * (range-start)
 */
    স্থির Scheme_Object *
get_range_start(
    শূন্য		*data UNUSED,
    পূর্ণাঙ্ক			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    প্রত্যাগম scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    স্থির Scheme_Object *
get_range_end(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
    প্রত্যাগম scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    স্থির Scheme_Object *
mzscheme_beep(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    প্রত্যাগম scheme_void;
}

স্থির Scheme_Object *M_global = রিক্ত;

/*
 * (get-option {option-name}) [buffer/window]
 */
    স্থির Scheme_Object *
get_option(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    মান;
    বর্ণ	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = রিক্ত;
    Scheme_Object   *name = রিক্ত;
    পূর্ণাঙ্ক		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    যদি (argc > 1)
    {
	যদি (M_global == রিক্ত)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_প্রতীক("global");
	    MZ_GC_CHECK();
	}

	যদি (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	অন্যথা যদি (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	অন্যথা যদি (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	অন্যথা
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_মান(BYTE_STRING_মান(name), &মান, (char_u **)&strval,
								  রিক্ত, scope);
    curbuf = save_curb;
    curwin = save_curw;

    বিকল্প (rc)
    {
    পক্ষ gov_bool:
    পক্ষ gov_সংখ্যা:
	MZ_GC_UNREG();
	প্রত্যাগম scheme_make_integer_মান(মান);
    পক্ষ gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	প্রত্যাগম rval;
    পক্ষ gov_hidden_bool:
    পক্ষ gov_hidden_সংখ্যা:
    পক্ষ gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    পক্ষ gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    প্রত্যাগম scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    স্থির Scheme_Object *
set_option(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    char_u	*command = রিক্ত;
    পূর্ণাঙ্ক		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = রিক্ত;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    যদি (argc > 1)
    {
	যদি (M_global == রিক্ত)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_প্রতীক("global");
	    MZ_GC_CHECK();
	}

	যদি (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	অন্যথা যদি (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	অন্যথা যদি (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	অন্যথা
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_মান(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    প্রত্যাগম scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    স্থির Scheme_Object *
get_curr_win(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
    প্রত্যাগম (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    স্থির Scheme_Object *
get_window_গুনতি(
    শূন্য		*data UNUSED,
    পূর্ণাঙ্ক			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    পূর্ণাঙ্ক	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    প্রত্যাগম scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    স্থির Scheme_Object *
get_window_list(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    প্রতি ( ; w != রিক্ত; w = w->w_next)
	যদি (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    প্রত্যাগম list;
}

    স্থির Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = রিক্ত;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    যদি (win->w_mzscheme_ref != রিক্ত)
	প্রত্যাগম (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, পরিমাণ(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(রিক্ত);
#অন্যথা
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    প্রত্যাগম (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    স্থির Scheme_Object *
get_window_অঙ্ক(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
    পূর্ণাঙ্ক		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    প্রতি (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    প্রত্যাগম scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    স্থির Scheme_Object *
get_window_by_অঙ্ক(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    পূর্ণাঙ্ক		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    যদি (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    প্রতি ( ; win != রিক্ত; win = win->w_next, --fnum)
	যদি (fnum == 1)	    // to be 1-based
	    প্রত্যাগম window_new(win);

    প্রত্যাগম scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    স্থির Scheme_Object *
get_window_buffer(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    প্রত্যাগম buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    স্থির Scheme_Object *
get_window_height(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    প্রত্যাগম scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    স্থির Scheme_Object *
set_window_height(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    পূর্ণাঙ্ক		    height;

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
    প্রত্যাগম scheme_void;
}

/*
 * (get-win-width [window])
 */
    স্থির Scheme_Object *
get_window_width(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    প্রত্যাগম scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    স্থির Scheme_Object *
set_window_width(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    পূর্ণাঙ্ক		    width = 0;

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
    প্রত্যাগম scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    স্থির Scheme_Object *
get_cursor(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    প্রত্যাগম scheme_make_pair(scheme_make_integer_মান((long)pos.lnum),
		    scheme_make_integer_মান((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    স্থির Scheme_Object *
set_cursor(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
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

    যদি (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_সারি_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    প্রত্যাগম scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    স্থির Scheme_Object *
mzscheme_open_buffer(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    পূর্ণাঙ্ক		    অঙ্ক = 0;
    Scheme_Object   *onum = রিক্ত;
    Scheme_Object   *buf = রিক্ত;
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
    অঙ্ক = buflist_add(BYTE_STRING_মান(fname), BLN_LISTED | BLN_CURBUF);

    যদি (অঙ্ক == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(অঙ্ক);
    buf = get_buffer_by_অঙ্ক(data, 1, &onum);
    MZ_GC_UNREG();
    প্রত্যাগম buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    স্থির Scheme_Object *
get_buffer_by_অঙ্ক(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    পূর্ণাঙ্ক		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	যদি (buf->b_fnum == fnum)
	    প্রত্যাগম buffer_new(buf);

    প্রত্যাগম scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    স্থির Scheme_Object *
get_buffer_by_name(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = রিক্ত;
    Scheme_Object *fname = রিক্ত;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	যদি (buf->b_ffname == রিক্ত || buf->b_sfname == রিক্ত)
	    // empty string
	{
	    যদি (BYTE_STRING_মান(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	অন্যথা যদি (!fnamecmp(buf->b_ffname, BYTE_STRING_মান(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_মান(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    প্রত্যাগম buffer;
}

/*
 * (get-next-buff [buffer])
 */
    স্থির Scheme_Object *
get_next_buffer(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    যদি (buf->b_next == রিক্ত)
	প্রত্যাগম scheme_false;
    অন্যথা
	প্রত্যাগম buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    স্থির Scheme_Object *
get_prev_buffer(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    যদি (buf->b_prev == রিক্ত)
	প্রত্যাগম scheme_false;
    অন্যথা
	প্রত্যাগম buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    স্থির Scheme_Object *
get_buffer_অঙ্ক(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    প্রত্যাগম scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    স্থির Scheme_Object *
get_buffer_গুনতি(
    শূন্য		*data UNUSED,
    পূর্ণাঙ্ক			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    পূর্ণাঙ্ক	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    প্রত্যাগম scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    স্থির Scheme_Object *
get_buffer_name(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    প্রত্যাগম scheme_make_byte_string((বর্ণ *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    স্থির Scheme_Object *
get_curr_buffer(
    শূন্য		*data UNUSED,
    পূর্ণাঙ্ক			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    প্রত্যাগম (Scheme_Object *)get_vim_curr_buffer();
}

    স্থির Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = রিক্ত;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    যদি (buf->b_mzscheme_ref)
	প্রত্যাগম (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, পরিমাণ(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(রিক্ত);
#অন্যথা
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    প্রত্যাগম (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    স্থির Scheme_Object *
get_buffer_size(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    প্রত্যাগম scheme_make_integer(buf->buf->b_ml.ml_সারি_গুনতি);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    স্থির Scheme_Object *
get_buffer_সারি(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    পূর্ণাঙ্ক		    linenr;
    char_u	    *সারি;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    সারি = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    প্রত্যাগম scheme_make_byte_string((বর্ণ *)সারি);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    স্থির Scheme_Object *
get_buffer_সারি_list(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    পূর্ণাঙ্ক		    i, hi, lo, n;
    Scheme_Object   *list = রিক্ত;

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
    যদি (lo < 0)
	lo = 0;

    যদি (hi < 0)
	hi = 0;
    যদি (hi < lo)
	hi = lo;

    n = hi - lo;

    প্রতি (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (বর্ণ *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    প্রত্যাগম list;
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
    স্থির Scheme_Object *
set_buffer_সারি(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *সারি = রিক্ত;
    বর্ণ	    *save;
    পূর্ণাঙ্ক		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, সারি);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    যদি (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    সারি = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_সারি_range(n, buf->buf);

    যদি (SCHEME_FALSEP(সারি))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	যদি (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	অন্যথা যদি (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	যদি (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_সারিগুলি_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	প্রত্যাগম scheme_void;
    }
    অন্যথা
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_সারি(সারি);

	curbuf = buf->buf;

	যদি (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	অন্যথা যদি (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	অন্যথা
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	যদি (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	প্রত্যাগম scheme_void;
    }
}

    স্থির শূন্য
free_array(বর্ণ **array)
{
    বর্ণ **curr = array;
    যাবত্ (*curr != রিক্ত)
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
    স্থির Scheme_Object *
set_buffer_সারি_list(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = রিক্ত;
    Scheme_Object   *সারি_list = রিক্ত;
    পূর্ণাঙ্ক		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, সারি_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    যদি (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    সারি_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    যদি (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_সারি_range(lo, buf->buf);	    // inclusive
    check_সারি_range(hi - 1, buf->buf);	    // exclusive

    যদি (SCHEME_FALSEP(সারি_list) || SCHEME_NULLP(সারি_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	যদি (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	অন্যথা
	{
	    প্রতি (i = 0; i < old_len; i++)
		যদি (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    যদি (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_সারিগুলি_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	প্রত্যাগম scheme_void;
    }
    অন্যথা
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(সারি_list);
	MZ_GC_CHECK();
	যদি (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	অন্যথা
	{
	    বর্ণ		**array = রিক্ত;
	    Scheme_Object   *সারি = রিক্ত;
	    Scheme_Object   *rest = রিক্ত;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, সারি);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(বর্ণ *, new_len + 1);

	    rest = সারি_list;
	    প্রতি (i = 0; i < new_len; ++i)
	    {
		সারি = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		যদি (!SCHEME_STRINGP(সারি))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_সারি(সারি);
	    }

	    curbuf = buf->buf;

	    যদি (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    প্রতি (i = 0; i < old_len - new_len; ++i)
	    {
		যদি (ml_delete((linenr_T)lo) == FAIL)
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
	    প্রতি (i = 0; i < old_len && i < new_len; i++)
		যদি (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    যাবত্ (i < new_len)
	    {
		যদি (ml_append((linenr_T)(lo + i - 1),
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
	changed_সারিগুলি((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	যদি (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	প্রত্যাগম scheme_void;
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
    স্থির Scheme_Object *
ঢোকাও_buffer_সারি_list(শূন্য *data, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = রিক্ত;
    Scheme_Object   *list = রিক্ত;
    বর্ণ	    *str = রিক্ত;
    পূর্ণাঙ্ক		    i, n, size;

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

    যদি (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    যদি (n != 0)	    // 0 can be used in insert
	check_সারি_range(n, buf->buf);
    যদি (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_সারি(list);
	curbuf = buf->buf;

	যদি (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	অন্যথা যদি (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	অন্যথা
	{
	    vim_free(str);
	    appended_সারিগুলি_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	প্রত্যাগম scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    যদি (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    অন্যথা
    {
	Scheme_Object   *সারি = রিক্ত;
	Scheme_Object   *rest = রিক্ত;
	বর্ণ		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, সারি);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(বর্ণ *, size + 1);

	rest = list;
	প্রতি (i = 0; i < size; ++i)
	{
	    সারি = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_সারি(সারি);
	}

	curbuf = buf->buf;

	যদি (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	অন্যথা
	{
	    প্রতি (i = 0; i < size; ++i)
		যদি (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    যদি (i > 0)
		appended_সারিগুলি_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    প্রত্যাগম scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    স্থির Scheme_Object *
vim_bufferp(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv)
{
    যদি (SCHEME_VIMBUFFERP(argv[0]))
	প্রত্যাগম scheme_true;
    অন্যথা
	প্রত্যাগম scheme_false;
}

/*
 * (win? obj)
 */
    স্থির Scheme_Object *
vim_windowp(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv)
{
    যদি (SCHEME_VIMWINDOWP(argv[0]))
	প্রত্যাগম scheme_true;
    অন্যথা
	প্রত্যাগম scheme_false;
}

/*
 * (buff-valid? obj)
 */
    স্থির Scheme_Object *
vim_buffer_validp(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv)
{
    যদি (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_মান)
	প্রত্যাগম scheme_true;
    অন্যথা
	প্রত্যাগম scheme_false;
}

/*
 * (win-valid? obj)
 */
    স্থির Scheme_Object *
vim_window_validp(শূন্য *data UNUSED, পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv)
{
    যদি (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_মান)
	প্রত্যাগম scheme_true;
    অন্যথা
	প্রত্যাগম scheme_false;
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
    স্থির বর্ণ *
string_to_সারি(Scheme_Object *obj)
{
    বর্ণ	*scheme_str = রিক্ত;
    বর্ণ	*vim_str = রিক্ত;
    OUTPUT_LEN_TYPE	len;
    পূর্ণাঙ্ক		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    যদি (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    প্রতি (i = 0; i < len; ++i)
    {
	যদি (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	অন্যথা
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    প্রত্যাগম vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    স্থির Scheme_Object *
vim_to_mzscheme(typval_T *vim_মান)
{
    Scheme_Object	*result = রিক্ত;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = রিক্ত;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_মান, 1, visited);

    MZ_GC_UNREG();
    প্রত্যাগম result;
}

    স্থির Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_মান,
    পূর্ণাঙ্ক			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = রিক্ত;
    পূর্ণাঙ্ক		    new_মান_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    যদি (depth > 100)
    {
	MZ_GC_UNREG();
	প্রত্যাগম scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_মান);
    MZ_GC_CHECK();
    যদি (result != রিক্ত) // found, do nothing
	new_মান_2 = FALSE;
    অন্যথা যদি (vim_মান->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((বর্ণ *)vim_মান->vval.v_string);
	MZ_GC_CHECK();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_সংখ্যা)
    {
	result = scheme_make_integer((long)vim_মান->vval.v_সংখ্যা);
	MZ_GC_CHECK();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((দ্বিগুণ)vim_মান->vval.v_float);
	MZ_GC_CHECK();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_LIST)
    {
	list_T		*list = vim_মান->vval.v_list;
	listitem_T	*curr;

	যদি (list == রিক্ত || list->lv_first == রিক্ত)
	    result = scheme_null;
	অন্যথা
	{
	    Scheme_Object   *obj = রিক্ত;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    যাবত্ (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_DICT)
    {
	Scheme_Object	  *চাবি = রিক্ত;
	Scheme_Object	  *obj = রিক্ত;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, চাবি);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	যদি (vim_মান->vval.v_dict != রিক্ত)
	{
	    hashtab_T	*ht = &vim_মান->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
	    {
		যদি (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    চাবি = scheme_make_byte_string((বর্ণ *)hi->hi_চাবি);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, চাবি, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = রিক্ত;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((বর্ণ *)vim_মান->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ধ্রুব বর্ণ *)BYTE_STRING_মান(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    অন্যথা যদি (vim_মান->v_type == VAR_PARTIAL)
    {
	যদি (vim_মান->vval.v_partial == রিক্ত)
	    result = scheme_null;
	অন্যথা
	{
	    Scheme_Object *funcname = রিক্ত;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (বর্ণ *)partial_name(vim_মান->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ধ্রুব বর্ণ *)BYTE_STRING_মান(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    অন্যথা যদি (vim_মান->v_type == VAR_BOOL || vim_মান->v_type == VAR_SPECIAL)
    {
	যদি (vim_মান->vval.v_সংখ্যা <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_মান->vval.v_সংখ্যা);
	অন্যথা
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    অন্যথা
    {
	result = scheme_void;
	new_মান_2 = FALSE;
    }
    যদি (new_মান_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_মান, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    প্রত্যাগম result;
}

    স্থির পূর্ণাঙ্ক
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    পূর্ণাঙ্ক i, status;
    Scheme_Hash_Table	*visited = রিক্ত;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    প্রতি (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	যদি (visited->vals[i] != রিক্ত)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    প্রত্যাগম status;
}
    স্থির পূর্ণাঙ্ক
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, পূর্ণাঙ্ক depth,
	Scheme_Hash_Table *visited)
{
    পূর্ণাঙ্ক		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    যদি (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = 0;
	প্রত্যাগম FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    যদি (found != রিক্ত)
	copy_tv(found, tv);
    অন্যথা যদি (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_সংখ্যা = VVAL_NULL;
    }
    অন্যথা যদি (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = SCHEME_INT_VAL(obj);
    }
    অন্যথা যদি (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_সংখ্যা = SCHEME_TRUEP(obj);
    }
    অন্যথা যদি (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    অন্যথা যদি (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_মান(obj));
    }
# যদি MZSCHEME_VERSION_MAJOR >= 299
    অন্যথা যদি (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = রিক্ত;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_মান(tmp));
	MZ_GC_UNREG();
    }
# endif
    অন্যথা যদি (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	যদি (list == রিক্ত)
	    status = FAIL;
	অন্যথা
	{
	    পূর্ণাঙ্ক		    i;
	    Scheme_Object   *curr = রিক্ত;
	    Scheme_Object   *cval = রিক্ত;
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
	    যদি (v == রিক্ত)
		status = FAIL;
	    অন্যথা
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		যদি (SCHEME_VECTORP(obj))
		{
		    প্রতি (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			যদি (status == FAIL)
			    ভঙ্গ;
			status = list_append_tv(list, v);
			clear_tv(v);
			যদি (status == FAIL)
			    ভঙ্গ;
		    }
		}
		অন্যথা যদি (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    প্রতি (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			যদি (status == FAIL)
			    ভঙ্গ;
			status = list_append_tv(list, v);
			clear_tv(v);
			যদি (status == FAIL)
			    ভঙ্গ;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    যদি (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			যদি (status == OK)
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
    অন্যথা যদি (SCHEME_HASHTP(obj))
    {
	পূর্ণাঙ্ক		i;
	dict_T		*dict;
	Scheme_Object   *চাবি = রিক্ত;
	Scheme_Object   *val = রিক্ত;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, চাবি);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	যদি (dict == রিক্ত)
	    status = FAIL;
	অন্যথা
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    প্রতি (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		যদি (((Scheme_Hash_Table *) obj)->vals[i] != রিক্ত)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_সারি(
				((Scheme_Hash_Table *) obj)->চাবিগুলি[i]));
		    // convert Scheme val to Vim and add it to the dict
		    যদি (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			ভঙ্গ;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    অন্যথা
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_সারি(obj);
    }
    MZ_GC_UNREG();
    প্রত্যাগম status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    স্থির Scheme_Object *
vim_funcref(শূন্য *name, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    পূর্ণাঙ্ক i;
    typval_T args;
    পূর্ণাঙ্ক status = OK;
    Scheme_Object *result = রিক্ত;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    যদি (list == রিক্ত)
	status = FAIL;
    অন্যথা
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	প্রতি (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    যদি (v == রিক্ত)
		status = FAIL;
	    অন্যথা
	    {
		status = mzscheme_to_vim(argv[i], v);
		যদি (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	যদি (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_মান((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    যদি (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    অন্যথা
	raise_if_error();
    প্রত্যাগম result;
}

    শূন্য
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = রিক্ত;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    যদি (mzscheme_init())
    {
	MZ_GC_UNREG();
	প্রত্যাগম;
    }

    MZ_GC_CHECK();
    যদি (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    স্থির পূর্ণাঙ্ক
vim_error_check(শূন্য)
{
    প্রত্যাগম (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    স্থির শূন্য
register_vim_exn(শূন্য)
{
    পূর্ণাঙ্ক	nc = 0;
    পূর্ণাঙ্ক i;
    Scheme_Object   *struct_exn = রিক্ত;
    Scheme_Object   *exn_name = রিক্ত;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_প্রতীক("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_মান("struct:exn");
    MZ_GC_CHECK();

    যদি (vim_exn == রিক্ত)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, রিক্ত, 0, 0, রিক্ত, রিক্ত
#যদি MZSCHEME_VERSION_MAJOR >= 299
		, রিক্ত
#endif
		);


    {
	Scheme_Object   **tmp = রিক্ত;
	Scheme_Object   *exn_names[5] = {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};
	Scheme_Object   *exn_মানসমূহ[5] = {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_মানসমূহ, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * পরিমাণ(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_মানসমূহ(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_মানসমূহ, tmp, nc * পরিমাণ(Scheme_Object *));
	MZ_GC_CHECK();

	প্রতি (i = 0; i < nc; i++)
	{
	    scheme_add_global_প্রতীক(exn_names[i],
		    exn_মানসমূহ[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    শূন্য
raise_vim_exn(ধ্রুব বর্ণ *add_info)
{
    বর্ণ	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {রিক্ত, রিক্ত};
    Scheme_Object   *exn = রিক্ত;
    Scheme_Object   *byte_string = রিক্ত;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    যদি (add_info != রিক্ত)
    {
	বর্ণ		*c_string = রিক্ত;
	Scheme_Object   *info = রিক্ত;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (পূর্ণাঙ্ক)STRLEN(fmt), 1, &info, রিক্ত);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    অন্যথা
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#যদি MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#অন্যথা
    argv[1] = scheme_current_continuation_marks(রিক্ত);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    শূন্য
raise_if_error(শূন্য)
{
    যদি (vim_error_check())
	raise_vim_exn(রিক্ত);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    স্থির vim_mz_buffer *
get_buffer_arg(ধ্রুব বর্ণ *fname, পূর্ণাঙ্ক argnum, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    যদি (argc < argnum + 1)
	প্রত্যাগম get_vim_curr_buffer();
    যদি (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (শূন্য)get_valid_buffer(argv[argnum]);
    প্রত্যাগম b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    স্থির vim_mz_window *
get_window_arg(ধ্রুব বর্ণ *fname, পূর্ণাঙ্ক argnum, পূর্ণাঙ্ক argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    যদি (argc < argnum + 1)
	প্রত্যাগম get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    যদি (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (শূন্য)get_valid_window(argv[argnum]);
    প্রত্যাগম w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(শূন্য *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    যদি (buf == INVALID_BUFFER_মান)
	scheme_signal_error(_("buffer is invalid"));
    প্রত্যাগম buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(শূন্য *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    যদি (win == INVALID_WINDOW_মান)
	scheme_signal_error(_("window is invalid"));
    প্রত্যাগম win;
}

    পূর্ণাঙ্ক
mzthreads_allowed(শূন্য)
{
    প্রত্যাগম mz_threads_allow;
}

    স্থির পূর্ণাঙ্ক
সারি_in_range(linenr_T lnum, buf_T *buf)
{
    প্রত্যাগম (lnum > 0 && lnum <= buf->b_ml.ml_সারি_গুনতি);
}

    স্থির শূন্য
check_সারি_range(linenr_T lnum, buf_T *buf)
{
    যদি (!সারি_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    স্থির শূন্য
mz_fix_cursor(পূর্ণাঙ্ক lo, পূর্ণাঙ্ক hi, পূর্ণাঙ্ক extra)
{
    যদি (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	যদি (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	অন্যথা যদি (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	অন্যথা
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

স্থির Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_সারি, "get-buff-line", 1, 2},
    {set_buffer_সারি, "set-buff-line", 2, 3},
    {get_buffer_সারি_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_অঙ্ক, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_সারি_list, "set-buff-line-list", 3, 4},
    {ঢোকাও_buffer_সারি_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_গুনতি, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_অঙ্ক, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_গুনতি, "win-count", 0, 0},
    {get_window_by_অঙ্ক, "get-win-by-num", 1, 1},
    {get_window_অঙ্ক, "get-win-num", 0, 1},
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
    স্থির vim_mz_buffer *
get_vim_curr_buffer(শূন্য)
{
    যদি (curbuf->b_mzscheme_ref == রিক্ত)
	প্রত্যাগম (vim_mz_buffer *)buffer_new(curbuf);
    অন্যথা
	প্রত্যাগম BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    স্থির vim_mz_window *
get_vim_curr_window(শূন্য)
{
    যদি (curwin->w_mzscheme_ref == রিক্ত)
	প্রত্যাগম (vim_mz_window *)window_new(curwin);
    অন্যথা
	প্রত্যাগম WINDOW_REF(curwin);
}

    স্থির শূন্য
make_modules(শূন্য)
{
    পূর্ণাঙ্ক		    i;
    Scheme_Env	    *mod = রিক্ত;
    Scheme_Object   *vimext_প্রতীক = রিক্ত;
    Scheme_Object   *closed_prim = রিক্ত;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_প্রতীক);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_প্রতীক = scheme_intern_প্রতীক("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_প্রতীক, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    প্রতি (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(prims); i++)
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
স্থির Scheme_Object *M_write = রিক্ত;
স্থির Scheme_Object *M_read = রিক্ত;
স্থির Scheme_Object *M_execute = রিক্ত;
স্থির Scheme_Object *M_delete = রিক্ত;

    স্থির শূন্য
sandbox_check(শূন্য)
{
    যদি (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    স্থির Scheme_Object *
sandbox_file_guard(পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv)
{
    যদি (!sandbox)
	প্রত্যাগম scheme_void;

    Scheme_Object *requested_access = argv[2];

    যদি (M_write == রিক্ত)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_প্রতীক("write");
	MZ_GC_CHECK();
    }
    যদি (M_read == রিক্ত)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_প্রতীক("read");
	MZ_GC_CHECK();
    }
    যদি (M_execute == রিক্ত)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_প্রতীক("execute");
	MZ_GC_CHECK();
    }
    যদি (M_delete == রিক্ত)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_প্রতীক("delete");
	MZ_GC_CHECK();
    }

    যাবত্ (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	যদি (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    প্রত্যাগম scheme_void;
}

    স্থির Scheme_Object *
sandbox_network_guard(পূর্ণাঙ্ক argc UNUSED, Scheme_Object **argv UNUSED)
{
    প্রত্যাগম scheme_void;
}
#endif

#endif
