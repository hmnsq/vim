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

#ضمن "vim.h"

#ضمن "if_mzsch.h"

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#𐡄𐡍 defined(FEAT_MZSCHEME)

/*
 * scheme_register_tls_space is only available on 32-bit Windows until
 * racket-6.3.  See
 * http://docs.racket-lang.org/inside/im_memoryalloc.html?q=scheme_register_tls_space
 */
#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 500 && defined(MSWIN) \
	&& defined(USE_THREAD_LOCAL) \
	&& (!defined(_WIN64) || MZSCHEME_VERSION_MAJOR >= 603)
# define HAVE_TLS_SPACE 1
#endif

/*
 * Since version 4.x precise GC requires trampolined startup.
 * Futures and places in version 5.x need it too.
 */
#𐡄𐡍 defined(MZ_PRECISE_GC) && MZSCHEME_VERSION_MAJOR >= 400 \
    || MZSCHEME_VERSION_MAJOR >= 500 \
	&& (defined(MZ_USE_FUTURES) || defined(MZ_USE_PLACES))
# define TRAMPOLINED_MZVIM_STARTUP
#endif

// Base data structures
#define SCHEME_VIMBUFFERP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_buffer_type)
#define SCHEME_VIMWINDOWP(obj)  SAME_TYPE(SCHEME_TYPE(obj), mz_window_type)

تعريفنوع بنية
{
    Scheme_Object   so;
    buf_T	    *buf;
} vim_mz_buffer;

#define INVALID_BUFFER_VALUE ((buf_T *)(-1))

تعريفنوع بنية
{
    Scheme_Object   so;
    win_T	    *win;
} vim_mz_window;

#define INVALID_WINDOW_VALUE ((win_T *)(-1))

/*
 * Prims that form MzScheme Vim interface
 */
تعريفنوع بنية
{
    Scheme_Closed_Prim	*prim;
    حرف	*name;
    صحيح		mina;	// arity information
    صحيح		maxa;
} Vim_Prim;

تعريفنوع بنية
{
    حرف	    *name;
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
ساكن Scheme_Object *sandbox_file_guard(صحيح, Scheme_Object **);
ساكن Scheme_Object *sandbox_network_guard(صحيح, Scheme_Object **);
ساكن فراغ sandbox_check(فراغ);
#endif
//  Buffer-related commands
ساكن Scheme_Object *buffer_new(buf_T *buf);
ساكن Scheme_Object *get_buffer_by_num(فراغ *, صحيح, Scheme_Object **);
ساكن vim_mz_buffer *get_vim_curr_buffer(فراغ);

//  Window-related commands
ساكن Scheme_Object *window_new(win_T *win);
ساكن vim_mz_window *get_vim_curr_window(فراغ);

/*
 *========================================================================
 *  Internal Function Prototypes
 *========================================================================
 */
ساكن صحيح vim_error_check(فراغ);
ساكن صحيح do_mzscheme_command(exarg_T *, فراغ *, Scheme_Closed_Prim *what);
ساكن صحيح startup_mzscheme(فراغ);
ساكن حرف *string_to_line(Scheme_Object *obj);
#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 501
# define OUTPUT_LEN_TYPE intptr_t
#وإلا
# define OUTPUT_LEN_TYPE long
#endif
ساكن فراغ do_output(حرف *mesg, OUTPUT_LEN_TYPE len);
ساكن فراغ do_printf(حرف *format, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
ساكن فراغ do_flush(فراغ);
ساكن Scheme_Object *_apply_thunk_catch_exceptions(
	Scheme_Object *, Scheme_Object **);
ساكن Scheme_Object *extract_exn_message(Scheme_Object *v);
ساكن Scheme_Object *do_eval(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن Scheme_Object *do_load(فراغ *, صحيح noargc, Scheme_Object **noargv);
ساكن فراغ register_vim_exn(فراغ);
ساكن vim_mz_buffer *get_buffer_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن vim_mz_window *get_window_arg(ثابت حرف *fname, صحيح argnum,
	صحيح argc, Scheme_Object **argv);
ساكن صحيح line_in_range(linenr_T, buf_T *);
ساكن فراغ check_line_range(linenr_T, buf_T *);
ساكن فراغ mz_fix_cursor(صحيح lo, صحيح hi, صحيح extra);

ساكن صحيح eval_with_exn_handling(فراغ *, Scheme_Closed_Prim *,
	    Scheme_Object **ret);
ساكن فراغ make_modules(فراغ);
ساكن فراغ init_exn_catching_apply(فراغ);
ساكن صحيح mzscheme_env_main(Scheme_Env *env, صحيح argc, حرف **argv);
ساكن صحيح mzscheme_init(فراغ);
#ifdef FEAT_EVAL
ساكن Scheme_Object *vim_to_mzscheme(typval_T *vim_value);
ساكن Scheme_Object *vim_to_mzscheme_impl(typval_T *vim_value, صحيح depth,
	Scheme_Hash_Table *visited);
ساكن صحيح mzscheme_to_vim(Scheme_Object *obj, typval_T *tv);
ساكن صحيح mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح depth,
	Scheme_Hash_Table *visited);
ساكن Scheme_Object *vim_funcref(فراغ *data, صحيح argc, Scheme_Object **argv);
#endif

#ifdef MZ_PRECISE_GC
ساكن صحيح buffer_size_proc(فراغ *obj UNUSED)
{
    أرجع gcBYTES_TO_WORDS(حجم(vim_mz_buffer));
}
ساكن صحيح buffer_mark_proc(فراغ *obj)
{
    أرجع buffer_size_proc(obj);
}
ساكن صحيح buffer_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the buffer is alive
    // vim_mz_buffer* buf = (vim_mz_buffer*) obj;
    // buf->buf->b_mzscheme_ref = GC_fixup_self(obj);

    أرجع buffer_size_proc(obj);
}
ساكن صحيح window_size_proc(فراغ *obj UNUSED)
{
    أرجع gcBYTES_TO_WORDS(حجم(vim_mz_window));
}
ساكن صحيح window_mark_proc(فراغ *obj)
{
    أرجع window_size_proc(obj);
}
ساكن صحيح window_fixup_proc(فراغ *obj)
{
    // apparently not needed as the object will be uncollectable while
    // the window is alive
    // vim_mz_window* win = (vim_mz_window*) obj;
    // win->win->w_mzscheme_ref = GC_fixup_self(obj);
    //
    أرجع window_size_proc(obj);
}
// with precise GC, w_mzscheme_ref and b_mzscheme_ref are immobile boxes
// containing pointers to a window/buffer
// with conservative GC these are simply pointers
# define WINDOW_REF(win) *(vim_mz_window **)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) *(vim_mz_buffer **)((buf)->b_mzscheme_ref)
#وإلا
# define WINDOW_REF(win) (vim_mz_window *)((win)->w_mzscheme_ref)
# define BUFFER_REF(buf) (vim_mz_buffer *)((buf)->b_mzscheme_ref)
#endif

#𐡄𐡍 defined(DYNAMIC_MZSCHEME)
ساكن Scheme_Object *dll_scheme_eof;
ساكن Scheme_Object *dll_scheme_false;
ساكن Scheme_Object *dll_scheme_void;
ساكن Scheme_Object *dll_scheme_null;
ساكن Scheme_Object *dll_scheme_true;

ساكن Scheme_Thread **dll_scheme_current_thread_ptr;

ساكن فراغ (**dll_scheme_console_printf_ptr)(حرف *str, ...);
ساكن فراغ (**dll_scheme_console_output_ptr)(حرف *str, OUTPUT_LEN_TYPE len);
ساكن فراغ (**dll_scheme_notify_multithread_ptr)(صحيح on);

ساكن فراغ *(*dll_GC_malloc)(size_t size_in_bytes);
ساكن فراغ *(*dll_GC_malloc_atomic)(size_t size_in_bytes);
ساكن Scheme_Env *(*dll_scheme_basic_env)(فراغ);
ساكن فراغ (*dll_scheme_check_threads)(فراغ);
ساكن فراغ (*dll_scheme_register_static)(فراغ *ptr, long size);
ساكن فراغ (*dll_scheme_set_stack_base)(فراغ *base, صحيح no_auto_statics);
ساكن فراغ (*dll_scheme_add_global)(ثابت حرف *name, Scheme_Object *val,
	Scheme_Env *env);
ساكن فراغ (*dll_scheme_add_global_symbol)(Scheme_Object *name,
	Scheme_Object *val, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_apply)(Scheme_Object *rator, صحيح num_rands,
	Scheme_Object **rands);
ساكن Scheme_Object *(*dll_scheme_builtin_value)(ثابت حرف *name);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
ساكن Scheme_Object *(*dll_scheme_byte_string_to_char_string)(Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_make_path)(ثابت حرف *chars);
# endif
ساكن فراغ (*dll_scheme_close_input_port)(Scheme_Object *port);
ساكن فراغ (*dll_scheme_count_lines)(Scheme_Object *port);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 360
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(فراغ);
# وإلا
ساكن Scheme_Object *(*dll_scheme_current_continuation_marks)(Scheme_Object *prompt_tag);
# endif
ساكن فراغ (*dll_scheme_display)(Scheme_Object *obj, Scheme_Object *port);
ساكن حرف *(*dll_scheme_display_to_string)(Scheme_Object *obj, OUTPUT_LEN_TYPE *len);
ساكن صحيح (*dll_scheme_eq)(Scheme_Object *obj1, Scheme_Object *obj2);
ساكن Scheme_Object *(*dll_scheme_do_eval)(Scheme_Object *obj,
	صحيح _num_rands, Scheme_Object **rands, صحيح val);
ساكن فراغ (*dll_scheme_dont_gc_ptr)(فراغ *p);
ساكن Scheme_Object *(*dll_scheme_eval)(Scheme_Object *obj, Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string)(ثابت حرف *str,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_eval_string_all)(ثابت حرف *str,
	Scheme_Env *env, صحيح all);
ساكن فراغ (*dll_scheme_finish_primitive_module)(Scheme_Env *env);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
ساكن حرف *(*dll_scheme_format)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, long *rlen);
# وإلا
ساكن حرف *(*dll_scheme_format_utf8)(حرف *format, صحيح flen, صحيح argc,
	Scheme_Object **argv, OUTPUT_LEN_TYPE *rlen);
ساكن Scheme_Object *(*dll_scheme_get_param)(Scheme_Config *c, صحيح pos);
# endif
ساكن فراغ (*dll_scheme_gc_ptr_ok)(فراغ *p);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
ساكن حرف *(*dll_scheme_get_sized_string_output)(Scheme_Object *,
	long *len);
# وإلا
ساكن حرف *(*dll_scheme_get_sized_byte_string_output)(Scheme_Object *,
	OUTPUT_LEN_TYPE *len);
# endif
ساكن Scheme_Object *(*dll_scheme_intern_symbol)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_lookup_global)(Scheme_Object *symbol,
	Scheme_Env *env);
ساكن Scheme_Object *(*dll_scheme_make_closed_prim_w_arity)
    (Scheme_Closed_Prim *prim, فراغ *data, ثابت حرف *name, mzshort mina,
     mzshort maxa);
ساكن Scheme_Object *(*dll_scheme_make_integer_value)(long i);
ساكن Scheme_Object *(*dll_scheme_make_pair)(Scheme_Object *car,
	Scheme_Object *cdr);
ساكن Scheme_Object *(*dll_scheme_make_prim_w_arity)(Scheme_Prim *prim,
	ثابت حرف *name, mzshort mina, mzshort maxa);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
ساكن Scheme_Object *(*dll_scheme_make_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_string_output_port)();
# وإلا
ساكن Scheme_Object *(*dll_scheme_make_byte_string)(ثابت حرف *chars);
ساكن Scheme_Object *(*dll_scheme_make_byte_string_output_port)();
# endif
ساكن Scheme_Object *(*dll_scheme_make_struct_instance)(Scheme_Object *stype,
	صحيح argc, Scheme_Object **argv);
ساكن Scheme_Object **(*dll_scheme_make_struct_names)(Scheme_Object *base,
	Scheme_Object *field_names, صحيح flags, صحيح *count_out);
ساكن Scheme_Object *(*dll_scheme_make_struct_type)(Scheme_Object *base,
	Scheme_Object *parent, Scheme_Object *inspector, صحيح num_fields,
	صحيح num_uninit_fields, Scheme_Object *uninit_val,
	Scheme_Object *properties
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
	, Scheme_Object *guard
# endif
	);
ساكن Scheme_Object **(*dll_scheme_make_struct_values)(
	Scheme_Object *struct_type, Scheme_Object **names, صحيح count,
	صحيح flags);
ساكن Scheme_Type (*dll_scheme_make_type)(ثابت حرف *name);
ساكن Scheme_Object *(*dll_scheme_make_vector)(صحيح size,
	Scheme_Object *fill);
ساكن فراغ *(*dll_scheme_malloc_fail_ok)(فراغ *(*f)(size_t), size_t);
ساكن Scheme_Object *(*dll_scheme_open_input_file)(ثابت حرف *name,
	ثابت حرف *who);
ساكن Scheme_Env *(*dll_scheme_primitive_module)(Scheme_Object *name,
	Scheme_Env *for_env);
ساكن صحيح (*dll_scheme_proper_list_length)(Scheme_Object *list);
ساكن فراغ (*dll_scheme_raise)(Scheme_Object *exn);
ساكن Scheme_Object *(*dll_scheme_read)(Scheme_Object *port);
ساكن فراغ (*dll_scheme_signal_error)(ثابت حرف *msg, ...);
ساكن فراغ (*dll_scheme_wrong_type)(ثابت حرف *name, ثابت حرف *expected,
	صحيح which, صحيح argc, Scheme_Object **argv);
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
ساكن فراغ (*dll_scheme_set_param)(Scheme_Config *c, صحيح pos,
	Scheme_Object *o);
ساكن Scheme_Config *(*dll_scheme_current_config)(فراغ);
ساكن Scheme_Object *(*dll_scheme_char_string_to_byte_string)
    (Scheme_Object *s);
ساكن Scheme_Object *(*dll_scheme_char_string_to_path)
    (Scheme_Object *s);
ساكن فراغ *(*dll_scheme_set_collects_path)(Scheme_Object *p);
# endif
ساكن Scheme_Hash_Table *(*dll_scheme_make_hash_table)(صحيح type);
ساكن فراغ (*dll_scheme_hash_set)(Scheme_Hash_Table *table,
	Scheme_Object *key, Scheme_Object *value);
ساكن Scheme_Object *(*dll_scheme_hash_get)(Scheme_Hash_Table *table,
	Scheme_Object *key);
ساكن Scheme_Object *(*dll_scheme_make_double)(مزدوج d);
ساكن Scheme_Object *(*dll_scheme_make_sized_byte_string)(حرف *chars,
	long len, صحيح copy);
ساكن Scheme_Object *(*dll_scheme_namespace_require)(Scheme_Object *req);
ساكن Scheme_Object *(*dll_scheme_dynamic_wind)(فراغ (*pre)(فراغ *), Scheme_Object *(* متطاير act)(فراغ *), فراغ (* متطاير post)(فراغ *), Scheme_Object *(*jmp_handler)(فراغ *), فراغ * متطاير data);
# ifdef MZ_PRECISE_GC
ساكن فراغ *(*dll_GC_malloc_one_tagged)(size_t size_in_bytes);
ساكن فراغ (*dll_GC_register_traversers)(short tag, Size_Proc size, Mark_Proc mark, Fixup_Proc fixup, صحيح is_constant_size, صحيح is_atomic);
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 400
ساكن فراغ (*dll_scheme_init_collection_paths)(Scheme_Env *global_env, Scheme_Object *extra_dirs);
ساكن فراغ **(*dll_scheme_malloc_immobile_box)(فراغ *p);
ساكن فراغ (*dll_scheme_free_immobile_box)(فراغ **b);
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 500
#  ifdef TRAMPOLINED_MZVIM_STARTUP
ساكن صحيح (*dll_scheme_main_setup)(صحيح no_auto_statics, Scheme_Env_Main _main, صحيح argc, حرف **argv);
#   𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
ساكن فراغ (*dll_scheme_register_tls_space)(فراغ *tls_space, صحيح _tls_index);
#   endif
#  endif
#  𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
ساكن Thread_Local_Variables *(*dll_scheme_external_get_thread_local_variables)(فراغ);
#  endif
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 600
ساكن فراغ (*dll_scheme_embedded_load)(intptr_t len, ثابت حرف *s, صحيح predefined);
ساكن فراغ (*dll_scheme_register_embedded_load)(intptr_t len, ثابت حرف *s);
ساكن فراغ (*dll_scheme_set_config_path)(Scheme_Object *p);
# endif

# 𐡄𐡍 defined(DYNAMIC_MZSCHEME)

// arrays are imported directly
#  define scheme_eof dll_scheme_eof
#  define scheme_false dll_scheme_false
#  define scheme_void dll_scheme_void
#  define scheme_null dll_scheme_null
#  define scheme_true dll_scheme_true

// pointers are GetProcAddress'ed as pointers to pointer
#  𐡄𐡍 !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
#   define scheme_current_thread (*dll_scheme_current_thread_ptr)
#  endif
#  define scheme_console_printf (*dll_scheme_console_printf_ptr)
#  define scheme_console_output (*dll_scheme_console_output_ptr)
#  define scheme_notify_multithread (*dll_scheme_notify_multithread_ptr)

// and functions in a usual way
#  define GC_malloc dll_GC_malloc
#  define GC_malloc_atomic dll_GC_malloc_atomic

#  define scheme_add_global dll_scheme_add_global
#  define scheme_add_global_symbol dll_scheme_add_global_symbol
#  define scheme_apply dll_scheme_apply
#  define scheme_basic_env dll_scheme_basic_env
#  define scheme_builtin_value dll_scheme_builtin_value
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
#   define scheme_byte_string_to_char_string dll_scheme_byte_string_to_char_string
#   define scheme_make_path dll_scheme_make_path
#  endif
#  define scheme_check_threads dll_scheme_check_threads
#  define scheme_close_input_port dll_scheme_close_input_port
#  define scheme_count_lines dll_scheme_count_lines
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
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
#   define scheme_format dll_scheme_format
#  وإلا
#   define scheme_format_utf8 dll_scheme_format_utf8
#  endif
#  define scheme_gc_ptr_ok dll_scheme_gc_ptr_ok
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
#   define scheme_get_sized_byte_string_output dll_scheme_get_sized_string_output
#  وإلا
#   define scheme_get_sized_byte_string_output \
    dll_scheme_get_sized_byte_string_output
#   define scheme_get_param dll_scheme_get_param
#  endif
#  define scheme_intern_symbol dll_scheme_intern_symbol
#  define scheme_lookup_global dll_scheme_lookup_global
#  define scheme_make_closed_prim_w_arity dll_scheme_make_closed_prim_w_arity
#  define scheme_make_integer_value dll_scheme_make_integer_value
#  define scheme_make_pair dll_scheme_make_pair
#  define scheme_make_prim_w_arity dll_scheme_make_prim_w_arity
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
#   define scheme_make_byte_string dll_scheme_make_string
#   define scheme_make_byte_string_output_port dll_scheme_make_string_output_port
#  وإلا
#   define scheme_make_byte_string dll_scheme_make_byte_string
#   define scheme_make_byte_string_output_port \
    dll_scheme_make_byte_string_output_port
#  endif
#  define scheme_make_struct_instance dll_scheme_make_struct_instance
#  define scheme_make_struct_names dll_scheme_make_struct_names
#  define scheme_make_struct_type dll_scheme_make_struct_type
#  define scheme_make_struct_values dll_scheme_make_struct_values
#  define scheme_make_type dll_scheme_make_type
#  define scheme_make_vector dll_scheme_make_vector
#  define scheme_malloc_fail_ok dll_scheme_malloc_fail_ok
#  define scheme_open_input_file dll_scheme_open_input_file
#  define scheme_primitive_module dll_scheme_primitive_module
#  define scheme_proper_list_length dll_scheme_proper_list_length
#  define scheme_raise dll_scheme_raise
#  define scheme_read dll_scheme_read
#  define scheme_register_static dll_scheme_register_static
#  define scheme_set_stack_base dll_scheme_set_stack_base
#  define scheme_signal_error dll_scheme_signal_error
#  define scheme_wrong_type dll_scheme_wrong_type
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
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
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 400
#   ifdef TRAMPOLINED_MZVIM_STARTUP
#    define scheme_main_setup dll_scheme_main_setup
#    𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
#     define scheme_register_tls_space dll_scheme_register_tls_space
#    endif
#   endif
#   define scheme_init_collection_paths dll_scheme_init_collection_paths
#   define scheme_malloc_immobile_box dll_scheme_malloc_immobile_box
#   define scheme_free_immobile_box dll_scheme_free_immobile_box
#  endif
#  𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 600
#   define scheme_embedded_load dll_scheme_embedded_load
#   define scheme_register_embedded_load dll_scheme_register_embedded_load
#   define scheme_set_config_path dll_scheme_set_config_path
#  endif

#  ifndef PROTO
#   𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 500
#    𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
// define as function for macro in schthread.h
Thread_Local_Variables *
scheme_external_get_thread_local_variables(فراغ)
{
    أرجع dll_scheme_external_get_thread_local_variables();
}
#    endif
#   endif
#  endif

# endif

تعريفنوع بنية
{
    حرف    *name;
    فراغ    **ptr;
} Thunk_Info;

ساكن Thunk_Info mzgc_imports[] = {
    {"GC_malloc", (فراغ **)&dll_GC_malloc},
    {"GC_malloc_atomic", (فراغ **)&dll_GC_malloc_atomic},
    {عدم, عدم}};

ساكن Thunk_Info mzsch_imports[] = {
    {"scheme_eof", (فراغ **)&dll_scheme_eof},
    {"scheme_false", (فراغ **)&dll_scheme_false},
    {"scheme_void", (فراغ **)&dll_scheme_void},
    {"scheme_null", (فراغ **)&dll_scheme_null},
    {"scheme_true", (فراغ **)&dll_scheme_true},
# 𐡄𐡍 !defined(USE_THREAD_LOCAL) && !defined(LINK_EXTENSIONS_BY_TABLE)
    {"scheme_current_thread", (فراغ **)&dll_scheme_current_thread_ptr},
# endif
    {"scheme_console_printf", (فراغ **)&dll_scheme_console_printf_ptr},
    {"scheme_console_output", (فراغ **)&dll_scheme_console_output_ptr},
    {"scheme_notify_multithread",
	(فراغ **)&dll_scheme_notify_multithread_ptr},
    {"scheme_add_global", (فراغ **)&dll_scheme_add_global},
    {"scheme_add_global_symbol", (فراغ **)&dll_scheme_add_global_symbol},
    {"scheme_apply", (فراغ **)&dll_scheme_apply},
    {"scheme_basic_env", (فراغ **)&dll_scheme_basic_env},
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_byte_string_to_char_string", (فراغ **)&dll_scheme_byte_string_to_char_string},
    {"scheme_make_path", (فراغ **)&dll_scheme_make_path},
# endif
    {"scheme_builtin_value", (فراغ **)&dll_scheme_builtin_value},
    {"scheme_check_threads", (فراغ **)&dll_scheme_check_threads},
    {"scheme_close_input_port", (فراغ **)&dll_scheme_close_input_port},
    {"scheme_count_lines", (فراغ **)&dll_scheme_count_lines},
    {"scheme_current_continuation_marks",
	(فراغ **)&dll_scheme_current_continuation_marks},
    {"scheme_display", (فراغ **)&dll_scheme_display},
    {"scheme_display_to_string", (فراغ **)&dll_scheme_display_to_string},
    {"scheme_do_eval", (فراغ **)&dll_scheme_do_eval},
    {"scheme_dont_gc_ptr", (فراغ **)&dll_scheme_dont_gc_ptr},
    {"scheme_eq", (فراغ **)&dll_scheme_eq},
    {"scheme_eval", (فراغ **)&dll_scheme_eval},
    {"scheme_eval_string", (فراغ **)&dll_scheme_eval_string},
    {"scheme_eval_string_all", (فراغ **)&dll_scheme_eval_string_all},
    {"scheme_finish_primitive_module",
	(فراغ **)&dll_scheme_finish_primitive_module},
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
    {"scheme_format", (فراغ **)&dll_scheme_format},
# وإلا
    {"scheme_format_utf8", (فراغ **)&dll_scheme_format_utf8},
    {"scheme_get_param", (فراغ **)&dll_scheme_get_param},
# endif
    {"scheme_gc_ptr_ok", (فراغ **)&dll_scheme_gc_ptr_ok},
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
    {"scheme_get_sized_string_output",
	(فراغ **)&dll_scheme_get_sized_string_output},
# وإلا
    {"scheme_get_sized_byte_string_output",
	(فراغ **)&dll_scheme_get_sized_byte_string_output},
# endif
    {"scheme_intern_symbol", (فراغ **)&dll_scheme_intern_symbol},
    {"scheme_lookup_global", (فراغ **)&dll_scheme_lookup_global},
    {"scheme_make_closed_prim_w_arity",
	(فراغ **)&dll_scheme_make_closed_prim_w_arity},
    {"scheme_make_integer_value", (فراغ **)&dll_scheme_make_integer_value},
    {"scheme_make_pair", (فراغ **)&dll_scheme_make_pair},
    {"scheme_make_prim_w_arity", (فراغ **)&dll_scheme_make_prim_w_arity},
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
    {"scheme_make_string", (فراغ **)&dll_scheme_make_string},
    {"scheme_make_string_output_port",
	(فراغ **)&dll_scheme_make_string_output_port},
# وإلا
    {"scheme_make_byte_string", (فراغ **)&dll_scheme_make_byte_string},
    {"scheme_make_byte_string_output_port",
	(فراغ **)&dll_scheme_make_byte_string_output_port},
# endif
    {"scheme_make_struct_instance",
	(فراغ **)&dll_scheme_make_struct_instance},
    {"scheme_make_struct_names", (فراغ **)&dll_scheme_make_struct_names},
    {"scheme_make_struct_type", (فراغ **)&dll_scheme_make_struct_type},
    {"scheme_make_struct_values", (فراغ **)&dll_scheme_make_struct_values},
    {"scheme_make_type", (فراغ **)&dll_scheme_make_type},
    {"scheme_make_vector", (فراغ **)&dll_scheme_make_vector},
    {"scheme_malloc_fail_ok", (فراغ **)&dll_scheme_malloc_fail_ok},
    {"scheme_open_input_file", (فراغ **)&dll_scheme_open_input_file},
    {"scheme_primitive_module", (فراغ **)&dll_scheme_primitive_module},
    {"scheme_proper_list_length", (فراغ **)&dll_scheme_proper_list_length},
    {"scheme_raise", (فراغ **)&dll_scheme_raise},
    {"scheme_read", (فراغ **)&dll_scheme_read},
    {"scheme_register_static", (فراغ **)&dll_scheme_register_static},
    {"scheme_set_stack_base", (فراغ **)&dll_scheme_set_stack_base},
    {"scheme_signal_error", (فراغ **)&dll_scheme_signal_error},
    {"scheme_wrong_type", (فراغ **)&dll_scheme_wrong_type},
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
    {"scheme_set_param", (فراغ **)&dll_scheme_set_param},
    {"scheme_current_config", (فراغ **)&dll_scheme_current_config},
    {"scheme_char_string_to_byte_string",
	(فراغ **)&dll_scheme_char_string_to_byte_string},
    {"scheme_char_string_to_path", (فراغ **)&dll_scheme_char_string_to_path},
    {"scheme_set_collects_path", (فراغ **)&dll_scheme_set_collects_path},
# endif
    {"scheme_make_hash_table", (فراغ **)&dll_scheme_make_hash_table},
    {"scheme_hash_set", (فراغ **)&dll_scheme_hash_set},
    {"scheme_hash_get", (فراغ **)&dll_scheme_hash_get},
    {"scheme_make_double", (فراغ **)&dll_scheme_make_double},
    {"scheme_make_sized_byte_string", (فراغ **)&dll_scheme_make_sized_byte_string},
    {"scheme_namespace_require", (فراغ **)&dll_scheme_namespace_require},
    {"scheme_dynamic_wind", (فراغ **)&dll_scheme_dynamic_wind},
# ifdef MZ_PRECISE_GC
    {"GC_malloc_one_tagged", (فراغ **)&dll_GC_malloc_one_tagged},
    {"GC_register_traversers", (فراغ **)&dll_GC_register_traversers},
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 400
#  ifdef TRAMPOLINED_MZVIM_STARTUP
    {"scheme_main_setup", (فراغ **)&dll_scheme_main_setup},
#   𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || MZSCHEME_VERSION_MAJOR >= 603
    {"scheme_register_tls_space", (فراغ **)&dll_scheme_register_tls_space},
#   endif
#  endif
    {"scheme_init_collection_paths", (فراغ **)&dll_scheme_init_collection_paths},
    {"scheme_malloc_immobile_box", (فراغ **)&dll_scheme_malloc_immobile_box},
    {"scheme_free_immobile_box", (فراغ **)&dll_scheme_free_immobile_box},
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 500
#  𐡄𐡍 defined(IMPLEMENT_THREAD_LOCAL_VIA_WIN_TLS) || defined(IMPLEMENT_THREAD_LOCAL_EXTERNALLY_VIA_PROC)
    {"scheme_external_get_thread_local_variables", (فراغ **)&dll_scheme_external_get_thread_local_variables},
#  endif
# endif
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 600
    {"scheme_embedded_load", (فراغ **)&dll_scheme_embedded_load},
    {"scheme_register_embedded_load", (فراغ **)&dll_scheme_register_embedded_load},
    {"scheme_set_config_path", (فراغ **)&dll_scheme_set_config_path},
# endif
    {عدم, عدم}};

ساكن HINSTANCE hMzGC = 0;
ساكن HINSTANCE hMzSch = 0;

ساكن فراغ dynamic_mzscheme_end(فراغ);
ساكن صحيح mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll,
	صحيح verbose);

    ساكن صحيح
mzscheme_runtime_link_init(حرف *sch_dll, حرف *gc_dll, صحيح verbose)
{
    Thunk_Info *thunk = عدم;

    𐡄𐡍 (hMzGC && hMzSch)
	أرجع OK;
    hMzSch = vimLoadLib(sch_dll);
    hMzGC = vimLoadLib(gc_dll);

    𐡄𐡍 (!hMzGC)
    {
	𐡄𐡍 (verbose)
	    semsg(_(e_could_not_load_library_str_str), gc_dll, GetWin32Error());
	أرجع FAIL;
    }

    𐡄𐡍 (!hMzSch)
    {
	𐡄𐡍 (verbose)
	    semsg(_(e_could_not_load_library_str_str), sch_dll, GetWin32Error());
	أرجع FAIL;
    }

    لكل (thunk = mzsch_imports; thunk->name; thunk++)
    {
	𐡄𐡍 ((*thunk->ptr =
		    (فراغ *)GetProcAddress(hMzSch, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    𐡄𐡍 (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    أرجع FAIL;
	}
    }
    لكل (thunk = mzgc_imports; thunk->name; thunk++)
    {
	𐡄𐡍 ((*thunk->ptr =
		    (فراغ *)GetProcAddress(hMzGC, thunk->name)) == عدم)
	{
	    FreeLibrary(hMzSch);
	    hMzSch = 0;
	    FreeLibrary(hMzGC);
	    hMzGC = 0;
	    𐡄𐡍 (verbose)
		semsg(_(e_could_not_load_library_function_str), thunk->name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

    صحيح
mzscheme_enabled(صحيح verbose)
{
    أرجع mzscheme_runtime_link_init(
	    (حرف *)p_mzschemedll, (حرف *)p_mzschemegcdll, verbose) == OK;
}

    ساكن فراغ
dynamic_mzscheme_end(فراغ)
{
    𐡄𐡍 (hMzSch)
    {
	FreeLibrary(hMzSch);
	hMzSch = 0;
    }
    𐡄𐡍 (hMzGC)
    {
	FreeLibrary(hMzGC);
	hMzGC = 0;
    }
}
#endif // DYNAMIC_MZSCHEME

#𐡄𐡍 MZSCHEME_VERSION_MAJOR < 299
# define GUARANTEED_STRING_ARG(proc, num) GUARANTEE_STRING(proc, num)
#وإلا
    ساكن Scheme_Object *
guaranteed_byte_string_arg(حرف *proc, صحيح num, صحيح argc, Scheme_Object **argv)
{
    𐡄𐡍 (SCHEME_BYTE_STRINGP(argv[num]))
    {
	أرجع argv[num];
    }
    وإلا 𐡄𐡍 (SCHEME_CHAR_STRINGP(argv[num]))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, argv[num]);
	MZ_GC_VAR_IN_REG(1, tmp);
	MZ_GC_REG();
	tmp = scheme_char_string_to_byte_string(argv[num]);
	MZ_GC_UNREG();
	أرجع tmp;
    }
    وإلا
	scheme_wrong_type(proc, "string", num, argc, argv);
    // unreachable
    أرجع scheme_void;
}
# define GUARANTEED_STRING_ARG(proc, num) guaranteed_byte_string_arg(proc, num, argc, argv)
#endif

// need to put it here for dynamic stuff to work
#𐡄𐡍 defined(INCLUDE_MZSCHEME_BASE)
# include "mzscheme_base.c"
#endif

/*
 *========================================================================
 *  1. MzScheme interpreter startup
 *========================================================================
 */

ساكن Scheme_Type mz_buffer_type;
ساكن Scheme_Type mz_window_type;

ساكن صحيح initialized = FALSE;
#ifdef DYNAMIC_MZSCHEME
ساكن صحيح disabled = FALSE;
#endif
ساكن صحيح load_base_module_failed = FALSE;

// global environment
ساكن Scheme_Env    *environment = عدم;
// output/error handlers
ساكن Scheme_Object *curout = عدم;
ساكن Scheme_Object *curerr = عدم;
// exn:vim exception
ساكن Scheme_Object *exn_catching_apply = عدم;
ساكن Scheme_Object *exn_p = عدم;
ساكن Scheme_Object *exn_message = عدم;
ساكن Scheme_Object *vim_exn = عدم; // Vim Error exception

#𐡄𐡍 !defined(MZ_PRECISE_GC) || MZSCHEME_VERSION_MAJOR < 400
ساكن فراغ *stack_base = عدم;
#endif

ساكن long range_start;
ساكن long range_end;

// MzScheme threads scheduling stuff
ساكن صحيح mz_threads_allow = 0;

#𐡄𐡍 defined(FEAT_GUI_MSWIN)
ساكن فراغ CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);
ساكن UINT timer_id = 0;
#وإلاإذا defined(FEAT_GUI_GTK)
ساكن gboolean timer_proc(gpointer);
ساكن guint timer_id = 0;
#وإلاإذا defined(FEAT_GUI_MOTIF)
ساكن فراغ timer_proc(XtPointer, XtIntervalId *);
ساكن XtIntervalId timer_id = (XtIntervalId)0;
#endif

#𐡄𐡍 !defined(FEAT_GUI_MSWIN) || defined(VIMDLL) // Win32 console and Unix
    فراغ
mzvim_check_threads(فراغ)
{
    // Last time MzScheme threads were scheduled
    ساكن time_t mz_last_time = 0;

    𐡄𐡍 (mz_threads_allow && p_mzq > 0)
    {
	time_t now = time(عدم);

	𐡄𐡍 ((now - mz_last_time) * 1000 > p_mzq)
	{
	    mz_last_time = now;
	    scheme_check_threads();
	}
    }
}
#endif

#𐡄𐡍 defined(MZSCHEME_GUI_THREADS)
ساكن فراغ setup_timer(فراغ);
ساكن فراغ remove_timer(فراغ);

// timers are presented in GUI only
# 𐡄𐡍 defined(FEAT_GUI_MSWIN)
    ساكن فراغ CALLBACK
timer_proc(
    HWND	hwnd UNUSED,
    UINT	uMsg UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime UNUSED)
# elif defined(FEAT_GUI_GTK)
    ساكن gboolean
timer_proc(gpointer data UNUSED)
# elif defined(FEAT_GUI_MOTIF)
    ساكن فراغ
timer_proc(XtPointer timed_out UNUSED, XtIntervalId *interval_id UNUSED)
# endif
{
    scheme_check_threads();
# 𐡄𐡍 defined(FEAT_GUI_GTK)
    أرجع TRUE; // continue receiving notifications
# elif defined(FEAT_GUI_MOTIF)
    // renew timeout
    𐡄𐡍 (mz_threads_allow && p_mzq > 0)
	timer_id = XtAppAddTimeOut(app_context, p_mzq,
		timer_proc, عدم);
# endif
}

    ساكن فراغ
setup_timer(فراغ)
{
# 𐡄𐡍 defined(FEAT_GUI_MSWIN)
    timer_id = SetTimer(عدم, 0, p_mzq, timer_proc);
# elif defined(FEAT_GUI_GTK)
    timer_id = g_timeout_add((guint)p_mzq, (GSourceFunc)timer_proc, عدم);
# elif defined(FEAT_GUI_MOTIF)
    timer_id = XtAppAddTimeOut(app_context, p_mzq, timer_proc, عدم);
# endif
}

    ساكن فراغ
remove_timer(فراغ)
{
# 𐡄𐡍 defined(FEAT_GUI_MSWIN)
    KillTimer(عدم, timer_id);
# elif defined(FEAT_GUI_GTK)
    g_source_remove(timer_id);
# elif defined(FEAT_GUI_MOTIF)
    XtRemoveTimeOut(timer_id);
# endif
    timer_id = 0;
}

#endif // MZSCHEME_GUI_THREADS

    حرف *
did_set_mzquantum(optset_T *args UNUSED)
{
#𐡄𐡍 defined(MZSCHEME_GUI_THREADS)
    // reset timer
    𐡄𐡍 (timer_id != 0)
	remove_timer();
    𐡄𐡍 (mz_threads_allow && p_mzq > 0 && gui.in_use)
	setup_timer();
#endif
    أرجع عدم;
}

    ساكن فراغ
notify_multithread(صحيح on)
{
    mz_threads_allow = on;
#ifdef MZSCHEME_GUI_THREADS
    𐡄𐡍 (on && timer_id == 0 && p_mzq > 0 && gui.in_use)
	setup_timer();
    𐡄𐡍 (!on && timer_id != 0)
	remove_timer();
#endif
}

    فراغ
mzscheme_end(فراغ)
{
    // We can not unload the DLL.  Racket's thread might be still alive.
#𐡄𐡍 0
# ifdef DYNAMIC_MZSCHEME
    dynamic_mzscheme_end();
# endif
#endif
}

#𐡄𐡍 HAVE_TLS_SPACE && !defined(VIMDLL)
# 𐡄𐡍 defined(_MSC_VER)
ساكن __declspec(thread) فراغ *tls_space;
خارجي intptr_t _tls_index;
# elif defined(__MINGW32__)
ساكن __thread فراغ *tls_space;
خارجي intptr_t _tls_index;
# وإلا
ساكن THREAD_LOCAL فراغ *tls_space;
ساكن intptr_t _tls_index = 0;
# endif
#endif

/*
 * mzscheme_main() is called early in main().
 * We may call scheme_main_setup() which calls mzscheme_env_main() which then
 * trampolines into vim_main2(), which never returns.
 */
    صحيح
mzscheme_main(فراغ)
{
    صحيح	    argc = 0;
    حرف    *argv = عدم;

#ifdef DYNAMIC_MZSCHEME
    /*
     * Racket requires trampolined startup.  We can not load it later.
     * If dynamic dll loading is failed, disable it.
     */
    𐡄𐡍 (!mzscheme_enabled(FALSE))
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
#endif
#ifdef HAVE_TLS_SPACE
# ifdef VIMDLL
    فراغ **ptls_space;
    intptr_t tls_index;
    فراغ (*pget_tls_info)(فراغ ***ptls_space, intptr_t *ptls_index);

    // Get the address of get_tls_info() from (g)vim.exe.
    pget_tls_info = (فراغ *)GetProcAddress(
				GetModuleHandle(عدم), "get_tls_info");
    𐡄𐡍 (pget_tls_info == عدم)
    {
	disabled = TRUE;
	أرجع vim_main2();
    }
    pget_tls_info(&ptls_space, &tls_index);
    scheme_register_tls_space(ptls_space, tls_index);
# وإلا
    scheme_register_tls_space(&tls_space, _tls_index);
# endif
#endif
#ifdef TRAMPOLINED_MZVIM_STARTUP
    أرجع scheme_main_setup(TRUE, mzscheme_env_main, argc, &argv);
#وإلا
    أرجع mzscheme_env_main(عدم, argc, &argv);
#endif
}

    ساكن صحيح
mzscheme_env_main(Scheme_Env *env, صحيح argc UNUSED, حرف **argv UNUSED)
{
#ifdef TRAMPOLINED_MZVIM_STARTUP
    // Scheme has created the environment for us
    environment = env;
#وإلا
# ifdef MZ_PRECISE_GC
    Scheme_Object   *dummy = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, dummy);

    stack_base = &__gc_var_stack__;
# وإلا
    صحيح dummy = 0;
    stack_base = (فراغ *)&dummy;
# endif
#endif

    vim_main2();
    // not reached, vim_main2() will loop until exit()

    أرجع 0;
}

    ساكن Scheme_Object*
load_base_module(فراغ *data)
{
    scheme_namespace_require(scheme_intern_symbol((حرف *)data));
    أرجع scheme_null;
}

    ساكن Scheme_Object *
load_base_module_on_error(فراغ *data UNUSED)
{
    load_base_module_failed = TRUE;
    أرجع scheme_null;
}

    ساكن صحيح
startup_mzscheme(فراغ)
{
#ifndef TRAMPOLINED_MZVIM_STARTUP
    scheme_set_stack_base(stack_base, 1);
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
#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
    {
	Scheme_Object *coll_path = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, coll_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCOLLECTS", &mustfree);
	𐡄𐡍 (s != عدم)
	{
	    coll_path = scheme_make_path((حرف *)s);
	    MZ_GC_CHECK();
	    𐡄𐡍 (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_COLLECTS
	𐡄𐡍 (coll_path == عدم)
	{
	    coll_path = scheme_make_path(MZSCHEME_COLLECTS);
	    MZ_GC_CHECK();
	}
# endif
	𐡄𐡍 (coll_path != عدم)
	{
	    scheme_set_collects_path(coll_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#وإلا
# ifdef MZSCHEME_COLLECTS
    {
	Scheme_Object	*coll_string = عدم;
	Scheme_Object	*coll_pair = عدم;
	Scheme_Config	*config = عدم;

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

#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 600
    {
	Scheme_Object *config_path = عدم;
	صحيح mustfree = FALSE;
	char_u *s;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, config_path);
	MZ_GC_REG();
	// workaround for dynamic loading on windows
	s = vim_getenv((char_u *)"PLTCONFIGDIR", &mustfree);
	𐡄𐡍 (s != عدم)
	{
	    config_path = scheme_make_path((حرف *)s);
	    MZ_GC_CHECK();
	    𐡄𐡍 (mustfree)
		vim_free(s);
	}
# ifdef MZSCHEME_CONFIGDIR
	𐡄𐡍 (config_path == عدم)
	{
	    config_path = scheme_make_path(MZSCHEME_CONFIGDIR);
	    MZ_GC_CHECK();
	}
# endif
	𐡄𐡍 (config_path != عدم)
	{
	    scheme_set_config_path(config_path);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
#endif

#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 400
    scheme_init_collection_paths(environment, scheme_null);
#endif

    /*
     * versions 4.x do not provide Scheme bindings by default
     * we need to add them explicitly
     */
    {
	// use error handler to avoid abort
	scheme_dynamic_wind(عدم, load_base_module, عدم,
				    load_base_module_on_error, "racket/base");
	𐡄𐡍 (load_base_module_failed)
	{
	    load_base_module_failed = FALSE;
	    scheme_dynamic_wind(عدم, load_base_module, عدم,
				    load_base_module_on_error, "scheme/base");
	    𐡄𐡍 (load_base_module_failed)
		أرجع -1;
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
	Scheme_Object	*make_security_guard = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, make_security_guard);
	MZ_GC_REG();

# 𐡄𐡍 MZSCHEME_VERSION_MAJOR < 400
	{
	    Scheme_Object	*make_security_guard_symbol = عدم;
	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, make_security_guard_symbol);
	    MZ_GC_REG();
	    make_security_guard_symbol = scheme_intern_symbol("make-security-guard");
	    MZ_GC_CHECK();
	    make_security_guard = scheme_lookup_global(
		    make_security_guard_symbol, environment);
	    MZ_GC_UNREG();
	}
# وإلا
	make_security_guard = scheme_builtin_value("make-security-guard");
	MZ_GC_CHECK();
# endif

	// setup sandbox guards
	𐡄𐡍 (make_security_guard != عدم)
	{
	    Scheme_Object   *args[3] = {عدم, عدم, عدم};
	    Scheme_Object   *guard = عدم;
	    Scheme_Config   *config = عدم;
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

    أرجع 0;
}

/*
 * This routine is called for each new invocation of MzScheme
 * to make sure things are properly initialized.
 */
    ساكن صحيح
mzscheme_init(فراغ)
{
    𐡄𐡍 (!initialized)
    {
#ifdef DYNAMIC_MZSCHEME
	𐡄𐡍 (disabled || !mzscheme_enabled(TRUE))
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded));
	    أرجع -1;
	}
#endif
	𐡄𐡍 (load_base_module_failed || startup_mzscheme())
	{
	    emsg(_(e_sorry_this_command_is_disabled_the_mzscheme_racket_base_module_could_not_be_loaded));
	    أرجع -1;
	}
	initialized = TRUE;
    }
    {
	Scheme_Config	*config = عدم;
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

    أرجع 0;
}

/*
 *========================================================================
 *  2.  External Interface
 *========================================================================
 */

/*
 * Evaluate command with exception handling
 */
    ساكن صحيح
eval_with_exn_handling(
    فراغ		*data,
    Scheme_Closed_Prim	*what,
    Scheme_Object	**ret)
{
    Scheme_Object   *value = عدم;
    Scheme_Object   *exn = عدم;
    Scheme_Object   *prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, value);
    MZ_GC_VAR_IN_REG(1, exn);
    MZ_GC_VAR_IN_REG(2, prim);
    MZ_GC_REG();

    prim = scheme_make_closed_prim_w_arity(what, data, "mzvim", 0, 0);
    MZ_GC_CHECK();
    value = _apply_thunk_catch_exceptions(prim, &exn);
    MZ_GC_CHECK();

    𐡄𐡍 (!value)
    {
	value = extract_exn_message(exn);
	// Got an exn?
	𐡄𐡍 (value)
	{
	    scheme_display(value, curerr);   //  Send to stderr-vim
	    MZ_GC_CHECK();
	    do_flush();
	}
	MZ_GC_UNREG();
	// `raise' was called on some arbitrary value
	أرجع FAIL;
    }

    𐡄𐡍 (ret != عدم)	// if pointer to retval supported give it up
	*ret = value;
    // Print any result, as long as it's not a void
    وإلا 𐡄𐡍 (!SCHEME_VOIDP(value))
    {
	scheme_display(value, curout);  // Send to stdout-vim
	MZ_GC_CHECK();
    }

    do_flush();
    MZ_GC_UNREG();
    أرجع OK;
}

/*
 * :mzscheme
 */
    ساكن صحيح
do_mzscheme_command(exarg_T *eap, فراغ *data, Scheme_Closed_Prim *what)
{
    𐡄𐡍 (mzscheme_init())
	أرجع FAIL;

    range_start = eap->line1;
    range_end = eap->line2;

    أرجع eval_with_exn_handling(data, what, عدم);
}

/*
 * Routine called by VIM when deleting a buffer
 */
    فراغ
mzscheme_buffer_free(buf_T *buf)
{
    𐡄𐡍 (buf->b_mzscheme_ref == عدم)
	أرجع;

    vim_mz_buffer *bp = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, bp);
    MZ_GC_REG();

    bp = BUFFER_REF(buf);
    bp->buf = INVALID_BUFFER_VALUE;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(buf->b_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(bp);
#endif
    buf->b_mzscheme_ref = عدم;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * Routine called by VIM when deleting a Window
 */
    فراغ
mzscheme_window_free(win_T *win)
{
    𐡄𐡍 (win->w_mzscheme_ref == عدم)
	أرجع;

    vim_mz_window *wp = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, wp);
    MZ_GC_REG();
    wp = WINDOW_REF(win);
    wp->win = INVALID_WINDOW_VALUE;
#ifdef MZ_PRECISE_GC
    scheme_free_immobile_box(win->w_mzscheme_ref);
#وإلا
    scheme_gc_ptr_ok(wp);
#endif
    win->w_mzscheme_ref = عدم;
    MZ_GC_CHECK();
    MZ_GC_UNREG();
}

/*
 * ":mzscheme" (or ":mz")
 */
    فراغ
ex_mzscheme(exarg_T *eap)
{
    char_u	*script;

    script = script_get(eap, eap->arg);
    𐡄𐡍 (eap->skip)
	أرجع;

    𐡄𐡍 (script == عدم)
	do_mzscheme_command(eap, eap->arg, do_eval);
    وإلا
    {
	do_mzscheme_command(eap, script, do_eval);
	vim_free(script);
    }
}

    ساكن Scheme_Object *
do_load(فراغ *data, صحيح noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    Scheme_Object   *expr = عدم;
    Scheme_Object   *result = عدم;
    حرف	    *file = عدم;
    Port_Info	    *pinfo = (Port_Info *)data;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, expr);
    MZ_GC_VAR_IN_REG(1, result);
    MZ_GC_VAR_IN_REG(2, file);
    MZ_GC_REG();

    file = (حرف *)scheme_malloc_fail_ok(scheme_malloc_atomic, MAXPATHL + 1);
    MZ_GC_CHECK();

    // make Vim expansion
    expand_env((char_u *)pinfo->name, (char_u *)file, MAXPATHL);
    pinfo->port = scheme_open_input_file(file, "mzfile");
    MZ_GC_CHECK();
    scheme_count_lines(pinfo->port);  // to get accurate read error location
    MZ_GC_CHECK();

    // Like REPL but print only last result
    طالما (!SCHEME_EOFP(expr = scheme_read(pinfo->port)))
    {
	result = scheme_eval(expr, environment);
	MZ_GC_CHECK();
    }

    // errors will be caught in do_mzscheme_command and ex_mzfile
    scheme_close_input_port(pinfo->port);
    MZ_GC_CHECK();
    pinfo->port = عدم;
    MZ_GC_UNREG();
    أرجع result;
}

/*
 * :mzfile
 */
    فراغ
ex_mzfile(exarg_T *eap)
{
    Port_Info	pinfo = {عدم, عدم};

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, pinfo.port);
    MZ_GC_REG();

    pinfo.name = (حرف *)eap->arg;
    𐡄𐡍 (do_mzscheme_command(eap, &pinfo, do_load) != OK
	    && pinfo.port != عدم)	// looks like port was not closed
    {
	scheme_close_input_port(pinfo.port);
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
    ساكن فراغ
init_exn_catching_apply(فراغ)
{
    𐡄𐡍 (exn_catching_apply)
	أرجع;

    حرف *e =
	"(lambda (thunk) "
	"(with-handlers ([void (lambda (exn) (cons #f exn))]) "
	"(cons #t (thunk))))";

    exn_catching_apply = scheme_eval_string(e, environment);
    MZ_GC_CHECK();
    exn_p = scheme_builtin_value("exn?");
    MZ_GC_CHECK();
    exn_message = scheme_builtin_value("exn-message");
    MZ_GC_CHECK();
}

/*
 * This function applies a thunk, returning the Scheme value if there's
 * no exception, otherwise returning NULL and setting *exn to the raised
 * value (usually an exn structure).
 */
    ساكن Scheme_Object *
_apply_thunk_catch_exceptions(Scheme_Object *f, Scheme_Object **exn)
{
    Scheme_Object *v;

    v = _scheme_apply(exn_catching_apply, 1, &f);
    // v is a pair: (cons #t value) or (cons #f exn)

    𐡄𐡍 (SCHEME_TRUEP(SCHEME_CAR(v)))
	أرجع SCHEME_CDR(v);
    وإلا
    {
	*exn = SCHEME_CDR(v);
	أرجع عدم;
    }
}

    ساكن Scheme_Object *
extract_exn_message(Scheme_Object *v)
{
    𐡄𐡍 (SCHEME_TRUEP(_scheme_apply(exn_p, 1, &v)))
	أرجع _scheme_apply(exn_message, 1, &v);
    وإلا
	أرجع عدم; // Not an exn structure
}

    ساكن Scheme_Object *
do_eval(فراغ *s, صحيح noargc UNUSED, Scheme_Object **noargv UNUSED)
{
    أرجع scheme_eval_string_all((حرف *)s, environment, TRUE);
}

/*
 *========================================================================
 *  3.  MzScheme I/O Handlers
 *========================================================================
 */
    ساكن فراغ
do_intrnl_output(حرف *mesg, صحيح error)
{
    حرف *p, *prev;

    prev = mesg;
    p = strchr(prev, '\n');
    طالما (p)
    {
	*p = '\0';
	𐡄𐡍 (error)
	    emsg(prev);
	وإلا
	    msg(prev);
	prev = p + 1;
	p = strchr(prev, '\n');
    }

    𐡄𐡍 (error)
	emsg(prev);
    وإلا
	msg(prev);
}

    ساكن فراغ
do_output(حرف *mesg, OUTPUT_LEN_TYPE len UNUSED)
{
    // TODO: use len, the string may not be NUL terminated
    do_intrnl_output(mesg, 0);
}

    ساكن فراغ
do_err_output(حرف *mesg)
{
    do_intrnl_output(mesg, 1);
}

    ساكن فراغ
do_printf(حرف *format, ...)
{
    do_intrnl_output(format, 1);
}

    ساكن فراغ
do_flush(فراغ)
{
    حرف *buff;
    OUTPUT_LEN_TYPE length;

    buff = scheme_get_sized_byte_string_output(curerr, &length);
    MZ_GC_CHECK();
    𐡄𐡍 (length)
    {
	do_err_output(buff);
	أرجع;
    }

    buff = scheme_get_sized_byte_string_output(curout, &length);
    MZ_GC_CHECK();
    𐡄𐡍 (length)
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
    ساكن Scheme_Object *
vim_command(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = عدم;
    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    // may be use do_cmdline_cmd?
    do_cmdline(BYTE_STRING_VALUE(cmd), عدم, عدم, DOCMD_NOWAIT|DOCMD_VERBOSE);
    update_screen(UPD_VALID);

    MZ_GC_UNREG();
    raise_if_error();
    أرجع scheme_void;
}

/*
 * (eval {expr-string})
 */
    ساكن Scheme_Object *
vim_eval(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
#ifdef FEAT_EVAL
    Vim_Prim		*prim = (Vim_Prim *)data;
    Scheme_Object	*result = عدم;
    typval_T		*vim_result;
    Scheme_Object *expr = عدم;
    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, expr);
    MZ_GC_REG();
    expr = GUARANTEED_STRING_ARG(prim->name, 0);

    vim_result = eval_expr(BYTE_STRING_VALUE(expr), عدم);

    𐡄𐡍 (vim_result == عدم)
	raise_vim_exn(_("invalid expression"));

    result = vim_to_mzscheme(vim_result);
    MZ_GC_CHECK();
    free_tv(vim_result);

    MZ_GC_UNREG();
    أرجع result;
#وإلا
    raise_vim_exn(_("expressions disabled at compile time"));
    // unreachable
    أرجع scheme_false;
#endif
}

/*
 * (range-start)
 */
    ساكن Scheme_Object *
get_range_start(
    فراغ		*data UNUSED,
    صحيح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    أرجع scheme_make_integer(range_start);
}

/*
 * (range-end)
 */
    ساكن Scheme_Object *
get_range_end(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
    أرجع scheme_make_integer(range_end);
}

/*
 * (beep)
 */
    ساكن Scheme_Object *
mzscheme_beep(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
    vim_beep(BO_LANG);
    أرجع scheme_void;
}

ساكن Scheme_Object *M_global = عدم;

/*
 * (get-option {option-name}) [buffer/window]
 */
    ساكن Scheme_Object *
get_option(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    long	    value;
    حرف	    *strval;
    getoption_T	    rc;
    Scheme_Object   *rval = عدم;
    Scheme_Object   *name = عدم;
    صحيح		    scope = 0;
    buf_T	    *save_curb = curbuf;
    win_T	    *save_curw = curwin;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, rval);
    MZ_GC_VAR_IN_REG(1, name);
    MZ_GC_REG();

    name = GUARANTEED_STRING_ARG(prim->name, 0);

    𐡄𐡍 (argc > 1)
    {
	𐡄𐡍 (M_global == عدم)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_symbol("global");
	    MZ_GC_CHECK();
	}

	𐡄𐡍 (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وإلا 𐡄𐡍 (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	وإلا 𐡄𐡍 (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);

	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	وإلا
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    rc = get_option_value(BYTE_STRING_VALUE(name), &value, (char_u **)&strval,
								  عدم, scope);
    curbuf = save_curb;
    curwin = save_curw;

    بدل (rc)
    {
    حالة gov_bool:
    حالة gov_number:
	MZ_GC_UNREG();
	أرجع scheme_make_integer_value(value);
    حالة gov_string:
	rval = scheme_make_byte_string(strval);
	MZ_GC_CHECK();
	vim_free(strval);
	MZ_GC_UNREG();
	أرجع rval;
    حالة gov_hidden_bool:
    حالة gov_hidden_number:
    حالة gov_hidden_string:
	MZ_GC_UNREG();
	raise_vim_exn(_("hidden option"));
	//NOTREACHED
    حالة gov_unknown:
	MZ_GC_UNREG();
	raise_vim_exn(_("unknown option"));
	//NOTREACHED
    }
    // unreachable
    أرجع scheme_void;
}

/*
 * (set-option {option-changing-string} [buffer/window])
 */
    ساكن Scheme_Object *
set_option(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    char_u	*command = عدم;
    صحيح		scope = 0;
    buf_T	*save_curb = curbuf;
    win_T	*save_curw = curwin;
    Vim_Prim	*prim = (Vim_Prim *)data;
    Scheme_Object *cmd = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, cmd);
    MZ_GC_REG();
    cmd = GUARANTEED_STRING_ARG(prim->name, 0);

    𐡄𐡍 (argc > 1)
    {
	𐡄𐡍 (M_global == عدم)
	{
	    MZ_REGISTER_STATIC(M_global);
	    M_global = scheme_intern_symbol("global");
	    MZ_GC_CHECK();
	}

	𐡄𐡍 (argv[1] == M_global)
	    scope = OPT_GLOBAL;
	وإلا 𐡄𐡍 (SCHEME_VIMBUFFERP(argv[1]))
	{
	    curbuf = get_valid_buffer(argv[1]);
	    scope = OPT_LOCAL;
	}
	وإلا 𐡄𐡍 (SCHEME_VIMWINDOWP(argv[1]))
	{
	    win_T *win = get_valid_window(argv[1]);
	    curwin = win;
	    curbuf = win->w_buffer;
	    scope = OPT_LOCAL;
	}
	وإلا
	    scheme_wrong_type(prim->name, "vim-buffer/window", 1, argc, argv);
    }

    // do_set can modify cmd, make copy
    command = vim_strsave(BYTE_STRING_VALUE(cmd));
    MZ_GC_UNREG();
    do_set(command, scope);
    vim_free(command);
    update_screen(UPD_NOT_VALID);
    curbuf = save_curb;
    curwin = save_curw;
    raise_if_error();
    أرجع scheme_void;
}

/*
 *===========================================================================
 *  5. Vim Window-related Manipulation Functions
 *===========================================================================
 */

/*
 * (curr-win)
 */
    ساكن Scheme_Object *
get_curr_win(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
    أرجع (Scheme_Object *)get_vim_curr_window();
}

/*
 * (win-count)
 */
    ساكن Scheme_Object *
get_window_count(
    فراغ		*data UNUSED,
    صحيح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    صحيح	    n = 0;
    win_T   *w;

    FOR_ALL_WINDOWS(w)
	++n;
    أرجع scheme_make_integer(n);
}

/*
 * (get-win-list [buffer])
 */
    ساكن Scheme_Object *
get_window_list(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *list;
    win_T	    *w = firstwin;

    buf = get_buffer_arg(prim->name, 0, argc, argv);
    list = scheme_null;

    لكل ( ; w != عدم; w = w->w_next)
	𐡄𐡍 (w->w_buffer == buf->buf)
	{
	    list = scheme_make_pair(window_new(w), list);
	    MZ_GC_CHECK();
	}

    أرجع list;
}

    ساكن Scheme_Object *
window_new(win_T *win)
{
    vim_mz_window *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of windows underneath us.
    // If we add a "w_mzscheme_ref" field to the win_T structure,
    // then we can get at it in win_free() in vim.
    //
    // On a win_free() we set the Scheme object's win_T *field
    // to an invalid value. We trap all uses of a window
    // object, and reject them if the win_T *field is invalid.
    𐡄𐡍 (win->w_mzscheme_ref != عدم)
	أرجع (Scheme_Object *)WINDOW_REF(win);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_window));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    win->w_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because win isn't visible to GC
#endif
    MZ_GC_CHECK();
    WINDOW_REF(win) = self;
    MZ_GC_CHECK();
    self->win = win;
    self->so.type = mz_window_type;

    MZ_GC_UNREG();
    أرجع (Scheme_Object *)self;
}

/*
 * (get-win-num [window])
 */
    ساكن Scheme_Object *
get_window_num(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
    صحيح		nr = 1;
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = get_window_arg(prim->name, 0, argc, argv)->win;
    win_T	*wp;

    لكل (wp = firstwin; wp != win; wp = wp->w_next)
	++nr;

    أرجع scheme_make_integer(nr);
}

/*
 * (get-win-by-num {windownum})
 */
    ساكن Scheme_Object *
get_window_by_num(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    win_T	*win = firstwin;
    صحيح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    𐡄𐡍 (fnum < 1)
	scheme_signal_error(_("window index is out of range"));

    لكل ( ; win != عدم; win = win->w_next, --fnum)
	𐡄𐡍 (fnum == 1)	    // to be 1-based
	    أرجع window_new(win);

    أرجع scheme_false;
}

/*
 * (get-win-buffer [window])
 */
    ساكن Scheme_Object *
get_window_buffer(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    أرجع buffer_new(win->win->w_buffer);
}

/*
 * (get-win-height [window])
 */
    ساكن Scheme_Object *
get_window_height(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_height);
}

/*
 * (set-win-height {height} [window])
 */
    ساكن Scheme_Object *
set_window_height(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    صحيح		    height;

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
    أرجع scheme_void;
}

/*
 * (get-win-width [window])
 */
    ساكن Scheme_Object *
get_window_width(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win = get_window_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(win->win->w_width);
}

/*
 * (set-win-width {width} [window])
 */
    ساكن Scheme_Object *
set_window_width(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    win_T	    *savewin;
    صحيح		    width = 0;

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
    أرجع scheme_void;
}

/*
 * (get-cursor [window]) -> (line . col)
 */
    ساكن Scheme_Object *
get_cursor(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_window   *win;
    pos_T	    pos;

    win = get_window_arg(prim->name, 0, argc, argv);
    pos = win->win->w_cursor;
    أرجع scheme_make_pair(scheme_make_integer_value((long)pos.lnum),
		    scheme_make_integer_value((long)pos.col + 1));
}

/*
 * (set-cursor (line . col) [window])
 */
    ساكن Scheme_Object *
set_cursor(فراغ *data, صحيح argc, Scheme_Object **argv)
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

    𐡄𐡍 (!SCHEME_INTP(SCHEME_CAR(argv[0]))
	    || !SCHEME_INTP(SCHEME_CDR(argv[0])))
	scheme_wrong_type(prim->name, "integer pair", 0, argc, argv);

    lnum = SCHEME_INT_VAL(SCHEME_CAR(argv[0]));
    col = SCHEME_INT_VAL(SCHEME_CDR(argv[0])) - 1;

    check_line_range(lnum, win->win->w_buffer);
    // don't know how to catch invalid column value

    win->win->w_cursor.lnum = lnum;
    win->win->w_cursor.col = col;
    win->win->w_set_curswant = true;
    update_screen(UPD_VALID);

    raise_if_error();
    أرجع scheme_void;
}
/*
 *===========================================================================
 *  6. Vim Buffer-related Manipulation Functions
 *===========================================================================
 */

/*
 * (open-buff {filename})
 */
    ساكن Scheme_Object *
mzscheme_open_buffer(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    صحيح		    num = 0;
    Scheme_Object   *onum = عدم;
    Scheme_Object   *buf = عدم;
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
    num = buflist_add(BYTE_STRING_VALUE(fname), BLN_LISTED | BLN_CURBUF);

    𐡄𐡍 (num == 0)
	raise_vim_exn(_("couldn't open buffer"));

    onum = scheme_make_integer(num);
    buf = get_buffer_by_num(data, 1, &onum);
    MZ_GC_UNREG();
    أرجع buf;
}

/*
 * (get-buff-by-num {buffernum})
 */
    ساكن Scheme_Object *
get_buffer_by_num(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    صحيح		fnum;

    fnum = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));

    FOR_ALL_BUFFERS(buf)
	𐡄𐡍 (buf->b_fnum == fnum)
	    أرجع buffer_new(buf);

    أرجع scheme_false;
}

/*
 * (get-buff-by-name {buffername})
 */
    ساكن Scheme_Object *
get_buffer_by_name(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf;
    Scheme_Object *buffer = عدم;
    Scheme_Object *fname = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, buffer);
    MZ_GC_VAR_IN_REG(1, fname);
    MZ_GC_REG();
    fname = GUARANTEED_STRING_ARG(prim->name, 0);
    buffer = scheme_false;

    FOR_ALL_BUFFERS(buf)
    {
	𐡄𐡍 (buf->b_ffname == عدم || buf->b_sfname == عدم)
	    // empty string
	{
	    𐡄𐡍 (BYTE_STRING_VALUE(fname)[0] == NUL)
		buffer = buffer_new(buf);
	}
	وإلا 𐡄𐡍 (!fnamecmp(buf->b_ffname, BYTE_STRING_VALUE(fname))
		|| !fnamecmp(buf->b_sfname, BYTE_STRING_VALUE(fname)))
	{
	    // either short or long filename matches
	    buffer = buffer_new(buf);
	}
    }

    MZ_GC_UNREG();
    أرجع buffer;
}

/*
 * (get-next-buff [buffer])
 */
    ساكن Scheme_Object *
get_next_buffer(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    𐡄𐡍 (buf->b_next == عدم)
	أرجع scheme_false;
    وإلا
	أرجع buffer_new(buf->b_next);
}

/*
 * (get-prev-buff [buffer])
 */
    ساكن Scheme_Object *
get_prev_buffer(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	*prim = (Vim_Prim *)data;
    buf_T	*buf = get_buffer_arg(prim->name, 0, argc, argv)->buf;

    𐡄𐡍 (buf->b_prev == عدم)
	أرجع scheme_false;
    وإلا
	أرجع buffer_new(buf->b_prev);
}

/*
 * (get-buff-num [buffer])
 */
    ساكن Scheme_Object *
get_buffer_num(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_fnum);
}

/*
 * (buff-count)
 */
    ساكن Scheme_Object *
get_buffer_count(
    فراغ		*data UNUSED,
    صحيح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    buf_T   *b;
    صحيح	    n = 0;

    FOR_ALL_BUFFERS(b) ++n;
    أرجع scheme_make_integer(n);
}

/*
 * (get-buff-name [buffer])
 */
    ساكن Scheme_Object *
get_buffer_name(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_byte_string((حرف *)buf->buf->b_ffname);
}

/*
 * (curr-buff)
 */
    ساكن Scheme_Object *
get_curr_buffer(
    فراغ		*data UNUSED,
    صحيح			argc UNUSED,
    Scheme_Object	**argv UNUSED)
{
    أرجع (Scheme_Object *)get_vim_curr_buffer();
}

    ساكن Scheme_Object *
buffer_new(buf_T *buf)
{
    vim_mz_buffer *self = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, self);

    // We need to handle deletion of buffers underneath us.
    // If we add a "b_mzscheme_ref" field to the buf_T structure,
    // then we can get at it in buf_freeall() in vim.
    𐡄𐡍 (buf->b_mzscheme_ref)
	أرجع (Scheme_Object *)BUFFER_REF(buf);

    MZ_GC_REG();
    self = scheme_malloc_fail_ok(scheme_malloc_tagged, حجم(vim_mz_buffer));
    CLEAR_POINTER(self);
#ifdef MZ_PRECISE_GC
    buf->b_mzscheme_ref = scheme_malloc_immobile_box(عدم);
#وإلا
    scheme_dont_gc_ptr(self);	// because buf isn't visible to GC
#endif
    MZ_GC_CHECK();
    BUFFER_REF(buf) = self;
    MZ_GC_CHECK();
    self->buf = buf;
    self->so.type = mz_buffer_type;

    MZ_GC_UNREG();
    أرجع (Scheme_Object *)self;
}

/*
 * (get-buff-size [buffer])
 *
 * Get the size (number of lines) in the current buffer.
 */
    ساكن Scheme_Object *
get_buffer_size(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = get_buffer_arg(prim->name, 0, argc, argv);

    أرجع scheme_make_integer(buf->buf->b_ml.ml_line_count);
}

/*
 * (get-buff-line {linenr} [buffer])
 *
 * Get a line from the specified buffer. The line number is
 * in Vim format (1-based). The line is returned as a MzScheme
 * string object.
 */
    ساكن Scheme_Object *
get_buffer_line(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    صحيح		    linenr;
    char_u	    *line;

    buf = get_buffer_arg(prim->name, 1, argc, argv);
    linenr = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    line = ml_get_buf(buf->buf, (linenr_T)linenr, FALSE);

    raise_if_error();
    أرجع scheme_make_byte_string((حرف *)line);
}


/*
 * (get-buff-line-list {start} {end} [buffer])
 *
 * Get a list of lines from the specified buffer. The line numbers
 * are in Vim format (1-based). The range is from lo up to, but not
 * including, hi. The list is returned as a list of string objects.
 */
    ساكن Scheme_Object *
get_buffer_line_list(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    صحيح		    i, hi, lo, n;
    Scheme_Object   *list = عدم;

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
    𐡄𐡍 (lo < 0)
	lo = 0;

    𐡄𐡍 (hi < 0)
	hi = 0;
    𐡄𐡍 (hi < lo)
	hi = lo;

    n = hi - lo;

    لكل (i = n; i >= 0; --i)
    {
	Scheme_Object *str = scheme_make_byte_string(
		       (حرف *)ml_get_buf(buf->buf, (linenr_T)(lo+i), FALSE));
	raise_if_error();

	// Set the list item
	list = scheme_make_pair(str, list);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    أرجع list;
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
    ساكن Scheme_Object *
set_buffer_line(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the value of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A string	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf;
    Scheme_Object   *line = عدم;
    حرف	    *save;
    صحيح		    n;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, line);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    n = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    𐡄𐡍 (!SCHEME_STRINGP(argv[1]) && !SCHEME_FALSEP(argv[1]))
	scheme_wrong_type(prim->name, "string or #f", 1, argc, argv);
    line = argv[1];
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    check_line_range(n, buf->buf);

    𐡄𐡍 (SCHEME_FALSEP(line))
    {
	buf_T	    *savebuf = curbuf;

	curbuf = buf->buf;

	𐡄𐡍 (u_savedel((linenr_T)n, 1L) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا 𐡄𐡍 (ml_delete((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot delete line"));
	}
	𐡄𐡍 (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(n, n + 1, -1);
	deleted_lines_mark((linenr_T)n, 1L);

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
    وإلا
    {
	// Otherwise it's a line
	buf_T	    *savebuf = curbuf;

	save = string_to_line(line);

	curbuf = buf->buf;

	𐡄𐡍 (u_savesub((linenr_T)n) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا 𐡄𐡍 (ml_replace((linenr_T)n, (char_u *)save, TRUE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(save);
	    raise_vim_exn(_("cannot replace line"));
	}
	وإلا
	{
	    vim_free(save);
	    changed_bytes((linenr_T)n, 0);
	}

	curbuf = savebuf;

	// Check that the cursor is not beyond the end of the line now.
	𐡄𐡍 (buf->buf == curwin->w_buffer)
	    check_cursor_col();

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
}

    ساكن فراغ
free_array(حرف **array)
{
    حرف **curr = array;
    طالما (*curr != عدم)
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
    ساكن Scheme_Object *
set_buffer_line_list(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    // First of all, we check the type of the supplied MzScheme object.
    // There are three cases:
    //	  1. #f - this is a deletion.
    //	  2. A list	   - this is a replacement.
    //	  3. Anything else - this is an error.
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = عدم;
    Scheme_Object   *line_list = عدم;
    صحيح		    i, old_len, new_len, hi, lo;
    long	    extra;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, line_list);
    MZ_GC_REG();

#ifdef HAVE_SANDBOX
    sandbox_check();
#endif
    lo = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 0));
    hi = SCHEME_INT_VAL(GUARANTEE_INTEGER(prim->name, 1));
    𐡄𐡍 (!SCHEME_PAIRP(argv[2])
	    && !SCHEME_FALSEP(argv[2]) && !SCHEME_NULLP(argv[2]))
	scheme_wrong_type(prim->name, "list or #f", 2, argc, argv);
    line_list = argv[2];
    buf = get_buffer_arg(prim->name, 3, argc, argv);
    old_len = hi - lo;
    𐡄𐡍 (old_len < 0) // process inverse values wisely
    {
	i = lo;
	lo = hi;
	hi = i;
	old_len = -old_len;
    }
    extra = 0;

    check_line_range(lo, buf->buf);	    // inclusive
    check_line_range(hi - 1, buf->buf);	    // exclusive

    𐡄𐡍 (SCHEME_FALSEP(line_list) || SCHEME_NULLP(line_list))
    {
	buf_T	*savebuf = curbuf;
	curbuf = buf->buf;

	𐡄𐡍 (u_savedel((linenr_T)lo, (long)old_len) == FAIL)
	{
	    curbuf = savebuf;
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا
	{
	    لكل (i = 0; i < old_len; i++)
		𐡄𐡍 (ml_delete((linenr_T)lo) == FAIL)
		{
		    curbuf = savebuf;
		    raise_vim_exn(_("cannot delete line"));
		}
	    𐡄𐡍 (buf->buf == curwin->w_buffer)
		mz_fix_cursor(lo, hi, -old_len);
	    deleted_lines_mark((linenr_T)lo, (long)old_len);
	}

	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }
    وإلا
    {
	buf_T	*savebuf = curbuf;

	// List
	new_len = scheme_proper_list_length(line_list);
	MZ_GC_CHECK();
	𐡄𐡍 (new_len < 0)	// improper or cyclic list
	    scheme_wrong_type(prim->name, "proper list",
		    2, argc, argv);
	وإلا
	{
	    حرف		**array = عدم;
	    Scheme_Object   *line = عدم;
	    Scheme_Object   *rest = عدم;

	    MZ_GC_DECL_REG(2);
	    MZ_GC_VAR_IN_REG(0, line);
	    MZ_GC_VAR_IN_REG(1, rest);
	    MZ_GC_REG();

	    array = ALLOC_CLEAR_MULT(حرف *, new_len + 1);

	    rest = line_list;
	    لكل (i = 0; i < new_len; ++i)
	    {
		line = SCHEME_CAR(rest);
		rest = SCHEME_CDR(rest);
		𐡄𐡍 (!SCHEME_STRINGP(line))
		{
		    free_array(array);
		    scheme_wrong_type(prim->name, "string-list", 2, argc, argv);
		}
		array[i] = string_to_line(line);
	    }

	    curbuf = buf->buf;

	    𐡄𐡍 (u_save((linenr_T)(lo-1), (linenr_T)hi) == FAIL)
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
	    لكل (i = 0; i < old_len - new_len; ++i)
	    {
		𐡄𐡍 (ml_delete((linenr_T)lo) == FAIL)
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
	    لكل (i = 0; i < old_len && i < new_len; i++)
		𐡄𐡍 (ml_replace((linenr_T)(lo+i), (char_u *)array[i], TRUE) == FAIL)
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
	    طالما (i < new_len)
	    {
		𐡄𐡍 (ml_append((linenr_T)(lo + i - 1),
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
	changed_lines((linenr_T)lo, 0, (linenr_T)hi, (long)extra);

	𐡄𐡍 (buf->buf == curwin->w_buffer)
	    mz_fix_cursor(lo, hi, extra);
	curbuf = savebuf;

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
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
    ساكن Scheme_Object *
insert_buffer_line_list(فراغ *data, صحيح argc, Scheme_Object **argv)
{
    Vim_Prim	    *prim = (Vim_Prim *)data;
    vim_mz_buffer   *buf = عدم;
    Scheme_Object   *list = عدم;
    حرف	    *str = عدم;
    صحيح		    i, n, size;

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

    𐡄𐡍 (!SCHEME_STRINGP(list) && !SCHEME_PAIRP(list))
	scheme_wrong_type(prim->name, "string or list", 1, argc, argv);
    buf = get_buffer_arg(prim->name, 2, argc, argv);

    𐡄𐡍 (n != 0)	    // 0 can be used in insert
	check_line_range(n, buf->buf);
    𐡄𐡍 (SCHEME_STRINGP(list))
    {
	buf_T	    *savebuf = curbuf;

	str = string_to_line(list);
	curbuf = buf->buf;

	𐡄𐡍 (u_save((linenr_T)n, (linenr_T)(n+1)) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا 𐡄𐡍 (ml_append((linenr_T)n, (char_u *)str, 0, FALSE) == FAIL)
	{
	    curbuf = savebuf;
	    vim_free(str);
	    raise_vim_exn(_("cannot insert line"));
	}
	وإلا
	{
	    vim_free(str);
	    appended_lines_mark((linenr_T)n, 1L);
	}

	curbuf = savebuf;
	update_screen(UPD_VALID);

	MZ_GC_UNREG();
	raise_if_error();
	أرجع scheme_void;
    }

    // List
    size = scheme_proper_list_length(list);
    MZ_GC_CHECK();
    𐡄𐡍 (size < 0)	// improper or cyclic list
	scheme_wrong_type(prim->name, "proper list",
		2, argc, argv);
    وإلا
    {
	Scheme_Object   *line = عدم;
	Scheme_Object   *rest = عدم;
	حرف		**array;
	buf_T		*savebuf = curbuf;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, line);
	MZ_GC_VAR_IN_REG(1, rest);
	MZ_GC_REG();

	array = ALLOC_CLEAR_MULT(حرف *, size + 1);

	rest = list;
	لكل (i = 0; i < size; ++i)
	{
	    line = SCHEME_CAR(rest);
	    rest = SCHEME_CDR(rest);
	    array[i] = string_to_line(line);
	}

	curbuf = buf->buf;

	𐡄𐡍 (u_save((linenr_T)n, (linenr_T)(n + 1)) == FAIL)
	{
	    curbuf = savebuf;
	    free_array(array);
	    raise_vim_exn(_("cannot save undo information"));
	}
	وإلا
	{
	    لكل (i = 0; i < size; ++i)
		𐡄𐡍 (ml_append((linenr_T)(n + i), (char_u *)array[i],
			    0, FALSE) == FAIL)
		{
		    curbuf = savebuf;
		    free_array(array);
		    raise_vim_exn(_("cannot insert line"));
		}

	    𐡄𐡍 (i > 0)
		appended_lines_mark((linenr_T)n, (long)i);
	}
	free_array(array);
	MZ_GC_UNREG();
	curbuf = savebuf;
	update_screen(UPD_VALID);
    }

    MZ_GC_UNREG();
    raise_if_error();
    أرجع scheme_void;
}

/*
 * Predicates
 */
/*
 * (buff? obj)
 */
    ساكن Scheme_Object *
vim_bufferp(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv)
{
    𐡄𐡍 (SCHEME_VIMBUFFERP(argv[0]))
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (win? obj)
 */
    ساكن Scheme_Object *
vim_windowp(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv)
{
    𐡄𐡍 (SCHEME_VIMWINDOWP(argv[0]))
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (buff-valid? obj)
 */
    ساكن Scheme_Object *
vim_buffer_validp(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv)
{
    𐡄𐡍 (SCHEME_VIMBUFFERP(argv[0])
	    && ((vim_mz_buffer *)argv[0])->buf != INVALID_BUFFER_VALUE)
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
}

/*
 * (win-valid? obj)
 */
    ساكن Scheme_Object *
vim_window_validp(فراغ *data UNUSED, صحيح argc UNUSED, Scheme_Object **argv)
{
    𐡄𐡍 (SCHEME_VIMWINDOWP(argv[0])
	    && ((vim_mz_window *)argv[0])->win != INVALID_WINDOW_VALUE)
	أرجع scheme_true;
    وإلا
	أرجع scheme_false;
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
    ساكن حرف *
string_to_line(Scheme_Object *obj)
{
    حرف	*scheme_str = عدم;
    حرف	*vim_str = عدم;
    OUTPUT_LEN_TYPE	len;
    صحيح		i;

    scheme_str = scheme_display_to_string(obj, &len);

    // Error checking: String must not contain newlines, as we
    // are replacing a single line, and we must replace it with
    // a single line.
    𐡄𐡍 (memchr(scheme_str, '\n', len))
	scheme_signal_error(_("string cannot contain newlines"));

    vim_str = alloc(len + 1);

    // Create a copy of the string, with internal nulls replaced by
    // newline characters, as is the vim convention.
    لكل (i = 0; i < len; ++i)
    {
	𐡄𐡍 (scheme_str[i] == '\0')
	    vim_str[i] = '\n';
	وإلا
	    vim_str[i] = scheme_str[i];
    }

    vim_str[i] = '\0';

    MZ_GC_CHECK();
    أرجع vim_str;
}

#ifdef FEAT_EVAL
/*
 * Convert Vim value into MzScheme, adopted from if_python.c
 */
    ساكن Scheme_Object *
vim_to_mzscheme(typval_T *vim_value)
{
    Scheme_Object	*result = عدم;
    // hash table to store visited values to avoid infinite loops
    Scheme_Hash_Table	*visited = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    result = vim_to_mzscheme_impl(vim_value, 1, visited);

    MZ_GC_UNREG();
    أرجع result;
}

    ساكن Scheme_Object *
vim_to_mzscheme_impl(
    typval_T		*vim_value,
    صحيح			depth,
    Scheme_Hash_Table	*visited)
{
    Scheme_Object   *result = عدم;
    صحيح		    new_value = TRUE;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    // Avoid infinite recursion
    𐡄𐡍 (depth > 100)
    {
	MZ_GC_UNREG();
	أرجع scheme_void;
    }

    // Check if we run into a recursive loop.  The item must be in visited
    // then and we can use it again.
    result = scheme_hash_get(visited, (Scheme_Object *)vim_value);
    MZ_GC_CHECK();
    𐡄𐡍 (result != عدم) // found, do nothing
	new_value = FALSE;
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_STRING)
    {
	result = scheme_make_byte_string((حرف *)vim_value->vval.v_string);
	MZ_GC_CHECK();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_NUMBER)
    {
	result = scheme_make_integer((long)vim_value->vval.v_number);
	MZ_GC_CHECK();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_FLOAT)
    {
	result = scheme_make_double((مزدوج)vim_value->vval.v_float);
	MZ_GC_CHECK();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_LIST)
    {
	list_T		*list = vim_value->vval.v_list;
	listitem_T	*curr;

	𐡄𐡍 (list == عدم || list->lv_first == عدم)
	    result = scheme_null;
	وإلا
	{
	    Scheme_Object   *obj = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, obj);
	    MZ_GC_REG();

	    curr = list->lv_u.mat.lv_last;
	    obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
	    result = scheme_make_pair(obj, scheme_null);
	    MZ_GC_CHECK();

	    طالما (curr != list->lv_first)
	    {
		curr = curr->li_prev;
		obj = vim_to_mzscheme_impl(&curr->li_tv, depth + 1, visited);
		result = scheme_make_pair(obj, result);
		MZ_GC_CHECK();
	    }
	}
	MZ_GC_UNREG();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_DICT)
    {
	Scheme_Object	  *key = عدم;
	Scheme_Object	  *obj = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, key);
	MZ_GC_VAR_IN_REG(1, obj);
	MZ_GC_REG();

	result = (Scheme_Object *)scheme_make_hash_table(SCHEME_hash_ptr);
	MZ_GC_CHECK();
	𐡄𐡍 (vim_value->vval.v_dict != عدم)
	{
	    hashtab_T	*ht = &vim_value->vval.v_dict->dv_hashtab;
	    long_u	todo = ht->ht_used;
	    hashitem_T	*hi;
	    dictitem_T	*di;

	    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	    {
		𐡄𐡍 (!HASHITEM_EMPTY(hi))
		{
		    --todo;

		    di = dict_lookup(hi);
		    obj = vim_to_mzscheme_impl(&di->di_tv, depth + 1, visited);
		    key = scheme_make_byte_string((حرف *)hi->hi_key);
		    MZ_GC_CHECK();
		    scheme_hash_set((Scheme_Hash_Table *)result, key, obj);
		    MZ_GC_CHECK();
		}
	    }
	}
	MZ_GC_UNREG();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_FUNC)
    {
	Scheme_Object *funcname = عدم;

	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, funcname);
	MZ_GC_REG();

	// FIXME: func_ref() and func_unref() are needed.
	funcname = scheme_make_byte_string((حرف *)vim_value->vval.v_string);
	MZ_GC_CHECK();
	result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		(ثابت حرف *)BYTE_STRING_VALUE(funcname), 0, -1);
	MZ_GC_CHECK();

	MZ_GC_UNREG();
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_PARTIAL)
    {
	𐡄𐡍 (vim_value->vval.v_partial == عدم)
	    result = scheme_null;
	وإلا
	{
	    Scheme_Object *funcname = عدم;

	    MZ_GC_DECL_REG(1);
	    MZ_GC_VAR_IN_REG(0, funcname);
	    MZ_GC_REG();

	    // FIXME: func_ref() and func_unref() are needed.
	    // TODO: Support pt_dict and pt_argv.
	    funcname = scheme_make_byte_string(
			      (حرف *)partial_name(vim_value->vval.v_partial));
	    MZ_GC_CHECK();
	    result = scheme_make_closed_prim_w_arity(vim_funcref, funcname,
		    (ثابت حرف *)BYTE_STRING_VALUE(funcname), 0, -1);
	    MZ_GC_CHECK();

	    MZ_GC_UNREG();
	}
    }
    وإلا 𐡄𐡍 (vim_value->v_type == VAR_BOOL || vim_value->v_type == VAR_SPECIAL)
    {
	𐡄𐡍 (vim_value->vval.v_number <= VVAL_TRUE)
	    result = scheme_make_integer((long)vim_value->vval.v_number);
	وإلا
	    result = scheme_null;
	MZ_GC_CHECK();
    }
    وإلا
    {
	result = scheme_void;
	new_value = FALSE;
    }
    𐡄𐡍 (new_value)
    {
	scheme_hash_set(visited, (Scheme_Object *)vim_value, result);
	MZ_GC_CHECK();
    }
    MZ_GC_UNREG();
    أرجع result;
}

    ساكن صحيح
mzscheme_to_vim(Scheme_Object *obj, typval_T *tv)
{
    صحيح i, status;
    Scheme_Hash_Table	*visited = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    visited = scheme_make_hash_table(SCHEME_hash_ptr);
    MZ_GC_CHECK();

    status = mzscheme_to_vim_impl(obj, tv, 1, visited);
    لكل (i = 0; i < visited->size; ++i)
    {
	// free up remembered objects
	𐡄𐡍 (visited->vals[i] != عدم)
	    free_tv((typval_T *)visited->vals[i]);
    }

    MZ_GC_UNREG();
    أرجع status;
}
    ساكن صحيح
mzscheme_to_vim_impl(Scheme_Object *obj, typval_T *tv, صحيح depth,
	Scheme_Hash_Table *visited)
{
    صحيح		status = OK;
    typval_T	*found;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, obj);
    MZ_GC_VAR_IN_REG(1, visited);
    MZ_GC_REG();

    MZ_GC_CHECK();
    𐡄𐡍 (depth > 100) // limit the deepest recursion level
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = 0;
	أرجع FAIL;
    }

    found = (typval_T *)scheme_hash_get(visited, obj);
    𐡄𐡍 (found != عدم)
	copy_tv(found, tv);
    وإلا 𐡄𐡍 (SCHEME_VOIDP(obj))
    {
	tv->v_type = VAR_SPECIAL;
	tv->vval.v_number = VVAL_NULL;
    }
    وإلا 𐡄𐡍 (SCHEME_INTP(obj))
    {
	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = SCHEME_INT_VAL(obj);
    }
    وإلا 𐡄𐡍 (SCHEME_BOOLP(obj))
    {
	tv->v_type = VAR_BOOL;
	tv->vval.v_number = SCHEME_TRUEP(obj);
    }
    وإلا 𐡄𐡍 (SCHEME_DBLP(obj))
    {
	tv->v_type = VAR_FLOAT;
	tv->vval.v_float = SCHEME_DBL_VAL(obj);
    }
    وإلا 𐡄𐡍 (SCHEME_BYTE_STRINGP(obj))
    {
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_VALUE(obj));
    }
# 𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
    وإلا 𐡄𐡍 (SCHEME_CHAR_STRINGP(obj))
    {
	Scheme_Object *tmp = عدم;
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, tmp);
	MZ_GC_REG();

	tmp = scheme_char_string_to_byte_string(obj);
	tv->v_type = VAR_STRING;
	tv->vval.v_string = vim_strsave(BYTE_STRING_VALUE(tmp));
	MZ_GC_UNREG();
    }
# endif
    وإلا 𐡄𐡍 (SCHEME_VECTORP(obj) || SCHEME_NULLP(obj)
	    || SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
    {
	list_T  *list = list_alloc();
	𐡄𐡍 (list == عدم)
	    status = FAIL;
	وإلا
	{
	    صحيح		    i;
	    Scheme_Object   *curr = عدم;
	    Scheme_Object   *cval = عدم;
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
	    𐡄𐡍 (v == عدم)
		status = FAIL;
	    وإلا
	    {
		// add the value in advance to allow handling of self-referential
		// data structures
		typval_T    *visited_tv = ALLOC_ONE(typval_T);
		copy_tv(tv, visited_tv);
		scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

		𐡄𐡍 (SCHEME_VECTORP(obj))
		{
		    لكل (i = 0; i < SCHEME_VEC_SIZE(obj); ++i)
		    {
			cval = SCHEME_VEC_ELS(obj)[i];
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			𐡄𐡍 (status == FAIL)
			    اكسر;
			status = list_append_tv(list, v);
			clear_tv(v);
			𐡄𐡍 (status == FAIL)
			    اكسر;
		    }
		}
		وإلا 𐡄𐡍 (SCHEME_PAIRP(obj) || SCHEME_MUTABLE_PAIRP(obj))
		{
		    لكل (curr = obj;
			    SCHEME_PAIRP(curr) || SCHEME_MUTABLE_PAIRP(curr);
			    curr = SCHEME_CDR(curr))
		    {
			cval = SCHEME_CAR(curr);
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			𐡄𐡍 (status == FAIL)
			    اكسر;
			status = list_append_tv(list, v);
			clear_tv(v);
			𐡄𐡍 (status == FAIL)
			    اكسر;
		    }
		    // improper list not terminated with null
		    // need to handle the last element
		    𐡄𐡍 (status == OK && !SCHEME_NULLP(curr))
		    {
			status = mzscheme_to_vim_impl(cval, v, depth + 1, visited);
			𐡄𐡍 (status == OK)
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
    وإلا 𐡄𐡍 (SCHEME_HASHTP(obj))
    {
	صحيح		i;
	dict_T		*dict;
	Scheme_Object   *key = عدم;
	Scheme_Object   *val = عدم;

	MZ_GC_DECL_REG(2);
	MZ_GC_VAR_IN_REG(0, key);
	MZ_GC_VAR_IN_REG(1, val);
	MZ_GC_REG();

	dict = dict_alloc();
	𐡄𐡍 (dict == عدم)
	    status = FAIL;
	وإلا
	{
	    typval_T    *visited_tv = ALLOC_ONE(typval_T);

	    tv->v_type = VAR_DICT;
	    tv->vval.v_dict = dict;
	    ++dict->dv_refcount;

	    copy_tv(tv, visited_tv);
	    scheme_hash_set(visited, obj, (Scheme_Object *)visited_tv);

	    لكل (i = 0; i < ((Scheme_Hash_Table *)obj)->size; ++i)
	    {
		𐡄𐡍 (((Scheme_Hash_Table *) obj)->vals[i] != عدم)
		{
		    // generate item for `display'ed Scheme key
		    dictitem_T  *item = dictitem_alloc((char_u *)string_to_line(
				((Scheme_Hash_Table *) obj)->keys[i]));
		    // convert Scheme val to Vim and add it to the dict
		    𐡄𐡍 (mzscheme_to_vim_impl(((Scheme_Hash_Table *) obj)->vals[i],
				    &item->di_tv, depth + 1, visited) == FAIL
			    || dict_add(dict, item) == FAIL)
		    {
			dictitem_free(item);
			status = FAIL;
			اكسر;
		    }
		}

	    }
	}
	MZ_GC_UNREG();
    }
    وإلا
    {
	// `display' any other value to string
	tv->v_type = VAR_STRING;
	tv->vval.v_string = (char_u *)string_to_line(obj);
    }
    MZ_GC_UNREG();
    أرجع status;
}

/*
 * Scheme prim procedure wrapping Vim funcref
 */
    ساكن Scheme_Object *
vim_funcref(فراغ *name, صحيح argc, Scheme_Object **argv)
{
    صحيح i;
    typval_T args;
    صحيح status = OK;
    Scheme_Object *result = عدم;
    list_T *list = list_alloc();

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, result);
    MZ_GC_REG();

    result = scheme_void;
    𐡄𐡍 (list == عدم)
	status = FAIL;
    وإلا
    {
	args.v_type = VAR_LIST;
	args.vval.v_list = list;
	++list->lv_refcount;
	لكل (i = 0; status == OK && i < argc; ++i)
	{
	    typval_T *v = ALLOC_ONE(typval_T);
	    𐡄𐡍 (v == عدم)
		status = FAIL;
	    وإلا
	    {
		status = mzscheme_to_vim(argv[i], v);
		𐡄𐡍 (status == OK)
		{
		    status = list_append_tv(list, v);
		    clear_tv(v);
		}
		vim_free(v);
	    }
	}
	𐡄𐡍 (status == OK)
	{
	    typval_T ret;
	    ret.v_type = VAR_UNKNOWN;

	    mzscheme_call_vim(BYTE_STRING_VALUE((Scheme_Object *)name), &args, &ret);
	    MZ_GC_CHECK();
	    result = vim_to_mzscheme(&ret);
	    clear_tv(&ret);
	    MZ_GC_CHECK();
	}
    }
    clear_tv(&args);
    MZ_GC_UNREG();
    𐡄𐡍 (status != OK)
	raise_vim_exn(_("error converting Scheme values to Vim"));
    وإلا
	raise_if_error();
    أرجع result;
}

    فراغ
do_mzeval(char_u *str, typval_T *rettv)
{
    Scheme_Object	*ret = عدم;

    MZ_GC_DECL_REG(1);
    MZ_GC_VAR_IN_REG(0, ret);
    MZ_GC_REG();

    𐡄𐡍 (mzscheme_init())
    {
	MZ_GC_UNREG();
	أرجع;
    }

    MZ_GC_CHECK();
    𐡄𐡍 (eval_with_exn_handling(str, do_eval, &ret) == OK)
	mzscheme_to_vim(ret, rettv);

    MZ_GC_UNREG();
}
#endif

/*
 * Check to see whether a Vim error has been reported, or a keyboard
 * interrupt (from vim --> got_int) has been detected.
 */
    ساكن صحيح
vim_error_check(فراغ)
{
    أرجع (got_int || did_emsg);
}

/*
 * register Scheme exn:vim
 */
    ساكن فراغ
register_vim_exn(فراغ)
{
    صحيح	nc = 0;
    صحيح i;
    Scheme_Object   *struct_exn = عدم;
    Scheme_Object   *exn_name = عدم;

    MZ_GC_DECL_REG(2);
    MZ_GC_VAR_IN_REG(0, struct_exn);
    MZ_GC_VAR_IN_REG(1, exn_name);
    MZ_GC_REG();

    exn_name = scheme_intern_symbol("exn:vim");
    MZ_GC_CHECK();
    struct_exn = scheme_builtin_value("struct:exn");
    MZ_GC_CHECK();

    𐡄𐡍 (vim_exn == عدم)
	vim_exn = scheme_make_struct_type(exn_name,
		struct_exn, عدم, 0, 0, عدم, عدم
#𐡄𐡍 MZSCHEME_VERSION_MAJOR >= 299
		, عدم
#endif
		);


    {
	Scheme_Object   **tmp = عدم;
	Scheme_Object   *exn_names[5] = {عدم, عدم, عدم, عدم, عدم};
	Scheme_Object   *exn_values[5] = {عدم, عدم, عدم, عدم, عدم};
	MZ_GC_DECL_REG(6);
	MZ_GC_ARRAY_VAR_IN_REG(0, exn_names, 5);
	MZ_GC_ARRAY_VAR_IN_REG(3, exn_values, 5);
	MZ_GC_REG();

	tmp = scheme_make_struct_names(exn_name, scheme_null, 0, &nc);
	mch_memmove(exn_names, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	tmp = scheme_make_struct_values(vim_exn, exn_names, nc, 0);
	mch_memmove(exn_values, tmp, nc * حجم(Scheme_Object *));
	MZ_GC_CHECK();

	لكل (i = 0; i < nc; i++)
	{
	    scheme_add_global_symbol(exn_names[i],
		    exn_values[i], environment);
	    MZ_GC_CHECK();
	}
	MZ_GC_UNREG();
    }
    MZ_GC_UNREG();
}

/*
 * raise exn:vim, may be with additional info string
 */
    فراغ
raise_vim_exn(ثابت حرف *add_info)
{
    حرف	    *fmt = _("Vim error: ~a");
    Scheme_Object   *argv[2] = {عدم, عدم};
    Scheme_Object   *exn = عدم;
    Scheme_Object   *byte_string = عدم;

    MZ_GC_DECL_REG(5);
    MZ_GC_ARRAY_VAR_IN_REG(0, argv, 2);
    MZ_GC_VAR_IN_REG(3, exn);
    MZ_GC_VAR_IN_REG(4, byte_string);
    MZ_GC_REG();

    𐡄𐡍 (add_info != عدم)
    {
	حرف		*c_string = عدم;
	Scheme_Object   *info = عدم;

	MZ_GC_DECL_REG(3);
	MZ_GC_VAR_IN_REG(0, c_string);
	MZ_GC_VAR_IN_REG(2, info);
	MZ_GC_REG();

	info = scheme_make_byte_string(add_info);
	MZ_GC_CHECK();
	c_string = scheme_format_utf8(fmt, (صحيح)STRLEN(fmt), 1, &info, عدم);
	MZ_GC_CHECK();
	byte_string = scheme_make_byte_string(c_string);
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	SCHEME_SET_IMMUTABLE(argv[0]);
	MZ_GC_UNREG();
    }
    وإلا
    {
	byte_string = scheme_make_byte_string(_("Vim error"));
	MZ_GC_CHECK();
	argv[0] = scheme_byte_string_to_char_string(byte_string);
	MZ_GC_CHECK();
    }
    MZ_GC_CHECK();

#𐡄𐡍 MZSCHEME_VERSION_MAJOR < 360
    argv[1] = scheme_current_continuation_marks();
    MZ_GC_CHECK();
#وإلا
    argv[1] = scheme_current_continuation_marks(عدم);
    MZ_GC_CHECK();
#endif

    exn = scheme_make_struct_instance(vim_exn, 2, argv);
    MZ_GC_CHECK();
    scheme_raise(exn);
    MZ_GC_UNREG();
}

    فراغ
raise_if_error(فراغ)
{
    𐡄𐡍 (vim_error_check())
	raise_vim_exn(عدم);
}

/*
 * get buffer:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساكن vim_mz_buffer *
get_buffer_arg(ثابت حرف *fname, صحيح argnum, صحيح argc, Scheme_Object **argv)
{
    vim_mz_buffer *b;

    𐡄𐡍 (argc < argnum + 1)
	أرجع get_vim_curr_buffer();
    𐡄𐡍 (!SCHEME_VIMBUFFERP(argv[argnum]))
	scheme_wrong_type(fname, "vim-buffer", argnum, argc, argv);
    b = (vim_mz_buffer *)argv[argnum];
    (فراغ)get_valid_buffer(argv[argnum]);
    أرجع b;
}

/*
 * get window:
 * either current
 * or passed as argv[argnum] with checks
 */
    ساكن vim_mz_window *
get_window_arg(ثابت حرف *fname, صحيح argnum, صحيح argc, Scheme_Object **argv)
{
    vim_mz_window *w;

    𐡄𐡍 (argc < argnum + 1)
	أرجع get_vim_curr_window();
    w = (vim_mz_window *)argv[argnum];
    𐡄𐡍 (!SCHEME_VIMWINDOWP(argv[argnum]))
	scheme_wrong_type(fname, "vim-window", argnum, argc, argv);
    (فراغ)get_valid_window(argv[argnum]);
    أرجع w;
}

/*
 * get valid Vim buffer from Scheme_Object*
 */
buf_T *get_valid_buffer(فراغ *obj)
{
    buf_T *buf = ((vim_mz_buffer *)obj)->buf;

    𐡄𐡍 (buf == INVALID_BUFFER_VALUE)
	scheme_signal_error(_("buffer is invalid"));
    أرجع buf;
}

/*
 * get valid Vim window from Scheme_Object*
 */
win_T *get_valid_window(فراغ *obj)
{
    win_T *win = ((vim_mz_window *)obj)->win;
    𐡄𐡍 (win == INVALID_WINDOW_VALUE)
	scheme_signal_error(_("window is invalid"));
    أرجع win;
}

    صحيح
mzthreads_allowed(فراغ)
{
    أرجع mz_threads_allow;
}

    ساكن صحيح
line_in_range(linenr_T lnum, buf_T *buf)
{
    أرجع (lnum > 0 && lnum <= buf->b_ml.ml_line_count);
}

    ساكن فراغ
check_line_range(linenr_T lnum, buf_T *buf)
{
    𐡄𐡍 (!line_in_range(lnum, buf))
	scheme_signal_error(_("linenr out of range"));
}

/*
 * Check if deleting lines made the cursor position invalid
 * (or you'll get msg from Vim about invalid linenr).
 * Changed the lines from "lo" to "hi" and added "extra" lines (negative if
 * deleted). Got from if_python.c
 */
    ساكن فراغ
mz_fix_cursor(صحيح lo, صحيح hi, صحيح extra)
{
    𐡄𐡍 (curwin->w_cursor.lnum >= lo)
    {
	// Adjust the cursor position if it's in/after the changed
	// lines.
	𐡄𐡍 (curwin->w_cursor.lnum >= hi)
	{
	    curwin->w_cursor.lnum += extra;
	    check_cursor_col();
	}
	وإلا 𐡄𐡍 (extra < 0)
	{
	    curwin->w_cursor.lnum = lo;
	    check_cursor();
	}
	وإلا
	    check_cursor_col();
	changed_cline_bef_curs();
    }
    invalidate_botline();
}

ساكن Vim_Prim prims[]=
{
    /*
     * Buffer-related commands
     */
    {get_buffer_line, "get-buff-line", 1, 2},
    {set_buffer_line, "set-buff-line", 2, 3},
    {get_buffer_line_list, "get-buff-line-list", 2, 3},
    {get_buffer_name, "get-buff-name", 0, 1},
    {get_buffer_num, "get-buff-num", 0, 1},
    {get_buffer_size, "get-buff-size", 0, 1},
    {set_buffer_line_list, "set-buff-line-list", 3, 4},
    {insert_buffer_line_list, "insert-buff-line-list", 2, 3},
    {get_curr_buffer, "curr-buff", 0, 0},
    {get_buffer_count, "buff-count", 0, 0},
    {get_next_buffer, "get-next-buff", 0, 1},
    {get_prev_buffer, "get-prev-buff", 0, 1},
    {mzscheme_open_buffer, "open-buff", 1, 1},
    {get_buffer_by_name, "get-buff-by-name", 1, 1},
    {get_buffer_by_num, "get-buff-by-num", 1, 1},
    /*
     * Window-related commands
     */
    {get_curr_win, "curr-win", 0, 0},
    {get_window_count, "win-count", 0, 0},
    {get_window_by_num, "get-win-by-num", 1, 1},
    {get_window_num, "get-win-num", 0, 1},
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
    ساكن vim_mz_buffer *
get_vim_curr_buffer(فراغ)
{
    𐡄𐡍 (curbuf->b_mzscheme_ref == عدم)
	أرجع (vim_mz_buffer *)buffer_new(curbuf);
    وإلا
	أرجع BUFFER_REF(curbuf);
}

/*
 * return MzScheme wrapper for curwin
 */
    ساكن vim_mz_window *
get_vim_curr_window(فراغ)
{
    𐡄𐡍 (curwin->w_mzscheme_ref == عدم)
	أرجع (vim_mz_window *)window_new(curwin);
    وإلا
	أرجع WINDOW_REF(curwin);
}

    ساكن فراغ
make_modules(فراغ)
{
    صحيح		    i;
    Scheme_Env	    *mod = عدم;
    Scheme_Object   *vimext_symbol = عدم;
    Scheme_Object   *closed_prim = عدم;

    MZ_GC_DECL_REG(3);
    MZ_GC_VAR_IN_REG(0, mod);
    MZ_GC_VAR_IN_REG(1, vimext_symbol);
    MZ_GC_VAR_IN_REG(2, closed_prim);
    MZ_GC_REG();

    vimext_symbol = scheme_intern_symbol("vimext");
    MZ_GC_CHECK();
    mod = scheme_primitive_module(vimext_symbol, environment);
    MZ_GC_CHECK();
    // all prims made closed so they can access their own names
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(prims); i++)
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
ساكن Scheme_Object *M_write = عدم;
ساكن Scheme_Object *M_read = عدم;
ساكن Scheme_Object *M_execute = عدم;
ساكن Scheme_Object *M_delete = عدم;

    ساكن فراغ
sandbox_check(فراغ)
{
    𐡄𐡍 (sandbox)
	raise_vim_exn(_("not allowed in the Vim sandbox"));
}

/*
 * security guards to force Vim's sandbox restrictions on MzScheme level
 */
    ساكن Scheme_Object *
sandbox_file_guard(صحيح argc UNUSED, Scheme_Object **argv)
{
    𐡄𐡍 (!sandbox)
	أرجع scheme_void;

    Scheme_Object *requested_access = argv[2];

    𐡄𐡍 (M_write == عدم)
    {
	MZ_REGISTER_STATIC(M_write);
	M_write = scheme_intern_symbol("write");
	MZ_GC_CHECK();
    }
    𐡄𐡍 (M_read == عدم)
    {
	MZ_REGISTER_STATIC(M_read);
	M_read = scheme_intern_symbol("read");
	MZ_GC_CHECK();
    }
    𐡄𐡍 (M_execute == عدم)
    {
	MZ_REGISTER_STATIC(M_execute);
	M_execute = scheme_intern_symbol("execute");
	MZ_GC_CHECK();
    }
    𐡄𐡍 (M_delete == عدم)
    {
	MZ_REGISTER_STATIC(M_delete);
	M_delete = scheme_intern_symbol("delete");
	MZ_GC_CHECK();
    }

    طالما (!SCHEME_NULLP(requested_access))
    {
	Scheme_Object *item = SCHEME_CAR(requested_access);
	𐡄𐡍 (scheme_eq(item, M_write) || scheme_eq(item, M_read)
		|| scheme_eq(item, M_execute) || scheme_eq(item, M_delete))
	    raise_vim_exn(_("not allowed in the Vim sandbox"));
	requested_access = SCHEME_CDR(requested_access);
    }

    أرجع scheme_void;
}

    ساكن Scheme_Object *
sandbox_network_guard(صحيح argc UNUSED, Scheme_Object **argv UNUSED)
{
    أرجع scheme_void;
}
#endif

#endif
