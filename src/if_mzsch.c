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

#अन्तर्भाव "vim.h"

#अन्तर्भाव "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#यदि defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#यदि MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#यदि defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

प्रकारनिर्देश संरचना
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_मूल्य ((buf_T *)(-1))

प्रकारनिर्देश संरचना
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_मूल्य ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
प्रकारनिर्देश संरचना
{
    Scheme_Closed_Prim	*prim;
    वर्ण	*name;
    पूर्णाङ्क		mina;	// arity information
    पूर्णाङ्क		maxa;
} Vim_Prim;

प्रकारनिर्देश संरचना
{
    वर्ण	    *name;
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
स्थिर Scheme_Object *sandbox_file_guard(पूर्णाङ्क, Scheme_Object **);
स्थिर Scheme_Object *sandbox_network_guard(पूर्णाङ्क, Scheme_Object **);
स्थिर शून्य sandbox_check(शून्य);
#endif
//  Buffer-related commands
स्थिर Scheme_Object *buffer_new(buf_T *buf);
स्थिर Scheme_Object *get_buffer_by_अंक(शून्य *, पूर्णाङ्क, Scheme_Object **);
स्थिर vim_mz_buffer *get_vim_curr_buffer(शून्य);

//  Window-related commands
स्थिर Scheme_Object *window_new(win_T *win);
स्थिर vim_mz_window *get_vim_curr_window(शून्य);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
स्थिर पूर्णाङ्क vim_error_check(शून्य);
स्थिर पूर्णाङ्क do_mzscheme_command(exarg_T *, शून्य *, Scheme_Closed_Prim *what);
स्थिर पूर्णाङ्क startup_mzscheme(शून्य);
स्थिर वर्ण *string_to_पंक्ति(Scheme_Object *obj);
#यदि MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#अन्यथा
# define OUTPUT_LEN_TYPE दीर्घ
#endif
स्थिर शून्य do_output(वर्ण *mesg, OUTPUT_LEN_TYPE len);
स्थिर शून्य do_printf(वर्ण *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
स्थिर शून्य do_flush(शून्य);
स्थिर Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
स्थिर Scheme_Object *extract_exn_message(Scheme_Object *v);
स्थिर Scheme_Object *do_eval(शून्य *, पूर्णाङ्क noargc, Scheme_Object **noargv);
स्थिर Scheme_Object *do_load(शून्य *, पूर्णाङ्क noargc, Scheme_Object **noargv);
स्थिर शून्य register_vim_exn(शून्य);
स्थिर vim_mz_buffer *get_buffer_arg(ध्रुव वर्ण *fname, पूर्णाङ्क argnum,
	पूर्णाङ्क argc, Scheme_Object **argv);
स्थिर vim_mz_window *get_window_arg(ध्रुव वर्ण *fname, पूर्णाङ्क argnum,
	पूर्णाङ्क argc, Scheme_Object **argv);
स्थिर पूर्णाङ्क पंक्ति_in_range(linenr_T, buf_T *);
स्थिर शून्य check_पंक्ति_range(linenr_T, buf_T *);
स्थिर शून्य mz_fix_cursor(पूर्णाङ्क lo, पूर्णाङ्क hi, पूर्णाङ्क extra);

स्थिर पूर्णाङ्क eval_with_exn_handling(शून्य *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
स्थिर शून्य make_modules(शून्य);
स्थिर शून्य init_exn_catching_apply(शून्य);
स्थिर पूर्णाङ्क mzscheme_env_main(Scheme_Env *env, पूर्णाङ्क argc, वर्ण **argv);
स्थिर पूर्णाङ्क mzscheme_init(शून्य);
#ifdef FEAT_EVAL
स्थिर Scheme_Object *vim_to_mzscheme(typval_T *vim_मूल्य);
स्थिर Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_मूल्य, पूर्णाङ्क depth,
	Scheme_Hash_Table *visited);
स्थिर पूर्णाङ्क mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
स्थिर पूर्णाङ्क mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, पूर्णाङ्क depth,
	Scheme_Hash_Table *visited);
स्थिर Scheme_Object *vim_funcref(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
स्थिर पूर्णाङ्क buffer_size_proc(शून्य *obj UNUSED)
{
    प्रत्यागम gc_BYTES_TO_शब्दगण(परिमाण(vim_mz_buffer));
}
स्थिर पूर्णाङ्क buffer_mark_proc(शून्य *obj)
{
    प्रत्यागम buffer_size_proc(obj);
}
स्थिर पूर्णाङ्क buffer_fixup_proc(शून्य *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    प्रत्यागम buffer_size_proc(obj);
}
स्थिर पूर्णाङ्क window_size_proc(शून्य *obj UNUSED)
{
    प्रत्यागम gc_BYTES_TO_शब्दगण(परिमाण(vim_mz_window));
}
स्थिर पूर्णाङ्क window_mark_proc(शून्य *obj)
{
    प्रत्यागम window_size_proc(obj);
}
स्थिर पूर्णाङ्क window_fixup_proc(शून्य *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    प्रत्यागम window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#अन्यथा
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#यदि defined(DYNAMIC_MZSCHEME)
स्थिर Scheme_Object *dll_scheme_eof;
स्थिर Scheme_Object *dll_scheme_false;
स्थिर Scheme_Object *dll_scheme_void;
स्थिर Scheme_Object *dll_scheme_null;
स्थिर Scheme_Object *dll_scheme_true;

स्थिर Scheme_Thread **dll_scheme_current_thread_ptr;

स्थिर शून्य (**dll_scheme_console_printf_ptr)(वर्ण *str, ...);
स्थिर शून्य (**dll_scheme_console_output_ptr)(वर्ण *str, OUTPUT_LEN_TYPE len);
स्थिर शून्य (**dll_scheme_notify_multithread_ptr)(पूर्णाङ्क on);

स्थिर शून्य *(*dll_GC_malloc)(माप_प्रकार size_in_bytes);
स्थिर शून्य *(*dll_GC_malloc_atomic)(माप_प्रकार size_in_bytes);
स्थिर Scheme_Env *(*dll_scheme_basic_env)(शून्य);
स्थिर शून्य (*dll_scheme_check_threads)(शून्य);
स्थिर शून्य (*dll_scheme_register_static)(शून्य *ptr, दीर्घ size);
स्थिर शून्य (*dll_scheme_set_ढेर_base)(शून्य *base, पूर्णाङ्क no_auto_statics);
स्थिर शून्य (*dll_scheme_add_global)(ध्रुव वर्ण *name, Scheme_Object *val,
	Scheme_Env *env);
स्थिर शून्य (*dll_scheme_add_global_प्रतीक)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
स्थिर Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, पूर्णाङ्क अंक_rands,
	Scheme_Object **rands);
स्थिर Scheme_Object *(*dll_scheme_builtin_मूल्य)(ध्रुव वर्ण *name);
# यदि MZSCHEME_VERSION_MAJOR >= 299
स्थिर Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
स्थिर Scheme_Object *(*dll_scheme_make_path)(ध्रुव वर्ण *chars);
# endif
स्थिर शून्य (*dll_scheme_close_निवेश_port)(Scheme_Object *port);
स्थिर शून्य (*dll_scheme_गिनती_पंक्तियाँ)(Scheme_Object *port);
# यदि MZSCHEME_VERSION_MAJOR < 360
स्थिर Scheme_Object *(*dll_scheme_current_continuation_marks)(शून्य);
# अन्यथा
स्थिर Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
स्थिर शून्य (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
स्थिर वर्ण *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
स्थिर पूर्णाङ्क (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
स्थिर Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	पूर्णाङ्क अंक_rands_2, Scheme_Object **rands, पूर्णाङ्क val);
स्थिर शून्य (*dll_scheme_dont_gc_ptr)(शून्य *p);
स्थिर Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
स्थिर Scheme_Object *(*dll_scheme_eval_string)(ध्रुव वर्ण *str,
	Scheme_Env *env);
स्थिर Scheme_Object *(*dll_scheme_eval_string_all)(ध्रुव वर्ण *str,
	Scheme_Env *env, पूर्णाङ्क all);
स्थिर शून्य (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# यदि MZSCHEME_VERSION_MAJOR < 299
स्थिर वर्ण *(*dll_scheme_format)(वर्ण *format, पूर्णाङ्क flen, पूर्णाङ्क argc,
	Scheme_Object **argv, दीर्घ *rlen);
# अन्यथा
स्थिर वर्ण *(*dll_scheme_format_utf8)(वर्ण *format, पूर्णाङ्क flen, पूर्णाङ्क argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
स्थिर Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, पूर्णाङ्क pos);
# endif
स्थिर शून्य (*dll_scheme_gc_ptr_ok)(शून्य *p);
# यदि MZSCHEME_VERSION_MAJOR < 299
स्थिर वर्ण *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	दीर्घ *len);
# अन्यथा
स्थिर वर्ण *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
स्थिर Scheme_Object *(*dll_scheme_intern_प्रतीक)(ध्रुव वर्ण *name);
स्थिर Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *प्रतीक,
	Scheme_Env *env);
स्थिर Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, शून्य *data, ध्रुव वर्ण *name, mzshort mina,
     mzshort maxa);
स्थिर Scheme_Object *(*dll_scheme_make_integer_मूल्य)(दीर्घ i);
स्थिर Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
स्थिर Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ध्रुव वर्ण *name, mzshort mina, mzshort maxa);
# यदि MZSCHEME_VERSION_MAJOR < 299
स्थिर Scheme_Object *(*dll_scheme_make_string)(ध्रुव वर्ण *chars);
स्थिर Scheme_Object *(*dll_scheme_make_string_output_port)();
# अन्यथा
स्थिर Scheme_Object *(*dll_scheme_make_byte_string)(ध्रुव वर्ण *chars);
स्थिर Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
स्थिर Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	पूर्णाङ्क argc, Scheme_Object **argv);
स्थिर Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, पूर्णाङ्क flags, पूर्णाङ्क *गिनती_out);
स्थिर Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, पूर्णाङ्क अंक_fields,
	पूर्णाङ्क अंक_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# यदि MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
स्थिर Scheme_Object **(*dll_scheme_make_struct_मूल्यावली)(
	Scheme_Object *struct_type, Scheme_Object **names, पूर्णाङ्क गिनती,
	पूर्णाङ्क flags);
स्थिर Scheme_Type (*dll_scheme_make_type)(ध्रुव वर्ण *name);
स्थिर Scheme_Object *(*dll_scheme_make_vector)(पूर्णाङ्क size,
	Scheme_Object *fill);
स्थिर शून्य *(*dll_scheme_malloc_fail_ok)(शून्य *(*f)(माप_प्रकार), माप_प्रकार);
स्थिर Scheme_Object *(*dll_scheme_open_निवेश_file)(ध्रुव वर्ण *name,
	ध्रुव वर्ण *who);
स्थिर Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
स्थिर पूर्णाङ्क (*dll_scheme_proper_list_length)(Scheme_Object *list);
स्थिर शून्य (*dll_scheme_raise)(Scheme_Object *exn);
स्थिर Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
स्थिर शून्य (*dll_scheme_signal_error)(ध्रुव वर्ण *संदेश, ...);
स्थिर शून्य (*dll_scheme_wrong_type)(ध्रुव वर्ण *name, ध्रुव वर्ण *expected,
	पूर्णाङ्क which, पूर्णाङ्क argc, Scheme_Object **argv);
# यदि MZSCHEME_VERSION_MAJOR >= 299
स्थिर शून्य (*dll_scheme_set_param)(Scheme_Config *c, पूर्णाङ्क pos,
	Scheme_Object *o);
स्थिर Scheme_Config *(*dll_scheme_current_config)(शून्य);
स्थिर Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
स्थिर Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
स्थिर शून्य *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
स्थिर Scheme_Hash_Table *(*dll_scheme_make_hash_table)(पूर्णाङ्क type);
स्थिर शून्य (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *कुंजी, Scheme_Object *मूल्य);
स्थिर Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *कुंजी);
स्थिर Scheme_Object *(*dll_scheme_make_double)(द्विगुण d);
स्थिर Scheme_Object *(*dll_scheme_make_sized_byte_string)(वर्ण *chars,
	दीर्घ len, पूर्णाङ्क copy);
स्थिर Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
स्थिर Scheme_Object *(*dll_scheme_dynamic_wind)(शून्य (*pre)(शून्य *), Scheme_Object *(* चञ्चल act)(शून्य *), शून्य (* चञ्चल post)(शून्य *), Scheme_Object *(*jmp_handler)(शून्य *), शून्य * चञ्चल data);
# ifdef MZ_PRECISE_GC
स्थिर शून्य *(*dll_GC_malloc_one_tagged)(माप_प्रकार size_in_bytes);
स्थिर शून्य (*dll_GC_register_traversers)(लघु tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, पूर्णाङ्क is_constant_size, पूर्णाङ्क is_atomic);
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 400
स्थिर शून्य (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
स्थिर शून्य **(*dll_scheme_malloc_immobile_box)(शून्य *p);
स्थिर शून्य (*dll_scheme_free_immobile_box)(शून्य **b);
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
स्थिर पूर्णाङ्क (*dll_scheme_main_setup)(पूर्णाङ्क no_auto_statics, Scheme_Env_Main _main, पूर्णाङ्क argc, वर्ण **argv);
#   यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
स्थिर शून्य (*dll_scheme_register_tls_space)(शून्य *tls_space, पूर्णाङ्क _tls_index);
#   endif
#  endif
#  यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
स्थिर Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(शून्य);
#  endif
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 600
स्थिर शून्य (*dll_scheme_embedded_load)(intptr_t len, ध्रुव वर्ण *s, पूर्णाङ्क predefined);
स्थिर शून्य (*dll_scheme_register_embedded_load)(intptr_t len, ध्रुव वर्ण *s);
स्थिर शून्य (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# यदि defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  यदि !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_प्रतीक dll_scheme_add_global_प्रतीक
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_मूल्य dll_scheme_builtin_मूल्य
#  यदि MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_निवेश_port dll_scheme_close_निवेश_port
#  define scheme_गिनती_पंक्तियाँ dll_scheme_गिनती_पंक्तियाँ
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
#  यदि MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  अन्यथा
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  यदि MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  अन्यथा
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_प्रतीक dll_scheme_intern_प्रतीक
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_मूल्य dll_scheme_make_integer_मूल्य
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  यदि MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  अन्यथा
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_मूल्यावली dll_scheme_make_struct_मूल्यावली
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_निवेश_file dll_scheme_open_निवेश_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_ढेर_base dll_scheme_set_ढेर_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  यदि MZSCHEME_VERSION_MAJOR >= 299
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
#  यदि MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  यदि MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   यदि MZSCHEME_VERSION_MAJOR >= 500
#    यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(शून्य)
{
    प्रत्यागम dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

प्रकारनिर्देश संरचना
{
    वर्ण    *name;
    शून्य    **ptr;
} Thunk_Info;

स्थिर Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (शून्य **)&dll_GC_malloc},
    {"GC_malloc_atomic", (शून्य **)&dll_GC_malloc_atomic},
    {रिक्त, रिक्त}};

स्थिर Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (शून्य **)&dll_scheme_eof},
    {"scheme_false", (शून्य **)&dll_scheme_false},
    {"scheme_void", (शून्य **)&dll_scheme_void},
    {"scheme_null", (शून्य **)&dll_scheme_null},
    {"scheme_true", (शून्य **)&dll_scheme_true},
# यदि !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (शून्य **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (शून्य **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (शून्य **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(शून्य **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (शून्य **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (शून्य **)&dll_scheme_add_global_प्रतीक},
    {"scheme_apply", (शून्य **)&dll_scheme_apply},
    {"scheme_basic_env", (शून्य **)&dll_scheme_basic_env},
# यदि MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (शून्य **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (शून्य **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (शून्य **)&dll_scheme_builtin_मूल्य},
    {"scheme_check_threads", (शून्य **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (शून्य **)&dll_scheme_close_निवेश_port},
    {"scheme_count_lines", (शून्य **)&dll_scheme_गिनती_पंक्तियाँ},
    {"scheme_current_continuation_marks",
	(शून्य **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (शून्य **)&dll_scheme_display},
    {"scheme_display_to_string", (शून्य **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (शून्य **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (शून्य **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (शून्य **)&dll_scheme_eq},
    {"scheme_eval", (शून्य **)&dll_scheme_eval},
    {"scheme_eval_string", (शून्य **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (शून्य **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(शून्य **)&dll_scheme_finish_primitive_module},
# यदि MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (शून्य **)&dll_scheme_format},
# अन्यथा
    {"scheme_format_utf8", (शून्य **)&dll_scheme_format_utf8},
    {"scheme_get_param", (शून्य **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (शून्य **)&dll_scheme_gc_ptr_ok},
# यदि MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(शून्य **)&dll_scheme_get_sized_string_output},
# अन्यथा
    {"scheme_get_sized_byte_string_output",
	(शून्य **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (शून्य **)&dll_scheme_intern_प्रतीक},
    {"scheme_lookup_global", (शून्य **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(शून्य **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (शून्य **)&dll_scheme_make_integer_मूल्य},
    {"scheme_make_pair", (शून्य **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (शून्य **)&dll_scheme_make_prim_w_arity},
# यदि MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (शून्य **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(शून्य **)&dll_scheme_make_string_output_port},
# अन्यथा
    {"scheme_make_byte_string", (शून्य **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(शून्य **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(शून्य **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (शून्य **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (शून्य **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (शून्य **)&dll_scheme_make_struct_मूल्यावली},
    {"scheme_make_type", (शून्य **)&dll_scheme_make_type},
    {"scheme_make_vector", (शून्य **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (शून्य **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (शून्य **)&dll_scheme_open_निवेश_file},
    {"scheme_primitive_module", (शून्य **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (शून्य **)&dll_scheme_proper_list_length},
    {"scheme_raise", (शून्य **)&dll_scheme_raise},
    {"scheme_read", (शून्य **)&dll_scheme_read},
    {"scheme_register_static", (शून्य **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (शून्य **)&dll_scheme_set_ढेर_base},
    {"scheme_signal_error", (शून्य **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (शून्य **)&dll_scheme_wrong_type},
# यदि MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (शून्य **)&dll_scheme_set_param},
    {"scheme_current_config", (शून्य **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(शून्य **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (शून्य **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (शून्य **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (शून्य **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (शून्य **)&dll_scheme_hash_set},
    {"scheme_hash_get", (शून्य **)&dll_scheme_hash_get},
    {"scheme_make_double", (शून्य **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (शून्य **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (शून्य **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (शून्य **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (शून्य **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (शून्य **)&dll_GC_register_traversers},
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (शून्य **)&dll_scheme_main_setup},
#   यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (शून्य **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (शून्य **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (शून्य **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (शून्य **)&dll_scheme_free_immobile_box},
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 500
#  यदि defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (शून्य **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# यदि MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (शून्य **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (शून्य **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (शून्य **)&dll_scheme_set_config_path},
# endif
    {रिक्त, रिक्त}};

स्थिर HINSTANCE hMzGC = 0;
स्थिर HINSTANCE hMzSch = 0;

स्थिर शून्य dynamic_mzscheme_end(शून्य);
स्थिर पूर्णाङ्क mzscheme_runtime_link_init(वर्ण *sch_dll, वर्ण *gc_dll,
	पूर्णाङ्क verbose);

    स्थिर पूर्णाङ्क
mzscheme_runtime_link_init(वर्ण *sch_dll, वर्ण *gc_dll, पूर्णाङ्क verbose)
{
    Thunk_Info *thunk = रिक्त;

    यदि (hMzGC && hMzSch)
	प्रत्यागम OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    यदि (!hMzGC)
    {
	यदि (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	प्रत्यागम FAIL;
    }

    यदि (!hMzSch)
    {
	यदि (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	प्रत्यागम FAIL;
    }

    प्रति (thunk = mzsch_imports; thunk->name; thunk++)
    {
	यदि ((*thunk->ptr =
		    (शून्य *)GetProcAddress(hMzSch, thunk->name)) == रिक्त)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    यदि (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    प्रत्यागम FAIL;
	}
    }
    प्रति (thunk = mzgc_imports; thunk->name; thunk++)
    {
	यदि ((*thunk->ptr =
		    (शून्य *)GetProcAddress(hMzGC, thunk->name)) == रिक्त)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    यदि (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    प्रत्यागम FAIL;
	}
    }
    प्रत्यागम OK;
}

    पूर्णाङ्क
mzscheme_enabled(पूर्णाङ्क verbose)
{
    प्रत्यागम mzscheme_runtime_link_init(
	    (वर्ण *)p_mzschemedll, (वर्ण *)p_mzschemegcdll, verbose) == OK;
}

    स्थिर शून्य
dynamic_mzscheme_end(शून्य)
{
    यदि (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    यदि (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#यदि MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, अंक) GUARANTEE_STRING(proc, अंक)
#अन्यथा
    स्थिर Scheme_Object *
guaranteed_byte_string_arg(वर्ण *proc, पूर्णाङ्क अंक, पूर्णाङ्क argc, Scheme_Object **argv)
{
    यदि (SCHEME_BYTE_STRINGP(argv[अंक]))
    {
	प्रत्यागम argv[अंक];
    }
    अन्यथा यदि (SCHEME_CHAR_STRINGP(argv[अंक]))
    {
	Scheme_Object *tmp = रिक्त;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[अंक]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[अंक]);
	MZ_GC_UNREG();
	प्रत्यागम tmp;
    }
    अन्यथा
	scheme_wrong_type(proc, "string", अंक, argc, argv);
    // unreachable
    प्रत्यागम scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, अंक) guaranteed_byte_string_arg(proc, अंक, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#यदि defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

स्थिर Scheme_Type mz_buffer_type;
स्थिर Scheme_Type mz_window_type;

स्थिर पूर्णाङ्क initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
स्थिर पूर्णाङ्क disabled = FALSE;
#endif
स्थिर पूर्णाङ्क load_base_module_failed = FALSE;

// global environment
स्थिर Scheme_Env    *environment = रिक्त;
// output/error handlers
स्थिर Scheme_Object *curout = रिक्त;
स्थिर Scheme_Object *curerr = रिक्त;
// exn:vim exception
स्थिर Scheme_Object *exn_catching_apply = रिक्त;
स्थिर Scheme_Object *exn_p = रिक्त;
स्थिर Scheme_Object *exn_message = रिक्त;
स्थिर Scheme_Object *vim_exn = रिक्त; // Vim Error exception

#यदि !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
स्थिर शून्य *ढेर_base = रिक्त;
#endif

स्थिर दीर्घ range_start;
स्थिर दीर्घ range_end;

// MzScheme threads scheduling stuff
स्थिर पूर्णाङ्क mz_threads_allow = 0;

#यदि defined(FEAT_GUI_MSWIN)
स्थिर शून्य CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
स्थिर UINT timer_id = 0;
#नोचेद्यदि defined(FEAT_GUI_GTK)
स्थिर gboolean timer_proc(gpointer);
स्थिर guint timer_id = 0;
#नोचेद्यदि defined(FEAT_GUI_MOTIF)
स्थिर शून्य timer_proc(XtPointer, XtIntervalId *);
स्थिर XtIntervalId timer_id = (XtIntervalId)0;
#endif

#यदि !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    शून्य
mzvim_check_threads(शून्य)
{
    // Last time MzScheme threads were scheduled
    स्थिर समय_प्रकार mz_last_time = 0;

    यदि (mz_threads_allow && p_mzq > 0)
    {
	समय_प्रकार now = समय(रिक्त);

	यदि ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#यदि defined(MZSCHEME_GUI_THREADS)
स्थिर शून्य setup_timer(शून्य);
स्थिर शून्य remove_timer(शून्य);

// timers are presented in GUI only
# यदि defined(FEAT_GUI_MSWIN)
    स्थिर शून्य CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_संदेश UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    स्थिर gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    स्थिर शून्य
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# यदि defined(FEAT_GUI_GTK)
    प्रत्यागम TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    यदि (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, रिक्त);
# endif
}

    स्थिर शून्य
setup_timer(शून्य)
{
# यदि defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(रिक्त, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, रिक्त);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, रिक्त);
# endif
}

    स्थिर शून्य
remove_timer(शून्य)
{
# यदि defined(FEAT_GUI_MSWIN)
    KillTimer(रिक्त, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    वर्ण *
did_set_mzquantum(optset_T *args UNUSED)
{
#यदि defined(MZSCHEME_GUI_THREADS)
    // reset timer
    यदि (timer_id != 0)
	remove_timer();
    यदि (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    प्रत्यागम रिक्त;
}

    स्थिर शून्य
notify_multithread(पूर्णाङ्क on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    यदि (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    यदि (!on && timer_id != 0)
	remove_timer();
#endif
}

    शून्य
mzscheme_end(शून्य)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#यदि 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#यदि HAVE_TLS_SPACE && !defined(VIMDLL)
# यदि defined(_MSC_VER)
स्थिर __declspec(thread) शून्य *tls_space;
बाह्य intptr_t _tls_index;
# elif defined(__MINGW32__)
स्थिर __thread शून्य *tls_space;
बाह्य intptr_t _tls_index;
# अन्यथा
स्थिर THREAD_LOCAL शून्य *tls_space;
स्थिर intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    पूर्णाङ्क
mzscheme_main(शून्य)
{
    पूर्णाङ्क	    argc = 0;
    वर्ण    *argv = रिक्त;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    यदि (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	प्रत्यागम vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    शून्य **ptls_space;
    intptr_t tls_index;
    शून्य (*pget_tls_info)(शून्य ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (शून्य *)GetProcAddress(
				GetModuleHandle(रिक्त), "get_tls_info");
    यदि (pget_tls_info == रिक्त)
    {
	disabled = TRUE;
	प्रत्यागम vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# अन्यथा
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    प्रत्यागम scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#अन्यथा
    प्रत्यागम mzscheme_env_main(रिक्त, argc, &argv);
#endif
}

    स्थिर पूर्णाङ्क
mzscheme_env_main(Scheme_Env *env, पूर्णाङ्क argc UNUSED, वर्ण **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#अन्यथा
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = रिक्त;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    ढेर_base = &__gc_var_stack__;
# अन्यथा
    पूर्णाङ्क dummy = 0;
    ढेर_base = (शून्य *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    प्रत्यागम 0;
}

    स्थिर Scheme_Object*
load_base_module(शून्य *data)
{
    scheme_namespace_require(scheme_intern_प्रतीक((वर्ण *)data));
    प्रत्यागम scheme_null;
}

    स्थिर Scheme_Object *
load_base_module_on_error(शून्य *data UNUSED)
{
    load_base_module_failed = TRUE;
    प्रत्यागम scheme_null;
}

    स्थिर पूर्णाङ्क
startup_mzscheme(शून्य)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_ढेर_base(ढेर_base, 1);
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
#यदि MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = रिक्त;
	पूर्णाङ्क mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	यदि (s != रिक्त)
	{
	    coll_path = scheme_make_path((वर्ण *)s);
	    MZ_GC_CHECK();
	    यदि (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	यदि (coll_path == रिक्त)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	यदि (coll_path != रिक्त)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#अन्यथा
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = रिक्त;
	Scheme_Object	*coll_pair = रिक्त;
	Scheme_Config	*config = रिक्त;

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

#यदि MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = रिक्त;
	पूर्णाङ्क mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	यदि (s != रिक्त)
	{
	    config_path = scheme_make_path((वर्ण *)s);
	    MZ_GC_CHECK();
	    यदि (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	यदि (config_path == रिक्त)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	यदि (config_path != रिक्त)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#यदि MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(रिक्त, load_base_module, रिक्त,
				    load_base_module_on_error, "racket/base");
	यदि (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(रिक्त, load_base_module, रिक्त,
				    load_base_module_on_error, "scheme/base");
	    यदि (load_base_module_failed)
		प्रत्यागम -1;
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
	Scheme_Object	*make_security_guard = रिक्त;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# यदि MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_प्रतीक = रिक्त;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_प्रतीक);
	    MZ_GC_REG();
	    make_security_guard_प्रतीक = scheme_intern_प्रतीक("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_प्रतीक, environment);
	    MZ_GC_UNREG();
	}
# अन्यथा
	make_security_guard = scheme_builtin_मूल्य("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	यदि (make_security_guard != रिक्त)
	{
	    Scheme_Object   *args[3] = {रिक्त, रिक्त, रिक्त};
	    Scheme_Object   *guard = रिक्त;
	    Scheme_Config   *config = रिक्त;
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

    प्रत्यागम 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    स्थिर पूर्णाङ्क
mzscheme_init(शून्य)
{
    यदि (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	यदि (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    प्रत्यागम -1;
	}
#endif
	यदि (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    प्रत्यागम -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = रिक्त;
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

    प्रत्यागम 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    स्थिर पूर्णाङ्क
eval_with_exn_handling(
    शून्य		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *मूल्य = रिक्त;
    Scheme_Object   *exn = रिक्त;
    Scheme_Object   *prim = रिक्त;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, मूल्य);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    मूल्य = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    यदि (!मूल्य)
    {
	मूल्य = extract_exn_message(exn);
	// Got an exn?
	यदि (मूल्य)
	{
	    scheme_display(मूल्य, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	प्रत्यागम FAIL;
    }

    यदि (ret != रिक्त)	// if pointer to retval supported give it up
	*ret = मूल्य;
    // Print any result, as long as it's not a void
    अन्यथा यदि (!SCHEME_VOIDP(मूल्य))
    {
	scheme_display(मूल्य, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    प्रत्यागम OK;
}

/*
 * :mzscheme
 */
    स्थिर पूर्णाङ्क
do_mzscheme_command(exarg_T *eap, शून्य *data, Scheme_Closed_Prim *what)
{
    यदि (mzscheme_init())
	प्रत्यागम FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    प्रत्यागम eval_with_exn_handling(data, what, रिक्त);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    शून्य
mzscheme_buffer_free(buf_T *buf)
{
    यदि (buf->b_mzscheme_ref == रिक्त)
	प्रत्यागम;

    vim_mz_buffer *bp = रिक्त;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_मूल्य;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#अन्यथा
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = रिक्त;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    शून्य
mzscheme_window_free(win_T *win)
{
    यदि (win->w_mzscheme_ref == रिक्त)
	प्रत्यागम;

    vim_mz_window *wp = रिक्त;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_मूल्य;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#अन्यथा
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = रिक्त;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    शून्य
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    यदि (eap->skip)
	प्रत्यागम;

    यदि (script == रिक्त)
	do_mzscheme_command(eap, eap->arg, do_eval);
    अन्यथा
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    स्थिर Scheme_Object *
do_load(शून्य *data, पूर्णाङ्क noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *व्यंजक = रिक्त;
    Scheme_Object   *result = रिक्त;
    वर्ण	    *file = रिक्त;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, व्यंजक);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (वर्ण *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_निवेश_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_गिनती_पंक्तियाँ(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    यावत् (!SCHEME_EOFP(व्यंजक = scheme_read(pinfo->port)))
    {
	result = scheme_eval(व्यंजक, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_निवेश_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = रिक्त;
    MZ_GC_UNREG();
    प्रत्यागम result;
}

/*
 * :mzfile
 */
    शून्य
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {रिक्त, रिक्त};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (वर्ण *)eap->arg;
    यदि (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != रिक्त)	// looks like port was not closed
    {
	scheme_close_निवेश_port(pinfo.port);
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
    स्थिर शून्य
init_exn_catching_apply(शून्य)
{
    यदि (exn_catching_apply)
	प्रत्यागम;

    वर्ण *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_मूल्य("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_मूल्य("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    स्थिर Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    यदि (SCHEME_TRUEP(SCHEME_CAR(v)))
	प्रत्यागम SCHEME_CDR(v);
    अन्यथा
    {
	*exn = SCHEME_CDR(v);
	प्रत्यागम रिक्त;
    }
}

    स्थिर Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    यदि (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	प्रत्यागम _scheme_apply(exn_message, 1, &v);
    अन्यथा
	प्रत्यागम रिक्त; // Not an exn structure
}

    स्थिर Scheme_Object *
do_eval(शून्य *s, पूर्णाङ्क noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    प्रत्यागम scheme_eval_string_all((वर्ण *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    स्थिर शून्य
do_intrnl_output(वर्ण *mesg, पूर्णाङ्क error)
{
    वर्ण *p, *prev;

    prev = mesg;
    p = म_अक्षर(prev, '\n');
    यावत् (p)
    {
	*p = '\0';
	यदि (error)
	    emsg(prev);
	अन्यथा
	    संदेश(prev);
	prev = p + 1;
	p = म_अक्षर(prev, '\n');
    }

    यदि (error)
	emsg(prev);
    अन्यथा
	संदेश(prev);
}

    स्थिर शून्य
do_output(वर्ण *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    स्थिर शून्य
do_err_output(वर्ण *mesg)
{
    do_intrnl_output(mesg, 1);
}

    स्थिर शून्य
do_printf(वर्ण *format, ...)
{
    do_intrnl_output(format, 1);
}

    स्थिर शून्य
do_flush(शून्य)
{
    वर्ण *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    यदि (length)
    {
	do_err_output(buff);
	प्रत्यागम;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    यदि (length)
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
    स्थिर Scheme_Object *
vim_command(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = रिक्त;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_मूल्य(cmd), रिक्त, रिक्त, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    प्रत्यागम scheme_void;
}

/*
 * (eval {expr-string})
 */
    स्थिर Scheme_Object *
vim_eval(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = रिक्त;
    typval_T		*vim_result;
    Scheme_Object *व्यंजक = रिक्त;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, व्यंजक);
    MZ_GC_REG();
    व्यंजक = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_व्यंजक(BYTE_STRING_मूल्य(व्यंजक), रिक्त);

    यदि (vim_result == रिक्त)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    प्रत्यागम result;
#अन्यथा
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    प्रत्यागम scheme_false;
#endif
}

/*
 * (range-start)
 */
    स्थिर Scheme_Object *
get_range_start(
    शून्य		*data UNUSED,
    पूर्णाङ्क			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    प्रत्यागम scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    स्थिर Scheme_Object *
get_range_end(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
    प्रत्यागम scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    स्थिर Scheme_Object *
mzscheme_beep(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    प्रत्यागम scheme_void;
}

स्थिर Scheme_Object *M_global = रिक्त;

/*
 * (get-option {option-name}) [buffer/window]
 */
    स्थिर Scheme_Object *
get_option(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    दीर्घ	    मूल्य;
    वर्ण	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = रिक्त;
    Scheme_Object   *name = रिक्त;
    पूर्णाङ्क		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    यदि (argc > 1)
    {
	यदि (M_global == रिक्त)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_प्रतीक("global");
	    MZ_GC_CHECK();
	}

	यदि (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	अन्यथा यदि (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	अन्यथा यदि (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	अन्यथा
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_मूल्य(BYTE_STRING_मूल्य(name), &मूल्य, (char_u **)&strval,
								  रिक्त, scope);
    curbuf = save_curb;
    curwin = save_curw;

    विकल्प (rc)
    {
    पक्ष gov_bool:
    पक्ष gov_संख्या:
	MZ_GC_UNREG();
	प्रत्यागम scheme_make_integer_मूल्य(मूल्य);
    पक्ष gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	प्रत्यागम rval;
    पक्ष gov_hidden_bool:
    पक्ष gov_hidden_संख्या:
    पक्ष gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    पक्ष gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    प्रत्यागम scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    स्थिर Scheme_Object *
set_option(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    char_u	*command = रिक्त;
    पूर्णाङ्क		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = रिक्त;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    यदि (argc > 1)
    {
	यदि (M_global == रिक्त)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_प्रतीक("global");
	    MZ_GC_CHECK();
	}

	यदि (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	अन्यथा यदि (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	अन्यथा यदि (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	अन्यथा
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_मूल्य(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    प्रत्यागम scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    स्थिर Scheme_Object *
get_curr_win(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
    प्रत्यागम (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    स्थिर Scheme_Object *
get_window_गिनती(
    शून्य		*data UNUSED,
    पूर्णाङ्क			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    पूर्णाङ्क	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    प्रत्यागम scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    स्थिर Scheme_Object *
get_window_list(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    प्रति ( ; w != रिक्त; w = w->w_next)
	यदि (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    प्रत्यागम list;
}

    स्थिर Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = रिक्त;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    यदि (win->w_mzscheme_ref != रिक्त)
	प्रत्यागम (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, परिमाण(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(रिक्त);
#अन्यथा
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    प्रत्यागम (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    स्थिर Scheme_Object *
get_window_अंक(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
    पूर्णाङ्क		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    प्रति (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    प्रत्यागम scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    स्थिर Scheme_Object *
get_window_by_अंक(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    पूर्णाङ्क		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    यदि (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    प्रति ( ; win != रिक्त; win = win->w_next, --fnum)
	यदि (fnum == 1)	    // to be 1-based
	    प्रत्यागम window_new(win);

    प्रत्यागम scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    स्थिर Scheme_Object *
get_window_buffer(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    प्रत्यागम buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    स्थिर Scheme_Object *
get_window_height(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    प्रत्यागम scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    स्थिर Scheme_Object *
set_window_height(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    पूर्णाङ्क		    height;

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
    प्रत्यागम scheme_void;
}

/*
 * (get-win-width [window])
 */
    स्थिर Scheme_Object *
get_window_width(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    प्रत्यागम scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    स्थिर Scheme_Object *
set_window_width(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    पूर्णाङ्क		    width = 0;

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
    प्रत्यागम scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    स्थिर Scheme_Object *
get_cursor(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    प्रत्यागम scheme_make_pair(scheme_make_integer_मूल्य((दीर्घ)pos.lnum),
		    scheme_make_integer_मूल्य((दीर्घ)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    स्थिर Scheme_Object *
set_cursor(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    दीर्घ	    lnum = 0;
    दीर्घ	    col = 0;

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    win = get_window_arg(prim->name, 1, argc, argv);
    GUARANTEE_PAIR(prim->name, 0);

    यदि (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_पंक्ति_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    प्रत्यागम scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    स्थिर Scheme_Object *
mzscheme_open_buffer(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    पूर्णाङ्क		    अंक = 0;
    Scheme_Object   *onum = रिक्त;
    Scheme_Object   *buf = रिक्त;
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
    अंक = buflist_add(BYTE_STRING_मूल्य(fname), BLN_LISTED | BLN_CURBUF);

    यदि (अंक == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(अंक);
    buf = get_buffer_by_अंक(data, 1, &onum);
    MZ_GC_UNREG();
    प्रत्यागम buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    स्थिर Scheme_Object *
get_buffer_by_अंक(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    पूर्णाङ्क		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	यदि (buf->b_fnum == fnum)
	    प्रत्यागम buffer_new(buf);

    प्रत्यागम scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    स्थिर Scheme_Object *
get_buffer_by_name(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = रिक्त;
    Scheme_Object *fname = रिक्त;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	यदि (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त)
	    // empty string
	{
	    यदि (BYTE_STRING_मूल्य(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	अन्यथा यदि (!fnamecmp(buf->b_ffname, BYTE_STRING_मूल्य(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_मूल्य(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    प्रत्यागम buffer;
}

/*
 * (get-next-buff [buffer])
 */
    स्थिर Scheme_Object *
get_next_buffer(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    यदि (buf->b_next == रिक्त)
	प्रत्यागम scheme_false;
    अन्यथा
	प्रत्यागम buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    स्थिर Scheme_Object *
get_prev_buffer(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    यदि (buf->b_prev == रिक्त)
	प्रत्यागम scheme_false;
    अन्यथा
	प्रत्यागम buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    स्थिर Scheme_Object *
get_buffer_अंक(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    प्रत्यागम scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    स्थिर Scheme_Object *
get_buffer_गिनती(
    शून्य		*data UNUSED,
    पूर्णाङ्क			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    पूर्णाङ्क	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    प्रत्यागम scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    स्थिर Scheme_Object *
get_buffer_name(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    प्रत्यागम scheme_make_byte_string((वर्ण *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    स्थिर Scheme_Object *
get_curr_buffer(
    शून्य		*data UNUSED,
    पूर्णाङ्क			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    प्रत्यागम (Scheme_Object *)get_vim_curr_buffer();
}

    स्थिर Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = रिक्त;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    यदि (buf->b_mzscheme_ref)
	प्रत्यागम (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, परिमाण(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(रिक्त);
#अन्यथा
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    प्रत्यागम (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    स्थिर Scheme_Object *
get_buffer_size(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    प्रत्यागम scheme_make_integer(buf->buf->b_ml.ml_पंक्ति_गिनती);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    स्थिर Scheme_Object *
get_buffer_पंक्ति(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    पूर्णाङ्क		    linenr;
    char_u	    *पंक्ति;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    पंक्ति = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    प्रत्यागम scheme_make_byte_string((वर्ण *)पंक्ति);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    स्थिर Scheme_Object *
get_buffer_पंक्ति_list(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    पूर्णाङ्क		    i, hi, lo, n;
    Scheme_Object   *list = रिक्त;

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
    यदि (lo < 0)
	lo = 0;

    यदि (hi < 0)
	hi = 0;
    यदि (hi < lo)
	hi = lo;

    n = hi - lo;

    प्रति (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (वर्ण *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    प्रत्यागम list;
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
    स्थिर Scheme_Object *
set_buffer_पंक्ति(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *पंक्ति = रिक्त;
    वर्ण	    *save;
    पूर्णाङ्क		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, पंक्ति);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    यदि (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    पंक्ति = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_पंक्ति_range(n, buf->buf);

    यदि (SCHEME_FALSEP(पंक्ति))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	यदि (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	अन्यथा यदि (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	यदि (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_पंक्तियाँ_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	प्रत्यागम scheme_void;
    }
    अन्यथा
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_पंक्ति(पंक्ति);

	curbuf = buf->buf;

	यदि (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	अन्यथा यदि (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	अन्यथा
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	यदि (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	प्रत्यागम scheme_void;
    }
}

    स्थिर शून्य
free_array(वर्ण **array)
{
    वर्ण **curr = array;
    यावत् (*curr != रिक्त)
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
    स्थिर Scheme_Object *
set_buffer_पंक्ति_list(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = रिक्त;
    Scheme_Object   *पंक्ति_list = रिक्त;
    पूर्णाङ्क		    i, old_len, new_len, hi, lo;
    दीर्घ	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, पंक्ति_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    यदि (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    पंक्ति_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    यदि (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_पंक्ति_range(lo, buf->buf);	    // inclusive
    check_पंक्ति_range(hi - 1, buf->buf);	    // exclusive

    यदि (SCHEME_FALSEP(पंक्ति_list) || SCHEME_NULLP(पंक्ति_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	यदि (u_savedel((linenr_T)lo, (दीर्घ)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	अन्यथा
	{
	    प्रति (i = 0; i < old_len; i++)
		यदि (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    यदि (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_पंक्तियाँ_mark((linenr_T)lo, (दीर्घ)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	प्रत्यागम scheme_void;
    }
    अन्यथा
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(पंक्ति_list);
	MZ_GC_CHECK();
	यदि (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	अन्यथा
	{
	    वर्ण		**array = रिक्त;
	    Scheme_Object   *पंक्ति = रिक्त;
	    Scheme_Object   *rest = रिक्त;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, पंक्ति);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(वर्ण *, new_len + 1);

	    rest = पंक्ति_list;
	    प्रति (i = 0; i < new_len; ++i)
	    {
		पंक्ति = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		यदि (!SCHEME_STRINGP(पंक्ति))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_पंक्ति(पंक्ति);
	    }

	    curbuf = buf->buf;

	    यदि (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    प्रति (i = 0; i < old_len - new_len; ++i)
	    {
		यदि (ml_delete((linenr_T)lo) == FAIL)
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
	    प्रति (i = 0; i < old_len && i < new_len; i++)
		यदि (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    यावत् (i < new_len)
	    {
		यदि (ml_append((linenr_T)(lo + i - 1),
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
						  (दीर्घ)MAXLNUM, (दीर्घ)extra);
	changed_पंक्तियाँ((linenr_T)lo, 0, (linenr_T)hi, (दीर्घ)extra);

	यदि (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	प्रत्यागम scheme_void;
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
    स्थिर Scheme_Object *
जोड़ो_buffer_पंक्ति_list(शून्य *data, पूर्णाङ्क argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = रिक्त;
    Scheme_Object   *list = रिक्त;
    वर्ण	    *str = रिक्त;
    पूर्णाङ्क		    i, n, size;

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

    यदि (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    यदि (n != 0)	    // 0 can be used in insert
	check_पंक्ति_range(n, buf->buf);
    यदि (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_पंक्ति(list);
	curbuf = buf->buf;

	यदि (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	अन्यथा यदि (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	अन्यथा
	{
	    vim_free(str);
	    appended_पंक्तियाँ_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	प्रत्यागम scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    यदि (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    अन्यथा
    {
	Scheme_Object   *पंक्ति = रिक्त;
	Scheme_Object   *rest = रिक्त;
	वर्ण		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, पंक्ति);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(वर्ण *, size + 1);

	rest = list;
	प्रति (i = 0; i < size; ++i)
	{
	    पंक्ति = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_पंक्ति(पंक्ति);
	}

	curbuf = buf->buf;

	यदि (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	अन्यथा
	{
	    प्रति (i = 0; i < size; ++i)
		यदि (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    यदि (i > 0)
		appended_पंक्तियाँ_mark((linenr_T)n, (दीर्घ)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    प्रत्यागम scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    स्थिर Scheme_Object *
vim_bufferp(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv)
{
    यदि (SCHEME_VIMBUFFERP(argv[0]))
	प्रत्यागम scheme_true;
    अन्यथा
	प्रत्यागम scheme_false;
}

/*
 * (win? obj)
 */
    स्थिर Scheme_Object *
vim_windowp(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv)
{
    यदि (SCHEME_VIMWINDOWP(argv[0]))
	प्रत्यागम scheme_true;
    अन्यथा
	प्रत्यागम scheme_false;
}

/*
 * (buff-valid? obj)
 */
    स्थिर Scheme_Object *
vim_buffer_validp(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv)
{
    यदि (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_मूल्य)
	प्रत्यागम scheme_true;
    अन्यथा
	प्रत्यागम scheme_false;
}

/*
 * (win-valid? obj)
 */
    स्थिर Scheme_Object *
vim_window_validp(शून्य *data UNUSED, पूर्णाङ्क argc UNUSED, Scheme_Object **argv)
{
    यदि (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_मूल्य)
	प्रत्यागम scheme_true;
    अन्यथा
	प्रत्यागम scheme_false;
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
    स्थिर वर्ण *
string_to_पंक्ति(Scheme_Object *obj)
{
    वर्ण	*scheme_str = रिक्त;
    वर्ण	*vim_str = रिक्त;
    OUTPUT_LEN_TYPE	len;
    पूर्णाङ्क		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    यदि (स_प्रथम(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    प्रति (i = 0; i < len; ++i)
    {
	यदि (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	अन्यथा
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    प्रत्यागम vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    स्थिर Scheme_Object *
vim_to_mzscheme(typval_T *vim_मूल्य)
{
    Scheme_Object	*result = रिक्त;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = रिक्त;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_मूल्य, 1, visited);

    MZ_GC_UNREG();
    प्रत्यागम result;
}

    स्थिर Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_मूल्य,
    पूर्णाङ्क			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = रिक्त;
    पूर्णाङ्क		    new_मूल्य_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    यदि (depth > 100)
    {
	MZ_GC_UNREG();
	प्रत्यागम scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_मूल्य);
    MZ_GC_CHECK();
    यदि (result != रिक्त) // found, do nothing
	new_मूल्य_2 = FALSE;
    अन्यथा यदि (vim_मूल्य->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((वर्ण *)vim_मूल्य->vval.v_string);
	MZ_GC_CHECK();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_संख्या)
    {
	result = scheme_make_integer((दीर्घ)vim_मूल्य->vval.v_संख्या);
	MZ_GC_CHECK();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((द्विगुण)vim_मूल्य->vval.v_float);
	MZ_GC_CHECK();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_LIST)
    {
	list_T		*list = vim_मूल्य->vval.v_list;
	listitem_T	*curr;

	यदि (list == रिक्त || list->lv_first == रिक्त)
	    result = scheme_null;
	अन्यथा
	{
	    Scheme_Object   *obj = रिक्त;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    यावत् (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_DICT)
    {
	Scheme_Object	  *कुंजी = रिक्त;
	Scheme_Object	  *obj = रिक्त;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, कुंजी);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	यदि (vim_मूल्य->vval.v_dict != रिक्त)
	{
	    hashtab_T	*ht = &vim_मूल्य->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_वस्तुएँ(ht, hi, todo)
	    {
		यदि (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    कुंजी = scheme_make_byte_string((वर्ण *)hi->hi_कुंजी);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, कुंजी, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = रिक्त;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((वर्ण *)vim_मूल्य->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ध्रुव वर्ण *)BYTE_STRING_मूल्य(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_PARTIAL)
    {
	यदि (vim_मूल्य->vval.v_partial == रिक्त)
	    result = scheme_null;
	अन्यथा
	{
	    Scheme_Object *funcname = रिक्त;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (वर्ण *)partial_name(vim_मूल्य->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ध्रुव वर्ण *)BYTE_STRING_मूल्य(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    अन्यथा यदि (vim_मूल्य->v_type == VAR_BOOL || vim_मूल्य->v_type == VAR_SPECIAL)
    {
	यदि (vim_मूल्य->vval.v_संख्या <= VVAL_TRUE)
	    result = scheme_make_integer((दीर्घ)vim_मूल्य->vval.v_संख्या);
	अन्यथा
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    अन्यथा
    {
	result = scheme_void;
	new_मूल्य_2 = FALSE;
    }
    यदि (new_मूल्य_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_मूल्य, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    प्रत्यागम result;
}

    स्थिर पूर्णाङ्क
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    पूर्णाङ्क i, status;
    Scheme_Hash_Table	*visited = रिक्त;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    प्रति (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	यदि (visited->vals[i] != रिक्त)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    प्रत्यागम status;
}
    स्थिर पूर्णाङ्क
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, पूर्णाङ्क depth,
	Scheme_Hash_Table *visited)
{
    पूर्णाङ्क		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    यदि (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_संख्या;
	tv->vval.v_संख्या = 0;
	प्रत्यागम FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    यदि (found != रिक्त)
	copy_tv(found, tv);
    अन्यथा यदि (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_संख्या = VVAL_NULL;
    }
    अन्यथा यदि (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_संख्या;
	tv->vval.v_संख्या = SCHEME_INT_VAL(obj);
    }
    अन्यथा यदि (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_संख्या = SCHEME_TRUEP(obj);
    }
    अन्यथा यदि (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    अन्यथा यदि (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_मूल्य(obj));
    }
# यदि MZSCHEME_VERSION_MAJOR >= 299
    अन्यथा यदि (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = रिक्त;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_मूल्य(tmp));
	MZ_GC_UNREG();
    }
# endif
    अन्यथा यदि (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	यदि (list == रिक्त)
	    status = FAIL;
	अन्यथा
	{
	    पूर्णाङ्क		    i;
	    Scheme_Object   *curr = रिक्त;
	    Scheme_Object   *cval = रिक्त;
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
	    यदि (v == रिक्त)
		status = FAIL;
	    अन्यथा
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		यदि (SCHEME_VECTORP(obj))
		{
		    प्रति (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			यदि (status == FAIL)
			    भङ्ग;
			status = list_append_tv(list, v);
			clear_tv(v);
			यदि (status == FAIL)
			    भङ्ग;
		    }
		}
		अन्यथा यदि (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    प्रति (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			यदि (status == FAIL)
			    भङ्ग;
			status = list_append_tv(list, v);
			clear_tv(v);
			यदि (status == FAIL)
			    भङ्ग;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    यदि (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			यदि (status == OK)
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
    अन्यथा यदि (SCHEME_HASHTP(obj))
    {
	पूर्णाङ्क		i;
	dict_T		*dict;
	Scheme_Object   *कुंजी = रिक्त;
	Scheme_Object   *val = रिक्त;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, कुंजी);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	यदि (dict == रिक्त)
	    status = FAIL;
	अन्यथा
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    प्रति (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		यदि (((Scheme_Hash_Table *) obj)->vals[i] != रिक्त)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_पंक्ति(
				((Scheme_Hash_Table *) obj)->कुंजियाँ[i]));
		    // convert Scheme val to Vim and add it to the dict
		    यदि (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			भङ्ग;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    अन्यथा
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_पंक्ति(obj);
    }
    MZ_GC_UNREG();
    प्रत्यागम status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    स्थिर Scheme_Object *
vim_funcref(शून्य *name, पूर्णाङ्क argc, Scheme_Object **argv)
{
    पूर्णाङ्क i;
    typval_T args;
    पूर्णाङ्क status = OK;
    Scheme_Object *result = रिक्त;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    यदि (list == रिक्त)
	status = FAIL;
    अन्यथा
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	प्रति (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    यदि (v == रिक्त)
		status = FAIL;
	    अन्यथा
	    {
		status = mzscheme_to_vim(argv[i], v);
		यदि (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	यदि (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_मूल्य((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    यदि (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    अन्यथा
	raise_if_error();
    प्रत्यागम result;
}

    शून्य
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = रिक्त;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    यदि (mzscheme_init())
    {
	MZ_GC_UNREG();
	प्रत्यागम;
    }

    MZ_GC_CHECK();
    यदि (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    स्थिर पूर्णाङ्क
vim_error_check(शून्य)
{
    प्रत्यागम (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    स्थिर शून्य
register_vim_exn(शून्य)
{
    पूर्णाङ्क	nc = 0;
    पूर्णाङ्क i;
    Scheme_Object   *struct_exn = रिक्त;
    Scheme_Object   *exn_name = रिक्त;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_प्रतीक("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_मूल्य("struct:exn");
    MZ_GC_CHECK();

    यदि (vim_exn == रिक्त)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, रिक्त, 0, 0, रिक्त, रिक्त
#यदि MZSCHEME_VERSION_MAJOR >= 299
		, रिक्त
#endif
		);


    {
	Scheme_Object   **tmp = रिक्त;
	Scheme_Object   *exn_names[5] = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
	Scheme_Object   *exn_मूल्यावली[5] = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_मूल्यावली, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * परिमाण(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_मूल्यावली(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_मूल्यावली, tmp, nc * परिमाण(Scheme_Object *));
	MZ_GC_CHECK();

	प्रति (i = 0; i < nc; i++)
	{
	    scheme_add_global_प्रतीक(exn_names[i],
		    exn_मूल्यावली[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    शून्य
raise_vim_exn(ध्रुव वर्ण *add_info)
{
    वर्ण	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {रिक्त, रिक्त};
    Scheme_Object   *exn = रिक्त;
    Scheme_Object   *byte_string = रिक्त;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    यदि (add_info != रिक्त)
    {
	वर्ण		*c_string = रिक्त;
	Scheme_Object   *info = रिक्त;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (पूर्णाङ्क)STRLEN(fmt), 1, &info, रिक्त);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    अन्यथा
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#यदि MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#अन्यथा
    argv[1] = scheme_current_continuation_marks(रिक्त);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    शून्य
raise_if_error(शून्य)
{
    यदि (vim_error_check())
	raise_vim_exn(रिक्त);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    स्थिर vim_mz_buffer *
get_buffer_arg(ध्रुव वर्ण *fname, पूर्णाङ्क argnum, पूर्णाङ्क argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    यदि (argc < argnum + 1)
	प्रत्यागम get_vim_curr_buffer();
    यदि (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (शून्य)get_valid_buffer(argv[argnum]);
    प्रत्यागम b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    स्थिर vim_mz_window *
get_window_arg(ध्रुव वर्ण *fname, पूर्णाङ्क argnum, पूर्णाङ्क argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    यदि (argc < argnum + 1)
	प्रत्यागम get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    यदि (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (शून्य)get_valid_window(argv[argnum]);
    प्रत्यागम w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(शून्य *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    यदि (buf == INVALID_BUFFER_मूल्य)
	scheme_signal_error(_("buffer is invalid"));
    प्रत्यागम buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(शून्य *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    यदि (win == INVALID_WINDOW_मूल्य)
	scheme_signal_error(_("window is invalid"));
    प्रत्यागम win;
}

    पूर्णाङ्क
mzthreads_allowed(शून्य)
{
    प्रत्यागम mz_threads_allow;
}

    स्थिर पूर्णाङ्क
पंक्ति_in_range(linenr_T lnum, buf_T *buf)
{
    प्रत्यागम (lnum > 0 && lnum <= buf->b_ml.ml_पंक्ति_गिनती);
}

    स्थिर शून्य
check_पंक्ति_range(linenr_T lnum, buf_T *buf)
{
    यदि (!पंक्ति_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    स्थिर शून्य
mz_fix_cursor(पूर्णाङ्क lo, पूर्णाङ्क hi, पूर्णाङ्क extra)
{
    यदि (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	यदि (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	अन्यथा यदि (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	अन्यथा
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

स्थिर Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_पंक्ति, "get-buff-line", 1, 2},
    {set_buffer_पंक्ति, "set-buff-line", 2, 3},
    {get_buffer_पंक्ति_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_अंक, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_पंक्ति_list, "set-buff-line-list", 3, 4},
    {जोड़ो_buffer_पंक्ति_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_गिनती, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_अंक, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_गिनती, "win-count", 0, 0},
    {get_window_by_अंक, "get-win-by-num", 1, 1},
    {get_window_अंक, "get-win-num", 0, 1},
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
    स्थिर vim_mz_buffer *
get_vim_curr_buffer(शून्य)
{
    यदि (curbuf->b_mzscheme_ref == रिक्त)
	प्रत्यागम (vim_mz_buffer *)buffer_new(curbuf);
    अन्यथा
	प्रत्यागम BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    स्थिर vim_mz_window *
get_vim_curr_window(शून्य)
{
    यदि (curwin->w_mzscheme_ref == रिक्त)
	प्रत्यागम (vim_mz_window *)window_new(curwin);
    अन्यथा
	प्रत्यागम WINDOW_REF(curwin);
}

    स्थिर शून्य
make_modules(शून्य)
{
    पूर्णाङ्क		    i;
    Scheme_Env	    *mod = रिक्त;
    Scheme_Object   *vimext_प्रतीक = रिक्त;
    Scheme_Object   *closed_prim = रिक्त;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_प्रतीक);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_प्रतीक = scheme_intern_प्रतीक("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_प्रतीक, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(prims); i++)
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
स्थिर Scheme_Object *M_write = रिक्त;
स्थिर Scheme_Object *M_read = रिक्त;
स्थिर Scheme_Object *M_execute = रिक्त;
स्थिर Scheme_Object *M_delete = रिक्त;

    स्थिर शून्य
sandbox_check(शून्य)
{
    यदि (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    स्थिर Scheme_Object *
sandbox_file_guard(पूर्णाङ्क argc UNUSED, Scheme_Object **argv)
{
    यदि (!sandbox)
	प्रत्यागम scheme_void;

    Scheme_Object *requested_access = argv[2];

    यदि (M_write == रिक्त)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_प्रतीक("write");
	MZ_GC_CHECK();
    }
    यदि (M_read == रिक्त)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_प्रतीक("read");
	MZ_GC_CHECK();
    }
    यदि (M_execute == रिक्त)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_प्रतीक("execute");
	MZ_GC_CHECK();
    }
    यदि (M_delete == रिक्त)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_प्रतीक("delete");
	MZ_GC_CHECK();
    }

    यावत् (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	यदि (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    प्रत्यागम scheme_void;
}

    स्थिर Scheme_Object *
sandbox_network_guard(पूर्णाङ्क argc UNUSED, Scheme_Object **argv UNUSED)
{
    प्रत्यागम scheme_void;
}
#endif

#endif
