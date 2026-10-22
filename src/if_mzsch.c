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
#اگر MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#اگر defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

تعریفنوع ساختار
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_مقدار ((buf_T *)(-1))

تعریفنوع ساختار
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_مقدار ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
تعریفنوع ساختار
{
    Scheme_Closed_Prim	*prim;
    نویسه	*name;
    صحیح		mina;	// arity information
    صحیح		maxa;
} Vim_Prim;

تعریفنوع ساختار
{
    نویسه	    *name;
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
ایستا Scheme_Object *sandbox_file_guard(صحیح, Scheme_Object **);
ایستا Scheme_Object *sandbox_network_guard(صحیح, Scheme_Object **);
ایستا تهی sandbox_check(تهی);
#endif
//  Buffer-related commands
ایستا Scheme_Object *buffer_new(buf_T *buf);
ایستا Scheme_Object *get_buffer_by_رقم(تهی *, صحیح, Scheme_Object **);
ایستا vim_mz_buffer *get_vim_curr_buffer(تهی);

//  Window-related commands
ایستا Scheme_Object *window_new(win_T *win);
ایستا vim_mz_window *get_vim_curr_window(تهی);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ایستا صحیح vim_error_check(تهی);
ایستا صحیح do_mzscheme_command(exarg_T *, تهی *, Scheme_Closed_Prim *what);
ایستا صحیح startup_mzscheme(تهی);
ایستا نویسه *string_to_سطر(Scheme_Object *obj);
#اگر MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#وگرنه
# define OUTPUT_LEN_TYPE long
#endif
ایستا تهی do_output(نویسه *mesg, OUTPUT_LEN_TYPE len);
ایستا تهی do_printf(نویسه *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ایستا تهی do_flush(تهی);
ایستا Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ایستا Scheme_Object *extract_exn_message(Scheme_Object *v);
ایستا Scheme_Object *do_eval(تهی *, صحیح noargc, Scheme_Object **noargv);
ایستا Scheme_Object *do_load(تهی *, صحیح noargc, Scheme_Object **noargv);
ایستا تهی register_vim_exn(تهی);
ایستا vim_mz_buffer *get_buffer_arg(ثابت نویسه *fname, صحیح argnum,
	صحیح argc, Scheme_Object **argv);
ایستا vim_mz_window *get_window_arg(ثابت نویسه *fname, صحیح argnum,
	صحیح argc, Scheme_Object **argv);
ایستا صحیح سطر_in_range(linenr_T, buf_T *);
ایستا تهی check_سطر_range(linenr_T, buf_T *);
ایستا تهی mz_fix_cursor(صحیح lo, صحیح hi, صحیح extra);

ایستا صحیح eval_with_exn_handling(تهی *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ایستا تهی make_modules(تهی);
ایستا تهی init_exn_catching_apply(تهی);
ایستا صحیح mzscheme_env_main(Scheme_Env *env, صحیح argc, نویسه **argv);
ایستا صحیح mzscheme_init(تهی);
#ifdef FEAT_EVAL
ایستا Scheme_Object *vim_to_mzscheme(typval_T *vim_مقدار);
ایستا Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_مقدار, صحیح depth,
	Scheme_Hash_Table *visited);
ایستا صحیح mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
ایستا صحیح mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, صحیح depth,
	Scheme_Hash_Table *visited);
ایستا Scheme_Object *vim_funcref(تهی *data, صحیح argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
ایستا صحیح buffer_size_proc(تهی *obj UNUSED)
{
    بازگشت gc_BYTES_TO_واژگان(اندازه(vim_mz_buffer));
}
ایستا صحیح buffer_mark_proc(تهی *obj)
{
    بازگشت buffer_size_proc(obj);
}
ایستا صحیح buffer_fixup_proc(تهی *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    بازگشت buffer_size_proc(obj);
}
ایستا صحیح window_size_proc(تهی *obj UNUSED)
{
    بازگشت gc_BYTES_TO_واژگان(اندازه(vim_mz_window));
}
ایستا صحیح window_mark_proc(تهی *obj)
{
    بازگشت window_size_proc(obj);
}
ایستا صحیح window_fixup_proc(تهی *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    بازگشت window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#وگرنه
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#اگر defined(DYNAMIC_MZSCHEME)
ایستا Scheme_Object *dll_scheme_eof;
ایستا Scheme_Object *dll_scheme_false;
ایستا Scheme_Object *dll_scheme_void;
ایستا Scheme_Object *dll_scheme_null;
ایستا Scheme_Object *dll_scheme_true;

ایستا Scheme_Thread **dll_scheme_current_thread_ptr;

ایستا تهی (**dll_scheme_console_printf_ptr)(نویسه *str, ...);
ایستا تهی (**dll_scheme_console_output_ptr)(نویسه *str, OUTPUT_LEN_TYPE len);
ایستا تهی (**dll_scheme_notify_multithread_ptr)(صحیح on);

ایستا تهی *(*dll_GC_malloc)(size_t size_in_bytes);
ایستا تهی *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
ایستا Scheme_Env *(*dll_scheme_basic_env)(تهی);
ایستا تهی (*dll_scheme_check_threads)(تهی);
ایستا تهی (*dll_scheme_register_static)(تهی *ptr, long size);
ایستا تهی (*dll_scheme_set_پشته_base)(تهی *base, صحیح no_auto_statics);
ایستا تهی (*dll_scheme_add_global)(ثابت نویسه *name, Scheme_Object *val,
	Scheme_Env *env);
ایستا تهی (*dll_scheme_add_global_نماد)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ایستا Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, صحیح رقم_rands,
	Scheme_Object **rands);
ایستا Scheme_Object *(*dll_scheme_builtin_مقدار)(ثابت نویسه *name);
# اگر MZSCHEME_VERSION_MAJOR >= 299
ایستا Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
ایستا Scheme_Object *(*dll_scheme_make_path)(ثابت نویسه *chars);
# endif
ایستا تهی (*dll_scheme_close_درون_داد_port)(Scheme_Object *port);
ایستا تهی (*dll_scheme_شمار_سطرها)(Scheme_Object *port);
# اگر MZSCHEME_VERSION_MAJOR < 360
ایستا Scheme_Object *(*dll_scheme_current_continuation_marks)(تهی);
# وگرنه
ایستا Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
ایستا تهی (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
ایستا نویسه *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ایستا صحیح (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ایستا Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	صحیح رقم_rands_2, Scheme_Object **rands, صحیح val);
ایستا تهی (*dll_scheme_dont_gc_ptr)(تهی *p);
ایستا Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ایستا Scheme_Object *(*dll_scheme_eval_string)(ثابت نویسه *str,
	Scheme_Env *env);
ایستا Scheme_Object *(*dll_scheme_eval_string_all)(ثابت نویسه *str,
	Scheme_Env *env, صحیح all);
ایستا تهی (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# اگر MZSCHEME_VERSION_MAJOR < 299
ایستا نویسه *(*dll_scheme_format)(نویسه *format, صحیح flen, صحیح argc,
	Scheme_Object **argv, long *rlen);
# وگرنه
ایستا نویسه *(*dll_scheme_format_utf8)(نویسه *format, صحیح flen, صحیح argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ایستا Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, صحیح pos);
# endif
ایستا تهی (*dll_scheme_gc_ptr_ok)(تهی *p);
# اگر MZSCHEME_VERSION_MAJOR < 299
ایستا نویسه *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# وگرنه
ایستا نویسه *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ایستا Scheme_Object *(*dll_scheme_intern_نماد)(ثابت نویسه *name);
ایستا Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *نماد,
	Scheme_Env *env);
ایستا Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, تهی *data, ثابت نویسه *name, mzshort mina,
     mzshort maxa);
ایستا Scheme_Object *(*dll_scheme_make_integer_مقدار)(long i);
ایستا Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ایستا Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ثابت نویسه *name, mzshort mina, mzshort maxa);
# اگر MZSCHEME_VERSION_MAJOR < 299
ایستا Scheme_Object *(*dll_scheme_make_string)(ثابت نویسه *chars);
ایستا Scheme_Object *(*dll_scheme_make_string_output_port)();
# وگرنه
ایستا Scheme_Object *(*dll_scheme_make_byte_string)(ثابت نویسه *chars);
ایستا Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
ایستا Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	صحیح argc, Scheme_Object **argv);
ایستا Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, صحیح flags, صحیح *شمار_out);
ایستا Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, صحیح رقم_fields,
	صحیح رقم_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# اگر MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ایستا Scheme_Object **(*dll_scheme_make_struct_مقادیر)(
	Scheme_Object *struct_type, Scheme_Object **names, صحیح شمار,
	صحیح flags);
ایستا Scheme_Type (*dll_scheme_make_type)(ثابت نویسه *name);
ایستا Scheme_Object *(*dll_scheme_make_vector)(صحیح size,
	Scheme_Object *fill);
ایستا تهی *(*dll_scheme_malloc_fail_ok)(تهی *(*f)(size_t), size_t);
ایستا Scheme_Object *(*dll_scheme_open_درون_داد_file)(ثابت نویسه *name,
	ثابت نویسه *who);
ایستا Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ایستا صحیح (*dll_scheme_proper_list_length)(Scheme_Object *list);
ایستا تهی (*dll_scheme_raise)(Scheme_Object *exn);
ایستا Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
ایستا تهی (*dll_scheme_signal_error)(ثابت نویسه *پیام, ...);
ایستا تهی (*dll_scheme_wrong_type)(ثابت نویسه *name, ثابت نویسه *expected,
	صحیح which, صحیح argc, Scheme_Object **argv);
# اگر MZSCHEME_VERSION_MAJOR >= 299
ایستا تهی (*dll_scheme_set_param)(Scheme_Config *c, صحیح pos,
	Scheme_Object *o);
ایستا Scheme_Config *(*dll_scheme_current_config)(تهی);
ایستا Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
ایستا Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
ایستا تهی *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
ایستا Scheme_Hash_Table *(*dll_scheme_make_hash_table)(صحیح type);
ایستا تهی (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *کلید, Scheme_Object *مقدار);
ایستا Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *کلید);
ایستا Scheme_Object *(*dll_scheme_make_double)(دوگانه d);
ایستا Scheme_Object *(*dll_scheme_make_sized_byte_string)(نویسه *chars,
	long len, صحیح copy);
ایستا Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ایستا Scheme_Object *(*dll_scheme_dynamic_wind)(تهی (*pre)(تهی *), Scheme_Object *(* فرار act)(تهی *), تهی (* فرار post)(تهی *), Scheme_Object *(*jmp_handler)(تهی *), تهی * فرار data);
# ifdef MZ_PRECISE_GC
ایستا تهی *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
ایستا تهی (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, صحیح is_constant_size, صحیح is_atomic);
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 400
ایستا تهی (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ایستا تهی **(*dll_scheme_malloc_immobile_box)(تهی *p);
ایستا تهی (*dll_scheme_free_immobile_box)(تهی **b);
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ایستا صحیح (*dll_scheme_main_setup)(صحیح no_auto_statics, Scheme_Env_Main _main, صحیح argc, نویسه **argv);
#   اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
ایستا تهی (*dll_scheme_register_tls_space)(تهی *tls_space, صحیح _tls_index);
#   endif
#  endif
#  اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
ایستا Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(تهی);
#  endif
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 600
ایستا تهی (*dll_scheme_embedded_load)(intptr_t len, ثابت نویسه *s, صحیح predefined);
ایستا تهی (*dll_scheme_register_embedded_load)(intptr_t len, ثابت نویسه *s);
ایستا تهی (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# اگر defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  اگر !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_نماد dll_scheme_add_global_نماد
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_مقدار dll_scheme_builtin_مقدار
#  اگر MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_درون_داد_port dll_scheme_close_درون_داد_port
#  define scheme_شمار_سطرها dll_scheme_شمار_سطرها
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
#  اگر MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  وگرنه
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  اگر MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  وگرنه
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_نماد dll_scheme_intern_نماد
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_مقدار dll_scheme_make_integer_مقدار
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  اگر MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  وگرنه
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_مقادیر dll_scheme_make_struct_مقادیر
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_درون_داد_file dll_scheme_open_درون_داد_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_پشته_base dll_scheme_set_پشته_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  اگر MZSCHEME_VERSION_MAJOR >= 299
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
#  اگر MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  اگر MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   اگر MZSCHEME_VERSION_MAJOR >= 500
#    اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(تهی)
{
    بازگشت dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

تعریفنوع ساختار
{
    نویسه    *name;
    تهی    **ptr;
} Thunk_Info;

ایستا Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (تهی **)&dll_GC_malloc},
    {"GC_malloc_atomic", (تهی **)&dll_GC_malloc_atomic},
    {پوچ, پوچ}};

ایستا Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (تهی **)&dll_scheme_eof},
    {"scheme_false", (تهی **)&dll_scheme_false},
    {"scheme_void", (تهی **)&dll_scheme_void},
    {"scheme_null", (تهی **)&dll_scheme_null},
    {"scheme_true", (تهی **)&dll_scheme_true},
# اگر !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (تهی **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (تهی **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (تهی **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(تهی **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (تهی **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (تهی **)&dll_scheme_add_global_نماد},
    {"scheme_apply", (تهی **)&dll_scheme_apply},
    {"scheme_basic_env", (تهی **)&dll_scheme_basic_env},
# اگر MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (تهی **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (تهی **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (تهی **)&dll_scheme_builtin_مقدار},
    {"scheme_check_threads", (تهی **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (تهی **)&dll_scheme_close_درون_داد_port},
    {"scheme_count_lines", (تهی **)&dll_scheme_شمار_سطرها},
    {"scheme_current_continuation_marks",
	(تهی **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (تهی **)&dll_scheme_display},
    {"scheme_display_to_string", (تهی **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (تهی **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (تهی **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (تهی **)&dll_scheme_eq},
    {"scheme_eval", (تهی **)&dll_scheme_eval},
    {"scheme_eval_string", (تهی **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (تهی **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(تهی **)&dll_scheme_finish_primitive_module},
# اگر MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (تهی **)&dll_scheme_format},
# وگرنه
    {"scheme_format_utf8", (تهی **)&dll_scheme_format_utf8},
    {"scheme_get_param", (تهی **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (تهی **)&dll_scheme_gc_ptr_ok},
# اگر MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(تهی **)&dll_scheme_get_sized_string_output},
# وگرنه
    {"scheme_get_sized_byte_string_output",
	(تهی **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (تهی **)&dll_scheme_intern_نماد},
    {"scheme_lookup_global", (تهی **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(تهی **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (تهی **)&dll_scheme_make_integer_مقدار},
    {"scheme_make_pair", (تهی **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (تهی **)&dll_scheme_make_prim_w_arity},
# اگر MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (تهی **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(تهی **)&dll_scheme_make_string_output_port},
# وگرنه
    {"scheme_make_byte_string", (تهی **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(تهی **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(تهی **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (تهی **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (تهی **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (تهی **)&dll_scheme_make_struct_مقادیر},
    {"scheme_make_type", (تهی **)&dll_scheme_make_type},
    {"scheme_make_vector", (تهی **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (تهی **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (تهی **)&dll_scheme_open_درون_داد_file},
    {"scheme_primitive_module", (تهی **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (تهی **)&dll_scheme_proper_list_length},
    {"scheme_raise", (تهی **)&dll_scheme_raise},
    {"scheme_read", (تهی **)&dll_scheme_read},
    {"scheme_register_static", (تهی **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (تهی **)&dll_scheme_set_پشته_base},
    {"scheme_signal_error", (تهی **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (تهی **)&dll_scheme_wrong_type},
# اگر MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (تهی **)&dll_scheme_set_param},
    {"scheme_current_config", (تهی **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(تهی **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (تهی **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (تهی **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (تهی **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (تهی **)&dll_scheme_hash_set},
    {"scheme_hash_get", (تهی **)&dll_scheme_hash_get},
    {"scheme_make_double", (تهی **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (تهی **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (تهی **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (تهی **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (تهی **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (تهی **)&dll_GC_register_traversers},
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (تهی **)&dll_scheme_main_setup},
#   اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (تهی **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (تهی **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (تهی **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (تهی **)&dll_scheme_free_immobile_box},
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 500
#  اگر defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (تهی **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# اگر MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (تهی **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (تهی **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (تهی **)&dll_scheme_set_config_path},
# endif
    {پوچ, پوچ}};

ایستا HINSTANCE hMzGC = 0;
ایستا HINSTANCE hMzSch = 0;

ایستا تهی dynamic_mzscheme_end(تهی);
ایستا صحیح mzscheme_runtime_link_init(نویسه *sch_dll, نویسه *gc_dll,
	صحیح verbose);

    ایستا صحیح
mzscheme_runtime_link_init(نویسه *sch_dll, نویسه *gc_dll, صحیح verbose)
{
    Thunk_Info *thunk = پوچ;

    اگر (hMzGC && hMzSch)
	بازگشت OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    اگر (!hMzGC)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	بازگشت FAIL;
    }

    اگر (!hMzSch)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	بازگشت FAIL;
    }

    برای (thunk = mzsch_imports; thunk->name; thunk++)
    {
	اگر ((*thunk->ptr =
		    (تهی *)GetProcAddress(hMzSch, thunk->name)) == پوچ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    بازگشت FAIL;
	}
    }
    برای (thunk = mzgc_imports; thunk->name; thunk++)
    {
	اگر ((*thunk->ptr =
		    (تهی *)GetProcAddress(hMzGC, thunk->name)) == پوچ)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

    صحیح
mzscheme_enabled(صحیح verbose)
{
    بازگشت mzscheme_runtime_link_init(
	    (نویسه *)p_mzschemedll, (نویسه *)p_mzschemegcdll, verbose) == OK;
}

    ایستا تهی
dynamic_mzscheme_end(تهی)
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
#endif // DYNAMIC_MZSCHEME

#اگر MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, رقم) GUARANTEE_STRING(proc, رقم)
#وگرنه
    ایستا Scheme_Object *
guaranteed_byte_string_arg(نویسه *proc, صحیح رقم, صحیح argc, Scheme_Object **argv)
{
    اگر (SCHEME_BYTE_STRINGP(argv[رقم]))
    {
	بازگشت argv[رقم];
    }
    وگرنه اگر (SCHEME_CHAR_STRINGP(argv[رقم]))
    {
	Scheme_Object *tmp = پوچ;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[رقم]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[رقم]);
	MZ_GC_UNREG();
	بازگشت tmp;
    }
    وگرنه
	scheme_wrong_type(proc, "string", رقم, argc, argv);
    // unreachable
    بازگشت scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, رقم) guaranteed_byte_string_arg(proc, رقم, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#اگر defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ایستا Scheme_Type mz_buffer_type;
ایستا Scheme_Type mz_window_type;

ایستا صحیح initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
ایستا صحیح disabled = FALSE;
#endif
ایستا صحیح load_base_module_failed = FALSE;

// global environment
ایستا Scheme_Env    *environment = پوچ;
// output/error handlers
ایستا Scheme_Object *curout = پوچ;
ایستا Scheme_Object *curerr = پوچ;
// exn:vim exception
ایستا Scheme_Object *exn_catching_apply = پوچ;
ایستا Scheme_Object *exn_p = پوچ;
ایستا Scheme_Object *exn_message = پوچ;
ایستا Scheme_Object *vim_exn = پوچ; // Vim Error exception

#اگر !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
ایستا تهی *پشته_base = پوچ;
#endif

ایستا long range_start;
ایستا long range_end;

// MzScheme threads scheduling stuff
ایستا صحیح mz_threads_allow = 0;

#اگر defined(FEAT_GUI_MSWIN)
ایستا تهی CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ایستا UINT timer_id = 0;
#وگرنهاگر defined(FEAT_GUI_GTK)
ایستا gboolean timer_proc(gpointer);
ایستا guint timer_id = 0;
#وگرنهاگر defined(FEAT_GUI_MOTIF)
ایستا تهی timer_proc(XtPointer, XtIntervalId *);
ایستا XtIntervalId timer_id = (XtIntervalId)0;
#endif

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    تهی
mzvim_check_threads(تهی)
{
    // Last time MzScheme threads were scheduled
    ایستا time_t mz_last_time = 0;

    اگر (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(پوچ);

	اگر ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#اگر defined(MZSCHEME_GUI_THREADS)
ایستا تهی setup_timer(تهی);
ایستا تهی remove_timer(تهی);

// timers are presented in GUI only
# اگر defined(FEAT_GUI_MSWIN)
    ایستا تهی CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_پیام UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    ایستا gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    ایستا تهی
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# اگر defined(FEAT_GUI_GTK)
    بازگشت TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    اگر (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, پوچ);
# endif
}

    ایستا تهی
setup_timer(تهی)
{
# اگر defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(پوچ, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, پوچ);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, پوچ);
# endif
}

    ایستا تهی
remove_timer(تهی)
{
# اگر defined(FEAT_GUI_MSWIN)
    KillTimer(پوچ, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    نویسه *
did_set_mzquantum(optset_T *args UNUSED)
{
#اگر defined(MZSCHEME_GUI_THREADS)
    // reset timer
    اگر (timer_id != 0)
	remove_timer();
    اگر (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    بازگشت پوچ;
}

    ایستا تهی
notify_multithread(صحیح on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    اگر (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    اگر (!on && timer_id != 0)
	remove_timer();
#endif
}

    تهی
mzscheme_end(تهی)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#اگر 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#اگر HAVE_TLS_SPACE && !defined(VIMDLL)
# اگر defined(_MSC_VER)
ایستا __declspec(thread) تهی *tls_space;
بیرونی intptr_t _tls_index;
# elif defined(__MINGW32__)
ایستا __thread تهی *tls_space;
بیرونی intptr_t _tls_index;
# وگرنه
ایستا THREAD_LOCAL تهی *tls_space;
ایستا intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    صحیح
mzscheme_main(تهی)
{
    صحیح	    argc = 0;
    نویسه    *argv = پوچ;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    اگر (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	بازگشت vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    تهی **ptls_space;
    intptr_t tls_index;
    تهی (*pget_tls_info)(تهی ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (تهی *)GetProcAddress(
				GetModuleHandle(پوچ), "get_tls_info");
    اگر (pget_tls_info == پوچ)
    {
	disabled = TRUE;
	بازگشت vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# وگرنه
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    بازگشت scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#وگرنه
    بازگشت mzscheme_env_main(پوچ, argc, &argv);
#endif
}

    ایستا صحیح
mzscheme_env_main(Scheme_Env *env, صحیح argc UNUSED, نویسه **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#وگرنه
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = پوچ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    پشته_base = &__gc_var_stack__;
# وگرنه
    صحیح dummy = 0;
    پشته_base = (تهی *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    بازگشت 0;
}

    ایستا Scheme_Object*
load_base_module(تهی *data)
{
    scheme_namespace_require(scheme_intern_نماد((نویسه *)data));
    بازگشت scheme_null;
}

    ایستا Scheme_Object *
load_base_module_on_error(تهی *data UNUSED)
{
    load_base_module_failed = TRUE;
    بازگشت scheme_null;
}

    ایستا صحیح
startup_mzscheme(تهی)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_پشته_base(پشته_base, 1);
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
#اگر MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = پوچ;
	صحیح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	اگر (s != پوچ)
	{
	    coll_path = scheme_make_path((نویسه *)s);
	    MZ_GC_CHECK();
	    اگر (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	اگر (coll_path == پوچ)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	اگر (coll_path != پوچ)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#وگرنه
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = پوچ;
	Scheme_Object	*coll_pair = پوچ;
	Scheme_Config	*config = پوچ;

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

#اگر MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = پوچ;
	صحیح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	اگر (s != پوچ)
	{
	    config_path = scheme_make_path((نویسه *)s);
	    MZ_GC_CHECK();
	    اگر (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	اگر (config_path == پوچ)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	اگر (config_path != پوچ)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#اگر MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(پوچ, load_base_module, پوچ,
				    load_base_module_on_error, "racket/base");
	اگر (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(پوچ, load_base_module, پوچ,
				    load_base_module_on_error, "scheme/base");
	    اگر (load_base_module_failed)
		بازگشت -1;
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
	Scheme_Object	*make_security_guard = پوچ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# اگر MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_نماد = پوچ;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_نماد);
	    MZ_GC_REG();
	    make_security_guard_نماد = scheme_intern_نماد("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_نماد, environment);
	    MZ_GC_UNREG();
	}
# وگرنه
	make_security_guard = scheme_builtin_مقدار("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	اگر (make_security_guard != پوچ)
	{
	    Scheme_Object   *args[3] = {پوچ, پوچ, پوچ};
	    Scheme_Object   *guard = پوچ;
	    Scheme_Config   *config = پوچ;
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

    بازگشت 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ایستا صحیح
mzscheme_init(تهی)
{
    اگر (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	اگر (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    بازگشت -1;
	}
#endif
	اگر (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    بازگشت -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = پوچ;
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

    بازگشت 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ایستا صحیح
eval_with_exn_handling(
    تهی		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *مقدار = پوچ;
    Scheme_Object   *exn = پوچ;
    Scheme_Object   *prim = پوچ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, مقدار);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    مقدار = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    اگر (!مقدار)
    {
	مقدار = extract_exn_message(exn);
	// Got an exn?
	اگر (مقدار)
	{
	    scheme_display(مقدار, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	بازگشت FAIL;
    }

    اگر (ret != پوچ)	// if pointer to retval supported give it up
	*ret = مقدار;
    // Print any result, as long as it's not a void
    وگرنه اگر (!SCHEME_VOIDP(مقدار))
    {
	scheme_display(مقدار, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    بازگشت OK;
}

/*
 * :mzscheme
 */
    ایستا صحیح
do_mzscheme_command(exarg_T *eap, تهی *data, Scheme_Closed_Prim *what)
{
    اگر (mzscheme_init())
	بازگشت FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    بازگشت eval_with_exn_handling(data, what, پوچ);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    تهی
mzscheme_buffer_free(buf_T *buf)
{
    اگر (buf->b_mzscheme_ref == پوچ)
	بازگشت;

    vim_mz_buffer *bp = پوچ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_مقدار;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#وگرنه
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = پوچ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    تهی
mzscheme_window_free(win_T *win)
{
    اگر (win->w_mzscheme_ref == پوچ)
	بازگشت;

    vim_mz_window *wp = پوچ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_مقدار;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#وگرنه
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = پوچ;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    تهی
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    اگر (eap->skip)
	بازگشت;

    اگر (script == پوچ)
	do_mzscheme_command(eap, eap->arg, do_eval);
    وگرنه
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ایستا Scheme_Object *
do_load(تهی *data, صحیح noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *عبارت = پوچ;
    Scheme_Object   *result = پوچ;
    نویسه	    *file = پوچ;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, عبارت);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (نویسه *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_درون_داد_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_شمار_سطرها(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    وقتی (!SCHEME_EOFP(عبارت = scheme_read(pinfo->port)))
    {
	result = scheme_eval(عبارت, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_درون_داد_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = پوچ;
    MZ_GC_UNREG();
    بازگشت result;
}

/*
 * :mzfile
 */
    تهی
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {پوچ, پوچ};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (نویسه *)eap->arg;
    اگر (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != پوچ)	// looks like port was not closed
    {
	scheme_close_درون_داد_port(pinfo.port);
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
    ایستا تهی
init_exn_catching_apply(تهی)
{
    اگر (exn_catching_apply)
	بازگشت;

    نویسه *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_مقدار("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_مقدار("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ایستا Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    اگر (SCHEME_TRUEP(SCHEME_CAR(v)))
	بازگشت SCHEME_CDR(v);
    وگرنه
    {
	*exn = SCHEME_CDR(v);
	بازگشت پوچ;
    }
}

    ایستا Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    اگر (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	بازگشت _scheme_apply(exn_message, 1, &v);
    وگرنه
	بازگشت پوچ; // Not an exn structure
}

    ایستا Scheme_Object *
do_eval(تهی *s, صحیح noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    بازگشت scheme_eval_string_all((نویسه *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ایستا تهی
do_intrnl_output(نویسه *mesg, صحیح error)
{
    نویسه *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    وقتی (p)
    {
	*p = '\0';
	اگر (error)
	    emsg(prev);
	وگرنه
	    پیام(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    اگر (error)
	emsg(prev);
    وگرنه
	پیام(prev);
}

    ایستا تهی
do_output(نویسه *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ایستا تهی
do_err_output(نویسه *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ایستا تهی
do_printf(نویسه *format, ...)
{
    do_intrnl_output(format, 1);
}

    ایستا تهی
do_flush(تهی)
{
    نویسه *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    اگر (length)
    {
	do_err_output(buff);
	بازگشت;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    اگر (length)
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
    ایستا Scheme_Object *
vim_command(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = پوچ;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_مقدار(cmd), پوچ, پوچ, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    بازگشت scheme_void;
}

/*
 * (eval {expr-string})
 */
    ایستا Scheme_Object *
vim_eval(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = پوچ;
    typval_T		*vim_result;
    Scheme_Object *عبارت = پوچ;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, عبارت);
    MZ_GC_REG();
    عبارت = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_عبارت(BYTE_STRING_مقدار(عبارت), پوچ);

    اگر (vim_result == پوچ)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    بازگشت result;
#وگرنه
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    بازگشت scheme_false;
#endif
}

/*
 * (range-start)
 */
    ایستا Scheme_Object *
get_range_start(
    تهی		*data UNUSED,
    صحیح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    بازگشت scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    ایستا Scheme_Object *
get_range_end(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
    بازگشت scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    ایستا Scheme_Object *
mzscheme_beep(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    بازگشت scheme_void;
}

ایستا Scheme_Object *M_global = پوچ;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ایستا Scheme_Object *
get_option(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    مقدار;
    نویسه	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = پوچ;
    Scheme_Object   *name = پوچ;
    صحیح		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    اگر (argc > 1)
    {
	اگر (M_global == پوچ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_نماد("global");
	    MZ_GC_CHECK();
	}

	اگر (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وگرنه اگر (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	وگرنه اگر (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	وگرنه
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_مقدار(BYTE_STRING_مقدار(name), &مقدار, (char_u **)&strval,
								  پوچ, scope);
    curbuf = save_curb;
    curwin = save_curw;

    انتخاب (rc)
    {
    حالت gov_bool:
    حالت gov_عدد:
	MZ_GC_UNREG();
	بازگشت scheme_make_integer_مقدار(مقدار);
    حالت gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	بازگشت rval;
    حالت gov_hidden_bool:
    حالت gov_hidden_عدد:
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
    بازگشت scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ایستا Scheme_Object *
set_option(تهی *data, صحیح argc, Scheme_Object **argv)
{
    char_u	*command = پوچ;
    صحیح		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = پوچ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    اگر (argc > 1)
    {
	اگر (M_global == پوچ)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_نماد("global");
	    MZ_GC_CHECK();
	}

	اگر (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وگرنه اگر (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	وگرنه اگر (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	وگرنه
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_مقدار(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    بازگشت scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ایستا Scheme_Object *
get_curr_win(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
    بازگشت (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    ایستا Scheme_Object *
get_window_شمار(
    تهی		*data UNUSED,
    صحیح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    صحیح	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    بازگشت scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ایستا Scheme_Object *
get_window_list(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    برای ( ; w != پوچ; w = w->w_next)
	اگر (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    بازگشت list;
}

    ایستا Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = پوچ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    اگر (win->w_mzscheme_ref != پوچ)
	بازگشت (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, اندازه(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(پوچ);
#وگرنه
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    بازگشت (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ایستا Scheme_Object *
get_window_رقم(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
    صحیح		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    برای (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    بازگشت scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ایستا Scheme_Object *
get_window_by_رقم(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    صحیح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    اگر (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    برای ( ; win != پوچ; win = win->w_next, --fnum)
	اگر (fnum == 1)	    // to be 1-based
	    بازگشت window_new(win);

    بازگشت scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ایستا Scheme_Object *
get_window_buffer(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    بازگشت buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    ایستا Scheme_Object *
get_window_height(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    بازگشت scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    ایستا Scheme_Object *
set_window_height(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    صحیح		    height;

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
    بازگشت scheme_void;
}

/*
 * (get-win-width [window])
 */
    ایستا Scheme_Object *
get_window_width(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    بازگشت scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    ایستا Scheme_Object *
set_window_width(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    صحیح		    width = 0;

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
    بازگشت scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ایستا Scheme_Object *
get_cursor(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    بازگشت scheme_make_pair(scheme_make_integer_مقدار((long)pos.lnum),
		    scheme_make_integer_مقدار((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ایستا Scheme_Object *
set_cursor(تهی *data, صحیح argc, Scheme_Object **argv)
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

    اگر (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_سطر_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    بازگشت scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ایستا Scheme_Object *
mzscheme_open_buffer(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    صحیح		    رقم = 0;
    Scheme_Object   *onum = پوچ;
    Scheme_Object   *buf = پوچ;
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
    رقم = buflist_add(BYTE_STRING_مقدار(fname), BLN_LISTED | BLN_CURBUF);

    اگر (رقم == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(رقم);
    buf = get_buffer_by_رقم(data, 1, &onum);
    MZ_GC_UNREG();
    بازگشت buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ایستا Scheme_Object *
get_buffer_by_رقم(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    صحیح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	اگر (buf->b_fnum == fnum)
	    بازگشت buffer_new(buf);

    بازگشت scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ایستا Scheme_Object *
get_buffer_by_name(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = پوچ;
    Scheme_Object *fname = پوچ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	اگر (buf->b_ffname == پوچ || buf->b_sfname == پوچ)
	    // empty string
	{
	    اگر (BYTE_STRING_مقدار(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	وگرنه اگر (!fnamecmp(buf->b_ffname, BYTE_STRING_مقدار(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_مقدار(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    بازگشت buffer;
}

/*
 * (get-next-buff [buffer])
 */
    ایستا Scheme_Object *
get_next_buffer(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    اگر (buf->b_next == پوچ)
	بازگشت scheme_false;
    وگرنه
	بازگشت buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    ایستا Scheme_Object *
get_prev_buffer(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    اگر (buf->b_prev == پوچ)
	بازگشت scheme_false;
    وگرنه
	بازگشت buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ایستا Scheme_Object *
get_buffer_رقم(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    بازگشت scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ایستا Scheme_Object *
get_buffer_شمار(
    تهی		*data UNUSED,
    صحیح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    صحیح	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    بازگشت scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ایستا Scheme_Object *
get_buffer_name(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    بازگشت scheme_make_byte_string((نویسه *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ایستا Scheme_Object *
get_curr_buffer(
    تهی		*data UNUSED,
    صحیح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    بازگشت (Scheme_Object *)get_vim_curr_buffer();
}

    ایستا Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = پوچ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    اگر (buf->b_mzscheme_ref)
	بازگشت (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, اندازه(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(پوچ);
#وگرنه
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    بازگشت (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ایستا Scheme_Object *
get_buffer_size(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    بازگشت scheme_make_integer(buf->buf->b_ml.ml_سطر_شمار);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ایستا Scheme_Object *
get_buffer_سطر(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    صحیح		    linenr;
    char_u	    *سطر;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    سطر = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    بازگشت scheme_make_byte_string((نویسه *)سطر);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ایستا Scheme_Object *
get_buffer_سطر_list(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    صحیح		    i, hi, lo, n;
    Scheme_Object   *list = پوچ;

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
    اگر (lo < 0)
	lo = 0;

    اگر (hi < 0)
	hi = 0;
    اگر (hi < lo)
	hi = lo;

    n = hi - lo;

    برای (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (نویسه *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    بازگشت list;
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
    ایستا Scheme_Object *
set_buffer_سطر(تهی *data, صحیح argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *سطر = پوچ;
    نویسه	    *save;
    صحیح		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, سطر);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    اگر (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    سطر = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

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
	وگرنه اگر (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	اگر (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_سطرها_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	بازگشت scheme_void;
    }
    وگرنه
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_سطر(سطر);

	curbuf = buf->buf;

	اگر (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وگرنه اگر (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	وگرنه
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	اگر (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	بازگشت scheme_void;
    }
}

    ایستا تهی
free_array(نویسه **array)
{
    نویسه **curr = array;
    وقتی (*curr != پوچ)
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
    ایستا Scheme_Object *
set_buffer_سطر_list(تهی *data, صحیح argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = پوچ;
    Scheme_Object   *سطر_list = پوچ;
    صحیح		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, سطر_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    اگر (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    سطر_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
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

	اگر (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	وگرنه
	{
	    برای (i = 0; i < old_len; i++)
		اگر (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    اگر (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_سطرها_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	بازگشت scheme_void;
    }
    وگرنه
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(سطر_list);
	MZ_GC_CHECK();
	اگر (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	وگرنه
	{
	    نویسه		**array = پوچ;
	    Scheme_Object   *سطر = پوچ;
	    Scheme_Object   *rest = پوچ;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, سطر);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(نویسه *, new_len + 1);

	    rest = سطر_list;
	    برای (i = 0; i < new_len; ++i)
	    {
		سطر = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		اگر (!SCHEME_STRINGP(سطر))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_سطر(سطر);
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
	    برای (i = 0; i < old_len - new_len; ++i)
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
	    برای (i = 0; i < old_len && i < new_len; i++)
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
	    وقتی (i < new_len)
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
						  (long)MAXLNUM, (long)extra);
	changed_سطرها((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	اگر (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	بازگشت scheme_void;
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
    ایستا Scheme_Object *
درج_buffer_سطر_list(تهی *data, صحیح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = پوچ;
    Scheme_Object   *list = پوچ;
    نویسه	    *str = پوچ;
    صحیح		    i, n, size;

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

    اگر (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    اگر (n != 0)	    // 0 can be used in insert
	check_سطر_range(n, buf->buf);
    اگر (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_سطر(list);
	curbuf = buf->buf;

	اگر (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وگرنه اگر (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	وگرنه
	{
	    vim_free(str);
	    appended_سطرها_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	بازگشت scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    اگر (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    وگرنه
    {
	Scheme_Object   *سطر = پوچ;
	Scheme_Object   *rest = پوچ;
	نویسه		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, سطر);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(نویسه *, size + 1);

	rest = list;
	برای (i = 0; i < size; ++i)
	{
	    سطر = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_سطر(سطر);
	}

	curbuf = buf->buf;

	اگر (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وگرنه
	{
	    برای (i = 0; i < size; ++i)
		اگر (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    اگر (i > 0)
		appended_سطرها_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    بازگشت scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ایستا Scheme_Object *
vim_bufferp(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv)
{
    اگر (SCHEME_VIMBUFFERP(argv[0]))
	بازگشت scheme_true;
    وگرنه
	بازگشت scheme_false;
}

/*
 * (win? obj)
 */
    ایستا Scheme_Object *
vim_windowp(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv)
{
    اگر (SCHEME_VIMWINDOWP(argv[0]))
	بازگشت scheme_true;
    وگرنه
	بازگشت scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ایستا Scheme_Object *
vim_buffer_validp(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv)
{
    اگر (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_مقدار)
	بازگشت scheme_true;
    وگرنه
	بازگشت scheme_false;
}

/*
 * (win-valid? obj)
 */
    ایستا Scheme_Object *
vim_window_validp(تهی *data UNUSED, صحیح argc UNUSED, Scheme_Object **argv)
{
    اگر (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_مقدار)
	بازگشت scheme_true;
    وگرنه
	بازگشت scheme_false;
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
    ایستا نویسه *
string_to_سطر(Scheme_Object *obj)
{
    نویسه	*scheme_str = پوچ;
    نویسه	*vim_str = پوچ;
    OUTPUT_LEN_TYPE	len;
    صحیح		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    اگر (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    برای (i = 0; i < len; ++i)
    {
	اگر (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	وگرنه
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    بازگشت vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ایستا Scheme_Object *
vim_to_mzscheme(typval_T *vim_مقدار)
{
    Scheme_Object	*result = پوچ;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = پوچ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_مقدار, 1, visited);

    MZ_GC_UNREG();
    بازگشت result;
}

    ایستا Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_مقدار,
    صحیح			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = پوچ;
    صحیح		    new_مقدار_2 = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    اگر (depth > 100)
    {
	MZ_GC_UNREG();
	بازگشت scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_مقدار);
    MZ_GC_CHECK();
    اگر (result != پوچ) // found, do nothing
	new_مقدار_2 = FALSE;
    وگرنه اگر (vim_مقدار->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((نویسه *)vim_مقدار->vval.v_string);
	MZ_GC_CHECK();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_عدد)
    {
	result = scheme_make_integer((long)vim_مقدار->vval.v_عدد);
	MZ_GC_CHECK();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((دوگانه)vim_مقدار->vval.v_float);
	MZ_GC_CHECK();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_LIST)
    {
	list_T		*list = vim_مقدار->vval.v_list;
	listitem_T	*curr;

	اگر (list == پوچ || list->lv_first == پوچ)
	    result = scheme_null;
	وگرنه
	{
	    Scheme_Object   *obj = پوچ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    وقتی (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_DICT)
    {
	Scheme_Object	  *کلید = پوچ;
	Scheme_Object	  *obj = پوچ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, کلید);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	اگر (vim_مقدار->vval.v_dict != پوچ)
	{
	    hashtab_T	*ht = &vim_مقدار->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
	    {
		اگر (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    کلید = scheme_make_byte_string((نویسه *)hi->hi_کلید);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, کلید, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = پوچ;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((نویسه *)vim_مقدار->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ثابت نویسه *)BYTE_STRING_مقدار(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_PARTIAL)
    {
	اگر (vim_مقدار->vval.v_partial == پوچ)
	    result = scheme_null;
	وگرنه
	{
	    Scheme_Object *funcname = پوچ;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (نویسه *)partial_name(vim_مقدار->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ثابت نویسه *)BYTE_STRING_مقدار(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    وگرنه اگر (vim_مقدار->v_type == VAR_BOOL || vim_مقدار->v_type == VAR_SPECIAL)
    {
	اگر (vim_مقدار->vval.v_عدد <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_مقدار->vval.v_عدد);
	وگرنه
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    وگرنه
    {
	result = scheme_void;
	new_مقدار_2 = FALSE;
    }
    اگر (new_مقدار_2)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_مقدار, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    بازگشت result;
}

    ایستا صحیح
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    صحیح i, status;
    Scheme_Hash_Table	*visited = پوچ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    برای (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	اگر (visited->vals[i] != پوچ)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    بازگشت status;
}
    ایستا صحیح
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, صحیح depth,
	Scheme_Hash_Table *visited)
{
    صحیح		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    اگر (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = 0;
	بازگشت FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    اگر (found != پوچ)
	copy_tv(found, tv);
    وگرنه اگر (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_عدد = VVAL_NULL;
    }
    وگرنه اگر (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = SCHEME_INT_VAL(obj);
    }
    وگرنه اگر (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_عدد = SCHEME_TRUEP(obj);
    }
    وگرنه اگر (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    وگرنه اگر (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_مقدار(obj));
    }
# اگر MZSCHEME_VERSION_MAJOR >= 299
    وگرنه اگر (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = پوچ;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_مقدار(tmp));
	MZ_GC_UNREG();
    }
# endif
    وگرنه اگر (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	اگر (list == پوچ)
	    status = FAIL;
	وگرنه
	{
	    صحیح		    i;
	    Scheme_Object   *curr = پوچ;
	    Scheme_Object   *cval = پوچ;
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
	    اگر (v == پوچ)
		status = FAIL;
	    وگرنه
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		اگر (SCHEME_VECTORP(obj))
		{
		    برای (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			اگر (status == FAIL)
			    بشکن;
			status = list_append_tv(list, v);
			clear_tv(v);
			اگر (status == FAIL)
			    بشکن;
		    }
		}
		وگرنه اگر (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    برای (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			اگر (status == FAIL)
			    بشکن;
			status = list_append_tv(list, v);
			clear_tv(v);
			اگر (status == FAIL)
			    بشکن;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    اگر (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			اگر (status == OK)
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
    وگرنه اگر (SCHEME_HASHTP(obj))
    {
	صحیح		i;
	dict_T		*dict;
	Scheme_Object   *کلید = پوچ;
	Scheme_Object   *val = پوچ;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, کلید);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	اگر (dict == پوچ)
	    status = FAIL;
	وگرنه
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    برای (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		اگر (((Scheme_Hash_Table *) obj)->vals[i] != پوچ)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_سطر(
				((Scheme_Hash_Table *) obj)->کلیدها[i]));
		    // convert Scheme val to Vim and add it to the dict
		    اگر (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			بشکن;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    وگرنه
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_سطر(obj);
    }
    MZ_GC_UNREG();
    بازگشت status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ایستا Scheme_Object *
vim_funcref(تهی *name, صحیح argc, Scheme_Object **argv)
{
    صحیح i;
    typval_T args;
    صحیح status = OK;
    Scheme_Object *result = پوچ;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    اگر (list == پوچ)
	status = FAIL;
    وگرنه
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	برای (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    اگر (v == پوچ)
		status = FAIL;
	    وگرنه
	    {
		status = mzscheme_to_vim(argv[i], v);
		اگر (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	اگر (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_مقدار((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    اگر (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    وگرنه
	raise_if_error();
    بازگشت result;
}

    تهی
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = پوچ;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    اگر (mzscheme_init())
    {
	MZ_GC_UNREG();
	بازگشت;
    }

    MZ_GC_CHECK();
    اگر (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ایستا صحیح
vim_error_check(تهی)
{
    بازگشت (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ایستا تهی
register_vim_exn(تهی)
{
    صحیح	nc = 0;
    صحیح i;
    Scheme_Object   *struct_exn = پوچ;
    Scheme_Object   *exn_name = پوچ;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_نماد("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_مقدار("struct:exn");
    MZ_GC_CHECK();

    اگر (vim_exn == پوچ)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, پوچ, 0, 0, پوچ, پوچ
#اگر MZSCHEME_VERSION_MAJOR >= 299
		, پوچ
#endif
		);


    {
	Scheme_Object   **tmp = پوچ;
	Scheme_Object   *exn_names[5] = {پوچ, پوچ, پوچ, پوچ, پوچ};
	Scheme_Object   *exn_مقادیر[5] = {پوچ, پوچ, پوچ, پوچ, پوچ};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_مقادیر, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * اندازه(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_مقادیر(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_مقادیر, tmp, nc * اندازه(Scheme_Object *));
	MZ_GC_CHECK();

	برای (i = 0; i < nc; i++)
	{
	    scheme_add_global_نماد(exn_names[i],
		    exn_مقادیر[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    تهی
raise_vim_exn(ثابت نویسه *add_info)
{
    نویسه	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {پوچ, پوچ};
    Scheme_Object   *exn = پوچ;
    Scheme_Object   *byte_string = پوچ;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    اگر (add_info != پوچ)
    {
	نویسه		*c_string = پوچ;
	Scheme_Object   *info = پوچ;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (صحیح)STRLEN(fmt), 1, &info, پوچ);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    وگرنه
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#اگر MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#وگرنه
    argv[1] = scheme_current_continuation_marks(پوچ);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    تهی
raise_if_error(تهی)
{
    اگر (vim_error_check())
	raise_vim_exn(پوچ);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ایستا vim_mz_buffer *
get_buffer_arg(ثابت نویسه *fname, صحیح argnum, صحیح argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    اگر (argc < argnum + 1)
	بازگشت get_vim_curr_buffer();
    اگر (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (تهی)get_valid_buffer(argv[argnum]);
    بازگشت b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ایستا vim_mz_window *
get_window_arg(ثابت نویسه *fname, صحیح argnum, صحیح argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    اگر (argc < argnum + 1)
	بازگشت get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    اگر (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (تهی)get_valid_window(argv[argnum]);
    بازگشت w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(تهی *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    اگر (buf == INVALID_BUFFER_مقدار)
	scheme_signal_error(_("buffer is invalid"));
    بازگشت buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(تهی *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    اگر (win == INVALID_WINDOW_مقدار)
	scheme_signal_error(_("window is invalid"));
    بازگشت win;
}

    صحیح
mzthreads_allowed(تهی)
{
    بازگشت mz_threads_allow;
}

    ایستا صحیح
سطر_in_range(linenr_T lnum, buf_T *buf)
{
    بازگشت (lnum > 0 && lnum <= buf->b_ml.ml_سطر_شمار);
}

    ایستا تهی
check_سطر_range(linenr_T lnum, buf_T *buf)
{
    اگر (!سطر_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ایستا تهی
mz_fix_cursor(صحیح lo, صحیح hi, صحیح extra)
{
    اگر (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	اگر (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	وگرنه اگر (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وگرنه
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

ایستا Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_سطر, "get-buff-line", 1, 2},
    {set_buffer_سطر, "set-buff-line", 2, 3},
    {get_buffer_سطر_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_رقم, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_سطر_list, "set-buff-line-list", 3, 4},
    {درج_buffer_سطر_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_شمار, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_رقم, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_شمار, "win-count", 0, 0},
    {get_window_by_رقم, "get-win-by-num", 1, 1},
    {get_window_رقم, "get-win-num", 0, 1},
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
    ایستا vim_mz_buffer *
get_vim_curr_buffer(تهی)
{
    اگر (curbuf->b_mzscheme_ref == پوچ)
	بازگشت (vim_mz_buffer *)buffer_new(curbuf);
    وگرنه
	بازگشت BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ایستا vim_mz_window *
get_vim_curr_window(تهی)
{
    اگر (curwin->w_mzscheme_ref == پوچ)
	بازگشت (vim_mz_window *)window_new(curwin);
    وگرنه
	بازگشت WINDOW_REF(curwin);
}

    ایستا تهی
make_modules(تهی)
{
    صحیح		    i;
    Scheme_Env	    *mod = پوچ;
    Scheme_Object   *vimext_نماد = پوچ;
    Scheme_Object   *closed_prim = پوچ;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_نماد);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_نماد = scheme_intern_نماد("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_نماد, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(prims); i++)
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
ایستا Scheme_Object *M_write = پوچ;
ایستا Scheme_Object *M_read = پوچ;
ایستا Scheme_Object *M_execute = پوچ;
ایستا Scheme_Object *M_delete = پوچ;

    ایستا تهی
sandbox_check(تهی)
{
    اگر (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ایستا Scheme_Object *
sandbox_file_guard(صحیح argc UNUSED, Scheme_Object **argv)
{
    اگر (!sandbox)
	بازگشت scheme_void;

    Scheme_Object *requested_access = argv[2];

    اگر (M_write == پوچ)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_نماد("write");
	MZ_GC_CHECK();
    }
    اگر (M_read == پوچ)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_نماد("read");
	MZ_GC_CHECK();
    }
    اگر (M_execute == پوچ)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_نماد("execute");
	MZ_GC_CHECK();
    }
    اگر (M_delete == پوچ)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_نماد("delete");
	MZ_GC_CHECK();
    }

    وقتی (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	اگر (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    بازگشت scheme_void;
}

    ایستا Scheme_Object *
sandbox_network_guard(صحیح argc UNUSED, Scheme_Object **argv UNUSED)
{
    بازگشت scheme_void;
}
#endif

#endif
