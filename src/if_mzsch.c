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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#எனில் defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#எனில் MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#எனில் defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

வகைவரை கட்டமைப்பு
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_மதிப்பு ((buf_T *)(-1))

வகைவரை கட்டமைப்பு
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_மதிப்பு ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
வகைவரை கட்டமைப்பு
{
    Scheme_Closed_Prim	*prim;
    எழுத்து	*name;
    முழு		mina;	// arity information
    முழு		maxa;
} Vim_Prim;

வகைவரை கட்டமைப்பு
{
    எழுத்து	    *name;
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
நிலையான Scheme_Object *sandbox_file_guard(முழு, Scheme_Object **);
நிலையான Scheme_Object *sandbox_network_guard(முழு, Scheme_Object **);
நிலையான வெறுமை sandbox_check(வெறுமை);
#endif
//  Buffer-related commands
நிலையான Scheme_Object *buffer_new(buf_T *buf);
நிலையான Scheme_Object *get_buffer_by_இலக்கம்(வெறுமை *, முழு, Scheme_Object **);
நிலையான vim_mz_buffer *get_vim_curr_buffer(வெறுமை);

//  Window-related commands
நிலையான Scheme_Object *window_new(win_T *win);
நிலையான vim_mz_window *get_vim_curr_window(வெறுமை);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
நிலையான முழு vim_error_check(வெறுமை);
நிலையான முழு do_mzscheme_command(exarg_T *, வெறுமை *, Scheme_Closed_Prim *what);
நிலையான முழு startup_mzscheme(வெறுமை);
நிலையான எழுத்து *string_to_வரி(Scheme_Object *obj);
#எனில் MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#இல்லையெனில்
# define OUTPUT_LEN_TYPE long
#endif
நிலையான வெறுமை do_output(எழுத்து *mesg, OUTPUT_LEN_TYPE len);
நிலையான வெறுமை do_printf(எழுத்து *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
நிலையான வெறுமை do_flush(வெறுமை);
நிலையான Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
நிலையான Scheme_Object *extract_exn_message(Scheme_Object *v);
நிலையான Scheme_Object *do_eval(வெறுமை *, முழு noargc, Scheme_Object **noargv);
நிலையான Scheme_Object *do_load(வெறுமை *, முழு noargc, Scheme_Object **noargv);
நிலையான வெறுமை register_vim_exn(வெறுமை);
நிலையான vim_mz_buffer *get_buffer_arg(மாறிலி எழுத்து *fname, முழு argnum,
	முழு argc, Scheme_Object **argv);
நிலையான vim_mz_window *get_window_arg(மாறிலி எழுத்து *fname, முழு argnum,
	முழு argc, Scheme_Object **argv);
நிலையான முழு வரி_in_range(linenr_T, buf_T *);
நிலையான வெறுமை check_வரி_range(linenr_T, buf_T *);
நிலையான வெறுமை mz_fix_cursor(முழு lo, முழு hi, முழு extra);

நிலையான முழு eval_with_exn_handling(வெறுமை *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
நிலையான வெறுமை make_modules(வெறுமை);
நிலையான வெறுமை init_exn_catching_apply(வெறுமை);
நிலையான முழு mzscheme_env_main(Scheme_Env *env, முழு argc, எழுத்து **argv);
நிலையான முழு mzscheme_init(வெறுமை);
#ifdef FEAT_EVAL
நிலையான Scheme_Object *vim_to_mzscheme(typval_T *vim_மதிப்பு);
நிலையான Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_மதிப்பு, முழு depth,
	Scheme_Hash_Table *visited);
நிலையான முழு mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
நிலையான முழு mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, முழு depth,
	Scheme_Hash_Table *visited);
நிலையான Scheme_Object *vim_funcref(வெறுமை *data, முழு argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
நிலையான முழு buffer_size_proc(வெறுமை *obj UNUSED)
{
    திரும்பு gc_BYTES_TO_சொற்கள்(அளவு(vim_mz_buffer));
}
நிலையான முழு buffer_mark_proc(வெறுமை *obj)
{
    திரும்பு buffer_size_proc(obj);
}
நிலையான முழு buffer_fixup_proc(வெறுமை *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    திரும்பு buffer_size_proc(obj);
}
நிலையான முழு window_size_proc(வெறுமை *obj UNUSED)
{
    திரும்பு gc_BYTES_TO_சொற்கள்(அளவு(vim_mz_window));
}
நிலையான முழு window_mark_proc(வெறுமை *obj)
{
    திரும்பு window_size_proc(obj);
}
நிலையான முழு window_fixup_proc(வெறுமை *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    திரும்பு window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#இல்லையெனில்
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#எனில் defined(DYNAMIC_MZSCHEME)
நிலையான Scheme_Object *dll_scheme_eof;
நிலையான Scheme_Object *dll_scheme_false;
நிலையான Scheme_Object *dll_scheme_void;
நிலையான Scheme_Object *dll_scheme_null;
நிலையான Scheme_Object *dll_scheme_true;

நிலையான Scheme_Thread **dll_scheme_current_thread_ptr;

நிலையான வெறுமை (**dll_scheme_console_printf_ptr)(எழுத்து *str, ...);
நிலையான வெறுமை (**dll_scheme_console_output_ptr)(எழுத்து *str, OUTPUT_LEN_TYPE len);
நிலையான வெறுமை (**dll_scheme_notify_multithread_ptr)(முழு on);

நிலையான வெறுமை *(*dll_GC_malloc)(size_t size_in_bytes);
நிலையான வெறுமை *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
நிலையான Scheme_Env *(*dll_scheme_basic_env)(வெறுமை);
நிலையான வெறுமை (*dll_scheme_check_threads)(வெறுமை);
நிலையான வெறுமை (*dll_scheme_register_static)(வெறுமை *ptr, long size);
நிலையான வெறுமை (*dll_scheme_set_அடுக்கு_base)(வெறுமை *base, முழு no_auto_statics);
நிலையான வெறுமை (*dll_scheme_add_global)(மாறிலி எழுத்து *name, Scheme_Object *val,
	Scheme_Env *env);
நிலையான வெறுமை (*dll_scheme_add_global_குறியீடு)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
நிலையான Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, முழு இலக்கம்_rands,
	Scheme_Object **rands);
நிலையான Scheme_Object *(*dll_scheme_builtin_மதிப்பு)(மாறிலி எழுத்து *name);
# எனில் MZSCHEME_VERSION_MAJOR >= 299
நிலையான Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
நிலையான Scheme_Object *(*dll_scheme_make_path)(மாறிலி எழுத்து *chars);
# endif
நிலையான வெறுமை (*dll_scheme_close_உள்ளீடு_port)(Scheme_Object *port);
நிலையான வெறுமை (*dll_scheme_மொத்தம்_வரிகள்)(Scheme_Object *port);
# எனில் MZSCHEME_VERSION_MAJOR < 360
நிலையான Scheme_Object *(*dll_scheme_current_continuation_marks)(வெறுமை);
# இல்லையெனில்
நிலையான Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
நிலையான வெறுமை (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
நிலையான எழுத்து *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
நிலையான முழு (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
நிலையான Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	முழு இலக்கம்_rands_2, Scheme_Object **rands, முழு val);
நிலையான வெறுமை (*dll_scheme_dont_gc_ptr)(வெறுமை *p);
நிலையான Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
நிலையான Scheme_Object *(*dll_scheme_eval_string)(மாறிலி எழுத்து *str,
	Scheme_Env *env);
நிலையான Scheme_Object *(*dll_scheme_eval_string_all)(மாறிலி எழுத்து *str,
	Scheme_Env *env, முழு all);
நிலையான வெறுமை (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# எனில் MZSCHEME_VERSION_MAJOR < 299
நிலையான எழுத்து *(*dll_scheme_format)(எழுத்து *format, முழு flen, முழு argc,
	Scheme_Object **argv, long *rlen);
# இல்லையெனில்
நிலையான எழுத்து *(*dll_scheme_format_utf8)(எழுத்து *format, முழு flen, முழு argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
நிலையான Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, முழு pos);
# endif
நிலையான வெறுமை (*dll_scheme_gc_ptr_ok)(வெறுமை *p);
# எனில் MZSCHEME_VERSION_MAJOR < 299
நிலையான எழுத்து *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# இல்லையெனில்
நிலையான எழுத்து *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
நிலையான Scheme_Object *(*dll_scheme_intern_குறியீடு)(மாறிலி எழுத்து *name);
நிலையான Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *குறியீடு,
	Scheme_Env *env);
நிலையான Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, வெறுமை *data, மாறிலி எழுத்து *name, mzshort mina,
     mzshort maxa);
நிலையான Scheme_Object *(*dll_scheme_make_integer_மதிப்பு)(long i);
நிலையான Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
நிலையான Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	மாறிலி எழுத்து *name, mzshort mina, mzshort maxa);
# எனில் MZSCHEME_VERSION_MAJOR < 299
நிலையான Scheme_Object *(*dll_scheme_make_string)(மாறிலி எழுத்து *chars);
நிலையான Scheme_Object *(*dll_scheme_make_string_output_port)();
# இல்லையெனில்
நிலையான Scheme_Object *(*dll_scheme_make_byte_string)(மாறிலி எழுத்து *chars);
நிலையான Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
நிலையான Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	முழு argc, Scheme_Object **argv);
நிலையான Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, முழு flags, முழு *மொத்தம்_out);
நிலையான Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, முழு இலக்கம்_fields,
	முழு இலக்கம்_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# எனில் MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
நிலையான Scheme_Object **(*dll_scheme_make_struct_மதிப்புகள்)(
	Scheme_Object *struct_type, Scheme_Object **names, முழு மொத்தம்,
	முழு flags);
நிலையான Scheme_Type (*dll_scheme_make_type)(மாறிலி எழுத்து *name);
நிலையான Scheme_Object *(*dll_scheme_make_vector)(முழு size,
	Scheme_Object *fill);
நிலையான வெறுமை *(*dll_scheme_malloc_fail_ok)(வெறுமை *(*f)(size_t), size_t);
நிலையான Scheme_Object *(*dll_scheme_open_உள்ளீடு_file)(மாறிலி எழுத்து *name,
	மாறிலி எழுத்து *who);
நிலையான Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
நிலையான முழு (*dll_scheme_proper_list_length)(Scheme_Object *list);
நிலையான வெறுமை (*dll_scheme_raise)(Scheme_Object *exn);
நிலையான Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
நிலையான வெறுமை (*dll_scheme_signal_error)(மாறிலி எழுத்து *செய்தி, ...);
நிலையான வெறுமை (*dll_scheme_wrong_type)(மாறிலி எழுத்து *name, மாறிலி எழுத்து *expected,
	முழு which, முழு argc, Scheme_Object **argv);
# எனில் MZSCHEME_VERSION_MAJOR >= 299
நிலையான வெறுமை (*dll_scheme_set_param)(Scheme_Config *c, முழு pos,
	Scheme_Object *o);
நிலையான Scheme_Config *(*dll_scheme_current_config)(வெறுமை);
நிலையான Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
நிலையான Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
நிலையான வெறுமை *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
நிலையான Scheme_Hash_Table *(*dll_scheme_make_hash_table)(முழு type);
நிலையான வெறுமை (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *சாவி, Scheme_Object *மதிப்பு);
நிலையான Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *சாவி);
நிலையான Scheme_Object *(*dll_scheme_make_double)(இரட்டை d);
நிலையான Scheme_Object *(*dll_scheme_make_sized_byte_string)(எழுத்து *chars,
	long len, முழு copy);
நிலையான Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
நிலையான Scheme_Object *(*dll_scheme_dynamic_wind)(வெறுமை (*pre)(வெறுமை *), Scheme_Object *(* நிலையற்ற act)(வெறுமை *), வெறுமை (* நிலையற்ற post)(வெறுமை *), Scheme_Object *(*jmp_handler)(வெறுமை *), வெறுமை * நிலையற்ற data);
# ifdef MZ_PRECISE_GC
நிலையான வெறுமை *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
நிலையான வெறுமை (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, முழு is_constant_size, முழு is_atomic);
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 400
நிலையான வெறுமை (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
நிலையான வெறுமை **(*dll_scheme_malloc_immobile_box)(வெறுமை *p);
நிலையான வெறுமை (*dll_scheme_free_immobile_box)(வெறுமை **b);
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
நிலையான முழு (*dll_scheme_main_setup)(முழு no_auto_statics, Scheme_Env_Main _main, முழு argc, எழுத்து **argv);
#   எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
நிலையான வெறுமை (*dll_scheme_register_tls_space)(வெறுமை *tls_space, முழு _tls_index);
#   endif
#  endif
#  எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
நிலையான Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(வெறுமை);
#  endif
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 600
நிலையான வெறுமை (*dll_scheme_embedded_load)(intptr_t len, மாறிலி எழுத்து *s, முழு predefined);
நிலையான வெறுமை (*dll_scheme_register_embedded_load)(intptr_t len, மாறிலி எழுத்து *s);
நிலையான வெறுமை (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# எனில் defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  எனில் !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_குறியீடு dll_scheme_add_global_குறியீடு
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_மதிப்பு dll_scheme_builtin_மதிப்பு
#  எனில் MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_உள்ளீடு_port dll_scheme_close_உள்ளீடு_port
#  define scheme_மொத்தம்_வரிகள் dll_scheme_மொத்தம்_வரிகள்
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
#  எனில் MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  இல்லையெனில்
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  எனில் MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  இல்லையெனில்
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_குறியீடு dll_scheme_intern_குறியீடு
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_மதிப்பு dll_scheme_make_integer_மதிப்பு
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  எனில் MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  இல்லையெனில்
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_மதிப்புகள் dll_scheme_make_struct_மதிப்புகள்
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_உள்ளீடு_file dll_scheme_open_உள்ளீடு_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_அடுக்கு_base dll_scheme_set_அடுக்கு_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  எனில் MZSCHEME_VERSION_MAJOR >= 299
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
#  எனில் MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  எனில் MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   எனில் MZSCHEME_VERSION_MAJOR >= 500
#    எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(வெறுமை)
{
    திரும்பு dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

வகைவரை கட்டமைப்பு
{
    எழுத்து    *name;
    வெறுமை    **ptr;
} Thunk_Info;

நிலையான Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (வெறுமை **)&dll_GC_malloc},
    {"GC_malloc_atomic", (வெறுமை **)&dll_GC_malloc_atomic},
    {வெற்று, வெற்று}};

நிலையான Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (வெறுமை **)&dll_scheme_eof},
    {"scheme_false", (வெறுமை **)&dll_scheme_false},
    {"scheme_void", (வெறுமை **)&dll_scheme_void},
    {"scheme_null", (வெறுமை **)&dll_scheme_null},
    {"scheme_true", (வெறுமை **)&dll_scheme_true},
# எனில் !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (வெறுமை **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (வெறுமை **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (வெறுமை **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(வெறுமை **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (வெறுமை **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (வெறுமை **)&dll_scheme_add_global_குறியீடு},
    {"scheme_apply", (வெறுமை **)&dll_scheme_apply},
    {"scheme_basic_env", (வெறுமை **)&dll_scheme_basic_env},
# எனில் MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (வெறுமை **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (வெறுமை **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (வெறுமை **)&dll_scheme_builtin_மதிப்பு},
    {"scheme_check_threads", (வெறுமை **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (வெறுமை **)&dll_scheme_close_உள்ளீடு_port},
    {"scheme_count_lines", (வெறுமை **)&dll_scheme_மொத்தம்_வரிகள்},
    {"scheme_current_continuation_marks",
	(வெறுமை **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (வெறுமை **)&dll_scheme_display},
    {"scheme_display_to_string", (வெறுமை **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (வெறுமை **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (வெறுமை **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (வெறுமை **)&dll_scheme_eq},
    {"scheme_eval", (வெறுமை **)&dll_scheme_eval},
    {"scheme_eval_string", (வெறுமை **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (வெறுமை **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(வெறுமை **)&dll_scheme_finish_primitive_module},
# எனில் MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (வெறுமை **)&dll_scheme_format},
# இல்லையெனில்
    {"scheme_format_utf8", (வெறுமை **)&dll_scheme_format_utf8},
    {"scheme_get_param", (வெறுமை **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (வெறுமை **)&dll_scheme_gc_ptr_ok},
# எனில் MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(வெறுமை **)&dll_scheme_get_sized_string_output},
# இல்லையெனில்
    {"scheme_get_sized_byte_string_output",
	(வெறுமை **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (வெறுமை **)&dll_scheme_intern_குறியீடு},
    {"scheme_lookup_global", (வெறுமை **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(வெறுமை **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (வெறுமை **)&dll_scheme_make_integer_மதிப்பு},
    {"scheme_make_pair", (வெறுமை **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (வெறுமை **)&dll_scheme_make_prim_w_arity},
# எனில் MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (வெறுமை **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(வெறுமை **)&dll_scheme_make_string_output_port},
# இல்லையெனில்
    {"scheme_make_byte_string", (வெறுமை **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(வெறுமை **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(வெறுமை **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (வெறுமை **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (வெறுமை **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (வெறுமை **)&dll_scheme_make_struct_மதிப்புகள்},
    {"scheme_make_type", (வெறுமை **)&dll_scheme_make_type},
    {"scheme_make_vector", (வெறுமை **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (வெறுமை **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (வெறுமை **)&dll_scheme_open_உள்ளீடு_file},
    {"scheme_primitive_module", (வெறுமை **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (வெறுமை **)&dll_scheme_proper_list_length},
    {"scheme_raise", (வெறுமை **)&dll_scheme_raise},
    {"scheme_read", (வெறுமை **)&dll_scheme_read},
    {"scheme_register_static", (வெறுமை **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (வெறுமை **)&dll_scheme_set_அடுக்கு_base},
    {"scheme_signal_error", (வெறுமை **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (வெறுமை **)&dll_scheme_wrong_type},
# எனில் MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (வெறுமை **)&dll_scheme_set_param},
    {"scheme_current_config", (வெறுமை **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(வெறுமை **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (வெறுமை **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (வெறுமை **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (வெறுமை **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (வெறுமை **)&dll_scheme_hash_set},
    {"scheme_hash_get", (வெறுமை **)&dll_scheme_hash_get},
    {"scheme_make_double", (வெறுமை **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (வெறுமை **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (வெறுமை **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (வெறுமை **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (வெறுமை **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (வெறுமை **)&dll_GC_register_traversers},
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (வெறுமை **)&dll_scheme_main_setup},
#   எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (வெறுமை **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (வெறுமை **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (வெறுமை **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (வெறுமை **)&dll_scheme_free_immobile_box},
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 500
#  எனில் defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (வெறுமை **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# எனில் MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (வெறுமை **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (வெறுமை **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (வெறுமை **)&dll_scheme_set_config_path},
# endif
    {வெற்று, வெற்று}};

நிலையான HINSTANCE hMzGC = 0;
நிலையான HINSTANCE hMzSch = 0;

நிலையான வெறுமை dynamic_mzscheme_end(வெறுமை);
நிலையான முழு mzscheme_runtime_link_init(எழுத்து *sch_dll, எழுத்து *gc_dll,
	முழு verbose);

    நிலையான முழு
mzscheme_runtime_link_init(எழுத்து *sch_dll, எழுத்து *gc_dll, முழு verbose)
{
    Thunk_Info *thunk = வெற்று;

    எனில் (hMzGC && hMzSch)
	திரும்பு OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    எனில் (!hMzGC)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	திரும்பு FAIL;
    }

    எனில் (!hMzSch)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	திரும்பு FAIL;
    }

    க்கு (thunk = mzsch_imports; thunk->name; thunk++)
    {
	எனில் ((*thunk->ptr =
		    (வெறுமை *)GetProcAddress(hMzSch, thunk->name)) == வெற்று)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    திரும்பு FAIL;
	}
    }
    க்கு (thunk = mzgc_imports; thunk->name; thunk++)
    {
	எனில் ((*thunk->ptr =
		    (வெறுமை *)GetProcAddress(hMzGC, thunk->name)) == வெற்று)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

    முழு
mzscheme_enabled(முழு verbose)
{
    திரும்பு mzscheme_runtime_link_init(
	    (எழுத்து *)p_mzschemedll, (எழுத்து *)p_mzschemegcdll, verbose) == OK;
}

    நிலையான வெறுமை
dynamic_mzscheme_end(வெறுமை)
{
    எனில் (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    எனில் (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#எனில் MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, இலக்கம்) GUARANTEE_STRING(proc, இலக்கம்)
#இல்லையெனில்
    நிலையான Scheme_Object *
guaranteed_byte_string_arg(எழுத்து *proc, முழு இலக்கம், முழு argc, Scheme_Object **argv)
{
    எனில் (SCHEME_BYTE_STRINGP(argv[இலக்கம்]))
    {
	திரும்பு argv[இலக்கம்];
    }
    இல்லையெனில் எனில் (SCHEME_CHAR_STRINGP(argv[இலக்கம்]))
    {
	Scheme_Object *tmp = வெற்று;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[இலக்கம்]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[இலக்கம்]);
	MZ_GC_UNREG();
	திரும்பு tmp;
    }
    இல்லையெனில்
	scheme_wrong_type(proc, "string", இலக்கம், argc, argv);
    // unreachable
    திரும்பு scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, இலக்கம்) guaranteed_byte_string_arg(proc, இலக்கம், argc, argv)
#endif

// need to put it here for dynamic stuff to work
#எனில் defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

நிலையான Scheme_Type mz_buffer_type;
நிலையான Scheme_Type mz_window_type;

நிலையான முழு initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
நிலையான முழு disabled = FALSE;
#endif
நிலையான முழு load_base_module_failed = FALSE;

// global environment
நிலையான Scheme_Env    *environment = வெற்று;
// output/error handlers
நிலையான Scheme_Object *curout = வெற்று;
நிலையான Scheme_Object *curerr = வெற்று;
// exn:vim exception
நிலையான Scheme_Object *exn_catching_apply = வெற்று;
நிலையான Scheme_Object *exn_p = வெற்று;
நிலையான Scheme_Object *exn_message = வெற்று;
நிலையான Scheme_Object *vim_exn = வெற்று; // Vim Error exception

#எனில் !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
நிலையான வெறுமை *அடுக்கு_base = வெற்று;
#endif

நிலையான long range_start;
நிலையான long range_end;

// MzScheme threads scheduling stuff
நிலையான முழு mz_threads_allow = 0;

#எனில் defined(FEAT_GUI_MSWIN)
நிலையான வெறுமை CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
நிலையான UINT timer_id = 0;
#அல்லதெனில் defined(FEAT_GUI_GTK)
நிலையான gboolean timer_proc(gpointer);
நிலையான guint timer_id = 0;
#அல்லதெனில் defined(FEAT_GUI_MOTIF)
நிலையான வெறுமை timer_proc(XtPointer, XtIntervalId *);
நிலையான XtIntervalId timer_id = (XtIntervalId)0;
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    வெறுமை
mzvim_check_threads(வெறுமை)
{
    // Last time MzScheme threads were scheduled
    நிலையான time_t mz_last_time = 0;

    எனில் (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(வெற்று);

	எனில் ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#எனில் defined(MZSCHEME_GUI_THREADS)
நிலையான வெறுமை setup_timer(வெறுமை);
நிலையான வெறுமை remove_timer(வெறுமை);

// timers are presented in GUI only
# எனில் defined(FEAT_GUI_MSWIN)
    நிலையான வெறுமை CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_செய்தி UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    நிலையான gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    நிலையான வெறுமை
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# எனில் defined(FEAT_GUI_GTK)
    திரும்பு TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    எனில் (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, வெற்று);
# endif
}

    நிலையான வெறுமை
setup_timer(வெறுமை)
{
# எனில் defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(வெற்று, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, வெற்று);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, வெற்று);
# endif
}

    நிலையான வெறுமை
remove_timer(வெறுமை)
{
# எனில் defined(FEAT_GUI_MSWIN)
    KillTimer(வெற்று, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    எழுத்து *
did_set_mzquantum(optset_T *args UNUSED)
{
#எனில் defined(MZSCHEME_GUI_THREADS)
    // reset timer
    எனில் (timer_id != 0)
	remove_timer();
    எனில் (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    திரும்பு வெற்று;
}

    நிலையான வெறுமை
notify_multithread(முழு on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    எனில் (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    எனில் (!on && timer_id != 0)
	remove_timer();
#endif
}

    வெறுமை
mzscheme_end(வெறுமை)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#எனில் 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#எனில் HAVE_TLS_SPACE && !defined(VIMDLL)
# எனில் defined(_MSC_VER)
நிலையான __declspec(thread) வெறுமை *tls_space;
வெளி intptr_t _tls_index;
# elif defined(__MINGW32__)
நிலையான __thread வெறுமை *tls_space;
வெளி intptr_t _tls_index;
# இல்லையெனில்
நிலையான THREAD_LOCAL வெறுமை *tls_space;
நிலையான intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    முழு
mzscheme_main(வெறுமை)
{
    முழு	    argc = 0;
    எழுத்து    *argv = வெற்று;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    எனில் (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	திரும்பு vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    வெறுமை **ptls_space;
    intptr_t tls_index;
    வெறுமை (*pget_tls_info)(வெறுமை ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (வெறுமை *)GetProcAddress(
				GetModuleHandle(வெற்று), "get_tls_info");
    எனில் (pget_tls_info == வெற்று)
    {
	disabled = TRUE;
	திரும்பு vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# இல்லையெனில்
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    திரும்பு scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#இல்லையெனில்
    திரும்பு mzscheme_env_main(வெற்று, argc, &argv);
#endif
}

    நிலையான முழு
mzscheme_env_main(Scheme_Env *env, முழு argc UNUSED, எழுத்து **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#இல்லையெனில்
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = வெற்று;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    அடுக்கு_base = &__gc_var_stack__;
# இல்லையெனில்
    முழு dummy = 0;
    அடுக்கு_base = (வெறுமை *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    திரும்பு 0;
}

    நிலையான Scheme_Object*
load_base_module(வெறுமை *data)
{
    scheme_namespace_require(scheme_intern_குறியீடு((எழுத்து *)data));
    திரும்பு scheme_null;
}

    நிலையான Scheme_Object *
load_base_module_on_error(வெறுமை *data UNUSED)
{
    load_base_module_failed = TRUE;
    திரும்பு scheme_null;
}

    நிலையான முழு
startup_mzscheme(வெறுமை)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_அடுக்கு_base(அடுக்கு_base, 1);
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
#எனில் MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = வெற்று;
	முழு mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	எனில் (s != வெற்று)
	{
	    coll_path = scheme_make_path((எழுத்து *)s);
	    MZ_GC_CHECK();
	    எனில் (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	எனில் (coll_path == வெற்று)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	எனில் (coll_path != வெற்று)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#இல்லையெனில்
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = வெற்று;
	Scheme_Object	*coll_pair = வெற்று;
	Scheme_Config	*config = வெற்று;

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

#எனில் MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = வெற்று;
	முழு mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	எனில் (s != வெற்று)
	{
	    config_path = scheme_make_path((எழுத்து *)s);
	    MZ_GC_CHECK();
	    எனில் (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	எனில் (config_path == வெற்று)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	எனில் (config_path != வெற்று)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#எனில் MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(வெற்று, load_base_module, வெற்று,
				    load_base_module_on_error, "racket/base");
	எனில் (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(வெற்று, load_base_module, வெற்று,
				    load_base_module_on_error, "scheme/base");
	    எனில் (load_base_module_failed)
		திரும்பு -1;
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
	Scheme_Object	*make_security_guard = வெற்று;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# எனில் MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_குறியீடு = வெற்று;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_குறியீடு);
	    MZ_GC_REG();
	    make_security_guard_குறியீடு = scheme_intern_குறியீடு("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_குறியீடு, environment);
	    MZ_GC_UNREG();
	}
# இல்லையெனில்
	make_security_guard = scheme_builtin_மதிப்பு("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	எனில் (make_security_guard != வெற்று)
	{
	    Scheme_Object   *args[3] = {வெற்று, வெற்று, வெற்று};
	    Scheme_Object   *guard = வெற்று;
	    Scheme_Config   *config = வெற்று;
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

    திரும்பு 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    நிலையான முழு
mzscheme_init(வெறுமை)
{
    எனில் (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	எனில் (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    திரும்பு -1;
	}
#endif
	எனில் (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    திரும்பு -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = வெற்று;
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

    திரும்பு 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    நிலையான முழு
eval_with_exn_handling(
    வெறுமை		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *மதிப்பு = வெற்று;
    Scheme_Object   *exn = வெற்று;
    Scheme_Object   *prim = வெற்று;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, மதிப்பு);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    மதிப்பு = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    எனில் (!மதிப்பு)
    {
	மதிப்பு = extract_exn_message(exn);
	// Got an exn?
	எனில் (மதிப்பு)
	{
	    scheme_display(மதிப்பு, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	திரும்பு FAIL;
    }

    எனில் (ret != வெற்று)	// if pointer to retval supported give it up
	*ret = மதிப்பு;
    // Print any result, as long as it's not a void
    இல்லையெனில் எனில் (!SCHEME_VOIDP(மதிப்பு))
    {
	scheme_display(மதிப்பு, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    திரும்பு OK;
}

/*
 * :mzscheme
 */
    நிலையான முழு
do_mzscheme_command(exarg_T *eap, வெறுமை *data, Scheme_Closed_Prim *what)
{
    எனில் (mzscheme_init())
	திரும்பு FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    திரும்பு eval_with_exn_handling(data, what, வெற்று);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    வெறுமை
mzscheme_buffer_free(buf_T *buf)
{
    எனில் (buf->b_mzscheme_ref == வெற்று)
	திரும்பு;

    vim_mz_buffer *bp = வெற்று;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_மதிப்பு;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#இல்லையெனில்
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = வெற்று;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    வெறுமை
mzscheme_window_free(win_T *win)
{
    எனில் (win->w_mzscheme_ref == வெற்று)
	திரும்பு;

    vim_mz_window *wp = வெற்று;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_மதிப்பு;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#இல்லையெனில்
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = வெற்று;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    வெறுமை
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    எனில் (eap->skip)
	திரும்பு;

    எனில் (script == வெற்று)
	do_mzscheme_command(eap, eap->arg, do_eval);
    இல்லையெனில்
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    நிலையான Scheme_Object *
do_load(வெறுமை *data, முழு noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *கோவை = வெற்று;
    Scheme_Object   *result = வெற்று;
    எழுத்து	    *file = வெற்று;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, கோவை);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (எழுத்து *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_உள்ளீடு_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_மொத்தம்_வரிகள்(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    போது (!SCHEME_EOFP(கோவை = scheme_read(pinfo->port)))
    {
	result = scheme_eval(கோவை, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_உள்ளீடு_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = வெற்று;
    MZ_GC_UNREG();
    திரும்பு result;
}

/*
 * :mzfile
 */
    வெறுமை
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {வெற்று, வெற்று};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (எழுத்து *)eap->arg;
    எனில் (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != வெற்று)	// looks like port was not closed
    {
	scheme_close_உள்ளீடு_port(pinfo.port);
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
    நிலையான வெறுமை
init_exn_catching_apply(வெறுமை)
{
    எனில் (exn_catching_apply)
	திரும்பு;

    எழுத்து *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_மதிப்பு("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_மதிப்பு("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    நிலையான Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    எனில் (SCHEME_TRUEP(SCHEME_CAR(v)))
	திரும்பு SCHEME_CDR(v);
    இல்லையெனில்
    {
	*exn = SCHEME_CDR(v);
	திரும்பு வெற்று;
    }
}

    நிலையான Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    எனில் (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	திரும்பு _scheme_apply(exn_message, 1, &v);
    இல்லையெனில்
	திரும்பு வெற்று; // Not an exn structure
}

    நிலையான Scheme_Object *
do_eval(வெறுமை *s, முழு noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    திரும்பு scheme_eval_string_all((எழுத்து *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    நிலையான வெறுமை
do_intrnl_output(எழுத்து *mesg, முழு error)
{
    எழுத்து *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    போது (p)
    {
	*p = '\0';
	எனில் (error)
	    emsg(prev);
	இல்லையெனில்
	    செய்தி(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    எனில் (error)
	emsg(prev);
    இல்லையெனில்
	செய்தி(prev);
}

    நிலையான வெறுமை
do_output(எழுத்து *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    நிலையான வெறுமை
do_err_output(எழுத்து *mesg)
{
    do_intrnl_output(mesg, 1);
}

    நிலையான வெறுமை
do_printf(எழுத்து *format, ...)
{
    do_intrnl_output(format, 1);
}

    நிலையான வெறுமை
do_flush(வெறுமை)
{
    எழுத்து *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    எனில் (length)
    {
	do_err_output(buff);
	திரும்பு;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    எனில் (length)
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
    நிலையான Scheme_Object *
vim_command(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = வெற்று;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_மதிப்பு(cmd), வெற்று, வெற்று, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    திரும்பு scheme_void;
}

/*
 * (eval {expr-string})
 */
    நிலையான Scheme_Object *
vim_eval(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = வெற்று;
    typval_T		*vim_result;
    Scheme_Object *கோவை = வெற்று;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, கோவை);
    MZ_GC_REG();
    கோவை = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_கோவை(BYTE_STRING_மதிப்பு(கோவை), வெற்று);

    எனில் (vim_result == வெற்று)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    திரும்பு result;
#இல்லையெனில்
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    திரும்பு scheme_false;
#endif
}

/*
 * (range-start)
 */
    நிலையான Scheme_Object *
get_range_start(
    வெறுமை		*data UNUSED,
    முழு			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    திரும்பு scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    நிலையான Scheme_Object *
get_range_end(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
    திரும்பு scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    நிலையான Scheme_Object *
mzscheme_beep(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    திரும்பு scheme_void;
}

நிலையான Scheme_Object *M_global = வெற்று;

/*
 * (get-option {option-name}) [buffer/window]
 */
    நிலையான Scheme_Object *
get_option(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    மதிப்பு;
    எழுத்து	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = வெற்று;
    Scheme_Object   *name = வெற்று;
    முழு		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    எனில் (argc > 1)
    {
	எனில் (M_global == வெற்று)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_குறியீடு("global");
	    MZ_GC_CHECK();
	}

	எனில் (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	இல்லையெனில் எனில் (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	இல்லையெனில் எனில் (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	இல்லையெனில்
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_மதிப்பு(BYTE_STRING_மதிப்பு(name), &மதிப்பு, (char_u **)&strval,
								  வெற்று, scope);
    curbuf = save_curb;
    curwin = save_curw;

    தேர்வு (rc)
    {
    நிலை gov_bool:
    நிலை gov_எண்:
	MZ_GC_UNREG();
	திரும்பு scheme_make_integer_மதிப்பு(மதிப்பு);
    நிலை gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	திரும்பு rval;
    நிலை gov_hidden_bool:
    நிலை gov_hidden_எண்:
    நிலை gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    நிலை gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    திரும்பு scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    நிலையான Scheme_Object *
set_option(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    char_u	*command = வெற்று;
    முழு		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = வெற்று;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    எனில் (argc > 1)
    {
	எனில் (M_global == வெற்று)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_குறியீடு("global");
	    MZ_GC_CHECK();
	}

	எனில் (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	இல்லையெனில் எனில் (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	இல்லையெனில் எனில் (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	இல்லையெனில்
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_மதிப்பு(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    திரும்பு scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    நிலையான Scheme_Object *
get_curr_win(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
    திரும்பு (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    நிலையான Scheme_Object *
get_window_மொத்தம்(
    வெறுமை		*data UNUSED,
    முழு			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    முழு	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    திரும்பு scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    நிலையான Scheme_Object *
get_window_list(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    க்கு ( ; w != வெற்று; w = w->w_next)
	எனில் (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    திரும்பு list;
}

    நிலையான Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = வெற்று;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    எனில் (win->w_mzscheme_ref != வெற்று)
	திரும்பு (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, அளவு(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(வெற்று);
#இல்லையெனில்
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    திரும்பு (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    நிலையான Scheme_Object *
get_window_இலக்கம்(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
    முழு		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    க்கு (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    திரும்பு scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    நிலையான Scheme_Object *
get_window_by_இலக்கம்(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    முழு		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    எனில் (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    க்கு ( ; win != வெற்று; win = win->w_next, --fnum)
	எனில் (fnum == 1)	    // to be 1-based
	    திரும்பு window_new(win);

    திரும்பு scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    நிலையான Scheme_Object *
get_window_buffer(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    திரும்பு buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    நிலையான Scheme_Object *
get_window_height(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    திரும்பு scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    நிலையான Scheme_Object *
set_window_height(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    முழு		    height;

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
    திரும்பு scheme_void;
}

/*
 * (get-win-width [window])
 */
    நிலையான Scheme_Object *
get_window_width(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    திரும்பு scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    நிலையான Scheme_Object *
set_window_width(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    முழு		    width = 0;

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
    திரும்பு scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    நிலையான Scheme_Object *
get_cursor(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    திரும்பு scheme_make_pair(scheme_make_integer_மதிப்பு((long)pos.lnum),
		    scheme_make_integer_மதிப்பு((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    நிலையான Scheme_Object *
set_cursor(வெறுமை *data, முழு argc, Scheme_Object **argv)
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

    எனில் (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_வரி_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    திரும்பு scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    நிலையான Scheme_Object *
mzscheme_open_buffer(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    முழு		    இலக்கம் = 0;
    Scheme_Object   *onum = வெற்று;
    Scheme_Object   *buf = வெற்று;
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
    இலக்கம் = buflist_add(BYTE_STRING_மதிப்பு(fname), BLN_LISTED | BLN_CURBUF);

    எனில் (இலக்கம் == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(இலக்கம்);
    buf = get_buffer_by_இலக்கம்(data, 1, &onum);
    MZ_GC_UNREG();
    திரும்பு buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    நிலையான Scheme_Object *
get_buffer_by_இலக்கம்(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    முழு		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	எனில் (buf->b_fnum == fnum)
	    திரும்பு buffer_new(buf);

    திரும்பு scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    நிலையான Scheme_Object *
get_buffer_by_name(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = வெற்று;
    Scheme_Object *fname = வெற்று;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	எனில் (buf->b_ffname == வெற்று || buf->b_sfname == வெற்று)
	    // empty string
	{
	    எனில் (BYTE_STRING_மதிப்பு(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	இல்லையெனில் எனில் (!fnamecmp(buf->b_ffname, BYTE_STRING_மதிப்பு(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_மதிப்பு(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    திரும்பு buffer;
}

/*
 * (get-next-buff [buffer])
 */
    நிலையான Scheme_Object *
get_next_buffer(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    எனில் (buf->b_next == வெற்று)
	திரும்பு scheme_false;
    இல்லையெனில்
	திரும்பு buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    நிலையான Scheme_Object *
get_prev_buffer(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    எனில் (buf->b_prev == வெற்று)
	திரும்பு scheme_false;
    இல்லையெனில்
	திரும்பு buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    நிலையான Scheme_Object *
get_buffer_இலக்கம்(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    திரும்பு scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    நிலையான Scheme_Object *
get_buffer_மொத்தம்(
    வெறுமை		*data UNUSED,
    முழு			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    முழு	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    திரும்பு scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    நிலையான Scheme_Object *
get_buffer_name(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    திரும்பு scheme_make_byte_string((எழுத்து *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    நிலையான Scheme_Object *
get_curr_buffer(
    வெறுமை		*data UNUSED,
    முழு			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    திரும்பு (Scheme_Object *)get_vim_curr_buffer();
}

    நிலையான Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = வெற்று;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    எனில் (buf->b_mzscheme_ref)
	திரும்பு (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, அளவு(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(வெற்று);
#இல்லையெனில்
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    திரும்பு (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    நிலையான Scheme_Object *
get_buffer_size(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    திரும்பு scheme_make_integer(buf->buf->b_ml.ml_வரி_மொத்தம்);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    நிலையான Scheme_Object *
get_buffer_வரி(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    முழு		    linenr;
    char_u	    *வரி;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    வரி = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    திரும்பு scheme_make_byte_string((எழுத்து *)வரி);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    நிலையான Scheme_Object *
get_buffer_வரி_list(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    முழு		    i, hi, lo, n;
    Scheme_Object   *list = வெற்று;

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
    எனில் (lo < 0)
	lo = 0;

    எனில் (hi < 0)
	hi = 0;
    எனில் (hi < lo)
	hi = lo;

    n = hi - lo;

    க்கு (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (எழுத்து *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    திரும்பு list;
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
    நிலையான Scheme_Object *
set_buffer_வரி(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *வரி = வெற்று;
    எழுத்து	    *save;
    முழு		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, வரி);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    எனில் (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    வரி = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_வரி_range(n, buf->buf);

    எனில் (SCHEME_FALSEP(வரி))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	எனில் (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	இல்லையெனில் எனில் (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	எனில் (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_வரிகள்_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	திரும்பு scheme_void;
    }
    இல்லையெனில்
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_வரி(வரி);

	curbuf = buf->buf;

	எனில் (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	இல்லையெனில் எனில் (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	இல்லையெனில்
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	எனில் (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	திரும்பு scheme_void;
    }
}

    நிலையான வெறுமை
free_array(எழுத்து **array)
{
    எழுத்து **curr = array;
    போது (*curr != வெற்று)
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
    நிலையான Scheme_Object *
set_buffer_வரி_list(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = வெற்று;
    Scheme_Object   *வரி_list = வெற்று;
    முழு		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, வரி_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    எனில் (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    வரி_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    எனில் (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_வரி_range(lo, buf->buf);	    // inclusive
    check_வரி_range(hi - 1, buf->buf);	    // exclusive

    எனில் (SCHEME_FALSEP(வரி_list) || SCHEME_NULLP(வரி_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	எனில் (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	இல்லையெனில்
	{
	    க்கு (i = 0; i < old_len; i++)
		எனில் (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    எனில் (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_வரிகள்_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	திரும்பு scheme_void;
    }
    இல்லையெனில்
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(வரி_list);
	MZ_GC_CHECK();
	எனில் (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	இல்லையெனில்
	{
	    எழுத்து		**array = வெற்று;
	    Scheme_Object   *வரி = வெற்று;
	    Scheme_Object   *rest = வெற்று;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, வரி);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(எழுத்து *, new_len + 1);

	    rest = வரி_list;
	    க்கு (i = 0; i < new_len; ++i)
	    {
		வரி = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		எனில் (!SCHEME_STRINGP(வரி))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_வரி(வரி);
	    }

	    curbuf = buf->buf;

	    எனில் (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    க்கு (i = 0; i < old_len - new_len; ++i)
	    {
		எனில் (ml_delete((linenr_T)lo) == FAIL)
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
	    க்கு (i = 0; i < old_len && i < new_len; i++)
		எனில் (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    போது (i < new_len)
	    {
		எனில் (ml_append((linenr_T)(lo + i - 1),
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
	changed_வரிகள்((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	எனில் (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	திரும்பு scheme_void;
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
    நிலையான Scheme_Object *
செருகு_buffer_வரி_list(வெறுமை *data, முழு argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = வெற்று;
    Scheme_Object   *list = வெற்று;
    எழுத்து	    *str = வெற்று;
    முழு		    i, n, size;

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

    எனில் (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    எனில் (n != 0)	    // 0 can be used in insert
	check_வரி_range(n, buf->buf);
    எனில் (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_வரி(list);
	curbuf = buf->buf;

	எனில் (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	இல்லையெனில் எனில் (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	இல்லையெனில்
	{
	    vim_free(str);
	    appended_வரிகள்_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	திரும்பு scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    எனில் (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    இல்லையெனில்
    {
	Scheme_Object   *வரி = வெற்று;
	Scheme_Object   *rest = வெற்று;
	எழுத்து		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, வரி);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(எழுத்து *, size + 1);

	rest = list;
	க்கு (i = 0; i < size; ++i)
	{
	    வரி = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_வரி(வரி);
	}

	curbuf = buf->buf;

	எனில் (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	இல்லையெனில்
	{
	    க்கு (i = 0; i < size; ++i)
		எனில் (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    எனில் (i > 0)
		appended_வரிகள்_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    திரும்பு scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    நிலையான Scheme_Object *
vim_bufferp(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv)
{
    எனில் (SCHEME_VIMBUFFERP(argv[0]))
	திரும்பு scheme_true;
    இல்லையெனில்
	திரும்பு scheme_false;
}

/*
 * (win? obj)
 */
    நிலையான Scheme_Object *
vim_windowp(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv)
{
    எனில் (SCHEME_VIMWINDOWP(argv[0]))
	திரும்பு scheme_true;
    இல்லையெனில்
	திரும்பு scheme_false;
}

/*
 * (buff-valid? obj)
 */
    நிலையான Scheme_Object *
vim_buffer_validp(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv)
{
    எனில் (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_மதிப்பு)
	திரும்பு scheme_true;
    இல்லையெனில்
	திரும்பு scheme_false;
}

/*
 * (win-valid? obj)
 */
    நிலையான Scheme_Object *
vim_window_validp(வெறுமை *data UNUSED, முழு argc UNUSED, Scheme_Object **argv)
{
    எனில் (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_மதிப்பு)
	திரும்பு scheme_true;
    இல்லையெனில்
	திரும்பு scheme_false;
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
    நிலையான எழுத்து *
string_to_வரி(Scheme_Object *obj)
{
    எழுத்து	*scheme_str = வெற்று;
    எழுத்து	*vim_str = வெற்று;
    OUTPUT_LEN_TYPE	len;
    முழு		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    எனில் (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    க்கு (i = 0; i < len; ++i)
    {
	எனில் (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	இல்லையெனில்
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    திரும்பு vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    நிலையான Scheme_Object *
vim_to_mzscheme(typval_T *vim_மதிப்பு)
{
    Scheme_Object	*result = வெற்று;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = வெற்று;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_மதிப்பு, 1, visited);

    MZ_GC_UNREG();
    திரும்பு result;
}

    நிலையான Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_மதிப்பு,
    முழு			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = வெற்று;
    முழு		    new_மதிப்பு_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    எனில் (depth > 100)
    {
	MZ_GC_UNREG();
	திரும்பு scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_மதிப்பு);
    MZ_GC_CHECK();
    எனில் (result != வெற்று) // found, do nothing
	new_மதிப்பு_2 = FALSE;
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((எழுத்து *)vim_மதிப்பு->vval.v_string);
	MZ_GC_CHECK();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_எண்)
    {
	result = scheme_make_integer((long)vim_மதிப்பு->vval.v_எண்);
	MZ_GC_CHECK();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((இரட்டை)vim_மதிப்பு->vval.v_float);
	MZ_GC_CHECK();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_LIST)
    {
	list_T		*list = vim_மதிப்பு->vval.v_list;
	listitem_T	*curr;

	எனில் (list == வெற்று || list->lv_first == வெற்று)
	    result = scheme_null;
	இல்லையெனில்
	{
	    Scheme_Object   *obj = வெற்று;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    போது (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_DICT)
    {
	Scheme_Object	  *சாவி = வெற்று;
	Scheme_Object	  *obj = வெற்று;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, சாவி);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	எனில் (vim_மதிப்பு->vval.v_dict != வெற்று)
	{
	    hashtab_T	*ht = &vim_மதிப்பு->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
	    {
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    சாவி = scheme_make_byte_string((எழுத்து *)hi->hi_சாவி);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, சாவி, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = வெற்று;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((எழுத்து *)vim_மதிப்பு->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(மாறிலி எழுத்து *)BYTE_STRING_மதிப்பு(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_PARTIAL)
    {
	எனில் (vim_மதிப்பு->vval.v_partial == வெற்று)
	    result = scheme_null;
	இல்லையெனில்
	{
	    Scheme_Object *funcname = வெற்று;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (எழுத்து *)partial_name(vim_மதிப்பு->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (மாறிலி எழுத்து *)BYTE_STRING_மதிப்பு(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    இல்லையெனில் எனில் (vim_மதிப்பு->v_type == VAR_BOOL || vim_மதிப்பு->v_type == VAR_SPECIAL)
    {
	எனில் (vim_மதிப்பு->vval.v_எண் <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_மதிப்பு->vval.v_எண்);
	இல்லையெனில்
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    இல்லையெனில்
    {
	result = scheme_void;
	new_மதிப்பு_2 = FALSE;
    }
    எனில் (new_மதிப்பு_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_மதிப்பு, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    திரும்பு result;
}

    நிலையான முழு
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    முழு i, status;
    Scheme_Hash_Table	*visited = வெற்று;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    க்கு (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	எனில் (visited->vals[i] != வெற்று)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    திரும்பு status;
}
    நிலையான முழு
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, முழு depth,
	Scheme_Hash_Table *visited)
{
    முழு		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    எனில் (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = 0;
	திரும்பு FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    எனில் (found != வெற்று)
	copy_tv(found, tv);
    இல்லையெனில் எனில் (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_எண் = VVAL_NULL;
    }
    இல்லையெனில் எனில் (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = SCHEME_INT_VAL(obj);
    }
    இல்லையெனில் எனில் (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_எண் = SCHEME_TRUEP(obj);
    }
    இல்லையெனில் எனில் (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    இல்லையெனில் எனில் (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_மதிப்பு(obj));
    }
# எனில் MZSCHEME_VERSION_MAJOR >= 299
    இல்லையெனில் எனில் (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = வெற்று;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_மதிப்பு(tmp));
	MZ_GC_UNREG();
    }
# endif
    இல்லையெனில் எனில் (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	எனில் (list == வெற்று)
	    status = FAIL;
	இல்லையெனில்
	{
	    முழு		    i;
	    Scheme_Object   *curr = வெற்று;
	    Scheme_Object   *cval = வெற்று;
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
	    எனில் (v == வெற்று)
		status = FAIL;
	    இல்லையெனில்
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		எனில் (SCHEME_VECTORP(obj))
		{
		    க்கு (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			எனில் (status == FAIL)
			    நிறுத்து;
			status = list_append_tv(list, v);
			clear_tv(v);
			எனில் (status == FAIL)
			    நிறுத்து;
		    }
		}
		இல்லையெனில் எனில் (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    க்கு (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			எனில் (status == FAIL)
			    நிறுத்து;
			status = list_append_tv(list, v);
			clear_tv(v);
			எனில் (status == FAIL)
			    நிறுத்து;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    எனில் (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			எனில் (status == OK)
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
    இல்லையெனில் எனில் (SCHEME_HASHTP(obj))
    {
	முழு		i;
	dict_T		*dict;
	Scheme_Object   *சாவி = வெற்று;
	Scheme_Object   *val = வெற்று;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, சாவி);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	எனில் (dict == வெற்று)
	    status = FAIL;
	இல்லையெனில்
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    க்கு (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		எனில் (((Scheme_Hash_Table *) obj)->vals[i] != வெற்று)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_வரி(
				((Scheme_Hash_Table *) obj)->சாவிகள்[i]));
		    // convert Scheme val to Vim and add it to the dict
		    எனில் (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			நிறுத்து;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    இல்லையெனில்
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_வரி(obj);
    }
    MZ_GC_UNREG();
    திரும்பு status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    நிலையான Scheme_Object *
vim_funcref(வெறுமை *name, முழு argc, Scheme_Object **argv)
{
    முழு i;
    typval_T args;
    முழு status = OK;
    Scheme_Object *result = வெற்று;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    எனில் (list == வெற்று)
	status = FAIL;
    இல்லையெனில்
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	க்கு (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    எனில் (v == வெற்று)
		status = FAIL;
	    இல்லையெனில்
	    {
		status = mzscheme_to_vim(argv[i], v);
		எனில் (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	எனில் (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_மதிப்பு((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    எனில் (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    இல்லையெனில்
	raise_if_error();
    திரும்பு result;
}

    வெறுமை
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = வெற்று;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    எனில் (mzscheme_init())
    {
	MZ_GC_UNREG();
	திரும்பு;
    }

    MZ_GC_CHECK();
    எனில் (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    நிலையான முழு
vim_error_check(வெறுமை)
{
    திரும்பு (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    நிலையான வெறுமை
register_vim_exn(வெறுமை)
{
    முழு	nc = 0;
    முழு i;
    Scheme_Object   *struct_exn = வெற்று;
    Scheme_Object   *exn_name = வெற்று;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_குறியீடு("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_மதிப்பு("struct:exn");
    MZ_GC_CHECK();

    எனில் (vim_exn == வெற்று)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, வெற்று, 0, 0, வெற்று, வெற்று
#எனில் MZSCHEME_VERSION_MAJOR >= 299
		, வெற்று
#endif
		);


    {
	Scheme_Object   **tmp = வெற்று;
	Scheme_Object   *exn_names[5] = {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};
	Scheme_Object   *exn_மதிப்புகள்[5] = {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_மதிப்புகள், 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * அளவு(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_மதிப்புகள்(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_மதிப்புகள், tmp, nc * அளவு(Scheme_Object *));
	MZ_GC_CHECK();

	க்கு (i = 0; i < nc; i++)
	{
	    scheme_add_global_குறியீடு(exn_names[i],
		    exn_மதிப்புகள்[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    வெறுமை
raise_vim_exn(மாறிலி எழுத்து *add_info)
{
    எழுத்து	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {வெற்று, வெற்று};
    Scheme_Object   *exn = வெற்று;
    Scheme_Object   *byte_string = வெற்று;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    எனில் (add_info != வெற்று)
    {
	எழுத்து		*c_string = வெற்று;
	Scheme_Object   *info = வெற்று;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (முழு)STRLEN(fmt), 1, &info, வெற்று);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    இல்லையெனில்
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#எனில் MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#இல்லையெனில்
    argv[1] = scheme_current_continuation_marks(வெற்று);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    வெறுமை
raise_if_error(வெறுமை)
{
    எனில் (vim_error_check())
	raise_vim_exn(வெற்று);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    நிலையான vim_mz_buffer *
get_buffer_arg(மாறிலி எழுத்து *fname, முழு argnum, முழு argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    எனில் (argc < argnum + 1)
	திரும்பு get_vim_curr_buffer();
    எனில் (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (வெறுமை)get_valid_buffer(argv[argnum]);
    திரும்பு b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    நிலையான vim_mz_window *
get_window_arg(மாறிலி எழுத்து *fname, முழு argnum, முழு argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    எனில் (argc < argnum + 1)
	திரும்பு get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    எனில் (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (வெறுமை)get_valid_window(argv[argnum]);
    திரும்பு w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(வெறுமை *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    எனில் (buf == INVALID_BUFFER_மதிப்பு)
	scheme_signal_error(_("buffer is invalid"));
    திரும்பு buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(வெறுமை *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    எனில் (win == INVALID_WINDOW_மதிப்பு)
	scheme_signal_error(_("window is invalid"));
    திரும்பு win;
}

    முழு
mzthreads_allowed(வெறுமை)
{
    திரும்பு mz_threads_allow;
}

    நிலையான முழு
வரி_in_range(linenr_T lnum, buf_T *buf)
{
    திரும்பு (lnum > 0 && lnum <= buf->b_ml.ml_வரி_மொத்தம்);
}

    நிலையான வெறுமை
check_வரி_range(linenr_T lnum, buf_T *buf)
{
    எனில் (!வரி_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    நிலையான வெறுமை
mz_fix_cursor(முழு lo, முழு hi, முழு extra)
{
    எனில் (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	எனில் (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	இல்லையெனில் எனில் (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	இல்லையெனில்
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

நிலையான Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_வரி, "get-buff-line", 1, 2},
    {set_buffer_வரி, "set-buff-line", 2, 3},
    {get_buffer_வரி_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_இலக்கம், "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_வரி_list, "set-buff-line-list", 3, 4},
    {செருகு_buffer_வரி_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_மொத்தம், "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_இலக்கம், "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_மொத்தம், "win-count", 0, 0},
    {get_window_by_இலக்கம், "get-win-by-num", 1, 1},
    {get_window_இலக்கம், "get-win-num", 0, 1},
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
    நிலையான vim_mz_buffer *
get_vim_curr_buffer(வெறுமை)
{
    எனில் (curbuf->b_mzscheme_ref == வெற்று)
	திரும்பு (vim_mz_buffer *)buffer_new(curbuf);
    இல்லையெனில்
	திரும்பு BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    நிலையான vim_mz_window *
get_vim_curr_window(வெறுமை)
{
    எனில் (curwin->w_mzscheme_ref == வெற்று)
	திரும்பு (vim_mz_window *)window_new(curwin);
    இல்லையெனில்
	திரும்பு WINDOW_REF(curwin);
}

    நிலையான வெறுமை
make_modules(வெறுமை)
{
    முழு		    i;
    Scheme_Env	    *mod = வெற்று;
    Scheme_Object   *vimext_குறியீடு = வெற்று;
    Scheme_Object   *closed_prim = வெற்று;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_குறியீடு);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_குறியீடு = scheme_intern_குறியீடு("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_குறியீடு, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(prims); i++)
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
நிலையான Scheme_Object *M_write = வெற்று;
நிலையான Scheme_Object *M_read = வெற்று;
நிலையான Scheme_Object *M_execute = வெற்று;
நிலையான Scheme_Object *M_delete = வெற்று;

    நிலையான வெறுமை
sandbox_check(வெறுமை)
{
    எனில் (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    நிலையான Scheme_Object *
sandbox_file_guard(முழு argc UNUSED, Scheme_Object **argv)
{
    எனில் (!sandbox)
	திரும்பு scheme_void;

    Scheme_Object *requested_access = argv[2];

    எனில் (M_write == வெற்று)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_குறியீடு("write");
	MZ_GC_CHECK();
    }
    எனில் (M_read == வெற்று)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_குறியீடு("read");
	MZ_GC_CHECK();
    }
    எனில் (M_execute == வெற்று)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_குறியீடு("execute");
	MZ_GC_CHECK();
    }
    எனில் (M_delete == வெற்று)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_குறியீடு("delete");
	MZ_GC_CHECK();
    }

    போது (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	எனில் (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    திரும்பு scheme_void;
}

    நிலையான Scheme_Object *
sandbox_network_guard(முழு argc UNUSED, Scheme_Object **argv UNUSED)
{
    திரும்பு scheme_void;
}
#endif

#endif
